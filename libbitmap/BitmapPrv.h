#ifndef __CBITMAPPRV__H__
#define __CBITMAPPRV__H__

namespace Regards
{
	namespace libbitmap
	{

		class CBitmapPrv
		{
		public:


			typedef struct CBITMAPPRV
			{
				unsigned int nTailleAffichageWidth, nTailleAffichageHeight;
				static int m_iParametreFiltre;
				BOOL fScroll;         // TRUE if scrolling occurred 
				BOOL fSize;           // TRUE if fBlt & WM_SIZE 
				bool m_bFullScreen;
				bool m_bBlocking;
				int xNewSize; 
				int yNewSize; 
				long nlPosLargeur, nlPosHauteur;
				int m_iMouseScrollX, m_iMouseScrollY;
				bool m_bAutoShrink;
				float fratio;
				float fLocalratio;
				long lPtTabRatio;
				int m_bHauteQualite;
				bool m_bPrinter;
				bool m_bInvert;
				int xImagePos;
				int yImagePos;
				int iWidthScreen;
				int iHeightScreen;
				bool m_bShowInfos;
				bool m_bShowHistogramme;
			}CBITMAPPRVDATA, * FAR LPCBITMAPPRV;

			CBitmapPrv()
			{
				ZeroMemory(&m_BitmapData,sizeof(CBITMAPPRVDATA));
				m_BitmapData.fratio = 1.0; //21
				m_BitmapData.lPtTabRatio = 21;
				m_BitmapData.m_bAutoShrink = false;
				m_BitmapData.m_bFullScreen = false;
				m_BitmapData.m_bBlocking = false;
				m_BitmapData.xNewSize = 0; 
				m_BitmapData.yNewSize = 0; 
				m_BitmapData.nlPosLargeur = m_BitmapData.nlPosHauteur = 0;
				m_BitmapData.m_iMouseScrollX = m_BitmapData.m_iMouseScrollY = 0;
				m_BitmapData.m_bBlocking = false;
				m_BitmapData.m_bHauteQualite = false;
				m_BitmapData.m_bPrinter = false;
				m_BitmapData.m_bInvert = false;
				m_BitmapData.iWidthScreen = GetDeviceCaps(GetDC(NULL),HORZRES);
				m_BitmapData.iHeightScreen = GetDeviceCaps(GetDC(NULL),VERTRES);
				m_BitmapData.m_bShowInfos = false;
				m_BitmapData.m_bShowHistogramme = false;

			};
			~CBitmapPrv()
			{
			};

			void TestMaxY(const long &m_lBitmapWidth, const long &right);
			void TestMaxX(const long &m_lBitmapHeight, const long &bottom);
			void CompleteBitmap(HDC &hdc, RECT &rc, int &left, int &top, int &xValue, int &yValue);

			CBITMAPPRVDATA m_BitmapData;

		};
	}
}

#endif