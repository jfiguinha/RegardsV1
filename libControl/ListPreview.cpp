// ListPreview.cpp: implementation of the CListPreview class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ListPreview.h"
#include ".\listpreviewdata.h"
#include <ItemInfo.h>
#include "LoadImage.h"
#include "statusbar.h"
#include <definerepmonitor.h>
#include <omp.h>
#include ".\miniaturegen.h"
#include ".\miniaturegendata.h"
#include "Objet.h"
#include "parameter.h"
#include <effet.h>

typedef std::vector<CMiniatureGen *> VectorMiniature;

void CListPreview::SetParent(HWND hWnd)
{
	hParent = hWnd;
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CListPreview::CListPreview()
{
	pimpl_ = new CListPreviewData();
	hThreadPreview = NULL;
	m_hEndThread = NULL;
	m_hProcessThread = NULL;
	m_hWaitThread = NULL;
	m_bShowFolder = true;
	m_hStopThread = NULL;
	dwThreadIdStop = 0;
	cAlbumThumbmail[0] = '\0';
	m_bDefineThumbnailSize = false;
}	

CListPreview::~CListPreview()
{
	delete pimpl_;

	//stopGenThumbnail();

	if(hThreadPreview != NULL)
	{
		TerminateThread(hThreadPreview,0);
		CloseHandle(hThreadPreview);
		hThreadPreview = NULL;
	}

	// fermeture dans handles
	if(m_hEndThread != NULL)
	{
		::CloseHandle(m_hEndThread);
		m_hEndThread = NULL;
	}

	if(m_hWaitThread != NULL)
	{
		::CloseHandle(m_hWaitThread);
		m_hWaitThread = NULL;
	}

	if(m_hProcessThread != NULL)
	{
		::CloseHandle(m_hProcessThread);
		m_hProcessThread = NULL;
	}

	m_IconeFileVector.clear();

}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

void CListPreview::DefineStatusBar(CStatusBar *m_StatusBar)
{
	m_ListStatusBar = m_StatusBar;
}


// CreateListView - creates a list-view control in report view. 
// Returns the handle to the new control if successful, or NULL 
// otherwise. 
// hWndParent - handle to the control's parent window. 
HWND CListPreview::CreateListView (HWND hwndParent, HINSTANCE hInst) 
{ 
	RECT rectClient;
	INITCOMMONCONTROLSEX icex;

	// Ensure that the common control DLL is loaded. 
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC  = ICC_LISTVIEW_CLASSES | ICC_COOL_CLASSES;
	InitCommonControlsEx(&icex); 

	GetClientRect(hwndParent,&rectClient);

	hParent = hwndParent;

	DWORD dwStyle =   WS_CHILD | LVS_SHAREIMAGELISTS |LVS_SHOWSELALWAYS | WS_BORDER | LVS_AUTOARRANGE | LVS_EDITLABELS ;

	m_hWnd = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_ACCEPTFILES,WC_LISTVIEW,"miniature",dwStyle,0,0,0, 0,hwndParent,NULL,hInstance,NULL);
	m_hWnd = m_hWnd;
	ListView_SetExtendedListViewStyle(m_hWnd, LVS_EX_ONECLICKACTIVATE );
	ListView_SetIconSpacing(m_hWnd, pimpl_->GetLargeur() + 20, pimpl_->GetHauteur() + 30);
	ListView_Arrange(m_hWnd,LVA_ALIGNTOP);

    LVCOLUMN lvColumn;
    CHAR szLabel[81];

    // Add columns
    lvColumn.mask = LVCF_TEXT|LVCF_WIDTH;
    lvColumn.cx = 125;
    lvColumn.pszText = szLabel;

    // Column 0
    //strcpy(szLabel,"Column 2");
	StringCchCopy(szLabel,81,"Column 2");
    ListView_InsertColumn(m_hWnd,0,&lvColumn);

    // Column 0
    //strcpy(szLabel,"Column 3");
	StringCchCopy(szLabel,81,"Column 3");
    ListView_InsertColumn(m_hWnd,1,&lvColumn);

    // Column 0
    //strcpy(szLabel,"Column 4");
	StringCchCopy(szLabel,81,"Column 4");
    ListView_InsertColumn(m_hWnd,2,&lvColumn);


    // Column 0
    //strcpy(szLabel,"Column 5");
	StringCchCopy(szLabel,81,"Column 5");
    ListView_InsertColumn(m_hWnd,3,&lvColumn);

	pimpl_->CreateHImageList(m_IconeFileVector);

	return m_hWnd;
}

