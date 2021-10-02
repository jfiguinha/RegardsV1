
#ifndef __FREEROTATEPICTURE__H__
#define __FREEROTATEPICTURE__H__

#include "LocalDialog.h"

class CIBitmap;

class CFreeRotatePicture : public CLocalDialog
{
public:
	CFreeRotatePicture(void);
	~CFreeRotatePicture(void);

	void Init(CIBitmap * Source);

	int GetReturnValue()
	{
		return m_iReturnValue;
	}

	double GetAngle()
	{
		return m_dAngle;
	}

private:

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnHScroll(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnActivate(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	CIBitmap * m_ciBitmapTestBitmap;
	CIBitmap * m_ciBitmapOldBitmap;
	int m_iAngle;
	double m_dAngle;
	UINT m_iWidth;
	UINT m_iHeight;
	int m_iReturnValue;

};

#endif