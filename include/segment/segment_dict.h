#ifndef NLP_SEGMENT_DICT_H_
#define NLP_SEGMENT_DICT_H_

#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <istream>
#include <ostream>
#include <cctype>
#include <math.h>

#include "kstring.hpp"
#include "normalize.h"
#include "darts.h"
#include "line_reader.h"
#include "utf8.h"

namespace knlp
{

template<class T>class KDictionary;
template<class T> void store_values_(const std::vector<std::string>& v, KDictionary<T>& dict);
template<class T> void save_values_(const std::string& nm, KDictionary<T>& dict);
template<class T> void load_values_(const std::string& nm, KDictionary<T>& dict);

template<class T = int32_t>
class KDictionary
{
    Darts::DoubleArray trie_;
    std::vector<T> values_;
    T min_, max_;

    friend void store_values_<T>(const std::vector<std::string>& v, KDictionary<T>& dict);
    friend void save_values_<T>(const std::string& nm, KDictionary<T>& dict);
    friend void load_values_<T>(const std::string& nm, KDictionary<T>& dict);

    void normalize_(std::string& str)const
    {
        Normalize::normalize(str);
    }

    void load_(const std::string& nm)
    {
        int32_t f = 1;
        try{
            if((f = trie_.open((nm+".bin").c_str()))==0)
               load_values_<T>(nm+".v", *this);
        }
        catch(...)
        {}

        if (0 == f)
            return;

        std::set<std::pair<std::string,std::string> > k_v;
        std::set<std::string> keySet;
        char* li = NULL;
        util::LineReader lr(nm);
        while((li = lr.line(li)) != NULL)
        {
            char* t = strchr(li, '\t');
            std::string k,v;
            if(t)
            {
                k = std::string(li, t);
                v = std::string(t+1);
            }else
                k = std::string(li);
            normalize_(k);
            if (k.length() == 0 || keySet.find(k)!=keySet.end())
                continue;

            k_v.insert(std::make_pair(k, v));
            keySet.insert(k);
        }
        
        std::vector<const char *> keys(k_v.size());
        std::vector<std::string> v(k_v.size());
        std::vector<std::size_t> lengths(k_v.size());
        std::vector<typename Darts::DoubleArray::value_type> values(k_v.size());

        uint32_t i = 0, ff = 0;
        for (std::set<std::pair<std::string,std::string> >::const_iterator it=k_v.begin();it!=k_v.end();++it,i++)
        {
            keys[i] = it->first.c_str()
              ,lengths[i]=it->first.length()
              ,values[i] = i;
            if (it->second.length())
                v[i]=it->second,ff=1;
        }
        assert(keys.size() == v.size());
        trie_.build(keys.size(), &keys[0], &lengths[0], &values[0]);
        trie_.save((nm+".bin").c_str());

        if (ff == 1)
        {
            store_values_<T>(v, *this);
            save_values_(nm+".v", *this);
        }
    }

public:

    KDictionary(const std::string& dict_nm)
    {
        load_(dict_nm);
        if (values_.size() == 0)return;
        min_ = *std::min_element(values_.begin(), values_.end());
        max_ = *std::max_element(values_.begin(), values_.end());
    }

    ~KDictionary()
    {
    }

    /**
     * Return value:
     *  0:  sucess
     *  1:  sucess, but no value for this key.
     *  -1: not found
     * */
    int32_t value(std::string key, T& v, bool nor = true)const
    {
        if (nor)
            normalize_(key);
        Darts::DoubleArray::result_pair_type res;
        trie_.exactMatchSearch(key.c_str(), res, key.length());

        if (res.length == 0 && res.value == -1)
            return -1;

        if ((std::size_t)res.value < values_.size())
        {
            v = values_[res.value];
            return 0;
        }

        return 1;
    }
    
    T min()const
    {
        return min_;
    }

    T max()const
    {
        return max_;
    }

    bool has_key(std::string key, bool nor = true)const
    {
        T v;
        int32_t r = value(key, v, nor);
        if(r >= 0)
            return true;
        return false;
    }

    void fmm(std::string line, std::vector<std::pair<std::string, T> >& r, bool nor = true)const
    {
        r.clear();
        if(nor)normalize_(line);
       // KString kstr(line);
        std::vector<uint32_t> lens, cumu_lens;
        // check encoding
        std::string::iterator str_end = utf8::find_invalid(line.begin(), line.end());
        // get bytes numbers
        line = std::string(line.begin(), str_end);
        std::string::iterator it = line.begin();
        while(it != line.end())
        {
            //std::string str = kstr.substr(i,1).get_bytes("utf-8");
            uint32_t code = utf8::next(it, line.end());
            std::string str;
            utf8::append(code, std::back_inserter(str));
            lens.push_back(str.length());
            if (cumu_lens.size() > 0)
                cumu_lens.push_back(str.length()+cumu_lens.back());
            else cumu_lens.push_back(str.length());
        }

        std::size_t key_pos=0;
        for (std::size_t j = 0; j <lens.size(); ++j) 
        {
            std::size_t last_j = j, jj=j;
            Darts::DoubleArray::value_type last_state = -1;
            typename Darts::DoubleArray::value_type state;
            std::size_t node_pos = 0;
            while(j<lens.size() 
              && (state=trie_.traverse(line.c_str(), node_pos, key_pos, cumu_lens[j]))!=-2
              )
            {
                //std::cout<<j<<":"<<lens[j]<<":"<<state<<":"<<line.substr(key_pos-lens[j],lens[j])<<"<<<<<<\n";;
                j++;
                if(state < 0)continue;
                last_state = state;
                last_j = j - 1;
            }

            if (last_state >=0)
            {
                T v = T();if ((uint32_t)last_state < values_.size())v = values_[last_state];
                r.push_back(std::make_pair(std::string(line.c_str()+cumu_lens[jj]-lens[jj], line.c_str()+cumu_lens[last_j]), v));
            }
            else
                r.push_back(std::make_pair(std::string(line.c_str()+cumu_lens[jj]-lens[jj], line.c_str()+cumu_lens[jj]), min()));
            j = last_j;
            key_pos = cumu_lens[j];
        }
    }

