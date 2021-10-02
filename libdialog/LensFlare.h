
#include "LocalDialog.h"

#ifndef __LENSFLARE__H__
#define __LENSFLARE__H__

class CFiltre;
class CEffet;
class CIBitmap;
class CLensFlareData;

class CLensFlare : public CLocalDialog
{
public:
	CLensFlare(HINSTANCE hInstance);
	~CLensFlare(void);

	void SetBitmapSource(CIBitmap * Source)
	{
		m_Source = Source;
	}

	int GetReturnValue()
	{
		return m_iReturnValue;
	}

private:

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnHScroll(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnMouseMove(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnPaint(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnDblClick(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void GenerateIcon();
	void ApplyFilter(HWND hDlg);
	COLORREF CallColorPicker(HWND hDlg,COLORREF clValue);

	HINSTANCE m_hInstance;

	int m_iReturnValue;
	int iIntensity;
	int iColorIntensity;
	CFiltre * m_cFiltre;
	CEffet * m_cEffet;
	CLensFlareData * m_cLensFlareData;

	//Pointeur sur la source -- Ne pas Effacer;
	CIBitmap * m_Source;
};

#endif