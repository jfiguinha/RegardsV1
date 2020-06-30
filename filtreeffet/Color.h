// Color.h: interface for the CColor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLOR_H__F8B9219D_43F1_43AA_98CA_98F7B545622E__INCLUDED_)
#define AFX_COLOR_H__F8B9219D_43F1_43AA_98CA_98F7B545622E__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

class CIBitmap;

struct HSB
{
	long Hue;
	long Saturation;
	long Brightness;
};

class CColor  
{
public:
	CColor();
	virtual ~CColor();

	static int HSBToRGB(HSB &HSBValue, COLORREF &rgb);
	static int RGBToHSB(HSB &HSBValue, COLORREF &rgb);
	static int CreateHSBMatrixColorGradient(CIBitmap * m_ciBitmap, const long &m_lHue);
	static int CreateHSBColorGradient(CIBitmap * m_ciBitmap);
	static int CreateColorBitmap(CIBitmap * m_ciBitmap, COLORREF m_rgbValue);
	static int CreateColorGradient(CIBitmap * m_ciBitmap, const COLORREF colorFrom, const COLORREF colorTo);
};

#endif // !defined(AFX_COLOR_H__F8B9219D_43F1_43AA_98CA_98F7B545622E__INCLUDED_)
