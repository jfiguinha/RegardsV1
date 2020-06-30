// Tga.cpp: implementation of the CTga class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tga.h"
#include "image.h"

// Definitions for image types.
#define TGA_Null 0
#define TGA_Map 1
#define TGA_RGB 2
#define TGA_Mono 3
#define TGA_RLEMap 9
#define TGA_RLERGB 10
#define TGA_RLEMono 11
#define TGA_CompMap 32
#define TGA_CompMap4 33

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTga::CTga()
{

}

CTga::~CTga()
{

}

int CTga::TGA(CIBitmap * m_ciBitmap,const char * szFileName)
{
    HANDLE handle = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, 
	                NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
    if ( handle == INVALID_HANDLE_VALUE )
        return NULL;

    TGA_Header header; 

    DWORD dwRead = 0;
    
	BYTE m_bData[18];

	ReadFile(handle, &m_bData, 18, & dwRead, NULL);

	///////////////////////////////////////////////////////////
	//Reading TGA header information
	///////////////////////////////////////////////////////////

	header.IdLength = m_bData[0];
	header.CmapType = m_bData[1];
	header.ImageType = m_bData[2];
	memcpy(&header.CmapIndex,m_bData + 3,2);
	memcpy(&header.CmapLength,m_bData + 5,2);
	header.CmapEntrySize = m_bData[7];
	memcpy(&header.X_Origin,m_bData + 8,2);
	memcpy(&header.Y_Origin,m_bData + 10,2);
	memcpy(&header.ImageWidth,m_bData + 12,2);
	memcpy(&header.ImageHeight,m_bData + 14,2);
	header.PixelDepth = m_bData[16];
	header.ImagDesc = m_bData[17];

	switch(header.ImageType)
	{
		case TGA_Null:
			m_ciBitmap->CreateBitmap(0,0);
			goto end;
			break;

		case TGA_Map:
			goto PALETTE;
			break;

		case TGA_RGB:
			{
				int iTaille = header.PixelDepth >> 3;

				BITMAPINFO bmp = { { sizeof(BITMAPINFOHEADER), header.ImageWidth, header.ImageHeight, 1, header.PixelDepth } };

				void * pBits = NULL;

				HBITMAP hBitmap;

				hBitmap = CreateDIBSection(NULL, & bmp, DIB_RGB_COLORS, & pBits, NULL, NULL);

				if (hBitmap==NULL )
				{
					CloseHandle(handle);
					return NULL;
				}

				ReadFile(handle, pBits, header.ImageWidth * header.ImageHeight * iTaille, & dwRead, NULL);
				
				CloseHandle(handle);

				m_ciBitmap->SetBitmap(hBitmap,false);

				m_ciBitmap->VertFlipBuf();

				DeleteObject(hBitmap);

				goto end;
			}
			break;

		case TGA_Mono:
			goto PALETTE;
			break;

		case TGA_RLEMap:
			goto PALETTE;
			break;

		case TGA_RLERGB:
			goto PALETTE;
			break;

		case TGA_RLEMono:
			goto PALETTE;
			break;

		case TGA_CompMap:
			goto PALETTE;
			break;

		case TGA_CompMap4:
			goto PALETTE;
			break;

		default:
			m_ciBitmap->CreateBitmap(0,0);
			goto end;
			break;
	}

PALETTE:

		//Lecture d'une image TGA avec une palette de couleur

		/* Setup the output structure */
		int width = header.ImageWidth;
		int height = header.ImageHeight;
		int num_bytes = width * height * 2;
		BYTE * data = (BYTE *)malloc(num_bytes);
		BYTE * identification = (BYTE *)malloc(header.IdLength);

		BYTE * rawData = (BYTE *)malloc(num_bytes * 4);

		rgb_color pal[256];

		/* Don't care for the image identification field */
		ReadFile(handle, identification, header.IdLength,  & dwRead, NULL);

		if (header.CmapType != 0)
		{ // read the palette
			
			ReadFile(handle,pal,header.CmapLength*3,  & dwRead, NULL);
		}

		//Transformation de la palette;

		ReadFile(handle, data, num_bytes,  & dwRead, NULL);

		// Read the image 
		int k = 0;
		int i = 0;
		for (int y=0; y< height; y++) 
		{
			for (int x=0; x< width; x++) 
			{
				int iValue = data[i];i++;
				rawData[k++] = pal[iValue].r;
				rawData[k++] = pal[iValue].g;
				rawData[k++] = pal[iValue].b;
				rawData[k++] = 0;
			}
		}


		m_ciBitmap->SetBitmap(rawData,width,height);

		free(data);
		free(identification);
		free(rawData);
	
		/* Close the file */
		CloseHandle(handle);

end:


	return 0;
}

