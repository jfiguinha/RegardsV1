

#ifndef __LANCERRECHERCHE__H__
#define __LANCERRECHERCHE__H__

#include "LocalDialog.h"

class CImageCalalogueRecherche;

class CLancerRechercher : public CLocalDialog
{
public:
	CLancerRechercher(HINSTANCE hInstance);
	~CLancerRechercher(void);

	void SetParam(CImageCalalogueRecherche * lpCatalogueRecherche)
	{
		m_lpCatalogueRecherche = lpCatalogueRecherche;
	}

	int GetReturnValue()
	{
		return m_iReturnValue;
	}

private:

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static ULONG __stdcall RechercherFichier(void * lParam);

	DWORD dwThreadId; 
	HANDLE hThread; 
	CImageCalalogueRecherche * m_lpCatalogueRecherche;
	int m_iReturnValue;
	HWND hWnd;
	HINSTANCE hInstanceLocal;
};

#endif