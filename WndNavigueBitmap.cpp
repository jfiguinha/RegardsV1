// WndNavigueBitmap.cpp: implementation of the CWndNavigueBitmap class.
//
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WndNavigueBitmap.h"
#include <Regardsbitmap.h>
#include <BitmapEffect.h>
#include <diaporama.h>
#include ".\naviguedata.h"
#include "ListPreviewAffWindow.h"
#include "loadimage.h"
#include "statusbar.h"
#include "fichier.h"
#include "objet.h"
#include "scroll.h"
#include "dessin.h"
#include "print.h"
#include "registryaccess.h"
#include "parameter.h"
#include <MargeData.h>
#include <ibitmap.h>
#include <exif.h>
#include <DefineRepMonitor.h>
#include <effet.h>


using namespace Regards::libbitmap;

/////////////////////////////////////////////////////////////////////////
//Fermeture du Thread
/////////////////////////////////////////////////////////////////////////

void CWndNavigueBitmap::CloseMonitor()
{
	if(hThreadMonitor != NULL && dwThreadIdMonitor != NULL)
	{
		int bResult = PostThreadMessage(dwThreadIdMonitor, WM_NULL, 0, 0);

		Sleep(1000);
		
		CloseHandle(hThreadMonitor);
		hThreadMonitor = NULL;
	}

	if(m_define != NULL)
	{
		delete m_define;
		m_define = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////
//Thread de monitorage
/////////////////////////////////////////////////////////////////////////

ULONG _stdcall CWndNavigueBitmap::MonitorDirectoryProc(LPVOID pvThis)
{
	DWORD m_dWaitValue = WAIT_OBJECT_0;
	DWORD m_dWaitValueAdd = WAIT_OBJECT_0 + 1;
	DWORD dwWaitStatus; 
	HANDLE dwChangeHandles[1]; 
	CDefineRepMonitor * m_define = (CDefineRepMonitor *)pvThis;
 
	// Watch the C:\WINDOWS directory for file creation and 
	// deletion. 
 
	dwChangeHandles[0] = FindFirstChangeNotification( 
		m_define->DirName,                 // directory to watch 
		FALSE,                         // do not watch the subtree 
		FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES); // watch file name changes 
 
	if (dwChangeHandles[0] == INVALID_HANDLE_VALUE) 
		ExitProcess(GetLastError()); 
 
 
	// Change notification is set. Now wait on both notification 
	// handles and refresh accordingly. 
	MSG msg;
 
	while (TRUE) 
	{ 
 
		// Wait for notification.

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			break;
		}


		Sleep(2000);
 
		dwWaitStatus = MsgWaitForMultipleObjects(1, &dwChangeHandles[0], 
			FALSE, INFINITE,QS_ALLINPUT); 

        if (dwWaitStatus >= m_dWaitValue && 
            dwWaitStatus < m_dWaitValueAdd)
		{ 
			// A file was created or deleted in C:\WINDOWS. 
			// Refresh this directory and restart the 
			// change notification. RefreshDirectory is an 
			// application-defined function. 
 
				//RefreshDirectory("C:\\WINDOWS") 
				SendMessage(m_define->hWnd,WM_COMMAND,MAKEWPARAM(ID_EDITION_MISEJOUR,0),(LPARAM)m_define->DirName);
		} 

		::FindNextChangeNotification(dwChangeHandles[0]);
	} 

	return 0;
}

BOOL CWndNavigueBitmap::MonitorThreadProc(HWND hWnd, char * cDirectory)
{
	if(m_define == NULL)
	{
		m_define = new CDefineRepMonitor();
		m_define->hWnd = hWnd;
		StringCchCopy(m_define->DirName,_MAX_DIR,cDirectory);

		if(hThreadMonitor != NULL)
		{
			int bResult = PostThreadMessage(dwThreadIdMonitor, WM_NULL, 0, 0);

			Sleep(1000);
			
			CloseHandle(hThreadMonitor);
			hThreadMonitor = NULL;
		}
				
		hThreadMonitor = (HANDLE)CreateThread(NULL, cuStackSize,
					MonitorDirectoryProc, m_define, 0, &dwThreadIdMonitor);	
	}
	return true;
}
////////////////////////////////////////////////
//Thread d'affichage
////////////////////////////////////////////////

ULONG __stdcall  CWndNavigueBitmap::AffichageMiniature(LPVOID parametre)
{
	OleInitialize(NULL);
	char m_cTempMessage[255];
	LoadString(hInstance,IDS_MINIATURELIST,m_cTempMessage,255);

	CNavigueData * m_PreviewData = (CNavigueData *)parametre;
	CListPreviewAffWindow * m_cListPreviewAffWindow = m_PreviewData->GetListPreview();

	if(m_cListPreviewAffWindow == NULL)
		m_cListPreviewAffWindow = new CListPreviewAffWindow();

	m_cListPreviewAffWindow->SetNavigueWindow(m_PreviewData->GetHWnd());
	m_cListPreviewAffWindow->SetDirectory(m_PreviewData->GetCDirectory());
	//m_cListPreviewAffWindow->SetHWnd(m_PreviewData->GetHWnd());

	RECT rc;
	GetWindowRect(m_PreviewData->GetHWnd(),&rc);

	rc.left = rc.right;
	rc.right = 140;
	rc.bottom = rc.bottom - rc.top;

	DWORD dWndStyle = WS_POPUPWINDOW & WS_CAPTION | WS_VISIBLE;
	DWORD Style = CS_HREDRAW | CS_VREDRAW;
	DWORD ExStyle = 0;

	m_cListPreviewAffWindow->CreateEx(m_PreviewData->GetHWnd(),hInstance,dWndStyle,Style,ExStyle,m_cTempMessage,IDM_REGARDS,0,IDC_ARROW,rc);
	m_PreviewData->SetPreviewHWnd(m_cListPreviewAffWindow->GetHWnd());
	SendMessage(m_PreviewData->GetHWnd(),WM_COMMAND,MAKEWPARAM(ID_MINIATURE_CREATE,0),0);
	m_cListPreviewAffWindow->Run();

	ExitThread(0);
	OleUninitialize();
	return 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWndNavigueBitmap::CWndNavigueBitmap()
{
	iTrie = 0;
	m_bTransition = false;
	hThread = NULL;
	m_define = NULL;
	dwThreadIdMonitor = NULL;
	hThreadMonitor = NULL;
	m_cDiaporama = new CDiaporama(hInstance);
	m_ChargementImage = new CLoadImage();
	m_cParameter = new CParameter();

	m_cNavigueData->InitCDirectory(255);
	m_cNavigueData->InitCFile(255);
	m_cNavigueData->SetCreateMiniature(false);
	m_cNavigueData->Set3D(false);
	m_cNavigueData->Set3DDiaporama(false);
	m_cNavigueData->SetVideo(false);
	m_cNavigueData->SetIncludeSubFolder(false);

}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndNavigueBitmap::~CWndNavigueBitmap()
{
	//Sauvegarde de la position de la fenetre



	if(m_cDiaporama != NULL)
		delete m_cDiaporama;

	m_cDiaporama = NULL;

	if(m_ChargementImage != NULL)
		delete m_ChargementImage;

	m_ChargementImage = NULL;

	if(hThread != NULL)
		CloseHandle(hThread);

	if(hThreadMonitor != NULL)
	{
		CloseMonitor();
	}

	if(m_cParameter != NULL)
		delete m_cParameter;

	m_cParameter = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CWndNavigueBitmap::AffichageImage(CIBitmap * m_ciBitmap,HWND hWnd, char * cFile,Regards::libbitmap::CBitmap &m_WndBitmap)
{
	#pragma omp critical(z)
	{
		m_ciBitmap->SetFilename(cFile);
		m_cNavigueData->SetShrink(true);
		m_cNavigueData->GetCBitmap()->SetShrinkImage(true);
		CObjet m_cObjet;
		CIBitmap * m_ciBitmapThread = new CIBitmap();
		m_ciBitmapThread->SetFilename(cFile);
		bool m_bLoad = false;


		if(CObjet::GetWindowsVersion() && m_cParameter->GetStartDiaporama() == 0)
		{
			int iFormat = m_cObjet.TestImageFormat(m_cNavigueData->GetCFile());
			char cInfos[255];
			m_cObjet.GetInfosFile(m_cNavigueData->GetCFile(),cInfos,255);
			SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_UPDATESTATUSBAR,1),(LPARAM)cInfos);
			SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_CHANGEMENT_IMAGE,0),(LPARAM)cFile);

			//if(!m_ciBitmap->GetUse())
			//{
				//Test si format 12 et chargement en static
				if(iFormat == 12)
				{
					LoadingImageFile(m_ciBitmap,hWnd,cFile,m_WndBitmap);
				}
				else
				{
					if(m_ChargementImage->ChargementImage(m_ciBitmapThread, cFile, hWnd, m_cNavigueData->GetFullScreen()))
						m_bLoad = true;

				}
			//}	
		}
		else
		{
			//if(!m_ciBitmap->GetUse())
			LoadingImageFile(m_ciBitmap,hWnd,cFile,m_WndBitmap);
		}

		if(!m_bLoad)
			delete m_ciBitmapThread;
	}
}

