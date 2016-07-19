#ifndef NORMALIZE_H_
#define NORMALIZE_H_

#include "trd2simp.h"
#include "kstring.hpp"

#include <string>


class Normalize
{
    //static Trad2Simp trd2smp_;
public:
     static void normalize(KString& kstr)
    {
		static Trad2Simp trd2smp_;
        try{
        kstr.to_dbc();
        kstr.to_lower_case();
        trd2smp_.transform(kstr);
        kstr.trim_into_1();
        kstr.trim_head_tail();
        }catch(...){}
    }

     static void normalize(std::string& str)
    {
        try{
        KString kstr(str);
        normalize(kstr);
        str = unicode_to_utf8(kstr);
        }
        catch(...){}
    }

     static std::string unicode_to_utf8(const KString& kstr)
    {
        std::string s;
        s.reserve(kstr.length() << 2);
        for(size_t i = 0; i < kstr.length(); ++i)
        {
            uint16_t unic = kstr[i];
            if ( unic <= 0x0000007F )
            {
                // * U-00000000 - U-0000007F:  0xxxxxxx
                s.append(1, unic & 0x7F);
            }
            else if ( unic >= 0x00000080 && unic <= 0x000007FF )
            {
                // * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx
                s.append(1, (((unic >> 6) & 0x1F) | 0xC0));
                s.append(1, ((unic & 0x3F) | 0x80));
            }
            else if ( unic >= 0x00000800 && unic <= 0x0000FFFF )
            {
                // * U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx
                s.append(1, (((unic >> 12) & 0x0F) | 0xE0));
                s.append(1, (((unic >>  6) & 0x3F) | 0x80));
                s.append(1, ((unic & 0x3F) | 0x80));
            }
            else if ( unic >= 0x00010000 && unic <= 0x001FFFFF )
            {
                // * U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                s.append(1, (((unic >> 18) & 0x07) | 0xF0));
                s.append(1, (((unic >> 12) & 0x3F) | 0x80));
                s.append(1, (((unic >>  6) & 0x3F) | 0x80));
                s.append(1, ((unic & 0x3F) | 0x80));
            }
            else if ( unic >= 0x00200000 && unic <= 0x03FFFFFF )
            {
                // * U-00200000 - U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
                s.append(1, (((unic >> 24) & 0x03) | 0xF8));
                s.append(1, (((unic >> 18) & 0x3F) | 0x80));
                s.append(1, (((unic >> 12) & 0x3F) | 0x80));
                s.append(1, (((unic >>  6) & 0x3F) | 0x80));
                s.append(1, ((unic & 0x3F) | 0x80));
            }
            else if ( unic >= 0x04000000 && unic <= 0x7FFFFFFF )
            {
                // * U-04000000 - U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
                s.append(1, (((unic >> 30) & 0x01) | 0xFC));
                s.append(1, (((unic >> 24) & 0x3F) | 0x80));
                s.append(1, (((unic >> 18) & 0x3F) | 0x80));
                s.append(1, (((unic >> 12) & 0x3F) | 0x80));
                s.append(1, (((unic >>  6) & 0x3F) | 0x80));
                s.append(1, ((unic & 0x3F) | 0x80));
            }
        }
        return s;
    }


};

#endif
