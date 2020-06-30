// TraitementBitmap.cpp: implementation of the CTraitementBitmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TraitementBitmap.h"
#include ".\traitementdata.h"
#include <ibitmap.h>
#include "libdialog\aboutdialog.h"
#include "WndAfficheBitmap.h"
#include "statusbar.h"
#include "twaincpp.h"
#include "toolbar.h"
#include "LoadImage.h"
#include "parameter.h"
#include "CoolMenu.h"
#include <definerepmonitor.h>

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void CTraitementBitmap::CopyImage(HWND hWnd, HANDLE hBitmap,TW_IMAGEINFO& info)
{
	if(m_pTraitementData->m_ciBitmap != NULL)
		delete m_pTraitementData->m_ciBitmap;

	m_pTraitementData->m_ciBitmap = new CIBitmap();

	m_pTraitementData->m_ciBitmap->HandleToBitmap(hBitmap,info.BitsPerPixel,NULL);

	if(m_pTraitementData->m_ciBitmap->GetPtBitmap() != NULL)
	{
		SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_GETTWAINFILE,0),(LPARAM)m_pTraitementData->m_ciBitmap);
	}

}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTraitementBitmap::CTraitementBitmap()
{
	m_pTraitementData = new CTraitementData();
	szWindowClass[0] = '\0';
	szTitle[0] = '\0';
	cFileName[0] = '\0';
}

CTraitementBitmap::~CTraitementBitmap()
{
	if(m_pTraitementData != NULL)
		delete m_pTraitementData;

	m_pTraitementData = NULL;
}

LRESULT CTraitementBitmap::OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
				case ID_IMAGE_BOUGERIMAGE: 
					lpttt->lpszText = MAKEINTRESOURCE(IDS_HANDTOOLS); 
					break; 
				case ID_IMAGE_COUPERIMAGE: 
					lpttt->lpszText = MAKEINTRESOURCE(IDS_CROPTOOLS); 
					break; 
				case ID_IMAGE_ZOOMERIMAGE: 
					lpttt->lpszText = MAKEINTRESOURCE(IDS_ZOOMTOOLS); 
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
			}
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

