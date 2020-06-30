// Dialog.h: interface for the CDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_H__05E9D723_6923_4E85_BDDC_7D36AE40C43D__INCLUDED_)
#define AFX_DIALOG_H__05E9D723_6923_4E85_BDDC_7D36AE40C43D__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include "MessageDialog.h"

class CLocalDialog  : public CMessageDialog
{
public:

	CLocalDialog();
	virtual ~CLocalDialog();
	
	int CreateDlg(HINSTANCE hInstance, LPCTSTR lpTemplate, HWND hWndParent);


private:

	static int __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif // !defined(AFX_DIALOG_H__05E9D723_6923_4E85_BDDC_7D36AE40C43D__INCLUDED_)
