// Tif.cpp: implementation of the CTif class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "image.h"
#include "cTiff.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTif::CTif()
{

}

CTif::~CTif()
{

}

int CTif::ReadTif(CIBitmap * m_ciBitmap, const char *szFileName)
{
    TIFF* tif = TIFFOpen(szFileName, "r");
    if (tif) 
	{
	  int pDestBPP = 32;

	  uint16  BitsPerSample;
	  uint16  SamplePerPixel;

	  // get tagged image parameters
	  TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &BitsPerSample);
	  TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &SamplePerPixel);

	  int ok;

	  TIFFRGBAImage img;
	  char emsg[1024];
	  BYTE * pBits = NULL;

	  ok = TIFFRGBAImageBegin(&img, tif, 0, emsg);

	  if (ok == 0)
	  {
		TIFFClose (tif);
		return -1;
	  }

	  try
	  {
		pBits = new BYTE [img.width*img.height*4];
	  }
	  catch (...)
	  {
		TIFFClose (tif);
		throw;
	  }

	  ok = TIFFRGBAImageGet(&img, (uint32 *) pBits, img.width, img.height);
	  if (!ok)
	  {
		TIFFRGBAImageEnd(&img);
		TIFFClose(tif);
		return -1;
	  }

	  m_ciBitmap->CreateBitmap(img.width,img.height);

	  BYTE * pData = m_ciBitmap->GetPtBitmap();//new BYTE[img.width * img.height * 4];

	  long k = 0;

	  // Correct the byte ordering
	  for (long y=img.height-1; y >= 0; y--)
	  {
		BYTE * pSrc = pBits+(img.height-y-1)*img.width*4;
		for  (UINT x=0; x<img.width; x++,k+=4)
		{
		  pData[k] = *(pSrc+2);
		  pData[k+1] = *(pSrc+1);
		  pData[k+2] = *(pSrc);
		  pData[k+3] = *(pSrc+2);
		  pSrc += 4;
		}
	  }
	  //pData[k]  = '\0';

	UINT nWidth = img.width;
	UINT nHeight = img.height;

	m_ciBitmap->ReCreateHBitmap(false);
	
	//CImage::VertFlipBuf(pData, nWidth * 4, nHeight);

	//m_ciBitmap->SetBitmap(pData,nWidth,nHeight);

	  delete[] pBits;
	  //delete[] pData;
	  TIFFRGBAImageEnd(&img);
	  TIFFClose(tif);
		
    }
	else
	{
		m_ciBitmap->CreateBitmap(0,0);
	}

	return 0;
}

void CTif::GetDimensions(const char *fileName, UINT *width, UINT *height)
{
    TIFF* tif = TIFFOpen(fileName, "r");
    if (tif) 
	{
		int pDestBPP = 32;

		uint16  BitsPerSample;
		uint16  SamplePerPixel;

		// get tagged image parameters
		TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &BitsPerSample);
		TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &SamplePerPixel);

		int ok;

		TIFFRGBAImage img;
		char emsg[1024];

		ok = TIFFRGBAImageBegin(&img, tif, 0, emsg);

		if (ok == 0)
		{
			TIFFClose (tif);
			return;
		}

		*width = img.width;
		*height = img.height;


		TIFFRGBAImageEnd(&img);

		TIFFClose(tif);
	}
	
}

////////////////////////////////////////////////////////////////////////////////
// Thanks to Abe <God.bless@marihuana.com>
bool CTif::SaveTif(CIBitmap &m_ciBitmap,const char * szFileName,const bool &bCompress,const int &iJpegQuality)
{
	TIFF *m_tif=NULL;
	int pagecount = 1;
	try
	{
		m_tif= TIFFOpen(szFileName, "w+b");
		if (m_tif==NULL) 
			throw "initialization fail";

		if (EncodeBody(m_tif,m_ciBitmap,bCompress,iJpegQuality)) 
			throw "Error saving TIFF file";

		if (!TIFFWriteDirectory(m_tif)) 
			throw "Error saving TIFF directory";
	} 
	catch (char *message) 
	{
		if (m_tif) 
			TIFFClose(m_tif);
		return false;
	}
	TIFFClose(m_tif);
	return true;
}
////////////////////////////////////////////////////////////////////////////////
bool CTif::EncodeBody(TIFF *m_tif, CIBitmap &m_ciBitmap,const bool &bCompress,const int &iJpegQuality)
{

	uint32 height= m_ciBitmap.GetBitmapHeigth();
	uint32 width= m_ciBitmap.GetBitmapWidth();
	uint16 bitcount= m_ciBitmap.GetBitmapDepth();
	uint16 bitspersample;
	uint16 samplesperpixel;
	uint16 photometric=0;
	uint16 compression;
//	uint16 pitch;
//	int line;
	uint32 x, y;

	samplesperpixel=4; 

	bitspersample = bitcount / samplesperpixel;

	photometric = PHOTOMETRIC_RGB;			

	// handle standard width/height/bpp stuff
	TIFFSetField(m_tif, TIFFTAG_IMAGEWIDTH, width);
	TIFFSetField(m_tif, TIFFTAG_IMAGELENGTH, height);
	TIFFSetField(m_tif, TIFFTAG_SAMPLESPERPIXEL, samplesperpixel);
	TIFFSetField(m_tif, TIFFTAG_BITSPERSAMPLE, bitspersample);
	TIFFSetField(m_tif, TIFFTAG_PHOTOMETRIC, photometric);
	TIFFSetField(m_tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);	// single image plane 
	TIFFSetField(m_tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(m_tif, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(m_tif, -1));  //<REC> gives better compression

	// handle metrics
	TIFFSetField(m_tif, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
	TIFFSetField(m_tif, TIFFTAG_XRESOLUTION, 72.0);
	TIFFSetField(m_tif, TIFFTAG_YRESOLUTION, 72.0);

	TIFFSetField(m_tif, TIFFTAG_SUBFILETYPE, 0);

	if(bCompress)
	{
		compression = COMPRESSION_JPEG;	//COMPRESSION_PACKBITS;
		TIFFSetField(m_tif, TIFFTAG_JPEGQUALITY, iJpegQuality); //needed for COMPRESSION_JPEG
		TIFFSetField(m_tif, TIFFTAG_ROWSPERSTRIP, 8); //needed for COMPRESSION_JPEG
	}
	else
	{
		compression = COMPRESSION_NONE;
	}

	TIFFSetField(m_tif, TIFFTAG_COMPRESSION, compression);

	// read the DIB lines from bottom to top
	// and save them in the TIF
	// -------------------------------------	
	//BYTE *bits;

	long k = 0;
	BYTE * data = m_ciBitmap.GetPtBitmap();


	BYTE * buffer = (BYTE *)malloc(width * 4);
	for (y = 0; y < height; y++) {
	
		int i = 0;

		k = (height - y - 1) * width * 4;

		for (x = 0; x < width; x++,k+=4,i+=4)
		{
			buffer[i] = data[k+2];
			buffer[i+1] = data[k+1];
			buffer[i+2] = data[k];
			buffer[i+3] = data[k+3];
		}
		// write the scanline to disc
		TIFFWriteScanline(m_tif, buffer, y, 0);
	}
	free(buffer);

	return false;
}