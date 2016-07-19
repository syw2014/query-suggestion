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
    str = "把";
   // Normalize::ToUTF8(str);
   // std::cout << "T: " << str << "\t T[0]:" << tt << std::endl;
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

// 6 Normalize::ToUtf16()
// Get utf16 encoding arrary of a string
BOOST_AUTO_TEST_CASE (ToUtf16) {
    std::string str("大智若愚");
    std::vector<uint16_t> utf16_chars;
    Normalize::ToUtf16(str, utf16_chars);
    BOOST_CHECK_MESSAGE( utf16_chars.size() == 4, str << "To utf16 size: " << utf16_chars.size());
    
    str = "银行abc";
    Normalize::ToUtf16(str, utf16_chars);
    BOOST_CHECK_MESSAGE( utf16_chars.size() == 5, str << "To utf16 size: " << utf16_chars.size());
}

// 7 Normalize::Utf16ToUTF8Str()
// Convert a chinese character with utf16 encoding(uint16_t) to a utf8 string
BOOST_AUTO_TEST_CASE (Utf16ToUTF8Str) {
    std::string str("中华人民共和国");
    std::vector<uint16_t> utf16_chars;
    Normalize::ToUtf16(str, utf16_chars);
    std::string utf8str;
    Normalize::Utf16ToUTF8Str(utf16_chars, utf8str);
    BOOST_CHECK_MESSAGE( str == utf8str, str << "After utf16 encoding and decoding: " << utf8str);
    
    str = "连衣裙Love";
    Normalize::ToUtf16(str, utf16_chars);
    Normalize::Utf16ToUTF8Str(utf16_chars, utf8str);
    BOOST_CHECK_MESSAGE( str == utf8str, str << "After utf16 encoding and decoding: " << utf8str);
}

// 8 Normalize::Utf16ToUTF8Str()
// Convert a chinese character with utf16 encoding(uint16_t) to a utf8 string
BOOST_AUTO_TEST_CASE (U16ToUTF8Str) {
    std::string str("爱");
    std::vector<uint16_t> utf16_chars;
    Normalize::ToUtf16(str, utf16_chars);
    std::string utf8str;
    Normalize::Utf16ToUTF8Str(utf16_chars[0], utf8str);
    BOOST_CHECK_MESSAGE( str == utf8str, str << "After utf16 encoding and decoding: " << utf8str);
}

BOOST_AUTO_TEST_SUITE_END()


