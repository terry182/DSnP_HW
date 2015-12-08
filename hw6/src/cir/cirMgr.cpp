/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
 ****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <set>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
    EXTRA_SPACE,
    MISSING_SPACE,
    ILLEGAL_WSPACE,
    ILLEGAL_NUM,
    ILLEGAL_IDENTIFIER,
    ILLEGAL_SYMBOL_TYPE,
    ILLEGAL_SYMBOL_NAME,
    MISSING_NUM,
    MISSING_IDENTIFIER,
    MISSING_NEWLINE,
    MISSING_DEF,
    CANNOT_INVERTED,
    MAX_LIT_ID,
    REDEF_GATE,
    REDEF_SYMBOLIC_NAME,
    REDEF_CONST,
    NUM_TOO_SMALL,
    NUM_TOO_BIG,

    DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool parseError(CirParseError err)
{
    switch (err) {
        case EXTRA_SPACE:
            cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
                << ": Extra space character is detected!!" << endl;
            break;
        case MISSING_SPACE:
            cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
                << ": Missing space character!!" << endl;
            break;
        case ILLEGAL_WSPACE: // for non-space white space character
            cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
                << ": Illegal white space char(" << errInt
                << ") is detected!!" << endl;
            break;
        case ILLEGAL_NUM:
            cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
                << errMsg << "!!" << endl;
            break;
        case ILLEGAL_IDENTIFIER:
            cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
                << errMsg << "\"!!" << endl;
            break;
        case ILLEGAL_SYMBOL_TYPE:
            cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
                << ": Illegal symbol type (" << errMsg << ")!!" << endl;
            break;
        case ILLEGAL_SYMBOL_NAME:
            cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
                << ": Symbolic name contains un-printable char(" << errInt
                << ")!!" << endl;
            break;
        case MISSING_NUM:
            cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
                << ": Missing " << errMsg << "!!" << endl;
            break;
        case MISSING_IDENTIFIER:
            cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
                << errMsg << "\"!!" << endl;
            break;
        case MISSING_NEWLINE:
            cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
                << ": A new line is expected here!!" << endl;
            break;
        case MISSING_DEF:
            cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
                << " definition!!" << endl;
            break;
        case CANNOT_INVERTED:
            cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
                << ": " << errMsg << " " << errInt << "(" << errInt/2
                << ") cannot be inverted!!" << endl;
            break;
        case MAX_LIT_ID:
            cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
                << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
                << endl;
            break;
        case REDEF_GATE:
            cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
                << "\" is redefined, previously defined as "
                << errGate->getTypeStr() << " in line " << errGate->getLineNo()
                << "!!" << endl;
            break;
        case REDEF_SYMBOLIC_NAME:
            cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
                << errMsg << errInt << "\" is redefined!!" << endl;
            break;
        case REDEF_CONST:
            cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
                << ": Cannot redefine const (" << errInt << ")!!" << endl;
            break;
        case NUM_TOO_SMALL:
            cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
                << " is too small (" << errInt << ")!!" << endl;
            break;
        case NUM_TOO_BIG:
            cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
                << " is too big (" << errInt << ")!!" << endl;
            break;
        default: break;
    }
    return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool CirMgr::readCircuit(const string& fileName)
{
    ifstream s(fileName);
    if (!s.is_open()) { cerr << "Cannot open design \"" << fileName << "\"!!\n"; return false; }// File cannot open
    string line;
    lineNo = -1;
    // Reading the first line
    if (s.eof()) return false;
    getline(s, line);
    string token="";
    ++lineNo;
    int tempInt = 0;
    for (colNo = 0; colNo < line.size(); ++colNo)
    {   
        if (line[colNo] < 32) 
        {   errInt = line[colNo];
            return parseError(ILLEGAL_WSPACE);  // Space character 
        }
        else if (line[colNo] != ' ')
        {   if (token == "aag" && isdigit(line[colNo])) return parseError(MISSING_SPACE);
            token += line[colNo];
        }
        else if (colNo == 0)
            return parseError(EXTRA_SPACE);
        else if (line[colNo-1] == ' ') // Extra white space
            return parseError(EXTRA_SPACE);
        else if (token != "aag")
        {   
            errMsg = token;
            return parseError(ILLEGAL_IDENTIFIER);
        }
        else { ++colNo; break; }
    }
    if (token == "")
    {   errMsg = "\"aag\"";
        return parseError(MISSING_IDENTIFIER);
    }
    token = "";
    int idx = 0;
    for (idx = 0; colNo < line.size(); ++colNo)
    {   if (line[colNo] < 32)
        {   errInt = line[colNo];
            return parseError(ILLEGAL_WSPACE);  // Space character 
        }
        else if (line[colNo] != ' ') token += line[colNo];
        else if (line[colNo-1] == ' ') // Extra white space
            return parseError(EXTRA_SPACE);
        else if (idx == 5)
            return parseError(MISSING_NEWLINE);
        else 
        {   if (token == "")
            {   errMsg = "number of vars";
                return parseError(MISSING_NUM);
            }
            else if (!myStr2Int(token, _params[idx++]))  // Some error handling here
            {   errMsg = "number (" + token+")"; 
                return parseError(ILLEGAL_NUM);// some error handling here
            }
            if (idx == 5)
               return parseError(MISSING_NEWLINE);
            token = ""; 
        }
    }
    if (token == "")
    {   errMsg = "number of vars";
        return parseError(MISSING_NUM);
    }
    else if (!myStr2Int(token, _params[idx++]))  // Some error handling here
    {   errMsg = "number (" + token+")"; 
        return parseError(ILLEGAL_NUM);// some error handling here
    }
    token = ""; 

    if (idx < 5)
    {   errMsg = "number of vars";
        return parseError(MISSING_NUM);
    }
    else if (_params[2] > 0)
    {
        errMsg = "latches";
        return parseError(ILLEGAL_NUM);
    }
    else if (_params[0] < _params[1] + _params[4])
    {   errInt = _params[0];
        errMsg = "Num of variables";
        return parseError(NUM_TOO_SMALL);
    }
    // End of Reading the first line

    // Read PI gates
    for (int i = 0; i < _params[1]; ++i)
    {   getline(s, line);
        if(s.eof())
        {   errMsg = "PI";
            return parseError(MISSING_DEF);
        }
        ++lineNo;
        string token = "";
        for (colNo = 0; colNo < line.size(); ++colNo)
        {   if (line[colNo] < 32)
            {   errInt = line[colNo];
                return parseError(ILLEGAL_WSPACE);  // Space character 
            }
            else if (line[colNo] != ' ') token += line[colNo];
            else 
            {   int id = 0;
                if (token == "") return parseError(EXTRA_SPACE);
                else if (!myStr2Int(token, id))
                {   errMsg = "number (" + token+")"; 
                    return parseError(ILLEGAL_NUM);// some error handling here
                }
                else 
                    return parseError(MISSING_NEWLINE);
            }
        }
        int id = 0;
        if (token == "")
        {   errMsg = "PI";
            return parseError(MISSING_DEF);
        }
        else if (!myStr2Int(token, id)) // some error handling here
        {   errMsg = "number (" + token+")"; 
            return parseError(ILLEGAL_NUM);// some error handling here
        }
        else if (id < 2)
        {   errInt = id;
            return parseError(REDEF_CONST);
        }
        else if (id & 1)
        {   errInt = id;
            errMsg = "PI";
            return parseError(CANNOT_INVERTED);
        }
        else if (id > _params[0]*2)
        {   errInt = id;
            return parseError(MAX_LIT_ID);
        }
        else 
        {   CirGate* tmp = getGate(id/2);
            if (tmp)
            {   errGate = tmp;
                errInt = id;
                return parseError(REDEF_GATE);
            }
        }
        CirGate* g = new CirPiGate(id/2, lineNo+1);
        _gateList.push_back(g);
        _piList.push_back((CirPiGate*)g);
    }
    // End of Reading PI gates

    // Read PO gates
    int oid = _params[0];
    for (int i = 0; i < _params[3]; ++i)
    {   getline(s, line);
        if(s.eof())
        {   errMsg = "PO";
            return parseError(MISSING_DEF);
        }
        ++lineNo;
        string token="";
        for (colNo = 0; colNo < line.size(); ++colNo)
        {   if (line[colNo] < 32) 
            {   errInt = line[colNo];
                return parseError(ILLEGAL_WSPACE);  // Space character 
            }
            else if (line[colNo] != ' ') token += line[colNo];
            else 
            {   int id = 0;
                if (token == "") return parseError(EXTRA_SPACE);
                else if (!myStr2Int(token, id))
                {   errMsg = "number (" + token+")"; 
                    return parseError(ILLEGAL_NUM);// some error handling here
                }
                else 
                    return parseError(MISSING_NEWLINE);
            }
        }
        int id = 0;
        if (token == "")
        {   errMsg = "PO";
            return parseError(MISSING_DEF);
        }
        else if (!myStr2Int(line, id)) // some error handling here
        {   errMsg = "number (" + token+")"; 
            return parseError(ILLEGAL_NUM);
        }
        else if (id > _params[0]*2+1)
        {   errInt = id;
            return parseError(MAX_LIT_ID);
        }
        CirGate* g = new CirPoGate(++oid, lineNo+1);
        CirGate* temp = getGate(id/2);
        if (!temp)
        {   
            temp = new CirAigGate(id/2, 0);
            _gateList.push_back(temp);
            temp->_fanout.push_back((size_t)g | (size_t)(id & 1));
            g->_fanin.push_back((size_t)(temp) | (size_t)(id & 1));
        }
        else
        {   temp->_fanout.push_back((size_t)g | (size_t)(id & 1));
            g->_fanin.push_back((size_t)(temp) | (size_t)(id & 1));
        }
        _gateList.push_back(g);
        _poList.push_back((CirPoGate*)g);
    }
    // end of Reading PO Gate
    // Reading AIGs
    for (int i = 0; i < _params[4]; ++i)
    {   getline(s, line);
        if (s.eof())
        {   errMsg = "AIG";
            return parseError(MISSING_DEF);
        }
        ++lineNo;
        string token="";
        bool flag = 0;
        CirAigGate* g = 0;
        for (colNo = 0; colNo < line.size(); ++colNo)
        {   if (line[colNo] < 32)
            {   errInt = line[colNo];
                return parseError(ILLEGAL_WSPACE);  // Space character 
            }
            else if (line[colNo] != ' ') token += line[colNo];
            else
            {   int id = 0;
                if (token == "")
                    return parseError(EXTRA_SPACE);
                else if (!myStr2Int(token, id))
                {   errMsg = "number (" + token+")"; 
                    return parseError(ILLEGAL_NUM);
                }
                else if (id > _params[0]*2)
                {   errInt = id;
                    return parseError(MAX_LIT_ID);
                }

                if (!flag)
                {   
                    if (id < 2)
                    {   errInt = id;
                        return parseError(REDEF_CONST);
                    }
                    else if (id & 1)
                    {   errInt = id;
                        errMsg = "AIG";
                        return parseError(CANNOT_INVERTED);
                    }
                    CirGate* tmp = getGate(id/2);
                    if (tmp != 0)
                    {   if (tmp->_lineNum) 
                        {   errGate = tmp;
                            errInt = id;
                            return parseError(REDEF_GATE);
                        }
                        tmp->_lineNum = lineNo+1;
                        _aigList.push_back((CirAigGate*)tmp);
                        g = (CirAigGate*)tmp;  // Before definition, must be Aig
                    }
                    else
                    {   g = new CirAigGate(id/2, lineNo+1);
                        _gateList.push_back(g);
                        _aigList.push_back((CirAigGate*)g);
                    }
                    flag = 1;
                }
                else 
                {   CirGate* tmp = getGate(id/2);
                    if (!tmp)  
                    {   tmp = new CirAigGate(id/2, 0);
                        _gateList.push_back(tmp);
                    }
                    tmp->_fanout.push_back((size_t)g | (size_t)(id & 1));
                    g->_fanin.push_back((size_t)(tmp) | (size_t)(id & 1));
                }
                token = "";
            }
        }
        int id = 0;
        if (!flag && token == "")
        {   errMsg = "AIG";
            return parseError(MISSING_DEF);
        }
        else if (token == "")
            return parseError(EXTRA_SPACE);
        else if (!myStr2Int(token, id))
        {   errMsg = "number" + token;
            return parseError(ILLEGAL_NUM);
        }

        if (!flag)
        {   
            if (id < 2)
            {   errInt = id;
                return parseError(REDEF_CONST);
            }
            else if (id & 1)
            {   errInt = id;
                errMsg = "AIG";
                return parseError(CANNOT_INVERTED);
            }
            else if (id > _params[0]*2)
            {   errInt = id;
                return parseError(MAX_LIT_ID);
            }
            else 
            {   errMsg = "AIG inputs";
                return parseError(MISSING_NUM);
            }
        }
        else 
        {   
            if (id > _params[0]*2+1)
            {   errInt = id;
                return parseError(MAX_LIT_ID);
            }
            CirGate* tmp = getGate(id/2);
            if (!tmp)
            {   tmp = new CirAigGate(id/2, 0);   
                _gateList.push_back(tmp);
            }
                
            tmp->_fanout.push_back((size_t)g | (size_t)(id & 1));
            g->_fanin.push_back((size_t)(tmp) | (size_t)(id & 1));

            if (g->_fanin.size() < 2)
            {   errMsg = "AIG inputs";
                return parseError(MISSING_NUM);
            }
        }
        token = "";
    }

    // end of Reading AIGs
    // Extra things
    int type = -1; //0 for i , 1 for o, 2 for c
    while (!s.eof())
    {   getline(s, line);
        if (s.eof()) break;
        ++lineNo;
        string token="";
        colNo = 0; // Reset col No here 
        if (line.size() == 0) return parseError(EXTRA_SPACE);
        if (line[0] == ' ') return parseError(EXTRA_SPACE); // Wtf is this
        if (line[0] == 'i') type = 0;
        else if (line[0] == 'o') type = 1;
        else if (line[0] == 'c')
        {   if (line.size() != 1)
                return parseError(MISSING_NEWLINE);
            while (!s.eof())
            {   getline(s, line);
                ++lineNo;
                if (line.size() == 0) break;
                _comments.push_back(line);
            }
            break;
        }
        else 
        {   errMsg = line[0];
            return parseError(ILLEGAL_SYMBOL_TYPE);
        }
        bool flag = 0;
        for (colNo = 1; colNo < line.size(); ++colNo)
        {   if (line[colNo] < 32)
            {   errInt = line[colNo];
                return parseError(ILLEGAL_WSPACE);  // Space character 
            }
            else if (line[colNo] != ' ') token += line[colNo];
            else 
            {   int idx;
                if(!myStr2Int(token, idx))
                {   errMsg = "symbol index(" + token + ")";
                    return parseError(ILLEGAL_NUM);
                }
                flag = 1;
                if (type == 0)
                {   if (idx >= _piList.size())
                    {   errMsg = "symbol index(" + token + ")";
                        return parseError(ILLEGAL_NUM);
                    }
                    else if (_piList[idx]->_name != "")
                    {   errMsg = "i";
                        errInt = idx;
                        return parseError(REDEF_SYMBOLIC_NAME);
                    }
                    token = "";
                    for (int j = colNo+1; j < line.size(); ++j)
                    {   if (line[j] < 32)
                        {   errInt = line[colNo];
                            return parseError(ILLEGAL_SYMBOL_NAME);  // Space character 
                        }
                        else if (line[j] != ' ')token += line[j];
                    }
                    if (token == "") flag = 0;
                    else _piList[idx]->_name = token;
                } 
                else if (type == 1)
                {   if (idx >= _poList.size())
                    {   errMsg = "symbol index(" + token + ")";
                        return parseError(ILLEGAL_NUM);
                    }
                    else if (_poList[idx]->_name != "")
                    {   errMsg = "o";
                        errInt = idx;
                        return parseError(REDEF_SYMBOLIC_NAME);
                    }
                    token = "";
                    for (int j = colNo+1; j < line.size(); ++j)
                    {   if (line[j] < 32)
                        {   errInt = line[colNo];
                            return parseError(ILLEGAL_SYMBOL_NAME);  // Space character 
                        }
                        else if (line[j] != ' ')token += line[j];
                    }
                    if (token == "") flag = 0;
                    else _poList[idx]->_name = token;
                }
                break;
            }
        }
            if (!flag) 
            {
                errMsg = "symbolic name";
                return parseError(MISSING_IDENTIFIER);
            }
    }
    lineNo = colNo = 0; // Reset the two counters
    errMsg = "";
    return true;
}

CirGate* CirMgr::getGate(unsigned gid) const
{   for (size_t i = 0; i < _gateList.size(); ++i)
    if (gid == _gateList[i]->_id)
        return _gateList[i];
    return 0;
}


/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
  Circuit Statistics
  ==================
  PI          20
  PO          12
  AIG        130
  ------------------
  Total      162
 *********************/
void
CirMgr::printSummary() const
{
    int total = 0;
    cout << "Circuit Statistics" << endl;
    cout << "==================" << endl;
    cout << "  PI" <<  right << setw(12) << _piList.size()<< endl;
    total += _piList.size();
    cout << "  PO" <<  right << setw(12) <<_poList.size() << endl;
    total += _poList.size();
    cout << "  AIG" << right << setw(11) << _aigList.size() << endl;
    total += _aigList.size();
    cout << "------------------" << endl;
    cout << "  Total" << right << setw(9) << total << endl;
}

void CirMgr::printNetlist() const
{   bool flag[_params[0]+_params[3]+1];
    memset(flag, 0, sizeof(flag));
    int cnt = 0;
    for (int i = 0; i < _poList.size(); ++i)
        _poList[i]->netflow(cnt, flag);
}

void
CirMgr::printPIs() const
{
    cout << "PIs of the circuit:";
    for (int i = 0; i < _piList.size(); ++i)
        cout << " " << _piList[i]->_id;
    cout << endl;
}

void
CirMgr::printPOs() const
{
    cout << "POs of the circuit:";
    for (int i = 0; i < _poList.size(); ++i)
        cout << " " << _poList[i]->_id;
    cout << endl;
}

void
CirMgr::printFloatGates() const
{   set<int> p, q;
    for (int i = 0; i < _gateList.size(); ++i)
        if (_gateList[i]->getTypeStr() == "UNDEF")
        {   for (int j = 0; j < _gateList[i]->_fanout.size(); ++j)
            {   CirGate* out = (CirGate*)(_gateList[i]->_fanout[j] & ~(size_t)(0x1));
                p.insert(out->_id);
            }
        }
        else if (_gateList[i]->getTypeStr() != "PO" && _gateList[i]->_fanout.size() == 0)
            q.insert(_gateList[i]->_id);
    if (p.size())
    {   cout << "Gates with floating fanin(s):";
        for (std::set<int>::iterator it=p.begin(); it!=p.end(); ++it)
            cout << " " << *it;
        cout << endl;
    }
    if (q.size())
    {   cout << "Gates defined but not used:";
        for (std::set<int>::iterator it=q.begin(); it!=q.end(); ++it)
            cout << " " << *it;
        cout << endl;
    }
}

void
CirMgr::writeAag(ostream& outfile) const
{
    // header
    outfile << "aag";
    for (int i = 0; i < 5; ++i) outfile << " " << _params[i];
    outfile << endl;
    
    // PI
    for (int i = 0; i < _piList.size(); ++i)
        outfile << _piList[i]->_id*2 << endl;
    
    // PO
    for (int i = 0; i < _poList.size(); ++i)
    {   CirGate* ptr = (CirGate*)(_poList[i]->_fanin[0] & ~(size_t)(0x1));
        outfile << ptr->_id*2 + (_poList[i]->_fanin[0] & 1) << endl;
    }

    // AIG
    for (int i = 0; i < _aigList.size(); ++i)
    {   outfile << (_aigList[i]->_id << 1);
        for (int j = 0; j < _aigList[i]->_fanin.size(); ++j)
        {   CirGate* ptr = (CirGate*)(_aigList[i]->_fanin[j] & ~(size_t)0x1);
            outfile << " " << ptr->_id*2 + (_aigList[i]->_fanin[j] & 1);
        }
        outfile << endl;
    }
    
    for (int i = 0; i < _piList.size(); ++i)
    {   if (_piList[i]->_name != "")
            outfile << "i" << i << " " << _piList[i]->_name << endl;
    }
    for (int i = 0; i < _poList.size(); ++i)
    {   if (_poList[i]->_name != "")
            outfile << "o" << i << " " << _poList[i]->_name << endl;
    }
    if (_comments.size())
    {   outfile << "c" << endl;
        for (int i = 0; i < _comments.size(); ++i)
            outfile << _comments[i] << endl;
    }
}

