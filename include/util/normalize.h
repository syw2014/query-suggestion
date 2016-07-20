/*************************************************************************
 @ File Name: normalize.h
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Tue 12 Jul 2016 02:02:37 PM CST
 ************************************************************************/
// Normalization module contains several functions to convert one string 
// to a normalized string,like utf8 encoding,lower case,etc. And the encoding
// was based on a open source "utf8-cpp".

#ifndef UTIL_NORMALIZE_H
#define UTIL_NORMALIZE_H

#include <iostream>
#include <string>
#include <stdint.h>
#include <ctype.h>
#include "utf8.h"

class Normalize {
    public:
        Normalize(){
        }
        
        // IsDigital
        static bool IsDigit(char c) {
            if (c >= '0' && c <= '9')
                return true;
            return false;
        }

        // IsAlpha
        static bool IsAlpha(char c) {
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
                return true;
            return false;
        }

        // IsConnector
        static bool IsConnector(char c) {
            if ( c == '.' || c == '-' || c == '+')
                return true;
            return false;
        }

        // IsBreakPunctuation
        static bool IsBreakPunct(char c) {
            if (!IsPunctuation(c))
                return false;
            if (c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}')
                return true;

            return false;
        }

        // IsPunctuation
        static bool IsPunctuation(char c) {
            return ispunct(c);
        }

        // Convert a string to utf8 encoding, replace any invalid codes by unicode
        static bool ToUTF8(std::string& str) {
            if (str.empty()) 
                return false;
            std::string temp;
            utf8::replace_invalid(str.begin(), str.end(), std::back_inserter(temp));
            
            str = temp;
            return true;
        }

        // TODO:
        // remove invalid utf8 encoding if it exists.
        static bool RemoveInvalidUTF8(std::string& str) {
            std::string::iterator iter = utf8::find_invalid(str.begin(), str.end());
            if (iter == str.end())
                return false;
            std::string temp(str.begin(), iter);
            str = temp;
            return true;
        }

        // Convert a string to lower case
        static void ToLower(std::string& str) {
            std::string ustr(str);
            str = "";
            std::string::size_type idx;
            for (idx = 0; idx < ustr.length(); ++idx) {
                str += ToLower_(ustr[idx]);
            }
        }
        
        // Convert a string to upper case
        static void ToUpper(std::string& str) {
            std::string ustr(str);
            str = "";
            std::string::size_type idx;
            for (idx = 0; idx < ustr.length(); ++idx) {
                str += ToUpper_(ustr[idx]);
            }
        }
        
        // Check a string whether it is a valid utf8 encoding string
        static bool IsValidUTF8(const std::string& str) {
            if (str.empty())
                return false;
            std::string::const_iterator iter = utf8::find_invalid(str.begin(), str.end());
            if (iter != str.end()) {
                std::cout << "Invalid code found!" << std::endl;
                std::string temp(str.begin(), iter);
                std::cout << "This part is fine: " << temp << std::endl;
                return false;
            }
            return true;
        }

        // Check string is utf8 encode
        static bool GetUS2Char(const std::string& str, std::vector<uint32_t>& uChars) {
            uChars.clear();
            std::string ustr(str);
            // Avoid throwing exceptions
            RemoveInvalidUTF8(ustr);
            std::string::iterator iter = ustr.begin();
            while (iter != ustr.end()) {
                uint32_t code = utf8::next(iter, ustr.end());
                uChars.push_back(code);
            }
            return true;
        }

        template<typename T>
        static bool RemoveElement(std::vector<T>& lVec, uint32_t pos
                        ,std::vector<T>& rVec) {
            return true;
        }

        // Determine whether a string is a chinese characters
       static bool IsChinese(const std::string& str) {
            if (!IsValidUTF8(str)) {
               // std::cout << "string is a invalid utf8 encoding!\n";
                return false;
            }
            std::vector<uint16_t> unicodes;
            utf8::utf8to16(str.begin(), str.end(), std::back_inserter(unicodes));
            //std::cout << "size: " << unicodes.size() << std::endl;
            for (uint32_t i = 0; i < unicodes.size(); ++i) {
                if (!IsChineseChar_(unicodes[i])) {
                    return false;
                }
            }
            return true;
        }   

