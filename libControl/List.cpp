// List.cpp: implementation of the CList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "List.h"
#include "loadimage.h"
#include <SADirRead.h>
#include "objet.h"
#include "pidl.h"
#include <ItemInfo.h>

#define C_COLUMNS 5



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CList::CList()
{
	hParent = NULL;
}

CList::~CList()
{

}




// CreateListView - creates a list-view control in report view. 
// Returns the handle to the new control if successful, or NULL 
// otherwise. 
// hWndParent - handle to the control's parent window. 
HWND CList::CreateListView (HWND hwndParent, HINSTANCE hInst, const char * szName, const int iShareImage) 
{ 
	RECT rectClient;
	//HWND hLV;
	INITCOMMONCONTROLSEX icex;

	// Ensure that the common control DLL is loaded. 
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC  = ICC_LISTVIEW_CLASSES | ICC_COOL_CLASSES;
	InitCommonControlsEx(&icex); 
	DWORD       dwStyle, dwExStyle;
	//

	dwStyle =   WS_TABSTOP | 
				WS_CHILD | 
				WS_BORDER | 
				WS_VISIBLE |
				LVS_AUTOARRANGE |
				LVS_LIST | 
				LVS_SHAREIMAGELISTS |
				LVS_SHOWSELALWAYS |
				LVS_EDITLABELS |
				0;

	//WS_CHILD|WS_VISIBLE|WS_BORDER | LVS_EDITLABELS | LVS_SHAREIMAGELISTS

	GetClientRect(hwndParent,&rectClient);


    // Create and fill list view
	m_hWnd = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_ACCEPTFILES,WC_LISTVIEW,szName, dwStyle,0,0,0, 0,hwndParent,NULL,hInstance,NULL);
	
	hParent = hwndParent;

	dwExStyle = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;

	//dwExStyle = LVS_EX_BORDERSELECT;

	ListView_SetExtendedListViewStyle(m_hWnd, dwExStyle);

	SendMessage(m_hWnd,WM_SETREDRAW,true,0);

	//SetWindowTheme(m_hWnd, L"explorer", NULL);

	//Initialisation des colonnes

    char szText[256];     // temporary buffer 
    LVCOLUMN lvc; 
    int iCol; 
 
    // Initialize the LVCOLUMN structure.
    // The mask specifies that the format, width, text, and subitem
    // members of the structure are valid. 
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
     
    // Add the columns. 
    for (iCol = 0; iCol < C_COLUMNS; iCol++) 
	{ 
        lvc.iSubItem = iCol;
        lvc.pszText = szText;	
        lvc.cx = 100;           // width of column in pixels
		lvc.fmt = LVCFMT_LEFT;  // left-aligned column
		/*
        if ( iCol < 2 )
          {
          lvc.fmt = LVCFMT_LEFT;  // left-aligned column
          }
        else
          { 
          lvc.fmt = LVCFMT_RIGHT;  // right-aligned column
          }  */
        LoadString(hInstance, IDS_FIRSTCOLUMN + iCol, 
                szText, sizeof(szText));
        if (ListView_InsertColumn(m_hWnd, iCol, &lvc) == -1) 
            return FALSE; 
    } 

	return m_hWnd;
}

// SetView - sets a list-view's window style to change the view. 
// hWndListView - handle to the list-view control. 
// dwView - value specifying the new view style.
  
VOID CList::SetView(DWORD dwView) 
{ 
	// Retrieve the current window style. 
	DWORD dwStyle = GetWindowLong(m_hWnd, GWL_STYLE); 

	// Only set the window style if the view bits have changed. 
	if ((dwStyle & LVS_TYPEMASK) != dwView) 
		SetWindowLong(m_hWnd, GWL_STYLE,
		   (dwStyle & ~LVS_TYPEMASK) | dwView); 
} 


// InitListViewImageLists - creates image lists for a list-view control.
// This function only creates image lists. It does not insert the
// items into the control, which is necessary for the control to be 
// visible.   
// Returns TRUE if successful, or FALSE otherwise. 
// hWndListView - handle to the list-view control. 
BOOL CList::InitListViewImageLists() 
{ 
	SHFILEINFO shfi;
	HIMAGELIST hmIconeLarge;
	HIMAGELIST hmIconePetite;

	InitCommonControls(); 

	ZeroMemory( &shfi, sizeof(SHFILEINFO) );

	hmIconeLarge = (HIMAGELIST)SHGetFileInfo( "C:\\",
	0,
	&shfi,
	sizeof( SHFILEINFO ),
	SHGFI_SYSICONINDEX | SHGFI_SMALLICON );

	SendMessage(m_hWnd, LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM)hmIconeLarge );

	ZeroMemory( &shfi, sizeof(SHFILEINFO) );

	hmIconePetite = (HIMAGELIST)SHGetFileInfo( "C:\\",
	0,
	&shfi,
	sizeof( SHFILEINFO ),
	SHGFI_SYSICONINDEX | SHGFI_LARGEICON );

	SendMessage(m_hWnd, LVM_SETIMAGELIST, LVSIL_NORMAL, (LPARAM)hmIconePetite );

    return TRUE; 
} 

