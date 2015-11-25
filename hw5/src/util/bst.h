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
        _data(x), _parent(p), _lchild(l), _rchild(r){}
    private: 
        T _data;
        BSTreeNode<T> *_parent, *_lchild, *_rchild;  // Relation Pointers
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
                iterator& operator ++ () { _node = findNext(_node); return (*this); }
                iterator operator ++ (int) { iterator tmp(*this); _node = findNext(_node); return tmp; }
                iterator& operator -- () { _node = findPrev(_node); return (*this); }
                iterator operator -- (int) { iterator tmp(*this); _node = findPrev(_node); return tmp;}

                iterator& operator = (const iterator& i) { _node = i._node; return (*this);}
                bool operator != (const iterator& i) const { return (_node != i._node); }
                bool operator == (const iterator& i) const { return (_node == i._node); }

            private:
                BSTreeNode<T>* _node;

                BSTreeNode<T>* findNext(BSTreeNode<T>* n) const
                {   assert(n != 0); // Cannot be null.
                    BSTreeNode<T>* ptr = n;
                    if (n->_rchild)
                    {   ptr = n->_rchild;
                        while (ptr->_lchild) ptr = ptr->_lchild;
                        return ptr;
                    }
                    else 
                    {   bool dir = 0, found = 0;
                        while (ptr->_parent)
                        {   
                            dir = (ptr->_parent->_rchild == ptr);
                            ptr = ptr->_parent;
                            if (!dir) { found = 1; break; } // found when parent's left node.
                        }
                        if (!found) ptr = n;
                    }
                    return ptr;
                }

                BSTreeNode<T>* findPrev(BSTreeNode<T>* n) const
                {   assert(n != 0);
                    BSTreeNode<T>* ptr = n;
                    if (n->_lchild)
                    {   ptr = n->_lchild;
                        while (ptr->_rchild) ptr = ptr->_rchild;
                    }
                    else 
                    {   bool dir = 0, found = 0;
                        while (ptr->_parent)
                        {   dir = (ptr->_parent->_lchild == ptr);
                            ptr = ptr->_parent;
                            if (!dir) { found = 1; break; } // found when parent's right node.
                        }
                        if (!found) ptr = n;
                    }
                    return ptr;
                }
        };

        iterator begin() const 
        {   BSTreeNode<T>* ptr = _root;
            while (ptr->_lchild) ptr = ptr->_lchild;
            return iterator(ptr); 
        }
        iterator end() const { return iterator(_tail); }
        size_t size() const { return _size; }
        bool empty() const { return (_size == 0); }

        /*  This function insert a new element into a BST
         *  and return its iterator
         *  
         *  Local variables:
         *  dir = 1 => Left child
         *  dir = 0 => Right child
         *  p => The parent of the inserted node 
         *  ptr => The ptr of the inserted node.
         */
        iterator insert(const T& x)
        {   if (empty())
            {   BSTreeNode<T>* ptr = new BSTreeNode<T>(x, 0, 0, _tail);
                _root = ptr;
                ++_size;
                return iterator(ptr);
            }
            
            BSTreeNode<T>* ptr = _root, *p = ptr; // p for parent
            bool dir = 0; // direction, default is right for dummy 
            while (ptr != 0 && ptr != _tail)
            {   p = ptr;
                dir = (x < ptr->_data || ptr->_data == x);
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
            
            if (pos._node != _root)   // When it is not root.
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
            else  // root, must have right child 
            {   if ( pos._node->_lchild == 0 && pos._node->_rchild == _tail) { _root = _tail; _tail->_parent = 0; }
                else if (pos._node->_rchild == _tail)
                {   _root = pos._node->_lchild;
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

        // This function return false if x is not found or not deleted.
        bool erase(const T& x)  
        {   return erase(find(x));
        }

        bool pop_front()
        {   return erase(begin());
        }
        
        bool pop_back()
        {   return erase(iterator(_tail->_parent)); // the function will avoid the _tail->_parent = 0 situation
        }
        
        // This fucntion find the node in the BST that have value x
        // return end() if not found
        iterator find(const T& x)
        {   if (empty()) return end();

            BSTreeNode<T>* ptr = _root;
            while (ptr && ptr != _tail)
                if (x == ptr->_data) return iterator(ptr);
                else if (x < ptr->_data) ptr = ptr->_lchild;
                else ptr = ptr->_rchild;

            return end();
        }

        void sort() const {} 

        void print() const 
        {
            cout << _root->_data << endl;
            if (_root->_rchild != _tail) cout << _root->_rchild->_data << endl;
        } 

        void clear() 
        {   cleanup(_root);
            _root = _tail;
            _tail->_parent = 0;
            _size = 0;
        }

    private:
        BSTreeNode<T>* _root;
        BSTreeNode<T>* _tail;
        size_t _size;
        
        // This function is used to correctly link the relationship between parent and child
        // dir = 0   =>  parent->_lchild = child
        // dir = 1   =>  ........_rchild........
        // Used in function erase(iterator pos)
        inline void RelinkParent(BSTreeNode<T>* parent, BSTreeNode<T>* child, const bool &dir)
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
            {   bool dir = 0, found = 0;
                while (ptr->_parent)
                {   dir = (ptr->_parent->_rchild == ptr);
                    ptr = ptr->_parent;
                    if (!dir) { found = 1; break; } // found when parent's left node.
                }
                if (!found) ptr = _tail;
            }
            return ptr;
        }

        BSTreeNode<T>* findPrev(BSTreeNode<T>* n) const
        {   assert(n != 0);
            BSTreeNode<T>* ptr = _tail;
            if (n->_lchild)
            {   ptr = n->_lchild;
                while (ptr->_rchild) ptr = ptr->_rchild;
            }
            else 
            {   bool dir = 0, found = 0;
                while (ptr->_parent)
                {   dir = (ptr->_parent->_lchild == ptr);
                    ptr = ptr->_parent;
                    if (!dir) { found = 1; break; } // found when parent's right node.
                }
                if (!found) ptr = _tail;
            }
            return ptr;
        }

        void cleanup(BSTreeNode<T>* n)
        {   if (n == _tail) return;
            if (n->_lchild) cleanup(n->_lchild);
            if (n->_rchild) cleanup(n->_rchild);
            delete n;
        }
};

#endif // BST_H
