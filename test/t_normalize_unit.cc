/*************************************************************************
 @ File Name: t_normalize_unit.cc
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Tue 12 Jul 2016 05:30:00 PM CST
 ************************************************************************/
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE normalizeTest
//#include <boost/test/unit_test.hpp>
#include <iostream>
#include "unit_test.h"
#include "util/normalize.h"

// name of the test suite is normalizeTest
BOOST_AUTO_TEST_SUITE (normalizeTest)

// 1 Normalize::ToLower()
// Convert string to lower case
BOOST_AUTO_TEST_CASE (ToLower) {
    std::string str("ABCD-HK.3SG");
    Normalize::ToLower(str);
    BOOST_CHECK_MESSAGE(str == "abcd-hk.3sg", "ToLower result: " << str);
}

// 2 Normalize::ToUpper()
// Convert string to upper case
BOOST_AUTO_TEST_CASE (ToUpper) {
    std::string str("i love you 小红!");
    Normalize::ToUpper(str);
    BOOST_CHECK_MESSAGE(str == "I LOVE YOU 小红!", "ToUpper result: " << str);
}

// 3 Normalize::ToUTF8()
// Convert to utf8 encoding
BOOST_AUTO_TEST_CASE (UTF8Encoding) {
    std::string str("a\x80\xe0\xa0\xc0\xaf\xed\xa0\x80z");
    Normalize::ToUTF8(str);
    BOOST_CHECK_MESSAGE(str == "a����z", "To utf8 result:" << str);
}

// 4 Normalize::IsValidUTF8()
// Check a string is a valid utf8 encoding
BOOST_AUTO_TEST_CASE (IsUTF8Encoding) {
    // unknown encoding 日ш
    std::string str("\xe6\x97\xa5\xd1\x88\xfa");
    bool flag = Normalize::IsValidUTF8(str);
    BOOST_CHECK_MESSAGE(flag == false, "IsUTF8Encoding result: " << flag);
}

// 5 Normalize::IsChinese()
// Determine whether a string is a chinese characters
BOOST_AUTO_TEST_CASE (IsChinese) {
    // not chinese as it contains "》"
    std::string str("我爱中国》");
    bool flag = Normalize::IsChinese(str);
    BOOST_CHECK_MESSAGE( flag == false, str << "IsChinese result: " << flag);
    str = "青青原上草";
    flag = Normalize::IsChinese(str);
    BOOST_CHECK_MESSAGE( flag == true, str << " IsChinese result: " << flag);
}

BOOST_AUTO_TEST_SUITE_END()


