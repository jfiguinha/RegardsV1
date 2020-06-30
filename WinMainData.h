

#ifndef __WINMAINDATA__H__
#define __WINMAINDATA__H__

#include <TraitementInstance.h>
#include <IconeFile.h>

class CTreeWindow;
class CToolbar;
class CPane;
class CListWindow;
class CStatusBar;
#ifdef COOLMENU
class CCoolMenu;
#endif
class CWndSplitter;
class CParameter;

class CWinMainData
{
public:
	CWinMainData(void);
	~CWinMainData(void);

	void DestroyAllTraitementWindow();
	void ShowIntro(const BOOL &m_bShow);
	void DefineModeAffichage(HWND hwnd);
	void DeplaceStatusBar(HWND hwnd);
	void CreateContextMenu(UINT msg, WPARAM wParam, LPARAM lParam);
	void ShowPane();
	void DefineQualite(HWND hwnd);
	void AddTraitementWindow(HWND hWnd);
	void RemoveTraitementWindow(HWND hWnd);
	void AddTraitementInstance(CTraitementInstance &traitementInstance);
	void AffichageDossierRecherche(HWND &hwnd, const bool &m_bAffichageDossier, const bool &m_bAffichageRecherche);
	void AffichageDossierRepertoire(HWND &hwnd, const bool &m_bAffichageDossier, const bool &m_bAffichageRecherche);
	int GestionListeAffichage(const unsigned int &iAffichageNewValue,HWND hWndTreeWindow, HWND hwnd,WPARAM wParam);
	void AffichageMenuDossierRecherche(HWND &hwnd, const int &iAffichageToCheck, const int &iAffichageToUncheck);
	void AffichageCheckMenuState(HWND &hwnd, const int &iAffichageToCheck);
	void CreateWindowInstance(HWND hwnd);
	void HiddePane();



	const char * GetFileName()
	{
		return m_szFileNameMain;
	}

	void SetFile(const char * cValue)
	{
		StringCchCopy(m_cFile,_MAX_FNAME,cValue);
	}

	void SetDirectory(const char * cValue)
	{
		StringCchCopy(m_cDirectory,_MAX_DIR,cValue);
	}

	const char * GetFile()
	{
		return (const char *)m_cFile;
	}

	const char * GetDirectory()
	{
		return (const char *)m_cDirectory;
	}

	int GetOrderBy();

	void SetCreateWndTraitement(bool bValue)
	{
		m_bCreateWndTraitement = bValue;
	}

	HWND GetWindowIntro()
	{
		return hWndIntro;
	}

	HWND GetTreeWindow();
	HWND GetListWindow();

	void SetDragging(int iValue)
	{
		m_iDragging = iValue;
	}

	int GetDragging()
	{
		return m_iDragging;
	}

	void SetDraggingTree(int iValue)
	{
		m_iDraggingTree = iValue;
	}

	int GetDraggingTree()
	{
		return m_iDraggingTree;
	}

	bool GetImage()
	{
		return m_bImage;
	}

	void SetImage(bool bValue)
	{
		m_bImage = bValue;
	}

	bool GetCreateScreen()
	{
		return m_bCreateScreen;
	}

	void SetCreateScreen(bool bValue)
	{
		m_bCreateScreen = bValue;
	}

	HWND GetAffichageBitmapHandle()
	{
		return hWndAffichageBitmap;
	}

	void SetAffichageBitmapHandle(HWND hWnd)
	{
		hWndAffichageBitmap = hWnd;
	}


	HWND GetWindowProgressBarHandle()
	{
		return hWndPrgBar;
	}

	IContextMenu2 * GetContextMenu()
	{
		return g_pcm2;
	}

	void SetIconeFileVector(IconeFileVector * iconeFileVector)
	{
		m_IconeFileVector = iconeFileVector;
	}

	IconeFileVector * GetIconeFileVector()
	{
		return m_IconeFileVector;
	}


	void SetAffichageAlbum(int iValue)
	{
		m_iAffichageAlbum = iValue;
	}

	int GetAffichageAlbum()
	{
		return m_iAffichageAlbum;
	}

	void DestroyAllWindow();

	void SetModeAffichage(int iValue)
	{
		m_iModeAffichage = iValue;
	}

	void SetQualite(int iValue)
	{
		m_iQualite = iValue;
	}
	
	int GetModeAffichage()
	{
		return m_iModeAffichage;
	}

	void SetIntroWindow(HWND hWnd);

	HWND GetIntroWindow()
	{
		return hWndIntro;
	}

	int GetQualite()
	{
		return m_iQualite;
	}

	HWND GetSplitterWindow();

	HWND GetToolBarWindow()
	{
		return hWndToolbar;
	}

	HWND GetStatusWindow()
	{
		return hWndStatusWindow;
	}

	CParameter * m_cParameter;

private:

	int CreateSplitter(HWND hWnd);
	int CreateTreeWindow(HWND hWnd);
	int CreateListWindow(HWND hWnd);
	int CreatePaneWindow(HWND hWnd);

	int SaveWindowPosition();
	int LoadWindowPosition(int &iListSize, int &iTreeSize, int &iPanelSize);


	CTreeWindow * m_TreeWindow;
	CToolbar * m_cToolbar;
	CPane * m_cPane;
	CListWindow * m_ListWindow;
	CStatusBar * m_StatusBar;
	
	CWndSplitter * m_cSplitter;
	IContextMenu2  * g_pcm2;
	IconeFileVector * m_IconeFileVector;
	TraitementInstanceVector m_TraitementIntanceVector;

	HWND hwndMain;
	HWND hWndStatusWindow;
	HWND hWndAffichageBitmap;
	HWND hWndTraitementBitmap;
	HWND hWndToolbar;
	HWND hWndPrgBar;
	HWND hWndIntro;

	int m_iPreviewIcone;
	bool m_bCreateWndTraitement;
	bool m_bCreateWndCatalogue;
	int m_iAffichageAlbum;
	char m_cDirectory[_MAX_DIR];
	char m_cFile[_MAX_FNAME];
	char m_cTemp[MAX_PATH];
	char m_szFileNameMain[_MAX_FNAME];
	int m_iDragging;
	int m_iDraggingTree;
	bool m_bCreateScreen;
	//Donnée de stockage pour la base de registre
	int m_iQualite;
	bool m_bImage;
	bool m_bShowIntro;
	int m_iModeAffichage; 
	int m_iNbTraitementInstance;
};

#endif