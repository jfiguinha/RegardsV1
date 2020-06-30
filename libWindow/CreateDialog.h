

#ifndef __CREATEDIALOG__H__
#define __CREATEDIALOG__H__

#include "MessageDialog.h"

class CCreateDialog  : public CMessageDialog
{
public:
	CCreateDialog(void);
	~CCreateDialog(void);

	HWND CreateDlg(HINSTANCE hInstance, LPCTSTR lpTemplate, HWND hWndParent);

private:

	static int __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	HWND m_hWnd;
};

#endif