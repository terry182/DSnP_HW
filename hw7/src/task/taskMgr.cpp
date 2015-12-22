/****************************************************************************
  FileName     [ taskMgr.cpp ]
  PackageName  [ task ]
  Synopsis     [ Define member functions for task Manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
 ****************************************************************************/

#include <iostream>
#include <string>
#include <cassert>
#include "taskMgr.h"
#include "rnGen.h"
#include "util.h"

using namespace std;

TaskMgr *taskMgr = 0;

// BEGIN: DO NOT CHANGE THIS PART
TaskNode::TaskNode() 
{
    _name.resize(NAME_LEN);
    for (int i = 0; i < NAME_LEN; ++i)
        _name[i] = 'a' + rnGen(26);
    _load = rnGen(10000);
}

size_t
TaskNode::operator () () const 
{
    size_t k = 0, n = (_name.length() <= 5)? _name.length(): 5;
    for (size_t i = 0; i < n; ++i)
        k ^= (_name[i] << (i*6));
    return k;
}

ostream& operator << (ostream& os, const TaskNode& n)
{
    return os << "(" << n._name << ", " << n._load << ")";
}

TaskMgr::TaskMgr(size_t nMachines)
    : _taskHeap(nMachines), _taskHash(getHashSize(nMachines)) { }

    void
TaskMgr::remove(size_t nMachines)
{        
    for (size_t i = 0, n = nMachines; i < n; ++i) {
        size_t j = rnGen(size());
        assert(_taskHash.remove(_taskHeap[j]));
        _taskHeap.delData(j);
    }
}

// return true if TaskNode is successfully removed
// return false if no such node exists
    bool
TaskMgr::remove(const string& s)
{
    TaskNode n(s, 0);
    for (size_t i = 0, m = size(); i < m; ++i)
        if (_taskHeap[i] == n) { _taskHeap.delData(i); break; }
    return _taskHash.remove(n);
}
// END: DO NOT CHANGE THIS PART

// Exactly nMachines (nodes) will be added to Hash and Heap
// Note: a new task node can be created by the default constructor
//       i.e. TaskNode newNode;
void TaskMgr::add(size_t nMachines)
{
    // TODO...
    while(nMachines--)
    {   TaskNode newNode;
        if(_taskHash.insert(newNode))
        {   _taskHeap.insert(newNode);
            cout << "Task node inserted: " << newNode << endl;
        }
        else nMachines++;
    }
}

// return true if TaskNode is successfully inserted
// return false if equivalent node has already existed
bool TaskMgr::add(const string& s, size_t l)
{
    // TODO...
    TaskNode newNode(s, l);
    if (_taskHash.insert(newNode))
    {   _taskHeap.insert(newNode);
        cout << "Task node inserted: " << newNode << endl;
        return true;
    }
    return false;
}

// Assign the min task node with 'l' extra load.
// That is, the load of the min node will be increased by 'l'.
// The min node in the heap should be updated accordingly.
// The corresponding node in the hash should be updated, too.
// return false if taskMgr is empty
// otherwise, return true.
bool TaskMgr::assign(size_t l)
{
    // TODO...
    if (_taskHeap.size() == 0)
        return false;
    TaskNode d = _taskHeap[0];
    d += l;
    _taskHash.update(d);
    _taskHeap.delMin();
    _taskHeap.insert(d);
    // Heapify
    /*
    int idx = 0, child = 1, t = _taskHeap.size();
    while (child < t)
    {   if (child+1 < t && _taskHeap[child+1] < _taskHeap[child]) ++child;
        if (_taskHeap[idx] < _taskHeap[child]) break;
        swap(_taskHeap[idx], _taskHeap[child]);
        idx = child;
        child = idx*2+1;
    }
    */
    
    return true;
}

// WARNING: DO NOT CHANGE THESE TWO FUNCTIONS!!
void TaskMgr::printAllHash() const 
{
    HashSet<TaskNode>::iterator hi = _taskHash.begin();
    for (; hi != _taskHash.end(); ++hi)
        cout << *hi << endl;
}

void
TaskMgr::printAllHeap() const
{
    for (size_t i = 0, n = size(); i < n; ++i)
        cout << _taskHeap[i] << endl;
}
