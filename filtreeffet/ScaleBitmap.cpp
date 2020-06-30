#include "StdAfx.h"
#include "ScaleBitmap.h"


CScaleBitmap::CScaleBitmap(void)
{
}

CScaleBitmap::~CScaleBitmap(void)
{
}

///////////////////////////////////////////////////////////
// Main resize function

HBITMAP CScaleBitmap::ScaleBitmap(HBITMAP hBmp, WORD wNewWidth, WORD wNewHeight)
{

	return CopyScreenToBitmap(hBmp, wNewWidth, wNewHeight);

 BITMAP bmp;
 ::GetObject(hBmp, sizeof(BITMAP), &bmp);

 // check for valid size
 if ((bmp.bmWidth > wNewWidth && bmp.bmHeight < wNewHeight) || (bmp.bmWidth < wNewWidth && bmp.bmHeight > wNewHeight))
	return NULL;

 HDC hDC = ::GetDC(NULL);
 BITMAPINFO *pbi = PrepareRGBBitmapInfo((WORD)bmp.bmWidth,
                                        (WORD)bmp.bmHeight);

 BYTE *pData = new BYTE[pbi->bmiHeader.biSizeImage];
 ::GetDIBits(hDC, hBmp, 0, bmp.bmHeight, pData, pbi, DIB_RGB_COLORS);

 delete pbi;
 pbi = PrepareRGBBitmapInfo(wNewWidth, wNewHeight);
 BYTE *pData2 = new BYTE[pbi->bmiHeader.biSizeImage];

 if(bmp.bmWidth >= wNewWidth && bmp.bmHeight >= wNewHeight)
	ShrinkData(pData, (WORD)bmp.bmWidth, (WORD)bmp.bmHeight, pData2, wNewWidth, wNewHeight);
 else
	EnlargeData(pData, (WORD)bmp.bmWidth, (WORD)bmp.bmHeight, pData2, wNewWidth, wNewHeight);

 delete[] pData;

 HBITMAP hResBmp = ::CreateCompatibleBitmap(hDC,wNewWidth, wNewHeight);
 ::SetDIBits(hDC, hResBmp, 0, wNewHeight, pData2, pbi, DIB_RGB_COLORS);
 ::ReleaseDC(NULL, hDC);

 delete pbi;
 delete pData2;

 return hResBmp;
}

///////////////////////////////////////////////////////////

BITMAPINFO * CScaleBitmap::PrepareRGBBitmapInfo(WORD wWidth, WORD wHeight)
{
 BITMAPINFO *pRes = new BITMAPINFO;
 ::ZeroMemory(pRes, sizeof(BITMAPINFO));
 pRes->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
 pRes->bmiHeader.biWidth = wWidth;
 pRes->bmiHeader.biHeight = wHeight;
 pRes->bmiHeader.biPlanes = 1;
 pRes->bmiHeader.biBitCount = 24;
 pRes->bmiHeader.biSizeImage = ((3 * wWidth + 3) & ~3) * wHeight;
 //pRes->bmiHeader.biSizeImage = ((4 * wWidth + 3) & ~3) * wHeight;

 return pRes;
}

///////////////////////////////////////////////////////////

float * CScaleBitmap::CreateCoeff(int nLen, int nNewLen, BOOL bShrink)
{
	int nSum = 0, nSum2;
	float *pRes = new float[2 * nLen];
	float *pCoeff = pRes;
	float fNorm = (bShrink)? (float)nNewLen / nLen : 1;
	int	  nDenom = (bShrink)? nLen : nNewLen;

	::ZeroMemory(pRes, 2 * nLen * sizeof(float));
	for(int i = 0; i < nLen; i++, pCoeff += 2)
	{
		nSum2 = nSum + nNewLen;
		if(nSum2 > nLen)
		{
			*pCoeff = (float)(nLen - nSum) / nDenom;
			pCoeff[1] = (float)(nSum2 - nLen) / nDenom;
			nSum2 -= nLen;
		}
		else
		{
			*pCoeff = fNorm;
			if(nSum2 == nLen)
			{
				pCoeff[1] = -1;
				nSum2 = 0;
			}
		}
		nSum = nSum2;
	}

	return pRes;
}

///////////////////////////////////////////////////////////