/*
void CWndNavigueBitmap::TestOrientation(HWND hWnd, CIBitmap * ciBitmap)
{
	CObjet m_cObjet;
	int m_iOrientation = 1;
	int iFormatImage = m_cObjet.TestImageFormat(m_cNavigueData->GetCFile());
	if(iFormatImage == 1)
	{
		Cexif * m_cExif = new Cexif();
		FILE *hSource;
		errno_t err;
		err = fopen_s(&hSource, m_cNavigueData->GetCFile(), "rb");

		if(err == 0)
		{
			m_cExif->DecodeExif(hSource);
			if(m_cExif->m_exifinfo->IsExif)
				m_iOrientation = m_cExif->m_exifinfo->Orientation;

			fclose(hSource);
		}
		delete m_cExif;
	}

	if(m_iOrientation == 6)
	{
		ciBitmap->HorzFlipBuf();
		ciBitmap->Rotation90();
	}
	if(m_iOrientation == 8)
	{
		ciBitmap->Rotation90();
	}
}*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CWndNavigueBitmap::GetOptionDiaporama()
{
	int m_iSequence = 0;
	int m_iDelai = 0;
	bool m_bAutoSize = false;
	int m_iQualite = 0;
	bool m_bFullScreen = false;
	bool m_b3D = false;
	int m_iQuality3D = 2;

	CParameter::GetOptionDiaporama(m_iSequence,m_iDelai,m_bAutoSize,m_iQualite,m_bFullScreen,m_b3D,m_iQuality3D);

	m_cNavigueData->SetSequence(m_iSequence);
	m_cNavigueData->SetDuree(m_iDelai);
	m_cNavigueData->SetShrink(m_bAutoSize);
	m_cNavigueData->SetHauteQualite(m_iQualite);
	m_cNavigueData->SetFullScreen(m_bFullScreen);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CWndNavigueBitmap::LoadingImageFile(CIBitmap * m_ciBitmap,HWND hWnd, char * cFile,Regards::libbitmap::CBitmap &m_WndBitmap)
{
	CObjet m_cObjet;
	char cInfos[255];
	HCURSOR  hCur = LoadCursor(NULL, IDC_WAIT);
	hCur = SetCursor(hCur);

	m_ciBitmap->hWnd = hWnd;
	m_ChargementImage->ChargementImageStatic(m_ciBitmap, cFile, m_cNavigueData->GetFullScreen());
	m_WndBitmap.AffichageBitmap(hWnd, m_ciBitmap, 0, 0);

	SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_CHANGEMENT_IMAGE,0),(LPARAM)cFile);
	//Récupération des infos sur le fichier
	m_cObjet.GetInfosFile(m_cNavigueData->GetCFile(),cInfos,255);
	SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_UPDATESTATUSBAR,1),(LPARAM)cInfos);

	StringCchPrintf(cInfos,255,"%dx%d",m_cNavigueData->GetCIBitmap()->GetBitmapWidth(), m_cNavigueData->GetCIBitmap()->GetBitmapHeigth());
	SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(IDM_UPDATESTATUSBAR,2), (LPARAM)cInfos);
	SetCursor(LoadCursor(NULL, IDC_ARROW));
}

void CWndNavigueBitmap::SetHauteQualitePreview(HWND hWnd, int iQualite, int iChoix)
{
	CObjet::UncheckMenuStatePreviewQuality(GetParent());
	m_cNavigueData->SetHauteQualite(iQualite);
	m_cNavigueData->GetCBitmap()->SetHauteQualite(m_cNavigueData->GetHauteQualite());
	AffichageImage(m_cNavigueData->GetCIBitmap(),hWnd,m_cNavigueData->GetCFile(),*m_cNavigueData->GetCBitmap());
	CheckMenuItem(GetMenu(GetParent()),iChoix,MF_CHECKED);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndNavigueBitmap::OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	wmId    = LOWORD(wParam); 
	wmEvent = HIWORD(wParam); 

	SendMessage(::GetParent(hWnd),WM_COMMAND,MAKEWPARAM(IDM_ONCOMMANDEFFECT,0),(LPARAM)true);


	switch (wmId)
	{
		/*
		case IDM_INFOSPICTURE:
			{
				pPreviewData.hWndInfosFichier = m_cInfosFichierProc->CreateDlg(hInstance,MAKEINTRESOURCE(IDD_DLGKEYWORD),hWnd);//CreateDialogParam(hInstance,MAKEINTRESOURCE(IDD_DLGKEYWORD),hWnd,InfosFichierProc,(long)&pPreviewData);

				break;
			}*/

		case ID_IMAGE_AFFICHAGEHISTOGRAMME:
			{
				m_cNavigueData->GetCBitmap()->SetShowHistogramme(!m_cNavigueData->GetCBitmap()->GetShowHistogramme());
				SendMessage(hWnd,WM_ERASEBKGND,0,MAKELPARAM(IDM_REDRAW_PICTURE,0));
				if(m_cNavigueData->GetCBitmap()->GetShowHistogramme())
					CheckMenuItem(GetMenu(GetParent()),ID_IMAGE_AFFICHAGEHISTOGRAMME,MF_CHECKED);
				else
					CheckMenuItem(GetMenu(GetParent()),ID_IMAGE_AFFICHAGEHISTOGRAMME,MF_UNCHECKED);
			}
			break;

		case ID_IMAGES_AFFICHERINFOS:
			{
				m_cNavigueData->GetCBitmap()->SetShowInfos(!m_cNavigueData->GetCBitmap()->GetShowInfos());
				SendMessage(hWnd,WM_ERASEBKGND,0,MAKELPARAM(IDM_REDRAW_PICTURE,0));
				if(m_cNavigueData->GetCBitmap()->GetShowInfos())
					CheckMenuItem(GetMenu(GetParent()),ID_IMAGES_AFFICHERINFOS,MF_CHECKED);
				else
					CheckMenuItem(GetMenu(GetParent()),ID_IMAGES_AFFICHERINFOS,MF_UNCHECKED);
			}
			break;

		case ID_VIDEXTRACTPICTURE:
			{
				
					if(m_cNavigueData->GetVideo())
						SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(ID_VIDEXTRACTPICTURE,0),0);
				break;
			}

		case ID_IMAGE_BOUGERIMAGE: 
			{
				m_cNavigueData->SetSelect(0);
				SetCursor (LoadCursor (NULL, IDC_ARROW));
				if(CObjet::GetWindowsVersion() == 0)
					Windows98Update(hWnd, message, wParam, lParam);
				else
					SendMessage(hWnd,WM_ERASEBKGND,0,MAKELPARAM(IDM_REDRAW_PICTURE,0));
			}
			break; 


		case ID_IMAGE_ZOOMERIMAGE: 
			{
				m_cNavigueData->SetSelect(2);
				SetCursor (LoadCursor (GetModuleHandle(NULL), MAKEINTRESOURCE(IDC_ZOOMON))) ;
			}
			break; 

		case ID_EDITION_COPIER:
			{
				CIBitmap * m_ciBitmap = new CIBitmap();
				m_ciBitmap->SetBitmap(m_cNavigueData->GetCIBitmap()->hBitmap,false);
				OpenClipboard(hWnd);
				EmptyClipboard();
				SetClipboardData(CF_BITMAP,m_ciBitmap->hBitmap);
				CloseClipboard();
				delete m_ciBitmap;
			}
			break;

		case ID_IMAGE_INCLURERAW:
			{
				bool m_bValue = m_cNavigueData->GetRawFile();
				if(m_bValue)
					CheckMenuItem(GetMenu(GetParent()),ID_IMAGE_INCLURERAW,MF_UNCHECKED);
				else
					CheckMenuItem(GetMenu(GetParent()),ID_IMAGE_INCLURERAW,MF_CHECKED);

				m_cNavigueData->SetRawFile(!m_bValue);
				m_cNavigueData->GetFichier()->DefinirRepertoire(m_cNavigueData->GetCFirstFile(), iTrie, false, m_cNavigueData->GetRawFile());
				m_cNavigueData->GetFichier()->SetTrieFichier(iTrie);
				break;
			}

		case ID_IMAGES_AFFICHAGEMINIATURE:
			{
				if(!m_cNavigueData->GetCreateMiniature())
				{
					if(hThread != NULL)
						CloseHandle(hThread);

					m_cNavigueData->SetHWnd(hWnd);
				
					hThread = (HANDLE)CreateThread(NULL, cuStackSize,
							AffichageMiniature, (void *)m_cNavigueData, 0, &dwThreadTraitementId);

					SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);	

					m_cNavigueData->SetCreateMiniature(true);

					CheckMenuItem(GetMenu(GetParent()),ID_IMAGES_AFFICHAGEMINIATURE,MF_CHECKED);
				}
				else
				{
					CheckMenuItem(GetMenu(GetParent()),ID_IMAGES_AFFICHAGEMINIATURE,MF_UNCHECKED);
					SendMessage(m_cNavigueData->GetPreviewHWnd(),WM_DESTROY,0,0);
					DestroyWindow(m_cNavigueData->GetPreviewHWnd());

					if(hThread != NULL)
						CloseHandle(hThread);
					
					hThread = NULL;
					m_cNavigueData->SetCreateMiniature(false);
				}
			}
			break;

		case ID_MINIATURE_CREATE:
			{
				SendMessage(m_cNavigueData->GetPreviewHWnd(),WM_COMMAND,MAKEWPARAM(IDM_FILENAME,0),(LPARAM)m_cNavigueData->GetCFile());
			}
			break;

		case ID_EDITION_SUPPRIMER:
			{
				char clpText[255];
				char clpCaption[255];

				LoadString(hInstance,IDS_SUPPONEFILE,clpText,255);
				LoadString(hInstance,IDS_INFOS,clpCaption,255);

				if(MessageBox(NULL,clpText,clpCaption,MB_YESNO | MB_ICONWARNING) == IDYES)
				{
					m_cNavigueData->GetFichier()->DeleteFile(m_cNavigueData->GetCFile());
					
					char m_szFileName[MAX_PATH];
					m_cNavigueData->GetFichier()->SupprimerFichier(m_szFileName,MAX_PATH);

					//AffichageImage(m_ChargementImage,m_cNavigueData->GetCIBitmap(),hWnd,m_cNavigueData->GetCFile(),*m_cNavigueData->GetCBitmap());
					SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_FILENAME,0),(LPARAM)m_szFileName);
					m_cNavigueData->SetCFile(m_szFileName);
					delete[] m_szFileName;
				}
			}
			break;

		case ID_FICHIER_IMPRESSIONDIRECT:
			{
				m_cNavigueData->GetCIBitmap()->SetFilename(m_cNavigueData->GetCFile());
				CPrint::DisplayPrintPropertySheet(hInstance, hWnd, m_MargeData,1);
			}
			break;

		case ID_FICHIER_IMPRIMER:
			{
				m_cNavigueData->GetCIBitmap()->SetFilename(m_cNavigueData->GetCFile());
				CPrint::DisplayPrintPropertySheet(hInstance, hWnd, m_MargeData);
			}
			break;

		case ID_FICHIER_APERUAVANTIMPRESSION:
			{
				CPrint::DisplayPreview(hInstance, hWnd, m_MargeData);
			}
			break;


		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_NORMAL:
			{
				//CLoadImage m_ChargementImage;
				CObjet::UncheckMenuStatePreviewQuality(GetParent());
				m_cNavigueData->SetHauteQualite(false);
				m_cNavigueData->GetCBitmap()->SetHauteQualite(false);
				AffichageImage(m_cNavigueData->GetCIBitmap(),hWnd,m_cNavigueData->GetCFile(),*m_cNavigueData->GetCBitmap());
				CheckMenuItem(GetMenu(GetParent()),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_NORMAL,MF_CHECKED);
			}
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BOX:
			SetHauteQualitePreview(hWnd, BOXFILTER, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BOX);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BILINEAR:
			SetHauteQualitePreview(hWnd, BILINEARFILTER, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BILINEAR);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_GAUSSIAN:
			SetHauteQualitePreview(hWnd, GAUSSIANFILTER, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_GAUSSIAN);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_HAMMING:
			SetHauteQualitePreview(hWnd, HAMMINGFILTER, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_HAMMING);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BICUBIC:
			SetHauteQualitePreview(hWnd, CUBICFILTER, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BICUBIC);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BLACKMAN:
			SetHauteQualitePreview(hWnd, BLACKMANFILTER, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BLACKMAN);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_QUADRATIC:
			SetHauteQualitePreview(hWnd, QUADRATICFILTER, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_QUADRATIC);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_MITCHELL:
			SetHauteQualitePreview(hWnd, MITCHELLFILTER, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_MITCHELL);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_TRIANGLE:
			SetHauteQualitePreview(hWnd, TRIANGLEFILTER, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_TRIANGLE);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_SINC:
			SetHauteQualitePreview(hWnd, SINCFILTER, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_SINC);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BESSEL:
			SetHauteQualitePreview(hWnd, BESSELFILTER, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BESSEL);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BLACKMANBESSEL:
			SetHauteQualitePreview(hWnd, BLACKMANBESSELFILTER, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BLACKMANBESSEL);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BLACKMANSINC:
			SetHauteQualitePreview(hWnd, BLACKMANSINCFILTER, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BLACKMANSINC);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_LANCZOS:
			SetHauteQualitePreview(hWnd, LANCZOSFILTER, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_LANCZOS);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_HERMITE:
			SetHauteQualitePreview(hWnd, HERMITEFILTER, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_HERMITE);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_HANNING:
			SetHauteQualitePreview(hWnd, HANNINGFILTER, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_HANNING);
			break;

		case ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_CATROM:
			SetHauteQualitePreview(hWnd, CATROMFILTER, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_CATROM);
			break;

		case IDM_SETFULLSCREEN:
			{
				if(m_cNavigueData->GetVideo())
					SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_FULLSCREEN_VIDEO,IDM_FULLSCREEN_VIDEO),0);
				else
				{
					if(m_cNavigueData->GetCBitmap()->GetFullscreenState())
					{
						m_cNavigueData->SetFullScreen(false);
						m_cNavigueData->GetCBitmap()->SetFullScreen(false);
						if(!(m_cNavigueData->GetCBitmap()->GetShrinkState() && m_cNavigueData->GetCBitmap()->GetRatio() == 1.0))
							ShowScrollBar(hWnd,SB_BOTH,true);
					}
					else
					{
						m_cNavigueData->SetFullScreen(true);
						m_cNavigueData->GetCBitmap()->SetFullScreen(true);
						SetForegroundWindow(GetParent());
					}
				}
			}
			break;

		
		case ID_EDITION_SHRINKVIDEO:
			SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_SHRINK_VIDEO,IDM_SHRINK_VIDEO),0);
			break;


		case ID_IMAGES_INCLURELAFFICHAGEDESSOUSRPERTOIRES:
			{

				TCHAR temp[1024];
				if(m_cNavigueData->GetSubFolder())
				{
				
					m_cNavigueData->SetCFile(m_cNavigueData->GetCFirstFile());

					CheckMenuItem(GetMenu(GetParent()),ID_IMAGES_INCLURELAFFICHAGEDESSOUSRPERTOIRES,MF_UNCHECKED);
					m_cNavigueData->GetFichier()->DefinirRepertoire(m_cNavigueData->GetCFirstFile(), iTrie, false, m_cNavigueData->GetRawFile());
					m_cNavigueData->GetFichier()->SetTrieFichier(iTrie);
					m_cNavigueData->SetIncludeSubFolder(false);

					m_ChargementImage->ChargementImageStatic(m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetCFile(), m_cNavigueData->GetFullScreen() );

					SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_CHANGEMENT_IMAGE,0),(LPARAM)m_cNavigueData->GetCFile());
					SendMessage(hWnd,WM_ERASEBKGND,MAKEWPARAM(m_cNavigueData->GetCIBitmap()->GetBitmapWidth(),m_cNavigueData->GetCIBitmap()->GetBitmapHeigth()),MAKELPARAM(IDM_FILTREIMAGE_OK,0));


				}
				else
				{
					m_cNavigueData->SetCFirstFile(m_cNavigueData->GetCFile());
					CheckMenuItem(GetMenu(GetParent()),ID_IMAGES_INCLURELAFFICHAGEDESSOUSRPERTOIRES,MF_CHECKED);
					m_cNavigueData->GetFichier()->DefinirRepertoire(m_cNavigueData->GetCFile(), iTrie,true, m_cNavigueData->GetRawFile());
					m_cNavigueData->GetFichier()->SetTrieFichier(iTrie);
					m_cNavigueData->SetIncludeSubFolder(true);
				}

				int iNumImage = m_cNavigueData->GetFichier()->GetNbFiles();
				int iPosImage = m_cNavigueData->GetFichier()->GetPos();

				
				char m_cFile[50];
				LoadString(hInstance,IDS_FILE,m_cFile,50);

				StringCchPrintf(temp,1024,"%s : %d/%d",m_cFile,iPosImage,iNumImage);
				SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(IDM_UPDATESTATUSBAR,0), (LPARAM)temp);


			}
			break;

		case ID_DIAPORAMA_EN3D:
			{
				if(!m_cNavigueData->Get3DDiaporama())
				{
					m_cNavigueData->Set3DDiaporama(true);
					SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(IDM_SHOW_3DDIAPORAMA,0), (LPARAM)m_cNavigueData->GetFichier());
				}
				else
				{
					m_cNavigueData->Set3DDiaporama(false);
					SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_STOP_3DDIAPORAMA,0),0);
				}
			}
			break;


		case IDM_GETSHRINK:
			_itoa_s(m_cNavigueData->GetCBitmap()->GetShrinkState(),(char *)lParam, 2,2);
			break;

		case IDM_SETSHRINK:
			m_cNavigueData->GetCBitmap()->SetShrinkImage(m_cNavigueData->GetShrink());
			break;

		case ID_EDITION_MISEJOUR:
			{
				m_cNavigueData->GetFichier()->DefinirRepertoire(m_cNavigueData->GetCFile(), iTrie, m_cNavigueData->GetSubFolder(), m_cNavigueData->GetRawFile());
				m_cNavigueData->GetFichier()->SetTrieFichier(iTrie);
			}
			break;

		case IDM_FILENAME:
			{
				CObjet m_cObjet;
				TCHAR temp[1024];	
				char m_cFilenameTemp[MAX_PATH];
				strcpy_s(m_cFilenameTemp,MAX_PATH,(char *)lParam);

				if(strcmp(m_cFilenameTemp,m_cNavigueData->GetCFile()) == 0 && m_cObjet.TestImageFormat(m_cFilenameTemp) > 0)
				{
					SendMessage(hWnd,WM_ERASEBKGND,0,MAKELPARAM(IDM_REDRAW_OK,IDM_REDRAW_OK));
					return 0;
				}

				m_cNavigueData->SetCFile(m_cFilenameTemp);

				TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
				_splitpath_s(m_cNavigueData->GetCFile(), drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

				StringCchPrintf(temp,1024,"%s%s",drive,dir);
				m_cNavigueData->SetCDirectory(temp);
				iTrie = wmEvent;
				m_cNavigueData->GetCBitmap()->InitRatio();

				m_cNavigueData->GetFichier()->DefinirRepertoire(m_cNavigueData->GetCFile(), wmEvent,m_cNavigueData->GetSubFolder(), m_cNavigueData->GetRawFile());
				m_cNavigueData->GetFichier()->SetTrieFichier(wmEvent);
				m_cNavigueData->SetCFirstFile(m_cNavigueData->GetCFile());

				CloseMonitor();
				MonitorThreadProc(hWnd,temp);

#ifdef WMP9

				if(m_cObjet.TestMultimediaFormat(m_cNavigueData->GetCFile()) != 0)
				{
					if(!m_cNavigueData->GetVideo())
						m_cNavigueData->SetVideo(true);
						
					m_cNavigueData->GetCBitmap()->SetShrinkImage(m_cNavigueData->GetShrink());
					SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_PLAY_VIDEO,IDM_PLAY_VIDEO),(LPARAM)m_cNavigueData->GetCFile());

				}
				else
				{
					if(m_cObjet.Test3DFormat(m_cNavigueData->GetCFile()) != 0)
					{
						if(!m_cNavigueData->Get3D())
							m_cNavigueData->Set3D(true);

						SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_SHOW_3D,IDM_SHOW_3D),(LPARAM)m_cNavigueData->GetCFile());

					}
					else
					{
						int iFormat = m_cObjet.TestImageFormat(m_cNavigueData->GetCFile());

						//Test si format 12 et chargement en static
						if(iFormat == 12)
						{
							HCURSOR  hCur = LoadCursor(NULL, IDC_WAIT);
							hCur = SetCursor(hCur);
							m_cNavigueData->GetCIBitmap()->hWnd = hWnd;
							m_ChargementImage->ChargementImageStatic(m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetCFile(), m_cNavigueData->GetFullScreen());
							SetCursor(LoadCursor(NULL, IDC_ARROW));
						}
						else
						{
							m_ChargementImage->ChargementImageStatic(m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetCFile(), m_cNavigueData->GetFullScreen());
						}

						//m_cObjet.TestOrientation(m_cNavigueData->GetCIBitmap(),m_cNavigueData->GetCFile());

						StringCchPrintf(temp,1024,"%dx%d",m_cNavigueData->GetCIBitmap()->GetBitmapWidth(), m_cNavigueData->GetCIBitmap()->GetBitmapHeigth());
						SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(IDM_UPDATESTATUSBAR,2), (LPARAM)temp);


						char cInfos[255];
						m_cObjet.GetInfosFile(m_cNavigueData->GetCFile(),cInfos,255);
						SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_UPDATESTATUSBAR,1),(LPARAM)cInfos);
						SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_CHANGEMENT_IMAGE,0),(LPARAM)m_cNavigueData->GetCFile());
						SendMessage(hWnd,WM_ERASEBKGND,MAKEWPARAM(m_cNavigueData->GetCIBitmap()->GetBitmapWidth(),m_cNavigueData->GetCIBitmap()->GetBitmapHeigth()),MAKELPARAM(IDM_FILTREIMAGE_OK,0));

						
					}
				}
