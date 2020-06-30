// ImageScale.cpp: implementation of the CImageScale class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ImageScale.h"
#include "IBitmap.h"
#include "Fast2PassScale.h"  
#include "RotateByShearRGB.h"
#include "filters.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImageScale::CImageScale()
{
	iFacteur = 32 >> 3;
	m_OriginalBitmapBits = NULL;
	m_ScaledBitmapBits = NULL;
}

CImageScale::~CImageScale()
{
	try
	{
		if(m_ScaledBitmapBits != NULL)
			delete[] m_ScaledBitmapBits;

		if(m_OriginalBitmapBits != NULL)
 			delete[] m_OriginalBitmapBits;
	}
	catch(...)
	{
	}

	m_ScaledBitmapBits = NULL;
	m_OriginalBitmapBits = NULL;

}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////


void CImageScale::DesallocationMemoire(CIBitmap * m_ciBitmap,const WORD & width, const WORD & height)
{
	try
	{
		m_ciBitmap->SetBitmap(m_ScaledBitmapBits,width,height);

		if(iLocationMemoryType == 1)
		{
 			delete[] m_ScaledBitmapBits;
		}
		else
		{
			HeapFree (GetProcessHeap(), 0, m_ScaledBitmapBits);
		}


		m_ScaledBitmapBits = NULL;
		m_OriginalBitmapBits = NULL;
	}
	catch(...)
	{
	}
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

void CImageScale::AllocationMemoire(CIBitmap * m_ciBitmap, const WORD & width, const WORD & height)
{
 
	iLocalWidth = m_ciBitmap->GetBitmapWidth();
	iLocalHeight = m_ciBitmap->GetBitmapHeigth();

	iLocationMemoryType = 1;

 	m_OriginalBitmapBits = (COLORREF *)m_ciBitmap->GetPtBitmap();

	m_ScaledBitmapBits = new COLORREF[width * height];

	if(m_ScaledBitmapBits == NULL)
	{
		if(m_ScaledBitmapBits != NULL)
			delete[] m_ScaledBitmapBits;

		m_ScaledBitmapBits = (COLORREF *) HeapAlloc (GetProcessHeap (),
					HEAP_ZERO_MEMORY, width * height * sizeof(COLORREF)) ;

		iLocationMemoryType = 2;

	}
  
}


void CImageScale::ApplyRotation(CIBitmap * m_ciBitmap, WORD & width, WORD & height,const float &angle, const COLORREF &clrBack, const int &iType)
{
	if(m_ciBitmap->GetPtBitmap() == NULL)
		return;

	AllocationMemoire(m_ciBitmap, width, height);

	int iLocalWidth2;
	int iLocalHeight2;

	SIZE m_size = {iLocalWidth,iLocalHeight};
	SIZE m_sizedst;


	delete[] m_ScaledBitmapBits;m_ScaledBitmapBits = NULL;

	CRotateByShearRGB Rot(CRotateByShearRGB::Progress);
	m_ScaledBitmapBits = Rot.AllocAndRotate (
										m_OriginalBitmapBits,
										m_size,
										angle,
										&m_sizedst,
										clrBack
										);


	DesallocationMemoire(m_ciBitmap,m_sizedst.cx,m_sizedst.cy);

	width = m_sizedst.cx;
	height = m_sizedst.cy;

	//return m_ciBitmap->hBitmap;
}


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

void CImageScale::ApplyImageScale(CIBitmap * m_ciBitmap, const WORD & width, const WORD & height,const int &iType)
{
	if(m_ciBitmap->GetPtBitmap() == NULL)
		return;

	AllocationMemoire(m_ciBitmap, width, height);

	CFast2PassScale * m_LocalFilter = NULL;

	try
	{
		if(iType == 1)
		{
			m_LocalFilter = new CFast2PassScale();                  // Create an instance of the scaling class

 			m_LocalFilter->Scale(m_OriginalBitmapBits,    // Scale!
 							iLocalWidth,
 							iLocalHeight,
 							m_ScaledBitmapBits,
 							width,
 							height);
		}
		else
		{
			switch(iType)
			{
				case BOXFILTER:
					{
						m_LocalFilter = new CBoxFilter();
						m_LocalFilter->Scale(m_OriginalBitmapBits,iLocalWidth,iLocalHeight,
							m_ScaledBitmapBits,width,height);
					}
					break;

				case BILINEARFILTER:
					{
						m_LocalFilter = new CBilinearFilter();
						m_ScaledBitmapBits = m_LocalFilter->AllocAndScale(m_OriginalBitmapBits,iLocalWidth,iLocalHeight, width,height);
					}
					break;

				case GAUSSIANFILTER:
					{
						m_LocalFilter = new CGaussianFilter();
						m_LocalFilter->Scale(m_OriginalBitmapBits,iLocalWidth,iLocalHeight,
							m_ScaledBitmapBits,width,height);
					}
					break;

				case HAMMINGFILTER:
					{
						m_LocalFilter = new CHammingFilter();
						m_LocalFilter->Scale(m_OriginalBitmapBits,iLocalWidth,iLocalHeight,
							m_ScaledBitmapBits,width,height);
					}
					break;

				case CUBICFILTER:
					{
						m_LocalFilter = new CCubicFilter();
						m_LocalFilter->Scale(m_OriginalBitmapBits,iLocalWidth,iLocalHeight,
							m_ScaledBitmapBits,width,height);
					}
					break;

				case BLACKMANFILTER:
					{
						m_LocalFilter = new CBlackmanFilter();
						m_LocalFilter->Scale(m_OriginalBitmapBits,iLocalWidth,iLocalHeight,
							m_ScaledBitmapBits,width,height);
					}
					break;

				case QUADRATICFILTER:
					{
						m_LocalFilter = new CQuadraticFilter();
						m_LocalFilter->Scale(m_OriginalBitmapBits,iLocalWidth,iLocalHeight,
							m_ScaledBitmapBits,width,height);
					}
					break;

				case MITCHELLFILTER:
					{
						m_LocalFilter = new CMitchellFilter();
						m_LocalFilter->Scale(m_OriginalBitmapBits,iLocalWidth,iLocalHeight,
							m_ScaledBitmapBits,width,height);
					}
					break;

				case TRIANGLEFILTER:
					{
						m_LocalFilter = new CTriangleFilter();
						m_LocalFilter->Scale(m_OriginalBitmapBits,iLocalWidth,iLocalHeight,
							m_ScaledBitmapBits,width,height);
					}
					break;

				case SINCFILTER:
					{
						m_LocalFilter = new CSincFilter();
						m_LocalFilter->Scale(m_OriginalBitmapBits,iLocalWidth,iLocalHeight,
							m_ScaledBitmapBits,width,height);
					}
					break;

				case BESSELFILTER:
					{
						m_LocalFilter = new CBesselFilter();
						m_LocalFilter->Scale(m_OriginalBitmapBits,iLocalWidth,iLocalHeight,
							m_ScaledBitmapBits,width,height);
					}
					break;

				case BLACKMANBESSELFILTER:
					{
						m_LocalFilter = new CBlackmanBesselFilter();
						m_LocalFilter->Scale(m_OriginalBitmapBits,iLocalWidth,iLocalHeight,
							m_ScaledBitmapBits,width,height);
					}
					break;

				case BLACKMANSINCFILTER:
					{
						m_LocalFilter = new CBlackmanSincFilter();
						m_LocalFilter->Scale(m_OriginalBitmapBits,iLocalWidth,iLocalHeight,
							m_ScaledBitmapBits,width,height);
					}
					break;

				case LANCZOSFILTER:
					{
						m_LocalFilter = new CLanczosFilter();
						m_LocalFilter->Scale(m_OriginalBitmapBits,iLocalWidth,iLocalHeight,
							m_ScaledBitmapBits,width,height);
					}
					break;

				case HERMITEFILTER:
					{
						m_LocalFilter = new CHermiteFilter();
						m_LocalFilter->Scale(m_OriginalBitmapBits,iLocalWidth,iLocalHeight,
							m_ScaledBitmapBits,width,height);
					}
					break;

				case HANNINGFILTER:
					{
						m_LocalFilter = new CHanningFilter();
						m_LocalFilter->Scale(m_OriginalBitmapBits,iLocalWidth,iLocalHeight,
							m_ScaledBitmapBits,width,height);
					}
					break;

				case CATROMFILTER:
					{
						m_LocalFilter = new CCatromFilter();
						m_LocalFilter->Scale(m_OriginalBitmapBits,iLocalWidth,iLocalHeight,
							m_ScaledBitmapBits,width,height);
					}
					break;

				default:
					{
						m_LocalFilter = new CTriangleFilter();
						m_LocalFilter->Scale(m_OriginalBitmapBits,iLocalWidth,iLocalHeight,
							m_ScaledBitmapBits,width,height);

					}
					break;

			}
		}
	}
	catch(...)
	{
	}

	try
	{
		if(m_LocalFilter != NULL)
			delete m_LocalFilter;
		m_LocalFilter = NULL;
	}
	catch(...)
	{
	}

	try
	{
		DesallocationMemoire(m_ciBitmap,width,height);
	}
	catch(...)
	{
	}	
}
