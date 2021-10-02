
#include "LocalDialog.h"

#ifndef __BRIGHTCONTDIALOG__H__
#define __BRIGHTCONTDIALOG__H__

class CIBitmap;
#include <Regardsbitmap.h>
using namespace Regards::libbitmap;

class CBrightContDialog : public CLocalDialog
{
public:
	CBrightContDialog(void);
	virtual ~CBrightContDialog(void);

	void SetBitmapSource(CIBitmap * ciBitmapSrc);

	void SetBitmapHandleSrc(Regards::libbitmap::CBitmap * cBitmap)
	{
		m_cBitmap = cBitmap;
	}

	void SetHWND(HWND hWnd)
	{
		m_hWndBitmap = hWnd;
	}

	int GetReturnValue()
	{
		return m_iReturnValue;
	}

private:

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnClick(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnHScroll(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void ExecuteFilterPreview(HWND hWndParent, int m_lNewContrastVal, int m_lNewBrightnessVal);

	TCHAR szContrast[10], szBrightness[10];
	long lContrast, lBrightness;
	long lConstrastOld, lBrightnessOld;
	bool m_preview;
	HWND m_hWndBitmap;
	int m_iReturnValue;

	CIBitmap * m_ciBitmapOld;
	CIBitmap * m_ciBitmapSrc;
	Regards::libbitmap::CBitmap * m_cBitmap;
};

#endif