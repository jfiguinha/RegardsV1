// WinMain.cpp: implementation of the CWinMain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WinMain.h"
#include "objet.h"
#include "WinMainData.h"
#include "affichagebitmap.h"
#include "TraitementBitmap.h"
#include "CoolMenu.h"
#include "parameter.h"
#include <definerepmonitor.h>
#include <convertetrenommage.h>
#include <aboutdialog.h>
#define IDM_REDRAWWINDOW 0x01000
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWinMain::CWinMain()
{
	m_cWinMainData = new CWinMainData();
	hThreadTraitement = NULL;
	hThread = NULL;
#ifdef COOLMENU
	m_CCoolMenu = new CCoolMenu();
#endif
	m_hWnd = NULL;
	dwThreadId = 0; 
	dwThreadTraitementId = 0;  
	m_bCommandLine = false;
	m_bExplorer = false;
}

CWinMain::~CWinMain()
{
	m_cWinMainData->m_cParameter->SetOptionExploration(m_cWinMainData->GetQualite(),m_cWinMainData->GetImage(),m_cWinMainData->GetDirectory(),m_cWinMainData->GetModeAffichage());

	if(m_cWinMainData != NULL)
		delete m_cWinMainData;
	
#ifdef COOLMENU
	if(m_CCoolMenu != NULL)
		delete m_CCoolMenu;
#endif
}

////////////////////////////////////////////////
//Thread d'affichage
////////////////////////////////////////////////

ULONG __stdcall  CWinMain::AffichageBMP(LPVOID parametre)
{
	OleInitialize(NULL);

	CWinMain * m_pWinMain = (CWinMain * ) parametre;
	CAffichageBitmap * m_cAffichageBMP = new CAffichageBitmap();
	m_cAffichageBMP->CreateScreenWindow(m_pWinMain->m_hWnd, hInstance, SW_SHOW, m_pWinMain->m_cWinMainData->GetFile(), m_pWinMain->m_cWinMainData->GetOrderBy());
	m_pWinMain->m_bExplorer = m_cAffichageBMP->GetExplorer();
	delete m_cAffichageBMP;

	if(m_pWinMain->m_bExplorer && !m_pWinMain->m_bCommandLine)
	{
		SendMessage(m_pWinMain->m_hWnd,WM_COMMAND,MAKEWPARAM(IDM_REDRAWWINDOW,0),0);
		::ShowWindow(m_pWinMain->m_hWnd,SW_SHOWMAXIMIZED);
		::UpdateWindow(m_pWinMain->m_hWnd);
		BringWindowToTop(m_pWinMain->m_hWnd);
	}
	else if(m_pWinMain->m_bCommandLine && !m_pWinMain->m_bExplorer)
	{
		DestroyWindow(m_pWinMain->m_hWnd);
		OleUninitialize();
		exit(0);
	}

	m_pWinMain->m_bCommandLine = false;
	//ExitThread(0);
	OleUninitialize();
	return 0;
}

////////////////////////////////////////////////
//Thread d'affichage
////////////////////////////////////////////////

ULONG __stdcall CWinMain::TraitementBMP(LPVOID parametre)
{
	OleInitialize(NULL);
	CWinMain * m_pWinMain = (CWinMain * ) parametre;
	CTraitementBitmap * m_cTraitementBMP = new CTraitementBitmap();
	m_cTraitementBMP->SetFilename(m_pWinMain->m_cWinMainData->GetFile());
	RECT  rc = {0, 0, GetDeviceCaps(GetDC(NULL),HORZRES), GetDeviceCaps(GetDC(NULL),VERTRES)};
	DWORD dwndStlyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_MAXIMIZE;
	DWORD dStyle = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	DWORD dExStyle = 0;

	char m_cTempMessage[255];
	LoadString(hInstance,IDS_MODTRAITEMENT,m_cTempMessage,255);

	m_cTraitementBMP->SetBackgroundColor((HBRUSH)GetStockObject(BLACK_BRUSH));
	m_cTraitementBMP->CreateEx(NULL,hInstance,dwndStlyle,dStyle,dExStyle,m_cTempMessage,IDM_REGARDS,IDM_TRAITEMENTIMAGE,IDC_ARROW,rc);
	m_cTraitementBMP->SetParent(m_pWinMain->m_hWnd);
	m_cTraitementBMP->Run(IDR_TRTBITMAP);

	delete m_cTraitementBMP;

	m_pWinMain->m_cWinMainData->SetCreateWndTraitement(false);
	//m_pWinMain->pWinData.bCreateWndTraitement = false;
	//BringWindowToTop(m_pWinMain->m_hWnd);
	//ExitThread(0);
	OleUninitialize();
	return 0;
}

