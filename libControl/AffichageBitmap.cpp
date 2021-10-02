// AffichageBitmap.cpp: implementation of the CAffichageBitmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "affichagebitmap.h"
#include "affichagebitmapWindow.h"
#include "diaporama.h"
#include "parameter.h"
#include <winuser.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAffichageBitmap::CAffichageBitmap()
{
	pimpl_ = new CAffichageBitmapWindow();
	pimpl_->SetDone(TRUE);


	szWindowClass[0] = '\0';
	szTitle[0] = '\0';
	StringCchCopy(szWindowClass,MAX_LOADSTRING,"AffichageBitmap");
	hWnd = NULL;

	
}

CAffichageBitmap::~CAffichageBitmap()
{
	DestroyWindow(hWnd);
	delete pimpl_;
}

int CAffichageBitmap::CreateBitmapWindow(int nCmdShow)
{
	pimpl_->SetOnCreate(true);

	if(hWnd != NULL)
	{
		//CloseWindow(hWnd);
		DestroyWindow(hWnd);
		//return 0;
		//CloseHandle(hWnd);
	}

 	// TODO: Place code here.
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_AFFICHAGEBITMAP, szWindowClass, MAX_LOADSTRING);

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc	= (WNDPROC)CAffichageBitmap::WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= sizeof(this);
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDM_REGARDS);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);

	if(pimpl_->GetFullScreen())
		wcex.lpszMenuName	= NULL;
	else
		wcex.lpszMenuName	= (LPCSTR)IDM_AFFICHAGEBITMAP;

	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDM_REGARDS);

	RegisterClassEx(&wcex);

	if(pimpl_->GetFullScreen())
	{
		RECT m_rectScreen;
		if(CParameter::GetWindowPlacement("SOFTWARE\\Regards\\navigation",m_rectScreen) != -1)
		{
			MONITORINFO mi;
			mi.cbSize = sizeof( mi );
			HMONITOR hMonitor = MonitorFromRect(&m_rectScreen,MONITOR_DEFAULTTONEAREST);
			if(hMonitor != NULL)
			{
				GetMonitorInfo( hMonitor, &mi );
				hWnd = CreateWindowEx(WS_EX_TOPMOST, szWindowClass, szTitle, WS_POPUPWINDOW | WS_VISIBLE,
					mi.rcMonitor.left, 0, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom, NULL, NULL, hInstance, this);
			}
			else
			{
				// Perform application initialization:
				hWnd = CreateWindowEx(WS_EX_TOPMOST, szWindowClass, szTitle, WS_POPUPWINDOW | WS_VISIBLE,
				  0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInstance, this);
			}

		}
		else
		{
			hWnd = CreateWindowEx(WS_EX_TOPMOST, szWindowClass, szTitle, WS_POPUPWINDOW | WS_VISIBLE,
			  0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInstance, this);
		}

		if (!hWnd)
		{
		  return FALSE;
		}

		SetMenu(hWnd,NULL);

		pimpl_->SetAffichageWindow(hWnd);

		ShowWindow(hWnd, nCmdShow);
		
	}
	else
	{
		hWnd = CreateWindowEx(0L, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, this);

		if (!hWnd)
		{
		  return FALSE;
		}

		pimpl_->SetAffichageWindow(hWnd);

		ShowWindow(hWnd, nCmdShow);

		pimpl_->SetWindowPlacement(hWnd);
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_AFFICHAGEBITMAP);

	MSG msg;

	pimpl_->SetOnCreate(false);

	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if(pimpl_->VideoGetFullScreenState() != -1 && !pimpl_->Affichage3D())
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	}

	//DestroyWindow(hWnd);

	return 1;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

bool CAffichageBitmap::GetExplorer()
{
	return pimpl_->GetExplorer();
}

