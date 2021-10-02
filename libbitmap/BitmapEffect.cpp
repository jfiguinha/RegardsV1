#include "StdAfx.h"
#include "BitmapEffect.h"
#include "RegardsBitmap.h"
#include <effet.h>
#include <filtre.h>
#include <ibitmap.h>
#include <color.h>
#include "DialogFiltreEffet.h"
#include "objet.h"
#include "resource.h"
#include <CloudsBox.h>
#include <lensflare.h>
#include <RedimWorkSpace.h>
#include <MotionBlur.h>
#include <wienerfilter.h>
#include <freerotatepicture.h>
#include <EffetDialog.h>
#include <FiltreDialog.h>
#include <Histogramme.h>

using namespace Regards::libbitmap;

CBitmapEffect::CBitmapEffect(void)
{
}

CBitmapEffect::~CBitmapEffect(void)
{
}

/////////////////////////////////////////////////////////////////////////
//Application d'un filtre au bitmap ou d'un effet
/////////////////////////////////////////////////////////////////////////
int CBitmapEffect::EffetMotionBlur(HINSTANCE hInstance, HWND &hwndLocal, CIBitmap * m_ciBitmap,CFiltreEffet &m_filtre)
{
	int m_iRetourEffet = 0;
	CMotionBlur * m_cMotionBlur = new CMotionBlur();
	m_cMotionBlur->Init(m_ciBitmap);
	m_cMotionBlur->CreateDlg(hInstance,(LPCTSTR)IDD_DLGMOTIONBLUR,hwndLocal);
	m_iRetourEffet = m_cMotionBlur->GetReturnValue();
	if(m_iRetourEffet == 1)
	{
		HCURSOR hcurSave = SetCursor(LoadCursor(NULL,IDC_WAIT));
		CFiltre m_cFiltre;
		m_cFiltre.MotionBlur(NULL,m_ciBitmap,m_cMotionBlur->GetRadius(),m_cMotionBlur->GetSigma(),m_cMotionBlur->GetAngle());	
		SetCursor(hcurSave);
	}

	delete m_cMotionBlur;
	return m_iRetourEffet;
}

/////////////////////////////////////////////////////////////////////////
//Application d'un filtre au bitmap ou d'un effet
/////////////////////////////////////////////////////////////////////////
int CBitmapEffect::EffetRotation(HINSTANCE hInstance, HWND &hwndLocal, CIBitmap * m_ciBitmap,CFiltreEffet &m_filtre, float fratio)
{
	int m_iRetourEffet = 0;
	CEffetDialog * m_cEffetDialog = new CEffetDialog(&m_filtre);
	m_cEffetDialog->CreateDlg(hInstance,(LPCTSTR)IDD_APPLITRAITEMENT,hwndLocal);
	m_iRetourEffet = m_cEffetDialog->GetReturnValue();

	delete[] m_filtre.m_lParam;
	if(m_iRetourEffet)
 		SendMessage(hwndLocal,WM_ERASEBKGND,MAKEWPARAM(m_ciBitmap->GetBitmapWidth() * fratio, m_ciBitmap->GetBitmapHeigth() * fratio),MAKELPARAM(IDM_ZOOM_OK,0)); //pimpl_->m_BitmapData.fratio
	else
		SendMessage(hwndLocal,WM_COMMAND,MAKEWPARAM(EFFET_CANCEL,0),0);
	
	delete m_cEffetDialog;

	return m_iRetourEffet;
}

/////////////////////////////////////////////////////////////////////////
//Application d'un filtre au bitmap ou d'un effet
/////////////////////////////////////////////////////////////////////////
int CBitmapEffect::EffetCrop(HINSTANCE hInstance, HWND &hwndLocal, CIBitmap * m_ciBitmap,CFiltreEffet &m_filtre)
{
	int m_iRetourEffet = 0;
	CRedimWorkSpace * m_cRedimWorkSpace = new CRedimWorkSpace(m_ciBitmap->GetBitmapWidth(),m_ciBitmap->GetBitmapHeigth());
	m_cRedimWorkSpace->CreateDlg(hInstance,(LPCTSTR)IDD_DLGCANVAS,hwndLocal);
	
	m_iRetourEffet = m_cRedimWorkSpace->GetReturnValue();
	if(m_iRetourEffet)
	{
		CEffet m_cEffet;
		RECT m_rect = m_cRedimWorkSpace->GetRectangleValue();
		m_cEffet.CropBitmap(m_ciBitmap,m_rect);
		SendMessage(hwndLocal,WM_ERASEBKGND,MAKEWPARAM(m_ciBitmap->GetBitmapWidth(), m_ciBitmap->GetBitmapHeigth()),MAKELPARAM(IDM_ZOOM_OK,0));
	}

	delete[] m_filtre.m_lParam;
	delete m_cRedimWorkSpace;

	return m_iRetourEffet;
}

