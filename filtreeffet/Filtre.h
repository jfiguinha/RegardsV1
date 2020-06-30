// Filtre.h: interface for the CFiltre class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILTRE_H__4BECB89F_F082_48E3_884F_0EC1FE35A5DB__INCLUDED_)
#define AFX_FILTRE_H__4BECB89F_F082_48E3_884F_0EC1FE35A5DB__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

//#include "IBitmap.h"
//#include "DialogFiltreEffet.h"
//#include "FiltreMatrix.h"

class CMatrix;
class CFiltreEffet;
class CIBitmap;
class CFiltrePrv;
class CPerlinNoise;

class CFiltre  
{
public:

	//void AutoRotate(HWND hWnd, CIBitmap * m_ciBitmap);
	bool SimpleFilter(HWND hwnd, CIBitmap * m_ciBitmap, short * kernel,const short &Ksize,const short &Kfactor,const short &Koffset);

	void ReplaceColor(CIBitmap * m_ciBitmap,const COLORREF &m_color1, const COLORREF &m_color2, const int &iLarge = 0);
	void GrayScale(HWND hwnd, CIBitmap * m_ciBitmap);
	void Erode(HWND hwnd, CIBitmap * m_ciBitmap,const long &Ksize = 2);
	void Dilate(HWND hwnd, CIBitmap * m_ciBitmap,const long &Ksize = 2);
	void Negatif(HWND hwnd, CIBitmap * m_ciBitmap);
	
	void LensFlare(HWND hwnd, CIBitmap * m_ciBitmap, const int &iPosX, const int &iPosY, const int &iPuissance = 0, const int &iType = 0, const int &iIntensity = 25, const int &iColor = 0, const int &iColorIntensity = 30);
	void LensFlare(HWND hwnd, CIBitmap * m_ciBitmap, const int &iWidth, const int &iHeight, const int &iPosX, const int &iPosY, const int &iPuissance, const int &iType, const int &iIntensity, const int &iColor, const int &iColorIntensity);

	void Cercle(HWND hwnd, CIBitmap * m_ciBitmap);

	void Median(HWND hwnd, CIBitmap * m_ciBitmap);
	void Noise(HWND hwnd, CIBitmap * m_ciBitmap,const long &level);
	void Lightness(HWND hwnd, CIBitmap * m_ciBitmap,int m_lightness);
	void Contrast(HWND hwnd, CIBitmap * m_ciBitmap,double contrast, BYTE offset);
	void ModifyRGB(HWND hwnd, CIBitmap * m_ciBitmap,const long &m_lValue, const int &m_iCouleur);
	void Mosaic(HWND hwnd, CIBitmap * m_ciBitmap,const long &m_lValue);
	bool Filter(HWND hwnd, CIBitmap * m_ciBitmap, short * kernel,const short &Ksize,const short &Kfactor,const short &Koffset);
	bool Filter2Pass(HWND hwnd, CIBitmap * m_ciBitmap, short * kernel,const short &Ksize,const short &Kfactor,const short &Koffset);
	void BlackAndWhite(HWND hwnd, CIBitmap * m_ciBitmap);

	bool MotionBlur(HWND hwnd, CIBitmap * m_ciBitmap, const double &radius,const double &sigma,const double &angle);

	void Clouds(HWND hwnd, CIBitmap * m_ciBitmap,const COLORREF &m_color1, const COLORREF &m_color2,const float &freq, const float &pers,const int &octaves);

	void AppliquerFiltre(CFiltreEffet * m_lpFiltreEffet);

	void PhotoFiltre(HWND hwnd, CIBitmap * m_ciBitmap, COLORREF colorValue, int iDensity);
	void FiltreWiener(HWND hwnd, CIBitmap * m_ciBitmap,const CMatrix &FiltreDegradation,const float &rho = 0.002f, const int &iMethod = 1);

	CFiltre();

	CFiltre(CFiltreEffet * m_lpFiltreEffet)
	{
		this->m_lpFiltreEffet = m_lpFiltreEffet;
		m_bArreter = false;
	};

	virtual ~CFiltre();

	void ApplyColor(HWND hwnd, CIBitmap * m_ciBitmap, COLORREF colorref);

	int GetMotionBlurKernel(int width,const double sigma,double **kernel);

	bool m_bArreter;

private:
	//Intensite de conversion
	//alpha = 0.2989 beta = 0.5870 gamma = 0.1140 pour un total de 1
	static double alpha, beta, gamma;
	CFiltreEffet * m_lpFiltreEffet;
	WORD OldCtrlWord;
	

	inline short quick_select(short * arr, int n);
	

};

#endif // !defined(AFX_FILTRE_H__4BECB89F_F082_48E3_884F_0EC1FE35A5DB__INCLUDED_)
