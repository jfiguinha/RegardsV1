// CoolMenu.cpp: implementation of the CCoolMenu class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "CoolMenu.h"
#include "dessin.h"
#include "objet.h"

#define CXMENUADJUST 2		//2 pixel border to right of bitmap
#define CYMENUADJUST 2

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCoolMenu::CCoolMenu()
{
	iTypeAllocation = 0;
	mymi = NULL;
}

CCoolMenu::~CCoolMenu()
{
	if(mymi != NULL)
		HeapFree(GetProcessHeap(), 0, mymi);

	mymi = NULL;

}

//
//	Convert the specified menu popup into a "cool menu"
//	Bitmap can be any colour depth, but no palette support is provided
//	Better use 16 colour (4bit) for now
//	
BOOL CCoolMenu::MakeCoolMenu(HMENU hmenu, HINSTANCE hInst, LPCTSTR pBitmapName)
{
	HBITMAP hBitmap;
	BITMAP bm;
	HDC hdc;
	MENUITEMINFO mii;

	int nMaxWidthL = 0;
	int nMaxWidthR = 0;

	int menuheight = 0;		//start menu height at zero
	int bitmapheight;
	int bitmapwidth;
	
	int i, menucount = GetMenuItemCount(hmenu);
	HANDLE hOld;
	SIZE sz;
	

	iTypeAllocation = 1;

	//Allocate an ARRAY of custom menu structures. We will store
	//a pointer to each item in each menu's dwItemData area.
	mymi = (MyMenuItem *) HeapAlloc (GetProcessHeap (), 0, sizeof(MyMenuItem) * menucount);

	if((hBitmap = LoadBitmap(hInstance, pBitmapName)) == NULL)
	{
		return FALSE;
	}

	//get size of bitmap
	GetObject(hBitmap, sizeof bm, &bm);
	bitmapheight = bm.bmHeight;
	bitmapwidth  = bm.bmWidth;

	//store information for each menu item
	for(i = 0; i < menucount; i++)
	{
		mii.cbSize		= sizeof mii;
		mii.cch			= MAX_MENUITEM_TEXT;
		mii.fMask		= MIIM_TYPE;
		mii.fType		= MFT_STRING;
		mii.dwTypeData	= mymi[i].szText;

		//get the menu text
		if(!GetMenuItemInfo(hmenu, i, TRUE, &mii))
			continue;

		mymi[i].type = mii.fType;

		//if we have a separator, then make it narrow
		if(mii.fType & MF_SEPARATOR)
		{
			mymi[i].szText[0] = _T('\0');
			menuheight += GetSystemMetrics(SM_CYMENU) / 2;
		}
		else
		{
			menuheight += GetSystemMetrics(SM_CYMENU) + CYMENUADJUST;
		}

		//make each menu item owner-drawn
		mii.fMask = MIIM_TYPE | MIIM_DATA;
		mii.fType = MFT_OWNERDRAW;
		mii.dwItemData = (ULONG)&mymi[i];	
		SetMenuItemInfo(hmenu, i, TRUE, &mii);
	}

	hdc = GetDC(0);
	hOld = SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));

	// now work out the source y-coordinate of each item in the bitmap
	// also work out the width of each menu item
	for(i = 0; i < menucount; i++)
	{
		TCHAR *szText, *szPtr;
		
		mymi[i].hbmp_ref     = hBitmap;
		mymi[i].bitmapwidth  = bitmapwidth;
		mymi[i].bitmapy      = bitmapheight - menuheight;
		menuheight -= (mymi[i].type & MF_SEPARATOR) ? GetSystemMetrics(SM_CYMENU) / 2 : GetSystemMetrics(SM_CYMENU) + CYMENUADJUST;

		//we need to distinguish between the menu text,
		//and the accelerator text.
		szText = mymi[i].szText;
		szPtr  = strchr(szText, _T('\t'));
	
		//If there is a TAB character in the menu string, then the
		//text afterwards is the accelerator text, and needs to be
		//right aligned. In this case, we need to work out the maximum
		//size of the menu text, and the accelerator text, and keep them
		//separate.
		if(szPtr)
		{
			//calculate the size of the text (left hand side)
			GetTextExtentPoint32(hdc, szText, szPtr - szText, &sz);
			nMaxWidthL = max(nMaxWidthL, sz.cx);

			//calculate the size of the accelerator text (right aligned)
			GetTextExtentPoint32(hdc, szPtr + 1, lstrlen(szPtr + 1), &sz);
			nMaxWidthR = max(nMaxWidthR, sz.cx);
		}
		else
		{
			//if there is no accelerator, just work out the size of the text
			//as it is.
			GetTextExtentPoint32(hdc, szText, lstrlen(szText), &sz);
			nMaxWidthL = max(nMaxWidthL, sz.cx);
		}
	}

	SelectObject(hdc, hOld);
	ReleaseDC(0, hdc);

	//work out the longest string of text. We don't need to
	//set each item to the same value, because Windows would normally
	//work out the maximum size for us anyway. However, because
	//we need to support accelerator text, we have to do everything
	//ourselves.
	//The accelerators must all be aligned on the same position,
	//which is after the longest line of text on the LEFT. 
	for(i = 0; i < menucount; i++)
	{
		mymi[i].menuwidth = nMaxWidthL + nMaxWidthR + 4 + 
							GetSystemMetrics(SM_CXMENUCHECK) + bitmapwidth;
	}



	return TRUE;
}

