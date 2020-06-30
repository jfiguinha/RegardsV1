// Tree.cpp: implementation of the CTree class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tree.h"
#include <SADirRead.h>
#include "pidl.h"
#include "objet.h"
#include "parameter.h"
#include <ItemInfo.h>

class CTreePrv
{
public:

	typedef struct SpecialFolder
	{
		string m_szRealName;
		string m_szAffichageName;
	}SpecialFolder;

	typedef std::vector<SpecialFolder> SpecialFolderVector;

	typedef struct TREEPRV
	{
		int m_bFiles;
		string m_strRoot;
		string m_strError;
		int iNumEntree;
		bool m_blTerminer;
		bool m_blDelTerminer;
		bool m_bQuitterArbre;
		HIMAGELIST m_imgListLocal;
		SpecialFolderVector m_SpFolder;
		int iNumIconAlbum;
		int iTypeDonnee;
	}TREEPRVDATA, FAR * LPTREEPRVDATA;

	CTreePrv()
	{
		ZeroMemory(&pTreeData,sizeof(TREEPRVDATA));
		pTreeData.m_blTerminer = true;
		pTreeData.m_blDelTerminer = true;
		pTreeData.m_bQuitterArbre = false;
		pTreeData.iTypeDonnee = 1;

		pTreeData.m_bFiles = 0;
		pTreeData.iNumEntree = 0;
		pTreeData.m_imgListLocal = NULL;
		pTreeData.iNumIconAlbum = 0;

		m_cParameter = new CParameter();


	};
	~CTreePrv()
	{
		if(pTreeData.m_imgListLocal != NULL)
			ImageList_Destroy(pTreeData.m_imgListLocal);

		pTreeData.m_imgListLocal = NULL;

		if(m_cParameter != NULL)
			delete m_cParameter;
	};


	BOOL Tree_GetRootItems(HWND);
	void Tree_EnumObjects(HWND, HTREEITEM, LPSHELLFOLDER, LPITEMIDLIST);
	static int CALLBACK Tree_CompareProc(LPARAM, LPARAM, LPARAM);
	BOOL DisplayDrives(HWND hWndTree);
	string GetFullPath(HWND hwndTree, HTREEITEM hItem);
	string GetItemText(HWND hwndTree, HTREEITEM hItem);
	BOOL GetItemIdListFromPath (LPWSTR lpszPath, LPITEMIDLIST *lpItemIdList);

	HTREEITEM InsertItem(HWND hwndParent, LPSTR lpszFileName, int iSize, int iIcon, int iIconSel, HTREEITEM hParent);

	BOOL GetSysImgList(HWND hWndTree);
	LPCTSTR GetSubPath(LPCTSTR strPath);

	void AddItem(HWND hWndTree,LPITEMIDLIST iItemId,LPSHELLFOLDER  pParentFolder,CItemInfo * pItemChild);
	HTREEITEM AddItemToTree(HWND hWndTree,HTREEITEM hParent, LPCTSTR strPath);

	HTREEITEM DeleteDriveItem(HWND hWndTree, string stgDrive);
	int AddDriveItem(HWND hWndTree, string stgDrive);

	HTREEITEM FindTreeItem(HWND hWndTree, string stgDrive, HTREEITEM hParent, HTREEITEM &hPrevious);

	CPidl m_cPidl;
	HTREEITEM hPrev;
	TREEPRVDATA pTreeData;
	CParameter * m_cParameter;

};

/**************************************************************************
   global variables and definitions
**************************************************************************/

/**************************************************************************

   Tree_Notify()

**************************************************************************/

LRESULT CTree::Tree_Notify(LPARAM lParam)
{
LPNMHDR  lpnmh = (LPNMHDR) lParam;

switch(lpnmh->code)
   {
   case TVN_GETDISPINFO:
      {
      LPNMTVDISPINFO lpdi = (LPNMTVDISPINFO)lParam;
      CItemInfo *     pItem = (CItemInfo *)lpdi->item.lParam;

      //do we need to supply the text?
      if(lpdi->item.mask & TVIF_TEXT)
         {
         SHFILEINFO     sfi;
      
         //get the display name of the item
         if(SHGetFileInfo((LPCTSTR)pItem->GetPidlFQ(), 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME))
            {
            lstrcpyn(lpdi->item.pszText, sfi.szDisplayName, lpdi->item.cchTextMax);
            }
         }

      //do we need to supply the unselected image?
      if(lpdi->item.mask & TVIF_IMAGE)
         {
         SHFILEINFO     sfi;
         
         //get the unselected image for this item
         if(SHGetFileInfo((LPCTSTR)pItem->GetPidlFQ(), 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_LINKOVERLAY))
            {
            lpdi->item.iImage = sfi.iIcon;
            }
         }

      //do we need to supply the selected image?
      if(lpdi->item.mask & TVIF_SELECTEDIMAGE)
         {
         SHFILEINFO     sfi;
         
         //get the selected image for this item
         if(SHGetFileInfo((LPCTSTR)pItem->GetPidlFQ(), 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_OPENICON))
            {
            lpdi->item.iSelectedImage = sfi.iIcon;
            }
         }
      }
      break;

   case TVN_ITEMEXPANDING:
      {
      LPNMTREEVIEW   pnmtv = (LPNMTREEVIEW)lParam;
      
      switch(pnmtv->action)
         {
         case TVE_EXPAND:
            Tree_GetChildItems(pnmtv->itemNew.hItem);
            break;
         
         case TVE_COLLAPSE:
            //remove all of the items from this node
            TreeView_Expand(m_hWnd,pnmtv->itemNew.hItem, TVE_COLLAPSE | TVE_COLLAPSERESET);
            break;
         }
      }
      break;

   case TVN_DELETEITEM:
      {
		LPNMTREEVIEW   pnmtv = (LPNMTREEVIEW)lParam;
		CItemInfo *     pItem = (CItemInfo *)pnmtv->itemOld.lParam;
		delete pItem;
		pItem = NULL;
      }
      break;
   
	  /*
   case TVN_SELCHANGED:
      {
      LPNMTREEVIEW   pnmtv = (LPNMTREEVIEW)lParam;
      LPITEMINFO pItem = (LPITEMINFO)pnmtv->itemNew.lParam;

      List_DisplayFolder(GetDlgItem(hWnd, IDC_LISTVIEW), pItem);
      }*/
      break;
   }

return 0;
}



/**************************************************************************

   Tree_GetChildItems

**************************************************************************/

BOOL CTree::Tree_GetChildItems(HTREEITEM hParentItem)
{
	TVITEM         tvItem;
	CItemInfo *     pItem;
	LPSHELLFOLDER  pParentFolder = NULL;
	HRESULT        hr;
	HCURSOR        hCursor;
	TV_SORTCB      tvSort;


	//get the parent item's pidl
	ZeroMemory(&tvItem, sizeof(tvItem));
	tvItem.mask = TVIF_PARAM;
	tvItem.hItem = hParentItem;
	if(!TreeView_GetItem(m_hWnd, &tvItem))
	   return FALSE;

	//change the cursor
	hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));

	//turn redawing off in the TreeView. This will speed things up as we add items
	SendMessage(m_hWnd, WM_SETREDRAW, FALSE, 0);

	pItem = (CItemInfo *)tvItem.lParam;

	//if the parent folder is NULL, then we are at the root of the namespace, so the parent of this item is the desktop folder
	if(!pItem->GetShellFolder())
	{
	   hr = SHGetDesktopFolder(&pParentFolder);
	}
	//otherwise we need to get the IShellFolder for this item
	else
	{
	   hr = pItem->GetShellFolder()->BindToObject(pItem->GetPidlRel(), NULL, IID_IShellFolder, (LPVOID*)&pParentFolder);
	}

	if(FAILED(hr))
	   return FALSE;

	if(pItem->GetPidlRel() != NULL)
		pimpl_->Tree_EnumObjects(m_hWnd, hParentItem, pParentFolder, pItem->GetPidlFQ());

	//sort the new items
	tvSort.hParent = hParentItem;
	tvSort.lpfnCompare = (PFNTVCOMPARE)pimpl_->Tree_CompareProc;
	tvSort.lParam = 0;
	TreeView_SortChildrenCB(m_hWnd, &tvSort, 0);

	//turn redawing back on in the TreeView
	SendMessage(m_hWnd, WM_SETREDRAW, TRUE, 0);

	pParentFolder->Release();

	SetCursor(LoadCursor(NULL, IDC_ARROW));

	return TRUE;
}

