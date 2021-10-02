


#ifndef __PHOTOFILTREDIALOG__H__
#define __PHOTOFILTREDIALOG__H__

#include "LocalDialog.h"

class CIBitmap;

class CPhotoFiltre : public CLocalDialog
{
public:
	CPhotoFiltre(HINSTANCE hInstance);
	~CPhotoFiltre(void);

	int GetReturnValue()
	{
		return m_iReturnValue;
	}

	void GetValue(COLORREF &colorRef, int &iIntensity);

private:

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnHScroll(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDblClick(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	COLORREF GetColorFiltre(int iNumFiltre);
	LRESULT CallColorPicker(HWND hDlg,COLORREF &clValue);
	void AddToList(HWND hDlg);

	HBITMAP hBitmap;
	CIBitmap * m_ciBitmap;
	int m_iReturnValue;
	HWND hWndBitmap;
	COLORREF m_colorref;
	HINSTANCE m_hInstance;
	int m_iItensity;
};

#endif