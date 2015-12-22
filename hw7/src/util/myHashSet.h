/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
 ****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ()" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
    public:
        HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
        ~HashSet() { reset(); }

        // TODO: implement the HashSet<Data>::iterator Done.
        // o An iterator should be able to go through all the valid Data
        //   in the Hash
        // o Functions to be implemented:
        //   - constructor(s), destructor
        //   - operator '*': return the HashNode
        //   - ++/--iterator, iterator++/--
        //   - operators '=', '==', !="
        //
        class iterator
        {
            friend class HashSet<Data>;

            public:
                iterator(int b=0, int i=0, const HashSet<Data>* p = 0):_bNum(b), _iNum(i), _p(p){}
                iterator(const iterator& it):_bNum(it._bNum), _iNum(it._iNum), _p(it._p) {}
                ~iterator() {}
                Data& operator*() { return _p->_buckets[_bNum][_iNum];  }
                const Data& operator*() const  {return _p->_buckets[_bNum][_iNum];  }
                iterator& operator++() 
                {   if (_iNum == _p->_buckets[_bNum].size()-1)
                    {   do ++_bNum; while (_bNum < _p->_numBuckets && _p->_buckets[_bNum].empty());
                        _iNum = 0;
                    }
                    else _iNum++;
                    return (*this);
                }
                iterator operator++(int)
                {   iterator i = *this;
                    ++(*this);
                    return i;
                }
                iterator operator--()
                {   if (_iNum == 0)
                    {   do --_bNum; while(_bNum >= 0 && _p->_buckets[_bNum].empty());
                        _iNum = _p->_buckets[_bNum].size()-1;
                    }
                    else _iNum--;
                    return (*this);
                }
                iterator operator--(int)
                {   iterator i = *this;
                    --(*this);
                    return i;
                }

                bool operator == (const iterator& it) const { return (_bNum == it._bNum && _iNum == it._iNum && _p == it._p); }
                bool operator != (const iterator& it) const { return (_bNum != it._bNum || _iNum != it._iNum || _p != it._p); }

            private:
                int _bNum, _iNum;
                const HashSet<Data>* _p; // parent
        };

        void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
        void reset() {
            _numBuckets = 0;
            if (_buckets) { delete [] _buckets; _buckets = 0; }
        }
        void clear() {
            for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
        }
        size_t numBuckets() const { return _numBuckets; }

        vector<Data>& operator [] (size_t i) { return _buckets[i]; }
        const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

        // TODO: implement these functions Done.
        //
        // Point to the first valid data
        iterator begin() const 
        {   int idx = 0;
            for (; idx < _numBuckets; idx++)
                if (!_buckets[idx].empty())
                    break;
            
            return iterator(idx, 0, this); 
        }
        // Pass the end
        iterator end() const { return iterator(_numBuckets, 0, this); }
        // return true if no valid data
        bool empty() const 
        {   for (int idx = 0; idx < _numBuckets; ++idx)
                if (!_buckets[idx].empty())
                    return false;
            return true; 
        }
        // number of valid data
        size_t size() const 
        {   size_t s = 0;
            for (int i = 0; i < _numBuckets; ++i) s += _buckets[i].size();
            return s; 
        }

        // check if d is in the hash...
        // if yes, return true;
        // else return false;
        bool check(const Data& d) const 
        {   int b = bucketNum(d);
            return (find(_buckets[b].begin(), _buckets[b].end(), d) != _buckets[b].end()); 
        }

        // query if d is in the hash...
        // if yes, replace d with the data in the hash and return true;
        // else return false;
        bool query(Data& d) const 
        {   int b = bucketNum(d);
            for (int i = 0; i < _buckets[b].size(); ++i)
                if (d == _buckets[b][i])
                {   d = _buckets[b][i];
                    return true;
                }
            return false; 
        }

        // update the entry in hash that is equal to d
        // if found, update that entry with d and return true;
        // else insert d into hash as a new entry and return false;
        bool update(const Data& d) 
        {   int b = bucketNum(d);
            int flag = false;
            for (int i = 0; i < _buckets[b].size(); ++i)
                if (d == _buckets[b][i])
                {    _buckets[b][i] = d;
                    flag = true;
                    break;
                }
            if (flag) _buckets[b].push_back(d);
            return false; 
        }

        // return true if inserted successfully (i.e. d is not in the hash)
        // return false is d is already in the hash ==> will not insert
        bool insert(const Data& d) 
        {   if (check(d)) return false;
            _buckets[bucketNum(d)].push_back(d); 
            return true; 
        }

        // return true if removed successfully (i.e. d is in the hash)
        // return fasle otherwise (i.e. nothing is removed)
        bool remove(const Data& d) 
        {   int b = bucketNum(d);
            for (int i = 0; i < _buckets[b].size(); ++i)
                if (d == _buckets[b][i])
                {   _buckets[b].erase(_buckets[b].begin() + i);
                    return true;
                }
            return false; 
        }

    private:
        // Do not add any extra data member
        size_t            _numBuckets;
        vector<Data>*     _buckets;

        size_t bucketNum(const Data& d) const {
            return (d() % _numBuckets); 
        }
};

#endif // MY_HASH_SET_H
