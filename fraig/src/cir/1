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
{   vector<CirGate*> delList;
    bool flag[_gateList.size()];
    memset(flag, 0, sizeof(flag));

    flag[0] = true; // CONST should not be removed.
    for (int i = 0; i < _dfsList.size(); ++i) flag[_dfsList[i]->getId()] = true; // Those in _dfsList just let it be visited
    
    for (int i = 0, n = _gateList.size(); i < n; i++)
        if (_gateList[i]->getType() == AIG_GATE && _gateList[i]->_fanout.size() == 0)
            _gateList[i]->netflow(flag, delList, true);

    for (int i = 0; i < delList.size(); i++) 
    {   unsigned gid = delList[i]->getId();
        // Remove from Gate List
        _gateList[gid] = 0;
        if (delList[i]->getType == AIG_GATE) --_params[4]; // Number change.
        cout << "Sweeping: ";
        cout << delList[i]->getTypeStr() << "(" << gid << ") removed..." << endl;
    }
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
