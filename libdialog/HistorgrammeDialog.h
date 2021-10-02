
#ifndef __HISTOGRAMMEDIALOG__H__
#define __HISTOGRAMMEDIALOG__H__

#include "LocalDialog.h"

class CIBitmap;

class CHistorgrammeDialog : public CLocalDialog
{
public:
	CHistorgrammeDialog(void);
	~CHistorgrammeDialog(void);

	void SetBitmap(CIBitmap * ciBimap)
	{
		m_ciBimapPtr = ciBimap;
	}

private:

	void AddToList(HWND hDlg);
	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void CreateGradient(HWND hDlg, COLORREF colorFrom, COLORREF colorTo, HBITMAP &hBitmapOut);

	CIBitmap * m_ciBimapPtr;
	HBITMAP hBitmapMoyenne;
	HBITMAP hBitmapRed;
	HBITMAP hBitmapGreen;
	HBITMAP hBitmapBlue;

	HBITMAP hBmpGradientBlack;
	HBITMAP hBmpGradientBlue;
	HBITMAP hBmpGradientGreen;
	HBITMAP hBmpGradientRed;

	int m_iSelect;

};

#endif