/**************************************************************************

   Tree_EnumObjects

**************************************************************************/

void CTreePrv::Tree_EnumObjects(HWND hwndTreeView, 
                        HTREEITEM hParentItem, 
                        LPSHELLFOLDER pParentFolder, 
                        LPITEMIDLIST pidlParent)
{
char pszBuf[MAX_PATH];	
LPENUMIDLIST   pEnum;
   
if(pParentFolder == NULL)
	return;


if(SUCCEEDED(pParentFolder->EnumObjects(NULL, SHCONTF_FOLDERS, &pEnum)))
   {
   LPITEMIDLIST   pidlTemp;
   DWORD          dwFetched = 1;
   CItemInfo *     pItem;

   //enumerate the item's PIDLs
   while(S_OK == (pEnum->Next(1, &pidlTemp, &dwFetched)) && dwFetched)
      {
      TVITEM         tvItem;
      TVINSERTSTRUCT tvInsert;
      DWORD          dwAttribs;
	  STRRET strDispName;
	  CObjet m_cObjet;

      ZeroMemory(&tvItem, sizeof(tvItem));
      
      //fill in the TV_ITEM structure for this item
      tvItem.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN;

      //AddRef the parent folder so it's pointer stays valid
      pParentFolder->AddRef();

      //put the private information in the lParam
      pItem = new CItemInfo();
      /*
      pItem->pidlRel = pidlTemp;
      pItem->pidlFQ = m_cPidl.Pidl_Concatenate(pidlParent, pidlTemp);
	  pItem->iTypeDonne = pTreeData.iTypeDonnee;
	  */

	  pItem->SetPidlRel(pidlTemp);
	  pItem->SetPidlFQ(m_cPidl.Pidl_Concatenate(pidlParent, pidlTemp));
	  pItem->SetTypeDonne(pTreeData.iTypeDonnee);

	  SHFILEINFO  sfi;

	  SHGetFileInfo((LPCTSTR)pItem->GetPidlFQ(), 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME);
      
      pItem->SetShellFolder(pParentFolder);
      tvItem.lParam = (LPARAM)pItem;

      //text and images are done on a callback basis
      //tvItem.pszText = LPSTR_TEXTCALLBACK;

	  pParentFolder->GetDisplayNameOf(pidlTemp, SHGDN_FORPARSING, &strDispName);
	  StrRetToBuf(&strDispName, pidlTemp, pszBuf , MAX_PATH);
	  pItem->SetParseName(pszBuf);
	  m_cObjet.GetFileName(pItem->GetParseName(),pszBuf,MAX_PATH);
	  pItem->SetFileName(pszBuf);

	  tvItem.pszText = pItem->GetFileName();
      tvItem.iImage = tvItem.iSelectedImage = I_IMAGECALLBACK;

      //determine if the item has children
      dwAttribs = SFGAO_HASSUBFOLDER | SFGAO_FOLDER | SFGAO_DISPLAYATTRMASK | SFGAO_CANRENAME;
      pParentFolder->GetAttributesOf(1, (LPCITEMIDLIST*)&pidlTemp, &dwAttribs);
      tvItem.cChildren = (dwAttribs & SFGAO_HASSUBFOLDER);

      //determine if the item is shared
      if(dwAttribs & SFGAO_SHARE)
         {
         tvItem.mask |= TVIF_STATE;
         tvItem.stateMask |= TVIS_OVERLAYMASK;
         tvItem.state |= INDEXTOOVERLAYMASK(1); //1 is the index for the shared overlay image
         }
      
      //fill in the TV_INSERTSTRUCT structure for this item
      tvInsert.item = tvItem;
      tvInsert.hInsertAfter = TVI_LAST;
      tvInsert.hParent = hParentItem;

      TreeView_InsertItem(hwndTreeView, &tvInsert);

      dwFetched = 0;
      }

   pEnum->Release();
   }
}

/**************************************************************************

   Tree_CompareProc

**************************************************************************/

int CALLBACK CTreePrv::Tree_CompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	UNREFERENCED_PARAMETER(lParamSort);

	CItemInfo *  pItem1 = (CItemInfo *)lParam1;
	CItemInfo *  pItem2 = (CItemInfo *)lParam2;
	HRESULT     hr;

	hr = pItem1->GetShellFolder()->CompareIDs(   0,
											pItem1->GetPidlRel(),
											pItem2->GetPidlRel());

	if(FAILED(hr))
	return 0;

	return (short)SCODE_CODE(GetScode(hr));
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTree::CTree()
{
	pimpl_ = new CTreePrv();
	pimpl_->pTreeData.iTypeDonnee = 1;
	pimpl_->hPrev = (HTREEITEM) TVI_LAST;

}

CTree::~CTree()
{
	delete pimpl_;
}

BOOL CTree::DisplayDrives()
{
	return pimpl_->DisplayDrives(m_hWnd);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CreateATreeView - creates a tree-view control. 
// Returns the handle to the new control if successful,
// or NULL otherwise. 
// hwndParent - handle to the control's parent window. 
// lpszFileName - name of the file to parse for tree-view items.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HWND CTree::CreateATreeView(HWND hwndParent, HINSTANCE g_hinst) 
{ 
    RECT rcClient;  // dimensions of client area 
	 INITCOMMONCONTROLSEX icex;

	// Ensure that the common control DLL is loaded. 
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC  = ICC_TREEVIEW_CLASSES | ICC_COOL_CLASSES;
	InitCommonControlsEx(&icex); 

    // Ensure that the common control DLL is loaded. 
    //InitCommonControls(); 
 
    // Get the dimensions of the parent window's client area, and create 
    // the tree-view control. 
    GetClientRect(hwndParent, &rcClient); 


    m_hWnd = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_ACCEPTFILES, WC_TREEVIEW, "Tree View", 
        WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES | TVS_EDITLABELS | TVS_SHOWSELALWAYS | TVS_LINESATROOT| TVS_HASBUTTONS, 
        0, 0, 250, rcClient.bottom, 
        hwndParent, NULL, g_hinst, NULL); 

	SendMessage(m_hWnd,WM_SETREDRAW,true,0);

	pimpl_->GetSysImgList(m_hWnd);

	//SetWindowTheme(m_hWnd, L"explorer", NULL);
 
    return m_hWnd;
} 


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BOOL CTree::InitTree(LPCTSTR strRoot, BOOL bFiles)
{
	//Récupération des folders spéciaux

	
	CTreePrv::SpecialFolder m_SpTemp;
	pimpl_->pTreeData.iNumEntree = 0;
	//TCHAR szDisplayName[MAX_PATH];


	DWORD dwStyle = (DWORD)GetWindowLong(m_hWnd, GWL_STYLE);   // read the windowstyle
	if ( dwStyle & TVS_EDITLABELS ) 
	{
		// Don't allow the user to edit ItemLabels
		SetWindowLong(m_hWnd, TVS_EDITLABELS,0);
	}
	
	// Display the DirTree with the Rootname e.g. C:\
	// if Rootname == NULL then Display all Drives on this PC
    // First, we need the system-ImageList
	
	TreeView_DeleteAllItems(m_hWnd);


	////////////////////////////////////////////////////////////////////////////
	
	int iVersion = CObjet::GetWindowsVersionAvance();

	////////////////////////////////////////////////////////////////////////////

	HTREEITEM hParent;
    pimpl_->pTreeData.m_bFiles = bFiles;  // if TRUE, Display Path- and Filenames 
	if (strRoot == NULL || strRoot[0] == '\0' )
	{
		if ( !pimpl_->DisplayDrives(m_hWnd) )
			return FALSE;
		pimpl_->pTreeData.m_strRoot = "";
	}
    else
	{
		pimpl_->pTreeData.m_strRoot = strRoot;
		if (strcmp(pimpl_->pTreeData.m_strRoot.substr(1,pimpl_->pTreeData.m_strRoot.length() - 1).c_str(),"\\") == 0)
			pimpl_->pTreeData.m_strRoot += "\\";

		hParent = AddItem(TVI_ROOT, pimpl_->pTreeData.m_strRoot.c_str() );
		Tree_DisplayFolder(hParent, strRoot );
	}


	TreeView_SetScrollTime(m_hWnd, 100);

	return TRUE;	
}

