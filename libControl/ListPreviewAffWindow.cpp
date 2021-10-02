// ListPreviewAffWindow.cpp: implementation of the CListPreviewAffWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ListPreviewAffWindow.h"
#include "objet.h"
#include "listpreview.h"
#include <ItemInfo.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CListPreviewAffWindow::CListPreviewAffWindow()
{
	iOldYPos = 0;
	m_cDirectory = NULL;
	m_hWnd = NULL;
	m_ListPreview = new CListPreview();
	m_ListPreview->SetShowFolder(false);

	iItem = 0;
	m_hWndNavigue = NULL;
}

CListPreviewAffWindow::~CListPreviewAffWindow()
{
	if(m_ListPreview != NULL)
		delete m_ListPreview;
	
	m_ListPreview = NULL;
}


int CListPreviewAffWindow::Run()
{
	MSG msg;

	while(GetMessage(&msg, NULL, 0, 0))			// Is There A Message Waiting?
	{
		TranslateMessage(&msg);				// Translate The Message
		DispatchMessage(&msg);				// Dispatch The Message
	}

	return 0;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void CListPreviewAffWindow::DesHighlightItem(HWND hWnd,const int &iNumItem,const bool &m_bFlip)
{
	LVITEM pitem;

	ZeroMemory(&pitem,sizeof(LVITEM));

	pitem.iSubItem = 0;
	pitem.mask = LVIF_PARAM | LVIF_STATE;
	pitem.iItem = iNumItem;

	ListView_GetItem(hWnd, &pitem);

	if(!m_bFlip)
	{
		pitem.mask |= LVIF_STATE;
		pitem.stateMask |= LVIS_OVERLAYMASK  | LVIS_SELECTED;
		pitem.state = INDEXTOOVERLAYMASK(1); //1 is the index for the shared overlay image

		ListView_SetItem(hWnd, &pitem);
	}
	else
	{
		ListView_SetItemState(hWnd, iNumItem, 0, LVIS_OVERLAYMASK);
		//ListView_EnsureVisible(hWnd, iNumItem,true);
	}
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
LRESULT CListPreviewAffWindow::OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	m_hWnd = hWnd;
	m_hWndNavigue = GetParent();
	m_ListPreview->SetShowFolder(false);
	m_ListPreview->DefineThumbnailSize(90,70);
	m_ListPreview->CreateListView(hWnd,hInstance);
	m_ListPreview->SetAffichageAlaVolee(1);
	m_ListPreview->InitList(m_cDirectory);
	m_ListPreview->ShowWindow(SW_SHOW);		

	iItem = -1;
	
	return 0;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
LRESULT CListPreviewAffWindow::OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	wmId    = LOWORD(wParam); 
	wmEvent = HIWORD(wParam); 
	switch (wmId)
	{
		case WM_MOVE:
			{
				RECT rc;
				GetWindowRect(m_hWndNavigue,&rc);
				MoveWindow(hWnd, rc.right, rc.top, 140, rc.bottom - rc.top, TRUE);
			}
			break;

		case IDM_FILENAME:
			{
				char Temp[255];
				LVFINDINFO plvfi;
				ZeroMemory(&plvfi,sizeof(LVFINDINFO));

				TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
				_splitpath_s((const char *)lParam, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

				//_splitpath((const char *)lParam, drive, dir, fname, ext);

				StringCchPrintf(Temp,255,"%s%s",fname, ext);

				plvfi.flags = LVFI_STRING;
				plvfi.psz = Temp;

				int i = 0;
				i = ListView_FindItem(m_ListPreview->m_hWnd,-1,&plvfi);
				DesHighlightItem(m_ListPreview->m_hWnd,iItem,true);

				iItem = i;

				//POINT pt;

				
				LVITEM pitem;

				ZeroMemory(&pitem,sizeof(LVITEM));

				pitem.iSubItem = 0;
				pitem.mask = LVIF_PARAM | LVIF_STATE;
				pitem.iItem = iItem;

				ListView_GetItem(m_ListPreview->m_hWnd, &pitem);

				pitem.mask |= LVIF_STATE;
				pitem.stateMask |= LVIS_OVERLAYMASK | LVIS_SELECTED;
				pitem.state = INDEXTOOVERLAYMASK(1); //1 is the index for the shared overlay image

				ListView_SetItem(m_ListPreview->m_hWnd, &pitem);	

				m_ListPreview->List_EnsureVisible(iItem,iOldYPos);

				iOldYPos = iItem;

			}
			break;
	}

	return 0;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
LRESULT CListPreviewAffWindow::OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (((LPNMHDR)lParam)->code)
	{

			case LVN_KEYDOWN:
				{
					LPNMLVKEYDOWN pnkd;
					int iItemTemp;
					pnkd = (LPNMLVKEYDOWN) lParam; 


					switch(pnkd->wVKey)
					{
						case VK_UP:
							iItemTemp = ListView_GetNextItem(m_ListPreview->m_hWnd,iItem,LVNI_ABOVE);
							break;
						case VK_DOWN:
							iItemTemp = ListView_GetNextItem(m_ListPreview->m_hWnd,iItem,LVNI_BELOW);
							break;
					}

					switch(pnkd->wVKey)
					{
						case VK_UP:
						case VK_DOWN:
							{
								if(iItemTemp == -1)
									return 0;
								else
								{
									DesHighlightItem(m_ListPreview->m_hWnd,iItem,true);
									iItem = iItemTemp;
								}
								break;
							}
					}



					switch(pnkd->wVKey)
					{
						case VK_UP:
						case VK_DOWN:
							{
								ListView_SetItemState(m_ListPreview->m_hWnd, iItem, LVIS_SELECTED, 0);	
								DesHighlightItem(m_ListPreview->m_hWnd,iItem,false);
								char szFileNameTemp[MAX_PATH];
								string m_stgFilename;
								m_ListPreview->List_GetItemText(iItem, szFileNameTemp,MAX_PATH);
								SendMessage(m_hWnd,WM_COMMAND,MAKEWPARAM(IDM_RECEIVEIMAGE_PREVIEW,0),(LPARAM)szFileNameTemp);
								ListView_EnsureVisible(m_ListPreview->m_hWnd, iItem,true);
							}
							break;
					}
					return 1;
				}
				break;

		   case LVN_GETDISPINFO:
			   {
				   NMLVDISPINFO   *pdi = (NMLVDISPINFO*)lParam;
				   CItemInfo * pItem = (CItemInfo *)pdi->item.lParam;
					    				   
				   m_ListPreview->List_Notify(lParam);

			  }
			  break;

			case NM_DBLCLK:
			case NM_CLICK:
				{
					char szFileNameTemp[MAX_PATH];
					string m_stgFilename;
					m_ListPreview->List_GetItemText(iItem, szFileNameTemp,MAX_PATH);
					SendMessage(this->m_hWndNavigue,WM_COMMAND,MAKEWPARAM(IDM_RECEIVEIMAGE_PREVIEW,0),(LPARAM)szFileNameTemp);
				}
				break;

			case LVN_ITEMCHANGED:
				{
					
					LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE) lParam;
					
					if(lpnmitem->uNewState == 3 && iItem != lpnmitem->iItem)
					{
						if(iItem != -1)
							DesHighlightItem(m_ListPreview->m_hWnd,iItem,true);		

						iItem = lpnmitem->iItem;

						DesHighlightItem(m_ListPreview->m_hWnd,iItem,false);
					}
				}
				break;
	}
	return 0;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
LRESULT CListPreviewAffWindow::OnSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int y;
	RECT rc,rc2;
	GetWindowRect(hWnd,&rc);
	MoveWindow(m_ListPreview->m_hWnd, 0, 0, rc.right - rc.left - 5, rc.bottom - rc.top - 30, TRUE);
	
	ListView_GetItemRect(m_ListPreview->m_hWnd,0,&rc2,LVIR_BOUNDS);

	GetClientRect(m_ListPreview->m_hWnd,&rc);

	y = (rc.bottom - rc.top) / (rc2.bottom - rc2.top);

	m_ListPreview->SetNbMaxMiniature(y);
	return 0;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
LRESULT CListPreviewAffWindow::OnDestroy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	iOldYPos = 0;
	PostQuitMessage(0);
	return 0;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
LRESULT CListPreviewAffWindow::OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef _DEBUG
	OutputDebugString("CListPreviewAffWindow::OnPaint");
#endif
	PAINTSTRUCT ps;
	RECT rc;
	HDC hDC = BeginPaint( hWnd, &ps );
	SendMessage(m_ListPreview->m_hWnd,message,wParam,lParam);
	EndPaint(hWnd, &ps);
	ReleaseDC(hWnd,hDC);
	return 0;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
LRESULT CListPreviewAffWindow::OnEraseBackground(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef _DEBUG
	OutputDebugString("CListPreviewAffWindow::OnEraseBackground");
#endif
	SendMessage(m_ListPreview->m_hWnd,message,wParam,lParam);
	return 0;
}

