
#ifndef __JPEGSAVING__H__
#define __JPEGSAVING__H__

#include "../libWindow/LocalDialog.h"

class CJpegSaving : public CLocalDialog
{
public:
	CJpegSaving(void);
	~CJpegSaving(void);

	void SetQuality(long lValue)
	{
		m_lQuality = lValue;
	}

	long GetQuality()
	{
		return m_lQuality;
	}

private:

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnHScroll(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	long m_lQuality;
	TCHAR m_szQuality[10];
};

#endif
