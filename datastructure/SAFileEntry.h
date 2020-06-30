

#ifndef __SAFILEENTRY__H__
#define __SAFILEENTRY__H__

#include <string>
#include <vector>
using namespace std;

class CSAFileEntry
{
public:
	CSAFileEntry(void);
	~CSAFileEntry(void);

	void MakeFiletime(unsigned __int64 inTime, _FILETIME &outTime);

	string	m_sName;
	string m_sNameOnly;
	unsigned int attrib;
	time_t time_create;
	time_t time_access;
	time_t time_write;
	unsigned __int64 size;
	
};

typedef std::vector<CSAFileEntry> SAFileVector;


#endif