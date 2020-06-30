#include "StdAfx.h"
#include ".\lensflare.h"
#include ".\ColorBoxPicker.h"
#include <effet.h>
#include <filtre.h>
#include <color.h>
#include ".\lensflaredata.h"
#include "..\resource.h"
#include <ibitmap.h>

CLensFlare::CLensFlare(HINSTANCE hInstance)
{
	m_hInstance = hInstance;
	iIntensity = 25;
	m_iReturnValue = 0;
	iColorIntensity = 30;
	m_cFiltre = new CFiltre();
	m_cEffet = new CEffet();
	m_cLensFlareData = new CLensFlareData();
	CIBitmap * Source = NULL;
}

CLensFlare::~CLensFlare(void)
{
	if(m_cFiltre != NULL)
		delete m_cFiltre;

	m_cFiltre = NULL;

	if(m_cEffet != NULL)
		delete m_cEffet;

	m_cEffet = NULL;

	if(m_cLensFlareData != NULL)
		delete m_cLensFlareData;

	m_cLensFlareData = NULL;
}

////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////
void CLensFlare::GenerateIcon()
{
	//Avec ratio
	HBITMAP hBitmap;
	CIBitmap * m_ciBitmap = new CIBitmap();
	m_ciBitmap->SetBitmap(m_Source->GetPtBitmap(),m_Source->GetBitmapWidth(),m_Source->GetBitmapHeigth());
	hBitmap = m_cEffet->GenerateIcone(m_ciBitmap, 400, 300, m_cLensFlareData->iWidth, m_cLensFlareData->iHeight, 0, RGB(0,0,0), RGB(0,0,0));
	m_cLensFlareData->m_ciBitmapTempBitmap->SetBitmap(hBitmap,false);
	m_cLensFlareData->m_ciBitmapTestBitmap->SetBitmap(hBitmap,false);
	DeleteObject(hBitmap);
	delete m_ciBitmap;
}

////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////
LRESULT CLensFlare::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_cLensFlareData->iRayon = 100;
	m_cLensFlareData->x = 100;
	m_cLensFlareData->y = 100;
	m_cLensFlareData->iWidth = 0;
	m_cLensFlareData->iHeight = 0;
	m_cLensFlareData->iXMin = 0;
	m_cLensFlareData->iYMin = 0;

	m_cLensFlareData->m_ciBitmapColorBox->CreateBitmap(50,50);

	//Copy 2x
	//Transformation de l'image

	if(m_Source->GetBitmapHeigth() > m_Source->GetBitmapWidth())
		m_cLensFlareData->iRayon = m_Source->GetBitmapHeigth() / 10;
	else
		m_cLensFlareData->iRayon = m_Source->GetBitmapWidth() / 10;

	GenerateIcon();

	if(m_cLensFlareData->iWidth > m_cLensFlareData->iHeight)
		m_cLensFlareData->fRapport = (float)m_cLensFlareData->iWidth / (float)m_Source->GetBitmapWidth();
	else
		m_cLensFlareData->fRapport = (float)m_cLensFlareData->iHeight / (float)m_Source->GetBitmapHeigth();

	m_cLensFlareData->iTailleRayon = m_cLensFlareData->iRayon * m_cLensFlareData->fRapport;

	m_cLensFlareData->iXMin = (400 - m_cLensFlareData->iWidth);
	m_cLensFlareData->iYMin = (300 - m_cLensFlareData->iHeight);

	m_cLensFlareData->x = m_cLensFlareData->iXMin + m_cLensFlareData->iRayon;
	m_cLensFlareData->y = m_cLensFlareData->iYMin + m_cLensFlareData->iRayon;

	m_cFiltre->LensFlare(NULL,m_cLensFlareData->m_ciBitmapTestBitmap, m_cLensFlareData->iWidth, m_cLensFlareData->iHeight,m_cLensFlareData->x,m_cLensFlareData->iHeight - m_cLensFlareData->y,m_cLensFlareData->iTailleRayon,0,iIntensity,m_cLensFlareData->m_HsbColor.Hue,iColorIntensity);

	SendDlgItemMessage(hDlg, IDC_PICTMATRIX, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_cLensFlareData->m_ciBitmapTestBitmap->hBitmap);

	SetDlgItemInt(hDlg,IDC_EDITX,m_cLensFlareData->x,true);
	SetDlgItemInt(hDlg,IDC_EDITY,m_cLensFlareData->y,true);
	SetDlgItemInt(hDlg,IDC_EDRAYON,iIntensity,true);
	SetDlgItemInt(hDlg,IDC_EDINTENSITE,iColorIntensity,true);

	SendDlgItemMessage(hDlg, IDC_SLISELECTCOLOR, TBM_SETRANGE, (WPARAM) 0, (LPARAM) MAKELONG(0,100)); 
	SendDlgItemMessage(hDlg, IDC_SLISELECTINTENSITE, TBM_SETRANGE, (WPARAM) 0, (LPARAM) MAKELONG(0,100)); 

	COLORREF m_RGBcolor = RGB(255,0,0);

	CColor::RGBToHSB(m_cLensFlareData->m_HsbColor,m_RGBcolor);

	SendDlgItemMessage(hDlg, IDC_SLISELECTCOLOR,TBM_SETPOS, 1, iIntensity);

	SendDlgItemMessage(hDlg, IDC_SLISELECTINTENSITE,TBM_SETPOS, 1, iColorIntensity);

	CColor::CreateColorBitmap(m_cLensFlareData->m_ciBitmapColorBox,RGB(255,0,0));
	SendDlgItemMessage(hDlg, IDC_IMGTEMP, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_cLensFlareData->m_ciBitmapColorBox->hBitmap);
	return TRUE;
}