LRESULT CTraitementBitmap::OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	wmId    = LOWORD(wParam); 
	wmEvent = HIWORD(wParam); 
	// Parse the menu selections:
	char cInfos[MAX_PATH];
	CObjet m_cObjet;

	switch (wmId)
	{

		case ID_FICHIER_SELECTIONNERSOURCE:
			{
				HMENU hMenu;
				hMenu = GetMenu(hWnd);
				
				SelectSource();
				if(IsValidDriver())
					EnableMenuItem(hMenu, ID_FICHIER_ACQUISITION, MF_ENABLED);
				else
					EnableMenuItem(hMenu, ID_FICHIER_ACQUISITION, MF_GRAYED);
			}
				
			break;

		case ID_FICHIER_ACQUISITION:
			if(IsValidDriver())
				Acquire(TWCPP_ANYCOUNT);
			else
			{
				SelectSource();
				if(IsValidDriver())
					Acquire(TWCPP_ANYCOUNT);
			}
			break;

		case IDM_GETHWNDPRGBAR:
			{
				CDefineRepMonitor * m_define;
				m_define = (CDefineRepMonitor *)lParam;
				m_define->hWnd = m_pTraitementData->hWndPrgBar;
			}
			break;


		case IDM_UPDATESTATUS:
			StringCchPrintf(cInfos,MAX_PATH,"%dx%d",LOWORD(lParam), HIWORD(lParam));
			m_pTraitementData->m_StatusBar->UpdateStatusBar(cInfos,0,0);
			break;

		case IDM_FILENAME:
			StringCchCopy(m_pTraitementData->cFile,MAX_PATH,(const char *)lParam);
			break;
		
		case IDM_DIRNAME:
			StringCchCopy(m_pTraitementData->cDirectory,MAX_PATH,(const char *)lParam);
			m_pTraitementData->m_StatusBar->UpdateStatusBar(m_pTraitementData->cDirectory,1,0);
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

		case IDM_CHANGEMENT_IMAGE:
			StringCchCopy(m_pTraitementData->cFile,MAX_PATH,(const char *)lParam);
			SetWindowText(m_pTraitementData->cFile);
			m_cObjet.GetInfosFile(m_pTraitementData->cFile,cInfos,MAX_PATH);
			m_pTraitementData->m_StatusBar->UpdateStatusBar(cInfos,2,0);
			break;

		case IDM_IMAGES_SHRINKIMAGE:
			char value[2];
			SendMessage(m_pTraitementData->m_CWndAfficheBitmap->m_hWnd,WM_COMMAND,MAKEWPARAM(IDM_GETSHRINK,0),(LPARAM)value);
			if(atoi(value))
				CheckMenuItem(GetMenu(hWnd),IDM_IMAGES_SHRINKIMAGE,MF_UNCHECKED);
			else
				CheckMenuItem(GetMenu(hWnd),IDM_IMAGES_SHRINKIMAGE,MF_CHECKED);
				

		default:
		   SendMessage(m_pTraitementData->m_CWndAfficheBitmap->m_hWnd, message, wParam, lParam);
		   return 0;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

LRESULT CTraitementBitmap::OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char cInfos[MAX_PATH];
	CObjet m_cObjet;

	m_pTraitementData->hWndStatusWindow = m_pTraitementData->m_StatusBar->CreateStatusBarCtrl(hWnd);
	m_pTraitementData->m_StatusBar->CreateMultipartStatus(m_pTraitementData->hWndStatusWindow,4,"100","300","800","1200","-1");

	m_pTraitementData->m_bFullScreen = false;

	m_cObjet.GetInfosFile(m_pTraitementData->cFile,cInfos,MAX_PATH);

	TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
	_splitpath_s(m_pTraitementData->cFile, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);	
	//_splitpath(m_pTraitementData->cFile, drive, dir, fname, ext);
	StringCchCopy(m_pTraitementData->cDirectory,MAX_PATH,drive);
	StringCbCat(m_pTraitementData->cDirectory,MAX_PATH,dir);

	m_pTraitementData->m_StatusBar->UpdateStatusBar(m_pTraitementData->cDirectory,1,0);
	m_pTraitementData->m_StatusBar->UpdateStatusBar(cInfos,2,0);
	
	RECT rc = {CW_USEDEFAULT, 0, CW_USEDEFAULT, 0};
	DWORD dwndStlyle = WS_CHILD | WS_VISIBLE;
	DWORD dStyle = CS_HREDRAW | CS_VREDRAW;
	DWORD dExStyle = WS_EX_STATICEDGE;
	m_pTraitementData->m_CWndAfficheBitmap->CreateEx(hWnd,hInstance,dwndStlyle,dStyle,dExStyle,"TraitementBitmap",0,0,IDC_ARROW,rc);
	
	SetWindowText(m_pTraitementData->cFile);

	RECT rcClient;
	GetClientRect(m_pTraitementData->hWndStatusWindow, &rcClient); 
	m_pTraitementData->hWndPrgBar = CreateWindowEx(0, PROGRESS_CLASS, (LPSTR) NULL, 
		WS_CHILD, 101, 
		3, 
		198, rcClient.bottom - rcClient.top - 3, 
		m_pTraitementData->hWndStatusWindow, (HMENU) 0, hInstance, NULL); 

	SendMessage(m_pTraitementData->hWndPrgBar, PBM_SETRANGE, 0, MAKELPARAM(0,100)); 
	SendMessage(m_pTraitementData->hWndPrgBar, PBM_SETSTEP, (WPARAM) 1, 0); 

	

	HMENU hMnu,hmenu, hsub,hSubMenu;
	hMnu = GetMenu(hWnd);
	EnableMenuItem(hMnu, ID_EDITION_ANNULER, MF_GRAYED);

#ifdef COOLMENU
	hmenu = GetMenu(hWnd);
	for(int i = 0;i < 7;i++)
	{
		hsub  = GetSubMenu(hmenu, i);
		m_pTraitementData->m_CCoolMenu->MakeCoolMenu(hsub, hInstance, MAKEINTRESOURCE(IDB_MENUBMP));
	}

	hsub  = GetSubMenu(hmenu, 2);
	hSubMenu = GetSubMenu(hsub, 0);
	m_pTraitementData->m_CCoolMenu->MakeCoolMenu(hSubMenu, hInstance, MAKEINTRESOURCE(IDB_MENUBMP));
	hSubMenu = GetSubMenu(hsub, 2);
	m_pTraitementData->m_CCoolMenu->MakeCoolMenu(hSubMenu, hInstance, MAKEINTRESOURCE(IDB_MENUBMP));
	hSubMenu = GetSubMenu(hsub, 4);
	m_pTraitementData->m_CCoolMenu->MakeCoolMenu(hSubMenu, hInstance, MAKEINTRESOURCE(IDB_MENUBMP));

	hsub  = GetSubMenu(hmenu, 3);
	hSubMenu = GetSubMenu(hsub, 0);
	m_pTraitementData->m_CCoolMenu->MakeCoolMenu(hSubMenu, hInstance, MAKEINTRESOURCE(IDB_MENUBMP));

#endif
	SendMessage(m_pTraitementData->m_CWndAfficheBitmap->m_hWnd, WM_COMMAND, MAKEWPARAM(IDM_FILENAME,0), (LPARAM)m_pTraitementData->cFile);

	SendMessage(m_pTraitementData->m_CWndAfficheBitmap->m_hWnd, WM_COMMAND, MAKEWPARAM(IDM_GETHWNDPRGBAR,0), (LPARAM)m_pTraitementData->hWndPrgBar);

	m_pTraitementData->hWndToolbar = m_pTraitementData->m_CToolbar->CreateRebar(hWnd,hInstance, 5);

	HMENU hMenu;
	hMenu = GetMenu(hWnd);
	EnableMenuItem(hMenu, ID_FICHIER_ACQUISITION, MF_ENABLED);

	CParameter::LoadWindowPlacement("SOFTWARE\\Regards\\traitement",hWnd);	

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

LRESULT CTraitementBitmap::OnDestroy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	CParameter::SaveWindowPlacement("SOFTWARE\\Regards\\traitement",hWnd);	

	m_pTraitementData->m_CWndAfficheBitmap->VerifModification();

	SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_DESTROYTRAITEMENT,0),(LPARAM)hWnd);

