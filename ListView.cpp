// ListView.cpp: implementation of the CListView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ListView.h"
#include "Pidl.h"
#include "Shlwapi.h"
#include "objet.h"
#include "statusbar.h"
#include "parameter.h"
#include <ItemInfo.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CListView::CListView()
{
	m_bAfficheImage = false;
	m_ListStatusBar = NULL;
	g_psfCurFolder = NULL;
	g_pidlCurFQ = NULL;
	m_cPidl = new CPidl();
	m_bShowFolder = true;
	m_cParameter = new CParameter();

	NbFiles = 0;
	cDirectory[0] = '\0';
	
}

CListView::~CListView()
{
	delete m_cPidl;
	delete m_cParameter;
}

/**************************************************************************

   List_DoItemMenu

**************************************************************************/

int CListView::List_DoItemMenu(VECTITEM &m_vectItem, 
                        UINT cbItems, 
                        LPPOINT pptScreen, IContextMenu2  *g_pcm2, LPMALLOC  g_pMalloc)
{
	LPITEMIDLIST   *pPidls;
	UINT  idCmd = -1;

	pPidls = (LPITEMIDLIST*)g_pMalloc->Alloc(sizeof(LPITEMIDLIST) * cbItems);
	if(!pPidls)
	   return idCmd;

	LVITEM   lvItem;
	UINT     i;

	for(i = 0; i < cbItems; i++)
	   {
	   ZeroMemory(&lvItem, sizeof(lvItem));
	   lvItem.mask = LVIF_PARAM;
	   lvItem.iItem = m_vectItem[i];
	   if(ListView_GetItem(m_hWnd, &lvItem))
		  {
		  CItemInfo *  pInfo = (CItemInfo *)lvItem.lParam;
		  pPidls[i] = pInfo->GetPidlRel();
		  }
	   }

	if(pPidls[0])
	   {
	   HRESULT        hr;
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
										   GetParent(m_hWnd),
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
						  cmi.hwnd = (HWND)GetParent(m_hWnd);
						  cmi.lpVerb = (LPCSTR)(INT_PTR)(idCmd - 1);
						  cmi.lpParameters = NULL;
						  cmi.lpDirectory = NULL;
						  cmi.nShow = SW_SHOWNORMAL;
						  cmi.dwHotKey = 0;
						  cmi.hIcon = NULL;
						  hr = pcm->InvokeCommand(&cmi);

						  /*
						  The operation performed by the context menu may have changed 
						  the contents of the folder, so do a refresh.
						  */
						  //List_Refresh(hwndListView);
					  }
				   }
				}

			 pcm->Release();
			 }
		  }
	   }

	g_pMalloc->Free(pPidls);

	return idCmd;
}
/**************************************************************************

   List_ExecuteCommandMenu

**************************************************************************/

void CListView::List_ExecuteCommandMenu(VECTITEM &m_vectItem, 
                        UINT cbItems, 
                        LPMALLOC  g_pMalloc, const char * Verb)
{
	LPITEMIDLIST   *pPidls;

	pPidls = (LPITEMIDLIST*)g_pMalloc->Alloc(sizeof(LPITEMIDLIST) * cbItems);
	if(!pPidls)
	   return;

	LVITEM   lvItem;
	UINT     i;

	for(i = 0; i < cbItems; i++)
	   {
	   ZeroMemory(&lvItem, sizeof(lvItem));
	   lvItem.mask = LVIF_PARAM;
	   lvItem.iItem = m_vectItem[i];
	   if(ListView_GetItem(m_hWnd, &lvItem))
		  {
		  CItemInfo *  pInfo = (CItemInfo *)lvItem.lParam;
		  pPidls[i] = pInfo->GetPidlRel();
		  }
	   }

	if(pPidls[0])
	   {
	   HRESULT        hr;
	   IContextMenu   *pcm;

	   if(g_psfCurFolder)
		  {
		  hr = g_psfCurFolder->GetUIObjectOf( m_hWnd, 
											  cbItems, 
											  (LPCITEMIDLIST*)pPidls, 
											  IID_IContextMenu, 
											  NULL, 
											  (LPVOID*)&pcm);

		  //if(SUCCEEDED(hr))
		//	 {
			 //HMENU hPopup;
			 //hPopup = CreatePopupMenu();
			 //if(hPopup)
			//	{
			//	hr = pcm->QueryContextMenu(hPopup, 0, 1, 0x7fff, CMF_NORMAL | CMF_EXPLORE);

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

						  /*
						  The operation performed by the context menu may have changed 
						  the contents of the folder, so do a refresh.
						  */
						  //List_Refresh(hwndListView);
				   //}
				//}

			 pcm->Release();
			 }
		  }
	   }

	g_pMalloc->Free(pPidls);
}
/**************************************************************************

   List_GetItemText()

**************************************************************************/