//////////////////////////////////////////////////////////
//Récupération des icones systèmes
//////////////////////////////////////////////////////////

BOOL CTreePrv::GetSysImgList(HWND hwndTV)
{
	HIMAGELIST m_imgList;
	SHFILEINFO shfi;

	ZeroMemory( &shfi, sizeof(SHFILEINFO) );

	m_imgList = (HIMAGELIST)SHGetFileInfo( "C:\\",
		0,
		&shfi,
		sizeof( SHFILEINFO ),
		SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

	SendMessage(hwndTV, TVM_SETIMAGELIST, TVSIL_NORMAL, (LPARAM)m_imgList);

	return TRUE;   // OK
}

BOOL CTreePrv::GetItemIdListFromPath (LPWSTR lpszPath, LPITEMIDLIST *lpItemIdList)
{
   LPSHELLFOLDER pShellFolder = NULL;
   HRESULT         hr;
   ULONG         chUsed;

   // Get desktop IShellFolder interface
   if (SHGetDesktopFolder (&pShellFolder) != NOERROR)
      return FALSE;     // failed



   // convert the path to an ITEMIDLIST
   hr = pShellFolder->ParseDisplayName (
                  NULL,           // owner window
                  NULL,           // reserved (must be NULL)
                  lpszPath,       // folder name
                  &chUsed,    // number of chars parsed
                  lpItemIdList,   // ITEMIDLIST
                  NULL            // attributes (can be NULL)
               );
      
   if (FAILED(hr))
   {
      pShellFolder->Release();
      *lpItemIdList = NULL;
      return FALSE;
   }
   
   pShellFolder->Release();
   return TRUE;
} // GetItemIdListFromPath

HTREEITEM CTree::DeleteDriveItem(string stgDrive)
{
	return pimpl_->DeleteDriveItem(m_hWnd, stgDrive);
}

int CTree::AddDriveItem(string stgDrive)
{
	return pimpl_->AddDriveItem(m_hWnd, stgDrive);
}

//////////////////////////////////////////////////////////////
//Suppression d'un disque
//////////////////////////////////////////////////////////////
HTREEITEM CTreePrv::DeleteDriveItem(HWND hWndTree, string stgDrive)
{
	HTREEITEM m_hTreePrevious = TVI_ROOT;
	HTREEITEM m_hTreeItem = FindTreeItem(hWndTree,stgDrive.substr(0,2),TVI_ROOT, m_hTreePrevious);
	if(m_hTreeItem != NULL)
		TreeView_DeleteItem(hWndTree, m_hTreeItem);
	return m_hTreePrevious;
}

//////////////////////////////////////////////////////////////
//Suppression d'un disque
//////////////////////////////////////////////////////////////
HTREEITEM CTreePrv::FindTreeItem(HWND hWndTree, string stgDrive, HTREEITEM hParent, HTREEITEM &hPrevious)
{
	//HTREEITEM m_hTreeItem = hParent;

	HTREEITEM hChild = TreeView_GetChild(hWndTree, hParent );
	while ( hChild )
	{
		if(GetFullPath(hWndTree, hChild) == stgDrive)
		{
			return hChild;
		}

		hPrevious = hChild;
		hChild = TreeView_GetNextItem(hWndTree, hChild,TVGN_NEXT );

	}
	hPrevious = NULL;
	return NULL;
}

//////////////////////////////////////////////////////////////
//Ajout d'un disque
//////////////////////////////////////////////////////////////
int CTreePrv::AddDriveItem(HWND hWndTree, string stgDrive)
{
	//
	// Displaying the Availible Drives on this PC
	// This are the First Items in the TreeCtrl
	//
	//TreeView_DeleteAllItems(hWndTree);

	LPITEMIDLIST  pidl;

	//CSIDL_DRIVES

	if(SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl)))
	{
		//TV_ITEM           tvItem;
		//TV_INSERTSTRUCT   tvInsert;
		LPSHELLFOLDER     pDesktop;
		//HTREEITEM         hParentItem;
		CItemInfo *        pItemParent;
			//HTREEITEM hPrev = TVI_FIRST;

		//get the desktop's IShellFolder
		if(FAILED(SHGetDesktopFolder(&pDesktop)))
			return FALSE;
		   
		//put the private information in the lParam
		pItemParent = new CItemInfo();

		pItemParent->SetPidlRel(pidl);
		pItemParent->SetPidlFQ(pidl);

		LPITEMIDLIST iItemIdMesImages;
		LPITEMIDLIST iItemIdMesDocuments;

		///////////////////////////////////////////////////////////////////////
		//Insertion des lecteurs
		///////////////////////////////////////////////////////////////////////

		HRESULT hr;
		LPSHELLFOLDER  pParentFolder = NULL;
		LPITEMIDLIST   pidlTemp;
		CItemInfo *     pItem;
		hr = SHGetDesktopFolder(&pParentFolder);

		if(SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_DRIVES, &pidlTemp)))
		{
			//put the private information in the lParam
			pItem = new CItemInfo();
	
			pItem->SetPidlRel(pidlTemp);
			pItem->SetPidlFQ(m_cPidl.Pidl_Concatenate(pItemParent->GetPidlRel(), pidlTemp));
			pItem->SetShellFolder(pParentFolder);

			
			HRESULT hr = pItem->GetShellFolder()->BindToObject(pItem->GetPidlRel(), NULL, IID_IShellFolder, (LPVOID*)&pParentFolder);

			LPENUMIDLIST   pEnum;
	   
			if(pParentFolder == NULL)
				return 0;


			if(SUCCEEDED(pParentFolder->EnumObjects(NULL, SHCONTF_FOLDERS, &pEnum)))
			{
			LPITEMIDLIST   pidlTemp;
			DWORD          dwFetched = 1;
			string m_stgValuePrevious = "";
			//CItemInfo *     pItemChild;

			//Insertion des données pour Mes Documents et Mes Images

			//enumerate the item's PIDLs
			while(S_OK == (pEnum->Next(1, &pidlTemp, &dwFetched)) && dwFetched)
				{
				  
				//AddRef the parent folder so it's pointer stays valid
				pParentFolder->AddRef();

				CItemInfo * pItemChild = new CItemInfo();
				pItemChild->SetPidlRel(pidlTemp);
				pItemChild->SetPidlFQ(m_cPidl.Pidl_Concatenate(pItem->GetPidlFQ(), pidlTemp));
				pItemChild->SetShellFolder(pParentFolder);
				pItemChild->SetTypeDonne(pTreeData.iTypeDonnee);

				SHFILEINFO  sfi;

				SHGetFileInfo((LPCTSTR)pItemChild->GetPidlFQ(), 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME | SHGFI_SYSICONINDEX);

				string m_stgValue = (LPCTSTR)pidlTemp->mkid.abID;

				pItem->SetShellFolder(pParentFolder);

				if(m_stgValue.find(":\\") != -1 && m_stgValue.substr(1,3) == stgDrive)
				{				  
					TVINSERTSTRUCT tvInsert;
					SpecialFolder m_SpTemp;
					TVITEM         tvItem;
					SHFILEINFO  sfi;
					DWORD          dwAttribs;
					int i = 0;

					ZeroMemory(&tvItem, sizeof(tvItem));
	      
					//fill in the TV_ITEM structure for this item
					tvItem.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN;

					SHGetFileInfo((LPCTSTR)pItemChild->GetPidlFQ(), 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME | SHGFI_SYSICONINDEX);

					m_stgValue = m_stgValue.substr(1,m_stgValue.length() - 2);
					m_SpTemp.m_szRealName = m_stgValue.c_str();

					m_SpTemp.m_szAffichageName = sfi.szDisplayName;

					pTreeData.m_SpFolder.push_back(m_SpTemp);

					pItem->SetShellFolder(pParentFolder);
					tvItem.lParam = (LPARAM)pItemChild;

					//text and images are done on a callback basis
					tvItem.pszText = sfi.szDisplayName;
					tvItem.iImage = tvItem.iSelectedImage = sfi.iIcon;

						//tvi.pszText = lpszFileName; 
					tvItem.cchTextMax = lstrlen(sfi.szDisplayName); 

					//determine if the item has children
					dwAttribs = SFGAO_HASSUBFOLDER | SFGAO_FOLDER | SFGAO_DISPLAYATTRMASK | SFGAO_CANRENAME;
					pParentFolder->GetAttributesOf(1, (LPCITEMIDLIST*)&pidlTemp, &dwAttribs);
					tvItem.cChildren = (dwAttribs & SFGAO_HASSUBFOLDER);

					//determine if the item is shared
					if(dwAttribs & SFGAO_SHARE)
						{
						tvItem.mask |= TVIF_STATE;
						tvItem.stateMask |= TVIS_OVERLAYMASK;
						tvItem.state |= INDEXTOOVERLAYMASK(1); //1 is the index for the shared overlay image
						}
	      
					//fill in the TV_INSERTSTRUCT structure for this item
					HTREEITEM hPrevious = TVI_ROOT;
					tvInsert.item = tvItem;
					tvInsert.hInsertAfter = FindTreeItem(hWndTree, m_stgValuePrevious.substr(1,2), TVI_ROOT, hPrevious);
					tvInsert.hParent = TVI_ROOT;

					hPrev = TreeView_InsertItem(hWndTree, &tvInsert);

				}
				else
				{
					m_stgValuePrevious = m_stgValue;
					delete pItemChild;
				}

				dwFetched = 0;
				}

			pEnum->Release();
			}

			//free(pItem);
			delete pItem;
		}

		//free(pItemParent);
		delete pItemParent;

		pDesktop->Release();

	////////////////////////////////////////////////////////////////////////////////
	//Ajout des albums
	////////////////////////////////////////////////////////////////////////////////

		return TRUE;
	}

	return TRUE;

	return 0;
}

