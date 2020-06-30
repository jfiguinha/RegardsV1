

#ifndef __REDIMWORKSPACE__H__
#define __REDIMWORKSPACE__H__

#include "../libWindow/LocalDialog.h"

class CRedimWorkSpace : public CLocalDialog
{
public:
	CRedimWorkSpace(long lWidth, long lHeight);
	virtual ~CRedimWorkSpace(void);

	int GetReturnValue()
	{
		return m_iReturnValue;
	}

	RECT GetRectangleValue()
	{
		return m_rect;
	}

private:

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void DefineRectangle(int iButtonPush, RECT &rc,long width, long height);

	int m_iReturnValue;
	long m_lWidthImage, m_lHeightImage;
	int m_iButtonPush;
	RECT m_rect;

};

#endif
