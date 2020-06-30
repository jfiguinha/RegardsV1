// PhotoDigital.h: interface for the CPhotoDigital class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHOTODIGITAL_H__E9F34A4E_CBA7_4D90_B422_DBD0D7D9397A__INCLUDED_)
#define AFX_PHOTODIGITAL_H__E9F34A4E_CBA7_4D90_B422_DBD0D7D9397A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TwainCpp.h"

class CPhotoDigital : public CTwain  
{
public:

	typedef struct tagPHOTODIGITAL
	{
		char m_szDirectory[MAX_PATH];
		char m_szFilename[255];
		int iTypeIncrementation;
		int iFormatImage;
	}PHOTODIGITALDATA, * LPPHOTODIGITALDATA;

	void SetParametre(PHOTODIGITALDATA &m_PHOTODIGITALDATA);
	HWND CreatePhotoWindow(HWND hWndParent, int nCmdShow);

	CPhotoDigital();
	virtual ~CPhotoDigital();
	void CopyImage(HWND hWnd, HANDLE hBitmap,TW_IMAGEINFO& info);
private:
	PHOTODIGITALDATA m_PHOTODIGITALDATA;	
	static int iNumImage;
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif // !defined(AFX_PHOTODIGITAL_H__E9F34A4E_CBA7_4D90_B422_DBD0D7D9397A__INCLUDED_)
