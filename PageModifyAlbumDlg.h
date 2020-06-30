

#ifndef __CPAGEMODIFYALBUMDLG__H__
#define __CPAGEMODIFYALBUMDLG__H__

#include <IconeFile.h>
#include <SAFileEntry.h>

class CAlbumWizard;
class CAlbumData;

class CPageModifyAlbumDlg
{
public:
	CPageModifyAlbumDlg(void);
	~CPageModifyAlbumDlg(void);

	void SetParam(CAlbumWizard * cAlbumWizard)
	{
		m_cAlbumWizard = cAlbumWizard;
	}

	LRESULT OnInitDialog(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnNotify(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:

	CAlbumWizard * m_cAlbumWizard;
	char sPath[MAX_PATH];
	char sTemp[MAX_PATH];
	int iNumber;
	int iNumberImageAlbum;
	SAFileVector files;
	IconeFileVector m_IconeFileVector;
	//CAlbumData * m_AlbumLocal;
	CAlbumData * m_AlbumLocal;
	CAlbumData * m_Album;
	int iCase;
};

#endif