void CWinMain::SelectAlbum(HWND hwnd)
{
	OPENFILENAME ofn;
#ifdef ENGLISH
	TCHAR szFilter[] = TEXT ("Files DB  (*.DB)\0*.db\0");
#endif
#ifdef SPANISH
	TCHAR szFilter[] = TEXT ("Archivos DB  (*.DB)\0*.db\0");
#endif
#ifdef FRENCH
	TCHAR szFilter[] = TEXT ("Fichiers DB  (*.DB)\0*.db\0");
#endif

	TCHAR pstrTitleName[MAX_PATH];
	TCHAR pstrFileName[MAX_PATH];
	
	ofn.lStructSize       = sizeof (OPENFILENAME) ;
	ofn.hwndOwner         = hwnd ;
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
	ofn.lpstrDefExt       = TEXT ("db") ;
	ofn.lCustData         = 0L ;
	ofn.lpfnHook          = NULL ;
	ofn.lpTemplateName    = NULL ;
	ofn.lpstrFile         = pstrFileName ;
	ofn.lpstrFileTitle    = pstrTitleName ;
	ofn.Flags             = OFN_OVERWRITEPROMPT;

	if (GetOpenFileName(&ofn))
	{
		char m_szDirectory[MAX_PATH];
		TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
		_splitpath_s(ofn.lpstrFile, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);	
		StringCchCopy(m_szDirectory,MAX_PATH,drive);
		StringCbCat(m_szDirectory,MAX_PATH,dir);
		m_cWinMainData->m_cParameter->SetOptionAlbumDir(m_szDirectory);
		CObjet::AffichageMessage(IDS_MODIFREDEMARRAGE,IDS_INFOS);
		
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
//Gestion des événements de Commande
//////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWinMain::OnCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	//CObjet m_cObjet;

	wmId    = LOWORD(wParam); 
	wmEvent = HIWORD(wParam); 
	// Parse the menu selections:
	switch (wmId)
	{
		case IDM_REDRAWWINDOW:
			{
				::ShowWindow(hwnd,SW_SHOWMAXIMIZED);
				::UpdateWindow(hwnd);
				BringWindowToTop(hwnd);
			}
			break;

		case ID_AFFICHAGE_SAUVERLESMINIATURES:
			{
				CParameter * m_cParameter = new CParameter();
				if(CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_SAUVERLESMINIATURES,MF_UNCHECKED) == MF_UNCHECKED)
				{
					CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_SAUVERLESMINIATURES,MF_CHECKED);
					m_cParameter->SetSaveOptionMiniature(1);
				}
				else
				{
					CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_SAUVERLESMINIATURES,MF_UNCHECKED);
					m_cParameter->SetSaveOptionMiniature(0);
				}
				
				delete m_cParameter;
			}
			break;

		case IDM_SITEWEB:
			{
				char m_Site[255];
				LoadString(hInstance,IDS_ADRESSEWEB,m_Site,255);
				ShellExecute(NULL,"open","iexplore",m_Site,NULL,SW_SHOW);
			}
			break;

		case ID_MINIATURESIZE:
			{
				SendMessage(m_cWinMainData->GetTreeWindow(), WM_COMMAND, MAKEWPARAM(ID_MINIATURESIZE, 0),0);
				break;
			}

		case ID_OUTILS_CONVERSIONETRENOMMAGEDEFICHIERS:
			{
				CConvertEtRenommage * m_cConvertEtRenommage = new CConvertEtRenommage(hInstance);
				m_cConvertEtRenommage->CreateDlg(hInstance,(LPCTSTR)IDD_CONVERTRENOMMAGE, hwnd);
				delete m_cConvertEtRenommage;
			}
			break;

		case ID_ENDINITIALIZE_FODER:
			if(m_cWinMainData->GetWindowIntro() != NULL)
			{
				::ShowWindow(m_cWinMainData->GetWindowIntro(),SW_HIDE);
			}
			break;

		case ID_CATALOGUE_SELECTIONDUNALBUM:
			{
				SelectAlbum(hwnd);
				break;
			}
			break;

		case ID_DO_IMAGE:
			SendMessage(m_cWinMainData->GetTreeWindow(),WM_COMMAND,wParam,lParam);
			break;

		case ID_AIDE_AIDE:
			{
				char clpText[255];
				LoadString(hInstance,IDS_HELPFILENAME,clpText,255);
				ShellExecute(NULL, "open", clpText, NULL, NULL, SW_SHOWNORMAL );
			}
			break;

		case ID_EDITION_ASSOCIERREGARDSAUXIMAGES:
			{
				CObjet m_cObjet;
				m_cObjet.CreateAssociation(hwnd);
			}
			break;

		case ID_CATALOGUE_NOUVELALBUM:
		case IDM_NEW:
			SendMessage(m_cWinMainData->GetListWindow(), WM_COMMAND, MAKEWPARAM(IDM_NEW, 0),0);
			break;

		
		case ID_OUTILS_AFFICHAGEDOSSIER:
			m_cWinMainData->AffichageDossierRepertoire(hwnd, true, false);
			break;

		case ID_OUTILS_AFFICHAGERECHERCHE:
			m_cWinMainData->AffichageDossierRecherche(hwnd, false, true);
			break;

		case ID_AFFICHAGE_CONTROLE_RECHERCHE:
			m_cWinMainData->AffichageDossierRecherche(hwnd, false, true);
			break;

		case ID_AFFICHAGE_CONTROL_FOLDERTREE:
			m_cWinMainData->AffichageDossierRepertoire(hwnd, true, false);
			break;
		//Couper
		case IDS_CUT:
		//Coller
		case IDS_PASTE:
		//Coller
		case IDS_COPY:
		case ID_AFFICHAGE_ACTUALISER:
		case IDM_ALBUMIMAGELIST:
		case ID_AFFICHAGE_REGENERERLESMINIATURES:
		case ID_EDITION_SELECTALLFILES:
		case ID_EDITION_RECHERCHERUNFICHIER:
		case IDM_INITCATALOGUE:
		case IDM_INITALBUM:
			SendMessage(m_cWinMainData->GetTreeWindow(),WM_COMMAND,wParam,lParam);
			break;

		case IDM_EDITION_SUPPRIMER:
			SendMessage(m_cWinMainData->GetTreeWindow(),WM_COMMAND,MAKEWPARAM(IDM_DELFILE,0),0);
			break;

		case ID_AFFICHAGE_AFFICHERLESDIMENSIONSDESIMAGES:
			{
				m_cWinMainData->AffichageCheckMenuState(hwnd,ID_AFFICHAGE_AFFICHERLESDIMENSIONSDESIMAGES);
				break;
			}
		

		case IDM_FICHIER_SAUVERALBUM:
		case ID_FICHIER_SAUVERALBUM:
		case ID_LISTE_CRERUNRPERTOIRE:
		case IDM_EDITION_REPERTOIRESUP:
		case ID_CATALOGUE_CRERUNCATALOGUE:
		case ID_CATALOGUE_RECHERCHER:
		case ID_CATALOGUE_COMPACTERLABASE:
		case ID_RECUP_PTATTRIBUT:
		case ID_ALBUM_SUPPRIMER:
		case IDM_SAVEALBUMTHUMBMAIL:
		case IDM_SAVEALBUM:
		case ID_LISTE_GESTIONSDESATTRIBUTS:
		case ID_CATALOGUE_GESTIONDESCATGORIES:
		case ID_LISTE_AJOUTSFICHIERSDANSCATALOGUE:
		case ID_LISTE_AJOUTSFICHIERSDIRECTDANSCATALOGUE:
			SendMessage(m_cWinMainData->GetListWindow(), WM_COMMAND, wParam, lParam);
			break;


		case ID_AFFICHAGE_CONTROLE_INFOSFICHIER:
			{
				if(wmEvent == 99)
				{
					SendMessage(m_cWinMainData->GetTreeWindow(),WM_COMMAND,wParam,lParam);
					break;
				}

				m_cWinMainData->AffichageCheckMenuState(hwnd,ID_AFFICHAGE_CONTROLE_INFOSFICHIER);
			}
			break;

		case ID_OUTILS_MISEENPAPIERPEINT_ETIRER:
			{
				CObjet m_cObjet;
				if(m_cObjet.TestImage(m_cWinMainData->GetFile()))
					m_cObjet.SetWallpaper(m_cWinMainData->GetFile(), "2","0", m_cWinMainData->GetTreeWindow());
			}
			break;

		case ID_OUTILS_MISEENPAPIERPEINT_MOSAIQUE:
			{
				CObjet m_cObjet;
				if(m_cObjet.TestImage(m_cWinMainData->GetFile()))
					m_cObjet.SetWallpaper(m_cWinMainData->GetFile(), "1","1", m_cWinMainData->GetTreeWindow());
			}
			break;

		case ID_OUTILS_MISEENPAPIERPEINT_CENTRER:
			{
				CObjet m_cObjet;
				if(m_cObjet.TestImage(m_cWinMainData->GetFile()))
					m_cObjet.SetWallpaper(m_cWinMainData->GetFile(), "1","0", m_cWinMainData->GetTreeWindow());
			}
			break;

		case ID_AFFICHAGE_AFFICHERUNIQUEMENTLESIMAGES:
			m_cWinMainData->SetImage(!m_cWinMainData->GetImage());
			m_cWinMainData->AffichageCheckMenuState(hwnd, ID_AFFICHAGE_AFFICHERUNIQUEMENTLESIMAGES);
			break;

		case IDM_FILENAME:
			m_cWinMainData->SetFile((const char *)lParam);
			break;
		
		case IDM_DIRNAME:
			m_cWinMainData->SetDirectory((const char *)lParam);
			break;

		case IDM_GETTRAITEMENTWND:
			{
				m_cWinMainData->AddTraitementWindow((HWND)lParam);
				break;
			}

		case IDM_DESTROYTRAITEMENT:
			{
				m_cWinMainData->RemoveTraitementWindow((HWND)lParam);
				break;
			}

		case IDM_GETAFFICHAGEWND:
			m_cWinMainData->SetCreateScreen(true);
			m_cWinMainData->SetAffichageBitmapHandle((HWND)lParam);

			if(m_cWinMainData->GetAffichageAlbum())
			{
				m_cWinMainData->SetAffichageAlbum(0);
				SendMessage(m_cWinMainData->GetAffichageBitmapHandle(),WM_COMMAND,MAKEWPARAM(IDM_ALBUMNAME,0),(LPARAM)m_cWinMainData->GetIconeFileVector());
			}
			else
 				SendMessage(m_cWinMainData->GetAffichageBitmapHandle(),WM_COMMAND,MAKEWPARAM(IDM_DIRNAME,0),(LPARAM)m_cWinMainData->GetDirectory());
			break;

		case IDM_DELAFFICHAGEWND:
			m_cWinMainData->SetCreateScreen(false);
			m_cWinMainData->SetAffichageBitmapHandle(NULL);
			break;

		case IDM_GETHWNDPRGBAR:
			CDefineRepMonitor * m_define;
			m_define = (CDefineRepMonitor *)lParam;
			m_define->hWnd = m_cWinMainData->GetWindowProgressBarHandle();
			break;

		case IDM_CONTEXTHANDLE:
			{
				IContextMenu2 * g_pcm2 = (IContextMenu2 *)lParam;
				g_pcm2 = m_cWinMainData->GetContextMenu();
			}
			break;

		case IDM_DRAGGING:
			m_cWinMainData->SetDragging(wmEvent);
			SendMessage(m_cWinMainData->GetListWindow(),WM_COMMAND,MAKEWPARAM(IDM_DRAGGING,wmEvent),lParam);
			break;

		case IDM_DRAGGING_TREE:
			m_cWinMainData->SetDraggingTree(wmEvent);
			break;

		case IDM_HELP_APROPOS:
			{
				CAboutDialog * m_CDialogAbout = new CAboutDialog(hInstance);
				m_CDialogAbout->CreateDlg(hInstance,(LPCTSTR)IDD_ABOUTBOX, hwnd);
				delete m_CDialogAbout;
			}
			break;

		case IDM_EXIT:
		   DestroyWindow(hwnd);
		   break;

		case IDM_INITDIRECTORY:
			m_cWinMainData->SetDirectory((const char *)lParam);
			SendMessage(m_cWinMainData->GetTreeWindow(),WM_COMMAND,MAKEWPARAM(IDM_INITLIST,0),lParam);
			break;

		case IDM_LISTCLICK:
			{
				CObjet m_cObjet;
				char m_cFileTemp[_MAX_FNAME];
				StringCchCopy(m_cFileTemp,_MAX_FNAME,(const char *)lParam);

				if(::IsWindowVisible(m_cWinMainData->GetAffichageBitmapHandle()))
				{
					if(m_cObjet.TestImage(m_cFileTemp, false))
					{
						SendMessage(m_cWinMainData->GetAffichageBitmapHandle(),WM_COMMAND,MAKEWPARAM(IDM_FILENAME,0),(LPARAM)m_cFileTemp);
						SendMessage(m_cWinMainData->GetAffichageBitmapHandle(),WM_NOTIFY,0,MAKELPARAM(1,1));
					}

				}

				m_cWinMainData->SetFile(m_cFileTemp);
			}
			break;

		case IDM_AFFICHAGE_IMAGE:
			{
				CObjet m_cObjet;
				wmEvent = EVENT_IMAGE;
				lParam = (LPARAM)m_cWinMainData->GetFile();
				if(!m_cObjet.TestImage(m_cWinMainData->GetFile()))
					return 0;
			}

		case IDM_LISTDBLCLICK:
			{
				CObjet m_cObjet;
				switch(wmEvent)
				{
					case EVENT_IMAGE:
						m_cWinMainData->SetFile((const char *)lParam);

	#ifdef WMP9
						if(!m_cObjet.TestImage(m_cWinMainData->GetFile()) && !m_cObjet.TestMultimediaFormat(m_cWinMainData->GetFile()))
							return 0;

	#else
						if(!m_cObjet.TestImage(m_cWinMainData->GetFile()))
							return 0;

	#endif

						if(!m_cWinMainData->GetCreateScreen())
						{
							if(hThread != NULL)
								CloseHandle(hThread);

							hThread = (HANDLE)CreateThread(NULL, cuStackSize,
								AffichageBMP, this, 0, &dwThreadId);

							SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);

							m_cWinMainData->SetCreateScreen(true);

						}
						else
						{
							RECT rect;
							GetWindowRect(m_cWinMainData->GetAffichageBitmapHandle(), &rect);
							SendMessage(m_cWinMainData->GetAffichageBitmapHandle(),WM_COMMAND,MAKEWPARAM(IDM_FILENAME,m_cWinMainData->GetOrderBy()),(LPARAM)m_cWinMainData->GetFile());
							SetWindowPos(m_cWinMainData->GetAffichageBitmapHandle(), hwnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);
						}
						break;

					case EVENT_FOLDER:
						{
							char m_cTempFolder[MAX_PATH];
							m_cWinMainData->SetDirectory((const char *)lParam);
							strcpy_s(m_cTempFolder,MAX_PATH,m_cWinMainData->GetDirectory());
							SendMessage(m_cWinMainData->GetListWindow(),WM_COMMAND,MAKEWPARAM(IDM_DIRNAME,0),(LPARAM)m_cWinMainData->GetDirectory());
							//Envoyé un message à Tree pour ouvrir arbre
							SendMessage(m_cWinMainData->GetListWindow(),WM_COMMAND,MAKEWPARAM(IDM_OUVRIR_ARBRE,0),0);
							SendMessage(m_cWinMainData->GetTreeWindow(),WM_COMMAND,MAKEWPARAM(IDM_INITLIST,0),(LPARAM)m_cTempFolder);
							m_cWinMainData->SetDirectory(m_cTempFolder);
						}
						break;

					case EVENT_ALBUM:
						m_cWinMainData->SetAffichageAlbum(1);
						m_cWinMainData->SetIconeFileVector((IconeFileVector *)lParam);
						break;

					case EVENT_FILE:
						m_cWinMainData->SetAffichageAlbum(0);
						break;
				}
			}
			break;

		case IDM_OUTILS_TRAITEMENTIMAGE:
			{
				CObjet m_cObjet;
				if(!m_cObjet.TestImageFormat(m_cWinMainData->GetFile()))
					return 0;

				CTraitementInstance m_TraitementInstance;

				m_TraitementInstance.hWnd = NULL;
				m_TraitementInstance.hThreadTraitement = (HANDLE)CreateThread(NULL, 1024,
							TraitementBMP, this, 0, &m_TraitementInstance.dwThreadId);

				m_cWinMainData->AddTraitementInstance(m_TraitementInstance);

				SetThreadPriority(m_TraitementInstance.hThreadTraitement, THREAD_PRIORITY_NORMAL);


			}
			break;

		case IDM_LARGEICON:
			m_cWinMainData->SetModeAffichage(1);
			m_cWinMainData->GestionListeAffichage(wmId, m_cWinMainData->GetTreeWindow(), hwnd, wParam);
			break;
		case IDM_SMALLICON:
			m_cWinMainData->SetModeAffichage(2);
			m_cWinMainData->GestionListeAffichage(wmId, m_cWinMainData->GetTreeWindow(), hwnd, wParam);
			break;
		case IDM_REPORTVIEW:
			m_cWinMainData->SetModeAffichage(4);
			m_cWinMainData->GestionListeAffichage(wmId, m_cWinMainData->GetTreeWindow(), hwnd, wParam);
			break;
		case IDM_LISTVIEW:
			m_cWinMainData->SetModeAffichage(3);
			m_cWinMainData->GestionListeAffichage(wmId, m_cWinMainData->GetTreeWindow(), hwnd, wParam);
			break;
		case IDM_THUMBMAIL:
			m_cWinMainData->SetModeAffichage(5);
			m_cWinMainData->GestionListeAffichage(wmId, m_cWinMainData->GetTreeWindow(), hwnd, wParam);
			break;
		case IDM_AFFICHAGE_CONTROL_PREVIEWAREA:
			m_cWinMainData->GestionListeAffichage(wmId, m_cWinMainData->GetTreeWindow(), hwnd, wParam);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_NORMAL:
			m_cWinMainData->SetQualite(0);
			CObjet::UncheckMenuStatePreviewQuality(hwnd);
			m_cWinMainData->DefineQualite(hwnd);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BESSEL:
			m_cWinMainData->SetQualite(BESSELFILTER);
			CObjet::UncheckMenuStatePreviewQuality(hwnd);
			m_cWinMainData->DefineQualite(hwnd);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BICUBIC:
			m_cWinMainData->SetQualite(CUBICFILTER);
			CObjet::UncheckMenuStatePreviewQuality(hwnd);
			m_cWinMainData->DefineQualite(hwnd);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BILINEAR:
			m_cWinMainData->SetQualite(BILINEARFILTER);
			CObjet::UncheckMenuStatePreviewQuality(hwnd);
			m_cWinMainData->DefineQualite(hwnd);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BLACKMAN:
			m_cWinMainData->SetQualite(BLACKMANFILTER);
			CObjet::UncheckMenuStatePreviewQuality(hwnd);
			m_cWinMainData->DefineQualite(hwnd);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BLACKMANBESSEL:
			m_cWinMainData->SetQualite(BLACKMANBESSELFILTER);
			CObjet::UncheckMenuStatePreviewQuality(hwnd);
			m_cWinMainData->DefineQualite(hwnd);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BLACKMANSINC:
			m_cWinMainData->SetQualite(BLACKMANSINCFILTER);
			CObjet::UncheckMenuStatePreviewQuality(hwnd);
			m_cWinMainData->DefineQualite(hwnd);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BOX:
			m_cWinMainData->SetQualite(BOXFILTER);
			CObjet::UncheckMenuStatePreviewQuality(hwnd);
			m_cWinMainData->DefineQualite(hwnd);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_CATROM:
			m_cWinMainData->SetQualite(CATROMFILTER);
			CObjet::UncheckMenuStatePreviewQuality(hwnd);
			m_cWinMainData->DefineQualite(hwnd);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_GAUSSIAN:
			m_cWinMainData->SetQualite(GAUSSIANFILTER);
			CObjet::UncheckMenuStatePreviewQuality(hwnd);
			m_cWinMainData->DefineQualite(hwnd);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_HAMMING:
			m_cWinMainData->SetQualite(HAMMINGFILTER);
			CObjet::UncheckMenuStatePreviewQuality(hwnd);
			m_cWinMainData->DefineQualite(hwnd);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_HANNING:
			m_cWinMainData->SetQualite(HANNINGFILTER);
			CObjet::UncheckMenuStatePreviewQuality(hwnd);
			m_cWinMainData->DefineQualite(hwnd);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_HERMITE:
			m_cWinMainData->SetQualite(HERMITEFILTER);
			CObjet::UncheckMenuStatePreviewQuality(hwnd);
			m_cWinMainData->DefineQualite(hwnd);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_LANCZOS:
			m_cWinMainData->SetQualite(LANCZOSFILTER);
			CObjet::UncheckMenuStatePreviewQuality(hwnd);
			m_cWinMainData->DefineQualite(hwnd);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_MITCHELL:
			m_cWinMainData->SetQualite(MITCHELLFILTER);
			CObjet::UncheckMenuStatePreviewQuality(hwnd);
			m_cWinMainData->DefineQualite(hwnd);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_QUADRATIC:
			m_cWinMainData->SetQualite(QUADRATICFILTER);
			CObjet::UncheckMenuStatePreviewQuality(hwnd);
			m_cWinMainData->DefineQualite(hwnd);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_SINC:
			m_cWinMainData->SetQualite(SINCFILTER);
			CObjet::UncheckMenuStatePreviewQuality(hwnd);
			m_cWinMainData->DefineQualite(hwnd);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_TRIANGLE:
			m_cWinMainData->SetQualite(TRIANGLEFILTER);
			CObjet::UncheckMenuStatePreviewQuality(hwnd);
			m_cWinMainData->DefineQualite(hwnd);
			
			break;

			
		default:
		   return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//Gestion des événements de notification
