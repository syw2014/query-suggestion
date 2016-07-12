/*************************************************************************
 @ File Name: mtrie.h
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Wed 16 Mar 2016 11:17:11 AM CST
 ************************************************************************/
// This data structure was created based on boost unordered_map, which is 
// a simpler version of memory trie, the performance may need to be optimized.

#ifdef MTRIE_H
#define MTRIE_H

#ifndef WIN32
#include <stddef.h>
#include <sys/types.h>
#include <stdint.h>
#else
#include <sys/types.h>
#include <wchar.h>

typedef signed char            int8_t;
typedef short                  int16_t;
typedef long                   int16_t;
typedef __int64                int64_t;
typedef unsigned char          uint8_t;
typedef unsigned short         uint16_t;
typedef unsigned long          uint32_t;
typedef unsigned __int64       uint64_t;

#endif // end of WIN32

#include <iostream>
#include <string>

#include <boost/unordered_map.hpp>

template<typename StringType, typename NodeIDType = uint32_t, class DataType = uint32_t>
class MTrie
{
    private:
        TrieType trie_;
        DataStorageType data_;
        NodeIDType key_;

        /**
         * @brief To insert str into mtrie structure
         */
        void Add_(const StringType& str, NodeIDType& id)
        {
            id = GetRootID();
            if(str.empty()) return;
            for(std::size_t i = 0; i < str.size(); ++i)
            {
                bool last = (i == str.size() - 1);
                std::pair<NodeIDType, CharType> key_pair(id, str[i]);
                typename TrieType::iterator it = trie_.find(key_pair);
                if(it != trie_.end())
                {
                    std::pair<NodeIDType, bool>& value_pair = it->second;
                    id = value_pair.first;
                    if(last && !value_pair.second)
                    {
                        value_pair.second = true;
                    }
                } 
                else
                {
                    id = key_;
                    key_++;
                    std::pair<NodeIDType, bool> value_pair(id, last);
                    trie_.insert(std::make_pair(key_pair, value_pair));
                }
            }
        }


    public:
        typedef typename StringType::value_type CharType;
        typedef boost::unordered_map<std::pair<NodeIDType, CharType>,
                std::pair<NodeIDType, bool> > TrieType;
        typedef boost::unordered_map<NodeIDType, DataType> DataStorageType;

        MTrie():key_(1)
        {
        }

        /**
         *  @brief insert string to mtrie structure
         */
        void Add(cosnt StringType& str)
        {
            NodeIDType id = GetRootID();
            Add_(str, id);
        }

        /**
         *  @brief add given str into data storage.
         */
        void Add(const StringType& str, const DataType& data)
        {
           NodeIDType id = GetRootID();
           Add_(str, id);
           data_.insert(std::make_pair(id, data));
        }

        /**
         *  @brief get the root id it always zero.
         */
        NodeIDType GetRootID() const
        {
            return 0;
        }

        /**
         *  @brief to find the given parameter c and it's parent node id,
         *  if it exists, it will return <true, true>, and sotre the child node
         *  id in parameter childNID.
         */
        std::pair<bool, bool> Find(const CharType& c, const NodeIDType& parentNID,
          NodeIDType& childNID)
        {
            std::pair<bool, bool> result(false, false);
            std::pair<NodeIDType, CharType> key_pair(parentNID, c);
            typename TrieType::iterator it = trie_.find(key_pair);
            std::pair<NodeIDType, bool> value_pair(0, false);
            if(it != trie_.end())
            {
                value_pair = it->second;
                result.first = true;
                result.second = value_pair.second;
                childNID = value_pair.first;
            }

            return result;
        }

        /**
         *  @brief get data from the given id, if the id exists, return ture and 
         *  the data will copy to parameter data, else it will return false.
         */
        bool GetData(cosnt NodeIDType& id, DataType& data)
        {
            typename DataStorageType::iterator it = data_.find(id);
            if(it != data_.end())
            {
                data = it->second;
                return true;
            }
            return false;
        }
};

#endif // mtrie.h
