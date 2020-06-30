// IBitmap.h: interface for the CIBitmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IBITMAP_H__181D57C1_9F23_4C54_8A1A_D7754E6C4C5F__INCLUDED_)
#define AFX_IBITMAP_H__181D57C1_9F23_4C54_8A1A_D7754E6C4C5F__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include <vector>
using namespace std;

#define DEPTHDEFO 32

class CIBitmap  
{
public:
	void CreateBitmap(const int &iWidth, const int &iHeight, const int &iDepth = 32);

	void SetUse(const bool &m_bUse);
	bool GetUse();
	HBITMAP HandleToBitmap(HANDLE hMem,int bits, HPALETTE hpal);

	void GetBitmap(HBITMAP &hBitmap);
	long GetBitmapSize();
	bool GetAffectationSimple();
	void SetAffectationSimple(const bool &m_blSimple);
	void GetFilename(char * szFilename);
	void SetFilename(const char * szFilename);

	void SpeedPictureShow(HWND hWnd);

	void GetColorValue(const int &x, const int &y, COLORREF &m_cValue);
	void SetColorValue(const int &x, const int &y,const COLORREF &m_cValue);
	void GetRGBColorValue(const int &x, const int &y, COLORREF &m_cValue);
	
	void SetBitmapData(BYTE * m_bData, const unsigned int &bmWidth,const unsigned int &bmHeight);

	void ReCreateHBitmap(const bool &m_bFlip = true);

	int SetBitmap(const HBITMAP &hBitmapTemp, const bool m_bFlip = true);
	int SetColorTranspBitmap(COLORREF Transp);

	void SetBitmap(BYTE * m_bBuffer, const unsigned int &bmWidth,const unsigned int &bmHeight, const bool &m_bFlip = false, const int &m_iBitCount = DEPTHDEFO);
	void SetBitmap(COLORREF * m_bBuffer, const unsigned int &bmWidth,const unsigned int &bmHeight, const bool &m_bFlip = false, const int &m_iBitCount = DEPTHDEFO);

	//void SetBitmapBase64(const char * m_bBuffer, const unsigned int &bmWidth,const unsigned int &bmHeight, const bool &m_bFlip = false, const int &m_iBitCount = 24);
	int SetBitmap(const HBITMAP &hBitmapTemp, const RECT &rc, const bool &m_bFlip = false);
		
	int InsertBitmap(CIBitmap * ciBitmap, int xPos, int yPos);
	int DrawColor(const COLORREF &color);
	int DessinerCarre(int iMarge, const COLORREF &color);
	int DessinerCarre(const RECT *lprc, const COLORREF &color);


	void GetPtBitmap(BYTE ** m_bData, unsigned int &bmWidth, unsigned int &bmHeight, unsigned long &m_lSize);
	void GetBitmap(BYTE ** m_bData, unsigned int &bmWidth, unsigned int &bmHeight, unsigned long &m_lSize);
	void GetBandWBitmap(BYTE * m_bData, unsigned int &bmWidth, unsigned int &bmHeight, unsigned long &m_lSize);

	BYTE * GetPtBitmap();

	COLORREF * GetColorRefPt();


	void ShowBitmap(HDC &hdc, const int &left,const int &top, int &xValue, int &yValue, const int &m_bHauteQualite, const bool &m_bAutoShrink, const int &nTailleAffichageWidth, const int &nTailleAffichageHeight, const float &fratio);

	const long GetWidthSize();
	const unsigned int GetBitmapWidth();
	const unsigned int GetBitmapHeigth();
	const unsigned short GetBitmapDepth();

	void SetBitmapWidth(const unsigned int &bmWidth);
	void SetBitmapHeigth(const unsigned int &bmHeigth);

	BOOL VertFlipBuf();
	BOOL HorzFlipBuf();
	BOOL Rotation90();

	CIBitmap();
	virtual ~CIBitmap();

	/////////////////////////////////////////////////////////////////////////
	//Constructeur de copie
	/////////////////////////////////////////////////////////////////////////

	//CIBitmap(CIBitmap &m_ciBitmap);
	CIBitmap(const CIBitmap &m_ciBitmap);

	HBITMAP hBitmap;
	HBITMAP hBitmapHisto;
	HWND hWnd;
	HWND hWndPrgBar;

	void SetOnSize(bool bSize)
	{
		m_bSize = bSize;
	}

	//char * m_bDataB64;
	BITMAPINFO bmiInfos;

private:
	
	int iTypeMemoryAllocation;
	char FileName[MAX_PATH];
	unsigned int m_iWidth;
	unsigned int m_iHeight;
	unsigned long m_lSize;
	unsigned short m_sDepth;
	bool m_blSimple;
	bool bInUse;
	bool m_bShow;
	bool m_bSize;
	BYTE * m_bData;

	void EraseData();


	CRITICAL_SECTION CriticalSection;

	vector<RGBQUAD> vPalette;


	void DesAllocationMemoire();
	void AllocationMemoire(const long &m_lSize);
	int GetPaletteSize(BITMAPINFOHEADER * bmInfo);
	WORD PaletteSize (VOID FAR * pv);
	WORD DibNumColors (VOID FAR * pv);
	void CreateHeaderInformation(const BITMAP &bitmap);
};

typedef std::vector<CIBitmap> CIBitmapVector;

#endif // !defined(AFX_IBITMAP_H__181D57C1_9F23_4C54_8A1A_D7754E6C4C5F__INCLUDED_)
