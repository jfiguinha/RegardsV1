// AlbumWizard.h: interface for the CAlbumWizard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALBUMWIZARD_H__EC62FCA4_66DC_4EB7_977E_021F9033F863__INCLUDED_)
#define AFX_ALBUMWIZARD_H__EC62FCA4_66DC_4EB7_977E_021F9033F863__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include <IconeFile.h>

class CAlbumWizardPrv;
class CCatalogue;
class CTree;
class CIconeFile;
class CAlbumData;
class CInitDlgProcAlbumWizard;
class CPageCritereAlbumDlg;
class CPageModifyAlbumDlg;

class CAlbumWizard  
{
public:
	CAlbumWizard();
	virtual ~CAlbumWizard();
	void CreateAlbumWizard(HWND hParent, HINSTANCE hInstance,int nCmdShow,CCatalogue * cCatalogue, HWND * hWndTree,CTree * classTree);

	HFONT GetTitleFont()
	{
		return hTitleFont;
	}

	CAlbumData * RecupPtAlbum();

	AttributVector * RecupPtAttribut();

	int VerifNomAlbum(const char * cValue);

	int GetValueChecked()
	{
		return iValueChecked;
	}

	int GetSelectedAlbum()
	{
		return m_iSelectAlbum;
	}

	int FinishPageModifyAlbum(HWND hwndDlg,CAlbumData * m_Album,IconeFileVector &m_IconeFileVector);
	

private:

	static BOOL CALLBACK InitDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK PageMenuDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK PagePhotoDigitalDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK PageAlbumMenuDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK PageSelectAlbumDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK PageModifyAlbumDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK PageCritereAlbumDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK EndDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:

	HFONT hTitleFont; //The title font for the Welcome and Completion pages
	BOOL IsBoxChecked; //The state of the first interior page's check box
	BOOL IsButtonClicked; //The state of the first interior page's group box
	int iValueChecked; //Valeur du bouton radio appuyez
	//other shared data added here
	CCatalogue * m_CCatalogue;
	int m_iSelectAlbum;
	HWND * m_hWndTree;
	CTree * m_classTree;
	HWND m_hWndParent;

	CInitDlgProcAlbumWizard * m_cInitDlgProc;
	CPageCritereAlbumDlg * m_cPageCritere;
	CPageModifyAlbumDlg * m_cPageModify;

};

#endif // !defined(AFX_ALBUMWIZARD_H__EC62FCA4_66DC_4EB7_977E_021F9033F863__INCLUDED_)
