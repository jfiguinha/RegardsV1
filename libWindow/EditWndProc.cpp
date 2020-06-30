#include "stdafx.h"
#include ".\editwndproc.h"

CEditWndProc::CEditWndProc(void)
{
	m_hWnd = NULL;
}

CEditWndProc::~CEditWndProc(void)
{
	if(m_hWnd != NULL)
		DestroyWindow(m_hWnd);
}

////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////
int CEditWndProc::CreateEx(HWND hParent, DWORD dExStyle, DWORD dWndStyle, RECT rc)
{
	// create the edit box
	m_hWnd = CreateWindowEx(
		dExStyle,
		"EDIT",
		"",
		dWndStyle,
		rc.left, rc.top,
		rc.right-rc.left, rc.bottom-rc.top,
		hParent,
		0,
		hInstance, NULL);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////
LRESULT CEditWndProc::OnChar(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(wParam) 
	{
		case 27: // ESC
		{
			// edit is cancelled, so no LVN_ENDLABELEDIT is sent
			DestroyWindow(hwnd);
			break;
		}
		break;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////
LRESULT CEditWndProc::OnKeyDown(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
		case VK_ESCAPE:
			{
				m_iKey = -1;
				DestroyWindow(hwnd);
			}
			break;

		case VK_RETURN:
		{
			int iTypeAllocation = 1;
			// get current text
			int textLength = (int)SendMessage(hwnd, WM_GETTEXTLENGTH, 0, 0) + 1;
			char *newText = (char *)malloc(sizeof(char)*textLength);

			if(newText == NULL)
			{
				//Probléme d'allocation mémoire
				//Essai d'allocation dans le tas
				iTypeAllocation = 2;
				newText = (LPTSTR) HeapAlloc (GetProcessHeap (), 0, sizeof(char)*textLength);
			}


			SendMessage(hwnd, WM_GETTEXT, (WPARAM)textLength, (LPARAM)newText);
			
			// send LVN_ENDLABELEDIT via WM_NOTIFY to the main window
			LV_DISPINFO dispInfo;
			dispInfo.hdr.hwndFrom = GetParent();
			dispInfo.hdr.idFrom = 0; // listview has no ID since it's manually created
			dispInfo.hdr.code = LVN_ENDLABELEDIT;
			dispInfo.item.mask = LVIF_TEXT;
			dispInfo.item.iItem = m_EditRow;
			dispInfo.item.iSubItem = m_EditCol;
			dispInfo.item.pszText = newText;
			dispInfo.item.cchTextMax = strlen(newText);

			ListView_SetItemText(GetParent(), m_EditRow, m_EditCol, newText);
			
			switch(iTypeAllocation)
			{
				case 2:
					HeapFree(GetProcessHeap (), 0,newText);
					break;

				default:
					free(newText);
			}

			DestroyWindow(hwnd);

			break;
		}
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////
LRESULT CEditWndProc::OnLButtonDown(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// get current text
	int textLength = (int)SendMessage(hwnd, WM_GETTEXTLENGTH, 0, 0) + 1;
	char *newText = (char *)malloc(sizeof(char)*textLength);
	int iTypeAllocation = 1;

	if(newText == NULL)
	{
		//Probléme d'allocation mémoire
		//Essai d'allocation dans le tas
		iTypeAllocation = 2;
		newText = (LPTSTR) HeapAlloc (GetProcessHeap (), 0, sizeof(char)*textLength);
	}

	SendMessage(hwnd, WM_GETTEXT, (WPARAM)textLength, (LPARAM)newText);
	
	// send LVN_ENDLABELEDIT via WM_NOTIFY to the main window
	LV_DISPINFO dispInfo;
	dispInfo.hdr.hwndFrom = GetParent();
	dispInfo.hdr.idFrom = 0; // listview has no ID since it's manually created
	dispInfo.hdr.code = LVN_ENDLABELEDIT;
	
	dispInfo.item.mask = LVIF_TEXT;
	dispInfo.item.iItem = m_EditRow;
	dispInfo.item.iSubItem = m_EditCol;
	dispInfo.item.pszText = newText;
	dispInfo.item.cchTextMax = strlen(newText);

	ListView_SetItemText(GetParent(), m_EditRow,m_EditCol, newText);
	
	switch(iTypeAllocation)
	{
		case 2:
			HeapFree(GetProcessHeap (), 0,newText);
			break;

		default:
			free(newText);
	}
	// return focus to the parent
	SetFocus(GetParent());

	DestroyWindow(hwnd);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////
LRESULT CEditWndProc::OnGetDlgCode(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_iKey = 0;
	return DLGC_WANTALLKEYS;
}

////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////
LRESULT CEditWndProc::OnKillFocus(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_iKey != -1)
	{
		int iTypeAllocation = 1;
		// get current text
		int textLength = (int)SendMessage(hwnd, WM_GETTEXTLENGTH, 0, 0) + 1;
		char *newText = (char *)malloc(sizeof(char)*textLength);

		if(newText == NULL)
		{
			//Probléme d'allocation mémoire
			//Essai d'allocation dans le tas
			iTypeAllocation = 2;
			newText = (LPTSTR) HeapAlloc (GetProcessHeap (), 0, sizeof(char)*textLength);
		}

		SendMessage(hwnd, WM_GETTEXT, (WPARAM)textLength, (LPARAM)newText);
		
		// send LVN_ENDLABELEDIT via WM_NOTIFY to the main window
		LV_DISPINFO dispInfo;
		dispInfo.hdr.hwndFrom = GetParent();
		dispInfo.hdr.idFrom = 0; // listview has no ID since it's manually created
		dispInfo.hdr.code = LVN_ENDLABELEDIT;
		
		dispInfo.item.mask = LVIF_TEXT;
		dispInfo.item.iItem = m_EditRow;
		dispInfo.item.iSubItem = m_EditCol;
		dispInfo.item.pszText = newText;
		dispInfo.item.cchTextMax = strlen(newText);

		ListView_SetItemText(GetParent(), m_EditRow, m_EditCol, newText);
		
		switch(iTypeAllocation)
		{
			case 2:
				HeapFree(GetProcessHeap (), 0,newText);
				break;

			default:
				free(newText);
		}
		DestroyWindow(hwnd);
	}

	return 0;
}