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

void genByPrefix(const std::string& str) {
    std::vector<std::string> chars, words, keys;
    pBuild->Parse(str, chars, words);

    pBuild->GenerateByPrefix(chars, keys, 10);
    std::cout << "==========GenerateByPrefix==========\nInput: " << str << std::endl;
    print_vector(keys);
}

void genByWordInfix(const std::string& str) {
    std::vector<std::string> chars, words, keys;
    pBuild->Parse(str, chars, words);

    pBuild->GenerateByWordInfix(words, keys, 10);
    std::cout << "=========GenerateByWordInfix===========\nInput: " << str << std::endl;
    print_vector(keys);
}


void genByWordSuffix(const std::string& str) {
    std::vector<std::string> chars, words, keys;
    pBuild->Parse(str, chars, words);

    pBuild->GenerateByWordSuffix(words, keys, 10);
    std::cout << "=========GenerateByWordSuffix===========\nInput: " << str << std::endl;
    print_vector(keys);
}


void genByPinYinPrefix(const std::string& str) {
    std::vector<std::string>  keys;

    pBuild->GenerateByPinYinPrefix(str, keys, 10);
    std::cout << "=========GenerateByPinYinPrefix===========\nInput: " << str << std::endl;
    print_vector(keys);
}

void genByShengMuPrefix(const std::string& str) {
    std::vector<std::string>  keys;

    pBuild->GenerateByShengMuPrefix(str, keys, 10);
    std::cout << "=========GenerateByPinYinPrefix===========\nInput: " << str << std::endl;
    print_vector(keys);
}


// --------------------------------
// test BuildEngine::parse
void test_parse(const std::string& str) {
    std::vector<std::string> chars, words;

    pBuild->Parse(str, chars, words);
    std::cout << "==========Parse==========\nInput: " << str << std::endl;
    std::cout << "Chars: ";
    print_vector(chars);
    std::cout << "Words: ";
    print_vector(words);
    std::cout << "====================\n";
}


// Case 1, BuildEngine::Parse()
BOOST_AUTO_TEST_CASE (Parse) {

    // 1
    test_parse("贝贝德皮诺");
    test_parse("bebedepino");
    test_parse("贝贝德皮诺bebedepino");
    test_parse("贝贝德皮诺》《bebed*)epino");
}

// Case 2, BuildEngine::GenerateByPrefix()
BOOST_AUTO_TEST_CASE (GenerateByPrefix) {
    genByPrefix("贝贝德=皮诺");
    genByPrefix("bebede)pino");
    genByPrefix("贝贝德皮诺bebedep");
}

// Case 3, BuildEngine::GenerateByWordInfix()
BOOST_AUTO_TEST_CASE (GenerateByWordInfix) {
    
    genByWordInfix("贝贝德皮诺");
}

// Case 4, BuildEnginie::GenerateByWordSuffix()
BOOST_AUTO_TEST_CASE (GenByWordSuffix) {
        
    genByWordSuffix("贝贝德皮诺");
}

// Case 5, BuildEngine::GenerateByPinYinPrefix()
BOOST_AUTO_TEST_CASE (GenByPinYinPrefix) {
    
    genByPinYinPrefix("贝贝德皮诺");
}

// Case 6, BuildEngine::GenerateByShengMuPrefix()
BOOST_AUTO_TEST_CASE (GenByShengMuPrefix) {

    genByShengMuPrefix("银行");
}

BOOST_AUTO_TEST_SUITE_END()

