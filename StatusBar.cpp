// StatusBar.cpp: implementation of the CStatusBar class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "StatusBar.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStatusBar::CStatusBar()
{
	m_uiStatusBar = 1;
	hWndStatusbar = NULL;
}

CStatusBar::~CStatusBar()
{

}

void CStatusBar::UpdateStatusBar(LPSTR lpszStatusString, WORD partNumber, WORD displayFlags)
{
    SendMessage(hWndStatusbar,
                SB_SETTEXT,
                partNumber | displayFlags,
                (LPARAM)lpszStatusString);
}

void CStatusBar::CreateMultipartStatus(HWND &hParent, const int &iNbPart,const char * cFirst,...)
{
    RECT rcClient; 
    HLOCAL hloc; 
	LPINT lpParts; 

	int j = 0;
  

	// Get the coordinates of the parent window's client area. 
	GetClientRect(hParent, &rcClient); 

	// Allocate an array for holding the right edge coordinates. 
	hloc = LocalAlloc(LHND, sizeof(int) * iNbPart); 
	lpParts = (LPINT)LocalLock(hloc); 


	lpParts[0] = atoi(cFirst);
	j++;

	va_list marker;
	va_start(marker, cFirst);     /* Initialize variable arguments. */
	while(j < iNbPart)
	{
		//char * Temp;
		//Temp = va_arg( marker, char *);
		lpParts[j] = atoi(va_arg( marker, char *));
		j++;

	}
	va_end( marker ); 

	/*
	lpParts[0] = 150;
	lpParts[1] = 500;
	lpParts[2] = 800;
	lpParts[3] = 900;
	lpParts[4] = 1000;*/

	// Calculate the right edge coordinate for each part, and 
	// copy the coordinates to the array. 
	/*
	nWidth = rcClient.right / iNbPart; 
	for (i = 0; i < iNbPart; i++) { 
		lpParts[i] = nWidth; 
		nWidth += nWidth; 
	} */

	// Tell the status bar to create the window parts. 
	SendMessage(hWndStatusbar, SB_SETPARTS, (WPARAM) iNbPart, 
		(LPARAM) lpParts); 

	// Free the array, and return. 
	LocalUnlock(hloc); 
	LocalFree(hloc); 
}

HWND CStatusBar::CreateStatusBarCtrl(HWND hParent)
{
	hWndStatusbar = CreateStatusWindow(WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, "Infos", hParent, m_uiStatusBar);
	return hWndStatusbar;
}

void CStatusBar::DeplaceStatusBar(HWND hParent)
{
	RECT rc;
	GetWindowRect(hParent,&rc);

	MoveWindow(hWndStatusbar,0,rc.bottom - 20,rc.right,rc.bottom,TRUE);
}