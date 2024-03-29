/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-2015 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <cstring>
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream&);
ParseChar getChar(istream&);
string trim(string);


//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void
CmdParser::readCmd()
{
   if (_dofile.is_open()) {
      readCmdInt(_dofile);
      _dofile.close();
   }
   else
      readCmdInt(cin);
}

void
CmdParser::readCmdInt(istream& istr)
{
   resetBufAndPrintPrompt();

   while (1) {
      ParseChar pch = getChar(istr);
      if (pch == INPUT_END_KEY) break;
      switch (pch) {
         case LINE_BEGIN_KEY :
         case HOME_KEY       : moveBufPtr(_readBuf); break;
         case LINE_END_KEY   :
         case END_KEY        : moveBufPtr(_readBufEnd); break;
         case BACK_SPACE_KEY : if(moveBufPtr(_readBufPtr-1)) deleteChar();/* TODO */ break;
         case DELETE_KEY     : deleteChar(); break;
         case NEWLINE_KEY    : addHistory();
                               cout << char(NEWLINE_KEY);
                               resetBufAndPrintPrompt(); break;
         case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
         case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
         case ARROW_RIGHT_KEY: moveBufPtr(_readBufPtr+1);/* TODO */ break;
         case ARROW_LEFT_KEY : moveBufPtr(_readBufPtr-1);/* TODO */ break;
         case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
         case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
         case TAB_KEY        :insertChar(' ', 8-(_readBufPtr - _readBuf)%8); break;
         case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
         case UNDEFINED_KEY:   mybeep(); break;
         default:  // printable character
            insertChar(char(pch)); break;
      }
      #ifdef TA_KB_SETTING
      taTestOnly();
      #endif
   }
}


// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool
CmdParser::moveBufPtr(char* const ptr)
{
   // TODO...
   // Status: Finished. Tried to make it beautiful.
   if (ptr >= _readBuf && ptr <= _readBufEnd)
   {
        if (ptr <= _readBufPtr)
            for (;_readBufPtr != ptr; _readBufPtr--)
                cout << char(8);
        else
            for (;_readBufPtr != ptr; _readBufPtr++)
                cout << *_readBufPtr;
   }
   else
   {   // Reaching the end. Can't move cursor
       mybeep();
       return false;
   }
   return true;
}


// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool
CmdParser::deleteChar()
{
   // TODO...
   // Status: Finished. Seems.
   if (_readBufPtr == _readBufEnd)
   {
       mybeep();
       return false;
   }
   for(char* d = _readBufPtr; d != _readBufEnd; d++) *d = *(d+1);
   printf("%s ", _readBufPtr);

   char* dummy = _readBufPtr;

    _readBufPtr = _readBufEnd;
    moveBufPtr(dummy);
    _readBufEnd--;

   return true;
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
//
void
CmdParser::insertChar(char ch, int repeat)
{
   // TODO...
   // Current Status: Finished. Tried to make it beautiful.
   assert(repeat >= 1);
   // shifting the remaing the string first and then insert characters
   for (char* d = _readBufEnd; d != _readBufPtr-1; d--) *(d+repeat) = *d;
   for (char* d = _readBufPtr; d != _readBufPtr+repeat; d++) *d = ch;

   // Print the string with characters inserted
   printf("%s", _readBufPtr);

   // Locate the final position of cursor
   char* dummy = _readBufPtr+repeat;

   // Moving pointers
   _readBufEnd += repeat;
   _readBufPtr = _readBufEnd;

   // Move cursor to its final position
   moveBufPtr(dummy);
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine()
{
   // TODO...
   // Status: Finished. Seems.
    moveBufPtr(_readBuf);
    // As _readBufEnd will decrease with deleteChar(),
    // the cursor won't move in deleteChar(),
    // Eventually, we will have _readBufEnd == _readBufPtr == _readBuf
    while(_readBufEnd != _readBuf) deleteChar();
    assert(*_readBufEnd == 0);
}


// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void
CmdParser::moveToHistory(int index)
{
   // TODO...
   // Status: Finished. Seems.
   if (index < _historyIdx)
   {    if (_historyIdx == 0)
        {  mybeep();
           return ;
        }
        else if (_historyIdx == _history.size())
        {   _history.push_back(string(_readBuf));
            _tempCmdStored = true;
        }
        else if (_tempCmdStored && _historyIdx == _history.size()-1)
           _history[_historyIdx] = string(_readBuf);
        if (index < 0) index = 0;
   }
   else
   {    if (_history.empty() || _historyIdx >= _history.size()-1)
        {   mybeep();
            return ;
        }
        if (index >= _history.size()) index = _history.size() -1;
   }
   deleteLine();
   strcpy(_readBuf, _history[index].c_str());
   printf("%s", _readBuf);
   _readBufPtr = _readBufEnd = _readBuf + _history[index].size();
   _historyIdx = index;
}


// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
//
void
CmdParser::addHistory()
{
   // TODO...
   // Status: Need to be test with moveToHistory()
   string readBuf(_readBuf);
   readBuf = trim(readBuf);
   if (_tempCmdStored)
   {    _history.pop_back();
        _tempCmdStored = false;
   }
   if (!readBuf.empty()) _history.push_back(readBuf);
   _historyIdx = _history.size();
}


// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory()
{
   deleteLine();
   strcpy(_readBuf, _history[_historyIdx].c_str());
   cout << _readBuf;
   _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
}

// This function trim the whitespaces in the beginning
// and the end of a string.
string trim(string s)
{   size_t f = s.find_first_not_of(' ');
    if (f == string::npos) // Nonwhitespace character not found.
        return string("");
    size_t e = s.find_last_not_of(' ');
    return s.substr(f, e-f+1);
}

