/*************************************************************************
 @ File Name: pinyinEngine.h
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Mon 11 Jul 2016 01:50:06 PM CST
 ************************************************************************/
#ifndef PINYIN_ENGINE_H
#define PINYIN_ENGINE_H


#include <iostream>
#include "util/mtrie.h"
#include "util/py_types.h"


class PinYinEngine {
    private:
        typedef MTrie<Ngram, uint32_t, double> TrieType;
        
        struct TransProbType {
            // unigram transition probability
            boost::unordered_map<Unigram, double> u_trans_prob_; 
            boost::unordered_map<Bigram, double> b_trans_prob_;  // bigram
            boost::unordered_map<Trigram, double> t_trans_prob_; // trigram

            // clear all data
            void clear() {
                u_trans_prob_.clear();
                b_trans_prob_.clear();
                t_trans_prob_.clear();
            }

            bool empty() {
                return u_trans_prob_.empty() && b_trans_prob_.empty() && t_trans_prob_.empty();
            }
        };

    public:
        // load resource
        bool Load() {
        }

        // get chinese character
        void GetCnChar() {
        }

        // get pinyin from chinese character
        void GetPinYin() {
        }

        void GetPinYinWithScore() {
        }

        // pinyin tokenizer
        void PySegment() {
        }

        // fuzzy segmentation
        void FuzzyPySegment() {
        }

    private:




};

#endif // pinyinEngine.h
