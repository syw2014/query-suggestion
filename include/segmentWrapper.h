/*************************************************************************
 @ File Name: segmentWrapper.h
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Mon 16 May 2016 11:16:23 AM CST
 ************************************************************************/
#ifndef SEGMENTWRAPPER_H
#define SGEMENTWRAPPER_H

#include <iostream>
#include "segment/segment.h"
//#include "PropSharedLock.h"

#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>

// Sort by second
// NOTE: Typename R muse be comparable
template<typename T, typename R>
class SORT
{
    public:
        typedef std::pair<T, R> VectorPair;
        
        static bool sortDescendBySecond(const VectorPair& lhs, const VectorPair& rhs){
            return lhs.second > rhs.second;
        }
        
        static bool sortAscendBySecond(const VectorPair& lhs, const VectorPair& rhs){
            return lhs.second < rhs.second;
        }
};

class SegmentWrapper
{
    private:
        std::string dictDir_;
        boost::unordered_set<std::string> stopWords_;
        knlp::HorseTokenize* tok_;
       // boost::unordered_map<int, std::vector<std::string> > itemID_tokens_;
       // std::vector<int> itemIDs_;

    public:
        SegmentWrapper(const std::string& dictDir)
          :dictDir_(dictDir),tok_(NULL){
              tok_ = new knlp::HorseTokenize(dictDir);
              loadSpecialWords((dictDir_+"/stop_words.utf8"));
          }

        ~SegmentWrapper(){
            if(tok_ != NULL)
                delete tok_;
            tok_ = NULL;
        }

        bool isDigital(uint16_t uchar){
            uint16_t zero = '0';
            uint16_t nine = '9';
            if(zero <= uchar && uchar <= nine)
                return true;
            return false;
        }

        bool isAlpha(uint16_t uchar){
            uint16_t a = 'a';
            uint16_t z = 'z';
            uint16_t A = 'A';
            uint16_t Z = 'Z';
            if((a <= uchar && uchar <= z)||(A <= uchar && uchar <= Z))
                return true;
            return false;
        }

        // To check a term si alphanumberic after removing dot
        bool isAlphaNumberic(const std::string& term){
            std::string nstr = boost::replace_all_copy(term, ".", "");
            std::string::size_type i;
            for(i = 0; i < nstr.size(); ++i){
                if(isDigital(nstr[i]) || isAlpha(nstr[i]))
                    continue;
                return false;
            }
            return true;
        }

        bool loadSpecialWords(const std::string& filename){
            if(filename.empty())
                return false;
            std::ifstream ifs(filename.c_str());
            std::string line;
            while(getline(ifs, line)){
                if(line.empty())
                    continue;
                boost::algorithm::trim(line);
                stopWords_.insert(line);
            }
            ifs.close();
            assert(stopWords_.size());
            return true;
        }
        
        // Clean rules
        bool isNeedClean_(const std::string& term){
            if(term.empty())
                return true;
            if(term.length() < 4 || term.length() > 31)
                return true;
            if(isAlphaNumberic(term))
                return true;
            return false;
        }

        bool isNeedClean(const std::string& token){
            if(stopWords_.end() != stopWords_.find(token) || isNeedClean_(token))
                return true;
            return false;
        }
        
        // dedup: it's true means you want to remove the deduplicate words afterm segmentation
        void segment(const std::string& line, std::vector<std::string>& token, bool dedup=true){
            token.clear();
            std::vector<std::pair<std::string, float> > tmp;
            try{
                tok_->tokenize(line, tmp);
            }catch(...){
                tmp.clear();
            }
            token.resize(tmp.size());
            for(uint32_t i = 0; i < tmp.size(); ++i)
                token[i] = tmp[i].first;
            if(dedup){
                std::set<std::string> set_(token.begin(), token.end());
                token.clear();
                std::copy(set_.begin(), set_.end(), std::back_inserter(token));
            }
        }

        // Segmentation and store tokens
        std::vector<std::string> segment(const std::string& title){
            std::vector<std::string> tokens;
            tokens.clear();
            if(title.empty())
                return tokens;
            // Lock
           // bool isLock;
           // ScopedWriteBoolLock lock(mutex_, isLock);

           // std::vector<std::string> tokens;
            subSegment(title, tokens, false);
            bigramModel(tokens);
            return tokens;
           /* boost::unordered_map<int, std::vector<std::string> >::iterator it;
            if(it != itemID_tokens_.end())
                it->second = tokens;
            else{
                itemID_tokens_[itemID] = tokens;
                itemIDs_.push_back(itemID);
            }*/
        }

