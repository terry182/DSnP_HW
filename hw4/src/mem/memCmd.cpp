/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{
   cout << setw(15) << left << "MTReset: "
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO Status: DONE.
   vector<string> options;
   if (!CmdExec::lexOptions(option, options)) return CMD_EXEC_ERROR;

   if (options.empty()) return CmdExec::errorOption(CMD_OPT_MISSING, "");

   bool alloc_array = false;
   int arraySize = 0, numObjects = 0;
   for (size_t i = 0, n = options.size(); i < n; ++i) // Parsing and error handling.
   {    if(myStrNCmp("-Array", options[i], 2) == 0)
       {    if (alloc_array) return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
            alloc_array = true;
            ++i;
            if (i >= n) return CmdExec::errorOption(CMD_OPT_MISSING, options[i-1]);
            if (!myStr2Int(options[i], arraySize) || arraySize < 1) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
       }
       else if (numObjects != 0) return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
       else if (!myStr2Int(options[i], numObjects) || numObjects < 1) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
   }
    if (numObjects == 0) return CmdExec::errorOption(CMD_OPT_MISSING, "");

    try {
        if (alloc_array) mtest.newArrs(numObjects, arraySize);
        else mtest.newObjs(numObjects);
    }
    catch (bad_alloc) {return CMD_EXEC_ERROR;}

   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{
   cout << setw(15) << left << "MTNew: "
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO Status: DONE.
    vector<string> options;
    if(!CmdExec::lexOptions(option, options)) return CMD_EXEC_ERROR;

    if (options.empty()) return CmdExec::errorOption(CMD_OPT_MISSING, "");

    bool Index= false, Random = false, Array = false;
    int objId = -1, numRandId = -1, numIdx = 0;
    for (size_t i = 0, n = options.size(); i < n; ++i) // Parsing and error handling.
    {   if (!myStrNCmp("-Random", options[i], 2))
        {  if(Index || Random ) return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
            ++i; Random = true;
           if(i >= n) return CmdExec::errorOption(CMD_OPT_MISSING, options[i-1]);
           if(!myStr2Int(options[i], numRandId) || numRandId <= 0)  return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
           numIdx = i;
        }
        else if (!myStrNCmp("-Index", options[i], 2))
        {  if(Index || Random ) return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
            ++i; Index = true;
           if(i >= n) return CmdExec::errorOption(CMD_OPT_MISSING, options[i-1]);
           if(!myStr2Int(options[i], objId) || objId < 0)  return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
           numIdx = i;
        }
        else if (!myStrNCmp("-Array", options[i], 2))
        {   if (Array) return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
            Array = true;
        }
        else if (Index | Random) return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
        else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
    }

    if (!Random && !Index) return CmdExec::errorOption(CMD_OPT_MISSING, "");

    if (Array)
    {   if (Index)
        {   if (objId >= mtest.getArrListSize())
            {   cerr << "Size of array list ("<< mtest.getArrListSize() << ") is <= " << objId << "!!" << endl;
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[numIdx]);
            }
            mtest.deleteArr(objId);
        }
        else if (Random)
        {   int s = mtest.getArrListSize();
            if (s == 0)
            {  cerr << "Size of array list is 0!!" << endl;
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[numIdx-1]);
            }
            for (int i = 0; i < numRandId; ++i)
               mtest.deleteArr(rnGen(s));
        }
    }
    else
    {   if (Index)
        {   if (objId >= mtest.getObjListSize())
            {   cerr << "Size of object list ("<< mtest.getObjListSize() << ") is <= " << objId << "!!" << endl;
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[numIdx]);
            }
            mtest.deleteObj(objId);
        }
        else if (Random)
        {   int s = mtest.getObjListSize();
            if (s == 0)
            {  cerr << "Size of object list is 0!!" << endl;
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[numIdx-1]);
            }
            for (int i = 0; i < numRandId; ++i)
               mtest.deleteObj(rnGen(s));
        }
    }

   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{
   cout << setw(15) << left << "MTDelete: "
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{
   cout << setw(15) << left << "MTPrint: "
        << "(memory test) print memory manager info" << endl;
}


