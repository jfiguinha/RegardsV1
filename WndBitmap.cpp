// WndBitmap.cpp: implementation of the CWndBitmap class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "WndBitmap.h"
#include <gl/glew.h>
#include <Regardsbitmap.h>
#include "statusbar.h"
#include "filtre.h"
#include "scroll.h"
#include "objet.h"
#include "ListPreviewAffWindow.h"
#include "NavigueData.h"
#include <ibitmap.h>
using namespace Regards::libbitmap;

//#ifndef WM_MOUSEWHEEL
//#include "zmouse.h"
//#endif

/////////////////////////////////////////////////////////////
//Variable static
/////////////////////////////////////////////////////////////

void CWndBitmap::KillGLWindow()							
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
}

////////////////////////////////////////////////////////////////////////////////////////
// Create OpenGL Window
////////////////////////////////////////////////////////////////////////////////////////

// Set up the pixel format
int CWndBitmap::MySetPixelFormat()
{
	PIXELFORMATDESCRIPTOR pfd = { 
	    sizeof(PIXELFORMATDESCRIPTOR),    // size of this pfd 
	    1,                                // version number 
	    PFD_DRAW_TO_WINDOW |              // support window 
	    PFD_SUPPORT_OPENGL |              // support OpenGL 
	    PFD_DOUBLEBUFFER,                 // double buffered 
	    PFD_TYPE_RGBA,                    // RGBA type 
	    32,                               // 24-bit color depth 
	    0, 0, 0, 0, 0, 0,                 // color bits ignored 
	    0,                                // no alpha buffer 
	    0,                                // shift bit ignored 
	    0,                                // no accumulation buffer 
	    0, 0, 0, 0,                       // accum bits ignored 
	    32,                               // 32-bit z-buffer     
	    0,                                // no stencil buffer 
	    0,                                // no auxiliary buffer 
	    PFD_MAIN_PLANE,                   // main layer 
	    0,                                // reserved 
	    0, 0, 0                           // layer masks ignored 
	}; 
	
	int  iPixelFormat; 
 
	// get the device context's best, available pixel format match 
	if((iPixelFormat = ChoosePixelFormat(hDC, &pfd)) == 0)
	{
		//MessageBox(NULL, "ChoosePixelFormat Failed", NULL, MB_OK);
		return 0;
	}
	 
	// make that match the device context's current pixel format 
	if(SetPixelFormat(hDC, iPixelFormat, &pfd) == FALSE)
	{
		//MessageBox(NULL, "SetPixelFormat Failed", NULL, MB_OK);
		return 0;
	}

    // Create the render context
	hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRC);

	glewInit();

	return TRUE;
}

void CWndBitmap::SetFullScreen(bool bValue)
{
	m_cNavigueData->SetFullScreen(bValue);
}

