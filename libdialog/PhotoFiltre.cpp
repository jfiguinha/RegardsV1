#include "StdAfx.h"
#include "PhotoFiltre.h"
#include ".\colorboxpicker.h"
#include "..\resource.h"
#include <IBitmap.h>
#include <filtre.h>
#include <color.h>

CPhotoFiltre::CPhotoFiltre(HINSTANCE hInstance)
{
	m_ciBitmap = new CIBitmap();
	hBitmap = NULL;
	m_hInstance = hInstance;
	m_iItensity = 25;
}

CPhotoFiltre::~CPhotoFiltre(void)
{
	if(hBitmap != NULL)
		DeleteObject(hBitmap);
	hBitmap = NULL;

	delete m_ciBitmap;
}

COLORREF CPhotoFiltre::GetColorFiltre(int iNumFiltre)
{
	switch(iNumFiltre)
	{
		case 0:
			return RGB(236,138,0);
		case 1:
			return RGB(255,177,19);
		case 2:
			return RGB(0,109,255);
		case 3:
			return RGB(0,181,255);
		case 4:
			return RGB(234,26,26);
		case 5:
			return RGB(243,132,23);
		case 6:
			return RGB(249,227,28);
		case 7:
			return RGB(25,201,25);
		case 8:
			return RGB(29,203,234);
		case 9://Blue
			return RGB(29,53,234);
		case 10://Violet
			return RGB(155,29,234);
		case 11://Magenta
			return RGB(227,24,227);
		case 12://Sepia
			return RGB(172,122,51);
		case 13://Underwater
			return RGB(0,194,177);
	}

	return RGB(0,0,0);
}

void CPhotoFiltre::GetValue(COLORREF &colorRef, int &iIntensity)
{
	colorRef = m_colorref;
	iIntensity = m_iItensity;
}

void CPhotoFiltre::AddToList(HWND hDlg)
{
	LRESULT lResult = SendDlgItemMessage(hDlg, IDC_CMBFILTRE, CB_ADDSTRING, 0, (LPARAM)"Warming Filter (85)");
	lResult = SendDlgItemMessage(hDlg, IDC_CMBFILTRE, CB_ADDSTRING, 0, (LPARAM)"Warming Filter (81)");
	lResult = SendDlgItemMessage(hDlg, IDC_CMBFILTRE, CB_ADDSTRING, 0, (LPARAM)"Cooling Filter (80)");
	lResult = SendDlgItemMessage(hDlg, IDC_CMBFILTRE, CB_ADDSTRING, 0, (LPARAM)"Cooling Filter (82)");	
	lResult = SendDlgItemMessage(hDlg, IDC_CMBFILTRE, CB_ADDSTRING, 0, (LPARAM)"RED");	
	lResult = SendDlgItemMessage(hDlg, IDC_CMBFILTRE, CB_ADDSTRING, 0, (LPARAM)"ORANGE");	
	lResult = SendDlgItemMessage(hDlg, IDC_CMBFILTRE, CB_ADDSTRING, 0, (LPARAM)"YELLOW");	
	lResult = SendDlgItemMessage(hDlg, IDC_CMBFILTRE, CB_ADDSTRING, 0, (LPARAM)"Green");	
	lResult = SendDlgItemMessage(hDlg, IDC_CMBFILTRE, CB_ADDSTRING, 0, (LPARAM)"Cyan");	
	lResult = SendDlgItemMessage(hDlg, IDC_CMBFILTRE, CB_ADDSTRING, 0, (LPARAM)"Blue");	
	lResult = SendDlgItemMessage(hDlg, IDC_CMBFILTRE, CB_ADDSTRING, 0, (LPARAM)"Violet");	
	lResult = SendDlgItemMessage(hDlg, IDC_CMBFILTRE, CB_ADDSTRING, 0, (LPARAM)"Magenta");	
	lResult = SendDlgItemMessage(hDlg, IDC_CMBFILTRE, CB_ADDSTRING, 0, (LPARAM)"Sepia");	
	lResult = SendDlgItemMessage(hDlg, IDC_CMBFILTRE, CB_ADDSTRING, 0, (LPARAM)"Underwater");	
}

LRESULT CPhotoFiltre::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SendDlgItemMessage(hDlg, IDC_SLDDENSITY, TBM_SETRANGE, (WPARAM) 0, (LPARAM) MAKELONG(0,100)); 
	SendDlgItemMessage(hDlg, IDC_SLDDENSITY, TBM_SETPOS, (WPARAM)25, (LPARAM)25); 

	m_ciBitmap->CreateBitmap(50,50);

	m_colorref = GetColorFiltre(0);

	CColor::CreateColorBitmap(m_ciBitmap,m_colorref);

	SendDlgItemMessage(hDlg, IDC_IMGCOLOR, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmap->hBitmap);

	hWndBitmap = GetDlgItem(hDlg, IDC_IMGCOLOR);
	SetClassLong(hWndBitmap,GWL_STYLE,WS_CHILD|WS_VISIBLE|SS_NOTIFY|SS_BITMAP);

	SetDlgItemInt(hDlg,IDC_EDDENSITY,m_iItensity,true);

	SetParent(hWndBitmap,hDlg);

	HWND hWndCombo = GetDlgItem(hDlg, IDC_CMBFILTRE);
	SetParent(hDlg,hWndCombo);

	AddToList(hDlg);

	SendDlgItemMessage(hDlg, IDC_RDFILTRE, BM_SETCHECK, BST_CHECKED, 0);
	int iValue = SendDlgItemMessage(hDlg, IDC_CMBFILTRE, CB_SETCURSEL, 0, 0);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////