    void subtokens(const std::vector<std::pair<std::string, T> >& tks,
                    std::vector<std::pair<std::string, T> >& subs)const
    {
        subs.clear();
        for (uint32_t i=0; i<tks.size(); ++i)
        {
            KString kstr(tks[i].first);
            if (kstr.length()<=2)
            {
                subs.push_back(tks[i]);
                continue;
            }

            std::string line = kstr.substr(0, kstr.length()-1).get_bytes("utf-8");
            std::vector<std::pair<std::string, T> > ss;
            do{
                std::vector<std::pair<std::string, T> > s;
                fmm(line, s, false);
                if (ss.size() == 0)line = tks[i].first;
                ss.push_back(s[0]);
                line = line.substr(s[0].first.length());
            }while(line.length());

            if (ss.size() >= kstr.length())
                 subs.push_back(tks[i]);
            else
                 subs.insert(subs.end(), ss.begin(), ss.end());
        }
    }
};

template<class T>
  inline void store_values_(const std::vector<std::string>& v, KDictionary<T>& dict)
  {
      dict.values_.resize(v.size(), 0);
      for (uint32_t i=0;i<v.size();++i)
          dict.values_[i] = atoi(v[i].c_str());
  }

template<class T>
  inline void save_values_(const std::string& nm, KDictionary<T>& dict)
  {
      FILE* f = fopen(nm.c_str(), "w+");
      if (!f)
          return;
      uint32_t s = dict.values_.size();
      assert(fwrite(&s, sizeof(s), 1, f) == 1);
      assert(fwrite(&dict.values_[0], dict.values_.size()*sizeof(T), 1, f) == 1);
      fclose(f);
  }

template<class T>
  inline void load_values_(const std::string& nm, KDictionary<T>& dict)
  {
      FILE* f = fopen(nm.c_str(), "r");
      if (!f)
          return;
      uint32_t s = 0;
      assert(fread(&s, sizeof(s), 1, f) == 1);
      dict.values_.resize(s);
      assert(fread(&dict.values_[0], dict.values_.size()*sizeof(T), 1, f) == 1);
      fclose(f);
  }

template<>
  inline void store_values_<const char*>(const std::vector<std::string>& v, KDictionary<const char*>& dict)
  {
      dict.values_.resize(v.size(), NULL);
      for (uint32_t i=0;i<v.size();++i)
      {
          char* b = new char[v[i].length()+1];
          memset(b, 0, v[i].length()+1);
          strcpy(b, v[i].c_str());
          dict.values_[i] = b;
      }
  }

template<>
  inline void store_values_<float>(const std::vector<std::string>& v, KDictionary<float>& dict)
  {
      dict.values_.resize(v.size(), 0);
      for (uint32_t i=0;i<v.size();++i)
          dict.values_[i] = atof(v[i].c_str());
  }

template<>
  inline void store_values_<uint32_t>(const std::vector<std::string>& v, KDictionary<uint32_t>& dict)
  {
      dict.values_.resize(v.size(), 0);
      for (uint32_t i=0;i<v.size();++i)
          dict.values_[i] = atoi(v[i].c_str());
  }

template<>
  inline void save_values_<const char*>(const std::string& nm, KDictionary<const char*>& dict)
  {
      FILE* f = fopen(nm.c_str(), "w+");
      if (!f)
          return;
      uint32_t s = dict.values_.size();
      fwrite(&s, 1, sizeof(s), f);
      for (uint32_t i=0;i<dict.values_.size();++i)
      {
          s = strlen(dict.values_[i]);
          fwrite(&s, 1, sizeof(s), f);
          fwrite(dict.values_[i], 1, s, f);
      }
      fclose(f);
  }

template<>
  inline void load_values_<const char*>(const std::string& nm, KDictionary<const char*>& dict)
  {
      FILE* f = fopen(nm.c_str(), "r");
      if (!f)
          return;
      uint32_t s = 0;
      assert(fread(&s, sizeof(s), 1, f) == 1);
      dict.values_.resize(s);
      for (uint32_t i=0;i<dict.values_.size();++i)
      {
          assert(fread(&s, sizeof(s), 1, f) == 1);
          char* b = new char[s+1];memset(b, 0, s+1);
          assert(fread(b, s, 1, f) == 1);
          dict.values_[i] = b;
      }
      fclose(f);
  }


}//namespace

#endif

