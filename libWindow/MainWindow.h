// MainWindow.h: interface for the CMainWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINWINDOW_H__A8B37A00_7E82_4C35_98EB_D9BE3809CE13__INCLUDED_)
#define AFX_MAINWINDOW_H__A8B37A00_7E82_4C35_98EB_D9BE3809CE13__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include "LocalWindow.h"

class CMainWindow : public CLocalWindow  
{

public:

	CMainWindow();
	virtual ~CMainWindow();


	int Run(int iAccelerator);

	
	LRESULT OnDestroy(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		PostQuitMessage(0);
		return 0;
	};
};

#endif // !defined(AFX_MAINWINDOW_H__A8B37A00_7E82_4C35_98EB_D9BE3809CE13__INCLUDED_)
