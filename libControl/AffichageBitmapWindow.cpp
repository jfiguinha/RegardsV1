#include "stdafx.h"
#include ".\affichagebitmapwindow.h"
#include "AffichageBitmap.h"
#include "WndNavigueBitmap.h"
#include "statusbar.h"
#include "toolbar.h"
#include "traitementBitmap.h"
#include <DropTgt.h>
#include "CoolMenu.h"
#include "registryaccess.h"
#include <OpenGL.h>
#include <OpenGLDiaporama.h>
#include <aboutdialog.h>
#include <diaporama.h>
#include <TraitementInstance.h>
#include <IconeFile.h>
#include "parameter.h"
#include "LoadImage.h"
#include <VideoAPI.h>
#include "GestionTraitementWindow.h"
#ifdef WMP9

//#include "initguid.h"
#include <CWMPHost.h>

#endif

LRESULT CAffichageBitmapWindow::VideoGetFullScreenState()
{
	if(m_wmpHost != NULL)
		return m_wmpHost->OnGetFullScreenState();
	return false;
}

CAffichageBitmapWindow::CAffichageBitmapWindow(void)
{
	m_cParameter = new CParameter();
	m_pDropTarget = new CDropTarget;

	m_cOpenGL = NULL;
	m_cOpenGLDiaporama = NULL;
	m_wmpHost = NULL;

	m_CWndBitmap = new CWndNavigueBitmap();
	m_StatusBar = new CStatusBar();
 	m_cToolbar = new CToolbar();
	m_cGestionTraitementWindow = new CGestionTraitementWindow();

#ifdef COOLMENU
	m_CCoolMenu = NULL;
#endif

	m_iNumTexture3D = 0;

	//
	m_bExplorer = false;
	m_IconeFileVector = NULL;

	m_bPlayVideo = false;
	m_b3D = false;
	m_b3DDiaporama = false;

	m_bCreate = false;
	m_bFirst = true;

	cDirectory[0] = '\0';
	cFile[0] = '\0';

	hwndAffichageBitmap = NULL;
	hParent = NULL;
	hWndToolbar = NULL;
	hWndStatusWindow = NULL;
	HwndBitmap = NULL;

	m_iAffichageAlbum = 0;
	m_bFullScreen = false;
	m_bBlocking = false;
	m_iFileTrie = 0;
	m_bDone = false;

	m_bOldState = WINDOWMODE;

}

CAffichageBitmapWindow::~CAffichageBitmapWindow(void)
{
	delete m_cParameter;

	if(m_cGestionTraitementWindow != NULL)
		delete m_cGestionTraitementWindow;
	m_cGestionTraitementWindow = NULL;

	if(m_pDropTarget != NULL)
		delete m_pDropTarget;

	m_pDropTarget = NULL;

	if(m_cOpenGL != NULL)
	{
		m_cOpenGL->KillGLWindow();
		delete m_cOpenGL;
	}

	m_cOpenGL = NULL;

	if(m_cOpenGLDiaporama != NULL)
	{
		m_cOpenGLDiaporama->KillGLWindow();
		delete m_cOpenGLDiaporama;
	}

	m_cOpenGLDiaporama = NULL;

	if(m_wmpHost != NULL)
	{
		m_wmpHost->DestroyWindow();
		delete m_wmpHost;
	}

	m_wmpHost = NULL;

	if(m_CWndBitmap != NULL)
		delete m_CWndBitmap;

	m_CWndBitmap = NULL;

	if(m_StatusBar != NULL)
		delete m_StatusBar;

	m_StatusBar = NULL;
	
	if(m_cToolbar != NULL)
		delete m_cToolbar;

	m_cToolbar = NULL;

#ifdef COOLMENU
	if(m_CCoolMenu != NULL)
		delete m_CCoolMenu;

	m_CCoolMenu = NULL;
#endif
}

