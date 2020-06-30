// Color.cpp: implementation of the CColor class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Color.h"
#include "ibitmap.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CColor::CColor()
{

}

CColor::~CColor()
{

}

int CColor::HSBToRGB(HSB &HSBValue, COLORREF &rgb)
{
	if(HSBValue.Saturation > 0)
	{
		double nH = (double)(HSBValue.Hue % 360) / (double)60;
		double nL = (double)HSBValue.Brightness / (double)100;
		double nS = (double)HSBValue.Saturation / (double)100;

		UINT lH = (UINT)nH;
		double nF = nH - lH;

	    double nP = nL * (1 - nS);
	    double nQ = nL * (1 - nS * nF);
		double nT = nL * (1 - nS * (1 - nF));

		switch(lH)
		{
			case 0:
				{
					rgb = RGB(nL * 255,nT * 255,nP * 255);
				}
				break;

			case 1:
				{
					rgb = RGB(nQ * 255,nL * 255,nP * 255);
				}
				break;

			case 2:
				{
					rgb = RGB(nP * 255,nL * 255,nT * 255);
				}
				break;

			case 3:
				{
					rgb = RGB(nP * 255,nQ * 255,nL * 255);
				}
				break;

			case 4:
				{
					rgb = RGB(nT * 255,nP * 255,nL * 255);
				}
				break;

			case 5:
				{
					rgb = RGB(nL * 255,nP * 255,nQ * 255);
				}
				break;
		}

	}
	else
	{
		UINT iRed = ((HSBValue.Brightness * 255) / 100);
		rgb = RGB(iRed,iRed,iRed);
	}

	return 0;
}

int CColor::RGBToHSB(HSB &HSBValue, COLORREF &rgb)
{
	float nTemp;
	float lMin, lMax;
	float R, G, B, lDelta;

	R = GetRValue(rgb);
	G = GetGValue(rgb);
	B = GetBValue(rgb);
  
	lMax = (R > G) ? ((R > B) ? R : B) : ((G > B) ? G : B);
	lMin = (R < G) ? ((R < B) ? R : B) : ((G < B) ? G : B);

	lDelta = lMax - lMin;

	HSBValue.Brightness = (lMax * 100) / 255;

	if(lMax > 0)
	{
		HSBValue.Saturation = (lDelta / lMax) * 100;
		if(lDelta > 0)
		{
			if(lMax == R)
			{
				nTemp = (G - B) / lDelta;
			}
			else if(lMax == G)
			{
				nTemp = 2 + (B - R) / lDelta;
			}
			else
			{
				nTemp = 4 + (R - G) / lDelta;
			}
		}	
		HSBValue.Hue = nTemp * 60;
		if(HSBValue.Hue < 0)
		{
			HSBValue.Hue = HSBValue.Hue + 360;
		}
	}

	return 0;
}

int CColor::CreateColorGradient(CIBitmap * m_ciBitmap, const COLORREF colorFrom, const COLORREF colorTo)
{
	BYTE * pBitsSrc = NULL;
	ULONG m_lSize;
	UINT bmWidth, bmHeight;
	m_ciBitmap->GetPtBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize);

	float m_fRedEtape = (float)(GetRValue(colorTo) - GetRValue(colorFrom)) / (float) bmWidth;
	float m_fGreenEtape = (float)(GetGValue(colorTo) - GetGValue(colorFrom)) /  (float)bmWidth;
	float m_fBlueEtape = (float)(GetBValue(colorTo) - GetBValue(colorFrom)) /  (float)bmWidth;

	float m_fRedValue = GetRValue(colorFrom);
	float m_fGreenValue = GetGValue(colorFrom);
	float m_fBlueValue = GetBValue(colorFrom);

	for(int x = 0;x < bmWidth;x++)
	{
		COLORREF m_color = RGB((int)m_fRedValue,(int)m_fGreenValue,(int)m_fBlueValue);
		for(int y = 0;y < bmHeight;y++)
		{
			m_ciBitmap->SetColorValue(x, y, m_color);
		}

		m_fRedValue += m_fRedEtape;
		m_fGreenValue += m_fGreenEtape;
		m_fBlueValue += m_fBlueEtape;
	}

	m_ciBitmap->ReCreateHBitmap(false);
	return 0;
}

