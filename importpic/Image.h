// Image.h: interface for the CImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGE_H__1F1A324B_5A21_4E05_8E9D_7FA340DA2657__INCLUDED_)
#define AFX_IMAGE_H__1F1A324B_5A21_4E05_8E9D_7FA340DA2657__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000


struct rgb_color { BYTE r,g,b; };

class CImage  
{
public:

	static BOOL VertFlipBuf(BYTE  * inbuf, 
					   UINT widthBytes, 
					   UINT height);

	static void YCbCr2RGB(const int &Y,const int &Cb, const int &Cr,int &r, int &g, int &b);
	static BOOL BGRFromRGB(BYTE *buf, UINT widthPix, UINT height);
	static BYTE * MakeDwordAlignedBuf(BYTE *dataBuf,
									 UINT widthPix,				// pixels!!
									 UINT height,
									 UINT *uiOutWidthBytes);		// bytes!!!

	static BYTE * RGBFromDWORDAligned(BYTE *inBuf,
										UINT widthPix, 
										UINT widthBytes,
										UINT height);

	static BOOL MakeGrayScale(BYTE *buf, UINT widthPix, UINT height);


	static BOOL Convert24to32(BYTE  * inbuf,BYTE * * outbuf, UINT width, UINT height);
};

#endif // !defined(AFX_IMAGE_H__1F1A324B_5A21_4E05_8E9D_7FA340DA2657__INCLUDED_)
