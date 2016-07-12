/*************************************************************************
 @ File Name: pinyinDict.h
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Mon 07 Mar 2016 02:04:09 PM CST
 ************************************************************************/
#ifndef PINYINDICT_H
#define PINYINDICT_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include "util/string/kstring.hpp"
#include "knlp/normalize.h"
#include "knlp/darts.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

class PinyinDict
{
    public:
        typedef Darts::DoubleArray Trie;

    private:
        Trie trie_;
        std::vector<std::string> pinyins_;
        boost::unordered_map<std::string, bool> filter_pinyin_;

    void load_(const std::string& nm){

        int32_t f = 1;
        try{
            if((f = trie_.open((nm+"/pinyin.bin").c_str())) == 0)
            {
            }
        }catch(...){}

        std::ifstream ifs((nm+"/pinyin.txt").c_str());
        std::string line;
        std::set<std::string> pinyinSets;
        while(getline(ifs, line)){
            boost::algorithm::trim(line);
            std::vector<std::string> vec;
            boost::algorithm::split(vec, line, boost::algorithm::is_any_of(" "));
            if(vec.size() != 2) continue;
            std::string pinyin = vec[1].substr(0, vec[1].length()-1);
            //std::cout << "Test!!! " << pinyin << std::endl;
            pinyinSets.insert(pinyin);
        }
        //build
       // std::vector<std::string> pvecs(pinyinSets.begin(),pinyinSets.end());
       // pinyins_.swap(pvecs);
       pinyins_.insert(pinyins_.begin(),pinyinSets.begin(),pinyinSets.end());
       std::cout << "PinyinDict:\t" << pinyins_.size() << std::endl;
       
       std::vector<std::size_t> lengths(pinyins_.size());
       typedef Darts::DoubleArray::value_type value_type;
       std::vector<value_type>  states(pinyins_.size());
       std::vector<const char*> keys(pinyins_.size());
       for(uint32_t i = 0; i < pinyins_.size(); ++i){
           keys[i] = pinyins_[i].c_str();
           lengths[i] = pinyins_[i].length();
           states[i] = i;
       }

       assert(keys.size() == pinyins_.size());
       trie_.build(keys.size(), &keys[0], &lengths[0], &states[0]);
       // trie_.save((nm+"/pinyin.bin").c_str());
    }

    public:
        PinyinDict(const std::string& nm){
            load_(nm);
            if(pinyins_.size() == 0) return;
        }

        ~PinyinDict()
        {}
        void fmm(std::string line, std::vector<std::string>& r) const{
            r.clear();
            std::vector<uint32_t> lens, cumu_lens;
            for(uint32_t i = 0; i < line.length();++i){
                std::string str = line.substr(i,1);
                lens.push_back(str.length());
                if(cumu_lens.size() > 0)
                    cumu_lens.push_back(str.length()+cumu_lens.back());
                else
                    cumu_lens.push_back(str.length());
            }

            std::size_t key_pos = 0;
            for(std::size_t j = 0; j < lens.size(); ++j){
                std::size_t last_j = j, jj=j;
                Darts::DoubleArray::value_type last_state = -1;
                Darts::DoubleArray::value_type state;
                std::size_t node_pos = 0;
                while(j<lens.size()
                  &&(state=trie_.traverse(line.c_str(), node_pos, key_pos, cumu_lens[j])) !=-2)
                {
                    j++;
                    if(state < 0) continue;
                    last_state = state;
                    last_j = j-1;
                }

                // found
                if(last_state >=0){
                    std::string py;
                    if((uint32_t)last_state < pinyins_.size()){
                        //val = pinyins_[last_state];
                        py = std::string(line.c_str()+cumu_lens[jj]-lens[jj], line.c_str()+cumu_lens[last_j]);
                        r.push_back(py);
                    }
                }else{
                    std::string py;
                    py = std::string(line.c_str()+cumu_lens[jj]-lens[jj], line.c_str()+cumu_lens[jj]);
                    r.push_back(py);
                }
                j = last_j;
                key_pos = cumu_lens[j];
            }
        }
        void merge(std::vector<std::string>& pylist)const{
            std::vector<int32_t> flags(pylist.size(),0);
            for(uint32_t i = 0; i < pylist.size(); ++i){
                uint32_t j = 0;
                for(;j < pylist[i].length();j++)
                    if((pylist[i][j] >= '0' && pylist[i][j] <= '9')
                      || (pylist[i][j] >= 'a' && pylist[i][j] <= 'z' && pylist[i].length() == 1))
                        break;
                
                if(j < pylist[i].length()) flags[i] = 1;
            }

            for(uint32_t i = 0; i < pylist.size(); ++i)
                if(flags[i]){

                    uint32_t t = i;
                    i++;
                    while(i<pylist.size() && flags[i]){
                        pylist[t] += pylist[i];
                       // std::cout << "Test text: " << pylist[t] << '\n';
                        pylist.erase(pylist.begin()+i);
                        flags.erase(flags.begin()+i);
                    }
                }
        }

        // Return value:
        // True: is in dict  
        bool hasKey(std::string& key)const{
            Trie::result_pair_type res;
            trie_.exactMatchSearch(key.c_str(), res, key.length());
            if(res.length != 0 && res.value != -1)
                return true;
            return false;
        }
};

#endif // pinyinDict.h

