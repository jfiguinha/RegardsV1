
#ifndef __NEWALBUM__H__
#define __NEWALBUM__H__

#include "../libWindow/LocalDialog.h"

class CNewAlbum : public CLocalDialog
{
public:
	CNewAlbum(void);
	~CNewAlbum(void);

	void SetWindow(HWND hWnd)
	{
		m_hParentWnd = hWnd;
	}

private:

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HWND m_hParentWnd;
};

#endif