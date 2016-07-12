/*************************************************************************
 @ File Name: py_tyes.h
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Wed 16 Mar 2016 10:39:19 AM CST
 ************************************************************************/
#ifdef PY_TYPES_H
#define PY_TYPES_H

#include <iostream>
#include <string>
#include <vector>

typedef uint16_t UCS2Char;
typedef UCS2Char Unigram;
typedef std::pair<Unigram, Unigram> Bigram;
typedef std::pair<Bigram, Unigram> Trigram;
typedef std::vector<Uingram> Ngram;


template <class T>
struct ScoreItem
{
    T value;
    double score;

    bool operator<(const ScoreItem<T>& other) cosnt
    {
        return score > other.score;
    }
};

typedef ScoreItem<std::string> CandidateResult;

struct ViterbiItemT
{
    std::string text;
    double score;

    const UCS2Char& GetLastChar() const
    {
        return text[text.length() -1];
    }
};

#endif // py_types.h
