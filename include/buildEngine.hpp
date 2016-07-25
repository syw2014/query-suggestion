/*************************************************************************
 @ File Name: buildEngine.h
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Mon 11 Jul 2016 01:42:29 PM CST
 ************************************************************************/
// Data building module contains data process,tokenize, prefix gerenation
//

#ifndef MODULE_BUILD_ENGINE_HPP
#define MODULE_BUILD_ENGINE_HPP

#include <set>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/concept_check.hpp>
#include <boost/unordered_map.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

// segment
#include "segmentWrapper.h"
#include "dictionary.hpp"

typedef std::vector<std::pair<uint32_t, double> > TermIDTFType;
typedef boost::unordered_map<std::string, TermIDTFType> KeyInfoType;
typedef boost::unordered_map<std::string, std::vector<uint32_t> > KeyTermIDsType;

// Build engine for data module build
// Provide  interfaces for data module building.
class BuildEngine {
    private:
        static const double prefix_w_ = 10000.0;       // different weight for different prefix type
        static const double pinyin_prefix_w_ = 1000.0; 
        static const double shengmu_w_ = 100.0; 
        static const double word_infix_w_ = 10.0;
        static const double word_suffix_w_ = 1.0;

        static const uint32_t topK_ = 15;     // default number of candidate words

        static std:string res_dir_;           // resource directory, pinyin and token resource

        // main data structure
        std::auto_ptr<SegmentWrapper> segWrapper_;  // chinese string tokenizer
        std::auto_ptr<Dictionary> pySegDict_;       // pinyin tokenizer
        
        std::vector<std::string> terms_;  // total term set generated from corpus
        std::vector<double> tf_;          // term frequency , it was corresponded to words in terms_
        KeyTermIDsType key_termIds_;      // key(prefix) to term ids map

        std::set<std::string> shm_ ;      // total sheng mu set


        // extract sheng mu from pinyin
        // @pinyin: pinyin string vector
        // @shm: sheng mu extract form pinyin
        // eg: 行 hang, xing shengmu: h,x
        void GetShengMuByPinYin_(const std::vector<std::string>& pinyin
                                ,std::vector<std::string>& shm) {
            shm.clear();
            if (pinyin.empty())
                return;
            
            std::set<std::string> head;
            for (uint32_t i = 0; i < pinyin.size(); ++i) {
                const std::string& py = pinyin[i];
                if (py.empty())
                    continue;
                else {
                    std::string ch;
                    ch += py[0]; // first char
                    if ((char)py[0] >= 0) {
                        if (head.insert(ch).second)
                            shm.push_back(ch);
                    } else {
                        if (head.insert(py).second)
                            shm.push_back(py);
                    }
                }
            }
        }

        // store elements and it's weight in vector into map
        // @keys: 
        // @key_info: key information map
        // @termid: the index id of string which generated keys
        // @weight: keys weight
        void StoreInKeyMap(const std::vector<std::string>& keys
                           ,KeyInfoType& key_info
                           ,const uint32_t termid
                           ,const double weight) {
            if (keys.empty())
                return;
            for (uint32_t idx = 0; idx < keys.size(); ++idx) {
                key_info[keys[idx]].push_back(std::make_pair(termid, weight));
            }
        }

    public:
        BuildEngine() {
            // resource directory check
            if (!boost::filesystem::exists(res_dir_)) {
                std::cout << "resouce directory: " << res_dir_ << "is not exist!" << std::endl;
                std::cout << "Tips: resource directory may like: \"../resource/\"" << std::endl;
                return;
            }

            // construct tokenizer
            segWrapper_.reset(new SegmentWrapper(res_dir_+"dict"));
            pySegDict_.reset(new Dictionary(res_dir_+"cn"));

            // load shengmu from files.
            // b,p,m,f,d,t,n,l,g,k,k,j,q,x,zh,ch,sh,r,z,c,s,y,w
            std::ifstream ifs((res_dir_+"cn/ShengMu.txt"));
            if (ifs) {
                std::cout << "Open file " << (res_dir_+"cn/ShengMu.txt") << "failed!" << std::endl;
                return;
            }
            std::string line;
            while (getline(ifs, line)) {
                boost::algorithm::trim(line);
                if (line.empty())
                    continue;
                shm_.insert(line);
            }
            ifs.close();
        }

