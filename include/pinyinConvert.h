/*************************************************************************
 @ File Name: pinyinConvert.h
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Wed 09 Mar 2016 10:30:45 AM CST
 ************************************************************************/
#ifndef PINYINCONVERT_H
#define PINYINCONVERT_H

#include <iostream>
#include <boost/unordered_map.hpp>
#include <boost/tuple/tuple.hpp>
#include "mtrie.h"
#include "py_types.h"

class PinYinConvert
{
   
   typedef MTrie<std::string, uint32_t> PinyinDictType;
   typedef MTrie<std::string, uint32_t, std::string> FuzzyDictType;
   typedef boost::unordered_map<izenelib::util::UCS2Char, std::vector<std::string> > Cn2PinyinType;
   typedef boost::unordered_map<std::string, std::vector<izenelib::util::UCS2Char> > Pinyin2CnType;

   typedef boost::tuple<uint32_t, uint32_t izenelib::util::UString> QueryLogType;
   typedef std::pair<uint32_t, izenelib::util::UString> PropertyLabelType;
   struct TransProbType
   {
       boost::unordered_map<Unigram, double> u_trans_prob_;
       boost::unordered_map<Bigram, double> b_trans_prob_;
       boost::unordered_map<Trigram, double> t_trans_prob_;
       void clear()
       {
           u_trans_prob_.clear();
           b_trans_prob_.clear();
           t_trans_prob_.clear();
       }
       bool empty() const
       {
           return u_trans_prob_.empty() && b_trans_prob_.empty() && t_trans_prob_.empty();
       }
   };

public:
   explicit PinYinConvert(const std::string& collection_dir = "");
   

   // load resource
   bool load();
   
   bool getResult(const izenelib::util::UString& input,
        std::vector<std::pair<double, std::string> >& pinyin_list,
        std::vector<izenelib::util::UString>& output);

   void AddPinyinMap(const std::string& pinyin, const izenelib::util::UCS2Char& cn_char);

   void getPinyin(const izenelib::util::UString& cn_chars, std::vector<std::string>& result_list);

   void getChar(const std::string& pinyin, std::vector<std::string>& result_list);

   void getRelativeList(const izenelib::util::UString& hanzi, 
            std::vector<std::pair<izenelib::util::UString,uint32_t> >& ResultList);

   static std::string res_dir_;

private:
   void loadRawTextTransProb_(TransProbType& trans_prob, const std::string file);

   void flushRawTextTransProb_(const std::string& file, const TransProbType& trans_prob);

   void transProb_(const izenelib::util::UCS2Char& from, const izenelib::util::UCS2Char& to);

   void updateItem_(TransProbType& trans_prob, const uint32_t df, const izenelib::util::UString& text);

   int getInputType_(const izenelib::util::UString& input);

   bool getResultWithScore_(const izenelib::util::UString& input
        int type,
        std::vector<std::pair<double, std::string> >& pinyin_list,
        std::vector<CandidateResult>& output);

   void getResultByPinyin_(const std::string& pinyin, double pinyin_score,
        std::vector<CandidateResult>& output);

   // trigram 
   void getResultByPinyinT_(const std::string& pinyin, double pinyin_score,
        std::vector<CandidateResult>& output);

   void getResultByPinyinTRecur_(const std::string& pinyin,double base_score,
        std::pair<double, izenelib::UString>& mid_result,
        std::vector<CandidateResult>& output);

   double getScore_(const izenelib::util::UString& text,double ori_score,
            double pinyin_score);

   bool isCandidate_(const izenelib::util::UString& text, double ori_score,
        double pinyin_score, double& score);

   bool isCandidateResult_(const izenelib::util::UString& text, double ori_score,
        double pinyin_score, double& score);


   static TransProbType global_trans_prob_;
   TransProbType collection_trans_prob_;

   std::string collection_dir_;
   double threshold_;
   double mid_threshold_;
   uint16_t max_pinyin_term_;
   
   PinyinDictType pinyin_dict_;
   Pinyin2CnType pinyin2cn_;
   Cn2PinyinType cn2pinyin_;
   boost::unordered_map<std::string, bool> filter_pinyin_;

   boost::mutex mutex_;
}



#endif