#define F_DELTA		0.0001f

void CScaleBitmap::ShrinkData(BYTE *pInBuff,
                WORD wWidth,
                WORD wHeight,
                BYTE *pOutBuff,
                WORD wNewWidth,
                WORD wNewHeight)
{
 BYTE  *pLine = pInBuff, *pPix;
 BYTE  *pOutLine = pOutBuff;
 DWORD dwInLn = (3 * wWidth + 3) & ~3;
 DWORD dwOutLn = (3 * wNewWidth + 3) & ~3;
 int   x, y, i, ii;
 BOOL  bCrossRow, bCrossCol;
 float *pRowCoeff = CreateCoeff(wWidth, wNewWidth, TRUE);
 float *pColCoeff = CreateCoeff(wHeight, wNewHeight, TRUE);
 float fTmp, *pXCoeff, *pYCoeff = pColCoeff;
 DWORD dwBuffLn = 3 * wNewWidth * sizeof(float);
 float *fBuff = new float[6 * wNewWidth];
 float *fCurrLn = fBuff,
       *fCurrPix,
       *fNextLn = fBuff + 3 * wNewWidth,
       *fNextPix;

 ::ZeroMemory(fBuff, 2 * dwBuffLn);

 y = 0;
 while(y < wNewHeight)
 {
  pPix = pLine;
  pLine += dwInLn;

  fCurrPix = fCurrLn;
  fNextPix = fNextLn;

  x = 0;
  pXCoeff = pRowCoeff;
  bCrossRow = pYCoeff[1] > F_DELTA;
  while(x < wNewWidth)
  {
   fTmp = *pXCoeff * *pYCoeff;
   for(i = 0; i < 3; i++)
   fCurrPix[i] += fTmp * pPix[i];
   bCrossCol = pXCoeff[1] > F_DELTA;
   if(bCrossCol)
   {
    fTmp = pXCoeff[1] * *pYCoeff;
    for(i = 0, ii = 3; i < 3; i++, ii++)
     fCurrPix[ii] += fTmp * pPix[i];
   }

   if(bCrossRow)
   {
    fTmp = *pXCoeff * pYCoeff[1];
    for(i = 0; i < 3; i++)
     fNextPix[i] += fTmp * pPix[i];
    if(bCrossCol)
    {
     fTmp = pXCoeff[1] * pYCoeff[1];
     for(i = 0, ii = 3; i < 3; i++, ii++)
      fNextPix[ii] += fTmp * pPix[i];
    }
   }

  if(fabs(pXCoeff[1]) > F_DELTA)
  {
   x++;
   fCurrPix += 3;
   fNextPix += 3;
  }

  pXCoeff += 2;
  pPix += 3;
 }

 if(fabs(pYCoeff[1]) > F_DELTA)
 {
  // set result line
  fCurrPix = fCurrLn;
  pPix = pOutLine;
  for(i = 3 * wNewWidth; i > 0; i--, fCurrPix++, pPix++)
  *pPix = (BYTE)*fCurrPix;

  // prepare line buffers
  fCurrPix = fNextLn;
  fNextLn = fCurrLn;
  fCurrLn = fCurrPix;
  ::ZeroMemory(fNextLn, dwBuffLn);

  y++;
  pOutLine += dwOutLn;
  }
  pYCoeff += 2;
 }

 delete [] pRowCoeff;
 delete [] pColCoeff;
 delete [] fBuff;
}

///////////////////////////////////////////////////////////

