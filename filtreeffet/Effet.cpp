// Effet.cpp: implementation of the CEffet class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Effet.h"
#include "ibitmap.h"
#include "dessin.h"
#include "ImageScale.h"
#include "../fichier.h"
#include "../objet.h"
#include <loadpicture.h>

#define F_DELTA	0.0001f
//static double pi = 3.1415926535;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffet::CEffet()
{
	m_lpFiltreEffet = NULL;
}

CEffet::~CEffet()
{

}

void CEffet::CropBitmap(CIBitmap * m_ciBitmap, const RECT &rc)
{
	if (m_ciBitmap->hBitmap)
	{
	
		RECT m_rcLocal = rc;

		if(rc.bottom < rc.top)
		{
			long m_lValue = rc.bottom;
			m_rcLocal.bottom = rc.top;
			m_rcLocal.top = m_lValue;
		}

		if(m_rcLocal.right < m_rcLocal.left)
		{
			long m_lValue = m_rcLocal.right;
			m_rcLocal.right = rc.left;
			m_rcLocal.left = m_lValue;
		}

		long height = m_rcLocal.bottom - m_rcLocal.top;
		long width = m_rcLocal.right - m_rcLocal.left;
		long pos, pos2;
		int iTaille = 4;
		long lWidthSize2 = ((((32 * width) + 31) / 32) * 4);
		long lWidthSize = m_ciBitmap->GetWidthSize();

		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize;

		m_ciBitmap->GetBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize);

		BYTE * pData= new BYTE[((iTaille * width + iTaille) & ~iTaille) * height];

		
		for(long y = m_rcLocal.top; y < m_rcLocal.bottom; y++)
		{
			for(long x = m_rcLocal.left; x < m_rcLocal.right; x++)
			{
				pos = ((bmHeight - y) * lWidthSize) + (x * 4);
				if(pos > m_lSize)
				{
					continue;
				}
				pos2 = (((height - 1) - (y - m_rcLocal.top)) * lWidthSize2) + ((x - m_rcLocal.left) * 4);
				*(pData + pos2) = *(pBitsSrc + pos);
				*(pData + pos2 + 1) = *(pBitsSrc + pos + 1);
				*(pData + pos2 + 2) = *(pBitsSrc + pos + 2);
			}
		}

		m_ciBitmap->SetBitmap(pData, width, height);

		delete[] pBitsSrc;
		delete[] pData;
		
	}

}

HBITMAP CEffet::ScaleBitmap(HBITMAP hBitmap, const WORD & width, const WORD & height)
{
	if(hBitmap == NULL)
		return NULL;
	

    HDC hScrDC, hMemDC;         // screen DC and memory DC 
	HBITMAP hOldBitmap;
	bool bSuccess;
	BITMAP bmp;
	::GetObject(hBitmap, sizeof(BITMAP), &bmp);
	
 
    // create a DC for the screen and create 
    // a memory DC compatible to screen DC 

	hOldBitmap = ::CreateCompatibleBitmap(GetDC(NULL),width, height);
     
    hScrDC = CreateCompatibleDC (GetDC(NULL));
    hMemDC = CreateCompatibleDC (GetDC(NULL));
 
    // select new bitmap into memory DC 
    SelectObject(hScrDC, hBitmap); 
	SelectObject(hMemDC, hOldBitmap); 
 
	SetStretchBltMode (hMemDC, COLORONCOLOR); 

    // bitblt screen DC to memory DC 
    //BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY); 
    bSuccess = StretchBlt(hMemDC, 0,  0,  
            width, 
            height, hScrDC, 0,  
            0,  bmp.bmWidth, 
			bmp.bmHeight, SRCCOPY); 

 
    // select old bitmap back into memory DC and get handle to 
    // bitmap of the screen 
     
    //hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap); 
	//(HBITMAP)SelectObject(hMemDC, hBitmap); 
 
    // clean up 
 
    DeleteDC(hScrDC); 
    DeleteDC(hMemDC); 
 
    // return handle to the bitmap 

	::GetObject(hOldBitmap, sizeof(BITMAP), &bmp);
 
    return hOldBitmap;

}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

