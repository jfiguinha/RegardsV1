// Splitter.h: interface for the CSplitter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPLITTER_H__8ADFE5D3_1E4B_4016_AA91_26BC88D581AA__INCLUDED_)
#define AFX_SPLITTER_H__8ADFE5D3_1E4B_4016_AA91_26BC88D581AA__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include "LocalWindow.h"

class CWndSplitter : public CLocalWindow  
{
public:

	LRESULT OnCreate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


	void SetHorizontalSplit(const bool &m_bTrue);
	void SetSplitWindow(HWND hWnd1, HWND hWnd2);
	void SetSplitPos(DWORD dwSplitterPos);

	int RedrawSplitter();

	CWndSplitter();
	virtual ~CWndSplitter();

protected:

	void DrawXorBar(HDC hdc, int x1, int y1, int width, int height);
	void DrawBar();

	int LButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	int LButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	int MouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	int Size(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HWND hWnd1;
	HWND hWnd2;
	bool bSplitterMoving;
	bool m_bHorizontal;
	HCURSOR hCursor;
	int dwSplitterPos;
	int dwSplitterPosOld;
	int iRight;
	int iBottom;
	RECT rect;

};

#endif // !defined(AFX_SPLITTER_H__8ADFE5D3_1E4B_4016_AA91_26BC88D581AA__INCLUDED_)
