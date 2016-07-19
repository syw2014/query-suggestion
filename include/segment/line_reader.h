
#ifndef LINE_READER_H_
#define LINE_READER_H_

#include <sys/types.h>

#include <iostream>
#include <vector>
#include <cstdio>
#include <cstring>

namespace util{

class LineReader
{
    FILE* f_;
    char* mem_;
    uint64_t bytes_;

    bool next_block_()
    {
        if (::feof(f_))
          return false;
        memset(mem_, 0, bytes_);
        uint64_t p = ftell(f_);
        if(fread(mem_, bytes_, 1, f_)!=1);
        //std::cout<<mem_<<"PPPP\n";
        char* m = mem_;
        char* la_n = NULL;
        while((uint64_t)(m - mem_) < bytes_ && *m != 0)
        {
            if (*m == '\n')
              *m = 0, la_n = m;
            m++;
        }

        if (m == mem_ && ::feof(f_))
          return false;

        if ((uint64_t)(m-mem_)<bytes_ || ::feof(f_))
          return true;

        assert(la_n != NULL);
        fseek(f_, p + (la_n - mem_+1), SEEK_SET);
        assert((uint64_t)(la_n - mem_+1) <= bytes_);
        memset(la_n+1, 0, bytes_ -(la_n - mem_+1));
        return true;
    }
    public:
        LineReader(const std::string& nm, uint64_t buf_size = 1000000)
        {
            f_ = fopen(nm.c_str(), "r");
            if (!f_)
              throw std::runtime_error("can't open file.");

            bytes_ = buf_size;
            mem_ = new char[buf_size];
        }

        ~LineReader()
        {
            if(f_)fclose(f_);
            if (mem_)delete [] mem_;
        }

        char* line(char* prev_line = NULL)
        {
            if (!prev_line)
            {
                fseek(f_, 0, SEEK_SET);
                if(next_block_())return mem_;
                return NULL;
            }

            assert(prev_line >= mem_);
            assert(prev_line < mem_+bytes_);
            //std::cout<<prev_line<<"LLLL\n";
            //std::cout<<prev_line-mem_<<std::endl;
            while(*prev_line != 0)++prev_line;
            assert(prev_line < mem_+bytes_);

            prev_line++;
            if (prev_line >= mem_+bytes_ || *prev_line  == 0)
            {
                if(next_block_())return mem_;
                return NULL;
            }
            return prev_line;
        }
};

}

#endif
