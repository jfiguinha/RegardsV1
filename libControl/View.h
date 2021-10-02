// View.h: interface for the CView class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <atlbase.h>
#include <atlwin.h>
extern CComModule _Module;

#include <atlcom.h>

#include <atlwin.h>
//#include <atlframe.h>

class CView : public CWindowImpl<CView>
{
public:
	DECLARE_WND_CLASS(NULL)

	DWORD m_dwCommCtrlMajor;
	DWORD m_dwCommCtrlMinor;
	bool m_bAlpha;

	CView() :
		m_dwCommCtrlMajor(0),
		m_dwCommCtrlMinor(0),
		m_bAlpha(false)
	{ }

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CView)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CPaintDC dc(m_hWnd);

		//CString strText;
		//strText.Format(_T("Common Controls version %u.%2.2u"), m_dwCommCtrlMajor, m_dwCommCtrlMinor);
		dc.TextOut(10, 20, "text");

		dc.TextOut(10, 40, m_bAlpha ? _T("Using 32-bit (alpha channel) images") : _T("Using standard 4-bit images"));

		return 0;
	}
};