//////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWinMain::OnNotify(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	/////////////////////////////////////////
	//Récupération des différents messages
	//du controle treeview
	/////////////////////////////////////////

	int idButton;

	switch(LOWORD(wParam))
	{
		case IDM_HIDDENPANE:
			{
				m_cWinMainData->HiddePane();
				return 0;
			}
			break;
	}

	switch(HIWORD(lParam))
	{
		case 89:
			m_cWinMainData->SetCreateScreen(false);
			break;
	}

	switch (((LPNMHDR) lParam)->code) 
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
				case IDM_LARGEICON: 
					lpttt->lpszText = MAKEINTRESOURCE(IDS_ICONELARGE); 
					break; 
				case IDM_SMALLICON: 
					lpttt->lpszText = MAKEINTRESOURCE(IDS_ICONEPETITE); 
					break; 
				case IDM_LISTVIEW: 
					lpttt->lpszText = MAKEINTRESOURCE(IDS_ICONELIST); 
					break; 
				case IDM_REPORTVIEW: 
					lpttt->lpszText = MAKEINTRESOURCE(IDS_ICONEDETAIL); 
					break; 
				case IDM_THUMBMAIL: 
					lpttt->lpszText = MAKEINTRESOURCE(IDS_ICONEMINI); 
					break; 
				case IDM_EDITION_REPERTOIRESUP: 
					lpttt->lpszText = MAKEINTRESOURCE(IDS_REPERTOIRESUP); 
					break; 
				case IDM_AFFICHAGE_IMAGE:
					lpttt->lpszText = MAKEINTRESOURCE(IDS_AFFICHAGEIMAGE);
					break;
				case IDM_OUTILS_TRAITEMENTIMAGE:
					lpttt->lpszText = MAKEINTRESOURCE(IDS_TRAITEMENTIMAGE);
					break;

				case ID_OUTILS_AFFICHAGERECHERCHE:
					lpttt->lpszText = MAKEINTRESOURCE(IDS_AFFICHAGERECHERCHE);
					break;

				case ID_OUTILS_AFFICHAGEDOSSIER:
					lpttt->lpszText = MAKEINTRESOURCE(IDS_AFFICHAGEDOSSIER);
					break;

				case IDM_NEW:
					lpttt->lpszText = MAKEINTRESOURCE(IDS_WIZARD);
					break;
			}

		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWinMain::OnContextMenu(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	POINT ptScreen;
	POINT ptClient;

	ptScreen.x = GET_X_LPARAM(lParam);
	ptScreen.y = GET_Y_LPARAM(lParam);
	ptClient = ptScreen;
	ScreenToClient(hwnd, &ptClient);
	HWND  hwndOver = ChildWindowFromPoint(hwnd, ptClient);

	if(hwndOver == m_cWinMainData->GetTreeWindow())
	{
		SendMessage(m_cWinMainData->GetTreeWindow(),WM_COMMAND,MAKEWPARAM(WM_CONTEXTMENU,0),lParam);
	}
	else if(m_cWinMainData->GetListWindow() == hwndOver)
	{
		SendMessage(m_cWinMainData->GetListWindow(),WM_COMMAND,MAKEWPARAM(WM_CONTEXTMENU,0),lParam);
	}

	return 0;

}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