/**************************************************************************

   List_Notify()

**************************************************************************/

void CList::DeleteItem(char *FileName)
{
	LVFINDINFO lvInfos;
	int iItem;
	POINT pt;

	pt.x = 0;
	pt.y = 0;

	lvInfos.flags = LVFI_STRING;
	lvInfos.psz = FileName;
	lvInfos.lParam = NULL;
	lvInfos.pt = pt;
	lvInfos.vkDirection = VK_DOWN;

	iItem = ListView_FindItem(m_hWnd,0,&lvInfos);

    ListView_DeleteItem(m_hWnd,iItem);
}

 
/**************************************************************************

   List_Notify()

**************************************************************************/

int CList::AddItem(const char *FileName)
{
	IShellFolder *psfDeskTop = NULL;
	LPITEMIDLIST pidlDocFiles = NULL;
	ULONG chEaten;
	HRESULT hr;
	OLECHAR       olePath[MAX_PATH];

	LVITEM   lvItem;
	DWORD    dwAttr;
	int dIndex;
	CItemInfo *   pItem;

	ZeroMemory(&lvItem, sizeof(lvItem));

	//fill in the TV_ITEM structure for this item
	lvItem.mask = LVIF_PARAM | LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;

	//put the private information in the lParam
	pItem = new CItemInfo();

	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, FileName, -1,
					   olePath, MAX_PATH);

	//pzSourceFiles[0] = '\0';
	hr = SHGetDesktopFolder(&psfDeskTop);

	hr = psfDeskTop->ParseDisplayName(NULL, NULL, olePath, 
		 &chEaten, &pidlDocFiles, NULL);

	pItem->SetPidlFQ(pidlDocFiles);
	pItem->SetPidlRel(pidlDocFiles);
	pItem->SetParseName(FileName);
	pItem->SetShellFolder(psfDeskTop);

	lvItem.lParam = (LPARAM)pItem;

	//text and images are done on a callback basis
	lvItem.pszText = LPSTR_TEXTCALLBACK;
	lvItem.iImage = I_IMAGECALLBACK;

	//determine if the item is shared
	dwAttr = SFGAO_DISPLAYATTRMASK;
	
	psfDeskTop->GetAttributesOf(1, (LPCITEMIDLIST*)&pidlDocFiles, &dwAttr);

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

	dIndex = ListView_InsertItem(m_hWnd, &lvItem);

	//Ajout de l'information sur la Taille

	lvItem.iItem = dIndex;
	lvItem.iSubItem = 1;

	ListView_SetItem(m_hWnd,&lvItem);

	lvItem.iItem = dIndex;
	lvItem.iSubItem = 2;

	ListView_SetItem(m_hWnd,&lvItem);

	return dIndex;

}

/**************************************************************************

   List_DoBackgroundMenu

**************************************************************************/

void CList::List_DoBackgroundMenu(LPPOINT pptScreen, IContextMenu2  *g_pcm2)
{
HWND           hwndParent = GetParent();
HRESULT        hr;

if(g_psfCurFolder)
   {
   IContextMenu   *pcm;

   hr = g_psfCurFolder->CreateViewObject( hwndParent, 
                                          IID_IContextMenu, 
                                          (LPVOID*)&pcm);

   if(SUCCEEDED(hr))
      {
      HMENU hMenu;

      hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDM_MENULISTE));
      if(hMenu)
         {
         HMENU hPopup;
         hPopup = GetSubMenu(hMenu, 0);

         if(hPopup)
            {
            int   i;
            UINT  idCmdFirst;
            UINT  idCmd;
         
            //find the largest ID in the menu
            i = 0;
            idCmdFirst = 0;

            while((idCmd = GetMenuItemID(hPopup, i)) != (UINT)-1)
               {
               if(idCmd > idCmdFirst)
                  idCmdFirst = idCmd;
            
               i++;
               }

            hr = pcm->QueryContextMenu(hPopup, 0, ++idCmdFirst, (UINT)-1, CMF_NORMAL | CMF_EXPLORE);

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
                  if(idCmd > idCmdFirst)
                     {
                     //this is one of our commands
                     FORWARD_WM_COMMAND(m_hWnd, idCmd, NULL, 0, SendMessage);
                     }
                  else
                     {
						 CMINVOKECOMMANDINFO  cmi;
						 cmi.cbSize = sizeof(CMINVOKECOMMANDINFO);
						 cmi.fMask = 0;
						 cmi.hwnd = hwndParent;
						 cmi.lpVerb = (LPCSTR)(INT_PTR)(idCmd - idCmdFirst);
						 cmi.lpParameters = NULL;
						 cmi.lpDirectory = NULL;
						 cmi.nShow = SW_SHOWNORMAL;
						 cmi.dwHotKey = 0;
						 cmi.hIcon = NULL;
						 hr = pcm->InvokeCommand(&cmi);

                     /*
                     The operation performed by the context menu may have 
                     changed the contents of the folder, so do a refresh.
                     */
                     //List_Refresh(hwndListView);
                     }
                  }
               }
            }
         DestroyMenu(hMenu);
         }

      pcm->Release();
      }
   }
}





