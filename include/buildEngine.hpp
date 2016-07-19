/*************************************************************************
 @ File Name: buildEngine.h
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Mon 11 Jul 2016 01:42:29 PM CST
 ************************************************************************/
#ifndef MODULE_BUILD_ENGINE_HPP
#define MODULE_BUILD_ENGINE_HPP

#include <set>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/concept_check.hpp>
#include <boost/unordered_map.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

// segment
#include "segmentWrapper.h"

// Build engine for data module build
// Provide a interface for data building
class BuildEngine {
};

#endif // buildEngine.hpp
