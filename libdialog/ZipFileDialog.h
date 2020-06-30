

#ifndef __ZIPFILEDIALOG__H__
#define __ZIPFILEDIALOG__H__

#include <vector>
#include "../libWindow/LocalDialog.h"

class CFiles;
#ifdef COOLMENU
class CCoolMenu;
#endif
class CToolbar;

typedef vector < int > IntArray;

class CZipFileDialog : public CLocalDialog
{
public:
	CZipFileDialog(HINSTANCE hInst);
	~CZipFileDialog(void);

	void SetParam(const char * szFile)
	{
		StringCchCopy(m_cCompressionFichier,MAX_PATH,szFile);
	}

private:

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnNotify(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnMeasureItem(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDrawItem(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	int AjoutFichierZip(HWND &hDlg, const int &i);
	int VoirFichier(int iNumItem);

	HINSTANCE hInstance;
	TCHAR szFileName[MAX_PATH];
	CFiles * m_CFiles;
#ifdef COOLMENU
	CCoolMenu * m_CCoolMenu;
#endif
	CToolbar * m_cToolbar;

	int iNbFiles;
	HWND hWndToolbar;
	HWND hWndStatusBar;
	
	IntArray m_IntArray;

	TCHAR m_cCompressionFichier[MAX_PATH];
};

#endif