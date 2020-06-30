

#ifndef __REGISTRYACCESS__H__
#define __REGISTRYACCESS__H__

class CRegistryAccess
{
public:
	CRegistryAccess(void);
	~CRegistryAccess(void);

	int GetKeyValue(HKEY KeyD, const char * Key, const char * SubKey, char * Value);
	int SetKeyValue(HKEY KeyD, const char * Key, const char * SubKey, const char * NewValue);
	int CreateNewKey(LPTSTR sNewKeyName, HKEY lPredefinedKey);

	//Write
	long WriteOrCreateKeyValue(const char * cKeyValue);
	long WriteStringValue(const char * cValue,const char * KeyName);
	long WriteIntValue(int iValue,const char * KeyName);

	//Read
	long OpenKeyValue(const char * cKeyValue);
	int ReadStringValue(char * cValue,int iSize,const char * KeyName);
	int ReadIntValue(const char * KeyName);

	long WriteBinValue(const char * KeyName);

private:

	HKEY  hKey; 
	DWORD  dwDisposition; 

};

#endif