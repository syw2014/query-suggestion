/*************************************************************************
 @ File Name: t_suggestion_unit.cc
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Tue 26 Jul 2016 05:50:25 PM CST
 ************************************************************************/
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE SuggestionTest

#include "unit_test.h"
#include <iostream>
#include "suggestion.hpp"

boost::shared_ptr<Suggestion> pSuggest(new Suggestion("../resource/"));

BOOST_AUTO_TEST_SUITE (SuggestionTest)

// generate test terms
void gendata() {

    // construct data
    std::ofstream ofs(".terms.txt");
    if (!ofs) {
        std::cout << "Open file .terms.txt file error!\n";
    }
    // write test data
    ofs << "贝贝德皮诺" << "\t" << 300 << "\t" << 123 << std::endl;
    ofs << "bebedepino" << "\t" << 250 << "\t" << 231 << std::endl;
    ofs << "背背佳" << "\t" << 130 << "\t" << 42 <<std::endl;
    ofs << "brand25" << "\t" << 50 << "\t" << 83 << std::endl; 
    ofs.close();
}

// remove space
void print_str(const std::string& str) {
    std::cout << "===========RemoveSpace===========\nInput:" << str << std::endl;
    std::cout << "Result: " << pSuggest->RemoveSpace(str) << std::endl;
}

// Case 1, Suggestion::RemoveSpace()
BOOST_AUTO_TEST_CASE (removeSpace) {
    
   print_str("be bedi    pino"); 
}

BOOST_AUTO_TEST_SUITE_END()
