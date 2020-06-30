
#ifndef __LENSFLAREDATA__H__
#define __LENSFLAREDATA__H__

#include <color.h>

class CIBitmap;

class CLensFlareData
{
public:
	CLensFlareData(void);
	~CLensFlareData(void);

	CIBitmap * m_ciBitmapTempBitmap;
	CIBitmap * m_ciBitmapTestBitmap;
	CIBitmap * m_ciBitmapColorBox;
	

	float fRapport;
	int iRayon;
	int iTailleRayon;
	int x;
	int y;
	HSB m_HsbColor;
	UINT iWidth ;
	UINT iHeight;
	UINT iXMin;
	UINT iYMin;
};

#endif