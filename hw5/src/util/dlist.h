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
            const T& operator * () const { return *(_node); }
            T& operator * () { return *(_node); }
            iterator& operator ++ () { _node = _node->_next; return *(this); }
            iterator operator ++ (int) { iterator tmp(*this); _node = _node->_next; return tmp; }
            iterator& operator -- () { _node = _node->_prev; return *(this); }
            iterator operator -- (int) { iterator tmp(*this); _node = _node->_prev; return tmp; }

            iterator& operator = (const iterator& i) { _node = i._node; return *(this); }

            bool operator != (const iterator& i) const { return (node != i._node); }
            bool operator == (const iterator& i) const { return (node == i._node); }

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
            for (interator i = begin(); i != end(); ++i) ++cnt;
            return cnt;
        }

        void push_back(const T& x) 
        {   
            DListNode *newNode = new DListNode(x, _head->_prev, _head);
            _head->_prev->_next = newNode;
            _head->_prev = newNode;
        }
        void pop_front() 
        {   
            if (empty()) return;
            DListNode *tmp = _head->_next;
            _head->_next = tmp->_next;
            _head->_next->_prev = _head;
            delete tmp;
        }
        void pop_back() 
        { 
            if (empty()) return;
            DListNode *tmp = _head->_prev;
            _head->_prev = tmp->_prev;
            _head->_prev->_next = _head;
            delete tmp;
        }

        // return false if nothing to erase
        bool erase(iterator pos) { return false; }
        bool erase(const T& x) { return false; }

        void clear() { }  // delete all nodes except for the dummy node

        void sort() const { }

    private:
        DListNode<T>*  _head;  // = dummy node if list is empty

        // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
