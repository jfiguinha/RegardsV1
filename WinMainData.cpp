#include "stdafx.h"
#include ".\winmaindata.h"
#include "treewindow.h"
#include "listwindow.h"
#include "Toolbar.h"
#include "statusbar.h"
#include "AffichageBitmap.h"
#include "TraitementBitmap.h"
#include "objet.h"
#include "libWindow\splitter.h"
#include "libWindow\pane.h"
#include "wndintro.h"
#include "parameter.h"
#include ".\registryaccess.h"


CWinMainData::CWinMainData(void)
{

	m_cPane = NULL;
	g_pcm2 = NULL;
	m_bShowIntro = 1;
	m_cToolbar  = NULL;
	
	m_StatusBar = NULL;
	m_cSplitter = NULL;
	m_TreeWindow = NULL;
	m_ListWindow = NULL;
	m_cPane = NULL;
	hWndIntro = NULL;
	m_iNbTraitementInstance = 0;

	m_TreeWindow = new CTreeWindow();
	m_cToolbar = new CToolbar();
	m_ListWindow = new CListWindow();
	m_StatusBar = new CStatusBar();
	m_cParameter = new CParameter();
	
	m_cSplitter = new CWndSplitter();
	m_cPane = new CPane();

	m_iPreviewIcone = IDM_LARGEICON;
	m_bCreateWndTraitement = false;
	m_bCreateWndCatalogue = false;
	m_iAffichageAlbum = 0;
	m_bCreateScreen = false;
	m_iDragging = 0;
	m_iDraggingTree = 0;

	m_cDirectory[0] = '\0';
	m_cFile[0] = '\0';
	m_cTemp[0] = '\0';
	m_szFileNameMain[0] = '\0';

}

CWinMainData::~CWinMainData(void)
{
	if(m_cToolbar != NULL)
		delete m_cToolbar;

	m_cToolbar = NULL;

	if(m_StatusBar != NULL)
		delete m_StatusBar;

	m_StatusBar = NULL;

	if(m_cSplitter != NULL)
		delete m_cSplitter;

	m_cSplitter = NULL;

	if(m_TreeWindow != NULL)
		delete m_TreeWindow;

	m_TreeWindow = NULL;

	if(m_ListWindow != NULL)
		delete m_ListWindow;

	m_ListWindow = NULL;

	if(m_cPane != NULL)
		delete m_cPane;

	m_cPane = NULL;

	if(m_cParameter != NULL)
		delete m_cParameter;

	m_cParameter = NULL;

}

int CWinMainData::GetOrderBy()
{
	return m_TreeWindow->GetOrderBy();
}

HWND CWinMainData::GetTreeWindow()
{
	return m_TreeWindow->m_hWnd;
}

HWND CWinMainData::GetListWindow()
{
	return m_ListWindow->m_hWnd;
}

void CWinMainData::AddTraitementInstance(CTraitementInstance &traitementInstance)
{
	m_TraitementIntanceVector.push_back(traitementInstance);
}

void CWinMainData::AddTraitementWindow(HWND hWnd)
{
	TraitementInstanceVector::iterator fit;
	fit = m_TraitementIntanceVector.begin();
	fit += m_iNbTraitementInstance;
	fit->hWnd = hWnd;
	m_iNbTraitementInstance++;
}

void CWinMainData::RemoveTraitementWindow(HWND hWnd)
{
	TraitementInstanceVector::iterator fit;
	HWND hWndLocal = hWnd;

	for (fit = m_TraitementIntanceVector.begin(); fit!=m_TraitementIntanceVector.end(); fit++)
	{
		if((*fit).hWnd == hWndLocal)
		{
			if((*fit).hThreadTraitement != NULL)
			{
				CloseHandle((*fit).hThreadTraitement);
				m_iNbTraitementInstance--;
			}
			m_TraitementIntanceVector.erase(fit);
			break;
		}
	}
}

