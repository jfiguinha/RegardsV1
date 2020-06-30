// Bmp.cpp: implementation of the CBmp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Bmp.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBmp::CBmp()
{

}

CBmp::~CBmp()
{

}

int CBmp::ReadBmp(CIBitmap * m_ciBitmap, const char *szFileName)
{
	 //return ReadBmp2(m_ciBitmap, szFileName);
	 BOOL               bSuccess ;
	 DWORD              dwFileSize, dwHighSize, dwBytesRead ;
	 HANDLE             hFile ;
	 BITMAPFILEHEADER * pbmfh;

	 hFile = CreateFile (szFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
						 OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL) ;

	 if (hFile == INVALID_HANDLE_VALUE)
		  return NULL ;

	 dwFileSize = GetFileSize (hFile, &dwHighSize) ;

	 if (dwHighSize)
	 {
		  CloseHandle (hFile) ;
		  return NULL ;
	 }

	 pbmfh = (BITMAPFILEHEADER * )malloc (dwFileSize) ;

	 if (!pbmfh)
	 {
		  CloseHandle (hFile) ;
		  return NULL ;
	 }

	 bSuccess = ReadFile (hFile, pbmfh, dwFileSize, &dwBytesRead, NULL) ;
	 
	 CloseHandle (hFile) ;

	 if (!bSuccess || (dwBytesRead != dwFileSize)         
				   || (pbmfh->bfType != * (WORD *) "BM") 
				   || (pbmfh->bfSize != dwFileSize))
	 {
		  
		  free (pbmfh) ;
		  ReadBmp2(m_ciBitmap, szFileName);
		  return NULL ;
	 }
	 else if(pbmfh->bfOffBits != NULL)
	 {
		 try
		 {
			HBITMAP hBitmap;
			
			hBitmap = CreateDIBitmap (GetDC(NULL),              
								   (BITMAPINFOHEADER *) (pbmfh + 1),
								   CBM_INIT,
								   (BYTE *) pbmfh + pbmfh->bfOffBits,
								   (BITMAPINFO *) (pbmfh + 1),
								   DIB_RGB_COLORS) ;

			m_ciBitmap->SetBitmap(hBitmap,false);

			DeleteObject(hBitmap);

			free(pbmfh);
		 }
		 catch(...)
		 {
		 }
	 }
	 return 0;

}

int CBmp::ReadBmp2(CIBitmap * m_ciBitmap, const char *szFileName)
{
	HBITMAP hBitmap;

	hBitmap = (HBITMAP)::LoadImage(
					NULL,
					szFileName,
					IMAGE_BITMAP,
					0,
					0,
					LR_LOADFROMFILE|LR_CREATEDIBSECTION);

	m_ciBitmap->SetBitmap(hBitmap,false);

	DeleteObject(hBitmap);

	 return 0;
}

/*
int CBmp::SaveBMP(CIBitmap &m_ciBitmap, LPSTR filename)
{
    // BITMAP
    BITMAP bi;
    memset(&bi,0,sizeof(bi));
    if(GetObject(m_ciBitmap.hBitmap,sizeof(bi),&bi) == 0)return FALSE;

    LONG filesize = bi.bmWidthBytes * bi.bmHeight + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    LONG bpp = 0;
    switch(bi.bmBitsPixel)
	{
    case 2:
        filesize += (sizeof(RGBQUAD) * 2);
        bpp = 2;
        break;
    case 4:
        filesize += (sizeof(RGBQUAD) * 16);
        bpp = 16;
        break;
    case 8:
        filesize += (sizeof(RGBQUAD) * 256);
        bpp = 256;
        break;
    }

    HFILE fp = _lcreat(filename,0);
    if(fp == HFILE_ERROR)return FALSE;

    BITMAPFILEHEADER fh;
    memset(&fh,0,sizeof(fh));
    memcpy(&fh.bfType,"BM",2);

    fh.bfSize = filesize;
    fh.bfReserved1 = 0;
    fh.bfReserved2 = 0;
    fh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * bpp;
    _lwrite(fp,(LPSTR)&fh,sizeof(fh));

    LPBITMAPINFO pbi = (LPBITMAPINFO)malloc(sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * bpp);
    memset(pbi,0,sizeof(BITMAPINFOHEADER));
    pbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbi->bmiHeader.biWidth = bi.bmWidth;
    pbi->bmiHeader.biHeight = bi.bmHeight;
    pbi->bmiHeader.biPlanes = 1;
    pbi->bmiHeader.biBitCount = bi.bmBitsPixel;
    pbi->bmiHeader.biCompression = BI_RGB;
    if(bpp != 0)
	{
        GetDIBColorTable(GetDC(NULL),0,bpp,pbi->bmiColors);
    }
    _lwrite(fp,(LPSTR)pbi,sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * bpp);

    //LPBYTE bits = (LPBYTE)malloc(bi.bmWidthBytes * bi.bmHeight);

    HDC dc = GetDC(0);
    HDC mdc = CreateCompatibleDC(dc);
    ReleaseDC(0,dc);

    //int l = GetDIBits(mdc,m_ciBitmap.hBitmap,0,bi.bmHeight,bits,pbi,DIB_RGB_COLORS);

	LPBYTE bits = m_ciBitmap.GetPtBitmap();
    _lwrite(fp,(char *)bits,bi.bmWidthBytes * bi.bmHeight);

    free(pbi);
    //free(bits);
    _lclose(fp);

	return 0;

}*/

