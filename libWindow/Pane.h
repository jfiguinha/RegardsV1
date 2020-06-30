// Pane.h: interface for the CPane class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PANE_H__BBFA68F9_1F89_4784_9914_07FDF1544571__INCLUDED_)
#define AFX_PANE_H__BBFA68F9_1F89_4784_9914_07FDF1544571__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include "LocalWindow.h"

class CStatic;
class CTooltip;

class CPane : public CLocalWindow  
{

public:

	int SetTooltipText(const char * m_szTooltip);

	void SetTitle(const char * m_szTitle);


	CPane();
	virtual ~CPane();

	void SetWindow(HWND hWnd)
	{
		hWnd1 = hWnd;
	};

	LRESULT OnCreate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseLeave(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseHover(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnTimer(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnShowWindow(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:

	HFONT GetAFont(int fnFont);

	HWND hWnd1;
	CStatic * m_cLibelle;
	CStatic * m_cIcone;
	CStatic * m_cIconeOn;
	CTooltip * m_cTooltip;

	HCURSOR hCursor;
	HCURSOR hCursorHand;
	HBITMAP m_croix;
	HBITMAP m_croixOn;

	RECT rcFermer;
	bool m_bHover;
	bool m_bMouseMove;
	int m_bTracking;
};

#endif // !defined(AFX_PANE_H__BBFA68F9_1F89_4784_9914_07FDF1544571__INCLUDED_)
