#include "stdafx.h"
#include "FiltrePrv.h"
#include "transfoasm.h"
#include "floattoint.h"
#include "ibitmap.h"
#include "color.h"


void CFiltrePrv::GetBitmapInfo(CIBitmap * m_ciBitmap)
{
	m_ciBitmap->GetBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize);
}

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
CFiltrePrv::CFiltrePrv()
{
	pBitsSrc = NULL;
	bmWidth = 0;
	bmHeight = 0;
	m_lSize = 0;
	iTypeTransfert = 0;
	iBilinearFiltering = 0;
	iWidthMax = 0;
	iHeightMax = 0;
	iIntensity = 100;
	OldCtrlWord = Changex87FPToTrunc();
}

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
CFiltrePrv::~CFiltrePrv()
{
	DeleteBitsSrv();
}

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
void CFiltrePrv::Halo(const int &x, const int &y,CIBitmap * m_ciBitmap,const int &iColor, const int &iTaille, const int &iWidth, const float &fAlpha2, const int &iCentre)
{
	GetBitmapInfo(m_ciBitmap);

	iTypeTransfert = 1;

	float y1 = 0.3f;
	float x1 = 0.5f;
	float a = (y1 - 1.0f) / (x1 * (x1 - 1.0f));
	float b = 1.0f - a;
	int i = 0;

	if(iCentre)
	{				
		for(i = 0;i < iTaille - iWidth;i++)
		{
			float fAlpha = ((float)i / (float)(iTaille));
			fAlpha = (a * (fAlpha * fAlpha) + b * fAlpha);

			if(fAlpha < 0.0f)
				fAlpha = 0.0f;

			HSB m_value = {iColor,(fAlpha * iIntensity),100};
			COLORREF m_rgbValue;
			CColor::HSBToRGB(m_value,m_rgbValue);		

			MidpointCircle(x,y,i,m_ciBitmap,m_rgbValue,fAlpha2);
		}
	}

	const double m_dCent = 100;

	for(i = iTaille - iWidth;i < iTaille;i++)
	{
		
		float fAlpha;
		int j = 100 - i;
		if(j < 0)
			fAlpha = 1.0f;
		else
		{		
			double m_iIntensity = ((double)((double)(i) / m_dCent));

			m_iIntensity = asin(m_iIntensity);

			double m = (m_iIntensity * 90) / pi;
				
			m_iIntensity = exp(-m*m*0.006)*0.50 + exp(-m*0.03)*(1-0.50);

			fAlpha = 1.0f - m_iIntensity;

			if(fAlpha > 1.0f)
				fAlpha = 1.0f;
		}

		HSB m_value = {iColor,(fAlpha * iIntensity),100};
		COLORREF m_rgbValue;
		CColor::HSBToRGB(m_value,m_rgbValue);

		if(i == iTaille - 1)
			MidpointCircle(x,y,i,m_ciBitmap,m_rgbValue,fAlpha2,true);
		else
			MidpointCircle(x,y,i,m_ciBitmap,m_rgbValue,fAlpha2);
	}

	iTypeTransfert = 0;

	DeleteBitsSrv();

	if(iBilinearFiltering)
		RadialBlur(x,y,m_ciBitmap,iTaille);
}

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
void CFiltrePrv::HaloGradient(const int &x, const int &y,CIBitmap * m_ciBitmap, const int &iTaille, const int &iWidth, const float &fAlpha2)
{
	GetBitmapInfo(m_ciBitmap);

	iTypeTransfert = 1;

	int iNb = (360 / iWidth);

	int i = 0;

	for(i = iTaille - iWidth;i < iTaille;i++)
	{
		float fAlpha;
		int j = iTaille - i;
		if(j < 0)
			fAlpha = 1.0f;
		else
		{		
			double m_iIntensity = ((double)((double)(i) / (double)(100)));

			m_iIntensity = asin(m_iIntensity);

			double m = (m_iIntensity * 90) / pi;
				
			m_iIntensity = exp(-m*m*0.006)*0.50 + exp(-m*0.03)*(1-0.50);

			fAlpha = 1.0f - m_iIntensity;

			if(fAlpha > 1.0f)
				fAlpha = 1.0f;
		}

		HSB m_value = {iNb * j,50,100};
		COLORREF m_rgbValue;
		CColor::HSBToRGB(m_value,m_rgbValue);

		if(i == iTaille - 1)
			MidpointCircle(x,y,i,m_ciBitmap,m_rgbValue,fAlpha2,true);
		else
			MidpointCircle(x,y,i,m_ciBitmap,m_rgbValue,fAlpha2);
	}

	iTypeTransfert = 0;

	DeleteBitsSrv();

	if(iBilinearFiltering)
	{	
		for(i = iTaille - iWidth;i < iTaille - 1;i++)
			RadialBlur(x,y,m_ciBitmap,i,1);

		RadialBlur(x,y,m_ciBitmap,i);
	}
}

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
void CFiltrePrv::DefineZoneBilinear(const int &x, const int &y,CIBitmap * m_ciBitmap, const int &iTaille)
{
	int i = 0;
	int j = 0;

	for(i = y - iTaille;i < y + iTaille;i++)
	{
		for(j = x - iTaille;j < x + iTaille;j++)
		{
			COLORREF m_color;
			if(i > 1 && j > 1 && j < m_ciBitmap->GetBitmapWidth() - 1 && i < m_ciBitmap->GetBitmapHeigth() - 1)
			{
				BilinearFiltering(j,i,m_ciBitmap,m_color);
				m_ciBitmap->SetColorValue(j, i,m_color);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
void CFiltrePrv::Circle(const int &x, const int &y,CIBitmap * m_ciBitmap,const COLORREF &m_color, const int &iTaille, const float &fAlpha)
{
	GetBitmapInfo(m_ciBitmap);

	iTypeTransfert = 1;

	MidpointCircle(x,y,iTaille-1,m_ciBitmap,m_color,fAlpha,true,true);

	iTypeTransfert = 0;

	DeleteBitsSrv();

	if(iBilinearFiltering)
		RadialBlur(x,y,m_ciBitmap,iTaille);
}

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
void CFiltrePrv::CircleGradient(const int &x, const int &y,CIBitmap * m_ciBitmap,const COLORREF &m_color, const int &iTaille,const float &fAlpha)
{
	GetBitmapInfo(m_ciBitmap);

	iTypeTransfert = 1;

	int i = 0;

	for(i = 0;i < iTaille;i++)
	{
		if(i == iTaille - 1)
			MidpointCircle(x,y,i,m_ciBitmap,m_color,((float)(i) / (float)(iTaille)),true);
		else
			MidpointCircle(x,y,i,m_ciBitmap,m_color,((float)(i) / (float)(iTaille)));
	}

	iTypeTransfert = 0;

	DeleteBitsSrv();
	
	if(iBilinearFiltering)
		RadialBlur(x,y,m_ciBitmap,iTaille);
}

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
void CFiltrePrv::RadialBlur(const int &xFrom, const int &yFrom,CIBitmap * m_ciBitmap, const int &iRayon,const int &iSize)
{
	//int iDiff = 4;
	int iTaille = iRayon;// + iDiff;
	int r2 = iTaille * iTaille;
	int y = 0,x=0;
			
	int i = 0;
	int j = 0;

	do
	{
		
		float fValue = -sqrt((double)r2 - y*y);

		/////////////////////////////////////////////////////////////////////
		//Antialiasing avant
		/////////////////////////////////////////////////////////////////////

		x = (int)fValue;

		int xLocal = xFrom - x;
		int xLocal2 = xFrom + x;
		int yLocal = yFrom + y;
		int yLocal2 = yFrom - y;

		DefineZoneBilinear(xLocal,yLocal,m_ciBitmap,iSize);
		DefineZoneBilinear(xLocal2,yLocal,m_ciBitmap,iSize);
		DefineZoneBilinear(xLocal,yLocal2,m_ciBitmap,iSize);
		DefineZoneBilinear(xLocal2,yLocal2,m_ciBitmap,iSize);

		xLocal = xFrom - y;
		xLocal2 = xFrom + y;
		yLocal = yFrom + x;
		yLocal2 = yFrom - x;

		DefineZoneBilinear(xLocal,yLocal,m_ciBitmap,iSize);
		DefineZoneBilinear(xLocal2,yLocal,m_ciBitmap,iSize);
		DefineZoneBilinear(xLocal,yLocal2,m_ciBitmap,iSize);
		DefineZoneBilinear(xLocal2,yLocal2,m_ciBitmap,iSize);

		y++;

	}while(y < -x);
}

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
void CFiltrePrv::RadialBlurCercleVide(const int &xFrom, const int &yFrom,CIBitmap * m_ciBitmap, const int &iRayon,const int &iCercleTaille)
{
	int iDiff = 4;
	int iTaille = 50 + iDiff;
	int r2 = iTaille * iTaille;
	int y = 0,x=0;
			
	int i = 0;
	int j = 0;

	for(i = yFrom - iTaille;i < yFrom + iTaille;i++)
	{
		int yLocalValue = i - yFrom;
		//Détermination pour ce y du x Max + 1;
		float fValue = sqrt((double)r2 - abs(yLocalValue*yLocalValue));
		x = (int)fValue;

		if(i >= 0)
		{
			m_ciBitmap->SetColorValue(xFrom + iTaille - x,i ,RGB(0,0,255));
			m_ciBitmap->SetColorValue(xFrom + iTaille + x,i ,RGB(0,0,255));
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
void CFiltrePrv::Burst(const int &x, const int &y,CIBitmap * m_ciBitmap,const int &iTaille,const int &iColor, const int &iIntensity, const int &iColorIntensity)
{
	GetBitmapInfo(m_ciBitmap);

	iTypeTransfert = 1;

	//float m_Alpha;

	//float y1 = 1.0f - (iIntensity/100.0f);
	float y1 = 0.6f;
	float x1 = 0.5f;

	float a = (y1 - 1.0f) / (x1 * (x1 - 1.0f));
	float b = 1.0f - a;


	int i = 0;

	for(i = 0;i < iTaille;i++)
	{
		float fAlpha;
		//double m_iIntensity;

		int j = iTaille - i;

		//Test nouvelle version de l'intensité

		float k = (float)i / (float)iTaille;

		fAlpha = a * (k * k) + b * k;

		if(fAlpha < 0.0f)
			fAlpha = 0.0f;

		HSB m_value = {iColor,iColorIntensity * fAlpha,100};
		COLORREF m_rgbValue;
		CColor::HSBToRGB(m_value,m_rgbValue);

		if(i == iTaille - 1)
			MidpointCircle(x,y,i,m_ciBitmap,m_rgbValue,fAlpha,true);
		else
			MidpointCircle(x,y,i,m_ciBitmap,m_rgbValue,fAlpha);
	}

	iTypeTransfert = 0;

	DeleteBitsSrv();

	if(iBilinearFiltering)
		RadialBlur(x,y,m_ciBitmap,iTaille);
}

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
void CFiltrePrv::SetAlphaColorValue(const int &xFrom, const int &yFrom, const int &x, const int &y,const float &fAlpha,const COLORREF &m_color, CIBitmap * m_ciBitmap)
{
	COLORREF m_color2;

	float fDifference = 1.0f - fAlpha;

	int iXMin = m_ciBitmap->GetBitmapWidth() - iWidthMax;
	int iYMin =  m_ciBitmap->GetBitmapHeigth() - iHeightMax;


	if(iTypeTransfert == 0)
	{
		if(xFrom >= iWidthMax || xFrom < iXMin || x < iXMin || x >= iWidthMax )
			return;

		if(yFrom >= iHeightMax || yFrom < iYMin || y < iYMin || y >= iHeightMax)
			return;

		if(iYMin < 0)
		{
			if(y >= iHeightMax + iYMin)
				return;
		}

		if(iXMin < 0)
		{
			if(x >= iWidthMax + iXMin)
				return;
		}
	
		m_ciBitmap->GetRGBColorValue(xFrom, yFrom,m_color2);
	}
	else
	{

		if(x < iXMin || x >= iWidthMax)
			return;

		if(y < iYMin || y >= iHeightMax)
			return;

		if(iYMin < 0)
		{
			if(y >= iHeightMax + iYMin)
				return;
		}

		if(iXMin < 0)
		{
			if(x >= iWidthMax + iXMin)
				return;
		}

		int iPos = y * m_ciBitmap->GetWidthSize() + x * 4;
		m_color2 = RGB(*(pBitsSrc + iPos + 2),*(pBitsSrc + iPos + 1),*(pBitsSrc + iPos));
	}

	m_color2 = RGB(GetRValue(m_color) * fDifference + GetRValue(m_color2) * fAlpha
					,GetGValue(m_color) * fDifference + GetGValue(m_color2) * fAlpha
					,GetBValue(m_color) * fDifference  + GetBValue(m_color2) * fAlpha);

	m_ciBitmap->SetColorValue(x, y,m_color2);
}

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
void CFiltrePrv::MidpointCircle(const int &xFrom, const int &yFrom,const int &Rayon, CIBitmap * m_ciBitmap, const COLORREF &m_color, const float &fAlpha, const bool &bAntialiasing, const bool &bFullCircle)
{

	int r2;
	int iOldy = 0;
	int iOldx = 0;

	int x,y;

	r2 = Rayon * Rayon;
	int iDemiRayon = -(Rayon>>1) + 1;

	int iFin = (int)sqrt((double)r2 - iDemiRayon*iDemiRayon) - 1;

		for (x = -Rayon; x < iDemiRayon; x++) 
		{
			
			float fValue = sqrt((double)r2 - x*x);

			//int y = (int)fValue;

			int y = FloatToInt(fValue);
			
			
			if(bFullCircle)
			{
				for(int i = 0;i < iOldy;i++)
				{
					WriteCirclePoint(xFrom,yFrom,i,x,fAlpha,m_color,m_ciBitmap);
				}
			}

			for(int i = iOldy;i < Rayon;i++)
			{
				float fValue2 = sqrt((double)i*i + (-x*-x));
				//int iValue = (int)fValue2;

				int iValue = FloatToInt(fValue2);

				if(iValue < Rayon)
				{
					WriteCirclePoint(xFrom,yFrom,i,x,fAlpha,m_color,m_ciBitmap);
				}
				else if(bAntialiasing)
				{
					float fDifference = (fValue - Rayon);

					if(fDifference > 1.0f || fDifference < 0.0f)
						break;

					fDifference = fAlpha + ((1.0f - fAlpha) * fDifference);

					WriteCirclePoint(xFrom,yFrom,i,x,fDifference,m_color,m_ciBitmap);
				}
			}
		
			iOldy = y;	
		}
		for (y = Rayon; y >= iFin; y--) 
		{
			
			float fValue = -sqrt((double)r2 - y*y);

			//int x = (int)fValue;

			int x = FloatToInt(fValue);
			
			
			if(bFullCircle)
			{
				for(int i = iOldx;i <= 0;i++)
				{
					WriteCirclePoint(xFrom,yFrom,y,i,fAlpha,m_color,m_ciBitmap);
				}
			}

			for(int i = iOldx;i > -Rayon;i--)
			{
				float fValue2 = sqrt((double)i*i + (y*y));
				//int iValue = (int)fValue2;
				int iValue = FloatToInt(fValue2);

				if(iValue < Rayon)
				{
					WriteCirclePoint(xFrom,yFrom,y,i,fAlpha,m_color,m_ciBitmap);
				}
				else if(bAntialiasing)
				{
					
					float fDifference = fValue2 - Rayon;

					if(fDifference > 1.0f)
						break;

					fDifference = fAlpha + ((1.0f - fAlpha) * fDifference);

					WriteCirclePoint(xFrom,yFrom,y,i,fDifference,m_color,m_ciBitmap);


				}
			}
			iOldx = x;	
		}

	if(bFullCircle)
	{
		for(y = 0; y <= iFin+1; y++)
		{
			for(x = iDemiRayon;x <= 0;x++)
			{
				WriteCirclePoint(xFrom,yFrom,y,x,fAlpha,m_color,m_ciBitmap);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
void CFiltrePrv::WriteCirclePoint(const int &xFrom, const int &yFrom, const int &i, const int &x, const float &fAlpha, const COLORREF &m_color,
	CIBitmap * m_ciBitmap)
{
	float fAlpha2 = fAlpha;
	if(fAlpha > 1.0f)
		fAlpha2 = 1.0f;

	SetAlphaColorValue(xFrom + x,yFrom + i + 1,xFrom + x, yFrom + i,fAlpha2,m_color,m_ciBitmap);
	SetAlphaColorValue(xFrom + x,yFrom - i - 1,xFrom + x, yFrom - i,fAlpha2,m_color,m_ciBitmap);
	SetAlphaColorValue(xFrom - x,yFrom + i + 1,xFrom - x, yFrom + i,fAlpha2,m_color,m_ciBitmap);
	SetAlphaColorValue(xFrom - x,yFrom - i - 1,xFrom - x, yFrom - i,fAlpha2,m_color,m_ciBitmap);
}

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
void CFiltrePrv::WriteCirclePoint2(const int &xFrom, const int &yFrom, const int &i, const int &x, const float &fAlpha, const COLORREF &m_color,
	CIBitmap * m_ciBitmap)
{
	float fAlpha2 = fAlpha;
	if(fAlpha > 1.0f)
		fAlpha2 = 1.0f;

	SetAlphaColorValue(xFrom + x,yFrom + i,xFrom + x, yFrom + i,fAlpha2,m_color,m_ciBitmap);
	SetAlphaColorValue(xFrom + x,yFrom - i,xFrom + x, yFrom - i,fAlpha2,m_color,m_ciBitmap);
	SetAlphaColorValue(xFrom - x,yFrom + i,xFrom - x, yFrom + i,fAlpha2,m_color,m_ciBitmap);
	SetAlphaColorValue(xFrom - x,yFrom - i,xFrom - x, yFrom - i,fAlpha2,m_color,m_ciBitmap);
}

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
int CFiltrePrv::BilinearFiltering(const int &iSrcX,const int &iSrcY, CIBitmap * m_ciBitmap, COLORREF &m_colorValue)
{

	COLORREF pTopLeft, pTopRight, pBottomLeft, pBottomRight;

	int iXMin = m_ciBitmap->GetBitmapWidth() - iWidthMax;
	int iYMin =  m_ciBitmap->GetBitmapHeigth() - iHeightMax;


	if(iSrcX >= iWidthMax || iSrcX < iXMin)
		return 0;

	if(iSrcY >= iHeightMax || iSrcY < iYMin)
		return 0;


	m_ciBitmap->GetRGBColorValue(iSrcX-1, iSrcY+1,pTopLeft);
	m_ciBitmap->GetRGBColorValue(iSrcX+1, iSrcY+1,pTopRight);

	m_ciBitmap->GetRGBColorValue(iSrcX-1, iSrcY-1,pBottomLeft);
	m_ciBitmap->GetRGBColorValue(iSrcX+1, iSrcY-1,pBottomRight);
	
	
	// red interpolation
	int redFP =	(GetRValue(pTopLeft) +
					GetRValue(pTopRight)	+
					GetRValue(pBottomLeft) +
					GetRValue(pBottomRight)
					) >> 2;
	
	// green interpolation
	
	int greenFP =	(GetGValue(pTopLeft) +
					GetGValue(pTopRight)	+
					GetGValue(pBottomLeft) +
					GetGValue(pBottomRight)
					) >> 2;


	// blue interpolation
	//int blueFP = (GetBValue(pTopLeft) +  GetBValue(pTopRight) + GetBValue(pBottomLeft) + GetBValue(pBottomRight)) >> 2;                 

	int blueFP =	(GetBValue(pTopLeft) +
					GetBValue(pTopRight)	+
					GetBValue(pBottomLeft) +
					GetBValue(pBottomRight)
					) >> 2;

	m_colorValue = RGB(redFP,greenFP,blueFP);

	return 1;

}

///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////
void CFiltrePrv::MidpointLine(const int &xFrom, const int &yFrom,const int &xTo, const int &yTo, CIBitmap * m_ciBitmap, const COLORREF &m_color, const float &fAlpha, const bool &m_bAntialiasing)
{
 	int x, y;
 	float a = 0.0f, b = 0.0f, fValue = 0.0f;

 	a = ((float)(yTo - yFrom) / (float)(xTo - xFrom));
 	b = yFrom - (a * xFrom);

 	x = xFrom;
 	y = yFrom;

 	SetAlphaColorValue(x,y,x, y,fAlpha,m_color,m_ciBitmap);

 	while(x < xTo)
 	{

 		x++;

 		fValue = a * x + b;

 		//Définition de l'antialiasing

 		y = (int)fValue;

 		SetAlphaColorValue(x,y,x, y,fAlpha,m_color,m_ciBitmap);
 	}
}