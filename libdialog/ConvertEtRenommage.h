

#ifndef __CONVERTRENAME__H__
#define __CONVERTRENAME__H__

#include "../libWindow/LocalDialog.h"

class CConvertData;

class CConvertEtRenommage : public CLocalDialog
{
public:
	CConvertEtRenommage(HINSTANCE hInstance);
	~CConvertEtRenommage(void);

private:

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void InsertImageFormat(HWND hDlg, const char * cFormat, int iNumberFormat, int iDialogItem);

	HINSTANCE m_hInstance;
	CConvertData * m_cConvertData;

};

#endif
