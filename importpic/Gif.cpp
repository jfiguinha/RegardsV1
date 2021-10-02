// Gif.cpp: implementation of the CGif class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Gif.h"
#include "image.h"


typedef struct 
{
char Signature[6];
short ScreenWidth;
short ScreenHeight;
byte Depth;
byte Background;
byte Reserved;
}GIFHEADER;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGif::CGif()
{
	ColorMap = NULL;
}

CGif::~CGif()
{
}

//////////////////////////////////////////////////////////////////////////
//GIF Reader
//////////////////////////////////////////////////////////////////////////

void CGif::DumpScreen2RGB(BYTE * Buffer,
			   GifRowType *ScreenBuffer,
			   int ScreenWidth, int ScreenHeight)
{

	GifColorType * ColorMapEntry;
	GifRowType GifRow;
	long k = 0;

	//Buffer = new BYTE[ScreenHeight * ScreenWidth * 4];

	for (long i = 0; i < ScreenHeight; i++) 
	{
		GifRow = *ScreenBuffer;ScreenBuffer++;
		GifQprintf((char *)"\b\b\b\b%-4d", ScreenHeight - i);
		for (long j = 0; j < ScreenWidth; j++) 
		{
			ColorMapEntry = &ColorMap->Colors[GifRow[j]];
			*Buffer++ = ColorMapEntry->Blue;
			*Buffer++ = ColorMapEntry->Green;
			*Buffer++ = ColorMapEntry->Red;
			*Buffer++ = 0;
		}
	}

	*Buffer  = '\0';
}

bool CGif::ValidGif(const char * szFileName)
{

	GIFHEADER gifHeader;
	FILE * file_pointer = NULL;

	ZeroMemory(&gifHeader,sizeof(GIFHEADER));

	errno_t err;
	err  = fopen_s(&file_pointer, szFileName, "r");
	if(err != 0)
	{
		return false;
	}

	/* Next should be the "avih" chunk with the main AVI Header. */
	fread(&gifHeader,sizeof(GIFHEADER),1,file_pointer);

	string m_stgSignature = gifHeader.Signature;

	int m_iIndex = m_stgSignature.find("GIF");

	fclose(file_pointer);

	if(m_iIndex != 0)
		return false;

	return true;
}

