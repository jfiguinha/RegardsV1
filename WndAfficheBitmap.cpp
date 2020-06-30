// WndAfficheBitmap.cpp: implementation of the CWndAfficheBitmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WndAfficheBitmap.h"
#include ".\naviguedata.h"
#include <Regardsbitmap.h>
#include <ibitmap.h>
#include "statusbar.h"
#include "Crop.h"
#include "fichier.h"
#include "effet.h"
#include <DropTgt.h>  
#include "dessin.h"
#include "scroll.h"
#include "objet.h"
#include "print.h"
#include "loadimage.h"
#include <PhotoFiltre.h>
#include <ResizeWindow.h>
#include <RGBDialog.h>
#include <BrightContDialog.h>
#include <HistorgrammeDialog.h>
#include <MargeData.h>
#include "Parameter.h"
#include "libgpgpu\GpGpuEffect.h"
using namespace Regards::libbitmap;

#define MAX_ELEMENT 10

void CWndAfficheBitmap::InitDessin()
{
	if(m_cDessin != NULL)
	{
		delete m_cDessin;
		m_cDessin = NULL;
	}
}



void CWndAfficheBitmap::VidageMemoire()
{
	//Vidage de la mémoire
	
	while(m_iAction != 0)
	{
		delete m_lpBitmapData[m_iAction - 1].m_ciBitmap;
		m_iAction--;
	}


	if(m_lpBitmapData != NULL)
	{
		HeapFree(GetProcessHeap(),0,m_lpBitmapData);
	}

	m_iAction = 0;
}

void CWndAfficheBitmap::Windows98Update(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	m_cNavigueData->GetCIBitmap()->SetUse(true);
	m_cNavigueData->GetCBitmap()->SetHauteQualite(m_cNavigueData->GetHauteQualite());
	EraseBgrCreate(hWnd, message, wParam, lParam);
	m_cNavigueData->GetCIBitmap()->SetUse(false);
	SendMessage(hWnd,WM_PAINT,0,0);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWndAfficheBitmap::CWndAfficheBitmap()
{
	m_cDessin = NULL;
	m_lpBitmapData = NULL;
	m_iAction = 0;
	m_ChargementImage = NULL;
	m_MargeData = NULL;
	hWndLevel = NULL;
}

LRESULT CWndAfficheBitmap::OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//Récupération des valeurs en x et y
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);

	RECT rc;
	GetClientRect(hWnd,&rc);


	if(HIWORD(lParam) > 0 && HIWORD(lParam) < rc.bottom && LOWORD(lParam) > 0 && LOWORD(lParam) < rc.right)
	{
		switch(m_cNavigueData->GetSelect())
		{
			case 1:
				{
					//MouseAndKeyCreate(hWnd, message, wParam, lParam, pNavigueData);
					if(!m_cNavigueData->GetCBitmap()->GetMouseBlock())
						m_cDessin->Selection(y + m_cNavigueData->GetCScroll()->GetCurrentVScroll(),x + m_cNavigueData->GetCScroll()->GetCurrentHScroll());
					else
					{
						//Deplacement d'écran si on atteint les 20%
						MouseAndKeyCreate(hWnd, message, wParam, lParam);
							
						m_cDessin->MouseMove(x,y,m_cNavigueData->GetCScroll()->GetCurrentHScroll(),m_cNavigueData->GetCScroll()->GetCurrentVScroll());
						
						//SendMessage(hWnd,WM_PAINT,0,0);
						SendMessage(hWnd,WM_ERASEBKGND,0,MAKELPARAM(IDM_REDRAW_OK,IDM_REDRAW_OK));

					}
				}
				break;

			case 2:
				{
					if(HIWORD(GetKeyState(VK_CONTROL)) != 0)
					{
						//Zoom On
						SetCursor (LoadCursor (hInstance, MAKEINTRESOURCE(IDC_ZOOMOUT))) ;
					}
					else
					{
						//Zoom Out
						SetCursor (LoadCursor (hInstance, MAKEINTRESOURCE(IDC_ZOOMON))) ;
					}
				}
				break;

			default:
				MouseAndKeyCreate(hWnd, message, wParam, lParam);
				break;
		}
	}
	return 0;
}

