#include "StdAfx.h"
#include ".\colorboxpicker.h"
#include <ibitmap.h>
#include <dessin.h>
#include "resource.h"

#define IDM_DESSINCOLOR 0x025

CColorBoxPicker::CColorBoxPicker(void)
{
	m_ciBitmapColorMatrix = new CIBitmap();
	m_ciBitmapColorGradient = new CIBitmap();
	m_ciBitmapColorBox = new CIBitmap();
	x = 0;
	y = 0;
	m_iReturnValue = 0;
}

CColorBoxPicker::~CColorBoxPicker(void)
{
	if(m_ciBitmapColorMatrix != NULL)
		delete m_ciBitmapColorMatrix;

	m_ciBitmapColorMatrix = NULL;

	if(m_ciBitmapColorGradient != NULL)
		delete m_ciBitmapColorGradient;

	m_ciBitmapColorGradient = NULL;

	if(m_ciBitmapColorBox != NULL)
		delete m_ciBitmapColorBox;

	m_ciBitmapColorBox = NULL;
}

LRESULT CColorBoxPicker::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_ciBitmapColorMatrix->CreateBitmap(259,259);
	m_ciBitmapColorGradient->CreateBitmap(30,255);
	m_ciBitmapColorBox->CreateBitmap(50,50);

	CColor::RGBToHSB(m_Hsbcolor,m_clValue);

	CColor::CreateHSBMatrixColorGradient(m_ciBitmapColorMatrix,m_Hsbcolor.Hue);

	CColor::CreateHSBColorGradient(m_ciBitmapColorGradient);

	CColor::CreateColorBitmap(m_ciBitmapColorBox,m_clValue);

	HWND hWnd = GetDlgItem(hDlg, IDC_PICTGRADIENT);
	HWND hWnd2 = GetDlgItem(hDlg, IDC_PICTMATRIX);
	RECT rc;
	RECT rc2;
	GetWindowRect(hWnd2,&rc);

	POINT pt = {rc.left,rc.top};

	ScreenToClient(hDlg,&pt);

	SendDlgItemMessage(hDlg, 
		IDC_SLISELECTCOLOR, 
		TBM_SETRANGE, 
		(WPARAM) 0, 
		(LPARAM) MAKELONG(0,360)); 

	HWND hWndSlid = GetDlgItem(hDlg,IDC_SLISELECTCOLOR);
	GetClientRect(hWndSlid,&rc2);

	MoveWindow(hWndSlid,340,40,rc2.right,283,false);

	SendDlgItemMessage(hDlg, IDC_SLISELECTCOLOR,TBM_SETPOS, 1, 360 - m_Hsbcolor.Hue);
	
	DefineDialogItem(hDlg);

	SendDlgItemMessage(hDlg, IDC_PICTMATRIX, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapColorMatrix->hBitmap);
	SendDlgItemMessage(hDlg, IDC_PICTGRADIENT, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapColorGradient->hBitmap);
	SendDlgItemMessage(hDlg, IDC_IMGTEMP, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapColorBox->hBitmap);

	return TRUE;
}

void CColorBoxPicker::DefineDialogItem(HWND hDlg)
{
	SetDlgItemInt(hDlg,IDC_T,m_Hsbcolor.Hue,true);
	SetDlgItemInt(hDlg,IDC_S,m_Hsbcolor.Saturation,true);
	SetDlgItemInt(hDlg,IDC_L,m_Hsbcolor.Brightness,true);
	SetDlgItemInt(hDlg,IDC_R,GetRValue(m_clValue),true);
	SetDlgItemInt(hDlg,IDC_G,GetGValue(m_clValue),true);
	SetDlgItemInt(hDlg,IDC_B,GetBValue(m_clValue),true);

	char temp[20];
	sprintf_s(temp,20,"#%2x%2x%2x",GetRValue(m_clValue),GetGValue(m_clValue),GetBValue(m_clValue));
	SetDlgItemText(hDlg,IDC_EDHEXA,temp);
}

