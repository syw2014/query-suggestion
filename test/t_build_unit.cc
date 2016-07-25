/*************************************************************************
 @ File Name: t_build_unit.cc
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Thu 21 Jul 2016 04:21:35 PM CST
 ************************************************************************/
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE DataBuildEngineTest

#include <iostream>
#include <string>
#include "unit_test.h"
#include "buildEngine.hpp"



std::string BuildEngine::res_dir_("../resource/"); // resource dir
BOOST_AUTO_TEST_SUITE (BuildEngineTest)

//std::string BuildEngine::res_dir_("../resource/"); // resource dir
boost::shared_ptr<BuildEngine> pBuild(new BuildEngine());


// construct
//pBuild.reset(new BuildEngine());


// helper
void print_vector(const std::vector<std::string>& vec) {
    for (uint32_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i] << ",";
    }
    std::cout << "\n";
}

// test BuildEngine::parse
void test_parse(const std::string& str) {
    std::vector<std::string> chars, words;

    pBuild->Parse(str, chars, words);
    std::cout << "Input: " << str << std::endl;
    std::cout << "Chars: ";
    print_vector(chars);
    std::cout << "Words: ";
    print_vector(words);
}

// case 1, BuildEngine::Parse()
BOOST_AUTO_TEST_CASE (parse) {

    // 1
    test_parse("贝贝德皮诺");
    test_parse("bebedepino");
    test_parse("贝贝德皮诺bebedepino");

}

BOOST_AUTO_TEST_SUITE_END()