////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////
LRESULT CLensFlare::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{
		case IDC_BUTTON1:
			EndDialog(hDlg, LOWORD(0));
			m_iReturnValue = 0;
			return TRUE;

		case IDOK:
			{
				iIntensity = SendDlgItemMessage(hDlg, IDC_SLISELECTCOLOR,TBM_GETPOS, 0, 0);
				iColorIntensity = SendDlgItemMessage(hDlg, IDC_SLISELECTINTENSITE,TBM_GETPOS, 0, 0);
				m_cLensFlareData->x -= m_cLensFlareData->iXMin;
				//m_lpLensFlareData->y -= m_lpLensFlareData->iYMin;
				m_cLensFlareData->x *= (float)m_Source->GetBitmapWidth() / (float)(m_cLensFlareData->iWidth - m_cLensFlareData->iXMin);//((float)x / (float)m_ciBitmapTestBitmap->GetBitmapWidth()) * Source->GetBitmapWidth();
				m_cLensFlareData->y *= (float)m_Source->GetBitmapHeigth() / (float)(m_cLensFlareData->iHeight - m_cLensFlareData->iYMin);//((float)(m_ciBitmapTempBitmap->GetBitmapHeigth() - y) / (float)m_ciBitmapTestBitmap->GetBitmapHeigth()) * Source->GetBitmapHeigth();
				m_cFiltre->LensFlare(NULL,m_Source,m_cLensFlareData->x,m_Source->GetBitmapHeigth() - m_cLensFlareData->y,m_cLensFlareData->iRayon,1,iIntensity,m_cLensFlareData->m_HsbColor.Hue,iColorIntensity);
				EndDialog(hDlg, LOWORD(1));
				m_iReturnValue = 1;
			}
			return TRUE;

		case IDCANCEL:
			m_iReturnValue = 0;
			EndDialog(hDlg, LOWORD(0));
			return TRUE;

	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////
LRESULT CLensFlare::OnHScroll(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//Mise à jour des données de l'affichage
    // User moved the slider.
    switch(LOWORD(wParam))
    {
		case TB_PAGEDOWN:
		case SB_THUMBTRACK:
		case TB_PAGEUP:
			{
				HWND hWnd = GetDlgItem(hDlg, IDC_PICTMATRIX);

				iIntensity = SendDlgItemMessage(hDlg, IDC_SLISELECTCOLOR,TBM_GETPOS, 0, 0);

				iColorIntensity = SendDlgItemMessage(hDlg, IDC_SLISELECTINTENSITE,TBM_GETPOS, 0, 0);

				if(iIntensity <= 2)
					iIntensity = 3;				

				m_cFiltre->LensFlare(NULL,m_cLensFlareData->m_ciBitmapTestBitmap, m_cLensFlareData->iWidth, m_cLensFlareData->iHeight,m_cLensFlareData->x,m_cLensFlareData->m_ciBitmapTempBitmap->GetBitmapHeigth() - m_cLensFlareData->y,m_cLensFlareData->iTailleRayon,0,iIntensity,m_cLensFlareData->m_HsbColor.Hue,iColorIntensity);
				
				m_cLensFlareData->m_ciBitmapTestBitmap->SpeedPictureShow(hWnd);

				m_cLensFlareData->m_ciBitmapTestBitmap->SetBitmap(m_cLensFlareData->m_ciBitmapTempBitmap->hBitmap,false);

				SetDlgItemInt(hDlg,IDC_EDINTENSITE,iColorIntensity,true);

				SetDlgItemInt(hDlg,IDC_EDRAYON,iIntensity,true);

			}
			break;
	}
	return TRUE;
}


////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////
LRESULT CLensFlare::OnMouseMove(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(MK_LBUTTON == wParam)
	{
		iIntensity = SendDlgItemMessage(hDlg, IDC_SLISELECTCOLOR,TBM_GETPOS, 0, 0);
		iColorIntensity = SendDlgItemMessage(hDlg, IDC_SLISELECTINTENSITE,TBM_GETPOS, 0, 0);

		POINT pt;
		RECT rc2;
		//int xPos, yPos;
		pt.x = GET_X_LPARAM(lParam); 
		pt.y = GET_Y_LPARAM(lParam); 

		HWND hWnd = GetDlgItem(hDlg, IDC_PICTMATRIX);

		GetWindowRect(hWnd,&rc2);

		POINT pt2 = {rc2.left,rc2.top};

		ScreenToClient(hDlg,&pt2);
		
		if(pt.x >= pt2.x + m_cLensFlareData->iXMin && pt.x <= pt2.x + m_cLensFlareData->iWidth)
		{
			if(pt.y >= pt2.y + m_cLensFlareData->iYMin && pt.y <= pt2.y + m_cLensFlareData->iHeight)
			{
				m_cLensFlareData->x = pt.x;
				m_cLensFlareData->y = pt.y -  142 - m_cLensFlareData->iYMin;

				SetDlgItemInt(hDlg,IDC_EDITX,m_cLensFlareData->x,true);
				SetDlgItemInt(hDlg,IDC_EDITY,m_cLensFlareData->y,true);

				m_cFiltre->LensFlare(NULL,m_cLensFlareData->m_ciBitmapTestBitmap, m_cLensFlareData->iWidth, m_cLensFlareData->iHeight,m_cLensFlareData->x,m_cLensFlareData->iHeight - m_cLensFlareData->y,m_cLensFlareData->iTailleRayon,0,iIntensity,m_cLensFlareData->m_HsbColor.Hue,iColorIntensity);
				m_cLensFlareData->m_ciBitmapTestBitmap->SpeedPictureShow(hWnd);
				m_cLensFlareData->m_ciBitmapTestBitmap->SetBitmap(m_cLensFlareData->m_ciBitmapTempBitmap->hBitmap,false);

			}
		}
	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////
LRESULT CLensFlare::OnPaint(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	iIntensity = SendDlgItemMessage(hDlg, IDC_SLISELECTCOLOR,TBM_GETPOS, 0, 0);
	iColorIntensity = SendDlgItemMessage(hDlg, IDC_SLISELECTINTENSITE,TBM_GETPOS, 0, 0);
	m_cFiltre->LensFlare(NULL,m_cLensFlareData->m_ciBitmapTestBitmap, m_cLensFlareData->iWidth, m_cLensFlareData->iHeight,m_cLensFlareData->x,m_cLensFlareData->iHeight - m_cLensFlareData->y,m_cLensFlareData->iTailleRayon,0,iIntensity,m_cLensFlareData->m_HsbColor.Hue,iColorIntensity);
	SendDlgItemMessage(hDlg, IDC_PICTMATRIX, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_cLensFlareData->m_ciBitmapTestBitmap->hBitmap);

	return FALSE;
}

////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////
LRESULT CLensFlare::OnDblClick(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	iIntensity = SendDlgItemMessage(hDlg, IDC_SLISELECTCOLOR,TBM_GETPOS, 0, 0);
	iColorIntensity = SendDlgItemMessage(hDlg, IDC_SLISELECTINTENSITE,TBM_GETPOS, 0, 0);
	
	POINT pt;
	RECT rc2;
	//char Temp[255];

	pt.x = GET_X_LPARAM(lParam); 
	pt.y = GET_Y_LPARAM(lParam); 

	//int xPos, yPos;

	HWND hWnd = GetDlgItem(hDlg, IDC_IMGTEMP);
	GetWindowRect(hWnd,&rc2);
	POINT pt2 = {rc2.left,rc2.top};
		
	ScreenToClient(hDlg,&pt2);

	if(pt.x >= pt2.x && pt.x <= pt2.x + m_cLensFlareData->m_ciBitmapColorBox->GetBitmapWidth())
	{
		if(pt.y >= pt2.y && pt.y <= pt2.y + m_cLensFlareData->m_ciBitmapColorBox->GetBitmapHeigth())
		{
			CFiltre m_cFiltre;
			COLORREF m_clValue;

			CColor::HSBToRGB(m_cLensFlareData->m_HsbColor,m_clValue);
			
			m_clValue = CallColorPicker(hDlg,m_clValue);

			CColor::CreateColorBitmap(m_cLensFlareData->m_ciBitmapColorBox,m_clValue);

			SendDlgItemMessage(hDlg, IDC_IMGTEMP, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_cLensFlareData->m_ciBitmapColorBox->hBitmap);

			CColor::RGBToHSB(m_cLensFlareData->m_HsbColor,m_clValue);

			hWnd = GetDlgItem(hDlg, IDC_PICTMATRIX);

			ApplyFilter(hDlg);
		}
	}
	return TRUE;
}

void CLensFlare::ApplyFilter(HWND hDlg)
{
	m_cLensFlareData->m_ciBitmapTestBitmap->SetBitmap(m_cLensFlareData->m_ciBitmapTempBitmap->hBitmap,false);
	m_cFiltre->LensFlare(NULL,m_cLensFlareData->m_ciBitmapTestBitmap, m_cLensFlareData->iWidth, m_cLensFlareData->iHeight,m_cLensFlareData->x,m_cLensFlareData->m_ciBitmapTempBitmap->GetBitmapHeigth() - m_cLensFlareData->y,m_cLensFlareData->iTailleRayon,0,iIntensity,m_cLensFlareData->m_HsbColor.Hue,iColorIntensity);		
	SendDlgItemMessage(hDlg, IDC_PICTMATRIX, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_cLensFlareData->m_ciBitmapTestBitmap->hBitmap);
}

COLORREF CLensFlare::CallColorPicker(HWND hDlg,COLORREF clValue)
{
	CColorBoxPicker * m_cColorBoxPicker = new CColorBoxPicker();
	m_cColorBoxPicker->SetColorValue(clValue);
	m_cColorBoxPicker->CreateDlg(m_hInstance,(LPCTSTR)IDD_DLGCOLOR,hDlg);

	int m_iReturnValue = m_cColorBoxPicker->GetReturnValue();

	if(m_iReturnValue == 1)
		clValue = m_cColorBoxPicker->GetColorValue();

	delete m_cColorBoxPicker;

	return clValue;
}
