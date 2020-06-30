// WndIntro.h: interface for the CWndIntro class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDINTRO_H__F85D17A3_4C4C_46EB_9423_2CDF29E88E21__INCLUDED_)
#define AFX_WNDINTRO_H__F85D17A3_4C4C_46EB_9423_2CDF29E88E21__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include "libWindow/MainWindow.h"

class CWndIntro : public CMainWindow  
{
public:

	CWndIntro();
	virtual ~CWndIntro();

private:

	LRESULT OnEraseBackground(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnShowWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnTimer(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void Paint(HWND hWnd);

	char buffer[256];
	HWND hWndIntro;
	int iNbPoint;
};

#endif // !defined(AFX_WNDINTRO_H__F85D17A3_4C4C_46EB_9423_2CDF29E88E21__INCLUDED_)