///////////////////////////////////////////////////////////////////
//Récupération des principaux disques
///////////////////////////////////////////////////////////////////

BOOL CTreePrv::DisplayDrives(HWND hWndTree)
{
	//
	// Displaying the Availible Drives on this PC
	// This are the First Items in the TreeCtrl
	//
	//TreeView_DeleteAllItems(hWndTree);

	LPITEMIDLIST  pidl;

	//CSIDL_DRIVES

	if(SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl)))
	{
		//TV_ITEM           tvItem;
		//TV_INSERTSTRUCT   tvInsert;
		LPSHELLFOLDER     pDesktop;
		//HTREEITEM         hParentItem;
		CItemInfo *        pItemParent;
			//HTREEITEM hPrev = TVI_FIRST;

		//get the desktop's IShellFolder
		if(FAILED(SHGetDesktopFolder(&pDesktop)))
			return FALSE;
		   
		//put the private information in the lParam
		pItemParent = new CItemInfo();

		pItemParent->SetPidlRel(pidl);
		pItemParent->SetPidlFQ(pidl);

		LPITEMIDLIST iItemIdMesImages;
		LPITEMIDLIST iItemIdMesVideos;
		LPITEMIDLIST iItemIdMesDocuments;

		///////////////////////////////////////////////////////////////////////
		//Insertion des lecteurs
		///////////////////////////////////////////////////////////////////////

		HRESULT hr;
		LPSHELLFOLDER  pParentFolder = NULL;
		LPITEMIDLIST   pidlTemp;
		CItemInfo *     pItem;
		hr = SHGetDesktopFolder(&pParentFolder);

		if(SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_DRIVES, &pidlTemp)))
		{
			//put the private information in the lParam
			pItem = new CItemInfo();
	
			pItem->SetPidlRel(pidlTemp);
			pItem->SetPidlFQ(m_cPidl.Pidl_Concatenate(pItemParent->GetPidlRel(), pidlTemp));
			pItem->SetShellFolder(pParentFolder);

			
			HRESULT hr = pItem->GetShellFolder()->BindToObject(pItem->GetPidlRel(), NULL, IID_IShellFolder, (LPVOID*)&pParentFolder);

			LPENUMIDLIST   pEnum;
	   
			if(pParentFolder == NULL)
				return 0;

			int iVersion = CObjet::GetWindowsVersionAvance();

			int m_iReturnDoc = 0;

			if(iVersion >= 50)
			{
				m_iReturnDoc = SHGetSpecialFolderLocation(hWndTree, CSIDL_PROFILE, &iItemIdMesDocuments);
			}
			else
			{
				m_iReturnDoc = SHGetSpecialFolderLocation(hWndTree, CSIDL_PERSONAL, &iItemIdMesDocuments);
			}

			if(SHGetSpecialFolderLocation(hWndTree, CSIDL_MYPICTURES, &iItemIdMesImages) == 0 && m_iReturnDoc == 0)
			{
				CItemInfo * pItemChild = new CItemInfo();
				pItemChild->SetPidlRel(iItemIdMesImages);
				pItemChild->SetPidlFQ(m_cPidl.Pidl_Concatenate(iItemIdMesDocuments, iItemIdMesImages));
				pItemChild->SetShellFolder(pParentFolder);
				pItemChild->SetTypeDonne(pTreeData.iTypeDonnee);
				AddItem(hWndTree,iItemIdMesImages,pParentFolder,pItemChild);
			}

			if(m_iReturnDoc == 0)
			{
				CItemInfo * pItemChild = new CItemInfo();
				pItemChild->SetPidlRel(iItemIdMesDocuments);
				pItemChild->SetPidlFQ(m_cPidl.Pidl_Concatenate(pItemParent->GetPidlFQ(), iItemIdMesDocuments));
				pItemChild->SetTypeDonne(pTreeData.iTypeDonnee);
				AddItem(hWndTree,iItemIdMesDocuments,pParentFolder,pItemChild);
			}


			//////////////////////////////////////////////////////////////////////////////////
			//Ajout du répertoire de gravure
			//////////////////////////////////////////////////////////////////////////////////

			string m_stgBurningRep = m_cParameter->GetBurningDirectory();
	
			wchar_t cFilePath[MAX_PATH];

			size_t m_sizeTConvert;
			mbstowcs_s(&m_sizeTConvert,cFilePath,MAX_PATH, m_stgBurningRep.c_str(), m_stgBurningRep.length()+1);

			//mbstowcs(cFilePath, m_stgBurningRep.c_str(),  512);

			if(GetItemIdListFromPath(cFilePath,&iItemIdMesImages))
			{
				CItemInfo * pItemChild = new CItemInfo();
				pItemChild->SetPidlRel(pidlTemp);
				pItemChild->SetPidlFQ(m_cPidl.Pidl_Concatenate(pItem->GetPidlFQ(), pidlTemp));
				pItemChild->SetShellFolder(pParentFolder);
				pItemChild->SetTypeDonne(pTreeData.iTypeDonnee);
				AddItem(hWndTree,iItemIdMesImages,pParentFolder,pItemChild);
			}


			if(SUCCEEDED(pParentFolder->EnumObjects(NULL, SHCONTF_FOLDERS, &pEnum)))
			{
				LPITEMIDLIST   pidlTemp;
				DWORD          dwFetched = 1;
				//CItemInfo *     pItemChild;

				//Insertion des données pour Mes Documents et Mes Images

				//enumerate the item's PIDLs
				while(S_OK == (pEnum->Next(1, &pidlTemp, &dwFetched)) && dwFetched)
				{
					  
					//AddRef the parent folder so it's pointer stays valid
					pParentFolder->AddRef();

					CItemInfo * pItemChild = new CItemInfo();
					pItemChild->SetPidlRel(pidlTemp);
					pItemChild->SetPidlFQ(m_cPidl.Pidl_Concatenate(pItem->GetPidlFQ(), pidlTemp));
					pItemChild->SetShellFolder(pParentFolder);
					pItemChild->SetTypeDonne(pTreeData.iTypeDonnee);

					SHFILEINFO  sfi;

					SHGetFileInfo((LPCTSTR)pItemChild->GetPidlFQ(), 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME | SHGFI_SYSICONINDEX);


					string m_stgValue = (LPCTSTR)pidlTemp->mkid.abID;

					pItem->SetShellFolder(pParentFolder);


					if(m_stgValue.find(":\\") != -1)
					{
						  
						TVINSERTSTRUCT tvInsert;
						SpecialFolder m_SpTemp;
						TVITEM         tvItem;
						SHFILEINFO  sfi;
						DWORD          dwAttribs;
						int i = 0;

						ZeroMemory(&tvItem, sizeof(tvItem));
		      
						//fill in the TV_ITEM structure for this item
						tvItem.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN;


						SHGetFileInfo((LPCTSTR)pItemChild->GetPidlFQ(), 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME | SHGFI_SYSICONINDEX);

						m_stgValue = m_stgValue.substr(1,m_stgValue.length() - 2);
						m_SpTemp.m_szRealName = m_stgValue.c_str();

						m_SpTemp.m_szAffichageName = sfi.szDisplayName;

						pTreeData.m_SpFolder.push_back(m_SpTemp);

						pItem->SetShellFolder(pParentFolder);
						tvItem.lParam = (LPARAM)pItemChild;

						//text and images are done on a callback basis
						tvItem.pszText = sfi.szDisplayName;
						tvItem.iImage = tvItem.iSelectedImage = sfi.iIcon;

							//tvi.pszText = lpszFileName; 
						tvItem.cchTextMax = lstrlen(sfi.szDisplayName); 

						//determine if the item has children
						dwAttribs = SFGAO_HASSUBFOLDER | SFGAO_FOLDER | SFGAO_DISPLAYATTRMASK | SFGAO_CANRENAME;
						pParentFolder->GetAttributesOf(1, (LPCITEMIDLIST*)&pidlTemp, &dwAttribs);
						tvItem.cChildren = (dwAttribs & SFGAO_HASSUBFOLDER);

						//determine if the item is shared
						if(dwAttribs & SFGAO_SHARE)
							{
							tvItem.mask |= TVIF_STATE;
							tvItem.stateMask |= TVIS_OVERLAYMASK;
							tvItem.state |= INDEXTOOVERLAYMASK(1); //1 is the index for the shared overlay image
							}
		      
						//fill in the TV_INSERTSTRUCT structure for this item
						tvInsert.item = tvItem;
						tvInsert.hInsertAfter = hPrev;
						tvInsert.hParent = TVI_ROOT;

						hPrev = TreeView_InsertItem(hWndTree, &tvInsert);

					}
					else
					{
						//HeapFree(GetProcessHeap(),HEAP_ZERO_MEMORY,pItemChild);
						delete pItemChild;
					}

					dwFetched = 0;
				}

				pEnum->Release();
			}

			//free(pItem);
			delete pItem;
		}

		//free(pItemParent);
		delete pItemParent;

		pDesktop->Release();

	////////////////////////////////////////////////////////////////////////////////
	//Ajout des albums
	////////////////////////////////////////////////////////////////////////////////

		return TRUE;
	}

	return TRUE;

}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

