/****************************************************************************
  FileName     [ cirDef.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic data or var for cir package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_DEF_H
#define CIR_DEF_H

#include <vector>
#include "myHashMap.h"

using namespace std;

// TODO: define your own typedef or enum

// Definition of Gates
class CirGate;
class CirPiGate;
class CirPoGate;
class ConstGate;
class CirAigGate;

class CirMgr;
class SatSolver;

// Hash Key Definition for strash
class HashKey;

enum GateType
{
    UNDEF_GATE = 0,
    PI_GATE = 1,
    PO_GATE = 2,
    AIG_GATE = 3,
    CONST_GATE = 4,

    TOT_GATE
};

// FECGroup Class for simulation
class FECGroup
{
    private:
        list<size_t> gate;

    public:
        FECGroup(){}

        class iterator
        {
            public:
                iterator() {}
                iterator(std::list<size_t>::iterator i) { it = i; }

                CirGate* operator* () { return (CirGate*)(*it & ~(size_t)(0x1)); }
                const CirGate* operator* () const { return (CirGate*)(*it & ~(size_t)(0x1)); }

                bool getInverse() const { return ((*it) & 1); }

                iterator& operator++ () { it++; return (*this); }
                iterator operator ++ (int) { iterator li=(*this); ++(*this); return li; }

                iterator& operator= (const iterator& i) { it = i.it; return *this; }

                bool operator == (const iterator& i) const { return (it == i.it); }
                bool operator != (const iterator& i) const { return !(*this == i); }

            private:
                list<size_t>::iterator it;
        };

        iterator begin()
        {   return iterator( gate.begin() ); }

        iterator end()
        {    return iterator(gate.end());  }

        size_t size() { return gate.size(); }

        FECGroup& addGate(const CirGate* g, const bool &inv)
        {     cout << "Somebody inside" << endl;
              gate.push_back((size_t)(g) | (size_t)inv);
              return(*this);
        }
};

#endif // CIR_DEF_H
