// ImageScale.h: interface for the CImageScale class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGESCALE_H__8A8453CC_A4A3_481C_A476_7B363A3683FD__INCLUDED_)
#define AFX_IMAGESCALE_H__8A8453CC_A4A3_481C_A476_7B363A3683FD__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

/*
#include "IBitmap.h"
#include "Fast2PassScale.h"  
#include "RotateByShear.h"
*/


class CIBitmap;



class CImageScale  
{
public:

	CImageScale();
	virtual ~CImageScale();

	void ApplyImageScale(CIBitmap * m_ciBitmap, const WORD & width, const WORD & height,const int &iType);
	void ApplyRotation(CIBitmap * m_ciBitmap, WORD & width, WORD & height,const float &angle, const COLORREF &clrBack = RGB(0,0,0),const int &iType = 0);


private:

	void AllocationMemoire(CIBitmap * m_ciBitmap, const WORD & width, const WORD & height);
	void DesallocationMemoire(CIBitmap * m_ciBitmap,const WORD & width, const WORD & height);

	COLORREF * m_OriginalBitmapBits;
	COLORREF * m_ScaledBitmapBits;
	int iLocalWidth;
	int iLocalHeight;
	int iFacteur;
	int iLocationMemoryType;
};

#endif // !defined(AFX_IMAGESCALE_H__8A8453CC_A4A3_481C_A476_7B363A3683FD__INCLUDED_)