CWndAfficheBitmap::~CWndAfficheBitmap()
{
	CParameter::SetGpGpu(m_cNavigueData->GetGpGpu());

	
	if(m_lpBitmapData != NULL)
		VidageMemoire();

	m_lpBitmapData = NULL;

	if(m_cDessin != NULL)
		delete m_cDessin;

	m_cDessin = NULL;

}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void CWndAfficheBitmap::AddBitmap()
{
	char m_cCancelMessage[20];
	
	char cTemp[20];
	HMENU hMnu, hmenu;

	if(m_lpBitmapData == NULL)
	{
		m_lpBitmapData = (LPBITMAPDATA)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(LPBITMAPDATA) * MAX_ELEMENT);
	}

	if(m_iAction >= MAX_ELEMENT)
	{
		m_iAction = MAX_ELEMENT - 1;
		//Dêplacement des pointeurs de mémoire

		delete m_lpBitmapData[0].m_ciBitmap;

		for(int j = 0;j < MAX_ELEMENT;j++)
		{
			m_lpBitmapData[j] = m_lpBitmapData[j + 1];
		}
	}
	CIBitmap * m_ciBitmapPt;

	m_lpBitmapData[m_iAction].m_ciBitmap = new CIBitmap();
	if (m_lpBitmapData[m_iAction].m_ciBitmap == NULL)
	{
		CObjet::ErreurInformation();
	}

	m_ciBitmapPt = m_lpBitmapData[m_iAction].m_ciBitmap;

	m_ciBitmapPt->SetBitmap(m_cNavigueData->GetCIBitmap()->GetPtBitmap(),m_cNavigueData->GetCIBitmap()->GetBitmapWidth(),m_cNavigueData->GetCIBitmap()->GetBitmapHeigth());
	
	m_iAction++;

	hMnu = GetMenu(GetParent());
	hmenu = GetSubMenu(hMnu, 1); 

	LoadString(hInstance,IDS_CANCEL,m_cCancelMessage,20);
	StringCchPrintf(cTemp,20,"%s (%d)",m_cCancelMessage,m_iAction);

	if(m_iAction == 1)
		EnableMenuItem(hMnu, ID_EDITION_ANNULER, MF_ENABLED);

	ModifyMenu(hmenu,0,MF_BYPOSITION,ID_EDITION_ANNULER,cTemp);

}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void CWndAfficheBitmap::RemoveBitmap()
{
	char cTemp[20];
	char m_szString[55];
	HMENU hMnu, hmenu;
	CIBitmap * m_ciBitmapPt;

	m_ciBitmapPt = m_lpBitmapData[m_iAction - 1].m_ciBitmap;

	m_cNavigueData->GetCIBitmap()->SetBitmap(m_ciBitmapPt->GetPtBitmap(),m_ciBitmapPt->GetBitmapWidth(),m_ciBitmapPt->GetBitmapHeigth());

	//m_ciBitmapPt->EraseData();
	delete m_ciBitmapPt;

	m_iAction--;
	
	LoadString(hInstance,IDS_CANCEL,m_szString,55);

	hMnu = GetMenu(GetParent());
	hmenu = GetSubMenu(hMnu, 1); 

	if(m_iAction < 1)
	{
		StringCchCopy(cTemp,20,m_szString);
		ModifyMenu(hmenu,0,MF_BYPOSITION,ID_EDITION_ANNULER,cTemp);
		EnableMenuItem(hMnu, ID_EDITION_ANNULER, MF_GRAYED);
	}
	else
	{
		StringCchPrintf(cTemp,20,"%s (%d)",m_szString,m_iAction);
		ModifyMenu(hmenu,0,MF_BYPOSITION,ID_EDITION_ANNULER,cTemp);
	}
	
	SendMessage(m_hWnd,WM_ERASEBKGND,0,MAKELPARAM(IDM_REDRAW_PICTURE,0));
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void CWndAfficheBitmap::VerifModification()
{
	if(m_iAction != 0)
	{
		char m_cTemp1[255];
		char m_cTemp2[255];

		LoadString(hInstance,IDS_INFOS,m_cTemp1,255);
		LoadString(hInstance,IDS_SAVEPICTURE,m_cTemp2,255);

		if(MessageBox(NULL,m_cTemp2,m_cTemp1,MB_YESNO) == IDYES)
			m_cNavigueData->GetCFichier()->SauverFichier(m_cNavigueData->GetCFile(),*m_cNavigueData->GetCIBitmap(),m_hWnd,NULL);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CWndAfficheBitmap::OnDblClick(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(!m_cNavigueData->Get3D() && !m_cNavigueData->Get3DDiaporama() && !m_cNavigueData->GetVideo())
		OnCommand(hWnd,WM_COMMAND,MAKEWPARAM(IDM_IMAGES_SHRINKIMAGE,0),(LPARAM)0);
	return 0;
}

LRESULT CWndAfficheBitmap::OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	char cTemp[20];
	HMENU hMnu, hmenu;
	wmId    = LOWORD(wParam); 
	wmEvent = HIWORD(wParam); 

	
	// Parse the menu selections:

	switch (wmId)
	{
		case ID_OPTIONS_GPGPU:
			{
				if(!m_cNavigueData->GetGpGpu())
				{
					CheckMenuItem(GetMenu(GetParent()),ID_OPTIONS_GPGPU,MF_CHECKED);
					m_cNavigueData->SetGpGpu(true);
				}
				else
				{
					CheckMenuItem(GetMenu(GetParent()),ID_OPTIONS_GPGPU,MF_UNCHECKED);		
					m_cNavigueData->SetGpGpu(false);
				}
				break;
			}

		case ID_IMAGE_AFFICHERL:
			{
				m_cNavigueData->GetCBitmap()->SetShowHistogramme(!m_cNavigueData->GetCBitmap()->GetShowHistogramme());
				SendMessage(hWnd,WM_ERASEBKGND,0,MAKELPARAM(IDM_REDRAW_PICTURE,0));
				if(m_cNavigueData->GetCBitmap()->GetShowHistogramme())
					CheckMenuItem(GetMenu(GetParent()),ID_IMAGE_AFFICHAGEHISTOGRAMME,MF_CHECKED);
				else
					CheckMenuItem(GetMenu(GetParent()),ID_IMAGE_AFFICHAGEHISTOGRAMME,MF_UNCHECKED);
			}
			break;

		case ID_IMAGE_AFFICHERLESINFOSIMAGES:
			{
				m_cNavigueData->GetCBitmap()->SetShowInfos(!m_cNavigueData->GetCBitmap()->GetShowInfos());
				SendMessage(hWnd,WM_ERASEBKGND,0,MAKELPARAM(IDM_REDRAW_PICTURE,0));
				if(m_cNavigueData->GetCBitmap()->GetShowInfos())
					CheckMenuItem(GetMenu(GetParent()),ID_IMAGES_AFFICHERINFOS,MF_CHECKED);
				else
					CheckMenuItem(GetMenu(GetParent()),ID_IMAGES_AFFICHERINFOS,MF_UNCHECKED);
			}
			break;

		case ID_EDITION_COPIER:
			{
				CIBitmap * m_ciBitmap = new CIBitmap();
				m_ciBitmap->SetBitmap(m_cNavigueData->GetCIBitmap()->hBitmap,false);
				OpenClipboard(hWnd);
				EmptyClipboard();
				SetClipboardData(CF_BITMAP,m_ciBitmap->hBitmap);
				CloseClipboard();
				delete m_ciBitmap;
			}
			break;



		case ID_IMAGE_BOUGERIMAGE: 
			{
				InitDessin();
				
				m_cNavigueData->SetSelect(0);
				SetCursor (LoadCursor (NULL, IDC_ARROW));
				
				if(CObjet::GetWindowsVersion() == 0)
					Windows98Update(hWnd, message, wParam, lParam);
				else
					SendMessage(hWnd,WM_ERASEBKGND,0,MAKELPARAM(IDM_REDRAW_PICTURE,0));
			}
			break; 

		case ID_IMAGE_REINIT:
			{
				m_cNavigueData->GetCScroll()->DefineSize(0, 0,0,0);
				m_cNavigueData->GetCScroll()->Size(hWnd, 0,0);
				m_cNavigueData->GetCBitmap()->SetShrinkImage(true);
				SendMessage(hWnd,WM_ERASEBKGND,0,MAKELPARAM(IDM_CHGIMAGE_OK,0));
			}
			break;


		case ID_IMAGE_ZOOMERIMAGE: 
			{
				//InitDessin();

				m_cNavigueData->SetSelect(2);

				//SendMessage(hWnd,WM_ERASEBKGND,0,MAKELPARAM(IDM_CHGIMAGE_OK,0));

				SetCursor (LoadCursor (hInstance, MAKEINTRESOURCE(IDC_ZOOMON))) ;
			}
			break; 

		case ID_IMAGE_COUPERIMAGE: 
			{
				InitDessin();

				m_cNavigueData->GetCScroll()->DefineSize(0, 0,0,0);
				m_cNavigueData->GetCScroll()->Size(hWnd, 0,0);
				m_cNavigueData->GetCBitmap()->SetShrinkImage(true);

				SendMessage(hWnd,WM_ERASEBKGND,0,MAKELPARAM(IDM_CHGIMAGE_OK,0));

				m_cDessin = new CCrop();

				m_cNavigueData->SetSelect(1);

				//Initialisation du ratio
				SetDessinRatio(m_cNavigueData->GetCBitmap()->GetRatio());

				SetCursor (LoadCursor (hInstance, MAKEINTRESOURCE(IDC_CURSELECT))) ;
			}
			break; 


		case ID_FICHIER_IMPRESSIONDIRECT:
			{
				m_cNavigueData->GetCIBitmap()->SetFilename(m_cNavigueData->GetCFile());
				CPrint::DisplayPrintPropertySheet(hInstance, hWnd, m_MargeData,1);
			}
			break;

		case ID_FICHIER_APERUAVANTIMPRESSION:
			{
				CPrint::DisplayPreview(hInstance, hWnd, m_MargeData);
			}
			break;

		case ID_FICHIER_IMPRIMER:
			{
				m_cNavigueData->GetCIBitmap()->SetFilename(m_cNavigueData->GetCFile());
				CPrint::DisplayPrintPropertySheet(hInstance, hWnd, m_MargeData);
			}
			break;

		case EFFET_CANCEL:
		case ID_EDITION_ANNULER:

			if(m_iAction > 0)
				RemoveBitmap();

			break;

		case ID_EDITION_VIDAGEMEMOIRE:
			{
				VidageMemoire();
				hMnu = GetMenu(GetParent());
				hmenu = GetSubMenu(hMnu, 1); 
				char m_szMessage[255];
				LoadString(hInstance,IDS_CANCEL,m_szMessage,255);
				ModifyMenu(hmenu,0,MF_BYPOSITION,ID_EDITION_ANNULER,m_szMessage);
				EnableMenuItem(hMnu, ID_EDITION_ANNULER, MF_GRAYED);
			}
			break;


		case IDM_GETSHRINK:
			_itoa_s(m_cNavigueData->GetCBitmap()->GetShrinkState(),(char *)lParam, 2,2);
			break;

		case IDM_GETHWNDPRGBAR:
			{
				m_cNavigueData->GetCIBitmap()->hWndPrgBar = (HWND)lParam;
			}
			break;

		case IDM_FILENAME:
			{
				m_cNavigueData->SetCFile((char *)lParam);
				m_cNavigueData->GetCFichier()->DefinirRepertoire(m_cNavigueData->GetCFile());
				m_cNavigueData->GetCIBitmap()->SetFilename(m_cNavigueData->GetCFile());
				m_cNavigueData->GetCBitmap()->InitRatio();
				
				CObjet m_cObjet;
				int iFormat = m_cObjet.TestImageFormat(m_cNavigueData->GetCFile());
				//Test si format 12 et chargement en static
				if(iFormat == 12)
				{
					HCURSOR  hCur = LoadCursor(NULL, IDC_WAIT);
					hCur = SetCursor(hCur);
					m_cNavigueData->GetCIBitmap()->hWnd = hWnd;
					m_ChargementImage->ChargementImageStatic(m_cNavigueData->GetCIBitmap(),m_cNavigueData->GetCFile(),true);
					SetCursor(LoadCursor(NULL, IDC_ARROW));
				}
				else
					m_ChargementImage->ChargementImageStatic(m_cNavigueData->GetCIBitmap(),m_cNavigueData->GetCFile());

				m_cNavigueData->GetCBitmap()->SetBitmapSize(m_cNavigueData->GetCIBitmap()->GetBitmapWidth(),m_cNavigueData->GetCIBitmap()->GetBitmapHeigth());
				m_cNavigueData->GetCBitmap()->SetShrinkImage(true);
				m_cNavigueData->GetCBitmap()->InitScrolling();
				SendMessage(hWnd,WM_ERASEBKGND,MAKEWPARAM(m_cNavigueData->GetCIBitmap()->GetBitmapWidth(),m_cNavigueData->GetCIBitmap()->GetBitmapHeigth()),MAKELPARAM(IDM_FILTREIMAGE_OK,0));
			}
			break;
		
		case IDM_DIRNAME:
			m_cNavigueData->SetCDirectory((char *)lParam);
			break;

		case IDM_GETTWAINFILE:
			{
				AddBitmap();
				CIBitmap * m_ciTempBitmap =  (CIBitmap *)lParam;
				m_cNavigueData->GetCIBitmap()->SetBitmap(m_ciTempBitmap->GetPtBitmap(),m_ciTempBitmap->GetBitmapWidth(),m_ciTempBitmap->GetBitmapHeigth());
				ShowScrollBar(hWnd,SB_BOTH,false);
				m_cNavigueData->GetCBitmap()->SetShrinkImage(true);
				m_cNavigueData->GetCBitmap()->InitScrolling();
				m_cNavigueData->GetCBitmap()->SetBitmapSize(m_cNavigueData->GetCIBitmap()->GetBitmapWidth(),m_cNavigueData->GetCIBitmap()->GetBitmapHeigth());
				SendMessage(hWnd,WM_ERASEBKGND,MAKEWPARAM(m_cNavigueData->GetCIBitmap()->GetBitmapWidth(),m_cNavigueData->GetCIBitmap()->GetBitmapHeigth()),MAKELPARAM(IDM_FILTREIMAGE_OK,0));
			}
			break;

		case IDM_OPENFILE:
			char m_szFilename[MAX_PATH];
			m_cNavigueData->GetCFichier()->ChargerFichier(m_szFilename,MAX_PATH,hWnd);
			m_cNavigueData->SetCFile(m_szFilename);
			m_cNavigueData->GetCIBitmap()->SetFilename(m_szFilename);
			m_ChargementImage->ChargementImageStatic(m_cNavigueData->GetCIBitmap(),m_cNavigueData->GetCFile());
			m_cNavigueData->GetCBitmap()->InitScrolling();
			SendMessage(hWnd,WM_ERASEBKGND,MAKEWPARAM(m_cNavigueData->GetCIBitmap()->GetBitmapWidth(), m_cNavigueData->GetCIBitmap()->GetBitmapHeigth()),MAKELPARAM(IDM_FILTREIMAGE_OK,0));
			break;

		case IDM_IMAGES_SHRINKIMAGE:
			{


				if(m_cNavigueData->GetCBitmap()->GetShrinkState())
				{
					ShowScrollBar(hWnd,SB_BOTH,true);
					m_cNavigueData->GetCBitmap()->SetShrinkImage(false);
					SetDessinRatio(m_cNavigueData->GetCBitmap()->GetRatio());
					SendMessage(hWnd,WM_ERASEBKGND,MAKEWPARAM(m_cNavigueData->GetCIBitmap()->GetBitmapWidth(), m_cNavigueData->GetCIBitmap()->GetBitmapHeigth()),MAKELPARAM(IDM_FILTREIMAGE_OK,0));
				}
				else
				{
					m_cNavigueData->GetCScroll()->DefineSize(0, 0,0,0);
					m_cNavigueData->GetCScroll()->Size(hWnd, 0,0);
					m_cNavigueData->GetCBitmap()->SetShrinkImage(true);
					SendMessage(hWnd,WM_ERASEBKGND,0,MAKELPARAM(IDM_CHGIMAGE_OK,0));
				}

			}
			break;

		case IDM_FICHIER_REVERT:
			m_cNavigueData->GetCIBitmap()->SetFilename(m_cNavigueData->GetCFile());
			m_ChargementImage->ChargementImageStatic(m_cNavigueData->GetCIBitmap(),m_cNavigueData->GetCFile());
			m_cNavigueData->GetCBitmap()->InitScrolling();
			SendMessage(hWnd,WM_ERASEBKGND,MAKEWPARAM(m_cNavigueData->GetCIBitmap()->GetBitmapWidth(), m_cNavigueData->GetCIBitmap()->GetBitmapHeigth()),MAKELPARAM(IDM_FILTREIMAGE_OK,0));
			break;

		case IDM_FICHIER_SAUVERFICHIER:
			m_cNavigueData->GetCFichier()->SauverFichier(m_cNavigueData->GetCFile(),*m_cNavigueData->GetCIBitmap(),hWnd,NULL);
			break;

		case ID_FILTRE_AJOUTDEBRUIT:
			AddBitmap();
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),FILTRE_NOISE);
			break;
			
		case IDM_FILTRE_NOIRETBLANC:
			AddBitmap();
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),FILTRE_GRAYSCALE);
			break;

		case ID_MODE_SEPIA:
			AddBitmap();
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),FILTRE_SEPIA);
			break;

		case ID_IMAGE_MODE_NOIRETBLANC:
			AddBitmap();
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),FILTRE_BLACKANDWHITE);
			break;

		case ID_FILTRES_MEDIAN:
			AddBitmap();
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),FILTRE_MEDIAN);
			break;

		case ID_AJUSTEMENT_PHOTOFILTRE:
			{
				CPhotoFiltre * m_cPhotoFiltre = new CPhotoFiltre(hInstance);
				m_cPhotoFiltre->CreateDlg(hInstance,(LPCTSTR)IDD_DLGPHOTOFILTER,hWnd);
				if(m_cPhotoFiltre->GetReturnValue() == 1)
				{
					AddBitmap();
					COLORREF m_colorref = RGB(0,0,0);
					int m_iIntensity = 25;
					m_cPhotoFiltre->GetValue(m_colorref,m_iIntensity);
					m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),FILTRE_PHOTO, m_colorref, m_iIntensity);
				}
				delete m_cPhotoFiltre;
			}
			break;	

		case ID_AJUSTEMENT_NIVEAU:
			{
				CHistorgrammeDialog * m_cHistogramme = new CHistorgrammeDialog();
				m_cHistogramme->SetBitmap(m_cNavigueData->GetCIBitmap());
				m_cHistogramme->CreateDlg(hInstance,(LPCTSTR)IDD_DLG_HISTOGRAMME,hWnd);
				delete m_cHistogramme;
				break;
			}

		case ID_FILTRES_NUAGES:
			AddBitmap();
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),ID_FILTRES_NUAGES,RGB(128,128,128),RGB(0,0,0));
			if(m_cNavigueData->GetCBitmap()->GetReturnEffet() == 0)
				RemoveBitmap();
			break;

		case ID_FILTRES_FLOU_MOTIONBLUR:
			AddBitmap();
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),ID_FILTRES_FLOU_MOTIONBLUR);
			if(m_cNavigueData->GetCBitmap()->GetReturnEffet() == 0)
				RemoveBitmap();
			break;

		case ID_FILTRES_FILTERDEWIENER:
			AddBitmap();
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),ID_FILTRES_FILTERDEWIENER);
			if(m_cNavigueData->GetCBitmap()->GetReturnEffet() == 0)
				RemoveBitmap();
			break;


		case IDM_FILTRE_NEGATIF:
			//AddBitmap(hWnd,pNavigueData);
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),FILTRE_NEGATIF);
			break;

		case IDM_FILTRE_BLUR:
			AddBitmap();
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),FILTRE_BLUR);
			break;

		case ID_FILTRES_LENSFLARE:
			AddBitmap();
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),FILTRE_LENSFLARE);
			//if(m_cNavigueData->GetCBitmap()->GetReturnEffet() == 0)
			//	RemoveBitmap();
			break;

		case ID_FILTRES_SHARPENAVANC:
			AddBitmap();
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),FILTRE_SHARPENSTRONG);
			break;

		case ID_FILTRES_CERCLEANTI:
			AddBitmap();
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),ID_FILTRES_CERCLEANTI);
			break;

		case ID_FILTRE_ERODE:
			AddBitmap();
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),FILTRE_ERODE);
			break;

		case ID_FILTRE_DILATE:
			AddBitmap();
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),FILTRE_DILATE);
			break;

		case ID_FILTRES_SHARPEN:
			AddBitmap();
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),FILTRE_SHARPEN);
			break;

		case ID_FILTRES_EMBOSS:
			AddBitmap();
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),FILTRE_EMBOSS);
			break;

		case ID_FILTRES_EDGE:
			AddBitmap();
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),FILTRE_EDGE);
			break;

		case ID_FILTRES_SOFTEN:
			AddBitmap();
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),FILTRE_SOFTEN);
			break;

		case ID_FILTRES_MOSAQUE:
			AddBitmap();
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),FILTRE_MOSAIC,5);
			break;

		case ID_FILTRES_FLOUGAUSSIEN:
			AddBitmap();
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),FILTRE_GAUSSIANBLUR);
			break;

		case IDM_IMAGES_ROTATEIMAGE_180:
			//AddBitmap(hWnd,pNavigueData);
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),EFFET_ROTATION,180,0,0,0);
			break;

		case IDM_IMAGES_ROTATEIMAGE_90CW:
			//AddBitmap(hWnd,pNavigueData);
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),EFFET_ROTATION,90,0,0,0);
			break;

		case IDM_IMAGES_ROTATEIMAGE_90CCW:
			//AddBitmap(hWnd,pNavigueData);
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),EFFET_ROTATION,270,0,0,0);
			break;

		case ID_IMAGE_ROTERIMAGE_ROTATIONLIBRE:
			AddBitmap();
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),ID_IMAGE_ROTERIMAGE_ROTATIONLIBRE,0,0,0,0);
			if(m_cNavigueData->GetCBitmap()->GetReturnEffet() == 0)
			{
				RemoveBitmap();
			}
			break;

		case IDM_IMAGES_ROTATEIMAGE_FLIPHORIZONTAL:
			//AddBitmap(hWnd,pNavigueData);
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),EFFET_FLIP,1,0,0,0);
			break;

		case IDM_IMAGES_ROTATEIMAGE_FLIPVERTICAL:
			//AddBitmap(hWnd,pNavigueData);
			m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetGpGpu(),EFFET_FLIP,0,0,0,0);
			break;

		case IDM_IMAGES_ZOOM_ZOOMIN:
			{
				if(wmEvent != 5678)
				{
					RECT rc;
					GetClientRect(hWnd,&rc);
					int x = rc.right / 2;
					int y = rc.bottom / 2;
					ZoomIn(hWnd,MAKELPARAM(x,y));
				}
				else
					ZoomIn(hWnd,lParam);

				
				float fRatio = m_cNavigueData->GetCBitmap()->GetRatio();
				char m_szRatio[500];
				StringCchPrintf(m_szRatio,500,"%s - ratio %f",m_cNavigueData->GetCFile(),fRatio);
				::SetWindowText(::GetParent(hWnd),m_szRatio);

				switch(m_cNavigueData->GetSelect())
				{
					case 1:
						{
							SetDessinRatio(fRatio);
						}
						break;
				}

				if(CObjet::GetWindowsVersion() == 0)
					Windows98Update(hWnd, message, wParam, lParam);
			}
			break;

		case IDM_IMAGES_ZOOM_ZOOMOUT:
			{

				if(wmEvent != 5678)
				{
					RECT rc;
					GetClientRect(hWnd,&rc);
					int x = rc.right / 2;
					int y = rc.bottom / 2;
					ZoomOut(hWnd,MAKELPARAM(x,y));
				}
				else
					ZoomOut(hWnd,lParam);

				float fRatio = m_cNavigueData->GetCBitmap()->GetRatio();
				char m_szRatio[500];
				StringCchPrintf(m_szRatio,500,"%s - ratio %f",m_cNavigueData->GetCFile(),fRatio);
				::SetWindowText(::GetParent(hWnd),m_szRatio);

				switch(m_cNavigueData->GetSelect())
				{
					case 1:
						{
							SetDessinRatio(fRatio);
						}
						break;
				}

				if(CObjet::GetWindowsVersion() == 0)
					Windows98Update(hWnd, message, wParam, lParam);
			}
			break;

		case IDM_FILTRE_SCALE:
			{
			
				CResizeWindow * m_cResizeWindow = new CResizeWindow();
				m_cResizeWindow->SetIBitmap(m_cNavigueData->GetCIBitmap());
				m_cResizeWindow->CreateDlg(hInstance,(LPCTSTR)IDD_REDIMIMAGE, hWnd);
				int m_iResult = m_cResizeWindow->GetReturnValue();
				if(m_iResult == 1)
				{
					int m_iFiltre = m_cResizeWindow->GetFiltre();
					int m_iWidth = m_cResizeWindow->GetWidth();
					int m_iHeight = m_cResizeWindow->GetHeight();
					SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_APPLIQUER_SCALE,m_iFiltre),MAKELPARAM(m_iWidth,m_iHeight));
				}

				delete m_cResizeWindow;

			}
			break;

		case ID_IMAGE_REDIMMENSIONNERLAFEUILLEDETRAVAIL:
			{
				AddBitmap();
				m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(),EFFET_CROP,0);

			}
			break;

		case IDM_FILTRE_BRIGHTNESSCONTRAST:
			{

				AddBitmap();

				CBrightContDialog * m_cBrightContDialog = new CBrightContDialog();
				m_cBrightContDialog->SetBitmapHandleSrc( m_cNavigueData->GetCBitmap());
				m_cBrightContDialog->SetBitmapSource(m_cNavigueData->GetCIBitmap());
				m_cBrightContDialog->SetHWND(hWnd);
				m_cBrightContDialog->CreateDlg(hInstance,(LPCTSTR)IDD_BRIGHTCONT, hWnd);
				int i = m_cBrightContDialog->GetReturnValue();
		
				if(i == 0)
					RemoveBitmap();

				delete m_cBrightContDialog;
			}
			break;

		case ID_IMAGE_AJUSTEMENT_BALANCEDESCOULEURS:
			{
				AddBitmap();
				CRGBDialog * m_cRgbDialog = new CRGBDialog();
				m_cRgbDialog->SetCBitmap(m_cNavigueData->GetCBitmap(),m_cNavigueData->GetCIBitmap());
				m_cRgbDialog->CreateDlg(hInstance,(LPCTSTR)IDD_RGB,hWnd);
				if(m_cRgbDialog->GetReturnCode() == 0)
					RemoveBitmap();
				else
					SendMessage(m_hWnd,WM_ERASEBKGND,MAKEWPARAM(m_cNavigueData->GetCIBitmap()->GetBitmapWidth(), m_cNavigueData->GetCIBitmap()->GetBitmapHeigth()),MAKELPARAM(IDM_ZOOM_OK,0));

				delete m_cRgbDialog;
			}
			break;


		case IDM_APPLIQUER_SCALE:
			{
				AddBitmap();
				CEffet m_cEffet;
				m_cEffet.ScaleBitmap(m_cNavigueData->GetCIBitmap(),LOWORD(lParam),HIWORD(lParam),wmEvent);
				SendMessage(m_hWnd,WM_ERASEBKGND,MAKEWPARAM(m_cNavigueData->GetCIBitmap()->GetBitmapWidth(), m_cNavigueData->GetCIBitmap()->GetBitmapHeigth()),MAKELPARAM(IDM_ZOOM_OK,0));
				SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(IDM_UPDATESTATUS,0), MAKELPARAM(LOWORD(lParam),HIWORD(lParam)));
			}
			break;


		default:
		   return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

