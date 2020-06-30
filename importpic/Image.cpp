// Image.cpp: implementation of the CImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Image.h"

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CImage::Convert24to32(BYTE  * inbuf,BYTE * * outbuf, UINT width, UINT height)
{
	long lWidthSize = ((((width * 24) + 31) & ~31) >> 3);
	long k = 0;

	*outbuf = (BYTE *)malloc(width * height * 4);

	for(int y = 0;y < height;y++)
	{
		for(int x = 0;x < width;x++)
		{
			int iPos = y * lWidthSize + x * 3;
			*outbuf[k++] = inbuf[iPos];
			*outbuf[k++] = inbuf[iPos+1];
			*outbuf[k++] = inbuf[iPos+2];
			*outbuf[k++] = 0;
		}
	}

	return true;
}

//
//	vertically flip a buffer 
//	note, this operates on a buffer of widthBytes bytes, not pixels!!!
//

BOOL CImage::VertFlipBuf(BYTE  * inbuf, 
					   UINT widthBytes, 
					   UINT height)
{   
	BYTE  *tb1;
	BYTE  *tb2;

	if (inbuf==NULL)
		return FALSE;

	UINT bufsize;

	bufsize=widthBytes;

	tb1= (BYTE *)new BYTE[bufsize];
	if (tb1==NULL) {
		return FALSE;
	}

	tb2= (BYTE *)new BYTE [bufsize];
	if (tb2==NULL) {
		delete [] tb1;
		return FALSE;
	}
	
	UINT row_cnt;     
	ULONG off1=0;
	ULONG off2=0;

	for (row_cnt=0;row_cnt<(height+1)/2;row_cnt++) {
		off1=row_cnt*bufsize;
		off2=((height-1)-row_cnt)*bufsize;   
		
		memcpy(tb1,inbuf+off1,bufsize);
		memcpy(tb2,inbuf+off2,bufsize);	
		memcpy(inbuf+off1,tb2,bufsize);
		memcpy(inbuf+off2,tb1,bufsize);
	}	

	delete [] tb1;
	delete [] tb2;

	return TRUE;
}        


void CImage::YCbCr2RGB(const int &Y,const int &Cb, const int &Cr,int &r, int &g, int &b)
{
	int C=256;
	double c11 = 0.0054980*C;
	double c12 = 0;
	double c13 = 0.0051681*C;
	double c21 = 0.0054980*C;
	double c22 =-0.0015446*C;
	double c23 =-0.0026325*C;
	double c31 = 0.0054980*C;
	double c32 = 0.0079533*C;
	double c33 = 0;

	r=(int)(c11*Y +c12*(Cb-156) +c13*(Cr-137));
	g=(int)(c21*Y +c22*(Cb-156) +c23*(Cr-137));
	b=(int)(c31*Y +c32*(Cb-156) +c33*(Cr-137));

	if(r<0)
		r=0;

	if(g<0)
		g=0;

	if(b<0)
		b=0;

	if(r>255)
		r=255;

	if(g>255)
		g=255;

	if(b>255)
		b=255;

}


BOOL CImage::BGRFromRGB(BYTE *buf, UINT widthPix, UINT height)
{
	if (buf==NULL)
		return FALSE;

	try
	{
		UINT col, row;
		for (row=0;row<height;row++) {
			for (col=0;col<widthPix;col++) {
				LPBYTE pRed, pGrn, pBlu;
				pRed = buf + row * widthPix * 3 + col * 3;
				pGrn = buf + row * widthPix * 3 + col * 3 + 1;
				pBlu = buf + row * widthPix * 3 + col * 3 + 2;

				// swap red and blue
				BYTE tmp;
				tmp = *pRed;
				*pRed = *pBlu;
				*pBlu = tmp;
			}
		}
	}
	catch(...)
	{
		return false;
	}
	return TRUE;
}



BYTE * CImage::MakeDwordAlignedBuf(BYTE *dataBuf,
									 UINT widthPix,				// pixels!!
									 UINT height,
									 UINT *uiOutWidthBytes)		// bytes!!!
{
	////////////////////////////////////////////////////////////
	// what's going on here? this certainly means trouble 
	if (dataBuf==NULL)
		return NULL;

	////////////////////////////////////////////////////////////
	// how big is the smallest DWORD-aligned buffer that we can use?
	UINT uiWidthBytes;
	uiWidthBytes = WIDTHBYTES(widthPix * 24);

	DWORD dwNewsize=(DWORD)((DWORD)uiWidthBytes * 
							(DWORD)height);
	BYTE *pNew;

	////////////////////////////////////////////////////////////
	// alloc and open our new buffer
	pNew=(BYTE *)new BYTE[dwNewsize];
	if (pNew==NULL) {
		return NULL;
	}
	
	////////////////////////////////////////////////////////////
	// copy row-by-row
	UINT uiInWidthBytes = widthPix * 3;
	UINT uiCount;
	for (uiCount=0;uiCount < height;uiCount++) {
		BYTE * bpInAdd = NULL;
		BYTE * bpOutAdd = NULL;
		ULONG lInOff;
		ULONG lOutOff;

		lInOff=uiInWidthBytes * uiCount;
		lOutOff=uiWidthBytes * uiCount;

		bpInAdd= dataBuf + lInOff;
		bpOutAdd= pNew + lOutOff;

		memcpy(bpOutAdd,bpInAdd,uiInWidthBytes);
	}

	*uiOutWidthBytes=uiWidthBytes;
	return pNew;
}


BYTE *CImage::RGBFromDWORDAligned(BYTE *inBuf,
									UINT widthPix, 
									UINT widthBytes,
									UINT height)
{
	if (inBuf==NULL)
		return NULL;


	BYTE *tmp;
	tmp=(BYTE *)new BYTE[height * widthPix * 3];
	if (tmp==NULL)
		return NULL;

	UINT row;

	for (row=0;row<height;row++) {
		memcpy((tmp+row * widthPix * 3), 
				(inBuf + row * widthBytes), 
				widthPix * 3);
	}

	return tmp;
}


//
//	Note! this does its stuff on buffers with a whole number of pixels
//	per data row!!
//

BOOL CImage::MakeGrayScale(BYTE *buf, UINT widthPix, UINT height)
{
	if (buf==NULL)
		return FALSE;

	UINT row,col;
	for (row=0;row<height;row++) {
		for (col=0;col<widthPix;col++) {
			LPBYTE pRed, pGrn, pBlu;
			pRed = buf + row * widthPix * 3 + col * 3;
			pGrn = buf + row * widthPix * 3 + col * 3 + 1;
			pBlu = buf + row * widthPix * 3 + col * 3 + 2;

			// luminance
			int lum = (int)(.299 * (double)(*pRed) + .587 * (double)(*pGrn) + .114 * (double)(*pBlu));

			*pRed = (BYTE)lum;
			*pGrn = (BYTE)lum;
			*pBlu = (BYTE)lum;
		}
	}
	return TRUE;
}