LRESULT CPhotoFiltre::CallColorPicker(HWND hDlg,COLORREF &clValue)
{
	bool m_bReturnValue = false;
	CColorBoxPicker * m_cColorBoxPicker = new CColorBoxPicker();
	m_cColorBoxPicker->SetColorValue(clValue);
	m_cColorBoxPicker->CreateDlg(m_hInstance,(LPCTSTR)IDD_DLGCOLOR,hDlg);
	int m_iReturnValue = m_cColorBoxPicker->GetReturnValue();
	
	if(m_iReturnValue == 1)
	{
		clValue = m_cColorBoxPicker->GetColorValue();
		m_bReturnValue = true;
	}
	delete m_cColorBoxPicker;
	return m_bReturnValue;
}

LRESULT CPhotoFiltre::OnDblClick(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	RECT rc2;

	pt.x = GET_X_LPARAM(lParam); 
	pt.y = GET_Y_LPARAM(lParam); 

	HWND hWnd = GetDlgItem(hDlg, IDC_IMGCOLOR);
	GetWindowRect(hWnd,&rc2);
	POINT pt2 = {rc2.left,rc2.top};
		
	ScreenToClient(hDlg,&pt2);

	if(pt.x >= pt2.x && pt.x <= pt2.x + m_ciBitmap->GetBitmapWidth())
	{
		if(pt.y >= pt2.y && pt.y <= pt2.y + m_ciBitmap->GetBitmapHeigth())
		{
			COLORREF m_colorrefTemp = m_colorref;
			if(CallColorPicker(hDlg,m_colorref))
			{
				CColor::CreateColorBitmap(m_ciBitmap,m_colorref);
				SendDlgItemMessage(hDlg, IDC_IMGCOLOR, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmap->hBitmap);
				SendDlgItemMessage(hDlg, IDC_RADIOFILTRE, BM_SETCHECK, BST_CHECKED, 0);
				SendDlgItemMessage(hDlg, IDC_RDFILTRE, BM_SETCHECK, BST_UNCHECKED, 0);
			}
			else
			{
				m_colorref = m_colorrefTemp;
			}
		}
	}

	return 0;
}

LRESULT CPhotoFiltre::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam);

	switch (wmId)
	{
		case IDC_CMBFILTRE:
		{
			switch(wmEvent)
			{
				case CBN_SELCHANGE:
					{
						HWND hWndCombo = GetDlgItem(hDlg, IDC_CMBFILTRE);
						int m_iNumSelect = SendMessage(hWndCombo,CB_GETCURSEL,0,0);
						m_colorref = GetColorFiltre(m_iNumSelect);
						CColor::CreateColorBitmap(m_ciBitmap,m_colorref);
						SendDlgItemMessage(hDlg, IDC_IMGCOLOR, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmap->hBitmap);
						return 0;
					}
			}
		}

		case IDC_RDFILTRE:
			{
				SendDlgItemMessage(hDlg, IDC_RDFILTRE, BM_SETCHECK, BST_CHECKED, 0);
				SendDlgItemMessage(hDlg, IDC_RADIOFILTRE, BM_SETCHECK, BST_UNCHECKED, 0);
				break;
			}

		case IDC_RADIOFILTRE:
			{
				SendDlgItemMessage(hDlg, IDC_RADIOFILTRE, BM_SETCHECK, BST_CHECKED, 0);
				SendDlgItemMessage(hDlg, IDC_RDFILTRE, BM_SETCHECK, BST_UNCHECKED, 0);
				break;
			}

		case IDOK:
			{
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

LRESULT CPhotoFiltre::OnHScroll(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//Mise à jour des données de l'affichage
    // User moved the slider.
    switch(LOWORD(wParam))
    {
		case TB_PAGEDOWN:
		case SB_THUMBTRACK:
		case TB_PAGEUP:
			{
				m_iItensity = SendDlgItemMessage(hDlg, IDC_SLDDENSITY,TBM_GETPOS, 0, 0);
				SetDlgItemInt(hDlg,IDC_EDDENSITY,m_iItensity,true);
			}
			break;
	}

	return FALSE;
}

LRESULT CPhotoFiltre::OnPaint(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SendDlgItemMessage(hDlg, IDC_IMGCOLOR, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmap->hBitmap);
	return 0;
}