#ifdef COOLMENU
	HMENU hmenu, hsub,hSubMenu;

	hmenu = GetMenu(hWnd);
	for(int i = 0;i < 7;i++)
	{
		hsub  = GetSubMenu(hmenu, i);
		m_pTraitementData->m_CCoolMenu->RemoveCoolMenu(hsub);
	}

	hsub  = GetSubMenu(hmenu, 2);
	hSubMenu = GetSubMenu(hsub, 0);
	m_pTraitementData->m_CCoolMenu->RemoveCoolMenu(hSubMenu);
	hSubMenu = GetSubMenu(hsub, 2);
	m_pTraitementData->m_CCoolMenu->RemoveCoolMenu(hSubMenu);
	hSubMenu = GetSubMenu(hsub, 4);
	m_pTraitementData->m_CCoolMenu->RemoveCoolMenu(hSubMenu);

	hsub  = GetSubMenu(hmenu, 3);
	hSubMenu = GetSubMenu(hsub, 0);
	m_pTraitementData->m_CCoolMenu->RemoveCoolMenu(hSubMenu);
#endif

	PostQuitMessage(0);

	return 0;
}

LRESULT CTraitementBitmap::OnSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int yTop;
	yTop = 0;
	int xNewSize; 
	int yNewSize; 
 	yNewSize = HIWORD (lParam); 
	xNewSize = LOWORD (lParam); 
	m_pTraitementData->m_StatusBar->DeplaceStatusBar(hWnd);
	//yNewSize -= 60;
	//yTop = 40;

	RECT rc,rcPos;
	::GetClientRect(m_pTraitementData->hWndStatusWindow,&rc);
	yNewSize -= rc.bottom;
	::GetClientRect(m_pTraitementData->hWndToolbar,&rc);
	yNewSize -= rc.bottom;
	yTop = rc.bottom;

	::GetClientRect(hWnd,&rcPos);

	MoveWindow(m_pTraitementData->m_CWndAfficheBitmap->m_hWnd,0,yTop, xNewSize, yNewSize,true);

	MoveWindow(m_pTraitementData->hWndToolbar,rc.left,rc.top, rcPos.right, rc.bottom,true);
	return 0;
}