/**************************************************************************

   List_Notify()

**************************************************************************/

void CList::DefineStatusBar(CStatusBar *m_StatusBar)
{
	m_ListStatusBar = m_StatusBar;
}	

/**************************************************************************

   List_Notify()

**************************************************************************/

void CList::DefineAfficheImage(const bool &m_bVisible)
{
	m_bAfficheImage = m_bVisible;
}


/**************************************************************************

   List_Notify()

**************************************************************************/

LRESULT CList::List_Notify(LPARAM lParam, const int &iTypeData)
{
LPNMHDR  pnmh = (LPNMHDR) lParam;


switch(pnmh->code)
   {
   case LVN_GETDISPINFO:
      {
      NMLVDISPINFO   *pdi = (NMLVDISPINFO*)lParam;
      CItemInfo *     pItem = (CItemInfo *)pdi->item.lParam;

      //do we need to supply the text?
      if(pdi->item.mask & LVIF_TEXT)
         {
         switch(pdi->item.iSubItem)
            {
            case 0:  //name
               {
               

				   if(pItem->GetPidlFQ() != NULL)
				   {
					   SHFILEINFO     sfi;
					   //get the display name of the item
					   if(SHGetFileInfo((LPCTSTR)pItem->GetPidlFQ(), 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME))
						  {
							lstrcpyn(pdi->item.pszText, sfi.szDisplayName, pdi->item.cchTextMax);
						  }
					   }
				}
               break;

            case 1:  //size
				{
					if(pItem->GetPidlFQ() != NULL)
					{
						char szLabel[MAX_PATH];
						struct _finddata_t  c_file;
						CObjet m_cObjet;

						if(iTypeData == 0)
						{
							SHFILEINFO     sfi;

							//get the display name of the item
							if(SHGetFileInfo((LPCTSTR)pItem->GetPidlFQ(), 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME))
							{
								//StringCchPrintf(szLabel,"%s%s",cDirectory,sfi.szDisplayName);

								//strcpy(szLabel,pItem->szParseName);
								StringCchCopy(szLabel,MAX_PATH,pItem->GetParseName());

								long fHandle = _findfirst(szLabel, &c_file );
								if(fHandle == -1)
								{
									break;
								}
								m_cObjet.GetInfosTaille(c_file.size,szLabel,255);
								lstrcpyn(pdi->item.pszText, szLabel, pdi->item.cchTextMax);
								_findclose(fHandle);
							}
						}
						else
						{
								long fHandle = _findfirst(pItem->GetParseName(), &c_file );
								if(fHandle == -1)
								{
									break;
								}
								m_cObjet.GetInfosTaille(c_file.size,szLabel,255);
								lstrcpyn(pdi->item.pszText, szLabel, pdi->item.cchTextMax);
								_findclose(fHandle);
						}
					}
					else
					{
						lstrcpyn(pdi->item.pszText, pItem->GetSize(), pdi->item.cchTextMax);
					}
				}
               break;

			case 2: //Date
				{

					if(pItem->GetPidlFQ() != NULL)
					{
						char ValueTime[255];
						char Temp[45];
						struct tm newtime;
						struct _finddata_t  c_file;
						errno_t err;

						if(iTypeData == 0)
						{
							SHFILEINFO     sfi;
							//get the display name of the item
							if(SHGetFileInfo((LPCTSTR)pItem->GetPidlFQ(), 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME))
							{
								//StringCchPrintf(ValueTime,"%s%s",cDirectory,sfi.szDisplayName);

								//strcpy(ValueTime,pItem->szParseName);
								StringCchCopy(ValueTime,255,pItem->GetParseName());

								long fHandle = _findfirst(ValueTime, &c_file );
								if(fHandle == -1)
								{
									break;
								}
								//newtime = localtime(&c_file.time_create); 

								err = _localtime64_s(&newtime, &c_file.time_create);
								if(err == 0)
								{
									strftime(Temp,45,"%d/%m/%Y %H:%M:%S",&newtime);
									lstrcpyn(pdi->item.pszText, Temp, pdi->item.cchTextMax);
								}
								_findclose(fHandle);
							}
						}
						else
						{
							long fHandle = _findfirst(pItem->GetParseName(), &c_file );
							if(fHandle == -1)
							{
								break;
							}
							//newtime = localtime(&c_file.time_create); 
							//if(newtime != NULL)

							err = _localtime64_s(&newtime, &c_file.time_create);
							if(err == 0)
							{
								strftime(Temp,45,"%d/%m/%Y %H:%M:%S",&newtime);
								lstrcpyn(pdi->item.pszText, Temp, pdi->item.cchTextMax);
							}
							_findclose(fHandle);
						}
					}
					else
					{
							lstrcpyn(pdi->item.pszText, pItem->GetDate(), pdi->item.cchTextMax);
					}
				}
				break;


			case 3:
				{
					char Temp[45];
					UINT width;
					UINT height;

					if(pItem->GetPidlFQ() != NULL)
					{
						CObjet m_cObjet;
						CLoadImage m_cLoadImage;
						if(iTypeData == 0)
						{
							SHFILEINFO     sfi;
							//get the display name of the item
							if(SHGetFileInfo((LPCTSTR)pItem->GetPidlFQ(), 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME))
							{
								if(m_cLoadImage.GetDimensionImageStatic(pItem->GetParseName(), &width, &height) != 0)
								{
									StringCchPrintf(Temp,45,"%dx%d",width,height);
									lstrcpyn(pdi->item.pszText, Temp, pdi->item.cchTextMax);
								}
							}
						}
						else
						{
							//SendMessage(hWndMiniature,WM_COMMAND,MAKEWPARAM(IDC_TRAITEMENTDATA,0),(LPARAM)pItem);
							if(m_cLoadImage.GetDimensionImageStatic(pItem->GetParseName(), &width, &height) != 0)
							{
								StringCchPrintf(Temp,45,"%dx%d",width,height);
								lstrcpyn(pdi->item.pszText, Temp, pdi->item.cchTextMax);
							}
						}
					}
					else
					{

						lstrcpyn(pdi->item.pszText, pItem->GetDimension(), pdi->item.cchTextMax);
					}
				}
				break;

			case 4:
				{
					   //SHFILEINFO     sfi;

					   if(pItem->GetPidlFQ() == NULL)
					   {
						   CObjet m_cObject;
							char Temp[MAX_PATH];
							char Repertoire[MAX_PATH];
							//strcpy(Repertoire,pItem->szParseName);

							StringCchCopy(Repertoire,MAX_PATH,pItem->GetParseName());

							m_cObject.GetFileName(Repertoire,Temp,MAX_PATH);

							Repertoire[strlen(Repertoire) - strlen(Temp) - 1] = '\0';

							lstrcpyn(pdi->item.pszText, Repertoire, pdi->item.cchTextMax);
					   }
				}
				break;
            }
         }

      //do we need to supply the unselected image?
      if(pdi->item.mask & LVIF_IMAGE)
         {
		  if(pItem->GetPidlFQ() == NULL)
		  {
			  //Recherche de l'icone de Regards
				char filename[MAX_PATH];
				GetModuleFileName(hInstance,filename,MAX_PATH);

				 SHFILEINFO     sfi;
         
				 //get the unselected image for this item
				 if(SHGetFileInfo(filename, 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_SMALLICON))
					{
					pdi->item.iImage = sfi.iIcon;
					}
		  }
		  else
		  {
			 SHFILEINFO     sfi;
         
			 //get the unselected image for this item
			 if(SHGetFileInfo((LPCTSTR)pItem->GetPidlFQ(), 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_LINKOVERLAY))
				{
				pdi->item.iImage = sfi.iIcon;
				}
			 }
	  }
      }
      break;

   case LVN_DELETEITEM:
      {
		  LPNMLISTVIEW   pnmlv = (LPNMLISTVIEW)lParam;
		  CItemInfo *     pItem = (CItemInfo *)pnmlv->lParam;

		  if(pItem != NULL)
		  {
			  delete pItem;
			  pItem = NULL;
		  }
      }
      break;

   
   default:
      break;
   }

return 0;
}


/**************************************************************************

   List_Refresh()

**************************************************************************/

HRESULT CList::List_Refresh()
{
	return List_DisplayFolder(NULL);
}


