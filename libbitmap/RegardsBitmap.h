// Bitmap.h: interface for the CBitmap class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __CBITMAP__H__
#define __CBITMAP__H__

class CIBitmap;

namespace Regards
{
	namespace libbitmap
	{
		class CBitmapPrv;

		class CBitmap
		{
		public:

			CBitmap(HINSTANCE hInstance);
			CBitmap();
			virtual ~CBitmap();

			void SetHInstance(HINSTANCE hInstance)
			{
				m_hInstance = hInstance;
			}

			void GetRatioInf();
			void GetRatioSup();
			void SetLargeurPos(const int &iLargeur);
			void SetPosHauteur(const int &iHauteur);
			float GetRatioScreen(CIBitmap &m_ciBitmap,const RECT &rc);
			int GetTailleAffichageHeigth();
			int GetTailleAffichageWidth();
			void SetInvertSensPhoto(const bool &m_bValue);
			void SetPrinter(const bool &m_bValue);
			void SetHauteQualite(const int &m_bHauteQualite);
			void InitRatio();
			bool GetMouseBlock();
			void SetMouseBlock(const bool &m_bBlock);
			int &GetfScroll();
			void PositionnementImage(DWORD lParam, const long &m_xMax, const long &m_yMax, CIBitmap &m_ciBitmap);
			void SetMouseScroll(const int &m_iMouseX, const int &m_iMouseY);
			int GetyNewSize();
			int GetxNewSize();
			void SetfScroll(const bool &m_bState);
			void SetSize(const int &m_xNewSize, const int &m_yNewSize);
			void SetfSize(const bool &m_blState);
			bool GetFullscreenState();
			bool GetShrinkState();
			void InitScrolling();
			void SetDeplacement(const long &m_lDeplacement, const bool &m_bType, const long &m_lTailleMax, const long &m_lTailleImage);
			long GetPosHauteur();
			long GetPosLargeur();
			void SetPosition(const long &m_lLargeur, const long &m_lHauteur);
			void SetFullScreen(const bool &m_bValue);
			
			//void AffichageBitmap(HWND hWnd,CIBitmap &m_ciBitmap, const int &xCurrentScroll, const int &yCurrentScroll);
			
			void AffichageBitmap(HWND hWnd,CIBitmap * m_ciBitmap, const int &xCurrentScroll, const int &yCurrentScroll);
			void AffichageBitmap(HDC &hdc,RECT &rc,CIBitmap * m_ciBitmap, const int &xCurrentScroll, const int &yCurrentScroll);

			int AppliquerFiltre(HWND &hwndLocal, CIBitmap * m_ciBitmap, const bool &gpGpu, const int &NumFiltre, const long &paramatre = 0, const long &paramatre2 = 0, const bool &bAffichage = 1, const int &iDialog = 1);

			void SetShrinkImage(const bool &m_bShrink = true);
			float GetRatio();
			void DefineRatioSup(HWND &hWnd,const int &nTailleAffichageWidth, const int &nTailleAffichageHeight);
			void DefineRatioInf(HWND &hWnd,const int &nTailleAffichageWidth, const int &nTailleAffichageHeight);

			void DeterminePos(RECT &rc, int &left, int &top);
			void DeterminePos(RECT &rc,const int &nTailleAffichageWidth, const int &nTailleAffichageHeight, int &left, int &top);
			void SetBitmapSize(const int &m_xNewSize, const int &m_yNewSize);

			int GetReturnEffet()
			{
				return m_iRetourEffet;
			}

			void SetShowInfos(bool bValue);
			bool GetShowInfos();

			void SetShowHistogramme(bool bValue);
			bool GetShowHistogramme();

			//LRESULT ShowInfos(HWND hWnd, HDC hDC, CIBitmap * ciBitmap);
			//LRESULT ShowHistogramme(HWND hWnd, HDC hDC, CIBitmap * ciBitmap, RECT rc);

		private:







			CBitmapPrv * pimpl_;
			HINSTANCE m_hInstance;
			int m_iRetourEffet;
			
		};
	}
}

#endif // !defined(AFX_BITMAP_H__990E8455_7065_4437_96E6_A07D48B057B4__INCLUDED_)

