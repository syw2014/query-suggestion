/*************************************************************************
 @ File Name: dictionary.h
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Wed 13 Jul 2016 05:41:24 PM CST
 ************************************************************************/
// Trie struct for quick find, pinyin tokenizer, pinyin -> chinese character
// chinese -> pinyin, fuzzy pinyin map, pinyin filter map.

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <iostream>
#include <fstream>

#include "util/normalize.h"
#include "util/darts.h"

#include <boost/lexical_cast.hpp>
#include <boost/unordered_map.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>


typedef Darts::DoubleArray Trie;
typedef uint16_t UCS2Char;
typedef boost::unordered_map<UCS2Char, std::vector<std::string> > Cn2PinYinType; // chinese word to pinyin list
typedef boost::unordered_map<std::string, std::vector<UCS2Char> > PinYin2CnType; // pinyin to word list


// class dictionary was created based on trie , usage of this class
// Dictioanry::Segment(): given a string, sement it into pinyin tokens, like "yinhang" , tokens:'yin','hang'; 
// Dictionary::GetChar(): given pinyin return it's chinese character list
// Dictionary::GetPinYin(): given a chinese char return it's pinyin list, do not distinguish ployphone.
class Dictionary
{
    private:
        Trie trie_; // trie tree
        Cn2PinYinType cn2pinyin_;         // chinese character -> pinyin list
        PinYin2CnType pinyin2cn_;         // pinyin -> chinese character list
        std::vector<std::string> pinyin_; // all single pinyin        
        boost::unordered_map<std::string, bool> filter_pinyin_; // filter pinyin
        
        // load pinyin and chinese character from file
        // @dir: the resource path
        void Load_(const std::string& dir) {
           
            // TODO:
            // quick load
            /* int32_t flag = 1; // to check if the bin file opened successfully.
            try {
                if ((flag = trie_.open((dir+"/pinyin.bin").c_str())) == 0) {
                }
            } catch(...) {  // do not throw exception
            } */
            
            std::ifstream ifs((dir+"/pinyin.txt").c_str());
            if(!ifs.is_open()) {
                std::cout << "Open " << (dir+"/pinyin.txt") << "failed!\n";
                return;
            }
            std::string line;
            std::set<std::string> pinyinSet;
            while (getline(ifs, line)) {
                boost::algorithm::trim(line);
                std::vector<std::string> vec;
                boost::algorithm::split(vec, line, boost::is_any_of(" "));
                if (vec.size() != 2)
                    continue;
                std::string cnChar = vec[0];
                std::string pinyin = vec[1].substr(0, vec[1].length()-1); 
                Normalize::ToUTF8(cnChar);
                // filter pinyin
                if (filter_pinyin_.find(pinyin) == filter_pinyin_.end())
                AddPinYinCnMap(pinyin, cnChar);
                pinyinSet.insert(pinyin);
            }
            pinyin_.insert(pinyin_.end(), pinyinSet.begin(), pinyinSet.end());
            std::cout << "Resouces loaded pinyin size: " << pinyin_.size() << std::endl; 
        }

    public:
        Dictionary(const std::string& dir) {
            cn2pinyin_.clear();
            pinyin2cn_.clear();
            pinyin_.clear();
            filter_pinyin_.clear();

            Init();
            LoadResource(dir);
        }
        ~Dictionary() {
        }

