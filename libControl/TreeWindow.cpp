// TreeWindow.cpp: implementation of the CTreeWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TreeWindow.h"
//Fichier de decompression Zip
#include <files.h>
#include <SADirRead.h>
#include <LocalMapi.h>
#include "graveurisowizard.h"
#include "objet.h"
#include "statusbar.h"
#include "list.h"
#include "preview.h"
#include "listpreview.h"
#include "pane.h"
#include <ItemInfo.h>
#include <deplacefile.h>
#include <DeplaceFichier.h>
#include <ZipFileDialog.h>
#include <dialogfindproc.h>
#include <DefineSizeIcone.h>
#include ".\treewindowdata.h"
#include "parameter.h"

/////////////////////////////////////////////////////////////////////////////////
//Fonction de récupération des mises à jour d'un répertoire
/////////////////////////////////////////////////////////////////////////////////

ULONG _stdcall CTreeWindow::MonitorDirectoryProc(LPVOID pvThis)
{
	DWORD m_dwWaitValue = WAIT_OBJECT_0;
	DWORD dwWaitStatus; 
	HANDLE dwChangeHandles[1]; 
	
	CTreeWindow * m_define = (CTreeWindow *)pvThis;
	string m_stgDirName = m_define->m_pTreeData->cDirectory;
 
	// Watch the C:\WINDOWS directory for file creation and 
	// deletion. 
 
	if(!CObjet::IsValidateFolder(m_define->m_pTreeData->cDirectory))
	{
		//IDS_DRIVENOTAVAILABLE
		//CObjet::AffichageMessage(IDS_DRIVENOTAVAILABLE,IDS_INFOS);
		//MessageBox(NULL,"Le lecteur en cours n'est pas disponible !","Informations",0);
		return 0;
	}
	

	dwChangeHandles[0] = FindFirstChangeNotification( 
		m_stgDirName.c_str(),                 // directory to watch 
		FALSE,                         // do not watch the subtree 
		FILE_NOTIFY_CHANGE_FILE_NAME); // watch file name changes 
 
	if (dwChangeHandles[0] == INVALID_HANDLE_VALUE) 
		return 0;
 
 
	// Change notification is set. Now wait on both notification 
	// handles and refresh accordingly. 

	MSG msg;

	
 
	while (TRUE) 
	{ 
 
		// Wait for notification.

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			SendMessage(m_define->m_hWnd,WM_COMMAND,MAKEWPARAM(IDM_FINMONITOR,0),0);
			break;
		}
			

		Sleep(1000);
 
		dwWaitStatus = MsgWaitForMultipleObjects(1, &dwChangeHandles[0], 
			FALSE, INFINITE,QS_ALLINPUT); 

        if (dwWaitStatus >= m_dwWaitValue && m_define->m_pTreeData->iTypeData == 0)
		{ 

			
			//Création d'une fonction de mise à jour
			
			if(m_define->m_pTreeData->iPauseMonitor == 0)
			{
				m_define->m_pTreeData->cList->List_UpdateFolder();
			}
			else
				m_define->m_pTreeData->iPauseMonitor = 0;
		} 

		FindNextChangeNotification(dwChangeHandles[0]);
	} 

	
	return 0;
}


/////////////////////////////////////////////////////////
//Déclaration des membres statiques
/////////////////////////////////////////////////////////

//int CTreeWindow::iOrderBy;
//CStatusBar * CTreeWindow::m_TreeWindStatusBar;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTreeWindow::CTreeWindow()
{
	m_pTreeData = new CTreeWindowData();
	iChoix = 0;
	iOldYPos = 0;
	iOrderBy = 0;
	m_TreeWindStatusBar = NULL;
	m_cParameter = new CParameter();
}


CTreeWindow::~CTreeWindow()
{

	if(m_pTreeData != NULL)
		delete m_pTreeData;

	m_pTreeData = NULL;

	if(m_cParameter != NULL)
		delete m_cParameter;

	m_cParameter = NULL;
}

//////////////////////////////////////////////////////////////////////////////
//Obtention de l'ordre de trie de l'affichage
//////////////////////////////////////////////////////////////////////////////

int CTreeWindow::GetOrderBy()
{
	return iOrderBy;
}

int CTreeWindow::TestRepertoireFile(const char * File1, const char * File2)
{
	int iDirectory1, iDirectory2;

	if(strcmp(File1,"0.00 Ko") == 0)
		iDirectory1 = 1;
	else
		iDirectory1 = 0;

	if(strcmp(File2,"0.00 Ko") == 0)
		iDirectory2 = 1;
	else
		iDirectory2 = 0;

	return (iDirectory2 == iDirectory1);
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

int CALLBACK CTreeWindow::MyCompProc(LPARAM lp1, LPARAM lp2, LPARAM lp3)
{

    //LV_FINDINFO lvf;
    int nItem1, nItem2;
    char buf1[255], buf2[255];
	char buf3[255], buf4[255];

	int iReturn;

	//LPTREEWINDOWDATA pTreeData = (LPTREEWINDOWDATA)lp3;
	CTreeWindow * m_TreeWindow = (CTreeWindow *)lp3;

	nItem1 = lp1;
	nItem2 = lp2;
    
    ListView_GetItemText(m_TreeWindow->m_pTreeData->cList->m_hWnd, nItem1, m_TreeWindow->m_pTreeData->iSubItem, buf1, sizeof(buf1));
    ListView_GetItemText(m_TreeWindow->m_pTreeData->cList->m_hWnd, nItem2, m_TreeWindow->m_pTreeData->iSubItem, buf2, sizeof(buf2));

	//Transformation en low caractere pour la comparaison

	switch((int)m_TreeWindow->m_pTreeData->iSubItem)
	{
		case 0:
			{
			
				ListView_GetItemText(m_TreeWindow->m_pTreeData->cList->m_hWnd, nItem1, 1, buf3, sizeof(buf3));
				ListView_GetItemText(m_TreeWindow->m_pTreeData->cList->m_hWnd, nItem2, 1, buf4, sizeof(buf4));

				if(m_TreeWindow->m_pTreeData->iColumnUp == 1)
					iReturn = _stricmp(buf2,buf1);
				else
					iReturn = _stricmp(buf1,buf2);

				if(m_TreeWindow->TestRepertoireFile(buf3,buf4))
					return iReturn;
				else
				{
					if(m_TreeWindow->m_pTreeData->iColumnUp == 1)
						return FALSE;
					else
						return TRUE;
				}
			}
			break;

		case 1:
			{
				int value1, value2;
				if(strcmp(buf2,"0.00 Ko") == 0 || strcmp(buf1,"0.00 Ko") == 0)
				{
						if(m_TreeWindow->m_pTreeData->iColumnUp == 1)
							return FALSE;
						else
							return TRUE;
				}
				else
				{
					value1 = atoi(buf1);
					value2 = atoi(buf2);
				}

				if(m_TreeWindow->m_pTreeData->iColumnUp == 1)
					iReturn = value2 > value1;
				else
					iReturn = value1 > value2;

				return iReturn;
			}
			break;

		case 2:
			{
				char DateTemp1[255];
				char DateTemp2[255];

				StringCchPrintf(DateTemp1,255,"%.4s%.2s%.2s%.2s%.2s%.2s",buf1 + 6,buf1 + 3,buf1,buf1 + 11, buf1 + 14,buf1 + 17 );
				StringCchPrintf(DateTemp2,255,"%.4s%.2s%.2s%.2s%.2s%.2s",buf2 + 6,buf2 + 3,buf2,buf2 + 11, buf2 + 14,buf2 + 17 );

				ListView_GetItemText(m_TreeWindow->m_pTreeData->cList->m_hWnd, nItem1, 1, buf3, sizeof(buf3));
				ListView_GetItemText(m_TreeWindow->m_pTreeData->cList->m_hWnd, nItem2, 1, buf4, sizeof(buf4));


				if(m_TreeWindow->TestRepertoireFile(buf3,buf4))
				{
					if(m_TreeWindow->m_pTreeData->iColumnUp == 1)
						iReturn = _stricmp(DateTemp2,DateTemp1);
					else
						iReturn = _stricmp(DateTemp1,DateTemp2);

					return iReturn;
				}
				else
				{
					if(m_TreeWindow->m_pTreeData->iColumnUp == 1)
						return FALSE;
					else
						return TRUE;
				}
			}
			break;
	}


	return TRUE;
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

int CTreeWindow::DeleteListFile(HWND hWnd)
{
	m_pTreeData->iPauseMonitor = 1;
	m_pTreeData->iDelete = 1;			

	if(m_pTreeData->m_ListPreview->IsWindowVisible())
	{
		m_pTreeData->m_ListPreview->PausePreview();

		m_pTreeData->m_ListPreview->List_ExecuteCommandMenu(m_pTreeData->m_vectItem, m_pTreeData->m_vectItem.size(),m_pTreeData->g_pMalloc, "delete");

		m_pTreeData->m_ListPreview->ResumePreview();
	}
	else
	{
		m_pTreeData->iPauseMonitor = 1;
		m_pTreeData->cList->List_ExecuteCommandMenu(m_pTreeData->m_vectItem, m_pTreeData->m_vectItem.size(),m_pTreeData->g_pMalloc, "delete");

		m_pTreeData->iPauseMonitor = 1;
		

	}


	m_pTreeData->iDelete = 0;


	Actualiser();
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

int CTreeWindow::ShowStyleListe(const long &Style)
{
	LONG dNotView = ~(LVS_ICON|LVS_SMALLICON|LVS_LIST|LVS_REPORT);

	if(m_pTreeData->m_ListPreview->IsWindowVisible())
	{
		m_pTreeData->m_ListPreview->ShowWindow(SW_HIDE);
				
		m_pTreeData->m_ListPreview->SauverListe();
		m_pTreeData->m_ListPreview->ViderMemoire();



		switch(m_pTreeData->iTypeData)
		{
			case 0:
				m_pTreeData->cList->List_DisplayFolder(m_pTreeData->cDirectory);
				break;
			case 1:
				m_pTreeData->cList->List_DisplayAlbum(m_pTreeData->m_IconeFileVectorAlbum);
				break;
			case 2:
				m_pTreeData->cList->List_DisplayAlbum(m_pTreeData->m_IconeFileVectorAlbum);
				break;
		}
	}

	SetWindowLong(m_pTreeData->cList->m_hWnd,GWL_STYLE,GetWindowLong(m_pTreeData->cList->m_hWnd,GWL_STYLE)&dNotView|Style);

	m_pTreeData->cList->ShowWindow(SW_SHOW);

	this->SetSplitWindow(m_pTreeData->m_cPane->m_hWnd,m_pTreeData->cList->m_hWnd);
	
	//m_pTreeData->cSplitter->SizeWindowContents(m_pTreeData->m_cPane->m_hWnd, m_pTreeData->cList->m_hWnd);
	
	this->RedrawSplitter();

	//this->OnPaint(m_hWnd,WM_PAINT,0,0);

	
	if(m_pTreeData->m_cPane->IsWindowVisible())
	{
		m_pTreeData->m_cPane->ShowWindow(SW_HIDE);
		m_pTreeData->m_cPane->ShowWindow(SW_SHOW);
	}

	//SendMessage(m_pTreeData->m_cPane->m_hWnd,WM_SIZE,0,1);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

int CTreeWindow::ProcessNotifyDoubleClick(const int &iItem)
{

	if(iItem < 0)
		return -1;

	if(m_pTreeData->iTypeData == 1 || m_pTreeData->iTypeData == 2)
	{
		char szFileNameTemp[MAX_PATH];
		if(m_pTreeData->cList->IsWindowVisible())
		{
			m_pTreeData->cList->List_GetItemText(iItem, szFileNameTemp,MAX_PATH);
		}
		else
		{
			m_pTreeData->m_ListPreview->List_GetItemText(iItem, szFileNameTemp,MAX_PATH);
		}

		SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_LISTDBLCLICK,EVENT_ALBUM),(LPARAM)m_pTreeData->m_IconeFileVectorAlbum);
		SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_LISTDBLCLICK,EVENT_IMAGE),(LPARAM)szFileNameTemp);
	}
	else
	{
		CObjet m_CObjet;
		int iValue;
		char szFileNameTreeWindow[MAX_PATH];
		SHFILEINFO shFinfo;

		if(m_pTreeData->cList->IsWindowVisible())
		{
			m_pTreeData->cList->List_GetItemText(iItem, szFileNameTreeWindow,MAX_PATH);
		}
		else
			m_pTreeData->m_ListPreview->List_GetItemText(iItem, szFileNameTreeWindow,MAX_PATH);

		iValue = m_CObjet.TestCompressionFormat(szFileNameTreeWindow);

		if(iValue == 0)
		{

			//Test Si SubFolder
			SHGetFileInfo(szFileNameTreeWindow, 0, &shFinfo, sizeof(shFinfo), SHGFI_ATTRIBUTES);

			//Test si fichier Zip


			if(shFinfo.dwAttributes & SFGAO_FOLDER)
			{
				SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_LISTDBLCLICK,EVENT_FOLDER),(LPARAM)szFileNameTreeWindow);
				
			}
			else
			{
				//Test si format de gravure:
				iValue = m_CObjet.TestGravureFormat(szFileNameTreeWindow);
				if(iValue == 0)
				{
					SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_LISTDBLCLICK,EVENT_FILE),0);
					SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_LISTDBLCLICK,EVENT_IMAGE),(LPARAM)szFileNameTreeWindow);
				}
				else
				{
					CGraveurISOWizard m_cGraveur;
					m_cGraveur.BurnIsoWizard(GetParent(),szFileNameTreeWindow);
				}
			}

		}
		else
		{
			switch(iValue)
			{
				case 1:
					//DialogBoxParam(hInstance, (LPCTSTR)IDD_ZIPDLG, m_hWnd, (DLGPROC)CDialog::ZipFileDialog,(LPARAM)szFileNameTreeWindow);
					{
						CZipFileDialog * m_ZipFileDialog = new CZipFileDialog(hInstance);
						m_ZipFileDialog->SetParam(szFileNameTreeWindow);
						m_ZipFileDialog->CreateDlg(hInstance, (LPCTSTR)IDD_ZIPDLG, m_hWnd);
						delete m_ZipFileDialog;
					}
					break;
					
			}
		}
	}
	return 0;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

