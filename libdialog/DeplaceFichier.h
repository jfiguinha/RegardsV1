
#ifndef __DEPLACEFICHIER__H__
#define __DEPLACEFICHIER__H__

#include "../libWindow/LocalDialog.h"

class CDeplaceFile;

class CDeplaceFichier : public CLocalDialog
{
public:
	CDeplaceFichier(void);
	~CDeplaceFichier(void);

	void SetParam(CDeplaceFile * cDeplaceFile)
	{
		m_cDeplaceFile = cDeplaceFile;
	}

	static void CopyDirectory(const char * m_szFilenameSource, const char * m_szFilenameDestination, HWND hWnd);
	static void CopyFileDirectory(const char * m_szFilenameSource, const char * m_szFilenameDestination, HWND hWnd);


private:

	static ULONG __stdcall CopierFichier(LPVOID parametre);
	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	CDeplaceFile * m_cDeplaceFile;
	DWORD dwThreadId; 
	HANDLE hThread;
};

#endif
