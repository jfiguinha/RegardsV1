#include "stdafx.h"
#include ".\traitementdata.h"
#include <ibitmap.h>
#include "WndAfficheBitmap.h"
#include "statusbar.h"

#include "toolbar.h"
#include "LoadImage.h"
#include "CoolMenu.h"

CTraitementData::CTraitementData(void)
{
	m_CWndAfficheBitmap = NULL;
	m_StatusBar = NULL;
#ifdef COOLMENU
	m_CCoolMenu = NULL;
#endif
	m_CToolbar = NULL;
	m_ciBitmap = NULL;

	m_CWndAfficheBitmap = new CWndAfficheBitmap();
	m_StatusBar = new CStatusBar();

#ifdef COOLMENU
	m_CCoolMenu = new CCoolMenu();	
#endif
	m_CToolbar = new CToolbar();

	cDirectory[0] = '\0';
	cFile[0] = '\0';

	hWndToolbar = NULL;
	hParent = NULL;
	hWndPrgBar = NULL;
	hWndStatusWindow = NULL;
	HwndBitmap = NULL;

	m_bFullScreen = false;
	m_bBlocking = false;
}

CTraitementData::~CTraitementData(void)
{
	if(m_CToolbar != NULL)
		delete m_CToolbar;

#ifdef COOLMENU
	if(m_CCoolMenu != NULL)
		delete m_CCoolMenu;
#endif

	if(m_CWndAfficheBitmap != NULL)
		delete m_CWndAfficheBitmap;

	m_CWndAfficheBitmap = NULL;

	if(m_StatusBar != NULL)
		delete m_StatusBar;

	if(m_ciBitmap != NULL)
		delete m_ciBitmap;

	m_CWndAfficheBitmap = NULL;
	m_StatusBar = NULL;
#ifdef COOLMENU
	m_CCoolMenu = NULL;
#endif
	m_CToolbar = NULL;
	m_ciBitmap = NULL;
}