///////////////////////////////////////////////////////////////////////
// Some code to create the list-view control.
///////////////////////////////////////////////////////////////////////

int CListPreview::InitList(const char * cDir,const char * Localisation)
{
	//Suppression de toutes les images

	bool m_bChargerIcone = true;
	m_bAfficheImage = true;
	int i = 0;

	//Terminer les miniatures
	StopListMiniatureHandle();

	cAlbumThumbmail[0] = '\0';
	StringCchCopy(cDirectory,MAX_PATH,cDir);

	m_IconeFileVector.erase(m_IconeFileVector.begin(),m_IconeFileVector.end());

	iAlbum = 0;

	if(pimpl_->GetAffichageVolee() == 0)
	{
		if(pimpl_->ChargeDatabaseFolder(cDir, Localisation,m_IconeFileVector) != -1)
			m_bChargerIcone = false;
	}

	if(m_bChargerIcone)
	{
		pimpl_->CreateFolderIcone();
	}

	ListView_SetImageList(m_hWnd, pimpl_->hImgThumbmail, LVSIL_NORMAL); 
	List_DisplayFolder(cDir);
	
	CreateListMiniatureHandle();

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////////
void CListPreview::StopListMiniatureHandle()
{

	HCURSOR  hCur = LoadCursor(NULL, IDC_WAIT);
	hCur = SetCursor(hCur);

	if(pimpl_->GetAffichageVolee() == 0)
	{
		if(hThreadPreview != NULL)
		{
			this->stopGenThumbnail();
		}
	}

	hCur = LoadCursor (NULL, IDC_SIZEWE);
	hCur = SetCursor(hCur);

}

ULONG __stdcall CListPreview::List_StopMiniature(LPVOID param)
{
	CListPreview * m_lpListData = (CListPreview *)param;

	CObjet::StartWait();

	// declenche la fin du thread
	//::SetEvent(m_lpListData->m_hEndThread);

	PostThreadMessage(m_lpListData->dwThreadIdPreview,WM_QUIT,0,0);
	//Sleep(1000);

	// attend que le thread soit terminé
	::WaitForSingleObject(m_lpListData->m_hWaitThread, 5000);

	// fermeture dans handles
	if(m_lpListData->m_hEndThread != NULL)
	{
		::CloseHandle(m_lpListData->m_hEndThread);
		m_lpListData->m_hEndThread = NULL;
	}

	if(m_lpListData->m_hWaitThread != NULL)
	{
		::CloseHandle(m_lpListData->m_hWaitThread);
		m_lpListData->m_hWaitThread = NULL;
	}

	if(m_lpListData->m_hProcessThread != NULL)
	{
		::CloseHandle(m_lpListData->m_hProcessThread);
		m_lpListData->m_hProcessThread = NULL;
	}

	//TerminateThread(hThreadPreview,0);
	CloseHandle(m_lpListData->hThreadPreview);
	m_lpListData->hThreadPreview = NULL;

	CObjet::EndWait();

	SendMessage(m_lpListData->hParent,WM_COMMAND,MAKEWPARAM(IDM_FINGENTHUMBNAIL,0),0);

	ExitThread(0);
	return 0;
}


///////////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////////
void CListPreview::CreateListMiniatureHandle()
{
	if(pimpl_->GetAffichageVolee() == 0)
	{
		if(ListView_GetItemCount(m_hWnd) > 0)
		{
			if(hThreadPreview == NULL)
			{
				// Creation des  events
				m_hEndThread = CreateEvent(0, TRUE, FALSE, 0);
				m_hWaitThread = CreateEvent(0, TRUE, FALSE, 0);
				m_hProcessThread = CreateEvent(0, TRUE, FALSE, 0);

				//Démarrage du thread de récupération des données
				hThreadPreview = (HANDLE)	CreateThread(NULL, cuStackSize,
							List_CreateMiniature, this, 0, &dwThreadIdPreview);
				SetThreadPriority(hThreadPreview, THREAD_PRIORITY_NORMAL);
			}
		}
		else
		{
			CloseHandle(hThreadPreview);
			hThreadPreview = NULL;
		}
	}
}

///////////////////////////////////////////////////////////////////////
// Some code to create the list-view control.
///////////////////////////////////////////////////////////////////////

int CListPreview::InitList(IconeFileVector * m_IconeFileVectorAlbum, const char * Localisation)
{

	bool m_bChargerIcone = true;
	m_bAfficheImage = true;
	
	StopListMiniatureHandle();

	//strcpy(cDirectory,"");
	cDirectory[0] ='\0';

	if(strcmp(Localisation,"-1") != 0)
		StringCchCopy(cAlbumThumbmail,MAX_PATH,Localisation);

	m_IconeFileVector.erase(m_IconeFileVector.begin(),m_IconeFileVector.end());

	if(strcmp(Localisation,"-2") == 0)
	{
		pimpl_->SetFileDataBase(33);
	}
	else
	{
		if(pimpl_->GetAffichageVolee() == 0)
		{
			if(pimpl_->ChargeThumbmail(Localisation,m_IconeFileVector) != -1)
				m_bChargerIcone = false;
		}
	}

	if(m_bChargerIcone)
		pimpl_->CreateFolderIcone();

	ListView_SetImageList(m_hWnd, pimpl_->hImgThumbmail, LVSIL_NORMAL); 
	List_DisplayAlbum(m_IconeFileVectorAlbum);

	iAlbum = 1;

	CreateListMiniatureHandle();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

void CListPreview::ViderMemoireListe()
{
	InitList(cDirectory,"0");
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

void CListPreview::SauverListe()
{
	if(hThreadPreview != NULL)
	{
		TerminateThread(hThreadPreview,1);
		CloseHandle(hThreadPreview);
		hThreadPreview = NULL;
	}

	if(pimpl_->GetAffichageVolee() == 0)
	{
		if(strcmp(cDirectory,"") != 0)
		{
			pimpl_->SauverDatabaseFolder(cDirectory,m_IconeFileVector);
		}
		else
		{
			if(strcmp(cAlbumThumbmail,"") != 0)
				pimpl_->SaveThumbmail(cAlbumThumbmail,m_IconeFileVector);
		}
	}	
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

int CListPreview::stopGenThumbnail()
{
	int i = 0;
	if(m_hStopThread != NULL)
		CloseHandle(m_hStopThread);

	m_hStopThread = NULL;

	if(hThreadPreview != NULL)
	{
		//Démarrage du thread de récupération des données
		m_hStopThread = (HANDLE)	CreateThread(NULL, cuStackSize,
					List_StopMiniature, this, 0, &dwThreadIdStop);

		CObjet::StartWait();
		SetThreadPriority(m_hStopThread, THREAD_PRIORITY_NORMAL);
		

		return 1;
	}

	return 0;
	
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

void CListPreview::PausePreview()
{
	SuspendThread(hThreadPreview);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

void CListPreview::ResumePreview()
{
	ResumeThread(hThreadPreview);
}


/**************************************************************************

   List_Refresh()

**************************************************************************/

HRESULT CListPreview::List_Refresh()
{
	return List_DisplayFolder(NULL);
}


/**************************************************************************

   List_Notify()

**************************************************************************/

LRESULT CListPreview::List_Notify(LPARAM lParam)
{
LPNMHDR  pnmh = (LPNMHDR) lParam;
CRITICAL_SECTION CriticalSection; 
InitializeCriticalSection(&CriticalSection);

switch(pnmh->code)
   {
   case LVN_GETDISPINFO:
      {
			try
			{
				EnterCriticalSection(&CriticalSection); 

				NMLVDISPINFO   *pdi = (NMLVDISPINFO*)lParam;
				CItemInfo *  pItem = (CItemInfo *)pdi->item.lParam;
				DWORD    dwAttr = 0;

				//do we need to supply the text?
				if(pdi->item.mask & LVIF_TEXT)
				{
					switch(pdi->item.iSubItem)
					{
						case 0:  //name
						{

							SHFILEINFO     sfi;
							if(pItem->GetPidlFQ() != NULL)
							{
								//get the display name of the item
								if(SHGetFileInfo((LPCTSTR)pItem->GetPidlFQ(), 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME))
									lstrcpyn(pdi->item.pszText, sfi.szDisplayName, pdi->item.cchTextMax);
							}
						}
						break;
					}
				}


				//do we need to supply the unselected image?
				if(pdi->item.mask & LVIF_IMAGE)
				{
					if(pdi->item.iImage < 0 || pimpl_->GetAffichageVolee())
					{
						if(pItem->GetPidlRel() != NULL)
						{
							LPCITEMIDLIST m_itemTemp = pItem->GetPidlRel();
							pItem->GetShellFolder()->GetAttributesOf(1, (LPCITEMIDLIST*)&m_itemTemp, &dwAttr);
						}

						if((dwAttr & SFGAO_FOLDER) && pimpl_->GetFileDataBase() != 33 && pItem->GetPidlRel() != NULL)
						{
							if(pItem->GetImage() == 0)
								pdi->item.iImage = pimpl_->GetNumFolder();
							else
								pdi->item.iImage = pItem->GetImage();
						}
						else
						{
							if(pimpl_->GetAffichageVolee() == 0)  
							{
								if(pItem->GetImage() < 0 || pItem->GetImage() > 65000)
									pdi->item.iImage = 0;
								else
									pdi->item.iImage = pItem->GetImage();
							}
							else
							{
								#ifdef _DEBUG
									OutputDebugString("LVN_GETDISPINFO");
								#endif

								if(pItem->GetImage() == 0)
								{
									//Création de l'image
									HBITMAP hBitmapResult;
									int i;
									CLoadImage m_ChargementImage;		
									hBitmapResult = m_ChargementImage.ChargementIconeStatic(pItem->GetParseName(), pimpl_->GetLargeur(), pimpl_->GetHauteur(), 0, 0, 1, RGB(255,255,255),RGB(230,230,230));
									if(hBitmapResult != NULL)
									{
										pItem->SetImage(ImageList_Add(pimpl_->hImgThumbmail,hBitmapResult,NULL));				
										pdi->item.iImage = pItem->GetImage();
									}
								}
								else
									pdi->item.iImage = pItem->GetImage();
							}
						}
					}
				}
				LeaveCriticalSection(&CriticalSection);
				
			  }
			  catch(...)
			  {
				  LeaveCriticalSection(&CriticalSection);
			  }

	  }
      break;

   case LVN_DELETEITEM:
      {
			LPNMLISTVIEW   pnmlv = (LPNMLISTVIEW)lParam;
			CItemInfo *     pItem = (CItemInfo *)pnmlv->lParam;

			//SuspendThread(hThreadPreview);

			if(pnmlv->lParam > 0)
			{
			  try
			  {
				  EnterCriticalSection(&CriticalSection); 
	    		
					if(pItem != NULL)
						delete pItem;

					pItem = NULL;

				  LeaveCriticalSection(&CriticalSection);
			  }
			  catch(...)
			  {
				  LeaveCriticalSection(&CriticalSection);
			  }
			}

      }
      break;
   
   default:
      break;
   }

	DeleteCriticalSection(&CriticalSection);
	return 0;
}

void CListPreview::ViderMemoire()
{
	ImageList_RemoveAll(pimpl_->hImgThumbmail);
	m_IconeFileVector.clear();

}

HIMAGELIST CListPreview::GetImageListHandle()
{
	return pimpl_->hImgThumbmail;
}

void CListPreview::SetAffichageAlaVolee(const int &iVolee)
{
	pimpl_->SetAffichageVolee(iVolee);
}

void CListPreview::SetNbMaxMiniature(const int &iMaxMiniature)
{
	m_iMaxMiniature = iMaxMiniature * 2 + 3;
}

////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////
ULONG CListPreview::List_CreatePicture(int &iNumImage, int nItem, HWND hWnd, HWND hWndMiniature, CMiniatureGen * pMiniatureGen, CMiniatureGen * pMiniatureGenVideo, CMiniatureGen * pMiniatureGenRaw, CMiniatureGen * pMiniatureGenFolder)
{
		long iResult = 0;
		CIconeFile m_IconeFileEqual;
		CIconeFile m_IconeFileTemp;
		IconeFileVector::iterator m_IconeFile;
		HBITMAP hBitmapResult = NULL;
		CItemInfo * pItem = NULL;
		long lNumIcone = -1;	
		LVITEM pitem;
		pitem.iSubItem = 0;
		pitem.mask = LVIF_IMAGE | LVIF_PARAM;
		pitem.iItem = nItem;
		CObjet m_cObjet;

		if(!ListView_GetItem(m_hWnd, &pitem))
			pitem.iImage = 0;

		pItem = (CItemInfo *)pitem.lParam;

		DWORD    dwAttr;	

		//string m_stgTemp = pItem->GetParseName();

		if(pItem != NULL || pimpl_->GetFileDataBase() == 33)
		{	
			if(pItem->GetPidlRel() != NULL)
			{	
				//::MessageBox(NULL,pItem->GetParseName(),"pItem->GetPidlRel() != NULL",0);
				LPCITEMIDLIST m_itemTemp = pItem->GetPidlRel();
				pItem->GetShellFolder()->GetAttributesOf(1, (LPCITEMIDLIST*)&m_itemTemp, &dwAttr);	
			}
			else if(iAlbum != 1)
				return 0;


			bool m_bFolder = m_cObjet.TestRepertoire(pItem->GetParseName());

			if(m_bFolder && !m_bShowFolder)
				return 0;

			if(m_bFolder || m_cObjet.TestZipFormat(pItem->GetParseName()))
			{
				//::MessageBox(NULL,pItem->GetParseName(),"Infos",0);
					
				if(pimpl_->GetFileDataBase() != 33)
				{
					StringCchCopy(m_IconeFileTemp.m_stgFullPathName,MAX_PATH,pItem->GetParseName());
					m_IconeFile = find(m_IconeFileVector.begin(),m_IconeFileVector.end(),m_IconeFileTemp);
					if(m_IconeFile != m_IconeFileVector.end())
						lNumIcone = m_IconeFile->lNumIcone;
				}

				if(lNumIcone == -1)
				{				
					CMiniatureGenData * m_pMiniatureGenData = CreateHandleData();
					m_pMiniatureGenData->SetItemInfo(pItem);
					m_pMiniatureGenData->SetNumItemInfo(nItem);
					m_pMiniatureGenData->SetFilename(pItem->GetParseName());
					m_pMiniatureGenData->SetIfFolder(true);
					SendMessage(hWndMiniature,WM_COMMAND,MAKEWPARAM(IDC_TRAITEMENTDATA,0),(LPARAM)m_pMiniatureGenData);
					

				}

				iNumImage++;
				iResult = 1;
				pItem->SetImage(lNumIcone);
				ListView_RedrawItems(m_hWnd,nItem,nItem);
				SendMessage(hWnd, PBM_SETPOS, iNumImage, 0); 
				char Temp[255];
				StringCchPrintf(Temp,255,"%d Miniatures",iNumImage);
				if(m_ListStatusBar != NULL)
					m_ListStatusBar->UpdateStatusBar(Temp, 0, 4);


			}
			else if(pItem->GetPidlRel() != NULL || iAlbum == 1)
			{			

				//::MessageBox(NULL,pItem->GetParseName(),"Other",0);
							
				if(pimpl_->GetFileDataBase() != 33)
				{
					StringCchCopy(m_IconeFileTemp.m_stgFullPathName,MAX_PATH,pItem->GetParseName());
					m_IconeFile = find(m_IconeFileVector.begin(),m_IconeFileVector.end(),m_IconeFileTemp);
					if(m_IconeFile != m_IconeFileVector.end())
						lNumIcone = m_IconeFile->lNumIcone;
				}

				if(lNumIcone != -1)
				{
					IMAGEINFO ImageInfo;
					ZeroMemory(&ImageInfo,sizeof(IMAGEINFO));
					if(ImageList_GetImageInfo(pimpl_->hImgThumbmail,lNumIcone,&ImageInfo))
					{
						SIZE size;
						ZeroMemory(&size,sizeof(SIZE));
						size.cx = abs(ImageInfo.rcImage.right - ImageInfo.rcImage.left);
						size.cy = abs(ImageInfo.rcImage.top - ImageInfo.rcImage.bottom);
						if(size.cx != pimpl_->GetLargeur() || size.cy != pimpl_->GetHauteur())
						{
							lNumIcone = -1;
							pimpl_->CreateHImageList(m_IconeFileVector);
						}
					}
				}

				if(lNumIcone == -1)
				{
					
					char * filename = pItem->GetParseName();
					CMiniatureGenData * m_pMiniatureGenData = CreateHandleData();
					m_pMiniatureGenData->SetItemInfo(pItem);
					m_pMiniatureGenData->SetNumItemInfo(nItem);
					m_pMiniatureGenData->SetFilename(filename);
					if(m_cObjet.TestVideoFormat(pItem->GetParseName()) > 0)
						SendMessage(pMiniatureGenVideo->GetWindowHandle(),WM_COMMAND,MAKEWPARAM(IDC_TRAITEMENTDATA,0),(LPARAM)m_pMiniatureGenData);
					else
					{
						//Test Num Format 
						int m_iNumFormat = m_cObjet.TestImageFormat(pItem->GetParseName());
						if(m_iNumFormat > 0)
						{
							if(m_iNumFormat == 12)
								SendMessage(pMiniatureGenRaw->GetWindowHandle(),WM_COMMAND,MAKEWPARAM(IDC_TRAITEMENTDATA,0),(LPARAM)m_pMiniatureGenData);
							else
								SendMessage(hWndMiniature,WM_COMMAND,MAKEWPARAM(IDC_TRAITEMENTDATA,0),(LPARAM)m_pMiniatureGenData);
						}
						else
						{
							//Folder
							CMiniatureGenData * m_pMiniatureGenData = CreateHandleData();
							m_pMiniatureGenData->SetItemInfo(pItem);
							m_pMiniatureGenData->SetNumItemInfo(nItem);
							m_pMiniatureGenData->SetFilename(pItem->GetParseName());
							m_pMiniatureGenData->SetIfFolder(true);
							SendMessage(pMiniatureGenFolder->GetWindowHandle(),WM_COMMAND,MAKEWPARAM(IDC_TRAITEMENTDATA,0),(LPARAM)m_pMiniatureGenData);
						}
					}

				}
				else
				{
					iNumImage++;
					iResult = 1;
					pItem->SetImage(lNumIcone);
					ListView_RedrawItems(m_hWnd,nItem,nItem);
					SendMessage(hWnd, PBM_SETPOS, iNumImage, 0); 
					char Temp[255];
					StringCchPrintf(Temp,255,"%d Miniatures",iNumImage);
					if(m_ListStatusBar != NULL)
					{
						m_ListStatusBar->UpdateStatusBar(Temp, 0, 4);
					}
				}
				

			}
		}



	return lNumIcone;
}

CMiniatureGen * CListPreview::CreateHandle(int iNumThread)
{
	CMiniatureGen * m_pMiniatureGen = new CMiniatureGen();
	m_pMiniatureGen->SetThreadId(dwThreadIdPreview);
	HWND hWndMiniature = m_pMiniatureGen->CreateDlg(hInstance,MAKEINTRESOURCE(IDD_DLGKEYWORD),m_hWnd);
	::ShowWindow(hWndMiniature, SW_HIDE);
	return m_pMiniatureGen;
}

CMiniatureGenData * CListPreview::CreateHandleData()
{
	CMiniatureGenData * m_pMiniatureGenData = new CMiniatureGenData();
	m_pMiniatureGenData->SetThreadId(dwThreadIdPreview);
	m_pMiniatureGenData->SetIconeSize(pimpl_->GetHauteur(),pimpl_->GetLargeur());
	return m_pMiniatureGenData;
}

void CListPreview::DefineThumbnailSize(const int &iWidth, const int &iHeight)
{
	m_bDefineThumbnailSize = true;
	pimpl_->SetThumbnailSize(iWidth,iHeight);
}

void CListPreview::GestionMessageMiniature(bool &bEnd,int &iNumTerminerThread, int &iNumImage,const int &iNbThread)
{
	MSG msg;
	char Temp[MAX_PATH];

	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		switch(msg.message)
		{
			case WM_QUIT:
				bEnd = true;
				break;

			case WM_TERMINER_THREAD:
				iNumTerminerThread++;
				break;

			case WM_STOP_DOWNLOAD_AUTO:
				break;

			case WM_START_DOWNLOAD_AUTO:
				break;

			case WM_DOWNLOAD_ONESHOT:
				{
					#pragma omp critical
					{
						CMiniatureGenData * m_pMiniatureGen  = (CMiniatureGenData * )msg.lParam;
						CItemInfo * pItem = m_pMiniatureGen->GetItemInfo();
						if(m_pMiniatureGen->GetBitmapResult() != NULL)
						{
							long lNumIcone = ImageList_Add(pimpl_->hImgThumbmail,m_pMiniatureGen->GetBitmapResult(),NULL);
							if(lNumIcone != -1)
							{
								try
								{
									CIconeFile m_IconeFileTemp;
									StringCchCopy(m_IconeFileTemp.m_stgFullPathName,MAX_PATH,m_pMiniatureGen->GetFilename().c_str());
									m_IconeFileTemp.lNumIcone = lNumIcone;
									m_IconeFileVector.push_back(m_IconeFileTemp);
								}
								catch(...)
								{
								}
								pItem->SetImage(lNumIcone);
								ListView_RedrawItems(m_hWnd,m_pMiniatureGen->GetNumItemInfo(),m_pMiniatureGen->GetNumItemInfo());
							}
						}
						else
						{
							//sprintf_s(Temp,"Erreur Bitmap NULL Filename : %s \n",m_pMiniatureGen->GetFilename().c_str());
							//::MessageBox(NULL,Temp,"Erreur icone",0);
						}

						CDefineRepMonitor m_definir;
						HWND hWndMain = GetParent(GetParent(hParent));
						SendMessage(hWndMain,WM_COMMAND,MAKEWPARAM(IDM_GETHWNDPRGBAR,0),(LPARAM)&m_definir);

						iNumImage++;
						SendMessage(m_definir.hWnd, PBM_SETPOS, iNumImage, 0); 

						StringCchPrintf(Temp,255,"%d Miniatures",iNumImage);
						if(m_ListStatusBar != NULL)
							m_ListStatusBar->UpdateStatusBar(Temp, 0, 4);

						delete m_pMiniatureGen;
					}
				}
				break;
			default :
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ULONG __stdcall CListPreview::List_CreateMiniature(LPVOID param)
{
    //OleInitialize(NULL);
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CListPreview * m_lpListData = (CListPreview *)param;
	int _iNbItem = ListView_GetItemCount(m_lpListData->m_hWnd);
	VectorMiniature m_vectorMiniature;
	char Temp[255];
	int m_iNbThread = omp_get_num_procs();
	int iReturn = CParameter::GetNbThread(m_iNbThread);

	CParameter::SetNbThread(m_iNbThread);

	if(_iNbItem < m_iNbThread)
		m_iNbThread = 1;

	//int m_iNbThread = 4;

	m_lpListData->pimpl_->UpdateData();

	//CRITICAL_SECTION CriticalSection; 

	int nItem = 0;
	int iNbItem = ListView_GetItemCount(m_lpListData->m_hWnd);
	int iNumImage = 0;
	int iNumTerminerThread = 0;
	bool m_bEnd = false;

	if(iNbItem > 0)
	{
		IconeFileVector m_localIconeFileVector;

		CDefineRepMonitor m_definir;
		HWND hWndMain = GetParent(GetParent(m_lpListData->hParent));
		SendMessage(hWndMain,WM_COMMAND,MAKEWPARAM(IDM_GETHWNDPRGBAR,0),(LPARAM)&m_definir);
		
		ListView_SetIconSpacing(m_lpListData->m_hWnd, m_lpListData->pimpl_->GetLargeur() + 20, m_lpListData->pimpl_->GetHauteur() + 30);

		m_localIconeFileVector = m_lpListData->m_IconeFileVector;

		::ShowWindow(m_definir.hWnd,SW_SHOW);

		SendMessage(m_definir.hWnd, PBM_SETRANGE, 0, MAKELPARAM(0,ListView_GetItemCount(m_lpListData->m_hWnd))); 
		SendMessage(m_definir.hWnd, PBM_SETSTEP, (WPARAM) 1, 0); 


		int m_iNbImageStart = ImageList_GetImageCount(m_lpListData->pimpl_->hImgThumbmail);

		for(int i = 0;i < m_iNbThread;i++)
			m_vectorMiniature.push_back(m_lpListData->CreateHandle(i));

		//Création d'un handle pour les videos
		CMiniatureGen * m_pMiniatureGenVideo = m_lpListData->CreateHandle(99);
		CMiniatureGen * m_pMiniatureGenRaw = m_lpListData->CreateHandle(98);
		CMiniatureGen * m_pMiniatureGenFolder = m_lpListData->CreateHandle(97);

		try
		{
			
			int nIdleCount = 0;
			bool m_bFirstPassage = true;

			ListView_Arrange(m_lpListData->m_hWnd,LVA_ALIGNTOP);

			while (true)
			{
				int _iNbImageCount = ImageList_GetImageCount(m_lpListData->pimpl_->hImgThumbmail) - 3;
				if(m_bEnd)
				{
					for(int i = 0;i < m_iNbThread;i++)
					{
						CMiniatureGen * m_pMiniatureGen  = (CMiniatureGen * )m_vectorMiniature[i];
						SendMessage(m_pMiniatureGen->GetWindowHandle(),WM_COMMAND,MAKEWPARAM(IDC_ENDTRAITEMENTDATA,0),0);
					}		

					SendMessage(m_pMiniatureGenVideo->GetWindowHandle(),WM_COMMAND,MAKEWPARAM(IDC_ENDTRAITEMENTDATA,0),0);
					SendMessage(m_pMiniatureGenRaw->GetWindowHandle(),WM_COMMAND,MAKEWPARAM(IDC_ENDTRAITEMENTDATA,0),0);
					SendMessage(m_pMiniatureGenFolder->GetWindowHandle(),WM_COMMAND,MAKEWPARAM(IDC_ENDTRAITEMENTDATA,0),0);
					//m_bEnd = false;


					do
					{
						m_lpListData->GestionMessageMiniature(m_bEnd,iNumTerminerThread,iNumImage,m_iNbThread);
						Sleep(20);
					}while(iNumTerminerThread < (m_iNbThread+3));
					goto CLEANUP;

				}
				
				if(iNumTerminerThread == (m_iNbThread+3))
				{
					m_bEnd = true;
				}

				if(_iNbImageCount == iNbItem && !m_bFirstPassage)
				{
					m_bEnd = true;
				}
				
				m_lpListData->GestionMessageMiniature(m_bEnd,iNumTerminerThread,iNumImage,m_iNbThread);
				if(m_bEnd)
					continue;
				
				if(nItem != -1)
				{
					for(int i = 0;i < m_iNbThread;i++)
					{
						long m_lReturn = -1;
						CMiniatureGen * m_pMiniatureGen  = (CMiniatureGen * )m_vectorMiniature[i];
						if(nItem != -1 && !m_bEnd)
						{
							do
							{
								m_lReturn = m_lpListData->List_CreatePicture(iNumImage,nItem,m_definir.hWnd, m_pMiniatureGen->GetWindowHandle(), m_pMiniatureGen, m_pMiniatureGenVideo, m_pMiniatureGenRaw, m_pMiniatureGenFolder);
								nItem = ListView_GetNextItem(m_lpListData->m_hWnd, nItem, LVNI_ALL); 

								m_lpListData->GestionMessageMiniature(m_bEnd,iNumTerminerThread,iNumImage,m_iNbThread);
								if(m_bEnd)
									break;

							}while(m_lReturn != -1 && nItem != -1);
						}

						if(m_bEnd)
							break;
					}

					m_bFirstPassage = false;
				}
				Sleep(20);
			}
			
		}
		catch (...)
		{
			//LeaveCriticalSection(&CriticalSection);
		}

	CLEANUP:


		if(!m_bEnd)
			ListView_RedrawItems(m_lpListData->m_hWnd,0,ImageList_GetImageCount(m_lpListData->pimpl_->hImgThumbmail));
		
		for(int i = 0;i < m_iNbThread;i++)
		{
			CMiniatureGen * m_pMiniatureGen  = (CMiniatureGen * )m_vectorMiniature[i];
			delete m_pMiniatureGen;
		}

		delete m_pMiniatureGenVideo;
		delete m_pMiniatureGenRaw;
		delete m_pMiniatureGenFolder;

		m_vectorMiniature.clear();

	}


	//if(m_bEnd)
	::SetEvent(m_lpListData->m_hWaitThread);

	::CloseHandle(m_lpListData->hThreadPreview);
	m_lpListData->hThreadPreview = NULL;

	::CloseHandle(m_lpListData->m_hEndThread);
	m_lpListData->m_hEndThread = NULL;

	::CloseHandle(m_lpListData->m_hWaitThread);
	m_lpListData->m_hWaitThread = NULL;

	::CloseHandle(m_lpListData->m_hProcessThread);
	m_lpListData->m_hProcessThread = NULL;

	CoUninitialize();
	ExitThread(0);
	return 0;

}
