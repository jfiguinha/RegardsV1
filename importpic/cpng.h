// Png.h: interface for the CPng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PNG_H__65155807_EC3D_4128_A7A8_67BF2B3CFFEE__INCLUDED_)
#define AFX_PNG_H__65155807_EC3D_4128_A7A8_67BF2B3CFFEE__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include "png.h"

class CIBitmap;

class CPng  
{
public:


	int GetDimensions(const char * fileName,UINT *width,UINT *height);
	CPng();
	virtual ~CPng();

	int ReadPng(CIBitmap * m_ciBitmap,const char * szFileName);

private:

	static void PngReadProc(png_structp png_ptr, png_bytep data, png_size_t length);
	static void to4bpp(png_structp png_ptr, png_row_infop row_info, png_bytep data);
	static void pngtest_error(png_structp png_ptr, png_const_charp message);
	static void pngtest_warning(png_structp png_ptr, png_const_charp message);

};

#endif // !defined(AFX_PNG_H__65155807_EC3D_4128_A7A8_67BF2B3CFFEE__INCLUDED_)
