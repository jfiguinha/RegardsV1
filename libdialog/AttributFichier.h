
#ifndef __ATTRIBUTFICHIER__H__
#define __ATTRIBUTFICHIER__H__

#include "LocalDialog.h"
#include ".\attributdata.h"

class CAlbumCritereNotify;
class CDialogCatalogueData;
class CAttribut;

class CAttributFichier : public CLocalDialog, public CAttributData
{
public:

	CAttributFichier(){};

	CAttributFichier(HINSTANCE hInstance);
	~CAttributFichier(void);

	int GetReturnValue()
	{
		return m_iReturnValue;
	}


private:

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	

protected:

	LRESULT OnNotify(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	int m_iNumItem;
	int m_iReturnValue;
	CAlbumCritereNotify * m_cAlbumCritereNotify;

	
};

#endif