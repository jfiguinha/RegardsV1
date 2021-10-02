#include "StdAfx.h"
#include ".\wienerfilter.h"
#include "resource.h"
#include <IBitmap.h>
#include <Filtre.h>
#include <Effet.h>
#include <Matrix.h>
#include "objet.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWienerFilter::CWienerFilter(void)
{
	m_ciBitmapTestBitmap = new CIBitmap();
	m_ciBitmapOldBitmap = new CIBitmap();
	m_cMatrixFiltre = new CMatrix();
	m_iReturnValue = 0;
	m_iTailleFiltreL = 1;
	m_iTailleFiltreH = 1;
	m_rho = 0.002;
	m_iMethode = 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWienerFilter::~CWienerFilter(void)
{
	if(m_ciBitmapTestBitmap != NULL)
		delete m_ciBitmapTestBitmap;

	m_ciBitmapTestBitmap = NULL;

	if(m_ciBitmapOldBitmap != NULL)
		delete m_ciBitmapOldBitmap;

	m_ciBitmapOldBitmap = NULL;

	if(m_cMatrixFiltre != NULL)
		delete m_cMatrixFiltre;

	m_cMatrixFiltre = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CWienerFilter::Init(CIBitmap * Source)
{
	//Transformation de l'image
	CEffet m_cEffet;
	UINT iWidth = 0;
	UINT iHeight = 0;
	m_iMethode = 1;
	HBITMAP hBitmap;
	CIBitmap * m_ciBitmap = new CIBitmap();
	m_ciBitmap->SetBitmap(Source->GetPtBitmap(),Source->GetBitmapWidth(),Source->GetBitmapHeigth());
	hBitmap = m_cEffet.GenerateIcone(Source, 256, 256, iWidth, iHeight, 0, RGB(0,0,0));	
	m_ciBitmapTestBitmap->SetBitmap(hBitmap,false);
	m_ciBitmapOldBitmap->SetBitmap(hBitmap,false);
	DeleteObject(hBitmap);
	delete m_ciBitmap;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWienerFilter::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char Temp[50];
	
	HWND hWndLocal;

	SendDlgItemMessage(hDlg, IDC_PICTMATRIX, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapTestBitmap->hBitmap);

	SendDlgItemMessage(hDlg, IDC_RDWIENER, BM_SETCHECK,1,0);

	SetDlgItemInt(hDlg,IDC_EDFILTERH,m_iTailleFiltreH,true);
	SetDlgItemInt(hDlg,IDC_EDFILTERL,m_iTailleFiltreL,true);

	hWndLocal = GetDlgItem(hDlg, IDC_EDFILTERH);
	
	SendDlgItemMessage(hDlg, IDC_SPFILTERH, UDM_SETBUDDY,(WPARAM)hWndLocal,0);
	SendDlgItemMessage(hDlg, IDC_SPFILTERH, UDM_SETRANGE,0,(LPARAM) MAKELONG((short)27, (short)1));
	SendDlgItemMessage(hDlg, IDC_SPFILTERH, UDM_SETPOS, 0L, MAKELONG(1, 0));

	hWndLocal = GetDlgItem(hDlg, IDC_EDFILTERL);
	
	SendDlgItemMessage(hDlg, IDC_SPFILTERL, UDM_SETBUDDY,(WPARAM)hWndLocal,0);
	SendDlgItemMessage(hDlg, IDC_SPFILTERL, UDM_SETRANGE,0,(LPARAM) MAKELONG((short)27, (short)1));
	SendDlgItemMessage(hDlg, IDC_SPFILTERL, UDM_SETPOS, 0L, MAKELONG(1, 0));

	StringCchPrintf(Temp,50,"%f",m_rho);

	SetDlgItemText(hDlg,IDC_EDRHO,Temp);

	m_cMatrixFiltre->ones(m_iTailleFiltreH,m_iTailleFiltreL);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWienerFilter::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{
		case IDC_RDCOSINUS:
			m_iMethode = 2;
			break;

		case IDC_RDWIENER:
			m_iMethode = 1;
			break;

		case IDC_BTPREVIEW:
			{
				CFiltre m_cFiltre;

				HCURSOR hcurSave = SetCursor(LoadCursor(NULL,IDC_WAIT));

				HWND hWnd = GetDlgItem(hDlg, IDC_PICTMATRIX);
				char cTemp[255];
				GetDlgItemText(hDlg, IDC_EDRHO, cTemp,255);
				m_rho = atof(cTemp);

				m_ciBitmapTestBitmap->SetBitmap(m_ciBitmapOldBitmap->GetPtBitmap(),m_ciBitmapOldBitmap->GetBitmapWidth(),m_ciBitmapOldBitmap->GetBitmapHeigth());

				m_cFiltre.FiltreWiener(NULL,m_ciBitmapTestBitmap,*m_cMatrixFiltre,m_rho,m_iMethode);					

				m_ciBitmapTestBitmap->SpeedPictureShow(hWnd);

				SetCursor(hcurSave);

			}
			break;

		case IDC_BUTTON1:
			m_iReturnValue = 0;
			EndDialog(hDlg, LOWORD(0));
			return TRUE;

		case IDOK:
			{
			
				char cTemp[255];
				SendDlgItemMessage(hDlg, IDC_EDRHO, WM_GETTEXT, 255, (LPARAM)cTemp);
				//Vérification si nombre flottant
				m_rho = atof(cTemp);

				if(m_rho == 0.0)
				{
					CObjet::AffichageMessage(IDS_MSGWIENERNBFLOT,IDS_INFOS,MB_ICONEXCLAMATION);
					m_iReturnValue = 0;
					EndDialog(hDlg, LOWORD(0));
					return 0;
				}

				m_iReturnValue = 1;

				EndDialog(hDlg, LOWORD(1));
			}
			return TRUE;

		case IDCANCEL:
			m_iReturnValue = 0;
			EndDialog(hDlg, LOWORD(0));
			return TRUE;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWienerFilter::OnPaint(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SendDlgItemMessage(hDlg, IDC_PICTMATRIX, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapTestBitmap->hBitmap);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWienerFilter::OnActivate(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!HIWORD(wParam))								// Check Minimization State
	{
		HWND hWnd = GetDlgItem(hDlg, IDC_PICTMATRIX);
		m_ciBitmapTestBitmap->SpeedPictureShow(hWnd);
							// Program Is Active
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWienerFilter::OnNotify(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int idCtrl = (int) wParam; 
	switch(idCtrl)
	{
		case IDC_SPFILTERH:
			{
				LPNMUPDOWN lpnmud = (LPNMUPDOWN) lParam;
				m_iTailleFiltreH += lpnmud->iDelta * 2;
				if(m_iTailleFiltreH < 0  || m_iTailleFiltreH > 27)
					m_iTailleFiltreH = 1;
				SetDlgItemInt(hDlg,IDC_EDFILTERH,m_iTailleFiltreH,true);

				m_cMatrixFiltre->ones(m_iTailleFiltreH,m_iTailleFiltreL);
			}
			break;

		case IDC_SPFILTERL:
			{
				LPNMUPDOWN lpnmud = (LPNMUPDOWN) lParam;
				m_iTailleFiltreL += lpnmud->iDelta * 2;
				if(m_iTailleFiltreL < 0 || m_iTailleFiltreL > 27)
					m_iTailleFiltreL = 1;
				SetDlgItemInt(hDlg,IDC_EDFILTERL,m_iTailleFiltreL,true);

				m_cMatrixFiltre->ones(m_iTailleFiltreH,m_iTailleFiltreL);
			}
			break;
	}

	return 0;
}
