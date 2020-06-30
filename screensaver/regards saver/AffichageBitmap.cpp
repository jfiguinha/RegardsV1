// AffichageBitmap.cpp: implementation of the CAffichageBitmap class.
//
//////////////////////////////////////////////////////////////////////

#include "AffichageBitmap.h"
#include "effet.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAffichageBitmap::CAffichageBitmap()
{

}

CAffichageBitmap::~CAffichageBitmap()
{

}

void CAffichageBitmap::AffichageBitmap(HWND hWnd,HBITMAP hBitmap,const char * szFileName)
{
	//Affichage des images
	HDC hdc;
	RECT rc;

	GetClientRect(hWnd, &rc);
	rc.right = GetSystemMetrics(SM_CXSCREEN);

	int xValue = 0, yValue = 0;
	int left, top;

	BITMAP bm;
	GetObject (hBitmap, sizeof (BITMAP), &bm);

	hdc = GetDC(hWnd);

	float fLocalratio;

	if(bm.bmWidth > bm.bmHeight)
		fLocalratio = (float)rc.right / (float)(bm.bmWidth);
	else
		fLocalratio = (float)rc.bottom / (float)(bm.bmHeight);

	nTailleAffichageWidth = bm.bmWidth * fLocalratio;
	nTailleAffichageHeight = bm.bmHeight * fLocalratio;

	if(nTailleAffichageHeight > rc.bottom)
	{
		fLocalratio = (float)rc.bottom / (float)(bm.bmHeight);
		nTailleAffichageWidth = bm.bmWidth * fLocalratio;
		nTailleAffichageHeight = bm.bmHeight * fLocalratio;
	}
	else
	{
		if(nTailleAffichageWidth > rc.right)
		{
			fLocalratio = (float)rc.right / (float)(bm.bmWidth);
			nTailleAffichageWidth = bm.bmWidth * fLocalratio;
			nTailleAffichageHeight = bm.bmHeight * fLocalratio;
		}
	}

	DeterminePos(rc,left,top);

	ShowBitmap(hdc, left, top, xValue, yValue, hBitmap);

	CompleteBitmap(hdc, rc, left, top, xValue, yValue);

	SetTextColor(GetDC(hWnd), RGB(73,225,134));

	SetBkMode(GetDC(hWnd), TRANSPARENT);

	DrawText(GetDC(hWnd),szFileName,strlen(szFileName),&rc,DT_TOP | DT_LEFT);

	DeleteObject(hBitmap);

	ReleaseDC(hWnd,hdc);
}

void CAffichageBitmap::CompleteBitmap(HDC hdc, RECT rc, int left, int top, int xValue, int yValue)
{

	HBRUSH hBrush;
	RECT rc2;
	RECT rc3;

	hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);

	
	rc2 = rc;
	
	rc3 = rc;
	rc.left = nTailleAffichageWidth - xValue + left;
	
	FillRect(hdc, &rc, hBrush); 
	rc.left = 0;
	rc.top = nTailleAffichageHeight - yValue + top;
	FillRect(hdc, &rc, hBrush); 

	if(left > 0)
	{
		rc2.left = 0;
		rc2.right = left;
		rc2.bottom = rc.bottom;
		FillRect(hdc, &rc2, hBrush); 
	}

	if(top > 0)
	{
		rc3.top = 0;
		rc3.bottom = top;
		rc3.right = rc.right;
		FillRect(hdc, &rc3, hBrush); 
	}
}

void CAffichageBitmap::ShowBitmap(HDC hdc, int left,int top, int xValue, int yValue, HBITMAP hBitmap)
{
	CEffet m_CEffet;
	HBITMAP hBitmapTemp;
	hBitmapTemp = m_CEffet.ScaleBitmap(hBitmap, nTailleAffichageWidth, nTailleAffichageHeight, true);

	HDC hdcMem = CreateCompatibleDC(hdc); 

	SelectObject (hdcMem, hBitmapTemp) ;

	BitBlt(hdc,left,top, nTailleAffichageWidth, nTailleAffichageHeight, hdcMem, 0, 0,SRCCOPY); 

	DeleteDC (hdcMem);

	DeleteObject(hBitmapTemp);

}

void CAffichageBitmap::DeterminePos(RECT rc, int &left, int &top)
{
	if(rc.right > nTailleAffichageWidth)
		left = ((rc.right - nTailleAffichageWidth) / 2);
	else
	{
		left = 0;
	}

	if(rc.bottom > nTailleAffichageHeight)
	{
		top = ((rc.bottom - nTailleAffichageHeight) / 2);
	}
	else
	{
		top = 0;
	}

}