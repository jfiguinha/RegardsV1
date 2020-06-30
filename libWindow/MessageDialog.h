// MessageDialog.h: interface for the CMessageDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGEDIALOG_H__5BF22369_6CE8_451C_981D_CE4400CC510B__INCLUDED_)
#define AFX_MESSAGEDIALOG_H__5BF22369_6CE8_451C_981D_CE4400CC510B__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

class CMessageDialog  
{
public:

	CMessageDialog();
	virtual ~CMessageDialog();

	virtual LRESULT OnGetDlgCode(HWND hWnd,UINT uMsg, WPARAM wParam,LPARAM lParam)
	{
		return 0;
	}

	virtual LRESULT OnActivate(HWND hWnd,UINT uMsg, WPARAM wParam,LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnShowWindow(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnSysCommand(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnInitDialog(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnEraseBackground(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnTimer(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnNotify(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnCommand(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnContextMenu(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnCreate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnSetFocus(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnPaint(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnMeasureItem(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnDrawItem(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnKeyUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnKeyDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnSize(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnDblClick(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnClose(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		OnDestroy(hWnd, uMsg, wParam, lParam);
		return 0;
	};
	virtual LRESULT OnDestroy(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//PostQuitMessage(0);
		return 0;
	};

	virtual LRESULT OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};
	virtual LRESULT OnLButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};

	virtual	LRESULT OnMouseHover(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnMouseLeave(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnNCMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};


	virtual LRESULT OnMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};


	virtual LRESULT OnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnHScroll(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnVScroll(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnRButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};

	virtual LRESULT OnRButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	};

};

#endif // !defined(AFX_MESSAGEDIALOG_H__5BF22369_6CE8_451C_981D_CE4400CC510B__INCLUDED_)