#else
				m_ChargementImage.ChargementImageStatic(*m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetCFile(), m_cNavigueData->GetFullScreen());
				StringCchPrintf(temp,1024,"%dx%d",m_cNavigueData->GetCIBitmap()->GetBitmapWidth(), m_cNavigueData->GetCIBitmap()->GetBitmapHeigth());
				SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(IDM_UPDATESTATUSBAR,2), (LPARAM)temp);


				char cInfos[255];
				m_cObjet.GetInfosFile(m_cNavigueData->GetCFile(),cInfos);
				SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_UPDATESTATUSBAR,1),(LPARAM)cInfos);
				SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_CHANGEMENT_IMAGE,0),(LPARAM)m_cNavigueData->GetCFile());
				SendMessage(hWnd,WM_ERASEBKGND,MAKEWPARAM(m_cNavigueData->GetCIBitmap()->GetBitmapWidth(),m_cNavigueData->GetCIBitmap()->GetBitmapHeigth()),MAKELPARAM(IDM_FILTREIMAGE_OK,0));

#endif


				int iNumImage = m_cNavigueData->GetFichier()->GetNbFiles();
				int iPosImage = m_cNavigueData->GetFichier()->GetPos();

				char m_cFile[50];
				LoadString(hInstance,IDS_FILE,m_cFile,50);
				StringCchPrintf(temp,1024,"%s : %d/%d",m_cFile, iPosImage,iNumImage);
				SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(IDM_UPDATESTATUSBAR,0), (LPARAM)temp);

			}
			break;

		case IDM_ALBUMNAME:
			m_cNavigueData->GetFichier()->DefinirListeImage(m_cNavigueData->GetCFile(), (IconeFileVector *)lParam);
			break;
		
		case IDM_DIRNAME:
			{
				m_cNavigueData->SetCDirectory((char *)lParam);
				CloseMonitor();
				MonitorThreadProc(hWnd,(char *)lParam);
				break;
			}

		case IDM_OPENFILE:

			if(m_cNavigueData->GetVideo())
			{
				m_cNavigueData->SetVideo(false);
				SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_STOP_VIDEO,0),0);
			}
			else if(m_cNavigueData->Get3D())
			{
				m_cNavigueData->Set3D(false);
				SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_STOP_3D,0),0);
			}
			else if(m_cNavigueData->Get3DDiaporama())
			{
				m_cNavigueData->Set3DDiaporama(false);
				SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_STOP_3DDIAPORAMA,0),0);
			}
			else
			{
				char m_szFilename[MAX_PATH];
				m_cNavigueData->GetFichier()->ChargerFichier(m_szFilename,MAX_PATH,hWnd);
				m_cNavigueData->SetCFile(m_szFilename);
				m_cNavigueData->GetCIBitmap()->SetFilename(m_szFilename);
				AffichageImage(m_cNavigueData->GetCIBitmap(),hWnd,m_cNavigueData->GetCFile(),*m_cNavigueData->GetCBitmap());
			}

			break;

		case IDM_IMAGES_SHRINKIMAGE:
			{
				if(m_cNavigueData->GetVideo())
				{
					SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_SHRINK_VIDEO,IDM_SHRINK_VIDEO),0);
				}
				else
				{
					if(m_cNavigueData->GetCBitmap()->GetShrinkState())
					{
						m_cNavigueData->SetShrink(false);
						ShowScrollBar(hWnd,SB_BOTH,true);
						m_cNavigueData->GetCBitmap()->SetShrinkImage(false);
					}
					else
					{
						m_cNavigueData->SetShrink(true);
						m_cNavigueData->GetCBitmap()->InitRatio();
						ShowScrollBar(hWnd,SB_BOTH,false);
						m_cNavigueData->GetCBitmap()->SetShrinkImage(true);
					}
					SendMessage(hWnd,WM_ERASEBKGND,MAKEWPARAM(m_cNavigueData->GetCIBitmap()->GetBitmapWidth(),m_cNavigueData->GetCIBitmap()->GetBitmapHeigth()),MAKELPARAM(IDM_FILTREIMAGE_OK,0));
				}
			}
			break;

		case IDM_FICHIER_REVERT:
			//m_ChargementImage->ChargementImage(m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetCFile(), hWnd, m_cNavigueData->GetFullScreen());
			AffichageImage(m_cNavigueData->GetCIBitmap(),hWnd,m_cNavigueData->GetCFile(),*m_cNavigueData->GetCBitmap());
			break;

		case IDM_FICHIER_SAUVERFICHIER:
			m_cNavigueData->GetFichier()->SauverFichier(m_cNavigueData->GetCFile(),*m_cNavigueData->GetCIBitmap(),hWnd,NULL);
			break;

		case ID_VIDO_ALLERAUBOOKMARL:
		case ID_VIDO_DUREENCOURS:
			{
				if(m_cNavigueData->GetVideo())
					SendMessage(GetParent(),WM_COMMAND,wParam,lParam);
			}

		case IDM_RECEIVEIMAGE_PREVIEW:
			{
				char * m_cFile = (char *)lParam;
				m_cNavigueData->SetCFile(m_cFile);
				OuvrirImage();
			}
			break;

		case IDM_IMAGES_SUIVANTE:
			{
				char m_szFilename[MAX_PATH];
				CObjet m_cObjet;
				if(m_cNavigueData->GetFichier()->AffichageImageSuivante(m_szFilename,MAX_PATH))
				{
					m_cNavigueData->SetCFile(m_szFilename);
	#ifdef WMP9
				OuvrirVideo();
	#else
				OuvrirImage();
	#endif

				}
				break;
			}

		case IDM_IMAGES_PRCDENTE:
			{
				char m_szFilename[MAX_PATH];
				CObjet m_cObjet;
				//char temp[255];
				if(m_cNavigueData->GetFichier()->AffichageImagePrecedente(m_szFilename,MAX_PATH))
				{
					m_cNavigueData->SetCFile(m_szFilename);
	#ifdef WMP9

				OuvrirVideo();
	#else
				OuvrirImage();
	#endif
				}
				break;
			}

		case ID_IMAGES_VISUALISATION_PREMIREIMAGE:
			{
				char m_szFilename[MAX_PATH];
				m_cNavigueData->GetFichier()->AffichageImagePremiere(m_szFilename,MAX_PATH);
				m_cNavigueData->SetCFile(m_szFilename);
	#ifdef WMP9
				OuvrirVideo();
	#else
				OuvrirImage();
	#endif
			}
			break;

		case IDM_RETOURWINDOW:
			{
				MouseAndKeyCreate(hWnd, WM_KEYDOWN, VK_ESCAPE, lParam);
			}
			break;

		case ID_IMAGES_VISUALISATION_DERNIREIMAGE:
			{
				char m_szFilename[MAX_PATH];
				m_cNavigueData->GetFichier()->AffichageImageDerniere(m_szFilename,MAX_PATH);
				m_cNavigueData->SetCFile(m_szFilename);

	#ifdef WMP9
				OuvrirVideo();
	#else
				OuvrirImage();
	#endif
			}
			break;


		case IDM_IMAGES_ROTATEIMAGE_180:
			{
				OutputDebugString("IDM_IMAGES_ROTATEIMAGE_180 default\n");
				//m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), EFFET_ROTATION,180,0,0,0);
				//SetFocus(hWnd);
				m_cNavigueData->GetCIBitmap()->VertFlipBuf();
				SendMessage(hWnd,WM_ERASEBKGND,MAKEWPARAM(m_cNavigueData->GetCIBitmap()->GetBitmapWidth(),m_cNavigueData->GetCIBitmap()->GetBitmapHeigth()),MAKELPARAM(IDM_FILTREIMAGE_OK,0));
			}
			break;

		case IDM_IMAGES_ROTATEIMAGE_90CW:
			{
				OutputDebugString("IDM_IMAGES_ROTATEIMAGE_90CW default\n");
				//m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), EFFET_ROTATION,90,0,0,0);
				//SetFocus(hWnd);
				m_cNavigueData->GetCIBitmap()->Rotation90();
				m_cNavigueData->GetCIBitmap()->HorzFlipBuf();
				SendMessage(hWnd,WM_ERASEBKGND,MAKEWPARAM(m_cNavigueData->GetCIBitmap()->GetBitmapWidth(),m_cNavigueData->GetCIBitmap()->GetBitmapHeigth()),MAKELPARAM(IDM_FILTREIMAGE_OK,0));
			}
			break;

		case IDM_IMAGES_ROTATEIMAGE_90CCW:
			//OutputDebugString("IDM_IMAGES_ROTATEIMAGE_90CCW default\n");
			//m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), EFFET_ROTATION,270,0,0,0);
			//SetFocus(hWnd);
			m_cNavigueData->GetCIBitmap()->HorzFlipBuf();
			m_cNavigueData->GetCIBitmap()->Rotation90();
			SendMessage(hWnd,WM_ERASEBKGND,MAKEWPARAM(m_cNavigueData->GetCIBitmap()->GetBitmapWidth(),m_cNavigueData->GetCIBitmap()->GetBitmapHeigth()),MAKELPARAM(IDM_FILTREIMAGE_OK,0));
			break;

		case IDM_IMAGES_ROTATEIMAGE_FLIPHORIZONTAL:
			OutputDebugString("IDM_IMAGES_ROTATEIMAGE_FLIPHORIZONTAL default\n");
			//m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), EFFET_FLIP,1,0,0,0);
			//SetFocus(hWnd);
			m_cNavigueData->GetCIBitmap()->HorzFlipBuf();
			SendMessage(hWnd,WM_ERASEBKGND,MAKEWPARAM(m_cNavigueData->GetCIBitmap()->GetBitmapWidth(),m_cNavigueData->GetCIBitmap()->GetBitmapHeigth()),MAKELPARAM(IDM_FILTREIMAGE_OK,0));
			break;

		case IDM_IMAGES_ROTATEIMAGE_FLIPVERTICAL:
			OutputDebugString("IDM_IMAGES_ROTATEIMAGE_FLIPVERTICAL default\n");
			//m_cNavigueData->GetCBitmap()->AppliquerFiltre(hWnd, m_cNavigueData->GetCIBitmap(), EFFET_FLIP,0,0,0,0);
			//SetFocus(hWnd);
			m_cNavigueData->GetCIBitmap()->VertFlipBuf();
			SendMessage(hWnd,WM_ERASEBKGND,MAKEWPARAM(m_cNavigueData->GetCIBitmap()->GetBitmapWidth(),m_cNavigueData->GetCIBitmap()->GetBitmapHeigth()),MAKELPARAM(IDM_FILTREIMAGE_OK,0));
			break;

		case IDM_IMAGES_ZOOM_ZOOMIN:
			{
				if(wmEvent != 5678)
				{
					RECT rc;
					GetClientRect(hWnd,&rc);
					int x = rc.right / 2;
					int y = rc.bottom / 2;
					ZoomIn(hWnd,MAKELPARAM(x,y));
				}
				else
					ZoomIn(hWnd,lParam);

				float fRatio = m_cNavigueData->GetCBitmap()->GetRatio();
				char m_szRatio[500];
				StringCchPrintf(m_szRatio,500,"%s - ratio %f",m_cNavigueData->GetCFile(),fRatio);
				::SetWindowText(GetParent(),m_szRatio);

				if(CObjet::GetWindowsVersion() == 0)
					Windows98Update(hWnd, message, MAKEWPARAM(IDM_IMAGES_ZOOM_ZOOMIN,1), MAKELPARAM(IDM_ZOOM_OK,0));
			}
			break;

		case IDM_IMAGES_ZOOM_ZOOMOUT:
			{			

				if(wmEvent != 5678)
				{
					RECT rc;
					GetClientRect(hWnd,&rc);
					int x = rc.right / 2;
					int y = rc.bottom / 2;
					ZoomOut(hWnd,MAKELPARAM(x,y));
				}
				else
					ZoomOut(hWnd,lParam);


				float fRatio = m_cNavigueData->GetCBitmap()->GetRatio();
				char m_szRatio[500];
				StringCchPrintf(m_szRatio,500,"%s - ratio %f",m_cNavigueData->GetCFile(),fRatio);
				::SetWindowText(GetParent(),m_szRatio);

				if(CObjet::GetWindowsVersion() == 0)
					Windows98Update(hWnd, message, MAKEWPARAM(IDM_IMAGES_ZOOM_ZOOMOUT,1), MAKELPARAM(IDM_ZOOM_OK,0));
			}
			break;


		case ID_IMAGES_STARTDIAPORAMA:
			{
				//Récupération des informations concernant le diaporama		
				//Sauvegarde des informations sur l'image
				m_cParameter->SetOptionAffichage(m_cNavigueData->GetHauteQualite(),m_cNavigueData->GetShrink(),m_cNavigueData->GetRawFile());
				m_cParameter->SetStartDiaporama(true);

				GetOptionDiaporama();

				KillTimer(hWnd,TIMER_DIAPORAMA);
				CheckMenuItem(GetMenu(GetParent()),ID_IMAGES_STARTDIAPORAMA,MF_CHECKED);
				CheckMenuItem(GetMenu(GetParent()),ID_IMAGES_STOPDIAPORAMA,MF_UNCHECKED);
				SetTimer(hWnd,TIMER_DIAPORAMA,m_cNavigueData->GetDuree() * 1000,NULL);
				m_cNavigueData->GetCBitmap()->SetShrinkImage(m_cNavigueData->GetShrink());


				CheckMenuItem(GetMenu(GetParent()),ID_IMAGES_STARTDIAPORAMA,MF_CHECKED);
				CheckMenuItem(GetMenu(GetParent()),ID_IMAGES_STOPDIAPORAMA,MF_UNCHECKED);

				if(m_cNavigueData->GetFullScreen())
					SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_IMAGES_VISUALISATION_PLAINECRAN,0),0);
			}

			break;

		case ID_IMAGES_STOPDIAPORAMA:
			{
				//Récupération des informations concernant la visualisation
				KillTimer(hWnd,TIMER_DIAPORAMA);
				m_cParameter->SetStartDiaporama(false);

				int m_iHauteQualite = 0;
				bool m_bShrink = false;
				bool m_bRawFile = false;

				m_cParameter->GetOptionAffichage(m_iHauteQualite,m_bShrink, m_bRawFile);

				m_cNavigueData->SetRawFile(m_bRawFile);
				m_cNavigueData->SetHauteQualite(m_iHauteQualite);
				m_cNavigueData->SetShrink(m_bShrink);

				m_cNavigueData->GetCBitmap()->SetShrinkImage(m_cNavigueData->GetShrink());
				m_cNavigueData->GetCBitmap()->SetHauteQualite(m_cNavigueData->GetHauteQualite());

				CheckMenuItem(GetMenu(GetParent()),ID_IMAGES_STARTDIAPORAMA,MF_UNCHECKED);
				CheckMenuItem(GetMenu(GetParent()),ID_IMAGES_STOPDIAPORAMA,MF_CHECKED);
				
			}	
			break;

		default:
			SendMessage(::GetParent(hWnd),WM_COMMAND,MAKEWPARAM(IDM_ONCOMMANDEFFECT,0),(LPARAM)false);
		   return DefWindowProc(hWnd, message, wParam, lParam);
	}

	SendMessage(::GetParent(hWnd),WM_COMMAND,MAKEWPARAM(IDM_ONCOMMANDEFFECT,0),(LPARAM)false);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void CWndNavigueBitmap::OuvrirVideo()
{
	CObjet m_cObjet;
	char temp[255];

	if(m_cObjet.TestMultimediaFormat(m_cNavigueData->GetCFile()) != 0)
	{
		if(!m_cNavigueData->GetVideo())
			m_cNavigueData->SetVideo(true);
			
		ShowScrollBar(m_hWnd,SB_BOTH,false);
		m_cNavigueData->GetCBitmap()->SetShrinkImage(true);

		SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_PLAY_VIDEO,IDM_PLAY_VIDEO),(LPARAM)m_cNavigueData->GetCFile());

		int iNumImage = m_cNavigueData->GetFichier()->GetNbFiles();
		int iPosImage = m_cNavigueData->GetFichier()->GetPos();
		char m_cFile[50];
		LoadString(hInstance,IDS_FILE,m_cFile,50);
		StringCchPrintf(temp,255,"%s : %d/%d",m_cFile,iPosImage,iNumImage);
		SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(IDM_UPDATESTATUSBAR,0), (LPARAM)temp);
	}

	if(m_cObjet.Test3DFormat(m_cNavigueData->GetCFile()) != 0)
	{
		if(!m_cNavigueData->Get3D())
			m_cNavigueData->Set3D(true);
			
		ShowScrollBar(m_hWnd,SB_BOTH,false);
		m_cNavigueData->GetCBitmap()->SetShrinkImage(true);

		SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_SHOW_3D,IDM_SHOW_3D),(LPARAM)m_cNavigueData->GetCFile());

		int iNumImage = m_cNavigueData->GetFichier()->GetNbFiles();
		int iPosImage = m_cNavigueData->GetFichier()->GetPos();
		char m_cFile[50];
		LoadString(hInstance,IDS_FILE,m_cFile,50);
		StringCchPrintf(temp,255,"%s : %d/%d",m_cFile, iPosImage,iNumImage);
		SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(IDM_UPDATESTATUSBAR,0), (LPARAM)temp);
	}

	if(m_cObjet.TestImageFormat(m_cNavigueData->GetCFile()) != 0)
	{
		OuvrirImage();
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void CWndNavigueBitmap::OuvrirImage()
{
	char temp[255];

	AffichageImage(m_cNavigueData->GetCIBitmap(),m_hWnd,m_cNavigueData->GetCFile(),*m_cNavigueData->GetCBitmap());

	if(m_cNavigueData->GetCreateMiniature())
		SendMessage(m_cNavigueData->GetPreviewHWnd(),WM_COMMAND,MAKEWPARAM(IDM_FILENAME,0),(LPARAM)m_cNavigueData->GetCFile());

	if(m_cNavigueData->GetVideo())
	{
		m_cNavigueData->SetVideo(false);
		SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_STOP_VIDEO,IDM_STOP_VIDEO),0);
	}

	if(m_cNavigueData->Get3D() || m_cNavigueData->Get3DDiaporama())
	{
		m_cNavigueData->Set3D(false);
		m_cNavigueData->Set3DDiaporama(false);
		SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_STOP_3D,IDM_STOP_3D),0);
	}

	int iNumImage = m_cNavigueData->GetFichier()->GetNbFiles();
	int iPosImage = m_cNavigueData->GetFichier()->GetPos();
	char m_cFile[50];
	LoadString(hInstance,IDS_FILE,m_cFile,50);
	StringCchPrintf(temp,255,"%s : %d/%d",m_cFile,iPosImage,iNumImage);
	SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(IDM_UPDATESTATUSBAR,0), (LPARAM)temp);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndNavigueBitmap::OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	m_cNavigueData->SetHWnd(hWnd);
	CObjet m_cObjet;
	m_cNavigueData->SetSelect(0);
	SetCursor (LoadCursor (NULL, IDC_ARROW));

	m_cNavigueData->GetCBitmap()->SetHInstance(hInstance);

	if(m_cParameter->GetStartDiaporama() == 1)
	{
		GetOptionDiaporama();
		KillTimer(hWnd,TIMER_DIAPORAMA);
		CheckMenuItem(GetMenu(GetParent()),ID_IMAGES_STARTDIAPORAMA,MF_CHECKED);
		CheckMenuItem(GetMenu(GetParent()),ID_IMAGES_STOPDIAPORAMA,MF_UNCHECKED);
		SetTimer(hWnd,TIMER_DIAPORAMA,m_cNavigueData->GetDuree() * 1000,NULL);
		m_cNavigueData->GetCBitmap()->SetShrinkImage(m_cNavigueData->GetShrink());
	}
	else
	{

		int m_iHauteQualite = 0;
		bool m_bShrink = false;
		bool m_bValueRaw = false;
		m_cParameter->GetOptionAffichage(m_iHauteQualite,m_bShrink,m_bValueRaw);
		m_cNavigueData->SetHauteQualite(m_iHauteQualite);
		m_cNavigueData->SetShrink(m_bShrink);
		m_cNavigueData->SetRawFile(m_bValueRaw);
		m_cNavigueData->GetCBitmap()->SetShrinkImage(m_cNavigueData->GetShrink());
		m_cNavigueData->GetCBitmap()->SetHauteQualite(m_cNavigueData->GetHauteQualite());

		if(m_bValueRaw)
			CheckMenuItem(GetMenu(GetParent()),ID_IMAGE_INCLURERAW,MF_CHECKED);
		else
			CheckMenuItem(GetMenu(GetParent()),ID_IMAGE_INCLURERAW,MF_UNCHECKED);
	}



	m_MargeData = new CMargeData();

	m_MargeData->m_fBas = DEFAUTPRINTDISTANCE;
	m_MargeData->m_fHaut = DEFAUTPRINTDISTANCE;
	m_MargeData->m_fGauche = DEFAUTPRINTDISTANCE;
	m_MargeData->m_fDroite = DEFAUTPRINTDISTANCE;
	m_MargeData->m_ciBitmap = m_cNavigueData->GetCIBitmap();

	m_cNavigueData->Set3D(false);
	m_cNavigueData->Set3DDiaporama(false);
	m_cNavigueData->SetVideo(false);
	m_cNavigueData->SetCreateMiniature(false);

	switch(m_cNavigueData->GetHauteQualite())
	{
		case 0:
			CheckMenuItem(GetMenu(GetParent()),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_NORMAL,MF_CHECKED);
			break;

		case BOXFILTER:
			CheckMenuItem(GetMenu(GetParent()),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BOX,MF_CHECKED);
			break;

		case BILINEARFILTER:
			CheckMenuItem(GetMenu(GetParent()),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BILINEAR,MF_CHECKED);
			break;

		case GAUSSIANFILTER:
			CheckMenuItem(GetMenu(GetParent()),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_GAUSSIAN,MF_CHECKED);
			break;

		case HAMMINGFILTER:
			CheckMenuItem(GetMenu(GetParent()),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_HAMMING,MF_CHECKED);
			break;

		case CUBICFILTER:
			CheckMenuItem(GetMenu(GetParent()),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BICUBIC,MF_CHECKED);
			break;

		case BLACKMANFILTER:
			CheckMenuItem(GetMenu(GetParent()),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BLACKMAN,MF_CHECKED);
			break;

		case QUADRATICFILTER:
			CheckMenuItem(GetMenu(GetParent()),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_QUADRATIC,MF_CHECKED);
			break;

		case MITCHELLFILTER:
			CheckMenuItem(GetMenu(GetParent()),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_MITCHELL,MF_CHECKED);
			break;

		case TRIANGLEFILTER:
			CheckMenuItem(GetMenu(GetParent()),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_TRIANGLE,MF_CHECKED);
			break;

		case SINCFILTER:
			CheckMenuItem(GetMenu(GetParent()),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_SINC,MF_CHECKED);
			break;

		case BESSELFILTER:
			CheckMenuItem(GetMenu(GetParent()),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BESSEL,MF_CHECKED);
			break;

		case BLACKMANBESSELFILTER:
			CheckMenuItem(GetMenu(GetParent()),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BLACKMANBESSEL,MF_CHECKED);
			break;

		case BLACKMANSINCFILTER:
			CheckMenuItem(GetMenu(GetParent()),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BLACKMANSINC,MF_CHECKED);
			break;

		case LANCZOSFILTER:
			CheckMenuItem(GetMenu(GetParent()),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_LANCZOS,MF_CHECKED);
			break;

		case HERMITEFILTER:
			CheckMenuItem(GetMenu(GetParent()),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_HERMITE,MF_CHECKED);
			break;

		case HANNINGFILTER:
			CheckMenuItem(GetMenu(GetParent()),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_HANNING,MF_CHECKED);
			break;

		case CATROMFILTER:
			CheckMenuItem(GetMenu(GetParent()),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_CATROM,MF_CHECKED);
			break;
	}

	ProcessCreate(hWnd, message, wParam, lParam);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndNavigueBitmap::OnSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	if(m_cNavigueData->GetPreviewHWnd() != NULL)
	{
		SendMessage(m_cNavigueData->GetPreviewHWnd(),WM_COMMAND,MAKEWPARAM(WM_MOVE,0),0);
	}

	if(!m_cNavigueData->GetVideo())
	{
		m_cNavigueData->GetCIBitmap()->SetOnSize(true);
		KillTimer(hWnd,TIMER_UPDATEPICTURE);
		int iReturn = SizeCreate(hWnd, message, wParam, lParam);
		SetTimer(hWnd,TIMER_UPDATEPICTURE,100,0);
		return iReturn;
	}
	else
	{
		RECT rc;
		GetClientRect(GetParent(),&rc);	
		int yNewSize, xNewSize;
   		yNewSize = HIWORD (lParam); 
		xNewSize = LOWORD (lParam); 
		m_cNavigueData->GetCBitmap()->SetSize(xNewSize,yNewSize);
		ShowScrollBar(hWnd,SB_BOTH,false);
		m_cNavigueData->GetCBitmap()->SetShrinkImage(true);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndNavigueBitmap::OnEraseBackground(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_bTransition)
		return 0;

	int iValue = 0;
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint( hWnd, &ps );
	m_cNavigueData->GetCIBitmap()->SetFilename(m_cNavigueData->GetCFile());

	if(!m_cNavigueData->GetVideo())
	{
		switch(LOWORD(lParam))
		{
			case IDM_ZOOM_OK:
				{
					iValue = EraseBgrCreate(hWnd, message, wParam, lParam);
					break;
				}
			case IDM_CHGIMAGE_OK :
				{
					#pragma omp critical(y)
					{
						CObjet m_cObjet;
						CIBitmap * m_ciBitmapThread = (CIBitmap *)wParam;

						char m_cTemp[255];
						m_ciBitmapThread->GetFilename(m_cTemp);
						if(strcmp(m_cNavigueData->GetCFile(),m_cTemp) != 0)
						{
							SetTimer(hWnd,TIMER_RELOADPICTURE,100,0);
						}				
						KillTimer(hWnd,TIMER_UPDATEPICTURE);
						//m_cObjet.TestOrientation(m_ciBitmapThread,m_cNavigueData->GetCFile());
						m_cNavigueData->SetCIBitmap(m_ciBitmapThread);
						iValue = EraseBgrCreate(hWnd, message, wParam, lParam);
						
					}
				}
				break;
				
				
			default:
				{	
					m_cNavigueData->GetCBitmap()->SetHauteQualite(m_cNavigueData->GetHauteQualite());
					iValue = EraseBgrCreate(hWnd, message, wParam, lParam);
				}
		}
	}

	EndPaint(hWnd, &ps);
	ReleaseDC(hWnd,hDC);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndNavigueBitmap::OnDestroy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{


	CObjet m_cObjet;

	if(m_cNavigueData->GetVideo())
	{
		m_cNavigueData->SetVideo(false);
		SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_STOP_VIDEO,IDM_STOP_VIDEO),0);
	}

	if(m_cNavigueData->Get3D())
	{
		m_cNavigueData->Set3D(false);
		SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_STOP_3D,IDM_STOP_3D),0);
	}

	CheckMenuItem(GetMenu(GetParent()),ID_IMAGES_AFFICHAGEMINIATURE,MF_UNCHECKED);

	if(m_cNavigueData->GetCreateMiniature())
	{
		CheckMenuItem(GetMenu(GetParent()),ID_IMAGES_AFFICHAGEMINIATURE,MF_UNCHECKED);
		SendMessage(m_cNavigueData->GetPreviewHWnd(),WM_DESTROY,0,0);
		DestroyWindow(m_cNavigueData->GetPreviewHWnd());
		m_cNavigueData->SetCreateMiniature(false);
	}

	if(m_cParameter->GetStartDiaporama() == 0)
		m_cParameter->SetOptionAffichage(m_cNavigueData->GetHauteQualite(),m_cNavigueData->GetShrink(),m_cNavigueData->GetRawFile());

	CloseMonitor();

	m_cNavigueData->SetCreateMiniature(false);

	PostQuitMessage(0);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndNavigueBitmap::OnMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	SendMessage(m_cNavigueData->GetPreviewHWnd(),WM_COMMAND,MAKEWPARAM(WM_MOVE,0),0);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CWndNavigueBitmap::OnDblClick(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(!m_cNavigueData->Get3D() && !m_cNavigueData->Get3DDiaporama() && !m_cNavigueData->GetVideo() && m_cNavigueData->GetSelect() != 1)
	{
		OnCommand(hWnd,WM_COMMAND,MAKEWPARAM(IDM_IMAGES_SHRINKIMAGE,0),(LPARAM)0);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndNavigueBitmap::OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//Récupération des valeurs en x et y
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);

	RECT rc;
	GetClientRect(hWnd,&rc);

	switch(m_cNavigueData->GetSelect())
	{
		case 1:
			{
				//MouseAndKeyCreate(hWnd, message, wParam, lParam, pNavigueData);
				if(!m_cNavigueData->GetCBitmap()->GetMouseBlock())
					m_cNavigueData->GetDessin()->Selection(y + m_cNavigueData->GetCScroll()->GetCurrentVScroll(),x + m_cNavigueData->GetCScroll()->GetCurrentHScroll());
				else
				{
					//Deplacement d'écran si on atteint les 20%
					MouseAndKeyCreate(hWnd, message, wParam, lParam);
						
					m_cNavigueData->GetDessin()->MouseMove(x,y,m_cNavigueData->GetCScroll()->GetCurrentHScroll(),m_cNavigueData->GetCScroll()->GetCurrentVScroll());
					
					//SendMessage(hWnd,WM_PAINT,0,0);
					SendMessage(hWnd,WM_ERASEBKGND,0,MAKELPARAM(IDM_REDRAW_OK,IDM_REDRAW_OK));

				}
			}
			break;

		case 2:
			{
				if(HIWORD(GetKeyState(VK_CONTROL)) != 0)
				{
					//Zoom On
					SetCursor (LoadCursor (hInstance, MAKEINTRESOURCE(IDC_ZOOMOUT))) ;
				}
				else
				{
					//Zoom Out
					SetCursor (LoadCursor (hInstance, MAKEINTRESOURCE(IDC_ZOOMON))) ;
				}
			}
			break;

		default:
			MouseAndKeyCreate(hWnd, message, wParam, lParam);
			break;
	}
	//}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndNavigueBitmap::OnMouseWheel(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return MouseAndKeyCreate(hWnd, message, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndNavigueBitmap::OnLButtonDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(m_cNavigueData->GetSelect())
	{
		case 1:
			{
				int yNewSize, xNewSize;
			   	yNewSize = HIWORD (lParam); 
				xNewSize = LOWORD (lParam); 
				m_cNavigueData->GetCBitmap()->SetInvertSensPhoto(true);
				m_cNavigueData->GetCBitmap()->SetMouseBlock(true);
				m_cNavigueData->GetCBitmap()->SetMouseScroll(LOWORD(lParam), HIWORD(lParam));
				SetCapture(hWnd);
				m_cNavigueData->GetDessin()->InitPoint(xNewSize,yNewSize,m_cNavigueData->GetCScroll()->GetCurrentHScroll(),m_cNavigueData->GetCScroll()->GetCurrentVScroll());
				SendMessage(hWnd,WM_ERASEBKGND,0,0);
			}
			break;

		case 2:
			{
				if(HIWORD(GetKeyState(VK_CONTROL)) != 0)
					SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_IMAGES_ZOOM_ZOOMOUT,5678),lParam);
				else
					SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_IMAGES_ZOOM_ZOOMIN,5678),lParam);
			}

		default:
			return MouseAndKeyCreate(hWnd, message, wParam, lParam);
			break;
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndNavigueBitmap::OnLButtonUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(m_cNavigueData->GetSelect())
	{
		case 1:
			{
				m_cNavigueData->GetCBitmap()->SetMouseBlock(false);
				m_cNavigueData->GetCBitmap()->SetMouseScroll(0, 0);
				SetCursor (LoadCursor (NULL, IDC_ARROW)) ;
				ReleaseCapture();
				m_cNavigueData->GetCBitmap()->SetInvertSensPhoto(false);
			}
			break;

		//case 2:
		//	break;

		default:
			return MouseAndKeyCreate(hWnd, message, wParam, lParam);
			break;
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndNavigueBitmap::OnHScroll(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return MouseAndKeyCreate(hWnd, message, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndNavigueBitmap::OnVScroll(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return MouseAndKeyCreate(hWnd, message, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndNavigueBitmap::OnKeyDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return MouseAndKeyCreate(hWnd, message, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndNavigueBitmap::OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef _DEBUG
	OutputDebugString("WM_PAINT \n");
#endif

	if(m_bTransition)
		return 0;

	m_cNavigueData->GetCIBitmap()->SetFilename(m_cNavigueData->GetCFile());

	PAINTSTRUCT ps;
	RECT rc;
	HFONT font;
	HDC hDC = BeginPaint( hWnd, &ps );
	GetClientRect(hWnd,&rc);

	if(!m_cNavigueData->GetVideo())
	{
		m_cNavigueData->GetCBitmap()->SetHauteQualite(m_cNavigueData->GetHauteQualite());
		
		if(CObjet::GetWindowsVersion() == 0)
			EraseBgrCreate(hWnd, message, wParam, lParam);

		if(!m_cNavigueData->GetCIBitmap()->GetUse())
			m_cNavigueData->GetCBitmap()->AffichageBitmap(hDC,rc, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetCScroll()->GetCurrentHScroll(), m_cNavigueData->GetCScroll()->GetCurrentVScroll());
	}

	if(m_cNavigueData->GetCBitmap()->GetShowInfos())
	{
		CBitmapEffect::ShowInfos(hWnd,hDC,m_cNavigueData->GetCIBitmap());
	}

	if(m_cNavigueData->GetCBitmap()->GetShowHistogramme())
	{
		CBitmapEffect::ShowHistogramme(hWnd,hDC,m_cNavigueData->GetCIBitmap(),rc);
	}

	EndPaint(hWnd, &ps);
	ReleaseDC(hWnd,hDC);
	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//Gestion des Timers
/////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CWndNavigueBitmap::OnTimer(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
		case TIMER_DIAPORAMA:
			//Passer à l'image suivante:
			if(m_cNavigueData->GetSequence() == 1)
				SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_IMAGES_SUIVANTE,0), 0);
			else
				SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_IMAGES_PRCDENTE,0), 0);
			break;

		case TIMER_UPDATEPICTURE:
			KillTimer(hWnd,TIMER_UPDATEPICTURE);
			m_cNavigueData->GetCIBitmap()->SetOnSize(false);
			SendMessage(hWnd,WM_ERASEBKGND,0,MAKELPARAM(IDM_REDRAW_PICTURE,0));
			break;

		case TIMER_RELOADPICTURE:
			KillTimer(hWnd,TIMER_RELOADPICTURE);
			AffichageImage(m_cNavigueData->GetCIBitmap(),hWnd,m_cNavigueData->GetCFile(),*m_cNavigueData->GetCBitmap());
			break;
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndNavigueBitmap::OnRButtonDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CObjet m_cObjet;
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	m_cObjet.DisplayContextMenu(GetParent(),pt, hInstance, (LPCSTR)IDM_AFFICHAGEBITMAP,2);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void CWndNavigueBitmap::Windows98Update(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	m_cNavigueData->GetCBitmap()->SetHauteQualite(m_cNavigueData->GetHauteQualite());
	EraseBgrCreate(hWnd, message, wParam, lParam);
	SendMessage(hWnd,WM_PAINT,0,0);
}
