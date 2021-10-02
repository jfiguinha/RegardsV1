// Toolbar.h: interface for the CToolbar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLBAR_H__893EE98C_B32F_40AA_9A97_B3057465CF42__INCLUDED_)
#define AFX_TOOLBAR_H__893EE98C_B32F_40AA_9A97_B3057465CF42__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include "objet.h"

#define MAX_LEN 255
#define NUM_BUTTONS 3

class CToolbar
{
public:
	CToolbar();
	virtual ~CToolbar();
	HWND CreateToolBarMain(HWND hWndParent, HINSTANCE hInst);
	HWND CreateToolBarPicture(HWND hWndParent, HINSTANCE hInst);
	HWND CreateToolBarTraitement(HWND hWndParent, HINSTANCE hInst);
	HWND CreateToolBarCatalogue(HWND hWndParent, HINSTANCE hInst);
	HWND CreateToolBarWinzip(HWND hWndParent, HINSTANCE hInst);
	HWND CreateMenuToolbar(HWND hWndParent, HINSTANCE hInst, int idRessource);

	HWND CreateRebar(HWND hwndOwner, HINSTANCE g_hinst, const int &iNumToolbar = 1);

private:
	HIMAGELIST himlHot;

};

#endif // !defined(AFX_TOOLBAR_H__893EE98C_B32F_40AA_9A97_B3057465CF42__INCLUDED_)
