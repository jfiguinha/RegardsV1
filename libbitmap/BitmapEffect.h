

#ifndef __CBITMAPEFFECT__H__
#define __CBITMAPEFFECT__H__

class CIBitmap;
class CFiltreEffet;

namespace Regards
{
	namespace libbitmap
	{
		class CBitmapPrv;

		class CBitmapEffect
		{
		public:
			CBitmapEffect(void);
			~CBitmapEffect(void);
			static int EffetMotionBlur(HINSTANCE hInstance, HWND &hwndLocal, CIBitmap * m_ciBitmap, CFiltreEffet &m_filtre);
			static int EffetRotation(HINSTANCE hInstance, HWND &hwndLocal, CIBitmap * m_ciBitmap,CFiltreEffet &m_filtre, float fratio);
			static int EffetCrop(HINSTANCE hInstance, HWND &hwndLocal, CIBitmap * m_ciBitmap,CFiltreEffet &m_filtre);
			static int EffetScale(HINSTANCE hInstance, HWND &hwndLocal, CIBitmap * m_ciBitmap,CFiltreEffet &m_filtre);
			static int EffetFlip(HINSTANCE hInstance, HWND &hwndLocal, CIBitmap * m_ciBitmap,CFiltreEffet &m_filtre);
			static int EffetNuages(HINSTANCE hInstance, HWND &hwndLocal, CIBitmap * m_ciBitmap,CFiltreEffet &m_filtre);
			static int EffetDefault(HINSTANCE hInstance, HWND &hwndLocal, CIBitmap * m_ciBitmap,CFiltreEffet &m_filtre);
			static int EffetWiener(HINSTANCE hInstance, HWND &hwndLocal, CIBitmap * m_ciBitmap,CFiltreEffet &m_filtre);
			static int EffetRotationLibre(HINSTANCE hInstance, HWND &hwndLocal, CIBitmap * m_ciBitmap,CFiltreEffet &m_filtre, float fRatio);
			static int EffetLensFlare(HINSTANCE hInstance, HWND &hwndLocal, CIBitmap * m_ciBitmap,CFiltreEffet &m_filtre);
			static LRESULT ShowHistogramme(HWND hWnd, HDC hDC, CIBitmap * ciBitmap, RECT rc);
			static LRESULT ShowInfos(HWND hWnd, HDC hDC, CIBitmap * ciBitmap);

			static int log2(int zahl)
			{
				int log;
				log = 0;
				if (zahl != 0)
				{
					while ((zahl & 1) == 0)  
					{
						zahl>>=1;
						log++;
					}
				}
				return log;
			}
		};
	}
}

#endif