void CWndBitmap::SetShrinkImage(bool bValue)
{
	m_cNavigueData->GetCBitmap()->SetShrinkImage(bValue);
	if(bValue)
	{
		m_cNavigueData->GetCBitmap()->SetBitmapSize(m_cNavigueData->GetCIBitmap()->GetBitmapWidth(),m_cNavigueData->GetCIBitmap()->GetBitmapHeigth());
		m_cNavigueData->GetCBitmap()->InitScrolling();
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWndBitmap::CWndBitmap()
{
	m_cNavigueData = new CNavigueData();
}

CWndBitmap::~CWndBitmap()
{
	KillGLWindow();

	if(m_cNavigueData != NULL)
		delete m_cNavigueData;

	m_cNavigueData = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndBitmap::SizeCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	GetClientRect(hWnd,&rc);	
	int yNewSize, xNewSize;
   	yNewSize = HIWORD (lParam); 
	xNewSize = LOWORD (lParam); 

	CObjet m_cObjet;

	m_cNavigueData->GetCBitmap()->SetfSize(true);

	m_cNavigueData->GetCBitmap()->SetSize(xNewSize,yNewSize);

	if(!m_cNavigueData->GetCBitmap()->GetFullscreenState())
	{
		//if(m_cObjet.GetWindowsVersion() == 0)
		//	m_cNavigueData->GetCBitmap()->AffichageBitmap(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetCScroll()->GetCurrentHScroll(), m_cNavigueData->GetCScroll()->GetCurrentVScroll());
		
		m_cNavigueData->GetCScroll()->DefineSize(m_cNavigueData->GetCIBitmap()->GetBitmapWidth() * m_cNavigueData->GetCBitmap()->GetRatio(), m_cNavigueData->GetCIBitmap()->GetBitmapHeigth() * m_cNavigueData->GetCBitmap()->GetRatio(),0,0);
	}
	else
	{
		//if(m_cObjet.GetWindowsVersion() == 0)
		//	m_cNavigueData->GetCBitmap()->AffichageBitmap(hWnd, m_cNavigueData->GetCIBitmap(), 0, 0);
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndBitmap::ProcessCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	hDC = GetDC(hWnd);
	MySetPixelFormat();
	m_cNavigueData->GetCScroll()->CreateScrollWindow(hWnd,hInstance);
	m_cNavigueData->GetCScroll()->Initialize();
	m_cNavigueData->GetCScroll()->DefineSize(m_cNavigueData->GetCIBitmap()->GetBitmapWidth() * m_cNavigueData->GetCBitmap()->GetRatio(), m_cNavigueData->GetCIBitmap()->GetBitmapHeigth() * m_cNavigueData->GetCBitmap()->GetRatio(),0,0);
	m_cNavigueData->GetCBitmap()->SetfScroll(true);
	m_cNavigueData->GetCBitmap()->SetfSize(true);
	ShowScrollBar(hWnd,SB_BOTH,true);
	m_cNavigueData->GetCBitmap()->InitScrolling();
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndBitmap::EraseBgrCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	GetClientRect(hWnd,&rc);

	switch(LOWORD(lParam))
	{

		case IDM_CHGIMAGE_OK :
			{
#ifdef _DEBUG
	OutputDebugString("EraseBgrCreate IDM_CHGIMAGE_OK\n");
#endif

				m_cNavigueData->GetCBitmap()->InitScrolling();
				m_cNavigueData->GetCBitmap()->InitRatio();

				if((m_cNavigueData->GetCBitmap()->GetShrinkState() && m_cNavigueData->GetCBitmap()->GetRatio() == 1.0) || m_cNavigueData->GetCBitmap()->GetFullscreenState())
				{
					m_cNavigueData->GetCScroll()->DefineSize(0, 0,0,0);
					m_cNavigueData->GetCScroll()->Size(hWnd, 0,0);
					m_cNavigueData->GetCBitmap()->AffichageBitmap(hWnd, m_cNavigueData->GetCIBitmap(), 0, 0);					
				}
				else
				{
					m_cNavigueData->GetCScroll()->DefineSize(m_cNavigueData->GetCIBitmap()->GetBitmapWidth(),m_cNavigueData->GetCIBitmap()->GetBitmapHeigth(),0,0);
					SendMessage(hWnd,WM_HSCROLL,MAKEWPARAM(SB_THUMBPOSITION,0),MAKELPARAM(0,0));
					SendMessage(hWnd,WM_VSCROLL,MAKEWPARAM(SB_THUMBPOSITION,0),MAKELPARAM(0,0));
					m_cNavigueData->GetCScroll()->Size(hWnd, m_cNavigueData->GetCBitmap()->GetxNewSize(), m_cNavigueData->GetCBitmap()->GetyNewSize());
					m_cNavigueData->GetCBitmap()->AffichageBitmap(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetCScroll()->GetCurrentHScroll(), m_cNavigueData->GetCScroll()->GetCurrentVScroll());
				}
				
				SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(IDM_UPDATESTATUS,0), MAKELPARAM(m_cNavigueData->GetCIBitmap()->GetBitmapWidth(),m_cNavigueData->GetCIBitmap()->GetBitmapHeigth()));
				return 0;
			}
			break;

		case IDM_FILTREIMAGE_OK:
			{
#ifdef _DEBUG
	OutputDebugString("EraseBgrCreate IDM_FILTREIMAGE_OK\n");
#endif

				if((m_cNavigueData->GetCBitmap()->GetShrinkState() && m_cNavigueData->GetCBitmap()->GetRatio() == 1.0) || m_cNavigueData->GetCBitmap()->GetFullscreenState())
				{
					m_cNavigueData->GetCScroll()->DefineSize(0, 0,0,0);
					m_cNavigueData->GetCScroll()->Size(hWnd, 0,0);
					m_cNavigueData->GetCBitmap()->AffichageBitmap(hWnd,m_cNavigueData->GetCIBitmap(), 0, 0);
					SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(IDM_UPDATESTATUS,0), MAKELPARAM(LOWORD(wParam),HIWORD(wParam)));
					return 0;

				}
				else
				{
					m_cNavigueData->GetCScroll()->DefineSize(LOWORD(wParam),HIWORD(wParam),0,0);
					SendMessage(hWnd,WM_HSCROLL,MAKEWPARAM(SB_THUMBPOSITION,0),MAKELPARAM(0,0));
					SendMessage(hWnd,WM_VSCROLL,MAKEWPARAM(SB_THUMBPOSITION,0),MAKELPARAM(0,0));

					m_cNavigueData->GetCScroll()->Size(hWnd, m_cNavigueData->GetCBitmap()->GetxNewSize(), m_cNavigueData->GetCBitmap()->GetyNewSize());
					m_cNavigueData->GetCBitmap()->AffichageBitmap(hWnd,m_cNavigueData->GetCIBitmap(), 0, 0);
				}

				SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(IDM_UPDATESTATUS,0), MAKELPARAM(LOWORD(wParam),HIWORD(wParam)));

				return 0;
			}
			break;


		case IDM_ZOOM_OK:
			{		
#ifdef _DEBUG
	OutputDebugString("EraseBgrCreate IDM_ZOOM_OK\n");
#endif

				if(!m_cNavigueData->GetCBitmap()->GetFullscreenState())
				{

					m_cNavigueData->GetCScroll()->DefineSize(LOWORD(wParam),HIWORD(wParam),0,0);
				
					if(!m_cNavigueData->GetCBitmap()->GetShrinkState())
						m_cNavigueData->GetCScroll()->Size(hWnd, m_cNavigueData->GetCBitmap()->GetxNewSize(), m_cNavigueData->GetCBitmap()->GetyNewSize());

					SendMessage(hWnd,WM_HSCROLL,MAKEWPARAM(SB_THUMBPOSITION,m_cNavigueData->GetCBitmap()->GetPosLargeur()),lParam);
					SendMessage(hWnd,WM_VSCROLL,MAKEWPARAM(SB_THUMBPOSITION,m_cNavigueData->GetCBitmap()->GetPosHauteur()),lParam);
					m_cNavigueData->GetCBitmap()->AffichageBitmap(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetCScroll()->GetCurrentHScroll(), m_cNavigueData->GetCScroll()->GetCurrentVScroll());

				}
				else
				{
					//PositionnementImage(hWnd, lParam, m_cNavigueData->GetCBitmap(), m_cNavigueData->GetCIBitmap());
					m_cNavigueData->GetCBitmap()->AffichageBitmap(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetCBitmap()->GetPosLargeur(), m_cNavigueData->GetCBitmap()->GetPosHauteur());
				}
				return 0;

			}				
			break;

		case IDM_REDRAW_PICTURE:
#ifdef _DEBUG
	OutputDebugString("EraseBgrCreate IDM_REDRAW_PICTURE\n");
#endif

			m_cNavigueData->GetCBitmap()->AffichageBitmap(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetCBitmap()->GetPosLargeur(), m_cNavigueData->GetCBitmap()->GetPosHauteur());
			break;

		case IDM_REDRAW_OK:
			{

#ifdef _DEBUG
	OutputDebugString("EraseBgrCreate IDM_REDRAW_OK\n");
#endif
				if(m_cNavigueData->GetCBitmap()->GetMouseBlock())
				{
					switch(m_cNavigueData->GetSelect())
					{
						case 1:
							m_cNavigueData->GetCBitmap()->AffichageBitmap(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetCBitmap()->GetPosLargeur(), m_cNavigueData->GetCBitmap()->GetPosHauteur());
							break;
					}
				}

			}
			break;
	}

	
	if((m_cNavigueData->GetCBitmap()->GetShrinkState() && m_cNavigueData->GetCBitmap()->GetRatio() == 1.0) || m_cNavigueData->GetCBitmap()->GetFullscreenState())
	{
		
#ifdef _DEBUG
	OutputDebugString("EraseBgrCreate TOTO 1\n");
#endif

		if(CObjet::GetWindowsVersion() == 0)
			m_cNavigueData->GetCBitmap()->AffichageBitmap(hWnd, m_cNavigueData->GetCIBitmap(), 0, 0);

		if(m_cNavigueData->GetCBitmap()->GetFullscreenState())
		{
			m_cNavigueData->GetCScroll()->DefineSize(0, 0,0,0);
			m_cNavigueData->GetCScroll()->Size(hWnd, 0,0);
		}
		return 0;
	}

	if(LOWORD(lParam) != IDM_ZOOM_OK)
	{
#ifdef _DEBUG
	OutputDebugString("EraseBgrCreate TOTO 2\n");
#endif

		m_cNavigueData->GetCScroll()->Size(hWnd, m_cNavigueData->GetCBitmap()->GetxNewSize(), m_cNavigueData->GetCBitmap()->GetyNewSize());
	}


	if(CObjet::GetWindowsVersion() == 0)
	{
#ifdef _DEBUG
	OutputDebugString("EraseBgrCreate TOTO 3\n");
#endif

		if(m_cNavigueData->GetCBitmap()->GetFullscreenState() || (m_cNavigueData->GetCBitmap()->GetShrinkState() && m_cNavigueData->GetCBitmap()->GetRatio() == 1.0))
			m_cNavigueData->GetCBitmap()->AffichageBitmap(hWnd, m_cNavigueData->GetCIBitmap(), 0, 0);
		else
			m_cNavigueData->GetCBitmap()->AffichageBitmap(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetCScroll()->GetCurrentHScroll(), m_cNavigueData->GetCScroll()->GetCurrentVScroll());
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CWndBitmap::MouseAndKeyCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//static int m_iMouseScrollX, m_iMouseScrollY;
	//static bool m_bBlocking;

	switch (message) 
	{ 
		case WM_KEYDOWN:
			RECT rc;
			GetClientRect(hWnd,&rc);
			switch(wParam)
			{
				case 187:
					SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_IMAGES_ZOOM_ZOOMIN,0),0);
					break;

				case 219:
					SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_IMAGES_ZOOM_ZOOMOUT,0),0);
					break;

				case VK_UP:
					DeplacementCurseur(hWnd,rc,-40,true,lParam,  m_cNavigueData->GetCBitmap(), m_cNavigueData->GetCIBitmap());
					break;
				case VK_LEFT:
					DeplacementCurseur(hWnd,rc,-40,false,lParam,  m_cNavigueData->GetCBitmap(), m_cNavigueData->GetCIBitmap());
					break;
				case VK_DOWN:
					DeplacementCurseur(hWnd,rc,+40,true,lParam,  m_cNavigueData->GetCBitmap(), m_cNavigueData->GetCIBitmap());
					break;
				case VK_RIGHT:
					DeplacementCurseur(hWnd,rc,+40,false,lParam,  m_cNavigueData->GetCBitmap(), m_cNavigueData->GetCIBitmap());
					break;

				case VK_F4:

