#include "StdAfx.h"
#include ".\rgbdialog.h"
#include "resource.h"
#include <IBitmap.h>
#include <effet.h>
#include <Regardsbitmap.h>
using namespace Regards::libbitmap;

CRGBDialog::CRGBDialog(void)
{
	lRed = 0;
	lGreen = 0;
	lBlue = 0;
	m_ciBitmapLast = new CIBitmap();
	m_iAction = 0;
	m_iReturnCode = 0;
}

CRGBDialog::~CRGBDialog(void)
{
}

LRESULT CRGBDialog::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TCHAR szRed[10], szGreen[10], szBlue[10];
	CEffet * m_cEffet = new CEffet();
	UpdateWindow(hDlg);

	StringCchPrintf(szRed,10,"%d",0);
	StringCchPrintf(szGreen,10,"%d",0);
	StringCchPrintf(szBlue,10,"%d",0);
	
	//m_dialogBitmap = (LPDIALOGBITMAP)lParam;
	//hWndBitmap = GetParent(hDlg);

	//Initialisation des Edit Box


	SendDlgItemMessage(hDlg, IDC_SLDRED, TBM_SETPAGESIZE, (WPARAM) 0, (LPARAM) 50);
	SendDlgItemMessage(hDlg, IDC_SLDGREEN, TBM_SETPAGESIZE, (WPARAM) 0, (LPARAM) 50);
	SendDlgItemMessage(hDlg, IDC_SLDBLUE, TBM_SETPAGESIZE, (WPARAM) 0, (LPARAM) 50);

	SendDlgItemMessage(hDlg, IDC_SLDRED, WM_SETTEXT, (WPARAM) 0, (LPARAM) szRed);
	SendDlgItemMessage(hDlg, IDC_SLDGREEN, WM_SETTEXT, (WPARAM) 0, (LPARAM) szGreen); 
	SendDlgItemMessage(hDlg, IDC_SLDBLUE, WM_SETTEXT, (WPARAM) 0, (LPARAM) szBlue); 

	//Initialisation des Slider BRIGHTNESS

	//RED
	SendDlgItemMessage(hDlg, IDC_SLDRED, TBM_SETRANGEMAX, (WPARAM) 1, (LPARAM) 255); 
	SendDlgItemMessage(hDlg, IDC_SLDRED, TBM_SETRANGEMIN, (WPARAM) 1, (LPARAM) -255); 

	//GREEN
	SendDlgItemMessage(hDlg, IDC_SLDGREEN, TBM_SETRANGEMAX, (WPARAM) 1, (LPARAM) 255); 
	SendDlgItemMessage(hDlg, IDC_SLDGREEN, TBM_SETRANGEMIN, (WPARAM) 1, (LPARAM) -255); 

	//BLUE
	SendDlgItemMessage(hDlg, IDC_SLDBLUE, TBM_SETRANGEMAX, (WPARAM) 1, (LPARAM) 255); 
	SendDlgItemMessage(hDlg, IDC_SLDBLUE, TBM_SETRANGEMIN, (WPARAM) 1, (LPARAM) -255); 

	//Taille des bitmaps 200x200

	UINT iWidth = 0;
	UINT iHeight = 0;		

	CIBitmap * m_ciBitmapTemp = new CIBitmap();
	m_ciBitmapTemp->SetBitmap(m_ciBitmap->GetPtBitmap(),m_ciBitmap->GetBitmapWidth(),m_ciBitmap->GetBitmapHeigth());
	hBitmap = m_cEffet->GenerateIcone(m_ciBitmapTemp, 140, 140, iWidth, iHeight, 0, RGB(0,0,0), RGB(0,0,0));

	SendDlgItemMessage(hDlg,IDC_BMBEFORE,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)hBitmap);

	m_ciBitmapLast->SetBitmap(hBitmap, false);

	SendDlgItemMessage(hDlg,IDC_AFTER,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapLast->hBitmap);

	delete m_cEffet;
	delete m_ciBitmapTemp;
	//

	return TRUE;
}

LRESULT CRGBDialog::OnPaint(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	RECT rc;
	HDC hDC = BeginPaint( hDlg, &ps );
	
	SendDlgItemMessage(hDlg,IDC_BMBEFORE,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)hBitmap);
	SendDlgItemMessage(hDlg,IDC_AFTER,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapLast->hBitmap);

	EndPaint(hDlg, &ps);
	ReleaseDC(hDlg,hDC);

	return 0;
}

LRESULT CRGBDialog::OnVScroll(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//Mise à jour des données de l'affichage
    // User moved the slider.
    switch(LOWORD(wParam))
    {
		case TB_PAGEDOWN:
		case SB_THUMBTRACK:
		case TB_PAGEUP:
			{
				lBlue = -(SendDlgItemMessage(hDlg, IDC_SLDRED,TBM_GETPOS, 0, 0));
				lGreen = -(SendDlgItemMessage(hDlg, IDC_SLDGREEN,TBM_GETPOS, 0, 0));
				lRed = -(SendDlgItemMessage(hDlg, IDC_SLDBLUE,TBM_GETPOS, 0, 0));
				//m_ciBitmapLast.EraseData();
				m_ciBitmapLast->SetBitmap(hBitmap, false);
				if(lRed != 0)
					m_cBitmap->AppliquerFiltre(hDlg,m_ciBitmapLast, FILTRE_RGB,lRed,1,1,0);

				if(lGreen != 0)
					m_cBitmap->AppliquerFiltre(hDlg,m_ciBitmapLast, FILTRE_RGB,lGreen,2,1,0);
					
				if(lBlue != 0)
					m_cBitmap->AppliquerFiltre(hDlg,m_ciBitmapLast, FILTRE_RGB,lBlue,3,1,0);

				m_ciBitmapLast->SpeedPictureShow(GetDlgItem(hDlg,IDC_AFTER));
			}
	}
	return FALSE;
}

LRESULT CRGBDialog::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{


		case IDOK:
			{

				char szValue[20];
				char Temp[5];

				szValue[0] = '\0';

				lBlue = -(SendDlgItemMessage(hDlg, IDC_SLDRED,TBM_GETPOS, 0, 0));
				lGreen = -(SendDlgItemMessage(hDlg, IDC_SLDGREEN,TBM_GETPOS, 0, 0));
				lRed = -(SendDlgItemMessage(hDlg, IDC_SLDBLUE,TBM_GETPOS, 0, 0));

				if(lRed != 0)
					m_cBitmap->AppliquerFiltre(hDlg,m_ciBitmap, FILTRE_RGB,lRed,1,1,0);

				if(lGreen != 0)
					m_cBitmap->AppliquerFiltre(hDlg,m_ciBitmap, FILTRE_RGB,lGreen,2,1,0);
					
				if(lBlue != 0)
					m_cBitmap->AppliquerFiltre(hDlg,m_ciBitmap, FILTRE_RGB,lBlue,3,1,0);

				
				DeleteObject(hBitmap);

				//SendMessage(GetParent(hDlg),WM_COMMAND,MAKEWPARAM(IDM_APPLIQUER_RGB,0),(LPARAM)szValue);

				m_iReturnCode = 1;

				EndDialog(hDlg, LOWORD(wParam));

			}

			return TRUE;

		case IDCANCEL:
			{
				DeleteObject(hBitmap);
				EndDialog(hDlg, LOWORD(wParam));
			}
			return TRUE;

	}
	return TRUE;
}
