#include "StdAfx.h"
#include ".\resizewindow.h"
#include <Fast2PassScale.h>
#include <ibitmap.h>
#include "resource.h"
#include "parameter.h"

CResizeWindow::CResizeWindow(void)
{
	m_iWidth = 100;
	m_iHeight = 100;
	m_iFiltre = 0;
	m_iReturnValue = 0;
	m_iOriginalWidth = 0;
	m_iOriginalHeight = 0;
	m_cParameter = new CParameter();
}

CResizeWindow::~CResizeWindow(void)
{
	delete m_cParameter;
}


void CResizeWindow::SetFiltreItemString(HWND hDlg,int iFiltre, LPCTSTR lpValue)
{
	LRESULT lResult = SendDlgItemMessage(hDlg, IDC_LSTFILTRE, CB_ADDSTRING , iFiltre, (LPARAM)lpValue);
	m_iNumFiltre++;
	if(lResult != ERROR_SUCCESS)
	{
	}
}

void CResizeWindow::InitDialogText(HWND hDlg)
{
	m_iNumFiltre = 0;
	SetFiltreItemString(hDlg, 0, (LPCTSTR)"Normal");
	SetFiltreItemString(hDlg, 0,(LPCTSTR)"BILINEAR");
	SetFiltreItemString(hDlg, 0, (LPCTSTR)"BOX");
	SetFiltreItemString(hDlg, 0, (LPCTSTR)"CATROM");
	SetFiltreItemString(hDlg, 0, (LPCTSTR)"GAUSSIAN");
	SetFiltreItemString(hDlg, 0, (LPCTSTR)"HAMMING");
	SetFiltreItemString(hDlg, 0, (LPCTSTR)"BICUBIC");
	SetFiltreItemString(hDlg, 0, (LPCTSTR)"BLACKMAN");
	SetFiltreItemString(hDlg, 0, (LPCTSTR)"QUADRATIC");
	SetFiltreItemString(hDlg, 0, (LPCTSTR)"MITCHELL");
	SetFiltreItemString(hDlg, 0, (LPCTSTR)"TRIANGLE");
	SetFiltreItemString(hDlg, 0, (LPCTSTR)"SINC");
	SetFiltreItemString(hDlg, 0, (LPCTSTR)"BESSEL");
	SetFiltreItemString(hDlg, 0, (LPCTSTR)"BLACKMANBESSEL");
	SetFiltreItemString(hDlg, 0, (LPCTSTR)"BLACKMANSINC");
	SetFiltreItemString(hDlg, 0, (LPCTSTR)"LANCZOS");
	SetFiltreItemString(hDlg, 0, (LPCTSTR)"HERMITE");
	SetFiltreItemString(hDlg, 0, (LPCTSTR)"HANNING");
}

int CResizeWindow::IntToFiltre(int iNumPos)
{
	switch(iNumPos)
	{
		case 1: return BILINEARFILTER;
		case 2: return BOXFILTER;
		case 3: return CATROMFILTER;
		case 4: return GAUSSIANFILTER;
		case 5: return HAMMINGFILTER;
		case 6: return CUBICFILTER;
		case 7: return BLACKMANFILTER;
		case 8: return QUADRATICFILTER;
		case 9: return MITCHELLFILTER;
		case 10: return TRIANGLEFILTER;
		case 11: return SINCFILTER;
		case 12: return BESSELFILTER;
		case 13: return BLACKMANBESSELFILTER;
		case 14: return BLACKMANSINCFILTER;
		case 15: return LANCZOSFILTER;
		case 16: return HERMITEFILTER;
		case 17: return HANNINGFILTER;
		default:
			break;
	}
	return 0;
}

LRESULT CResizeWindow::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	
	TCHAR szWidth[10], szHeight[10];
	m_iFiltre = m_cParameter->GetOptionTraitement();

	iModify = 0;

	InitDialogText(hDlg);

	m_iOriginalWidth = m_iWidth = m_ciBitmap->GetBitmapWidth();
	m_iOriginalHeight = m_iHeight = m_ciBitmap->GetBitmapHeigth();

	SendDlgItemMessage(hDlg, IDC_LSTFILTRE, CB_SETCURSEL, m_iFiltre, 0);

	StringCchPrintf(szWidth,10,"%d",m_iWidth);
	StringCchPrintf(szHeight,10,"%d",m_iHeight);

	m_dRapport = (double)m_iWidth / (double)m_iHeight;

	SendDlgItemMessage(hDlg, 
		IDC_LARGEUR, 
		WM_SETTEXT, 
		(WPARAM) 0, 
		(LPARAM) szWidth); 

	SendDlgItemMessage(hDlg, 
		IDC_HAUTEUR, 
		WM_SETTEXT, 
		(WPARAM) 0, 
		(LPARAM) szHeight); 

	SendDlgItemMessage(hDlg, 
		IDC_EDPOURCENT, 
		WM_SETTEXT, 
		(WPARAM) 0, 
		(LPARAM)"100"); 

	iCheckOn = 0;

	

	return TRUE;
}