        // Given itemid ,return title token set
       /* void getTokens(int itemID, std::vector<std::string>& token){
            bool isLock;
            ScopedReadBoolLock lock(mutex_, isLock);
            token.clear();
            boost::unordered_map<int, std::vector<std::string> >::iterator it;
            it = itemID_tokens_.find(itemID);
            if(it != itemID_tokens_.end())
                token = it->second;
        }
        
        std::vector<int> getItemIDs(){
            bool isLock;
            ScopedReadBoolLock lock(mutex_, isLock);
            return itemIDs_;
        }*/
        // sub tokenize
        void subSegment(const std::vector<std::string>& token, std::vector<std::string>& subtoken){
            subtoken.clear();
            if(token.empty())
                return;
            std::vector<std::pair<std::string, float> > tmp, subtmp;
            tmp.resize(token.size());
            for(uint32_t i = 0; i < token.size(); ++i)
                tmp[i] = std::make_pair(token[i], 1.0);
            tok_->subtokenize(tmp,subtmp);
            subtoken.resize(subtmp.size());
            for(uint32_t j = 0; j < subtmp.size(); ++j)
                subtoken[j] = subtmp[j].first;
        }
        // dedup: it's true means you want to remove the deduplicate words afterm segmentation
        //
        void subSegment(const std::string& line, std::vector<std::string>& token, bool dedup=true){
            token.clear();
            std::vector<std::pair<std::string, float> > tmp, subtmp, subtoken;
            try{
                tok_->tokenize(line, tmp);
            }catch(...){
                tmp.clear();
            }
            for(uint32_t i = 0; i < tmp.size(); ++i)
            {
                if(tmp[i].first.length() > 9){
                    tok_->subtokenize(tmp[i].first, subtoken);
                    subtmp.push_back(tmp[i]);
                    for(uint32_t j = 0; j < subtoken.size(); ++j)
                        token.push_back(subtoken[j].first);
                }
                else
                    token.push_back(tmp[i].first);
            }
            if(dedup){
                std::set<std::string> set_(token.begin(), token.end());
                token.clear();
                std::copy(set_.begin(), set_.end(), std::back_inserter(token));
            }
        }

        // Extend terms based on bigram
        // before extension: t1, t2, t3
        // after extension: t1,t2,t3, t1_t2, t2_t3
        void bigramModel(std::vector<std::string>& token){
            // clean tokens
            std::vector<std::string>::iterator it;
            for(it = token.begin(); it != token.end(); ++it){
                if((*it).empty() || isNeedClean(*it)){
                    token.erase(it);
                    it--;
                }
            }
            std::vector<std::string> tmp;
            tmp.swap(token);
            std::size_t i, j, size = tmp.size();
            std::string bigram;
            for(i = 0; i < size; ++i){
                token.push_back(tmp[i]);
                j = i + 1;
                if(j < size){
                    bigram = tmp[i] + tmp[j];
                    token.push_back(bigram);
                }
            }
        }

        // Find the intersection between token1 and token2
        void intersect(const std::vector<std::string>& token1
                        ,const std::vector<std::string>& token2
                        ,std::vector<std::string>& result){
            result.clear();
            if(token1.empty() || token2.empty())
                return;
            boost::unordered_map<std::string, int> token;
            for(uint32_t i = 0; i < token1.size(); ++i)
                token.insert(std::make_pair(token1[i], 1));
            for(uint32_t j = 0; j < token2.size(); ++j)
                if(token.end() != token.find(token2[j]))
                    result.push_back(token2[j]);
        }

        // Compute content similarity between t1 and t2, we choose the simplest way
        // sim = (intersection-size / token1.size()) * (intersection-size / token2.size())
        // we don't consider the semantic similarity, so most of the similarity maybe zero. 
        double computeContentSim2(const std::string& t1, const std::string& t2){
            double sim = 0.0;
            if(t1.empty() || t2.empty())
                return sim;
            std::vector<std::string> token1, token2,result;
            subSegment(t1, token1, false);
            bigramModel(token1);
            subSegment(t2, token2, false);
            bigramModel(token2);
            intersect(token1, token2, result);
            // To ensure the size of token is big than zero
            if(token1.empty() || token2.empty())
                return sim;
            sim = ((double)result.size()/token1.size()) * ((double)result.size()/token2.size());
            return sim;
        }
        
        // Through ItemID find it's token set and compute it's similarity.
       /* double computeContentSim(int itemID1, int itemID2){
            
            bool isLock;
            ScopedReadBoolLock lock(mutex_, isLock);

            double sim = 0.0;
            std::vector<std::string> token1, token2,result;
            boost::unordered_map<int, std::vector<std::string> >::iterator it;
            it = itemID_tokens_.find(itemID1);
            if(it == itemID_tokens_.end() || it->second.empty())
                return sim;
            token1 = it->second;
            it = itemID_tokens_.find(itemID2);
            if(it == itemID_tokens_.end() || it->second.empty())
                return sim;
            token2 = it->second;
            
            intersect(token1, token2, result);
            sim = ((double)result.size()/token1.size()) * ((double)result.size()/token2.size());
            return sim;
        }*/

        double computeContentSim(const std::vector<std::string>& token1
                            ,const std::vector<std::string>& token2){
            double sim = 0.0;
            if(token1.empty() || token2.empty())
                return sim;
            std::vector<std::string> result;
            intersect(token1, token2, result);
            sim = ((double)result.size()/token1.size()) * ((double)result.size()/token2.size());
            return sim;
        }
};


#endif // segmentWrapper.h