LRESULT CWndAfficheBitmap::OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	m_ChargementImage = new CLoadImage();
	m_cNavigueData->GetCIBitmap()->hWndPrgBar = NULL;
	m_iAction = 0;

	m_MargeData = new CMargeData();

	m_MargeData->m_fBas = DEFAUTPRINTDISTANCE;
	m_MargeData->m_fHaut = DEFAUTPRINTDISTANCE;
	m_MargeData->m_fGauche = DEFAUTPRINTDISTANCE;
	m_MargeData->m_fDroite = DEFAUTPRINTDISTANCE;
	m_MargeData->m_ciBitmap = m_cNavigueData->GetCIBitmap();
	m_cNavigueData->SetSelect(0);

	ProcessCreate(hWnd, message, wParam, lParam);

	m_cNavigueData->GetCBitmap()->SetHauteQualite(TRIANGLEFILTER);

	

	

	CGpGpuEffect * m_gpGpuEffect = new CGpGpuEffect();
	if(m_gpGpuEffect->Initialize())
	{
		char Temp[255];
		::GetWindowText(::GetParent(hWnd),Temp,255);
		sprintf_s(Temp,"%s - GPU Acceleration Enabled",Temp);
		::SetWindowText(::GetParent(hWnd),Temp);

	}
	delete m_gpGpuEffect;
	return 0;
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


