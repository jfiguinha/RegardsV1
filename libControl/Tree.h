// Tree.h: interface for the CTree class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TREE_H__C8ADBD52_6AA2_43AF_910B_09CEB0828CB3__INCLUDED_)
#define AFX_TREE_H__C8ADBD52_6AA2_43AF_910B_09CEB0828CB3__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

/*
#include <windows.h>
#include "Shlobj.h"
#include "commctrl.h"
#include <string>
using namespace std;
*/

class CTreePrv;

class CTree
{

public:

	BOOL Tree_GetChildItems(HTREEITEM);
	HTREEITEM ExpandedItem(HTREEITEM hParent, const char * cDirectory);
	void TreeViewGetSelectName(char *cDirName, int iSize);
	void TreeViewGetNodeName(HTREEITEM hChild, char *cDirName, int iSize);
	HTREEITEM AddItem(HTREEITEM hParent, LPCTSTR strPath);
	void ExpandItem(HTREEITEM hItem, UINT nCode = TVE_EXPAND);
	int SpecialFolderToPath(const char * SpecialFolder,char * Path, int iSize);
	BOOL DisplayDrives();

	CTree();
	virtual ~CTree();

	///////////////////////////////////////////////////////////
	//Fonctions utiles
	///////////////////////////////////////////////////////////
	BOOL InitTree(LPCTSTR strRoot, BOOL bFiles);
	HWND CreateATreeView(HWND hwndParent, HINSTANCE g_hinst);
	void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	int Tree_ExecuteCommandMenu(HTREEITEM hItem, LPMALLOC  g_pMalloc, const char * Verb);

	//Nouvelles fonctions
	string GetItemText(HTREEITEM hItem);
	LRESULT Tree_Notify(LPARAM);
	HRESULT Tree_DisplayFolder(HTREEITEM hParent, const char * m_Path);
	HRESULT Tree_DisplayContextMenu(HTREEITEM hParent, const char * m_Path, LPPOINT pptScreen,IContextMenu2  * g_pcm2);
	int Tree_DoItemMenu(HTREEITEM hItem, LPPOINT pptScreen,IContextMenu2  * g_pcm2);

	HTREEITEM DeleteDriveItem(string stgDrive);
	int AddDriveItem(string stgDrive);
	string GetFullPath(HTREEITEM hItem);

	HWND m_hWnd;

private:


	CTreePrv * pimpl_;

};

#endif // !defined(AFX_TREE_H__C8ADBD52_6AA2_43AF_910B_09CEB0828CB3__INCLUDED_)
