// ListView.h: interface for the CListView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTVIEW_H__7C1B633A_C06E_49AC_9065_2DAE670F072B__INCLUDED_)
#define AFX_LISTVIEW_H__7C1B633A_C06E_49AC_9065_2DAE670F072B__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include <Iconefile.h>

class CPidl;
class CStatusBar;
class CItemInfo;
class CParameter;

class CListView  
{
public:

	BOOL IsWindowVisible()
	{
		return ::IsWindowVisible(m_hWnd);
	};

	BOOL ShowWindow(int iShow)
	{
		return ::ShowWindow(m_hWnd,iShow);
	};

	void List_EnsureVisible(const int &iItem, const int &iOldPos);

	CListView();
	virtual ~CListView();

	void List_LockCurrentFolder(CItemInfo * pItemInfo);
	void List_ReleaseCurrentFolder(void);
	static int CALLBACK List_CompareProc(LPARAM, LPARAM, LPARAM);
	void List_EnumObjects(LPSHELLFOLDER pParentFolder, LPITEMIDLIST pidlParent,const bool &m_bUpdate = false);
	void List_EnumAlbum(IconeFileVector * m_IconeFileVector);
	HRESULT List_DisplayFolder(const char * m_Path);
	HRESULT List_DisplayAlbum(IconeFileVector * m_IconeFileVector);
	void List_GetItemText(int hItem, char * szLabel, int iSize);
	void List_ExecuteCommandMenu(VECTITEM &m_vectItem, 
							UINT cbItems, 
							LPMALLOC  g_pMalloc, const char * Verb);
	int List_DoItemMenu(VECTITEM &m_vectItem, UINT cbItems, LPPOINT pptScreen, IContextMenu2  *g_pcm2,LPMALLOC  g_pMalloc);
	HRESULT List_UpdateFolder();
	HWND m_hWnd;

	void SetShowFolder(bool bValue)
	{
		m_bShowFolder = bValue;
	}

	bool GetShowFolder()
	{
		return m_bShowFolder;
	}

protected:

	IShellFolder * g_psfCurFolder;
	LPITEMIDLIST g_pidlCurFQ;
	CPidl * m_cPidl;
	int NbFiles;
	bool m_bShowFolder;
	bool m_bAfficheImage;
	char cDirectory[MAX_PATH];
	CStatusBar * m_ListStatusBar;
	CParameter * m_cParameter;

};

#endif // !defined(AFX_LISTVIEW_H__7C1B633A_C06E_49AC_9065_2DAE670F072B__INCLUDED_)