void CWinMain::ShowIntro(const BOOL &m_bShow)
{
	m_cWinMainData->ShowIntro(m_bShow);
}

void CWinMain::SetIntroWindow(HWND hWnd)
{
	m_cWinMainData->SetIntroWindow(hWnd);
}

void CWinMain::SetCommandLine(char * szCommandLine)
{
	m_cWinMainData->SetFile(szCommandLine);
	m_bCommandLine = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////


LRESULT CWinMain::OnCreate(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	m_hWnd = hwnd;
	HMENU hmenu, hSubMenu,hsub;
	char m_szMessage[255];

	if(m_bCommandLine)
	{
		AffichageBMP(this);
		return 0;
	}

	//Initialisation des classes

	if(m_cWinMainData->GetWindowIntro() != NULL)
	{

		LoadString(hInstance,IDS_STGCREATIONFENETRE,m_szMessage,255);
		SendMessage(m_cWinMainData->GetWindowIntro(),WM_COMMAND,MAKEWPARAM(IDM_SETTEXT,0),(LPARAM)m_szMessage);
	}
	
	m_cWinMainData->CreateWindowInstance(hwnd);

	if(m_cWinMainData->GetWindowIntro() != NULL)
	{
		//char m_szMessage[255];
		LoadString(hInstance,IDS_STGCREATIONMENU,m_szMessage,255);
		SendMessage(m_cWinMainData->GetWindowIntro(),WM_COMMAND,MAKEWPARAM(IDM_SETTEXT,0),(LPARAM)m_szMessage);

	}

	//Gestion du COOLMENU
#ifdef COOLMENU
	hmenu = GetMenu(hwnd);
	for(int i = 0;i < 7;i++)
	{
		hsub  = GetSubMenu(hmenu, i);
		m_CCoolMenu->MakeCoolMenu(hsub, hInstance, MAKEINTRESOURCE(IDB_MENUBMP));
	}

	hsub  = GetSubMenu(hmenu, 2);
	hSubMenu = GetSubMenu(hsub, 0);
	m_CCoolMenu->MakeCoolMenu(hSubMenu, hInstance, MAKEINTRESOURCE(IDB_MENUBMP));
	hSubMenu = GetSubMenu(hsub, 1);
	m_CCoolMenu->MakeCoolMenu(hSubMenu, hInstance, MAKEINTRESOURCE(IDB_MENUBMP));
	hSubMenu = GetSubMenu(hsub, 12);
	m_CCoolMenu->MakeCoolMenu(hSubMenu, hInstance, MAKEINTRESOURCE(IDB_MENUBMP));
	hsub  = GetSubMenu(hmenu, 5);
	hSubMenu = GetSubMenu(hsub, 5);
	m_CCoolMenu->MakeCoolMenu(hSubMenu, hInstance, MAKEINTRESOURCE(IDB_MENUBMP));
#endif

	RECT rc;
	HDC hdc = GetDC(hwnd);
	GetClientRect(hwnd,&rc);
	rc.top += 40;
	rc.bottom -= 20;
	FillRect(hdc, &rc, CreateSolidBrush(GetSysColor(COLOR_BTNFACE))); 
	ReleaseDC(hwnd,hdc);

	m_cWinMainData->DefineQualite(hwnd);

	//Seulement la liste des images
	if(m_cWinMainData->GetImage())
		m_cWinMainData->AffichageCheckMenuState(hwnd, ID_AFFICHAGE_AFFICHERUNIQUEMENTLESIMAGES);

	SendMessage(m_cWinMainData->GetListWindow(),WM_COMMAND,MAKEWPARAM(ID_INITIALIZE_FOLDER,0),(LPARAM)m_cWinMainData->GetDirectory());

	m_cWinMainData->DefineModeAffichage(hwnd);

	if(m_cWinMainData->GetWindowIntro() != NULL)
	{
		//char m_szMessage[255];
		LoadString(hInstance,IDS_SGINITPARAM,m_szMessage,255);
		SendMessage(m_cWinMainData->GetWindowIntro(),WM_COMMAND,MAKEWPARAM(IDM_SETTEXT,0),(LPARAM)m_szMessage);

	}
	//Transformation en spécial folder si c'est le cas !

	int iReturn = CParameter::LoadWindowPlacement("SOFTWARE\\Regards\\exploreur",hwnd);
	if(iReturn == -1)
	{
		WINDOWPLACEMENT _windowPlacement;
		GetWindowPlacement(hwnd,&_windowPlacement);
		_windowPlacement.showCmd = SW_SHOWMAXIMIZED;
		SetWindowPlacement(hwnd,&_windowPlacement);
	}

	int m_iOption = 0;
	CParameter * m_cParameter = new CParameter();
	m_cParameter->GetSaveOptionMiniature(m_iOption);
	delete m_cParameter;

	if(m_iOption)
		CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_SAUVERLESMINIATURES,MF_CHECKED);
	else
		CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_SAUVERLESMINIATURES,MF_UNCHECKED);	

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CWinMain::OnActivate(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
 	SendMessage(m_cWinMainData->GetSplitterWindow(), msg,wParam,lParam);
 	SendMessage(m_cWinMainData->GetToolBarWindow(), msg,wParam,lParam);
 	SendMessage(m_cWinMainData->GetStatusWindow(), msg,wParam,lParam);

	/*
	WINDOWPLACEMENT _windowPlacement;
	GetWindowPlacement(hWnd,&_windowPlacement);
	_windowPlacement.showCmd = SW_SHOWMAXIMIZED;
	SetWindowPlacement(hWnd,&_windowPlacement);*/
 	return 0;
}

