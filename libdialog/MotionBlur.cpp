#include "StdAfx.h"
#include ".\motionblur.h"
#include "..\resource.h"
#include <IBitmap.h>
#include <Filtre.h>
#include <Effet.h>

CMotionBlur::CMotionBlur(void)
{
	m_ciBitmapTestBitmap = new CIBitmap();
	m_ciBitmapOldBitmap = new CIBitmap();
	m_dradius = 0.0f;
	m_dsigma = 0.0f;
	m_dAngle = 0.0;

	m_iRadius = 0;
	m_iSigma = 0;
	m_iAngle = 0;
}

CMotionBlur::~CMotionBlur(void)
{
	if(m_ciBitmapTestBitmap != NULL)
		delete m_ciBitmapTestBitmap;
	m_ciBitmapTestBitmap = NULL;

	if(m_ciBitmapOldBitmap != NULL)
		delete m_ciBitmapOldBitmap;
	m_ciBitmapOldBitmap = NULL;
}

void CMotionBlur::Init(CIBitmap * Source)
{
	CEffet m_cEffet;
	
	UINT iWidth = 0;
	UINT iHeight = 0;

	HBITMAP hBitmap;
	CIBitmap * m_ciBitmap = new CIBitmap();
	m_ciBitmap->SetBitmap(Source->GetPtBitmap(),Source->GetBitmapWidth(),Source->GetBitmapHeigth());
	hBitmap = m_cEffet.GenerateIcone(m_ciBitmap, 300, 300, iWidth, iHeight, 0, RGB(0,0,0), RGB(0,0,0));	
	m_ciBitmapTestBitmap->SetBitmap(hBitmap,false);
	m_ciBitmapOldBitmap->SetBitmap(hBitmap,false);
	
	DeleteObject(hBitmap);
	delete m_ciBitmap;
}

LRESULT CMotionBlur::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//Avec ratio
	SendDlgItemMessage(hDlg, IDC_PICTMATRIX, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapTestBitmap->hBitmap);

	SetDlgItemInt(hDlg,IDC_EDRADIUS,m_iRadius,true);
	SetDlgItemInt(hDlg,IDC_EDSIGMA,m_iSigma,true);
	SetDlgItemInt(hDlg,IDC_EDANGLE,m_iAngle,true);

	SendDlgItemMessage(hDlg, IDC_SLRADIUS, TBM_SETRANGE, (WPARAM) 0, (LPARAM) MAKELONG(0,100)); 
	SendDlgItemMessage(hDlg, IDC_SLSIGMA, TBM_SETRANGE, (WPARAM) 0, (LPARAM) MAKELONG(0,100)); 
	SendDlgItemMessage(hDlg, IDC_SLANGLE, TBM_SETRANGE, (WPARAM) 0, (LPARAM) MAKELONG(0,360)); 

	SendDlgItemMessage(hDlg, IDC_SLRADIUS,TBM_SETPOS, 1, m_iRadius);
	SendDlgItemMessage(hDlg, IDC_SLSIGMA,TBM_SETPOS, 1, m_iSigma);
	SendDlgItemMessage(hDlg, IDC_SLANGLE,TBM_SETPOS, 1, m_iAngle);
	return 0;
}

LRESULT CMotionBlur::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

LRESULT CMotionBlur::OnHScroll(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//Mise à jour des données de l'affichage
    // User moved the slider.
    switch(LOWORD(wParam))
    {
		case TB_PAGEDOWN:
		case SB_THUMBTRACK:
		case TB_PAGEUP:
			{
				HCURSOR hcurSave = SetCursor(LoadCursor(NULL,IDC_WAIT));

				CFiltre m_cFiltre;
				HWND hWnd = GetDlgItem(hDlg, IDC_PICTMATRIX);
				//int iOctave;

				m_iRadius = SendDlgItemMessage(hDlg, IDC_SLRADIUS,TBM_GETPOS, 0, 0);
				m_iSigma = SendDlgItemMessage(hDlg, IDC_SLSIGMA,TBM_GETPOS, 0, 0);
				m_iAngle = SendDlgItemMessage(hDlg, IDC_SLANGLE,TBM_GETPOS, 0, 0);

				m_dradius = m_iRadius;
				m_dsigma = m_iSigma;
				m_dAngle = m_iAngle;

				m_ciBitmapTestBitmap->SetBitmap(m_ciBitmapOldBitmap->GetPtBitmap(),m_ciBitmapOldBitmap->GetBitmapWidth(),m_ciBitmapOldBitmap->GetBitmapHeigth());

				m_cFiltre.MotionBlur(NULL,m_ciBitmapTestBitmap,m_dradius,m_dsigma,m_dAngle);						

				m_ciBitmapTestBitmap->SpeedPictureShow(hWnd);

				SetDlgItemInt(hDlg,IDC_EDRADIUS,m_iRadius,true);
				SetDlgItemInt(hDlg,IDC_EDSIGMA,m_iSigma,true);
				SetDlgItemInt(hDlg,IDC_EDANGLE,m_iAngle,true);

				SetCursor(hcurSave);


			}
			break;
	}
	return 0;
}

LRESULT CMotionBlur::OnPaint(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SendDlgItemMessage(hDlg, IDC_PICTMATRIX, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapTestBitmap->hBitmap);
	return 0;
}

LRESULT CMotionBlur::OnActivate(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SendDlgItemMessage(hDlg, IDC_PICTMATRIX, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapTestBitmap->hBitmap);
	return 0;
}

