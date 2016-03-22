#ifndef base_ExcelParser_h
#define base_ExcelParser_h

#include "base.h"
#include <string>
#include <vector>
#include <map>
#include "LPCValue.h"

NS_CT_BEGIN

class ExcelParser;

class ExcelRow
{
public:
    ExcelRow(ExcelParser* owner);
    ~ExcelRow();

    int           QueryInt(std::string columnName) const;
    std::string   QueryString(std::string columnName) const;
    bool          QueryBool(std::string columnName) const;
    float         QueryFloat(std::string columnName) const;
    void          QueryIntVector(std::string columnName, std::string delimit, std::vector<int>& destVec) const;
    void          QueryFloatVector(std::string columnName, std::string delimit, std::vector<float>& destVec) const;
    void          QueryStringVector(std::string columnName, std::string delimit, std::vector<std::string>& destVec) const;

    bool          Contains(std::string columnName);

    void          Add(LPCValue* v) { mProperties.push_back(v); };

    ExcelRow*     GetNext() { return mNext; };
    void          SetNext(ExcelRow* value) { mNext = value; };
    
    void          SetIntKey(int key) { mIntKey = key; }
    void          SetStrKey(std::string key) { mStrKey = key; }

private:
    ExcelParser*		    mOwner;
    std::vector<LPCValue*>	mProperties;

    ExcelRow*               mNext;
    
    int                     mIntKey;
    std::string             mStrKey;

};

///////////////////////////////////////////////////////// ExcelParser //////////////////////////////////////////////////////////////////////

class ExcelParser
{
public:
    static void         SetPath(std::string path) { BIN_EXCEL_FOLDER = path; };
    static ExcelParser* Get(std::string excelName);
    static ExcelParser* Reload(std::string excelName);
	static void         Remove(std::string excelName);
    
    ExcelRow*           FindByKeyInt(int key);
    ExcelRow*           FindByKeyStr(std::string key);

    std::map<std::string, int>*   Columns();
    std::vector<ExcelRow*>*       Rows();
    
    int                 RowCount();
    ExcelRow*           GetRow(int index);
    std::string         GetName();

private:
    ExcelParser();
public:
    ~ExcelParser();

public:
    static const int    VERSION_BIN = 1;
    static const char*  BIN_EXCEL_EXT;

    static std::string  BIN_EXCEL_FOLDER;

private:
    std::string                              mName;
    // column info{name, index}
    std::map<std::string, int>               mColumns;
    // all rows
    std::vector<ExcelRow*>                   mRows;

    // key point to row
    std::map<int, ExcelRow*>*                mRowKeyIntIndexs;
    std::map<std::string, ExcelRow*>*        mRowKeyStrIndexs;
	
    // all excel
    static std::map<std::string, ExcelParser*>*   mExcelParsers; // vs will crash when it is not point, but mac work well
};

NS_CT_END

#endif //base_ExcelParser_h
