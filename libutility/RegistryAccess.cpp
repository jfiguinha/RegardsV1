#include "stdafx.h"
#include ".\registryaccess.h"

CRegistryAccess::CRegistryAccess(void)
{
	hKey = NULL;
}

CRegistryAccess::~CRegistryAccess(void)
{
	if(hKey != NULL)
		RegCloseKey(hKey);

	hKey = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

int CRegistryAccess::GetKeyValue(HKEY KeyD, const char * Key, const char * SubKey, char * Value)
{

	HKEY hKeyResult;
	long Result;

	Result = RegOpenKeyEx(KeyD, Key, 0, KEY_ALL_ACCESS, &hKeyResult);

	if(Result != ERROR_SUCCESS)
		return 0;

	Result = RegQueryValueEx(hKeyResult, SubKey, 0, (ULONG *)1, (UCHAR *)Value, (ULONG *)strlen(Value));
	if(Result != ERROR_SUCCESS)
		return 0;

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

int CRegistryAccess::CreateNewKey(LPTSTR sNewKeyName, HKEY lPredefinedKey)
{
   HKEY hKey;
   DWORD result;
   
   RegCreateKeyEx(lPredefinedKey, sNewKeyName, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &hKey, &result);
   
   RegCloseKey(hKey);

   hKey = NULL;

   return 0;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

int CRegistryAccess::SetKeyValue(HKEY KeyD, const char * Key, const char * SubKey, const char * NewValue)
{

	HKEY hKey;
	long Result;
	Result = RegOpenKeyEx(KeyD, Key, 0, KEY_ALL_ACCESS, &hKey);

	if(Result != ERROR_SUCCESS)
		return 0;

	Result = RegSetValueEx(hKey, SubKey, 0, 1, (UCHAR*)NewValue, (ULONG)strlen(NewValue));

	if(Result != ERROR_SUCCESS)
	   return 0;

	return 1;

}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

long CRegistryAccess::WriteOrCreateKeyValue(const char * cKeyValue)
{
	return RegCreateKeyEx(HKEY_CURRENT_USER,cKeyValue,	0, NULL, REG_OPTION_NON_VOLATILE, 
		KEY_WRITE, NULL, &hKey, &dwDisposition);
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

long CRegistryAccess::WriteIntValue(int iValue,const char * KeyName)
{
	long m_lReturnValue = -1;
	if(hKey != NULL)
	{
		char  dummy[5];
		StringCchPrintf(dummy,5, "%d", iValue);
		m_lReturnValue = RegSetValueEx(hKey, KeyName, 0, REG_SZ,(unsigned char *)dummy, strlen(dummy));
	}

	return m_lReturnValue;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

long CRegistryAccess::WriteBinValue(const char * KeyName)
{
	long m_lReturnValue = -1;
	if(hKey != NULL)
	{
		char  dummy[5];
		StringCchPrintf(dummy,5, "%d", 0);
		m_lReturnValue = RegSetValueEx(hKey, KeyName, 0, REG_NONE,(unsigned char *)dummy, strlen(dummy));
	}

	return m_lReturnValue;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

long CRegistryAccess::WriteStringValue(const char * cValue,const char * KeyName)
{
	long m_lReturnValue = -1;
	if(hKey != NULL)
		m_lReturnValue = RegSetValueEx(hKey, KeyName, 0, REG_SZ,(unsigned char *)cValue, strlen(cValue));

	return m_lReturnValue;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

long CRegistryAccess::OpenKeyValue(const char * cKeyValue)
{
	return RegOpenKeyEx(HKEY_CURRENT_USER,cKeyValue,0, KEY_READ, &hKey );
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

int CRegistryAccess::ReadStringValue(char * cValue,int iSize,const char * KeyName)
{
	DWORD groesse = 255;
	DWORD art = REG_SZ;
	char puffer[255];
	if(hKey != NULL)
	{
		LRESULT lResult = RegQueryValueEx(hKey, KeyName, 0, &art, (unsigned char *)puffer, &groesse);	
		if(lResult == ERROR_SUCCESS)
		{
			StringCchCopy(cValue,iSize,puffer);
			return 0;
		}
	}
	return -1;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

int CRegistryAccess::ReadIntValue(const char * KeyName)
{
	DWORD groesse = 255;
	DWORD art = REG_SZ;
	char puffer[255];

	if(hKey != NULL)
	{
		//::MessageBox(NULL,KeyName,"infos",0);
		LRESULT lResult = RegQueryValueEx(hKey, KeyName, 0, &art, (unsigned char *)puffer, &groesse);	

		if(lResult == ERROR_SUCCESS)
			return atoi(puffer);
	}

	return -1;
}