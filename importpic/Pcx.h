// Pcx.h: interface for the CPcx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PCX_H__EDD7DADD_9B55_4690_BC77_C997F1B5D04F__INCLUDED_)
#define AFX_PCX_H__EDD7DADD_9B55_4690_BC77_C997F1B5D04F__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

class CIBitmap;

#define PCX_HEADER_SIZE 128	// The size in bytes of a .pcx header file
#define PCX_PALETTE_TEST 769 // This is the offset from the bottom of a .pcx file
							// that (in a version 5 PCX file only) will be set to '12'
						   // if the .pcx file has a palette associated with it (is 8-bit)

class CPcx  
{

	
	typedef struct PCX_HEADER
	{
		char Manufacturer;	// Always 10 -- 
		char Version; // Version information
					 // 0 = Ver2.5, 2 = Ver2.8 with palette, 3 = Ver2.8 without palette,
					// 4 = PC Paintbrush for Windows, 5 = Ver 3.0
		
		char Encoding; // 1 = Run Length Encoding
		
		char BitsPerPixel; // Number of bits that represent a pixel
		
		short xMin, yMin, xMax, yMax; // Image Dimensions
		
		short HorDPI; // Horizontal Resolution of pixels in dots per inch
		short VerDPI; // Vertical Resolution of pixels in dots per inch
		
		char Colormap[48]; // Color palette setting
		
		char Reserved1; // Should be set to zero
		char nPlanes; // Number of color planes
		short BytesPerLine; // Number of bytes to allocate per scanline
							// Must be an even number
		
		short PaletteInfo; // How to interpet palette

		short HorScreenSize; // Horizontal screen size in pixels
		short VerScreenSize; // Vertical screen size in pixels

		char Filler[54]; // An addtional 54 bytes that must be set to 0 -- 
						 // Total size of a .pcx header is 128 bytes
	};

	   // Standard PCX header
   struct PCXHead {
     char   ID;
     char   Version;
     char   Encoding;
     char   BitPerPixel;
     short  X1;
     short  Y1;
     short  X2;
     short  Y2;
     short  HRes;
     short  VRes;
     char   ClrMap[16*3];
     char   Reserved1;
     char   NumPlanes;
     short  BPL;
     short  Pal_t;
     char   Filler[58];
   } sHeader;

public:
	void GetDimensions(const char * fileName,UINT *width,UINT *height);
	CPcx();
	virtual ~CPcx();

	bool PCX_SetPalette(FILE * file_pointer, const PCX_HEADER &pcx_header);

	int ReadPcx(CIBitmap * m_ciBitmap,const char * szFileName);

	int ReadPcx2(CIBitmap * m_ciBitmap, const char * szFileName);

	bool FillScanLine(FILE *file_pointer, BYTE *scan_line, int total_bytes);


};

#endif // !defined(AFX_PCX_H__EDD7DADD_9B55_4690_BC77_C997F1B5D04F__INCLUDED_)
