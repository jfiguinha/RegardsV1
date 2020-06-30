// Pane.cpp: implementation of the CPane class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Pane.h"
#include "static.h"
#include "tooltip.h"
#include "../resource.h"

#define BOLD 1
#define ITALIC 2
#define ULINE 3
#define TIMER_TOOLTIP 0x05001

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPane::CPane()
{
	m_cLibelle = new CStatic();
	m_cIcone = new CStatic();
	m_cIconeOn = new CStatic();
	m_cTooltip = new CTooltip();

	m_bHover = FALSE;
	m_bTracking = FALSE;
	m_bMouseMove = FALSE;
	//Creation de la croix en HBITMAP

	WORD _dotPatternBmp[8] = 
	{ 
		0x003c, 0x0018, 0x0081, 0x00c3, 
		0x00c3, 0x0081, 0x0018, 0x003c
	};

	m_croix = CreateBitmap(8,8,1,1,_dotPatternBmp);

	RECT rc ={0,0,15,15};

	hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
	hCursorHand = LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND));

	m_croixOn = CreateCompatibleBitmap(GetDC(NULL),15,15);

	HDC memDC = CreateCompatibleDC (GetDC(NULL));

	SelectObject ( memDC, m_croixOn ); 

	FillRect(memDC, &rc, m_bkgndColor);

	//Couleur de fond 
	COLORREF m_bgColor = GetPixel(memDC,0,0);

	COLORREF m_bgLight = RGB(GetRValue(m_bgColor) + 20,GetGValue(m_bgColor) + 20,GetBValue(m_bgColor) + 20);
	COLORREF m_bgDark = RGB(GetRValue(m_bgColor) - 20,GetGValue(m_bgColor) - 20,GetBValue(m_bgColor) - 20);

	HPEN hpenLight = CreatePen(PS_SOLID, 1, m_bgLight);
	HPEN hpenDark = CreatePen(PS_SOLID, 1, m_bgDark);

	SelectObject(memDC,hpenLight);
	MoveToEx(memDC,0,0,NULL);
	LineTo(memDC,14,0);
	MoveToEx(memDC,0,0,NULL);
	LineTo(memDC,0,14);

	SelectObject(memDC,hpenDark);
	
	MoveToEx(memDC,14,0,NULL);
	LineTo(memDC,14,14);
	
	MoveToEx(memDC,0,14,NULL);
	LineTo(memDC,14,14);

	DeleteDC(memDC);
	DeleteObject(hpenLight);
	DeleteObject(hpenDark);

}

CPane::~CPane()
{
	if(m_cLibelle != NULL)
		delete m_cLibelle;

	m_cLibelle = NULL;

	if(m_cIcone != NULL)
		delete m_cIcone;

	if(m_cIconeOn != NULL)
		delete m_cIconeOn;

	if(m_cTooltip != NULL)
		delete m_cTooltip;

	DeleteObject(m_croix);
	DeleteObject(m_croixOn);

}

LRESULT CPane::OnPaint(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	RECT rc;
	HDC hDC = BeginPaint( hWnd, &ps );
	GetClientRect(hWnd,&rc);
	SendMessage(m_cLibelle->m_hWnd,uMsg,wParam,lParam);
	SendMessage(m_cIconeOn->m_hWnd,uMsg,wParam,lParam);
	SendMessage(m_cIcone->m_hWnd,uMsg,wParam,lParam);
	SendMessage(hWnd1,uMsg,wParam,lParam);
	EndPaint( hWnd, &ps );
	ReleaseDC(hWnd,hDC);
	return 0;
}

LRESULT CPane::OnCreate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char Temp[255];
	RECT rc = {0,0,0,0};

	DWORD wndExStyle = 0;
	DWORD wndEditStyle1 = WS_CHILD|WS_VISIBLE|SS_BITMAP;
	DWORD wndEditStyle2 = WS_CHILD|WS_VISIBLE;
	
	m_cLibelle->CreateEx(hWnd,wndExStyle,wndEditStyle2,rc);

	m_cIconeOn->CreateEx(hWnd,wndExStyle,wndEditStyle1,rc);
	m_cIcone->CreateEx(hWnd,wndExStyle,wndEditStyle1,rc);

	

	m_cIconeOn->ShowWindow(SW_HIDE);

	LoadString(hInstance,IDS_DOSSIER,Temp,255);
	SetTitle(Temp);



	m_cIcone->SetImage(IMAGE_BITMAP,m_croix);
	m_cIconeOn->SetImage(IMAGE_BITMAP,m_croixOn);


	m_cTooltip->CreateEx(GetParent(),0,0,rc);

	return 0;
}

