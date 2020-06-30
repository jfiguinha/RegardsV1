// IBitmap.cpp: implementation of the CIBitmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IBitmap.h"
#include "effet.h"

/*
#include <gl\gl.h>	
#include <gl\glaux.h>	
#if !(defined(__ICC) || defined(__ICL) || defined(__ECC) || defined(__ECL))
	#include <malloc.h>
#endif
*/

//-------------------------------------------------------
//Constructeur par copy
//-------------------------------------------------------
CIBitmap::CIBitmap(const CIBitmap &m_ciBitmap)
{

	m_iWidth = m_ciBitmap.m_iWidth;
	m_iHeight = m_ciBitmap.m_iHeight;
	
	if(m_iWidth == 0 && m_iHeight == 0)
	{
		m_bData = NULL;
		m_sDepth = 32;
		m_blSimple = false;
		bInUse = false;
		iTypeMemoryAllocation = 0;
		hBitmap = NULL;
		m_iWidth = 0;
		m_iHeight = 0;
		m_lSize = 0;
		m_bShow = false;
		m_bSize = false;
		hBitmapHisto = NULL;
		return;
	}

	int iTaille = 4;

	memset(&bmiInfos,0,sizeof(BITMAPINFO));
	bmiInfos.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiInfos.bmiHeader.biWidth = m_iWidth;
	bmiInfos.bmiHeader.biHeight = m_iHeight;
	bmiInfos.bmiHeader.biPlanes = 1;
	bmiInfos.bmiHeader.biBitCount = 32;
	bmiInfos.bmiHeader.biCompression = BI_RGB;
	bmiInfos.bmiHeader.biSizeImage = iTaille * m_iWidth * m_iHeight; 

	if(m_bData != NULL || hBitmap != NULL)
	{
		if(m_bData != NULL)
		{
			switch(iTypeMemoryAllocation)
			{
				case 0:
					delete[] m_bData;
					break;

				case 1:
					HeapFree (GetProcessHeap(), 0, m_bData) ;
					break;
			}

			m_bData = NULL;
		}

		if(hBitmap != NULL)
		{
			//DeleteObject(hBitmap);
			hBitmap = NULL;
		}		
	}

	m_blSimple = false;
	iTypeMemoryAllocation = 0;

	m_bData = new BYTE[bmiInfos.bmiHeader.biSizeImage + bmiInfos.bmiHeader.biWidth * 4];

	if(m_bData == NULL)
	{
		//Probléme d'allocation mémoire passage par un heap
		iTypeMemoryAllocation = 1;
		m_bData = (BYTE *) HeapAlloc (GetProcessHeap (),
				HEAP_ZERO_MEMORY, bmiInfos.bmiHeader.biSizeImage + bmiInfos.bmiHeader.biWidth * 4) ;
	}

	memcpy(m_bData, m_ciBitmap.m_bData, bmiInfos.bmiHeader.biSizeImage);

	hBitmap = CreateDIBitmap (GetDC(NULL),              
				&bmiInfos.bmiHeader,
				CBM_INIT,
				m_bData,
				(LPBITMAPINFO)&bmiInfos,
				DIB_RGB_COLORS);

	/*
	BITMAP bitmap;

	bitmap.bmWidth = bmiInfos.bmiHeader.biWidth;
	bitmap.bmHeight = bmiInfos.bmiHeader.biHeight;

	int iTaille = 4;

	memzero(&m_ciBitmap.bmiInfos,sizeof(BITMAPINFO));
	m_ciBitmap.bmiInfos.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_ciBitmap.bmiInfos.bmiHeader.biWidth = bitmap.bmWidth;
	m_ciBitmap.bmiInfos.bmiHeader.biHeight = bitmap.bmHeight;
	m_ciBitmap.bmiInfos.bmiHeader.biPlanes = 1;
	m_ciBitmap.bmiInfos.bmiHeader.biBitCount = 32;
	m_ciBitmap.bmiInfos.bmiHeader.biCompression = BI_RGB;
	m_ciBitmap.bmiInfos.bmiHeader.biSizeImage = iTaille * bitmap.bmWidth * bitmap.bmHeight; 

	m_ciBitmap.m_iWidth = bitmap.bmWidth;
	m_ciBitmap.m_iHeight = bitmap.bmHeight;


	if(m_ciBitmap.m_bData != NULL || m_ciBitmap.hBitmap != NULL)
	{
		if(m_ciBitmap.m_bData != NULL)
		{
			switch(m_ciBitmap.iTypeMemoryAllocation)
			{
				case 0:
					delete[] m_ciBitmap.m_bData;
					break;

				case 1:
					HeapFree (GetProcessHeap(), 0, m_ciBitmap.m_bData) ;
					break;
			}

			m_ciBitmap.m_bData = NULL;
		}

		if(m_ciBitmap.hBitmap != NULL)
		{
			DeleteObject(m_ciBitmap.hBitmap);
			m_ciBitmap.hBitmap = NULL;
		}		
	}

	m_ciBitmap.iTypeMemoryAllocation = 0;
	m_ciBitmap.m_bData = new BYTE[m_ciBitmap.bmiInfos.bmiHeader.biSizeImage + bmiInfos.bmiHeader.biWidth * 4];

	if(m_ciBitmap.m_bData == NULL)
	{
		//Probléme d'allocation mémoire passage par un heap
		m_ciBitmap.iTypeMemoryAllocation = 1;
		m_ciBitmap.m_bData = (BYTE *) HeapAlloc (GetProcessHeap (),
				HEAP_ZERO_MEMORY, m_ciBitmap.bmiInfos.bmiHeader.biSizeImage + bmiInfos.bmiHeader.biWidth * 4) ;
	}

	memcpy(m_ciBitmap.m_bData, m_bData, m_ciBitmap.bmiInfos.bmiHeader.biSizeImage);

	m_ciBitmap.hBitmap = CreateDIBitmap (GetDC(NULL),              
				&bmiInfos.bmiHeader,
				CBM_INIT,
				m_ciBitmap.m_bData,
				(LPBITMAPINFO)&bmiInfos,
				DIB_RGB_COLORS) ;
	*/
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIBitmap::CIBitmap()
{
	m_bData = NULL;
	m_sDepth = 32;
	m_blSimple = false;
	bInUse = false;
	iTypeMemoryAllocation = 0;
	hBitmap = NULL;
	m_iWidth = 0;
	m_iHeight = 0;
	m_lSize = 0;
	m_bShow = false;
	m_bSize = false;
	hBitmapHisto = NULL;
}


CIBitmap::~CIBitmap()
{
	EraseData();
}

void CIBitmap::SetBitmapWidth(const unsigned int &bmWidth)
{
	m_iWidth = bmWidth;

}

void CIBitmap::SetBitmapHeigth(const unsigned int &bmHeight)
{
	m_iHeight = bmHeight;
}


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void CIBitmap::DesAllocationMemoire()
{
	
	if(m_bData != NULL)
	{
		switch(iTypeMemoryAllocation)
		{
			case 0:
				delete[] m_bData;
				break;

			case 1:
				HeapFree (GetProcessHeap(), 0, m_bData) ;
				break;
		}

		m_bData = NULL;
	}

	if(hBitmap != NULL)
	{
		DeleteObject(hBitmap);
		hBitmap = NULL;
	}

}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void CIBitmap::AllocationMemoire(const long &m_lSize)
{
	if(m_bData != NULL || hBitmap != NULL)
	{
		DesAllocationMemoire();
	}

	iTypeMemoryAllocation = 0;
	m_bData = new BYTE[m_lSize + bmiInfos.bmiHeader.biWidth * 4];

	memset(m_bData,0,m_lSize + bmiInfos.bmiHeader.biWidth * 4);

	if(m_bData == NULL)
	{
		//Probléme d'allocation mémoire passage par un heap
		iTypeMemoryAllocation = 1;
		m_bData = (BYTE *) HeapAlloc (GetProcessHeap (),
				HEAP_ZERO_MEMORY, m_lSize + bmiInfos.bmiHeader.biWidth * 4) ;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void CIBitmap::SpeedPictureShow(HWND hWnd)
{
	HDC hdc = GetDC(hWnd);
	HDC hdcMem = CreateCompatibleDC(hdc); 

	SelectObject(hdcMem, hBitmap);

	BitBlt(hdc,0,0, GetBitmapWidth(), GetBitmapHeigth(), hdcMem, 0, 0,SRCCOPY); 

	DeleteDC (hdcMem);

	ReleaseDC(hWnd,hdc);
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void CIBitmap::EraseData()
{
	if(!m_blSimple)
	{
		DesAllocationMemoire();
	}
	if(hBitmap != NULL)
	{
		DeleteObject(hBitmap);
		hBitmap = NULL;
	}
	if(hBitmapHisto != NULL)
	{
		DeleteObject(hBitmapHisto);
		hBitmapHisto = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void CIBitmap::ShowBitmap(HDC &hdc, const int &left,const int &top, int &xValue, int &yValue, const int &m_bHauteQualite, const bool &m_bAutoShrink, const int &nTailleAffichageWidth, const int &nTailleAffichageHeight, const float &fratio)
{
	if(bInUse)
	{
		return;
	}

	#pragma omp critical(bInUse)
	{
		bInUse = true;
	}

	if(m_bHauteQualite > 0 && m_bAutoShrink && !m_bSize)
	{
		CIBitmap * m_ciBitmap = NULL;
		CEffet m_CEffet;
		bool m_bOK = true;
		if(nTailleAffichageWidth != 0 && nTailleAffichageHeight != 0)
		{
			try
			{
				m_ciBitmap = new CIBitmap();
				m_ciBitmap->SetBitmap(this->m_bData,this->m_iWidth,this->m_iHeight);				
				m_CEffet.ScaleBitmap(m_ciBitmap, nTailleAffichageWidth, nTailleAffichageHeight, m_bHauteQualite);


				//m_ciBitmap = new CIBitmap();
				//BYTE * m_dataOut = new BYTE[nTailleAffichageWidth * nTailleAffichageHeight * 4];
				//gluScaleImage(GL_RGBA,this->m_iWidth,this->m_iHeight,GL_UNSIGNED_BYTE,this->m_bData,nTailleAffichageWidth,nTailleAffichageHeight,GL_UNSIGNED_BYTE,m_dataOut);
				//m_ciBitmap->SetAffectationSimple(true);
				//m_ciBitmap->SetBitmap(m_dataOut,nTailleAffichageWidth,nTailleAffichageHeight);	
				//delete[] m_dataOut;
				
			}
			catch(...)
			{
				char cTempInfo[255];
				//sprintf_s(cTempInfo,255,"Width : %d Height : %d",nTailleAffichageWidth,nTailleAffichageHeight);
				//::MessageBox(NULL,cTempInfo,"Error",0);
				m_bOK = false;
			}

			if(m_bOK)
			{

				HDC hdcMem = CreateCompatibleDC(hdc); 

				SelectObject(hdcMem, m_ciBitmap->hBitmap);

				BitBlt(hdc,left,top, nTailleAffichageWidth, nTailleAffichageHeight, hdcMem, 0, 0,SRCCOPY); 

				DeleteDC (hdcMem);

				//DeleteObject(hBitmapTemp);
			}
			else
			{
				//DeleteObject(hBitmapTemp);
				SetStretchBltMode(hdc,COLORONCOLOR);

				StretchDIBits(hdc, left, top, nTailleAffichageWidth, nTailleAffichageHeight, 
					xValue / fratio, -(yValue / fratio), m_iWidth, m_iHeight,
					m_bData, &bmiInfos, DIB_RGB_COLORS, SRCCOPY);
			}

			//delete m_ciBitmap;
		}

		if(m_ciBitmap != NULL)
			delete m_ciBitmap;

	}
	else
	{

		//if(!m_blSimple)
		//{
			SetStretchBltMode(hdc,COLORONCOLOR);

			StretchDIBits(hdc, left, top, nTailleAffichageWidth, nTailleAffichageHeight, 
				xValue / fratio, -(yValue / fratio), m_iWidth, m_iHeight,
				m_bData, &bmiInfos, DIB_RGB_COLORS, SRCCOPY);

	}

	#pragma omp critical(bInUse)
	{
		bInUse = false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void CIBitmap::SetBitmap(BYTE * m_bBuffer, const unsigned int &bmWidth,const unsigned int &bmHeight, const bool &m_bFlip, const int &m_iBitCount)
{

	CEffet cEffet;

	//EraseData();

	BITMAP bitmap;

	bitmap.bmWidth = bmWidth;
	bitmap.bmHeight = bmHeight;
	bitmap.bmBitsPixel = m_iBitCount;

	CreateHeaderInformation(bitmap);

	if(!m_blSimple)
	{

		AllocationMemoire(bmiInfos.bmiHeader.biSizeImage);


		if(m_bBuffer != NULL)
		{
			memcpy(m_bData, m_bBuffer, bmiInfos.bmiHeader.biSizeImage);
		}

		if(m_bFlip)
			VertFlipBuf();


		hBitmap = CreateDIBitmap (GetDC(NULL),              
						   &bmiInfos.bmiHeader,
						   CBM_INIT,
						   m_bData,
						   (LPBITMAPINFO)&bmiInfos,
						   DIB_RGB_COLORS) ;


	}
	else
	{

		hBitmap = CreateDIBitmap (GetDC(NULL),              
						   &bmiInfos.bmiHeader,
						   CBM_INIT,
						   m_bBuffer,
						   (LPBITMAPINFO)&bmiInfos,
						   DIB_RGB_COLORS) ;

		if(m_bFlip)
			hBitmap = cEffet.GetFlipBitmap(this,false);

	}


}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void CIBitmap::SetBitmap(COLORREF * m_bBuffer, const unsigned int &bmWidth,const unsigned int &bmHeight, const bool &m_bFlip, const int &m_iBitCount)
{

	CEffet cEffet;

	//EraseData();

	BITMAP bitmap;

	bitmap.bmWidth = bmWidth;
	bitmap.bmHeight = bmHeight;
	bitmap.bmBitsPixel = m_iBitCount;

	CreateHeaderInformation(bitmap);

	if(!m_blSimple)
	{

		AllocationMemoire(bmiInfos.bmiHeader.biSizeImage);


		if(m_bBuffer != NULL)
		{
			memcpy(m_bData, m_bBuffer, bmWidth * bmHeight * 4);
		}

		if(m_bFlip)
			VertFlipBuf();


		hBitmap = CreateDIBitmap (GetDC(NULL),              
						   &bmiInfos.bmiHeader,
						   CBM_INIT,
						   m_bBuffer,
						   (LPBITMAPINFO)&bmiInfos,
						   DIB_RGB_COLORS) ;

		m_iWidth = bmWidth;
		m_iHeight = bmHeight;

	}
	else
	{
		hBitmap = CreateDIBitmap (GetDC(NULL),              
						   &bmiInfos.bmiHeader,
						   CBM_INIT,
						   m_bBuffer,
						   (LPBITMAPINFO)&bmiInfos,
						   DIB_RGB_COLORS) ;

		if(m_bFlip)
			hBitmap = cEffet.GetFlipBitmap(this,false);

	}


}



//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void CIBitmap::ReCreateHBitmap(const bool &m_bFlip)
{
	if(hBitmap != NULL)
	{
		DeleteObject(hBitmap);
		hBitmap = NULL;
	}

	CEffet cEffet;

	BITMAP bitmap;

	hBitmap = CreateDIBitmap (GetDC(NULL),              
		   &bmiInfos.bmiHeader,
		   CBM_INIT,
		   m_bData,
		   (LPBITMAPINFO)&bmiInfos,
		   DIB_RGB_COLORS) ;


	if(m_bFlip)
		hBitmap = cEffet.GetFlipBitmap(this,false);

	if(m_blSimple)
	{
		if(m_bData != NULL)
		{
			switch(iTypeMemoryAllocation)
			{
				case 0:
					_mm_free(m_bData);
					break;

				case 1:
					HeapFree (GetProcessHeap(), 0, m_bData) ;
					break;
			}

			m_bData = NULL;
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void CIBitmap::SetBitmapData(BYTE * m_bBuffer, const unsigned int &bmWidth,const unsigned int &bmHeight)
{
	//EraseData();

	BITMAP bitmap;

	bitmap.bmWidth = bmWidth;
	bitmap.bmHeight = bmHeight;
	bitmap.bmBitsPixel = DEPTHDEFO;

	CreateHeaderInformation(bitmap);

	AllocationMemoire(bmiInfos.bmiHeader.biSizeImage);

	memcpy(m_bData, m_bBuffer, bmiInfos.bmiHeader.biSizeImage);

	m_iWidth = bmWidth;
	m_iHeight = bmHeight;

}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void CIBitmap::GetBitmap(BYTE ** m_bData, unsigned int &bmWidth, unsigned int &bmHeight, unsigned long &m_lSize)
{
	if(*m_bData != NULL)
	{
		delete[] *m_bData;
		*m_bData = NULL;
	}

	m_lSize = bmiInfos.bmiHeader.biWidth * bmiInfos.bmiHeader.biHeight * 4;//bmiInfos.bmiHeader.biSizeImage;
	*m_bData = new BYTE[bmiInfos.bmiHeader.biSizeImage];
	
	memcpy(*m_bData,this->m_bData,bmiInfos.bmiHeader.biSizeImage);

	bmWidth = bmiInfos.bmiHeader.biWidth;
	bmHeight = bmiInfos.bmiHeader.biHeight;
	
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void CIBitmap::GetPtBitmap(BYTE ** m_bData, unsigned int &bmWidth, unsigned int &bmHeight, unsigned long &m_lSize)
{
	m_lSize = bmiInfos.bmiHeader.biWidth * bmiInfos.bmiHeader.biHeight * 4;//bmiInfos.bmiHeader.biSizeImage;
	*m_bData = this->m_bData;
	bmWidth = bmiInfos.bmiHeader.biWidth;
	bmHeight = bmiInfos.bmiHeader.biHeight;	
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

BYTE * CIBitmap::GetPtBitmap()
{
	return m_bData;
}

COLORREF * CIBitmap::GetColorRefPt()
{
	return (COLORREF *)m_bData;
}


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void CIBitmap::GetBandWBitmap(BYTE * m_bData, unsigned int &bmWidth, unsigned int &bmHeight, unsigned long &m_lSize)
{
	
	int i,j,m,k = 0;

	int lWidthSize = GetWidthSize();

	bmWidth = bmiInfos.bmiHeader.biWidth;
	bmHeight = bmiInfos.bmiHeader.biHeight;

	m_lSize = bmWidth * bmHeight;

	//#pragma omp parallel private(i,j,m,k)
	//{
		for(i = 0;i < bmHeight;i++)
		{
			m = (i * lWidthSize);

			//k = i * bmWidth;

			//#pragma omp for schedule(dynamic,1) nowait
			
			for(j = 0;j < bmWidth;j++)
			{
				int l = m + (j * 4);
				int k = i * bmWidth + j;

				//int Moyenne = ((((*(this->m_bData + l) + *(this->m_bData + l + 1)) >> 1) + *(this->m_bData + l + 2)) >> 1);

				int Moyenne = ((*(this->m_bData + l) * 0.144f) + (*(this->m_bData + l + 1) * 0.587f) + (*(this->m_bData + l + 2) * 0.299f));
				
				Moyenne < 128 ? *(m_bData + k) = 0: *(m_bData + k) = 255;
			}
		}
	//}
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

int CIBitmap::SetBitmap(const HBITMAP &hBitmapTemp, const bool m_bFlip)
{

	CEffet cEffet;
	BITMAP bitmap;
	HDC hDC = GetDC(NULL);

	if (hBitmapTemp)
	{

		//EraseData();

		GetObject (hBitmapTemp, sizeof (BITMAP), &bitmap);

		CreateHeaderInformation(bitmap);

		AllocationMemoire(bmiInfos.bmiHeader.biSizeImage);

		GetDIBits(hDC, hBitmapTemp, 0, bitmap.bmHeight, 
				 m_bData, &bmiInfos, DIB_RGB_COLORS); 



		if(m_bFlip)
		{
			VertFlipBuf();
			//hBitmap = cEffet.GetFlipBitmap(hBitmap,false);
		}

		hBitmap = CreateDIBitmap (hDC,              
							   &bmiInfos.bmiHeader,
							   CBM_INIT,
							   m_bData,
							   (LPBITMAPINFO)&bmiInfos,
							   DIB_RGB_COLORS) ;


		m_iWidth = bitmap.bmWidth;
		m_iHeight = bitmap.bmHeight;


		if(m_blSimple)
		{
			if(m_bData != NULL)
			{
				switch(iTypeMemoryAllocation)
				{
					case 0:
						_mm_free(m_bData);
						break;

					case 1:
						HeapFree (GetProcessHeap(), 0, m_bData) ;
						break;
				}

				m_bData = NULL;
			}
		}

	}
	else
	{
		//EraseData();
		m_iWidth = 0;
		m_iHeight = 0;
	}

	DeleteDC(hDC);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

int CIBitmap::SetColorTranspBitmap(COLORREF Transp)
{
	if(m_bData != NULL)
	{
		int i = 0;
        for(int y = 0;y < GetBitmapHeigth();y++)
		{
			for(int x = 0;x < GetBitmapWidth();x++)
			{	
				int Blue  = *(m_bData + i);
				int Green = *(m_bData + i + 1);
				int Red   = *(m_bData + i + 2);

				if((Blue == GetBValue(Transp)) && (Green == GetGValue(Transp)) && (Red == GetRValue(Transp)))
					*(m_bData + i + 3) = 0;
				else
					*(m_bData + i + 3) = 255;

				i += 4;
			}
		}
	}	
	return 0;
}

int CIBitmap::InsertBitmap(CIBitmap * ciBitmap,int xPos, int yPos)
{
	if(m_bData != NULL)
	{
		int yEnd = yPos + ciBitmap->GetBitmapHeigth();
		int xEnd = xPos + ciBitmap->GetBitmapWidth();

		if(yEnd > m_iHeight)
			yEnd = m_iHeight;

		if(xEnd > m_iWidth)
			xEnd = m_iWidth;

        for(int y = yPos;y < yEnd;y++)
		{
			for(int x = xPos;x < xEnd;x++)
			{	
				COLORREF color;
				ciBitmap->GetRGBColorValue(x - xPos,y - yPos, color);
				long lWidthSize = GetWidthSize();
				int i = (y * lWidthSize) + (x * 4);
				*(m_bData + i) = GetBValue(color);
				*(m_bData + i + 1) = GetGValue(color);
				*(m_bData + i + 2) = GetRValue(color);
			}
		}
	}

	return 0;
}

int CIBitmap::DessinerCarre(int iMarge, const COLORREF &color)
{
	if(m_bData != NULL)
	{
        for(int y = 0;y < m_iHeight;y++)
		{
			if(y < iMarge || y >= (m_iHeight - iMarge))
			{
				for(int x = 0;x < m_iWidth;x++)
				{	
					long lWidthSize = GetWidthSize();
					int i = (y * lWidthSize) + (x * 4);
					*(m_bData + i) = GetBValue(color);
					*(m_bData + i + 1) = GetGValue(color);
					*(m_bData + i + 2) = GetRValue(color);
				}
			}
			else
			{
				for(int x = 0;x < iMarge;x++)
				{	
					long lWidthSize = GetWidthSize();
					int i = (y * lWidthSize) + (x * 4);
					*(m_bData + i) = GetBValue(color);
					*(m_bData + i + 1) = GetGValue(color);
					*(m_bData + i + 2) = GetRValue(color);
				}

				for(int x = m_iWidth - iMarge;x < m_iWidth;x++)
				{	
					long lWidthSize = GetWidthSize();
					int i = (y * lWidthSize) + (x * 4);
					*(m_bData + i) = GetBValue(color);
					*(m_bData + i + 1) = GetGValue(color);
					*(m_bData + i + 2) = GetRValue(color);
				}

			}

		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////
//On rempli une zone de la meme couleur
/////////////////////////////////////////////////////////////
int CIBitmap::DessinerCarre(const RECT * lprc, const COLORREF &color)
{
	if(m_bData != NULL)
	{
		for(int y = lprc->top;y < lprc->bottom;y++)
		{
			for(int x = lprc->left;x < lprc->right;x++)
			{	
				long lWidthSize = GetWidthSize();
				int i = (y * lWidthSize) + (x * 4);
				*(m_bData + i) = GetBValue(color);
				*(m_bData + i + 1) = GetGValue(color);
				*(m_bData + i + 2) = GetRValue(color);
			}
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

int CIBitmap::DrawColor(const COLORREF &color)
{
	if(m_bData != NULL)
	{
        for(int y = 0;y < m_iHeight;y++)
		{
			for(int x = 0;x < m_iWidth;x++)
			{	
				long lWidthSize = GetWidthSize();
				int i = (y * lWidthSize) + (x * 4);
				*(m_bData + i) = GetBValue(color);
				*(m_bData + i + 1) = GetGValue(color);
				*(m_bData + i + 2) = GetRValue(color);
			}
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

int CIBitmap::SetBitmap(const HBITMAP &hBitmapTemp, const RECT &rc, const bool &m_bFlip)
{

	CEffet cEffet;
	BITMAP bitmap;
	HDC hDC = GetDC(NULL);

	if (hBitmapTemp)
	{

		//EraseData();

		GetObject (hBitmapTemp, sizeof (BITMAP), &bitmap);

		CreateHeaderInformation(bitmap);

		AllocationMemoire(bmiInfos.bmiHeader.biSizeImage);

		GetDIBits(hDC, hBitmapTemp, 0, bitmap.bmHeight, 
				 m_bData, &bmiInfos, DIB_RGB_COLORS); 


		hBitmap = CreateDIBitmap (hDC,              
							   &bmiInfos.bmiHeader,
							   CBM_INIT,
							   m_bData,
							   (LPBITMAPINFO)&bmiInfos,
							   DIB_RGB_COLORS) ;


		m_iWidth = bitmap.bmWidth;
		m_iHeight = bitmap.bmHeight;

		if(m_bFlip)
			hBitmap = cEffet.GetFlipBitmap(this,false);

		if(m_blSimple)
		{
			//DesAllocationMemoire();
			//delete[] m_bData;
			//m_bData = NULL;
		}

	}
	else
	{
		//EraseData();
		m_iWidth = 0;
		m_iHeight = 0;
	}

	DeleteDC(hDC);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

const unsigned int CIBitmap::GetBitmapWidth()
{
	return bmiInfos.bmiHeader.biWidth;
}

const unsigned int CIBitmap::GetBitmapHeigth()
{
	return bmiInfos.bmiHeader.biHeight;
}

const unsigned short CIBitmap::GetBitmapDepth()
{
	return bmiInfos.bmiHeader.biBitCount;
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void CIBitmap::SetFilename(const char *szFilename)
{
	StringCchCopy(FileName,MAX_PATH, szFilename);
}

void CIBitmap::GetFilename(char *szFilename)
{
	StringCchCopy(szFilename,MAX_PATH,FileName);
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void CIBitmap::SetAffectationSimple(const bool &m_blSimple)
{
	this->m_blSimple = m_blSimple;	
}

bool CIBitmap::GetAffectationSimple()
{
	return m_blSimple;
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void CIBitmap::GetColorValue(const int &x, const int &y, COLORREF &m_cValue)
{
	if(m_bData != NULL)
	{
		long lWidthSize = GetWidthSize();

		int i = (y * lWidthSize) + (x * 4);

		m_cValue = RGB(*(m_bData + i),*(m_bData + i + 1),*(m_bData + i + 2));

	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void CIBitmap::GetRGBColorValue(const int &x, const int &y, COLORREF &m_cValue)
{
	if(m_bData != NULL && x >= 0 && y >= 0 && x < bmiInfos.bmiHeader.biWidth && y < bmiInfos.bmiHeader.biHeight)
	{
		long lWidthSize = GetWidthSize();

		int i = (y * lWidthSize) + (x * 4);

		m_cValue = RGB(*(m_bData + i + 2),*(m_bData + i + 1),*(m_bData + i));

	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void CIBitmap::SetColorValue(const int &x, const int &y,const COLORREF &m_cValue)
{
	if(m_bData != NULL && x >= 0 && y >= 0 && x < bmiInfos.bmiHeader.biWidth && y < bmiInfos.bmiHeader.biHeight)
	{
		long lWidthSize = GetWidthSize();
		int i = (y * lWidthSize) + (x * 4);
		*(m_bData + i) = GetBValue(m_cValue);
		*(m_bData + i + 1) = GetGValue(m_cValue);
		*(m_bData + i + 2) = GetRValue(m_cValue);

	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

long CIBitmap::GetBitmapSize()
{
	return bmiInfos.bmiHeader.biSizeImage;
}

void CIBitmap::GetBitmap(HBITMAP &hBitmap)
{
	HDC hDC = GetDC(NULL);
	hBitmap = CreateDIBitmap (hDC,              
						   &bmiInfos.bmiHeader,
						   CBM_INIT,
						   m_bData,
						   (LPBITMAPINFO)&bmiInfos,
						   DIB_RGB_COLORS) ;
	DeleteDC(hDC);
}

//DEL void CIBitmap::GetBase64(char *m_cBase64)
//DEL {
//DEL 	Base64 m_Base64;
//DEL 	m_Base64.Base64ByteEnc(m_bData, bmiInfos.bmiHeader.biSizeImage,m_cBase64);
//DEL }

int CIBitmap::GetPaletteSize(BITMAPINFOHEADER * bmInfo)
{
	switch(bmInfo->biBitCount)
	{
	case 1:
			return 2;
	case 4:
			return 16;
	case 8:
			return 256;
	default:
			return 0;
	}
}


/****************************************************************************
 *                                                                          *
 *  FUNCTION   : DibNumColors(VOID FAR * pv)                                *
 *                                                                          *
 *  PURPOSE    : Determines the number of colors in the DIB by looking at   *
 *               the BitCount filed in the info block.                      *
 *                                                                          *
 *  RETURNS    : The number of colors in the DIB.                           *
 *                                                                          *
 ****************************************************************************/

WORD CIBitmap::DibNumColors (VOID FAR * pv)
{
    INT                 bits;
    LPBITMAPINFOHEADER  lpbi;
    LPBITMAPCOREHEADER  lpbc;

    lpbi = ((LPBITMAPINFOHEADER)pv);
    lpbc = ((LPBITMAPCOREHEADER)pv);

    /*  With the BITMAPINFO format headers, the size of the palette
     *  is in biClrUsed, whereas in the BITMAPCORE - style headers, it
     *  is dependent on the bits per pixel ( = 2 raised to the power of
     *  bits/pixel).
     */
    if (lpbi->biSize != sizeof(BITMAPCOREHEADER)){
        if (lpbi->biClrUsed != 0)
            return (WORD)lpbi->biClrUsed;
        bits = lpbi->biBitCount;
    }
    else
        bits = lpbc->bcBitCount;

    switch (bits){
        case 1:
                return 2;
        case 4:
                return 16;
        case 8:
                return 256;
        default:
                /* A 24 bitcount DIB has no color table */
                return 0;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

WORD CIBitmap::PaletteSize (VOID FAR * pv)
{
    LPBITMAPINFOHEADER lpbi;
    WORD               NumColors;

    lpbi      = (LPBITMAPINFOHEADER)pv;
    NumColors = DibNumColors(lpbi);

    if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
        return (WORD)(NumColors * sizeof(RGBTRIPLE));
    else
        return (WORD)(NumColors * sizeof(RGBQUAD));
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

HBITMAP CIBitmap::HandleToBitmap(HANDLE hDIB, int bits, HPALETTE   hpal)
{

    LPBITMAPINFOHEADER  lpbi;
    HPALETTE            hpalT;
    HDC                 hdc;
	UCHAR *lpVoid;
	RGBQUAD *pRgb = NULL;
	RGBQUAD * m_pRGB = NULL;

	static unsigned char masktable[] = { 0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01 };


	m_blSimple = false;

    if (!hDIB)
        return NULL;

	lpVoid = (UCHAR *)GlobalLock(hDIB);


    lpbi = (LPBITMAPINFOHEADER )lpVoid;



    if (!lpbi)
	{
		GlobalUnlock(hDIB);
        return NULL;
	}

    hdc = GetDC(NULL);

	pRgb = (RGBQUAD *)(lpVoid + sizeof(BITMAPINFOHEADER) );


		//Copie dans 
	bmiInfos = *(LPBITMAPINFO)lpbi;

	int iLocalAllocation = 0;
	BYTE * m_dTemp = new BYTE[bmiInfos.bmiHeader.biSizeImage];

	if(m_dTemp == NULL)
	{
		//Probléme d'allocation mémoire passage par un heap
		iLocalAllocation = 1;
		m_dTemp = (BYTE *) HeapAlloc (GetProcessHeap (),
				HEAP_ZERO_MEMORY, bmiInfos.bmiHeader.biSizeImage) ;
	}


	AllocationMemoire(bmiInfos.bmiHeader.biWidth * bmiInfos.bmiHeader.biHeight * 4);

	//memcpy(m_dTemp,(LPSTR)lpVoid + lpbi->biSize + PaletteSize(lpVoid),bmiInfos.bmiHeader.biSizeImage);

	memcpy(m_dTemp,(LPSTR)lpVoid + lpbi->biSize + PaletteSize(lpVoid),bmiInfos.bmiHeader.biSizeImage);

	//Conversion de 24 vers 32 bits

	switch(bmiInfos.bmiHeader.biBitCount)
	{
		case 24:
			{
				BITMAP bitmap;
				bitmap.bmWidth = bmiInfos.bmiHeader.biWidth;
				bitmap.bmHeight = bmiInfos.bmiHeader.biHeight;
				bitmap.bmBitsPixel = DEPTHDEFO;
				CreateHeaderInformation(bitmap);

				long lWidthSize = (((24 * bmiInfos.bmiHeader.biWidth) + 31) / 32) * 4;
				
				int k = 0, j = 0;

				for(int y = 0;y < bmiInfos.bmiHeader.biHeight;y++)
				{
					for(int x = 0;x < bmiInfos.bmiHeader.biWidth;x++)
					{
						j = y * lWidthSize + x * 3;
						memcpy(m_bData+k,m_dTemp+j,3);
						m_bData[k+3] = 0;
						k+=4;
					}
				}


			}
			break;

		case 8:
			{

				//Récupération de la palette et application à l'image
				BITMAP bitmap;
				bitmap.bmWidth = bmiInfos.bmiHeader.biWidth;
				bitmap.bmHeight = bmiInfos.bmiHeader.biHeight;
				bitmap.bmBitsPixel = DEPTHDEFO;
				CreateHeaderInformation(bitmap);
		
				//Récupération de la palette

				m_pRGB = (RGBQUAD *)malloc(sizeof(RGBQUAD)*PaletteSize(lpVoid));

				memcpy(m_pRGB,pRgb,sizeof(RGBQUAD)*PaletteSize(lpVoid));


				long lWidthSize = (((8 * bmiInfos.bmiHeader.biWidth) + 31) / 32) * 4;
			
				//PALETTEENTRY m_Palette;
				int k = 0, j = 0;
				for(int y = 0;y < bmiInfos.bmiHeader.biHeight;y++)
				{
					for(int x = 0;x < bmiInfos.bmiHeader.biWidth;x++)
					{
						j = lWidthSize * y + x;
						int iPaletteValue = m_dTemp[j];

						m_bData[k] = m_pRGB[iPaletteValue].rgbBlue;
						m_bData[k+1] = m_pRGB[iPaletteValue].rgbGreen ;
						m_bData[k+2] = m_pRGB[iPaletteValue].rgbRed ;
						m_bData[k+3] = m_pRGB[iPaletteValue].rgbReserved;

						k+=4;
					}
				}

				free(m_pRGB);

			}
			break;

		case 16:
			{
				int bytes = (bmiInfos.bmiHeader.biBitCount*bmiInfos.bmiHeader.biWidth)>>3;
				int height = bmiInfos.bmiHeader.biHeight;
				int width = bmiInfos.bmiHeader.biWidth;

				while(bytes%4) bytes++;

				int i;
				BYTE **ptr;
				BYTE ** m_pLinePtr = (BYTE **)malloc(sizeof(BYTE *)*height);
				if(!m_pLinePtr) 
					return FALSE;

				for(i=0,ptr=m_pLinePtr; i < height; i++,ptr++)
				{
					*ptr = m_dTemp + (height-i-1)*bytes;
				}
				
				//Black & White

				int index = 0;
				BITMAP bitmap;
				bitmap.bmWidth = bmiInfos.bmiHeader.biWidth;
				bitmap.bmHeight = bmiInfos.bmiHeader.biHeight;
				bitmap.bmBitsPixel = DEPTHDEFO;
				CreateHeaderInformation(bitmap);
		
				//Récupération de la palette

				m_pRGB = (RGBQUAD *)malloc(sizeof(RGBQUAD)*PaletteSize(lpVoid));

				memcpy(m_pRGB,pRgb,sizeof(RGBQUAD)*PaletteSize(lpVoid));

			
				//PALETTEENTRY m_Palette;
				int k = 0, j = 0;
				for(int y = bmiInfos.bmiHeader.biHeight - 1;y >= 0;y--)
				{
					BYTE * src = m_pLinePtr[y];

					for(int x = 0;x < bmiInfos.bmiHeader.biWidth;x++)
					{

						if(x & 1)
						{
							index = src[x>>1] & 0x0f;
						}
						else
						{
							index = (src[x>>1] >> 4) & 0x0f;
						}

						m_bData[k] = m_pRGB[index].rgbBlue;
						m_bData[k+1] = m_pRGB[index].rgbGreen ;
						m_bData[k+2] = m_pRGB[index].rgbRed ;
						m_bData[k+3] = 0;

						k+=4;
					}
				}

				free(m_pRGB);
				free(m_pLinePtr);
			}
			break;

		case 1:
			{

				int bytes = (bmiInfos.bmiHeader.biBitCount*bmiInfos.bmiHeader.biWidth)>>3;
				int height = bmiInfos.bmiHeader.biHeight;
				int width = bmiInfos.bmiHeader.biWidth;

				while(bytes%4) bytes++;

				int i;
				BYTE **ptr;
				BYTE ** m_pLinePtr = (BYTE **)malloc(sizeof(BYTE *)*height);
				if(!m_pLinePtr) 
					return FALSE;

				for(i=0,ptr=m_pLinePtr; i < height; i++,ptr++)
				{
					*ptr = m_dTemp + (height-i-1)*bytes;
				}
				
				//Black & White

				int index = 0;
				BITMAP bitmap;
				bitmap.bmWidth = bmiInfos.bmiHeader.biWidth;
				bitmap.bmHeight = bmiInfos.bmiHeader.biHeight;
				bitmap.bmBitsPixel = DEPTHDEFO;
				CreateHeaderInformation(bitmap);
		
				//Récupération de la palette

				m_pRGB = (RGBQUAD *)malloc(sizeof(RGBQUAD)*PaletteSize(lpVoid));

				memcpy(m_pRGB,pRgb,sizeof(RGBQUAD)*PaletteSize(lpVoid));

			
				//PALETTEENTRY m_Palette;
				int k = 0, j = 0;
				for(int y = bmiInfos.bmiHeader.biHeight - 1;y >= 0;y--)
				{
					BYTE * src = m_pLinePtr[y];

					for(int x = 0;x < bmiInfos.bmiHeader.biWidth;x++)
					{

						if(src[x>>3] & masktable[x&7])
						{
							index = 1;
						}
						else
						{
							index = 0;
						}

						m_bData[k] = m_pRGB[index].rgbBlue;
						m_bData[k+1] = m_pRGB[index].rgbGreen ;
						m_bData[k+2] = m_pRGB[index].rgbRed ;
						m_bData[k+3] = 0;

						k+=4;
					}
				}

				free(m_pRGB);
				free(m_pLinePtr);
			}
			break;


		case 32:
			memcpy(m_bData,m_dTemp,bmiInfos.bmiHeader.biSizeImage);
			break;

	}



    hBitmap = CreateDIBitmap(hdc,
                &bmiInfos.bmiHeader,
                (LONG)CBM_INIT,
                m_bData,
                &bmiInfos,
                DIB_RGB_COLORS );


    ReleaseDC(NULL,hdc);
    GlobalUnlock(hDIB);

	if(iLocalAllocation == 0)
		delete[] m_dTemp;
	else
		HeapFree (GetProcessHeap(), 0, m_dTemp) ;


    return hBitmap;

}

bool CIBitmap::GetUse()
{
	bool m_bValue = false;
	#pragma omp critical(bInUse)
	{
		m_bValue = bInUse;
	}
	return m_bValue;
}

void CIBitmap::SetUse(const bool &m_bUse)
{
	#pragma omp critical(bInUse)
	{
		bInUse = m_bUse;
	}
	
}

const long CIBitmap::GetWidthSize()
{
	if(m_sDepth == 24)
		return ((((bmiInfos.bmiHeader.biWidth * 24) + 31) & ~31) >> 3);

	return bmiInfos.bmiHeader.biWidth * 4;
}


void CIBitmap::CreateHeaderInformation(const BITMAP &bitmap)
{
	int iTaille = 4;
	memset(&bmiInfos,0,sizeof(BITMAPINFO));
	bmiInfos.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiInfos.bmiHeader.biWidth = bitmap.bmWidth;
	bmiInfos.bmiHeader.biHeight = bitmap.bmHeight;
	bmiInfos.bmiHeader.biPlanes = 1;
	bmiInfos.bmiHeader.biCompression = BI_RGB;

	if(m_sDepth == 24)
	{
		bmiInfos.bmiHeader.biBitCount = 24;
		bmiInfos.bmiHeader.biSizeImage = ((((bmiInfos.bmiHeader.biWidth * 24) + 31) & ~31) >> 3) * bitmap.bmHeight;
	}
	else
	{
		bmiInfos.bmiHeader.biBitCount = 32;
		bmiInfos.bmiHeader.biSizeImage = iTaille * bitmap.bmWidth * bitmap.bmHeight; //((iTaille * bitmap.bmWidth + iTaille) & ~iTaille) * bitmap.bmHeight;
	}
	m_iWidth = bitmap.bmWidth;
	m_iHeight = bitmap.bmHeight;


}

BOOL CIBitmap::HorzFlipBuf()
{
	BYTE  *tb1;
	BYTE  *tb2;

	if (m_bData==NULL)
		return FALSE;

	int MiddleX = (m_iWidth >> 1) << 2;

	BYTE m_bDataBuffer[4];

	int iPos = 0;
	int iPos1 = 0;
	int iPos2 = 0;

	int iWidth4 = m_iWidth << 2;

	for(int y = 0;y < m_iHeight;y++)
	{
		iPos = y * iWidth4;
		iPos2 = iPos + iWidth4 - 4;

		for(int x = 0;x < MiddleX;x+=4)
		{
			memcpy(m_bDataBuffer,m_bData + iPos+x,4); 
			memcpy(m_bData + iPos+x,m_bData + iPos2-x,4);
			memcpy(m_bData + iPos2-x,m_bDataBuffer,4);
			
		}
	}


	return TRUE;
}

BOOL CIBitmap::Rotation90()
{
	if (m_bData==NULL)
		return FALSE;

	int iLocalAllocation = 0;

	int iImageSize = m_iWidth * m_iHeight * 4;

	BYTE * m_dTemp = new BYTE[iImageSize];

	if(m_dTemp == NULL)
	{
		//Probléme d'allocation mémoire passage par un heap
		iLocalAllocation = 1;
		m_dTemp = (BYTE *) HeapAlloc (GetProcessHeap (),
				HEAP_ZERO_MEMORY, iImageSize) ;
	}


	int iPos1 = 0;
	int iPos2 = 0;

	int iWidth4 = m_iWidth << 2;
	int iHeight4 = m_iHeight << 2;

	for(int y = 0;y < m_iHeight;y++)
	{
		for(int x = 0;x < m_iWidth;x++)
		{
			iPos1 = y * iWidth4 + (x << 2);
			iPos2 = x * iHeight4 + (y << 2);
			memcpy(m_dTemp + iPos2,m_bData + iPos1,4);
			
		}
	}



	int iWidthTemp = m_iWidth;
	m_iWidth = m_iHeight;
	m_iHeight = iWidthTemp;

	memcpy(m_bData,m_dTemp,iImageSize);

	bmiInfos.bmiHeader.biWidth = m_iWidth;
	bmiInfos.bmiHeader.biHeight = m_iHeight;


	if(iLocalAllocation == 0)
		delete[] m_dTemp;
	else
		HeapFree (GetProcessHeap(), 0, m_dTemp) ;

	return TRUE;
}


BOOL CIBitmap::VertFlipBuf()
{   
	BYTE  *tb1;
	BYTE  *tb2;

	if (m_bData==NULL)
		return FALSE;

	UINT bufsize;

	bufsize= GetWidthSize();

	tb1= (BYTE *)new BYTE[bufsize];
	if (tb1==NULL) {
		return FALSE;
	}

	tb2= (BYTE *)new BYTE [bufsize];
	if (tb2==NULL) {
		delete [] tb1;
		return FALSE;
	}
	
	UINT row_cnt;     
	ULONG off1=0;
	ULONG off2=0;

	for (row_cnt=0;row_cnt<(m_iHeight+1)/2;row_cnt++) {
		off1=row_cnt*bufsize;
		off2=((m_iHeight-1)-row_cnt)*bufsize;   
		
		memcpy(tb1,m_bData+off1,bufsize);
		memcpy(tb2,m_bData+off2,bufsize);	
		memcpy(m_bData+off1,tb2,bufsize);
		memcpy(m_bData+off2,tb1,bufsize);
	}


	delete [] tb1;
	delete [] tb2;

	return TRUE;
}        

void CIBitmap::CreateBitmap(const int &iWidth, const int &iHeight, const int &iDepth)
{
	BITMAP bitmap;
	bitmap.bmWidth = iWidth;
	bitmap.bmHeight = iHeight;
	bitmap.bmBitsPixel = iDepth;

	CreateHeaderInformation(bitmap);

	AllocationMemoire(bmiInfos.bmiHeader.biSizeImage);
}
