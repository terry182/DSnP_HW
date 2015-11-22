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
        BSTreeNode(const T& x, BSTreeNode<T>* p=0, BSTreeNode<T>* l=0, BSTreeNode<T>* r=0, size_t c=0): 
        _lchild(l), _rchild(r), _parent(p), _data(x), _child(c) {}
    private: 
        T _data;
        BSTreeNode<T> *_lchild, *_rchild, *_parent;  // Relation Pointers
        size_t _child;
};


template <class T>
class BSTree
{
    // TODO: design your own class!!
    public:
        BSTree(){
            _root = new BSTreeNode<T>(T());
        }
        ~BSTree(){
            clear(); delete _root;
        }
        class iterator { };

        size_t size() const
        {   
            return _root->_child;
        }
    
    private:
        BSTreeNode<T>* _root;
};

#endif // BST_H
