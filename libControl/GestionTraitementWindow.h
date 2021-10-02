#ifndef __GESTIONTRAITEMENTWINDOW__H__
#define __GESTIONTRAITEMENTWINDOW__H__

#include <TraitementInstance.h>

//---------------------------------------------------------
//Classe de Gestion des fenêtres de traitement
//---------------------------------------------------------
class CGestionTraitementWindow
{
public:

	//---------------------------------------
	//Constructeur
	//---------------------------------------
	CGestionTraitementWindow()
	{
		m_iNbTraitementInstance = 0;
		hwndAffichageBitmap = NULL;
		m_cFileName[0] = '\0';
	}

	//---------------------------------------
	//destructeur 
	//---------------------------------------
	~CGestionTraitementWindow()
	{
		DestroyAllTraitementWindow();
		m_TraitementIntanceVector.clear();
	}

	//---------------------------------------
	//Constructeur par copie
	//---------------------------------------
	CGestionTraitementWindow(const CGestionTraitementWindow &gestionTraitementWindow)
	{
		m_TraitementIntanceVector = gestionTraitementWindow.m_TraitementIntanceVector;
		hwndAffichageBitmap = gestionTraitementWindow.hwndAffichageBitmap;
		m_iNbTraitementInstance = gestionTraitementWindow.m_iNbTraitementInstance;
		strcpy_s(m_cFileName,MAX_PATH,gestionTraitementWindow.m_cFileName);
	}

	void DeleteSelectTraitementWindow(HWND hWndLocal);
	void DestroyAllTraitementWindow();
	void SetTraitementWindow(HWND hWnd);
	void CreateTraitementWindow(const char * filename,HWND hWndParent);

private:


	static ULONG __stdcall TraitementBMP(LPVOID parametre);

	//Variable
	TraitementInstanceVector m_TraitementIntanceVector;
	HWND hwndAffichageBitmap;
	int m_iNbTraitementInstance;
	char m_cFileName[MAX_PATH];
};

#endif