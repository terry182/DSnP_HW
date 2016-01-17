/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void CirMgr::sweep()
{   bool flag[_gateList.size()];
    memset(flag, 0, sizeof(flag));
    vector<CirGate*> delList;

    flag[0] = true; // CONST should not be removed.
    for (int i = 0; i < _dfsList.size(); ++i) flag[_dfsList[i]->getId()] = true; // Those in _dfsList just let it be visited
    for (int i = 0; i < _piList.size(); ++i) flag[_piList[i]->getId()] = true;

    for (int i = 0, n = _gateList.size(); i < n; i++)
        if (flag[i])
        {   vector<size_t>::iterator p = _gateList[i]->_fanout.begin();
            for (unsigned j = 0, m = _gateList[i]->_fanout.size(); j < m; j++)
            {   CirGate* ptr = (CirGate*)(_gateList[i]->_fanout[j] & ~(size_t)(0x1));
                if (!flag[ptr->getId()]) _gateList[i]->_fanout.erase(p+j);
            }
        }
        else if (_gateList[i] && _gateList[i]->getType() == AIG_GATE)
        {   --_params[4];     //  number
            cout << "Sweeeping: ";
            delList.push_back(_gateList[i]);
            cout << _gateList[i]->getTypeStr() << "(" << i << ")";
            _gateList[i] = 0;
            cout << " removed..." << endl;
        }
        else if (_gateList[i])
        {   delList.push_back(_gateList[i]);
            cout << "Sweeeping: ";
            cout << _gateList[i]->getTypeStr() << "(" << i << ")";
            cout << " removed..." << endl;
            _gateList[i] = 0;
        }
    for (int i = 0; i < delList.size(); ++i)  delete delList[i];

}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void CirMgr::optimize()
{    for (size_t i = 0; i < _dfsList.size(); i++)
          if (_dfsList[i]->getType() == AIG_GATE)
          {    std::vector<size_t> &ls = _dfsList[i]->_fanin;
               CirGate* ptr[2] = {(CirGate*)(ls[0] & ~(size_t)0x1), (CirGate*)(ls[1] & ~(size_t)0x1)};

               if (ls[0] == ls[1])   // A and A = A
                  replaceGate(_dfsList[i], ptr[0], ls[0]&1); // replace _dfsList[i] with _dfsList[i]->_fanin
               else if (ptr[0] == ptr[1])   // A and Not A = 0
                  replaceGate(_dfsList[i], _gateList[0], 0);// replace _dfsList[i] with const 0
               else if (ls[0] == (size_t)_gateList[0] || ls[1] == (size_t)_gateList[0]) // O and A = 0
                    replaceGate(_dfsList[i], _gateList[0], 0); // same as above
               else if (ptr[0] == _gateList[0] || ptr[1] == _gateList[0]) // 1 and A = A
                    replaceGate(_dfsList[i], ptr[(ptr[0] == _gateList[0])], ls[(ptr[0] == _gateList[0])]&1);

               for (int i = 0; i < 2; ++i)
                  for (int j = 0; j < ptr[i]->_fanout.size(); ++j)
                      if (((CirGate*)(ptr[i]->_fanout[j] & ~(size_t)0x1)) == _dfsList[i])
                           ptr[i]->_fanout.erase(ptr[i]->_fanout.begin()+j);
                           
               _gateList[_dfsList[i]->getId()] = 0;         // delete instance in gateList
               delete _dfsList[i];
          }

    buildDFSList();
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
void CirMgr::replaceGate(CirGate* ori, CirGate* tar, const bool& inverse)
{    for (size_t i = 0; i < ori->_fanout.size(); i++)
     {    CirGate* ptr = (CirGate*)(ori->_fanout[i] & ~(size_t)0x1);
          cout << "Target Gate: " << ptr->getId() << endl;
          vector<size_t> &ls = ptr->_fanin;
          for (size_t j = 0; j < ls.size(); j++)
          {    if  ( ((CirGate*)(ls[j] & ~(size_t)0x1)) == ori )
                    ls[j] = ((size_t)tar | (size_t)(inverse));
          }
          tar->_fanout.push_back( (size_t) ptr | (size_t)(inverse));
     }
}
