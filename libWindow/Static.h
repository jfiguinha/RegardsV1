// Static.h: interface for the CStatic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATIC_H__96945B9A_CA1F_467D_8C12_B1639770FFE7__INCLUDED_)
#define AFX_STATIC_H__96945B9A_CA1F_467D_8C12_B1639770FFE7__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include "LocalWindow.h"

class CStatic : public CLocalWindow  
{
public:
	int CreateEx(HWND hParent,DWORD dExStyle,DWORD dWndStyle,RECT rc);
	CStatic();
	virtual ~CStatic();

	int SetText(const char * m_szTitle)
	{

		return SendMessage( 
		  m_hWnd,              // handle to destination window 
		  WM_SETTEXT,               // message to send
		  (WPARAM)0,          // not used; must be zero
		  (LPARAM) m_szTitle           // window-text string (LPCTSTR)
		);
	}

	int SetFont(HFONT hFont,const bool &m_bRedraw)
	{

		return SendMessage( 
		  m_hWnd,              // handle to destination window 
		  WM_SETFONT,               // message to send
		  (WPARAM) hFont,          // not used; must be zero
		  (LPARAM) 0           // window-text string (LPCTSTR)
		);
	}

	int SetImage(int iImageType,HANDLE hBitmap)
	{

		return SendMessage( 
		  (HWND) m_hWnd,              // handle to destination window 
		  STM_SETIMAGE,             // message to send
		  (WPARAM) iImageType,          // image type
		  (LPARAM) hBitmap          // handle to image (HANDLE)
		);

	}

};

#endif // !defined(AFX_STATIC_H__96945B9A_CA1F_467D_8C12_B1639770FFE7__INCLUDED_)
