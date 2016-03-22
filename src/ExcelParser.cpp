#include "../include/ExcelParser.h"
#include "../include/StreamOp.h"
#include "../include/Util.h"

#include <stdlib.h>

using namespace std;
NS_CT_BEGIN

const char* ExcelParser::BIN_EXCEL_EXT = ".bytes";
string      ExcelParser::BIN_EXCEL_FOLDER = "";
map<string, ExcelParser*>* ExcelParser::mExcelParsers = NULL;


ExcelRow::ExcelRow(ExcelParser* owner)
    : mNext(NULL)
,mIntKey(-1)
{
    mOwner = owner;
    mProperties.reserve(mOwner->Columns()->size());
}

ExcelRow::~ExcelRow()
{
    vector<LPCValue*>::iterator it;
    for (it = mProperties.begin(); it != mProperties.end(); ++it)
    {
        delete *it;
    }
    mProperties.clear();
}

int ExcelRow::QueryInt(string columnName) const
{
	map<string, int>::iterator it = mOwner->Columns()->find(columnName);
	if (it != mOwner->Columns()->end())
	{
		if (mProperties[it->second]->IsInt())
			return mProperties[it->second]->AsInt();
	}
    else
    {
        if (mIntKey >= 0)
            Util::log("QueryInt [%s] [%d] [%s]: not exist!", mOwner->GetName().c_str(), mIntKey, columnName.c_str());
        else
            Util::log("QueryInt [%s] [%s] [%s]: not exist!", mOwner->GetName().c_str(), mStrKey.c_str(), columnName.c_str());
    }
	return 0;
}

string ExcelRow::QueryString(string columnName) const
{
	map<string, int>::iterator it = mOwner->Columns()->find(columnName);
	if (it != mOwner->Columns()->end())
	{
		if (mProperties[it->second]->IsString())
			return mProperties[it->second]->AsString();
    }
    else
    {
        if (mIntKey >= 0)
            Util::log("QueryInt [%s] [%d] [%s]: not exist!", mOwner->GetName().c_str(), mIntKey, columnName.c_str());
        else
            Util::log("QueryInt [%s] [%s] [%s]: not exist!", mOwner->GetName().c_str(), mStrKey.c_str(), columnName.c_str());
    }
    return "";
}

bool ExcelRow::QueryBool(string columnName) const
{
	map<string, int>::iterator it = mOwner->Columns()->find(columnName);
	if (it != mOwner->Columns()->end())
	{
		if (mProperties[it->second]->IsBool())
			return mProperties[it->second]->AsBool();
    }
    else
    {
        if (mIntKey >= 0)
            Util::log("QueryInt [%s] [%d] [%s]: not exist!", mOwner->GetName().c_str(), mIntKey, columnName.c_str());
        else
            Util::log("QueryInt [%s] [%s] [%s]: not exist!", mOwner->GetName().c_str(), mStrKey.c_str(), columnName.c_str());
    }
	return false;
}

float ExcelRow::QueryFloat(string columnName) const
{
	map<string, int>::iterator it = mOwner->Columns()->find(columnName);
	if (it != mOwner->Columns()->end())
	{
		if (mProperties[it->second]->IsFloat())
			return mProperties[it->second]->AsFloat();
    }
    else
    {
        if (mIntKey >= 0)
            Util::log("QueryInt [%s] [%d] [%s]: not exist!", mOwner->GetName().c_str(), mIntKey, columnName.c_str());
        else
            Util::log("QueryInt [%s] [%s] [%s]: not exist!", mOwner->GetName().c_str(), mStrKey.c_str(), columnName.c_str());
    }
	return 0.0f;
}

void ExcelRow::QueryIntVector(string columnName, string delimit, vector<int> & destVec) const
{
    string content = QueryString(columnName);
	if (content.length() == 0)
		return;

    vector<string> vecTemp;
    Util::splitString(content, delimit, vecTemp);

    int size = vecTemp.size();
    for (int i = 0; i < size; i++)
    {
        destVec.push_back(atoi(vecTemp.at(i).c_str()));
    }
}

void ExcelRow::QueryFloatVector(string columnName, string delimit, vector<float> & destVec) const
{
    string content = QueryString(columnName);
	if (content.length() == 0)
		return;

    vector<string> vecTemp;
    Util::splitString(content, delimit, vecTemp);

    int size = vecTemp.size();
    for (int i = 0; i < size; i++)
    {
        destVec.push_back(atof(vecTemp.at(i).c_str()));
    }
}

void ExcelRow::QueryStringVector(string columnName, string delimit, vector<string> & destVec) const
{
    string content = QueryString(columnName);
	if (content.length() == 0)
		return;

    Util::splitString(content, delimit, destVec);
}

bool ExcelRow::Contains(string columnName)
{
	map<string, int>::iterator it = mOwner->Columns()->find(columnName);
	return (it != mOwner->Columns()->end()) ? true : false;
}

///////////////////////////////////////////////////////// ExcelParser //////////////////////////////////////////////////////////////////////

ExcelParser::ExcelParser()
: mRowKeyIntIndexs(0)
, mRowKeyStrIndexs(0)
{
}

ExcelParser::~ExcelParser()
{
    if (mRowKeyIntIndexs)
    {
        mRowKeyIntIndexs->clear();
        delete mRowKeyIntIndexs;
        mRowKeyIntIndexs = 0;
    }
    if (mRowKeyStrIndexs)
    {
        mRowKeyStrIndexs->clear();
        delete mRowKeyStrIndexs;
        mRowKeyStrIndexs = 0;
    }

    typedef vector<ExcelRow*>::iterator erIt;
    erIt end = mRows.end();
    for (erIt i = mRows.begin(); i != end; ++i)
    {
        delete *i;
    }
    mRows.clear();
}

ExcelParser* ExcelParser::Get(string excelName)
{
	if (mExcelParsers == NULL)
	{
		mExcelParsers = new map<string, ExcelParser*>();
	}
    map<string, ExcelParser*>::iterator it = mExcelParsers->find(excelName);
    if (it != mExcelParsers->end())
    {
        return it->second;
    }

    string path = BIN_EXCEL_FOLDER + excelName + BIN_EXCEL_EXT;
    
    unsigned char * buffer = 0;
    unsigned long size = 0;
    
    buffer = Util::getFileData(path.c_str(), &size);

    if (! buffer)
    {
        Util::log("ExcelParser::Get [%s]: not exist!", excelName.c_str());
        return 0;
    }

    MemStream ms(buffer, (int)size);

    // int32 - version
    // short - row count
    // short - column count
    // short - key, key2 column index
    // int32 - column name length, column name, int8 - column type
    int version = StreamOp::ReadInt(&ms);
    if (version != VERSION_BIN)
    {
        return 0;
    }

    bool result = true;
    ExcelParser* pEp = new ExcelParser();

    pEp->mName = excelName;

    int rowCount = StreamOp::ReadInt16(&ms);
    int columnCount = StreamOp::ReadInt16(&ms);

    int keyColIndex = StreamOp::ReadInt16(&ms);
    int keyColIndex2 = StreamOp::ReadInt16(&ms);
                
    int* columnType = new int[columnCount];

    string colName;
    int colType;

    int colIdx = 0;
    ExcelRow* eRowItem = 0;

    int     iReadValue      = 0;
    string  strReadValue    = "";
    float   fReadValue      = 0.0f;
    bool    bReadValue      = false;

    int     iKeyVal = 0;
    string  strKeyVal;

    pEp->mRows.reserve(rowCount);

    for (int i = 0; i < columnCount; i++)
    {
        colName = StreamOp::ReadString(&ms);
        colType = StreamOp::ReadInt8(&ms);

        pEp->mColumns.insert(pair<string, int>(colName, i));
        columnType[i] = colType;
    }

    int keyType = columnType[keyColIndex];
    int keyType2 = 0;
    
    const int INVALID_KEY = 20000;
    if (keyColIndex2 != INVALID_KEY)
    {
        keyType2 = columnType[keyColIndex2];
        
        if (keyType == keyType2)
        {
            Util::log("[%s]: 2 key type is same!", excelName.c_str());
            result = false;
            goto Exit;
        }
    }
    // first key
    if (keyType == 2) // string
    {
        pEp->mRowKeyStrIndexs = new map<string, ExcelRow*>();
    }
    else if (keyType == 1 || keyType == 5 || keyType == 6) // int
    {
        pEp->mRowKeyIntIndexs = new map<int, ExcelRow*>();
    }
    else
    {
        Util::log("[%s]: can not be key, key only can be int or string!", excelName.c_str());
        result = false;
        goto Exit;
    }
    // second key
    if (keyColIndex2 != INVALID_KEY)
    {
        if (keyType2 == 2) // string
        {
            pEp->mRowKeyStrIndexs = new map<string, ExcelRow*>();
        }
        else if (keyType2 == 1 || keyType2 == 5 || keyType2 == 6) // int
        {
            pEp->mRowKeyIntIndexs = new map<int, ExcelRow*>();
        }
        else
        {
            Util::log("[%s]: can not be key, key only can be int or string!", excelName.c_str());
            result = false;
            goto Exit;
        }
    }

    eRowItem = new ExcelRow(pEp);

    while (ms.Pos() < ms.Length())
    {
        switch (columnType[colIdx])
        {
        default:
            break;

        case 1: // int, int32
            iReadValue = StreamOp::ReadInt(&ms);
            eRowItem->Add(LPCValue::Create(iReadValue));
            break;

        case 2: // string
            strReadValue = StreamOp::ReadString(&ms);
            eRowItem->Add(LPCValue::Create(strReadValue));
            break;

        case 3: // bool
            bReadValue = StreamOp::ReadBool(&ms);
            eRowItem->Add(LPCValue::Create(bReadValue));
            break;

        case 4: // float
            fReadValue = StreamOp::ReadFloat(&ms);
            eRowItem->Add(LPCValue::Create(fReadValue));
            break;

        case 5: // int16
            iReadValue = StreamOp::ReadInt16(&ms);
            eRowItem->Add(LPCValue::Create(iReadValue));
            break;

        case 6: // int8
            iReadValue = StreamOp::ReadInt8(&ms);
            eRowItem->Add(LPCValue::Create(iReadValue));
            break;
        }
    
        if (colIdx == keyColIndex)
        {
            if (keyType == 2) // string
            {
                strKeyVal = strReadValue;
            }
            else if (keyType == 1 || keyType == 5 || keyType == 6) // int
            {
                iKeyVal = iReadValue;
            }
        }
        
        if (keyColIndex2 != INVALID_KEY)
        {
            if (colIdx == keyColIndex2)
            {
                if (keyType2 == 2) // string
                {
                    strKeyVal = strReadValue;
                }
                else if (keyType2 == 1 || keyType2 == 5 || keyType2 == 6) // int
                {
                    iKeyVal = iReadValue;
                }
            }
        }

        colIdx++;
        if (colIdx >= columnCount)
        {
            colIdx = 0;

            if (keyType == 2) // string
            {
                map<string, ExcelRow*>::iterator it = pEp->mRowKeyStrIndexs->find(strKeyVal);
                if (it != pEp->mRowKeyStrIndexs->end())
                {
                    //Util::log("key %s already exist！", strKeyVal.c_str());
                    //goto Exit;
                    ExcelRow* pNext = it->second;
                    while (pNext->GetNext() != NULL)
                    {
                        pNext = pNext->GetNext();
                    }
                    pNext->SetNext(eRowItem);
                    eRowItem->SetNext(NULL);
                }
                else
                {
                    eRowItem->SetStrKey(strKeyVal);
                    pEp->mRowKeyStrIndexs->insert(pair<string, ExcelRow*>(strKeyVal, eRowItem));
                }
            }
            else if (keyType == 1 || keyType == 5 || keyType == 6) // int
            {
                map<int, ExcelRow*>::iterator it = pEp->mRowKeyIntIndexs->find(iKeyVal);
                if (it != pEp->mRowKeyIntIndexs->end())
                {
                    //Util::log("key %d already exist！", iKeyVal);
                    //goto Exit;
                    ExcelRow* pNext = it->second;
                    while (pNext->GetNext() != NULL)
                    {
                        pNext = pNext->GetNext();
                    }
                    pNext->SetNext(eRowItem);
                    eRowItem->SetNext(NULL);
                }
                else
                {
                    eRowItem->SetIntKey(iKeyVal);
                    pEp->mRowKeyIntIndexs->insert(pair<int, ExcelRow*>(iKeyVal, eRowItem));
                }
            }
            
            if (keyColIndex2 != INVALID_KEY)
            {
                if (keyType2 == 2) // string
                {
                    map<string, ExcelRow*>::iterator it = pEp->mRowKeyStrIndexs->find(strKeyVal);
                    if (it != pEp->mRowKeyStrIndexs->end())
                    {
                        //Util::log("key %s already exist！", strKeyVal.c_str());
                        //goto Exit;
                        ExcelRow* pNext = it->second;
                        while (pNext->GetNext() != NULL)
                        {
                            pNext = pNext->GetNext();
                        }
                        pNext->SetNext(eRowItem);
                        eRowItem->SetNext(NULL);
                    }
                    else
                    {
                        eRowItem->SetStrKey(strKeyVal);
                        pEp->mRowKeyStrIndexs->insert(pair<string, ExcelRow*>(strKeyVal, eRowItem));
                    }
                }
                else if (keyType2 == 1 || keyType2 == 5 || keyType2 == 6) // int
                {
                    map<int, ExcelRow*>::iterator it = pEp->mRowKeyIntIndexs->find(iKeyVal);
                    if (it != pEp->mRowKeyIntIndexs->end())
                    {
                        //Util::log("key %d already exist！", iKeyVal);
                        //goto Exit;
                        ExcelRow* pNext = it->second;
                        while (pNext->GetNext() != NULL)
                        {
                            pNext = pNext->GetNext();
                        }
                        pNext->SetNext(eRowItem);
                        eRowItem->SetNext(NULL);
                    }
                    else
                    {
                        eRowItem->SetIntKey(iKeyVal);
                        pEp->mRowKeyIntIndexs->insert(pair<int, ExcelRow*>(iKeyVal, eRowItem));
                    }
                }
            }

            pEp->mRows.push_back(eRowItem);
            // next row
            eRowItem = new ExcelRow(pEp);
        }
    }

Exit:
    if (result)
    {
        mExcelParsers->insert(pair<string, ExcelParser*>(excelName, pEp));
    }
	else
    {
        delete pEp;
        pEp = NULL;
    }

    delete[] columnType;
    columnType = 0;

    return pEp;
}

ExcelParser* ExcelParser::Reload(string excelName)
{
    Remove(excelName);

    return Get(excelName);
}

void ExcelParser::Remove(string excelName)
{
    map<string, ExcelParser*>::iterator it = mExcelParsers->find(excelName);
    if (it != mExcelParsers->end())
    {
        delete it->second;
        mExcelParsers->erase(it);
    }
}

ExcelRow* ExcelParser::FindByKeyInt(int key)
{
    if (mRowKeyIntIndexs == 0)
    {
        return 0;
    }

    map<int, ExcelRow*>::iterator it = mRowKeyIntIndexs->find(key);
    if (it != mRowKeyIntIndexs->end())
    {
        return it->second;
    }
    return 0;
}

ExcelRow* ExcelParser::FindByKeyStr(string key)
{
    if (mRowKeyStrIndexs == 0)
    {
        return 0;
    }

    map<string, ExcelRow*>::iterator it = mRowKeyStrIndexs->find(key);
    if (it != mRowKeyStrIndexs->end())
    {
        return it->second;
    }
    return 0;
}

map<string, int>* ExcelParser::Columns()
{
    return &mColumns;
}

vector<ExcelRow*>* ExcelParser::Rows()
{
    return &mRows;
}

int ExcelParser::RowCount()
{
    return mRows.size();
}

ExcelRow* ExcelParser::GetRow(int index)
{
    if (index < 0 || index >= mRows.size())
        return NULL;
    return mRows[index];
}

string ExcelParser::GetName()
{
    return mName;
}

NS_CT_END
