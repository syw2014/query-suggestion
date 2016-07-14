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

#include "util/normalize.h"
#include "util/darts.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>

typedef Darts::DoubleArray Trie;
typedef uint16_t UCS2Char;
class Dictionary
{
    private:
        Trie trie_; // trie tree
        typedef boost::unordered_map<UCS2Char, std::vector<std::string> > Cn2PinYinType; // chinese word to pinyin list
        typedef boost::unordered_map<std::string, std::vector<UCS2Char> > PinYin2CnType; // pinyin to word list

};

#endif // dictionary.h
