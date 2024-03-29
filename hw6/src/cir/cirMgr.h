/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
 ****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"
#include "cirGate.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
    public:
        CirMgr() { memset(_params, 0, sizeof(_params)); }
        ~CirMgr() {}

        // Access functions
        // return '0' if "gid" corresponds to an undefined gate.
        CirGate* getGate(unsigned gid) const;

        // Member functions about circuit construction
        bool readCircuit(const string&);

        // Member functions about circuit reporting
        void printSummary() const;
        void printNetlist() const;
        void printPIs() const;
        void printPOs() const;
        void printFloatGates() const;
        void writeAag(ostream&) const;

    private:
        int              _params[5];    // M I L O A
        vector<CirGate*> _gateList;
        vector<CirPiGate*>   _piList;       // Store things in pointers
        vector<CirPoGate*>   _poList;       
        vector<CirAigGate*>   _aigList;
        vector<string>   _comments;
};

#endif // CIR_MGR_H
