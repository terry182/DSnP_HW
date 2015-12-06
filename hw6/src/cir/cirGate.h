/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
 ****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
    friend class CirMgr;
    friend class CirAigGate;
    public:
        CirGate(int id = 0, int lineNum = 0):_id(id), _lineNum(lineNum) {}
        virtual ~CirGate() {}

        // Basic access methods
        virtual string getTypeStr() const {return "";}
        unsigned getLineNo() const { return _lineNum; }

        // Printing functions
        virtual void printGate() const = 0;
        void reportGate() const;
        virtual void reportFanin(int level) const;
        virtual void reportFanout(int level) const;
        void netflow(int &cnt, bool flag[]) const;

    private:
        int _id;
        int _lineNum;
        vector<size_t> _fanin;  // Size_t with last bit representing  invert
        vector<size_t> _fanout;
};

class CirAigGate: public CirGate
{
    friend class CirMgr;
    public:
        CirAigGate(int id = 0, int lineNum = 0):CirGate(id, lineNum) {}
        void printGate() const{};
        string getTypeStr() const
        {   if (_id == 0) return "CONST0";
            return ((getLineNo()) ? "AIG" : "UNDEF"); 
        }
};

class CirPiGate: public CirGate
{
    friend class CirMgr;
    friend class CirGate;
    public:
        CirPiGate(int id = 0, int lineNum = 0):CirGate(id, lineNum), _name("") {}
        string getTypeStr() const { return "PI"; }
        void printGate() const
        {   
        }
    private:
        string _name;
};

class CirPoGate: public CirGate
{
    friend class CirMgr;
    friend class CirGate;
    public:
        CirPoGate(int id = 0, int lineNum = 0):CirGate(id, lineNum), _name("") {}
        string getTypeStr() const { return "PO"; }
        void printGate() const
        {   
        }
    private:
        string _name;
};
#endif // CIR_GATE_H