LRESULT CTraitementBitmap::OnMeasureItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef COOLMENU
	if(wParam == 0)
		return m_pTraitementData->m_CCoolMenu->CoolMenu_MeasureItem(wParam, lParam);
#endif
	return 0;
}

LRESULT CTraitementBitmap::OnPaint(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	RECT rc;
	HDC hDC = BeginPaint( hWnd, &ps );
	
	SendMessage(m_pTraitementData->hWndToolbar, msg,wParam,lParam);
	SendMessage(m_pTraitementData->hWndPrgBar, msg,wParam,lParam);
	SendMessage(m_pTraitementData->hWndStatusWindow, msg,wParam,lParam);
	SendMessage(m_pTraitementData->HwndBitmap, msg,wParam,lParam);

	EndPaint(hWnd, &ps);
	ReleaseDC(hWnd,hDC);
	return 0;
}

LRESULT CTraitementBitmap::OnEraseBackground(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	GetClientRect(m_pTraitementData->hWndToolbar,&rc);
	RedrawWindow(m_pTraitementData->hWndToolbar,&rc,NULL,RDW_INTERNALPAINT);
	GetClientRect(m_pTraitementData->hWndPrgBar,&rc);
	RedrawWindow(m_pTraitementData->hWndPrgBar,&rc,NULL,RDW_INTERNALPAINT);
	GetClientRect(m_pTraitementData->hWndStatusWindow,&rc);
	RedrawWindow(m_pTraitementData->hWndStatusWindow,&rc,NULL,RDW_INTERNALPAINT);
	GetClientRect(m_pTraitementData->HwndBitmap,&rc);
	RedrawWindow(m_pTraitementData->HwndBitmap,&rc,NULL,RDW_INTERNALPAINT);
	return 0;
}

LRESULT CTraitementBitmap::OnDrawItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef COOLMENU
	if(wParam == 0)
		return m_pTraitementData->m_CCoolMenu->CoolMenu_DrawItem(wParam, lParam);
#endif
	return 0;
}

LRESULT CTraitementBitmap::OnKeyDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	SendMessage(m_pTraitementData->m_CWndAfficheBitmap->m_hWnd, message, wParam, lParam);
	return 0;
}

LRESULT CTraitementBitmap::OnMouseWheel(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	SendMessage(m_pTraitementData->m_CWndAfficheBitmap->m_hWnd, message, wParam, lParam);
	return 0;
}

int CTraitementBitmap::Run(int iAccelerator)
{
	MSG msg;

	InitTwain(m_hWnd);

	HACCEL hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)iAccelerator);

	SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDM_GETTRAITEMENTWND,0),(LPARAM)m_hWnd);
	
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if(!ProcessMessage(msg))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
	}

	ReleaseTwain();

	return 0;
}

int CTraitementBitmap::SetFilename(const char *m_szFilename)
{
	StringCchCopy(m_pTraitementData->cFile,MAX_PATH,m_szFilename);
	return 0;
}
