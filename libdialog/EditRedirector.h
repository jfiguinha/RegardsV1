

#ifndef __CEDITREDIRECTOR__H__
#define __CEDITREDIRECTOR__H__

#include "redirector.h"

class CAspiDriver;
class CEdit;
class CParameter;

class CEditRedirector  : public CRedirector 
{
public:

	CEditRedirector();

	virtual ~CEditRedirector();

	void SetParserWndHandle(HWND hWnd)
	{
		hWndDialogBox = hWnd;
	}

	LRESULT OnInitDialog(HWND LocalhWnd);
	void WriteStdError(LPCSTR pszError);
	void WriteStdOut(LPCSTR pszOutput);


	void SignalApplicationEnding();
	string GetTempIsoDirectory();
	string GetTempIsoFile();
	LRESULT CreateIsoFile(HWND hWnd,CAspiDriver * m_cAspiDriver,char * TempIsoFile);
	LRESULT BurnIsoFile(HWND hWnd,char * m_szCommand);
	INT64 GetIsoFileSize();
	INT64 GetIsoFileSize(string m_stgFile);

private:

	HWND hWndEdit;
	HWND hWndDialogBox;
	CEdit * m_pWnd;
	string m_stgValue;
	int iTabIndice[128];
	int iIsoCreating;
	int iIsoWriting;
	DWORD dwThreadId; 
	HANDLE hThread;
	CParameter * m_cParameter;
};

#endif