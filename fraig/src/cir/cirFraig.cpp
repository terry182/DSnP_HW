/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{   HashMap<HashKey, CirGate*> hash(_params[0]*4+2);
    for (unsigned i = 0, n = _dfsList.size(); i < n; i++)
      if (_dfsList[i]->getType() == AIG_GATE) // Only Merge AIG_GATE
      {    HashKey k(_dfsList[i]);
           CirGate* mergeGate; // Gate that need to be merged.
           if (hash.check(k, mergeGate))
           {    replaceGate(_dfsList[i], mergeGate, 0);

                _params[4]--;
                _gateList[_dfsList[i]->getId()] = 0;
                delete _dfsList[i];
           }
           else hash.forceInsert(k, _dfsList[i]);
      }
    buildDFSList();
}

void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
