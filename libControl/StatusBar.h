// StatusBar.h: interface for the CStatusBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATUSBAR_H__2522289B_C0FB_4D17_914D_5C738121F24E__INCLUDED_)
#define AFX_STATUSBAR_H__2522289B_C0FB_4D17_914D_5C738121F24E__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

//#include <windows.h>

class CStatusBar  
{
public:

	HWND CreateStatusBarCtrl(HWND hParent);
	CStatusBar();
	virtual ~CStatusBar();
	void UpdateStatusBar(LPSTR lpszStatusString, WORD partNumber, WORD displayFlags);
	void DeplaceStatusBar(HWND hParent);
	void CreateMultipartStatus(HWND &hParent, const int &iNbPart,const char * cFirst,...);

private:
	HWND hWndStatusbar;
	int m_uiStatusBar;


};

#endif // !defined(AFX_STATUSBAR_H__2522289B_C0FB_4D17_914D_5C738121F24E__INCLUDED_)
