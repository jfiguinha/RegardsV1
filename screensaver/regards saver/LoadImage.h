// LoadImage.h: interface for the CLoadImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOADIMAGE_H__82AFF87E_08DA_4E68_A5D4_B35A6D880B8F__INCLUDED_)
#define AFX_LOADIMAGE_H__82AFF87E_08DA_4E68_A5D4_B35A6D880B8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "LoadPicture.h"
#include "ibitmap.h"

class CLoadImage  
{
public:
	CLoadImage();
	virtual ~CLoadImage();

	void CloseThread();
	int ChargementImageStatic(CIBitmap &m_ciBitmap, const char * szFileName);
	int ChargementIconeStatic(HBITMAP &hBitmapTemp, const char *szFileName, const int &iIconeWidth, const int &iIconeHeight, const unsigned int &Width, const unsigned int &Height, const int &iMarge, const COLORREF &m_color1 = RGB( 230, 230, 230), const COLORREF &m_color2 = RGB(0,0,0));

	int BMPSAVE(const HBITMAP &bmp,LPSTR filename);
	int JPEGSAVE(const HBITMAP &bmp, LPSTR filename, const long lQuality);
	int JPEGSAVE(const HANDLE &hDib, LPSTR filename, const long lQuality);
	
	static int GetDimensionImageStatic(const char *szFileName, UINT * width, UINT * height);
	int ChargementImage(CIBitmap * m_ciBitmap, const char * szFileName, const HWND &hWnd = NULL);

private:

	static CLoadPicture pimpl_;
	static bool m_bHandleTerminer;
	static bool m_bTerminer;
	static char cFileName[255];
		
	static UINT __stdcall ChargerImage(LPVOID parametre);

	DWORD dwThreadId; 
	HANDLE hThread;

};

#endif // !defined(AFX_LOADIMAGE_H__82AFF87E_08DA_4E68_A5D4_B35A6D880B8F__INCLUDED_)
