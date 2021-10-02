

#ifndef __LISTWINDOWDATA__H__
#define __LISTWINDOWDATA__H__


#include <IconeFile.h>

class CAlbum;
class CAlbumData;
class CImageCatalogue;
class CTree;
class CWebBrowser;

class CListWindowData
{
public:
	CListWindowData(void);
	~CListWindowData(void);

//Fonction
	LRESULT GestionDesCategories(HWND hWnd);
	LRESULT GestionDesAttributs(HWND hWnd, const char * szFileName, HWND hWndParent);
	LRESULT CreateNewAlbum(const char * albumName);
	LRESULT DeleteAlbum(HWND hWndParent);
	LRESULT InitializeFolder(HWND hWnd,const char * szFolder, HWND hWndParent);
	void SendInitMessage(const int &iNumAlbum,int iNumTypeDonne, HWND hWndParent);

//Parametre
	CAlbum * m_CAlbumCatalogue;
	CAlbumData * m_Album;
	CImageCatalogue * m_CImageCatalogue;
	CTree * classTree;
	IContextMenu2  *g_pcm2;
	CWebBrowser * m_CWebBrowser;
	POINT ptOld;

	HIMAGELIST himl;    // handle to image list
	HTREEITEM m_ItemOld;
	LPMALLOC  g_pMalloc;

	char cFile[MAX_PATH];
	char cDirectory[MAX_PATH];
	char cDirectoryTemp[MAX_PATH];

	int g_fDragging;
	int iListDragging;
 	int iNumAlbum;
	int iInitializeFolder;
	int iRecherche;

	IconeFileVector * m_IconeFileVector;
	bool m_bDblClick;

#ifdef SQLSERVERCE	

	IconeFileVector m_IconeFileVectorAlbum;

#endif

private:

	LRESULT UpdateCategorie(AttributVector * attributVectorAlbum);

};

#endif