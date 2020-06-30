#ifndef __FILTREPRV__H__
#define __FILTREPRV__H__


class CIBitmap;

class CFiltrePrv
{
	public:

		CFiltrePrv();

		~CFiltrePrv();

		void Halo(const int &x, const int &y,CIBitmap * m_ciBitmap,const int &iColor, const int &iTaille, const int &iWidth, const float &fAlpha2 = 0.9f, const int &iCentre = 1);

		void HaloGradient(const int &x, const int &y,CIBitmap * m_ciBitmap, const int &iTaille, const int &iWidth, const float &fAlpha2 = 0.9f);

		void DefineZoneBilinear(const int &x, const int &y,CIBitmap * m_ciBitmap, const int &iTaille);

		void Circle(const int &x, const int &y,CIBitmap * m_ciBitmap,const COLORREF &m_color, const int &iTaille, const float &fAlpha);
		
		void CircleGradient(const int &x, const int &y,CIBitmap * m_ciBitmap,const COLORREF &m_color, const int &iTaille,const float &fAlpha = 0.8f);

		void RadialBlur(const int &xFrom, const int &yFrom,CIBitmap * m_ciBitmap, const int &iRayon,const int &iSize = 2);

		void RadialBlurCercleVide(const int &xFrom, const int &yFrom,CIBitmap * m_ciBitmap, const int &iRayon,const int &iCercleTaille);

		void Burst(const int &x, const int &y,CIBitmap * m_ciBitmap,const int &iTaille,const int &iColor, const int &iIntensity = 25, const int &iColorIntensity = 30);
		
		void SetAlphaColorValue(const int &xFrom, const int &yFrom, const int &x, const int &y,const float &fAlpha,const COLORREF &m_color, CIBitmap * m_ciBitmap);
		
		void MidpointCircle(const int &xFrom, const int &yFrom,const int &Rayon, CIBitmap * m_ciBitmap, const COLORREF &m_color, const float &fAlpha, const bool &bAntialiasing = false, const bool &bFullCircle = false);

		void WriteCirclePoint(const int &xFrom, const int &yFrom, const int &i, const int &x, const float &fAlpha, const COLORREF &m_color,
			CIBitmap * m_ciBitmap);

		void WriteCirclePoint2(const int &xFrom, const int &yFrom, const int &i, const int &x, const float &fAlpha, const COLORREF &m_color,
			CIBitmap * m_ciBitmap);

		int BilinearFiltering(const int &iSrcX,const int &iSrcY, CIBitmap * m_ciBitmap, COLORREF &m_colorValue);

		void MidpointLine(const int &xFrom, const int &yFrom,const int &xTo, const int &yTo, CIBitmap * m_ciBitmap, const COLORREF &m_color, const float &fAlpha, const bool &m_bAntialiasing);

		////////////////////////////////////////////////////
		//Property
		////////////////////////////////////////////////////

		inline void DeleteBitsSrv()
		{
			if(pBitsSrc != NULL)
				delete[] pBitsSrc;

			pBitsSrc = NULL;
		}

		inline void SetBilinearFiltering(int iValue)
		{
			iBilinearFiltering = iValue;
		}

		inline void SetHeightMax(UINT iValue)
		{
			iHeightMax = iValue;
		}

		inline void SetWidthMax(UINT iValue)
		{
			iWidthMax = iValue;
		}

		inline void SetIntensity(int iValue)
		{
			iIntensity = iValue;
		}

		inline void SetTypeTransfert(int iValue)
		{
			iTypeTransfert = iValue;
		}

		void GetBitmapInfo(CIBitmap * m_ciBitmap);

private:

		typedef struct KERNELSTORAGE
		{
			int m_dData[256];
		}DATAKERNELSTORAGE, * LPDATAKERNELSTORAGE;

		BYTE * pBitsSrc;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize;
		int iTypeTransfert;
		int iBilinearFiltering;
		UINT iWidthMax, iHeightMax;
		int iIntensity;
		WORD OldCtrlWord;

};

#endif