LRESULT CPane::OnShowWindow(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(wParam == false)
	{
		m_cIconeOn->ShowWindow(SW_HIDE);
		m_cTooltip->TrackActive(false);
		KillTimer(m_hWnd,TIMER_TOOLTIP);
		SetCursor(hCursor);
		m_bHover=FALSE;
	}
	return 0;
}

LRESULT CPane::OnSize(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	GetClientRect(hWnd,&rc);

	rcFermer = rc;
	rcFermer.left = rc.right - 18;
	rcFermer.top = 6;
	rcFermer.right = rc.right - 3;
	rcFermer.bottom = 21;

	MoveWindow(m_cIconeOn->m_hWnd,rc.right - 18,6,15,15,TRUE);

	MoveWindow(m_cIcone->m_hWnd,rc.right - 15,9,8,8,TRUE);

	MoveWindow(m_cLibelle->m_hWnd,5,5,rc.right - 20,20,TRUE);

	MoveWindow(hWnd1,0,25,rc.right,rc.bottom - 25,TRUE);

	return 0;
}

LRESULT CPane::OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int xPos = GET_X_LPARAM(lParam); 
	int yPos = GET_Y_LPARAM(lParam); 

	if(m_bMouseMove)
		KillTimer(m_hWnd,TIMER_TOOLTIP);

	if((xPos > rcFermer.left && rcFermer.right > xPos) && 
		(yPos > rcFermer.top && rcFermer.bottom > yPos))
	{
		m_cIconeOn->ShowWindow(SW_SHOW);
		SetCursor(hCursorHand);
		if(m_bMouseMove)
			SetTimer(m_hWnd,TIMER_TOOLTIP,1000,NULL);

		m_bMouseMove = FALSE;
	}
	else
	{
		m_bMouseMove = TRUE;
		m_cIconeOn->ShowWindow(SW_HIDE);
		m_cTooltip->TrackActive(false);
		SetCursor(hCursor);
	}

	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}


	m_cTooltip->TrackPosition();
	

	return 0;
};

LRESULT CPane::OnMouseHover(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_bHover=TRUE;
	return 0;
}

LRESULT CPane::OnTimer(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
		case TIMER_TOOLTIP:
			if(m_cIconeOn->IsWindowVisible())
				m_cTooltip->TrackActive(true);

			KillTimer(m_hWnd,TIMER_TOOLTIP);
			break;
	}
	return 0;
}

LRESULT CPane::OnMouseLeave(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_cIconeOn->ShowWindow(SW_HIDE);
	m_cTooltip->TrackActive(false);

	m_bTracking = FALSE;
	m_bHover=FALSE;
	return 0;
}

LRESULT CPane::OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_cIconeOn->IsWindowVisible())
	{
		this->ShowWindow(SW_HIDE);
		m_cTooltip->ShowWindow(SW_HIDE);
		SendMessage(m_hParent,WM_NOTIFY,MAKEWPARAM(IDM_HIDDENPANE,hWnd),(LPARAM)false);
	}

	return 0;
}


void CPane::SetTitle(const char *m_szTitle)
{
	//m_cPanelDialog.SetTitle(m_szTitle);

	HFONT hFont = GetAFont(0);

	
	m_cLibelle->SetText(m_szTitle);
	m_cLibelle->SetFont(hFont,true);

}

HFONT CPane::GetAFont(int fnFont) 
{ 
	
    LOGFONT lf;  // structure for font information  
 /*
    // Get a handle to the ANSI fixed-pitch font, and copy 
    // information about the font to a LOGFONT structure. 
 
    GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), 
        &lf); 
 
    // Set the font attributes, as appropriate.  
 
    if (fnFont == BOLD) 
        lf.lfWeight = FW_BOLD; 
    else 
        lf.lfWeight = FW_NORMAL; 
 
    lf.lfItalic = (fnFont == ITALIC); 
    lf.lfItalic = (fnFont == ULINE); 

	lf.lfHeight = 10;*/

    // Create the font, and then return its handle.  

	//LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	HDC hdc = ::GetDC(NULL);
	lf.lfHeight = 14;
	//lf.lfFaceName = "Times";
	::ReleaseDC(NULL, hdc);
	lf.lfWeight = FW_NORMAL;
	return CreateFontIndirect(&lf);
} 


int CPane::SetTooltipText(const char *m_szTooltip)
{
	return m_cTooltip->SetLibelle(m_szTooltip);
}