void CCoolMenu::RemoveCoolMenu(HMENU hmenu)
{
	MENUITEMINFO mii;
	MyMenuItem *mymi;

	mii.cbSize = sizeof mii;
	mii.fMask = MIIM_DATA;
	GetMenuItemInfo(hmenu, 0, TRUE, &mii);

	mymi = (MyMenuItem *)mii.dwItemData;

	if(mymi != NULL)
	{
		
		//delete the bitmap
		if(CObjet::GetWindowsVersion())
			DeleteObject(mymi->hbmp_ref);
		
		//if(mymi != NULL)
		//	HeapFree(GetProcessHeap(), 0, mymi);

	}

	mymi = NULL;
	//HeapFree(GetProcessHeap(), 0, mymi);
}

LRESULT CCoolMenu::CoolMenu_MeasureItem(WPARAM wParam, LPARAM lParam)
{
	if(wParam == 0)
	{
		MEASUREITEMSTRUCT *mis = (MEASUREITEMSTRUCT *)lParam;
		
		MyMenuItem *mmi = (MyMenuItem *)mis->itemData;
		
		if(mmi->type & MFT_SEPARATOR)
		{
			mis->itemHeight = GetSystemMetrics(SM_CYMENU) / 2;
			mis->itemWidth  = 0;
		}
		else
		{
			mis->itemHeight = GetSystemMetrics(SM_CYMENU) + CYMENUADJUST;
			mis->itemWidth = mmi->menuwidth;
		}
	
		return TRUE;
	}

	return FALSE;
}

#define OBM_CHECK 32760 

void CCoolMenu::CoolMenu_DrawCheck(HDC hdc, RECT *rc, BOOL bSelected)
{
	HBITMAP hbmCheck;
	int cx, cy;
	BITMAP bm;
	RECT  rcDest = *rc;   
	POINT p;
	SIZE  delta;

	HDC memdc;
	HBITMAP hOldBM;
	COLORREF colorOld;

	hbmCheck = LoadBitmap(NULL, MAKEINTRESOURCE(OBM_CHECK));

	// center bitmap in caller's rectangle   
	GetObject(hbmCheck, sizeof bm, &bm);   
	cx = bm.bmWidth;
	cy = bm.bmHeight;   
	
	delta.cx = (rc->right-rc->left - cx) / 2;
	delta.cy = (rc->bottom-rc->top - cy) / 2;
	
	if(rc->right-rc->left > cx)
	{
		SetRect(&rcDest, rc->left+delta.cx, rc->top + delta.cy, 0, 0);   
		rcDest.right = rcDest.left + cx;
		rcDest.bottom = rcDest.top + cy;
		p.x = 0;
		p.y = 0;
	}
	else
	{
		p.x = -delta.cx;   
		p.y = -delta.cy;
	}
   
	// select checkmark into memory DC
	memdc = CreateCompatibleDC(hdc);
	hOldBM = (HBITMAP)SelectObject(memdc, hbmCheck);
   
	// set BG color based on selected state   
	colorOld = SetBkColor(hdc, RGB(229,238,222));
	BitBlt(hdc, rcDest.left, rcDest.top, rcDest.right-rcDest.left, rcDest.bottom-rcDest.top, memdc, p.x, p.y, SRCCOPY);
	
	// restore
	SetBkColor(hdc, colorOld);
	SelectObject(memdc, hOldBM);	
	DeleteDC(memdc);
	
	// draw pushed-in highlight.
#ifdef MENUCHECKBORDERS
    InflateRect(&rcDest, 0,-1);
	DrawEdge(hdc, &rcDest, BDR_SUNKENOUTER, BF_RECT);
#endif
}

int CCoolMenu::GreyText1(HDC hdc, LPSTR str, int count, RECT *rect, UINT flag, LPDRAWTEXTPARAMS lp)
{
	return DrawTextEx(hdc, str, count, rect, flag, lp);
}

