

#ifndef __GRAVEURISOWIZARD_H
#define __GRAVEURISOWIZARD_H

#include <IconeFile.h>

class CGraveurISOWizardPrv;

class CGraveurISOWizard
{
public:
	CGraveurISOWizard(void);
	~CGraveurISOWizard(void);

	void CreateAlbumWizard(HWND hParent, const string &m_stgFilename, const int &iType, IconeFileVector m_IconeFileVector);
	void CreateFolderWizard(HWND hParent, const string &m_stgFilename);
	void BurnIsoWizard(HWND hParent, const string &m_stgFilename);

private:

	int CreatePropertySheetIso(HWND hwndOwner, const string &m_stgFilename);
	int CreatePropertySheetFolder(HWND hwndOwner, const string &m_stgFilename);
	int CreatePropertySheet(HWND hwndOwner, const string &m_stgFilename, const int &iType, IconeFileVector m_IconeFileVector);
	static BOOL __stdcall InitDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL __stdcall SelectBurner (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL __stdcall CreateIsoFile (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL __stdcall GravureDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL __stdcall EndDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif