
#ifndef __DIALOGFINDPROC__H__
#define __DIALOGFINDPROC__H__

#include "LocalDialog.h"

class CTreeWindowData;

class CDialogFindProc : public CLocalDialog
{
public:
	CDialogFindProc(void);
	~CDialogFindProc(void);

	void SetParam(CTreeWindowData * pTreeData);

private:

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnGetDlgCode(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	int iOldYPos;
	int iNumItem;
	int iPos;
	CTreeWindowData * m_pTreeData;

};

#endif
