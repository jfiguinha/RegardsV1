

#ifndef __INFOSFICHIERPROC__H__
#define __INFOSFICHIERPROC__H__

#include "CreateDialog.h"

class CWebInfoFichier;
class CPreviewData;

class CInfosFichierProc : public CCreateDialog
{
public:
	CInfosFichierProc(void);
	~CInfosFichierProc(void);

	void SetParam(CPreviewData * pPreviewData);

private:

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	CWebInfoFichier * m_cWebInfoFichier;
	CPreviewData * m_pPreviewData;
	HBRUSH g_hbrBackground;
	HWND hFocus;
};

#endif