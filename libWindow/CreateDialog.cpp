#include "stdafx.h"
#include ".\createdialog.h"

CCreateDialog::CCreateDialog(void)
{
}

CCreateDialog::~CCreateDialog(void)
{
}

HWND CCreateDialog::CreateDlg(HINSTANCE hInstance, LPCTSTR lpTemplate, HWND hWndParent)
{
	m_hWnd = CreateDialogParam(hInstance, lpTemplate, hWndParent,WndProc,(LPARAM)this);
	return m_hWnd;
}

int __stdcall CCreateDialog::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_INITDIALOG:
			{
				OleInitialize(NULL);
				CCreateDialog * m_cTraitement = (CCreateDialog *)(lParam);
				SetWindowLong (hWnd, GWL_USERDATA, (long) m_cTraitement);
				return m_cTraitement->OnInitDialog(hWnd, msg, wParam, lParam);
			}

			
		case WM_SYSCOMMAND:
		   {
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnSysCommand(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
		   }

		case WM_ACTIVATE:
		   {
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnActivate(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
		   }

		case WM_SHOWWINDOW:
		   {
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnShowWindow(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
		   }

		case WM_LBUTTONDBLCLK:
			{
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnDblClick(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_ERASEBKGND:
		   {
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnEraseBackground(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
		   }


		case WM_TIMER:
		   {
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnTimer(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
		   }

		
	   case WM_CONTEXTMENU:
		   {
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnContextMenu(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
		   }

			
		case WM_SETFOCUS:
			{
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnSetFocus(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

	   case WM_PAINT:
			{
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnPaint(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_MEASUREITEM:
			{
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnMeasureItem(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_DRAWITEM:
			{
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnDrawItem(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_KEYUP:
			{
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnKeyUp(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_KEYDOWN:
			{
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnKeyDown(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_SIZE:
			{
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnSize(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_CLOSE:
			{
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnClose(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_COMMAND:
			{
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnCommand(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_NOTIFY:
			{
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnNotify(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_DESTROY:
			{
				LRESULT lResult = 0;
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				{
					lResult = m_cTraitement->OnDestroy(hWnd, msg, wParam, lParam);
					OleUninitialize();
					SetWindowLong(hWnd,GWL_USERDATA,NULL);
				}
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
				return lResult;
			}

		case WM_LBUTTONDOWN:
			{
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnLButtonDown(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_LBUTTONUP:
			{
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnLButtonUp(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_MOUSEMOVE:
			{
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnMouseMove(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_MOUSELEAVE:
			{
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnMouseLeave(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}
		

		case WM_MOUSEHOVER:
			{
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnMouseHover(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}


		case WM_NCMOUSEMOVE:
			{
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
				return m_cTraitement->OnNCMouseMove(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_MOVE:
			{
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnMove(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_MOUSEWHEEL:
			{
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnMouseWheel(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_HSCROLL:
			{
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnHScroll(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_VSCROLL:
			{
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnVScroll(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_RBUTTONDOWN:
			{
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnRButtonDown(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		case WM_RBUTTONUP:
			{
				CCreateDialog * m_cTraitement = (CCreateDialog *) GetWindowLong (hWnd, GWL_USERDATA);
				if(m_cTraitement != NULL)
					return m_cTraitement->OnRButtonUp(hWnd, msg, wParam, lParam);
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}

	}

	return 0;
}
