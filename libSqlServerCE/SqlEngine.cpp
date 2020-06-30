#include "StdAfx.h"
#include "SqlEngine.h"
#include "SqlLib.h"

using namespace LIBSQLSERVERCE;

// Initialisation du singleton à NULL
CSqlLib *CSqlEngine::_singleton = NULL;

CSqlEngine::CSqlEngine(void)
{
}

CSqlEngine::~CSqlEngine(void)
{
}

LPSTR CSqlEngine::UnicodeToAnsi(LPCWSTR s)
{
	if (s==NULL) 
		return NULL;

	int cw=lstrlenW(s);
	if (cw==0) 
	{
		CHAR *psz=new CHAR[1];
		*psz='\0';
		return psz;
	}

	int cc=WideCharToMultiByte(CP_ACP,0,s,cw,NULL,0,NULL,NULL);
	if (cc==0) 
		return NULL;

	CHAR *psz=new CHAR[cc+1];
	cc=WideCharToMultiByte(CP_ACP,0,s,cw,psz,cc,NULL,NULL);
	if (cc==0) 
	{
		delete[] psz;
		return NULL;
	}
	psz[cc]='\0';
	return psz;
}

/*
 * AnsiToUnicode converts the ANSI string pszA to a Unicode string
 * and returns the Unicode string through ppszW. Space for the
 * the converted string is allocated by AnsiToUnicode.
 */ 

HRESULT CSqlEngine::AnsiToUnicode(LPCSTR pszA, LPOLESTR* ppszW)
{

    ULONG cCharacters;
    DWORD dwError;

    // If input is null then just return the same.
    if (NULL == pszA)
    {
        *ppszW = NULL;
        return NOERROR;
    }

    // Determine number of wide characters to be allocated for the
    // Unicode string.
    cCharacters =  strlen(pszA)+1;

    // Use of the OLE allocator is required if the resultant Unicode
    // string will be passed to another COM component and if that
    // component will free it. Otherwise you can use your own allocator.
    *ppszW = (LPOLESTR) CoTaskMemAlloc(cCharacters*2);
    if (NULL == *ppszW)
        return E_OUTOFMEMORY;

    // Covert to Unicode.
    if (0 == MultiByteToWideChar(CP_ACP, 0, pszA, cCharacters,
                  *ppszW, cCharacters))
    {
        dwError = GetLastError();
        CoTaskMemFree(*ppszW);
        *ppszW = NULL;
        return HRESULT_FROM_WIN32(dwError);
    }

    return NOERROR;
}

void CSqlEngine::Initialize(LPCWSTR lpFilename)
{
	if (NULL == _singleton)
	{
		_singleton =  new CSqlLib();
		HRESULT hr = _singleton->InitDatabase(lpFilename);
		if(FAILED(hr))
		{
			delete _singleton;
			_singleton = NULL;
		}
	}
}

// Fonctions de création et destruction du singleton
CSqlLib * CSqlEngine::getInstance()
{
	return _singleton;
}

void CSqlEngine::kill()
{
	if (NULL != _singleton)
	{
		delete _singleton;
		_singleton = NULL;
	}
}