void CScaleBitmap::EnlargeData(BYTE *pInBuff,
                 WORD wWidth,
                 WORD wHeight,
                 BYTE *pOutBuff,
                 WORD wNewWidth,
                 WORD wNewHeight)
{
 BYTE  *pLine = pInBuff,
       *pPix = pLine,
       *pPixOld,
       *pUpPix,
       *pUpPixOld;
 BYTE  *pOutLine = pOutBuff, *pOutPix;
 DWORD dwInLn = (3 * wWidth + 3) & ~3;
 DWORD dwOutLn = (3 * wNewWidth + 3) & ~3;
 int   x, y, i;
 BOOL  bCrossRow, bCrossCol;
 float *pRowCoeff = CreateCoeff(wNewWidth, wWidth, FALSE);
 float *pColCoeff = CreateCoeff(wNewHeight, wHeight, FALSE);
 float fTmp, fPtTmp[3], *pXCoeff, *pYCoeff = pColCoeff;

 y = 0;
 while(y < wHeight)
 {
  bCrossRow = pYCoeff[1] > F_DELTA;
  x = 0;
  pXCoeff = pRowCoeff;
  pOutPix = pOutLine;
  pOutLine += dwOutLn;
  pUpPix = pLine;
  if(fabs(pYCoeff[1]) > F_DELTA)
  {
   y++;
   pLine += dwInLn;
   pPix = pLine;
  }

  while(x < wWidth)
  {
   bCrossCol = pXCoeff[1] > F_DELTA;
   pUpPixOld = pUpPix;
   pPixOld = pPix;
   if(fabs(pXCoeff[1]) > F_DELTA)
   {
    x++;
    pUpPix += 3;
    pPix += 3;
   }
   fTmp = *pXCoeff * *pYCoeff;
   for(i = 0; i < 3; i++)
    fPtTmp[i] = fTmp * pUpPixOld[i];
   if(bCrossCol)
   {
    fTmp = pXCoeff[1] * *pYCoeff;
    for(i = 0; i < 3; i++)
    fPtTmp[i] += fTmp * pUpPix[i];
   }
   if(bCrossRow)
   {
    fTmp = *pXCoeff * pYCoeff[1];
    for(i = 0; i < 3; i++)
    fPtTmp[i] += fTmp * pPixOld[i];
    if(bCrossCol)
    {
     fTmp = pXCoeff[1] * pYCoeff[1];
     for(i = 0; i < 3; i++)
      fPtTmp[i] += fTmp * pPix[i];
    }
   }
   for(i = 0; i < 3; i++, pOutPix++)
    *pOutPix = (BYTE)fPtTmp[i];
   pXCoeff += 2;
  }
  pYCoeff += 2;
 }

 delete [] pRowCoeff;
 delete [] pColCoeff;
}

/************************************************************************* 
 * 
 * CopyScreenToBitmap() 
 * 
 * Parameter: 
 * 
 * LPRECT lpRect    - specifies the window 
 * 
 * Return Value: 
 * 
 * HDIB             - identifies the device-dependent bitmap 
 * 
 * Description: 
 * 
 * This function copies the specified part of the screen to a device- 
 * dependent bitmap. 
 * 
 * 
 ************************************************************************/ 
 
HBITMAP CScaleBitmap::CopyScreenToBitmap(HBITMAP hBitmap, int iWidth, int iHeight) 
{ 
    HDC         hScrDC, hMemDC;         // screen DC and memory DC 
	HBITMAP hOldBitmap;
	bool bSuccess;
	BITMAP bmp;
	::GetObject(hBitmap, sizeof(BITMAP), &bmp);
	
 
    // create a DC for the screen and create 
    // a memory DC compatible to screen DC 

	hOldBitmap = ::CreateCompatibleBitmap(GetDC(NULL),iWidth, iHeight);
     
    hScrDC = CreateCompatibleDC (GetDC(NULL));
    hMemDC = CreateCompatibleDC (GetDC(NULL));
 
    // select new bitmap into memory DC 
    SelectObject(hScrDC, hBitmap); 
	SelectObject(hMemDC, hOldBitmap); 
 
	SetStretchBltMode (hMemDC, COLORONCOLOR); 

    // bitblt screen DC to memory DC 
    //BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY); 
    bSuccess = StretchBlt(hMemDC, 0,  0,  
            iWidth, 
            iHeight, hScrDC, 0,  
            0,  bmp.bmWidth, 
			bmp.bmHeight, SRCCOPY); 

 
    // select old bitmap back into memory DC and get handle to 
    // bitmap of the screen 
     
    //hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap); 
	//(HBITMAP)SelectObject(hMemDC, hBitmap); 
 
    // clean up 
 
    DeleteDC(hScrDC); 
    DeleteDC(hMemDC); 
 
    // return handle to the bitmap 

	::GetObject(hOldBitmap, sizeof(BITMAP), &bmp);
 
    return hOldBitmap;//(HBITMAP)CopyImage(hOldBitmap, IMAGE_BITMAP, 0, 0, 0);; 
} 