void CTreeWindow::DesHighlightItem(const int &iNumItem,const bool &m_bFlip)
{
	LVITEM pitem;

	ZeroMemory(&pitem,sizeof(LVITEM));

	pitem.iSubItem = 0;
	pitem.mask = LVIF_PARAM | LVIF_STATE;
	pitem.iItem = iNumItem;

	ListView_GetItem(m_pTreeData->m_ListPreview->m_hWnd, &pitem);

	if(!m_bFlip)
	{
		pitem.mask |= LVIF_STATE;
		pitem.stateMask |= LVIS_OVERLAYMASK | LVIS_SELECTED;
		pitem.state = INDEXTOOVERLAYMASK(1); //1 is the index for the shared overlay image

		ListView_SetItem(m_pTreeData->m_ListPreview->m_hWnd, &pitem);

	}
	else
	{
		ListView_SetItemState(m_pTreeData->m_ListPreview->m_hWnd, iNumItem, 0, LVIS_OVERLAYMASK);
	}
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

int CTreeWindow::DeplacementCurseur(int iNumItem, UINT flags)
{

	for (VECTITEM::const_iterator dit = m_pTreeData->m_vectItem.begin(); dit!=m_pTreeData->m_vectItem.end(); dit++)
	{
		int iItem = *dit;
		DesHighlightItem(iItem);
	}	

	m_pTreeData->m_vectItem.clear();

	iNumItem = ListView_GetNextItem(m_pTreeData->m_ListPreview->m_hWnd,iNumItem,flags);
	ListView_SetItemState(m_pTreeData->m_ListPreview->m_hWnd, iNumItem, LVIS_SELECTED, 0);
	//DesHighlightItem(iNumItem,false);
	m_pTreeData->m_vectItem.push_back(iNumItem);
	return 0;
}


LRESULT CTreeWindow::OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static char szEditFileName[255];
	static HWND hEdit;
	static int iLabelEdit;
	int idCtrl;
	LPNMHDR pnmh;
	LPNMLISTVIEW pnmv;
	idCtrl = (int) wParam; 
	pnmh = (LPNMHDR) lParam; 
	LPNMITEMACTIVATE lpnmitem;

	switch(LOWORD(wParam))
	{
		case IDM_HIDDENPANE:
			{
				CheckMenuItem(GetMenu(GetParent()),IDM_AFFICHAGE_CONTROL_PREVIEWAREA,MF_UNCHECKED);
				m_pTreeData->m_cPane->ShowWindow(SW_HIDE);

				this->RedrawSplitter();

				return 0;
			}
			break;
	}

	if(pnmh == NULL)
		return 0;

	switch (((LPNMHDR)lParam)->code)
	{

	   case LVN_GETDISPINFO:
		   {
			   NMLVDISPINFO   *pdi = (NMLVDISPINFO*)lParam;
			   CItemInfo * pItem = (CItemInfo *)pdi->item.lParam;
			   
			   if(m_pTreeData->iAffichageDimension == 0)
			   {
				  if(pdi->item.mask & LVIF_TEXT)
				  {
					 if(pdi->item.iSubItem == 3)
						 break;
				  }

			   }

			   if(m_pTreeData->cList->IsWindowVisible())
			   {
					m_pTreeData->cList->List_Notify(lParam, m_pTreeData->iTypeData);
			   }
			   else
			   {
				   if(m_pTreeData->m_ListPreview->IsWindowVisible())
					{
						m_pTreeData->m_ListPreview->List_Notify(lParam);
					}
			   }
		  }
		  break;

	   case LVN_DELETEITEM:
		   {
			   if(m_pTreeData->cList->IsWindowVisible())
				m_pTreeData->cList->List_Notify(lParam, m_pTreeData->iTypeData);
			  else
			  {
				  if(m_pTreeData->m_ListPreview->IsWindowVisible())
					m_pTreeData->m_ListPreview->List_Notify(lParam);
			  }
		   }
		  break;


		case LVN_COLUMNCLICK:
			{
				pnmv = (LPNMLISTVIEW) lParam;

				if(m_pTreeData->iColumnUp == 0)
					m_pTreeData->iColumnUp = 1;
				else
					m_pTreeData->iColumnUp = 0;

				switch(pnmv->iSubItem)
				{
					case 0:
						iOrderBy = m_pTreeData->iColumnUp;
						break;
					case 1:
						iOrderBy = m_pTreeData->iColumnUp + 2;
						break;
					case 2:
						iOrderBy = m_pTreeData->iColumnUp + 4;
						break;
				}
				m_pTreeData->iSubItem = pnmv->iSubItem;

				ListView_SortItemsEx(m_pTreeData->cList->m_hWnd, MyCompProc, (LPARAM)this);
			}
			break;

		case NM_RELEASEDCAPTURE:
			m_pTreeData->m_bCaptureList = true;
			break;

	   case LVN_BEGINDRAG:
		   {
			   if(!m_pTreeData->iDragEnable)
				   break;

				if(m_pTreeData->bDragging == TRUE)
				{
					m_pTreeData->bDragging = FALSE;
					break;
				}
				
				SendMessage(m_pTreeData->cList->m_hWnd, WM_SETREDRAW, 0, 0);

				POINT pt;
				POINT p;
				int iPos, iHeight;
				IMAGEINFO imf;
				CIconeFile m_IconeFileTemp;
				// You can set your customized cursor here

				p.x = 8;
				p.y = 8;

				m_pTreeData->bDragging = TRUE;

				m_pTreeData->m_IconeFileVector.clear();

				if(m_pTreeData->iTypeData != 0)
					break;

				if(m_pTreeData->cList->IsWindowVisible())
				{
					iPos = ListView_GetNextItem(m_pTreeData->cList->m_hWnd, -1, LVNI_SELECTED);
					m_pTreeData->cList->List_GetItemText(iPos, m_IconeFileTemp.m_stgFullPathName, MAX_PATH);
					m_pTreeData->hDragImageList = ListView_CreateDragImage(m_pTreeData->cList->m_hWnd, iPos, &p);
					ImageList_GetImageInfo(m_pTreeData->hDragImageList, 0, &imf);
					iHeight = imf.rcImage.bottom;
				}
				else
				{
					if(m_pTreeData->m_ListPreview->IsWindowVisible())
					{
						m_pTreeData->hDragImageList = ListView_CreateDragImage(m_pTreeData->m_ListPreview->m_hWnd, m_pTreeData->iNumItem, &p);
						ImageList_GetImageInfo(m_pTreeData->hDragImageList, 0, &imf);
						iHeight = imf.rcImage.bottom;
					}
					else
						return 0;
				}

				SelectListImage();

				// Now we can initialize then start the drag action
				ImageList_BeginDrag(m_pTreeData->hDragImageList, 0, 0, 0);

				pt = ((NM_LISTVIEW*) ((LPNMHDR)lParam))->ptAction;
				ClientToScreen(m_pTreeData->cList->m_hWnd, &pt);

				ImageList_DragEnter(GetDesktopWindow(), pt.x, pt.y);

				
				// Don't forget to capture the mouse
				SetCapture(GetParent());
				SetFocus(GetParent());
				SendMessage(GetParent(),WM_COMMAND, MAKEWPARAM(IDM_DRAGGING,1),(LPARAM)&m_pTreeData->m_IconeFileVector);

				//SendMessage(hWnd2,WM_SETREDRAW,0,0);
			}
			break;

			
		case LVN_KEYDOWN:
			{

				LPNMLVKEYDOWN pnkd;
				pnkd = (LPNMLVKEYDOWN) lParam; 
				short ctrl_value = HIWORD(GetKeyState(VK_CONTROL));
				
				switch(pnkd->wVKey)
				{
					case VK_DELETE:
						SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_EDITION_SUPPRIMER,IDM_EDITION_SUPPRIMER),0);
						break;

					case VK_RETURN:
						if(m_pTreeData->cList->IsWindowVisible())
							ProcessNotifyDoubleClick(ListView_GetNextItem(m_pTreeData->cList->m_hWnd,-1,LVNI_SELECTED)); 
						else
						{
							if(m_pTreeData->m_ListPreview->IsWindowVisible())
								ProcessNotifyDoubleClick(m_pTreeData->iNumItem); 
						}
						break;


					case 67:
						{
							//Copier
							//MessageBox(NULL,"test","test",0);
							if(ctrl_value == -1)
								SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDS_COPY,0),0);
						}
						break;

					case 70:
						{
							//Copier
							//MessageBox(NULL,"test","test",0);
							if(ctrl_value == -1)
								SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(ID_EDITION_RECHERCHERUNFICHIER,0),0);
						}
						break;

					case 86:
						{
							//Coller
							if(ctrl_value == -1)
								SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDS_PASTE,0),0);
						}
						break;

					case 88:
						{
							//Couper
							if(ctrl_value == -1)
								SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDS_CUT,0),0);
						}
						break;

					case 65:
						{
							//Tout sélectionner
							if(ctrl_value == -1)
							{
								m_pTreeData->m_vectItem.clear();
								if(m_pTreeData->m_ListPreview->IsWindowVisible())
								{
									int iNumLocal = ListView_GetNextItem(m_pTreeData->m_ListPreview->m_hWnd,-1,LVNI_ALL);
									while(iNumLocal != -1)
									{
										m_pTreeData->m_vectItem.push_back(iNumLocal);
										DesHighlightItem(iNumLocal,false);
										iNumLocal = ListView_GetNextItem(m_pTreeData->m_ListPreview->m_hWnd,iNumLocal,LVNI_ALL);
									}
								}
								else
								{
									ListView_SetItemState(m_pTreeData->cList->m_hWnd, -1, LVIS_SELECTED | LVIS_DROPHILITED  , LVIS_SELECTED);
									int iNumLocal = ListView_GetNextItem(m_pTreeData->cList->m_hWnd,-1,LVNI_ALL);
									while(iNumLocal != -1)
									{
										m_pTreeData->m_vectItem.push_back(iNumLocal);
										DesHighlightItem(iNumLocal,false);
										iNumLocal = ListView_GetNextItem(m_pTreeData->cList->m_hWnd,iNumLocal,LVNI_ALL);
									}
								}
							}
						}
						break;

					case VK_ESCAPE:
						SendMessage(GetParent(),WM_KEYDOWN,VK_ESCAPE,0);
						break;

					case VK_F1:
						SendMessage(GetParent(),WM_KEYDOWN,VK_F1,0);
						break;

					case VK_F2:
						{
							int iIndex;
							iIndex=SendMessage(m_pTreeData->cList->m_hWnd,LVM_GETNEXTITEM,-1,LVIS_SELECTED); // get selected item
							ListView_EditLabel(m_pTreeData->cList->m_hWnd,iIndex);
						}
						break;

					case VK_F5:
						
						if(m_pTreeData->m_ListPreview->IsWindowVisible())
						{
							for (VECTITEM::const_iterator dit = m_pTreeData->m_vectItem.begin(); dit!=m_pTreeData->m_vectItem.end(); dit++)
							{
								int iItem = *dit;
								DesHighlightItem(iItem,false);
							}
							
						}
						SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(ID_AFFICHAGE_ACTUALISER,0),0);
						break;

					case VK_UP:
						if(m_pTreeData->m_ListPreview->IsWindowVisible())
						{
							return DeplacementCurseur(m_pTreeData->iNumItem, LVNI_ABOVE);
						}
						break;

					case VK_LEFT:
						if(m_pTreeData->m_ListPreview->IsWindowVisible())
						{
							return DeplacementCurseur(m_pTreeData->iNumItem, LVNI_TOLEFT);
						}
						break;

					case VK_RIGHT:
						if(m_pTreeData->m_ListPreview->IsWindowVisible())
						{
							return DeplacementCurseur(m_pTreeData->iNumItem, LVNI_TORIGHT);
						}
						break;

					case VK_DOWN:
						if(m_pTreeData->m_ListPreview->IsWindowVisible())
						{
							return DeplacementCurseur(m_pTreeData->iNumItem, LVNI_BELOW);
						}
						break;

				}
				return 0;
			}
			break;

		case LVN_MARQUEEBEGIN:
			{
				for (VECTITEM::const_iterator dit = m_pTreeData->m_vectItem.begin(); dit!=m_pTreeData->m_vectItem.end(); dit++)
				{
					int iItem = *dit;
					DesHighlightItem(iItem);
				}	

				m_pTreeData->m_vectItem.clear();
				m_pTreeData->iMarquee = 1;
			}
			break;
			
		case NM_CLICK:
			{
				//SetFocus(hWnd);
				HWND hWndList;
				if(m_pTreeData->m_ListPreview->IsWindowVisible())
					hWndList = m_pTreeData->m_ListPreview->m_hWnd;
				else
					hWndList = m_pTreeData->cList->m_hWnd;

				SetFocus(hWndList);

				if(GetKeyState(VK_SHIFT) < 0 || GetKeyState(VK_CONTROL) < 0)
				{
					//m_pTreeData->m_vectItem.push_back(m_pTreeData->iNumItem);
					if(!m_pTreeData->m_ListPreview->IsWindowVisible())
						m_pTreeData->m_vectItem.clear();

					int iNumLocalItem = ListView_GetNextItem(hWndList,-1,LVIS_SELECTED);
					while(iNumLocalItem != -1)
					{
						m_pTreeData->m_vectItem.push_back(iNumLocalItem);
						DesHighlightItem(iNumLocalItem,false);
						iNumLocalItem = ListView_GetNextItem(hWndList,iNumLocalItem,LVIS_SELECTED);
					}
					if(m_pTreeData->m_ListPreview->IsWindowVisible())
						m_pTreeData->m_vectItem.push_back(m_pTreeData->iNumItem);

					break;
				}


				if((m_pTreeData->iMarquee == 0 && GetKeyState(VK_SHIFT) >= 0 && GetKeyState(VK_CONTROL) >= 0) || (m_pTreeData->iMarquee == 0 && m_pTreeData->m_ListPreview->IsWindowVisible()))
				{
					for (VECTITEM::const_iterator dit = m_pTreeData->m_vectItem.begin(); dit!=m_pTreeData->m_vectItem.end(); dit++)
					{
						int iItem = *dit;
						if(iItem != m_pTreeData->iNumItem)
						{
							DesHighlightItem(iItem);
						}
					}	
					m_pTreeData->m_vectItem.clear();
					m_pTreeData->m_vectItem.push_back(m_pTreeData->iNumItem);
				}	

				m_pTreeData->iMarquee = 0;

				
			}
			break;

			
		case LVN_ITEMCHANGED:
			{
				LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE) lParam;

				if(lpnmitem->uNewState & LVIS_SELECTED && !m_pTreeData->bDragging && !m_pTreeData->iMarquee)
				{
					m_pTreeData->iNumItem = lpnmitem->iItem;
					
					char cFilenameTemp[MAX_PATH];
					if(m_pTreeData->cList->IsWindowVisible())
						m_pTreeData->cList->List_GetItemText(lpnmitem->iItem, cFilenameTemp, MAX_PATH);
					else
					{
						m_pTreeData->m_ListPreview->List_GetItemText(lpnmitem->iItem, cFilenameTemp, MAX_PATH);
					}

					char cInfos[MAX_PATH];
					CObjet m_cObjet;
					m_cObjet.GetInfosFile(cFilenameTemp, cInfos,MAX_PATH);
					if(cInfos == NULL)
						break;
					
					SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_LISTCLICK,0),(LPARAM)cFilenameTemp);
					
					//Prévisualisation
					if(m_pTreeData->m_cPane->IsWindowVisible())
					{
						if(_stricmp(m_pTreeData->cFileTemp,cFilenameTemp) != 0)
						{

							m_TreeWindStatusBar->UpdateStatusBar(cInfos,2,0);

							//Récupération de l'espace libre sur le disque
							//m_cObjet.GetDiskSpaceInfos(cFilenameTemp,cInfos,255);
							//m_TreeWindStatusBar->UpdateStatusBar(cInfos,4,0);
							
							
							if(m_pTreeData->iTypeData == 1 && m_pTreeData->iInfosFichier == 1)
							{
								IconeFileVector::const_iterator fit;
								static CIconeFile IconeFileTemp;
								
								if(iOrderBy == 0)
								{
									m_pTreeData->iNumSelectedItem = lpnmitem->iItem;
									IconeFileTemp = m_pTreeData->m_IconeFileVectorAlbum->at(lpnmitem->iItem);
									SendMessage(m_pTreeData->m_Preview->m_hWnd,WM_COMMAND,MAKEWPARAM(IDM_ALBUMPT,0),(LPARAM)&IconeFileTemp);
								}
								else
								{
									long j = 0;
									for (fit = m_pTreeData->m_IconeFileVectorAlbum->begin(); fit!= m_pTreeData->m_IconeFileVectorAlbum->end(); fit++,j++)
									{
										m_pTreeData->iNumSelectedItem = j;
										IconeFileTemp = *fit;
										if(_stricmp(IconeFileTemp.m_stgFullPathName,cFilenameTemp) == 0)
										{
											SendMessage(m_pTreeData->m_Preview->m_hWnd,WM_COMMAND,MAKEWPARAM(IDM_ALBUMPT,0),(LPARAM)&IconeFileTemp);
											break;
										}
									}
								}
								
							}
							SendMessage(m_pTreeData->m_Preview->m_hWnd,WM_COMMAND,MAKEWPARAM(IDM_FILENAME,m_pTreeData->iTypeData),(LPARAM)cFilenameTemp);

							

						}
					}
				}
				if(!m_pTreeData->iMarquee)
				{	
					if(m_pTreeData->m_ListPreview->IsWindowVisible() && lpnmitem->uNewState == 3)
					{

							//Récupération du numéro de l'image
							LVITEM pitem;
							//CIBitmap m_ciBitmap;
							//CFiltre m_cFiltre;
							//POINT pt;

							ZeroMemory(&pitem,sizeof(LVITEM));

							pitem.iSubItem = 0;
							pitem.mask = LVIF_IMAGE | LVIF_PARAM;
							pitem.iItem = lpnmitem->iItem;
							ListView_GetItem(m_pTreeData->m_ListPreview->m_hWnd, &pitem);

							DesHighlightItem(lpnmitem->iItem,false);

							m_pTreeData->iNumItem = lpnmitem->iItem;

					}
				}
				else
				{
						//ListView_SetItemState(m_pTreeData->m_ListPreview->m_hWnd, lpnmitem->iItem,0 ,LVIS_SELECTED);
						//m_pTreeData->iNumItem = lpnmitem->iItem;

						DesHighlightItem(lpnmitem->iItem,false);
						m_pTreeData->m_vectItem.push_back(lpnmitem->iItem);
				}
				
				return 0;
			}


		case NM_DBLCLK:
			{
				lpnmitem = (LPNMITEMACTIVATE) lParam;
				if(m_pTreeData->cList->IsWindowVisible())
				{
					int iPos = ListView_GetNextItem(m_pTreeData->cList->m_hWnd, -1, LVNI_SELECTED);
					if(iPos != -1)
						ProcessNotifyDoubleClick(lpnmitem->iItem);
				}
				else
				{
					if(m_pTreeData->m_ListPreview->IsWindowVisible())
						ProcessNotifyDoubleClick(m_pTreeData->iNumItem); 
					
				}
			}
			return 0;

		case LVN_BEGINLABELEDIT:
			{
				LVITEM LvItem;
				memset(&LvItem,0,sizeof(LvItem));

				iLabelEdit = 1;

				hEdit=ListView_GetEditControl(m_pTreeData->cList->m_hWnd);
				LvItem.mask=LVIF_TEXT;
				LvItem.iSubItem=0;                        // we get the item only (change for sub)
				LvItem.pszText=szEditFileName;                      // text type
				GetWindowText(hEdit, szEditFileName, sizeof(szEditFileName)); // get the text into a buffer
			}
			break;

		case LVN_ENDLABELEDIT:
			{

				short iValueShort = HIWORD(GetKeyState(VK_ESCAPE));
				if(iValueShort == -1)
				{
					iLabelEdit = 0;
					break;
				}

				int iIndex;
				char text[255]="";
				char cTemp1[MAX_PATH];
				char cTemp2[MAX_PATH];
				LVITEM LvItem;
				memset(&LvItem,0,sizeof(LvItem));

				iIndex=SendMessage(m_pTreeData->cList->m_hWnd,LVM_GETNEXTITEM,-1,LVNI_FOCUSED); // get selected item
 
				LvItem.mask=LVIF_TEXT;
				LvItem.iSubItem=0;                        // we get the item only (change for sub)
				LvItem.pszText=text;                      // text type
				GetWindowText(hEdit, text, sizeof(text)); // get the text into a buffer

				m_pTreeData->iPauseMonitor = 1;

				if(_stricmp(text,szEditFileName) != 0)
				{
					//Renommage
					StringCchPrintf(cTemp1,MAX_PATH,"%s\\%s",m_pTreeData->cDirectory,text);
					StringCchPrintf(cTemp2,MAX_PATH,"%s\\%s",m_pTreeData->cDirectory,szEditFileName);
					rename(cTemp2,cTemp1);
					SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(ID_AFFICHAGE_ACTUALISER,0),0);

					if(m_pTreeData->iCreerRepertoire)
					{
						SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(ID_LISTE_CRERUNRPERTOIRE,0),(LPARAM)0);
						m_pTreeData->iCreerRepertoire = 0;
					}

				}

				iLabelEdit = 0;
			}
			break;

	}
	return 0;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void CTreeWindow::SelectListImage()
{
	CObjet m_cObjet;
	m_pTreeData->m_IconeFileVector.clear();
	char TempFullPath[512];

	if(m_pTreeData->m_ListPreview->IsWindowVisible())
	{
		for (VECTITEM::const_iterator dit = m_pTreeData->m_vectItem.begin(); dit!=m_pTreeData->m_vectItem.end(); dit++)
		{
			CIconeFile m_IconeFileTemp;
			int iItem = *dit;
			m_pTreeData->m_ListPreview->List_GetItemText(iItem, TempFullPath, MAX_PATH);
			m_cObjet.AlbumGestionAddItem(TempFullPath,m_IconeFileTemp);
			m_pTreeData->m_IconeFileVector.push_back(m_IconeFileTemp);
		}
	}
	else
	{
		int iPos = ListView_GetNextItem(m_pTreeData->cList->m_hWnd, -1, LVNI_SELECTED);
		while (iPos != -1) 
		{
			CIconeFile m_IconeFileTemp;
			m_pTreeData->cList->List_GetItemText(iPos, TempFullPath, MAX_PATH);
			
			m_cObjet.AlbumGestionAddItem(TempFullPath,m_IconeFileTemp);
			
			//m_cObjet.GetFileName(m_IconeFileTemp.m_stgFullPathName,m_IconeFileTemp.m_stgFileName);
			/*
			ListView_GetItemText(m_pTreeData->cList->m_hWnd, iPos, 1, m_IconeFileTemp.szSize, sizeof(m_IconeFileTemp.szSize));
			ListView_GetItemText(m_pTreeData->cList->m_hWnd, iPos, 2, m_IconeFileTemp.szTimeCreate, sizeof(m_IconeFileTemp.szTimeCreate));*/
			m_pTreeData->m_IconeFileVector.push_back(m_IconeFileTemp);
			iPos = ListView_GetNextItem(m_pTreeData->cList->m_hWnd, iPos, LVNI_SELECTED);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void CTreeWindow::DeplacerListImage(int iType, char * destPath)
{
	CDeplaceFile * m_deplaceFile = new CDeplaceFile();
	m_deplaceFile->cDir = new char[MAX_PATH];
	strcpy_s(m_deplaceFile->cDir,MAX_PATH,destPath);
	m_deplaceFile->iDepl = iType;
	m_deplaceFile->m_IconeFile = &m_pTreeData->m_IconeFileVector;

	CDeplaceFichier * m_cDeplaceFichier = new CDeplaceFichier();
	m_cDeplaceFichier->SetParam(m_deplaceFile);
	m_cDeplaceFichier->CreateDlg(hInstance, (LPCTSTR)IDD_FICHIER, m_hWnd);

	delete m_cDeplaceFichier;
	delete[] m_deplaceFile->cDir;
	delete m_deplaceFile;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void CTreeWindow::SelectListImage(int iType)
{
	CObjet m_cObjet;
	//Sélectionner du répertoire
	char sPath[MAX_PATH];
	
	StringCchCopy(sPath,MAX_PATH,m_pTreeData->cDirectory);

	if(m_cObjet.GetDirectory(m_hWnd, sPath))
	{
		SelectListImage();
		DeplacerListImage(iType,sPath);
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CTreeWindow::OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint( hWnd, &ps );
		
	if(m_pTreeData->m_cPane->IsWindowVisible())
		SendMessage(m_pTreeData->m_cPane->m_hWnd,message,wParam,lParam);
	if(m_pTreeData->cList->IsWindowVisible())
		SendMessage(m_pTreeData->cList->m_hWnd,message,wParam,lParam);

	if(m_pTreeData->m_ListPreview->IsWindowVisible())
		SendMessage(m_pTreeData->m_ListPreview->m_hWnd,message,wParam,lParam);


	EndPaint( hWnd, &ps );
	ReleaseDC(hWnd,hDC);

	return 0;
}

LRESULT CTreeWindow::OnEraseBackground(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_pTreeData->m_cPane->IsWindowVisible())
		SendMessage(m_pTreeData->m_cPane->m_hWnd,message,wParam,lParam);
	if(m_pTreeData->cList->IsWindowVisible())
		SendMessage(m_pTreeData->cList->m_hWnd,message,wParam,lParam);

	if(m_pTreeData->m_ListPreview->IsWindowVisible())
		SendMessage(m_pTreeData->m_ListPreview->m_hWnd,message,wParam,lParam);

	return 0;
}

void CTreeWindow::Actualiser(LPARAM lParam)
{
	//HWND hWnd;
	POINT pt;
	

	m_pTreeData->iAffichageAfterMiniature = 8;

	if(lParam != NULL)
	{
		if(_stricmp(m_pTreeData->cDirectory,(const char *)lParam) != 0)
			return;
	}
	
	if(m_pTreeData->m_ListPreview->IsWindowVisible())
	{
		//hWnd = m_pTreeData->m_ListPreview->m_hWnd;

		m_pTreeData->iDemarre = 1;
		if(m_pTreeData->m_ListPreview->stopGenThumbnail())
		{
			Sleep(1000);
			return;
		}

		if(m_pTreeData->iTypeData == 0)
		{
			//ListView_DeleteAllItems(hWnd);
			m_pTreeData->m_ListPreview->InitList(m_pTreeData->cDirectory);
		}
		else
		{
			//ListView_DeleteAllItems(hWnd);
			m_pTreeData->m_ListPreview->InitList(m_pTreeData->m_IconeFileVectorAlbum, cNumAlbum);	
		}
		
		DesHighlightItem(m_pTreeData->iNumItem,false);	

		m_pTreeData->m_ListPreview->List_EnsureVisible(m_pTreeData->iNumItem,iOldYPos);

		m_pTreeData->iDemarre = 0;
	}
	else
	{
		ListView_GetItemPosition(m_pTreeData->cList->m_hWnd,m_pTreeData->iNumItem,&pt);

		if(m_pTreeData->iTypeData == 0)
			m_pTreeData->cList->List_DisplayFolder(m_pTreeData->cDirectory);
		else
			m_pTreeData->cList->List_DisplayAlbum(m_pTreeData->m_IconeFileVectorAlbum);

		m_pTreeData->cList->List_EnsureVisible(m_pTreeData->iNumItem,iOldYPos);

		ListView_SetItemState(m_pTreeData->cList->m_hWnd,m_pTreeData->iNumItem,LVIS_SELECTED | LVIS_DROPHILITED, LVIS_SELECTED);

	}

	iOldYPos = m_pTreeData->iNumItem;
}

LRESULT CTreeWindow::FinGenThumbnail()
{
	CObjet::EndWait();

	m_pTreeData->iTypeAffichage = 0;

	if(m_pTreeData->iDemarre == 0)
		return 0;
	
	//Sleep(1000);

	switch(m_pTreeData->iAffichageAfterMiniature)
	{
		case 1:
			ShowStyleListe(LVS_ICON);
			SendMessage(m_pTreeData->m_cPane->m_hWnd,WM_SIZE,0,1);
			break;
		case 2:
			ShowStyleListe(LVS_SMALLICON);
			SendMessage(m_pTreeData->m_cPane->m_hWnd,WM_SIZE,0,1);
			break;
		case 3:
			ShowStyleListe(LVS_REPORT);
			SendMessage(m_pTreeData->m_cPane->m_hWnd,WM_SIZE,0,1);
			break;
		case 4:
			ShowStyleListe(LVS_LIST);
			SendMessage(m_pTreeData->m_cPane->m_hWnd,WM_SIZE,0,1);
			break;
		case 5:
			m_pTreeData->iTypeAffichage = 1;
			if(!m_pTreeData->m_ListPreview->IsWindowVisible())
			{
				m_pTreeData->cList->ShowWindow(SW_HIDE);
				m_pTreeData->m_ListPreview->ShowWindow(SW_SHOW);

				switch(m_pTreeData->iTypeData)
				{
					case 0:
						m_pTreeData->m_ListPreview->InitList(m_pTreeData->cDirectory);
						break;
					case 1:
						m_pTreeData->m_ListPreview->InitList(m_pTreeData->m_IconeFileVectorAlbum,cNumAlbum);
						break;
					case 2:
						m_pTreeData->m_ListPreview->InitList(m_pTreeData->m_IconeFileVectorAlbum,cNumAlbum);
						break;
				}

			
				this->RedrawSplitter();
				

				if(m_pTreeData->m_cPane->IsWindowVisible())
				{
					m_pTreeData->m_cPane->ShowWindow(SW_HIDE);
					m_pTreeData->m_cPane->ShowWindow(SW_SHOW);
				}
			}
			break;

		case 6:
			{
				m_pTreeData->m_ListPreview->SauverListe();
				m_pTreeData->iTypeAffichage = 1;
				m_pTreeData->m_ListPreview->InitList(m_pTreeData->cDirectory);
				StringCchCopy(m_pTreeData->cTempDir,MAX_PATH,m_pTreeData->cDirectory);
			}
			break;

		case 7:
			{
				m_pTreeData->iTypeAffichage = 1;
				switch(m_pTreeData->iTypeData)
				{
					case 0:
						m_pTreeData->m_ListPreview->InitList(m_pTreeData->cDirectory,"-1");
						break;
					case 1:
						m_pTreeData->m_ListPreview->InitList(m_pTreeData->m_IconeFileVectorAlbum,"-1");
						break;
					case 2:
						m_pTreeData->m_ListPreview->InitList(m_pTreeData->m_IconeFileVectorAlbum,"-1");
						break;
				}
			}
			break;

		case 8:
			{
				if(m_pTreeData->iTypeData == 0)
					m_pTreeData->m_ListPreview->InitList(m_pTreeData->cDirectory);
				else
					m_pTreeData->m_ListPreview->InitList(m_pTreeData->m_IconeFileVectorAlbum, cNumAlbum);	

				m_pTreeData->m_ListPreview->List_EnsureVisible(m_pTreeData->iNumItem,iOldYPos);
			}
			break;

		case 9:
			{
				m_pTreeData->iTypeAffichage = 1;
				m_pTreeData->m_ListPreview->SauverListe();
				m_pTreeData->m_ListPreview->InitList(m_pTreeData->m_IconeFileVectorAlbum,cNumAlbum);
			}
			break;

		case 10:
			{
				m_pTreeData->iTypeAffichage = 1;
				m_pTreeData->m_ListPreview->SauverListe();
				m_pTreeData->m_ListPreview->InitList(m_pTreeData->cDirectory);
			}
			break;
	}

	m_pTreeData->iDemarre = 0;
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CTreeWindow::OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	char cInfos[255];
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam); 
	//static HIMAGELIST hImgListe;

	// Parse the menu selections:
	switch (wmId)
	{
		case IDM_LISTDBLCLICK:
			{
				SendMessage(GetParent(),WM_COMMAND,wParam,lParam);
				return 0;
			}

		case ID_MINIATURESIZE:
			{
				int m_iLargeur = 90;
				int m_iHauteur = 70;
				m_cParameter->GetOptionMiniature(m_iLargeur,m_iHauteur);
				CDefineSizeIcone * m_cDefineSizeIcone = new CDefineSizeIcone();
				m_cDefineSizeIcone->SetParam(m_iLargeur,m_iHauteur);
				m_cDefineSizeIcone->CreateDlg(hInstance,(LPCTSTR)IDD_DLGICONESIZE,hWnd);
				int m_iRetour = m_cDefineSizeIcone->GetReturnCode();
				if(m_iRetour == 1)
				{
					m_iHauteur = m_cDefineSizeIcone->GetHauteur();
					m_iLargeur = m_cDefineSizeIcone->GetLargeur();
					m_cParameter->SetOptionMiniature(m_iLargeur,m_iHauteur);
					SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(ID_AFFICHAGE_REGENERERLESMINIATURES,0),0);
				}

				delete m_cDefineSizeIcone;

			}
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT:
			{
				int m_iQuality = (int)lParam;
				this->m_pTreeData->m_Preview->SetPictureQuality(m_iQuality);
			}
			break;

		case IDS_CUT:
			{
				if(m_pTreeData->cList->IsWindowVisible())
				{
						SelectListImage();
						iChoix = 1;
				}
			}
			break;

		case IDM_CANCELDRAG:
			m_pTreeData->bDragging = 1;
			break;


		//Coller
		case IDS_PASTE:
			{
				m_pTreeData->iPauseMonitor = 1;
				if(m_pTreeData->cList->IsWindowVisible())
				{
					switch(m_pTreeData->iTypeData)
					{
						case 0:
							DeplacerListImage(iChoix,m_pTreeData->cDirectory);
							break;
						case 1:
							//Copie dans un album
							//Récupération du numéro de l'album
							SendMessage(GetParent(),message, MAKEWPARAM(ID_LISTE_AJOUTSFICHIERSDIRECTDANSCATALOGUE,0), (LPARAM)&m_pTreeData->m_IconeFileVector);
							break;
					}
					Actualiser();
				}
			}
			break;

		//Coller
		case IDS_COPY:
			{
				if(m_pTreeData->cList->IsWindowVisible())
				{
					SelectListImage();
					iChoix = 0;
				}
			}
			break;

		case ID_LISTE_ENVOYEMEL:
			{
				CMapi m_cMapi;
				HWND hWndList;
				VectorAttachment m_VectorAttachment;

				if(m_pTreeData->m_ListPreview->IsWindowVisible())
					hWndList = m_pTreeData->m_ListPreview->m_hWnd;
				else
					hWndList = m_pTreeData->cList->m_hWnd;

				for(int i = 0; i < m_pTreeData->m_vectItem.size(); i++)
				{
					LVITEM lvItem;
					ZeroMemory(&lvItem, sizeof(lvItem));
					lvItem.mask = LVIF_PARAM;
					lvItem.iItem = m_pTreeData->m_vectItem[i];
					if(ListView_GetItem(hWndList, &lvItem))
					{
						CItemInfo *  pInfo = (CItemInfo *)lvItem.lParam;
						m_VectorAttachment.push_back(pInfo->GetParseName());
					}
				}

				if(m_VectorAttachment.size() > 0)
					m_cMapi.SendEmail("",&m_VectorAttachment);

				BringWindowToTop(GetParent());
			}
			break;

		case ID_LISTE_COPY:
			{
				//Affichage d'un sous-menu
				//HWND hWndList;

				//Vidage du presse papier et insertion des données dedant
				OpenClipboard(hWnd);

				EmptyClipboard();

				CloseClipboard(); 

				if(m_pTreeData->m_ListPreview->IsWindowVisible())
					m_pTreeData->m_ListPreview->List_ExecuteCommandMenu(m_pTreeData->m_vectItem, m_pTreeData->m_vectItem.size(),m_pTreeData->g_pMalloc, "copy");
				else
					m_pTreeData->cList->List_ExecuteCommandMenu(m_pTreeData->m_vectItem, m_pTreeData->m_vectItem.size(),m_pTreeData->g_pMalloc, "copy");

				


			}
			break;

		case ID_LISTE_PASTE:
			{
				m_pTreeData->iPauseMonitor = 1;
				if(OleGetClipboard(&m_pTreeData->pDataObj) == S_OK)
				{
					STGMEDIUM medium;
					FORMATETC fmte = {(CLIPFORMAT)CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
					HRESULT hres = m_pTreeData->pDataObj->GetData(&fmte, &medium);
					if (SUCCEEDED(hres))
					{
						HDROP m_hDrop = (HDROP)GlobalLock(medium.hGlobal);
						if (m_hDrop)
						{
							int iFile = 0xFFFFFFFF;
							int iNbFile;
							char * cTemp = NULL;
						    
							iNbFile = DragQueryFile(m_hDrop,iFile,cTemp,0);

							m_pTreeData->m_IconeFileVector.clear();

							for(int j = 0;j < iNbFile;j++)
							{
								CIconeFile m_IconeFileTemp;
								DragQueryFile(m_hDrop,j,m_IconeFileTemp.m_stgFullPathName,MAX_PATH);
								m_IconeFileTemp.lNumIcone = 0;
								m_pTreeData->m_IconeFileVector.push_back(m_IconeFileTemp);
							}

							//Génération d'un vecteur de fichier à copier
							//Appel de la fonction de copie
							DeplacerListImage(0,m_pTreeData->cDirectory);
							
							GlobalUnlock(medium.hGlobal);
						}
						else
						{
						   hres = E_UNEXPECTED;
						}
						ReleaseStgMedium(&medium);

						Actualiser();
					}
					return hres;
				}
			}
			break;

		case IDM_NODRAGGING:
			SendMessage(GetParent(),WM_KEYDOWN,VK_ESCAPE,0);	
			//m_pTreeData->bDragging = lParam;
			break;


		case ID_RECUP_PTATTRIBUT:
		case ID_OUTILS_MISEENPAPIERPEINT_ETIRER:
		case ID_OUTILS_MISEENPAPIERPEINT_MOSAIQUE:
		case ID_OUTILS_MISEENPAPIERPEINT_CENTRER:
			SendMessage(GetParent(),message,wParam,lParam);
			break;

		case ID_AFFICHAGE_AFFICHERLESDIMENSIONSDESIMAGES:
			{
				if(m_pTreeData->iAffichageDimension == 1)
					m_pTreeData->iAffichageDimension = 0;
				else
					m_pTreeData->iAffichageDimension = 1;

				//SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(ID_AFFICHAGE_ACTUALISER,0),0);

			}
			break;

		case ID_LISTE_COMPRESSERENZIP:
			{
				if(m_pTreeData->cList->IsWindowVisible() && m_pTreeData->iTypeData == 0)
				{
					char lpPathName[MAX_PATH];
					char TempName[MAX_PATH];
					char lpDefaultName[] = "monfichierzip";
					int iIndex;
					CFiles m_CFiles;
					CSADirRead m_CSADirRead;
					SAFileVector::const_iterator fit;
					SAFileVector files;
					CObjet m_cObjet;

					//Récupération du chemin par défaut
					StringCchPrintf(lpPathName,MAX_PATH,"%s\\%s.zip",m_pTreeData->cDirectory,lpDefaultName);
					m_CFiles.Create(lpPathName);

					// Ok, now we create a drag-image for all selected items
					int iPos = ListView_GetNextItem(m_pTreeData->cList->m_hWnd, -1, LVNI_SELECTED);
					while (iPos != -1) 
					{
						m_pTreeData->cList->List_GetItemText(iPos, lpPathName, MAX_PATH);

						m_CSADirRead.m_iNbNiveaux = 1;
						m_CSADirRead.GetDirs(lpPathName);
						m_CSADirRead.m_bRecurse = true;
						m_CSADirRead.GetDirs(lpPathName);
						m_CSADirRead.GetFiles("*.*");
						files = m_CSADirRead.Files();

						if(files.size() > 0)
						{
							for (fit = files.begin(); fit!=files.end(); fit++)
							{
								m_CFiles.AddFileByName((*fit).m_sName.c_str(),1,true,true);
							}
						}
						else
							m_CFiles.AddFileByName(lpPathName,1,false,true);

						
						iPos = ListView_GetNextItem(m_pTreeData->cList->m_hWnd, iPos, LVNI_SELECTED);
					}


					m_CFiles.Close();

					StringCchPrintf(TempName,MAX_PATH,"%s.zip",lpDefaultName);
					StringCchPrintf(lpPathName,MAX_PATH,"%s\\%s.zip",m_pTreeData->cDirectory,lpDefaultName);

					iIndex = m_pTreeData->cList->AddItem(lpPathName);

					ListView_EditLabel(m_pTreeData->cList->m_hWnd,iIndex);	
				}
			}
			break;

		case ID_LISTE_DCOMPRESSERICI:
			{
				if(m_pTreeData->cList->IsWindowVisible() && m_pTreeData->iTypeData == 0)
				{
					char szPath[MAX_PATH];
					int iNbFiles;
					CFiles m_CFiles;

					iNbFiles=SendMessage(m_pTreeData->cList->m_hWnd,LVM_GETNEXTITEM,-1,LVNI_FOCUSED);

					m_pTreeData->cList->List_GetItemText(iNbFiles, szPath, MAX_PATH);

					m_CFiles.Open(szPath);
					iNbFiles = m_CFiles.getNbFiles();

					for(int i = 0; i < iNbFiles;i++)
					{
						CFichierZip m_CFichierZip;
						string m_stgFilename;
						string m_stgname;
						m_CFichierZip = m_CFiles.ExtractFile(i);
						m_CFichierZip.Extract(m_pTreeData->cDirectory,&m_stgFilename);

					}

					m_CFiles.Close();
					ListView_DeleteAllItems(m_pTreeData->cList->m_hWnd);
					m_pTreeData->cList->List_DisplayFolder(m_pTreeData->cDirectory);
				}

			}
			break;

		case ID_LISTE_DCOMPRESSERVERS:
			{
				if(m_pTreeData->cList->IsWindowVisible() && m_pTreeData->iTypeData == 0)
				{
					char szPath[MAX_PATH];
					int iNbFiles;
					CFiles m_CFiles;
					char sPath[MAX_PATH];
					CObjet m_cObjet;

					StringCchCopy(sPath,MAX_PATH,"c:");

					if(m_cObjet.GetDirectory(hWnd, sPath))
					{
						iNbFiles=SendMessage(m_pTreeData->cList->m_hWnd,LVM_GETNEXTITEM,-1,LVNI_FOCUSED);
						
						m_pTreeData->cList->List_GetItemText(iNbFiles, szPath, MAX_PATH);

						m_CFiles.Open(szPath);
						iNbFiles = m_CFiles.getNbFiles();

						for(int i = 0; i < iNbFiles;i++)
						{
							CFichierZip m_CFichierZip;
							string m_stgFilename;
							string m_stgname;
							m_CFichierZip = m_CFiles.ExtractFile(i);
							m_CFichierZip.Extract(sPath,&m_stgFilename);

						}
						m_CFiles.Close();
					}
				}

			}
			break;

		case ID_LISTE_NOUVEAU_CRERUNFICHIERZIP:
			{
				if(m_pTreeData->cList->IsWindowVisible() && m_pTreeData->iTypeData == 0)
				{
					char lpPathName[MAX_PATH];
					char lpDefaultName[] = "monfichierzip";
					int iIndex;
					CFiles m_CFiles;
					CObjet m_cObjet;

					//Récupération du chemin par défaut
					StringCchPrintf(lpPathName,MAX_PATH,"%s\\%s.zip",m_pTreeData->cDirectory,lpDefaultName);
					m_CFiles.Create(lpPathName);
					m_CFiles.Close();

					iIndex = m_pTreeData->cList->AddItem(lpPathName);

					ListView_EditLabel(m_pTreeData->cList->m_hWnd,iIndex);
				}
			}
			break;

		case ID_LISTE_CRERUNRPERTOIRE:
			{
				if(m_pTreeData->cList->IsWindowVisible() && m_pTreeData->iTypeData == 0)
				{

					//Création d'un répertoire
					int i = 0;
					char lpPathName[MAX_PATH];
					int iIndex;
					char lpDefaultName[] = "nouveau dossier";
					CObjet m_cObjet;
					//Récupération du chemin par défaut
					do
					{
						i++;
						StringCchPrintf(lpPathName,MAX_PATH,"%s\\%s %d",m_pTreeData->cDirectory,lpDefaultName,i);
					}while(!CreateDirectory(lpPathName,NULL));

					iIndex = m_pTreeData->cList->AddItem(lpPathName);

					m_pTreeData->iCreerRepertoire = 1;
					ListView_EditLabel(m_pTreeData->cList->m_hWnd,iIndex);


				}
			}

			break;

		//Actualisation de l'affichage
		case ID_AFFICHAGE_ACTUALISER:
			{
				Actualiser(lParam);
			}
			break;

		case ID_AFFICHAGE_REGENERERLESMINIATURES:
			{
				if(m_pTreeData->m_ListPreview->IsWindowVisible())
				{
					m_pTreeData->iDemarre = 1;
					m_pTreeData->iAffichageAfterMiniature = 7;
					if(m_pTreeData->m_ListPreview->stopGenThumbnail())
					{
						Sleep(1000);
						return 0;
					}

					switch(m_pTreeData->iTypeData)
					{
						case 0:
							//ListView_DeleteAllItems(m_pTreeData->m_ListPreview->m_hWnd);
							m_pTreeData->m_ListPreview->InitList(m_pTreeData->cDirectory,"-1");
							break;
						case 1:
							//ListView_DeleteAllItems(m_pTreeData->m_ListPreview->m_hWnd);
							m_pTreeData->m_ListPreview->InitList(m_pTreeData->m_IconeFileVectorAlbum,"-1");
							break;

						case 2:
							//ListView_DeleteAllItems(m_pTreeData->m_ListPreview->m_hWnd);
							m_pTreeData->m_ListPreview->InitList(m_pTreeData->m_IconeFileVectorAlbum,"-1");
							break;
					}
				}
			}
			break;

		case ID_LISTE_AFFICHAGEIMAGE:
		case ID_ALBUM_AFFICHAGEDELIMAGE:
			{
				int iPos = ListView_GetNextItem(m_pTreeData->cList->m_hWnd, -1, LVNI_SELECTED);
				ProcessNotifyDoubleClick(iPos);
			}
			break;

		case ID_ALBUM_COPIERVERS:
			{
				SelectListImage(0);
			}
			break;

		case ID_LISTE_COPIER:
			{
				SelectListImage(0);
			}
			break;

		case ID_LISTE_DPLACERVERS:
			{
				SelectListImage(1);
			}
			break;

		case ID_LISTE_AJOUTSFICHIERSDANSCATALOGUE:
			{

				m_pTreeData->m_IconeFileVector.clear();

				if(m_pTreeData->cList->IsWindowVisible() && m_pTreeData->iTypeData == 0)
				{
					SelectListImage();
				}
				else
				{
					return 0;
				}
				SendMessage(GetParent(),message, wParam, (LPARAM)&m_pTreeData->m_IconeFileVector);
			}
			break;

		case IDM_SAVEALBUMTHUMBMAIL:
			SendMessage(GetParent(),message, wParam, lParam);
			break;


		case ID_LISTE_GESTIONSDESATTRIBUTS:
			{
				char szFileNameTemp[MAX_PATH];
				int iPos;

				if(m_pTreeData->m_ListPreview->IsWindowVisible())
				{
					//iPos = ListView_GetNextItem(m_pTreeData->cList->m_hWnd, -1, LVNI_SELECTED);
					m_pTreeData->m_ListPreview->List_GetItemText(m_pTreeData->iNumItem, szFileNameTemp, MAX_PATH);
				}
				else
				{
					iPos = ListView_GetNextItem(m_pTreeData->cList->m_hWnd, -1, LVNI_SELECTED);
					m_pTreeData->cList->List_GetItemText(iPos, szFileNameTemp, MAX_PATH);
				}
				SendMessage(GetParent(), message, wParam, (LPARAM)szFileNameTemp);

			}
			break;

		case IDM_STOPWORKING:
			{
				m_pTreeData->bDragging = FALSE;
				ImageList_DragLeave(m_pTreeData->cList->m_hWnd);
				ImageList_EndDrag();
				ImageList_Destroy(m_pTreeData->hDragImageList);
				ReleaseCapture();
				SendMessage(hWnd2,WM_SETREDRAW,1,0);
			}
			break;

		case ID_EDITION_SELECTALLFILES:
			{
				ListView_SetItemState(m_pTreeData->cList->m_hWnd, -1, LVIS_SELECTED | LVIS_DROPHILITED  , LVIS_SELECTED);
				//SetFocus(m_pTreeData->cList->m_hWnd);
			}
			break;

			/*
		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_NORMAL:
			m_pTreeData->m_Preview->SetQuality(0);
			break;
		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BESSEL:
			m_pTreeData->m_Preview->SetQuality(BESSELFILTER);
			break;
		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BICUBIC:
			m_pTreeData->m_Preview->SetQuality(CUBICFILTER);
			break;
		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BILINEAR:
			m_pTreeData->m_Preview->SetQuality(BILINEARFILTER);
			break;
		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BLACKMAN:
			m_pTreeData->m_Preview->SetQuality(BLACKMANFILTER);
			break;
		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BLACKMANBESSEL:
			m_pTreeData->m_Preview->SetQuality(BLACKMANBESSELFILTER);
			break;
		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BLACKMANSINC:
			m_pTreeData->m_Preview->SetQuality(BLACKMANSINCFILTER);
			break;
		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BOX:
			m_pTreeData->m_Preview->SetQuality(BOXFILTER);
			break;
		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_CATROM:
			m_pTreeData->m_Preview->SetQuality(CATROMFILTER);
			break;
		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_GAUSSIAN:
			m_pTreeData->m_Preview->SetQuality(GAUSSIANFILTER);
			break;
		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_HAMMING:
			m_pTreeData->m_Preview->SetQuality(HAMMINGFILTER);
			break;
		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_HANNING:
			m_pTreeData->m_Preview->SetQuality(HANNINGFILTER);
			break;
		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_HERMITE:
			m_pTreeData->m_Preview->SetQuality(HERMITEFILTER);
			break;
		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_LANCZOS:
			m_pTreeData->m_Preview->SetQuality(LANCZOSFILTER);
			break;
		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_MITCHELL:
			m_pTreeData->m_Preview->SetQuality(MITCHELLFILTER);
			break;
		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_QUADRATIC:
			m_pTreeData->m_Preview->SetQuality(QUADRATICFILTER);
			break;
		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_SINC:
			m_pTreeData->m_Preview->SetQuality(SINCFILTER);
			break;
		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_TRIANGLE:
			m_pTreeData->m_Preview->SetQuality(TRIANGLEFILTER);
			break;*/

		case IDM_SAVEBMPFILE:
			m_pTreeData->m_Preview->SaveBitmap((char *)lParam);
			break;

		case ID_AFFICHAGE_AFFICHERUNIQUEMENTLESIMAGES:
			{
				m_pTreeData->cList->DefineAfficheImage(wmEvent);
				m_pTreeData->cList->List_DisplayFolder(m_pTreeData->cDirectory);
			}
			break;


		case IDM_UPDATESTATUS:
			{
				StringCchPrintf(cInfos,255,"%dx%d",LOWORD(lParam), HIWORD(lParam));
				m_TreeWindStatusBar->UpdateStatusBar(cInfos,3,0);
			}
			break;

		case IDM_ALBUMIMAGELIST:
			{
				int iNumAlbum;
				iNumAlbum = (int)lParam;
				StringCchPrintf(cNumAlbum,5,"%d",iNumAlbum);
			}
			break;

		case IDM_ALBUMNAME:
			{
				//Récupération d'un pointeur sur un vecteur Icone
				m_pTreeData->iTypeData = 1;
				m_pTreeData->m_IconeFileVectorAlbum = (IconeFileVector *)lParam;
			}
			break;

		case IDM_CATALOGUENAME:
			{
				//Récupération d'un pointeur sur un vecteur Icone
				m_pTreeData->iTypeData = 2;
				m_pTreeData->m_IconeFileVectorAlbum = (IconeFileVector *)lParam;
			}
			break;

		case IDM_FILENAME:
			StringCchCopy(m_pTreeData->cFile,MAX_PATH,(const char *)lParam);
			break;
		
		case IDM_DIRNAME:
			{
				m_pTreeData->iTypeData = 0;
				StringCchCopy(m_pTreeData->cDirectory,MAX_PATH,(const char *)lParam);
			}
			break;

		case ID_LISTE_SUPPRIMER:
		case ID_ALBUM_SUPPRIMER:
		case IDM_DELFILE:
			{
				if(m_pTreeData->iTypeData == 0)
				{
					if(m_pTreeData->m_ListPreview->IsWindowVisible())
						DeleteListFile(m_pTreeData->m_ListPreview->m_hWnd);
					else
						DeleteListFile(m_pTreeData->cList->m_hWnd);


				}
				else
				{
					//static VECTITEM m_vectCopy = m_pTreeData->m_vectItem;
					if(m_pTreeData->m_ListPreview->IsWindowVisible())
						SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(ID_ALBUM_SUPPRIMER,m_pTreeData->m_ListPreview->m_hWnd),(LPARAM)&m_pTreeData->m_vectItem);
					else
						SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(ID_ALBUM_SUPPRIMER,m_pTreeData->cList->m_hWnd),(LPARAM)&m_pTreeData->m_vectItem);
				}
				SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(ID_LISTE_CRERUNRPERTOIRE,0),0);
			}
			break;

		case IDM_FINGENTHUMBNAIL:
			{
				FinGenThumbnail();
			}
			break;

		case IDM_LARGEICON:
			{
				m_pTreeData->iAffichageAfterMiniature = 1;
				if(m_pTreeData->m_ListPreview->IsWindowVisible())
				{
					m_pTreeData->iDemarre = 1;
					if(m_pTreeData->m_ListPreview->stopGenThumbnail())
					{
						Sleep(1000);
						return 0;
					}
				}

					ShowStyleListe(LVS_ICON);
					m_pTreeData->iTypeAffichage = 0;
				//ShowStyleListe(LVS_ICON);
				//SendMessage(m_pTreeData->m_cPane->m_hWnd,WM_SIZE,0,1);
			}
			break;

		case IDM_SMALLICON:
			{
				m_pTreeData->iAffichageAfterMiniature = 2;
				if(m_pTreeData->m_ListPreview->IsWindowVisible())
				{
					m_pTreeData->iDemarre = 1;
					if(m_pTreeData->m_ListPreview->stopGenThumbnail())
					{
						Sleep(1000);
						return 0;
					}
				}

				ShowStyleListe(LVS_SMALLICON);
				m_pTreeData->iTypeAffichage = 0;
				
				//SendMessage(m_pTreeData->m_cPane->m_hWnd,WM_SIZE,0,1);
			}
			break;

		case IDM_REPORTVIEW:
			{
				m_pTreeData->iAffichageAfterMiniature = 3;
				if(m_pTreeData->m_ListPreview->IsWindowVisible())
				{
					m_pTreeData->iDemarre = 1;
					if(m_pTreeData->m_ListPreview->stopGenThumbnail())
					{
						Sleep(1000);
						return 0;
					}
				}

					ShowStyleListe(LVS_REPORT);
					m_pTreeData->iTypeAffichage = 0;
				//ShowStyleListe(LVS_REPORT);
				//SendMessage(m_pTreeData->m_cPane->m_hWnd,WM_SIZE,0,1);
			}
			break;

		case IDM_LISTVIEW:
			{
				m_pTreeData->iAffichageAfterMiniature = 4;
				if(m_pTreeData->m_ListPreview->IsWindowVisible())
				{
					m_pTreeData->iDemarre = 1;
					if(m_pTreeData->m_ListPreview->stopGenThumbnail())
					{
						Sleep(1000);
						return 0;
					}
				}
				m_pTreeData->iTypeAffichage = 0;
				ShowStyleListe(LVS_LIST);
				//SendMessage(m_pTreeData->m_cPane->m_hWnd,WM_SIZE,0,1);
			}
			break;

		case IDM_THUMBMAIL:
			{
				m_pTreeData->iAffichageAfterMiniature = 5;
				m_pTreeData->iTypeAffichage = 1;
				if(m_pTreeData->m_ListPreview->IsWindowVisible())
				{
					m_pTreeData->iDemarre = 1;
					if(m_pTreeData->m_ListPreview->stopGenThumbnail())
					{
						Sleep(1000);
						return 0;
					}
				}
				m_pTreeData->iTypeAffichage = 1;
				if(!m_pTreeData->m_ListPreview->IsWindowVisible())
				{
					m_pTreeData->m_vectItem.clear();
					m_pTreeData->m_vectItem.push_back(0);
					m_pTreeData->cList->ShowWindow(SW_HIDE);
					m_pTreeData->m_ListPreview->ShowWindow(SW_SHOW);

					this->SetSplitWindow(m_pTreeData->m_cPane->m_hWnd,m_pTreeData->m_ListPreview->m_hWnd);

					m_pTreeData->m_ListPreview->DefineStatusBar(this->m_TreeWindStatusBar);
					
					switch(m_pTreeData->iTypeData)
					{
						case 0:
							m_pTreeData->m_ListPreview->InitList(m_pTreeData->cDirectory);
							break;
						case 1:
							m_pTreeData->m_ListPreview->InitList(m_pTreeData->m_IconeFileVectorAlbum,cNumAlbum);
							break;
						case 2:
							m_pTreeData->m_ListPreview->InitList(m_pTreeData->m_IconeFileVectorAlbum,cNumAlbum);
							break;
					}

					
					this->RedrawSplitter();
					

					if(m_pTreeData->m_cPane->IsWindowVisible())
					{
						m_pTreeData->m_cPane->ShowWindow(SW_HIDE);
						m_pTreeData->m_cPane->ShowWindow(SW_SHOW);
					}

					//this->OnPaint(m_hWnd,WM_PAINT,0,0);
				}
			}
			break;

		case ID_AFFICHAGE_CONTROLE_INFOSFICHIER:
			{
				if(wmEvent != 99)
				{
					if(m_pTreeData->iInfosFichier == 1)
						m_pTreeData->iInfosFichier = 0;
					else
						m_pTreeData->iInfosFichier = 1;

					SendMessage(m_pTreeData->m_Preview->m_hWnd,message, wParam, lParam);
				}
				else
				{
					CIconeFile IconeFileTemp = m_pTreeData->m_IconeFileVectorAlbum->at(m_pTreeData->iNumSelectedItem);
					SendMessage(m_pTreeData->m_Preview->m_hWnd,message, wParam, (LPARAM)&IconeFileTemp);
				}
			}
			break;

		case IDM_AFFICHAGE_CONTROL_PREVIEWAREA:
			{
				if(!m_pTreeData->m_cPane->IsWindowVisible())
				{

					CheckMenuItem(GetMenu(GetParent()),IDM_AFFICHAGE_CONTROL_PREVIEWAREA,MF_CHECKED);
					m_pTreeData->m_cPane->ShowWindow(SW_SHOW);
				}
				else
				{
					CheckMenuItem(GetMenu(GetParent()),IDM_AFFICHAGE_CONTROL_PREVIEWAREA,MF_UNCHECKED);
					m_pTreeData->m_cPane->ShowWindow(SW_HIDE);

				}

				this->RedrawSplitter();

			}
			break;

		case IDM_INITLIST:
			{
				//static int iTypeAffichage = 0;

				//::MessageBox(NULL,"IDM_INITLIST","Infos",0);

				m_pTreeData->iPauseMonitor = 1;

				StringCchCopy(m_pTreeData->cDirectory,MAX_PATH,(const char *)lParam);

				//Test si répertoire à afficher

				if(m_pTreeData->hThread != NULL)
				{
					PostThreadMessage(m_pTreeData->dwThreadId, WM_NULL, 0, 0);
				}
				else
				{
					m_pTreeData->hThread = CreateThread(NULL, cuStackSize,
							MonitorDirectoryProc, this, 0, &m_pTreeData->dwThreadId);

					SetThreadPriority(m_pTreeData->hThread, THREAD_PRIORITY_NORMAL);
				}

		
				if(_stricmp(m_pTreeData->cTempDir, m_pTreeData->cDirectory) != 0 || m_pTreeData->iTypeData == 1)
				{
					m_pTreeData->iTypeData = 0;
					
					if(!m_pTreeData->m_ListPreview->IsWindowVisible())
					{
						m_pTreeData->iPauseMonitor = 1;
						m_pTreeData->cList->List_DisplayFolder(m_pTreeData->cDirectory);
						m_pTreeData->iTypeAffichage = 0;
					}
					else
					{
						
						if(m_pTreeData->iTypeAffichage == 1)
						{
							m_pTreeData->iDemarre = 1;
							m_pTreeData->iAffichageAfterMiniature = 6; //3
							if(m_pTreeData->m_ListPreview->stopGenThumbnail())
							{
								m_pTreeData->m_Preview->SetDirName(m_pTreeData->cDirectory);
								StringCchCopy(m_pTreeData->cTempDir,MAX_PATH,m_pTreeData->cDirectory);
								Sleep(1000);
								return 0;
							}
							else
							{
								m_pTreeData->m_ListPreview->SauverListe();
								m_pTreeData->m_ListPreview->InitList(m_pTreeData->cDirectory);
							}
						}
						else
						{
							if(m_pTreeData->m_ListPreview->stopGenThumbnail() == 0)
							{
								m_pTreeData->m_ListPreview->SauverListe();
								m_pTreeData->m_ListPreview->InitList(m_pTreeData->cDirectory);
							}
							else
							{
								m_pTreeData->iDemarre = 1;
								m_pTreeData->iAffichageAfterMiniature = 10; //3
								Sleep(1000);
								return 0;
							}
						}
						m_pTreeData->iTypeAffichage = 0;
						m_pTreeData->iPauseMonitor = 1;
					}
					m_pTreeData->m_Preview->SetDirName(m_pTreeData->cDirectory);
				}
				StringCchCopy(m_pTreeData->cTempDir,MAX_PATH,m_pTreeData->cDirectory);
			}
			break;

		case IDM_GETHWNDPRGBAR:
			{
				SendMessage(GetParent(),message, wParam, lParam);
				break;
			}

		case IDM_INITALBUM:
			{
				m_pTreeData->iTypeData = 1;

				m_pTreeData->m_IconeFileVectorAlbum = (IconeFileVector *)lParam;

				if(m_pTreeData->hThread != NULL)
				{
					PostThreadMessage(m_pTreeData->dwThreadId, WM_NULL, 0, 0);
				}

				if(!m_pTreeData->m_ListPreview->IsWindowVisible())
					m_pTreeData->cList->List_DisplayAlbum(m_pTreeData->m_IconeFileVectorAlbum);
				else
				{
					//On vérifie que le thread est terminé
					m_pTreeData->iDemarre = 1;
					m_pTreeData->iAffichageAfterMiniature = 9; //3
					if(m_pTreeData->m_ListPreview->stopGenThumbnail() == 0)
						m_pTreeData->m_ListPreview->InitList(m_pTreeData->m_IconeFileVectorAlbum, cNumAlbum);
					else
						Sleep(1000);
				}
			}
			break;

		case IDM_INITCATALOGUE:
			{
				m_pTreeData->iTypeData = 2;

				m_pTreeData->m_IconeFileVectorAlbum = (IconeFileVector *)lParam;

				if(m_pTreeData->hThread != NULL)
				{
					PostThreadMessage(m_pTreeData->dwThreadId, WM_NULL, 0, 0);
				}

				if(!m_pTreeData->m_ListPreview->IsWindowVisible())
				{
					if(m_pTreeData->m_IconeFileVectorAlbum != NULL)
						m_pTreeData->cList->List_DisplayAlbum(m_pTreeData->m_IconeFileVectorAlbum);
				}
				else
				{
					m_pTreeData->iDemarre = 1;
					m_pTreeData->iAffichageAfterMiniature = 9; //3
					if(m_pTreeData->m_ListPreview->stopGenThumbnail() == 0)
						m_pTreeData->m_ListPreview->InitList(m_pTreeData->m_IconeFileVectorAlbum, cNumAlbum);
					else
						Sleep(1000);
				}
			}
			break;

		case IDM_FINMONITOR:
			{
				CloseHandle(m_pTreeData->hThread);
				m_pTreeData->hThread = NULL;

				if(m_pTreeData->iTypeData == 0)
				{				
					m_pTreeData->hThread = CreateThread(NULL, cuStackSize,
							MonitorDirectoryProc, this, 0, &m_pTreeData->dwThreadId);

					SetThreadPriority(m_pTreeData->hThread, THREAD_PRIORITY_NORMAL);
				}

			}
			break;

		case ID_EDITION_RECHERCHERUNFICHIER:
			{
				if(!m_pTreeData->m_ListPreview->IsWindowVisible())
				{
					CDialogFindProc * m_cDialogFind = new CDialogFindProc();
					m_cDialogFind->SetParam(m_pTreeData);
					m_cDialogFind->CreateDlg(hInstance,(LPCTSTR)IDD_DLGLISTFIND,m_hWnd);
					//DialogBoxParam(hInstance,(LPCTSTR)IDD_DLGLISTFIND,m_hWnd,(DLGPROC)DialogFindProc,(LPARAM)m_pTreeData);
					delete m_cDialogFind;
				}
			}
			break;

		default:
		   return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CTreeWindow::OnContextMenu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CObjet m_cObjet;
	//Affichage d'un sous-menu
	LVHITTESTINFO  lvhti;
	POINT ptScreen;
	ptScreen.x = GET_X_LPARAM(lParam);
	ptScreen.y = GET_Y_LPARAM(lParam);
	int iCommand = -1;

	HWND hWndList;
	if(m_pTreeData->m_ListPreview->IsWindowVisible())
		hWndList = m_pTreeData->m_ListPreview->m_hWnd;
	else
		hWndList = m_pTreeData->cList->m_hWnd;

	if(m_pTreeData->m_vectItem.size() <= 1)
	{
		m_pTreeData->m_vectItem.clear();
		m_pTreeData->iNumItem = ListView_GetNextItem(hWndList,-1,LVIS_SELECTED);
		m_pTreeData->m_vectItem.push_back(m_pTreeData->iNumItem);

	}


	//HWND hWndList;

	if(m_pTreeData->m_ListPreview->IsWindowVisible())
	{
		lvhti.pt = ptScreen;
		//ScreenToClient(m_pTreeData->m_ListPreview->m_hWnd, &lvhti.pt);
		ScreenToClient(hWnd, &lvhti.pt);

		lvhti.flags = LVHT_NOWHERE;

		ListView_HitTest(m_pTreeData->m_ListPreview->m_hWnd, &lvhti);

		if(LVHT_ONITEM & lvhti.flags)
		{
			char * cTemp = new char[MAX_PATH];
			if (cTemp == NULL)
			{
				CObjet::ErreurInformation();
			}
			ListView_GetItemText(m_pTreeData->m_ListPreview->m_hWnd,m_pTreeData->iNumItem,0,cTemp,MAX_PATH);

			if(m_pTreeData->m_vectItem.size() == 1 && m_cObjet.TestImageFormat(cTemp) == 0)
			{
				iCommand = m_pTreeData->m_ListPreview->List_DoItemMenu(m_pTreeData->m_vectItem, m_pTreeData->m_vectItem.size(), &ptScreen,m_pTreeData->g_pcm2,m_pTreeData->g_pMalloc);
			}
			else
			{
				//lvhti.pt.x = ptScreen.x;
				switch(m_pTreeData->iTypeData)
				{
					case 0:
						m_cObjet.DisplayContextMenu(hWnd,lvhti.pt, hInstance, (LPCSTR)IDM_MENULISTE,0);
						break;

					case 1:
						m_cObjet.DisplayContextMenu(hWnd,lvhti.pt, hInstance, (LPCSTR)IDM_MENULISTE,1);
						break;

					case 2:
						m_cObjet.DisplayContextMenu(hWnd,lvhti.pt, hInstance, (LPCSTR)IDM_MENULISTE,4);
						break;
				}						
			}
			delete[] cTemp;
		}
		else
		{
			//lvhti.pt.x = ptScreen.x;
			m_cObjet.DisplayContextMenu(hWnd,lvhti.pt, hInstance, (LPCSTR)IDM_MENULISTE,2);
		}
	}
	else
	{
		lvhti.pt = ptScreen;
		ScreenToClient(hWnd, &lvhti.pt);

		lvhti.flags = LVHT_NOWHERE;

		ListView_HitTest(m_pTreeData->cList->m_hWnd, &lvhti);

		if(LVHT_ONITEM & lvhti.flags)
		{
			char * cTemp = new char[MAX_PATH];

			ListView_GetItemText(m_pTreeData->cList->m_hWnd,m_pTreeData->iNumItem,0,cTemp,MAX_PATH);

			if(m_cObjet.TestImageFormat(cTemp) == 0 && m_cObjet.TestVideoFormat(cTemp) == 0)
			{
				iCommand = m_pTreeData->cList->List_DoItemMenu(m_pTreeData->m_vectItem, m_pTreeData->m_vectItem.size(), &ptScreen,m_pTreeData->g_pcm2,m_pTreeData->g_pMalloc);
			}
			else
			{
				//lvhti.pt.x = ptScreen.x;
				switch(m_pTreeData->iTypeData)
				{
					case 0:
						m_cObjet.DisplayContextMenu(hWnd,lvhti.pt, hInstance, (LPCSTR)IDM_MENULISTE,0);
						break;

					case 1:
						m_cObjet.DisplayContextMenu(hWnd,lvhti.pt, hInstance, (LPCSTR)IDM_MENULISTE,1);
						break;

					case 2:
						m_cObjet.DisplayContextMenu(hWnd,lvhti.pt, hInstance, (LPCSTR)IDM_MENULISTE,4);
						break;
				}						
			}
			delete[] cTemp;
		}
		else
		{
			/*
			RECT rc;
			if(m_pTreeData->cList->IsWindowVisible())
				GetWindowRect(m_pTreeData->cList->m_hWnd,&rc);
			else
				GetWindowRect(m_pTreeData->m_ListPreview->m_hWnd,&rc);

			if(ptScreen.x > rc.left)
			{
				lvhti.pt.x = ptScreen.x;
				m_cObjet.DisplayContextMenu(hWnd,lvhti.pt, hInstance, (LPCSTR)IDM_MENULISTE,2);
			}*/
			m_cObjet.DisplayContextMenu(hWnd,lvhti.pt, hInstance, (LPCSTR)IDM_MENULISTE,2);
		}
	}

	//SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(ID_AFFICHAGE_ACTUALISER,0),0);

	//SendMessage(hWnd,WM_PAINT,0,0);

	if(iCommand == 18)
	{
		Actualiser();
		SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(ID_LISTE_CRERUNRPERTOIRE,0),0);
	}

	return 0;
}

