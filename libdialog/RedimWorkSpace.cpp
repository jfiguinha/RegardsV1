#include "StdAfx.h"
#include ".\redimworkspace.h"
#include "..\resource.h"
#include "..\objet.h"

CRedimWorkSpace::CRedimWorkSpace(long lWidth, long lHeight)
{
	m_iButtonPush = 5;
	m_lWidthImage = lWidth;
	m_lHeightImage = lHeight;
}

CRedimWorkSpace::~CRedimWorkSpace(void)
{
}

LRESULT CRedimWorkSpace::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TCHAR szWidth[10], szHeight[10];
	StringCchPrintf(szWidth,10,"%d",m_lWidthImage);
	StringCchPrintf(szHeight,10,"%d",m_lHeightImage);
	SendDlgItemMessage(hDlg, IDC_STLARGEUR, WM_SETTEXT, (WPARAM) 0, (LPARAM) szWidth); 
	SendDlgItemMessage(hDlg, IDC_STHAUTEUR, WM_SETTEXT, (WPARAM) 0, (LPARAM) szHeight); 
	SendDlgItemMessage(hDlg, IDC_EDLARGEUR, WM_SETTEXT, (WPARAM) 0, (LPARAM) szWidth); 
	SendDlgItemMessage(hDlg, IDC_EDHAUTEUR, WM_SETTEXT, (WPARAM) 0, (LPARAM) szHeight); 
	return 0;
}

LRESULT CRedimWorkSpace::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{
		case IDC_RDHG:
			m_iButtonPush = 1;
			break;
		case IDC_RDHM:
			m_iButtonPush = 2;
			break;
		case IDC_RDHD:
			m_iButtonPush = 3;
			break;
		case IDC_RDBG:
			m_iButtonPush = 7;
			break;
		case IDC_RDBM:
			m_iButtonPush = 8;
			break;
		case IDC_RDBD:
			m_iButtonPush = 9;
			break;
		case IDC_RDMG:
			m_iButtonPush = 4;
			break;
		case IDC_RDMM:
			m_iButtonPush = 5;
			break;
		case IDC_RDMD:
			m_iButtonPush = 6;
			break;

		case IDOK:
			{
				TCHAR szWidth[10], szHeight[10];

				SendDlgItemMessage(hDlg, IDC_EDLARGEUR, WM_GETTEXT, (WPARAM) 10, (LPARAM) szWidth); 
				SendDlgItemMessage(hDlg, IDC_EDHAUTEUR, WM_GETTEXT, (WPARAM) 10, (LPARAM) szHeight); 

				int iValue = SendDlgItemMessage(hDlg, IDC_LSTFILTRE, CB_GETCURSEL, 0, 0);

				long width = atol(szWidth);
				long height = atol(szHeight);

				if(width > m_lWidthImage)
					width = m_lWidthImage;

				if(height > m_lHeightImage)
					height = m_lHeightImage;

				if(width == 0 || height == 0)
				{
					CObjet::AffichageMessage(IDS_MSGREDIMSPACE,IDS_ERREUR,MB_ICONWARNING);
					//MessageBox(NULL,"Veuillez insérer une valeur supérieur à 0 !","Erreur",MB_ICONWARNING);
					return 0;
				}

				DefineRectangle(m_iButtonPush,m_rect,width,height);

				//m_cEffet.CropBitmap(Source,rc);
				m_iReturnValue = 1;
				EndDialog(hDlg, LOWORD(1));
			}
			return TRUE;

		case IDCANCEL:
			{
				m_iReturnValue = 0;
				EndDialog(hDlg, LOWORD(0));
			}
			return TRUE;
	}

	return 0;
}

void CRedimWorkSpace::DefineRectangle(int iButtonPush, RECT &rc,long width, long height)
{
	switch(iButtonPush)
	{
		case 1:
			//HG
			rc.bottom = height;
			rc.top = 0;
			rc.right = width;
			rc.left = 0;
			break;

		case 2:
			{
				int iMilieu = m_lWidthImage / 2;
				int iMilieu2 = width / 2;
				rc.bottom = height;
				rc.top = 0;
				rc.left = iMilieu - iMilieu2;
				rc.right = rc.left +  width;
			}
			break;

		case 3:
			{
				rc.bottom = height;
				rc.top = 0;
				rc.left = m_lWidthImage - width;
				rc.right = m_lWidthImage;
			}
			break;

		case 4:
			{
				
				int iMilieuH = m_lHeightImage / 2;
				int iMilieuH2 = height / 2;


				rc.bottom = iMilieuH + iMilieuH2;
				rc.top = iMilieuH - iMilieuH2;
				rc.left = 0;
				rc.right = width;
			}
			break;

		case 5:
			{
				int iMilieu = m_lWidthImage / 2;
				int iMilieu2 = width / 2;
				int iMilieuH = m_lHeightImage / 2;
				int iMilieuH2 = height / 2;

				rc.bottom = iMilieuH + iMilieuH2;
				rc.top = iMilieuH - iMilieuH2;
				rc.left = iMilieu - iMilieu2;
				rc.right = rc.left +  width;
			}
			break;

		case 6:
			{
				int iMilieuH = m_lHeightImage / 2;
				int iMilieuH2 = height / 2;

				rc.bottom = iMilieuH + iMilieuH2;
				rc.top = iMilieuH - iMilieuH2;
				rc.left = m_lWidthImage - width;
				rc.right = m_lWidthImage;
			}
			break;

		case 7:
			{
				rc.bottom = m_lHeightImage;
				rc.top = m_lHeightImage - height;
				rc.left = 0;
				rc.right = width;
			}
			break;

		case 8:
			{
				int iMilieu = m_lWidthImage / 2;
				int iMilieu2 = width / 2;

				rc.bottom = m_lHeightImage;
				rc.top = m_lHeightImage - height;
				rc.left = iMilieu - iMilieu2;
				rc.right = rc.left +  width;
			}
			break;

		case 9:
			{
				rc.bottom = m_lHeightImage;
				rc.top = m_lHeightImage - height;
				rc.left = m_lWidthImage - width;
				rc.right = m_lWidthImage;
			}
			break;
	}
}