int CBmp::SaveBMP(CIBitmap &m_ciBitmap, const char * filename)
{
    BITMAPINFOHEADER bmpInfoHeader = m_ciBitmap.bmiInfos.bmiHeader;
	/*
    // Set the size
    bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    // Bit count
    bmpInfoHeader.biBitCount = m_ciBitmap.bmiInfos.bmiHeader;
    // Use all colors
    bmpInfoHeader.biClrImportant = 0;
    // Use as many colors according to bits per pixel
    bmpInfoHeader.biClrUsed = 0;
    // Store as un Compressed
    bmpInfoHeader.biCompression = BI_RGB;
    // Set the height in pixels
    bmpInfoHeader.biHeight = m_ciBitmap.GetBitmapHeigth();
    // Width of the Image in pixels
	bmpInfoHeader.biWidth = m_ciBitmap.GetBitmapWidth();
    // Default number of planes
    bmpInfoHeader.biPlanes = 1;
    // Calculate the image size in bytes
    bmpInfoHeader.biSizeImage = m_ciBitmap.GetBitmapWidth()* m_ciBitmap.GetBitmapHeigth() * (wBitsPerPixel/8); 
*/
    BITMAPFILEHEADER bfh = {0};
    // This value should be values of BM letters i.e 0×4D42
    // 0×4D = M 0×42 = B storing in reverse order to match with endian
    bfh.bfType=0x4D42;
    /* or
    bfh.bfType = ‘B’+(’M’ << 8);
    // <<8 used to shift ‘M’ to end
    */
    // Offset to the RGBQUAD
    bfh.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
    // Total size of image including size of headers
    bfh.bfSize = bfh.bfOffBits + bmpInfoHeader.biSizeImage;
    // Create the file in disk to write
    HANDLE hFile = CreateFile( filename,GENERIC_WRITE, 0,NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL); 

    if( !hFile ) // return if error opening file
    {
        return -1;
    } 

    DWORD dwWritten = 0;
    // Write the File header
    WriteFile( hFile, &bfh, sizeof(bfh), &dwWritten , NULL );
    // Write the bitmap info header
    WriteFile( hFile, &bmpInfoHeader, sizeof(bmpInfoHeader), &dwWritten, NULL );
    // Write the RGB Data
    WriteFile( hFile, m_ciBitmap.GetPtBitmap(), bmpInfoHeader.biSizeImage, &dwWritten, NULL );
    // Close the file handle
    CloseHandle( hFile );

	return 0;
}

void CBmp::GetJDimensions(const char *fileName, UINT *width, UINT *height)
{
     BITMAPFILEHEADER bmfh ;
     BITMAPINFO     * pbmi ;
     BOOL             bSuccess ;
     DWORD            dwInfoSize, dwBytesRead ;
     HANDLE           hFile ;

          // Ouvre le fichier: accès en lecture, interdit l'accès en écriture

     hFile = CreateFile (fileName, GENERIC_READ, FILE_SHARE_READ, NULL, 
                         OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL) ;

     if (hFile == INVALID_HANDLE_VALUE)
          return;

          // Lecture de BITMAPFILEHEADER

     bSuccess = ReadFile (hFile, &bmfh, sizeof (BITMAPFILEHEADER), 
                          &dwBytesRead, NULL) ;

     if (!bSuccess || (dwBytesRead != sizeof (BITMAPFILEHEADER))         
                   || (bmfh.bfType != * (WORD *) "BM"))
     {
          CloseHandle (hFile) ;
          return;
     }
          // Alloue mémoire pour la structure d'informations & lit la structure

     dwInfoSize = bmfh.bfOffBits - sizeof (BITMAPFILEHEADER) ;

     if (NULL == (pbmi = (BITMAPINFO *)malloc (dwInfoSize)))
     {
          CloseHandle (hFile) ;
          return;
     }

     bSuccess = ReadFile (hFile, pbmi, dwInfoSize, &dwBytesRead, NULL) ;


	 *width = pbmi->bmiHeader.biWidth;
	 *height = pbmi->bmiHeader.biHeight;


	CloseHandle (hFile) ;
	free (pbmi) ;
}
