/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
 ****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
    friend class DList<T>;
    friend class DList<T>::iterator;

    DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
        _data(d), _prev(p), _next(n) {}

    T              _data;
    DListNode<T>*  _prev;
    DListNode<T>*  _next;
};


template <class T>
class DList
{
    public:
        DList() {
            _head = new DListNode<T>(T());
            _head->_prev = _head->_next = _head; // _head is a dummy node
        }
        ~DList() { clear(); delete _head; }

        // DO NOT add any more data member or function for class iterator
        class iterator
        {
            friend class DList;

            public:
                iterator(DListNode<T>* n= 0): _node(n) {}
                iterator(const iterator& i) : _node(i._node) {}
                ~iterator() {} // Should NOT delete _node

                // TODO: implement these overloaded operators
                const T& operator * () const { return _node->_data; }
                T& operator * () { return _node->_data; }
                iterator& operator ++ () { _node = _node->_next; return *(this); }
                iterator operator ++ (int) { iterator tmp(*this); _node = _node->_next; return tmp; }
                iterator& operator -- () { _node = _node->_prev; return *(this); }
                iterator operator -- (int) { iterator tmp(*this); _node = _node->_prev; return tmp; }

                iterator& operator = (const iterator& i) { _node = i._node; return *(this); }

                bool operator != (const iterator& i) const { return (_node != i._node); }
                bool operator == (const iterator& i) const { return (_node == i._node); }

            private:
                DListNode<T>* _node;
        };

        // TODO: implement these functions
        iterator begin() const { return iterator(_head->_next); } // This is the first element
        iterator end() const { return iterator(_head); }
        bool empty() const { return (_head == _head->_next); }
        size_t size() const
        {   
            int cnt = 0;
            for (iterator i = begin(); i != end(); ++i) ++cnt;
            return cnt;
        }

        void push_back(const T& x) 
        {   
            DListNode<T> *newNode = new DListNode<T>(x, _head->_prev, _head);
            _head->_prev->_next = newNode;
            _head->_prev = newNode;
        }
        void pop_front() 
        {   
            if (empty()) return;
            DListNode<T> *tmp = _head->_next;
            _head->_next = tmp->_next;
            _head->_next->_prev = _head;
            delete tmp;
        }
        void pop_back() 
        { 
            if (empty()) return;
            DListNode<T> *tmp = _head->_prev;
            _head->_prev = tmp->_prev;
            _head->_prev->_next = _head;
            delete tmp;
        }

        // return false if nothing to erase
        bool erase(iterator pos) 
        {   if (empty()) return false;
            pos._node->_prev->_next = pos._node->_next;
            pos._node->_next->_prev = pos._node->_prev;
            delete pos._node;
            return true; 
        }
        bool erase(const T& x) 
        { 
            for (iterator i = begin(); i != end(); ++i)
                if (*i == x)
                   return erase(i);
            return false; 
        }

        void clear() 
        {   
            if (empty()) return;
            iterator i = begin(); ++i;
            for (; i != end(); ++i) delete i._node->_prev;
            delete _head->_prev;
            _head->_prev = _head->_next = _head;
        }  // delete all nodes except for the dummy node

        void sort() const 
        {    
            iterator l = begin(), r(_head->_prev); 
            mergeSort(l, r);
        }

    private:
        DListNode<T>*  _head;  // = dummy node if list is empty
        // [OPTIONAL TODO] helper functions; called by public member functions
        iterator findMid(iterator& l, iterator& r) const
        {
            iterator slow = l, fast = l;   // slow run 1 step , fast run 2 step per loop.
            while (fast != r)
            {
                if (++fast == r) break;
                ++fast;
                ++slow;
            }
            return slow;
        }
        void mergeSort(iterator& l, iterator& r) const
        {
            if (l == r) return;
            if (l._node == r._node->_prev)  // 2 elements
            {   
                if (*r < *l) swap(*r, *l);
                return;
            }
            iterator m = findMid(l, r); 
            iterator n = m; ++m;
            DListNode<T> *j = n._node->_next, *tail = l._node->_prev, *end = r._node->_next;  // j : the start of second linked list
            mergeSort(l, n);
            j = j->_prev; // Get back to the first array first avoid second array moving
            mergeSort(m, r);
            // do the merge
            j = j->_next; // Go back to the start of the second array
            DListNode<T>* i = tail->_next, *lend = j; // i: the start of the first array
            while (i != lend && j != end)
            {   
                if (i->_data < j->_data) 
                {   
                    tail->_next = i;
                    i->_prev = tail;
                    i = i->_next;
                }
                else
                {   
                    tail->_next = j;
                    j->_prev = tail;
                    j = j->_next;
                }
                tail = tail->_next;
            }
            if (i != lend)
             {  
                tail->_next = i;
                i->_prev = tail;
                while (i->_next != lend) i = i->_next;
                end->_prev = i;
                i->_next = end;
             }
            else
            { 
                tail->_next = j; 
                j->_prev = tail; 
            }
        }
};

#endif // DLIST_H
