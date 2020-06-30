#pragma once

#include <IconeFile.h>
#include <SAFileEntry.h>

class CCatalogue;
class CTree;
class CAlbumData;

typedef struct SHAREDWIZDATA {


} SHAREDWIZDATA, *LPSHAREDWIZDATA;

typedef struct AJOUTIMAGE{
	IconeFileVector * m_IconeFileVector;
	CAlbumData * m_AlbumLocal;
	CAlbumData * m_Album;
	AttributVector * m_AttributVector;
	SAFileVector * files;
	int iCas;
	int iValueChecked;
	HWND hWnd;
	HWND hDlg;
	DWORD dwThreadId; 
	HANDLE hThread;
	bool m_bTerminer;
}AJOUTIMAGEDATA, *LPAJOUTIMAGEDATA;


class CAlbumWizardInternal
{
public:
	CAlbumWizardInternal(void);
	~CAlbumWizardInternal(void);


};
