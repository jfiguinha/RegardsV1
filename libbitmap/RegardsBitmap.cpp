// Bitmap.cpp: implementation of the CBitmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RegardsBitmap.h"
#include "BitmapPrv.h"
#include <effet.h>
#include <filtre.h>
#include <ibitmap.h>
#include <color.h>
#include "DialogFiltreEffet.h"
#include <objet.h>
#include <resource.h>
#include <RedimWorkSpace.h>
#include <EffetDialog.h>
#include <FiltreDialog.h>
#include <Histogramme.h>
#include "BitmapEffect.h"

using namespace Regards::libbitmap;

//Tableau contenant les ratios
const double fTabRatio[] = {0.0006,0.001,0.002,0.003,0.004,0.005,0.007,0.01,0.015,0.02,0.03,0.04,0.05,0.06,0.0833,0.125,0.167,0.25,0.333,0.5,0.667,1,1.33,1.5,1.66,2,3,4,5,6,7,8,12,16};
const long lMax = 33;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBitmap::CBitmap(HINSTANCE hInstance)
{
	pimpl_ = new CBitmapPrv();
	m_hInstance = hInstance;
	m_iRetourEffet = 1;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBitmap::CBitmap()
{
	pimpl_ = new CBitmapPrv();
	m_hInstance = NULL;
}

CBitmap::~CBitmap()
{
	delete pimpl_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBitmap::DeterminePos(RECT &rc, int &left, int &top)
{
	if(rc.right > pimpl_->m_BitmapData.nTailleAffichageWidth)
		left = ((rc.right - pimpl_->m_BitmapData.nTailleAffichageWidth) / 2);
	else
	{
		if(pimpl_->m_BitmapData.m_bPrinter)
			left = rc.left;
		else
			left = 0;
	}

	if(rc.bottom > pimpl_->m_BitmapData.nTailleAffichageHeight)
	{
		top = ((rc.bottom - pimpl_->m_BitmapData.nTailleAffichageHeight) / 2);
	}
	else
	{
		if(pimpl_->m_BitmapData.m_bPrinter)
			top = rc.top;
		else
			top = 0;
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBitmap::DeterminePos(RECT &rc,const int &nTailleAffichageWidth, const int &nTailleAffichageHeight, int &left, int &top)
{
	if(rc.right > nTailleAffichageWidth)
		left = ((rc.right - nTailleAffichageWidth) / 2);
	else
		left = 0;

	if(rc.bottom > nTailleAffichageHeight)
		top = ((rc.bottom - nTailleAffichageHeight) / 2);
	else
		top = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBitmap::AffichageBitmap(HDC &hdc,RECT &rc,CIBitmap * m_ciBitmap, const int &xCurrentScroll, const int &yCurrentScroll)
{
	if(m_ciBitmap->GetUse())
		return;

	int xValue = xCurrentScroll, yValue = yCurrentScroll;
	int left, top;

	HDC sourceDC;

	if(m_ciBitmap->hBitmap == NULL && m_ciBitmap->GetPtBitmap() == NULL)
		return;

	if(pimpl_->m_BitmapData.xNewSize != 0 && pimpl_->m_BitmapData.yNewSize != 0)
	{
		rc.right = pimpl_->m_BitmapData.xNewSize;
		rc.bottom = pimpl_->m_BitmapData.yNewSize;
	}


	if(pimpl_->m_BitmapData.m_bAutoShrink && pimpl_->m_BitmapData.fratio == 1.0 && (m_ciBitmap->GetBitmapWidth() > rc.right || m_ciBitmap->GetBitmapHeigth() > rc.bottom))
	{
		sourceDC = CreateCompatibleDC(hdc);

		float fLocalratio;
		xValue = 0, yValue = 0;

		if(m_ciBitmap->GetBitmapWidth() > m_ciBitmap->GetBitmapHeigth())
			fLocalratio = (float)rc.right / (float)(m_ciBitmap->GetBitmapWidth());
		else
			fLocalratio = (float)rc.bottom / (float)(m_ciBitmap->GetBitmapHeigth());

		pimpl_->m_BitmapData.nTailleAffichageWidth = m_ciBitmap->GetBitmapWidth() * fLocalratio;
		pimpl_->m_BitmapData.nTailleAffichageHeight =m_ciBitmap->GetBitmapHeigth() * fLocalratio;

		if(pimpl_->m_BitmapData.nTailleAffichageHeight > rc.bottom)
		{
			fLocalratio = (float)rc.bottom / (float)(m_ciBitmap->GetBitmapHeigth());
			pimpl_->m_BitmapData.nTailleAffichageWidth = m_ciBitmap->GetBitmapWidth() * fLocalratio;
			pimpl_->m_BitmapData.nTailleAffichageHeight = m_ciBitmap->GetBitmapHeigth() * fLocalratio;
		}
		else
		{
			if(pimpl_->m_BitmapData.nTailleAffichageWidth > rc.right)
			{
				fLocalratio = (float)rc.right / (float)(m_ciBitmap->GetBitmapWidth());
				pimpl_->m_BitmapData.nTailleAffichageWidth = m_ciBitmap->GetBitmapWidth() * fLocalratio;
				pimpl_->m_BitmapData.nTailleAffichageHeight = m_ciBitmap->GetBitmapHeigth() * fLocalratio;
			}
		}

		//Détermination du ration par rapport au tableau
		
		if(fLocalratio != 0.0)
		{
			for(int i = 0;i < lMax;i++)
			{
				if(fLocalratio < fTabRatio[i])
				{
					pimpl_->m_BitmapData.lPtTabRatio = i-1;
					break;
				}
			}
		}

		DeterminePos(rc,left,top);

		HBITMAP hBitmap;
		hBitmap = CreateCompatibleBitmap(hdc,rc.right,rc.bottom);

		HBITMAP hbmOldSource = (HBITMAP)::SelectObject( sourceDC, hBitmap);

		m_ciBitmap->ShowBitmap(sourceDC, left, top, xValue, yValue,pimpl_->m_BitmapData.m_bHauteQualite,
			pimpl_->m_BitmapData.m_bAutoShrink, pimpl_->m_BitmapData.nTailleAffichageWidth, pimpl_->m_BitmapData.nTailleAffichageHeight,
			pimpl_->m_BitmapData.fratio);

		pimpl_->CompleteBitmap(sourceDC, rc, left, top, xValue, yValue);

		BitBlt(hdc,0,0, rc.right,rc.bottom, sourceDC, 0, 0,SRCCOPY);

		DeleteDC(sourceDC);
		DeleteObject(hbmOldSource);
		DeleteObject(hBitmap);
	}
	else
	{
		sourceDC = CreateCompatibleDC(hdc);
		HBITMAP hBitmap;
		hBitmap = CreateCompatibleBitmap(hdc,rc.right,rc.bottom);

		HBITMAP hbmOldSource = (HBITMAP)::SelectObject( sourceDC, hBitmap);

		if(pimpl_->m_BitmapData.fScroll)
		{
				xValue += rc.left; 
				yValue += rc.top;
		}

		if (pimpl_->m_BitmapData.fSize || pimpl_->m_BitmapData.fScroll) 
		{
			pimpl_->m_BitmapData.nTailleAffichageWidth = m_ciBitmap->GetBitmapWidth() * pimpl_->m_BitmapData.fratio;
			pimpl_->m_BitmapData.nTailleAffichageHeight = m_ciBitmap->GetBitmapHeigth() * pimpl_->m_BitmapData.fratio;

			DeterminePos(rc,left,top);
		
			m_ciBitmap->ShowBitmap(sourceDC, left, top, xValue, yValue,pimpl_->m_BitmapData.m_bHauteQualite,
				pimpl_->m_BitmapData.m_bAutoShrink, pimpl_->m_BitmapData.nTailleAffichageWidth, pimpl_->m_BitmapData.nTailleAffichageHeight,
				pimpl_->m_BitmapData.fratio);


			if(pimpl_->m_BitmapData.fSize)
				pimpl_->CompleteBitmap(sourceDC, rc, left, top, xValue, yValue);

			BitBlt(hdc,0,0, rc.right,rc.bottom, sourceDC, 0, 0,SRCCOPY);
		}

		DeleteDC(sourceDC);
		DeleteObject(hbmOldSource);
		DeleteObject(hBitmap);

	}

	///////////////////////////////////////////////
	//Test si l'image est trop grande
	///////////////////////////////////////////////

	//char Temp[255];
	//m_ciBitmap->GetFilename(Temp);

	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBitmap::AffichageBitmap(HWND hWnd,CIBitmap * m_ciBitmap, const int &xCurrentScroll, const int &yCurrentScroll)
{
	RECT rc;
	HDC hdc;
	hdc = GetDC(hWnd);
	GetClientRect(hWnd,&rc);
	AffichageBitmap(hdc, rc,m_ciBitmap, xCurrentScroll, yCurrentScroll);
	if(pimpl_->m_BitmapData.m_bShowInfos)
	{
#ifdef _DEBUG
	OutputDebugString("ShowInfos \n");
#endif
		CBitmapEffect::ShowInfos(hWnd,hdc,m_ciBitmap);
	}
	if(pimpl_->m_BitmapData.m_bShowHistogramme)
	{
		CBitmapEffect::ShowHistogramme(hWnd,hdc,m_ciBitmap, rc);
	}
	ReleaseDC(hWnd,hdc);
}

void CBitmap::SetShowInfos(bool bValue)
{
	pimpl_->m_BitmapData.m_bShowInfos = bValue;
}

bool CBitmap::GetShowInfos()
{
	return pimpl_->m_BitmapData.m_bShowInfos;
}

void CBitmap::SetShowHistogramme(bool bValue)
{
	pimpl_->m_BitmapData.m_bShowHistogramme = bValue;
}

bool CBitmap::GetShowHistogramme()
{
	return pimpl_->m_BitmapData.m_bShowHistogramme;
}

/////////////////////////////////////////////////////////////////////////
//Application d'un filtre au bitmap ou d'un effet
/////////////////////////////////////////////////////////////////////////

int CBitmap::AppliquerFiltre(HWND &hwndLocal, CIBitmap * m_ciBitmap, const bool &gpGpu, const int &NumFiltre, const long &paramatre, const long &paramatre2, const bool &bAffichage, const int &iDialog)//, HBITMAP &hBitmapTemp)
{
	CFiltreEffet m_filtre;
	m_filtre.m_ciBitmap = m_ciBitmap;
	m_filtre.NumFiltre = NumFiltre;
	m_filtre.m_lParam = new long[5];
	m_filtre.m_lParam[0] = paramatre;
	m_filtre.m_lParam[1] = paramatre2;
	m_filtre.m_bGpGpu = gpGpu;
	m_filtre.iHandlePrgBar = IDC_PRGBAR;
	m_filtre.iAffichage = iDialog;
	m_iRetourEffet = 1;

	
 	switch(NumFiltre)
 	{
 		case EFFET_ROTATION:
			{
				m_iRetourEffet = CBitmapEffect::EffetRotation(m_hInstance, hwndLocal, m_ciBitmap, m_filtre, pimpl_->m_BitmapData.fratio);
				return 0;
			}
 			break;

 		case EFFET_FLIP:
			{
				m_iRetourEffet = CBitmapEffect::EffetFlip(m_hInstance, hwndLocal, m_ciBitmap, m_filtre);
 				return 0;;
			}

		case EFFET_SCALE:
 		case EFFET_LOWSCALE:
			{
 				pimpl_->m_BitmapData.fratio = 1.0;
				//iRetour = DialogBoxParam(m_hInstance,(LPCTSTR)IDD_APPLITRAITEMENT,hwndLocal,(DLGPROC)CDialogFiltreEffet::EffetDialog,(LPARAM)&m_filtre); 
				m_iRetourEffet = CBitmapEffect::EffetScale(m_hInstance, hwndLocal, m_ciBitmap, m_filtre);
				return 0;
			}
 			break;

		case EFFET_CROP:
			{
				m_iRetourEffet = CBitmapEffect::EffetCrop(m_hInstance, hwndLocal, m_ciBitmap, m_filtre);
 				return 0;
			}
			break;

		case ID_AJUSTEMENT_PHOTOFILTRE:
			{
				CFiltre m_cFiltre;
				m_cFiltre.AppliquerFiltre(&m_filtre);
				return 0;
			}

		case ID_FILTRES_NUAGES:
			{
				m_iRetourEffet = CBitmapEffect::EffetNuages(m_hInstance, hwndLocal, m_ciBitmap, m_filtre);
			}
			break;

		case ID_FILTRES_FLOU_MOTIONBLUR:
			{
				m_iRetourEffet = CBitmapEffect::EffetMotionBlur(m_hInstance, hwndLocal, m_ciBitmap, m_filtre);
			}
			break;

		case ID_FILTRES_FILTERDEWIENER:
			{
				//Test des paramètres de l'image
				m_iRetourEffet = CBitmapEffect::EffetWiener(m_hInstance, hwndLocal, m_ciBitmap, m_filtre);
			}
			break;

		case ID_IMAGE_ROTERIMAGE_ROTATIONLIBRE:
			{
				m_iRetourEffet = CBitmapEffect::EffetRotationLibre(m_hInstance, hwndLocal, m_ciBitmap, m_filtre, pimpl_->m_BitmapData.fratio);
			}
			break;

		case FILTRE_LENSFLARE:
			{
				m_iRetourEffet = CBitmapEffect::EffetLensFlare(m_hInstance, hwndLocal, m_ciBitmap, m_filtre);
			}
			break;

		case ID_FILTRES_CERCLEANTI:
			{
				CFiltre m_cFiltre;
				m_cFiltre.Cercle(NULL,m_ciBitmap);
			}
			break;


 		default:
			{
				m_iRetourEffet = CBitmapEffect::EffetDefault(m_hInstance, hwndLocal, m_ciBitmap, m_filtre);
			}
 			break;

 	}
 		
	if(bAffichage)
		SendMessage(hwndLocal,WM_ERASEBKGND,0,MAKELPARAM(IDM_REDRAW_PICTURE,0));

	delete[] m_filtre.m_lParam;

 	return 0;
 }

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//Zoom Arrière sur une image
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBitmap::DefineRatioInf(HWND &hWnd,const int &nTailleAffichageWidth, const int &nTailleAffichageHeight)
{
	pimpl_->m_BitmapData.lPtTabRatio--;

	if(pimpl_->m_BitmapData.lPtTabRatio < 0)
		pimpl_->m_BitmapData.lPtTabRatio = 0;

	pimpl_->m_BitmapData.fratio = fTabRatio[pimpl_->m_BitmapData.lPtTabRatio];

	pimpl_->m_BitmapData.nTailleAffichageWidth = (int)((double)nTailleAffichageWidth * (double)pimpl_->m_BitmapData.fratio);
	pimpl_->m_BitmapData.nTailleAffichageHeight =(int)((double)nTailleAffichageHeight * (double)pimpl_->m_BitmapData.fratio);

	if(pimpl_->m_BitmapData.fratio != 1.0)
		pimpl_->m_BitmapData.m_bAutoShrink = false;

	//Calcul Nouvel position du curseur
	pimpl_->m_BitmapData.nlPosLargeur = pimpl_->m_BitmapData.fratio * (((pimpl_->m_BitmapData.nlPosLargeur) / fTabRatio[pimpl_->m_BitmapData.lPtTabRatio + 1]));
	pimpl_->m_BitmapData.nlPosHauteur = pimpl_->m_BitmapData.fratio * (((pimpl_->m_BitmapData.nlPosHauteur) / fTabRatio[pimpl_->m_BitmapData.lPtTabRatio + 1]));



#ifdef _DEBUG
	char Temp[50];
	StringCchPrintf(Temp,50,"Pos x : %d x : %d \n",pimpl_->m_BitmapData.nlPosLargeur,pimpl_->m_BitmapData.nlPosHauteur);
	OutputDebugString(Temp);
#endif

	SendMessage(hWnd,WM_ERASEBKGND,MAKEWPARAM(pimpl_->m_BitmapData.nTailleAffichageWidth, pimpl_->m_BitmapData.nTailleAffichageHeight),MAKELPARAM(IDM_ZOOM_OK,IDM_ZOOM_OK));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//Zoom avant sur une image
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBitmap::DefineRatioSup(HWND &hWnd,const int &nTailleAffichageWidth, const int &nTailleAffichageHeight)
{
	//int nTailleAffichageWidth, nTailleAffichageHeight;

	pimpl_->m_BitmapData.lPtTabRatio++;

	if(pimpl_->m_BitmapData.lPtTabRatio > lMax)
		pimpl_->m_BitmapData.lPtTabRatio = lMax;

	pimpl_->m_BitmapData.fratio = fTabRatio[pimpl_->m_BitmapData.lPtTabRatio];

	pimpl_->m_BitmapData.nTailleAffichageWidth = (int)((double)nTailleAffichageWidth * (double)pimpl_->m_BitmapData.fratio);
	pimpl_->m_BitmapData.nTailleAffichageHeight =(int)((double)nTailleAffichageHeight * (double)pimpl_->m_BitmapData.fratio);

	if(pimpl_->m_BitmapData.fratio != 1.0)
		pimpl_->m_BitmapData.m_bAutoShrink = false;

	pimpl_->m_BitmapData.nlPosLargeur = pimpl_->m_BitmapData.fratio * (pimpl_->m_BitmapData.nlPosLargeur / fTabRatio[pimpl_->m_BitmapData.lPtTabRatio - 1]);
	pimpl_->m_BitmapData.nlPosHauteur = pimpl_->m_BitmapData.fratio * (pimpl_->m_BitmapData.nlPosHauteur / fTabRatio[pimpl_->m_BitmapData.lPtTabRatio - 1]);

#ifdef _DEBUG
	char Temp[50];
	StringCchPrintf(Temp,50,"Pos x : %d x : %d \n",pimpl_->m_BitmapData.nlPosLargeur,pimpl_->m_BitmapData.nlPosHauteur);
	OutputDebugString(Temp);
#endif


	if(pimpl_->m_BitmapData.nTailleAffichageWidth > 64000 || pimpl_->m_BitmapData.nTailleAffichageHeight > 64000)
		DefineRatioInf(hWnd,nTailleAffichageWidth, nTailleAffichageHeight);
	else
		SendMessage(hWnd,WM_ERASEBKGND,MAKEWPARAM(pimpl_->m_BitmapData.nTailleAffichageWidth, pimpl_->m_BitmapData.nTailleAffichageHeight),MAKELPARAM(IDM_ZOOM_OK,IDM_ZOOM_OK));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//Récupération du ratio d'affichage
///////////////////////////////////////////////////////////////////////////////////////////////////////////

float CBitmap::GetRatio()
{
	return pimpl_->m_BitmapData.fratio;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//Indique que l'on souhaite contraindre l'image à la taille actuelle de l'écran
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBitmap::SetShrinkImage(const bool &m_bShrink)
{
	pimpl_->m_BitmapData.m_bAutoShrink = m_bShrink;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//Indique que l'on affichage le bitmap en plein écran
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBitmap::SetFullScreen(const bool &m_bValue)
{
	pimpl_->m_BitmapData.m_bFullScreen = m_bValue;
}

////////////////////////////////////////////////////////////////////////////////
//Affectation des positions des ascenseurs
////////////////////////////////////////////////////////////////////////////////

void CBitmap::SetPosition(const long &m_lLargeur, const long &m_lHauteur)
{
	pimpl_->m_BitmapData.nlPosLargeur = m_lLargeur;
	pimpl_->m_BitmapData.nlPosHauteur = m_lHauteur;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

long CBitmap::GetPosLargeur()
{
	return pimpl_->m_BitmapData.nlPosLargeur;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

long CBitmap::GetPosHauteur()
{
	return pimpl_->m_BitmapData.nlPosHauteur;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void CBitmap::SetDeplacement(const long &m_lDeplacement, const bool &m_bType, const long &m_lTailleMax, const long &m_lTailleImage)
{
	if(m_bType)
	{
		pimpl_->m_BitmapData.nlPosHauteur+=m_lDeplacement;
		pimpl_->TestMaxY(m_lTailleImage, m_lTailleMax);
	}
	else
	{
		pimpl_->m_BitmapData.nlPosLargeur+=m_lDeplacement;
		pimpl_->TestMaxX(m_lTailleImage, m_lTailleMax);
	}
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void CBitmap::InitScrolling()
{
	pimpl_->m_BitmapData.m_iMouseScrollX = pimpl_->m_BitmapData.m_iMouseScrollY = pimpl_->m_BitmapData.nlPosHauteur = pimpl_->m_BitmapData.nlPosLargeur = 0;	
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

bool CBitmap::GetShrinkState()
{
	return pimpl_->m_BitmapData.m_bAutoShrink;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

bool CBitmap::GetFullscreenState()
{	
	return pimpl_->m_BitmapData.m_bFullScreen;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void CBitmap::SetfSize(const bool &m_blState)
{
	pimpl_->m_BitmapData.fSize = m_blState;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void CBitmap::SetSize(const int &m_xNewSize, const int &m_yNewSize)
{
	pimpl_->m_BitmapData.xNewSize = m_xNewSize;
	pimpl_->m_BitmapData.yNewSize = m_yNewSize;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void CBitmap::SetBitmapSize(const int &m_xNewSize, const int &m_yNewSize)
{
	pimpl_->m_BitmapData.nTailleAffichageWidth = m_xNewSize;
	pimpl_->m_BitmapData.nTailleAffichageHeight = m_yNewSize;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void CBitmap::SetfScroll(const bool &m_bState)
{
	pimpl_->m_BitmapData.fScroll = m_bState;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

int CBitmap::GetxNewSize()
{
	return pimpl_->m_BitmapData.xNewSize;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

int CBitmap::GetyNewSize()
{
	return pimpl_->m_BitmapData.yNewSize;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void CBitmap::SetMouseScroll(const int &m_iMouseX, const int &m_iMouseY)
{
	pimpl_->m_BitmapData.m_iMouseScrollX = m_iMouseX;
	pimpl_->m_BitmapData.m_iMouseScrollY = m_iMouseY;
}

void CBitmap::PositionnementImage(DWORD lParam, const long &m_xMax, const long &m_yMax, CIBitmap &m_ciBitmap)
{
	if(pimpl_->m_BitmapData.m_iMouseScrollX < LOWORD(lParam))
	{
		if(pimpl_->m_BitmapData.m_bInvert)
			pimpl_->m_BitmapData.nlPosLargeur += (LOWORD(lParam) - pimpl_->m_BitmapData.m_iMouseScrollX);
		else
			pimpl_->m_BitmapData.nlPosLargeur += -(LOWORD(lParam) - pimpl_->m_BitmapData.m_iMouseScrollX);
	}
	else
	{
		if(pimpl_->m_BitmapData.m_bInvert)
			pimpl_->m_BitmapData.nlPosLargeur += -(pimpl_->m_BitmapData.m_iMouseScrollX - LOWORD(lParam));
		else
			pimpl_->m_BitmapData.nlPosLargeur += (pimpl_->m_BitmapData.m_iMouseScrollX - LOWORD(lParam));

	}

	pimpl_->m_BitmapData.m_iMouseScrollX = LOWORD(lParam);

	if(pimpl_->m_BitmapData.m_iMouseScrollY < HIWORD(lParam))
	{
		if(pimpl_->m_BitmapData.m_bInvert)
			pimpl_->m_BitmapData.nlPosHauteur += (HIWORD(lParam) - pimpl_->m_BitmapData.m_iMouseScrollY);
		else
			pimpl_->m_BitmapData.nlPosHauteur += -(HIWORD(lParam) - pimpl_->m_BitmapData.m_iMouseScrollY);
		
	}
	else
	{
		if(pimpl_->m_BitmapData.m_bInvert)
		{
			pimpl_->m_BitmapData.nlPosHauteur += -(pimpl_->m_BitmapData.m_iMouseScrollY - HIWORD(lParam));
		}
		else
		{
			pimpl_->m_BitmapData.nlPosHauteur += (pimpl_->m_BitmapData.m_iMouseScrollY - HIWORD(lParam));
		}
		
	}
	pimpl_->m_BitmapData.m_iMouseScrollY = HIWORD(lParam);
	

	pimpl_->TestMaxX(m_ciBitmap.GetBitmapWidth() * pimpl_->m_BitmapData.fratio, m_xMax);
	
	pimpl_->TestMaxY(m_ciBitmap.GetBitmapHeigth() * pimpl_->m_BitmapData.fratio, m_yMax);


}

int &CBitmap::GetfScroll()
{
	return pimpl_->m_BitmapData.fScroll;
}

void CBitmap::SetMouseBlock(const bool &m_bBlock)
{
	pimpl_->m_BitmapData.m_bBlocking = m_bBlock;
}

bool CBitmap::GetMouseBlock()
{
	return pimpl_->m_BitmapData.m_bBlocking;
}

void CBitmap::InitRatio()
{
	pimpl_->m_BitmapData.fratio = 1.0;
	pimpl_->m_BitmapData.lPtTabRatio = 21;
}

void CBitmap::SetHauteQualite(const int &m_bHauteQualite)
{
	pimpl_->m_BitmapData.m_bHauteQualite = m_bHauteQualite;
}

void CBitmap::SetPrinter(const bool &m_bValue)
{
	pimpl_->m_BitmapData.m_bPrinter = m_bValue;
}

void CBitmap::SetInvertSensPhoto(const bool &m_bValue)
{
	pimpl_->m_BitmapData.m_bInvert = m_bValue;
}

int CBitmap::GetTailleAffichageWidth()
{
	int nTailleAffichageWidth;

	//pimpl_->m_BitmapData.fratio = fTabRatio[pimpl_->m_BitmapData.lPtTabRatio];

	nTailleAffichageWidth = (int)((double)pimpl_->m_BitmapData.nTailleAffichageWidth * (double)pimpl_->m_BitmapData.fratio);

	return nTailleAffichageWidth;
}

int CBitmap::GetTailleAffichageHeigth()
{

	int nTailleAffichageHeight;

	//pimpl_->m_BitmapData.fratio = fTabRatio[pimpl_->m_BitmapData.lPtTabRatio];

	nTailleAffichageHeight =(int)((double)pimpl_->m_BitmapData.nTailleAffichageHeight * (double)pimpl_->m_BitmapData.fratio);

	return nTailleAffichageHeight;
}

float CBitmap::GetRatioScreen(CIBitmap &m_ciBitmap,const RECT &rc)
{
	float fLocalratio;

	if(m_ciBitmap.GetBitmapWidth() < rc.right && m_ciBitmap.GetBitmapHeigth() < rc.bottom)
		return 1.0;

	if(m_ciBitmap.GetBitmapWidth() > m_ciBitmap.GetBitmapHeigth())
		fLocalratio = (float)rc.right / (float)(m_ciBitmap.GetBitmapWidth());
	else
		fLocalratio = (float)rc.bottom / (float)(m_ciBitmap.GetBitmapHeigth());

	int nTailleAffichageWidth = m_ciBitmap.GetBitmapWidth() * fLocalratio;
	int nTailleAffichageHeight =m_ciBitmap.GetBitmapHeigth() * fLocalratio;

	if(nTailleAffichageHeight > rc.bottom)
	{
		fLocalratio = (float)rc.bottom / (float)(m_ciBitmap.GetBitmapHeigth());
	}
	else
	{
		if(nTailleAffichageWidth > rc.right)
		{
			fLocalratio = (float)rc.right / (float)(m_ciBitmap.GetBitmapWidth());
		}
	}

	return fLocalratio;
}

void CBitmap::SetPosHauteur(const int &iHauteur)
{
	pimpl_->m_BitmapData.nlPosHauteur = iHauteur;

}

void CBitmap::SetLargeurPos(const int &iLargeur)
{
	pimpl_->m_BitmapData.nlPosLargeur = iLargeur;

}

void CBitmap::GetRatioSup()
{
	pimpl_->m_BitmapData.lPtTabRatio++;

	if(pimpl_->m_BitmapData.lPtTabRatio > lMax)
		pimpl_->m_BitmapData.lPtTabRatio = lMax;

	pimpl_->m_BitmapData.fratio = fTabRatio[pimpl_->m_BitmapData.lPtTabRatio];


	if(pimpl_->m_BitmapData.fratio != 1.0)
		pimpl_->m_BitmapData.m_bAutoShrink = false;

}

void CBitmap::GetRatioInf()
{
	pimpl_->m_BitmapData.lPtTabRatio--;

	if(pimpl_->m_BitmapData.lPtTabRatio < 0)
		pimpl_->m_BitmapData.lPtTabRatio = 0;

	pimpl_->m_BitmapData.fratio = fTabRatio[pimpl_->m_BitmapData.lPtTabRatio];
	

	if(pimpl_->m_BitmapData.fratio != 1.0)
		pimpl_->m_BitmapData.m_bAutoShrink = false;

}
