#ifndef __ABOUTDIALOG__H__
#define __ABOUTDIALOG__H__

#include "../libWindow/LocalDialog.h"

class CAboutDialog : public CLocalDialog
{
public:
	CAboutDialog(HINSTANCE hInstance){m_hLocalInstance = hInstance;}
	~CAboutDialog(){}

private:

	HINSTANCE m_hLocalInstance;

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif