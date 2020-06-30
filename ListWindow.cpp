// ListWindow.cpp: implementation of the CListWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ListWindow.h"
#include ".\listwindowdata.h"
#include "graveurisowizard.h"
#include "tree.h"
#include "statusbar.h"
#include "catalogue1.h"
#include "WebBrowser.h"
#include "AlbumWizard.h"
#include "ImageCatalogue.h"
#include "Album.h"
#include "objet.h"
#include <SelectBurner.h>
#include <ItemInfo.h>
#include <albumdata.h>
#include <Iconefile.h>
#include <dialogcataloguedata.h>
#include <DeplaceFile.h>
#include <definerepmonitor.h>
#include <DeplaceFichier.h>
#include <SelectAlbum.h>
#include <AttributFichier.h>
#include <AttributRepertoire.h>
#include <NewAlbum.h>
#include <GestionCategorie.h>
#include "fichier.h"
#include "parameter.h"
#include <shlobj.h>

#ifdef SQLSERVERCE	
#include <SqlAlbumAttribut.h>
#include <SqlAlbumUtility.h>
#include <SqlAlbumIconeFileAttribut.h>
#include <SqlIconeFileAlbum.h>
#include <SqlEngine.h>
#include <SqlLib.h>
using namespace LIBSQLSERVERCE;
#endif




/////////////////////////////////////////////////////////////////////////////////
//Fonction de récupération des mises à jour d'un répertoire
/////////////////////////////////////////////////////////////////////////////////

UINT _stdcall CListWindow::MonitorDirectoryProc(LPVOID pvThis)
{
	DWORD dwValue = WAIT_OBJECT_0;
	DWORD dwWaitStatus; 
	HANDLE dwChangeHandles[1]; 
	CDefineRepMonitor * m_define = (CDefineRepMonitor *)pvThis;
 
	// Watch the C:\WINDOWS directory for file creation and 
	// deletion. 
 
	if(!CObjet::IsValidateFolder(m_define->DirName))
	{
		//IDS_DRIVENOTAVAILABLE
		//CObjet::AffichageMessage(IDS_DRIVENOTAVAILABLE,IDS_INFOS);
		//MessageBox(NULL,"Le lecteur en cours n'est pas disponible !","Informations",0);
		return 0;
	}
	

	dwChangeHandles[0] = FindFirstChangeNotification( 
		m_define->DirName,                 // directory to watch 
		TRUE,                         // do not watch the subtree 
		FILE_NOTIFY_CHANGE_DIR_NAME); // watch file name changes 
 
	if (dwChangeHandles[0] == INVALID_HANDLE_VALUE) 
		return 0;
 
 
	// Change notification is set. Now wait on both notification 
	// handles and refresh accordingly. 
 
	while (TRUE) 
	{ 
 
		// Wait for notification.

		Sleep(2000);
 
		dwWaitStatus = MsgWaitForMultipleObjects(1, &dwChangeHandles[0], 
			FALSE, INFINITE,QS_ALLINPUT); 

        if (dwWaitStatus >= dwValue)
		{ 
			SendMessage(m_define->hWnd,WM_COMMAND,MAKEWPARAM(ID_AFFICHAGE_ACTUALISER,0),(LPARAM)m_define->DirName);
		} 

		FindNextChangeNotification(dwChangeHandles[0]);
	} 

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////
//Fonction de récupération des mises à jour d'un répertoire
/////////////////////////////////////////////////////////////////////////////////

LRESULT CListWindow::OnMediaChange(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    SHNOTIFYSTRUCT *shns = (SHNOTIFYSTRUCT *)wParam;
    string strPath;//, strMsg;

    switch(lParam)
    {
        case SHCNE_MEDIAINSERTED:        // media inserted event
        {
            strPath = GetPathFromPIDL(shns->dwItem1);
            //if(!strPath.IsEmpty())
            //{
                // Process strPath as required, for now a simple message box
                //strMsg.Format("Media inserted into %s", strPath);
            //}
			break;
        }
        case SHCNE_MEDIAREMOVED:        // media removed event
        {
            strPath = GetPathFromPIDL(shns->dwItem1);
            //if(!strPath.IsEmpty())
            //{
                // Process strPath as required, for now a simple message box
                //strMsg.Format("Media removed from %s", strPath);
            //}
			break;
        }
		case SHCNE_DRIVEADD:
        {
            strPath = GetPathFromPIDL(shns->dwItem1);
			if(strPath.length() > 0)
				m_pListData->classTree->AddDriveItem(strPath);
			break;
        }
		case SHCNE_DRIVEREMOVED:
        {
            strPath = GetPathFromPIDL(shns->dwItem1);
			if(strPath.length() > 0)
			{
				HTREEITEM hPrevious = m_pListData->classTree->DeleteDriveItem(strPath);
				//Get Path
				if(hPrevious != NULL)
				{	
					string m_stgDirectory = m_pListData->classTree->GetFullPath(hPrevious);
					char drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR], drive2[_MAX_DRIVE];
					_splitpath_s(m_pListData->cDirectory, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
					_splitpath_s(strPath.c_str(), drive2, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);				
					
					if(strcmp(drive,drive2) == 0)
					{
						strcpy_s(m_pListData->cDirectory, MAX_PATH,m_stgDirectory.c_str());
						SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_INITDIRECTORY,0),(LPARAM)m_pListData->cDirectory);
					}
				}
			}
			break;
        }
        //case SHCNE_xxxx:  Add other events processing here
    }
	return 0;
}

string CListWindow::GetPathFromPIDL(DWORD pidl)
{
    char sPath[MAX_PATH];
    string strTemp = "";
    if(SHGetPathFromIDList((struct _ITEMIDLIST *)pidl, sPath))
        strTemp = sPath;
    
    return strTemp;
}

/////////////////////////////////////////////////////////////////////////////////
//Fonction de récupération des mises à jour d'un répertoire
/////////////////////////////////////////////////////////////////////////////////

