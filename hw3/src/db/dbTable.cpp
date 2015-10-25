/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
 ****************************************************************************/

#include <iomanip>
#include <string>
#include <cctype>
#include <cassert>
#include <set>
#include <algorithm>
#include "dbTable.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
    // TODO: to print out a row.
    // - Data are seperated by a space. No trailing space at the end.
    // - Null cells are printed as '.'
    for (int i = 0; i < r.size(); ++i)
        cout << (i ? " ": "") <<  (r[i] == INT_MAX ? '.': r[i]);
    return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
    // TODO: to print out a table
    // - Data are seperated by setw(6) and aligned right.
    // - Null cells should be left blank (printed as '.').
    for (int i = 0; i < t.nRows(); ++i)
    {    for (int j = 0; j < t.nCols(); ++j)
        {   cout << right << setw(6);
            if (t[i][j] == INT_MAX) cout << '.';
            else cout << t[i][j];
        }
        cout << endl;
    }

    return os;
}

int dbStr2Int(const string& str)
{
    if (str == "") return INT_MAX;
    int i = (str[0] == '-');
    int sign = i ? -1 : 1;
    int ans = 0;
    for (; i < str.size(); ++i)
       ans = ans*10 + str[i] - '0';
    return (ans*sign);
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
    // TODO: to read in data from csv file and store them in a table
    // - You can assume all the data of the table are in a single line.
    t.reset();
    string s, _tmp;
    getline(ifs, s);
    DBRow tmp;
    for (int i = 0; i < s.size(); ++i)
    {    if (s[i] == '\r')
        {   if(s[i-1] == '\r') break;
            tmp.addData(dbStr2Int(_tmp));
            t.addRow(tmp);
            tmp.reset();
            _tmp.clear();
        }
        else if (s[i] == ',')
        {
            tmp.addData(dbStr2Int(_tmp));
            _tmp.clear();
        }
        else _tmp += s[i];
    }
    return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void DBRow::removeCell(size_t c)
{
    // TODO
    _data.erase(_data.begin()+c);
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
    // TODO: called as a functional object that compares the data in r1 and r2
    //       based on the order defined in _sortOrder
    for (int i = 0; i < _sortOrder.size(); ++i)
        if (r1[_sortOrder[i]] < r2[_sortOrder[i]]) return true;
        else if (r1[_sortOrder[i]] > r2[_sortOrder[i]]) return false;

    return false;
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void DBTable::reset()
{
    // TODO
    _table.clear();
}

    void
DBTable::addCol(const vector<int>& d)
{
    // TODO: add a column to the right of the table. Data are in 'd'.
    for (int i = 0; i < _table.size(); ++i) _table[i].addData(d[i]);
}

    void
DBTable::delRow(int c)
{
    // TODO: delete row #c. Note #0 is the first row.
    _table.erase(_table.begin() + c);
}

    void
DBTable::delCol(int c)
{
    // delete col #c. Note #0 is the first row.
    for (size_t i = 0, n = _table.size(); i < n; ++i)
        _table[i].removeCell(c);
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float
DBTable::getMax(size_t c) const
{
    // TODO: get the max data in column #c
    bool flag = 0;
    int ans = INT_MIN;
    for (int i = 0; i < _table.size(); ++i)
        if (_table[i][c] != INT_MAX)
        {
            ans = max(ans, _table[i][c]);
            flag = 1;
        }
    return ( flag ? ans : NAN );
}

float
DBTable::getMin(size_t c) const
{
    // TODO: get the min data in column #c
    bool flag = 0;
    int ans = INT_MAX;
    for (int i = 0; i < _table.size(); ++i)
        if (_table[i][c] != INT_MAX)
        {
            ans = min(ans, _table[i][c]);
            flag = 1;
        }

    return (flag ? ans : NAN);
}

float
DBTable::getSum(size_t c) const
{
    // TODO: compute the sum of data in column #c
    int ans = 0;
    bool flag = 0;
    for (int i = 0; i < _table.size(); ++i)
        if (_table[i][c] != INT_MAX)
        {
            ans += _table[i][c];
            flag = 1;
        }
    return (flag ? ans : NAN);
}

int
DBTable::getCount(size_t c) const
{
    // TODO: compute the number of distinct data in column #c
    // - Ignore null cells
    set<int> temp;
    for (int i = 0; i < _table.size(); ++i)
        if (_table[i][c] != INT_MAX) temp.insert(_table[i][c]);
    return temp.size();
}

float
DBTable::getAve(size_t c) const
{
    // TODO: compute the average of data in column #c
    float ans = 0;
    int cnt = 0;
    for (int i = 0; i < _table.size(); ++i)
        if (_table[i][c] != INT_MAX)
        {
            ans += _table[i][c];
            cnt++;
        }

    return (cnt ? ans/cnt : NAN);
}

void
DBTable::sort(const struct DBSort& s)
{
    // TODO: sort the data according to the order of columns in 's'
    std::sort(_table.begin(), _table.end(), s);
}

void
DBTable::printCol(size_t c) const
{
    // TODO: to print out a column.
    // - Data are seperated by a space. No trailing space at the end.
    // - Null cells are printed as '.'
    for (int i = 0; i < _table.size(); ++i)
    {   cout << (i ? " ": "");
        if (_table[i][c] == INT_MAX) cout << '.';
        else cout <<  _table[i][c];
    }
}

void
DBTable::printSummary() const
{
    size_t nr = nRows(), nc = nCols(), nv = 0;
    for (size_t i = 0; i < nr; ++i)
        for (size_t j = 0; j < nc; ++j)
            if (_table[i][j] != INT_MAX) ++nv;
    cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}