        ~BuildEngine() {
            terms_.clear();
            tf_.clear();
            key_termIds_.clear();
        }

        // get data building results
        bool GetDataModule(std::vector<std::string>& terms
                           ,KeyTermIDsType& key_termids) {
            terms.clear();
            key_termids.clear():

            terms.swap(terms_);
            key_termids.swap(key_termids_);
        }

        // build data module from file
        // Notes: 
        // data structure in file must be like:
        // term \t freq \t result_num
        // the separator is tab
        bool Build(const std::string& nm) {
            terms_.clear();
            tf_.clear();
            key_termIds_.clear();
            
            std::ifstream ifs(nm.c_str());
            if(!ifs) {
                std::cout << "File " << nm << "open failed!" << std::endl;
                return false;
            }

            std::cout << "Start building...\n";
            std::string line;
            std::map<std::string, double> t_freq;
            // extract term and it's freq
            while (getline(ifs, line)) {
                if (line.empty())
                    continue;
                boost::algorithm::trim(line);
                std::size_t pos = line.find("\t");
                if (pos == std::string::npos)
                    continue;
                std::vector<std::string> vec;
                std::string term;
                boost::algorithm::split(vec, line, boost::algorithm::is_any_of("\t"));
                // the data must contians term and it's frequency
                if (vec.size() < 2)
                    continue;
                term = vec[0];
                double freq = 0.0;
                try {
                    freq = boost::lexical_cast<double>(vec[1]);
                } catch(...) {
                   // freq = 1.0;
                    std::cout << "bad line in:" << nm << ":" << line << std::endl;
                    continue;
                }
                t_freq[term] = freq;
            }
            ifs.close();
            // step2, store term and it's freq
            std::map<std::string, double>::iterator it = t_freq.begin();
            uint32_t size = t_freq.size();
            terms_.resize(size);
            tf_.resize(size);
            for (uint32_t idx = 0; it != t_freq.end() && idx < size; ++it, ++idx) {
                terms_[idx] = it->first;
                tf_[idx] = it->second;
            }
            t_req.clear();

            // step3, generate keys
            KeyInfoType key_info;
            for (uint32_t idx = 0; idx < terms_.size(); ++idx) {
                Generate(terms_[i], idx, key_info);
            }

            // step4 , compuate score for every key
            KeyInfoType::iterator iter;
            for (iter = key_info.begin(); iter != key_info.end(); ++iter) {
                std::vector<std::pair<uint32_t, double> >& info = iter->second;
                // score = tf * weight;
                for (uint32_t i = 0; i < info.size(); ++i) {
                    if (info[i].first > term_freq_.size()) {
                        info[i].second = 1.1;
                        continue;
                    }
                    info[i].second = term_freq_[info[i].first] * info[i].second;
                }
                sort(info.begin(), info.end(), sort<uint32_t, double>::sortDescendBySecond);
                // result deduplication
                std::vector<uint32_t> termsid;
                std::set<uint32_t> uniq_ids;
                for (uint32_t i = 0; i < info.size() && uniq_ids.size() <= 15; ++i) {
                    if (uniq_ids.insert(info[i].first).second) {
                        termsid.push_back(info[i].first);
                    }
                }
                key_termids_[it->first].swap(termsid);
                uniq_ids.clear();
            }

            std::cout << "key_termids size: " << key_termids_.size() << std::endl;
            std::cout << "Building completed!\n";
        }

