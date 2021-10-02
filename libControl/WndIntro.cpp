// WndIntro.cpp: implementation of the CWndIntro class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "WndIntro.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWndIntro::CWndIntro()
{
	buffer[0] = '\0';
	hWndIntro = NULL;
	char m_szMessage[255];
	LoadString(hInstance,IDS_STGINITENCOURS,m_szMessage,255);
	StringCchCopy(buffer,256,m_szMessage);
	iNbPoint = 0;
}

CWndIntro::~CWndIntro()
{
}

LRESULT CWndIntro::OnTimer(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
		case TIMER_INTRO:
			{
				iNbPoint++;
				iNbPoint = iNbPoint % 5;
				char m_szMessage[255];
				LoadString(hInstance,IDS_STGINITENCOURS,m_szMessage,255);
				StringCchCopy(buffer,256,m_szMessage);
				for(int i = 0;i < iNbPoint;i++)
				{
					StringCbCat(buffer,256,".");
				}
				Paint(hWnd);
			}
			break;
	}
	return 0;
}

LRESULT CWndIntro::OnCreate(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//SetTimer(hWnd,IDM_SETTEXT,100,NULL)
	Paint(hWnd);
	return 0;

}

LRESULT CWndIntro::OnShowWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(wParam == false)
		KillTimer(hWnd,IDM_SETTEXT);
	else
		SetTimer(hWnd,IDM_SETTEXT,100,NULL);

	Paint(hWnd);
	return 0;

}

LRESULT CWndIntro::OnCommand(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{
		case IDM_SETTEXT:
			StringCchCopy(buffer,256,(char*)lParam);
			Paint(hWnd);
			break;

	}

	return 0;

}

void CWndIntro::Paint(HWND hWnd)
{
	RECT rc;
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint( hWnd, &ps );
	GetClientRect(hWnd,&rc);

	HDC sourceDC;
	sourceDC = CreateCompatibleDC(hDC);

	SetBkMode(hDC, TRANSPARENT);

	HBITMAP hBitmap = LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_DLGINTRO));

	HBITMAP hbmOldSource = (HBITMAP)::SelectObject( sourceDC, hBitmap);

	BitBlt(hDC,0,0, rc.right,rc.bottom, sourceDC, 0, 0,SRCCOPY);

	TextOut(hDC, 10, 75, buffer, strlen(buffer)); 

	EndPaint( hWnd, &ps );
	ReleaseDC(hWnd,hDC);

	// Restore DCs
	SelectObject( sourceDC, hbmOldSource );
	DeleteObject(hbmOldSource);
	DeleteObject(hBitmap);
	DeleteDC(sourceDC);
}

LRESULT CWndIntro::OnPaint(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Paint(hWnd);
	return 0;

}

LRESULT CWndIntro::OnEraseBackground(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Paint(hWnd);
	return 0;

}