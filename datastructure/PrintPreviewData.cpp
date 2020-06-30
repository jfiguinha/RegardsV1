#include "stdafx.h"
#include ".\printpreviewdata.h"
#include "margedata.h"
#include <ibitmap.h>

CPrintPreviewData::CPrintPreviewData(void)
{
	m_ciBitmap = NULL;
	m_ciBitmapTemp = NULL;
	iWidthPageSize = 0;
	iHeightPageSize = 0;
	iWidthPixelSize = 0;
	iHeightPixelSize = 0;
	iPosWidth = 0;
	iPosHeight = 0;
	iPosLeft = 0;
	iPosTop = 0;
	iPosLeftPicture = 0;
	iPosTopPicture = 0;
	iPosBottomPicture = 0;
	iPosRightPicture = 0;
	m_MargeData = NULL;
	m_ciBitmapOriginalResize = NULL;
	m_iTypePos = 0;
	iOrientation = 0;
	m_dRatio = 0.0;
}

CPrintPreviewData::~CPrintPreviewData(void)
{
	if(m_ciBitmap != NULL)
	{
		delete m_ciBitmap;
		m_ciBitmap = NULL;
	}

	if(m_ciBitmapTemp != NULL)
	{
		delete m_ciBitmapTemp;
		m_ciBitmapTemp = NULL;
	}

	if(m_ciBitmapOriginalResize != NULL)
	{
		delete m_ciBitmapOriginalResize;
		m_ciBitmapOriginalResize = NULL;
	}
}