void CWinMainData::DestroyAllWindow()
{
	SaveWindowPosition();
	DestroyWindow(hWndStatusWindow);
	DestroyWindow(hWndPrgBar);
	DestroyWindow(hWndToolbar);
	DestroyWindow(m_cSplitter->m_hWnd);
	DestroyWindow(GetTreeWindow());
	DestroyWindow(GetListWindow());
	DestroyWindow(m_cPane->m_hWnd);
	DestroyAllTraitementWindow();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

void CWinMainData::DestroyAllTraitementWindow()
{
	TraitementInstanceVector::const_iterator fit;
	
	for (fit = m_TraitementIntanceVector.begin(); fit!= m_TraitementIntanceVector.end(); fit++)
	{
		DestroyWindow((*fit).hWnd);
		CloseHandle((*fit).hThreadTraitement);
	}

	m_TraitementIntanceVector.clear();
}

//Save Window Position
int CWinMainData::SaveWindowPosition()
{
	WINDOWINFO _windowInfo;
	int _iListSize = 0;
	int _iTreeSize = 0;
	int _iPanelSize = 0;

	

	GetWindowInfo(m_ListWindow->m_hWnd,&_windowInfo);
	_iListSize = _windowInfo.rcClient.right - _windowInfo.rcClient.left;
	GetWindowInfo(m_TreeWindow->m_hWnd,&_windowInfo);
	_iTreeSize = _windowInfo.rcClient.right - _windowInfo.rcClient.left;
	GetWindowInfo(m_cPane->m_hWnd,&_windowInfo);
	_iPanelSize = _windowInfo.rcClient.right - _windowInfo.rcClient.left;
	GetWindowInfo(m_cSplitter->m_hWnd,&_windowInfo);

	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->WriteOrCreateKeyValue("SOFTWARE\\Regards\\exploreur") == ERROR_SUCCESS)
	{
		if(::IsWindowVisible(m_ListWindow->m_hWnd))
			m_cRegistryAccess->WriteIntValue(_iListSize,"ListWindowSize");
		if(::IsWindowVisible(m_TreeWindow->m_hWnd))
			m_cRegistryAccess->WriteIntValue(_iTreeSize,"TreeWindowSize");
		if(::IsWindowVisible(m_cPane->m_hWnd))
			m_cRegistryAccess->WriteIntValue(_iPanelSize,"PanelSize");
		
	}
	delete m_cRegistryAccess;

	return 0;
}

int CWinMainData::LoadWindowPosition(int &iListSize, int &iTreeSize, int &iPanelSize)
{
	WINDOWINFO _windowInfo;
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->OpenKeyValue("SOFTWARE\\Regards\\exploreur") == ERROR_SUCCESS)
	{
		iListSize = m_cRegistryAccess->ReadIntValue("ListWindowSize");
		if(iListSize == -1)
		{
			delete m_cRegistryAccess;
			return -1;
		}

		iTreeSize = m_cRegistryAccess->ReadIntValue("TreeWindowSize");
		iPanelSize = m_cRegistryAccess->ReadIntValue("PanelSize");

	}
	else
	{
		delete m_cRegistryAccess;
		return -1;
	}
	delete m_cRegistryAccess;

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

