// Raw.h: interface for the CRaw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAW_H__2E8C23EF_E5BD_4DD1_95DF_D261F7FB29A5__INCLUDED_)
#define AFX_RAW_H__2E8C23EF_E5BD_4DD1_95DF_D261F7FB29A5__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

class CIBitmap;

typedef struct RAWDATAIMPORT
{
	CIBitmap * m_ciBitmap;
	const char * szFileName;
	int quick_interpolate;
	HWND hParent;
	HMODULE ModId;
}RAWDATAIMPORTDATA, * LPRAWDATAIMPORT;

class CCameraRaw  
{
public:

	CCameraRaw();
	virtual ~CCameraRaw();
	
	

	void GetDimensions(const char * fileName,UINT *width,UINT *height);
	int ReadRaw(CIBitmap * m_ciBitmap, const char *szFileName, bool bFullScreen);
	int ReadRawThumbnail(CIBitmap * m_ciBitmap, const char *szFileName,const int &ThumbWidth, const int &ThumbHeight);

private:
	static ULONG __stdcall ReadRawPrivate(LPVOID parametre);
	static LRESULT OpenFichier(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	HMODULE ModId;
	
};

#endif // !defined(AFX_RAW_H__2E8C23EF_E5BD_4DD1_95DF_D261F7FB29A5__INCLUDED_)