HTREEITEM CTreePrv::AddItemToTree(HWND hWndTree,HTREEITEM hParent, LPCTSTR strPath)
{
	// Adding the Item to the TreeCtrl with the current Icons
	int iIcon, iIconSel;
    string strTemp = strPath;
	CObjet m_cObjet;
	char m_szItem[MAX_PATH];

	iIcon = m_cObjet.GetSysImgIcon(strTemp);

	iIconSel = m_cObjet.GetSysImgOpenIcon(strTemp);

	if(iIcon == 0 || iIconSel == 0)
		iIcon = iIconSel = pTreeData.iNumIconAlbum;

	if(strlen(strPath) > 0)
	{
		if (strcmp(strTemp.substr(strTemp.length() - 1, 1).c_str(),"\\") == 0)
			 strTemp = strTemp.substr(0,strTemp.length() - 1);
	}

	StringCchCopy(m_szItem,MAX_PATH,strTemp.c_str());

	if ( hParent == TVI_ROOT )
		return InsertItem(hWndTree, m_szItem, MAX_PATH, iIcon, iIconSel, hParent );
	
	StringCchCopy(m_szItem,MAX_PATH,GetSubPath(strTemp.c_str()));
	return InsertItem(hWndTree, m_szItem, MAX_PATH, iIcon, iIconSel, hParent );
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

HTREEITEM CTreePrv::InsertItem(HWND hwndParent, LPSTR lpszFileName, int iSize, int iIcon, int iIconSel, HTREEITEM hParent)
{
    TVITEM tvi; 
    TVINSERTSTRUCT tvins; 
    
	HRESULT hr;
	LPSHELLFOLDER  pParentFolder = NULL;
	LPSHELLFOLDER  pParentFolder2 = NULL;
	OLECHAR olePath[MAX_PATH];
	LPITEMIDLIST pidlDocFiles = NULL;
	ULONG chEaten;
	IShellFolder *psfDeskTop = NULL;
	LPITEMIDLIST pidlParent;
	char cPath[MAX_PATH];
	ULONG pdwAttributes = SFGAO_VALIDATE;

	//strcpy(cPath,lpszFileName);
	StringCchCopy(cPath,MAX_PATH,lpszFileName);

    tvi.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN; 
 
    // Set the text of the item. 

	//Recherche du nom à afficher

	SpecialFolderVector::const_iterator fit;
	for (SpecialFolder SpecialFolderTemp : pTreeData.m_SpFolder)
	{
		if (_stricmp(lpszFileName, SpecialFolderTemp.m_szRealName.c_str()) == 0)
		{
			//strcpy(lpszFileName,SpecialFolderTemp.m_szAffichageName.c_str());
			StringCchCopy(lpszFileName, iSize, SpecialFolderTemp.m_szAffichageName.c_str());
			break;
		}
	}


	CItemInfo * pItem = new CItemInfo();
		
	hr = SHGetDesktopFolder(&psfDeskTop);

	//SHGetFolderLocation(NULL,CSIDL_DRIVES,NULL,0,&pidlParent);

	SHGetSpecialFolderLocation(NULL,CSIDL_DRIVES,&pidlParent);
	hr = psfDeskTop->BindToObject(pidlParent, NULL, IID_IShellFolder, (LPVOID*)&pParentFolder);

	//pzSourceFiles[0] = '\0';
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, cPath, -1, olePath, MAX_PATH);
	hr = pParentFolder->ParseDisplayName(NULL, NULL, olePath, &chEaten, &pidlDocFiles, &pdwAttributes);

	if(hr != 0)
	{
		StringCbCat(cPath,MAX_PATH,"\\");
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, cPath, -1, olePath, MAX_PATH);
		hr = pParentFolder->ParseDisplayName(NULL, NULL, olePath, &chEaten, &pidlDocFiles, &pdwAttributes);
	}
	
	if(hr == 0 && !(pdwAttributes & SFGAO_VALIDATE))
	{
		pItem->SetPidlRel(pidlDocFiles);
		pItem->SetPidlFQ(pidlParent);
		pItem->SetShellFolder(pParentFolder);
		pItem->SetTypeDonne(pTreeData.iTypeDonnee);
		tvi.lParam = (LPARAM)pItem; 
	}
	else
	{
		pItem->SetTypeDonne(pTreeData.iTypeDonnee);
		tvi.lParam = (LPARAM)pItem;
	}


   //fill in the TVITEM structure for this item

    tvi.pszText = lpszFileName; 
    tvi.cchTextMax = lstrlen(lpszFileName); 

 
    // Assume the item is not a parent item, so give it a 
    // document image. 
    tvi.iImage = iIcon; 
    tvi.iSelectedImage = iIconSel; 
	//tvi.lParam =

	//tvi.cChildren = iIcon;
 
    // Save the heading level in the item's application-defined 
    // data area. 
    
 
    tvins.item = tvi; 
    tvins.hInsertAfter = hPrev; 
 
    // Set the parent item based on the specified level. 

	tvins.hParent = hParent;

    // Add the item to the tree-view control. 
    hPrev = (HTREEITEM) SendMessage(hwndParent, TVM_INSERTITEM, 0, 
         (LPARAM) (LPTVINSERTSTRUCT) &tvins); 

 
	return hPrev;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

