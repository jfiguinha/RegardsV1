// Pcx.cpp: implementation of the CPcx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Pcx.h"
#include "image.h"


#define	BLUE_BITMAP_OFFSET	0
#define	GREEN_BITMAP_OFFSET	1
#define	RED_BITMAP_OFFSET	2

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPcx::CPcx()
{

}

CPcx::~CPcx()
{

}


// Fills "scan_line" with the current scanline
// If an error is incurred returns false -- otherwise returns true
bool CPcx::FillScanLine(FILE *file_pointer, BYTE *scan_line, int total_bytes)
{
	BYTE byte_value = 0; // The value of a particular byte (a color)
	BYTE byte_count = 0; // How many times this color is reapeated
	BYTE temp_byte = 0; // A temporary place holder to deternime if a particular color is 
					   // repeated or not
	
	int scanLineCount = 0; // Number of bytes we've traversed from left to right in 
						  // a particular scanline
	
	// Loop till the end of the scanline is reached
	while(scanLineCount < total_bytes)
	{
		// Read in temp_byte -- We'll use this to determine if the color needs to be 
		// repeated
		if(!fread(&temp_byte,sizeof(BYTE),1,file_pointer))
			return false;

							/*	**Side Note**
								
								  The encoding method works like this:
								  For each byte X read from the file,
								  if the top two bits of X are 1's then
								  count = 6 lowest bits of X
								  data = next byte following X
								  otherwise, count = 1 and  data = X
							*/
		
		// If the top two bits are set
		if((temp_byte & 0xC0) == 0xC0) 
		{
			byte_count = temp_byte & 0x3F; // bottom six bits are count of bytes to repeat
										  // of the next byte read in

			// read the byte value for repetition, check for errors
			if(!fread(&byte_value,sizeof(BYTE),1,file_pointer))
				return false;
			
		}
		else // byte that was read in is only repeated once
		{	
			byte_count = 1;
			byte_value = temp_byte;
		}

		// Fill the current portion of the scanline
		for(int i = 0; i < byte_count; i++, scanLineCount++)
			scan_line[scanLineCount] = byte_value;

	} // end of while(scanLineCount < total_bytes)

	return true; // "scan_line" filled successfully

} // end of FillScanLine(FILE *file_pointer, BYTE *scan_line, int total_bytes)

///////////////////////////////////////////////////////////////////////////////////////////////////
// If the .pcx file is Version 5 AND is 8-bit, there is a palette associated with it that we
// must load
///////////////////////////////////////////////////////////////////////////////////////////////////

bool CPcx::PCX_SetPalette(FILE *file_pointer, const PCX_HEADER &pcx_header)
{
	// Not version 5, DO NOT have to load palette
	if((int)pcx_header.Version != 5)
		return true; // No need to load palette

	// Ohterwise it's version 5 so we have to test to see if it has a palette (if it's 8-bit)
	
	BYTE test_byte; // byte to check to see if it's a 8-bit .pcx file
			
	// If fseek fails return false for an error has occurred
	if(fseek(file_pointer,-PCX_PALETTE_TEST,SEEK_END))
	{
			return false;
	}

	// If fread fails return false for an error has occurred
	if(!fread(&test_byte,sizeof(BYTE),1,file_pointer))
	{
			return false;
	}

	// Check the test byte -- If it DOES NOT equal '12' we DO NOT have a palette to read in
	if(test_byte != 12)
		return true;
	
	return false;
} // end of PCX_SetPalette(FILE file_pointer, PCX_HEADER pcx_header)

///////////////////////////////////////////////////////////////////////////////////////////////////
// Here's where all the magic happens in loading the .pcx file :)
///////////////////////////////////////////////////////////////////////////////////////////////////

int CPcx::ReadPcx(CIBitmap * m_ciBitmap,const char * szFileName)
{
	FILE * file_pointer = NULL;

	errno_t err;
	err  = fopen_s(&file_pointer, szFileName, "rb");
	if(err != 0)
	{
		return NULL;
	}
	
	//FILE * file_pointer = fopen(szFileName,"rb"); // Open the file
	
	PCX_HEADER pcx_header = {0}; // Create a zeroed out PCX_HEADER

	// Error Check
	if(!file_pointer)
	{
		return NULL;
	}
	
	// Read in the .pcx file header
	if(!fread(&pcx_header,sizeof(PCX_HEADER),1,file_pointer))
	{
		fclose(file_pointer);
		return NULL;
	}

	// Error Check -- Manufacturer better equal 10 otherwise this 
	// for sure isn't a .pcx file
	if((int)pcx_header.Manufacturer != 10)
	{
		fclose(file_pointer);
		return NULL;
	}

	
	// Okay we have the header -- Let's determine the width, height, and number of channels
	// that are in the .pcx file
	int width = pcx_header.xMax - pcx_header.xMin + 1;
	int height = pcx_header.yMax - pcx_header.yMin + 1; 
	int channels = pcx_header.nPlanes;

	if(channels == 3)
	{

		HBITMAP hBitmap;
		BYTE * surface_bits = NULL;

		// Set stride
		int bmp_stride = width * channels;

		while((bmp_stride % 4) != 0) // Ensure bmp_stride is dword aligned
			bmp_stride++;
		
		
		BITMAPINFO bmp_info = {0};

		// Initialize the parameters that we care about
		bmp_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmp_info.bmiHeader.biWidth = width;
		bmp_info.bmiHeader.biHeight = height;
		bmp_info.bmiHeader.biPlanes = 1;
		bmp_info.bmiHeader.biBitCount = channels * 8;
		bmp_info.bmiHeader.biCompression = BI_RGB;
		bmp_info.bmiHeader.biClrUsed = (channels == 1? 256:0); // If it's 8-bit set to 256
																   // Otherwise set to 0

		// Create a DIBSection. Note that this returns us two
		// things - an HBITMAP handle and a memory pointer, in
		// surface_bits.
		hBitmap = CreateDIBSection(GetDC(NULL), &bmp_info, DIB_RGB_COLORS,
								   (void**)&surface_bits, 0, 0);
		// Error Check
		if(!hBitmap)
		{
			fclose(file_pointer);

			ReadPcx2(m_ciBitmap,szFileName);
			return false;
		}

		//bitmap.set_size(NULL,width,height,channels);

		// Check if we need to load a palette -- if we do this function does that as well
		// Returns false if an error incurred
		
		if(!PCX_SetPalette(file_pointer, pcx_header))
		{
			// Displays MessageBox internally in function PCX_SetPalette()
			fclose(file_pointer);
			ReadPcx2(m_ciBitmap,szFileName);
			return NULL;
		}

		
		// Next seek past the PCX_HEADER
		fseek(file_pointer,PCX_HEADER_SIZE,SEEK_SET);

		// The total number of bytes in a scanline
		int total_bytes = (int)pcx_header.nPlanes * pcx_header.BytesPerLine;
		
		BYTE *scan_line = new BYTE[total_bytes];//(BYTE*)malloc(sizeof(BYTE) * total_bytes);

		// Error Check
		if(scan_line == NULL)
		{
			fclose(file_pointer);
			return NULL;
		}

		// Now loop over the entire .pcx file -- We'll fill each "scan_line" and then
		// convert that scan_line to our DIB_BITMAP depending on what type of .pcx we're dealing with
		for(int lineCount = 0; lineCount < height; lineCount++)
		{
			
			if(!FillScanLine(file_pointer, scan_line, total_bytes))
			{
				fclose(file_pointer);
				return NULL;
			}

			// If it's an 8-bit .pcx file, simply copy over the scanline
			if((int)pcx_header.nPlanes == 1) 
				memcpy(surface_bits + bmp_stride * (height - lineCount - 1),scan_line,bmp_stride);
			
			else // Converge "scan_line" into a the same form as a DIB_BITMAP 24-bit scanline
			{
				BYTE *DIB_scanline = new BYTE[total_bytes];
					
					// Error Check
					if(DIB_scanline == NULL)
					{
						fclose(file_pointer);
						return NULL;
					}
				
				// scan_line actually contains three lines -- one of red, green, blue
				// They need to be intergrated into one DIB_BITMAP scanline 
				for(int i = 0; i < width; i++)
				{
					DIB_scanline[i * pcx_header.nPlanes + RED_BITMAP_OFFSET] = scan_line[i];
					DIB_scanline[i * pcx_header.nPlanes + GREEN_BITMAP_OFFSET] = scan_line[i + pcx_header.BytesPerLine];
					DIB_scanline[i * pcx_header.nPlanes + BLUE_BITMAP_OFFSET] = scan_line[i + pcx_header.BytesPerLine * 2];
				}
				
				// Copy it over
				memcpy(surface_bits + bmp_stride * (height - lineCount - 1),DIB_scanline,bmp_stride);
				
				// Free mem
				delete[] DIB_scanline;
			
			}  // end of if...else ((int)pcx_header.nPlanes == 1)
		
		} // end of for(int lineCount = 0; lineCount < height; lineCount++)
		

		// Free mem
		delete[] scan_line;

		//delete[] surface_bits;

		fclose(file_pointer);

		m_ciBitmap->SetBitmap(hBitmap,false);

		DeleteObject(hBitmap);
	}
	else
	{
		fclose(file_pointer);
		return ReadPcx2(m_ciBitmap,szFileName);
	}
	return 1; // PCX was successfully loaded

} // end of LoadPCX(const char *file_name, DIB_BITMAP &bitmap, HDC hdc)


