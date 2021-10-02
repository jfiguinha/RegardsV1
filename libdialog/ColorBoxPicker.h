
#include "LocalDialog.h"
#include <color.h>

#ifndef __COLORBOXPICKER__H__
#define __COLORBOXPICKER__H__

class CIBitmap;

class CColorBoxPicker : public CLocalDialog
{
public:
	CColorBoxPicker(void);
	~CColorBoxPicker(void);

	void SetColorValue(COLORREF clValue)
	{
		m_clValue = clValue;
	}

	COLORREF GetColorValue()
	{
		return m_clValue;
	}

	int GetReturnValue()
	{
		return m_iReturnValue;
	}

private:


	CIBitmap * m_ciBitmapColorMatrix;
	CIBitmap * m_ciBitmapColorGradient;
	CIBitmap * m_ciBitmapColorBox;
	HSB m_Hsbcolor;
	COLORREF m_clValue;
	int x, y;
	int m_iReturnValue;

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnActivate(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnMouseMove(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnPaint(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnVScroll(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void DefineDialogItem(HWND hDlg);

};

#endif