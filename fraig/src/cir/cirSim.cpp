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


/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

// This return floor(log2 n);
static inline unsigned Log2(size_t n)
{   unsigned ans = 0;
    while (n >>= 1) ++ans;
    return ans;
}

static bool cmpID(size_t a, size_t b)
{    return (((CirGate*)(a & ~(size_t)0x1))->getId() < ((CirGate*)(b & ~(size_t)0x1))->getId());
}

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void CirMgr::randomSim()
{   //TODO: Change from Const Number Simulation to Size Dependent
    list<FECGroup> &fecGrps = _fecGrps;

    unsigned failCnt = 0, maxFail = Log2(_params[1]+_params[4]); // Magic Numberrrrrr
    unsigned cnt = 0;
    cout << "MAX_FAILS = " << maxFail << endl;


    HashMap<SimValue, FECGroup> newFecGrps(_dfsList.size());

    newFecGrps.forceInsert(SimValue(0), FECGroup(_gateList[0]));
    // First time of random
    for (unsigned j = 0, m = _dfsList.size(); j < m; ++j)
    {   if (_dfsList[j]->getType() == PI_GATE)  // Give it a input
        {   _dfsList[j]->_simValue = (((size_t)(rnGen(INT_MAX))) << 32) | (rnGen(INT_MAX));
            continue;
        }
        else if (_dfsList[j]->getType() == AIG_GATE)
        {   CirGate* ptr[2] = {(CirGate*)(_dfsList[j]->_fanin[0] & ~(size_t)0x1), (CirGate*)(_dfsList[j]->_fanin[1] & ~(size_t)0x1)};

            _dfsList[j]->_simValue = ~(size_t)(0x0);
            _dfsList[j]->_grp = 0;
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
            grp->addGate(_dfsList[j], 1);
        else
        {   FECGroup newGrp;
            newGrp.addGate(_dfsList[j], 0);
            newFecGrps.forceInsert(SimValue(_dfsList[j]->_simValue), newGrp);
        }
    }
    cnt += 64;
    if (_simLog)
    {     size_t I[_piList.size()], O[_piList.size()];
            for (int j = 0; j < _piList.size(); ++j)
                I[j] = _piList[j]->_simValue;
            for (int j = 0; j < _poList.size(); ++j)
                O[j] = _poList[j]->_simValue;

          for (int i = 0; i < 64; i++)
          {     for (int j = 0; j < _piList.size(); I[j++] >>= 1)
                     *_simLog << (I[j] & 1);
                cout << " ";
                for (int j = 0; j < _poList.size(); O[j++] >>= 1)
                        *_simLog << (O[j] & 1);
                cout << endl;
          }
    }
    //CollectValidFecGrp
    for (HashMap<SimValue, FECGroup>::iterator it = newFecGrps.begin(); it != newFecGrps.end(); it++)
        if ((*it).second.size() > 1) // Single Dog is not needed.
            fecGrps.push_back((*it).second);

    // End of First Time.
    //  cout << "First Time success" << endl;

    //    printFECPairs();
    // Start Randoming.

    while (failCnt < maxFail)
    {   // cout << "failCnt:" << failCnt << " maxFail:" << maxFail <<  " " << endl;
        cout << "Total # of FEC Groups:" << fecGrps.size() << endl;
        // Assign Value and simulate.
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
        }
        if (_simLog)
        {     size_t I[_piList.size()], O[_piList.size()];
                for (int j = 0; j < _piList.size(); ++j)
                    I[j] = _piList[j]->_simValue;
                for (int j = 0; j < _poList.size(); ++j)
                    O[j] = _poList[j]->_simValue;

              for (int i = 0; i < 64; i++)
              {     for (int j = 0; j < _piList.size(); I[j++] >>= 1)
                         *_simLog << (I[j] & 1);
                    cout << " ";
                    for (int j = 0; j < _poList.size(); O[j++] >>= 1)
                            *_simLog << (O[j] & 1);
                    cout << endl;
              }
        }
        size_t t = fecGrps.size(), n = 0;
        for (list<FECGroup>::iterator it = fecGrps.begin(); it != fecGrps.end() && n < t; n++)// for_each(fecGrp, fecGrps):
        {   FECGroup fecGrp = *it;
            newFecGrps.init(fecGrp.size());
            for(FECGroup::iterator i = fecGrp.begin(); i != fecGrp.end(); ++i)
            {   FECGroup* grp;
                if (newFecGrps.check(SimValue((*i)->_simValue), grp)) // Equivalent
                    grp->addGate(*i, 0);
                else if (newFecGrps.check(SimValue(~((*i)->_simValue)), grp)) // Inverse Equivalent
                    grp->addGate(*i, 1);
                else
                {   FECGroup newGrp;
                    newGrp.addGate(*i, 0);
                    newFecGrps.forceInsert(SimValue((*i)->_simValue), newGrp);
                }
            }
            //Delete the group first.
            it = fecGrps.erase(it);
            // CollectValidFecGrp(newFecGrps, fecGrp, fecGrps);
            for (HashMap<SimValue, FECGroup>::iterator i = newFecGrps.begin(); i != newFecGrps.end(); i++)
                if ((*i).second.size() > 1) // Single Dog is not needed.
                    fecGrps.push_back((*i).second);
        }
        if(fecGrps.size() == t) ++failCnt;
        cnt += 64;
    }

    // Build CirGate thing.
    for (list<FECGroup>::iterator it = fecGrps.begin(); it != fecGrps.end(); it++)
    {     FECGroup* fecGrp = &(*it);
          fecGrp->gate.sort(cmpID); // sort by ID
          for (FECGroup::iterator i = fecGrp->begin(); i != fecGrp->end(); ++i)
                  (*i)->_grp = (size_t)fecGrp | (size_t)i.getInverse();
    }
    cout << cnt << " patterns simlated." << endl;
}