int CColor::CreateHSBMatrixColorGradient(CIBitmap * m_ciBitmap, const long &m_lHue)
{
	if(m_ciBitmap->GetPtBitmap() != NULL)
	{
		//long Moyenne;
		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize;
		long lWidthSize = m_ciBitmap->GetWidthSize();

		//Définition du centre du lens flare

		m_ciBitmap->GetBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize);

		//Test du gradient de couleur

		UINT i = 0;
		for(UINT k = 0;k < bmHeight;k++)
		{
			for(UINT j = 0;j < bmWidth;j++,i+=4)
			{

				//UINT i = (k * lWidthSize) + (j * 4);

				
				if(k < 2 || k > bmHeight - 3 || j < 2 || j > bmWidth - 3)
				{
					*(pBitsSrc + i) = 0;
					*(pBitsSrc + i + 1) = 0;
					*(pBitsSrc + i + 2) = 0;
					*(pBitsSrc + i + 3) = 0;
				}
				else
				{
					HSB m_value = {m_lHue,((j - 2) * 100) / (bmHeight - 3),((k - 2) * 100) / (bmWidth - 3)};
					COLORREF m_rgbValue;

					HSBToRGB(m_value,m_rgbValue);

					*(pBitsSrc + i) = GetBValue(m_rgbValue);
					*(pBitsSrc + i + 1) = GetGValue(m_rgbValue);
					*(pBitsSrc + i + 2) = GetRValue(m_rgbValue);
					*(pBitsSrc + i + 3) = 0;
				}
			}
		}

		m_ciBitmap->SetBitmap(pBitsSrc, bmWidth, bmHeight);

		delete[] pBitsSrc;
	}
	return 0;
}

int CColor::CreateColorBitmap(CIBitmap * m_ciBitmap, COLORREF m_rgbValue)
{
	if(m_ciBitmap->GetPtBitmap() != NULL)
	{
		//long Moyenne;
		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize;
		long lWidthSize = m_ciBitmap->GetWidthSize();

		//Définition du centre du lens flare

		m_ciBitmap->GetBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize);

		//Test du gradient de couleur

		int i = 0;

		for(UINT k = 0;k < bmHeight;k++)
		{
			for(UINT j = 0;j < bmWidth;j++)
			{
				UINT i = (k * lWidthSize) + (j * 4);

				*(pBitsSrc + i) = GetBValue(m_rgbValue);
				*(pBitsSrc + i + 1) = GetGValue(m_rgbValue);
				*(pBitsSrc + i + 2) = GetRValue(m_rgbValue);
				*(pBitsSrc + i + 3) = 0;
			}
		}

		m_ciBitmap->SetBitmap(pBitsSrc, bmWidth, bmHeight);

		delete[] pBitsSrc;
	}
	return 0;
}


int CColor::CreateHSBColorGradient(CIBitmap * m_ciBitmap)
{
	if(m_ciBitmap->GetPtBitmap() != NULL)
	{
		//long Moyenne;
		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize;
		long lWidthSize = m_ciBitmap->GetWidthSize();

		//Définition du centre du lens flare

		m_ciBitmap->GetBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize);

		//Test du gradient de couleur
		for(UINT k = 0;k < bmHeight;k++)
		{
			HSB m_value = {(k * 360) / bmHeight,100,100};
			COLORREF m_rgbValue;
			CColor::HSBToRGB(m_value,m_rgbValue);

			for(UINT j = 0;j < 30;j++)
			{
				UINT i = (k * lWidthSize) + (j * 4);
				*(pBitsSrc + i) = GetBValue(m_rgbValue);
				*(pBitsSrc + i + 1) = GetGValue(m_rgbValue);
				*(pBitsSrc + i + 2) = GetRValue(m_rgbValue);
				*(pBitsSrc + i + 3) = 0;
			}
		}

		m_ciBitmap->SetBitmap(pBitsSrc, bmWidth, bmHeight);

		delete[] pBitsSrc;
	}
	return 0;
}