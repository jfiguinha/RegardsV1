// Preview.cpp: implementation of the CPreview class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Preview.h"
#include "Dessin.h"
#include "effet.h"
#include "filtre.h"
#include "objet.h"
#include "fichier.h"
#include "LoadImage.h"
#include "webinfofichier.h"
#include "mp3dialog.h"
#include <ibitmap.h>
#include <LoadPicture.h>
#include <exif.h>
#include ".\infosfichierproc.h"
#include ".\previewdata.h"
#include ".\registryaccess.h"
#ifdef WMP9

//#include "initguid.h"
#include "CWMPHost.h"

#endif



int CPreview::SaveWindowPosition()
{
	WINDOWINFO _windowInfo;
	int _iPreview = 0;
	GetWindowInfo(m_pPreviewData->hWndInfosFichier,&_windowInfo);
	_iPreview = _windowInfo.rcClient.bottom - _windowInfo.rcClient.top;

	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->WriteOrCreateKeyValue("SOFTWARE\\Regards\\exploreur") == ERROR_SUCCESS)
	{
		if(::IsWindowVisible(m_pPreviewData->hWndInfosFichier))
			m_cRegistryAccess->WriteIntValue(_iPreview,"infosFichierWindowSize");
	}
	delete m_cRegistryAccess;
	return 0;
}

int CPreview::LoadWindowPosition(int &iHauteurInfos)
{
	WINDOWINFO _windowInfo;
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->OpenKeyValue("SOFTWARE\\Regards\\exploreur") == ERROR_SUCCESS)
	{
		iHauteurInfos = m_cRegistryAccess->ReadIntValue("infosFichierWindowSize");
		if(iHauteurInfos == -1)
		{
			delete m_cRegistryAccess;
			return -1;
		}
	}
	else
	{
		delete m_cRegistryAccess;
		return -1;
	}
	delete m_cRegistryAccess;

	/*
	RECT rc;
	GetClientRect(m_hWnd,&rc);
	MoveWindow(m_pPreviewData->hWndInfosFichier,0, rc.bottom - iHauteurInfos, rc.right, iHauteurInfos,false);*/

	return 0;
}

void CPreview::SetPictureQuality(int iQualite)
{
	if(iQualite == 0)
		m_pPreviewData->m_bSmooth = TRIANGLEFILTER;
	else
		m_pPreviewData->m_bSmooth = iQualite;
	
}

/////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////

LRESULT CPreview::OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	m_cMp3Traitement = new CMP3Dialog();

	m_cInfosFichierProc->SetParam(m_pPreviewData);

#ifdef WMP9

#else
	m_pPreviewData->hMCI = MCIWndCreate(hWnd,hInstance, MCIWNDF_SHOWNAME|MCIWNDF_SHOWALL, NULL);
#endif

	m_pPreviewData->hWndMP3 = m_cMp3Traitement->CreateDlg(hInstance,MAKEINTRESOURCE(IDD_MP3PLAYER),hWnd);//CreateDialog(hInstance,MAKEINTRESOURCE(IDD_MP3PLAYER),hWnd,MP3Player);
	m_pPreviewData->hWndInfosFichier = m_cInfosFichierProc->CreateDlg(hInstance,MAKEINTRESOURCE(IDD_DLGKEYWORD),hWnd);//CreateDialogParam(hInstance,MAKEINTRESOURCE(IDD_DLGKEYWORD),hWnd,InfosFichierProc,(long)&pPreviewData);


#ifdef WMP9
	RECT rcPos = {0,0,0,0};
	m_wmpHost->Create(hWnd, rcPos, _T("WMP Host Container"), WS_CHILD, 0);
	m_wmpHost->ShowWindow(SW_HIDE);
#else
	::ShowWindow(m_pPreviewData->hMCI,SW_HIDE);
#endif
	
	::ShowWindow(m_pPreviewData->hWndMP3,SW_HIDE);
	::ShowWindow(m_pPreviewData->hWndInfosFichier,SW_HIDE);

	m_pPreviewData->hWndStaticBitmap = CreateWindowEx(WS_EX_CLIENTEDGE,"STATIC","", WS_CHILD|WS_VISIBLE|SS_NOTIFY|SS_BITMAP,
			 0,0,0,0,
			hWnd,
			 (HMENU)NULL,
			 hInstance,
			 NULL);

	

	return 0;
}

