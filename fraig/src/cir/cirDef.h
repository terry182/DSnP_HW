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

class CirGate;
class CirPiGate;
class CirPoGate;
class ConstGate;
class CirAigGate;
class CirMgr;
class SatSolver;

enum GateType
{
    UNDEF_GATE = 0,
    PI_GATE = 1,
    PO_GATE = 2,
    AIG_GATE = 3,
    CONST_GATE = 4,

    TOT_GATE
};

// Hash Key Definition for strash
class HashKey {

private:
    size_t in[2];
public:
    size_t operator() () const
    {
    }
    bool operator == (const HashKey& k) const
    {
      
    }
};


#endif // CIR_DEF_H
