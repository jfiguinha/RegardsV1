

#ifndef __TRAITEMENTDATA__H__
#define __TRAITEMENTDATA__H__

class CWndAfficheBitmap;
class CStatusBar;
#ifdef COOLMENU
class CCoolMenu;
#endif
class CToolbar;
class CIBitmap;

class CTraitementData
{
public:
	CTraitementData(void);
	~CTraitementData(void);

	char cDirectory[MAX_PATH];
	char cFile[MAX_PATH];
	CWndAfficheBitmap * m_CWndAfficheBitmap;
	CStatusBar * m_StatusBar;
#ifdef COOLMENU
	CCoolMenu * m_CCoolMenu;
#endif
	CToolbar * m_CToolbar;
	CIBitmap * m_ciBitmap;
	HWND hWndToolbar;
	//HWND hwndTraitementBitmap;
	HWND hParent;
	HWND hWndPrgBar;
	HWND hWndStatusWindow;
	HWND HwndBitmap;
	bool m_bFullScreen;
	bool m_bBlocking;
};

#endif