LRESULT CPreview::OnDblClick(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_pPreviewData->iTypeData == 1)
		SendMessage(::GetParent(GetParent()),WM_COMMAND,MAKEWPARAM(IDM_LISTDBLCLICK,EVENT_IMAGE),(LPARAM)m_pPreviewData->cFile);
	return 0;
}

/////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////

LRESULT CPreview::OnSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int iRight = LOWORD(lParam);
	int iBottom = HIWORD(lParam);
	m_bOnSize = true;
	RECT rc;
	HDC hDC = GetDC(hWnd);

	OutputDebugString("OnSize");

	switch(m_pPreviewData->iTypeData)
	{
		case 1:
			{	
				//CObjet m_cObjet;
				RECT rcWindow;
				GetClientRect(hWnd,&rcWindow);
				FillRect(hDC, &rcWindow, m_bkgndColor); 
				GetClientRect(m_pPreviewData->hWndInfosFichier,&rc);
				MoveWindow(m_pPreviewData->hWndInfosFichier,0,iBottom - rc.bottom,rcWindow.right,rc.bottom + 4,TRUE);
				ShowBitmap(0);
			}
			break;
		case 2:
#ifdef WMP9

			RECT rcWindow;

			if(wParam == 1 && m_iRightFile == 0 && m_iTopFile == 0)
			{
				GetClientRect(m_hWnd,&rc);
				int iHauteurInfos  = ((rc.bottom - rc.top) / 3);
				m_iRightFile = rc.right;
				m_iTopFile = iHauteurInfos + iHauteurInfos;
				MoveWindow(m_pPreviewData->hWndInfosFichier,0, m_iTopFile,m_iRightFile, rc.bottom - m_iTopFile,true);
				::ShowWindow(m_pPreviewData->hWndInfosFichier,SW_SHOW);
				GetClientRect(hWnd,&rcWindow);
				m_wmpHost->MoveWindow(0,rcWindow.top,rcWindow.right,rcWindow.bottom - iHauteurInfos);
			}
			else if(wParam == 2)
			{
				::ShowWindow(m_pPreviewData->hWndInfosFichier,SW_SHOW);
				GetClientRect(m_pPreviewData->hWndInfosFichier,&rc);
				GetClientRect(hWnd,&rcWindow);
				m_iTopFile = rcWindow.bottom - iBottom;
				MoveWindow(m_pPreviewData->hWndInfosFichier,0, m_iTopFile,rcWindow.right, rcWindow.bottom - m_iTopFile,true);
				GetClientRect(m_pPreviewData->hWndInfosFichier,&rc);
				m_wmpHost->MoveWindow(0,0,rcWindow.right,m_iTopFile);
			}
			else
			{
				::ShowWindow(m_pPreviewData->hWndInfosFichier,SW_SHOW);
				GetClientRect(m_pPreviewData->hWndInfosFichier,&rc);
				GetClientRect(hWnd,&rcWindow);
				MoveWindow(m_pPreviewData->hWndInfosFichier,0, m_iTopFile,rcWindow.right, rcWindow.bottom - m_iTopFile,true);
				GetClientRect(m_pPreviewData->hWndInfosFichier,&rc);
				m_wmpHost->MoveWindow(0,0,rcWindow.right,m_iTopFile);
			}

			//BOOL m_bValue = true;
			//m_wmpHost->OnSize(message, wParam, lParam, m_bValue);
#endif

			break;
	}



	m_bOnSize = false;
	ReleaseDC(hWnd,hDC);
	return 0;
}

/////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////

