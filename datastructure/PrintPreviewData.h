

#ifndef __PRINTPREVIEWDATA__H__
#define __PRINTPREVIEWDATA__H__

class CIBitmap;
class CMargeData;

class CPrintPreviewData
{
public:
	CPrintPreviewData(void);
	~CPrintPreviewData(void);

	CIBitmap * m_ciBitmap;
	CIBitmap * m_ciBitmapOriginalResize;
	CIBitmap * m_ciBitmapTemp;
	int iWidthPageSize, iHeightPageSize, iWidthPixelSize, iHeightPixelSize;
	int iPosWidth, iPosHeight, iPosLeft, iPosTop, iPosLeftPicture, iPosTopPicture, iPosBottomPicture, iPosRightPicture;
	CMargeData * m_MargeData;
	int m_iTypePos, iOrientation;
	double m_dRatio;
};

#endif