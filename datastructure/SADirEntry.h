

#ifndef __CSADirEntry__H__
#define __CSADirEntry__H__

#include <vector>
#include <string>
using namespace std;

class CSADirEntry
{
public:
	CSADirEntry(void);
	~CSADirEntry(void);

	CSADirEntry(const string &s)
	{
		m_sName = s;
	};

	string m_sName;
	string m_sNameOnly;
	unsigned int attrib;
	time_t time_create;
	time_t time_access;
	time_t time_write;
	unsigned __int64 size;
};

typedef std::vector<CSADirEntry> SADirVector;

#endif