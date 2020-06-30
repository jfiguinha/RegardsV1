#include "StdAfx.h"
#include "HistorgrammeDialog.h"
#include "../resource.h"
#include <IBitmap.h>
#include <histogramme.h>
#include <color.h>

CHistorgrammeDialog::CHistorgrammeDialog(void)
{
	m_ciBimapPtr = NULL;
	hBitmapMoyenne = NULL;
	m_iSelect = 0;
	hBmpGradientBlack = NULL;
	hBmpGradientBlue = NULL;
	hBmpGradientGreen = NULL;
	hBmpGradientRed = NULL;
}

CHistorgrammeDialog::~CHistorgrammeDialog(void)
{
	if(hBitmapMoyenne != NULL)
		DeleteObject(hBitmapMoyenne);
	hBitmapMoyenne = NULL;

	if(hBitmapRed != NULL)
		DeleteObject(hBitmapRed);
	hBitmapRed = NULL;

	if(hBitmapGreen != NULL)
		DeleteObject(hBitmapGreen);
	hBitmapGreen = NULL;

	if(hBitmapBlue != NULL)
		DeleteObject(hBitmapBlue);
	hBitmapBlue = NULL;

	if(hBmpGradientBlack != NULL)
		DeleteObject(hBmpGradientBlack);
	hBmpGradientBlack = NULL;

	if(hBmpGradientBlue != NULL)
		DeleteObject(hBmpGradientBlue);
	hBmpGradientBlue = NULL;

	if(hBmpGradientGreen != NULL)
		DeleteObject(hBmpGradientGreen);
	hBmpGradientGreen = NULL;

	if(hBmpGradientRed != NULL)
		DeleteObject(hBmpGradientRed);
	hBmpGradientRed = NULL;
}

void CHistorgrammeDialog::AddToList(HWND hDlg)
{
	LRESULT lResult = SendDlgItemMessage(hDlg, IDC_CMBNIVEAU, CB_ADDSTRING, 0, (LPARAM)"General Level");
	lResult = SendDlgItemMessage(hDlg, IDC_CMBNIVEAU, CB_ADDSTRING, 0, (LPARAM)"Red Level");
	lResult = SendDlgItemMessage(hDlg, IDC_CMBNIVEAU, CB_ADDSTRING, 0, (LPARAM)"Green Level");
	lResult = SendDlgItemMessage(hDlg, IDC_CMBNIVEAU, CB_ADDSTRING, 0, (LPARAM)"Blue Level");	
}

void CHistorgrammeDialog::CreateGradient(HWND hDlg, COLORREF colorFrom, COLORREF colorTo, HBITMAP &hBitmapOut)
{
	RECT rcFrame;
	HWND hWndFrame = GetDlgItem(hDlg, IDC_FRMGRADIENT);
	GetClientRect(hWndFrame,&rcFrame);
	CIBitmap * m_ciBitmap = new CIBitmap();
	m_ciBitmap->CreateBitmap(rcFrame.right - rcFrame.left,rcFrame.bottom - rcFrame.top);
	CColor::CreateColorGradient(m_ciBitmap,colorFrom,colorTo);
	hBitmapOut = (HBITMAP)CopyImage(m_ciBitmap->hBitmap, IMAGE_BITMAP, 0, 0, LR_COPYDELETEORG);
	delete m_ciBitmap;
}

