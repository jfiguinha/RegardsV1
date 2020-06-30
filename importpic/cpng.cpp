// Png.cpp: implementation of the CPng class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cpng.h"
#include "image.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPng::CPng()
{
	
}

CPng::~CPng()
{

}

/* This function is called when there is a warning, but the library thinks
 * it can continue anyway.  Replacement functions don't have to do anything
 * here if you don't want to.  In the default configuration, png_ptr is
 * not used, but it is passed in case it may be useful.
 */
void CPng::pngtest_warning(png_structp png_ptr, png_const_charp message)
{
}

/* This is the default error handling function.  Note that replacements for
 * this function MUST NOT RETURN, or the program will likely crash.  This
 * function is used by default, or if the program supplies NULL for the
 * error function pointer in png_set_error_fn().
 */
void CPng::pngtest_error(png_structp png_ptr, png_const_charp message)
{
   pngtest_warning(png_ptr, message);
   /* We can return because png_error calls the default handler, which is
      * actually OK in this case. */
}

///////////////////////////////////////////////////////////////////////////
//PNG
///////////////////////////////////////////////////////////////////////////


#define for if (0) ; else for

//---------------------------------------------------------------------------
// LoadPng
void CPng::PngReadProc(png_structp png_ptr, png_bytep data, png_size_t length) {
  ReadFile(png_get_io_ptr(png_ptr), data, length, (DWORD*)&length, NULL);
}

//---------------------------------------------------------------------------
// LoadPng
void CPng::to4bpp(png_structp png_ptr, png_row_infop row_info, png_bytep data) {
  
const png_byte pix[] = {
    0x00, 0x01, 0x02, 0x03, 0x10, 0x11, 0x12, 0x13,
    0x20, 0x21, 0x22, 0x23, 0x30, 0x31, 0x32, 0x33,
  };
  png_uint_32 rowb;
  png_byte *p, *q, c;

  rowb = (row_info->width + 1) / 2;
  q = data + rowb;
  p = data + rowb / 2;

  if (rowb % 2 == 1) {
    c = *p;
    *(--q) = pix[c >> 4];
  }
  while (p > data) {
    c = *(--p);
    *(--q) = pix[c & 0x0f];
    *(--q) = pix[c >> 4];
  }
  row_info->bit_depth   = 4;
  row_info->pixel_depth = 4;
  row_info->rowbytes    = rowb;
}

//---------------------------------------------------------------------------
// PNG
//---------------------------------------------------------------------------