void CAffichageBitmap::SetExplorer(bool bExplorer)
{
	pimpl_->SetExplorer(bExplorer);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK CAffichageBitmap::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{ 

		case WM_PAINT:
			{
				CAffichageBitmap * m_cAffichagePrv = (CAffichageBitmap *) GetWindowLong (hWnd, GWL_USERDATA);
				return m_cAffichagePrv->pimpl_->OnPaint(hWnd, message, wParam, lParam);
			}

		case WM_ERASEBKGND:
			{
				CAffichageBitmap * m_cAffichagePrv = (CAffichageBitmap *) GetWindowLong (hWnd, GWL_USERDATA);
				return m_cAffichagePrv->pimpl_->OnEraseBackGround(hWnd, message, wParam, lParam);
			}

		case WM_CREATE: 
			{
				CAffichageBitmap * m_cAffichagePrv = (CAffichageBitmap *)(((LPCREATESTRUCT) lParam)->lpCreateParams);
				SetWindowLong (hWnd, GWL_USERDATA, (long) m_cAffichagePrv);
				return m_cAffichagePrv->pimpl_->OnCreate(hWnd, message, wParam, lParam);
			}

		case WM_COMMAND:
			{
				CAffichageBitmap * m_cAffichagePrv = (CAffichageBitmap *) GetWindowLong (hWnd, GWL_USERDATA);
				int wmId = 0, wmEvent = 0;
				wmId    = LOWORD(wParam); 
				wmEvent = HIWORD(wParam); 

				switch (wmId)
				{
					case IDM_CREATEBITMAPWINDOW:
						m_cAffichagePrv->pimpl_->SetOnCreate(true);
						m_cAffichagePrv->CreateBitmapWindow(SW_SHOW);
						return 0;
						break;

				}

				return m_cAffichagePrv->pimpl_->OnCommand(hWnd, message, wParam, lParam);
			}

		case WM_KEYDOWN:
			{
				CAffichageBitmap * m_cAffichagePrv = (CAffichageBitmap *) GetWindowLong (hWnd, GWL_USERDATA);
				return m_cAffichagePrv->pimpl_->OnKeyDown(hWnd, message, wParam, lParam);
			}

		case WM_MOUSEWHEEL:
			{
				CAffichageBitmap * m_cAffichagePrv = (CAffichageBitmap *) GetWindowLong (hWnd, GWL_USERDATA);
				return m_cAffichagePrv->pimpl_->OnMouseWheel(hWnd, message, wParam, lParam);
			}

		case WM_SIZE: 
			{
				CAffichageBitmap * m_cAffichagePrv = (CAffichageBitmap *) GetWindowLong (hWnd, GWL_USERDATA);
				return m_cAffichagePrv->pimpl_->OnSize(hWnd, message, wParam, lParam);
			}

		case WM_MOVE: 
			{
				CAffichageBitmap * m_cAffichagePrv = (CAffichageBitmap *) GetWindowLong (hWnd, GWL_USERDATA);
				return m_cAffichagePrv->pimpl_->OnMove(hWnd, message, wParam, lParam);
			}

		case WM_NOTIFY:
			{
				CAffichageBitmap * m_cAffichagePrv = (CAffichageBitmap *) GetWindowLong (hWnd, GWL_USERDATA);
				return m_cAffichagePrv->pimpl_->OnNotify(hWnd, message, wParam, lParam);
			}

		case WM_DESTROY:
			{
				CAffichageBitmap * m_cAffichagePrv = (CAffichageBitmap *) GetWindowLong (hWnd, GWL_USERDATA);
				return m_cAffichagePrv->pimpl_->OnDestroy(hWnd, message, wParam, lParam);
			}

		case WM_KILLFOCUS:
			{
				CAffichageBitmap * m_cAffichagePrv = (CAffichageBitmap *) GetWindowLong (hWnd, GWL_USERDATA);
				return m_cAffichagePrv->pimpl_->OnKillFocus(hWnd, message, wParam, lParam);
			}

		case WM_EXITMENULOOP:
			{
				CAffichageBitmap * m_cAffichagePrv = (CAffichageBitmap *) GetWindowLong (hWnd, GWL_USERDATA);
				return m_cAffichagePrv->pimpl_->OnExitMenuLoop(hWnd, message, wParam, lParam);
			}

		case WM_MEASUREITEM:
			{
				CAffichageBitmap * m_cAffichagePrv = (CAffichageBitmap *) GetWindowLong (hWnd, GWL_USERDATA);
				return m_cAffichagePrv->pimpl_->OnMeasureItem(hWnd, message, wParam, lParam);
			}

		case WM_DRAWITEM:
			{
				CAffichageBitmap * m_cAffichagePrv = (CAffichageBitmap *) GetWindowLong (hWnd, GWL_USERDATA);
				return m_cAffichagePrv->pimpl_->OnDrawItem(hWnd, message, wParam, lParam);
			}

		case WM_WINDOWPOSCHANGING:
			{
				CAffichageBitmap * m_cAffichagePrv = (CAffichageBitmap *) GetWindowLong (hWnd, GWL_USERDATA);
				return m_cAffichagePrv->pimpl_->OnWindowPosChanging(hWnd, message, wParam, lParam);
			}

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;

}

int CAffichageBitmap::CreateScreenWindow(HWND hParent, HINSTANCE hInstance,int nCmdShow, const char * cFileName, const int iTrie)
{
	//Initialisation des variables
	//CObjet m_cObjet;
	CDiaporama * m_cDiaporama = new CDiaporama(hInstance);
	
	pimpl_->DefineStartValue(hParent,iTrie);

	TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
	_splitpath_s(cFileName, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);	
	
	//_splitpath(cFileName, drive, dir, fname, ext);

	//Test si le fichier existe ou non
	int fichier_source = 0;// _open(cFileName, _O_RDONLY | _O_BINARY);

	if(_sopen_s(&fichier_source,cFileName,_O_RDONLY | _O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE) == -1)
		return -1;

	if(fichier_source == -1)
		return -1;

	_close(fichier_source);

	pimpl_->SetFilename(cFileName);
	pimpl_->SetDirectory(drive,dir);

	pimpl_->m_cParameter->SetStartDiaporama(false);
	
	CreateBitmapWindow(nCmdShow);

	delete m_cDiaporama;
	return 0;
}

