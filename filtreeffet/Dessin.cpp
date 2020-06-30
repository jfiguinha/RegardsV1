// Dessin.cpp: implementation of the CDessin class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Dessin.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDessin::CDessin()
{
	m_fRatioValue = 1.0;
}

CDessin::~CDessin()
{

}

void CDessin::DessinerRectangleVide(HDC hdc,const int &iTaille, const RECT &rc, const COLORREF &rgb)
{
	HPEN hpen, hpenOld;
	HBRUSH hbrush, hbrushOld;

	// Create a green pen.
	hpen = CreatePen(PS_SOLID, iTaille, rgb);
	// Create a red brush.
	hbrush = CreateSolidBrush(rgb);

	// Select the new pen and brush, and then draw.
	hpenOld = (HPEN)SelectObject(hdc, hpen);
	hbrushOld = (HBRUSH)SelectObject(hdc, hbrush);

	MoveToEx(hdc,rc.left,rc.top,NULL);
	LineTo(hdc,rc.left,rc.bottom);
	LineTo(hdc,rc.right,rc.bottom);
	LineTo(hdc,rc.right,rc.top);
	LineTo(hdc,rc.left,rc.top);

	// Do not forget to clean up.
	DeleteObject(hpen);
	DeleteObject(hbrush);
}

void CDessin::DrawARectangle(HDC hdc, const RECT &rc, const COLORREF &rgb) 
{
    HPEN hpen, hpenOld;
    HBRUSH hbrush, hbrushOld;

    // Create a green pen.
    hpen = CreatePen(PS_SOLID, 2, rgb);
    // Create a red brush.
    hbrush = CreateSolidBrush(rgb);

    // Select the new pen and brush, and then draw.
    hpenOld = (HPEN)SelectObject(hdc, hpen);
    hbrushOld = (HBRUSH)SelectObject(hdc, hbrush);

    Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);

    // Do not forget to clean up.
    DeleteObject(hpen);
    DeleteObject(hbrush);
}

void CDessin::DessinerCarre(HDC &destDC,const int &iLargeur, const int &iHauteur, const int & iMarge, const int &iPosX, const int &iPosY, const COLORREF &rgb)
{
	RECT rc;

		BYTE temp = GetBValue(rgb);
		temp = GetRValue(rgb);
		temp = GetGValue(rgb);

	rc.left = iPosX;
	rc.top = iPosY;
	rc.right = iPosX + iMarge;
	rc.bottom = iPosY + iHauteur;
		
	CDessin::DrawARectangle(destDC,rc,rgb);

	//MoveToEx(destDC,iPosX,iPosY,NULL);
	rc.left = iPosX;
	rc.top = iPosY;
	rc.right = iPosX + iLargeur;
	rc.bottom = iPosY + iMarge;
	CDessin::DrawARectangle(destDC,rc,rgb);

	//MoveToEx(destDC,iPosX,iPosY,NULL);
	rc.left = iPosX;
	rc.top = iPosY + (iHauteur - iMarge);
	rc.right = iPosX + iLargeur;
	rc.bottom = iPosY + iHauteur;
	CDessin::DrawARectangle(destDC,rc,rgb);

	//MoveToEx(destDC,iPosX,iPosY,NULL);
	rc.left =iPosX + (iLargeur - iMarge);
	rc.top = iPosY;
	rc.right = iPosX + iLargeur;
	rc.bottom = iPosY + iHauteur;
	CDessin::DrawARectangle(destDC,rc,rgb);

}

void CDessin::SetMaxPosition(const RECT &m_rcPicture)
{
	m_rcAffichage = m_rcPicture;
}

void CDessin::SetRatio(const float &m_fValue)
{
	m_fRatioValue = m_fValue;
}