LRESULT CTreeWindow::OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char Temp[255];
	//Déclaration des variables
	m_pTreeData->cList = new CList();
	m_pTreeData->m_ListPreview = new CListPreview();
	m_pTreeData->m_Preview = new CPreview();
	m_pTreeData->m_cPane = new CPane();

	m_pTreeData->cFileTemp[0] = '\0';
	StringCchCopy(m_pTreeData->cDirectory,MAX_PATH,"c:");

	m_pTreeData->iInfosFichier = 1;

	CreatePaneWindow(hWnd);

	////////////////////////////////////////////////////
	//Création de la fenêtre de prévisualisation
	////////////////////////////////////////////////////

	m_pTreeData->cList->CreateListView(hWnd,hInstance,"ListWindow");
	m_pTreeData->cList->SetParent(hWnd);

	LoadString(hInstance,IDS_PREVIEW,Temp,255);
	m_pTreeData->m_cPane->SetTitle(Temp);

	CreatePreview(m_pTreeData->m_cPane->m_hWnd);

	m_pTreeData->m_Preview->SetParent(hWnd);
	m_pTreeData->m_cPane->SetWindow(m_pTreeData->m_Preview->m_hWnd);
	m_pTreeData->m_ListPreview->DefineStatusBar(m_TreeWindStatusBar);
	m_pTreeData->m_ListPreview->CreateListView(hWnd,hInstance);
	m_pTreeData->m_ListPreview->SetParent(hWnd);

	m_pTreeData->cList->DefineStatusBar(m_TreeWindStatusBar);

	m_pTreeData->cList->InitListViewImageLists();
	//m_pTreeData->cList->List_DisplayFolder("c:");  

	RECT rc;
	GetClientRect(hWnd,&rc);
	this->SetHorizontalSplit(0);

	int _iPreview = 0;
	int _iList = 0;
	
	if(m_pTreeData->LoadWindowPosition(_iPreview, _iList) != -1)
		this->SetSplitPos(_iPreview);
	else
		this->SetSplitPos(rc.right / 3);


	this->SetSplitWindow(m_pTreeData->m_cPane->m_hWnd,m_pTreeData->cList->m_hWnd);


	m_pTreeData->iAffichageDimension = 1;

	m_pTreeData->m_bCaptureList = 0;
	m_pTreeData->iMarquee = 0;

 	SHGetMalloc(&m_pTreeData->g_pMalloc);

	return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CTreeWindow::OnKeyDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i = 0;
	switch (wParam) 
	{
		case VK_ESCAPE:
			//Stop tout travail en cours
			SendMessage(hWnd2,WM_SETREDRAW,1,0);
			SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_STOPWORKING,0),0);
			break;

		default:
			i = 1;
			return 1;
			break;

	}

	return 0;
}

