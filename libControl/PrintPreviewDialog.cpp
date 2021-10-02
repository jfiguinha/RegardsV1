#include "stdafx.h"
#include ".\printpreviewdialog.h"
#include "Print.h"
#include <PrintPreviewData.h>
#include "objet.h"
#include <IBitmap.h>
#include <Effet.h>
#include <MargeData.h>

void CPrintPreviewDialog::SetParam(CMargeData * pMargeData)
{
	m_prtPrevData->m_MargeData = pMargeData;
}

CPrintPreviewDialog::CPrintPreviewDialog(void)
{
	m_prtPrevData = new CPrintPreviewData();
	Temp[0] = '\0';
	m_printPrv = new CPrintPrv();
	m_iReturnValue = 0;
}

CPrintPreviewDialog::~CPrintPreviewDialog(void)
{
	if(m_prtPrevData != NULL)
		delete m_prtPrevData;

	m_prtPrevData = NULL;

	if(m_printPrv != NULL)
		delete m_printPrv;

	m_printPrv = NULL;
}

LRESULT CPrintPreviewDialog::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//Initialisation des données
	TCHAR* buffer;
	
	if(m_printPrv->GetDefaultPrintName(&buffer, m_prtPrevData->iOrientation) == 1)
	{
		m_iReturnValue = 1;
		EndDialog(hDlg,1);
		return FALSE;
	}

	m_prtPrevData->m_ciBitmap = new CIBitmap();
	m_prtPrevData->m_ciBitmapTemp = new CIBitmap();


	//Récupération de la taille de l'image		
	
	RECT rc;
	HWND hWnd = GetDlgItem(hDlg,IDC_PCTPREV);

	GetClientRect(hWnd,&rc);

	//Création de l'image de papier à partir des informations concernant la feuille de papier

	m_prtPrevData->m_ciBitmap->CreateBitmap(255,255);
	m_prtPrevData->m_ciBitmapTemp->CreateBitmap(255,255);

	//Orientation du papier

	HDC hDC = CreateDC(NULL, buffer, NULL, NULL);

	//Récupération des dimensions de la feuille papier
	m_prtPrevData->iWidthPageSize = GetDeviceCaps (hDC, HORZSIZE);
	m_prtPrevData->iHeightPageSize = GetDeviceCaps (hDC, VERTSIZE);
	m_prtPrevData->iWidthPixelSize = GetDeviceCaps (hDC, HORZRES);
	m_prtPrevData->iHeightPixelSize = GetDeviceCaps (hDC, VERTRES);


	if(m_prtPrevData->iOrientation == 1)
	{			
		//Portrait 
		
		double m_dRatio = (double)(235)/(double)(m_prtPrevData->iHeightPageSize);
	
		m_prtPrevData->iPosWidth = m_dRatio * m_prtPrevData->iWidthPageSize;

		m_prtPrevData->iPosHeight = 235;

		m_prtPrevData->iPosTop = 10;

		m_prtPrevData->iPosLeft = (255 - m_prtPrevData->iPosWidth) / 2;

		//Dessin de la page

		if(m_prtPrevData->m_ciBitmapTemp->GetPtBitmap() != NULL)
		{
			//long Moyenne;
			BYTE * pBitsSrc = NULL;
			UINT bmWidth, bmHeight;
			long lWidthSize = m_prtPrevData->m_ciBitmapTemp->GetWidthSize();

			pBitsSrc = m_prtPrevData->m_ciBitmapTemp->GetPtBitmap();

			bmWidth = 255;
			bmHeight = 255;

			//Test du gradient de couleur

			UINT k,j;
			for(k = 0;k < bmHeight;k++)
			{
				for(j = 0;j < bmWidth;j++)
				{
					UINT i = (k * lWidthSize) + (j * 4);
					*(pBitsSrc + i) = 161;
					*(pBitsSrc + i + 1) = 157;
					*(pBitsSrc + i + 2) = 157;

				}
			}

			for(k = m_prtPrevData->iPosTop;k < m_prtPrevData->iPosHeight + m_prtPrevData->iPosTop;k++)
			{
				for(j = m_prtPrevData->iPosLeft;j < m_prtPrevData->iPosWidth + m_prtPrevData->iPosLeft;j++)
				{
					UINT i = (k * lWidthSize) + (j * 4);
					*(pBitsSrc + i) = 255;
					*(pBitsSrc + i + 1) = 255;
					*(pBitsSrc + i + 2) = 255;

				}
			}

			m_prtPrevData->m_ciBitmapTemp->ReCreateHBitmap(false);

			//Calcul de la taille de l'image


			if(m_prtPrevData->m_MargeData->m_ciBitmap->GetBitmapHeigth() > m_prtPrevData->m_MargeData->m_ciBitmap->GetBitmapWidth())
			{
				SetDlgItemText(hDlg, IDC_RDHAUT,"Gauche");
				SetDlgItemText(hDlg, IDC_RDBAS,"Droite");
				m_prtPrevData->m_MargeData->iPos = 1;
			}
			else
			{
				m_prtPrevData->m_MargeData->iPos = 0;
			}

			//delete[] pBitsSrc;
		}

	}
	else
	{
		//Landscape
		double m_dRatio = (double)(235)/(double)(m_prtPrevData->iWidthPageSize);
		
		m_prtPrevData->iPosHeight = m_dRatio * m_prtPrevData->iHeightPageSize;
		m_prtPrevData->iPosWidth = 235;
		m_prtPrevData->iPosTop = (255 - m_prtPrevData->iPosHeight)/2;
		m_prtPrevData->iPosLeft = 10;

		//Dessin de la page

		if(m_prtPrevData->m_ciBitmapTemp->GetPtBitmap() != NULL)
		{
			//long Moyenne;
			BYTE * pBitsSrc = NULL;
			UINT bmWidth, bmHeight;
			long lWidthSize = m_prtPrevData->m_ciBitmapTemp->GetWidthSize();

			bmWidth = 255;
			bmHeight = 255;


			pBitsSrc = m_prtPrevData->m_ciBitmapTemp->GetPtBitmap();

			//Test du gradient de couleur

			UINT k,j;
			for(k = 0;k < bmHeight;k++)
			{
				for(j = 0;j < bmWidth;j++)
				{
					UINT i = (k * lWidthSize) + (j * 4);
					*(pBitsSrc + i) = 161;
					*(pBitsSrc + i + 1) = 157;
					*(pBitsSrc + i + 2) = 157;

				}
			}

			for(k = m_prtPrevData->iPosTop;k < m_prtPrevData->iPosHeight + m_prtPrevData->iPosTop;k++)
			{
				for(j = m_prtPrevData->iPosLeft;j < m_prtPrevData->iPosWidth + m_prtPrevData->iPosLeft;j++)
				{
					UINT i = (k * lWidthSize) + (j * 4);
					*(pBitsSrc + i) = 255;
					*(pBitsSrc + i + 1) = 255;
					*(pBitsSrc + i + 2) = 255;

				}
			}

			m_prtPrevData->m_ciBitmapTemp->ReCreateHBitmap(false);

			//Calcul de la taille de l'image
			if(m_prtPrevData->m_MargeData->m_ciBitmap->GetBitmapHeigth() > m_prtPrevData->m_MargeData->m_ciBitmap->GetBitmapWidth())
			{
				SetDlgItemText(hDlg, IDC_RDHAUT,"Gauche");
				SetDlgItemText(hDlg, IDC_RDBAS,"Droite");
				m_prtPrevData->m_MargeData->iPos = 1;
			}
			else
			{
				m_prtPrevData->m_MargeData->iPos = 0;
			}

			//delete[] pBitsSrc;
		}
	}

	m_prtPrevData->m_ciBitmap->SetBitmap(m_prtPrevData->m_ciBitmapTemp->GetPtBitmap(), 255, 255);

	m_prtPrevData->iPosLeftPicture = (m_prtPrevData->m_MargeData->m_fGauche * m_prtPrevData->iPosWidth * 10) / m_prtPrevData->iWidthPageSize;
	m_prtPrevData->iPosTopPicture = (m_prtPrevData->m_MargeData->m_fHaut * m_prtPrevData->iPosHeight * 10) / m_prtPrevData->iHeightPageSize;
	m_prtPrevData->iPosBottomPicture = (m_prtPrevData->m_MargeData->m_fBas * m_prtPrevData->iPosHeight * 10) / m_prtPrevData->iHeightPageSize;
	m_prtPrevData->iPosRightPicture = (m_prtPrevData->m_MargeData->m_fDroite * m_prtPrevData->iPosWidth * 10) / m_prtPrevData->iWidthPageSize;

	m_printPrv->DetermineRatio(m_prtPrevData);

	m_printPrv->PrintPreviewPaint(hDlg,m_prtPrevData);

	SendDlgItemMessage(hDlg, IDC_PCTPREV, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_prtPrevData->m_ciBitmap->hBitmap);	


	delete[] buffer;

	ReleaseDC(NULL,hDC);

	//if(m_prtPrevData->m_MargeData->m_fHaut == 0)
	//	m_prtPrevData->m_MargeData->m_fHaut = DEFAUTPRINTDISTANCE;



	StringCchPrintf(Temp,50,"%f",m_prtPrevData->m_MargeData->m_fHaut);
	SetDlgItemText(hDlg, IDC_EDHAUT,Temp);
	StringCchPrintf(Temp,50,"%f",m_prtPrevData->m_MargeData->m_fBas);
	SetDlgItemText(hDlg, IDC_EDBAS,Temp);
	StringCchPrintf(Temp,50,"%f",m_prtPrevData->m_MargeData->m_fDroite);
	SetDlgItemText(hDlg, IDC_EDDROIT,Temp);
	StringCchPrintf(Temp,50,"%f",m_prtPrevData->m_MargeData->m_fGauche);
	SetDlgItemText(hDlg, IDC_EDGAUCHE,Temp);
	CheckDlgButton(hDlg, IDC_RDHAUT, 1);
	return 0;
}

