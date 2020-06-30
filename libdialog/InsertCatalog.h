

#ifndef __INSERTCATALOG__H__
#define __INSERTCATALOG__H__

#include "../libWindow/LocalDialog.h"
#include <IconeFile.h>
class CAlbumData;

class CInsertCatalog : public CLocalDialog
{
public:
	CInsertCatalog(HINSTANCE hInstance);
	~CInsertCatalog(void);

#ifdef SQLSERVERCE
	void SetParam(CAlbumData * cAlbum, IconeFileVector * iconeFileVector)
	{
		m_AlbumTemp = cAlbum;
		m_iconeFileVector = iconeFileVector;
	}
#else
	void SetParam(CAlbumData * cAlbum)
	{
		m_AlbumTemp = cAlbum;
	}
#endif

	int GetReturnValue()
	{
		return m_iReturnValue;
	}

private:

	static UINT _stdcall AjoutImageThread(LPVOID parametre);

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	CAlbumData * m_AlbumTemp;
	DWORD dwThreadId; 
	HANDLE hThread;
	HWND hWnd;
#ifdef SQLSERVERCE
	IconeFileVector * m_iconeFileVector;
#endif
	HINSTANCE hInstance;
	int m_iReturnValue;
};

#endif