void CListView::List_GetItemText(int hItem, char * szLabel, int iSize)
{
	LVITEM pitem;
	ZeroMemory(&pitem,sizeof(LVITEM));

	pitem.mask = LVIF_PARAM;
	pitem.iItem = hItem;
	ListView_GetItem(m_hWnd, &pitem);

	CItemInfo * pItem = (CItemInfo *)pitem.lParam;

	if(pItem != NULL)
	{
		StringCchCopy(szLabel,iSize,pItem->GetParseName());
	}
	else
	{
		szLabel[0] = '\0';
	}
}	

/**************************************************************************

   List_DisplayFolder()

**************************************************************************/

HRESULT CListView::List_DisplayFolder(const char * m_Path)
{
	CItemInfo * pItemInfo;
	IShellFolder *psfDeskTop = NULL;
	LPITEMIDLIST pidlDocFiles = NULL;
	ULONG chEaten;
	HRESULT hr;
	OLECHAR       olePath[MAX_PATH];
	ULONG pdwAttributes = SFGAO_VALIDATE;
	NbFiles = 0;
	
	if(m_Path[strlen(m_Path) - 1] != '\\')
		StringCchPrintf(cDirectory,MAX_PATH,"%s\\",m_Path);
	else
		StringCchPrintf(cDirectory,MAX_PATH,"%s",m_Path);

   MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, cDirectory, -1,
					   olePath, MAX_PATH);

    pItemInfo = new CItemInfo();

	//pzSourceFiles[0] = '\0';
	hr = SHGetDesktopFolder(&psfDeskTop);

	hr = psfDeskTop->ParseDisplayName(NULL, NULL, olePath, 
		 &chEaten, &pidlDocFiles, &pdwAttributes);

	pItemInfo->SetPidlFQ(pidlDocFiles);
	pItemInfo->SetPidlRel(pidlDocFiles);
	pItemInfo->SetShellFolder(psfDeskTop);


	if(pItemInfo)
	{
		List_ReleaseCurrentFolder();
		List_LockCurrentFolder(pItemInfo);
	}

	hr = E_FAIL;

	ListView_DeleteAllItems(m_hWnd);

	if(g_psfCurFolder)
	{
		HCURSOR  hCur = LoadCursor(NULL, IDC_WAIT);

		hCur = SetCursor(hCur);

		//turn redawing off in the ListView. This will speed things up as we add items
		SendMessage(m_hWnd, WM_SETREDRAW, FALSE, 0);

		if(!(pdwAttributes & SFGAO_VALIDATE))
		{
			List_EnumObjects(g_psfCurFolder, g_pidlCurFQ);

			ListView_SortItems(m_hWnd, List_CompareProc, 0);
		}

		//turn redawing back on
		SendMessage(m_hWnd, WM_SETREDRAW, TRUE, 0);

		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}

	/*
	//free up the pidls that we allocated
	m_cPidl->Pidl_Free(pItemInfo->pidlFQ);
	m_cPidl->Pidl_Free(pItemInfo->pidlRel);

	//this may be NULL if this is the root item
	if(pItemInfo->pParentFolder)
		pItemInfo->pParentFolder->Release();

	try
	{
		free((HGLOBAL)pItemInfo);
	}
	catch(...)
	{}
	*/

	delete pItemInfo;

	return hr;
}

/**************************************************************************

   List_LockCurrentFolder()

**************************************************************************/

void CListView::List_LockCurrentFolder(CItemInfo * pItemInfo)
{
	if(pItemInfo && pItemInfo->GetShellFolder())
	{
		pItemInfo->GetShellFolder()->BindToObject(pItemInfo->GetPidlRel(), NULL, IID_IShellFolder, (LPVOID*)&g_psfCurFolder);

		g_pidlCurFQ = m_cPidl->Pidl_Copy(pItemInfo->GetPidlFQ());
	}
	else
	{
		SHGetDesktopFolder(&g_psfCurFolder);

		if(pItemInfo)
		  g_pidlCurFQ = m_cPidl->Pidl_Copy(pItemInfo->GetPidlFQ());
	}
}
/**************************************************************************

   List_DisplayAlbum()

**************************************************************************/


