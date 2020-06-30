
#ifndef __FILTREDIALOG__H__
#define __FILTREDIALOG__H__

#include "../libWindow/LocalDialog.h"

class CFiltreEffet;

class CFiltreDialog : public CLocalDialog
{
public:
	CFiltreDialog(CFiltreEffet * lpFiltreEffet);
	~CFiltreDialog(void);

	inline int GetReturnValue()
	{
		return m_iReturnValue;
	}

private:

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	static unsigned long __stdcall AppelTraitementDialog(void * param);

	int m_iReturnValue;
	DWORD dwThreadId; 
	HANDLE hThread;
	CFiltreEffet * m_lpFiltreEffet;
};

#endif