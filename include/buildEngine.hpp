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
        void GetShengMuByPinYin_(const std::vector<std::string>& pinyin
                                ,std::vector<std::string>& shm) {
        }

    public:
        BuildEngine() {
        }
        ~BuildEngine() {
        }

        // get data building results
        bool GetDataModule(std::vector<std::string>& terms
                           ,KeyTermIDsType& key_termids) {
        }

        // build from file
        bool Build(const std::string& nm) {
        }

        // build from vector
        bool Build(const std::vector<std::string>& termVec) {
        }

        // store results to files
        bool Flush(const std::string& termFile, const std::string& keyFile) {
        }

        // parse string to chars and words
        bool Parse(const std::string& str
                    ,std::vector<std::string>& chars
                    ,std::vector<std::string>& words) {
        }

        // generate prefix
        bool Generate(const std::string& term
                      ,const uint32_t& termid
                      ,KeyInfoType& key_info
                      ,const uint32_t len = 10) {
        }

        // generate key by prefix
        bool GenerateByPrefix(const std::vector<std::string>& chars
                              ,std::vector<std::string>& keys
                              ,const uint32_t len ) {
        }

        // generate key by word infix
        bool GenerateByWordInfix(const std::vector<std::string>& words
                                 ,std::vector<std::string>& keys
                                 ,const uint32_t len) {
        }

        // generate key by word suffix
        bool GenerateByWordSuffix(const std::vector<std::string>& words
                                  ,std::vector<std::string>& keys
                                  ,const uint32_t len) {
        }

        // generate key by pinyin prefix
        bool GenerateByPinYinPrefix(const std::string& str
                                    ,std::vector<std::string>& keys
                                    ,const uint32_t len) {
        }

        // generate key by sheng mu prefix
        bool GenerateByShengMuPrefix(const std::string& str
                                     ,std::vector<std::string>& keys
                                     ,const uint32_t len) {
        }
};

#endif // buildEngine.hpp