LRESULT CTreeWindow::OnKillFocus(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CTreeWindow::OnDrawItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_pTreeData->g_pcm2)
	{
		m_pTreeData->g_pcm2->HandleMenuMsg(message, wParam, lParam);
	}

	return 0;
}

LRESULT CTreeWindow::OnMenuChar(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return OnDrawItem(hWnd, message, wParam, lParam);
}

LRESULT CTreeWindow::OnMeasureItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return OnDrawItem(hWnd, message, wParam, lParam);
}

LRESULT CTreeWindow::OnDestroy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	m_pTreeData->SaveWindowPosition();

	if(m_pTreeData->m_ListPreview != NULL)
	{
		m_pTreeData->m_ListPreview->SauverListe();
	}

	DestroyWindow(m_pTreeData->m_cPane->m_hWnd);
	DestroyWindow(m_pTreeData->cList->m_hWnd);
	DestroyWindow(m_pTreeData->m_ListPreview->m_hWnd);
	DestroyWindow(m_pTreeData->m_Preview->m_hWnd);

	if(m_pTreeData->hThread != NULL)
	{
		TerminateThread(m_pTreeData->hThread,1);
		CloseHandle(m_pTreeData->hThread);
		m_pTreeData->hThread = NULL;
	}

	m_pTreeData->g_pMalloc->Release();
	m_pTreeData->g_pMalloc = NULL;
	m_pTreeData->m_vectItem.clear();

	PostQuitMessage(0);

	return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CTreeWindow::OnLButtonUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_pTreeData->bDragging)
	{
		// End the drag-and-drop process
		m_pTreeData->bDragging = FALSE;
		ImageList_DragLeave(m_pTreeData->cList->m_hWnd);
		ImageList_EndDrag();
		ImageList_Destroy(m_pTreeData->hDragImageList);
		ReleaseCapture();
	}
	else
	{
		this->LButtonUp(hWnd,message,wParam,lParam);
		
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CTreeWindow::OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pTreeData->bDragging)
	{
		POINT p;
		p.x = LOWORD(lParam);
		p.y = HIWORD(lParam);
		ClientToScreen(FindWindow("Regards","Regards"), &p);
		ImageList_DragMove(p.x, p.y);
	}
	else
	{
		this->MouseMove(hWnd,message,wParam,lParam);
	}

	//m_cTooltip->TrackPosition();

	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void CTreeWindow::SetStatusBar(CStatusBar *m_StatusBar)
{
	m_TreeWindStatusBar = m_StatusBar;
	if(m_pTreeData->cList != NULL)
		m_pTreeData->cList->DefineStatusBar(m_StatusBar);

	if(m_pTreeData->m_ListPreview != NULL)
		m_pTreeData->m_ListPreview->DefineStatusBar(m_StatusBar);
}


int CTreeWindow::CreatePaneWindow(HWND hWnd)
{
	DWORD dwStyle = CS_HREDRAW | CS_VREDRAW;
	DWORD dwWndStyle = WS_CHILD | WS_VISIBLE;
	DWORD dwExStyle = 0;
	RECT rc = {0, 0, 0, 0};
	m_pTreeData->m_cPane->CreateEx(hWnd,hInstance,dwWndStyle,dwStyle,dwExStyle,"PaneWindow",0,0,IDC_SIZEWE,rc);

	return 0;
}
int CTreeWindow::CreatePreview(HWND hWnd)
{
	RECT rc = {0, 0, 0, 0};
	DWORD dwndStlyle = WS_CHILD | WS_VISIBLE;
	DWORD dStyle = CS_HREDRAW | CS_VREDRAW;
	DWORD dExStyle = WS_EX_CLIENTEDGE;
	m_pTreeData->m_Preview->SetBackgroundColor((HBRUSH)CreateSolidBrush(RGB( 230, 230, 230)));
	m_pTreeData->m_Preview->CreateEx(hWnd,hInstance,dwndStlyle,dStyle,dExStyle,"Preview",0,0,IDC_ARROW,rc);

	return 0;
}