LRESULT CWndAfficheBitmap::OnKeyUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (wParam) 
	{
		case VK_CONTROL:
			switch(m_cNavigueData->GetSelect())
			{
				case 2:
					SetCursor (LoadCursor (hInstance, MAKEINTRESOURCE(IDC_ZOOMON))) ;
					break;

			}
			break;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

LRESULT CWndAfficheBitmap::OnKeyDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (wParam) 
	{
		case VK_RETURN:
			//Valide
			switch(m_cNavigueData->GetSelect())
			{
				case 1:
					{

						//Gestion du ratio à faire

						CEffet m_cEffet;
						RECT rc;

						AddBitmap();

						m_cDessin->GetPos(rc);

						m_cEffet.CropBitmap(m_cNavigueData->GetCIBitmap(),rc);

						SetCursor (LoadCursor (NULL, IDC_ARROW));
						
						this->InitDessin();

						m_cNavigueData->SetSelect(0);

						SendMessage(hWnd,WM_ERASEBKGND,MAKEWPARAM(m_cNavigueData->GetCIBitmap()->GetBitmapWidth(),m_cNavigueData->GetCIBitmap()->GetBitmapHeigth()),MAKELPARAM(IDM_FILTREIMAGE_OK,0));



					}
					break;
			}
			break;

		case VK_ESCAPE:
			//Annule
			switch(m_cNavigueData->GetSelect())
			{
				case 1:
					//m_cNavigueData->GetSelect() = 0;
					m_cDessin->InitPoint(0,0,0,0);
					SendMessage(hWnd,WM_ERASEBKGND,0,0);
					break;
			}
			break;

		case VK_CONTROL:
			switch(m_cNavigueData->GetSelect())
			{
				case 2:
					SetCursor (LoadCursor (hInstance, MAKEINTRESOURCE(IDC_ZOOMOUT))) ;
					break;

			}
			break;
		

		default:
			MouseAndKeyCreate(hWnd, message, wParam, lParam);
			switch(m_cNavigueData->GetSelect())
			{
				case 1:
					SendMessage(hWnd,WM_ERASEBKGND,0,0);
					break;
			}
			return 0;

	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndAfficheBitmap::OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	RECT rc;
	HDC hDC = BeginPaint( hWnd, &ps );
	GetClientRect(hWnd,&rc);
	
	m_cNavigueData->GetCIBitmap()->SetFilename(m_cNavigueData->GetCFile());

	switch(m_cNavigueData->GetSelect())
	{
		case 1:
			//EraseBgrCreate(hWnd, message, wParam, lParam, pNavigueData);
			m_cNavigueData->GetCBitmap()->AffichageBitmap(hDC,rc, m_cNavigueData->GetCIBitmap(),m_cNavigueData->GetCScroll()->GetCurrentHScroll(), m_cNavigueData->GetCScroll()->GetCurrentVScroll());
			m_cDessin->Dessiner(hDC,m_cNavigueData->GetCScroll()->GetCurrentHScroll(), m_cNavigueData->GetCScroll()->GetCurrentVScroll());
			break;

		default:
			m_cNavigueData->GetCBitmap()->AffichageBitmap(hDC,rc, m_cNavigueData->GetCIBitmap(),m_cNavigueData->GetCScroll()->GetCurrentHScroll(), m_cNavigueData->GetCScroll()->GetCurrentVScroll());
			break;
	}

	EndPaint(hWnd, &ps);

	ReleaseDC(hWnd,hDC);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CWndAfficheBitmap::OnTimer(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case TIMER_UPDATEPICTURE:
		KillTimer(hWnd,TIMER_UPDATEPICTURE);
		m_cNavigueData->GetCIBitmap()->SetOnSize(false);
		SendMessage(hWnd,WM_ERASEBKGND,0,MAKELPARAM(IDM_REDRAW_PICTURE,0));
		break;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndAfficheBitmap::OnSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	m_cNavigueData->GetCIBitmap()->SetOnSize(true);
	KillTimer(hWnd,TIMER_UPDATEPICTURE);
	int iReturn = SizeCreate(hWnd, message, wParam, lParam);
	SetTimer(hWnd,TIMER_UPDATEPICTURE,100,0);
	return iReturn;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndAfficheBitmap::OnEraseBackground(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	EraseBgrCreate(hWnd, message, wParam, lParam);
	switch(m_cNavigueData->GetSelect())
	{
		case 1:
			HDC hDC = GetDC(hWnd);
			m_cDessin->Dessiner(hDC,m_cNavigueData->GetCScroll()->GetCurrentHScroll(), m_cNavigueData->GetCScroll()->GetCurrentVScroll());
			ReleaseDC(hWnd,hDC);
			break;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CWndAfficheBitmap::OnRButtonDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(m_cNavigueData->GetSelect())
	{

		case 2:
			{

				OnCommand(hWnd,WM_COMMAND,MAKEWPARAM(IDM_IMAGES_ZOOM_ZOOMOUT,0),0);
				break;
			}
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndAfficheBitmap::OnLButtonDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(m_cNavigueData->GetSelect())
	{
		case 1:
			{
				int yNewSize, xNewSize;
			   	yNewSize = HIWORD (lParam); 
				xNewSize = LOWORD (lParam); 
				m_cNavigueData->GetCBitmap()->SetInvertSensPhoto(true);
				m_cNavigueData->GetCBitmap()->SetMouseBlock(true);
				m_cNavigueData->GetCBitmap()->SetMouseScroll(LOWORD(lParam), HIWORD(lParam));
				SetCapture(hWnd);
				m_cDessin->InitPoint(xNewSize,yNewSize,m_cNavigueData->GetCScroll()->GetCurrentHScroll(),m_cNavigueData->GetCScroll()->GetCurrentVScroll());
				SendMessage(hWnd,WM_ERASEBKGND,0,0);
			}
			break;

		case 2:
			{
				if(HIWORD(GetKeyState(VK_CONTROL)) != 0)
				{
					SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_IMAGES_ZOOM_ZOOMOUT,5678),lParam);
				}
				else
				{
					SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_IMAGES_ZOOM_ZOOMIN,5678),lParam);
				}
			}

		default:
			return MouseAndKeyCreate(hWnd, message, wParam, lParam);
			break;
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndAfficheBitmap::OnLButtonUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(m_cNavigueData->GetSelect())
	{
		case 1:
			{
				m_cNavigueData->GetCBitmap()->SetMouseBlock(false);
				m_cNavigueData->GetCBitmap()->SetMouseScroll(0, 0);
				SetCursor (LoadCursor (NULL, IDC_ARROW)) ;
				ReleaseCapture();
				m_cNavigueData->GetCBitmap()->SetInvertSensPhoto(false);
			}
			break;

		//case 2:
		//	break;

		default:
			return MouseAndKeyCreate(hWnd, message, wParam, lParam);
			break;
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndAfficheBitmap::OnMouseWheel(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MouseAndKeyCreate(hWnd, message, wParam, lParam);
	switch(m_cNavigueData->GetSelect())
	{
		case 1:
			SendMessage(hWnd,WM_ERASEBKGND,0,0);
			break;
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndAfficheBitmap::OnVScroll(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return OnMouseWheel(hWnd,message,wParam,lParam);
}
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndAfficheBitmap::OnHScroll(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return OnMouseWheel(hWnd,message,wParam,lParam);
}



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void CWndAfficheBitmap::SetDessinRatio(const float &m_fRatio)
{
	RECT rc, rc2;
	int iLeft,iTop;
	int iTailleWidth, iTailleHeight;

	GetClientRect(m_hWnd,&rc2);

	if(m_cNavigueData->GetCBitmap()->GetShrinkState())
	{
		float m_fLocalRatio = m_cNavigueData->GetCBitmap()->GetRatioScreen(*m_cNavigueData->GetCIBitmap(),rc2);

		iTailleWidth = m_cNavigueData->GetCIBitmap()->GetBitmapWidth() * m_fLocalRatio;
		iTailleHeight = m_cNavigueData->GetCIBitmap()->GetBitmapHeigth() * m_fLocalRatio;

		if(m_cDessin != NULL)
			m_cDessin->SetRatio(m_fLocalRatio);

	}
	else
	{
		iTailleWidth = m_cNavigueData->GetCIBitmap()->GetBitmapWidth() * m_fRatio;
		iTailleHeight = m_cNavigueData->GetCIBitmap()->GetBitmapHeigth() * m_fRatio;

		if(m_cDessin != NULL)
			m_cDessin->SetRatio(m_fRatio);
	}

	//Récupération des données sur l'image


	m_cNavigueData->GetCBitmap()->DeterminePos(rc2,iTailleWidth,iTailleHeight,iLeft,iTop);

	rc.left = iLeft;
	rc.right = iLeft + iTailleWidth;
	rc.top = iTop;
	rc.bottom = iTop + iTailleHeight;

	if(m_cDessin != NULL)
		m_cDessin->SetMaxPosition(rc);

	SendMessage(m_hWnd,WM_ERASEBKGND,0,0);
}
