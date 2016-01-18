/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
 ****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <list>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions
/*******************************/
/*      Class Definitions      */
/*******************************/

class SimValue
{
    public:
        SimValue(size_t data = 0): _data(data) {}
        size_t operator() () const { return _data; }
        bool operator == (const SimValue& v) const { return (_data == v._data); }

    private:
        size_t _data;
};

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/


/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void CirMgr::randomSim()
{   //TODO: Change from Const Number Simulation to Size Dependent
    list<FECGroup> &fecGrps = _fecGrps;

    HashMap<SimValue, FECGroup> newFecGrps(_dfsList.size() + _dfsList.size());

    for (unsigned j = 0, m = _dfsList.size(); j < m; ++j)
    {   if (_dfsList[j]->getType() == PI_GATE)  // Give it a input
        {   _dfsList[j]->_simValue = (((size_t)(rnGen(INT_MAX))) << 32) | (rnGen(INT_MAX));
            continue;
        }
        else if (_dfsList[j]->getType() == AIG_GATE)
        {   CirGate* ptr[2] = {(CirGate*)(_dfsList[j]->_fanin[0] & ~(size_t)0x1), (CirGate*)(_dfsList[j]->_fanin[1] & ~(size_t)0x1)};

            _dfsList[j]->_simValue = ~(size_t)(0x0);

            // Value = 0 if UNDEF_GATE
            if (ptr[0]->getType() == UNDEF_GATE) ptr[0]->_simValue = 0;
            if (ptr[1]->getType() == UNDEF_GATE) ptr[1]->_simValue = 0;

            // Get Value
            for (int k = 0; k < 2; ++k)
                if (_dfsList[j]->_fanin[k] & 1) _dfsList[j]->_simValue &= ~(ptr[k]->_simValue);
                else _dfsList[j]->_simValue &= ptr[k]->_simValue;
        }
        else if (_dfsList[j]->getType() == CONST_GATE) _dfsList[j]->_simValue = 0;
        else continue;

        // Split One time first.
        FECGroup* grp = 0;
        if (newFecGrps.check(SimValue(_dfsList[j]->_simValue), grp)) // Equivalent
                grp->addGate(_dfsList[j], 0);
        else if (newFecGrps.check(SimValue(~(_dfsList[j]->_simValue)), grp)) // Inverse Equivalent
        {     cout << grp->size() << endl;
              grp->addGate(_dfsList[j], 1);
        }
        else
        {   FECGroup newGrp;
            newGrp.addGate(_dfsList[j], 0);
            newFecGrps.forceInsert(SimValue(_dfsList[j]->_simValue), newGrp);
        }
    }

    for (HashMap<SimValue, FECGroup>::iterator it = newFecGrps.begin(); it != newFecGrps.end(); it++)
            fecGrps.push_back((*it).second);
    int i = 0;
    for (list<FECGroup>::iterator it = fecGrps.begin(); it != fecGrps.end(); it++, i++)
    {     cout << "[" << i  << "] ";
          for (FECGroup::iterator itt = (*it).begin(); itt != (*it).end(); ++itt)
              cout << (itt.getInverse() ? "!" : "") << (*itt)->getId() << " ";

              cout << "Size :" << (*it).size();
          cout << endl;
    }

    for (list<FECGroup>::iterator it = fecGrps.begin(); it != fecGrps.end(); ++it)// for_each(fecGrp, fecGrps):
    {   FECGroup fecGrp = *it;
        Hash<SimValue, FECGroup> newFecGrps;

        for(FECGroup::iterator i = fecGrp.begin(); i != fecGrp.end(); ++i)
        {   FECGroup* grp;
            if (newFecGrps.check(SimData(_dfsList[j]->_simValue), grp)) // Equivalent
                grp->addGate(*i, 0);
            else if (newFecGrps.check(SimData(~(_dfsList[j]->_simValue)), grp)) // Inverse Equivalent
                grp->addGate(*i, 1);
        }

        // Delete the group first.
        fecGrps.erase(it);

        // CollectValidFecGrp(newFecGrps, fecGrp, fecGrps);
        for (HashMap<SimValue, FECGroup>::iterator it = newFecGrps.begin(); it != newFecGrps.end(); it++)
                fecGrps.push_back((*it).second);
    }

}

void CirMgr::fileSim(ifstream& patternFile)
{
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
