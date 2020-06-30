// Tga.h: interface for the CTga class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TGA_H__C778C003_4B10_4F1C_A817_8999E44E2A66__INCLUDED_)
#define AFX_TGA_H__C778C003_4B10_4F1C_A817_8999E44E2A66__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

class CIBitmap;

class CTga  
{
public:

	void GetDimensions(const char * fileName,UINT *width,UINT *height);

	struct TGA_Header
	{
		BYTE  IdLength;          // size of ID field that follows 18 byte header (0 usually)
		BYTE  CmapType;      // type of colour map 0=none, 1=has palette
		BYTE  ImageType;          // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

		short int CmapIndex;     // first colour map entry in palette
		short int CmapLength;    // number of colours in palette
		BYTE  CmapEntrySize;      // number of bits per palette entry 15,16,24,32

		short int X_Origin;             // image x origin
		short int Y_Origin;             // image y origin
		short ImageWidth;              // image width in pixels
		short ImageHeight;             // image height in pixels
		BYTE  PixelDepth;               // image bits per pixel 8,16,24,32
		BYTE  ImagDesc;         // image descriptor bits (vh flip bits)
	};

	CTga();
	virtual ~CTga();

	int TGA(CIBitmap * m_ciBitmap,const char * szFileName);
	int SaveTGA(CIBitmap &m_ciBitmap,const char * szFileName);

};

#endif // !defined(AFX_TGA_H__C778C003_4B10_4F1C_A817_8999E44E2A66__INCLUDED_)
