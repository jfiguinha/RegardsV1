
#ifndef __EFFETDIALOG__H__
#define __EFFETDIALOG__H__

#include "LocalDialog.h"

class CFiltreEffet;

class CEffetDialog : public CLocalDialog
{
public:
	CEffetDialog(CFiltreEffet * lpFiltreEffet);
	~CEffetDialog(void);

	inline int GetReturnValue()
	{
		return m_iReturnValue;
	}

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:



	static unsigned long __stdcall AppelTraitementDialog(void * param);

	int m_iReturnValue;
	CFiltreEffet * m_lpFiltreEffet;
	DWORD dwThreadId; 
	HANDLE hThread;
};

#endif