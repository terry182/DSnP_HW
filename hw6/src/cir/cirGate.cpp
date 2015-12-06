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
    if (getTypeStr() == "CONST0")
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
        CirGate* ptr = (CirGate*)(_fanin[i] & ~0x1);
        if (ptr->getTypeStr() == "UNDEF") cout << "*";
        if (_fanin[i] & 1) cout << "!";
        cout << ptr->_id;
    }
    if (getTypeStr() == "PI" && ((CirPiGate*)this)->_name != "") cout << " (" << ((CirPiGate*)this)->_name << ")";
    if (getTypeStr() == "PO" && ((CirPoGate*)this)->_name != "") cout << " (" << ((CirPoGate*)this)->_name << ")";
    cout << endl;
}

void
CirGate::reportGate() const
{
    cout << "==================================================" << endl;
    cout << "= " << getTypeStr() << "("<< _id << "), line " << getLineNo() << "= " << endl;
    cout << "==================================================" << endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
}