LRESULT CResizeWindow::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{
		case IDOK:
			{
				TCHAR szWidth[10], szHeight[10];

				SendDlgItemMessage(hDlg, IDC_LARGEUR, WM_GETTEXT, (WPARAM) 10, (LPARAM) szWidth); 
				SendDlgItemMessage(hDlg, IDC_HAUTEUR, WM_GETTEXT, (WPARAM) 10, (LPARAM) szHeight); 

				m_iFiltre = SendDlgItemMessage(hDlg, IDC_LSTFILTRE, CB_GETCURSEL, 0, 0);
				m_cParameter->SetOptionTraitement(m_iFiltre);
				m_iWidth = atoi(szWidth);
				m_iHeight = atoi(szHeight);
				m_iFiltre = IntToFiltre(m_iFiltre);
				m_iReturnValue = 1;
				EndDialog(hDlg, LOWORD(wParam));
			}
			return TRUE;

		case IDCANCEL:
			{
				m_iReturnValue = 0;
				EndDialog(hDlg, LOWORD(wParam));
			}
			return TRUE;

		case IDC_CHECKPROPORTION:
			{
				if(SendDlgItemMessage(hDlg, IDC_CHECKPROPORTION, BM_GETCHECK , (WPARAM)0, (LPARAM)0) == BST_CHECKED)
				{
					iCheckOn = 1;

					switch(iModify)
					{
						case 2:
							m_iWidth = (int)((double)m_dRapport * (double)m_iHeight);
							SetDlgItemInt(hDlg,IDC_LARGEUR,m_iWidth,0);
							break;

						case 1:
							m_iHeight = (int)((double)m_iWidth / (double)m_dRapport);
							SetDlgItemInt(hDlg,IDC_HAUTEUR,m_iHeight,0);
							break;

						default:
							break;
					}
				}
				else
					iCheckOn = 0;
			}
			break;

		case IDC_BTPRC:
			{
				int m_iPourcent = GetDlgItemInt(hDlg, IDC_EDPOURCENT, NULL, FALSE);
				m_iHeight = (int)((double)m_iOriginalHeight * (double)((double)m_iPourcent / (double)100));
				m_iWidth = (int)((double)m_iOriginalWidth * (double)((double)m_iPourcent / (double)100));
				SetDlgItemInt(hDlg,IDC_LARGEUR,m_iWidth,0);
				SetDlgItemInt(hDlg,IDC_HAUTEUR,m_iHeight,0);
				break;
			}

	}

	switch(wmEvent)
	{
		case EN_CHANGE:
			switch(wmId)
			{
				case IDC_LARGEUR:
					{
						iModify = 1;
						if(m_iWidth == GetDlgItemInt(hDlg, IDC_LARGEUR, NULL, FALSE))
							break;

						m_iWidth = GetDlgItemInt(hDlg, IDC_LARGEUR, NULL, FALSE);
						if(iCheckOn)
						{
							m_iHeight = (int)((double)m_iWidth / (double)m_dRapport);
							SetDlgItemInt(hDlg,IDC_HAUTEUR,m_iHeight,0);
						}
					}
					break;

				case IDC_HAUTEUR:
					{
						iModify = 2;
						if(m_iHeight == GetDlgItemInt(hDlg, IDC_HAUTEUR, NULL, FALSE))
							break;

						m_iHeight = GetDlgItemInt(hDlg, IDC_HAUTEUR, NULL, FALSE);
						if(iCheckOn)
						{
							m_iWidth = (int)((double)m_dRapport * (double)m_iHeight);
							SetDlgItemInt(hDlg,IDC_LARGEUR,m_iWidth,0);
						}
					}
					break;
				
			}
			break;	
	}

	return TRUE;
}