LRESULT CCoolMenu::CoolMenu_DrawItem(WPARAM wParam, LPARAM lParam)
{
	HDC hdcmem;
	HBITMAP hbmp;
	HDC hold;
	RECT rect;
	COLORREF clrBord = RGB(230,230,230);
	COLORREF clrFond = RGB(249,248,247);
	COLORREF clrRect = RGB(179,185,230);
	COLORREF clrFondRect = RGB(199,205,250);

	if(wParam == 0)
	{
		DRAWITEMSTRUCT *dis = (DRAWITEMSTRUCT *)lParam;
		MyMenuItem *mii = (MyMenuItem *)dis->itemData;

		//rect.left might not be zero, because of multi-column menus!
		rect = dis->rcItem;
		rect.right = rect.left + mii->bitmapwidth;
		
		hdcmem = CreateCompatibleDC(0);
		hbmp = mii->hbmp_ref;//LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
		hold = (HDC)SelectObject(hdcmem, mii->hbmp_ref);
		
		if(mii->bitmapy < 0)
		{
			RECT r = dis->rcItem;
			r.right = mii->bitmapwidth;
			FillRect(dis->hDC, &r,  CreateSolidBrush(clrBord));
		}
		else
			BitBlt(dis->hDC, rect.left, rect.top, mii->bitmapwidth, rect.bottom-rect.top, hdcmem, 0, mii->bitmapy, SRCCOPY);

		SelectObject(hdcmem, hold);
		DeleteDC(hdcmem);

		//if a separator
		if(mii->type & MFT_SEPARATOR)
		{
			rect.left = rect.right;
			rect.right = dis->rcItem.right;
			FillRect(dis->hDC, &rect, CreateSolidBrush(clrFond)); 
			rect.top += (rect.bottom-rect.top) / 2 - 1;
			DrawEdge(dis->hDC, &rect, EDGE_ETCHED, BF_TOP);
		}
		else
		{
			TCHAR *tabptr;
			int right;
			RECT rcTemp;

			rect.left = rect.right;
			right = rect.right;
			rect.right = dis->rcItem.right;
			
			if(dis->itemState & ODS_SELECTED)
			{
				if(dis->itemState & ODS_GRAYED)
					SetTextColor(dis->hDC, clrFond);//GetSysColor(COLOR_MENU)); 
				else
					SetTextColor(dis->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT)); 


				SetBkColor(dis->hDC, clrFondRect);//GetSysColor(COLOR_HIGHLIGHT));    
				SetBkMode(dis->hDC, OPAQUE);

				
			}
			else
			{
				if(dis->itemState & ODS_GRAYED)
					SetTextColor(dis->hDC, GetSysColor(COLOR_3DHIGHLIGHT)); 
				else
					SetTextColor(dis->hDC, GetSysColor(COLOR_MENUTEXT)); 

				SetBkColor(dis->hDC, clrFond);//GetSysColor(COLOR_MENU));  
	
			}

			rect.left = 0;


			if(dis->itemState & ODS_SELECTED)
			{
				rcTemp = rect;
				rcTemp.left += 2;
				rcTemp.right -= 2;
				rcTemp.top += 1;
				rcTemp.bottom -= 2;

				ExtTextOut(dis->hDC, 0, 0, ETO_OPAQUE, &rcTemp, 0, 0, 0);	


				HPEN hpen, hpenOld;
				HBRUSH hbrush, hbrushOld;

				// Create a green pen.
				hpen = CreatePen(PS_SOLID, 1, clrRect);
				// Create a red brush.
				hbrush = CreateSolidBrush(clrRect);

				// Select the new pen and brush, and then draw.
				hpenOld = (HPEN)SelectObject(dis->hDC, hpen);
				hbrushOld = (HBRUSH)SelectObject(dis->hDC, hbrush);

				MoveToEx(dis->hDC,rcTemp.left,rcTemp.top,NULL);
				LineTo(dis->hDC,rcTemp.left,rcTemp.bottom);
				LineTo(dis->hDC,rcTemp.right,rcTemp.bottom);
				LineTo(dis->hDC,rcTemp.right,rcTemp.top);
				LineTo(dis->hDC,rcTemp.left,rcTemp.top);

				// Do not forget to clean up.
				DeleteObject(hpen);
				DeleteObject(hbrush);
			}
			else
			{
				ExtTextOut(dis->hDC, 0, 0, ETO_OPAQUE, &rect, 0, 0, 0);
			}

			//FillRect(hdc, &rcBmp, hbrBkgnd);
			//Rectangle(dis->hDC

			rcTemp = rect;
			rcTemp.right = right;

			//
			
			rect.left += right;
		
			rect.left += GetSystemMetrics(SM_CXMENUCHECK) + 1 + CXMENUADJUST;
			rect.top  += 1;

			//search for a TAB character in the menu text. If we find
			//one, then this item has a menu accelerator
			if((tabptr = strchr(mii->szText, _T('\t'))) != 0)
			{
				

				//need to alter this code. Presently, accelerators
				//are right-aligned. Really, they should be left-aligned, but
				//on the right-hand side, after the normal menu text.
				if(dis->itemState & ODS_SELECTED || !(dis->itemState & ODS_GRAYED))
				{
					if(!(dis->itemState & ODS_SELECTED))
						FillRect(dis->hDC,&rcTemp,CreateSolidBrush(clrBord));

					DrawTextEx(dis->hDC, mii->szText,  tabptr-mii->szText, &rect, DT_SINGLELINE|DT_VCENTER|DT_EXPANDTABS, 0);
					rect.right -= GetSystemMetrics(SM_CXMENUCHECK);
					DrawTextEx(dis->hDC, tabptr+1, -1, &rect, DT_RIGHT|DT_SINGLELINE|DT_VCENTER|DT_EXPANDTABS, 0);
				}
				else
				{
					FillRect(dis->hDC,&rcTemp,CreateSolidBrush(clrBord));

					OffsetRect(&rect, 1, 1);

					SetTextColor(dis->hDC, GetSysColor(COLOR_3DHIGHLIGHT));
					DrawTextEx(dis->hDC, mii->szText,  tabptr-mii->szText, &rect, DT_SINGLELINE|DT_VCENTER|DT_EXPANDTABS, 0);
					rect.right -= GetSystemMetrics(SM_CXMENUCHECK);
					DrawTextEx(dis->hDC, tabptr+1, -1, &rect, DT_RIGHT|DT_SINGLELINE|DT_VCENTER|DT_EXPANDTABS, 0);

					rect.right += GetSystemMetrics(SM_CXMENUCHECK);
					OffsetRect(&rect, -1, -1);
					SetBkMode(dis->hDC, TRANSPARENT);

					SetTextColor(dis->hDC, GetSysColor(COLOR_3DSHADOW));
					DrawTextEx(dis->hDC, mii->szText,  tabptr-mii->szText, &rect, DT_SINGLELINE|DT_VCENTER|DT_EXPANDTABS, 0);
					rect.right -= GetSystemMetrics(SM_CXMENUCHECK);
					DrawTextEx(dis->hDC, tabptr+1, -1, &rect, DT_RIGHT|DT_SINGLELINE|DT_VCENTER|DT_EXPANDTABS, 0);

					SetBkMode(dis->hDC, OPAQUE);
				}

			}
			//otherwise we have a standard menu item
			else
			{
				if(dis->itemState & ODS_SELECTED || !(dis->itemState & ODS_GRAYED))
				{
					if(!(dis->itemState & ODS_SELECTED))
						FillRect(dis->hDC,&rcTemp,CreateSolidBrush(clrBord));

					DrawTextEx(dis->hDC, mii->szText, -1, &rect, DT_SINGLELINE|DT_VCENTER|DT_EXPANDTABS, 0);
				}
				else if(dis->itemState & ODS_GRAYED)
				{
					FillRect(dis->hDC,&rcTemp,CreateSolidBrush(clrBord));

					OffsetRect(&rect, 1, 1);
					SetTextColor(dis->hDC, GetSysColor(COLOR_3DHIGHLIGHT));
					DrawTextEx(dis->hDC, mii->szText, -1, &rect, DT_SINGLELINE|DT_VCENTER|DT_EXPANDTABS, 0);
					OffsetRect(&rect, -1, -1);
					SetBkMode(dis->hDC, TRANSPARENT);
					SetTextColor(dis->hDC, GetSysColor(COLOR_3DSHADOW));
					DrawTextEx(dis->hDC, mii->szText, -1, &rect, DT_SINGLELINE|DT_VCENTER|DT_EXPANDTABS, 0);
					SetBkMode(dis->hDC, OPAQUE);
				}
				
			}

			//finally draw the check mark
			if(MF_CHECKED & GetMenuState((HMENU)dis->hwndItem, dis->itemID, MF_BYCOMMAND))
			{
				if(!(dis->itemState & ODS_SELECTED))
					FillRect(dis->hDC,&rcTemp,CreateSolidBrush(clrBord));

				RECT r = dis->rcItem;
				r.left += mii->bitmapwidth + CXMENUADJUST;
				r.right = r.left + GetSystemMetrics(SM_CXMENUCHECK);
				
				CoolMenu_DrawCheck(dis->hDC, &r, (dis->itemState & ODS_SELECTED) == TRUE);
			}
		}
		
		return TRUE;
	}
	return FALSE;
}