// PhotoDigital.h: interface for the CPhotoDigital class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHOTODIGITAL_H__E9F34A4E_CBA7_4D90_B422_DBD0D7D9397A__INCLUDED_)
#define AFX_PHOTODIGITAL_H__E9F34A4E_CBA7_4D90_B422_DBD0D7D9397A__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include "TwainCpp.h"
#include <LocalWindow.h>

class CPhotoDigital : public CTwain,public CLocalWindow
{
public:

	typedef struct tagPHOTODIGITAL
	{
		char m_szDirectory[MAX_PATH];
		char m_szFilename[MAX_PATH];
		int iTypeIncrementation;
		int iFormatImage;
	}PHOTODIGITALDATA, * LPPHOTODIGITALDATA;

	void SetParametre(PHOTODIGITALDATA &m_PHOTODIGITALDATA);
	
	//HWND CreatePhotoWindow(HWND hWndParent, int nCmdShow);

	CPhotoDigital();
	virtual ~CPhotoDigital();
	void CopyImage(HWND hWnd, HANDLE hBitmap,TW_IMAGEINFO& info);

	LRESULT OnDestroy(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	int Run();

private:

	PHOTODIGITALDATA m_PHOTODIGITALDATA;	
	int iNumImage;
	//static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	//HWND hwndMain;
};

#endif // !defined(AFX_PHOTODIGITAL_H__E9F34A4E_CBA7_4D90_B422_DBD0D7D9397A__INCLUDED_)