int CWinMainData::CreateListWindow(HWND hWnd)
{
	int iListSize = 0;
	int iTreeSize = 0;
	int iPanelSize = 0;
	
	if(LoadWindowPosition(iListSize,iTreeSize,iPanelSize) == -1)
		iListSize = CW_USEDEFAULT;

	DWORD dwStyle = CS_HREDRAW | CS_VREDRAW;
	DWORD dwWndStyle = WS_CHILD | WS_VISIBLE;
	DWORD dwExStyle = WS_EX_CLIENTEDGE;
	RECT rc = {CW_USEDEFAULT, 0, iListSize, 0};
	m_ListWindow->CreateEx(hWnd,hInstance,dwWndStyle,dwStyle,dwExStyle,"ListWindow",0,0,IDC_ARROW,rc);

	//SetWindowTheme(m_ListWindow->m_hWnd, L"explorer", NULL);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

int CWinMainData::CreateTreeWindow(HWND hWnd)
{
	int iListSize = 0;
	int iTreeSize = 0;
	int iPanelSize = 0;
	if(LoadWindowPosition(iListSize,iTreeSize,iPanelSize) == -1)
		iTreeSize = CW_USEDEFAULT;

	DWORD dwStyle = CS_HREDRAW | CS_VREDRAW;
	DWORD dwWndStyle = WS_CHILD | WS_VISIBLE;
	DWORD dwExStyle = 0;
	RECT rc = {CW_USEDEFAULT, 0, iTreeSize, 0};
	GetClientRect(::GetParent(hWnd), &rc);
	m_TreeWindow->CreateEx(hWnd,hInstance,dwWndStyle,dwStyle,dwExStyle,"TreeWindow",0,0,IDC_ARROW,rc);

	//SetWindowTheme(m_TreeWindow->m_hWnd, L"explorer", NULL);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

int CWinMainData::CreatePaneWindow(HWND hWnd)
{
	int iListSize = 0;
	int iTreeSize = 0;
	int iPanelSize = 0;

	if(LoadWindowPosition(iListSize,iTreeSize,iPanelSize) == -1)
		iPanelSize = 0;

	DWORD dwStyle = CS_HREDRAW | CS_VREDRAW;
	DWORD dwWndStyle = WS_CHILD | WS_VISIBLE;
	DWORD dwExStyle = 0;
	RECT rc = {0, 0, iPanelSize, 0};
	m_cPane->CreateEx(hWnd,hInstance,dwWndStyle,dwStyle,dwExStyle,"PaneWindow",0,0,IDC_ARROW,rc);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

int CWinMainData::CreateSplitter(HWND hWnd)
{
	RECT rc = {CW_USEDEFAULT, 0, CW_USEDEFAULT, 0};
	

	int iBottom = rc.bottom;
	::GetClientRect(hWndStatusWindow,&rc);
	iBottom -= rc.bottom;
	::GetClientRect(hWndToolbar,&rc);
	iBottom -= rc.bottom;
	int iTop = rc.bottom;

	GetClientRect(hWnd, &rc);

	rc.top = iTop;
	rc.bottom -= iBottom;

	DWORD dwStyle = CS_HREDRAW | CS_VREDRAW;
	DWORD dwWndStyle = WS_CHILD | WS_VISIBLE;
	DWORD dwExStyle = WS_EX_CLIENTEDGE;
	
	m_cSplitter->CreateEx(hWnd,hInstance,dwWndStyle,dwStyle,dwExStyle,"Splitter",0,0,IDC_SIZEWE,rc);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

void CWinMainData::ShowIntro(const BOOL &m_bShow)
{
	m_bShowIntro = m_bShow;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////
HWND CWinMainData::GetSplitterWindow()
{
	return m_cSplitter->m_hWnd;
}

void CWinMainData::DefineQualite(HWND hwnd)
{
	switch(m_iQualite)
	{
		case 0:
			AffichageCheckMenuState(hwnd, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_NORMAL);
			break;

		case BOXFILTER:
			AffichageCheckMenuState(hwnd, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BOX);
			break;

		case BILINEARFILTER:
			AffichageCheckMenuState(hwnd, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BILINEAR);
			break;

		case GAUSSIANFILTER:
			AffichageCheckMenuState(hwnd, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_GAUSSIAN);
			break;

		case HAMMINGFILTER:
			AffichageCheckMenuState(hwnd, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_HAMMING);
			break;

		case CUBICFILTER:
			AffichageCheckMenuState(hwnd, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BICUBIC);
			break;

		case BLACKMANFILTER:
			AffichageCheckMenuState(hwnd, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BLACKMAN);
			break;

		case QUADRATICFILTER:
			AffichageCheckMenuState(hwnd, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_QUADRATIC);
			break;

		case MITCHELLFILTER:
			AffichageCheckMenuState(hwnd, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_MITCHELL);
			break;

		case TRIANGLEFILTER:
			AffichageCheckMenuState(hwnd, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_TRIANGLE);
			break;

		case SINCFILTER:
			AffichageCheckMenuState(hwnd, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_SINC);
			break;

		case BESSELFILTER:
			AffichageCheckMenuState(hwnd, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BESSEL);
			break;

		case BLACKMANBESSELFILTER:
			AffichageCheckMenuState(hwnd, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BLACKMANBESSEL);
			break;

		case BLACKMANSINCFILTER:
			AffichageCheckMenuState(hwnd, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BLACKMANSINC);
			break;

		case LANCZOSFILTER:
			AffichageCheckMenuState(hwnd, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_LANCZOS);
			break;

		case HERMITEFILTER:
			AffichageCheckMenuState(hwnd, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_HERMITE);
			break;

		case HANNINGFILTER:
			AffichageCheckMenuState(hwnd, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_HANNING);
			break;

		case CATROMFILTER:
			AffichageCheckMenuState(hwnd, ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_CATROM);							
			break;
	}
	SendMessage(GetTreeWindow(),WM_COMMAND,MAKEWPARAM(ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT,0),m_iQualite);
}

void CWinMainData::DefineModeAffichage(HWND hwnd)
{

	switch(m_iModeAffichage)
	{
		case 1:
			GestionListeAffichage(IDM_LARGEICON, GetTreeWindow(), hwnd, MAKEWPARAM(IDM_LARGEICON,0));
			break;
		case 2:
			GestionListeAffichage(IDM_SMALLICON, GetTreeWindow(), hwnd, MAKEWPARAM(IDM_SMALLICON,0));
			break;
		case 3:
			GestionListeAffichage(IDM_LISTVIEW, GetTreeWindow(), hwnd, MAKEWPARAM(IDM_LISTVIEW,0));
			break;
		case 4:
			GestionListeAffichage(IDM_REPORTVIEW, GetTreeWindow(), hwnd, MAKEWPARAM(IDM_REPORTVIEW,0));
			break;
		case 5:
			GestionListeAffichage(IDM_THUMBMAIL, GetTreeWindow(), hwnd, MAKEWPARAM(IDM_THUMBMAIL,0));
			break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////
void CWinMainData::CreateContextMenu(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(g_pcm2 != NULL)
	{
		g_pcm2->HandleMenuMsg(msg, wParam, lParam);
		g_pcm2 = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

void CWinMainData::ShowPane()
{
	if(!m_cPane->IsWindowVisible())
		m_cPane->ShowWindow(SW_SHOW);
}

void CWinMainData::DeplaceStatusBar(HWND hwnd)
{
	m_StatusBar->DeplaceStatusBar(hwnd);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

void CWinMainData::SetIntroWindow(HWND hWnd)
{
	hWndIntro = hWnd;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////


void CWinMainData::AffichageDossierRecherche(HWND &hwnd, const bool &m_bAffichageDossier, const bool &m_bAffichageRecherche)
{
	m_cPane->ShowWindow(SW_SHOW);
	SendMessage(m_ListWindow->m_hWnd, WM_COMMAND, MAKEWPARAM(ID_AFFICHAGE_CONTROLE_RECHERCHE, m_bAffichageRecherche),0);
	SendMessage(m_ListWindow->m_hWnd, WM_COMMAND, MAKEWPARAM(ID_AFFICHAGE_CONTROL_FOLDERTREE, m_bAffichageDossier),0);
	CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_CONTROLE_RECHERCHE,MF_CHECKED);
	CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_CONTROL_FOLDERTREE,MF_UNCHECKED);
	m_cSplitter->RedrawSplitter();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

void CWinMainData::AffichageDossierRepertoire(HWND &hwnd, const bool &m_bAffichageDossier, const bool &m_bAffichageRecherche)
{
	m_cPane->ShowWindow(SW_SHOW);
	SendMessage(m_ListWindow->m_hWnd, WM_COMMAND, MAKEWPARAM(ID_AFFICHAGE_CONTROLE_RECHERCHE, m_bAffichageRecherche),0);
	SendMessage(m_ListWindow->m_hWnd, WM_COMMAND, MAKEWPARAM(ID_AFFICHAGE_CONTROL_FOLDERTREE, m_bAffichageDossier),0);
	CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_CONTROL_FOLDERTREE,MF_CHECKED);
	CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_CONTROLE_RECHERCHE,MF_UNCHECKED);
	m_cSplitter->RedrawSplitter();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

void CWinMainData::AffichageMenuDossierRecherche(HWND &hwnd, const int &iAffichageToCheck, const int &iAffichageToUncheck)
{
	m_cPane->ShowWindow(SW_SHOW);

	if(CheckMenuItem(GetMenu(hwnd),iAffichageToCheck,MF_BYCOMMAND) == MF_UNCHECKED)
	{
		CheckMenuItem(GetMenu(hwnd),iAffichageToCheck,MF_CHECKED);

		if(GetMenuState(GetMenu(hwnd),iAffichageToUncheck,MF_BYCOMMAND) == MF_UNCHECKED)
			m_cPane->ShowWindow(SW_SHOW);

		SendMessage(m_ListWindow->m_hWnd, WM_COMMAND, MAKEWPARAM(iAffichageToCheck, true),0);

	}
	else
	{
		CheckMenuItem(GetMenu(hwnd),iAffichageToCheck,MF_UNCHECKED);

		if(GetMenuState(GetMenu(hwnd),iAffichageToUncheck,MF_BYCOMMAND) == MF_UNCHECKED)
			m_cPane->ShowWindow(SW_HIDE);

		SendMessage(m_ListWindow->m_hWnd, WM_COMMAND, MAKEWPARAM(iAffichageToCheck, false),0);

	}

	if(CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_CONTROLE_RECHERCHE,MF_BYCOMMAND) == MF_UNCHECKED &&
		CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_CONTROL_FOLDERTREE,MF_BYCOMMAND) == MF_UNCHECKED)
		m_cPane->ShowWindow(SW_HIDE);

	m_cSplitter->RedrawSplitter();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

void CWinMainData::AffichageCheckMenuState(HWND &hwnd, const int &iAffichageToCheck)
{
	if(CheckMenuItem(GetMenu(hwnd),iAffichageToCheck,MF_UNCHECKED) == MF_UNCHECKED)
	{
		CheckMenuItem(GetMenu(hwnd),iAffichageToCheck,MF_CHECKED);
		SendMessage(m_TreeWindow->m_hWnd,WM_COMMAND,MAKEWPARAM(iAffichageToCheck,true),0);
	}
	else
	{
		CheckMenuItem(GetMenu(hwnd),iAffichageToCheck,MF_UNCHECKED);
		SendMessage(m_TreeWindow->m_hWnd,WM_COMMAND,MAKEWPARAM(iAffichageToCheck,false),0);
	}
}

void CWinMainData::HiddePane()
{
	m_cPane->ShowWindow(SW_HIDE);
	m_cSplitter->RedrawSplitter();
}

void CWinMainData::CreateWindowInstance(HWND hwnd)
{

	hWndStatusWindow = m_StatusBar->CreateStatusBarCtrl(hwnd);
	m_StatusBar->CreateMultipartStatus(hwnd,5,"150","350","650","750","1000","-1");
	m_TreeWindow->SetStatusBar(m_StatusBar);
	hWndToolbar = m_cToolbar->CreateRebar(hwnd,hInstance);

	///////////////////////////////////////////////////////////////////////////
	//Create Liste Window
	///////////////////////////////////////////////////////////////////////////

	RECT rect;
	GetWindowRect(hwnd, &rect);

	int iListSize = 0;
	int iTreeSize = 0;
	int iPanelSize = 0;

	if(LoadWindowPosition(iListSize,iTreeSize,iPanelSize) != -1)
		m_cSplitter->SetSplitPos(iListSize);
	else
		m_cSplitter->SetSplitPos(rect.right / 3);

	m_cSplitter->SetHorizontalSplit(0);

	CreateSplitter(hwnd);
	CreatePaneWindow(m_cSplitter->m_hWnd);

	CreateListWindow(m_cPane->m_hWnd);
	CreateTreeWindow(m_cSplitter->m_hWnd);

	m_cSplitter->SetSplitWindow(m_cPane->m_hWnd,GetTreeWindow());

	m_cPane->SetWindow(GetListWindow());

	m_ListWindow->SetParent(hwnd);
	m_TreeWindow->SetParent(hwnd);
	m_cPane->SetParent(hwnd);

	RECT rcClient;
	GetClientRect(hWndStatusWindow, &rcClient); 
	hWndPrgBar = CreateWindowEx(0, PROGRESS_CLASS, (LPSTR) NULL, 
		WS_CHILD, 151, 
		3, 
		198, rcClient.bottom - rcClient.top - 3, 
		hWndStatusWindow, (HMENU) 0, hInstance, NULL); 

	SendMessage(hWndPrgBar, PBM_SETRANGE, 0, MAKELPARAM(0,100)); 
	SendMessage(hWndPrgBar, PBM_SETSTEP, (WPARAM) 1, 0); 

	m_cParameter->GetOptionExploration(m_iQualite,m_bImage,m_cDirectory,_MAX_DIR,m_iModeAffichage);

	

}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////

int CWinMainData::GestionListeAffichage(const unsigned int &iAffichageNewValue,HWND hWndTreeWindow, HWND hwnd,WPARAM wParam)
{
	CheckMenuItem(GetMenu(hwnd),m_iPreviewIcone,MF_UNCHECKED);
	CheckMenuItem(GetMenu(hwnd),iAffichageNewValue,MF_CHECKED);
	m_iPreviewIcone = iAffichageNewValue;
	SendMessage(hWndTreeWindow,WM_COMMAND,wParam,0);
	return 0;
}