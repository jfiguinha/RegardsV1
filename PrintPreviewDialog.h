
#ifndef __PRINTPREVIEWDIALOG__H__
#define __PRINTPREVIEWDIALOG__H__

#include "libWindow/LocalDialog.h"

class CPrintPreviewData;
class CPrintPrv;
class CMargeData;

class CPrintPreviewDialog : public CLocalDialog
{
public:
	CPrintPreviewDialog(void);
	~CPrintPreviewDialog(void);

	void SetParam(CMargeData * pMargeData);
	int GetReturnValue()
	{
		return m_iReturnValue;
	}

private:

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnEraseBackground(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	int m_iReturnValue;
	CPrintPreviewData * m_prtPrevData;
	char Temp[50];
	CPrintPrv * m_printPrv;
};

#endif