int CAffichageBitmapWindow::SaveVideoFrame(HWND hWnd,char * cFilename, int iSize)
{
	static OPENFILENAME ofn;

#ifdef ENGLISH
	static TCHAR szFilter[] = TEXT ("Files BMP  (*.BMP)\0*.bmp\0");
#endif
#ifdef SPANISH
	static TCHAR szFilter[] = TEXT ("Archivos BMP  (*.BMP)\0*.bmp\0");
#endif
#ifdef FRENCH
	static TCHAR szFilter[] = TEXT ("Fichiers BMP  (*.BMP)\0*.bmp\0");
#endif

	static TCHAR pstrTitleName[MAX_PATH];
	static TCHAR pstrFileName[MAX_PATH];
	TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];

	_splitpath_s(m_wmpHost->GetFilename(), drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

	StringCchCopy(pstrFileName,MAX_PATH,fname);

	ofn.lStructSize       = sizeof (OPENFILENAME) ;
	ofn.hwndOwner         = hWnd ;
	ofn.hInstance         = NULL ;
	ofn.lpstrFilter       = szFilter ;
	ofn.lpstrCustomFilter = NULL ;
	ofn.nMaxCustFilter    = 0 ;
	ofn.nFilterIndex      = 0 ;
	ofn.nMaxFile          = MAX_PATH ;
	ofn.lpstrFileTitle    = NULL ;          // Défini dans les fonctions Ouvrir et Fermer
	ofn.nMaxFileTitle     = MAX_PATH ;
	ofn.lpstrInitialDir   = NULL ;
	ofn.lpstrTitle        = NULL ;
	ofn.Flags             = 0 ;             // Défini dans les fonctions Ouvrir et Fermer
	ofn.nFileOffset       = 0 ;
	ofn.nFileExtension    = 0 ;
	ofn.lpstrDefExt       = TEXT ("BMP") ;
	ofn.lCustData         = 0L ;
	ofn.lpfnHook          = NULL ;
	ofn.lpTemplateName    = NULL ;
	ofn.lpstrFile         = pstrFileName ;
	ofn.lpstrFileTitle    = pstrTitleName ;
	ofn.Flags             = OFN_OVERWRITEPROMPT;

	if (!GetSaveFileName(&ofn))
		return 0;

	strcpy_s(cFilename,MAX_PATH,pstrFileName);

	return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////////
void CAffichageBitmapWindow::ExtractPictureFromVideo(HWND hWnd)
{
	if(m_bPlayVideo)
	{
		char m_cTempFile[MAX_PATH];
		if(SaveVideoFrame(hWnd, m_cTempFile,MAX_PATH))
		{
			//const char * m_cTempFile = "c:\\temp.bmp";
			CLoadImage m_cLoadImage;
			CVideoAPI * m_cVideo = new CVideoAPI();
			double m_dValue = 0;
			double m_dValueEnd = 0;
			UINT m_iWidth = 0;
			UINT m_iHeight = 0;
			m_cLoadImage.GetDimensionImageStatic(m_wmpHost->GetFilename(), &m_iWidth, &m_iHeight);
			m_wmpHost->GetCurrentPosition(m_dValue);
			m_cVideo->VideoToBmp(m_wmpHost->GetFilename(),m_cTempFile,m_dValue,m_iWidth,m_iHeight);
			delete m_cVideo;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CAffichageBitmapWindow::OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId = 0, wmEvent = 0;
	CObjet m_cObjet;
	wmId    = LOWORD(wParam); 
	wmEvent = HIWORD(wParam); 
	// Parse the menu selections:
	char cInfos[MAX_PATH];

	switch (wmId)
	{
		case ID_VIDEXTRACTPICTURE:
			{
				ExtractPictureFromVideo(hWnd);
				break;
			}

		case ID_FICHIER_TRAITEMENTIMAGE:
			{
				m_cGestionTraitementWindow->CreateTraitementWindow(cFile,hWnd);
				break;
			}

		case IDM_ONCOMMANDEFFECT:
			m_bCreate = (bool)lParam;
			break;

		case IDM_GETTRAITEMENTWND:
			{
				HWND hWndLocal = (HWND)lParam;
				m_cGestionTraitementWindow->SetTraitementWindow(hWndLocal);
				break;
			}

		case IDM_DESTROYTRAITEMENT:
			{
				HWND hWndLocal = (HWND)lParam;
				m_cGestionTraitementWindow->DeleteSelectTraitementWindow(hWndLocal);
				break;
			}

		case ID_FICHIER_EXPLORATEURIMAGE:
			{
				m_bExplorer = true;
				SendMessage(hWnd,WM_DESTROY,0,0);
				break;
			}

		case IDM_UPDATESTATUSBAR:
			{
				StringCchCopy(cInfos,MAX_PATH,(char *)lParam);
				m_StatusBar->UpdateStatusBar(cInfos,wmEvent,0);
				break;
			}

		case IDM_UPDATESTATUS:
			{
				int iHeigth = HIWORD(lParam);
				int iWidth = LOWORD(lParam);
				StringCchPrintf(cInfos,MAX_PATH,"%dx%d",iWidth,iHeigth);
				m_StatusBar->UpdateStatusBar(cInfos,2,0);
			}
			break;

		case IDM_FILENAME:
			{
				m_iAffichageAlbum = 0;
				StringCchCopy(cFile,MAX_PATH,(char *)lParam);
				SetWindowText(hWnd,(const char *)lParam);
				SendMessage(m_CWndBitmap->m_hWnd, message, wParam, lParam);
			}
			break;

		case IDM_ALBUMNAME:
			m_IconeFileVector = (IconeFileVector *)lParam;
			m_iAffichageAlbum = 1;
			SendMessage(m_CWndBitmap->m_hWnd, message, wParam, lParam);
			break;
		
		case IDM_DIRNAME:
			m_iAffichageAlbum = 0;
			StringCchCopy(cDirectory,MAX_PATH,(char *)lParam);
			m_StatusBar->UpdateStatusBar(cDirectory,3,0);
			break;

		case IDM_IMAGES_VISUALISATION_DIAPORAMA:
			{
				CDiaporama * m_cDiaporama = new CDiaporama(hInstance);
				m_cDiaporama->CreateDlg(hInstance,(LPCTSTR)IDD_SLIDESHOW, hWnd);
				delete m_cDiaporama;
			}
			break;

		case IDM_ABOUT:
			{
				CAboutDialog * m_CDialogAbout = new CAboutDialog(hInstance);
				m_CDialogAbout->CreateDlg(hInstance,(LPCTSTR)IDD_ABOUTBOX, hWnd);
				delete m_CDialogAbout;
			}
		   break;

		case IDM_QUITTER:
		   DestroyWindow(hWnd);
		   break;

		case IDM_IMAGES_VISUALISATION_PLAINECRAN:
			SendMessage(hWnd,WM_KEYDOWN,VK_F4,0);
			break;

		case IDM_CHANGEMENT_IMAGE:
			{
				HMENU hMenu = GetMenu(hWnd);
				EnableMenuItem(hMenu,3,MF_GRAYED);
				char * cValue = (char *)lParam;
				StringCchCopy(cFile,MAX_PATH,cValue);
				SetWindowText(hWnd, cFile);
			}
			break;

		case IDM_IMAGES_SHRINKIMAGE:
			{
				char value[2];
				SendMessage(m_CWndBitmap->m_hWnd,WM_COMMAND,MAKEWPARAM(IDM_GETSHRINK,0),(LPARAM)value);
				if(atoi(value))
					CheckMenuItem(GetMenu(hWnd),IDM_IMAGES_SHRINKIMAGE,MF_UNCHECKED);
				else
					CheckMenuItem(GetMenu(hWnd),IDM_IMAGES_SHRINKIMAGE,MF_CHECKED);
				SendMessage(m_CWndBitmap->m_hWnd, message, wParam, lParam);
			}
			break;

		////////////////////////////////////////////////////////////////////////////////////////////
		//3d
		////////////////////////////////////////////////////////////////////////////////////////////
		case IDM_SHOW_3D:
			{
				char * cValue = (char *)lParam;
				ShowFileInfos(hWnd,cValue);

				m_CWndBitmap->ShowWindow(SW_HIDE);

				StopVideo();
				StopDiaporama3D();
				
				m_cOpenGL->ShowWindow(SW_SHOW);

				RECT rcPos;
				GetClientRect(m_CWndBitmap->m_hWnd,&rcPos);
				if(!this->GetFullScreen())
					rcPos.top += 40;
				rcPos.bottom += 0;
				MoveWindow(m_cOpenGL->m_hWnd,rcPos.left,rcPos.top,rcPos.right,rcPos.bottom,false);
			
				m_cOpenGL->ChargerFichier(cValue);
				m_b3D = true;

			}
			break;

		case IDM_SHOW_3DDIAPORAMA:
			{
				CFichier * m_pFichier = (CFichier *)lParam;
				SetWindowText(hWnd, "Diaporama 3D");
				m_CWndBitmap->ShowWindow(SW_HIDE);

				StopVideo();
				Stop3D();
				
				m_cOpenGLDiaporama->ShowWindow(SW_SHOW);

				RECT rcPos;
				GetClientRect(m_CWndBitmap->m_hWnd,&rcPos);
				if(!this->GetFullScreen())
					rcPos.top += 40;
				rcPos.bottom += 0;
				MoveWindow(m_cOpenGLDiaporama->m_hWnd,rcPos.left,rcPos.top,rcPos.right,rcPos.bottom,false);

				m_cOpenGLDiaporama->SetNumTexture(m_iNumTexture3D);
				m_cOpenGLDiaporama->ChargerRepertoire(m_pFichier,2);		

				m_b3DDiaporama = true;
				
			}
			break;

		case IDM_STOP_3D:
			{
				Stop3D();
				m_CWndBitmap->ShowWindow(SW_SHOWNORMAL);
			}
			break;

		case IDM_STOP_3DDIAPORAMA:
			{
				StopDiaporama3D();
				m_CWndBitmap->ShowWindow(SW_SHOWNORMAL);
				break;
			}

		case IDM_UPDATESTATUSFILEINFO:
			{
				char * cValue = (char *)lParam;
				ShowFileInfos(hWnd,cValue);
			}
			break;

		////////////////////////////////////////////////////////////////////////////////////////////
		//Video
		////////////////////////////////////////////////////////////////////////////////////////////



#ifdef WMP9
			
		case IDM_PLAY_VIDEO:
			{
				char * filevideo = (char *)lParam;
				ShowFileInfos(hWnd,filevideo);

				m_CWndBitmap->ShowWindow(SW_HIDE);
				Stop3D();
				StopDiaporama3D();

				if(m_bPlayVideo)
				{
					m_wmpHost->OnClose();
					m_wmpHost->OnPlayFile(filevideo,strlen(filevideo)+1);
				}
				else
				{
					RECT rcPos = {0,0,0,0};

					m_bPlayVideo = true;

					if(m_bFullScreen)
					{
						rcPos.right = GetDeviceCaps(GetDC(NULL),HORZRES);
						rcPos.bottom = GetDeviceCaps(GetDC(NULL),VERTRES);
					}
					else
					{
						GetClientRect(m_CWndBitmap->m_hWnd,&rcPos);
						rcPos.top += 40;
						rcPos.bottom += 40;
					}
				
					m_wmpHost->MoveWindow(&rcPos,true);
					m_wmpHost->ShowWindow(SW_SHOWNORMAL);
					m_wmpHost->OnPlayFile(filevideo,strlen(filevideo)+1);
				}
			}
			break;

		case IDM_FULLSCREEN_VIDEO:
			{
				if(m_bPlayVideo == 1)
					m_wmpHost->OnWMPPlayerFullScreen();
			}
			break;

		case IDM_SHRINK_VIDEO:
			{
				if(m_bPlayVideo)
					m_wmpHost->OnWMPPlayer2StretchToFit();
			}
			break;
		
		case IDM_STOP_VIDEO:
			{
				StopVideo();
				m_CWndBitmap->ShowWindow(SW_SHOWNORMAL);
			}
			break;


		case ID_VIDO_DUREENCOURS:
			{
				if(m_bPlayVideo)
					m_wmpHost->CreateBookmarkFile();
				break;
			}

		case ID_VIDO_ALLERAUBOOKMARL:
			{
				if(m_bPlayVideo)
					m_wmpHost->ReadBookmarkFile();
				break;
			}


#endif

		default:
			if(m_cOpenGL->IsWindowVisible())
			{
				if(m_cOpenGL->OnCommand(hWnd, message, wParam, lParam) != 0)
					return 0;
			}
			else if(m_cOpenGLDiaporama->IsWindowVisible())
			{
				if(m_cOpenGLDiaporama->OnCommand(hWnd, message, wParam, lParam) != 0)
					return 0;
			}
			SendMessage(m_CWndBitmap->m_hWnd, message, wParam, lParam);
			return 0;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///Affichage des infos sur un fichier
////////////////////////////////////////////////////////////////////////////////////////////////
void CAffichageBitmapWindow::ShowFileInfos(HWND hWnd,const char * Filename)
{
	char cTemp[255];
	CObjet m_cObjet;
	m_cObjet.GetFileName(Filename,cTemp,255);
	StringCchCopy(cFile,MAX_PATH,Filename);
	SetWindowText(hWnd, cFile);
	m_StatusBar->UpdateStatusBar(cTemp,1,0);	
}

////////////////////////////////////////////////////////////////////////////////////////////////
///Arrêt de la video
////////////////////////////////////////////////////////////////////////////////////////////////
void CAffichageBitmapWindow::StopVideo()
{
	if(m_bPlayVideo)
	{
		m_wmpHost->OnClose();
		m_wmpHost->ShowWindow(SW_HIDE);
		m_bPlayVideo =false;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
///Arrêt de la 3D
////////////////////////////////////////////////////////////////////////////////////////////////
void CAffichageBitmapWindow::Stop3D()
{
	if(m_b3D)
	{
		m_cOpenGL->ShowWindow(SW_HIDE);
		m_b3D = false;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
///Arrêt du diaporama 3D
////////////////////////////////////////////////////////////////////////////////////////////////
void CAffichageBitmapWindow::StopDiaporama3D()
{
	if(m_b3DDiaporama)
	{
		m_cOpenGLDiaporama->ShowWindow(SW_HIDE);
		m_b3DDiaporama =false;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CAffichageBitmapWindow::OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int idButton;
	LPNMHDR value;

	value = (LPNMHDR)lParam;

	if(value == NULL)
		return 0;

	/////////////////////////////////////////
	//Récupération des différents messages
	//du controle treeview
	/////////////////////////////////////////

	switch (value->code) 
	{
		case TTN_GETDISPINFO: 
			{ 
			LPTOOLTIPTEXT lpttt; 

			lpttt = (LPTOOLTIPTEXT) lParam; 
			lpttt->hinst = hInstance; 

			// Specify the resource identifier of the descriptive 
			// text for the given button. 
			idButton = lpttt->hdr.idFrom; 
			switch (idButton) 
			{ 
				case IDM_IMAGES_PRCDENTE: 
					lpttt->lpszText = MAKEINTRESOURCE(IDS_IMGSUIVANTE); 
					break; 
				case IDM_IMAGES_SUIVANTE: 
					lpttt->lpszText = MAKEINTRESOURCE(IDS_IMGPRECEDENTE); 
					break; 
				case ID_IMAGES_VISUALISATION_PREMIREIMAGE: 
					lpttt->lpszText = MAKEINTRESOURCE(IDS_IMGPREMIER); 
					break; 
				case ID_IMAGES_VISUALISATION_DERNIREIMAGE: 
					lpttt->lpszText = MAKEINTRESOURCE(IDS_IMGDERNIER); 
					break; 
				case IDM_FICHIER_REVERT: 
					lpttt->lpszText = MAKEINTRESOURCE(IDS_REVERT); 
					break; 
				case IDM_IMAGES_VISUALISATION_PLAINECRAN: 
					lpttt->lpszText = MAKEINTRESOURCE(IDS_PLEINECRAN); 
					break; 
				case IDM_IMAGES_ROTATEIMAGE_90CW:
					lpttt->lpszText = MAKEINTRESOURCE(IDS_ROTATE90CW);
					break;
				case IDM_IMAGES_ROTATEIMAGE_90CCW:
					lpttt->lpszText = MAKEINTRESOURCE(IDS_ROTATE90CCW);
					break;

				case IDM_IMAGES_ROTATEIMAGE_FLIPHORIZONTAL:
					lpttt->lpszText = MAKEINTRESOURCE(IDS_FLIPHORZ);
					break;

				case IDM_IMAGES_ROTATEIMAGE_FLIPVERTICAL:
					lpttt->lpszText = MAKEINTRESOURCE(IDS_FLIPVERT);
					break;

				case ID_IMAGE_BOUGERIMAGE:
					lpttt->lpszText = MAKEINTRESOURCE(IDS_HANDTOOLS);
					break;

				case ID_IMAGE_ZOOMERIMAGE:
					lpttt->lpszText = MAKEINTRESOURCE(IDS_ZOOMTOOLS);
					break;
			}
		}
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CAffichageBitmapWindow::OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_pDropTarget != NULL && !m_bFullScreen)
	{
		if(RegisterDragDrop(hWnd, m_pDropTarget) == S_OK)
		{
			m_pDropTarget->SetWindow(hWnd);
		}
	}
	
	if(m_bFullScreen)
	{
		char value[2];
		m_CWndBitmap->SetFullScreen(true);
		m_CWndBitmap->SetShrinkImage(true);
		if(atoi(value))
			CheckMenuItem(GetMenu(hwndAffichageBitmap),IDM_IMAGES_SHRINKIMAGE,MF_CHECKED);
		else
			CheckMenuItem(GetMenu(hwndAffichageBitmap),IDM_IMAGES_SHRINKIMAGE,MF_UNCHECKED);

		m_bOldState = FULLSCREENMODE;
	}
	else
	{
		HMENU hsub,hSubMenu,hmenu;

		m_CWndBitmap->SetFullScreen(false);
		m_CWndBitmap->SetShrinkImage(true);

#ifdef COOLMENU
		if(m_CCoolMenu == NULL)
			m_CCoolMenu = new CCoolMenu();

		hmenu = GetMenu(hWnd);
		for(int i = 0;i < 5;i++)
		{
			hsub  = GetSubMenu(hmenu, i);
			m_CCoolMenu->MakeCoolMenu(hsub, hInstance, MAKEINTRESOURCE(IDB_MENUBMP));
		}

		hsub  = GetSubMenu(hmenu, 2);
		hSubMenu = GetSubMenu(hsub, 0);
		m_CCoolMenu->MakeCoolMenu(hSubMenu, hInstance, MAKEINTRESOURCE(IDB_MENUBMP));
		hSubMenu = GetSubMenu(hsub, 2);
		m_CCoolMenu->MakeCoolMenu(hSubMenu, hInstance, MAKEINTRESOURCE(IDB_MENUBMP));
		hSubMenu = GetSubMenu(hsub, 4);
		m_CCoolMenu->MakeCoolMenu(hSubMenu, hInstance, MAKEINTRESOURCE(IDB_MENUBMP));
		hSubMenu = GetSubMenu(hsub, 6);
		m_CCoolMenu->MakeCoolMenu(hSubMenu, hInstance, MAKEINTRESOURCE(IDB_MENUBMP));
		hSubMenu = GetSubMenu(hsub, 9);
		m_CCoolMenu->MakeCoolMenu(hSubMenu, hInstance, MAKEINTRESOURCE(IDB_MENUBMP));
#endif
		m_bOldState = WINDOWMODE;
	}

	int iLocalValue = m_iAffichageAlbum;
	char cInfos[255];
	CObjet m_cObjet;

	if(!m_bFullScreen)
	{
		hWndToolbar = m_cToolbar->CreateRebar(hWnd,hInstance, 2);
		hWndStatusWindow = m_StatusBar->CreateStatusBarCtrl(hWnd);
		m_StatusBar->CreateMultipartStatus(hWndStatusWindow,4,"100","400","500","1200","-1");
		m_cObjet.GetInfosFile(cFile,cInfos,255);
		m_StatusBar->UpdateStatusBar(cDirectory,3,0);
		m_StatusBar->UpdateStatusBar(cInfos,1,0);
		
		RECT rc = {CW_USEDEFAULT, 0, CW_USEDEFAULT, 0};
		DWORD dwndStlyle = WS_CHILD | WS_VISIBLE;
		//DWORD dStyle = CS_HREDRAW | CS_VREDRAW;
		DWORD dStyle = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		DWORD dExStyle = WS_EX_CLIENTEDGE;
		m_CWndBitmap->SetBackgroundColor((HBRUSH)(BLACK_BRUSH));
		m_CWndBitmap->CreateEx(hWnd,hInstance,dwndStlyle,dStyle,dExStyle,"NavigueBitmap",0,0,IDC_ARROW,rc);

	}
	else
	{
		RECT rc = {CW_USEDEFAULT, 0, CW_USEDEFAULT, 0};
		DWORD dwndStlyle = WS_CHILD | WS_VISIBLE;
		//DWORD dStyle = CS_HREDRAW | CS_VREDRAW;
		DWORD dStyle = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		DWORD dExStyle = 0;
		m_CWndBitmap->SetBackgroundColor((HBRUSH)(BLACK_BRUSH));
		m_CWndBitmap->CreateEx(hWnd,hInstance,dwndStlyle,dStyle,dExStyle,"NavigueBitmap",0,0,IDC_ARROW,rc);

	}

	

	if(m_cOpenGL == NULL)
		m_cOpenGL = new COpenGL();

	if(m_b3D)
	{
		DWORD dExStyle = 0;
		RECT  rc = {0, 0, GetDeviceCaps(GetDC(NULL),HORZRES), GetDeviceCaps(GetDC(NULL),VERTRES)};
		if(!m_bFullScreen)
		{
			rc.bottom = rc.right = CW_USEDEFAULT;
			dExStyle = WS_EX_CLIENTEDGE;
		}

		DWORD dwndStlyle = WS_CHILD | WS_VISIBLE;
		DWORD dStyle = CS_HREDRAW | CS_VREDRAW;
		m_cOpenGL->SetBackgroundColor((HBRUSH)(BLACK_BRUSH));
		m_cOpenGL->CreateEx(hWnd,hInstance,dwndStlyle,dStyle,dExStyle,"OPENGL",0,0,IDC_ARROW,rc);

	}
	else
	{
		DWORD dExStyle = 0;
		RECT rc = {0, 0, GetDeviceCaps(GetDC(NULL),HORZRES), GetDeviceCaps(GetDC(NULL),VERTRES)};

		if(!m_bFullScreen)
		{
			rc.bottom = rc.right = CW_USEDEFAULT;
			dExStyle = WS_EX_CLIENTEDGE;
		}

		DWORD dwndStlyle = WS_CHILD;
		DWORD dStyle = CS_HREDRAW | CS_VREDRAW;

		m_cOpenGL->SetBackgroundColor((HBRUSH)(BLACK_BRUSH));
		m_cOpenGL->CreateEx(hWnd,hInstance,dwndStlyle,dStyle,dExStyle,"OPENGL",0,0,IDC_ARROW,rc);

	}

	if(m_cOpenGLDiaporama == NULL)
	{
		m_cOpenGLDiaporama = new COpenGLDiaporama();
	}

	if(m_b3DDiaporama)
	{
		DWORD dExStyle = 0;
		RECT  rc = {0, 0, GetDeviceCaps(GetDC(NULL),HORZRES), GetDeviceCaps(GetDC(NULL),VERTRES)};
		if(!m_bFullScreen)
		{
			rc.bottom = rc.right = CW_USEDEFAULT;
			dExStyle = WS_EX_CLIENTEDGE;
		}

		DWORD dwndStlyle = WS_CHILD | WS_VISIBLE;
		DWORD dStyle = CS_HREDRAW | CS_VREDRAW;
		m_cOpenGLDiaporama->SetBackgroundColor((HBRUSH)(BLACK_BRUSH));
		m_cOpenGLDiaporama->CreateEx(hWnd,hInstance,dwndStlyle,dStyle,dExStyle,"OPENGLDIAPORAMA",0,0,IDC_ARROW,rc);

	}
	else
	{
		DWORD dExStyle = 0;
		RECT rc = {0, 0, GetDeviceCaps(GetDC(NULL),HORZRES), GetDeviceCaps(GetDC(NULL),VERTRES)};

		if(!m_bFullScreen)
		{
			rc.bottom = rc.right = CW_USEDEFAULT;
			dExStyle = WS_EX_CLIENTEDGE;
		}

		DWORD dwndStlyle = WS_CHILD;
		DWORD dStyle = CS_HREDRAW | CS_VREDRAW;

		m_cOpenGLDiaporama->SetBackgroundColor((HBRUSH)(BLACK_BRUSH));
		m_cOpenGLDiaporama->CreateEx(hWnd,hInstance,dwndStlyle,dStyle,dExStyle,"OPENGLDIAPORAMA",0,0,IDC_ARROW,rc);

	}


	#ifdef WMP9

	
	if(m_wmpHost == NULL)
		m_wmpHost = new CWMPHost();


	if(m_bPlayVideo)
	{
		RECT rcPos = {0,0,0,0};

		if(m_bFullScreen)
		{
			rcPos.right = GetDeviceCaps(GetDC(NULL),HORZRES);
			rcPos.bottom = GetDeviceCaps(GetDC(NULL),VERTRES);
		}
		m_wmpHost->Create(hWnd, rcPos, _T("WMP Host Container"), WS_CHILD, 0);
	}
	else
	{
		RECT rcPos = {0,0,0,0};
		m_wmpHost->Create(hWnd, rcPos, _T("WMP Host Container"), WS_CHILD, WS_EX_CLIENTEDGE);
	}
	
#endif

	SendMessage(m_CWndBitmap->m_hWnd, WM_COMMAND, MAKEWPARAM(IDM_FILENAME,m_iFileTrie), (LPARAM)cFile);

	m_bFirst = false;

	SetWindowText(hWnd, cFile);
	SendMessage(hParent,WM_COMMAND,MAKEWPARAM(IDM_GETAFFICHAGEWND,0),(LPARAM)hWnd);
	
	if(iLocalValue == 1)
	{
		SendMessage(m_CWndBitmap->m_hWnd, WM_COMMAND, MAKEWPARAM(IDM_ALBUMNAME,0), (LPARAM)m_IconeFileVector);
		m_iAffichageAlbum = iLocalValue;
	}

	if(m_b3DDiaporama)
		SendMessage(m_CWndBitmap->m_hWnd, WM_COMMAND, MAKEWPARAM(ID_DIAPORAMA_EN3D, 0), 0);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CAffichageBitmapWindow::OnMeasureItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_cOpenGL->IsWindowVisible())
	{
		m_cOpenGL->OnPaint(m_cOpenGL->m_hWnd, message, wParam, lParam);
	}
#ifdef COOLMENU
	if(wParam == 0)
	{
		return m_CCoolMenu->CoolMenu_MeasureItem(wParam, lParam);
	}
#endif
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CAffichageBitmapWindow::OnDrawItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_cOpenGL->IsWindowVisible())
	{
		m_cOpenGL->OnPaint(m_cOpenGL->m_hWnd, message, wParam, lParam);
	}

#ifdef COOLMENU
	if(wParam == 0)
	{
		return m_CCoolMenu->CoolMenu_DrawItem(wParam, lParam);
	}
#endif
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CAffichageBitmapWindow::OnKeyDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
		case 53:
			KillTimer(hWnd,TIMER_DIAPORAMA);
			break;

		case VK_F4:

			if(!m_bFullScreen && !m_bPlayVideo)
			{
				m_bCreate = true;
				m_bDone = FALSE;
				//DestroyWindow(hWnd);
				m_bFullScreen = true;
				//CreateBitmapWindow(SW_SHOW);
				if(hParent != NULL)
				{
					SendMessage(hwndAffichageBitmap,WM_COMMAND,MAKEWPARAM(IDM_CREATEBITMAPWINDOW,1),0);					
				}
			}
			break;

		case VK_ESCAPE:
			{	
				if(m_bFullScreen)
				{
					CDiaporama * m_cDiaporama = new CDiaporama(hInstance);
					m_bDone = FALSE;

					m_bFullScreen = false;
					m_bCreate = false;

					if(m_cParameter->GetStartDiaporama())
						SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(ID_IMAGES_STOPDIAPORAMA,0), 0);
					
					//DestroyWindow(hWnd);
					delete m_cDiaporama;
					//CreateBitmapWindow(SW_SHOW);
					if(hParent != NULL)
					{
						SendMessage(hwndAffichageBitmap,WM_COMMAND,MAKEWPARAM(IDM_CREATEBITMAPWINDOW,1),0);					
					}
				}
			}
			break;

		case VK_SPACE:
		case VK_NEXT:
			{
				SendMessage(m_CWndBitmap->m_hWnd, WM_COMMAND, MAKEWPARAM(IDM_IMAGES_SUIVANTE,0), lParam);
			}
			break;

		case VK_PRIOR:
			{
				SendMessage(m_CWndBitmap->m_hWnd, WM_COMMAND, MAKEWPARAM(IDM_IMAGES_PRCDENTE,0), lParam);
			}
			break;
	}


	if(m_cOpenGL->IsWindowVisible())
	{
		if(m_cOpenGL->ProcessKey(hWnd, message, wParam, lParam) != 0)
			return 0;
	}
	else if(m_cOpenGLDiaporama->IsWindowVisible())
	{
		if(m_cOpenGLDiaporama->ProcessKey(hWnd, message, wParam, lParam) != 0)
			return 0;
	}

		
	SendMessage(m_CWndBitmap->m_hWnd, message, wParam, lParam);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CAffichageBitmapWindow::OnMouseWheel(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_cOpenGL->IsWindowVisible())
		m_cOpenGL->OnMouseWheel(m_cOpenGL->m_hWnd, message, wParam, lParam);
	else if(m_cOpenGLDiaporama->IsWindowVisible())
		m_cOpenGLDiaporama->OnMouseWheel(m_cOpenGLDiaporama->m_hWnd, message, wParam, lParam);
	else
		m_CWndBitmap->OnMouseWheel(m_CWndBitmap->m_hWnd, message, wParam, lParam);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CAffichageBitmapWindow::OnSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef WMP9

	int yTop = 0;
 	int yNewSize = HIWORD (lParam); 
	int xNewSize = LOWORD (lParam);
	RECT rcPos,rc;

	if(!m_bFullScreen)
	{
		GetClientRect(hWnd,&rcPos);
		m_StatusBar->DeplaceStatusBar(hWnd);
		//yNewSize -= 60;
		//yTop = 40;
		
		::GetClientRect(hWndStatusWindow,&rc);
		yNewSize -= rc.bottom;
		::GetClientRect(hWndToolbar,&rc);
		yNewSize -= rc.bottom;
		yTop = rc.bottom;

		MoveWindow(hWndToolbar,rc.left,rc.top, rcPos.right, rc.bottom,true);
	}

	if(!m_bPlayVideo)
	{
		GetClientRect(hWnd,&rcPos);
		rcPos.top = yTop;
		::GetClientRect(hWndToolbar,&rc);
		rcPos.bottom = yNewSize+rc.bottom;
		rcPos.left = 0;
		rcPos.right = rcPos.right;
	}
	else
	{	
		GetClientRect(hWnd,&rcPos);
		::GetClientRect(hWndToolbar,&rc);
		rcPos.top = yTop;
		rcPos.bottom = yNewSize+rc.bottom;
		rcPos.left = 0;
		rcPos.right = rcPos.right;
	}

	MoveWindow(m_CWndBitmap->m_hWnd,0,yTop, xNewSize, yNewSize,true);
	MoveWindow(m_cOpenGL->m_hWnd,0,yTop, xNewSize, yNewSize,true);
	MoveWindow(m_cOpenGLDiaporama->m_hWnd,0,yTop, xNewSize, yNewSize,true);
	m_wmpHost->MoveWindow(&rcPos,true);

#else

	int yTop;
	yTop = 0;
 	yNewSize = HIWORD (lParam); 
	xNewSize = LOWORD (lParam);
	

	if(!m_bFullScreen)
	{
		GetClientRect(hWnd,&rcPos);
		m_StatusBar->DeplaceStatusBar(hWnd);
		//yNewSize -= 60;
		//yTop = 40;
		::GetClientRect(hWndStatusWindow,&rc);
		yNewSize -= rc.bottom;
		::GetClientRect(hWndToolbar,&rc);
		yNewSize -= rc.bottom;
		yTop = rc.bottom;

		MoveWindow(hWndToolbar,rc.left,rc.top, rcPos.right, rc.bottom,true);
	}
	
	MoveWindow(m_CWndBitmap->m_hWnd,0,yTop, xNewSize, yNewSize,true);

#endif

	return 0;
}

LRESULT CAffichageBitmapWindow::OnMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	SendMessage(m_CWndBitmap->m_hWnd, message,wParam,lParam);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CAffichageBitmapWindow::OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	RECT rc;
	HDC hDC = BeginPaint( hWnd, &ps );
	
	if(!m_bFullScreen)
		SendMessage(hWndToolbar, message,wParam,lParam);

	if(IsWindowVisible(m_CWndBitmap->m_hWnd))
		SendMessage(m_CWndBitmap->m_hWnd, message,wParam,lParam);

	if(IsWindowVisible(m_cOpenGL->m_hWnd))
		SendMessage(m_cOpenGL->m_hWnd, message,wParam,lParam);

	if(IsWindowVisible(m_cOpenGLDiaporama->m_hWnd))
		SendMessage(m_cOpenGLDiaporama->m_hWnd, message,wParam,lParam);

	EndPaint(hWnd, &ps);
	ReleaseDC(hWnd,hDC);

	return 0;
}

void CAffichageBitmapWindow::SetWindowPlacement(HWND hWnd)
{
	if(hWnd != NULL)
		CParameter::LoadWindowPlacement("SOFTWARE\\Regards\\navigation",hWnd);
}

LRESULT CAffichageBitmapWindow::OnWindowPosChanging(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPWINDOWPOS _lpWindowPos = (LPWINDOWPOS)lParam;
	_windowPos.cx = _lpWindowPos->cx;
	_windowPos.cy = _lpWindowPos->cy;
	_windowPos.x = _lpWindowPos->x;
	_windowPos.y = _lpWindowPos->y;
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CAffichageBitmapWindow::OnDestroy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HMENU hmenu,hsub,hSubMenu;

	if(m_bOldState == WINDOWMODE)
	{
		WINDOWPLACEMENT _wndPlacement;
		BOOL _bResult = GetWindowPlacement(hWnd, &_wndPlacement);
		CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
		if(m_cRegistryAccess->WriteOrCreateKeyValue("SOFTWARE\\Regards\\navigation") == ERROR_SUCCESS)
		{
			m_cRegistryAccess->WriteIntValue(_wndPlacement.rcNormalPosition.left,"left");
			m_cRegistryAccess->WriteIntValue(_wndPlacement.rcNormalPosition.top,"top");
			m_cRegistryAccess->WriteIntValue(_wndPlacement.rcNormalPosition.right,"right");
			m_cRegistryAccess->WriteIntValue(_wndPlacement.rcNormalPosition.bottom,"bottom");
			m_cRegistryAccess->WriteIntValue(_wndPlacement.flags,"flags");
			m_cRegistryAccess->WriteIntValue(_wndPlacement.showCmd,"showCmd");
			
		}
		delete m_cRegistryAccess;
	}
	
	DestroyWindow(m_CWndBitmap->m_hWnd);

#ifdef COOLMENU
	hmenu = GetMenu(hWnd);
	if(hmenu != NULL)
	{
		
		for(int i = 0;i < 5;i++)
		{
			hsub  = GetSubMenu(hmenu, i);
			m_CCoolMenu->RemoveCoolMenu(hsub);
		}

		hsub  = GetSubMenu(hmenu, 2);
		hSubMenu = GetSubMenu(hsub, 0);
		m_CCoolMenu->RemoveCoolMenu(hSubMenu);
		hSubMenu = GetSubMenu(hsub, 2);
		m_CCoolMenu->RemoveCoolMenu(hSubMenu);
		hSubMenu = GetSubMenu(hsub, 4);
		m_CCoolMenu->RemoveCoolMenu(hSubMenu);
		hSubMenu = GetSubMenu(hsub, 6);
		m_CCoolMenu->RemoveCoolMenu(hSubMenu);
		hSubMenu = GetSubMenu(hsub, 9);
		m_CCoolMenu->RemoveCoolMenu(hSubMenu);
	}

	if(m_CCoolMenu != NULL)
		delete m_CCoolMenu;

	m_CCoolMenu = NULL;
#endif
				
#ifdef WMP9

	if(m_wmpHost != NULL)
	{
		m_wmpHost->DestroyWindow();
		delete m_wmpHost;
	}
	m_wmpHost = NULL;

#endif

	if(m_cOpenGL != NULL)
	{
		m_cOpenGL->KillGLWindow();
		delete m_cOpenGL;
	}
	m_cOpenGL = NULL;

	if(m_cOpenGLDiaporama != NULL)
	{
		m_iNumTexture3D = m_cOpenGLDiaporama->GetNumTexture();
		m_cOpenGLDiaporama->KillGLWindow();
		//delete m_cOpenGLDiaporama;
	}
	//m_cOpenGLDiaporama = NULL;
	
	if(hParent != NULL)
		SendMessage(hParent,WM_COMMAND,MAKEWPARAM(IDM_DELAFFICHAGEWND,0),0);					
	if(m_pDropTarget != NULL)
		RevokeDragDrop(hWnd);

	PostQuitMessage(0);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CAffichageBitmapWindow::OnKillFocus(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_bFullScreen && !m_bCreate && !m_bPlayVideo)
	{
		CDiaporama * m_cDiaporama = new CDiaporama(hInstance);
		m_bDone = FALSE;

		if(m_cParameter->GetStartDiaporama())
			SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(ID_IMAGES_STOPDIAPORAMA,0), 0);
	
		DestroyWindow(hWnd);

		m_bFullScreen = false;
		delete m_cDiaporama;
		//CreateBitmapWindow(SW_SHOW);
		if(hParent != NULL)
		{
			SendMessage(hwndAffichageBitmap,WM_COMMAND,MAKEWPARAM(IDM_CREATEBITMAPWINDOW,1),0);					
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CAffichageBitmapWindow::OnExitMenuLoop(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_cOpenGL->IsWindowVisible())
	{
		m_cOpenGL->ShowWindow(SW_HIDE);
		m_cOpenGL->ShowWindow(SW_SHOWNORMAL);
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CAffichageBitmapWindow::OnEraseBackGround(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	if(!m_bFullScreen)
	{
		GetClientRect(hWndToolbar,&rc);
		RedrawWindow(hWndToolbar,&rc,NULL,RDW_INTERNALPAINT);
	}

	GetClientRect(m_CWndBitmap->m_hWnd,&rc);
	RedrawWindow(m_CWndBitmap->m_hWnd,&rc,NULL,RDW_INTERNALPAINT);
	GetClientRect(m_cOpenGL->m_hWnd,&rc);
	RedrawWindow(m_cOpenGL->m_hWnd,&rc,NULL,RDW_INTERNALPAINT);
	return 0;
}

