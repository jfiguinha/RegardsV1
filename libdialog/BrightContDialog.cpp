#include "StdAfx.h"
#include ".\brightcontdialog.h"
#include "..\resource.h"
#include <Regardsbitmap.h>
#include <ibitmap.h>
#include <filtre.h>

using namespace Regards::libbitmap;

//////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////
CBrightContDialog::CBrightContDialog(void)
{
	lContrast = 100;
	lBrightness = 100;
	lConstrastOld = 0;
	lBrightnessOld = 0;
	m_iReturnValue = 0;
	m_ciBitmapOld = new CIBitmap();
	m_hWndBitmap = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////
CBrightContDialog::~CBrightContDialog(void)
{
	if(m_ciBitmapOld != NULL)
		delete m_ciBitmapOld;

	m_ciBitmapOld = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////
void CBrightContDialog::SetBitmapSource(CIBitmap * ciBitmapSrc)
{
	m_ciBitmapOld->SetBitmap(ciBitmapSrc->GetPtBitmap(),ciBitmapSrc->GetBitmapWidth(),ciBitmapSrc->GetBitmapHeigth());
	m_ciBitmapSrc = ciBitmapSrc;
}

//////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////
void CBrightContDialog::ExecuteFilterPreview(HWND hWndParent, int m_lNewContrastVal, int m_lNewBrightnessVal)
{
	lConstrastOld = m_lNewContrastVal;
	m_cBitmap->AppliquerFiltre(hWndParent,m_ciBitmapSrc, FILTRE_CONTRAST,m_lNewContrastVal,0,0,0);
	lBrightnessOld = m_lNewBrightnessVal;
	m_cBitmap->AppliquerFiltre(hWndParent,m_ciBitmapSrc, FILTRE_INTENSITY,m_lNewBrightnessVal,0,0,0);
	SendMessage(m_hWndBitmap,WM_ERASEBKGND,0,MAKELPARAM(IDM_FILTREIMAGE_OK,0));
}

//////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////
LRESULT CBrightContDialog::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UpdateWindow(hDlg);

	//hWndBitmap = GetParent(hDlg);

	StringCchPrintf(szContrast,10,"%d",0);
	StringCchPrintf(szBrightness,10,"%d",0);

	//Initialisation des Edit Box

	SendDlgItemMessage(hDlg, IDC_EDBRIGHTNESS, WM_SETTEXT, (WPARAM) 0, (LPARAM) szBrightness); 
	SendDlgItemMessage(hDlg, IDC_EDCONTRAST, WM_SETTEXT, (WPARAM) 0, (LPARAM) szContrast); 

	//Initialisation des Slider BRIGHTNESS

	SendDlgItemMessage(hDlg, IDC_SLIDERBRIGHTNESS, TBM_SETRANGEMAX, (WPARAM) 1, (LPARAM) 100); 
	SendDlgItemMessage(hDlg, IDC_SLIDERBRIGHTNESS, TBM_SETRANGEMIN , (WPARAM) 1, (LPARAM) -100); 
	SendDlgItemMessage(hDlg, IDC_SLIDERBRIGHTNESS, TBM_SETPOS , (WPARAM) 1, (LPARAM) 0); 

	//Initialisation des Slider Contrast
	SendDlgItemMessage(hDlg, IDC_SLIDERCONTRAST, TBM_SETRANGEMAX, (WPARAM) 1, (LPARAM) 100); 
	SendDlgItemMessage(hDlg, IDC_SLIDERCONTRAST, TBM_SETRANGEMIN , (WPARAM) 1, (LPARAM) -100); 
	SendDlgItemMessage(hDlg, IDC_SLIDERCONTRAST, TBM_SETPOS , (WPARAM) 1, (LPARAM) 0); 


	m_preview = false;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////
LRESULT CBrightContDialog::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{
		case IDC_CHECKPREV:
			long m_lNewContrastVal, m_lNewBrightnessVal;
			if(SendDlgItemMessage(hDlg, IDC_CHECKPREV,	BM_GETCHECK, 0, 0) == BST_CHECKED)
			{
				m_preview = true;
				m_lNewContrastVal = SendDlgItemMessage(hDlg, IDC_SLIDERCONTRAST, TBM_GETPOS, 0, 0);
				m_lNewBrightnessVal = SendDlgItemMessage(hDlg, IDC_SLIDERBRIGHTNESS, TBM_GETPOS, 0, 0);
				m_ciBitmapSrc->SetBitmap(m_ciBitmapOld->GetPtBitmap(),m_ciBitmapOld->GetBitmapWidth(),m_ciBitmapOld->GetBitmapHeigth());
				ExecuteFilterPreview(hDlg,m_lNewContrastVal,m_lNewBrightnessVal);
				
			}
			else
				m_preview = false;
			break;


		case IDOK:
			{
				SendDlgItemMessage(hDlg, IDC_EDCONTRAST, WM_GETTEXT, (WPARAM) 10, (LPARAM) szContrast); 
				SendDlgItemMessage(hDlg, IDC_EDBRIGHTNESS, WM_GETTEXT, (WPARAM) 10, (LPARAM) szBrightness); 

				lContrast = atol(szContrast);
				lBrightness = atol(szBrightness);

				if(!m_preview)
				{
					m_ciBitmapSrc->SetBitmap(m_ciBitmapOld->GetPtBitmap(),m_ciBitmapOld->GetBitmapWidth(),m_ciBitmapOld->GetBitmapHeigth());
					ExecuteFilterPreview(hDlg,lContrast,lBrightness);
				}

				m_iReturnValue = 1;

				EndDialog(hDlg, LOWORD(1));
			}
			break;

		case IDCANCEL:
			{
				m_ciBitmapSrc->SetBitmap(m_ciBitmapOld->GetPtBitmap(),m_ciBitmapOld->GetBitmapWidth(),m_ciBitmapOld->GetBitmapHeigth());
				m_iReturnValue = 0;
				EndDialog(hDlg, LOWORD(0));
			}
			break;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////
LRESULT CBrightContDialog::OnHScroll(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//Mise à jour des données de l'affichage
    // User moved the slider.

	HWND hWndBitmap = GetParent(hDlg);

    switch(LOWORD(wParam))
    {
		case TB_PAGEDOWN:
		case SB_THUMBTRACK:
		case TB_PAGEUP:
			{
				long m_lNewContrastVal, m_lNewBrightnessVal;

				m_lNewContrastVal = SendDlgItemMessage(hDlg, IDC_SLIDERCONTRAST, TBM_GETPOS, 0, 0);
				m_lNewBrightnessVal = SendDlgItemMessage(hDlg, IDC_SLIDERBRIGHTNESS, TBM_GETPOS, 0, 0);

				if(lConstrastOld != m_lNewContrastVal)
				{
					StringCchPrintf(szContrast,10,"%d",m_lNewContrastVal);
					SendDlgItemMessage(hDlg, IDC_EDCONTRAST, WM_SETTEXT, (WPARAM) 0, (LPARAM) szContrast); 
					lConstrastOld = m_lNewContrastVal;
				}
				else if(lBrightnessOld != m_lNewBrightnessVal)
				{
					StringCchPrintf(szBrightness,10,"%d",m_lNewBrightnessVal);
					SendDlgItemMessage(hDlg, IDC_EDBRIGHTNESS, WM_SETTEXT, (WPARAM) 0, 	(LPARAM) szBrightness);
					lBrightnessOld = m_lNewBrightnessVal;
				}

				if(m_preview)
				{
					m_ciBitmapSrc->SetBitmap(m_ciBitmapOld->GetPtBitmap(),m_ciBitmapOld->GetBitmapWidth(),m_ciBitmapOld->GetBitmapHeigth());
					ExecuteFilterPreview(hDlg,m_lNewContrastVal,m_lNewBrightnessVal);
				}

			}
		break;
	}
	return FALSE;
}