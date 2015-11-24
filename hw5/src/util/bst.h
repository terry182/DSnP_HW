/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
 ****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
    // TODO: design your own class!!
    friend class BSTree<T>;
    friend class BSTree<T>::iterator;

    public:
        BSTreeNode(const T& x, BSTreeNode<T>* p=0, BSTreeNode<T>* l=0, BSTreeNode<T>* r=0): 
        _lchild(l), _rchild(r), _parent(p), _data(x){}
    private: 
        T _data;
        BSTreeNode<T> *_lchild, *_rchild, *_parent;  // Relation Pointers
};


template <class T>
class BSTree
{
    // TODO: design your own class!!
    public:
        BSTree():_size(0) { _root = _tail = new BSTreeNode<T>(T()); }
        ~BSTree() { clear(); delete _root; }

        class iterator 
        {   friend class BSTree;

            public:
                iterator(BSTreeNode<T>* x = 0): _node(x) {}
                iterator(const iterator& it): _node(it._node) {}
                ~iterator() {}

                const T& operator * () const { return _node->_data; }
                T& operator * () { return _node->_data; }

            private:
                BSTreeNode<T>* _node;
        };

        iterator begin() const { return iterator(_root); }
        iterator end() const { return iterator(_tail); }
        size_t size() const { return _size; }
        bool empty() const { return (!size()); }

        /*  This function insert a new element into a BST
         *  and return its iterator
         *  
         *  Local variables:
         *  dir = 0 => Left child
         *  dir = 1 => Right child
         *  p => The parent of the inserted node 
         *  ptr => The ptr of the inserted node.
         */
        iterator insert(const T& x)
        {   BSTreeNode<T>* ptr = _root, p = ptr; // p for parent
            bool dir = 1; // direction, default is right for dummy 
            while (ptr != 0 || ptr != _tail)
            {   p = ptr;
                dir = (ptr->_data < x || ptr->_data == x);
                if (dir) ptr = ptr->_lchild;
                else ptr = ptr->_rchild;
            }
            if (dir){ ptr = new BSTreeNode<T>(x, p); p->_lchild = ptr; }
            else if (ptr == _tail)   // Right child
            {   ptr = new BSTreeNode<T>(x, p, 0, _tail); 
                _tail->_parent = ptr;
                p->_rchild = ptr;
            }
            else{ ptr = new BSTreeNode<T>(x, p); p->_rchild = ptr; }
            ++_size;
            return iterator(ptr);
        }
        
        bool erase(iterator pos)
        {   if (empty() || pos._node == _tail) return false;
            
            if (pos._node != root)   // When it is not root.
            {   bool dir = (pos._node->_parent->_rchild == pos._node);

                if (pos._node->_lchild == 0 && pos._node->_rchild == 0) // leaf node
                {   if (!dir) pos._node->_parent->_lchild = 0;
                    else pos._node->_parent->_rchild = 0;  
                }
                else if ( pos._node->_lchild == 0 && pos._node->_rchild) // have right child only
                   RelinkParent(pos._node->_parent, pos._node->_rchild, dir);
                else if ( pos._node->_lchild && pos._node->_rchild == 0)  // have left child only
                   RelinkParent(pos._node->_parent, pos._node->_lchild, dir);
                else if (pos._node->_rchild == _tail)
                {   RelinkParent(pos._node->_parent, pos._node->_lchild, dir);
                    BSTreeNode<T>* ptr = pos._node->_lchild;
                    while (ptr->_rchild) ptr = ptr->_rchild;  // Append dummy node to the right
                    ptr->_rchild = _tail;
                    _tail->_parent = ptr;
                }
                else 
                {   BSTreeNode<T>* ptr = findNext(pos._node);
                    swap(ptr->_data, pos._node->_data);
                    return erase(iterator(ptr));   // This will not delete pos._node
                }
            }   
            else  // root
            {   if ( pos._node->_lchild == 0 && pos._node->_rchild == _tail) { _root = _tail; _tail->_parent = 0; }
                else if (pos._node->_rchild == _tail)
                {   _root = pos._node.->_lchild;
                    BSTreeNode<T>* ptr = pos._node->_lchild;
                    while (ptr->_rchild) ptr = ptr->_rchild;  // Append dummy node to the right
                    ptr->_rchild = _tail;
                    _tail->_parent = ptr;
                }
                else 
                {   BSTreeNode<T>* ptr = findNext(pos._node);
                    swap(ptr->_data, pos._node->_data);
                    return erase(iterator(ptr));   // This will not delete pos._node
                }
            }
            --_size;
            delete pos._node;
            return true;
        }

        void sort() const {} 
        

    private:
        BSTreeNode<T>* _root;
        BSTreeNode<T>* _tail;
        size_t _size;
        
        // This function is used to correctly link the relationship between parent and child
        // dir = 0   =>  parent->_lchild = child
        // dir = 1   =>  ........_rchild........
        // Used in function erase(iterator pos)
        inline void RelinkParent(const BSTreeNode<T>* parent, const BSTreeNode<T>* child, const bool &dir)
        {   if (!dir)
            {   parent->_lchild = child;
                child->_parent = parent;
            }
            else
            {   parent->_rchild = child;
                child->_parent = parent;
            }
        }

        // The function return the pointer the points to the
        // node that is successor of the give node. 
        // Return _tail if not found.
        BSTreeNode<T>* findNext(BSTreeNode<T>* n) const
        {   assert(n != 0); // Cannot be null.
            BSTreeNode<T>* ptr = _tail;
            if (n->_rchild)
            {   ptr = n->_rchild;
                while (ptr->_lchild) ptr = ptr->_lchild;
            }
            else 
            {   ptr = n->_parent;
                bool dir = (ptr->_rchild == n), found = 0;
                while (ptr->_parent)
                {   dir = (ptr->_parent->_rchild == ptr);
                    ptr = ptr->_parent
                    if (!dir) { found = 1; break; } // found when parent's left node.
                }
                if (!found) ptr = _tail;
            }
            return ptr;
        }
};

#endif // BST_H
