#include "StdAfx.h"
#include ".\freerotatepicture.h"
#include "..\resource.h"
#include <IBitmap.h>
#include <Effet.h>

/////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////

CFreeRotatePicture::CFreeRotatePicture(void)
{
	m_ciBitmapTestBitmap = new CIBitmap();
	m_ciBitmapOldBitmap = new CIBitmap();

	m_iAngle = 0;
	m_dAngle = 0.0;
	m_iWidth = 0;
	m_iHeight = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////

CFreeRotatePicture::~CFreeRotatePicture(void)
{
	if(m_ciBitmapTestBitmap != NULL)
		delete m_ciBitmapTestBitmap;

	m_ciBitmapTestBitmap = NULL;

	if(m_ciBitmapOldBitmap != NULL)
		delete m_ciBitmapOldBitmap;

	m_ciBitmapOldBitmap = NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////

void CFreeRotatePicture::Init(CIBitmap * Source)
{
	//Avec ratio
	HBITMAP hBitmap;
	CEffet m_cEffet;
	hBitmap = m_cEffet.GenerateIcone(Source->hBitmap, 300, 300, m_iWidth, m_iHeight, 0, RGB(0,0,0), RGB(0,0,0));
	m_ciBitmapTestBitmap->SetBitmap(hBitmap,false);
	m_ciBitmapOldBitmap->SetBitmap(hBitmap,false);				
	DeleteObject(hBitmap);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CFreeRotatePicture::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SendDlgItemMessage(hDlg, IDC_PICTMATRIX, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapTestBitmap->hBitmap);
	SetDlgItemInt(hDlg,IDC_EDANGLE,m_iAngle,true);
	SendDlgItemMessage(hDlg, IDC_SLANGLE, TBM_SETRANGE, (WPARAM) 0, (LPARAM) MAKELONG(0,360)); 
	SendDlgItemMessage(hDlg, IDC_SLANGLE,TBM_SETPOS, 1, 0);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CFreeRotatePicture::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{
		case IDC_BUTTON1:
			EndDialog(hDlg, LOWORD(0));
			return TRUE;

		case IDOK:
			m_dAngle = SendDlgItemMessage(hDlg, IDC_SLANGLE,TBM_GETPOS, 0, 0);
			//m_cEffet.GetFreeRotatedBitmap(Source,m_dAngle, RGB(0,0,0));
			m_iReturnValue = 1;
			EndDialog(hDlg, LOWORD(1));
			return TRUE;

		case IDCANCEL:
			m_iReturnValue = 0;
			EndDialog(hDlg, LOWORD(0));
			return TRUE;

	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CFreeRotatePicture::OnHScroll(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//Mise à jour des données de l'affichage
    // User moved the slider.
    switch(LOWORD(wParam))
    {
		case TB_PAGEDOWN:
		case SB_THUMBTRACK:
		case TB_PAGEUP:
			{
				CEffet m_cEffet;
				HBITMAP hBitmap;
				HWND hWnd = GetDlgItem(hDlg, IDC_PICTMATRIX);
				//int iOctave;

				m_ciBitmapTestBitmap->SetBitmap(m_ciBitmapOldBitmap->GetPtBitmap(),m_ciBitmapOldBitmap->GetBitmapWidth(),m_ciBitmapOldBitmap->GetBitmapHeigth());

				m_iAngle = SendDlgItemMessage(hDlg, IDC_SLANGLE,TBM_GETPOS, 0, 0);
				m_dAngle = m_iAngle;

				m_cEffet.GetFreeRotatedBitmap(m_ciBitmapTestBitmap,m_dAngle, RGB(0,0,0));
			
				hBitmap = m_cEffet.GenerateIcone(m_ciBitmapTestBitmap->hBitmap, 300, 300, m_iWidth, m_iHeight, 0, RGB(0,0,0), RGB(0,0,0));

				m_ciBitmapTestBitmap->SetBitmap(hBitmap,false);

				m_ciBitmapTestBitmap->SpeedPictureShow(hWnd);

				SetDlgItemInt(hDlg,IDC_EDANGLE,m_iAngle,true);

				DeleteObject(hBitmap);
			}
			break;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CFreeRotatePicture::OnPaint(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SendDlgItemMessage(hDlg, IDC_PICTMATRIX, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapTestBitmap->hBitmap);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CFreeRotatePicture::OnActivate(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!HIWORD(wParam))								// Check Minimization State
	{
		HWND hWnd = GetDlgItem(hDlg, IDC_PICTMATRIX);
		m_ciBitmapTestBitmap->SpeedPictureShow(hWnd);
							// Program Is Active
	}
	return 0;
}