       // Reload
       static bool IsChinese(const uint16_t& UCS2Char) {
           return IsChineseChar_(UCS2Char);
       }

       // convert an utf16 encoding(uint16_t) to a utf8 string
       static bool UnicodeToUTF8Str(const std::vector<uint16_t>& unicodes, std::string& utf8str) {
           utf8str = "";
           if (unicodes.empty()) {
               return false;
           }
           
           utf8::utf16to8(unicodes.begin(), unicodes.end(), std::back_inserter(utf8str));
           return true;
       }

       // convert an utf16 encoding(uint16_t) to a utf8 string
       static bool UnicodeToUTF8Str(const uint16_t& unicode, std::string& utf8str) {
           utf8str = "";
           std::vector<uint16_t> unicodes(1, unicode);
           if (unicodes.empty()) {
               return false;
           }
           
           utf8::utf16to8(unicodes.begin(), unicodes.end(), std::back_inserter(utf8str));
           return true;
       }

       // convert a string to unicode encoding, unicode vector
       static bool ToUnicode(const std::string& str, std::vector<uint16_t>& unicodes) {
           unicodes.clear();
           std::string ustr(str);
           // Avoid throwing exceptions
           RemoveInvalidUTF8(ustr);
           utf8::utf8to16(ustr.begin(), ustr.end(), std::back_inserter(unicodes));
           
           return true;
       }

       //
    private:
        // Determine whether a uint16_t char is a chinese character
        static bool IsChineseChar_(uint16_t ucs2char) {
            if(((ucs2char >= 0x2E80 && ucs2char <= 0x2EF3) // CJK Radicals
                  ||(ucs2char >= 0x2F00 && ucs2char <= 0x2FD5) // Kangxi Radicals Range: 0x2F00 - 0X2FDF
                  ||(ucs2char >= 0x3400 && ucs2char <= 0x4DB5) // CJK Unified Ideographs Extension A
                  ||(ucs2char >= 0x4E00 && ucs2char <= 0x9FC3) // CJK Unified Ideographs
                  ||(ucs2char >= 0xF900 && ucs2char <= 0xFAD9))// CJK Compatibility Ideographs  
              && ucs2char != 12289 // Chinese Punctuaion Unicode encoding, 、 
              && ucs2char != 12298 //《
              && ucs2char != 12290 // 。
              && ucs2char != 12299 // 》
              && ucs2char != 65292 // ，
              && ucs2char != 65311 // ？
              && ucs2char != 65281 // ！
              && ucs2char != 65306 // ：
              && ucs2char != 65307 // ；
              && ucs2char != 8220  // “
              && ucs2char != 8221  // ”
              && ucs2char != 12304 // 【
              && ucs2char != 12305 // 】
              && ucs2char != 65509 // ￥
              && ucs2char != 8230  // …
              && ucs2char != 65288 // （
              && ucs2char != 65289 // ）
              && ucs2char != 8212  // —
              && ucs2char != 20022 )// 、
                return true;

            return false;
        }

        // Convert a char to lower case
       static inline char ToUpper_(char chConv) {
            return (chConv >= 'a' && chConv <= 'z') ? (chConv & 0xdf) : chConv;
        }
       static inline wchar_t ToUpper_(wchar_t chConv) {
            return (chConv >= L'a' && chConv <= L'z') ? (chConv & 0x00df) : chConv;
        }

        // Convert a char to upper case
       static inline char ToLower_(char chConv) {
            return (chConv >= 'A' && chConv <= 'Z') ? (chConv | 0x20) : chConv;
        }
       static inline wchar_t ToLower_(wchar_t chConv) {
            return (chConv >= L'A' && chConv <= 'Z') ? (chConv | 0x0020) : chConv;
        }
};


#endif // util/normalize.h