int CPng::ReadPng(CIBitmap * m_ciBitmap,const char * szFileName)
{
	
	HANDLE hFile = CreateFile(szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	return -1;

	// PNG
	png_struct *png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
	CloseHandle(hFile);
	return -1;
	}

	png_info *info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
	png_destroy_read_struct(&png_ptr, NULL, NULL);
	CloseHandle(hFile);
	return -1;
	}
	png_info *end_info = png_create_info_struct(png_ptr);
	if (!end_info) {
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	CloseHandle(hFile);
	return -1;
	}

	png_set_read_fn(png_ptr, hFile, PngReadProc);
	png_uint_32 nWidth, nHeight;

	int nDepth, nPal;
	int nPngDepth, nColorType, nInterlaceType, nCompType, nFilterType;

	png_set_error_fn(png_ptr, (png_voidp)szFileName, pngtest_error, pngtest_warning);

	if(png_read_info(png_ptr, info_ptr) == -1)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		CloseHandle(hFile);
		return -1;
	}

	png_get_IHDR(png_ptr, info_ptr, &nWidth, &nHeight, &nPngDepth, &nColorType, &nInterlaceType, &nCompType, &nFilterType);

	if (nColorType == PNG_COLOR_TYPE_RGB || nColorType == PNG_COLOR_TYPE_RGB_ALPHA) {
	nPngDepth = 24;
	nDepth = 24;
	nPal = 0;
	} else {
	switch (nPngDepth) {
	  case 2:  nDepth = 4; break;
	  case 16: nDepth = 8; break;
	  default: nDepth = nPngDepth; break;
	}
	nPal = 1 << nDepth;
	}

	png_color * vPalette = NULL;
	if (nPal > 0)
	{
		vPalette = (png_color *)malloc(nPal * sizeof(png_color));
	}

	int nRowBytes = (nWidth * nDepth + 31) / 32 * 4;
	int nImgBytes = nRowBytes * nHeight;
	BYTE *pImgPtr = (BYTE*)GlobalAlloc(GMEM_FIXED, nImgBytes);
	
	//vector<BYTE*> vRowPtr;

	png_bytepp ppbRowPointers = (png_bytepp)malloc((nHeight) * sizeof(png_bytep));

	//vRowPtr.reserve(nHeight);

	//for (int y = nHeight - 1; y >= 0; --y)
	//	vRowPtr.push_back(pImgPtr + y * nRowBytes);

    for (int y = 0; y < nHeight; y++)
        ppbRowPointers[y] = pImgPtr + y * nRowBytes;


	if (nColorType & PNG_COLOR_MASK_ALPHA)
		png_set_strip_alpha(png_ptr);
	if (nPngDepth == 2)
		png_set_read_user_transform_fn(png_ptr, to4bpp);
	else if (nPngDepth == 16)
		png_set_strip_16(png_ptr);

	if (nColorType== PNG_COLOR_TYPE_RGB || nColorType == PNG_COLOR_TYPE_RGB_ALPHA)
	png_set_bgr(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	if (nPal > 0) 
	{
		if (nColorType == PNG_COLOR_TYPE_PALETTE) 
		{
			
			png_color * palette;

			int num_palette;
			png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
			if (num_palette > nPal)
				num_palette = nPal;

			//vPalette = (png_color *)malloc(nPal * sizeof(png_color));

			memset(vPalette, 0, nPal * sizeof(png_color));
			memcpy(vPalette, palette, num_palette * sizeof(png_color));
		} 
		else 
		{
			int depth = nPngDepth == 16 ? 8 : nPngDepth;
			png_build_grayscale_palette(depth, vPalette);
		}
	}

	png_read_image(png_ptr, ppbRowPointers);
	png_read_end(png_ptr, end_info);
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

	CloseHandle(hFile);


	
	BITMAPINFO* bi = (BITMAPINFO*)GlobalAlloc(GMEM_FIXED, sizeof BITMAPINFOHEADER + nPal * sizeof RGBQUAD);

	memset(bi, 0, sizeof BITMAPINFOHEADER);
	bi->bmiHeader.biSize        = sizeof BITMAPINFOHEADER;
	bi->bmiHeader.biWidth        = nWidth;
	bi->bmiHeader.biHeight      = nHeight;
	bi->bmiHeader.biPlanes      = 1;
	bi->bmiHeader.biBitCount    = nDepth;
	bi->bmiHeader.biCompression = BI_RGB;
	bi->bmiHeader.biSizeImage    = nImgBytes;
	bi->bmiHeader.biClrUsed      = nPal;

	for (int i = 0; i < nPal; ++i) 
	{
		bi->bmiColors[i].rgbRed   = vPalette[i].red;
		bi->bmiColors[i].rgbGreen = vPalette[i].green;
		bi->bmiColors[i].rgbBlue  = vPalette[i].blue;
	}

	HDC hdc = GetDC(NULL);
	//HBITMAP hBitmap;
	char *pBits;

	HBITMAP hBitmap;

	hBitmap = CreateDIBSection(hdc, bi, DIB_RGB_COLORS, (void**)&pBits, NULL, 0);
	if (pBits)
		memcpy(pBits, pImgPtr, nImgBytes);

	DeleteDC(hdc);

	m_ciBitmap->SetBitmap(hBitmap);

	DeleteObject(hBitmap);

	if(vPalette != NULL)
		free(vPalette);

	free(ppbRowPointers);

	GlobalFree(pImgPtr);
	GlobalFree(bi);
	return 0;
}

//-----------------
//  end of source
//-----------------


int CPng::GetDimensions(const char *fileName, UINT *width, UINT *height)
{

	HANDLE hFile = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return -1;

	// PNG
	png_struct *png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
	CloseHandle(hFile);
	return -1;
	}

	png_info *info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
	png_destroy_read_struct(&png_ptr, NULL, NULL);
	CloseHandle(hFile);
	return -1;
	}
	png_info *end_info = png_create_info_struct(png_ptr);
	if (!end_info) 
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		CloseHandle(hFile);
		return -1;
	}

	try
	{
		png_set_read_fn(png_ptr, hFile, PngReadProc);
		png_uint_32 nWidth, nHeight;

		png_set_error_fn(png_ptr, (png_voidp)fileName, pngtest_error, pngtest_warning);

		int nPngDepth, nColorType, nInterlaceType, nCompType, nFilterType;

		if(png_read_info(png_ptr, info_ptr) == 0)
		{
			png_get_IHDR(png_ptr, info_ptr, &nWidth, &nHeight, &nPngDepth, &nColorType, &nInterlaceType, &nCompType, &nFilterType);

			*width = nWidth;
			*height = nHeight;
		}
		else
		{
			*width = 0;
			*height = 0;
		}
	}
	catch(...)
	{
		*width = 0;
		*height = 0;
	}
	
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	CloseHandle(hFile);
	return 0;
}