#ifdef WMP9
					if(m_cNavigueData->GetVideo())
					{
						SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_SETFULLSCREEN,IDM_SETFULLSCREEN),0);
						break;
					}

#endif


					if(!m_cNavigueData->GetCBitmap()->GetFullscreenState())
					{
						SetWindowLong(hWnd,GWL_EXSTYLE,0);
						m_cNavigueData->GetCBitmap()->SetFullScreen(true);
						ShowScrollBar(hWnd,SB_BOTH,false);
					}
					break;

				case VK_ESCAPE:
					if(m_cNavigueData->GetCBitmap()->GetFullscreenState())
					{
						SetWindowLong(hWnd,GWL_EXSTYLE,WS_EX_CLIENTEDGE);
						m_cNavigueData->GetCBitmap()->SetFullScreen(false);
						m_cNavigueData->GetCBitmap()->AffichageBitmap(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetCScroll()->GetCurrentHScroll(), m_cNavigueData->GetCScroll()->GetCurrentVScroll());
					}
					return 0;

			}
			break;

		case WM_HSCROLL: 
			if(!m_cNavigueData->GetCBitmap()->GetFullscreenState())
			{
				m_cNavigueData->GetCScroll()->Hscroll(hWnd,LOWORD(wParam),HIWORD(wParam), m_cNavigueData->GetCBitmap()->GetfScroll());
				
				if(m_cNavigueData->GetCBitmap()->GetfScroll() && !m_cNavigueData->GetCBitmap()->GetMouseBlock())
					m_cNavigueData->GetCBitmap()->SetLargeurPos(m_cNavigueData->GetCScroll()->GetCurrentHScroll());
			}
			return 1L; 
 
		case WM_VSCROLL:
			if(!m_cNavigueData->GetCBitmap()->GetFullscreenState())
			{
				m_cNavigueData->GetCScroll()->Vscroll(hWnd,LOWORD(wParam), HIWORD(wParam), m_cNavigueData->GetCBitmap()->GetfScroll());
				if(m_cNavigueData->GetCBitmap()->GetfScroll() && !m_cNavigueData->GetCBitmap()->GetMouseBlock())
					m_cNavigueData->GetCBitmap()->SetPosHauteur(m_cNavigueData->GetCScroll()->GetCurrentVScroll());
			}
	
			return 1L;

		case WM_LBUTTONDOWN:
			{
				m_cNavigueData->GetCBitmap()->SetMouseBlock(true);
				m_cNavigueData->GetCBitmap()->SetMouseScroll(LOWORD(lParam), HIWORD(lParam));
				if(!m_cNavigueData->GetCBitmap()->GetFullscreenState())
					m_cNavigueData->GetCBitmap()->SetPosition(m_cNavigueData->GetCScroll()->GetCurrentHScroll(), m_cNavigueData->GetCScroll()->GetCurrentVScroll());

				SetCapture(hWnd);
				SetCursor (LoadCursor (hInstance, MAKEINTRESOURCE(IDC_CURSOR1))) ;
			}
			return 0;

		case WM_LBUTTONUP:
			{
				m_cNavigueData->GetCBitmap()->SetMouseBlock(false);
				m_cNavigueData->GetCBitmap()->SetMouseScroll(LOWORD(lParam), HIWORD(lParam));
				SetCursor (LoadCursor(NULL, IDC_ARROW)) ;
				ReleaseCapture();
			}
			return 0;

		case WM_MOUSEWHEEL:
			long zDelta;
			zDelta = HIWORD(wParam);
			if(zDelta == 120)
			{
				if(m_cNavigueData->GetCBitmap()->GetShrinkState())
					SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_IMAGES_PRCDENTE,0),0);
				else
					SendMessage(hWnd,WM_KEYDOWN,VK_UP,lParam);
			}
			else
			{
				if(m_cNavigueData->GetCBitmap()->GetShrinkState())
					SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_IMAGES_SUIVANTE,0),0);
				else
					SendMessage(hWnd,WM_KEYDOWN,VK_DOWN,lParam);
			}
			return 0;

		case WM_MOUSEMOVE:
			{
				if(m_cNavigueData->GetCBitmap()->GetMouseBlock() && !m_cNavigueData->GetCBitmap()->GetShrinkState())
				{
					

					PositionnementImage(hWnd, lParam, m_cNavigueData->GetCBitmap(), m_cNavigueData->GetCIBitmap());
					if(m_cNavigueData->GetCBitmap()->GetFullscreenState())
						m_cNavigueData->GetCBitmap()->AffichageBitmap(hWnd, m_cNavigueData->GetCIBitmap(), m_cNavigueData->GetCBitmap()->GetPosLargeur(), m_cNavigueData->GetCBitmap()->GetPosHauteur());
				}
			}
			return 0;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void CWndBitmap::PositionnementImage(HWND &hwnd, DWORD lParam,CBitmap * m_WndBitmap, CIBitmap * m_ciBitmap)
{

	
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);

	RECT rc;
	GetClientRect(hwnd,&rc);