LRESULT CPreview::OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam); 

	if(wParam == 0)
	{
		if((long)lParam == (long)m_pPreviewData->hWndStaticBitmap)
		{
			SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_LISTDBLCLICK,EVENT_IMAGE),(LPARAM)m_pPreviewData->cFile);
		}
		return 0;
	}



	// Parse the menu selections:
	switch (wmId)
	{

	case ID_LISTE_GESTIONSDESATTRIBUTS:
		{
			SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(ID_LISTE_GESTIONSDESATTRIBUTS,0),0);
			break;
		}

		case IDM_MEDIAFILENAME:
			{
				StringCchCopy(m_pPreviewData->cFile,MAX_PATH,(const char *)lParam);
				::ShowWindow(m_pPreviewData->hWndStaticBitmap,SW_HIDE);
				::ShowWindow(m_pPreviewData->hWndMP3,SW_HIDE);
				ShowMultimedia();
			}
			break;

		case IDM_FILENAME:
			{
				CObjet m_cObjet;
				int iValue;

				m_pPreviewData->iAlbum = wmEvent;
				
				StringCchCopy(m_pPreviewData->cFile,MAX_PATH,(const char *)lParam);

				//Test Type de fichier
				if((iValue = m_cObjet.TestMultimediaFormat(m_pPreviewData->cFile)) != 0)
				{

					::ShowWindow(m_pPreviewData->hWndStaticBitmap,SW_HIDE);

					if(iValue == 2)
					{
						if(m_pPreviewData->iTypeData != 3)
						{
							RECT rc;
							HDC hdc = GetDC(hWnd);
							GetClientRect(hWnd,&rc);
							FillRect(hdc, &rc, m_bkgndColor); 
							ReleaseDC(hWnd,hdc);
						}
						m_pPreviewData->iTypeData = 3;

#ifdef WMP9
						m_pPreviewData->iShowVideo = 0;
						m_wmpHost->OnClose();
						m_wmpHost->ShowWindow(SW_HIDE);
#else
						MCIWndClose(m_pPreviewData->hMCI);
						::ShowWindow(m_pPreviewData->hMCI,SW_HIDE);
#endif

						::ShowWindow(m_pPreviewData->hWndInfosFichier,SW_HIDE);
						ShowMP3();	
						SendMessage(m_pPreviewData->hWndMP3,WM_COMMAND,MAKEWPARAM(IDC_BTFICHIER,0),(LPARAM)m_pPreviewData->cFile);
						return 0;
					}
					else
					{

						m_pPreviewData->iTypeData = 2;
						SendMessage(m_pPreviewData->hWndMP3,WM_COMMAND,MAKEWPARAM(IDC_BTSTOP,0),0);
						::ShowWindow(m_pPreviewData->hWndMP3,SW_HIDE);
						::ShowWindow(m_pPreviewData->hWndInfosFichier,SW_HIDE);
						ShowMultimedia();
					}
					return 0;
				}

				if(m_pPreviewData->iTypeData != 1)
				{
					m_pPreviewData->iTypeData = 1;

#ifdef WMP9
					m_pPreviewData->iShowVideo = 0;
					m_wmpHost->OnClose();
					m_wmpHost->ShowWindow(SW_HIDE);
#else
					MCIWndClose(m_pPreviewData->hMCI);
					::ShowWindow(m_pPreviewData->hMCI,SW_HIDE);
#endif

					SendMessage(m_pPreviewData->hWndMP3,WM_COMMAND,MAKEWPARAM(IDC_BTSTOP,0),0);
					::ShowWindow(m_pPreviewData->hWndMP3,SW_HIDE);
					::ShowWindow(m_pPreviewData->hWndStaticBitmap,SW_SHOW);
					
				}

				int iFormat = m_cObjet.TestImageFormat(m_pPreviewData->cFile);
				if(iFormat != 0)
				{
					if(m_cObjet.GetWindowsVersion())
					{
						if(!m_pPreviewData->m_ciBitmap.GetUse())
						{
							//Test si format 12 et chargement en static
							if(iFormat == 12)
							{
								#pragma omp critical(previewibitmap)
								{
									m_pPreviewData->m_ciBitmap.hWnd = hWnd;
									m_pPreviewData->m_ChargementImage.ChargementImage(&m_pPreviewData->m_ciBitmap, m_pPreviewData->cFile, hWnd);
								}


							}
							else
							{
								#pragma omp critical(previewibitmap)
								{
									m_pPreviewData->m_ChargementImage.ChargementImage(&m_pPreviewData->m_ciBitmap, m_pPreviewData->cFile, hWnd);
								}
							}
						}
						
					}
					else
					{
						HCURSOR  hCur = LoadCursor(NULL, IDC_WAIT);
						hCur = SetCursor(hCur);
						m_pPreviewData->m_ChargementImage.ChargementImageStatic(&m_pPreviewData->m_ciBitmap, m_pPreviewData->cFile);
						SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(IDM_UPDATESTATUS,4), MAKELPARAM(m_pPreviewData->m_ciBitmap.GetBitmapWidth(),m_pPreviewData->m_ciBitmap.GetBitmapHeigth()));
						ShowBitmap();
						SendMessage(m_pPreviewData->hWndInfosFichier,WM_COMMAND,MAKEWPARAM(IDM_FILENAME,0),(LPARAM)m_pPreviewData);
						SetCursor(LoadCursor(NULL, IDC_ARROW));
					}
					
				}
				else
					m_pPreviewData->iTypeData = 0;
				return 0;
			}
			break;


		case IDM_INFOSFICHIERSIZECHANGE:
			{	
				if(!m_bOnSize)
				{
					if(m_pPreviewData->iShowVideo)
					{
						RECT rc;
						GetClientRect(m_pPreviewData->hWndInfosFichier,&rc);
						int iHauteurInfos = rc.bottom - rc.top;
						SendMessage(hWnd,WM_SIZE,2,MAKELPARAM(rc.right,iHauteurInfos));
					}
					else
						ShowBitmap(0);
				}
			}
			break;

		case IDM_ALBUMPT:
			{
				m_pPreviewData->iAlbum = 1;
				m_pPreviewData->IconeFileTemp = (CIconeFile *)lParam;
				SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(ID_RECUP_PTATTRIBUT,0),(LPARAM)&m_pPreviewData->m_AttributVector);
			}
			break;

		case ID_AFFICHAGE_CONTROLE_INFOSFICHIER:
			{
				if(wmEvent != 99)
				{
					if(m_pPreviewData->iAffichageInfos == 1)
						m_pPreviewData->iAffichageInfos = 0;
					else
						m_pPreviewData->iAffichageInfos = 1;
				}
				else
				{
					m_pPreviewData->IconeFileTemp = (CIconeFile *)lParam;
					SendMessage(m_pPreviewData->hWndInfosFichier,WM_COMMAND,MAKEWPARAM(IDC_UPDATE_FILEINFO,0),(LPARAM)m_pPreviewData);
					break;
				}

				if(m_pPreviewData->iTypeData == 1)
					ShowBitmap();
			}
			break;

	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////