HRESULT CListView::List_DisplayAlbum(IconeFileVector * m_IconeFileVector)
{
	CItemInfo * pItemInfo;
	IShellFolder *psfDeskTop = NULL;
	LPITEMIDLIST pidlDocFiles = NULL;
	HRESULT hr;

	pItemInfo = new CItemInfo();
	NbFiles = 0;
	
	if(pItemInfo)
	{
		List_ReleaseCurrentFolder();
	}
	

	hr = E_FAIL;

	ListView_DeleteAllItems(m_hWnd);

	HCURSOR  hCur = LoadCursor(NULL, IDC_WAIT);

	hCur = SetCursor(hCur);

	//turn redawing off in the ListView. This will speed things up as we add items
	SendMessage(m_hWnd, WM_SETREDRAW, FALSE, 0);

	List_EnumAlbum(m_IconeFileVector);

	//ListView_SortItems(hwndListView, List_CompareProc, 0);

	//turn redawing back on
	SendMessage(m_hWnd, WM_SETREDRAW, TRUE, 0);

	SetCursor(SetCursor(LoadCursor(NULL, IDC_ARROW)));

	delete pItemInfo;

	//GlobalFree((HGLOBAL)pItemInfo);

	  SHGetDesktopFolder(&g_psfCurFolder);

	return hr;

}

/**************************************************************************

   List_ReleaseCurrentFolder

**************************************************************************/

void CListView::List_ReleaseCurrentFolder(void)
{
	if(g_psfCurFolder)
	{
		g_psfCurFolder->Release();
		g_psfCurFolder = NULL;

		m_cPidl->Pidl_Free(g_pidlCurFQ);
		g_pidlCurFQ = NULL;
	}
}

/**************************************************************************

   List_CompareProc

**************************************************************************/

int CListView::List_CompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	UNREFERENCED_PARAMETER(lParamSort);

	CItemInfo *  pItem1 = (CItemInfo *)lParam1;
	CItemInfo *  pItem2 = (CItemInfo *)lParam2;
	HRESULT     hr;

	if(pItem1->GetPidlRel() != NULL && pItem2->GetPidlRel() != NULL)
	{ 

		hr = pItem1->GetShellFolder()->CompareIDs(   0,
												  pItem1->GetPidlRel(),
												  pItem2->GetPidlRel());
	}

	if(FAILED(hr))
	   return 0;

	return (short)SCODE_CODE(GetScode(hr));
}

/**************************************************************************

   List_EnumAlbum

**************************************************************************/


void CListView::List_EnumAlbum(IconeFileVector * m_IconeFileVector)
{
	CIconeFile m_IconeFileTemp;
	CObjet m_cObjet;
	char m_cTemp[MAX_PATH];
	NbFiles = 0;
	char m_szMessage[255];
	char cInfos[255];

	for (IconeFileVector::const_iterator dit = m_IconeFileVector->begin(); dit!=m_IconeFileVector->end(); dit++)
	{
		LVITEM   lvItem;
		//DWORD    dwAttr;
		int dIndex;
		CItemInfo *     pItem;

		m_IconeFileTemp = *dit;

		//TCHAR szParseName[MAX_PATH];

		ZeroMemory(&lvItem, sizeof(lvItem));

		//fill in the TV_ITEM structure for this item
		lvItem.mask = LVIF_PARAM | LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;

		//put the private information in the lParam
		pItem = new CItemInfo();
		pItem->SetDate(m_IconeFileTemp.szTimeCreate);
		pItem->SetSize(m_IconeFileTemp.szSize);

		string m_stgInfos = m_IconeFileTemp.informations;
		int iPos = m_stgInfos.find("dimension=");
		if(iPos != -1)
		{
			iPos += strlen("dimension=");
			int iPos2 = m_stgInfos.find(";",iPos);
			pItem->SetDimension(m_stgInfos.substr(iPos,iPos2-iPos).c_str());
		}
		else
		{
			pItem->SetDimension("0x0");
		}

		pItem->SetParseName(m_IconeFileTemp.m_stgFullPathName);
		
		//pItem->pParentFolder = psfDeskTop;
		pItem->SetShellFolder(NULL);

		lvItem.lParam = (LPARAM)pItem;

		lvItem.iItem = NbFiles;

		lvItem.cchTextMax = MAX_PATH;

		//text and images are done on a callback basis
		m_cObjet.GetFileName(m_IconeFileTemp.m_stgFullPathName,m_cTemp,MAX_PATH);
		pItem->SetFileName(m_cTemp);
		lvItem.pszText = pItem->GetFileName();
		
		lvItem.iImage = I_IMAGECALLBACK;

		dIndex = ListView_InsertItem(m_hWnd, &lvItem);


		lvItem.iItem = dIndex;
		lvItem.iSubItem = 1;

		ListView_SetItem(m_hWnd,&lvItem);

		lvItem.iItem = dIndex;
		lvItem.iSubItem = 2;

		ListView_SetItem(m_hWnd,&lvItem);

		NbFiles++;


		LoadString(hInstance,IDS_STGTOTALFICHIER,m_szMessage,255);

		StringCchPrintf(cInfos,255,m_szMessage,NbFiles);
		if(m_ListStatusBar != NULL)
			m_ListStatusBar->UpdateStatusBar(cInfos, 0, 0);

		//Sleep(200);

	}

}