int CGif::ReadGif(CIBitmap * m_ciBitmap,const char * szFileName)
{
	try
	{
		if(ValidGif(szFileName))
		{
			GifFileType *GifFile;
			GifRecordType RecordType;
			GifByteType *Extension;
			GifRowType *ScreenBuffer;
			int ImageNum = 0, BackGround = 0,  OneFileFlag = FALSE,
			HelpFlag = FALSE, ColorMapSize = 0,
			InterlacedOffset[] = { 0, 4, 2, 1 }, // The way Interlaced image should.
			InterlacedJumps[] = { 8, 8, 4, 2 };    // be read - offsets and jumps... 
			long i, j;

			int CurLine = 0;
			int Row, Col, Width, Height, Count, ExtCode;

			if ((GifFile = DGifOpenFileName(szFileName)) == NULL) 
			   goto error;

			// Allocate the screen as vector of column of rows. We cannt allocate    //
			// the all screen at once, as this broken minded CPU can allocate up to  //
			// 64k at a time and our image can be bigger than that:		     ////
			// Note this screen is device independent - its the screen as defined by //
			// the GIF file parameters itself.		//


			if ((ScreenBuffer = (GifRowType *)malloc(GifFile->SHeight * sizeof(GifRowType *))) == NULL)
				return -1;

			long Size = GifFile->SWidth * sizeof(GifPixelType);// Size in bytes one row.
			if ((ScreenBuffer[0] = (GifRowType) malloc(Size)) == NULL) // First row. 
				return -1;

			for (i = 0; i < GifFile->SWidth; i++)  // Set its color to BackGround. 
				ScreenBuffer[0][i] = GifFile->SBackGroundColor;

			for (i = 1; i < GifFile->SHeight; i++) 
			{
				// Allocate the other rows, and set their color to background too: 
				if ((ScreenBuffer[i] = (GifRowType) malloc(Size)) == NULL)
					goto error;

				memcpy(ScreenBuffer[i], ScreenBuffer[0], Size);
			}

			// Scan the content of the GIF file and load the image(s) in: //
			// Scan the content of the GIF file and load the image(s) in: //
			do {
			if (DGifGetRecordType(GifFile, &RecordType) == GIF_ERROR) {
				goto error;
			}
			switch (RecordType) {
				case IMAGE_DESC_RECORD_TYPE:
				if (DGifGetImageDesc(GifFile) == GIF_ERROR) {
					goto error;
				}
				Row = GifFile->Image.Top; // Image Position relative to Screen. //
				Col = GifFile->Image.Left;
				Width = GifFile->Image.Width;
				Height = GifFile->Image.Height;
				GifQprintf((char*)"\n%s: Image %d at (%d, %d) [%dx%d]:     ",
					"toto", ++ImageNum, Col, Row, Width, Height);
				if (GifFile->Image.Left + GifFile->Image.Width > GifFile->SWidth ||
				   GifFile->Image.Top + GifFile->Image.Height > GifFile->SHeight) {
					//fprintf(stderr, "Image %d is not confined to screen dimension, aborted.\n");
					goto error;
				}
				if (GifFile->Image.Interlace) {
					// Need to perform 4 passes on the images: 
					for (Count = i = 0; i < 4; i++)
					for (j = Row + InterlacedOffset[i]; j < Row + Height;
								 j += InterlacedJumps[i]) {
						GifQprintf((char*)"\b\b\b\b%-4d", Count++);
						if (DGifGetLine(GifFile, &ScreenBuffer[j][Col],
						Width) == GIF_ERROR) {
						//PrintGifError();
						goto error;
						}
					}
				}
				else {
					for (i = 0; i < Height; i++) {
					GifQprintf((char*)"\b\b\b\b%-4d", i);
					if (DGifGetLine(GifFile, &ScreenBuffer[Row++][Col],
						Width) == GIF_ERROR) {
						//PrintGifError();
						goto error;
					}
					}
				}
				break;
				case EXTENSION_RECORD_TYPE:
				// Skip any extension blocks in file: //
				if (DGifGetExtension(GifFile, &ExtCode, &Extension) == GIF_ERROR) {
					//PrintGifError();
					goto error;
				}
				while (Extension != NULL) {
					if (DGifGetExtensionNext(GifFile, &Extension) == GIF_ERROR) 
					{
						goto error;
					}
				}
				break;
				case TERMINATE_RECORD_TYPE:
				break;
				default:		    // Should be traps by DGifGetRecordType. 
				break;
			}
			}
			while (RecordType != TERMINATE_RECORD_TYPE);

			/* Lets dump it - set the global variables required and do it: */

			BackGround = GifFile->SBackGroundColor;
			ColorMap = (GifFile->Image.ColorMap
				? GifFile->Image.ColorMap
				: GifFile->SColorMap);

			UINT nWidth = GifFile->SWidth;
			UINT nHeight = GifFile->SHeight;


			m_ciBitmap->CreateBitmap(nWidth,nHeight);

			BYTE * Buffer = m_ciBitmap->GetPtBitmap();

			GifRowType GifRow;
			long k = 0;

			for (i = nHeight - 1; i >= 0; i--) 
			{
				GifRow = ScreenBuffer[i];

				for (j = 0; j < nWidth; j++) 
				{
					GifColorType ColorMapEntry = ColorMap->Colors[GifRow[j]];
					Buffer[k++] = ColorMapEntry.Blue;
					Buffer[k++] = ColorMapEntry.Green;
					Buffer[k++] = ColorMapEntry.Red;
					Buffer[k++] = 0;
				}
			}
			
			for (i = 0; i < nHeight; i++) 
				free(ScreenBuffer[i]);

			m_ciBitmap->ReCreateHBitmap(false);


			if(DGifCloseFile(GifFile) == GIF_ERROR)
				goto error;
		}
		else
		{
			m_ciBitmap->CreateBitmap(0,0);
		}
	}
	catch(...)
	{
		m_ciBitmap->CreateBitmap(0,0);
	}
	
  return 0;

error:
		m_ciBitmap->CreateBitmap(0,0);
		return -1;

}

void CGif::GetDimensions(const char *fileName, UINT *width, UINT *height)
{
	try
	{


		GIFHEADER gifHeader;
		FILE * file_pointer = NULL;

		ZeroMemory(&gifHeader,sizeof(GIFHEADER));

		errno_t err;
		err  = fopen_s(&file_pointer, fileName, "r");
		if(err != 0)
		{
			return;
		}

		/* Next should be the "avih" chunk with the main AVI Header. */
		fread(&gifHeader,sizeof(GIFHEADER),1,file_pointer);

		string m_stgSignature = gifHeader.Signature;
		int m_iIndex = m_stgSignature.find("GIF");

		if(m_iIndex == 0)
		{
			*width = (UINT)gifHeader.ScreenWidth;
			*height = (UINT)gifHeader.ScreenHeight;
		}

		fclose(file_pointer);

	}
	catch(...)
	{
		*width = 0;
		*height = 0;
	}

}
