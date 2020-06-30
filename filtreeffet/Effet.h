// Effet.h: interface for the CEffet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFET_H__43583D21_027C_48B1_B760_B7A9CB75CC8A__INCLUDED_)
#define AFX_EFFET_H__43583D21_027C_48B1_B760_B7A9CB75CC8A__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

class CIBitmap;
class CDialogFiltreEffet;
class CFiltreEffet;

class CEffet  
{
public:

	HBITMAP GenerateIcone(CIBitmap * m_ciBitmap, const int &iIconeWidth, const int &iIconeHeight, unsigned int &Width, unsigned int &Height, const int &iMarge, const COLORREF &m_color = RGB( 230, 230, 230), const COLORREF &rgb = RGB(0, 0, 0));
	HBITMAP GenerateIcone(HBITMAP hBitmap, const int &iIconeWidth, const int &iIconeHeight, unsigned int &Width, unsigned int &Height, const int &iMarge, const COLORREF &m_color, const COLORREF &rgb);

	HBITMAP GenerateFolderIcone(HBITMAP hBitmapFolder, const char * szFolderName, const int &iIconeWidth, const int &iIconeHeight, const int &iMarge, const COLORREF &m_color = RGB( 230, 230, 230), const COLORREF &rgb = RGB(0, 0, 0));


	CEffet();
	
	CEffet(CFiltreEffet * m_lpFiltreEffet)
	{
		this->m_lpFiltreEffet = m_lpFiltreEffet;
	};

	virtual ~CEffet();
	HBITMAP GetRotatedBitmap(CIBitmap * m_ciBitmap, const float &angle, const COLORREF &clrBack );
	HBITMAP GetRotatedBitmap98(CIBitmap * m_ciBitmap, const float &angle, const COLORREF &clrBack);
	HBITMAP GetFlipBitmap(CIBitmap * m_ciBitmap, const BOOL &bLateral);
	HBITMAP ScaleBitmap(HBITMAP hBitmap, const WORD & width, const WORD & height);
	void CropBitmap(CIBitmap * m_ciBitmap, const RECT &rc);
	void ScaleBitmap(CIBitmap * m_ciBitmap, const WORD & width, const WORD & height,const int &smooth = 1);
	void GetFreeRotatedBitmap(CIBitmap * m_ciBitmap, const float &angle, const COLORREF &clrBack, const int &type = 0);

private:
	void GetScaleBitmap(CIBitmap * m_ciBitmap, const int &iIconeWidth, const int &iIconeHeight, const char * szFilename);
	CFiltreEffet * m_lpFiltreEffet;
};

#endif // !defined(AFX_EFFET_H__43583D21_027C_48B1_B760_B7A9CB75CC8A__INCLUDED_)