/////////////////////////////////////////////////////////////////////////
//Application d'un filtre au bitmap ou d'un effet
/////////////////////////////////////////////////////////////////////////
int CBitmapEffect::EffetScale(HINSTANCE hInstance, HWND &hwndLocal, CIBitmap * m_ciBitmap,CFiltreEffet &m_filtre)
{
	int m_iRetourEffet = 0;
	CEffetDialog * m_cEffetDialog = new CEffetDialog(&m_filtre);
	m_cEffetDialog->CreateDlg(hInstance,(LPCTSTR)IDD_APPLITRAITEMENT,hwndLocal);
	m_iRetourEffet = m_cEffetDialog->GetReturnValue();

	delete[] m_filtre.m_lParam;
	if(m_iRetourEffet)
  		SendMessage(hwndLocal,WM_ERASEBKGND,MAKEWPARAM(m_ciBitmap->GetBitmapWidth(), m_ciBitmap->GetBitmapHeigth()),MAKELPARAM(IDM_ZOOM_OK,0));
	else
		SendMessage(hwndLocal,WM_COMMAND,MAKEWPARAM(EFFET_CANCEL,0),0);

	delete m_cEffetDialog;
	return m_iRetourEffet;
}

/////////////////////////////////////////////////////////////////////////
//Application d'un filtre au bitmap ou d'un effet
/////////////////////////////////////////////////////////////////////////
int CBitmapEffect::EffetFlip(HINSTANCE hInstance, HWND &hwndLocal, CIBitmap * m_ciBitmap,CFiltreEffet &m_filtre)
{
	int m_iRetourEffet = 0;
	CEffetDialog * m_cEffetDialog = new CEffetDialog(&m_filtre);
	m_cEffetDialog->CreateDlg(hInstance,(LPCTSTR)IDD_APPLITRAITEMENT,hwndLocal);
	m_iRetourEffet = m_cEffetDialog->GetReturnValue();
	if(m_iRetourEffet)
		SendMessage(hwndLocal,WM_ERASEBKGND,MAKEWPARAM(m_ciBitmap->GetBitmapWidth(), m_ciBitmap->GetBitmapHeigth()),MAKELPARAM(IDM_FILTREIMAGE_OK,0));
	else
		SendMessage(hwndLocal,WM_COMMAND,MAKEWPARAM(EFFET_CANCEL,0),0);	

	delete[] m_filtre.m_lParam;
	delete m_cEffetDialog;
	return m_iRetourEffet;
}



/////////////////////////////////////////////////////////////////////////
//Application d'un filtre au bitmap ou d'un effet
/////////////////////////////////////////////////////////////////////////
int CBitmapEffect::EffetNuages(HINSTANCE hInstance, HWND &hwndLocal, CIBitmap * m_ciBitmap,CFiltreEffet &m_filtre)
{
	int m_iRetourEffet = 0;
	CCloudsBox * m_cCloudBox = new CCloudsBox(hInstance);
	m_cCloudBox->CreateDlg(hInstance,(LPCTSTR)IDD_DLGCLOUDS,hwndLocal);
	m_iRetourEffet = m_cCloudBox->GetReturnValue();
	if(m_iRetourEffet == 1)
	{
		//CFiltre m_cFiltre;
		//m_cFiltre.Clouds(NULL,m_ciBitmap,m_cCloudBox->GetRGBcolor1(),m_cCloudBox->GetRGBcolor2(),m_cCloudBox->GetFrequence(),m_cCloudBox->GetAmplitude(),m_cCloudBox->GetOctave());			

		CIBitmap * m_ciClouds = m_cCloudBox->GetBitmap();
		CEffet m_cEffet;
		m_cEffet.ScaleBitmap(m_ciClouds,m_ciBitmap->GetBitmapWidth(),m_ciBitmap->GetBitmapHeigth(),1);
		m_ciBitmap->SetBitmap(m_ciClouds->GetPtBitmap(),m_ciBitmap->GetBitmapWidth(),m_ciBitmap->GetBitmapHeigth());

	}

	delete m_cCloudBox;
	return m_iRetourEffet;
}

