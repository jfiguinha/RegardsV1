// LoadImage.h: interface for the CLoadImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOADIMAGE_H__82AFF87E_08DA_4E68_A5D4_B35A6D880B8F__INCLUDED_)
#define AFX_LOADIMAGE_H__82AFF87E_08DA_4E68_A5D4_B35A6D880B8F__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000


//#include "LoadPicture.h"
//#include <ibitmap.h>

class CIBitmap;
class CLoadPicture;
class CVideoAPI;

class CLoadImage  
{
public:

	typedef struct STLOADIMAGETHREAD
	{
		bool m_bHandleTerminer;
		CIBitmap * m_ciBitmap;
		bool m_bFullScreen;
	}LOADIMAGETHREAD, * LPLOADIMAGETHREAD;

	CLoadImage();
	virtual ~CLoadImage();

	void CloseThread();
	int ChargementImageStatic(CIBitmap * m_ciBitmap, const char * szFileName, bool bFullScreen = FALSE);
	HBITMAP ChargementIconeStatic(const char *szFileName, const int &iIconeWidth, const int &iIconeHeight, const unsigned int &Width, const unsigned int &Height, const int &iMarge, const COLORREF &m_color1 = RGB( 230, 230, 230), const COLORREF &m_color2 = RGB(0,0,0), bool bFolder = false, bool bJpegQuality = false);

	/*
	int BMPSAVE(CIBitmap &m_ciBitmap,LPSTR filename);
	int JPEGSAVE(CIBitmap &m_ciBitmap, LPSTR filename, const long lQuality);
	int PDFSAVE(CIBitmap &m_ciBitmap,LPSTR filename,const int &m_bCompress, const int &iJpegQuality);
	int TIFSAVE(CIBitmap &m_ciBitmap,LPSTR filename,const bool &m_bCompress, const int &iJpegQuality);
	int TGASAVE(CIBitmap &m_ciBitmap,LPSTR filename);
	*/
	HBITMAP DoExtractVideoFrame(const char *szFileName, long &Width, long &Height);
	int GetDimensionImageStatic(const char *szFileName, UINT * width, UINT * height);
	int ChargementImage(CIBitmap * m_ciBitmap, const char * szFileName, const HWND &hWnd = NULL, bool bFullScreen = FALSE);

private:

	static ULONG __stdcall ChargerImage(LPVOID parametre);

	HBITMAP PictureToBitmap(LPBYTE pmem, DWORD nSize);
	HBITMAP FileToBitmap(LPCTSTR lpFileName);
	CVideoAPI * m_cLoadVideo;
	CLoadPicture * pimpl_;
	LOADIMAGETHREAD m_ImageThread;
	DWORD dwThreadId; 
	HANDLE hThread;
	static CRITICAL_SECTION CriticalSection; 
};

#endif // !defined(AFX_LOADIMAGE_H__82AFF87E_08DA_4E68_A5D4_B35A6D880B8F__INCLUDED_)
