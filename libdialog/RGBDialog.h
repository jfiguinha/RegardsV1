
#include "LocalDialog.h"

#ifndef __RGBDIALOG__H__
#define __RGBDIALOG__H__

class CIBitmap;
#include <Regardsbitmap.h>
using namespace Regards::libbitmap;

class CRGBDialog : public CLocalDialog
{
public:
	CRGBDialog(void);
	virtual ~CRGBDialog(void);

	void SetCBitmap(Regards::libbitmap::CBitmap * cBitmap,CIBitmap * ciBitmap)
	{
		m_cBitmap = cBitmap;
		m_ciBitmap = ciBitmap;
	}

	int GetReturnCode()
	{
		return m_iReturnCode;
	}

private:


	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnVScroll(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	//
	long lRed, lGreen, lBlue;
	CIBitmap * m_ciBitmapLast;
	HBITMAP hBitmap;
	int m_iAction;
	Regards::libbitmap::CBitmap * m_cBitmap;
	CIBitmap * m_ciBitmap;
	int m_iReturnCode;
};

#endif