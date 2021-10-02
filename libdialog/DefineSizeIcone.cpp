#include "StdAfx.h"
#include ".\definesizeicone.h"
#include "resource.h"
#include "objet.h"

CDefineSizeIcone::CDefineSizeIcone(void)
{
	m_iLargeur = 70;
	m_iHauteur = 90;
}

CDefineSizeIcone::~CDefineSizeIcone(void)
{
}

LRESULT CDefineSizeIcone::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char clpText[255];
	sprintf_s(clpText,255,"%d",m_iLargeur);
	SendDlgItemMessage(hDlg, IDC_EDLARGEUR, WM_SETTEXT , (WPARAM)0, (LPARAM)clpText);
	sprintf_s(clpText,255,"%d",m_iHauteur);
	SendDlgItemMessage(hDlg, IDC_EDHAUTEUR, WM_SETTEXT , (WPARAM)0, (LPARAM)clpText);
	CObjet::CenterWindow(hDlg);
	return 0;
}
LRESULT CDefineSizeIcone::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	int wmId, wmEvent;
	wmId    = LOWORD(wParam); 
	wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{
		case IDOK:
			{
				char cTemp[255];
				SendDlgItemMessage(hDlg, IDC_EDLARGEUR, WM_GETTEXT, 255, (LPARAM)cTemp);
				m_iLargeur = atoi(cTemp);
				SendDlgItemMessage(hDlg, IDC_EDHAUTEUR, WM_GETTEXT, 255, (LPARAM)cTemp);
				m_iHauteur = atoi(cTemp);
				if(m_iLargeur == 0 || m_iHauteur == 0)
				{
					CObjet::AffichageMessage(IDS_VALEURNONVALIDE,IDS_ERREUR);
					m_iReturnCode = 0;
					EndDialog(hDlg, LOWORD(0));
					break;
				}
				m_iReturnCode = 1;
				EndDialog(hDlg, LOWORD(1));
			}
			return TRUE;

		case IDCANCEL:
			m_iReturnCode = 0;
			EndDialog(hDlg, LOWORD(0));
			return TRUE;

	}

	return 0;
}