        // build from vector
        bool Build(const std::vector<std::string>& termVec) {
        }

        // store results to files
        // @termFile: file to store all terms
        // @keyFile: store all keys and term ids
        bool Flush(const std::string& termFile, const std::string& keyFile) {
            std::ofstream ofs_term(termFile.c_str());
            if (!ofs_term) {
                std::cout << "open file " << termFile << "failed!\n";
                return false;
            }
            std::ofstream ofs_key(keyFile.c_str());
            if (!ofs_key) {
                std::cout << "open file " << keyFile << "failed!\n";
                return false;
            }
            // store terms
            for (uint32_t i = 0; i < terms_.size(); ++i) {
                ofs_term << i << "\t" << terms_[i] << "\n";
            }
            ofs_term.close();

            // store keys
            KeyTermIDsType::iterator iter;
            for (iter = key_itermids_.begin(); iter != key_itermids_.end(); ++iter) {
                std::vector<uint32_t>& ids = iter->second;
                // candidate is itself
                // do not suggestion itself
                if (ids.size() == 1 && ids[0] < terms_.size() && terms_[ids[0]] == it->first)
                    continue;
                ofs_key << it->first; // key
                for (uint32_t i = 0; i < ids.size(); ++i) {
                    // make sure id is in the range of term vector
                    if (ids[i] > terms_.size())
                        continue;
                    // do not suggest itself
                    if (it->first == terms_[ids[i]])
                        continue;
                    ofs_key << "\t" << terms_[ids[i]];
                }
                ofs_key << "\n";
            }
            ofs_key.close();

            return true;
        }

        // parse string into chars and words
        // @chars: single unicode 
        // @words: string segmentation results
        bool Parse(const std::string& str
                    ,std::vector<std::string>& chars
                    ,std::vector<std::string>& words) {
            if (str.empty())
                return false;

            // extract chars
            chars.clear();
            std::vector<UnicodeType> unicodes;
            if (Normalize::ToUnicode(str, unicodes)) {
                //if(unicodes.empty())
                //    return false;
                chars.resize(unicode.size());
                for (uint32_t i = 0; i < unicodes.size(); ++i) {
                    std::string unicode;
                    Normalzie::UnicodeToUTF8Str(unicodes[i], unicode);
                    chars[i] = unicode;
                }
            }

            // extract words
            words.clear();
            segWrapper_->segment(str, words, false);

            return true;
        }

        // generate prefix
        // @str: segment tokens
        // @termid: token index
        // @key_info: index key, it's corresponding term id and tf
        // @num: number of keys
        bool Generate(const std::string& str
                      ,const uint32_t termid
                      ,KeyInfoType& key_info
                      ,const uint32_t num = 10) {
            if (str.empty())
                return false;
            
            std::vector<std::string> chars;
            std::vector<std::string> words;

            Parse(str, chars, words);

            std::vector<std::string> keys;

            // Start generation
            GenerateByPrefix(chars, keys, num);
            StoreInKeyMap(keys, key_info, termid, prefix_w_);
            
            GenerateByPinYinPrefix(str, keys, num);
            StoreInKeyMap(keys, key_info, termid, pinyin_prefix_w_);

            GenerateByShengMuPrefix(term, keys, num);
            StoreInKeyMap(keys, key_info, termid, shengmu_w_);

            GenereateByWordInfix(words, keys, num);
            StoreInKeyMap(keys, key_info, termid, word_infix_w_);

            GenerateByWordSuffix(words, keys, num);
            StoreInKeyMap(keys, key_info, termid, word_suffix_w_);

            return true;
        }

        // generate key by prefix
        // @chars: unicodes for input string
        // @keys: index key generated based on every unicode
        // @num: the number of unicodes chosen to be stored in keys 
        bool GenerateByPrefix(const std::vector<std::string>& chars
                              ,std::vector<std::string>& keys
                              ,const uint32_t num ) {
            keys.clear();
            std::string prefix("");
            for (uint32_t i = 0; i < chars.size() && i < num; ++i) {
                prefix += chars[i];
                keys.push_back(prefix);
            }

            return true;
        }

