// ChargementImage.h: interface for the CChargementImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHARGEMENTIMAGE_H__F024A707_5732_4BFB_825B_ECE794EEECC3__INCLUDED_)
#define AFX_CHARGEMENTIMAGE_H__F024A707_5732_4BFB_825B_ECE794EEECC3__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

class CIBitmap;
class JpegFile;
class CCameraRaw;
class CBmp;
class CTif;
class CGif;
class CPng;
class CTga;
class CPcx;
class CCPCD;
class CJ2K;
class CPsd;
class CPpm;
class CPdf;


class CLoadPicture
{
public:

	
	static const int BMP;
	static const int JPEG;
	static const int PDF;
	static const int TIFF;
	static const int TGA;

	static void GetDimensions(const char * fileName,UINT *width,UINT *height, const int &iFormat);
	static void ChargementImage(const int &m_iValue,const char *szFileName, CIBitmap * m_ciBitmap, bool bFullScreen = false);
	static BOOL GetImageFromJPG(const char * csFileName, HBITMAP &hBitmap,const long &JPGThumbWidth, const long &JPGThumbHeight);
	static BOOL GetImageFromRAW(const char * csFileName, CIBitmap * m_ciBitmap,const long &JPGThumbWidth, const long &JPGThumbHeight);
	static void SaveImage(const int &m_iValue, CIBitmap &m_ciBitmap,const char * szFileName,const int &compress = 0,const int &iJpegQuality = 0);

private:

	static JpegFile m_cJpeg;
	static CCameraRaw m_cRaw;
	static CBmp m_cBmp;
	static CTif m_cTif;
	static CGif m_cGif;
	static CPng m_cPng;
	static CTga m_cTga;
	static CPcx m_cPcx;
	static CCPCD m_cPcd;
	static CJ2K m_cJ2k;
	static CPsd m_cPsd;
	static CPpm m_cPpm;
	static CPdf m_cPdf;

};

#endif // !defined(AFX_CHARGEMENTIMAGE_H__F024A707_5732_4BFB_825B_ECE794EEECC3__INCLUDED_)
