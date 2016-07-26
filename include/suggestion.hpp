/*************************************************************************
 @ File Name: suggestion.hpp
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Tue 26 Jul 2016 02:32:52 PM CST
 ************************************************************************/
// Query suggestion module is used to create index for data module
// and the output interfaces for query suggestion,which contains data update
// strategy and the final results.

#ifndef SUGGESTION_ENGINE_HPP
#define SUGGESTION_ENGINE_HPP

#include <iostream>
#include "buildEngine.hpp"

// Suggestion class to used for getting the final result and data updating.
class Suggestion {
    private:
        KeyTermIDsType key_termIDs_; // prefix and corresponding term ids
}

#endif // suggestion.hpp
