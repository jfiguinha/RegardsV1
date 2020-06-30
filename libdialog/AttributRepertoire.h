

#ifndef __ATTRIBUTREPERTOIRE__H__
#define __ATTRIBUTREPERTOIRE__H__

#include ".\attributfichier.h"

class CAttributRepertoire : public CAttributFichier
{
public:
	CAttributRepertoire(HINSTANCE hInstance);
	~CAttributRepertoire(void);

private:

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif