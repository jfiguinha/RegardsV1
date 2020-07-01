#include "stdafx.h"
#include "SqlResult.h"


using namespace std;
using namespace Regards::Sqlite;

CSqlResult::CSqlResult()
{
	//this->pRes = pRes;
}


CSqlResult::~CSqlResult()
{
}

void CSqlResult::SetStatement(sqlite3_stmt * pRes)
{
	this->pRes = pRes;
	m_iColumnCount = sqlite3_column_count(pRes);
}

/*Result Set Definations*/
int	CSqlResult::GetColumnCount()
{
	return m_iColumnCount;
}

string CSqlResult::NextColumnName(const int &iClmnCount)
{
	if (iClmnCount > m_iColumnCount)
		return "";
	return sqlite3_column_name(pRes, iClmnCount);
}

bool CSqlResult::Next()
{
	return (sqlite3_step(pRes) == SQLITE_ROW) ? true : false;
}

string CSqlResult::ColumnDataText(const int & clmNum)
{
    if (clmNum > m_iColumnCount)
        return "";
    
    const unsigned char * textValue = sqlite3_column_text(pRes, clmNum);
    string utf8 =  reinterpret_cast<const char*>(textValue);
    return utf8;
    
    /*
    string ret;
    
#ifdef __WXMSW__

    const wxChar* text = reinterpret_cast<const wxChar*>(sqlite3_column_text16(pRes, clmNum));
    if (text)
        ret = text;
#else
    const char* text = reinterpret_cast<const char*>(sqlite3_column_text16(pRes, clmNum));
    int len = sqlite3_column_bytes16(pRes, clmNum);
    if (text)
    {
        ret = string(utf16_.cMB2WX(text));
    }
#endif
    
    
    string value;
    value.assign(ret.begin(), ret.end());
    return value;
     */
}

int CSqlResult::ColumnDataInt(const int & clmNum)
{
	if (clmNum > m_iColumnCount)
		return -1;
	return sqlite3_column_int(pRes, clmNum);
}

int CSqlResult::ColumnDataBlobSize(const int & clmNum)
{
	if (clmNum > m_iColumnCount)
		return -1;

	return sqlite3_column_bytes(pRes, clmNum);
}

int CSqlResult::ColumnDataBlob(const int & clmNum, void * & pzBlob,const int & pnBlob)
{
	if (clmNum > m_iColumnCount)
		return -1;
	memcpy(pzBlob, sqlite3_column_blob(pRes, clmNum), pnBlob);

	return 0;
}

const void * CSqlResult::ColumnDataBlob(const int & clmNum)
{
	if (clmNum > m_iColumnCount)
		return nullptr;
	return sqlite3_column_blob(pRes, clmNum);
}