UINT CListWindow::RegisterDriveNotification(HWND hWnd)
{
	LPITEMIDLIST ppidl;
	if(SHGetSpecialFolderLocation(hWnd, CSIDL_DESKTOP, &ppidl) == NOERROR)
	{
		SHChangeNotifyEntry shCNE;
		shCNE.pidl = ppidl;
		shCNE.fRecursive = TRUE;

		// Returns a positive integer registration identifier (ID).
		// Returns zero if out of memory or in response to invalid parameters.
		m_ulSHChangeNotifyRegister = SHChangeNotifyRegister(hWnd,  
										  // Hwnd to receive notification
				SHCNE_DISKEVENTS,                          
										  // Event types of interest (sources)
				SHCNE_MEDIAINSERTED|SHCNE_MEDIAREMOVED|SHCNE_DRIVEADD|SHCNE_DRIVEREMOVED,    
										  // Events of interest - use 
										  // SHCNE_ALLEVENTS for all events
				WM_USER_MEDIACHANGED,     
										  // Notification message to be sent 
										  // upon the event
				1,                         
										  // Number of entries in the pfsne array
				&shCNE);  // Array of SHChangeNotifyEntry structures that 
						  // contain the notifications. This array should 
						  // always be set to one when calling SHChnageNotifyRegister
						  // or SHChangeNotifyDeregister will not work properly.
    }

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////
//Fonction de récupération des mises à jour d'un répertoire
/////////////////////////////////////////////////////////////////////////////////

UINT CListWindow::DeRegisterDriveNotification(HWND hWnd)
{
	if(m_ulSHChangeNotifyRegister)
		SHChangeNotifyDeregister(m_ulSHChangeNotifyRegister);

	return 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CListWindow::CListWindow()
{
	m_pListData = new CListWindowData();
	m_cParameter = new CParameter();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CListWindow::~CListWindow()
{
	if(m_pListData != NULL)
		delete m_pListData;

	m_pListData = NULL;

	delete m_cParameter;

}

LRESULT CListWindow::OnDestroy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	KillTimer(hWnd,TIMER_SAVEDATA);
	m_pListData->m_CAlbumCatalogue->SaveAlbum();
	DestroyWindow(m_pListData->classTree->m_hWnd);
	DestroyWindow(m_pListData->m_CWebBrowser->m_hWnd);
	DeRegisterDriveNotification(hWnd);
	PostQuitMessage(0);
	return 0;
}





LRESULT CListWindow::OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
 	PAINTSTRUCT ps;
 	HDC hDC = BeginPaint( hWnd, &ps );
 	SendMessage(m_pListData->classTree->m_hWnd,message,wParam,lParam);
	SendMessage(m_pListData->m_CWebBrowser->m_hWnd,message,wParam,lParam);
 	EndPaint( hWnd, &ps );
	ReleaseDC(hWnd,hDC);
 	return 0;
}



/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CListWindow::OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	/////////////////////////////////////////
	//Récupération des différents messages
	//du controle treeview
	/////////////////////////////////////////

	int idCtrl;
	
	LPNMHDR pnmh;
	idCtrl = (int) wParam; 
	pnmh = (LPNMHDR) lParam; 
	LRESULT lResult;
	NM_TREEVIEW* pNMTreeView;
	pNMTreeView = (NM_TREEVIEW*)pnmh;


	switch (((LPNMHDR)lParam)->code)
	{
		case TVN_GETDISPINFO:
			m_pListData->classTree->Tree_Notify(lParam);
			break;

		case TVN_BEGINLABELEDIT:
			m_pListData->classTree->TreeViewGetSelectName(m_pListData->cDirectory,MAX_PATH);
			m_pListData->iNumAlbum = m_pListData->m_CAlbumCatalogue->DetermineNumAlbum(m_pListData->cDirectory);
			if(m_pListData->iNumAlbum == -1)
			{
				TreeView_EndEditLabelNow(m_pListData->classTree->m_hWnd, TRUE);
				return 0;
			}
			else
				return 0;
			break;

		case TVN_ENDLABELEDIT:
			{
				//Modification du libelle dans l'arbre
				if(m_pListData->iNumAlbum != -1)
				{
					NMTVDISPINFO * ptvdi;
					ptvdi = (LPNMTVDISPINFO) lParam;

					if(ptvdi->item.pszText != NULL)
					{
						if(m_pListData->m_CAlbumCatalogue->VerifNomAlbum(ptvdi->item.pszText) == -2)
						{
							TreeView_SetItem(m_pListData->classTree->m_hWnd,&ptvdi->item);
							m_pListData->m_CAlbumCatalogue->NewAlbumName(ptvdi->item.pszText,m_pListData->iNumAlbum);						
						}
						else
						{
							CObjet::AffichageMessage(IDS_ALBUMEXISTANT,IDS_INFOS);
						}
					}

				}
				return 0;
			}
			break;


		case TVN_ITEMEXPANDING:
			{
				  LPNMTREEVIEW   pnmtv = (LPNMTREEVIEW)lParam;
				  CObjet m_cObjet;
      
				  switch(pnmtv->action)
					 {
					 case TVE_EXPAND:
						 {
							CItemInfo * pItem = (CItemInfo *)pnmtv->itemNew.lParam;

							m_pListData->classTree->TreeViewGetNodeName(pnmtv->itemNew.hItem,m_pListData->cDirectory,MAX_PATH);

							if(pItem != NULL)
							{

								switch(pItem->GetTypeDonne())
								{
									case 2:
									{
										m_pListData->iNumAlbum = m_pListData->m_CAlbumCatalogue->DetermineNumAlbum(m_pListData->cDirectory);
									}
									break;

									case 3:
									{
										m_pListData->iNumAlbum = m_pListData->m_CImageCatalogue->DetermineNumAlbum(m_pListData->cDirectory);
									}
									break;

									default:
										{

											m_pListData->iNumAlbum = -1;
											m_pListData->classTree->OnItemexpanded(pnmh, &lResult);
											if(m_pListData->g_fDragging || m_pListData->iListDragging)
											{
												m_pListData->classTree->ExpandItem(pNMTreeView->itemNew.hItem);
											}


										}
										break;

								}

								if(!m_pListData->g_fDragging && !m_pListData->iListDragging && !m_pListData->iInitializeFolder)
									m_pListData->SendInitMessage(m_pListData->iNumAlbum,pItem->GetTypeDonne(), GetParent());
							}
							else
							{

								m_pListData->iNumAlbum = -1;
								m_pListData->classTree->OnItemexpanded(pnmh, &lResult);
								if(m_pListData->g_fDragging || m_pListData->iListDragging)
									m_pListData->classTree->ExpandItem(pNMTreeView->itemNew.hItem);

								if(!m_pListData->g_fDragging && !m_pListData->iListDragging && !m_pListData->iInitializeFolder)
									m_pListData->SendInitMessage(m_pListData->iNumAlbum,1, GetParent());

							}

							StringCchCopy(m_pListData->cDirectoryTemp,MAX_PATH,m_pListData->cDirectory);

							TreeView_SelectItem(m_pListData->classTree->m_hWnd,pnmtv->itemNew.hItem);
						 }
						break;

					 default:
						break;

					 }


				return 0;
			}

		
		case TVN_ITEMEXPANDED:
			{	
				
				CItemInfo * pItem = (CItemInfo *)pNMTreeView->itemNew.lParam;

				TreeView_SelectItem(m_pListData->classTree->m_hWnd,pNMTreeView->itemNew.hItem);

				m_pListData->classTree->TreeViewGetSelectName(m_pListData->cDirectory,MAX_PATH);

				if(!m_pListData->g_fDragging && !m_pListData->iListDragging)
				{

					if(pItem != NULL)
					{

						switch(pItem->GetTypeDonne())
						{
							case 2:
							{
								m_pListData->iNumAlbum = m_pListData->m_CAlbumCatalogue->DetermineNumAlbum(m_pListData->cDirectory);

								if(strcmp(m_pListData->cDirectory,"Album") != 0)
									m_pListData->classTree->OnItemexpanded(pnmh, &lResult);
							}
							break;

							case 3:
							{
								m_pListData->iNumAlbum = m_pListData->m_CImageCatalogue->DetermineNumAlbum(m_pListData->cDirectory);

								if(strcmp(m_pListData->cDirectory,"Catalogue") != 0)
									m_pListData->classTree->OnItemexpanded(pnmh, &lResult);
							}
							break;

							default:
							{
								m_pListData->iNumAlbum = -1;
								m_pListData->classTree->OnItemexpanded(pnmh, &lResult);
							}
							break;

						}
				
						m_pListData->SendInitMessage(m_pListData->iNumAlbum,pItem->GetTypeDonne(), GetParent());
					}
					else
					{
						m_pListData->iNumAlbum = -1;
						m_pListData->classTree->OnItemexpanded(pnmh, &lResult);
						m_pListData->SendInitMessage(m_pListData->iNumAlbum,1, GetParent());
					}

					StringCchCopy(m_pListData->cDirectoryTemp,MAX_PATH,m_pListData->cDirectory);

				}
			}

			break;

		case TVM_GETITEM:
			LPTVITEMEX pitem;
			pitem = (LPTVITEMEX) lParam;
			break;

			
		case TVN_SELCHANGED:
			{
				LPNMTREEVIEW lpNmTreeView;
				lpNmTreeView = (LPNMTREEVIEW)lParam;

				CItemInfo * pItem = (CItemInfo *)lpNmTreeView->itemNew.lParam;



				m_pListData->iRecherche = 0;

				if(lpNmTreeView->action == TVC_BYMOUSE ||  lpNmTreeView->action == TVC_BYKEYBOARD)
				{
					m_pListData->classTree->TreeViewGetSelectName(m_pListData->cDirectory,MAX_PATH);

					if(pItem == NULL)
					{
						m_pListData->iNumAlbum = -1;
						m_pListData->SendInitMessage(m_pListData->iNumAlbum,1, GetParent());
					}	
					else
					{
						switch(pItem->GetTypeDonne())
						{
							case 2:
							{
								m_pListData->iNumAlbum = m_pListData->m_CAlbumCatalogue->DetermineNumAlbum(m_pListData->cDirectory);
								if(strcmp(m_pListData->cDirectory,"Album") != 0)
									m_pListData->SendInitMessage(m_pListData->iNumAlbum,pItem->GetTypeDonne(), GetParent());
							}
							break;

							case 3:
							{
								//Interdiction de pouvoir supprimer un fichier
								m_pListData->iRecherche = 4;
								m_pListData->iNumAlbum = m_pListData->m_CImageCatalogue->DetermineNumAlbum(m_pListData->cDirectory);
								if(strcmp(m_pListData->cDirectory,"Catalogue") != 0)
									m_pListData->SendInitMessage(m_pListData->iNumAlbum,pItem->GetTypeDonne(), GetParent());
							}
							break;

							default:
							{

								m_pListData->iNumAlbum = -1;
								m_pListData->SendInitMessage(m_pListData->iNumAlbum,pItem->GetTypeDonne(), GetParent());
							}
							break;

						}
					}
				}
				else
				{
					return 1;
				}
					
				StringCchCopy(m_pListData->cDirectoryTemp,MAX_PATH,m_pListData->cDirectory);
			}
			break;

		case TVN_BEGINDRAG:
		   {
				
				LPNMTREEVIEW lpnmtv = (LPNMTREEVIEW) lParam;
				
				RECT rcItem;        // bounding rectangle of item 
				DWORD dwLevel;      // heading level of item 
				DWORD dwIndent;     // amount that child items are indented 
				IMAGEINFO imf;
				int iHeight;
				POINT pt;
				

				// Tell the tree-view control to create an image to use 
				// for dragging. 
				// For the first selected item, we simply create a single-line drag image
				m_pListData->himl = TreeView_CreateDragImage(m_pListData->classTree->m_hWnd, lpnmtv->itemNew.hItem); 
 				ImageList_GetImageInfo(m_pListData->himl, 0, &imf);
				iHeight = imf.rcImage.bottom;


				// Get the bounding rectangle of the item being dragged. 
				TreeView_GetItemRect(m_pListData->classTree->m_hWnd, lpnmtv->itemNew.hItem, &rcItem, TRUE); 

				// Get the heading level and the amount that the child items are 
				// indented. 
				dwLevel = lpnmtv->itemNew.lParam; 
				dwIndent = (DWORD) SendMessage(m_pListData->classTree->m_hWnd, TVM_GETINDENT, 0, 0); 

				// Start the drag operation. 
				ImageList_BeginDrag(m_pListData->himl, 0, 0, 0); 

				ClientToScreen(m_pListData->classTree->m_hWnd, &pt);

				ImageList_DragEnter(GetDesktopWindow(), pt.x, pt.y);

				// Hide the mouse pointer, and direct mouse input to the 
				// parent window. 
				//ShowCursor(FALSE); 
				SetCapture(GetParent()); 
				m_pListData->g_fDragging = TRUE; 
				SendMessage(GetParent(),WM_COMMAND, MAKEWPARAM(IDM_DRAGGING_TREE,1),0);

				//turn redawing off in the TreeView. This will speed things up as we add items
				SendMessage(m_pListData->classTree->m_hWnd, WM_SETREDRAW, FALSE, 0);

			}
			break;

		case TVN_KEYDOWN:
			{
				NMTVKEYDOWN * ptvkd;
				ptvkd = (LPNMTVKEYDOWN) lParam;
				switch(ptvkd->wVKey)
				{
					case VK_DELETE:
						{
							TV_ITEM tItem;
							tItem.hItem = TreeView_GetSelection(m_pListData->classTree->m_hWnd);
							tItem.mask = TVIF_HANDLE | TVIF_PARAM;

							if(TreeView_GetItem(m_pListData->classTree->m_hWnd, &tItem))
							{

								CItemInfo * pItem = (CItemInfo *)tItem.lParam;

								switch(pItem->GetTypeDonne())
								{
									case 1:
										{
											int iResult = m_pListData->classTree->Tree_ExecuteCommandMenu(tItem.hItem, m_pListData->g_pMalloc, "delete");
											if(iResult == 0)
												TreeView_DeleteItem(m_pListData->classTree->m_hWnd,tItem.hItem);
										}
										break;

									case 2:
										{
											//Suppression d'un album
											SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(ID_SOUSALBUM_SUPPRIMER,0),0);

										}
										break;

									case 3:
										{
											//Suppression d'un catalogue
											SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(ID_SOUSCATALOGUE_SUPPRIMERCATALOGUE,0),0);
										}
										break;

								}
							}
						}
						break;

					case VK_F2:
						TreeView_EditLabel(m_pListData->classTree->m_hWnd,TreeView_GetSelection(m_pListData->classTree->m_hWnd));
						break;

					case VK_F1:
						SendMessage(GetParent(),WM_KEYDOWN,VK_F1,0);
						break;

					case VK_F5:
						SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(ID_AFFICHAGE_ACTUALISER,0),0);
						break;

					default:
						return 1;
						break;

				}
			}
			break;

	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CListWindow::OnKeyDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CListWindow::OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam); 
	HTREEITEM hItemParent;

	switch(wmId)
	{
		case ID_GRAVURE_GRAVERLESFICHIERS:
			{
				char m_szMessage[255];
				char m_szCaption[255];
				CGraveurISOWizard m_cGraveur;
				LoadString(hInstance,IDS_STGSUPFILE,m_szMessage,255);
				LoadString(hInstance,IDS_INFOS,m_szCaption,255);
				m_cGraveur.CreateFolderWizard(hWnd,m_cParameter->GetBurningDirectory());
				if(MessageBox(NULL,m_szMessage,m_szCaption,MB_YESNO)==IDYES)
				{
					SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(ID_GRAVURE_SUPPRIMERLESFICHIERS,0),0);
				}

			}
			break;

		case ID_GRAVURE_SUPPRIMERLESFICHIERS:
			{
				   // make sure the path ends in a single "\"
					string baseName = m_cParameter->GetBurningDirectory();
					baseName+='\\';
					   
					string fullPath = baseName;
					fullPath += "*.*";
				   
					string fileName;
					   
					TCHAR drive[_MAX_DRIVE],m_cdir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
						
					try 
					{
						// find first file in current directory
						_finddata_t  c_file;
						long fhandle;
					      
						if ((fhandle=_findfirst( fullPath.c_str(), &c_file ))!=-1) 
						{
							if ((c_file.attrib & _A_SUBDIR)!=_A_SUBDIR) 
							{
								
								string m_stgFilepath = baseName;
								m_stgFilepath += c_file.name;
								_chmod(m_stgFilepath.c_str(),_S_IREAD | _S_IWRITE);
								DeleteFile(m_stgFilepath.c_str());
							}
						}
				         
						// find the rest of them	
						while(_findnext( fhandle, &c_file ) == 0 ) 
						{
							if ((c_file.attrib & _A_SUBDIR)!=_A_SUBDIR) 
							{
								
								string m_stgFilepath = baseName;
								m_stgFilepath += c_file.name;
								_chmod(m_stgFilepath.c_str(),_S_IREAD | _S_IWRITE);
								DeleteFile(m_stgFilepath.c_str());
							}
							else
							{
								if ((c_file.attrib & _A_SUBDIR)==_A_SUBDIR && strcmp(c_file.name,"..") != 0 && strcmp(c_file.name,".") !=  0) 
								{
									CFichier m_cFichier;
									string m_stgFilepath = baseName;
									m_stgFilepath += c_file.name;
									m_cFichier.DeleteDir(m_stgFilepath.c_str());
								}
							}
						}

						_findclose(fhandle);
					} 
					catch (...) 
					{
						return false;
					}
					  
					SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(ID_AFFICHAGE_ACTUALISER,0),0);
					return true;
			}
			break;

		case ID_GRAVURE_EFFACERLECD:
			{
				//int iValue = DialogBox(hInstance, (LPCTSTR)IDD_SELECTALBUM, hWnd, (DLGPROC)CDialog::SelectBurner);
				CSelectBurner * m_cSelectBurner = new CSelectBurner(hInstance);
				m_cSelectBurner->CreateDlg(hInstance,(LPCTSTR)IDD_SELECTALBUM,hWnd);
				delete m_cSelectBurner;

			}
			break;

		case ID_LISTE_CRERUNRPERTOIRE:
		case ID_AFFICHAGE_ACTUALISER:
			{
				HTREEITEM hTreeItem = TreeView_GetSelection(m_pListData->classTree->m_hWnd);
				m_pListData->classTree->ExpandItem(hTreeItem);
				SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(ID_AFFICHAGE_ACTUALISER,0),0);
			}
			break;

		case ID_SOUSALBUM_GRAVERSURCDROM:
			{
				CGraveurISOWizard m_cGraveur;
				char m_szAlbumName[MAX_PATH];
				string m_stgTemp;
				m_pListData->classTree->TreeViewGetSelectName(m_szAlbumName,MAX_PATH);
				HTREEITEM hParent = TreeView_GetSelection(m_pListData->classTree->m_hWnd);
				m_stgTemp = m_pListData->classTree->GetItemText(hParent);
				//Test si album valide
				if(m_pListData->m_CAlbumCatalogue->DetermineNumAlbum(m_szAlbumName) != -1)
				{
#ifdef SQLSERVERCE
					WCHAR m_wAlbumName[MAX_PATH];
					IconeFileVector m_IconeFileVectorAlbum;
					size_t m_sizeTConvert;	
					mbstowcs_s(&m_sizeTConvert,m_wAlbumName, MAX_PATH, m_pListData->m_Album->m_szAlbumName, MAX_PATH);

					CSqlIconeFileAlbum * m_cSqlIconeFileAlbum = new CSqlIconeFileAlbum();
					m_cSqlIconeFileAlbum->LoadIconeFileAlbum(&m_IconeFileVectorAlbum,m_wAlbumName);
					delete m_cSqlIconeFileAlbum;

					m_cGraveur.CreateAlbumWizard(hWnd,m_stgTemp,3,m_IconeFileVectorAlbum);
#else
					m_cGraveur.CreateAlbumWizard(hWnd,m_stgTemp,3,m_pListData->m_Album->m_IconeFileVector);

#endif
					
				}
			}
			break;

		case ID_RECUP_PTATTRIBUT:
			{
				AttributVector * * m_AttributVector = (AttributVector * *)lParam;
				*m_AttributVector = m_pListData->m_CAlbumCatalogue->RecupPtAttribut();
			}
			break;

		case ID_INITIALIZE_FOLDER:
			{
				
				char m_szPath[MAX_PATH];
				StringCchCopy(m_szPath,MAX_PATH,(const char *)lParam);
				m_pListData->InitializeFolder(hWnd,m_szPath,GetParent());

			}
			break;

		case ID_ALBUM_SUPPRIMER:
			{
				HWND hWndListe = (HWND)HIWORD(wParam);
				VECTITEM * m_vectItem = (VECTITEM *)lParam;

				switch(m_pListData->iRecherche)
				{
					case 0:
					{
						
#ifndef SQLSERVERCE						
						//Récupération du numero de l'album
						char Temp[MAX_PATH];
						m_pListData->classTree->TreeViewGetSelectName(Temp,MAX_PATH);
						m_pListData->iNumAlbum = m_pListData->m_CAlbumCatalogue->DetermineNumAlbum(Temp);
						m_pListData->m_CAlbumCatalogue->DeleteAlbumFile(m_pListData->iNumAlbum,hWndListe,m_vectItem);
#else
						char _AlbumName[MAX_PATH];
						m_pListData->classTree->TreeViewGetSelectName(_AlbumName,MAX_PATH);
						m_pListData->m_CAlbumCatalogue->DeleteAlbum(_AlbumName);

#endif
						break;
					}

					case 3:
					{
						m_pListData->m_CImageCatalogue->DeleteRechercheFile(hWndListe,m_vectItem);
						break;
					}
				}
			}
			break;


		case ID_CATALOGUE_RECHERCHER:
			{
				if(m_pListData->m_CImageCatalogue->Rechercher(hWnd,&m_pListData->m_IconeFileVector) == 1)
				{
					m_pListData->iRecherche = 3;
					SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_ALBUMIMAGELIST,0),(LPARAM)-2);
					SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_INITCATALOGUE,1),(LPARAM)m_pListData->m_IconeFileVector);
				}
				
			}
			break;


		case ID_OUTILS_AFFICHAGERECHERCHE:
			{
				SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(ID_OUTILS_AFFICHAGERECHERCHE,0),0);
			}
			break;

		case IDM_NEW:
			{
				CAlbumWizard m_CAlbumWizard;
				m_CAlbumWizard.CreateAlbumWizard(hWnd,hInstance,SW_SHOW,m_pListData->m_CAlbumCatalogue,&m_pListData->classTree->m_hWnd,m_pListData->classTree);
			}
			break;

		case IDM_FICHIER_SAUVERALBUM:
		case ID_FICHIER_SAUVERALBUM:
			{
				m_pListData->m_CAlbumCatalogue->SaveAlbum();
			}
			break;

		case ID_RECHERCHE:
			{
				//Lancement de la recherche
				int i = 0;
				char cValue[1024];

				CDefineRepMonitor m_define;
				SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_GETHWNDPRGBAR,0),(LPARAM)&m_define);

				m_pListData->iRecherche = 2;

				StringCchCopy(cValue,1024,(const char *)lParam);

				if(m_pListData->m_CAlbumCatalogue->RechercheImage(&m_pListData->m_IconeFileVector, cValue,m_define.hWnd))
				{
					SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_ALBUMIMAGELIST,0),(LPARAM)-1);
					SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_INITALBUM,0),(LPARAM)m_pListData->m_IconeFileVector);
				}
			}
			break;

		case ID_SOUSALBUM_AJUSTERCRITREALBUM:
			{
				char Temp[MAX_PATH];
				int iNumAlbum;

				

				//Récupération du numero de l'album
				m_pListData->classTree->TreeViewGetSelectName(Temp,MAX_PATH);
				iNumAlbum = m_pListData->m_CAlbumCatalogue->DetermineNumAlbum(Temp);

				if(iNumAlbum != -1)
				{
					int m_iReturnValue;
					CAlbumData * m_Album;
					CDialogCatalogueData m_dialogData;

					//Récupération d'un pointeur sur la liste des attributs (VecteurAttribut)
					AttributVector * m_AttributVector;
					AttributVector m_attributPicture;
					m_AttributVector = m_pListData->m_CAlbumCatalogue->RecupPtAttribut();
					
					//CDialog::SetAttributVector(m_AttributVector);

					//Récupération d'un pointeur sur l'entrée de l'image (IconeFile)
					m_Album = m_pListData->m_CAlbumCatalogue->RecupPtAlbum(iNumAlbum);
					//CDialog::SetAlbum(m_Album);

					m_dialogData.m_AttributVector = m_AttributVector;
					m_dialogData.m_AttributVectorPicture = &m_attributPicture;
					m_dialogData.m_Album = m_Album;
					//DialogBoxParam(hInstance, (LPCTSTR)IDD_ATTRIBUTIMAGE, hWnd, (DLGPROC)CDialog::AttributRepertoire,(LPARAM)&m_dialogData);
					CAttributRepertoire * m_cAttributRepertoire = new CAttributRepertoire(hInstance);
					m_cAttributRepertoire->SetParam(&m_dialogData);
					m_cAttributRepertoire->CreateDlg(hInstance, (LPCTSTR)IDD_ATTRIBUTIMAGE, hWnd);
					m_iReturnValue = m_cAttributRepertoire->GetReturnValue();
					delete m_cAttributRepertoire;

					if(m_iReturnValue)
					{
#ifdef SQLSERVERCE
						//On met à jour la valeur des catégories et des attributs
						CSqlAlbumUtility::UpdateCategorie(m_AttributVector);
						size_t m_sizeTConvert;
						WCHAR m_wAlbumName[MAX_PATH];
						mbstowcs_s(&m_sizeTConvert,m_wAlbumName, MAX_PATH, m_Album->m_szAlbumName, MAX_PATH);						
						CSqlAlbumUtility::UpdateFileAttribut(&m_attributPicture,m_wAlbumName);

#endif
					}
					
					m_attributPicture.clear();

				}
			}
			break;

		case ID_SOUSALBUM_RENOMMER:	
			TreeView_EditLabel(m_pListData->classTree->m_hWnd,TreeView_GetSelection(m_pListData->classTree->m_hWnd));
			break;

		case ID_SOUSALBUM_SUPPRIMER:
			{
				m_pListData->DeleteAlbum(GetParent());
			}
			break;

		case ID_SOUSCATALOGUE_SUPPRIMERCATALOGUE:
			{
				char Temp[MAX_PATH];
				char clpText[255];
				char clpCaption[255];

				LoadString(hInstance,IDS_SUPPCATALOGUE,clpText,255);
				LoadString(hInstance,IDS_INFOS,clpCaption,255);

				//Récupération du numéro de l'album
				m_pListData->classTree->TreeViewGetSelectName(Temp,MAX_PATH);
				m_pListData->iNumAlbum = m_pListData->m_CImageCatalogue->DetermineNumAlbum(Temp);
				//Affichage d'une boite de dialogue demandant la confirmation de la suppression
				if(MessageBox(NULL,clpText,clpCaption,MB_YESNO | MB_ICONWARNING) == IDYES)
				{
					m_pListData->m_CImageCatalogue->SuppAlbum(m_pListData->classTree->m_hWnd, m_pListData->classTree, m_pListData->iNumAlbum);
					SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_INITDIRECTORY,0),(LPARAM)"Catalogue");
				}

			}
			break;

		case ID_SOUSCATALOGUE_METTREJOURCATALOGUE:
			{
				char Temp[MAX_PATH];
				char clpText[255];
				char clpCaption[255];

				LoadString(hInstance,IDS_UPDATECATALOGUE,clpText,255);
				LoadString(hInstance,IDS_INFOS,clpCaption,255);

				//Récupération du numéro de l'album
				m_pListData->classTree->TreeViewGetSelectName(Temp,MAX_PATH);
				m_pListData->iNumAlbum = m_pListData->m_CImageCatalogue->DetermineNumAlbum(Temp);
				//Affichage d'une boite de dialogue demandant la confirmation de la suppression
				if(MessageBox(NULL,clpText,clpCaption,MB_YESNO | MB_ICONWARNING) == IDYES)
				{
					m_pListData->m_CImageCatalogue->MiseAJour(m_pListData->classTree->m_hWnd, m_pListData->iNumAlbum);
					m_pListData->SendInitMessage(m_pListData->iNumAlbum,3, GetParent());
				}

			}
			break;

		case ID_CATALOGUE_COMPACTERLABASE:
			{
				char clpText[255];
				char clpCaption[255];

				LoadString(hInstance,IDS_COMPACTCATALOGUE,clpText,255);
				LoadString(hInstance,IDS_INFOS,clpCaption,255);

				//Affichage d'une boite de dialogue demandant la confirmation de la suppression
				if(MessageBox(NULL,clpText,clpCaption,MB_YESNO | MB_ICONWARNING) == IDYES)
				{
#ifdef SQLSERVERCE	

					CSqlLib * _sqlLib = CSqlEngine::getInstance();
					if(_sqlLib != NULL)
					{
						_sqlLib->CompactDatabase();
						delete _sqlLib;
						_sqlLib = NULL;
					}

#else
					m_pListData->m_CImageCatalogue->CompacterFichierCatalogue();
#endif
				}

			}
			break;

		case ID_LISTE_AJOUTSFICHIERSDANSCATALOGUE:
			{
				int iValue;
				AlbumDataVector * m_VectorAlbum;
				m_pListData->m_IconeFileVector = (IconeFileVector *)lParam;
				m_VectorAlbum = m_pListData->m_CAlbumCatalogue->RecupPtVectorAlbum();

				//CDialog::SetAlbumVector(m_VectorAlbum);

				CSelectAlbum * m_cSelectAlbum = new CSelectAlbum();
				m_cSelectAlbum->SetParam(m_VectorAlbum);
				m_cSelectAlbum->CreateDlg(hInstance, (LPCTSTR)IDD_SELECTALBUM, hWnd);
				iValue = m_cSelectAlbum->GetReturnValue();
				
				//iValue = DialogBoxParam(hInstance, (LPCTSTR)IDD_SELECTALBUM, hWnd, (DLGPROC)CDialog::SelectAlbum,(LPARAM)&m_VectorAlbum);
				if(iValue != -1)
				{
#ifdef SQLSERVERCE
					WCHAR m_wAlbumName[MAX_PATH];
					size_t m_sizeTConvert;
					mbstowcs_s(&m_sizeTConvert,m_wAlbumName, MAX_PATH, m_cSelectAlbum->GetAlbumName(), MAX_PATH);
					CSqlIconeFileAlbum * m_cSqlIconeFileAlbum = new CSqlIconeFileAlbum();
					m_cSqlIconeFileAlbum->SaveIconeFileAlbum(m_pListData->m_IconeFileVector,m_wAlbumName);
					delete m_cSqlIconeFileAlbum;

#else

					m_pListData->iNumAlbum = iValue;
					CIconeFile m_IconeFileTemp;
					for (IconeFileVector::const_iterator dit = m_pListData->m_IconeFileVector->begin(); dit!= m_pListData->m_IconeFileVector->end(); dit++)
					{
						m_IconeFileTemp = *dit;
						m_pListData->m_CAlbumCatalogue->AddImageToAlbum(m_pListData->iNumAlbum, m_IconeFileTemp);
					}
					m_pListData->m_CAlbumCatalogue->TrieVecteur(m_pListData->iNumAlbum);
#endif
				}

				delete m_cSelectAlbum;

			}
			break;

		case ID_LISTE_AJOUTSFICHIERSDIRECTDANSCATALOGUE:
			{
#ifdef SQLSERVERCE

				WCHAR m_wAlbumName[MAX_PATH];
				size_t m_sizeTConvert;				
				m_pListData->m_IconeFileVector = (IconeFileVector *)lParam;
				CAlbumData * m_cSelectAlbum = m_pListData->m_CAlbumCatalogue->RecupPtAlbum(m_pListData->iNumAlbum);			
				mbstowcs_s(&m_sizeTConvert,m_wAlbumName, MAX_PATH, m_cSelectAlbum->m_szAlbumName, MAX_PATH);
				CSqlIconeFileAlbum * m_cSqlIconeFileAlbum = new CSqlIconeFileAlbum();
				m_cSqlIconeFileAlbum->SaveIconeFileAlbum(m_pListData->m_IconeFileVector,m_wAlbumName);
				delete m_cSqlIconeFileAlbum;

#else
				AlbumDataVector * m_VectorAlbum;
				m_pListData->m_IconeFileVector = (IconeFileVector *)lParam;
				m_VectorAlbum = m_pListData->m_CAlbumCatalogue->RecupPtVectorAlbum();
				CIconeFile m_IconeFileTemp;
				for (IconeFileVector::const_iterator dit = m_pListData->m_IconeFileVector->begin(); dit!= m_pListData->m_IconeFileVector->end(); dit++)
				{
					m_IconeFileTemp = *dit;
					m_pListData->m_CAlbumCatalogue->AddImageToAlbum(m_pListData->iNumAlbum, m_IconeFileTemp);
				}
				m_pListData->m_CAlbumCatalogue->TrieVecteur(m_pListData->iNumAlbum);
#endif
			}
			break;

		case ID_CATALOGUE_GESTIONDESCATGORIES:
			{
				m_pListData->GestionDesCategories(hWnd);
			}
			break;

		case ID_LISTE_GESTIONSDESATTRIBUTS:
			{
				char _szFileName[MAX_PATH];
				StringCchCopy(_szFileName,MAX_PATH,(const char *)lParam);
				m_pListData->GestionDesAttributs(hWnd, _szFileName, GetParent());	
			}
			break;

		case ID_ADDITEM:
			{
				AttributVector * m_AttributVector;
				//Attente que la page soit chargé
				m_AttributVector = m_pListData->m_CAlbumCatalogue->RecupPtAttribut();
				for(AttributVector::iterator dit = m_AttributVector->begin();dit != m_AttributVector->end();dit++)
				{
					m_pListData->m_CWebBrowser->AddItem(dit->cLibelle,dit->iNumAttribut);
				}
				break;
			}

		case ID_AFFICHAGE_CONTROLE_RECHERCHE:
			{
				if(m_pListData->m_CWebBrowser == NULL)
				{
					m_pListData->m_CWebBrowser = new CWebBrowser();

					DWORD dwStyle = CS_HREDRAW | CS_VREDRAW;
					DWORD dwWndStyle = WS_CHILD;
					DWORD dwExStyle = 0;
					RECT rc = {CW_USEDEFAULT, 0, CW_USEDEFAULT, 0};
					m_pListData->m_CWebBrowser->CreateEx(hWnd,hInstance,dwWndStyle,dwStyle,dwExStyle,"WEBBROWSER",0,0,IDC_ARROW,rc);
				}

				if(wmEvent)
				{
					m_pListData->m_CWebBrowser->LocalDisplayHTMLRessource();				
					m_pListData->m_CWebBrowser->ShowWindow(SW_SHOW);
					//m_pListData->m_CWebBrowser->RemoveAllItem();
				}
				else
					m_pListData->m_CWebBrowser->ShowWindow(SW_HIDE);

				RECT rc;
				GetClientRect(hWnd,&rc);
				OnSize(hWnd, WM_SIZE, 0, MAKELPARAM(rc.right,rc.bottom));

			}
			break;


		case IDM_STOPWORKING:
			{
				m_pListData->g_fDragging = FALSE;
				m_pListData->iListDragging = 0; 
				ImageList_DragLeave(m_pListData->classTree->m_hWnd);
				ImageList_EndDrag();
				ImageList_Destroy(m_pListData->himl);
				KillTimer(hWnd,TIMER_SCROLL);
				KillTimer(hWnd,TIMER_ITEM);
				ReleaseCapture();
				TreeView_SelectDropTarget(m_pListData->classTree->m_hWnd, (HTREEITEM)NULL);

				//turn redawing back on in the TreeView
				SendMessage(m_pListData->classTree->m_hWnd, WM_SETREDRAW, TRUE, 0);
			}
			break;

		case ID_ALBUM_CREERUNNOUVELALBUM:
			{
				//DialogBoxParam(hInstance, (LPCTSTR)IDD_NEWALBUM, hWnd, (DLGPROC)CDialog::NewAlbum,(LPARAM)hWnd);
				CNewAlbum * m_cNewAlbum = new CNewAlbum();
				m_cNewAlbum->SetWindow(hWnd);
				m_cNewAlbum->CreateDlg(hInstance, (LPCTSTR)IDD_NEWALBUM, hWnd);
				delete m_cNewAlbum;
			}
			break;

		case ID_CATALOGUE_CRERUNCATALOGUE:
			{
				m_pListData->m_CImageCatalogue->CreerCatalogue(hWnd,m_pListData->classTree->m_hWnd);
			}
			break;

		case IDM_ALBUMNAME:
			{
				char cTemp[255];
				StringCchCopy(cTemp,255,(const char *)lParam);
				m_pListData->CreateNewAlbum(cTemp);
			}
			break;

		case IDM_DRAGGING:
			{
				m_pListData->iListDragging = wmEvent;
				m_pListData->m_IconeFileVector = (IconeFileVector *)lParam;
				SetTimer(hWnd,TIMER_SCROLL,1000,NULL);
				
			}
			break;

		case ID_AIDEINFOS:
			SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(ID_AIDE_AIDE,0),0);
			break;

		case IDM_FILENAME:
			StringCchCopy(m_pListData->cFile,MAX_PATH,(const char *)lParam);
			break;
		
		case IDM_DIRNAME:
			StringCchCopy(m_pListData->cDirectory,MAX_PATH,(const char *)lParam);
			StringCchCopy(m_pListData->cDirectoryTemp,MAX_PATH,(const char *)lParam);
			break;

		case ID_AFFICHAGE_CONTROL_FOLDERTREE:
			{
				if(wmEvent)
					::ShowWindow(m_pListData->classTree->m_hWnd, SW_SHOW);
				else
					::ShowWindow(m_pListData->classTree->m_hWnd, SW_HIDE);

				//DestroyWindow(m_CWebBrowser->

				//m_pListData->cSplitter->SizeWindowContents(m_pListData->m_CWebBrowser->m_hWnd,m_pListData->classTree->m_hWnd);
				RECT rc;
				GetClientRect(hWnd,&rc);
				OnSize(hWnd, WM_SIZE, 0, MAKELPARAM(rc.right,rc.bottom));

			}
			break;

		case IDM_OUVRIR_ARBRE:
			{
				m_pListData->m_bDblClick = true;
				string m_szRep = m_pListData->cDirectory;
				//recherche du nom du dernier répertoire
				HTREEITEM hTreeItem = TreeView_GetSelection(m_pListData->classTree->m_hWnd);
				TreeView_Expand(m_pListData->classTree->m_hWnd,hTreeItem,TVE_EXPAND);
				hTreeItem = m_pListData->classTree->ExpandedItem(hTreeItem, m_szRep.c_str());
				
				if(!TreeView_Expand(m_pListData->classTree->m_hWnd,hTreeItem,TVE_EXPAND))
				{	
					//m_pListData->classTree->AddItem(hTreeItem,m_pListData->cDirectory);
					TreeView_Expand(m_pListData->classTree->m_hWnd,hTreeItem,TVE_EXPAND);
					TreeView_SelectItem(m_pListData->classTree->m_hWnd,hTreeItem);
				} 
				else
					TreeView_SelectItem(m_pListData->classTree->m_hWnd,hTreeItem);

				m_pListData->m_bDblClick = false;
			}
			break;


		case IDM_EDITION_REPERTOIRESUP:
			{
				char Temp[MAX_PATH];
				HTREEITEM hItemSelect;
				hItemSelect = TreeView_GetSelection(m_pListData->classTree->m_hWnd);
				if(hItemSelect != NULL)
				{
					m_pListData->classTree->TreeViewGetSelectName(Temp,MAX_PATH);
					if(_stricmp(Temp,m_pListData->cDirectory) == 0)
					{
						hItemParent = TreeView_GetParent(m_pListData->classTree->m_hWnd, hItemSelect);
						if(hItemParent != NULL)
						{
							TreeView_SelectItem(m_pListData->classTree->m_hWnd, hItemParent);
							m_pListData->classTree->TreeViewGetSelectName(Temp,MAX_PATH);
						}
					}

					//Recherche dans le nom du répertoire le nom du sous-répertoire
					SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_INITDIRECTORY,0),(LPARAM)Temp);
					StringCchCopy(m_pListData->cDirectory,MAX_PATH,Temp);
				}
			}
			break;


			
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CListWindow::OnTimer(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
		case TIMER_SAVEDATA:
			m_pListData->m_CAlbumCatalogue->SaveAlbum();
			break;

		case TIMER_SCROLL:
			{
				int iPos = GetScrollPos(m_pListData->classTree->m_hWnd,SB_VERT);
				int iMin,iMax;
				GetScrollRange(m_pListData->classTree->m_hWnd,SB_VERT,&iMin,&iMax);
				if(iMax == 0)
					return 0;

				int iReturn=0;
				RECT rc;
				GetWindowRect(m_pListData->classTree->m_hWnd,&rc);

				if(m_pListData->ptOld.y > rc.top - 160 && m_pListData->ptOld.y < rc.top && iPos != 0)
				{
					iReturn = SendMessage(m_pListData->classTree->m_hWnd,WM_VSCROLL,MAKEWPARAM(SB_LINEUP,m_pListData->ptOld.y),NULL);
					
					int iNewPos = GetScrollPos(m_pListData->classTree->m_hWnd,SB_VERT);
					if(iNewPos != iPos)
					{
						SendMessage(m_pListData->classTree->m_hWnd, WM_SETREDRAW, TRUE, 0);
						KillTimer(hWnd,TIMER_ITEM);
					}
				}
					
				if(m_pListData->ptOld.y > rc.bottom - 160 && m_pListData->ptOld.y < rc.bottom)
				{
					iReturn = SendMessage(m_pListData->classTree->m_hWnd,WM_VSCROLL,MAKEWPARAM(SB_LINEDOWN,m_pListData->ptOld.y),NULL);
					int iNewPos = GetScrollPos(m_pListData->classTree->m_hWnd,SB_VERT);
					if(iNewPos != iPos)
					{
						SendMessage(m_pListData->classTree->m_hWnd, WM_SETREDRAW, TRUE, 0);
						KillTimer(hWnd,TIMER_ITEM);
					}
				}
			}
			break;

		case TIMER_ITEM:
			{
				HTREEITEM hItemParent = TreeView_GetDropHilight(m_pListData->classTree->m_hWnd);
				TreeView_SelectItem(m_pListData->classTree->m_hWnd, hItemParent);
				TreeView_Expand(m_pListData->classTree->m_hWnd,hItemParent,TVE_EXPAND);
				SendMessage(m_pListData->classTree->m_hWnd, WM_SETREDRAW, TRUE, 0);
				KillTimer(hWnd,TIMER_ITEM);
			}
			break;
	}

	return 0;
}



