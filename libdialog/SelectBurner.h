

#ifndef __SELECTBURNER__H__
#define __SELECTBURNER__H__

#include "LocalDialog.h"

class CAspiDriver;

class CSelectBurner : public CLocalDialog
{
public:
	CSelectBurner(HINSTANCE hInstance);
	~CSelectBurner(void);

private:

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HINSTANCE m_hInstance;
	char m_szMessage[255];
	char m_szCaption[255];
	CAspiDriver * m_cAspiDriver;
	int iTabIndice[50];

};

#endif