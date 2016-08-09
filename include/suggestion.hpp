/*************************************************************************
 @ File Name: suggestion.hpp
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Tue 26 Jul 2016 02:32:52 PM CST
 ************************************************************************/
// Query suggestion module is used to create index for data module
// and the output interfaces for query suggestion,which contains data update
// strategy and the final results.

#ifndef SUGGESTION_ENGINE_HPP
#define SUGGESTION_ENGINE_HPP

#include <iostream>
#include "buildEngine.hpp"

// static global resource directory in build engine
std::string BuildEngine::res_dir_ = "";
// Suggestion class to used for getting the final result and data updating.
class Suggestion {
    private:
        boost::shared_ptr<BuildEngine> pBuild_;  // data building pointer
    
        KeyTermIDsType key_termids_; // prefix and corresponding term ids
        TermInfoType termsInfo_;     // all the term's information(tf, reserve field,stored based on their ids)
        
         std::string res_dir_;        // resource directory

    public:
        Suggestion(const std::string& res_dir)
           : res_dir_(res_dir) {
            if (!boost::filesystem::exists(res_dir)) {
                std::cout << "Resource directory " << res_dir << " not exists!\n";
                std::cout << "The resource directory may like \"../resource/\"\n";
                return;
            }
            BuildEngine::res_dir_ = res_dir_; 
            pBuild_.reset(new BuildEngine());
        }
        
        ~Suggestion() {
            key_termids_.clear();
            termsInfo_.clear();
        }

        // get final suggestion results
        // @key: user input string
        // @JsonRes: suggestion results in the form of json
        void GetJsonResult(const std::string& key, std::string& JsonRes) {

            std::vector<std::string> terms, attrs;
            GetSuggestion(key, terms, attrs);

            
            JsonRes = "[";
            if (terms.empty()) {
                JsonRes += "]";
                return;
            }

            for (uint32_t idx = 0; idx < terms.size(); ++idx) {
                if (terms[idx].empty()) 
                    continue;
                JsonRes += "{\"term\":\"" + terms[idx] + "\"";
                JsonRes += ",\"total_count\":" + attrs[idx] + "},";
            }

            // delete the last ','
            if (*JsonRes.rbegin() == ',')
                JsonRes.erase(JsonRes.size() - 1);
            JsonRes += "]";
        }

        // remove more space only reserve one space
        // @input: the input string
        // @return: return the lowercase
        std::string RemoveSpace(const std::string& input) {
            uint32_t state = 0, start = 0, end = input.length();
            // from start to find the first element which is not space or tab
            for (; start < input.size(); ++start) {
                if (input[start] != ' ' && input[start] != '\t') break;
            }
            
            // from end to find the first element which is not space or tab
            for (; end > 0;--end) {
                if (input[end-1] != ' ' && input[end - 1 ] != '\t') break;
            }

            // remove extra spaces keep only one in string
            std::string res("");
            for (; start < end; ++start) {
                if (input[start] == ' ' || input[start] == '\t')
                    ++state;
                else 
                    state = 0;

                if (state == 0) 
                    res += input[start];
                else if (state == 1)
                    res += ' ';
            }

            boost::to_lower(res);
            return res;
        }
        
        // get query suggestion according input key
        // @terms: candidate terms
        // @attrs: candidate term attributes
        // TODO: 
        // needs to be locked when read the data
        bool GetSuggestion(const std::string& key
                ,std::vector<std::string>& terms
                ,std::vector<std::string>& attrs) {
            terms.clear();
            attrs.clear();

            std::string nkey = RemoveSpace(key);
            
            // get results if exists
            KeyTermIDsType::iterator keyIter;
            keyIter = key_termids_.find(key);
            if (keyIter == key_termids_.end()) 
                return false;

            std::vector<uint32_t>& termIds = keyIter->second;
            uint32_t size = termIds.size();
            terms.resize(size);
            attrs.resize(size);
            for (uint32_t i = 0; i < size; ++i) {
                if (termIds[i] > termsInfo_.size())
                    continue;
                terms[i] = termsInfo_[termIds[i]].first; // term
                try {  // term result number
                    terms[i] = boost::lexical_cast<std::string>(termsInfo_[termIds[i]].second.second);
                } catch(...) {
                    terms[i] = "12";
                }
            }
            if (terms.size() != attrs.size())
                return false;

            return true;
        }

        // get data building module
        void GetDataModule(TermInfoType& termsInfo, KeyTermIDsType& key_termids) {

            pBuild_->GetDataModule(termsInfo, key_termids);
        }
        
        // Rebuilding the module and update data
        // TODO:
        // may consider data intergrity and validity so will be use write and read lock
        void ModuleUpate() {
        }

        // building data module with specified term file
        // @filename: term files the structure like [term '\t' tf '\t' result_num or other attribute(uint32_t)]
        void Build(const std::string& filename) {
            
            pBuild_->Build(filename);
            GetDataModule(termsInfo_, key_termids_);
        }

};

#endif // suggestion.hpp