LPCTSTR CTreePrv::GetSubPath(LPCTSTR strPath)
{
	//
	// getting the last SubPath from a PathString
	// e.g. C:\temp\readme.txt
	// the result = readme.txt
	static string strTemp;
	int     iPos;

	strTemp = strPath;

	if(strlen(strPath) > 0)
	{
		if (strcmp(strTemp.substr(1,strTemp.length() - 1).c_str(),"\\") == 0)
			strTemp = strTemp.substr(1,strTemp.length() - 1);

		iPos = strTemp.find_last_of("\\");
		if ( iPos != -1 )
			strTemp = strTemp.substr(iPos + 1,strTemp.length() - iPos - 1);

	}

	return (LPCTSTR)strTemp.c_str();
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

void CTree::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	string strPath;
	int iFois = 0;
	char m_szName[MAX_PATH];
	m_szName[0] = '\0';

	if ( pNMTreeView->itemNew.state & TVIS_EXPANDED )
	{
		ExpandItem(pNMTreeView->itemNew.hItem, TVE_EXPAND );
	}
	else
	{
		HTREEITEM hChild = TreeView_GetChild(m_hWnd, pNMTreeView->itemNew.hItem );
				
		while ( hChild ) 
		{
			TreeView_DeleteItem(m_hWnd,  hChild );
			hChild = TreeView_GetChild(m_hWnd, pNMTreeView->itemNew.hItem);
		}
		
		pimpl_->InsertItem(m_hWnd, m_szName, MAX_PATH, 0, 0, pNMTreeView->itemNew.hItem );

		
	}

	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

void CTree::ExpandItem(HTREEITEM hItem, UINT nCode)
{	

	string strPath;

	HTREEITEM hChild = TreeView_GetChild(m_hWnd, hItem);
	while ( hChild )
	{
		TreeView_DeleteItem(m_hWnd, hChild );
		hChild = TreeView_GetChild(m_hWnd, hItem);
	}
    
	strPath = pimpl_->GetFullPath(m_hWnd,  hItem);

	if(strPath != "Album" && strPath != "Catalogue")
		Tree_DisplayFolder(hItem, strPath.c_str());

}

string CTree::GetFullPath(HTREEITEM hItem)
{
	return pimpl_->GetFullPath(m_hWnd,  hItem);
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

string CTreePrv::GetFullPath(HWND hwndTree, HTREEITEM hItem)
{
	// get the Full Path of the item
	string strReturn;
	string strTemp;
	HTREEITEM hParent = hItem;
	int iPos;

	strReturn = "";

	while ( hParent )
	{
		
		strTemp  = GetItemText(hwndTree, hParent );
		strTemp += '\\';
		strReturn = strTemp + strReturn;
		hParent = TreeView_GetParent(hwndTree,  hParent );
	}
    
	iPos = strReturn.find_last_of("\\");
	if (iPos != 0)
	    strReturn = strReturn.substr(0,iPos);

    return strReturn;

}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

string CTree::GetItemText(HTREEITEM hItem)
{
	return pimpl_->GetItemText(m_hWnd, hItem);
}

string CTreePrv::GetItemText(HWND hwndTree, HTREEITEM hItem)
{
	string sValue;
	TV_ITEM tItem;
	HTREEITEM hParent;
	tItem.hItem = hItem;
	tItem.mask = TVIF_TEXT;
	tItem.pszText = (LPTSTR)malloc(255);
	int iTypeAllocation = 1;
	if(tItem.pszText == NULL)
	{
		//Probléme d'allocation mémoire
		//Essai d'allocation dans le tas
		iTypeAllocation = 2;
		tItem.pszText = (LPTSTR) HeapAlloc (GetProcessHeap (), 0, 255);
	}
	tItem.cchTextMax = 255;
	TreeView_GetItem(hwndTree, &tItem);
	sValue = tItem.pszText;

	switch(iTypeAllocation)
	{
		case 2:
			HeapFree(GetProcessHeap (), 0,tItem.pszText);
			break;

		default:
			free(tItem.pszText);
	}

	hParent = TreeView_GetParent(hwndTree,  hItem);

	if(sValue != "" && hParent == NULL)
	{
		SpecialFolderVector::const_iterator fit;
		for (SpecialFolder  SpecialFolderTemp : pTreeData.m_SpFolder)
		{
			if (sValue == SpecialFolderTemp.m_szAffichageName.c_str())
			{
				sValue = SpecialFolderTemp.m_szRealName;
				break;
			}
		}
		/*
		for (fit = pTreeData.m_SpFolder.begin(); fit!= pTreeData.m_SpFolder.end(); fit++)
		{
			SpecialFolder SpecialFolderTemp = *fit;
			if(sValue == SpecialFolderTemp.m_szAffichageName.c_str())
			{
				sValue = SpecialFolderTemp.m_szRealName;
				break;
			}
		}	*/		
	}

	return sValue;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

void CTree::TreeViewGetSelectName(char *cDirName, int iSize)
{

	string strReturn;
	string strTemp;
	HTREEITEM hParent = TreeView_GetSelection(m_hWnd);
	int iPos;

	strReturn = "";

	while ( hParent)
	{
		strTemp = GetItemText(hParent );
		strTemp += '\\';
		strReturn = strTemp + strReturn;
		hParent = TreeView_GetParent(m_hWnd,  hParent );
	}

	iPos = strReturn.find_last_of("\\");
	if (iPos != 0)
	    strReturn = strReturn.substr(0,iPos);

	//strcpy(cDirName,strReturn.c_str());
	StringCchCopy(cDirName,iSize,strReturn.c_str());
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

void CTree::TreeViewGetNodeName(HTREEITEM hChild, char *cDirName, int iSize)
{

	string strReturn;
	string strTemp;
	HTREEITEM hParent = hChild;
	int iPos;

	strReturn = "";

	while ( hParent)
	{
		strTemp  = GetItemText(hParent );
		strTemp += '\\';
		strReturn = strTemp + strReturn;
		hParent = TreeView_GetParent(m_hWnd,  hParent );
	}

	iPos = strReturn.find_last_of("\\");
	if (iPos != 0)
	    strReturn = strReturn.substr(0,iPos);

	//strcpy(cDirName,strReturn.c_str());
	StringCchCopy(cDirName,MAX_PATH,strReturn.c_str());
}

/**************************************************************************

   List_ExecuteCommandMenu

**************************************************************************/

int CTree::Tree_ExecuteCommandMenu(HTREEITEM hItem, 
                        LPMALLOC  g_pMalloc, const char * Verb)
{
	LPITEMIDLIST   *pPidls;
	int cbItems = 1;
	HRESULT        hr;

	pPidls = (LPITEMIDLIST*)g_pMalloc->Alloc(sizeof(LPITEMIDLIST) * cbItems);
	if(!pPidls)
	   return -1;

	TVITEM   tvItem;
	ZeroMemory(&tvItem, sizeof(tvItem));
	tvItem.mask = TVIF_PARAM;
	tvItem.hItem = hItem;

	CItemInfo *  pInfo;

	if(TreeView_GetItem(m_hWnd, &tvItem))
	{
		pInfo = (CItemInfo *)tvItem.lParam;
		pPidls[0] = pInfo->GetPidlRel();
	}
	else
		return -1;

	IShellFolder   * g_psfCurFolder = pInfo->GetShellFolder();

	if(pPidls[0])
	   {
	   
	   IContextMenu   *pcm;

	   if(g_psfCurFolder)
		  {
		  hr = g_psfCurFolder->GetUIObjectOf( m_hWnd, 
											  cbItems, 
											  (LPCITEMIDLIST*)pPidls, 
											  IID_IContextMenu, 
											  NULL, 
											  (LPVOID*)&pcm);



				if(SUCCEEDED(hr))
				   {
				   
						  CMINVOKECOMMANDINFO  cmi;
						  cmi.cbSize = sizeof(CMINVOKECOMMANDINFO);
						  cmi.fMask = 0;
						  cmi.hwnd = (HWND)GetParent(m_hWnd);
						  cmi.lpVerb = Verb;
						  cmi.lpParameters = NULL;
						  cmi.lpDirectory = NULL;
						  cmi.nShow = SW_SHOWNORMAL;
						  cmi.dwHotKey = 0;
						  cmi.hIcon = NULL;
						  hr = pcm->InvokeCommand(&cmi);


			 pcm->Release();
			 }
		  }
	   }

	g_pMalloc->Free(pPidls);

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
//Fonction utilisé lors du parcours des répertoires dans la liste 
//des fichiers
/////////////////////////////////////////////////////////////////////////////

int CTree::Tree_DoItemMenu(HTREEITEM hItem, LPPOINT pptScreen,IContextMenu2  * g_pcm2)
{
TVITEM   tvItem;
ZeroMemory(&tvItem, sizeof(tvItem));
tvItem.mask = TVIF_PARAM;
tvItem.hItem = hItem;
 UINT  idCmd = 0;
if(TreeView_GetItem(m_hWnd, &tvItem))
   {
   HWND           hwndParent = GetParent(m_hWnd);
   HRESULT        hr;
   CItemInfo *     pInfo = (CItemInfo *)tvItem.lParam;
   IContextMenu   *pcm;
   IShellFolder   *psfFolder = pInfo->GetShellFolder();

   if(!psfFolder)
      {
      SHGetDesktopFolder(&psfFolder);
      }
   else
      {
      psfFolder->AddRef();
      }

   if(psfFolder)
      {
		  LPCITEMIDLIST m_lpItemTemp = pInfo->GetPidlRel();
		hr = psfFolder->GetUIObjectOf(   hwndParent, 
                                       1, 
                                       (LPCITEMIDLIST*)&m_lpItemTemp, 
                                       IID_IContextMenu, 
                                       NULL, 
                                       (LPVOID*)&pcm);
		


      if(SUCCEEDED(hr))
         {
         HMENU hPopup;

         hPopup = CreatePopupMenu();
         if(hPopup)
            {
            hr = pcm->QueryContextMenu(hPopup, 0, 1, 0x7fff, CMF_NORMAL | CMF_EXPLORE);

            if(SUCCEEDED(hr))
               {
               pcm->QueryInterface(IID_IContextMenu2, (LPVOID*)&g_pcm2);

              

               idCmd = TrackPopupMenu( hPopup, 
                                       TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON, 
                                       pptScreen->x, 
                                       pptScreen->y,
                                       0,
                                       hwndParent,
                                       NULL);
            
               if(g_pcm2)
                  {
                  g_pcm2->Release();
                  g_pcm2 = NULL;
                  }

               if(idCmd)
                  {
                  CMINVOKECOMMANDINFO  cmi;
                  cmi.cbSize = sizeof(CMINVOKECOMMANDINFO);
                  cmi.fMask = 0;
                  cmi.hwnd = hwndParent;
                  cmi.lpVerb = (LPCSTR)(INT_PTR)(idCmd - 1);
                  cmi.lpParameters = NULL;
                  cmi.lpDirectory = NULL;
                  cmi.nShow = SW_SHOWNORMAL;
                  cmi.dwHotKey = 0;
                  cmi.hIcon = NULL;
                  hr = pcm->InvokeCommand(&cmi);
                  }
               }
            }
      
         pcm->Release();
         }

      psfFolder->Release();
      }
   }

return idCmd;
}

/////////////////////////////////////////////////////////////////////////////
//Fonction utilisé lors du parcours des répertoires dans la liste 
//des fichiers
/////////////////////////////////////////////////////////////////////////////

HRESULT CTree::Tree_DisplayContextMenu(HTREEITEM hParent, const char * m_Path, LPPOINT pptScreen, IContextMenu2  * g_pcm2)
{
	//TVITEM         tvItem;
	CItemInfo *     pItem;
	LPSHELLFOLDER  pParentFolder = NULL;
	HRESULT        hr;
	IShellFolder *psfDeskTop = NULL;
	LPITEMIDLIST pidlDocFiles = NULL;
	ULONG chEaten;
	OLECHAR       olePath[MAX_PATH];
	char cDirectory[MAX_PATH];
	HWND hwndParent = GetParent(m_hWnd);


	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_Path, -1,
					   olePath, MAX_PATH);

    pItem = new CItemInfo();
	if(pItem == NULL)
		return -1;

	hr = SHGetDesktopFolder(&psfDeskTop);

	hr = psfDeskTop->ParseDisplayName(NULL, NULL, olePath, 
		 &chEaten, &pidlDocFiles, NULL);

	pItem->SetPidlFQ(pidlDocFiles);
	pItem->SetPidlRel(pidlDocFiles);
	pItem->SetShellFolder(psfDeskTop);


	   IContextMenu   *pcm;
	   IShellFolder   *psfFolder = pItem->GetShellFolder();

	   if(!psfFolder)
		  {
		  SHGetDesktopFolder(&psfFolder);
		  }
	   else
		  {
		  psfFolder->AddRef();
		  }

	   if(psfFolder)
		  {
			LPCITEMIDLIST m_itemTemp = pItem->GetPidlRel();
		  hr = psfFolder->GetUIObjectOf(hwndParent, 
										   1, 
										   (LPCITEMIDLIST*)&m_itemTemp, 
										   IID_IContextMenu, 
										   NULL, 
										   (LPVOID*)&pcm);

      if(SUCCEEDED(hr))
         {
         HMENU hPopup;

         hPopup = CreatePopupMenu();
         if(hPopup)
            {
            hr = pcm->QueryContextMenu(hPopup, 0, 1, 0x7fff, CMF_NORMAL | CMF_EXPLORE);

            if(SUCCEEDED(hr))
               {
               pcm->QueryInterface(IID_IContextMenu2, (LPVOID*)&g_pcm2);

               UINT  idCmd;

               idCmd = TrackPopupMenu( hPopup, 
                                       TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON, 
                                       pptScreen->x, 
                                       pptScreen->y,
                                       0,
                                       NULL,
                                       NULL);
            
               if(g_pcm2)
                  {
                  g_pcm2->Release();
                  g_pcm2 = NULL;
                  }

               if(idCmd)
                  {
                  CMINVOKECOMMANDINFO  cmi;
                  cmi.cbSize = sizeof(CMINVOKECOMMANDINFO);
                  cmi.fMask = 0;
                  cmi.hwnd = NULL;
                  cmi.lpVerb = (LPCSTR)(INT_PTR)(idCmd - 1);
                  cmi.lpParameters = NULL;
                  cmi.lpDirectory = NULL;
                  cmi.nShow = SW_SHOWNORMAL;
                  cmi.dwHotKey = 0;
                  cmi.hIcon = NULL;
                  hr = pcm->InvokeCommand(&cmi);
                  }
               }
            }
      
         pcm->Release();
         }

      psfFolder->Release();
      }

	/*
	//free up the pidls that we allocated
	pimpl_->m_cPidl.Pidl_Free(pItem->pidlFQ);
	pimpl_->m_cPidl.Pidl_Free(pItem->pidlRel);

	//this may be NULL if this is the root item
	if(pItem->pParentFolder)
		pItem->pParentFolder->Release();

	try
	{
		free((HGLOBAL)pItem);
	}
	catch(...)
	{}
	*/

    if(pItem != NULL)
		delete pItem;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CTree::Tree_DisplayFolder(HTREEITEM hParent, const char * m_Path)
{

	//TVITEM         tvItem;
	CItemInfo *     pItem;
	LPSHELLFOLDER  pParentFolder = NULL;
	HRESULT        hr;
	HCURSOR        hCursor;
	TV_SORTCB      tvSort;
	IShellFolder *psfDeskTop = NULL;
	LPITEMIDLIST pidlDocFiles = NULL;
	ULONG chEaten;
	OLECHAR       olePath[MAX_PATH];
	char cDirectory[MAX_PATH];
	ULONG pdwAttributes = SFGAO_VALIDATE | SFGAO_NONENUMERATED | SFGAO_REMOVABLE | SFGAO_BROWSABLE;

	StringCchPrintf(cDirectory,MAX_PATH,"%s\\",m_Path);

	//Obtention du ClassId pour 


	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, cDirectory, -1,
					   olePath, MAX_PATH);

    pItem = new CItemInfo();

	//pzSourceFiles[0] = '\0';
	hr = SHGetDesktopFolder(&psfDeskTop);

	hr = psfDeskTop->ParseDisplayName(NULL, NULL, olePath, 
		 &chEaten, &pidlDocFiles, &pdwAttributes);

	pItem->SetPidlFQ(pidlDocFiles);
	pItem->SetPidlRel(pidlDocFiles);
	pItem->SetShellFolder(psfDeskTop);

	//get the parent item's pidl

	//change the cursor
	hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));

	//turn redawing off in the TreeView. This will speed things up as we add items
	SendMessage(m_hWnd, WM_SETREDRAW, FALSE, 0);

	//if the parent folder is NULL, then we are at the root of the namespace, so the parent of this item is the desktop folder
	if(!pItem->GetShellFolder())
	{
		hr = SHGetDesktopFolder(&pParentFolder);
	}
	//otherwise we need to get the IShellFolder for this item
	else
	{
		hr = pItem->GetShellFolder()->BindToObject(pItem->GetPidlRel(), NULL, IID_IShellFolder, (LPVOID*)&pParentFolder);
	}

	pimpl_->Tree_EnumObjects(m_hWnd, hParent, pParentFolder, pItem->GetPidlFQ());

	//sort the new items
	tvSort.hParent = hParent;
	tvSort.lpfnCompare = (PFNTVCOMPARE)pimpl_->Tree_CompareProc;
	tvSort.lParam = 0;
	TreeView_SortChildrenCB(m_hWnd, &tvSort, 0);

	//turn redawing back on in the TreeView
	SendMessage(m_hWnd, WM_SETREDRAW, TRUE, 0);
	//UpdateWindow(hwndTreeView);

	SetCursor(LoadCursor(NULL, IDC_ARROW));

	if(pParentFolder != NULL)
		pParentFolder->Release();

	pParentFolder = NULL;

	/*

	//free up the pidls that we allocated
	pimpl_->m_cPidl.Pidl_Free(pItem->pidlFQ);
	pimpl_->m_cPidl.Pidl_Free(pItem->pidlRel);

	//this may be NULL if this is the root item
	if(pItem->pParentFolder)
		pItem->pParentFolder->Release();

	try
	{
		free((HGLOBAL)pItem);
	}
	catch(...)
	{}
	*/

	delete pItem;


	return TRUE;

	//return hr;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////


HTREEITEM CTree::ExpandedItem(HTREEITEM hParent, const char * cDirectory)
{

	char strlwrDirectory[MAX_PATH];
	char strlwrPath[MAX_PATH];
	//strcpy(strlwrDirectory,cDirectory);
	StringCchCopy(strlwrDirectory,MAX_PATH,cDirectory);

	string strPath;
	
	string strPathNew = cDirectory;
	HTREEITEM hItem = NULL;
	HTREEITEM hChild = NULL;

	strPath = pimpl_->GetFullPath(m_hWnd,hParent);

	//strcpy(strlwrPath,strPath.c_str());
	StringCchCopy(strlwrPath,MAX_PATH,strPath.c_str());

	if(!_stricmp(strlwrPath,strlwrDirectory))
	{
		return hParent;
	}

	if(strPathNew == "")	
		strPathNew = strPath;
	else
	{
		strPathNew = strPathNew.substr(strPath.length() + 1,strPathNew.length() - strPath.length() - 1);
	}

	
	if(strPath != "Album" && strPath != "Catalogue")
	{
		//Tree_DisplayFolder(hwndTree, hParent, strPath.c_str());
		ExpandItem(hParent,0);
	}

	hChild = TreeView_GetChild(m_hWnd, hParent );
	while ( hChild )
	{
		if(GetItemText(hChild) == strPathNew)
		{
			return hChild;
		}

		
		hChild = TreeView_GetNextItem(m_hWnd, hChild,TVGN_NEXT );

	}

	return hParent;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void CTreePrv::AddItem(HWND hWndTree,LPITEMIDLIST iItemId, LPSHELLFOLDER  pParentFolder,CItemInfo * pItemChild)
{
	TVINSERTSTRUCT tvInsert;
	SpecialFolder m_SpTemp;
	TVITEM         tvItem;
	SHFILEINFO  sfi;
	DWORD          dwAttribs;
	char Temp[MAX_PATH];
	int i = 0;

	ZeroMemory(&tvItem, sizeof(tvItem));

	//fill in the TV_ITEM structure for this item
	tvItem.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN;


	SHGetPathFromIDList(iItemId, Temp);

	m_SpTemp.m_szRealName = Temp;

	if (strcmp(m_SpTemp.m_szRealName.substr(1,m_SpTemp.m_szRealName.length() - 1).c_str(),"\\") == 0)
	m_SpTemp.m_szRealName += "\\";


	SHGetFileInfo((LPCTSTR)iItemId, 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME | SHGFI_SYSICONINDEX);


	m_SpTemp.m_szAffichageName = sfi.szDisplayName;

	pTreeData.m_SpFolder.push_back(m_SpTemp);

	//pItem->pParentFolder = pParentFolder;
	tvItem.lParam = (LPARAM)pItemChild;

	//text and images are done on a callback basis
	tvItem.pszText = sfi.szDisplayName;
	tvItem.iImage = tvItem.iSelectedImage = sfi.iIcon;

	pTreeData.iNumIconAlbum = sfi.iIcon;

	//tvi.pszText = lpszFileName; 
	tvItem.cchTextMax = lstrlen(sfi.szDisplayName); 

	//determine if the item has children
	dwAttribs = SFGAO_HASSUBFOLDER | SFGAO_FOLDER | SFGAO_DISPLAYATTRMASK | SFGAO_CANRENAME;
	pParentFolder->GetAttributesOf(1, (LPCITEMIDLIST*)&iItemId, &dwAttribs);
	tvItem.cChildren = (dwAttribs & SFGAO_HASSUBFOLDER);

	//determine if the item is shared

	//fill in the TV_INSERTSTRUCT structure for this item
	tvInsert.item = tvItem;
	tvInsert.hInsertAfter = hPrev;
	tvInsert.hParent = TVI_ROOT;

	hPrev = TreeView_InsertItem(hWndTree, &tvInsert);
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

HTREEITEM CTree::AddItem(HTREEITEM hParent, LPCTSTR strPath)
{
	return pimpl_->AddItemToTree(m_hWnd,hParent, strPath);
}


///////////////////////////////////////////////////////////////////////////
//Conversion d'un répertoire spécial vers un nom de répertoire classic
///////////////////////////////////////////////////////////////////////////

int CTree::SpecialFolderToPath(const char * cSpecialFolder, char *Path, int iSize)
{
	CTreePrv::SpecialFolderVector::const_iterator fit;
	for (CTreePrv::SpecialFolder SpecialFolderTemp : pimpl_->pTreeData.m_SpFolder)
	{
		if (strcmp(cSpecialFolder, SpecialFolderTemp.m_szAffichageName.c_str()) == 0)
		{
			//strcpy(Path,SpecialFolderTemp.m_szRealName.c_str());
			StringCchCopy(Path, MAX_PATH, SpecialFolderTemp.m_szRealName.c_str());
			return 1;
		}
	}


	return 0;
}
