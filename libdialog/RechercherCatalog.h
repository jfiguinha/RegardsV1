
#ifndef __RECHERCHERCATALOG__H__
#define __RECHERCHERCATALOG__H__

#include "LocalDialog.h"

class CCatalogueData;

class CRechercherCatalog : public CLocalDialog
{
public:
	CRechercherCatalog(HINSTANCE hInstance);
	~CRechercherCatalog(void);

	void SetParam(CCatalogueData * catalogueData)
	{
		m_CatalogueData = catalogueData;
	}

	int GetReturnValue()
	{
		return m_iReturnValue;
	}

private:
	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	UINT CreateColonneCatalog(HWND hDlg, const int &iLibelle);
	
	UINT VerifPresenceItem(HWND hDlg, const char * m_cFilename);
	UINT AddItemCritere(HWND hDlgSrc);

	UINT InsertItemCatalog(HWND hDlg, HWND hDlgComp);
	UINT InsertItemCatalog(HWND hDlgSrc, HWND hDlgDst, HWND hDlgComp, const int &iNumItem);
	UINT InsertItemCatalog(HWND hDlgSrc, HWND hDlgDst, HWND hDlgComp);

	UINT SuppAllItemCatalog(HWND hDlgDst);

	UINT AffichageCritereBouton(HWND hDlg,int * iTabCritere, const int &iPointeur);

	CCatalogueData * m_CatalogueData;
	int iPointeur;
	string m_stgChaineRechercheAff;
	string m_stgChaineRecherche;
	int * iTabCritere;
	HINSTANCE hInstance;
	int m_iReturnValue;

};

#endif