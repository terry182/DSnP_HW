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

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void CirGate::netflow(bool flag[], vector<CirGate*>& list, bool checkDel) const
{   if (flag[_id]) return;
    if (checkDel && getType() == PI_GATE) return ;
    for (int i = 0; i < _fanin.size(); ++i)
    {   CirGate* ptr = (CirGate*)(_fanin[i] & ~(size_t)(0x1));
        if (checkDel || ptr->getType() != UNDEF_GATE)
            ptr->netflow(flag, list, checkDel);
    }

    flag[_id] = true; // Visited
    list.push_back((CirGate*)this);
}

void CirGate::faninFlow(int depth, int &level, bool neg, std::set<int> &s) const
{   // indent
    for (int i = 0; i < depth; ++i) cout << "  ";

    if (neg) cout << "!";

    if (getType() != CONST_GATE) cout << getTypeStr() << " " << _id;
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

void
CirGate::reportGate() const
{   stringstream s;
    s << "= " << getTypeStr() << "(" << _id << ")";
    if (getType() == PI_GATE && !((CirPiGate*)this)->_name.empty())
        s << "\"" << ((CirPiGate*)this)->_name << "\"";
    if (getType() == PO_GATE && !((CirPoGate*)this)->_name.empty())
        s << "\"" << ((CirPoGate*)this)->_name << "\"";
    s << ", line " << getLineNo() << endl;
    string p;
    getline(s, p);
    cout << "==================================================" << endl;
    cout << setw(49) << left << p << "="<< endl;
    s << "= FECs:";
    if (_grp)
    {   FECGroup* fecGrp = (FECGroup*)(_grp & ~(size_t)(0x1));
        bool inv = _grp & 1;
        for (FECGroup::iterator it = fecGrp->begin(); it != fecGrp->end(); ++it)
            if ((*it) != this)
                  s << ((it.getInverse()^inv) ? "!": "" ) << (*it)->getId() << " ";
    }
        s << endl;
        getline(s, p);
        cout << setw(49) << left << p << "="<< endl;


    s << "= Value: ";
    size_t v = _simValue;
    for (int cnt = 0; cnt < 32; cnt++)
    {     if (cnt && cnt % 4 == 0) s << "_";
          s << (v&1) ;
          v >>= 1;
    }
    getline(s, p);
    cout << setw(49) << left << p << "="<< endl;
    cout << "==================================================" << endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   set<int> set;
   faninFlow(0, level, 0, set);
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   set<int> set;
   fanoutFlow(0, level, 0, set);
}
