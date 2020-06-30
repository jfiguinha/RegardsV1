// Pidl.h: interface for the CPidl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PIDL_H__B91ECFA8_31B7_48B3_BC93_A745548F0FF8__INCLUDED_)
#define AFX_PIDL_H__B91ECFA8_31B7_48B3_BC93_A745548F0FF8__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

//#include <windows.h>
//#include <commctrl.h>
//#include <shlobj.h>

class CPidl  
{
public:
	CPidl();
	virtual ~CPidl();
	LPITEMIDLIST Pidl_GetNextItem(LPCITEMIDLIST pidl);
	LPITEMIDLIST Pidl_Create(UINT cbSize);
	UINT Pidl_GetSize(LPCITEMIDLIST pidl);
	LPITEMIDLIST Pidl_Copy(LPCITEMIDLIST pidlSource);
	void Pidl_Free(LPITEMIDLIST pidl);
	LPITEMIDLIST Pidl_Concatenate(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
	BOOL GetParentID(LPITEMIDLIST pidl);
	LPITEMIDLIST GetNextItemID(LPCITEMIDLIST pidl);


private:
	LPMALLOC g_pMalloc;
};

#endif // !defined(AFX_PIDL_H__B91ECFA8_31B7_48B3_BC93_A745548F0FF8__INCLUDED_)
