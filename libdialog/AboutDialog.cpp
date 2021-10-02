#include <stdafx.h>
#include "AboutDialog.h"
#include "resource.h"
#include "objet.h"

//////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////
LRESULT CAboutDialog::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char clpText[255];
	LoadString(m_hLocalInstance,IDS_VERSION,clpText,255);
	SendDlgItemMessage(hDlg, IDC_STTEXTVERSION, WM_SETTEXT , (WPARAM)0, (LPARAM)clpText);
	LoadString(m_hLocalInstance,IDS_AUTEUR,clpText,255);
	SendDlgItemMessage(hDlg, IDC_STTEXTAUTEUR, WM_SETTEXT , (WPARAM)0, (LPARAM)clpText);
	LoadString(m_hLocalInstance,IDS_COPYRIGHT,clpText,255);
	SendDlgItemMessage(hDlg, IDC_STTEXTCOPYRIGHT, WM_SETTEXT , (WPARAM)0, (LPARAM)clpText);
	CObjet::CenterWindow(hDlg);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////
LRESULT CAboutDialog::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	wmId    = LOWORD(wParam); 
	wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{
		case IDC_BUTTON1:
			EndDialog(hDlg, LOWORD(0));
			return TRUE;

		case IDOK:
			EndDialog(hDlg, LOWORD(1));
			return TRUE;

		case IDCANCEL:
			EndDialog(hDlg, LOWORD(0));
			return TRUE;

	}
	return S_OK;
}