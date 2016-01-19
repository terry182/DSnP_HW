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
#include <fstream>
#include <iostream>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"
#include "cirGate.h"

extern CirMgr *cirMgr;

class CirMgr
{
    public:
        CirMgr() {}
        ~CirMgr()
        {       for (int i = 0; i < _gateList.size(); ++i) delete _gateList[i];
        }

        // Access functions
        // return '0' if "gid" corresponds to an undefined gate.
        CirGate* getGate(unsigned gid) const {
            if (gid >= _gateList.size() || _gateList[gid] == 0) return 0;
            return (_gateList[gid]->getType() == UNDEF_GATE)? 0 : _gateList[gid];
        }

        // Get Max Num (M of MILOA)
        unsigned _maxNum() { return _params[0]; }

        // Member functions about circuit construction
        bool readCircuit(const string&);
        void buildDFSList();

        // Member functions about circuit optimization
        void sweep();
        void optimize();

        // Member functions about simulation
        void randomSim();
        void fileSim(ifstream&);
        void setSimLog(ofstream *logFile) { _simLog = logFile; }

        // Member functions about fraig
        void strash();
        void printFEC() const;
        void fraig();

        // Member functions about circuit reporting
        void printSummary() const;
        void printNetlist() const;
        void printPIs() const;
        void printPOs() const;
        void printFloatGates() const;
        void printFECPairs() const;
        void writeAag(ostream&) const;
        void writeGate(ostream&, CirGate*) const;

    private:
        int                  _params[5];    // M I L O A
        vector<CirGate*>     _gateList;
        vector<CirPiGate*>   _piList;       // Store things in pointers
        vector<CirPoGate*>   _poList;
        vector<CirGate*>     _dfsList;       // DFS List on the way
        vector<string>       _comments;
        ofstream             *_simLog;

        list<FECGroup>       _fecGrps;
        // Private member of optimize
        void replaceGate(CirGate*, CirGate*, const bool&);

        //Private member of simluation
        void simulate(size_t* const &init, list<FECGroup> &fecGrps, HashMap<SimValue, FECGroup> &newFecGrps);
        void simFirstTime(size_t* const &init, list<FECGroup> &fecGrps, HashMap<SimValue, FECGroup> &newFecGrps);


};

#endif // CIR_MGR_H