/*
bool CxImageTGA::Decode(CxFile *hFile)
{
	if (hFile == NULL) return false;

	TGAHEADER tgaHead;

  try
  {
	if (hFile->Read(&tgaHead,sizeof(tgaHead),1)==0)
		throw "Not a TGA";

	bool bCompressed;
	switch (tgaHead.ImageType){
	case TGA_Map:
	case TGA_RGB:
	case TGA_Mono:
		bCompressed = false;
		break;
	case TGA_RLEMap:
	case TGA_RLERGB:
	case TGA_RLEMono:
		bCompressed = true;
		break;
	default:
		throw "Unknown TGA image type";
	}

	if (tgaHead.ImageWidth==0 || tgaHead.ImageHeight==0 || tgaHead.PixelDepth==0 || tgaHead.CmapLength>256)
		throw "bad TGA header";

	if (tgaHead.PixelDepth!=8 && tgaHead.PixelDepth!=15 && tgaHead.PixelDepth!=16 && tgaHead.PixelDepth!=24 && tgaHead.PixelDepth!=32)
		throw "bad TGA header";

	if (tgaHead.IdLength>0) hFile->Seek(tgaHead.IdLength,SEEK_CUR); //skip descriptor

	Create(tgaHead.ImageWidth, tgaHead.ImageHeight, tgaHead.PixelDepth, CXIMAGE_FORMAT_TGA);
#if CXIMAGE_SUPPORT_ALPHA	// <vho>
	if (tgaHead.PixelDepth==32) AlphaCreate(); // Image has alpha channel
#endif //CXIMAGE_SUPPORT_ALPHA

	if (!IsValid()) throw "TGA Create failed";
	
	if (info.nEscape) throw "Cancelled"; // <vho> - cancel decoding

	if (tgaHead.CmapType != 0){ // read the palette
		rgb_color pal[256];
		hFile->Read(pal,tgaHead.CmapLength*sizeof(rgb_color), 1);
		for (int i=0;i<tgaHead.CmapLength; i++) SetPaletteColor((BYTE)i,pal[i].b,pal[i].g,pal[i].r);
	}

	if (tgaHead.ImageType == TGA_Mono || tgaHead.ImageType == TGA_RLEMono)
		SetGrayPalette();

	// Bits 4 & 5 of the Image Descriptor byte control the ordering of the pixels.
	bool bXReversed = ((tgaHead.ImagDesc & 16) == 16);
	bool bYReversed = ((tgaHead.ImagDesc & 32) == 32);

    CImageIterator iter(this);
	BYTE rleLeftover = 255; //for images with illegal packet boundary 
	BYTE* pDest;
    for (int y=0; y < tgaHead.ImageHeight; y++){

		if (info.nEscape) throw "Cancelled"; // <vho> - cancel decoding

		if (hFile == NULL || hFile->Eof()) throw "corrupted TGA";

		if (bYReversed) pDest = iter.GetRow(tgaHead.ImageHeight-y-1);
		else pDest = iter.GetRow(y);

		if (bCompressed) rleLeftover = ExpandCompressedLine(pDest,&tgaHead,hFile,tgaHead.ImageWidth,y,rleLeftover);
		else ExpandUncompressedLine  (pDest,&tgaHead,hFile,tgaHead.ImageWidth,y,0);
    }

	if (bXReversed) Mirror();

#if CXIMAGE_SUPPORT_ALPHA
	if (bYReversed && tgaHead.PixelDepth==32) AlphaFlip(); //<lioucr>
#endif //CXIMAGE_SUPPORT_ALPHA

  } catch (char *message) {
	strncpy(info.szLastError,message,255);
	return FALSE;
  }
    return true;
}

BYTE CxImageTGA::ExpandCompressedLine(BYTE* pDest,TGAHEADER* ptgaHead,CxFile *hFile,int width, int y, BYTE rleLeftover)
{
	try{
	BYTE rle;
	long filePos;
	for (int x=0; x<width; ){
		if (rleLeftover != 255){
            rle = rleLeftover;
            rleLeftover = 255;
        } else {
			hFile->Read(&rle,1,1);
		}
		if (rle & 128) { // RLE-Encoded packet
			rle -= 127; // Calculate real repeat count.
			if ((x+rle)>width){
				rleLeftover = 128 + (rle - (width - x) - 1);
                filePos = hFile->Tell();
				rle=width-x;
			}
			switch (ptgaHead->PixelDepth)
			{
			case 32: {
				RGBQUAD color;
				hFile->Read(&color,4,1);
				for (int ix = 0; ix < rle; ix++){
					memcpy(&pDest[3*ix],&color,3);
#if CXIMAGE_SUPPORT_ALPHA	// <vho>
					AlphaSet(ix+x,y,color.rgbReserved);
#endif //CXIMAGE_SUPPORT_ALPHA
				}
				break;
					 } 
			case 24: {
				rgb_color triple;
				hFile->Read(&triple,3,1);
				for (int ix = 0; ix < rle; ix++) memcpy(&pDest[3*ix],&triple,3);
				break;
					 }
			case 15:
			case 16: {
				WORD pixel;
				hFile->Read(&pixel,2,1);
				rgb_color triple;
				triple.r = (BYTE)(( pixel & 0x1F ) * 8);     // red
				triple.g = (BYTE)(( pixel >> 2 ) & 0x0F8);   // green
				triple.b = (BYTE)(( pixel >> 7 ) & 0x0F8);   // blue
				for (int ix = 0; ix < rle; ix++){
					memcpy(&pDest[3*ix],&triple,3);
				}
				break;
					 }
			case 8: {
				BYTE pixel;
				hFile->Read(&pixel,1,1);
				for (int ix = 0; ix < rle; ix++) pDest[ix] = pixel;
					}
			}
			if (rleLeftover!=255) hFile->Seek(filePos, SEEK_SET);
		} else { // Raw packet
			rle += 1; // Calculate real repeat count.
			if ((x+rle)>width){
                rleLeftover = rle - (width - x) - 1;
				rle=width-x;
			}
			ExpandUncompressedLine(pDest,ptgaHead,hFile,rle,y,x);
		}
		if (head.biBitCount == 24)	pDest += rle*3;	else pDest += rle;
		x += rle;
	}
	} catch(...){	}
	return rleLeftover;
}
////////////////////////////////////////////////////////////////////////////////
void CxImageTGA::ExpandUncompressedLine(BYTE* pDest,TGAHEADER* ptgaHead,CxFile *hFile,int width, int y, int xoffset)
{
	try{
	switch (ptgaHead->PixelDepth){
	case 8:
		hFile->Read(pDest,width,1);
		break;
	case 15:
	case 16:{
		BYTE* dst=pDest;
		WORD pixel;
		for (int x=0; x<width; x++){
			hFile->Read(&pixel,2,1);
			*dst++ = (BYTE)(( pixel & 0x1F ) * 8);     // blue
			*dst++ = (BYTE)(( pixel >> 2 ) & 0x0F8);   // green
			*dst++ = (BYTE)(( pixel >> 7 ) & 0x0F8);   // red
		}
		break;
			}
	case 24:
		hFile->Read(pDest,3*width,1);
		break;
	case 32:{
		BYTE* dst=pDest;
		for (int x=0; x<width; x++){
			RGBQUAD pixel;
			hFile->Read(&pixel,4,1);
			*dst++ = pixel.rgbBlue;
			*dst++ = pixel.rgbGreen;
			*dst++ = pixel.rgbRed;
#if CXIMAGE_SUPPORT_ALPHA	// <vho>
			AlphaSet(x+xoffset,y,pixel.rgbReserved); //alpha
#endif //CXIMAGE_SUPPORT_ALPHA
		}
		break;
			}
	}
	} catch(...){	}
}
*/
void CTga::GetDimensions(const char *fileName, UINT *width, UINT *height)
{
    HANDLE handle = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, 
	                NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
    if ( handle == INVALID_HANDLE_VALUE )
        return;

    TGA_Header header;

    DWORD dwRead = 0;
	BYTE m_bData[18];

	ReadFile(handle, &m_bData, 18, & dwRead, NULL);

	///////////////////////////////////////////////////////////
	//Reading TGA header information
	///////////////////////////////////////////////////////////

	header.IdLength = m_bData[0];
	header.CmapType = m_bData[1];
	header.ImageType = m_bData[2];
	memcpy(&header.CmapIndex,m_bData + 3,2);
	memcpy(&header.CmapLength,m_bData + 5,2);
	header.CmapEntrySize = m_bData[7];
	memcpy(&header.X_Origin,m_bData + 8,2);
	memcpy(&header.Y_Origin,m_bData + 10,2);
	memcpy(&header.ImageWidth,m_bData + 12,2);
	memcpy(&header.ImageHeight,m_bData + 14,2);
	header.PixelDepth = m_bData[16];
	header.ImagDesc = m_bData[17];

	*width = header.ImageWidth;
	*height = header.ImageHeight;

    CloseHandle(handle);

}

