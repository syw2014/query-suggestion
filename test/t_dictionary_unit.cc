/*************************************************************************
 @ File Name: t_dictionary_unit.cc
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Fri 15 Jul 2016 10:50:56 AM CST
 ************************************************************************/
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE DictionaryTest
#include <iostream>
#include "unit_test.h"
#include "dictionary.h"

// name of the test suite is DictionaryTest
BOOST_AUTO_TEST_SUITE (DictionaryTest)

// Dictionary object
std::string dir("../resource/cn");
Dictionary seg(dir);

// show segmentation results
void display_tokens(const std::string& input) {
    std::vector<std::string> vec;
    seg.Segment(input, vec);
    std::cout << "Input:" << input << std::endl;
    for (uint32_t i = 0; i < vec.size(); ++i) {
        std::cout << "Result: " << vec[i] << ",";
    }
    std::cout << std::endl;
}

// show pinyin conversion to chinese results
void display_convert_cn(const std::string& input) {
    std::vector<std::string> vec;
    if (!seg.GetChar(input, vec))
        return;
    
    std::cout << "Input Cn:" << input << std::endl;
    for (uint32_t i = 0; i < vec.size(); ++i) {
        std::cout << "Result: " << vec[i] << ",";
    }
    std::cout << std::endl;
    
}

// show chinese conversion to pinyin results
void display_convert_py(const std::string& input) {
    std::vector<std::string> vec;
    if (!seg.GetPinYin(input, vec))
        return;
    
    std::cout << "Input Cn:" << input << std::endl;
    for (uint32_t i = 0; i < vec.size(); ++i) {
        std::cout << "Result: " << vec[i] << ",";
    }
    std::cout << std::endl;
}

// show single chinese conversion to pinyin results
void display_cn_convert_py(const std::string& input) {
    std::vector<std::string> vec;
    if (!seg.GetPinYinTerm(input, vec))
        return;
    
    std::cout << "Input Cn:" << input << std::endl;
    for (uint32_t i = 0; i < vec.size(); ++i) {
        std::cout << "Result: " << vec[i] << ",";
    }
    std::cout << std::endl;
}

// Case 1 , pinyin segmentation
BOOST_AUTO_TEST_CASE(PinYinSegment) {
   display_tokens("nanaodeye"); 
   display_tokens("mangzuoni"); 
   display_tokens("woyaochitang"); 
   display_tokens("woyaochitan"); 
   display_tokens("congxin"); 

}

// Case 2, pinyin convert to chinese characters
BOOST_AUTO_TEST_CASE(PinYin2Cn) {
}

// Case 3, single chinese words conver to pinyin
BOOST_AUTO_TEST_CASE(Cn2PinYin) {
}

// Case 4, chinese string convers to pinyin
BOOST_AUTO_TEST_CASE(CnStr2PinYin) {
}

BOOST_AUTO_TEST_SUITE_END()