LRESULT CColorBoxPicker::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	wmId    = LOWORD(wParam); 
	wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{
		case IDC_BUTTON1:
			m_iReturnValue = 0;
			EndDialog(hDlg, LOWORD(0));
			return TRUE;

		case IDOK:
			m_iReturnValue = 1;
			EndDialog(hDlg, LOWORD(1));
			return TRUE;

		case IDCANCEL:
			m_iReturnValue = 0;
			EndDialog(hDlg, LOWORD(0));
			return TRUE;

		case IDM_DESSINCOLOR:
			{
				RECT rcTemp2;
				//POINT pt;

				HWND hWnd = GetDlgItem(hDlg, IDC_PICTMATRIX);

				HWND hWnd2 = GetDlgItem(hDlg, IDC_IMGTEMP);
				
				m_ciBitmapColorMatrix->GetRGBColorValue(x,y,m_clValue);
				
				CColor::CreateColorBitmap(m_ciBitmapColorBox,m_clValue);

				m_ciBitmapColorBox->SpeedPictureShow(hWnd2);

				rcTemp2.top = (m_ciBitmapColorMatrix->GetBitmapHeigth() - y) - 4;
				rcTemp2.bottom = rcTemp2.top + 8;

				rcTemp2.left = x - 4;
				rcTemp2.right = rcTemp2.left + 8;

				m_ciBitmapColorMatrix->SpeedPictureShow(hWnd);

				CColor::RGBToHSB(m_Hsbcolor,m_clValue);

				DefineDialogItem(hDlg);

				if(GetRValue(m_clValue) < 128 && GetGValue(m_clValue) < 128 && GetBValue(m_clValue) < 128)
					CDessin::DessinerRectangleVide(GetDC(hWnd),1,rcTemp2,RGB(255,255,255));
				else
					CDessin::DessinerRectangleVide(GetDC(hWnd),1,rcTemp2,RGB(0,0,0));
			}
			break;
	}
	return TRUE;
}

LRESULT CColorBoxPicker::OnActivate(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!HIWORD(wParam))								// Check Minimization State
	{
		HWND hWnd = GetDlgItem(hDlg, IDC_PICTMATRIX);
		HWND hWnd2 = GetDlgItem(hDlg, IDC_IMGTEMP);
		m_ciBitmapColorBox->SpeedPictureShow(hWnd2);
		m_ciBitmapColorMatrix->SpeedPictureShow(hWnd);									// Program Is Active

		SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDM_DESSINCOLOR,0),0);
	}
	return TRUE;
}

LRESULT CColorBoxPicker::OnMouseMove(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(MK_LBUTTON == wParam)
	{
		POINT pt;
		RECT rc2;
		//int xPos, yPos;
		pt.x = GET_X_LPARAM(lParam); 
		pt.y = GET_Y_LPARAM(lParam); 

		HWND hWnd = GetDlgItem(hDlg, IDC_PICTMATRIX);

		GetWindowRect(hWnd,&rc2);

		POINT pt2 = {rc2.left,rc2.top};

		ScreenToClient(hDlg,&pt2);
		
		if(pt.x >= pt2.x + 5 && pt.x <= pt2.x + m_ciBitmapColorMatrix->GetBitmapWidth() - 5)
		{
			if(pt.y >= pt2.y + 4 && pt.y <= pt2.y + m_ciBitmapColorMatrix->GetBitmapHeigth() - 5)
			{
				pt.x -= pt2.x;pt.y -= pt2.y;
				x = pt.x;
				y = m_ciBitmapColorMatrix->GetBitmapHeigth() - pt.y;
				SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDM_DESSINCOLOR,0),0);
					
			}
		}
	}
	return TRUE;
}

LRESULT CColorBoxPicker::OnPaint(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	long lValue = 360 - SendDlgItemMessage(hDlg, IDC_SLISELECTCOLOR,TBM_GETPOS, 0, 0);
	CColor::CreateHSBMatrixColorGradient(m_ciBitmapColorMatrix,lValue);
	SendDlgItemMessage(hDlg, IDC_PICTMATRIX, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapColorMatrix->hBitmap);
	SendDlgItemMessage(hDlg, IDC_PICTGRADIENT, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapColorGradient->hBitmap);
	SendDlgItemMessage(hDlg, IDC_IMGTEMP, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapColorBox->hBitmap);
	return FALSE;
}

LRESULT CColorBoxPicker::OnVScroll(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//Mise à jour des données de l'affichage
    // User moved the slider.
    switch(LOWORD(wParam))
    {
		case TB_PAGEDOWN:
		case SB_THUMBTRACK:
		case TB_PAGEUP:
			{
				long lValue = 360 - SendDlgItemMessage(hDlg, IDC_SLISELECTCOLOR,TBM_GETPOS, 0, 0);
				CColor::CreateHSBMatrixColorGradient(m_ciBitmapColorMatrix,lValue);
				SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDM_DESSINCOLOR,0),0);
			}
			break;
	}
	return TRUE;
}