        // generate key by word infix,
        // @words: segment tokens
        // @keys: index key generated based on infix in token terms, if the token number < 3
        //        then return directly, as there are only two chinese characters in string, no infix.
        // @num: the number of words chosen to be processed.
        bool GenerateByWordInfix(const std::vector<std::string>& words
                                 ,std::vector<std::string>& keys
                                 ,const uint32_t num) {
            keys.clear();
            uint32_t size = words.size();
            if (size < 3)
                return false;

            for (uint32_t i = 0; i < size - 1; ++i )
                keys.push_back(words[i]);

            return true;
        }

        // generate key by last term(suffix)
        // @workds: segment tokens
        // @keys: index keys 
        // @num: the number of words chosen to be processed
        bool GenerateByWordSuffix(const std::vector<std::string>& words
                                  ,std::vector<std::string>& keys
                                  ,const uint32_t num) {
            keys.clear();
            if (words.size() > 1)
                keys.push_back(words.back());
            else
                return false;

            return true;
        }

        // generate key by pinyin prefix
        // @str: input string, can be any combination
        // @keys: index key generated based on pinyin prefix which generated by Dictionary::GetPinYin()
        // @len: the length of prefix 
        bool GenerateByPinYinPrefix(const std::string& str
                                    ,std::vector<std::string>& keys
                                    ,const uint32_t len) {
            keys.clear();
            if (str.empty())
                return false;
            // get pinyin from pinyin module
            std::vector<std::string> pinyin;
            pySegDict_->GetPinYin(str, pinyin);
            
            // extract prefixes from pinyin string
            std::set<std::string> prefix;
            for (uint32_t i = 0; i < pinyin.size(); ++i) {
                std::string pre("");
                // process every pinyin
                for (uint32_t j = 0; j < pinyin[i].size(); ++j) {
                    pre += pinyin[i][j]; // each letter
                    if (prefix.insert(pre).second && pre.length() < num)
                        keys.push_back(pre);
                }
            }
            
            return true;
        }

        // generate key by sheng mu prefix
        // @str: input string
        // @keys: index key generated based on sheng mu prefix of pinyin
        // @len:
        // eg: 银行          ShengMu
        //     yin       ->   y
        //     hang xing ->  y x(polyphone)
        //     result: y, yx
        bool GenerateByShengMuPrefix(const std::string& str
                                     ,std::vector<std::string>& keys
                                     ,const uint32_t len) {
            keys.clear();
            if (str.empty())
                return false;

            std::vector<std::vector<std::string> > shm_list;
            // convert string to unicodes
            std::vector<UCS2Char> unicodes;
            Normalize::ToUnicode(str, unicodes);
            if (unicodes.empty())
                return false;
            for (uint32_t i = 0; i < unicodes.size(); ++i) {
                // get one letter
                UCS2Char uchar = unciodes[i];
                std::string ustr;
                Normalize::UnicodeToUTF8Str(uchar, ustr);
                // type inditification
                // space
                if (ustr == " ") {
                    shm_list.push_back(std::vector<std::string>(1, " ")); // insert a space
                } else {
                    // is chinese char, 
                    if (Normalize::IsChinese(uchar)) {
                        std::vector<std::string> shm;
                        std::vector<std::string> pinyin;
                        pySegDict_->GetPinYin(ustr, pinyin); // extract pinyin
                        GetShengMuByPinYin_(pinyin, shm);    // extract sheng mu
                        shm_list.push_back(std::vector<std::string>());
                        shm_list.back().swap(shm);
                    } else {
                        // is alphabet / digital
                        shm_list.push_back(ustr);
                    }
                }
            }
        }
};

#endif // buildEngine.hpp