        void Init() {

            // add other pinyin
  /*          pinyin_.push_back("chon");
            pinyin_.push_back("con");
            pinyin_.push_back("don");
            pinyin_.push_back("gon");
            pinyin_.push_back("hon");
            pinyin_.push_back("jion");
            pinyin_.push_back("kon");
            pinyin_.push_back("lon");
            pinyin_.push_back("non");
            pinyin_.push_back("qion");
            pinyin_.push_back("ron");
            pinyin_.push_back("son");
            pinyin_.push_back("ton");
            pinyin_.push_back("xion");
            pinyin_.push_back("yon");
            pinyin_.push_back("zhon");
            pinyin_.push_back("zon");
*/
            // add filter pinyin
            filter_pinyin_.insert(std::make_pair("n", 1));
            filter_pinyin_.insert(std::make_pair("ng", 1));
            filter_pinyin_.insert(std::make_pair("m", 1));
            filter_pinyin_.insert(std::make_pair("o", 1));
        }
        // load resource from file
        void LoadResource(const std::string& dir) {
            if (dir.empty()) {
                std::cout << "directory is not exists!\n";
                return;
            }

            // load pinyin from file
            Load_(dir);
            // build trie
            std::size_t SIZE = pinyin_.size();
            std::vector<std::size_t> lengths(SIZE);
            typedef Darts::DoubleArray::value_type value_type;
            std::vector<value_type> states(SIZE);
            std::vector<const char*> keys(SIZE);
            for (uint32_t i = 0; i < SIZE; ++i) {
                keys[i] = pinyin_[i].c_str();
                lengths[i] = pinyin_[i].length();
                states[i] = i;
            }
            
            assert(keys.size() == pinyin_.size());
            trie_.build(keys.size(), &keys[0], &lengths[0], &states[0]);
            // TODO:
            // save bin file
        }

        // Add pinyin and chinese char into map
        void AddPinYinCnMap(const std::string& pinyin, const std::string& cnChar) {
            // Add cnChar
            Cn2PinYinType::iterator cnIter;
            std::vector<UCS2Char> cnChars;
            utf8::utf8to16(cnChar.begin(), cnChar.end(), std::back_inserter(cnChars));
            cnIter = cn2pinyin_.find(cnChars[0]);
            if (cnIter == cn2pinyin_.end()) { // not found in map
                std::vector<std::string> pinyin_list(1, pinyin);
                cn2pinyin_.insert(std::make_pair(cnChars[0], pinyin_list));
            } else { // add in the previous list
                std::vector<std::string>& pinyin_list = cnIter->second;
                std::vector<std::string>::iterator pyIter;
                pyIter = std::find(pinyin_list.begin(), pinyin_list.end(), pinyin);
                if (pyIter == pinyin_list.end()) {
                    pinyin_list.push_back(pinyin);
                }
            }

            // Add pinyin
            PinYin2CnType::iterator pyIter;
            pyIter = pinyin2cn_.find(pinyin);
            if (pyIter == pinyin2cn_.end()) {
                std::vector<UCS2Char> cnChar_list(1, cnChars[0]);
                pinyin2cn_.insert(std::make_pair(pinyin, cnChar_list));
            } else {
                std::vector<UCS2Char>& cnChar_list = pyIter->second;
                std::vector<UCS2Char>::iterator cnIter;
                cnIter = std::find(cnChar_list.begin(), cnChar_list.end(), cnChars[0]);
                if (cnIter == cnChar_list.end()) {
                    cnChar_list.push_back(cnChars[0]);
                }
            }
        }

        // pinyin tokenizer
        void Segment(const std::string& pinyin, std::vector<std::string>& result) {
            if (pinyin.empty())
                return;
            Fmm(pinyin, result);
        }

        // get chinese character based on pinyin string
        bool GetChar(const std::string& pinyin, std::vector<std::string>& result) {
            PinYin2CnType::iterator cnIter;
            cnIter = pinyin2cn_.find(pinyin);
            if (cnIter != pinyin2cn_.end()) {
                std::vector<UCS2Char> cnChars;
                cnChars = cnIter->second;
                uint32_t size = cnChars.size();
                result.resize(size);
                for (uint32_t i = 0; i < size; ++i) {
                    std::string utf8str;
                    Normalize::Utf16ToUTF8Str(cnChars[i], utf8str);
                    //std::cout << "T: " << utf8str << std::endl;
                    result[i] = utf8str;
                }
                return true;
            }
            return false;
        }

        // get pinyin list based on chinese character string
        bool GetPinYin(const std::string& cnChar, std::vector<std::string>& result) {
            result.clear();
            if (cnChar.empty()) {
                return false;
            }

            GetPinYin_(cnChar, "", result);
            return true;
        }

