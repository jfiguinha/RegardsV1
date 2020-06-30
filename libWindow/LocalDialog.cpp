// Dialog.cpp: implementation of the CLocalDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LocalDialog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLocalDialog::CLocalDialog()
{

}

CLocalDialog::~CLocalDialog()
{

}


int CLocalDialog::CreateDlg(HINSTANCE hInstance, LPCTSTR lpTemplate, HWND hWndParent)
{
	return DialogBoxParam(hInstance, lpTemplate, hWndParent,WndProc,(LPARAM)this);
}

int __stdcall CLocalDialog::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_INITDIALOG:
			{
				OleInitialize(NULL);
				CLocalDialog * m_cTraitement = (CLocalDialog *)(lParam);
				SetWindowLong (hWnd, GWL_USERDATA, (long) m_cTraitement);
				return m_cTraitement->OnInitDialog(hWnd, msg, wParam, lParam);
			}

			
		case WM_SYSCOMMAND:
		   {
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnSysCommand(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
		   }

		case WM_ACTIVATE:
		   {
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnActivate(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
		   }

		case WM_SHOWWINDOW:
		   {
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnShowWindow(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
		   }

		case WM_LBUTTONDBLCLK:
			{
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnDblClick(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_ERASEBKGND:
		   {
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnEraseBackground(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
		   }


		case WM_TIMER:
		   {
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnTimer(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
		   }

		
	   case WM_CONTEXTMENU:
		   {
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnContextMenu(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
		   }

			
		case WM_SETFOCUS:
			{
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnSetFocus(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

	   case WM_PAINT:
			{
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnPaint(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_MEASUREITEM:
			{
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnMeasureItem(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_DRAWITEM:
			{
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnDrawItem(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_KEYUP:
			{
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnKeyUp(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_KEYDOWN:
			{
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnKeyDown(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_SIZE:
			{
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnSize(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_CLOSE:
			{
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnClose(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_COMMAND:
			{
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnCommand(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_NOTIFY:
			{
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnNotify(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_DESTROY:
			{
				LRESULT lResult = 0;
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					lResult = m_cTraitement->OnDestroy(hWnd, msg, wParam, lParam);
					SetWindowLong(hWnd,GWL_USERDATA,NULL);
					OleUninitialize();
				}
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);

				return lResult;
			}

		case WM_LBUTTONDOWN:
			{
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnLButtonDown(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_LBUTTONUP:
			{
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnLButtonUp(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_MOUSEMOVE:
			{
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnMouseMove(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_MOUSELEAVE:
			{
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnMouseLeave(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}
		

		case WM_MOUSEHOVER:
			{
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnMouseHover(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}


		case WM_NCMOUSEMOVE:
			{
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnNCMouseMove(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_MOVE:
			{
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnMove(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_MOUSEWHEEL:
			{
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnMouseWheel(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_HSCROLL:
			{
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnHScroll(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_VSCROLL:
			{
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnVScroll(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_RBUTTONDOWN:
			{
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnRButtonDown(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_RBUTTONUP:
			{
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnRButtonUp(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_GETDLGCODE:
			{
				CLocalDialog * m_cTraitement = (CLocalDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnGetDlgCode(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

	}

	return 0;
}