#ifdef _DEBUG
	char Pos[50];
	StringCchPrintf(Pos,50,"Position en x : %d en y %d \n",x,y);
	OutputDebugString(Pos);
#endif

	if(HIWORD(lParam) > 0 && HIWORD(lParam) < rc.bottom && LOWORD(lParam) > 0 && LOWORD(lParam) < rc.right)
	{

		m_WndBitmap->PositionnementImage(lParam, rc.right, rc.bottom, *m_ciBitmap);

		SendMessage(hwnd,WM_HSCROLL,MAKEWPARAM(SB_THUMBPOSITION,m_WndBitmap->GetPosLargeur()),lParam);

		SendMessage(hwnd,WM_VSCROLL,MAKEWPARAM(SB_THUMBPOSITION,m_WndBitmap->GetPosHauteur()),lParam);
	}

}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void CWndBitmap::DeplacementCurseur(HWND &hWnd,RECT &rc, const long &m_lDeplacement, const bool &m_bType, LPARAM lParam, CBitmap * m_WndBitmap, CIBitmap * m_ciBitmap)
{
	
	if(m_bType)
	{
		m_WndBitmap->SetDeplacement(m_lDeplacement, m_bType, rc.bottom, m_ciBitmap->GetBitmapHeigth() * m_WndBitmap->GetRatio());
		if(m_WndBitmap->GetFullscreenState())
			m_WndBitmap->AffichageBitmap(hWnd, m_ciBitmap, m_WndBitmap->GetPosLargeur(), m_WndBitmap->GetPosHauteur());
		else
			SendMessage(hWnd,WM_VSCROLL,MAKEWPARAM(SB_THUMBPOSITION,m_WndBitmap->GetPosHauteur()),lParam);

	}
	else
	{
		m_WndBitmap->SetDeplacement(m_lDeplacement, m_bType, rc.right, m_ciBitmap->GetBitmapWidth() * m_WndBitmap->GetRatio());
		if(m_WndBitmap->GetFullscreenState())
			m_WndBitmap->AffichageBitmap(hWnd, m_ciBitmap, m_WndBitmap->GetPosLargeur(), m_WndBitmap->GetPosHauteur());
		else
			SendMessage(hWnd,WM_HSCROLL,MAKEWPARAM(SB_THUMBPOSITION,m_WndBitmap->GetPosLargeur()),lParam);

	}
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void CWndBitmap::ZoomIn(HWND hWnd,LPARAM lParam)
{
	POINT pt;
	pt.y = HIWORD (lParam); 
	pt.x = LOWORD (lParam); 

	//Deplacement de l'image vers la zone de cliquage

	//Calcul du point de clique sur l'image d'origine :

	float fRatio = m_cNavigueData->GetCBitmap()->GetRatio();
	int iXImage = (pt.x + m_cNavigueData->GetCBitmap()->GetPosLargeur()) / fRatio;
	int iYImage = (pt.y + m_cNavigueData->GetCBitmap()->GetPosHauteur()) / fRatio;

	//Calcul du point de zoom
	m_cNavigueData->GetCBitmap()->GetRatioSup();

	ApplyZoom(hWnd,iXImage,iYImage);

}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void CWndBitmap::ZoomOut(HWND hWnd,LPARAM lParam)
{
	POINT pt;
	pt.y = HIWORD (lParam); 
	pt.x = LOWORD (lParam);

	float fRatio = m_cNavigueData->GetCBitmap()->GetRatio();
	int iXImage = (pt.x + m_cNavigueData->GetCBitmap()->GetPosLargeur()) / fRatio;
	int iYImage = (pt.y + m_cNavigueData->GetCBitmap()->GetPosHauteur()) / fRatio;

	//Calcul du point de zoom
	m_cNavigueData->GetCBitmap()->GetRatioInf();

	ApplyZoom(hWnd,iXImage,iYImage);


}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void CWndBitmap::ApplyZoom(HWND hWnd,int iXImage, int iYImage)
{

	float fRatio;

	fRatio = m_cNavigueData->GetCBitmap()->GetRatio();

	//Calcul du rectangle de visualisation sur l'image originale
	
	RECT rc;
	GetClientRect(hWnd,&rc);

	int iWidthVisu = (rc.right) / fRatio;
	int iHeightVisu = (rc.bottom) / fRatio;

	//Calcul des points d'affichage du rectangle de visualisation
	int iMaxY = m_cNavigueData->GetCIBitmap()->GetBitmapHeigth() - iHeightVisu;
	int iMaxX = m_cNavigueData->GetCIBitmap()->GetBitmapWidth() - iWidthVisu;

	if(iMaxY < 0)
		iMaxY = 0;

	if(iMaxX < 0)
		iMaxX = 0;


	int iXImageVisu = iXImage - (iWidthVisu / 2);
	int iYImageVisu = iYImage - (iHeightVisu / 2);

	//Test Max et Min

	if(iXImageVisu < 0)
		iXImageVisu = 0;

	if(iYImageVisu < 0)
		iYImageVisu = 0;

	if(iXImageVisu > iMaxX)
		iXImageVisu = iMaxX;

	if(iYImageVisu > iMaxY)
		iYImageVisu = iMaxY;

	//Calcul des points de visu avec le ratio

	iXImageVisu *= fRatio;
	iYImageVisu *= fRatio;

	//Taille du rectangle
	m_cNavigueData->GetCBitmap()->SetPosition(iXImageVisu,iYImageVisu);
	m_cNavigueData->GetCBitmap()->SetShrinkImage(false);
	SendMessage(hWnd,WM_ERASEBKGND,MAKEWPARAM(m_cNavigueData->GetCIBitmap()->GetBitmapWidth() * fRatio, m_cNavigueData->GetCIBitmap()->GetBitmapHeigth() * fRatio),MAKELPARAM(IDM_ZOOM_OK,IDM_ZOOM_OK));

}
