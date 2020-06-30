#include "stdafx.h"
#include ".\albumcriterenotify.h"
#include "..\resource.h"
#include <EditWndProc.h>

CAlbumCritereNotify::EDITDATA CAlbumCritereNotify::m_EditData;

CAlbumCritereNotify::CAlbumCritereNotify(HINSTANCE hInstance)
{
	hInstanceLocal = hInstance;
	ZeroMemory(&m_EditData,sizeof(EDITDATA));
}

CAlbumCritereNotify::~CAlbumCritereNotify(void)
{
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
LRESULT CAlbumCritereNotify::OnKeyDown(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPNMLVKEYDOWN pnkd;
	pnkd = (LPNMLVKEYDOWN) lParam;
	switch(pnkd->wVKey)
	{
		case VK_RETURN:
		case VK_F2:
			m_iSelectItem = SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_GETNEXTITEM,(WPARAM) -1, MAKELPARAM((UINT) LVNI_SELECTED, 0));
			if(m_iSelectItem != -1)
			{
				int m_iValue;
				m_iValue = SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_EDITLABEL ,(WPARAM) m_iSelectItem, 0);
			}
			break;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
LRESULT CAlbumCritereNotify::OnClick(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPNMITEMACTIVATE lpnmitem;
	lpnmitem = (LPNMITEMACTIVATE) lParam;
	m_iSelectItem = lpnmitem->iItem;

	LV_ITEM m_LVITEM;
	m_LVITEM.state = LVIS_FOCUSED | LVIS_SELECTED | LVIS_DROPHILITED;
	m_LVITEM.stateMask  = LVIS_SELECTED;
	
	SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_SETITEMSTATE,(WPARAM)m_iSelectItem, (LPARAM)&m_LVITEM);

	return 0;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
LRESULT CAlbumCritereNotify::OnDblClick(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPNMITEMACTIVATE lpnmitem;
	lpnmitem = (LPNMITEMACTIVATE) lParam;

	hLV = GetDlgItem(hDlg,IDC_LISTATTRIBUT);

	int iNumItem = SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_GETITEMCOUNT ,0,0);

	LVHITTESTINFO hti;
	memset(&hti, 0, sizeof(hti));

	hti.pt = lpnmitem->ptAction;
	ListView_SubItemHitTest(hLV, &hti);

	m_EditData.editRow = hti.iItem;
	m_EditData.editCol = hti.iSubItem;

	// save these in globals so the edit box can use them

	if(m_iNumNotEdit == hti.iSubItem)
		return 0;

	// onlySelected=true if the item clicked is selected, and no others are
	bool onlySelected = (ListView_GetItemState(hLV, hti.iItem, LVIS_SELECTED) != 0);
	for (int item=0; item<iNumItem; item++)
	{
		if (item!=hti.iItem && ListView_GetItemState(hLV, item, LVIS_SELECTED)!=0)
		{
			onlySelected = false;
		}
	}
	//bool ctrlKeyDown = ((wParam&MK_CONTROL) != 0);
	//bool shiftKeyDown = ((wParam&MK_SHIFT) != 0);

	if (onlySelected)// && !ctrlKeyDown && !shiftKeyDown)
	{
		// deselect all items
		char Temp[255];
		ListView_SetItemState(hLV, -1, 0, LVIS_SELECTED);

		// get the bounding box for the field to be edited
		RECT bounds;
		memset(&bounds, 0, sizeof(bounds));
		ListView_GetSubItemRect(hLV, hti.iItem, hti.iSubItem, LVIR_LABEL, &bounds);

		// create the edit box
		hEdit = CreateWindowEx(
			0,
			"EDIT",
			"",
			WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL,
			bounds.left, bounds.top,
			bounds.right-bounds.left, bounds.bottom-bounds.top,
			hLV,
			0,
			hInstanceLocal,
			&m_EditData);

		// subclass
		
		m_EditData.origEditWndProc = (WNDPROC)SetWindowLong(hEdit, GWL_WNDPROC, (DWORD)EditWndProc);

		// use the default GUI font, not the default system (fixed) font
		SendMessage(hEdit, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), FALSE);
		
		// set edit box text to what that field's value is
		ListView_GetItemText(hLV,hti.iItem, hti.iSubItem, Temp,255);

		SendMessage(hEdit, WM_SETTEXT, FALSE, (LPARAM)Temp);
		
		// select all text in the edit box
		SendMessage(hEdit, EM_SETSEL, (WPARAM)(INT)0, (LPARAM)(INT)-1);

		ShowWindow (hEdit, SW_SHOWNORMAL);
		UpdateWindow(hEdit);
		SetFocus(hEdit);
	} 
	else 
	{
		LPNMITEMACTIVATE lpnmitem;
		lpnmitem = (LPNMITEMACTIVATE) lParam;
		m_iSelectItem = lpnmitem->iItem;
		SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_EDITLABEL ,(WPARAM)m_iSelectItem, 0);
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CAlbumCritereNotify::OnBeginLabelEdit(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	NMLVDISPINFO * pdi;
	pdi = (NMLVDISPINFO *) lParam;
	m_LVItem = pdi->item;
	m_LVItem.pszText = Libelle;
	m_LVItem.cchTextMax = 255;
	SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_GETITEMTEXT,(WPARAM) m_iSelectItem,(LPARAM) (LPLVITEM) &m_LVItem);
	m_iSelectItem = SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_GETNEXTITEM,(WPARAM) -1, MAKELPARAM((UINT) LVNI_SELECTED, 0));		
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CAlbumCritereNotify::OnEndLabelEdit(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	NMLVDISPINFO * pdi;
	pdi = (NMLVDISPINFO *) lParam;
	if(pdi->item.cchTextMax == 0)
		SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_SETITEMTEXT , (WPARAM)m_iSelectItem, (LPARAM)(LVITEM *)&m_LVItem);
	else
		SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_SETITEMTEXT , (WPARAM)m_iSelectItem, (LPARAM)(LVITEM *)&pdi->item);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////////
