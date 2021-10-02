#include "StdAfx.h"
#include ".\cloudsbox.h"
#include ".\colorboxpicker.h"
#include "resource.h"
#include <IBitmap.h>
#include <filtre.h>
#include <color.h>

CCloudsBox::CCloudsBox(HINSTANCE hInstance)
{
	m_ciBitmapTestBitmap = new CIBitmap();
	m_ciBitmapColorBox1 = new CIBitmap();
	m_ciBitmapColorBox2 = new CIBitmap();

	m_RGBcolor1 = RGB(0,0,0);
	m_RGBcolor2 = RGB(255,255,255);

	m_Amplitude = 0.65f;
	m_Frequence = 0.01f;
	m_Octave = 8;

	m_iAmplitude = 65;
	m_iFrequence = 1;

	m_iReturnValue = 0;

	m_hInstance = hInstance;
}

CCloudsBox::~CCloudsBox(void)
{
	if(m_ciBitmapTestBitmap != NULL)
		delete m_ciBitmapTestBitmap;

	m_ciBitmapTestBitmap = NULL;

	if(m_ciBitmapColorBox1 != NULL)
		delete m_ciBitmapColorBox1;

	m_ciBitmapColorBox1 = NULL;

	if(m_ciBitmapColorBox2 != NULL)
		delete m_ciBitmapColorBox2;

	m_ciBitmapColorBox2 = NULL;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////
LRESULT CCloudsBox::CallColorPicker(HWND hDlg,COLORREF &clValue)
{
	CColorBoxPicker * m_cColorBoxPicker = new CColorBoxPicker();
	m_cColorBoxPicker->SetColorValue(clValue);
	m_cColorBoxPicker->CreateDlg(m_hInstance,(LPCTSTR)IDD_DLGCOLOR,hDlg);
	int m_iReturnValue = m_cColorBoxPicker->GetReturnValue();
	
	if(m_iReturnValue == 1)
		clValue = m_cColorBoxPicker->GetColorValue();

	delete m_cColorBoxPicker;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////
LRESULT CCloudsBox::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CFiltre m_cFiltre;

	m_ciBitmapColorBox1->CreateBitmap(50,50);
	m_ciBitmapColorBox2->CreateBitmap(50,50);
	m_ciBitmapTestBitmap->CreateBitmap(250,250);

	CColor::CreateColorBitmap(m_ciBitmapColorBox1,m_RGBcolor1);
	CColor::CreateColorBitmap(m_ciBitmapColorBox2,m_RGBcolor2);

	m_cFiltre.Clouds(NULL,m_ciBitmapTestBitmap,m_RGBcolor1,m_RGBcolor2,m_Frequence,m_Amplitude,m_Octave);

	SendDlgItemMessage(hDlg, IDC_PICTMATRIX, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapTestBitmap->hBitmap);

	SetDlgItemInt(hDlg,IDC_EDFREQUENCE,m_iFrequence,true);
	SetDlgItemInt(hDlg,IDC_EDAMPLITUDE,m_iAmplitude,true);

	SendDlgItemMessage(hDlg, IDC_SLIFREQUENCE, TBM_SETRANGE, (WPARAM) 0, (LPARAM) MAKELONG(0,100)); 
	SendDlgItemMessage(hDlg, IDC_SLIAMPLITUDE, TBM_SETRANGE, (WPARAM) 0, (LPARAM) MAKELONG(0,100)); 

	SendDlgItemMessage(hDlg, IDC_SLIFREQUENCE,TBM_SETPOS, 1, m_iFrequence);
	SendDlgItemMessage(hDlg, IDC_SLIAMPLITUDE,TBM_SETPOS, 1, m_iAmplitude);

	SendDlgItemMessage(hDlg, IDC_IMGCLR1, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapColorBox1->hBitmap);
	SendDlgItemMessage(hDlg, IDC_IMGCLR2, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapColorBox2->hBitmap);		

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////
LRESULT CCloudsBox::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{
		case IDC_BUTTON1:
			m_iReturnValue = 0;
			EndDialog(hDlg, LOWORD(0));
			return TRUE;

		case IDOK:
			{
				//CFiltre m_cFiltre;
				//m_cFiltre.Clouds(NULL,Source,m_RGBcolor1,m_RGBcolor2,m_Frequence,m_Amplitude,m_Octave);	

				m_iReturnValue = 1;
				EndDialog(hDlg, LOWORD(1));
			}
			return TRUE;

		case IDCANCEL:
			m_iReturnValue = 0;
			EndDialog(hDlg, LOWORD(0));
			return TRUE;

	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////
LRESULT CCloudsBox::OnHScroll(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//Mise à jour des données de l'affichage
    // User moved the slider.
    switch(LOWORD(wParam))
    {
		case TB_PAGEDOWN:
		case SB_THUMBTRACK:
		case TB_PAGEUP:
			{
				CFiltre m_cFiltre;
				HWND hWnd = GetDlgItem(hDlg, IDC_PICTMATRIX);
				//int iOctave;

				m_iFrequence = SendDlgItemMessage(hDlg, IDC_SLIFREQUENCE,TBM_GETPOS, 0, 0);
				m_iAmplitude = SendDlgItemMessage(hDlg, IDC_SLIAMPLITUDE,TBM_GETPOS, 0, 0);

				m_Amplitude = m_iAmplitude / 100.0f;
				m_Frequence = m_iFrequence / 100.0f;

				m_cFiltre.Clouds(NULL,m_ciBitmapTestBitmap,m_RGBcolor1,m_RGBcolor2,m_Frequence,m_Amplitude,m_Octave);						

				//m_ciBitmapTestBitmap->SpeedPictureShow(hWnd);

				SendDlgItemMessage(hDlg, IDC_PICTMATRIX, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapTestBitmap->hBitmap);

				SetDlgItemInt(hDlg,IDC_EDFREQUENCE,m_iFrequence,true);
				SetDlgItemInt(hDlg,IDC_EDAMPLITUDE,m_iAmplitude,true);
			}
			break;
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////
LRESULT CCloudsBox::OnPaint(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SendDlgItemMessage(hDlg, IDC_PICTMATRIX, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapTestBitmap->hBitmap);
	SendDlgItemMessage(hDlg, IDC_IMGCLR1, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapColorBox1->hBitmap);
	SendDlgItemMessage(hDlg, IDC_IMGCLR2, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapColorBox2->hBitmap);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////
LRESULT CCloudsBox::OnActivate(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!HIWORD(wParam))								// Check Minimization State
	{
		HWND hWnd = GetDlgItem(hDlg, IDC_PICTMATRIX);
		m_ciBitmapTestBitmap->SpeedPictureShow(hWnd);
							// Program Is Active
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////
LRESULT CCloudsBox::OnDblClick(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	RECT rc2;
	CFiltre m_cFiltre;

	pt.x = GET_X_LPARAM(lParam); 
	pt.y = GET_Y_LPARAM(lParam); 

	HWND hWnd = GetDlgItem(hDlg, IDC_IMGCLR1);
	GetWindowRect(hWnd,&rc2);
	POINT pt2 = {rc2.left,rc2.top};
		
	ScreenToClient(hDlg,&pt2);

	if(pt.x >= pt2.x && pt.x <= pt2.x + m_ciBitmapColorBox1->GetBitmapWidth())
	{
		if(pt.y >= pt2.y && pt.y <= pt2.y + m_ciBitmapColorBox1->GetBitmapHeigth())
		{
			if(CallColorPicker(hDlg,m_RGBcolor1))
			{
				CColor::CreateColorBitmap(m_ciBitmapColorBox1,m_RGBcolor1);
				SendDlgItemMessage(hDlg, IDC_IMGCLR1, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapColorBox1->hBitmap);
				hWnd = GetDlgItem(hDlg, IDC_PICTMATRIX);
			}

			m_cFiltre.Clouds(NULL,m_ciBitmapTestBitmap,m_RGBcolor1,m_RGBcolor2,m_Frequence,m_Amplitude,m_Octave);		
			SendDlgItemMessage(hDlg, IDC_PICTMATRIX, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapTestBitmap->hBitmap);
		}
		else
		{

			hWnd = GetDlgItem(hDlg, IDC_IMGCLR2);
			GetWindowRect(hWnd,&rc2);
			POINT pt2 = {rc2.left,rc2.top};
				
			ScreenToClient(hDlg,&pt2);

			if(pt.y >= pt2.y && pt.y <= pt2.y + m_ciBitmapColorBox2->GetBitmapHeigth())
			{

				if(CallColorPicker(hDlg,m_RGBcolor2))
				{
					CColor::CreateColorBitmap(m_ciBitmapColorBox2,m_RGBcolor2);

					SendDlgItemMessage(hDlg, IDC_IMGCLR2, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapColorBox2->hBitmap);

					hWnd = GetDlgItem(hDlg, IDC_PICTMATRIX);
				}

				m_cFiltre.Clouds(NULL,m_ciBitmapTestBitmap,m_RGBcolor1,m_RGBcolor2,m_Frequence,m_Amplitude,m_Octave);
				
				SendDlgItemMessage(hDlg, IDC_PICTMATRIX, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapTestBitmap->hBitmap);	
			}
		}
	}

	return FALSE;
}