LRESULT CPreview::OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	SetCursor (LoadCursor (NULL, IDC_ARROW)) ;
	return 0;
}

/////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////

LRESULT CPreview::OnDestroy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	SaveWindowPosition();
	m_pPreviewData->OnDestroy();

	PostQuitMessage(0);
	return 0;
}

/////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////

LRESULT CPreview::OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint( hWnd, &ps );

	switch(m_pPreviewData->iTypeData)
	{
		case 0:
			//GetClientRect(hWnd,&rc);
			//FillRect(hDC, &rc, m_bkgndColor); 
			break;
		case 1:
			{			
				SendMessage(m_pPreviewData->hWndStaticBitmap,WM_PAINT,0,0);
				SendMessage(m_pPreviewData->hWndInfosFichier,WM_PAINT,0,0);

			}
			break;
		case 2:
#ifdef WMP9
			m_wmpHost->RedrawWindow();
			SendMessage(m_pPreviewData->hWndInfosFichier,WM_PAINT,0,0);
#else
			SendMessage(m_pPreviewData->hMCI,WM_PAINT,0,0);
#endif

			break;
		case 3:
			
			SendMessage(m_pPreviewData->hWndMP3,WM_PAINT,0,0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
	}


	EndPaint( hWnd, &ps );
	ReleaseDC(hWnd,hDC);

	return 0;
}

/////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////

