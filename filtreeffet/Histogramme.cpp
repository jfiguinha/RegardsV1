#include "StdAfx.h"
#include "Histogramme.h"
#include "ibitmap.h"

CHistogramme::CHistogramme(void)
{
	m_ciBitmap = NULL;
	m_lMaxMoyenneValue = 0;
	m_lMaxRedValue = 0;
	m_lMaxGreenValue = 0;
	m_lMaxBlueValue = 0;
}

CHistogramme::~CHistogramme(void)
{
}


void CHistogramme::ParcoursBitmap()
{
	memset(&m_lMoyenneValue,0,256*sizeof(long));
	memset(&m_lRedValue,0,256*sizeof(long));
	memset(&m_lGreenValue,0,256*sizeof(long));
	memset(&m_lBlueValue,0,256*sizeof(long));
	if(m_ciBitmap != NULL)
	{
		for(int y = 0;y < m_ciBitmap->GetBitmapHeigth();y++)
		{
			for(int x = 0;x < m_ciBitmap->GetBitmapWidth();x++)
			{
				COLORREF m_color;
				m_ciBitmap->GetRGBColorValue(x, y, m_color);
				int Moyenne = (GetRValue(m_color) + GetGValue(m_color) + GetBValue(m_color)) / 3;
				m_lMoyenneValue[Moyenne] = m_lMoyenneValue[Moyenne] + 1;
				if(m_lMoyenneValue[Moyenne] > m_lMaxMoyenneValue)
					m_lMaxMoyenneValue = m_lMoyenneValue[Moyenne];

				m_lRedValue[GetRValue(m_color)] = m_lRedValue[GetRValue(m_color)] + 1;
				if(m_lRedValue[GetRValue(m_color)] > m_lMaxRedValue)
					m_lMaxRedValue = m_lRedValue[GetRValue(m_color)];

				m_lGreenValue[GetGValue(m_color)] = m_lGreenValue[GetGValue(m_color)] + 1;
				if(m_lGreenValue[GetGValue(m_color)] > m_lMaxGreenValue)
					m_lMaxGreenValue = m_lGreenValue[GetGValue(m_color)];

				m_lBlueValue[GetBValue(m_color)] = m_lBlueValue[GetBValue(m_color)] + 1;
				if(m_lBlueValue[GetBValue(m_color)] > m_lMaxBlueValue)
					m_lMaxBlueValue = m_lBlueValue[GetBValue(m_color)];
			}
		}
	}
}

int CHistogramme::GetValue(int iColor, int iYMax, int iPos)
{
	int m_iYSize = 0;
	switch(iColor)
	{
		case 0:
			{
				if(m_lMoyenneValue[iPos] != 0)
					m_iYSize = (m_lMoyenneValue[iPos] * iYMax) / m_lMaxMoyenneValue;
			}
			break;

		case 1:
			{
				if(m_lRedValue[iPos] != 0)
					m_iYSize = (m_lRedValue[iPos] * iYMax) / m_lMaxRedValue;
			}
			break;

		case 2:
			{
				if(m_lGreenValue[iPos] != 0)
					m_iYSize = (m_lGreenValue[iPos] * iYMax) / m_lMaxGreenValue;
			}
			break;

		case 3:
			{
				if(m_lBlueValue[iPos] != 0)
					m_iYSize = (m_lBlueValue[iPos] * iYMax) / m_lMaxBlueValue;
			}
			break;
	}

	return (iYMax - m_iYSize) + 5;
}

HBITMAP CHistogramme::GenerateHistogrammeBitmap(int iWidth, int iHeight, int iColor, COLORREF colorFond)
{
	int m_iMarge = 5;

	HDC destDC;
	destDC = CreateCompatibleDC( GetDC(NULL) );
	HBITMAP hbmResult = CreateCompatibleBitmap(GetDC(NULL), iWidth, iHeight);
	SelectObject( destDC, hbmResult );
	RECT rc;
	rc.top = 0;
	rc.bottom = iHeight;
	rc.left = 0;
	rc.right = iWidth;

	//SetBkColor(hDC, RGB(255,255,255));
	SetStretchBltMode(destDC,COLORONCOLOR);
	FillRect(destDC, &rc, CreateSolidBrush(colorFond)); 

	HPEN m_hPen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
	SelectObject(destDC, m_hPen);
	//MoveToEx(destDC, m_iMarge, m_iMarge, NULL);
	//LineTo(destDC, m_iMarge, iHeight - m_iMarge);
	MoveToEx(destDC, m_iMarge, iHeight - m_iMarge, NULL);
	LineTo(destDC, iWidth - m_iMarge, iHeight - m_iMarge);
	//MoveToEx(destDC, iWidth - m_iMarge, iHeight - m_iMarge, NULL);
	//LineTo(destDC, iWidth - m_iMarge, m_iMarge);

	int iYMax = (iHeight - (2*m_iMarge)); 
	int iDeplacement = (iWidth - (2*m_iMarge)) / 256;
	HPEN m_hPenLocal = CreatePen(PS_SOLID, iDeplacement, RGB(0,0,0));
	SelectObject(destDC, m_hPenLocal);
	for(int i = 0;i < 256;i++)
	{
		int m_iYSize = GetValue(iColor,iYMax,i);
		MoveToEx(destDC, (iDeplacement*i) + m_iMarge, iHeight - m_iMarge, NULL);
		LineTo(destDC, (iDeplacement*i) + m_iMarge, m_iYSize);
	}
	DeleteObject(m_hPenLocal);


	DeleteDC(destDC);
	DeleteObject(m_hPen);

	return (HBITMAP)CopyImage(hbmResult, IMAGE_BITMAP, 0, 0, LR_COPYDELETEORG);
}