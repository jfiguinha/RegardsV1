

#ifndef __CInsertAlbum__H__
#define __CInsertAlbum__H__

#include "../libWindow/LocalDialog.h"
#include <IconeFile.h>
#include <Attribut.h>
#include <SAFileEntry.h>

class CAlbumData;

class CInsertAlbum : public CLocalDialog
{
public:
	CInsertAlbum(HINSTANCE hInstance);
	~CInsertAlbum(void);

#ifdef SQLSERVERCE

	void SetAlbum(CAlbumData * album, IconeFileVector * lpAlbumconeFileVector)
	{
		m_Album = album;
		m_AlbumconeFileVector = lpAlbumconeFileVector;
	}
#else

	void SetAlbum(CAlbumData * album)
	{
		m_Album = album;
	}

#endif


	void SetCas(int iValue)
	{
		iCas = iValue;
	}

	void SetIconeFileVector(IconeFileVector * iconeFileVector)
	{
		m_IconeFileVector = iconeFileVector;
	}

	void SetSAFileVector(SAFileVector * files)
	{
		this->files = files;
	}

	void SetWindow(HWND hWnd)
	{
		m_hWndExtern = hWnd;
	}

	void SetValueChecked(int iValue)
	{
		iValueChecked = iValue;
	}

	void SetAttributVector(AttributVector * attributVector)
	{
		m_AttributVector = attributVector;
	}

private:

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	int VerifPresenceImage(CIconeFile &m_IconeFile,IconeFileVector * m_IconeFileVector);
	static UINT _stdcall AjoutImageThread(LPVOID parametre);

#ifdef SQLSERVERCE
	IconeFileVector * m_AlbumconeFileVector;
#endif

	IconeFileVector * m_IconeFileVector;
	CAlbumData * m_AlbumLocal;
	CAlbumData * m_Album;
	AttributVector * m_AttributVector;
	SAFileVector * files;
	int iCas;
	int iValueChecked;
	HWND m_hDlg;
	HWND m_hWndExtern;
	DWORD dwThreadId; 
	HANDLE hThread;
	bool m_bTerminer;
	HINSTANCE hInstance;
};

#endif