int CPcx::ReadPcx2(CIBitmap * m_ciBitmap, const char * szFileName)
{

	CImage m_image;
   // Open the file and put its entire content in memory
   FILE * pFile = NULL;//fopen( szFileName, "rb" );

	errno_t err;
	err  = fopen_s(&pFile, szFileName, "rb");
	if(err != 0)
	{
		return 0;
	}

   const long clFileSize = _filelength(_fileno(pFile));
   BYTE *pabFileData = (BYTE *)new BYTE[ clFileSize ];
   fread( pabFileData, clFileSize, 1, pFile );
   fclose( pFile );

   // Get the header
   memcpy( &sHeader, pabFileData, sizeof(sHeader) );

   // Each scan line MUST have a size that can be divided by a 'long' data type
   int iScanLineSize = sHeader.NumPlanes * sHeader.BPL;
   ldiv_t sDivResult = ldiv( iScanLineSize, sizeof(long) );
   if ( sDivResult.rem > 0 )
      iScanLineSize = (iScanLineSize/sizeof(long)+1) * sizeof(long);

   // Set the bitmap size data member
   int cx = sHeader.X2-sHeader.X1+1;
   int cy = sHeader.Y2-sHeader.Y1+1;
   const long clImageSize = iScanLineSize * cy;

   // Set the bitmap information

   RGBQUAD * pabPalette = new RGBQUAD[ (sizeof(RGBQUAD)*256) ];

   // Prepare a buffer large enough to hold the image
   BYTE * pabRawBitmap = (BYTE *)new BYTE[ clImageSize ];

   BYTE * pabBitmap = (BYTE *)new BYTE[ clImageSize * 4];

   if ( !pabRawBitmap )
   {
      delete [] pabFileData;
      return 0;
   }

   // Get the compressed image
   long lDataPos = 0;
   long lPos = 128;     // That's where the data begins

   for ( int iY=0; iY < cy; iY++ )
   {
	  // Decompress the scan line
	  int iX=0;
	  for (; iX < sHeader.BPL; )
	  {
		 UINT uiValue = pabFileData[lPos++];
		 if ( uiValue > 192 ) {  // Two high bits are set = Repeat
			uiValue -= 192;                  // Repeat how many times?
			BYTE Color = pabFileData[lPos++];  // What color?

			if ( iX <= cx )
			{  // Image data.  Place in the raw bitmap.
			   for ( BYTE bRepeat=0; bRepeat < uiValue; bRepeat++ )
			   {
				  pabRawBitmap[lDataPos++] = Color;
				  iX++;
			   }
			}
			else
			   iX += uiValue; // Outside the image.  Skip.
		 }
		 else
		 {
			if ( iX <= cx )
			   pabRawBitmap[lDataPos++] = uiValue;
			iX++;
		 }
	  }

	  // Pad the rest with zeros
	  if ( iX < iScanLineSize )
	  {
		 for ( ;iX < iScanLineSize; iX++ )
			pabRawBitmap[lDataPos++] = 0;
	  }
   }

	//BITMAPINFO * psBmpInfo = (BITMAPINFO *)new BYTE[ sizeof(BITMAPINFOHEADER)];
	//psBmpInfo->bmiHeader.biSize           = sizeof(BITMAPINFOHEADER);
	//psBmpInfo->bmiHeader.biWidth          = cx;
	//psBmpInfo->bmiHeader.biHeight         = -cy;

	if ( pabFileData[lPos++] == 12 )          // Simple validation
	{

	   //psBmpInfo->bmiHeader.biPlanes         = 1;//sHeader.NumPlanes;
	   //psBmpInfo->bmiHeader.biBitCount       = 32;//sHeader.BitPerPixel;
	   //psBmpInfo->bmiHeader.biCompression    = BI_RGB;
	   //psBmpInfo->bmiHeader.biSizeImage      = 0;
	   //psBmpInfo->bmiHeader.biXPelsPerMeter  = 0;
	   //psBmpInfo->bmiHeader.biYPelsPerMeter  = 0;
	   //psBmpInfo->bmiHeader.biClrUsed        = 0;
	   //psBmpInfo->bmiHeader.biClrImportant   = 0;


		// Get the palette
		for ( short Entry=0; Entry < 256; Entry++ )
		{
			pabPalette[Entry].rgbRed       = pabFileData[lPos++];
			pabPalette[Entry].rgbGreen     = pabFileData[lPos++];
			pabPalette[Entry].rgbBlue      = pabFileData[lPos++];
			pabPalette[Entry].rgbReserved  = 0;
		}

		int k = 0;

		for(int i = 0;i < clImageSize;i++)
		{
			int iValue = pabRawBitmap[i];
			pabBitmap[k++] = pabPalette[iValue].rgbBlue;
			pabBitmap[k++] = pabPalette[iValue].rgbGreen;
			pabBitmap[k++] = pabPalette[iValue].rgbRed;
			pabBitmap[k++] = 0;
		}

		m_image.VertFlipBuf(pabBitmap,cx * 4,cy);
		m_ciBitmap->SetBitmap(pabBitmap,cx,cy);

		/*
		HDC hdc = GetDC(NULL);

		m_ciBitmap->hBitmap = CreateDIBitmap (GetDC(NULL),              
						   &psBmpInfo->bmiHeader,
						   CBM_INIT,
						   pabBitmap,
						   (LPBITMAPINFO)psBmpInfo,
						   DIB_RGB_COLORS) ;

		//DeleteObject(hBitmap);
		DeleteDC(hdc);*/

	}



	delete[] pabFileData;
	//delete[] psBmpInfo;
	delete[] pabRawBitmap;
	delete[] pabPalette;
	delete[] pabBitmap;

	return 0;
}

void CPcx::GetDimensions(const char *fileName, UINT *width, UINT *height)
{
	FILE * file_pointer = NULL;//fopen(fileName,"rb"); // Open the file
	
	errno_t err;
	err  = fopen_s(&file_pointer, fileName, "rb");
	if(err != 0)
	{
		return;
	}

	PCX_HEADER pcx_header = {0}; // Create a zeroed out PCX_HEADER

	
	// Read in the .pcx file header
	if(!fread(&pcx_header,sizeof(PCX_HEADER),1,file_pointer))
	{
		fclose(file_pointer);
		return;
	}

	
	// Okay we have the header -- Let's determine the width, height, and number of channels
	// that are in the .pcx file
	*width = pcx_header.xMax - pcx_header.xMin + 1;
	*height = pcx_header.yMax - pcx_header.yMin + 1; 


	fclose(file_pointer);
}
