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
#include "dictionary.hpp"

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
    
    std::cout << "Input PinYin:" << input << std::endl;
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
   
   display_tokens("yinhangjiaapple"); 
   display_tokens("gongsicompany"); 
   display_tokens("shangshi123"); 
   
   display_tokens("qingguangxu15nian"); 
   display_tokens("sanxingSUMSONG"); 

}

// Case 2, pinyin convert to chinese characters
BOOST_AUTO_TEST_CASE(PinYin2Cn) {
    display_convert_cn("zhen");
    display_convert_cn("zi");
}

// Case 3, single chinese words conver to pinyin
BOOST_AUTO_TEST_CASE(Cn2PinYin) {
    display_cn_convert_py("白");
    display_cn_convert_py("爱");
    display_cn_convert_py("鬼");
}

// Case 4, chinese string convers to pinyin
BOOST_AUTO_TEST_CASE(CnStr2PinYin) {
    display_convert_py("我们的爱");
    display_convert_py("你在哪儿");
    display_convert_py("中过");
    display_convert_py("哈哈");
    
    display_convert_py("银行apple");
    display_convert_py("女王大人1234");
    display_convert_py("女王520么么哒");
    display_convert_py("尹汝杰541帮五买");
}

BOOST_AUTO_TEST_SUITE_END()