LRESULT CHistorgrammeDialog::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CHistogramme * m_cHistogramme = new CHistogramme();
	if(m_ciBimapPtr != NULL)
	{
		m_cHistogramme->SetBitmap(m_ciBimapPtr);
		m_cHistogramme->ParcoursBitmap();
		hBitmapMoyenne = m_cHistogramme->GenerateHistogrammeBitmap(266,166,0);
		hBitmapRed = m_cHistogramme->GenerateHistogrammeBitmap(266,166,1);
		hBitmapGreen = m_cHistogramme->GenerateHistogrammeBitmap(266,166,2);
		hBitmapBlue = m_cHistogramme->GenerateHistogrammeBitmap(266,166,3);
	}

	CreateGradient(hDlg, RGB(0,0,0), RGB(255,255,255), hBmpGradientBlack);
	CreateGradient(hDlg, RGB(0,0,0), RGB(255,0,0), hBmpGradientRed);
	CreateGradient(hDlg, RGB(0,0,0), RGB(0,255,0), hBmpGradientGreen);
	CreateGradient(hDlg, RGB(0,0,0), RGB(0,0,255), hBmpGradientBlue);

	SendDlgItemMessage(hDlg, IDC_IMGHISTOGRAMME, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)hBitmapMoyenne);
	SendDlgItemMessage(hDlg, IDC_IMGGRADIENT, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)hBmpGradientBlack);
	delete m_cHistogramme;

	AddToList(hDlg);
	SendDlgItemMessage(hDlg, IDC_CMBNIVEAU, CB_SETCURSEL, 0, 0);
	return 0;
}

LRESULT CHistorgrammeDialog::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam);

	switch (wmId)
	{
		case IDC_CMBNIVEAU:
		{
			switch(wmEvent)
			{
				case CBN_SELCHANGE:
					{
						HWND hWndCombo = GetDlgItem(hDlg, IDC_CMBNIVEAU);
						m_iSelect = SendMessage(hWndCombo,CB_GETCURSEL,0,0);
						switch(m_iSelect)
						{
						case 0:
							SendDlgItemMessage(hDlg, IDC_IMGHISTOGRAMME, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)hBitmapMoyenne);
							SendDlgItemMessage(hDlg, IDC_IMGGRADIENT, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)hBmpGradientBlack);
							break;
						case 1:
							SendDlgItemMessage(hDlg, IDC_IMGHISTOGRAMME, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)hBitmapRed);
							SendDlgItemMessage(hDlg, IDC_IMGGRADIENT, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)hBmpGradientRed);
							break;
						case 2:
							SendDlgItemMessage(hDlg, IDC_IMGHISTOGRAMME, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)hBitmapGreen);
							SendDlgItemMessage(hDlg, IDC_IMGGRADIENT, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)hBmpGradientGreen);
							break;
						case 3:
							SendDlgItemMessage(hDlg, IDC_IMGHISTOGRAMME, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)hBitmapBlue);
							SendDlgItemMessage(hDlg, IDC_IMGGRADIENT, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)hBmpGradientBlue);
							break;
						}
						return 0;
					}
			}
			break;
		}

		case IDOK:
			{
				EndDialog(hDlg, LOWORD(1));
			}
			return TRUE;

		case IDCANCEL:
			{
				EndDialog(hDlg, LOWORD(0));
			}
			return TRUE;
	}
	return 0;
}

LRESULT CHistorgrammeDialog::OnPaint(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(m_iSelect)
	{
		case 0:
			SendDlgItemMessage(hDlg, IDC_IMGHISTOGRAMME, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)hBitmapMoyenne);
			SendDlgItemMessage(hDlg, IDC_IMGGRADIENT, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)hBmpGradientBlack);
			break;
		case 1:
			SendDlgItemMessage(hDlg, IDC_IMGHISTOGRAMME, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)hBitmapRed);
			SendDlgItemMessage(hDlg, IDC_IMGGRADIENT, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)hBmpGradientRed);
			break;
		case 2:
			SendDlgItemMessage(hDlg, IDC_IMGHISTOGRAMME, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)hBitmapGreen);
			SendDlgItemMessage(hDlg, IDC_IMGGRADIENT, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)hBmpGradientGreen);
			break;
		case 3:
			SendDlgItemMessage(hDlg, IDC_IMGHISTOGRAMME, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)hBitmapBlue);
			SendDlgItemMessage(hDlg, IDC_IMGGRADIENT, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)hBmpGradientBlue);
			break;
	}
	return 0;
}