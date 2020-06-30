////////////////////////////////////////////////////////////
//
//	JpegFile - A C++ class to allow reading and writing of
//	RGB and Grayscale JPEG images. (actually, it reads all forms
//	that the JPEG lib will decode into RGB or grayscale) and
//	writes only RGB and Grayscale.
//
//	It is based on a Win32 compilation of the IJG V.6a code.
//
//	This will only work on 32-bit Windows systems. I have only 
//	tried this with Win 95, VC++ 4.1.
//
//	This class Copyright 1997, Chris Losinger
//	This is free to use and modify provided my name is included.
//
//	Comments:
//	Thanks to Robert Johnson for discovering a DWORD-alignment bug
//	Thanks to Lee Bode for catching a bug in CMfcappView::OnFileGetdimensionsjpg() 
//
////////////////////////////////////////////////////////////
//
//	General Usage:
//
//	#include this file.
//	link with jpeglib2.lib
//
//	All functions here are static. There is no need to have a JpegFile object.
//	There is actually nothing in a JpegFile object anyway. 
//
//	So, you can do this :
//
//		BOOL ok = JpegFile::vertFlipBuf(buf, widthbytes, height);
//
//	instead of this :
//
//		JpegFile jpgOb;
//		BOOL ok = jpgOb.vertFlipBuf(buf, widthbytes, height);
//
/////
//
//	Linking usage :
//	It is sometimes necessary to set /NODEFAULTLIB:LIBC (or LIBCD) to use this
//	class. 
//
/////
//
//	Error reporting:
//	The class generates message boxes in response to JPEG errors.
//
//	The JpegFile.cpp fn my_error_exit defines the behavior for
//	fatal errors : show msg box, return to caller.
//
//	Warnings are handled by jpeglib.lib - which	generates msg boxes too.
//	
////////////////////////////////////////////////////////////////



//
//	for DWORD aligning a buffer
//

class CIBitmap;

#ifdef __cplusplus
	extern "C" {
#endif // __cplusplus

#include "jpeglib.h"

#ifdef __cplusplus
	}
#endif // __cplusplus




#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

class JpegFile 
{
public:

	////////////////////////////////////////////////////////////////
	// read a JPEG file to an RGB buffer - 3 bytes per pixel
	// returns a ptr to a buffer .
	// caller is responsible for cleanup!!!
	// BYTE *buf = JpegFile::JpegFileToRGB(....);
	// delete [] buf;

	BYTE * JpegFileToRGB(const char * fileName,			// path to image
							   UINT *width,					// image width in pixels
							   UINT *height);				// image height

	////////////////////////////////////////////////////////////////
	// write a JPEG file from a 3-component, 1-byte per component buffer

	BOOL RGBToJpegFile(const char * fileName,				// path
							BYTE *dataBuf,					// RGB buffer
							UINT width,						// pixels
							UINT height,					// rows
							BOOL color,						// TRUE = RGB
															// FALSE = Grayscale
							int quality);					// 0 - 100


	////////////////////////////////////////////////////////////////
	// fetch width / height of an image
	
	BOOL GetJPGDimensions(const char * fileName,			// path
								UINT *width,				// pixels
								UINT *height);

	

	////////////////////////////////////////////////////////////////
	// these do nothing
	JpegFile();		// creates an empty object
	~JpegFile();	// destroys nothing


	BOOL JpegFromDib(CIBitmap &m_ciBitmap,     //Handle to DIB
					 int        nQuality, //JPEG quality (0-100)
					 const char * csJpeg,   //Pathname to jpeg file
					 char *   pcsMsg);   //Error msg to return


	BOOL GetImageFromJPG(const char * FileName, HBITMAP &hBitmap,const long &JPGThumbWidth, const long &JPGThumbHeight);


	int ReadJpeg(CIBitmap * m_ciBitmap,const char * szFileName);
	

private:

	int ReadJpeg_Intel(CIBitmap * m_ciBitmap,const char * szFileName);
	int ReadJpeg_Std(CIBitmap * m_ciBitmap,const char * szFileName);

	void RGBA_FPX_to_BGRA(BYTE* data,int width,int height);

	void j_putRGBScanline(BYTE *jpegline, 
					 int widthPix,
					 BYTE *outBuf,
					 int row);

	void j_putGrayScanlineToRGB(BYTE *jpegline, 
								 int widthPix,
								 BYTE *outBuf,
								 int row);

	BOOL DibToSamps(CIBitmap &m_ciBitmap,
					int                         nSampsPerRow,
					struct jpeg_compress_struct cinfo,
					JSAMPARRAY                  jsmpPixels,
					char *                    pcsMsg);

};