LRESULT CListWindow::OnDrawItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_pListData->g_pcm2)
	{
		m_pListData->g_pcm2->HandleMenuMsg(message, wParam, lParam);
	}
	return 0;
}
LRESULT CListWindow::OnMenuChar(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return OnDrawItem(hWnd,message,wParam,lParam);
}

LRESULT CListWindow::OnMeasureItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return OnDrawItem(hWnd,message,wParam,lParam);
}


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CListWindow::OnContextMenu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CObjet m_cObjet;
	POINT ptScreen;
	char Temp[MAX_PATH];

	TVHITTESTINFO  tvhti;

	ptScreen.x = GET_X_LPARAM(lParam);
	ptScreen.y = GET_Y_LPARAM(lParam);

	tvhti.pt = ptScreen;

	ScreenToClient(m_pListData->classTree->m_hWnd, &tvhti.pt);

	TreeView_HitTest(m_pListData->classTree->m_hWnd, &tvhti);

	m_pListData->classTree->TreeViewGetNodeName(tvhti.hItem,Temp,MAX_PATH);

	TV_ITEM tItem;
	tItem.hItem = tvhti.hItem;
	tItem.mask = TVIF_HANDLE | TVIF_PARAM;

	if(TreeView_GetItem(m_pListData->classTree->m_hWnd, &tItem))
	{

		CItemInfo * pItem = (CItemInfo *)tItem.lParam;

		//Test si le répertoire sélectionnée est le répertoire de gravure
		string m_stgRepBurning = m_cParameter->GetBurningDirectory();
		string m_stgRepEncours = Temp;

		if(_stricmp(m_stgRepBurning.c_str(),m_stgRepEncours.c_str()) == 0)
		{
			m_cObjet.DisplayContextMenu(hWnd,tvhti.pt, hInstance, (LPCSTR)IDM_ALBUM,4);
			return 0;
		}


		switch(pItem->GetTypeDonne())
		{
			case 1:
				{
					if(TVHT_ONITEM & tvhti.flags)
					{
					   int iCommand = m_pListData->classTree->Tree_DoItemMenu(tvhti.hItem, &ptScreen, m_pListData->g_pcm2);
					   if(iCommand == 18) //Suppression
						   TreeView_DeleteItem(m_pListData->classTree->m_hWnd,tvhti.hItem);

					   if(iCommand == 0)
					   {
						   char path[MAX_PATH];
						   if(m_pListData->classTree->SpecialFolderToPath(Temp,path,MAX_PATH))
							m_pListData->classTree->Tree_DisplayContextMenu(tvhti.hItem,path,&ptScreen, m_pListData->g_pcm2);
						   else
							m_pListData->classTree->Tree_DisplayContextMenu(tvhti.hItem,Temp,&ptScreen, m_pListData->g_pcm2);
					   }
					}
				}
				break;

			case 2:
				{
					if(strcmp(Temp,"Album") == 0)
						m_cObjet.DisplayContextMenu(hWnd,tvhti.pt, hInstance, (LPCSTR)IDM_ALBUM,0);
					else
						m_cObjet.DisplayContextMenu(hWnd,tvhti.pt, hInstance, (LPCSTR)IDM_ALBUM,1);

				}
				break;

			case 3:
				{
					if(strcmp(Temp,"Catalogue") == 0)
						m_cObjet.DisplayContextMenu(hWnd,tvhti.pt, hInstance, (LPCSTR)IDM_ALBUM,2);
					else
						m_cObjet.DisplayContextMenu(hWnd,tvhti.pt, hInstance, (LPCSTR)IDM_ALBUM,3);
				}
				break;

		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CListWindow::OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	m_pListData->m_CAlbumCatalogue = new CAlbum();
	m_pListData->classTree = new CTree();
	m_pListData->m_CWebBrowser = new CWebBrowser();
	m_pListData->m_CImageCatalogue = new CImageCatalogue();

	m_pListData->ptOld.x = 0;
	m_pListData->ptOld.y = 0;

	m_pListData->m_Album = NULL;


	DWORD dwStyle = CS_HREDRAW | CS_VREDRAW;
	DWORD dwWndStyle = WS_CHILD;
	DWORD dwExStyle = 0;
	RECT rc = {CW_USEDEFAULT, 0, CW_USEDEFAULT, 0};
	m_pListData->m_CWebBrowser->CreateEx(hWnd,hInstance,dwWndStyle,dwStyle,dwExStyle,"WEBBROWSER",0,0,IDC_ARROW,rc);

	m_pListData->classTree->CreateATreeView(hWnd,hInstance);

	m_pListData->classTree->InitTree("",false);

	m_pListData->m_CAlbumCatalogue->InitCatalogue(m_pListData->classTree->m_hWnd, m_pListData->classTree);

	m_pListData->m_CImageCatalogue->InitCatalogue(m_pListData->classTree->m_hWnd, m_pListData->classTree);

	//m_pListData->m_CWebBrowser->LocalDisplayHTMLRessource();

	SHGetMalloc(&m_pListData->g_pMalloc);

	SetTimer(hWnd,TIMER_SAVEDATA,60000,0);

	RegisterDriveNotification(hWnd);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CListWindow::OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	
	if(m_pListData->iListDragging)
	{
		KillTimer(hWnd,TIMER_SCROLL);

		int xPos = LOWORD(lParam);
		int yPos = HIWORD(lParam);
		int iReturn = 0;
		
		TVHITTESTINFO m_HITTESTINFO;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam) - 60;

		HTREEITEM hitem;
		UINT flags = 0;

		m_HITTESTINFO.flags = flags;
		m_HITTESTINFO.pt = pt;

		hitem = TreeView_HitTest(m_pListData->classTree->m_hWnd, &m_HITTESTINFO);

		SetTimer(hWnd,TIMER_SCROLL,200,NULL);

		if (hitem != NULL && m_pListData->m_ItemOld != hitem)
		{
			TreeView_Select(m_pListData->classTree->m_hWnd, hitem, TVGN_DROPHILITE );
			//Expansion suivant un Timer
			m_pListData->m_ItemOld = hitem;
			SetTimer(hWnd,TIMER_ITEM,1000,NULL);
		}


		

		pt.y += 60;
		m_pListData->ptOld = pt;

	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CListWindow::OnSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int iRight = LOWORD(lParam);
	int iBottom = HIWORD(lParam);

	if(m_pListData->m_CWebBrowser->IsWindowVisible())
		MoveWindow(m_pListData->m_CWebBrowser->m_hWnd,0,0,iRight,iBottom,TRUE);

	if(::IsWindowVisible(m_pListData->classTree->m_hWnd))
		MoveWindow(m_pListData->classTree->m_hWnd,0,0,iRight,iBottom,TRUE);

	return 0;
}



/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CListWindow::OnLButtonUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_pListData->g_fDragging || m_pListData->iListDragging)
	{
		KillTimer(hWnd,TIMER_SCROLL);
		KillTimer(hWnd,TIMER_ITEM);
		int iNumAlbum;
		char Temp[MAX_PATH];
		HTREEITEM hitem;
		CObjet m_cObjet;

		hitem = TreeView_GetDropHilight(m_pListData->classTree->m_hWnd);

		TV_ITEM tItem;
		tItem.hItem = hitem;
		tItem.mask = TVIF_HANDLE | TVIF_PARAM;
		TreeView_GetItem(m_pListData->classTree->m_hWnd, &tItem);

		CItemInfo * pItem = (CItemInfo *)tItem.lParam;

		if (hitem != NULL)
		{
			m_pListData->classTree->TreeViewGetNodeName(hitem,Temp,MAX_PATH);
			if(_stricmp(m_pListData->cDirectory,Temp) != 0)
			{
				StringCchCopy(m_pListData->cDirectory,MAX_PATH,Temp);
			}

			int iNumCatalogue = m_pListData->m_CImageCatalogue->DetermineNumAlbum(Temp);
			if(iNumCatalogue == -1)
			{
				if(_stricmp("Catalogue",Temp) == 0)
					iNumCatalogue = 0;
			}

			iNumAlbum = m_pListData->m_CAlbumCatalogue->DetermineNumAlbum(Temp);
			if(iNumAlbum == -1)
			{
				if(_stricmp("Album",Temp) == 0)
					iNumCatalogue = 0;
			}

			if(iNumAlbum >= 0)
			{
				CIconeFile m_IconeFileTemp;
				m_pListData->g_fDragging = FALSE;
				m_pListData->iListDragging = 0; 
				
#ifdef SQLSERVERCE

					CAlbumData * m_cSelectAlbum = m_pListData->m_CAlbumCatalogue->RecupPtAlbum(iNumAlbum);
					IconeFileVector m_IconeFileVector;
					WCHAR m_wAlbumName[MAX_PATH];
					size_t m_sizeTConvert;
					mbstowcs_s(&m_sizeTConvert,m_wAlbumName, MAX_PATH, m_cSelectAlbum->m_szAlbumName, MAX_PATH);

					for (IconeFileVector::const_iterator dit = m_pListData->m_IconeFileVector->begin(); dit!=m_pListData->m_IconeFileVector->end(); dit++)
					{
						m_IconeFileTemp = *dit;
						//Test si le fichier est une image ou non
						if(m_cObjet.TestImageFormat(m_IconeFileTemp.m_stgFullPathName) > 0 || m_cObjet.TestVideoFormat(m_IconeFileTemp.m_stgFullPathName) > 0)
							m_IconeFileVector.push_back(m_IconeFileTemp);
					}

					CSqlIconeFileAlbum * m_cSqlIconeFileAlbum = new CSqlIconeFileAlbum();
					m_cSqlIconeFileAlbum->SaveIconeFileAlbum(&m_IconeFileVector,m_wAlbumName);
					delete m_cSqlIconeFileAlbum;

#else

				for (IconeFileVector::const_iterator dit = m_pListData->m_IconeFileVector->begin(); dit!=m_pListData->m_IconeFileVector->end(); dit++)
				{
					m_IconeFileTemp = *dit;
					//Test si le fichier est une image ou non
					if(m_cObjet.TestImageFormat(m_IconeFileTemp.m_stgFullPathName) > 0 || m_cObjet.TestVideoFormat(m_IconeFileTemp.m_stgFullPathName) > 0)
						m_pListData->m_CAlbumCatalogue->AddImageToAlbum(iNumAlbum, m_IconeFileTemp);
				}
				m_pListData->m_CAlbumCatalogue->TrieVecteur(iNumAlbum);
#endif
			}
			else if(iNumCatalogue >= 0)
			{
				//Cas d'un catalogue -- Ne pas déplacer ni copier
				//Ne rien faire
			}
            else
			{
				int iDepl = 1;
				int nVirtKey = GetKeyState(VK_CONTROL);

				if(nVirtKey)
					iDepl = 0;

				CDeplaceFile * m_deplaceFile = new CDeplaceFile();
				m_deplaceFile->cDir = Temp;
				m_deplaceFile->iDepl = iDepl;
				m_deplaceFile->m_IconeFile = m_pListData->m_IconeFileVector;

				CDeplaceFichier * m_cDeplaceFichier = new CDeplaceFichier();
				m_cDeplaceFichier->SetParam(m_deplaceFile);
				m_cDeplaceFichier->CreateDlg(hInstance, (LPCTSTR)IDD_FICHIER, hWnd);

				delete m_cDeplaceFichier;
				delete m_deplaceFile;

				m_pListData->g_fDragging = 0;
				m_pListData->iListDragging = 0;
			}
		}
		else
			iNumAlbum = -2;

		ImageList_DragLeave(m_pListData->classTree->m_hWnd);
		ImageList_EndDrag();
		ImageList_Destroy(m_pListData->himl);
		ReleaseCapture();

		TreeView_SelectDropTarget(m_pListData->classTree->m_hWnd, (HTREEITEM)NULL);

		TreeView_SelectItem(m_pListData->classTree->m_hWnd,hitem);

		if(iNumAlbum != -2)
			m_pListData->SendInitMessage(iNumAlbum,pItem->GetTypeDonne(), GetParent());

		m_pListData->g_fDragging = 0;
		m_pListData->iListDragging = 0;


	}
	else
	{
		TreeView_SelectDropTarget(m_pListData->classTree->m_hWnd, (HTREEITEM)NULL);
	}
	return 0;
}