LRESULT CPreview::OnEraseBackground(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(lParam); 
	int wmEvent = HIWORD(lParam); 	
	
	switch(wmId)
	{
		case IDM_CHGIMAGE_OK:
			{
				
				if(wmEvent == 0)
				{

					#pragma omp critical(previewibitmap)
					{
						float fRatio;
						//Ecrasement des données de l'image pour allégement mémoire
						//CEffet m_cEffet;
						if(m_pPreviewData->m_ciBitmap.GetBitmapWidth() > m_pPreviewData->m_ciBitmap.GetBitmapHeigth())
							fRatio = (float)((float)GetSystemMetrics(SM_CXSCREEN) / (float)m_pPreviewData->m_ciBitmap.GetBitmapWidth());
						else
							fRatio = (float)((float)GetSystemMetrics(SM_CYSCREEN) / (float)m_pPreviewData->m_ciBitmap.GetBitmapHeigth());

						m_iRightTemp = 0;
						m_iBottomTemp = 0;
						//m_cEffet.ScaleBitmap(m_pPreviewData->m_ciBitmap, m_pPreviewData->m_ciBitmap.GetBitmapWidth()*fRatio, m_pPreviewData->m_ciBitmap.GetBitmapHeigth()*fRatio, TRIANGLEFILTER,1);
						ShowBitmap();
						SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(IDM_UPDATESTATUS,4), MAKELPARAM(m_pPreviewData->m_ciBitmap.GetBitmapWidth(),m_pPreviewData->m_ciBitmap.GetBitmapHeigth()));
						SendMessage(m_pPreviewData->hWndInfosFichier,WM_COMMAND,MAKEWPARAM(IDM_FILENAME,0),(LPARAM)m_pPreviewData);
					}
				}
				else
				{
					#pragma omp critical(previewibitmap)
					{
						SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(IDM_UPDATESTATUS,4), MAKELPARAM(m_pPreviewData->m_ciBitmap.GetBitmapWidth(),m_pPreviewData->m_ciBitmap.GetBitmapHeigth()));
					}
					SendMessage(m_pPreviewData->hWndInfosFichier,WM_COMMAND,MAKEWPARAM(IDM_FILENAME,0),(LPARAM)m_pPreviewData);
					

				}
			}
			break;
			
		default:
			if(m_pPreviewData->iShowVideo)
			{
				m_wmpHost->RedrawWindow();
				SendMessage(m_pPreviewData->hWndInfosFichier,WM_PAINT,0,0);
			}
			else
				return DefWindowProc(hWnd, message, wParam, lParam);
			break;

	}
	return 0;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPreview::CPreview()
{

#ifdef WMP9
	CoInitialize(NULL);
	m_wmpHost = new CWMPHost();
	m_iRightFile = 0;
	m_iTopFile = 0;
#endif

	m_bOnSize = false;
	m_pPreviewData = new CPreviewData();
	m_ciBitmapTemp = new CIBitmap();
	m_iRightTemp = 0;
	m_iBottomTemp = 0;
	m_cInfosFichierProc = new CInfosFichierProc();
	m_bShow = false;
}

CPreview::~CPreview()
{
	if(m_pPreviewData != NULL)
		delete m_pPreviewData;

	m_pPreviewData = NULL;

	if(m_ciBitmapTemp != NULL)
		delete m_ciBitmapTemp;

	m_ciBitmapTemp = NULL;

	if(m_cMp3Traitement != NULL)
		delete m_cMp3Traitement;

	m_cMp3Traitement = NULL;

	if(m_cInfosFichierProc != NULL)
		delete m_cInfosFichierProc;

	m_cInfosFichierProc = NULL;

#ifdef WMP9
	if(m_wmpHost != NULL)
	{
		//m_wmpHost->OnClose();
		//m_wmpHost->DestroyWindow();
		delete m_wmpHost;
	}
	m_wmpHost = NULL;

	::CoUninitialize();
#endif
}