int CTga::SaveTGA(CIBitmap &m_ciBitmap,const char * szFileName)
{

	int hFile;

	//hFile = _open(szFileName,_O_CREAT | _O_RDWR | _O_BINARY);

	if (_sopen_s(&hFile,szFileName,O_CREAT | _O_RDWR | _O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE)== -1) 
		return false;

	if(hFile == -1)
		return -1;

	TGA_Header tgaHead;

    tgaHead.IdLength = 0;				// Image ID Field Length
    tgaHead.CmapType = 0; // Color Map Type
    tgaHead.ImageType = (m_ciBitmap.GetBitmapDepth() == 8) ? (BYTE)TGA_Map : (BYTE)TGA_RGB; // Image Type

    tgaHead.CmapIndex=0;				// First Entry Index
    tgaHead.CmapLength=(m_ciBitmap.GetBitmapDepth() == 8) ? (BYTE)255 : (BYTE)0;   // Color Map Length
    tgaHead.CmapEntrySize=(m_ciBitmap.GetBitmapDepth() == 8) ? (BYTE)24 : (BYTE)0; // Color Map Entry Size

    tgaHead.X_Origin=0;					// X-origin of Image
    tgaHead.Y_Origin=0;					// Y-origin of Image
    tgaHead.ImageWidth=(WORD)m_ciBitmap.GetBitmapWidth();		// Image Width
    tgaHead.ImageHeight=(WORD)m_ciBitmap.GetBitmapHeigth();	// Image Height
    tgaHead.PixelDepth=(BYTE)m_ciBitmap.GetBitmapDepth();	// Pixel Depth
    tgaHead.ImagDesc=0;					// Image Descriptor

	tgaHead.PixelDepth=32; 

	BYTE m_bData[18];

	///////////////////////////////////////////////////////////
	//Reading TGA header information
	///////////////////////////////////////////////////////////

	m_bData[0] = 0;
	m_bData[1] = 0;
	m_bData[2] = TGA_RGB;

	memcpy(m_bData + 3,&tgaHead.CmapIndex,2);
	memcpy(m_bData + 5,&tgaHead.CmapLength,2);
	m_bData[7] = tgaHead.CmapEntrySize;
	memcpy(m_bData + 8,&tgaHead.X_Origin,2);
	memcpy(m_bData + 10,&tgaHead.Y_Origin,2);
	memcpy(m_bData + 12,&tgaHead.ImageWidth,2);
	memcpy(m_bData + 14,&tgaHead.ImageHeight,2);
	m_bData[16] = tgaHead.PixelDepth;
	m_bData[17] = tgaHead.ImagDesc;

	_write(hFile,m_bData,18);



	BYTE* pDest = (BYTE*)malloc(4*tgaHead.ImageWidth);
	COLORREF c;
	for (int y=0; y < tgaHead.ImageHeight; y++){
		for(int x=0, x4=0;x<tgaHead.ImageWidth;x++, x4+=4)
		{
			m_ciBitmap.GetColorValue(x,y,c);
			pDest[x4+0]=GetRValue(c);
			pDest[x4+1]=GetGValue(c);
			pDest[x4+2]=GetBValue(c);
			pDest[x4+3]=0;
		}
		//hFile->Write(pDest,4*tgaHead.ImageWidth,1);
		_write(hFile,pDest,4*tgaHead.ImageWidth);
	}
	free(pDest);

	_close(hFile);

	return true;
}