        // input chinese character and pinyin combination and get the pinyin
        // recursive function
        void GetPinYin_(const std::vector<UCS2Char>& cnChars,const std::string& mid_result
                        ,std::vector<std::string>& result_list) {
            if (result_list.size() >= 1024)
                return;

            std::vector<std::string> pinyin_term_list;
            
            // case 1, only chinese and has pinyin
            if (!cnChars.empty() && Normalize::IsChinese(cnChars[0])
                    && (GetPinYinTerm(cnChars[0], pinyin_term_list))) {
                std::vector<UCS2Char> remain(uchars.begin()+1,uchars.end());
                std::cout << "T1: " << remain.size() << std::endl;
                std::string new_mid(mid_result);
                for (uint32_t i = 0; i < pinyin_term_list.size(); ++i) {
                    std::string mid = new_mid + pinyin_term_list[i];
                    std::cout << "T2: " << mid << std::endl;
                    GetPinYin_(remain, mid, result_list);
                }
            } else {
                if (!cnChars.empty() && !Normalize::IsChinese(cnChars[0])) {
                    std::vector<UCS2Char> remain(uchars.begin()+1,uchars.end());
                    std::string tmp = Normalize::Utf16ToUTF8Str(remain[0]);
                    std::string mid = mid_result + tmp;
                    std::cout << "T3: " << mid << std::endl;
                    GetPinYin_(remain, mid, result_list);
                } else {
                    result_list.push_back(mid_result);
                    std::cout << "T4: " << mid_result << std::endl;
                }
            }
        }
        
        // get pinyin list from chinese character map
        bool GetPinYinTerm(const UCS2Char& cnChar, std::vector<std::string>& result) {
            Cn2PinYinType::iterator cnIter;
            cnIter = cn2pinyin_.find(cnChar);
            if (cnIter != cn2pinyin_.end()) {
                result = cnIter->second;
                return true;
            }
            return false;
        }

        // Reload
        // get pinyin list from chinese character map
        bool GetPinYinTerm(const std::string& cnChar, std::vector<std::string>& result) {
            std::vector<UCS2Char> cnChars;
            utf8::utf8to16(cnChar.begin(), cnChar.end(), std::back_inserter(cnChars));
            Cn2PinYinType::iterator cnIter;
            cnIter = cn2pinyin_.find(cnChars[0]);
            if (cnIter != cn2pinyin_.end()) {
                result = cnIter->second;
                return true;
            }
            return false;
        }

        // maximum match
        void Fmm(const std::string& line, std::vector<std::string>& r) {
            r.clear();
            std::string uline(line);
            std::vector<uint32_t> lens, cumu_lens;

            // remove invalid encoding
            Normalize::RemoveInvalidUTF8(uline);
            std::string::iterator it = uline.begin();
            while (it != uline.end()) {
                uint32_t code = utf8::next(it, uline.end());
                std::string _str; // insert from string back
                utf8::append(code, std::back_inserter(_str));
                lens.push_back(_str.length());
                if (cumu_lens.size() > 0) {
                    cumu_lens.push_back(_str.length()+cumu_lens.back());
                } else {
                    cumu_lens.push_back(_str.length());
                }
            }

            // start maximum match
            std::size_t key_pos = 0;
            for (std::size_t j = 0; j < lens.size(); ++j) {
                std::size_t last_j = j, jj = j;
                Trie::value_type last_state = -1;
                Trie::value_type state;
                std::size_t node_pos = 0;

                // traverse trie and check the node exist or not
                while (j < lens.size()
                    && (state=trie_.traverse(uline.c_str(), node_pos, key_pos, cumu_lens[j])) != -2) {
                    j++;
                    if (state < 0)
                        continue;
                    last_state = state;
                    last_j = j -1;
                }

                // found
                if (last_state >=0) {
                    std::string py;
                    if ((uint32_t)last_state < pinyin_.size()) {
                        py = std::string(uline.c_str()+cumu_lens[jj]-lens[jj],uline.c_str()+cumu_lens[last_j]);
                        r.push_back(py);
                    }
                } else {
                    std::string py;
                    py = std::string(uline.c_str()+cumu_lens[jj]-lens[jj], uline.c_str()+cumu_lens[jj]);
                    r.push_back(py);
                }
                j = last_j;
                key_pos = cumu_lens[j];
            }
        }
};

#endif // dictionary.h