LRESULT CPrintPreviewDialog::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{
		case IDCANCEL:
			{
				m_iReturnValue = 0;
				EndDialog(hDlg, LOWORD(0));
				return TRUE;

			}
		case IDOK:
			{
				m_iReturnValue = 1;
				EndDialog(hDlg, LOWORD(1));
				return TRUE;
			}

			
		case IDC_EDHAUT:
			{
				char cTemp[255];
				SendDlgItemMessage(hDlg, IDC_EDHAUT, WM_GETTEXT, 255, (LPARAM)cTemp);
				if(cTemp != NULL)
				{
					m_prtPrevData->m_MargeData->m_fHaut = atof(cTemp);
					m_prtPrevData->iPosTopPicture = (m_prtPrevData->m_MargeData->m_fHaut * m_prtPrevData->iPosHeight * 10) / m_prtPrevData->iHeightPageSize;
					
					if(m_prtPrevData->m_MargeData->iPos == 1)
						m_printPrv->DetermineRatio(m_prtPrevData);

					m_printPrv->PrintPreviewPaint(hDlg,m_prtPrevData);
					SendMessage(hDlg,WM_ERASEBKGND,0,0);
				}
			}
			break;

		case IDC_EDBAS:
			{
				char cTemp[255];
				SendDlgItemMessage(hDlg, IDC_EDBAS, WM_GETTEXT, 255, (LPARAM)cTemp);
				if(cTemp != NULL)
				{
					m_prtPrevData->m_MargeData->m_fBas = atof(cTemp);
					m_prtPrevData->iPosBottomPicture = (m_prtPrevData->m_MargeData->m_fBas * m_prtPrevData->iPosHeight * 10) / m_prtPrevData->iHeightPageSize;
					
					if(m_prtPrevData->m_MargeData->iPos == 1)
						m_printPrv->DetermineRatio(m_prtPrevData);

					m_printPrv->PrintPreviewPaint(hDlg,m_prtPrevData);
					SendMessage(hDlg,WM_ERASEBKGND,0,0);
				}
			}
			break;

		case IDC_EDDROIT:
			{
				char cTemp[255];
				SendDlgItemMessage(hDlg, IDC_EDDROIT, WM_GETTEXT, 255, (LPARAM)cTemp);
				if(cTemp != NULL)
				{
					m_prtPrevData->m_MargeData->m_fDroite = atof(cTemp);
					m_prtPrevData->iPosRightPicture = (m_prtPrevData->m_MargeData->m_fDroite * m_prtPrevData->iPosWidth * 10) / m_prtPrevData->iWidthPageSize;
					
					if(m_prtPrevData->m_MargeData->iPos == 0)
						m_printPrv->DetermineRatio(m_prtPrevData);

					m_printPrv->PrintPreviewPaint(hDlg,m_prtPrevData);
					SendMessage(hDlg,WM_ERASEBKGND,0,0);
				}
			}
			break;

		case IDC_EDGAUCHE:
			{
				char cTemp[255];
				SendDlgItemMessage(hDlg, IDC_EDGAUCHE, WM_GETTEXT, 255, (LPARAM)cTemp);
				if(cTemp != NULL)
				{
					m_prtPrevData->m_MargeData->m_fGauche = atof(cTemp);
					m_prtPrevData->iPosLeftPicture = (m_prtPrevData->m_MargeData->m_fGauche * m_prtPrevData->iPosWidth * 10) / m_prtPrevData->iWidthPageSize;
					
					if(m_prtPrevData->m_MargeData->iPos == 0)
						m_printPrv->DetermineRatio(m_prtPrevData);

					m_printPrv->PrintPreviewPaint(hDlg,m_prtPrevData);
					SendMessage(hDlg,WM_ERASEBKGND,0,0);
				}
			}
			break;


	}

	switch(wmEvent)
	{
		case BN_CLICKED:
			{
				switch (wmId)
				{
					case IDC_RDHAUT:
						{
							if(m_prtPrevData->m_MargeData->iPos == 0)
							{
								m_prtPrevData->m_MargeData->m_fHaut = DEFAUTPRINTDISTANCE;

								StringCchPrintf(Temp,50,"%f",m_prtPrevData->m_MargeData->m_fHaut);
								SetDlgItemText(hDlg, IDC_EDHAUT,Temp);

							}
							else
							{
								m_prtPrevData->m_MargeData->m_fGauche = DEFAUTPRINTDISTANCE;

								StringCchPrintf(Temp,50,"%f",m_prtPrevData->m_MargeData->m_fGauche);
								SetDlgItemText(hDlg, IDC_EDGAUCHE,Temp);

							}
						}
						break;

					case IDC_RDMIDL:
						{
							
							if(m_prtPrevData->m_MargeData->iPos == 0)
							{
								double m_dTailleImage = ((m_prtPrevData->m_MargeData->m_ciBitmap->GetBitmapHeigth() * m_prtPrevData->m_dRatio) * m_prtPrevData->iHeightPageSize) / m_prtPrevData->iPosHeight;
								m_prtPrevData->m_MargeData->m_fHaut = (m_prtPrevData->iHeightPageSize - m_dTailleImage) / 2;
								m_prtPrevData->m_MargeData->m_fHaut /= 10;

								StringCchPrintf(Temp,50,"%f",m_prtPrevData->m_MargeData->m_fHaut);
								SetDlgItemText(hDlg, IDC_EDHAUT,Temp);

							}
							else
							{
								double m_dTailleImage = ((m_prtPrevData->m_MargeData->m_ciBitmap->GetBitmapWidth() * m_prtPrevData->m_dRatio) * m_prtPrevData->iWidthPageSize) / m_prtPrevData->iPosWidth;
								m_prtPrevData->m_MargeData->m_fGauche = (m_prtPrevData->iWidthPageSize - m_dTailleImage) / 2;
								m_prtPrevData->m_MargeData->m_fGauche /= 10;

								StringCchPrintf(Temp,50,"%f",m_prtPrevData->m_MargeData->m_fGauche);
								SetDlgItemText(hDlg, IDC_EDGAUCHE,Temp);

							}
						}
						break;

					case IDC_RDBAS:
						{

							//Calcul avec les marges en hauteur et largeur
							if(m_prtPrevData->m_MargeData->iPos == 0)
							{
								double m_dTailleImage = ((m_prtPrevData->m_MargeData->m_ciBitmap->GetBitmapHeigth() * m_prtPrevData->m_dRatio) * m_prtPrevData->iHeightPageSize) / m_prtPrevData->iPosHeight;
								m_prtPrevData->m_MargeData->m_fHaut = m_prtPrevData->iHeightPageSize - (m_dTailleImage + m_prtPrevData->m_MargeData->m_fBas * 10);
								m_prtPrevData->m_MargeData->m_fHaut /= 10;

								StringCchPrintf(Temp,50,"%f",m_prtPrevData->m_MargeData->m_fHaut);
								SetDlgItemText(hDlg, IDC_EDHAUT,Temp);

							}
							else
							{
								double m_dTailleImage = ((m_prtPrevData->m_MargeData->m_ciBitmap->GetBitmapWidth() * m_prtPrevData->m_dRatio) * m_prtPrevData->iWidthPageSize) / m_prtPrevData->iPosWidth;
								m_prtPrevData->m_MargeData->m_fGauche = m_prtPrevData->iWidthPageSize - (m_dTailleImage + m_prtPrevData->m_MargeData->m_fDroite * 10);
								m_prtPrevData->m_MargeData->m_fGauche /= 10;

								StringCchPrintf(Temp,50,"%f",m_prtPrevData->m_MargeData->m_fGauche);
								SetDlgItemText(hDlg, IDC_EDGAUCHE,Temp);

							}
						}
						break;

					case IDC_CHKROTER:
						{
							//CBitmap m_cBitmap;
							//HWND hWnd = hDlg;
							
							m_prtPrevData->m_MargeData->m_fGauche = DEFAUTPRINTDISTANCE;
							m_prtPrevData->m_MargeData->m_fHaut = DEFAUTPRINTDISTANCE;
							m_prtPrevData->m_MargeData->m_fDroite = DEFAUTPRINTDISTANCE;
							m_prtPrevData->m_MargeData->m_fBas = DEFAUTPRINTDISTANCE;											
							
							if(IsDlgButtonChecked(hDlg,IDC_CHKROTER))
							{
								//90
								//m_cBitmap.AppliquerFiltre(hWnd, prtPrevData->m_MargeData->m_ciBitmap, EFFET_ROTATION,90,0,0,0);
								m_prtPrevData->m_MargeData->m_ciBitmap->Rotation90();
								m_prtPrevData->m_MargeData->m_ciBitmap->HorzFlipBuf();
							}
							else
							{
								//270
								//m_cBitmap.AppliquerFiltre(hWnd, prtPrevData->m_MargeData->m_ciBitmap, EFFET_ROTATION,270,0,0,0);
								m_prtPrevData->m_MargeData->m_ciBitmap->HorzFlipBuf();
								m_prtPrevData->m_MargeData->m_ciBitmap->Rotation90();
							}

							m_prtPrevData->m_MargeData->m_ciBitmap->ReCreateHBitmap(false);

							
							if(m_prtPrevData->m_MargeData->iPos == 0)
							{
								SetDlgItemText(hDlg, IDC_RDHAUT,"Gauche");
								SetDlgItemText(hDlg, IDC_RDBAS,"Droite");
								m_prtPrevData->m_MargeData->iPos = 1;

							}
							else
							{
								SetDlgItemText(hDlg, IDC_RDHAUT,"Haut");
								SetDlgItemText(hDlg, IDC_RDBAS,"Bas");
								m_prtPrevData->m_MargeData->iPos = 0;
							}

							ShowWindow(hDlg,SW_HIDE);
							ShowWindow(hDlg,SW_SHOW);

							m_prtPrevData->iPosLeftPicture = (m_prtPrevData->m_MargeData->m_fGauche * m_prtPrevData->iPosWidth * 10) / m_prtPrevData->iWidthPageSize;
							m_prtPrevData->iPosTopPicture = (m_prtPrevData->m_MargeData->m_fHaut * m_prtPrevData->iPosHeight * 10) / m_prtPrevData->iHeightPageSize;
							m_prtPrevData->iPosBottomPicture = (m_prtPrevData->m_MargeData->m_fBas * m_prtPrevData->iPosHeight * 10) / m_prtPrevData->iHeightPageSize;
							m_prtPrevData->iPosRightPicture = (m_prtPrevData->m_MargeData->m_fDroite * m_prtPrevData->iPosWidth * 10) / m_prtPrevData->iWidthPageSize;

							m_printPrv->DetermineRatio(m_prtPrevData);

							StringCchPrintf(Temp,50,"%f",m_prtPrevData->m_MargeData->m_fHaut);
							SetDlgItemText(hDlg, IDC_EDHAUT,Temp);
							StringCchPrintf(Temp,50,"%f",m_prtPrevData->m_MargeData->m_fBas);
							SetDlgItemText(hDlg, IDC_EDBAS,Temp);
							StringCchPrintf(Temp,50,"%f",m_prtPrevData->m_MargeData->m_fDroite);
							SetDlgItemText(hDlg, IDC_EDDROIT,Temp);
							StringCchPrintf(Temp,50,"%f",m_prtPrevData->m_MargeData->m_fGauche);
							SetDlgItemText(hDlg, IDC_EDGAUCHE,Temp);

						}
						break;
				}
				m_printPrv->PrintPreviewPaint(hDlg,m_prtPrevData);
				//SendMessage(hDlg,WM_ERASEBKGND,0,0);
			}
		}
	return 0;
}

LRESULT CPrintPreviewDialog::OnPaint(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_printPrv->RedrawPrintPage(hDlg,m_prtPrevData->m_ciBitmap,m_prtPrevData->m_MargeData);
	return 0;
}

LRESULT CPrintPreviewDialog::OnEraseBackground(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_printPrv->RedrawPrintPage(hDlg,m_prtPrevData->m_ciBitmap,m_prtPrevData->m_MargeData);
	return 0;
}

LRESULT CPrintPreviewDialog::OnDestroy(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_prtPrevData->m_ciBitmap != NULL)
		delete m_prtPrevData->m_ciBitmap;

	m_prtPrevData->m_ciBitmap = NULL;

	if(m_prtPrevData->m_ciBitmapTemp != NULL)
		delete m_prtPrevData->m_ciBitmapTemp;

	m_prtPrevData->m_ciBitmapTemp = NULL;

	return 0;
}