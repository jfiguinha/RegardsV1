#include "StdAfx.h"
#include ".\jpegsaving.h"
#include "resource.h"

CJpegSaving::CJpegSaving(void)
{
	m_lQuality = 0;
}

CJpegSaving::~CJpegSaving(void)
{
}

LRESULT CJpegSaving::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	StringCchPrintf(m_szQuality,10,"%d",10);

	//Initialisation des Edit Box

    SendDlgItemMessage(hDlg, IDC_EBQUALITY, WM_SETTEXT, (WPARAM) 0, (LPARAM) "10"); 

	//Initialisation des Slider BRIGHTNESS
    SendDlgItemMessage(hDlg, IDC_SLIDERQUALITY, TBM_SETRANGEMAX, (WPARAM) 1, (LPARAM) 10); 
	SendDlgItemMessage(hDlg, IDC_SLIDERQUALITY, TBM_SETRANGEMIN , (WPARAM) 1, (LPARAM) 1); 
	SendDlgItemMessage(hDlg, IDC_SLIDERQUALITY, TBM_SETPOS , (WPARAM) 1, (LPARAM) 10); 
	return 0;
}

LRESULT CJpegSaving::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{

		case IDOK:
			SendDlgItemMessage(hDlg, IDC_EDCONTRAST, WM_GETTEXT, (WPARAM) 10, (LPARAM) m_szQuality); 
			m_lQuality = atol(m_szQuality);
			m_lQuality *= 10;
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;

		case IDCANCEL:
			m_lQuality = -1;
			EndDialog(hDlg, LOWORD(wParam));
			return FALSE;

	}
	return 0;
}

LRESULT CJpegSaving::OnHScroll(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//Mise à jour des données de l'affichage
    // User moved the slider.
    switch(LOWORD(wParam))
    {
		case TB_PAGEDOWN:
		case SB_THUMBTRACK:
		case TB_PAGEUP:
			long m_lNewQualityVal;
			m_lNewQualityVal = SendDlgItemMessage(hDlg, IDC_SLIDERQUALITY,TBM_GETPOS, 0, 0);
			StringCchPrintf(m_szQuality,10,"%d",m_lNewQualityVal);
			SendDlgItemMessage(hDlg, IDC_EBQUALITY, WM_SETTEXT, (WPARAM) 0, (LPARAM) m_szQuality); 
			break;
	}
	return 0;
}