LRESULT CWinMain::OnEraseBackground(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	GetClientRect(m_cWinMainData->GetSplitterWindow(),&rc);
	RedrawWindow(m_cWinMainData->GetSplitterWindow(),&rc,NULL,RDW_INTERNALPAINT);
	GetClientRect(m_cWinMainData->GetToolBarWindow(),&rc);
	RedrawWindow(m_cWinMainData->GetToolBarWindow(),&rc,NULL,RDW_INTERNALPAINT);
	GetClientRect(m_cWinMainData->GetStatusWindow(),&rc);
	RedrawWindow(m_cWinMainData->GetStatusWindow(),&rc,NULL,RDW_INTERNALPAINT);
	return 0;
}

LRESULT CWinMain::OnPaint(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint( hwnd, &ps );
	SendMessage(m_cWinMainData->GetSplitterWindow(), msg,wParam,lParam);
	SendMessage(m_cWinMainData->GetToolBarWindow(), msg,wParam,lParam);
	SendMessage(m_cWinMainData->GetStatusWindow(), msg,wParam,lParam);
	EndPaint( hwnd, &ps );
	ReleaseDC(hwnd,hDC);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWinMain::OnMeasureItem(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	m_cWinMainData->CreateContextMenu(msg, wParam, lParam);

#ifdef COOLMENU
	if(wParam == 0)
		return m_CCoolMenu->CoolMenu_MeasureItem(wParam, lParam);
#endif
	
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

LRESULT CWinMain::OnDrawItem(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	m_cWinMainData->CreateContextMenu(msg, wParam, lParam);
#ifdef COOLMENU
	if(wParam == 0)
		return m_CCoolMenu->CoolMenu_DrawItem(wParam, lParam);
#endif
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

LRESULT CWinMain::OnKeyUp(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int i = 0;
	switch (wParam) 
	{
		case VK_CONTROL:
			if(m_cWinMainData->GetDraggingTree() || m_cWinMainData->GetDragging())
			{
				SetCursor(LoadCursor (NULL, IDC_ARROW)) ;
			}
			break;
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
LRESULT CWinMain::OnKeyDown(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int i = 0;
	switch (wParam) 
	{
		case VK_CONTROL:
			if(m_cWinMainData->GetDraggingTree() || m_cWinMainData->GetDragging())
			{
				//Chargement du curseur plus
				SetCursor (LoadCursor (hInstance, MAKEINTRESOURCE(IDC_PTPLUS))) ;
			}
			break;

		case VK_ESCAPE:
			//Stop tout travail en cours
			ReleaseCapture();
			m_cWinMainData->SetDraggingTree(0);
			m_cWinMainData->SetDragging(0);
			SendMessage(m_cWinMainData->GetListWindow(),WM_COMMAND,MAKEWPARAM(IDM_STOPWORKING,0),0);
			SendMessage(m_cWinMainData->GetTreeWindow(),WM_COMMAND,MAKEWPARAM(IDM_STOPWORKING,0),0);
			break;
		
		case VK_F1:
			ShellExecute(NULL, "open", "regards.chm", NULL, NULL, SW_SHOWNORMAL );
			break;

		case VK_F5:
			SendMessage(m_cWinMainData->GetTreeWindow(),WM_COMMAND,MAKEWPARAM(ID_AFFICHAGE_ACTUALISER,0),0);

	}

	return 0;

}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

LRESULT CWinMain::OnSize(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	m_cWinMainData->ShowPane();

	int iRight = LOWORD(lParam);
	int iBottom = HIWORD(lParam);

	//iBottom -= 55;

	RECT rc;
	::GetClientRect(m_cWinMainData->GetStatusWindow(),&rc);
	iBottom -= rc.bottom;
	::GetClientRect(m_cWinMainData->GetToolBarWindow(),&rc);
	iBottom -= rc.bottom;

	SendMessage(m_cWinMainData->GetSplitterWindow(), msg, wParam, MAKELPARAM(iRight,iBottom));
	//pWinData.cSplitter->OnSize(hwnd, msg, wParam, MAKELPARAM(iRight,iBottom));
	m_cWinMainData->DeplaceStatusBar(hwnd);

	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

LRESULT CWinMain::OnDestroy(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	CParameter::SaveWindowPlacement("SOFTWARE\\Regards\\exploreur",hwnd);	

	HMENU hmenu, hSubMenu,hsub;
	CObjet m_cObjet;

#ifdef COOLMENU
	hmenu = GetMenu(hwnd);
	for(int i = 0;i < 7;i++)
	{
		hsub  = GetSubMenu(hmenu, i);
		m_CCoolMenu->RemoveCoolMenu(hsub);
	}

	hsub  = GetSubMenu(hmenu, 2);
	hSubMenu = GetSubMenu(hsub, 0);
	m_CCoolMenu->RemoveCoolMenu(hSubMenu);
	hSubMenu = GetSubMenu(hsub, 1);
	m_CCoolMenu->RemoveCoolMenu(hSubMenu);
	//hSubMenu = GetSubMenu(hsub, 12);
	//m_CCoolMenu->RemoveCoolMenu(hSubMenu);

	hsub  = GetSubMenu(hmenu, 5);
	hSubMenu = GetSubMenu(hsub, 5);
	m_CCoolMenu->RemoveCoolMenu(hSubMenu);

	//Suppression des classes
#endif


	m_cWinMainData->DestroyAllWindow();

	PostQuitMessage(0);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

LRESULT CWinMain::OnLButtonDown(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

LRESULT CWinMain::OnLButtonUp(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(m_cWinMainData->GetDraggingTree() || m_cWinMainData->GetDragging())
	{
		ReleaseCapture();
		m_cWinMainData->SetDraggingTree(0);
		m_cWinMainData->SetDragging(0);
		SendMessage(m_cWinMainData->GetListWindow(),WM_LBUTTONUP,0,0);
		SendMessage(m_cWinMainData->GetTreeWindow(),WM_LBUTTONUP,0,0);
		return 0;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

LRESULT CWinMain::OnMouseMove(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(m_cWinMainData->GetDragging() || m_cWinMainData->GetDraggingTree())
	{
		
		if(m_cWinMainData->GetDragging())
			SendMessage(m_cWinMainData->GetListWindow(),WM_MOUSEMOVE,wParam,lParam);

		POINT p;
		p.x = LOWORD(lParam);
		p.y = HIWORD(lParam);
		ClientToScreen(hwnd, &p);
		ImageList_DragMove(p.x, p.y);
	}

	return 0;
}