/////////////////////////////////////////////////////////////////////////
//Application d'un filtre au bitmap ou d'un effet
/////////////////////////////////////////////////////////////////////////
int CBitmapEffect::EffetDefault(HINSTANCE hInstance, HWND &hwndLocal, CIBitmap * m_ciBitmap,CFiltreEffet &m_filtre)
{
	int m_iRetourEffet = 0;
	if(m_filtre.iAffichage == 1)
	{
		//DialogBoxParam(m_hInstance,(LPCTSTR)IDD_APPLITRAITEMENT,hwndLocal,(DLGPROC)CDialogFiltreEffet::FiltreDialog,(LPARAM)&m_filtre); 
		CFiltreDialog * m_cFiltreDialog = new CFiltreDialog(&m_filtre);
		m_cFiltreDialog->CreateDlg(hInstance,(LPCTSTR)IDD_APPLITRAITEMENT,hwndLocal);
		m_iRetourEffet = m_cFiltreDialog->GetReturnValue();
		delete m_cFiltreDialog;
	}
	else
	{
		CFiltre m_cFiltre;
		m_cFiltre.AppliquerFiltre(&m_filtre);
	}
	return m_iRetourEffet;
}



/////////////////////////////////////////////////////////////////////////
//Application d'un filtre au bitmap ou d'un effet
/////////////////////////////////////////////////////////////////////////
int CBitmapEffect::EffetWiener(HINSTANCE hInstance, HWND &hwndLocal, CIBitmap * m_ciBitmap,CFiltreEffet &m_filtre)
{
	//Test des paramètres de l'image
	int m_iRetourEffet = 0;
	double width = m_ciBitmap->GetBitmapWidth();
	int m_dValue = log2(width);

	if(m_ciBitmap->GetBitmapWidth() != m_ciBitmap->GetBitmapHeigth() || pow((double)2,(double)m_dValue) != width)
	{
		CObjet::AffichageMessage(IDS_MSGERRORWIENER,IDS_ERREUR,MB_ICONERROR);
	}
	else
	{
		//DialogBoxParam(m_hInstance,(LPCTSTR)IDD_DLGWIENER,hwndLocal,(DLGPROC)CDialog::WienerFilter,(LPARAM)m_ciBitmap);
		CWienerFilter * m_cWienerFilter = new CWienerFilter();
		m_cWienerFilter->Init(m_ciBitmap);
		m_cWienerFilter->CreateDlg(hInstance,(LPCTSTR)IDD_DLGWIENER,hwndLocal);
		m_iRetourEffet = m_cWienerFilter->GetReturnValue();
		if(m_iRetourEffet == 1)
		{
			HCURSOR hcurSave = SetCursor(LoadCursor(NULL,IDC_WAIT));
			CFiltre m_cFiltre;
			m_cFiltre.FiltreWiener(NULL,m_ciBitmap,*m_cWienerFilter->GetMatrix(),m_cWienerFilter->GetRho(),m_cWienerFilter->GetMethode());	
			SetCursor(hcurSave);
		}

		delete m_cWienerFilter;

	}
	return m_iRetourEffet;
}


/////////////////////////////////////////////////////////////////////////
//Application d'un filtre au bitmap ou d'un effet
/////////////////////////////////////////////////////////////////////////
int CBitmapEffect::EffetRotationLibre(HINSTANCE hInstance, HWND &hwndLocal, CIBitmap * m_ciBitmap,CFiltreEffet &m_filtre, float fRatio)
{
	int m_iRetourEffet = 0;
	CFreeRotatePicture * m_cFreeRotatePicture = new CFreeRotatePicture();
	m_cFreeRotatePicture->Init(m_ciBitmap);
	m_cFreeRotatePicture->CreateDlg(hInstance,(LPCTSTR)IDD_DLGROTATE,hwndLocal);
	m_iRetourEffet = m_cFreeRotatePicture->GetReturnValue();
	if(m_iRetourEffet == 1)
	{
		CEffet m_cEffet;
		m_cEffet.GetFreeRotatedBitmap(m_ciBitmap,m_cFreeRotatePicture->GetAngle(), RGB(0,0,0));
		SendMessage(hwndLocal,WM_ERASEBKGND,MAKEWPARAM(m_ciBitmap->GetBitmapWidth() * fRatio, m_ciBitmap->GetBitmapHeigth() * fRatio),MAKELPARAM(IDM_ZOOM_OK,0));
	}

	delete m_cFreeRotatePicture;
	return m_iRetourEffet;

}

