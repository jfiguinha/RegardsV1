// LoadImage.cpp: implementation of the CLoadImage class.
//
//////////////////////////////////////////////////////////////////////

#include "LoadImage.h"
#include "effet.h"
#include <process.h>

CLoadPicture CLoadImage::pimpl_;
char CLoadImage::cFileName[255];
bool CLoadImage::m_bTerminer = false;
bool CLoadImage::m_bHandleTerminer = true;

int TestImageFormat(const char *szFileName)
{
	UINT m_iValue = 0;
	TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[255];

	_splitpath(szFileName, drive, dir, fname, ext);

	if(strcmp(strlwr(ext),".bmp") == 0)
		return 2;

	if(strcmp(strlwr(ext),".pcx") == 0)
		return 7;

	if(strcmp(strlwr(ext),".jpg") == 0)
		return 1;

	if(strcmp(strlwr(ext),".jpe") == 0)
		return 1;

	if(strcmp(strlwr(ext),".jpeg") == 0)
		return 1;

	if(strcmp(strlwr(ext),".jfif") == 0)
		return 1;

	if(strcmp(strlwr(ext),".jif") == 0)
		return 1;

	if(strcmp(strlwr(ext),".tif") == 0)
		return 3;

	if(strcmp(strlwr(ext),".gif") == 0)
		return 4;

	if(strcmp(strlwr(ext),".png") == 0)
		return 5;

	if(strcmp(strlwr(ext),".tga") == 0)
		return 6;

	if(strcmp(strlwr(ext),".pcd") == 0)
		return 8;

	if(strcmp(strlwr(ext),".jp2") == 0)
		return 9;

	if(strcmp(strlwr(ext),".jpc") == 0)
		return 9;

	if(strcmp(strlwr(ext),".j2k") == 0)
		return 9;

	if(strcmp(strlwr(ext),".psd") == 0)
		return 10;

	if(strcmp(strlwr(ext),".pdd") == 0)
		return 10;

	return m_iValue;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoadImage::CLoadImage()
{
}

CLoadImage::~CLoadImage()
{
	CloseHandle(hThread);
}

int CLoadImage::ChargementImage(CIBitmap * m_ciBitmap, const char * szFileName, const HWND &hWnd)
{
	//DWORD dExitCode;


	if(!m_bHandleTerminer)
		return 0;

	CloseHandle(hThread);

	m_ciBitmap->hWnd = hWnd;
	m_ciBitmap->SetFilename(szFileName);


	hThread = (HANDLE)_beginthreadex(NULL, 1024,
					ChargerImage, m_ciBitmap, 0, (unsigned int*) &dwThreadId);	

	return 1;
}

/////////////////////////////////////////////////////////////////
//BMP Sauvegarde
/////////////////////////////////////////////////////////////////

int CLoadImage::BMPSAVE(const HBITMAP &bmp,LPSTR filename)
{
	return pimpl_.BMPSAVE(bmp,filename);
}



/////////////////////////////////////////////////////////////
//Example of use:
/////////////////////////////////////////////////////////////

int CLoadImage::JPEGSAVE(const HBITMAP &bmp, LPSTR filename, const long lQuality)
{

	char csMsg[255];
    HANDLE  hDib = pimpl_.DDBToDIB(bmp, BI_RGB);  //Use default palette

	csMsg[0] = '\0';

    //Turn DIB into JPEG file
    if (hDib != NULL)
    {
        if (!pimpl_.JpegFromDib(hDib,
                         lQuality,   //Quality setting
                         filename,
                         csMsg))
        {
            //MessageBox(NULL,csMsg,"Infos",0);
			return -1;
        }

        GlobalFree(hDib);
    }

    else
	{
        //MessageBox(NULL,"Failed to load IDB_TEST","Infos",0);
		return -1;
	}


	return 0;
}

/////////////////////////////////////////////////////////////
//Example of use:
/////////////////////////////////////////////////////////////

int CLoadImage::JPEGSAVE(const HANDLE &hDib, LPSTR filename, const long lQuality)
{
	char csMsg[255];
    if (hDib != NULL)
    {
        if (!pimpl_.JpegFromDib(hDib,
                         lQuality,   //Quality setting
                         filename,
                         csMsg))
        {
			return -1;
        }
    }
	return 0;
}

/////////////////////////////////////////////////////////////
//Example of use:
/////////////////////////////////////////////////////////////


UINT __stdcall CLoadImage::ChargerImage(LPVOID parametre)
{
	m_bHandleTerminer = false;

	CLoadPicture m_CChargementImage;
	HBITMAP hBitmap;
	try
	{

		EnterCriticalSection(&m_CChargementImage.CriticalSection); 
			
		CIBitmap * m_ciBitmap = (CIBitmap *)parametre;
		char cFileName[255];
		m_ciBitmap->GetFilename(cFileName);
		m_ciBitmap->EraseData();
		int m_iValue = TestImageFormat(cFileName);

		if(m_iValue > 0)
		{
			m_CChargementImage.ChargementImage(m_iValue,cFileName, hBitmap);
			m_ciBitmap->SetBitmap(hBitmap, false);
			DeleteObject(hBitmap);
		}

		LeaveCriticalSection(&m_CChargementImage.CriticalSection);

		//SendMessage(m_ciBitmap->hWnd, WM_ERASEBKGND, 0,MAKELPARAM(IDM_CHGIMAGE_OK,0));

	}
	catch(...)
	{
		if(hBitmap != NULL)
			DeleteObject(hBitmap);
		LeaveCriticalSection(&m_CChargementImage.CriticalSection);
	}


	m_bHandleTerminer = true;

	return 0;
}

int CLoadImage::ChargementImageStatic(CIBitmap &m_ciBitmap, const char *szFileName)
{
	//CIBitmap m_ciBitmapTemp;
	HBITMAP hBitmap;

	//m_ciBitmapTemp.SetAffectationSimple(true);
	int m_iValue = TestImageFormat(szFileName);

	try
	{

		if(m_iValue > 0)
		{
			pimpl_.ChargementImage(m_iValue,szFileName, hBitmap);
			m_ciBitmap.SetBitmap(hBitmap, false);
			DeleteObject(hBitmap);

		}

	}
	catch(...)
	{
	}
	return m_iValue;
}

int CLoadImage::GetDimensionImageStatic(const char *szFileName, UINT * width, UINT * height)
{
	//m_ciBitmapTemp.SetAffectationSimple(true);
	int m_iValue = TestImageFormat(szFileName);

	try
	{
		if(m_iValue > 0)
			pimpl_.GetDimensions(szFileName, width, height, m_iValue);
	}
	catch(...)
	{
	}
	return m_iValue;
}


int CLoadImage::ChargementIconeStatic(HBITMAP &hBitmapTemp, const char *szFileName, const int &iIconeWidth, const int &iIconeHeight, const unsigned int &Width, const unsigned int &Height, const int &iMarge, const COLORREF &m_color1, const COLORREF &m_color2)
{
	HBITMAP hBitmap;
	CEffet m_cEffet;

	//m_ciBitmapTemp.SetAffectationSimple(true);
	int m_iValue = TestImageFormat(szFileName);

	//MessageBox(NULL,szFileName,"Infos",0);

	try
	{
		switch(m_iValue)
		{
			case 1:
				{

					pimpl_.GetImageFromJPG(szFileName, hBitmap,0, 0);
					m_cEffet.GenerateIcone(hBitmap, hBitmapTemp, iIconeWidth, iIconeHeight, Width, Height, iMarge, m_color1, m_color2);
					DeleteObject(hBitmap);
					return 0;

				}
				break;

			default:
				if(m_iValue > 0)
					pimpl_.ChargementImage(m_iValue,szFileName, hBitmap);

				break;
		
		}

		m_cEffet.GenerateIcone(hBitmap, hBitmapTemp, iIconeWidth, iIconeHeight, Width, Height, iMarge, m_color1, m_color2);

		DeleteObject(hBitmap);

	}
	catch(...)
	{
	}
	return m_iValue;
}

////////////////////////////////////////////////////////////////////////////////

void CLoadImage::CloseThread()
{
	CloseHandle(hThread);
}