/**************************************************************************

   List_UpdateFolder()

**************************************************************************/

HRESULT CListView::List_UpdateFolder()
{
	CItemInfo * pItemInfo;
	IShellFolder *psfDeskTop = NULL;
	LPITEMIDLIST pidlDocFiles = NULL;
	ULONG chEaten;
	HRESULT hr;
	OLECHAR       olePath[MAX_PATH];
	ULONG pdwAttributes = SFGAO_VALIDATE;
	NbFiles = 0;
	
   MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, cDirectory, -1,
					   olePath, MAX_PATH);

    pItemInfo = new CItemInfo();

	hr = SHGetDesktopFolder(&psfDeskTop);

	hr = psfDeskTop->ParseDisplayName(NULL, NULL, olePath, 
		 &chEaten, &pidlDocFiles, &pdwAttributes);

	pItemInfo->SetPidlFQ(pidlDocFiles);
	pItemInfo->SetPidlRel(pidlDocFiles);
	pItemInfo->SetShellFolder(psfDeskTop);


	if(pItemInfo)
	{
		List_ReleaseCurrentFolder();
		List_LockCurrentFolder(pItemInfo);
	}

	hr = E_FAIL;

	if(g_psfCurFolder)
	{
		if(!(pdwAttributes & SFGAO_VALIDATE))
		{
			SendMessage(m_hWnd, WM_SETREDRAW, FALSE, 0);
			List_EnumObjects(g_psfCurFolder, g_pidlCurFQ,true);
			SendMessage(m_hWnd, WM_SETREDRAW, TRUE, 0);
		}
	}

	delete pItemInfo;
	return hr;
}

/**************************************************************************

   List_EnumObjects

**************************************************************************/

