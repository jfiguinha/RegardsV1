

#ifndef __CPAGECRITEREALBUMDLG__H__
#define __CPAGECRITEREALBUMDLG__H__

class CAlbumCritereNotify;
class CAlbumWizard;

class CPageCritereAlbumDlg
{
public:
	CPageCritereAlbumDlg(HINSTANCE hInstance);
	~CPageCritereAlbumDlg(void);

	void SetParam(CAlbumWizard * cAlbumWizard)
	{
		m_cAlbumWizard = cAlbumWizard;
	}

	LRESULT OnInitDialog(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnNotify(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:

	int iNumItem;
	int iSelectItem;
	HINSTANCE hInstance;
	CAlbumCritereNotify * m_cAlbumCritereNotify;
	CAlbumWizard * m_cAlbumWizard;
};

#endif
