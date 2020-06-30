// Message.h: interface for the CMessage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGE_H__E6B16032_1B84_4081_8461_7789E54FBC9E__INCLUDED_)
#define AFX_MESSAGE_H__E6B16032_1B84_4081_8461_7789E54FBC9E__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

class CMessage  
{
public:

	virtual LRESULT OnKillFocus(HWND hWnd,UINT uMsg, WPARAM wParam,LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnActivate(HWND hWnd,UINT uMsg, WPARAM wParam,LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnShowWindow(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnSysCommand(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnInitDialog(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnEraseBackground(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnTimer(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnNotify(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnCommand(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnContextMenu(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnCreate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnSetFocus(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnPaint(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnMeasureItem(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnDrawItem(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnKeyUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnKeyDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnSize(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnClose(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		OnDestroy(hWnd, uMsg, wParam, lParam);
		return 0;
	};

	virtual LRESULT OnDestroy(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//PostQuitMessage(0);
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};
	virtual LRESULT OnLButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual	LRESULT OnMouseHover(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnMouseLeave(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnNCMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};


	virtual LRESULT OnMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnDblClick(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnHScroll(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnVScroll(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnRButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnRButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnGetDlgCode(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	virtual LRESULT OnChar(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};	

	virtual LRESULT OnMediaChange(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};	

	virtual LRESULT OnWindowPosChanging(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	CMessage();
	virtual ~CMessage();

	
};

#endif // !defined(AFX_MESSAGE_H__E6B16032_1B84_4081_8461_7789E54FBC9E__INCLUDED_)
