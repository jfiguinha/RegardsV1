

#ifndef __TREEWINDOWDATA_H__
#define __TREEWINDOWDATA_H__

#include <IconeFile.h>

class CPane;
class CList;
class CListPreview;
class CPreview;
class CStatusBar;

class CTreeWindowData
{
public:
	CTreeWindowData(void);
	~CTreeWindowData(void);

	int SaveWindowPosition();
	int LoadWindowPosition(int &iPreview, int &iList);

	CPane * m_cPane;
	CList * cList;
	CListPreview * m_ListPreview;
	CPreview * m_Preview;
	IContextMenu2  *g_pcm2;
	LPMALLOC  g_pMalloc;
	IDataObject * pDataObj;
	IconeFileVector m_IconeFileVector;
	IconeFileVector * m_IconeFileVectorAlbum;

	char cDirectory[MAX_PATH];
	char cFileTemp[MAX_PATH];
	char cTempDir[MAX_PATH];
	char cFile[MAX_PATH];

	int iColumnUp;
	bool m_bClick;
	bool m_bCaptureList;
	bool m_bKeyDown;
	bool m_bClickDown;
	int iCount;
	int iCreerRepertoire;
	int iTypeData; //0 : Fichier, 1 : Album;
	int iDelete;
	int bDragging;
	int iSubItem;
	int iAffichageDimension;
	int iAffichageAfterMiniature;
	int iDemarre;
	int iTypeAffichage;
	int iNumItem;
	int iNumSelectedItem;
	int iInfosFichier;
	HIMAGELIST hDragImageList;
	VECTITEM m_vectItem;
	DWORD dwThreadId; 
	HANDLE hThread;
	int iMarquee;
	int iDragEnable;
	int iPauseMonitor;
};

#endif