#include "stdafx.h"
#include "BitmapPrv.h"

using namespace Regards::libbitmap;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//AJout des bordures au bitmap pour garder le rapport et le mettre en plein cadre
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CBitmapPrv::CompleteBitmap(HDC &hdc, RECT &rc, int &left, int &top, int &xValue, int &yValue)
{

	HBRUSH hBrush;

	if(m_BitmapData.m_bPrinter)
		hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
	else
		hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);

	RECT rc2 = rc;
	RECT rc3 = rc;
	rc.left = m_BitmapData.nTailleAffichageWidth - xValue + left;
	
	FillRect(hdc, &rc, hBrush); 
	rc.left = 0;
	rc.top = m_BitmapData.nTailleAffichageHeight - yValue + top;
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

	DeleteObject(hBrush);

}


////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void CBitmapPrv::TestMaxX(const long &m_lBitmapWidth, const long &right)
{
	long xValue = m_lBitmapWidth - right;

	if(m_BitmapData.nlPosLargeur >= xValue)
		m_BitmapData.nlPosLargeur = xValue;

	if(m_BitmapData.nlPosLargeur < 0)
		m_BitmapData.nlPosLargeur = 0;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void CBitmapPrv::TestMaxY(const long &m_lBitmapHeight, const long &bottom)
{
	long yValue = m_lBitmapHeight - bottom;

	if(m_BitmapData.nlPosHauteur >= yValue)
		m_BitmapData.nlPosHauteur = yValue;

	if(m_BitmapData.nlPosHauteur < 0)
		m_BitmapData.nlPosHauteur = 0;
}