HBITMAP CEffet::GetFlipBitmap(CIBitmap * m_ciBitmap, const BOOL &bLateral)
{


	// Create a memory DC compatible with the display
	HDC sourceDC, destDC;
	sourceDC = CreateCompatibleDC( NULL );
	destDC = CreateCompatibleDC( NULL );

	// Get logical coordinates
	BITMAP bm ;
	GetObject(m_ciBitmap->hBitmap, sizeof( bm ), &bm );

	// Create a bitmap to hold the result
	HBITMAP hbmResult = CreateCompatibleBitmap(GetDC(NULL), 
						bm.bmWidth, bm.bmHeight);

	// Select bitmaps into the DCs
	SelectObject( sourceDC, m_ciBitmap->hBitmap );
	SelectObject( destDC, hbmResult );
	
	if( bLateral )
		StretchBlt(destDC, 0, 0, bm.bmWidth, bm.bmHeight, sourceDC, 
				bm.bmWidth-1, 0, -bm.bmWidth, bm.bmHeight, SRCCOPY );
	else
		StretchBlt(destDC, 0, 0, bm.bmWidth, bm.bmHeight, sourceDC, 
				0, bm.bmHeight-1, bm.bmWidth, -bm.bmHeight, SRCCOPY );

	DeleteDC(sourceDC);
	DeleteDC(destDC);

	return hbmResult;
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

HBITMAP CEffet::GetRotatedBitmap(CIBitmap * m_ciBitmap, const float &angle, const COLORREF &clrBack )
{
	float radians;
	if(m_ciBitmap->hBitmap == NULL)
	{
		return NULL;
	}
	else
	{
		radians = (2*pi* angle)/360;
	}

	// Create a memory DC compatible with the display
	HDC sourceDC, destDC;
	sourceDC = CreateCompatibleDC( GetDC(NULL) );
	destDC = CreateCompatibleDC(  GetDC(NULL)  );

	// Get logical coordinates
	BITMAP bm;
	::GetObject( m_ciBitmap->hBitmap, sizeof( bm ), &bm );

	float cosine = (float)cos(radians);
	float sine = (float)sin(radians);

	// Compute dimensions of the resulting bitmap
	// First get the coordinates of the 3 corners other than origin
	int x1 = (int)(bm.bmHeight * sine);
	int y1 = (int)(bm.bmHeight * cosine);
	int x2 = (int)(bm.bmWidth * cosine + bm.bmHeight * sine);
	int y2 = (int)(bm.bmHeight * cosine - bm.bmWidth * sine);
	int x3 = (int)(bm.bmWidth * cosine);
	int y3 = (int)(-bm.bmWidth * sine);

	int minx = min(0,min(x1, min(x2,x3)));
	int miny = min(0,min(y1, min(y2,y3)));
	int maxx = max(0,max(x1, max(x2,x3)));
	int maxy = max(0,max(y1, max(y2,y3)));

	int w = maxx - minx;
	int h = maxy - miny;

	// Create a bitmap to hold the result
	HBITMAP hbmResult = ::CreateCompatibleBitmap(GetDC(NULL), w, h);

	SelectObject( sourceDC, m_ciBitmap->hBitmap );
	SelectObject( destDC, hbmResult );

	// Draw the background color before we change mapping mode
	HBRUSH hbrBack = CreateSolidBrush( clrBack );
	HBRUSH hbrOld = (HBRUSH)::SelectObject( destDC, hbrBack );

	PatBlt(destDC, 0, 0, w, h, PATCOPY );
	::DeleteObject( ::SelectObject( destDC, hbrOld ) );

	// We will use world transform to rotate the bitmap
	SetGraphicsMode(destDC, GM_ADVANCED);
	XFORM xform;
	xform.eM11 = cosine;
	xform.eM12 = -sine;
	xform.eM21 = sine;
	xform.eM22 = cosine;
	xform.eDx = (float)-minx;
	xform.eDy = (float)-miny;

	SetWorldTransform( destDC, &xform );

	// Now do the actual rotating - a pixel at a time
	BitBlt(destDC, 0,0,bm.bmWidth, bm.bmHeight, sourceDC, 0, 0, SRCCOPY );

	DeleteObject(hbrBack);
	DeleteDC(sourceDC);
	DeleteDC(destDC);

	return hbmResult;
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

void CEffet::GetFreeRotatedBitmap(CIBitmap * m_ciBitmap, const float &angle, const COLORREF &clrBack, const int &type)
{
	CImageScale m_LocalScale;
	WORD width = 1, height = 1;
	m_LocalScale.ApplyRotation(m_ciBitmap,width,height,angle,clrBack);
}

// GetRotatedBitmap	- Create a new bitmap with rotated image
// Returns		- Returns new bitmap with rotated image
// hBitmap		- Bitmap to rotate
// radians		- Angle of rotation in radians
// clrBack		- Color of pixels in the resulting bitmap that do
//			  not get covered by source pixels
// Note			- If the bitmap uses colors not in the system palette 
//			  then the result is unexpected. You can fix this by
//			  adding an argument for the logical palette.
HBITMAP CEffet::GetRotatedBitmap98(CIBitmap * m_ciBitmap, const float &angle, const COLORREF &clrBack)
{

	float radians;

	if(m_ciBitmap->hBitmap == NULL)
	{
		return NULL;
	}
	else
	{
		radians = (2*pi* angle)/360;
	}

	// Create a memory DC compatible with the display
	HDC sourceDC, destDC;
	sourceDC = CreateCompatibleDC(GetDC(NULL));
	destDC = CreateCompatibleDC(sourceDC);

	// Get logical coordinates
	BITMAP bm;
	::GetObject(m_ciBitmap->hBitmap, sizeof( bm ), &bm );

	float cosine = (float)cos(radians);
	float sine = (float)sin(radians);

	// Compute dimensions of the resulting bitmap
	// First get the coordinates of the 3 corners other than origin
	int x1 = (int)(-bm.bmHeight * sine);
	int y1 = (int)(bm.bmHeight * cosine);
	int x2 = (int)(bm.bmWidth * cosine - bm.bmHeight * sine);
	int y2 = (int)(bm.bmHeight * cosine + bm.bmWidth * sine);
	int x3 = (int)(bm.bmWidth * cosine);
	int y3 = (int)(bm.bmWidth * sine);

	int minx = min(0,min(x1, min(x2,x3)));
	int miny = min(0,min(y1, min(y2,y3)));
	int maxx = max(x1, max(x2,x3));
	int maxy = max(y1, max(y2,y3));

	int w = maxx - minx;
	int h = maxy - miny;


	// Create a bitmap to hold the result
	HBITMAP hbmResult = CreateCompatibleBitmap(GetDC(NULL), w, h);

	HBITMAP hbmOldSource = (HBITMAP)::SelectObject( sourceDC, m_ciBitmap->hBitmap );
	HBITMAP hbmOldDest = (HBITMAP)::SelectObject( destDC, hbmResult );

	// Draw the background color before we change mapping mode
	HBRUSH hbrBack = CreateSolidBrush( clrBack );
	HBRUSH hbrOld = (HBRUSH)::SelectObject( destDC, hbrBack );
	PatBlt(destDC, 0, 0, w, h, PATCOPY );
	DeleteObject(SelectObject( destDC, hbrOld ));

	// Set mapping mode so that +ve y axis is upwords
	SetMapMode(sourceDC,MM_ISOTROPIC);
	SetWindowExtEx(sourceDC,1,1,NULL);
	SetViewportExtEx(sourceDC,1,-1,NULL);
	SetViewportOrgEx(sourceDC,0, bm.bmHeight-1,NULL);

	SetMapMode(destDC,MM_ISOTROPIC);
	SetWindowExtEx(destDC,1,1,NULL);
	SetViewportExtEx(destDC,1,-1,NULL);
	SetWindowOrgEx(destDC,minx, maxy,NULL);

	// Now do the actual rotating - a pixel at a time
	// Computing the destination point for each source point
	// will leave a few pixels that do not get covered
	// So we use a reverse transform - e.i. compute the source point
	// for each destination point

	for( int y = miny; y < maxy; y++ )
	{


		for( int x = minx; x < maxx; x++ )
		{
			int sourcex = (int)(x*cosine + y*sine);
			int sourcey = (int)(y*cosine - x*sine);
			if( sourcex >= 0 && sourcex < bm.bmWidth && sourcey >= 0 
					&& sourcey < bm.bmHeight )
				SetPixel(destDC,x,y,GetPixel(sourceDC,sourcex,sourcey));
		}
	}

	// Restore DCs
	SelectObject( sourceDC, hbmOldSource );
	SelectObject( destDC, hbmOldDest );

	DeleteObject(hbmOldSource);
	DeleteObject(hbmOldDest);
	DeleteObject(m_ciBitmap->hBitmap);
	DeleteObject(hbrBack);
	DeleteObject(hbrOld);

	DeleteDC(sourceDC);
	DeleteDC(destDC);



	return hbmResult;
}

///////////////////////////////////////////////////////////
// ScaleBitmap
///////////////////////////////////////////////////////////

void CEffet::ScaleBitmap(CIBitmap * m_ciBitmap, const WORD & width, const WORD & height,const int &smooth)
{
	HBITMAP hBmpLocal;

	if(m_ciBitmap->GetPtBitmap() == NULL)
	{
 		return;
	}

	if(smooth > 0)
	{
		CImageScale * m_LocalScale = new CImageScale();

		try
		{
			m_LocalScale->ApplyImageScale(m_ciBitmap,width,height,smooth);
		}
		catch(...)
		{
		}

		try
		{
			delete m_LocalScale;
		}
		catch(...)
		{
		}
	}
	else
	{
		hBmpLocal = ScaleBitmap(m_ciBitmap->hBitmap, width, height);
		m_ciBitmap->SetBitmap(hBmpLocal,false);
		DeleteObject(hBmpLocal);
	}
}




///////////////////////////////////////////////////////////
//Distort Bitmap
///////////////////////////////////////////////////////////

HBITMAP CEffet::GenerateIcone(CIBitmap * m_ciBitmap, const int &iIconeWidth, const int &iIconeHeight, unsigned int &Width, unsigned int &Height, const int &iMarge, const COLORREF &m_color, const COLORREF &rgb)
{
	CDessin m_CDessin;
	float fratio;
	int iMargeLocal = 10;
	int iIconeWidthLocal = iIconeWidth - iMargeLocal,  iIconeHeightLocal = iIconeHeight - iMargeLocal;
	int iTailleIconeWidth = iIconeWidthLocal, iTailleIconeHeight = iIconeHeightLocal;
	HBITMAP hBitmap;

	if(iMarge == 0)
	{
		iIconeWidthLocal = iIconeWidth;
		iIconeHeightLocal = iIconeHeight;
		iTailleIconeWidth = iIconeWidthLocal;
		iTailleIconeHeight = iIconeHeightLocal;
		iMargeLocal = 0;

	}


	int iPosX = 0, iPosY = 0;
	RECT rc;


	rc.top = 0;
	rc.bottom = iIconeHeight;
	rc.left = 0;
	rc.right = iIconeWidth;
	
	// Create a memory DC compatible with the display
	//HDC sourceDC;
	HDC destDC;
	destDC = CreateCompatibleDC( GetDC(NULL) );

	// Get logical coordinates

	int iBitmapWidth = m_ciBitmap->GetBitmapWidth();
	int iBitmapHeight = m_ciBitmap->GetBitmapHeigth();

	if(iBitmapWidth == iBitmapHeight)
	{
		//Création d'une image correct
		if(iIconeWidth > iIconeHeight)
		{
			fratio = (float)iIconeHeightLocal / (float)iBitmapHeight;
			iTailleIconeWidth = fratio * iBitmapWidth;
			iPosX = (iIconeWidthLocal - iTailleIconeWidth) >> 1;
		}
		else
		{
			fratio = (float)iIconeWidthLocal / (float)iBitmapWidth;
			iTailleIconeHeight = fratio * iBitmapHeight;
			iPosY = (iIconeHeightLocal - iTailleIconeHeight) >> 1;

		}

	}
	else
	{
		//Création d'une image correct
		if(iBitmapWidth < iBitmapHeight)
		{
			fratio = (float)iIconeHeightLocal / (float)iBitmapHeight;
			iTailleIconeWidth = fratio * iBitmapWidth;
			iPosX = (iIconeWidthLocal - iTailleIconeWidth) >> 1;
		}
		else
		{
			fratio = (float)iIconeWidthLocal / (float)iBitmapWidth;
			iTailleIconeHeight = fratio * iBitmapHeight;
			iPosY = (iIconeHeightLocal - iTailleIconeHeight) >> 1;

		}
	}

	ScaleBitmap(m_ciBitmap,iTailleIconeWidth,iTailleIconeHeight,TRIANGLEFILTER);

	BITMAP bm ;

	//Conversion de l'image
	GetObject(m_ciBitmap->hBitmap, sizeof( bm ), &bm );

	hBitmap = CreateCompatibleBitmap(GetDC(NULL), iIconeWidth, iIconeHeight);

	// Select bitmaps into the DCs
	SelectObject( destDC, hBitmap );

	SetStretchBltMode(destDC,COLORONCOLOR);

	FillRect(destDC, &rc, CreateSolidBrush(m_color)); 
	
	if(iMargeLocal > 0)
	{
		StretchDIBits(destDC, iPosX + (iMargeLocal >> 1), iPosY + (iMargeLocal >> 1), iTailleIconeWidth, iTailleIconeHeight, 
				0, 0, bm.bmWidth, bm.bmHeight, m_ciBitmap->GetPtBitmap(), &m_ciBitmap->bmiInfos, DIB_RGB_COLORS, SRCCOPY);
	}
	else
	{
		StretchDIBits(destDC,  iPosX, iPosY, iTailleIconeWidth, iTailleIconeHeight, 
			0, 0, bm.bmWidth, bm.bmHeight, m_ciBitmap->GetPtBitmap(), &m_ciBitmap->bmiInfos, DIB_RGB_COLORS, SRCCOPY);
	}

	if(iMarge > 0)
	{
		m_CDessin.DessinerCarre(destDC, iIconeWidth, iIconeHeight, iMarge,0,0, rgb);
	}

	DeleteDC(destDC);


	Width = iTailleIconeWidth + iPosX;
	Height = iTailleIconeHeight + iPosY;

	return (HBITMAP)CopyImage(hBitmap, IMAGE_BITMAP, 0, 0, LR_COPYDELETEORG);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//Generate the scale bitmap
////////////////////////////////////////////////////////////////////////////////////////////////////////
void CEffet::GetScaleBitmap(CIBitmap * m_ciBitmap, const int &iIconeWidth, const int &iIconeHeight, const char * szFilename)
{
	CLoadPicture * m_cLoadPicture = new CLoadPicture();
	CObjet m_cObjet;
	int m_iFormat = 0;
	double fratio = 0.0;
	int iIconeWidthLocal = iIconeWidth;
	int iIconeHeightLocal = iIconeHeight;

	int iTailleIconeWidth = iIconeWidth, iTailleIconeHeight = iIconeHeight;

	m_iFormat = m_cObjet.TestImageFormat(szFilename);
	if(m_iFormat == 1)
	{
		HBITMAP m_hBitmapLocal;
		m_cLoadPicture->GetImageFromJPG(szFilename,m_hBitmapLocal,0,0);
		m_ciBitmap->SetBitmap(m_hBitmapLocal,false);
		DeleteObject(m_hBitmapLocal);
	}
	else
		m_cLoadPicture->ChargementImage(m_iFormat,szFilename,m_ciBitmap,false);



	int iBitmapWidth = m_ciBitmap->GetBitmapWidth();
	int iBitmapHeight = m_ciBitmap->GetBitmapHeigth();

	if(iBitmapWidth == iBitmapHeight)
	{
		//Création d'une image correct
		if(iIconeWidth > iIconeHeight)
		{
			fratio = (float)iIconeHeightLocal / (float)iBitmapHeight;
			iTailleIconeWidth = fratio * iBitmapWidth;
		}
		else
		{
			fratio = (float)iIconeWidthLocal / (float)iBitmapWidth;
			iTailleIconeHeight = fratio * iBitmapHeight;
		}

	}
	else
	{
		//Création d'une image correct
		if(iBitmapWidth < iBitmapHeight)
		{
			fratio = (float)iIconeHeightLocal / (float)iBitmapHeight;
			iTailleIconeWidth = fratio * iBitmapWidth;
		}
		else
		{
			fratio = (float)iIconeWidthLocal / (float)iBitmapWidth;
			iTailleIconeHeight = fratio * iBitmapHeight;

		}
	}

	ScaleBitmap(m_ciBitmap,iTailleIconeWidth,iTailleIconeHeight,TRIANGLEFILTER);

	delete m_cLoadPicture;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//Generate the folder Icon
////////////////////////////////////////////////////////////////////////////////////////////////////////
HBITMAP CEffet::GenerateFolderIcone(HBITMAP hBitmapFolder, const char * szFolderName, const int &iIconeWidth, const int &iIconeHeight, const int &iMarge, const COLORREF &m_color, const COLORREF &rgb)
{


	HBITMAP hBitmap = NULL;
	char m_szFilename[MAX_PATH];
	//Chargement de 4 images
	CFichier * m_cFichier = new CFichier();
	strcpy_s(m_szFilename,MAX_PATH,szFolderName);
	strcat_s(m_szFilename,MAX_PATH,"\\");
	m_cFichier->DefinirRepertoire(m_szFilename,0,false,false);
	CIBitmap * m_ciBitmapArr = new CIBitmap[4];

	int iPosX = 0, iPosY = 0;
	RECT rc;
	rc.top = 0;
	rc.bottom = iIconeHeight;
	rc.left = 0;
	rc.right = iIconeWidth;

	// Create a memory DC compatible with the display
	HDC sourceDC, destDC;
	sourceDC = CreateCompatibleDC( GetDC(NULL) );
	destDC = CreateCompatibleDC( GetDC(NULL) );

	// Get logical coordinates
	BITMAP bm ;
	GetObject( hBitmapFolder, sizeof( bm ), &bm );

	// Create a bitmap to hold the result

	hBitmap = CreateCompatibleBitmap(GetDC(NULL), iIconeWidth, iIconeHeight);

	// Select bitmaps into the DCs
	SelectObject( sourceDC, hBitmapFolder );
	SelectObject( destDC, hBitmap );

	SetStretchBltMode(destDC,COLORONCOLOR);


	int m_iNbGetFiles = m_cFichier->GetNbFiles();
	if(m_iNbGetFiles >= 4)
	{
		int m_iIncre = m_iNbGetFiles / 4;
		//Chargement de la première
		m_cFichier->AffichageImagePremiere(m_szFilename,MAX_PATH);
		GetScaleBitmap(&m_ciBitmapArr[0], iIconeWidth / 3, iIconeHeight / 3,m_szFilename);
		for(int j = 0;j < 2;j++)
		{
			for(int i = 0;i < m_iIncre;i++)
			{
				m_cFichier->AffichageImageSuivante(m_szFilename,MAX_PATH);
			}
			GetScaleBitmap(&m_ciBitmapArr[j+1], iIconeWidth / 3, iIconeHeight / 3,m_szFilename);
		}

		//Chargement de la dernière
		m_cFichier->AffichageImageDerniere(m_szFilename,MAX_PATH);
		GetScaleBitmap(&m_ciBitmapArr[3], iIconeWidth / 3, iIconeHeight / 3,m_szFilename);

		FillRect(destDC, &rc, CreateSolidBrush(RGB(177,199,223))); 

		rc.bottom = 8;
		rc.top = 0;
		
		//FillRect(destDC, &rc, CreateSolidBrush(RGB(255,255,255))); 

		//rc.right = iIconeWidth / 2;
		FillRect(destDC, &rc, CreateSolidBrush(RGB(141,167,202))); 

		int iPosX = 10;
		int iPosY = 10;

		int m_iCenterX = (iIconeWidth / 2);
		int m_iCenterY = ((iIconeHeight + 10) / 2);

		iPosX = (m_iCenterX - m_ciBitmapArr[0].GetBitmapWidth()) / 2;
		iPosY = (m_iCenterY - m_ciBitmapArr[0].GetBitmapHeigth()) / 2;;//m_iCenterY - m_ciBitmapArr[0].GetBitmapHeigth();

		StretchDIBits(destDC, iPosX, iPosY, m_ciBitmapArr[0].GetBitmapWidth(), m_ciBitmapArr[0].GetBitmapHeigth(), 
				0, 0, m_ciBitmapArr[0].GetBitmapWidth(), m_ciBitmapArr[0].GetBitmapHeigth(), m_ciBitmapArr[0].GetPtBitmap(), &m_ciBitmapArr[0].bmiInfos, DIB_RGB_COLORS, SRCCOPY);

		iPosX = m_iCenterX + (m_iCenterX - m_ciBitmapArr[1].GetBitmapWidth()) / 2;
		iPosY = (m_iCenterY - m_ciBitmapArr[1].GetBitmapHeigth()) / 2;

		StretchDIBits(destDC, iPosX, iPosY, m_ciBitmapArr[1].GetBitmapWidth(), m_ciBitmapArr[1].GetBitmapHeigth(), 
				0, 0, m_ciBitmapArr[1].GetBitmapWidth(), m_ciBitmapArr[1].GetBitmapHeigth(), m_ciBitmapArr[1].GetPtBitmap(), &m_ciBitmapArr[1].bmiInfos, DIB_RGB_COLORS, SRCCOPY);

		iPosX = (m_iCenterX - m_ciBitmapArr[2].GetBitmapWidth()) / 2;
		iPosY = m_iCenterY;
		StretchDIBits(destDC, iPosX, iPosY, m_ciBitmapArr[2].GetBitmapWidth(), m_ciBitmapArr[2].GetBitmapHeigth(), 
				0, 0, m_ciBitmapArr[2].GetBitmapWidth(), m_ciBitmapArr[2].GetBitmapHeigth(), m_ciBitmapArr[2].GetPtBitmap(), &m_ciBitmapArr[2].bmiInfos, DIB_RGB_COLORS, SRCCOPY);

		iPosX = m_iCenterX + (m_iCenterX - m_ciBitmapArr[3].GetBitmapWidth()) / 2;
		iPosY = m_iCenterY;
		StretchDIBits(destDC, iPosX, iPosY, m_ciBitmapArr[3].GetBitmapWidth(), m_ciBitmapArr[3].GetBitmapHeigth(), 
				0, 0, m_ciBitmapArr[3].GetBitmapWidth(), m_ciBitmapArr[3].GetBitmapHeigth(), m_ciBitmapArr[3].GetPtBitmap(), &m_ciBitmapArr[3].bmiInfos, DIB_RGB_COLORS, SRCCOPY);
	}
	else if(m_iNbGetFiles > 0)
	{
		m_cFichier->AffichageImagePremiere(m_szFilename,MAX_PATH);
		GetScaleBitmap(&m_ciBitmapArr[0], iIconeWidth / 3, iIconeHeight / 3,m_szFilename);
		for(int i = 1;i < m_iNbGetFiles;i++)
		{
			m_cFichier->AffichageImageSuivante(m_szFilename,MAX_PATH);
			GetScaleBitmap(&m_ciBitmapArr[i], iIconeWidth / 3, iIconeHeight / 3,m_szFilename);
		}

		int iPosX = 10;
		int iPosY = 10;

		FillRect(destDC, &rc, CreateSolidBrush(RGB(177,199,223))); 

		rc.bottom = 8;
		rc.top = 0;

		FillRect(destDC, &rc, CreateSolidBrush(RGB(141,167,202))); 

		int m_iCenterX = (iIconeWidth / 2);
		int m_iCenterY = ((iIconeHeight + 10) / 2);

		for(int i = 0;i < m_iNbGetFiles;i++)
		{
			switch(i)
			{
			case 0:
				iPosX = (m_iCenterX - m_ciBitmapArr[0].GetBitmapWidth()) / 2;
				iPosY = (m_iCenterY - m_ciBitmapArr[0].GetBitmapHeigth()) / 2;;

				StretchDIBits(destDC, iPosX, iPosY, m_ciBitmapArr[0].GetBitmapWidth(), m_ciBitmapArr[0].GetBitmapHeigth(), 
						0, 0, m_ciBitmapArr[0].GetBitmapWidth(), m_ciBitmapArr[0].GetBitmapHeigth(), m_ciBitmapArr[0].GetPtBitmap(), &m_ciBitmapArr[0].bmiInfos, DIB_RGB_COLORS, SRCCOPY);
				break;

			case 1:
				iPosX = m_iCenterX + (m_iCenterX - m_ciBitmapArr[1].GetBitmapWidth()) / 2;
				iPosY = (m_iCenterY - m_ciBitmapArr[1].GetBitmapHeigth()) / 2;

				StretchDIBits(destDC, iPosX, iPosY, m_ciBitmapArr[1].GetBitmapWidth(), m_ciBitmapArr[1].GetBitmapHeigth(), 
						0, 0, m_ciBitmapArr[1].GetBitmapWidth(), m_ciBitmapArr[1].GetBitmapHeigth(), m_ciBitmapArr[1].GetPtBitmap(), &m_ciBitmapArr[1].bmiInfos, DIB_RGB_COLORS, SRCCOPY);
				break;

			case 2:
				iPosX = (m_iCenterX - m_ciBitmapArr[2].GetBitmapWidth()) / 2;
				iPosY = m_iCenterY;
				StretchDIBits(destDC, iPosX, iPosY, m_ciBitmapArr[2].GetBitmapWidth(), m_ciBitmapArr[2].GetBitmapHeigth(), 
						0, 0, m_ciBitmapArr[2].GetBitmapWidth(), m_ciBitmapArr[2].GetBitmapHeigth(), m_ciBitmapArr[2].GetPtBitmap(), &m_ciBitmapArr[2].bmiInfos, DIB_RGB_COLORS, SRCCOPY);
				break;

			case 3:
				iPosX = m_iCenterX + (m_iCenterX - m_ciBitmapArr[3].GetBitmapWidth()) / 2;
				iPosY = m_iCenterY;
				StretchDIBits(destDC, iPosX, iPosY, m_ciBitmapArr[3].GetBitmapWidth(), m_ciBitmapArr[3].GetBitmapHeigth(), 
						0, 0, m_ciBitmapArr[3].GetBitmapWidth(), m_ciBitmapArr[3].GetBitmapHeigth(), m_ciBitmapArr[3].GetPtBitmap(), &m_ciBitmapArr[3].bmiInfos, DIB_RGB_COLORS, SRCCOPY);
				break;
			}
		}
	}
	else
	{
		m_ciBitmapArr[0].SetBitmap(hBitmapFolder,false);
		ScaleBitmap(&m_ciBitmapArr[0],iIconeWidth,iIconeHeight,TRIANGLEFILTER);

		int iPosX = 0;
		int iPosY = 0;

		StretchDIBits(destDC, iPosX, iPosY, m_ciBitmapArr[0].GetBitmapWidth(), m_ciBitmapArr[0].GetBitmapHeigth(), 
				0, 0, m_ciBitmapArr[0].GetBitmapWidth(), m_ciBitmapArr[0].GetBitmapHeigth(), m_ciBitmapArr[0].GetPtBitmap(), &m_ciBitmapArr[0].bmiInfos, DIB_RGB_COLORS, SRCCOPY);

	}

	CDessin m_CDessin;
	m_CDessin.DessinerCarre(destDC, iIconeWidth, iIconeHeight, iMarge,0,0, rgb);

	delete m_cFichier;
	delete[] m_ciBitmapArr;

	DeleteDC(sourceDC);
	DeleteDC(destDC);

	return (HBITMAP)CopyImage(hBitmap, IMAGE_BITMAP, 0, 0, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//Generate the folder Icon
////////////////////////////////////////////////////////////////////////////////////////////////////////
HBITMAP CEffet::GenerateIcone(HBITMAP hBitmap, const int &iIconeWidth, const int &iIconeHeight, unsigned int &Width, unsigned int &Height, const int &iMarge, const COLORREF &m_color, const COLORREF &rgb)
{
	if(hBitmap == NULL)
		return NULL;

	CDessin m_CDessin;
	HBITMAP hBitmapResult;
	float fratio;

	int iMargeLocal = 10;

	int iIconeWidthLocal = iIconeWidth - iMargeLocal,  iIconeHeightLocal = iIconeHeight - iMargeLocal;

	int iTailleIconeWidth = iIconeWidthLocal, iTailleIconeHeight = iIconeHeightLocal;

	
	if(iMarge == 0)
	{
		iIconeWidthLocal = iIconeWidth;
		iIconeHeightLocal = iIconeHeight;
		iTailleIconeWidth = iIconeWidthLocal;
		iTailleIconeHeight = iIconeHeightLocal;
		iMargeLocal = 0;

	}


	int iPosX = 0, iPosY = 0;
	RECT rc;


	rc.top = 0;
	rc.bottom = iIconeHeight;
	rc.left = 0;
	rc.right = iIconeWidth;
	
	// Create a memory DC compatible with the display
	HDC sourceDC, destDC;
	sourceDC = CreateCompatibleDC( GetDC(NULL) );
	destDC = CreateCompatibleDC( GetDC(NULL) );

	// Get logical coordinates
	BITMAP bm ;
	GetObject( hBitmap, sizeof( bm ), &bm );

	if(bm.bmWidth == bm.bmHeight)
	{
		//Création d'une image correct
		if(iIconeWidth > iIconeHeight)
		{
			fratio = (float)iIconeHeightLocal / (float)bm.bmHeight;
			iTailleIconeWidth = fratio * bm.bmWidth;
			iPosX = (iIconeWidthLocal - iTailleIconeWidth) >> 1;
		}
		else
		{
			fratio = (float)iIconeWidthLocal / (float)bm.bmWidth;
			iTailleIconeHeight = fratio * bm.bmHeight;
			iPosY = (iIconeHeightLocal - iTailleIconeHeight) >> 1;

		}

	}
	else
	{
		//Création d'une image correct
		if(bm.bmWidth < bm.bmHeight)
		{
			fratio = (float)iIconeHeightLocal / (float)bm.bmHeight;
			iTailleIconeWidth = fratio * bm.bmWidth;
			iPosX = (iIconeWidthLocal - iTailleIconeWidth) >> 1;
		}
		else
		{
			fratio = (float)iIconeWidthLocal / (float)bm.bmWidth;
			iTailleIconeHeight = fratio * bm.bmHeight;
			iPosY = (iIconeHeightLocal - iTailleIconeHeight) >> 1;

		}
	}

	// Create a bitmap to hold the result

	hBitmapResult = CreateCompatibleBitmap(GetDC(NULL), iIconeWidth, iIconeHeight);

	// Select bitmaps into the DCs
	SelectObject( sourceDC, hBitmap );
	SelectObject( destDC, hBitmapResult );

	SetStretchBltMode(destDC,COLORONCOLOR);

	FillRect(destDC, &rc, CreateSolidBrush(m_color)); 
	//FillRect(destDC, &rc, CreateSolidBrush(BLACK_BRUSH) ); 
	
	if(iMargeLocal > 0)
	{
		StretchBlt(destDC, iPosX + (iMargeLocal >> 1), iPosY + (iMargeLocal >> 1), iTailleIconeWidth, iTailleIconeHeight, sourceDC, 
				0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY );
	}
	else
	{
		StretchBlt(destDC, iPosX, iPosY, iTailleIconeWidth, iTailleIconeHeight, sourceDC, 
				0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY );
	}

	if(iMarge > 0)
	{
		m_CDessin.DessinerCarre(destDC, iIconeWidth, iIconeHeight, iMarge,0,0, rgb);
	}

	DeleteDC(sourceDC);
	DeleteDC(destDC);

	Width = iTailleIconeWidth + iPosX;
	Height = iTailleIconeHeight + iPosY;

	return (HBITMAP)CopyImage(hBitmapResult, IMAGE_BITMAP, 0, 0, LR_COPYDELETEORG);
}