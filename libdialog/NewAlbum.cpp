#include "StdAfx.h"
#include ".\newalbum.h"
#include "resource.h"

CNewAlbum::CNewAlbum(void)
{
}

CNewAlbum::~CNewAlbum(void)
{
}

LRESULT CNewAlbum::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CNewAlbum::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{
		case IDC_BUTTON1:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;

		case IDOK:
			{
				char Temp[255];
				//Récupération du Nom du nouveau Catalogue
				SendDlgItemMessage(hDlg, IDC_ALBUMNAME, WM_GETTEXT,(WPARAM) 255,(LPARAM) Temp );
				SendMessage(m_hParentWnd,WM_COMMAND,MAKEWPARAM(IDM_ALBUMNAME,0),(LPARAM) Temp);
				EndDialog(hDlg, LOWORD(wParam));
			}
			return TRUE;

		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;

	}

	return 0;
}