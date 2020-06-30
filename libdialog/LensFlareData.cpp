#include "StdAfx.h"
#include ".\lensflaredata.h"
#include <ibitmap.h>


CLensFlareData::CLensFlareData(void)
{
	m_ciBitmapTempBitmap = new CIBitmap();
	m_ciBitmapTestBitmap = new CIBitmap();
	m_ciBitmapColorBox = new CIBitmap();
	fRapport = 0.0;
	iRayon = 0;
	iTailleRayon = 0;
	x = 0;
	y = 0;
	iWidth = 0;
	iHeight = 0;
	iXMin = 0;
	iYMin = 0;
}

CLensFlareData::~CLensFlareData(void)
{
	if(m_ciBitmapTempBitmap != NULL)
		delete m_ciBitmapTempBitmap;

	m_ciBitmapTempBitmap = NULL;

	if(m_ciBitmapTestBitmap != NULL)
		delete m_ciBitmapTestBitmap;

	m_ciBitmapTestBitmap = NULL;

	if(m_ciBitmapColorBox != NULL)
		delete m_ciBitmapColorBox;

	m_ciBitmapColorBox = NULL;

}
