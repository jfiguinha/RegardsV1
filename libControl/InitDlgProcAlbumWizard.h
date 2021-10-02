

#ifndef __CINITDLGPROCALBUMWIZARD__H__
#define __CINITDLGPROCALBUMWIZARD__H__

class CAlbumWizard;

class CInitDlgProcAlbumWizard
{
public:
	CInitDlgProcAlbumWizard(void);
	~CInitDlgProcAlbumWizard(void);

	void SetParam(CAlbumWizard * cAlbumWizard)
	{
		m_cAlbumWizard = cAlbumWizard;
	}

	LRESULT OnInitDialog(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnNotify(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:

	CAlbumWizard * m_cAlbumWizard;
};

#endif