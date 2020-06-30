// LocalWindow.h: interface for the CLocalWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOCALWINDOW_H__B32657B4_03DF_4DCA_8784_695C2E150FB0__INCLUDED_)
#define AFX_LOCALWINDOW_H__B32657B4_03DF_4DCA_8784_695C2E150FB0__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include "message.h"

extern HINSTANCE hInstance;

class CLocalWindow : public CMessage
{
public:
	void SetBackgroundColor(HBRUSH m_bkgndColor);

	int Create(HWND hParent, HINSTANCE hInstance,DWORD m_dWndStyle,
						 DWORD m_dStyle, const char * szAppName,int iIcone,int iMenu,LPCSTR iCursor,RECT rcPos);

	int CreateEx(HWND hParent, HINSTANCE hInstance,DWORD m_dWndStyle,
						 DWORD m_dStyle,DWORD m_dExStyle, const char * szAppName,int iIcone,int iMenu,LPCSTR iCursor,RECT rcPos);




	CLocalWindow();
	virtual ~CLocalWindow();

	int ShowWindow(int nCmdShow);
	int UpdateWindow();

	void SetParent(HWND hWnd)
	{
		m_hParent = hWnd;
	};

	int IsWindowVisible()
	{
		try
		{
			return ::IsWindowVisible(m_hWnd);
		}
		catch(...)
		{
			return 0;
		}
	};

	int SetWindowText(LPCTSTR lpString)
	{
		return ::SetWindowText(m_hWnd,lpString);
	}

	HWND GetParent()
	{
		return m_hParent;
	}

	HWND m_hWnd;
	HWND m_hParent;
	HBRUSH m_bkgndColor;

private:

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif // !defined(AFX_LOCALWINDOW_H__B32657B4_03DF_4DCA_8784_695C2E150FB0__INCLUDED_)