/////////////////////////////////////////////////////////////////////////
//Application d'un filtre au bitmap ou d'un effet
/////////////////////////////////////////////////////////////////////////
int CBitmapEffect::EffetLensFlare(HINSTANCE hInstance, HWND &hwndLocal, CIBitmap * m_ciBitmap,CFiltreEffet &m_filtre)
{
	int m_iRetourEffet = 0;
	CLensFlare * m_cLensFlare= new CLensFlare(hInstance);
	m_cLensFlare->SetBitmapSource(m_ciBitmap);
	m_cLensFlare->CreateDlg(hInstance,(LPCTSTR)IDD_DLGLENSFLARE,hwndLocal);
	m_iRetourEffet = m_cLensFlare->GetReturnValue();
	if(m_iRetourEffet == 0)
		SendMessage(hwndLocal,WM_COMMAND,MAKEWPARAM(EFFET_CANCEL,0),0);

	delete m_cLensFlare;
	return m_iRetourEffet;
}

/////////////////////////////////////////////////////////////////////////
//Affichage de l'histogramme
/////////////////////////////////////////////////////////////////////////
LRESULT CBitmapEffect::ShowHistogramme(HWND hWnd, HDC hDC, CIBitmap * ciBitmap, RECT rc)
{
#ifdef _DEBUG
	OutputDebugString("ShowHistogramme \n");
#endif
	HDC hdcSrc = ::CreateCompatibleDC(NULL);
	if(ciBitmap->hBitmapHisto == NULL)
	{
		CHistogramme * m_cHistogramme = new CHistogramme();
		m_cHistogramme->SetBitmap(ciBitmap);
		m_cHistogramme->ParcoursBitmap();
		ciBitmap->hBitmapHisto = m_cHistogramme->GenerateHistogrammeBitmap(266,200,0);
		delete m_cHistogramme;
	}
	::SelectObject(hdcSrc, ciBitmap->hBitmapHisto);
	::BitBlt(hDC, 0, rc.bottom - 200, 266, 200, hdcSrc, 0, 0, SRCCOPY);

	DeleteDC(hdcSrc);

	return 0;
}

/////////////////////////////////////////////////////////////////////////
//Affichage des informations
/////////////////////////////////////////////////////////////////////////
LRESULT CBitmapEffect::ShowInfos(HWND hWnd, HDC hDC, CIBitmap * ciBitmap)
{
	PAINTSTRUCT ps;
	RECT rc;
	HFONT font;
	GetClientRect(hWnd,&rc);
	CObjet m_cObjet;
	LOGFONT LogFont;
	char m_szFilename[MAX_PATH];
	ciBitmap->GetFilename(m_szFilename);

    LogFont.lfStrikeOut = 0;
    LogFont.lfUnderline = 0;
    LogFont.lfHeight = 14;
    LogFont.lfEscapement = 0;
    LogFont.lfItalic = FALSE;
	StringCchCopy(LogFont.lfFaceName,32,TEXT("Verdana"));

    font = CreateFontIndirect(&LogFont);
    SelectObject(hDC, font);
	SetTextColor(hDC, RGB(0,128,0));
	SetBkMode(hDC, TRANSPARENT);

	int m_iHeightPos = 0;
	char cTemp[MAX_PATH];
	m_cObjet.GetFileName(m_szFilename,cTemp,MAX_PATH);
	TextOut(hDC, 20, m_iHeightPos, cTemp, strlen(cTemp));m_iHeightPos+=18;
	m_cObjet.GetInfosDateFile(m_szFilename,cTemp,MAX_PATH);
	TextOut(hDC, 20, m_iHeightPos, cTemp, strlen(cTemp));m_iHeightPos+=18;
	int iFormat = m_cObjet.TestImageFormat(m_szFilename);
	m_cObjet.GetFormatName(iFormat,cTemp,MAX_PATH);
	TextOut(hDC, 20, m_iHeightPos, cTemp, strlen(cTemp));m_iHeightPos+=18;
	sprintf_s(cTemp,MAX_PATH,"%dx%d",ciBitmap->GetBitmapWidth(),ciBitmap->GetBitmapHeigth());
	TextOut(hDC, 20, m_iHeightPos, cTemp, strlen(cTemp));m_iHeightPos+=18;
	m_cObjet.GetFileSize(m_szFilename,cTemp,MAX_PATH);
	TextOut(hDC, 20, m_iHeightPos, cTemp, strlen(cTemp));
    DeleteObject(font);		

	return 0;
}