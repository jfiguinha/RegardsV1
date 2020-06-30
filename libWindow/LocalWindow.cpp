// LocalWindow.cpp: implementation of the CLocalWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LocalWindow.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLocalWindow::CLocalWindow()
{
	m_bkgndColor = (HBRUSH)(COLOR_3DFACE+1);
	m_hWnd = NULL;
}

CLocalWindow::~CLocalWindow()
{

}

int CLocalWindow::Create(HWND hParent, HINSTANCE hInstance,DWORD m_dWndStyle,
						 DWORD m_dStyle,const char * szAppName,int iIcone,int iMenu,LPCSTR iCursor,RECT rcPos)
{
	WNDCLASSEX	wc;
	//RECT rc;

	m_hParent = hParent;
	
	//
	//	Register our main window class
	//
	wc.cbSize			= sizeof(wc);
	wc.style			= m_dStyle;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra = 0;

	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon (wc.hInstance, (LPCTSTR)iIcone);
	wc.hCursor			= LoadCursor (NULL, iCursor);
	wc.hbrBackground	= m_bkgndColor;
	wc.lpszMenuName		= (LPCSTR)iMenu;
	wc.lpszClassName	= szAppName;
	wc.hIconSm			= LoadIcon(wc.hInstance, (LPCTSTR)iIcone);

	RegisterClassEx(&wc);

	//GetWindowRect(GetDesktopWindow(),&rc);


	//
	//	Create the main window. This window
	//  will host two child controls.
	//
	m_hWnd = CreateWindow(		// extended style (not needed)
				szAppName,				// window class name
				szAppName,				// window caption
				m_dWndStyle,
				rcPos.left,			// initial x position
				rcPos.top,			// initial y position
				rcPos.right,			// initial x size
				rcPos.bottom,			// initial y size
				hParent,					// parent window handle
				NULL,					// use window class menu
				hInstance,					// program instance handle
				this);					// creation parameters


	return 0;
}

int CLocalWindow::ShowWindow(int nCmdShow)
{
	return ::ShowWindow(m_hWnd, nCmdShow);
}

int CLocalWindow::UpdateWindow()
{
	return ::UpdateWindow(m_hWnd);
}

int CLocalWindow::CreateEx(HWND hParent, HINSTANCE hInstance,DWORD m_dWndStyle,
						 DWORD m_dStyle,DWORD m_dExStyle, const char * szAppName,int iIcone,int iMenu,LPCSTR iCursor,RECT rcPos)
{
	WNDCLASSEX	wc;
	m_hParent = hParent;
	//
	//	Register our main window class
	//
	wc.cbSize			= sizeof(wc);
	wc.style			= m_dStyle;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;

	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon (wc.hInstance, (LPCTSTR)iIcone);
	wc.hCursor			= LoadCursor(NULL, iCursor);
	wc.hbrBackground	= m_bkgndColor;
	wc.lpszMenuName		= (LPCSTR)iMenu;
	wc.lpszClassName	= szAppName;
	wc.hIconSm			= LoadIcon(wc.hInstance, (LPCTSTR)iIcone);

	RegisterClassEx(&wc);

	//
	//	Create the main window. This window
	//  will host two child controls.
	//
	m_hWnd = CreateWindowEx(m_dExStyle,		// extended style (not needed)
				szAppName,				// window class name
				szAppName,				// window caption
				m_dWndStyle,
				rcPos.left,			// initial x position
				rcPos.top,			// initial y position
				rcPos.right,			// initial x size
				rcPos.bottom,			// initial y size
				hParent,					// parent window handle
				NULL,					// use window class menu
				hInstance,					// program instance handle
				this);					// creation parameters

	return 0;
}


LRESULT CALLBACK CLocalWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_SYSCOMMAND:
		   {
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnSysCommand(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
		   }

		case WM_GETDLGCODE:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnGetDlgCode(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
			}

		case WM_USER_MEDIACHANGED:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnMediaChange(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
			}

		case WM_KILLFOCUS:
		   {
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnKillFocus(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
		   }

		case WM_ACTIVATE:
		   {
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnActivate(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
		   }

		case WM_SHOWWINDOW:
		   {
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{			
						return m_cTraitement->OnShowWindow(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
		   }

		case WM_ERASEBKGND:
		   {
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnEraseBackground(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
		   }


		case WM_TIMER:
		   {
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnTimer(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
		   }

		
	   case WM_CONTEXTMENU:
		   {
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnContextMenu(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);
					}
				}
				break;
		   }

		case WM_CREATE:
			{
				OleInitialize(NULL);
				if(lParam != 0)
				{
					CLocalWindow * m_cTraitement = (CLocalWindow *)(((LPCREATESTRUCT) lParam)->lpCreateParams);
					SetWindowLong (hWnd, GWL_USERDATA, (long) m_cTraitement);
					return m_cTraitement->OnCreate(hWnd, msg, wParam, lParam);
				}
				else
				{
					SetWindowLong (hWnd, GWL_USERDATA, NULL);
					return DefWindowProc(hWnd, msg, wParam, lParam);
				}
				break;
			}

			
		case WM_SETFOCUS:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnSetFocus(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
			}

	   case WM_PAINT:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnPaint(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
			}

		case WM_MEASUREITEM:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnMeasureItem(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
			}

		case WM_DRAWITEM:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnDrawItem(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
			}

		case WM_KEYUP:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnKeyUp(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
			}

		case WM_KEYDOWN:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnKeyDown(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
			}

		case WM_SIZE:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnSize(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
			}

		case WM_CLOSE:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnClose(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
			}

		case WM_COMMAND:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnCommand(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
			}

		case WM_NOTIFY:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnNotify(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
			}

		case WM_DESTROY:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						m_cTraitement->OnDestroy(hWnd, msg, wParam, lParam);
						OleUninitialize();
						SetWindowLong(hWnd,GWL_USERDATA,NULL);
						return 0;
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;

			}

		case WM_LBUTTONDOWN:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnLButtonDown(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
			}

		case WM_LBUTTONDBLCLK:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnDblClick(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
			}
			break;

		case WM_CHAR:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnChar(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
			}


		case WM_LBUTTONUP:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnLButtonUp(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
			}

		case WM_MOUSEMOVE:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnMouseMove(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
			}

		case WM_MOUSELEAVE:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnMouseLeave(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
			}
		

		case WM_MOUSEHOVER:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnMouseHover(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
			}


		case WM_NCMOUSEMOVE:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnNCMouseMove(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
			}

		case WM_MOVE:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnMove(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
			}

		case WM_MOUSEWHEEL:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnMouseWheel(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
			}

		case WM_HSCROLL:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnHScroll(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
			}

		case WM_VSCROLL:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnVScroll(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
			}

		case WM_RBUTTONDOWN:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnRButtonDown(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);;
					}
				}
				break;
			}

		case WM_RBUTTONUP:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{
						return m_cTraitement->OnRButtonUp(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);
					}
				}
				break;
			}

		case WM_WINDOWPOSCHANGING:
			{
				CLocalWindow * m_cTraitement = (CLocalWindow *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					try
					{

						return m_cTraitement->OnWindowPosChanging(hWnd, msg, wParam, lParam);
					}
					catch(...)
					{
						return DefWindowProc(hWnd, msg, wParam, lParam);
					}
				}
				break;
			}

	default:
		try
		{
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		catch(...)
		{
		}
		break;
	}

	return 0;
}

void CLocalWindow::SetBackgroundColor(HBRUSH m_bkgndColor)
{
	this->m_bkgndColor = m_bkgndColor;
}
