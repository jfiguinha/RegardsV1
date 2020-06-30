
#ifndef __GESTIONCATEGORIE__H__
#define __GESTIONCATEGORIE__H__

#include "../libWindow/LocalDialog.h"

class CDialogCatalogueData;
class CAlbumCritereNotify;

class CGestionCategorie : public CLocalDialog
{
public:
	CGestionCategorie(HINSTANCE hInstance);
	~CGestionCategorie(void);

	void SetParam(CDialogCatalogueData * pDialogData)
	{
		m_lpDialogData = pDialogData;
	}

	bool GetReturnValue()
	{
		return m_bReturnValue;
	}

private:

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnNotify(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	CDialogCatalogueData * m_lpDialogData;
	CAlbumCritereNotify * m_cAlbumCritereNotify;
	HINSTANCE hInstanceLocal;
	int m_iNumItem;
	int m_iSelectItem;
	int m_iNumLast;
	bool m_bReturnValue;
};

#endif