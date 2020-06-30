// PhotoDigital.cpp: implementation of the CPhotoDigital class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PhotoDigital.h"
#include "LoadImage.h"
#include "objet.h"
#include <ibitmap.h>
#include <LoadPicture.h>
//int CPhotoDigital::iNumImage;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPhotoDigital::CPhotoDigital()
{
	iNumImage = 0;	
}

CPhotoDigital::~CPhotoDigital()
{
}

LRESULT CPhotoDigital::OnCreate(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}



LRESULT CPhotoDigital::OnCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
		case TWCPP_ENDTRANSFER:
			PostQuitMessage(0);
	}
	return 0;
}

int CPhotoDigital::Run()
{
	MSG	msg;

	InitTwain(m_hWnd);

	if(SelectSource())
		Acquire();

	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		ProcessMessage(msg);
		DispatchMessage(&msg);
	}
	
	ReleaseTwain();

	PostQuitMessage(0);

	SetWindowLong (m_hWnd, GWL_USERDATA, (long)NULL);


	return 0;
}

void CPhotoDigital::CopyImage(HWND hWnd, HANDLE hBitmap,TW_IMAGEINFO& info)
{
	//Traitement des images suivant les paramètres
	CLoadImage m_cChargementImage;
	
	CIBitmap * m_ciBitmap = new CIBitmap();
	
	m_ciBitmap->HandleToBitmap(hBitmap,info.BitsPerPixel,NULL);

	//Génération du nom de l'image;

	iNumImage++;

	char m_szTemp[10];
	char m_szPath[1024];
	long m_lNumImage = iNumImage * m_PHOTODIGITALDATA.iTypeIncrementation;
	StringCchPrintf(m_szTemp,10,"%d",m_lNumImage);

	switch(strlen(m_szTemp))
	{
		case 1:
			StringCchPrintf(m_szPath,1024,"%s\\%s00000%d",m_PHOTODIGITALDATA.m_szDirectory,m_PHOTODIGITALDATA.m_szFilename,m_lNumImage); 
			break;
		case 2:
			StringCchPrintf(m_szPath,1024,"%s\\%s0000%d",m_PHOTODIGITALDATA.m_szDirectory,m_PHOTODIGITALDATA.m_szFilename,m_lNumImage); 
			break;
		case 3:
			StringCchPrintf(m_szPath,1024,"%s\\%s000%d",m_PHOTODIGITALDATA.m_szDirectory,m_PHOTODIGITALDATA.m_szFilename,m_lNumImage); 
			break;
		case 4:
			StringCchPrintf(m_szPath,1024,"%s\\%s00%d",m_PHOTODIGITALDATA.m_szDirectory,m_PHOTODIGITALDATA.m_szFilename,m_lNumImage); 
			break;
		case 5:
			StringCchPrintf(m_szPath,1024,"%s\\%s0%d",m_PHOTODIGITALDATA.m_szDirectory,m_PHOTODIGITALDATA.m_szFilename,m_lNumImage); 
			break;
		default:
			StringCchPrintf(m_szPath,1024,"%s\\%s%d",m_PHOTODIGITALDATA.m_szDirectory,m_PHOTODIGITALDATA.m_szFilename,m_lNumImage); 
	}

	switch(m_PHOTODIGITALDATA.iFormatImage)
	{
		case 1:
			StringCbCat(m_szPath,1024,".jpg");
			CLoadPicture::SaveImage(CLoadPicture::JPEG,*m_ciBitmap,m_szPath,0,90);
			break;
		case 2:
			StringCbCat(m_szPath,1024,".bmp");
			CLoadPicture::SaveImage(CLoadPicture::BMP,*m_ciBitmap,m_szPath);
			break;
		case 3:
			StringCbCat(m_szPath,1024,".tga");
			CLoadPicture::SaveImage(CLoadPicture::TGA,*m_ciBitmap,m_szPath);
			break;
		case 4:
			StringCbCat(m_szPath,1024,".tif");
			CLoadPicture::SaveImage(CLoadPicture::TIFF,*m_ciBitmap,m_szPath,0,90);
			break;

		case 5:
			StringCbCat(m_szPath,1024,".pdf");
			CLoadPicture::SaveImage(CLoadPicture::PDF,*m_ciBitmap,m_szPath,2,0);
			break;
	}
	
	delete m_ciBitmap;

}

LRESULT CPhotoDigital::OnDestroy(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

void CPhotoDigital::SetParametre(PHOTODIGITALDATA &m_PHOTODIGITALDATA)
{
	iNumImage = 0;
	this->m_PHOTODIGITALDATA = m_PHOTODIGITALDATA;
}