void CListView::List_EnumObjects(LPSHELLFOLDER pParentFolder, 
                        LPITEMIDLIST pidlParent,const bool &m_bUpdate)
{
LPENUMIDLIST   pEnum;
int iNumItem = 0;
long int iTotalSize = 0;
int iLastNameView = 0;
char pszBuf[MAX_PATH];	
char cInfos[MAX_PATH];
char m_szMessage[255];
LoadString(hInstance,IDS_STGTOTALFICHIER,m_szMessage,255);

if(SUCCEEDED(pParentFolder->EnumObjects(NULL,SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, 
			 &pEnum)))
{
   LPITEMIDLIST   pidlTemp;
   DWORD          dwFetched = 1;
   CItemInfo *     pItem;
   CObjet m_cObjet;
   long NbFiles = 0;
   if(m_bUpdate)
	   NbFiles = ListView_GetItemCount(m_hWnd);

while((S_OK == pEnum->Next(1,&pidlTemp, &dwFetched)) && (dwFetched) == 1)
{
	  CObjet m_cObjet;
	  //char m_szFilename[512];
      LVITEM   lvItem;
      DWORD    dwAttr;
	  STRRET strDispName;
	  int dIndex;
	  int iResultFolder = 0;
	  NbFiles++;
	//	TCHAR szParseName[MAX_PATH];

      ZeroMemory(&lvItem, sizeof(lvItem));
      
      //fill in the TV_ITEM structure for this item
      lvItem.mask = LVIF_PARAM | LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;

      //AddRef the parent folder so it's pointer stays valid
      pParentFolder->AddRef();

      //put the private information in the lParam
      pItem = new CItemInfo();

	  pParentFolder->GetDisplayNameOf(pidlTemp, SHGDN_FORPARSING, &strDispName);
	  StrRetToBuf(&strDispName, pidlTemp, pszBuf , MAX_PATH);

	  pItem->SetParseName(pszBuf);

      pItem->SetPidlRel(pidlTemp);
      pItem->SetPidlFQ(m_cPidl->Pidl_Concatenate(pidlParent, pidlTemp));
      pItem->SetShellFolder(pParentFolder);
	  pItem->SetImage(0);
	  pItem->SetIPos(-1);
	  m_cObjet.GetFileName(pItem->GetParseName(),pszBuf,MAX_PATH);
	  pItem->SetFileName(pszBuf);

	  if(iLastNameView == 0)
	  {

		  m_cObjet.GetDiskSpaceInfos(pItem->GetParseName(),cInfos,MAX_PATH);
		  if(m_ListStatusBar != NULL)
			m_ListStatusBar->UpdateStatusBar(cInfos, 4, 0);

		  iLastNameView = 1;
	  }

	_finddata_t  c_file;
	long fhandle = _findfirst(pItem->GetParseName(), &c_file);
	if(fhandle != -1)
	{
		iTotalSize += c_file.size;
	}

	  if(m_bUpdate)
	  {
			
			
		  //Vérification si l'item existe déjà
			LVFINDINFO lvInfos;
			int iItem;
			POINT pt;

			pt.x = 0;
			pt.y = 0;

			lvInfos.flags = LVFI_STRING;
			lvInfos.psz = pItem->GetFileName();
			lvInfos.lParam = NULL;
			lvInfos.pt = pt;
			lvInfos.vkDirection = VK_DOWN;

			iItem = ListView_FindItem(m_hWnd,-1,&lvInfos);

			if(iItem != -1)
			{
				continue;
			}
	  }

  
      lvItem.lParam = (LPARAM)pItem;
	  lvItem.pszText = pItem->GetFileName();
      //text and images are done on a callback basis
      //lvItem.pszText = LPSTR_TEXTCALLBACK;
      lvItem.iImage = I_IMAGECALLBACK;

      //determine if the item is shared
      dwAttr = SFGAO_DISPLAYATTRMASK;
      pParentFolder->GetAttributesOf(1, (LPCITEMIDLIST*)&pidlTemp, &dwAttr);

      if(dwAttr & SFGAO_SHARE)
         {
         lvItem.mask |= LVIF_STATE;
         lvItem.stateMask |= LVIS_OVERLAYMASK;
         lvItem.state |= INDEXTOOVERLAYMASK(1); //1 is the index for the shared overlay image
         }

      if(dwAttr & SFGAO_GHOSTED)
         {
         lvItem.mask |= LVIF_STATE;
         lvItem.stateMask |= LVIS_CUT;
         lvItem.state |= LVIS_CUT;
         }

		if(m_bAfficheImage)
		{
			if(m_bShowFolder)
			{
				if(!(dwAttr & SFGAO_FOLDER) && m_cObjet.TestImageFormat(pItem->GetParseName()) == 0  && m_cObjet.TestVideoFormat(pItem->GetParseName()) == 0)
				{
					delete pItem;
					continue;
				}
			}
			else
			{
				if(m_cObjet.TestImageFormat(pItem->GetParseName()) == 0 && m_cObjet.TestVideoFormat(pItem->GetParseName()) == 0)
				{
					delete pItem;
					continue;
				}
			}

		}
      
		dIndex = ListView_InsertItem(m_hWnd, &lvItem);

		//Ajout de l'information sur la Taille

		lvItem.iItem = dIndex;
		lvItem.iSubItem = 1;

		ListView_SetItem(m_hWnd,&lvItem);
		
		lvItem.iItem = dIndex;
		lvItem.iSubItem = 2;

		ListView_SetItem(m_hWnd,&lvItem);

		StringCchPrintf(cInfos,255,m_szMessage,NbFiles);
		if(m_ListStatusBar != NULL)
			m_ListStatusBar->UpdateStatusBar(cInfos, 0, 0);


      dwFetched = 0;
      }
   pEnum->Release();
   }

    NbFiles = ListView_GetItemCount(m_hWnd);
	double m_fSize = (iTotalSize / 1024.0) / 1024.0;
	LoadString(hInstance,IDS_STGTOTALFICHIERFREE,m_szMessage,255);
	StringCchPrintf(cInfos,255,m_szMessage,NbFiles,m_fSize);
	if(m_ListStatusBar != NULL)
		m_ListStatusBar->UpdateStatusBar(cInfos, 0, 0);


}


void CListView::List_EnsureVisible(const int &iItem, const int &iOldPos)
{
	RECT rc, rc2;
	int y = 0;
	if(ListView_GetItemRect(m_hWnd,iItem,&rc2,LVIR_BOUNDS))
	{
		GetClientRect(m_hWnd,&rc);

		y = (rc.bottom - rc.top) / (rc2.bottom - rc2.top);

		y /= 2;

		if(iItem > (y + 1) && iItem < (ListView_GetItemCount(m_hWnd) - y - 1))
		{
			if(iItem > iOldPos)
				ListView_EnsureVisible(m_hWnd,iItem + y,false);
			else
				ListView_EnsureVisible(m_hWnd,iItem - y,false);
		}
		else
			ListView_EnsureVisible(m_hWnd,iItem,false);
	}
}
