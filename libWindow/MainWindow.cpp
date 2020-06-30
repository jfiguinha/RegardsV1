// MainWindow.cpp: implementation of the CMainWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainWindow.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMainWindow::CMainWindow()
{

}

CMainWindow::~CMainWindow()
{

}


int CMainWindow::Run(int iAccelerator)
{
	MSG			msg;
	HACCEL hAccelTable;

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)iAccelerator);

	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}