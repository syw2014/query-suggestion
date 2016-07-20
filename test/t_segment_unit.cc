/*************************************************************************
 @ File Name: t_segment_unit.cc
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Wed 20 Jul 2016 02:54:25 PM CST
 ************************************************************************/
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE SegmentWrapper

#include <iostream>
#include "unit_test.h"
#include "segmentWrapper.h"

BOOST_AUTO_TEST_SUITE (segment)

std::string resDir("../resource/dict");
std::auto_ptr<SegmentWrapper> segWrapper_(new SegmentWrapper(resDir));


// show results
void show_tokens(const std::string& str) {
    std::vector<std::string> tokens;
    segWrapper_->segment(str, tokens, false);

    std::cout <<"Input:" <<str << "\ntoken results:\n";
    for (uint32_t i = 0; i < tokens.size(); ++i) {
        std::cout << tokens[i] << ",";
    }
    std::cout << std::endl;
}

// case 1, segment unit test
BOOST_AUTO_TEST_CASE (segment) {
    show_tokens("中兴WP826A电信CDMA天翼4G家用办公无线座机固话插卡电话老人机");
    show_tokens("美迪惠尔可莱丝NMF针剂水库面膜 官方正品");
    show_tokens("韩国正品九朵云奇迹马油膏淡化痘印补水保湿面霜70g");
    show_tokens("保宁 婴幼儿衣物强力杀菌斑点喷雾祛除剂 500ml 母婴用品");
}

BOOST_AUTO_TEST_SUITE_END()