void CirMgr::fileSim(ifstream& patternFile)
{

    list<FECGroup> &fecGrps = _fecGrps;
    size_t p[_params[1]];
    memset(p, 0, sizeof(p));

    // Read pattern from file
    string s;
    unsigned cnt = 0;
    HashMap<SimValue, FECGroup> newFecGrps(_dfsList.size());

    while (patternFile >> s)
    {   if (s.size() != _params[1])
        {   cerr << "Error: Pattern(" << s << ") length(" << s.length() << ")" << "does not match the number of inputs(" << _params[1] << ") in a circuit!!\n";
            return;
        }

        for (size_t i = 0; i < _params[1]; ++i)
        {   if (s[i] != '0' && s[i] != '1')
            {   cerr << "Error: Pattern(" << s << ") contains a non-0/1 characters(\'" << s[i]  << "\').\n";
                return ;
            }
            else p[i] = (p[i] << 1) | (s[i] - '0');
        }
        if (++cnt % 64 == 0)
        {   if (cnt == 64)  simFirstTime(&p[0], fecGrps, newFecGrps);
            else simulate(&(p[0]), fecGrps, newFecGrps);  // Already stored 64 pattern
        }
    }

    if (cnt % 64)  simulate(&(p[0]), fecGrps, newFecGrps);

    // Build CirGate thing.
    for (list<FECGroup>::iterator it = fecGrps.begin(); it != fecGrps.end(); it++)
    {     FECGroup* fecGrp = &(*it);
            fecGrp->gate.sort(cmpID);   // Sort ID

          for (FECGroup::iterator i = fecGrp->begin(); i != fecGrp->end(); ++i)
                (*i)->_grp = (size_t)fecGrp | (size_t)i.getInverse();

    }

    cout << "Total # of FEC Groups:" << fecGrps.size() << endl;
    cout << cnt << " pattern simulated"<< endl;

}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/