void CAlbumCritereNotify::TraitementMessage(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPNMHDR pnmh = (LPNMHDR) lParam; 

	switch (pnmh->code)
	{

		case LVN_KEYDOWN:
			this->OnKeyDown(hDlg, uMsg, wParam, lParam);
			break;

		case NM_CLICK:
			this->OnClick(hDlg, uMsg, wParam, lParam);
			break;

		case NM_DBLCLK:
			this->OnDblClick(hDlg, uMsg, wParam, lParam);
			break;

		case LVN_BEGINLABELEDIT:
			this->OnBeginLabelEdit(hDlg, uMsg, wParam, lParam);
			break;

		case LVN_ENDLABELEDIT:
			this->OnEndLabelEdit(hDlg, uMsg, wParam, lParam);
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK CAlbumCritereNotify::EditWndProc(HWND hwnd, UINT mMsg, WPARAM wParam, LPARAM lParam)
{

	switch (mMsg)
	{

	case WM_CHAR:
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

	case WM_KEYDOWN:
	{
		switch (wParam)
		{
			case VK_ESCAPE:
				{
					m_EditData.iKey = -1;
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
				dispInfo.hdr.hwndFrom = GetParent(hwnd);
				dispInfo.hdr.idFrom = 0; // listview has no ID since it's manually created
				dispInfo.hdr.code = LVN_ENDLABELEDIT;
				
				dispInfo.item.mask = LVIF_TEXT;
				dispInfo.item.iItem = m_EditData.editRow;
				dispInfo.item.iSubItem = m_EditData.editCol;
				dispInfo.item.pszText = newText;
				dispInfo.item.cchTextMax = strlen(newText);

				ListView_SetItemText(GetParent(hwnd), m_EditData.editRow, m_EditData.editCol, newText);
				
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
		break;
	}
	break;

	case WM_LBUTTONDOWN:
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
			dispInfo.hdr.hwndFrom = GetParent(hwnd);
			dispInfo.hdr.idFrom = 0; // listview has no ID since it's manually created
			dispInfo.hdr.code = LVN_ENDLABELEDIT;
			
			dispInfo.item.mask = LVIF_TEXT;
			dispInfo.item.iItem = m_EditData.editRow;
			dispInfo.item.iSubItem = m_EditData.editCol;
			dispInfo.item.pszText = newText;
			dispInfo.item.cchTextMax = strlen(newText);

			ListView_SetItemText(GetParent(hwnd), m_EditData.editRow,m_EditData.editCol, newText);
			
			switch(iTypeAllocation)
			{
				case 2:
					HeapFree(GetProcessHeap (), 0,newText);
					break;

				default:
					free(newText);
			}
			// return focus to the parent
			SetFocus(GetParent(hwnd));

			DestroyWindow(hwnd);
		}
		break;

	case WM_GETDLGCODE:
		{
			m_EditData.iKey = 0;
		}
		return DLGC_WANTALLKEYS;


	case WM_KILLFOCUS:
		{
			if(m_EditData.iKey != -1)
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
				dispInfo.hdr.hwndFrom = GetParent(hwnd);
				dispInfo.hdr.idFrom = 0; // listview has no ID since it's manually created
				dispInfo.hdr.code = LVN_ENDLABELEDIT;
				
				dispInfo.item.mask = LVIF_TEXT;
				dispInfo.item.iItem = m_EditData.editRow;
				dispInfo.item.iSubItem = m_EditData.editCol;
				dispInfo.item.pszText = newText;
				dispInfo.item.cchTextMax = strlen(newText);

				ListView_SetItemText(GetParent(hwnd), m_EditData.editRow, m_EditData.editCol, newText);
				
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
		}
		break;

	}

	return CallWindowProc(m_EditData.origEditWndProc, hwnd, mMsg, wParam, lParam);
}
