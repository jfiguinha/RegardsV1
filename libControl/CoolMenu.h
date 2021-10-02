// CoolMenu.h: interface for the CCoolMenu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COOLMENU_H__6E9B840A_D384_4B0D_8871_075353BDFD54__INCLUDED_)
#define AFX_COOLMENU_H__6E9B840A_D384_4B0D_8871_075353BDFD54__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#define MAX_MENUITEM_TEXT 64

class CCoolMenu  
{
public:

	//	Define our own structure, which gets associated
	//  with each menu item.
	//
	typedef struct
	{
		HBITMAP hbmp_ref;		//each menu item will share a handle to the same bitmap
		UINT  type;				//MF_SEPERATOR etc
		int   bitmapy;			//position in the bitmap that we take the picture from
		int   bitmapwidth;		//width of the bitmap in the menu
		int   menuwidth;		//each item also needs to know how big to make the menu
		TCHAR szText[MAX_MENUITEM_TEXT];
	} MyMenuItem;



	CCoolMenu();
	virtual ~CCoolMenu();

	BOOL MakeCoolMenu(HMENU hmenu, HINSTANCE hInst, LPCTSTR pBitmapName);
	LRESULT CoolMenu_DrawItem(WPARAM wParam, LPARAM lParam);
	void RemoveCoolMenu(HMENU hmenu);
	LRESULT CoolMenu_MeasureItem(WPARAM wParam, LPARAM lParam);
	void CoolMenu_DrawCheck(HDC hdc, RECT *rc, BOOL bSelected);
	int GreyText1(HDC hdc, LPSTR str, int count, RECT *rect, UINT flag, LPDRAWTEXTPARAMS lp);

private:

	int iTypeAllocation;
	MyMenuItem * mymi;

};

#endif // !defined(AFX_COOLMENU_H__6E9B840A_D384_4B0D_8871_075353BDFD54__INCLUDED_)
