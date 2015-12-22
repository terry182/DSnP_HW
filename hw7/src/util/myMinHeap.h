/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
 ****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
    public:
        MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
        ~MinHeap() {}

        void clear() { _data.clear(); }

        // For the following member functions,
        // We don't respond for the case vector "_data" is empty!
        const Data& operator [] (size_t i) const { return _data[i]; }   
        Data& operator [] (size_t i) { return _data[i]; }

        size_t size() const { return _data.size(); }

        // TODO : Done.
        const Data& min() const { return _data[0]; }
        void insert(const Data& d) 
        {   _data.push_back(d);
            int idx = _data.size()-1;
            while (idx)
            {   if (_data[(idx-1)/2] < _data[idx]) break;
                swap(_data[(idx-1)/2], _data[idx]);
                idx = (idx-1)/2;
            }
        }
        void delMin() 
        {   int t = _data.size() - 1, idx = 0, child = 1;
            swap(_data[0], _data[t]);
            _data.resize(t);  // Size--;
            while (child < t)
            {   if (child+1 < t && _data[child+1] < _data[child]) ++child;  // Take the smaller child
                if (_data[idx] < _data[child]) break;
                swap(_data[idx], _data[child]);
                idx = child;
                child = idx*2 + 1;
            }
        }
        void delData(size_t i)
        {   size_t idx = i;
            while (idx)
            {   swap(_data[idx], _data[(idx-1)/2]);
                idx = (idx-1)/2;
            }
            delMin();
        }

    private:
        // DO NOT add or change data members
        vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