void CPreview::ShowMultimedia()
{
	//Test de Windows Media Player
	
#ifdef WMP9
	RECT rc;
	HDC hdc = GetDC(m_hWnd);

	if(!m_pPreviewData->iShowVideo)
	{
		//m_wmpHost->OnClose();
		m_pPreviewData->iShowVideo = 1;
		m_wmpHost->ShowWindow(SW_SHOWNORMAL);
		m_wmpHost->OnPlayFile(m_pPreviewData->cFile,strlen(m_pPreviewData->cFile)+1);
		SendMessage(m_hWnd,WM_SIZE,1,0);
		SendMessage(m_pPreviewData->hWndInfosFichier,WM_COMMAND,MAKEWPARAM(IDM_FILENAME,0),(LPARAM)m_pPreviewData);
	}
	else
	{
		m_wmpHost->OnPlayFile(m_pPreviewData->cFile,strlen(m_pPreviewData->cFile)+1);
		SendMessage(m_pPreviewData->hWndInfosFichier,WM_COMMAND,MAKEWPARAM(IDM_FILENAME,0),(LPARAM)m_pPreviewData);
		SendMessage(m_hWnd,WM_SIZE,0,0);
	}


#else

	RECT rc;
	HDC hdc = GetDC(m_hWnd);

	if(!::IsWindowVisible(m_pPreviewData->hMCI))
		::ShowWindow(m_pPreviewData->hMCI,SW_SHOW);

	if(m_pPreviewData->iShowVideo)
		MCIWndClose(m_pPreviewData->hMCI);

	if (MCIWndOpen(m_pPreviewData->hMCI,m_pPreviewData->cFile, 0) != 0)
	{
		::ShowWindow(m_pPreviewData->hMCI,SW_HIDE);
		m_pPreviewData->iShowVideo = 0;

	}
	else
	{
		MCIWndGetSource(m_pPreviewData->hMCI,&m_pPreviewData->rcVideo);

		hdc = GetDC(m_hWnd);
		GetClientRect(m_hWnd,&rc);
		FillRect(hdc, &rc, m_bkgndColor); 
		ReleaseDC(m_hWnd,hdc);
		
		m_pPreviewData->rcTemp.left = (rc.right - 320) / 2 ;
		m_pPreviewData->rcTemp.right  = 320;
		m_pPreviewData->rcTemp.top  = (rc.bottom - 240) / 2;
		m_pPreviewData->rcTemp.bottom = 240;

		m_pPreviewData->iShowVideo = 1;

		MoveWindow(m_pPreviewData->hMCI,m_pPreviewData->rcTemp.left, m_pPreviewData->rcTemp.top, m_pPreviewData->rcTemp.right, m_pPreviewData->rcTemp.bottom,false);
	}

#endif


	SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_NODRAGGING,0),0);
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void CPreview::ShowMP3()
{
	RECT rc;
	HDC hdc = GetDC(m_hWnd);
	GetClientRect(m_hWnd,&rc);
	ReleaseDC(m_hWnd,hdc);

	m_pPreviewData->rcTemp.left = (rc.right - 320) / 2 ;
	m_pPreviewData->rcTemp.right  = 320;
	m_pPreviewData->rcTemp.top  = (rc.bottom - 100) / 2;
	m_pPreviewData->rcTemp.bottom = 100;
	MoveWindow(m_pPreviewData->hWndMP3,m_pPreviewData->rcTemp.left, m_pPreviewData->rcTemp.top, m_pPreviewData->rcTemp.right, m_pPreviewData->rcTemp.bottom,true);
	::ShowWindow(m_pPreviewData->hWndMP3,SW_SHOW);
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void CPreview::ShowBitmap(const int &iRedraw)
{

	if(m_bShow)
		return;

	m_bShow = true;

	int iBottom, iPosY, iRight = 0, iLeft = 0, iHauteurInfos;
	float fRatio;
	RECT rc;//, rcInfos;
	
	HDC hdc = GetDC(m_hWnd);

	if(m_pPreviewData->iAffichageInfos == 1)
	{
		if(::IsWindowVisible(m_pPreviewData->hWndInfosFichier) == TRUE)
		{
			GetClientRect(m_pPreviewData->hWndInfosFichier,&rc);
			iHauteurInfos = rc.bottom - rc.top;
			GetClientRect(m_hWnd,&rc);
			if(iHauteurInfos <= 0 || iRedraw == 2)
			{	
				iHauteurInfos  = ((rc.bottom - rc.top) / 3);
			}
		}
		else
		{

			if(LoadWindowPosition(iHauteurInfos) == -1)
			{
				GetClientRect(m_hWnd,&rc);
				iHauteurInfos  = ((rc.bottom - rc.top) / 3);
			}
			else
			{
				//GetClientRect(m_pPreviewData->hWndInfosFichier,&rc);
				//iHauteurInfos = rc.bottom - rc.top;
				GetClientRect(m_hWnd,&rc);
				if(iHauteurInfos <= 0 || iRedraw == 2)
				{	
					iHauteurInfos  = ((rc.bottom - rc.top) / 3);
				}
			}
		}
	}
	else
	{
		GetClientRect(m_hWnd,&rc);
		::ShowWindow(m_pPreviewData->hWndInfosFichier,SW_HIDE);
		iHauteurInfos = 0;
	}
	rc.bottom -= iHauteurInfos + 20;
	
	m_pPreviewData->rcTemp.right  = rc.right - 10;
	m_pPreviewData->rcTemp.left = (rc.right - m_pPreviewData->rcTemp.right) / 2 ;
	m_pPreviewData->rcTemp.top  = rc.top + 20;
	m_pPreviewData->rcTemp.bottom = (rc.bottom - m_pPreviewData->rcTemp.top - 5);

	rc = m_pPreviewData->rcTemp;

	iRight = rc.right - 10;

	iLeft = rc.left + 5;

	fRatio = (float)((float)(iRight) / (float)m_pPreviewData->m_ciBitmap.GetBitmapWidth());
	iBottom = fRatio * m_pPreviewData->m_ciBitmap.GetBitmapHeigth();

	iPosY = (rc.bottom - iBottom) / 2 + rc.top;

	if(m_pPreviewData->m_ciBitmap.GetBitmapHeigth() > m_pPreviewData->m_ciBitmap.GetBitmapWidth() && iPosY < (20 + rc.top))
	{
		iBottom = rc.bottom - rc.top;
		fRatio = (float)((float)(iBottom) / (float)m_pPreviewData->m_ciBitmap.GetBitmapHeigth());
		iRight = fRatio * m_pPreviewData->m_ciBitmap.GetBitmapWidth();
		iLeft = (rc.right - iRight) / 2 + rc.left;
		iPosY = (rc.bottom - iBottom) / 2 + rc.top;
	
	}

	if(iRight > 0 && iBottom > 0 && (m_pPreviewData->m_ciBitmap.hBitmap != NULL || m_pPreviewData->m_ciBitmap.GetPtBitmap() != NULL))
	{
		CEffet cEffet;

		MoveWindow(m_pPreviewData->hWndStaticBitmap, iLeft, iPosY, iRight, iBottom, true);

		m_pPreviewData->rcTemp.left = iLeft;
		m_pPreviewData->rcTemp.top = iPosY;
		m_pPreviewData->rcTemp.bottom = iBottom;
		m_pPreviewData->rcTemp.right = iRight;

		if(m_pPreviewData->hBitmap != NULL)
			DeleteObject(m_pPreviewData->hBitmap);

		if(m_iRightTemp != iRight || m_iBottomTemp != iBottom)
		{
			m_ciBitmapTemp->SetBitmap(m_pPreviewData->m_ciBitmap.GetPtBitmap(),m_pPreviewData->m_ciBitmap.GetBitmapWidth(),m_pPreviewData->m_ciBitmap.GetBitmapHeigth());
			cEffet.ScaleBitmap(m_ciBitmapTemp, iRight, iBottom, m_pPreviewData->m_bSmooth);
			m_iRightTemp = iRight;
			m_iBottomTemp = iBottom;
		}

		SendMessage(m_pPreviewData->hWndStaticBitmap,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ciBitmapTemp->hBitmap);
		
		if((::IsWindowVisible(m_pPreviewData->hWndInfosFichier) == FALSE && m_pPreviewData->iAffichageInfos == 1) || iRedraw == 2)
		{
			GetClientRect(m_hWnd,&rc);

			/*
			m_pPreviewData->rcTemp.left = rc.left;
			m_pPreviewData->rcTemp.right  = rc.right;
			m_pPreviewData->rcTemp.top  = rc.bottom - iHauteurInfos;
			m_pPreviewData->rcTemp.bottom = iHauteurInfos - 5;*/

			MoveWindow(m_pPreviewData->hWndInfosFichier, 0, rc.bottom - iHauteurInfos, rc.right - rc.left, iHauteurInfos,true);

			if(iRedraw == 1)
				::ShowWindow(m_pPreviewData->hWndInfosFichier,SW_SHOW);
		}
	}

	m_bShow = false;
}

int CPreview::SetQuality(const int &iSmooth)
{
	m_pPreviewData->m_bSmooth = iSmooth;
	return 0;
}

int CPreview::SaveBitmap(const char *m_szFilename)
{
	m_pPreviewData->m_ChargementImage.ChargementImageStatic(&m_pPreviewData->m_ciBitmap, m_pPreviewData->cFile);
	CLoadPicture::SaveImage(CLoadPicture::BMP,m_pPreviewData->m_ciBitmap,(char*)m_szFilename);
	return 0;
}

int CPreview::SetDirName(const char *m_szDirectory)
{
	StringCchCopy(m_pPreviewData->cDirectory,MAX_PATH,m_szDirectory);
	return 0;
}