// This function simulate things,
// Init = 0  => Random Input
void CirMgr::simFirstTime(size_t* const &init, list<FECGroup> &fecGrps, HashMap<SimValue, FECGroup> &newFecGrps)
{
    if (init) for (int i = 0; i < _params[1]; ++i) _piList[i]->_simValue = init[i];

    newFecGrps.forceInsert(SimValue(0), FECGroup(_gateList[0]));

    for (unsigned j = 0, m = _dfsList.size(); j < m; ++j)
    {   if (init == 0 && _dfsList[j]->getType() == PI_GATE) // Need to random.
            _dfsList[j]->_simValue = (((size_t)(rnGen(INT_MAX))) << 32) | (rnGen(INT_MAX));
        else if (_dfsList[j]->getType() == AIG_GATE)
        {   CirGate* ptr[2] = {(CirGate*)(_dfsList[j]->_fanin[0] & ~(size_t)0x1), (CirGate*)(_dfsList[j]->_fanin[1] & ~(size_t)0x1)};

            _dfsList[j]->_simValue = ~(size_t)(0x0);
            _dfsList[j]->_grp = 0;

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

        FECGroup* grp = 0;
        if (newFecGrps.check(SimValue(_dfsList[j]->_simValue), grp)) // Equivalent
            grp->addGate(_dfsList[j], 0);
        else if (newFecGrps.check(SimValue(~(_dfsList[j]->_simValue)), grp)) // Inverse Equivalent
            grp->addGate(_dfsList[j], 1);
        else
        {   FECGroup newGrp;
            newGrp.addGate(_dfsList[j], 0);
            newFecGrps.forceInsert(SimValue(_dfsList[j]->_simValue), newGrp);
        }
    }

    if (_simLog)
    {     size_t I[_piList.size()], O[_piList.size()];
            for (int j = 0; j < _piList.size(); ++j)
                I[j] = _piList[j]->_simValue;
            for (int j = 0; j < _poList.size(); ++j)
                O[j] = _poList[j]->_simValue;

          for (int i = 0; i < 64; i++)
          {     for (int j = 0; j < _piList.size(); I[j++] >>= 1)
                     *_simLog << (I[j] & 1);
                cout << " ";
                for (int j = 0; j < _poList.size(); O[j++] >>= 1)
                        *_simLog << (O[j] & 1);
                cout << endl;
          }
    }
    //Collect Valid FecGrp
    for (HashMap<SimValue, FECGroup>::iterator it = newFecGrps.begin(); it != newFecGrps.end(); it++)
        if ((*it).second.size() > 1) // Single Dog is not needed.
            fecGrps.push_back((*it).second);

}


void CirMgr::simulate(size_t* const &init, list<FECGroup> &fecGrps, HashMap<SimValue, FECGroup> &newFecGrps)
{
    //  cout << "Total # of FECGroups: " <<  fecGrps.size() << endl;
    if (init) for (int i = 0; i < _params[1]; ++i) _piList[i]->_simValue = init[i];


    for (unsigned j = 0, m = _dfsList.size(); j < m; ++j)
    {   if (init == 0 && _dfsList[j]->getType() == PI_GATE) // Need to random.
        _dfsList[j]->_simValue = (((size_t)(rnGen(INT_MAX))) << 32) | (rnGen(INT_MAX));
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
    }
    if (_simLog)
    {     size_t I[_piList.size()], O[_piList.size()];
            for (int j = 0; j < _piList.size(); ++j)
                I[j] = _piList[j]->_simValue;
            for (int j = 0; j < _poList.size(); ++j)
                O[j] = _poList[j]->_simValue;

          for (int i = 0; i < 64; i++)
          {     for (int j = 0; j < _piList.size(); I[j++] >>= 1)
                     *_simLog << (I[j] & 1);
                cout << " ";
                for (int j = 0; j < _poList.size(); O[j++] >>= 1)
                        *_simLog << (O[j] & 1);
                cout << endl;
          }
    }

    size_t t = fecGrps.size(), i = 0;
    for (list<FECGroup>::iterator it = fecGrps.begin(); it != fecGrps.end() && i < t; i++)// for_each(fecGrp, fecGrps):
    {   FECGroup fecGrp = *it;
        newFecGrps.init(fecGrp.size());
        for(FECGroup::iterator i = fecGrp.begin(); i != fecGrp.end(); ++i)
        {   FECGroup* grp;
            if (newFecGrps.check(SimValue((*i)->_simValue), grp)) // Equivalent
                grp->addGate(*i, 0);
            else if (newFecGrps.check(SimValue(~((*i)->_simValue)), grp)) // Inverse Equivalent
                grp->addGate(*i, 1);
            else
            {   FECGroup newGrp;
                newGrp.addGate(*i, 0);
                newFecGrps.forceInsert(SimValue((*i)->_simValue), newGrp);
            }
        }
        //Delete the group first.
        it = fecGrps.erase(it);
        //Collect Valid FecGrp
        for (HashMap<SimValue, FECGroup>::iterator it = newFecGrps.begin(); it != newFecGrps.end(); it++)
            if ((*it).second.size() > 1) // Single Dog is not needed.
                fecGrps.push_back((*it).second);
    }


}
