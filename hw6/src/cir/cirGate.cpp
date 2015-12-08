/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
 ****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <set>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/

void CirGate::netflow(int &cnt, bool flag[]) const
{   assert(getTypeStr() != "UNDEF");
    if (flag[_id]) return;
    for (int i = 0; i < _fanin.size(); ++i)
    {   CirGate* ptr = (CirGate*)(_fanin[i] & ~(size_t)(0x1));
        if (ptr->getTypeStr() != "UNDEF")
            ptr->netflow(cnt, flag);
    }

    flag[_id] = true; // Visited
    cout << "["<< cnt++ << "] ";
    if (getTypeStr() == "CONST")
        cout << "CONST0";
    else if (getTypeStr() == "AIG")
        cout << "AIG";
    else if (getTypeStr() == "PI")
        cout << "PI ";
    else if (getTypeStr() == "PO")
        cout << "PO ";
    if (_id) cout << " " << _id;
    for (int i = 0; i < _fanin.size(); ++i)
    {   cout << " ";
        CirGate* ptr = (CirGate*)(_fanin[i] & ~(size_t)0x1);
        if (ptr->getTypeStr() == "UNDEF") cout << "*";
        if (_fanin[i] & 1) cout << "!";
        cout << ptr->_id;
    }
    if (getTypeStr() == "PI" && ((CirPiGate*)this)->_name != "") cout << " (" << ((CirPiGate*)this)->_name << ")";
    if (getTypeStr() == "PO" && ((CirPoGate*)this)->_name != "") cout << " (" << ((CirPoGate*)this)->_name << ")";
    cout << endl;
}

void CirGate::faninFlow(int depth, int &level, bool neg, std::set<int> &s) const
{   // indent
    for (int i = 0; i < depth; ++i) cout << "  ";
    
    if (neg) cout << "!";

    if (getTypeStr() != "CONST") cout << getTypeStr() << " " << _id;
    else cout << "CONST 0";

    if (depth < level)
    {   if (s.find(_id) != s.end()) cout << " (*)" << endl;
        else 
        {   cout << endl;
            if (_fanin.size()) s.insert(_id);
            for (int i = 0; i < _fanin.size(); ++i)
            {   CirGate* ptr = (CirGate*)(_fanin[i] & ~(size_t)(0x1));
                ptr->faninFlow(depth+1, level, _fanin[i]&1, s);
            }
        }
    }
    else cout << endl;
}

void CirGate::fanoutFlow(int depth, int &level, bool neg, std::set<int> &s) const
{   // indent
    for (int i = 0; i < depth; ++i) cout << "  ";
    
    if (neg) cout << "!";

    cout << getTypeStr() << " " << _id;
    if (depth < level)
    {   
        if (s.find(_id) != s.end()) cout << " (*)" << endl;
        else 
        {   cout << endl;
            if (_fanout.size()) s.insert(_id);
            for (int i = 0; i < _fanout.size(); ++i)
            {   CirGate* ptr = (CirGate*)(_fanout[i] & ~(size_t)(0x1));
                ptr->fanoutFlow(depth+1, level, _fanout[i]&1, s);
            }
        }
    }
    else cout << endl;
}
void CirGate::reportGate() const
{
    stringstream s;
    s << "= " << getTypeStr() << "(" << _id << ")"; 
    if (getTypeStr() == "PI" && ((CirPiGate*)this)->_name != "")
        s << "\"" << ((CirPiGate*)this)->_name << "\"";
    if (getTypeStr() == "PO" && ((CirPoGate*)this)->_name != "")
        s << "\"" << ((CirPoGate*)this)->_name << "\"";
    s << ", line " << getLineNo();
    string p;
    getline(s, p);
    cout << "==================================================" << endl;
    cout << setw(49) << left << p << "="<< endl;
    cout << "==================================================" << endl;
}

void CirGate::reportFanin(int level) const
{
    assert (level >= 0);
    set<int> set;
    faninFlow(0, level, 0, set);
}

void CirGate::reportFanout(int level) const
{
    assert (level >= 0);
    set<int> set;
    fanoutFlow(0, level, 0, set);
}

