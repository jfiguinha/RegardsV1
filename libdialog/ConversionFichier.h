
#ifndef __CONVERSIONFICHIER__H__
#define __CONVERSIONFICHIER__H__

#include "LocalDialog.h"

class CConvertData;

class CConversionFichier : public CLocalDialog
{
public:
	CConversionFichier(void);
	~CConversionFichier(void);

	void SetParam(CConvertData * cConvertData);

private:

	static ULONG __stdcall ConvertFichier(LPVOID parametre);

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	CConvertData * m_cConvertData;
	DWORD dwThreadId; 
	HANDLE hThread; 
};

#endif