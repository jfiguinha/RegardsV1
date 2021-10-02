

#ifndef __ASSOCIATION__H__
#define __ASSOCIATION__H__

#include "LocalDialog.h"

class CAssociation : public CLocalDialog
{
public:
	CAssociation(HINSTANCE hInstance);
	~CAssociation(void);

private:

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	int AssociateFileExt(LPTSTR szFileExtension);
	int AssociateRegards();
	int AssociateVistaRegards();
	int AssociateFileExtVista(LPTSTR szFileExtension);

	HINSTANCE hInstance;
};

#endif