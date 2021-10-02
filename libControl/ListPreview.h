// ListPreview.h: interface for the CListPreview class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTPREVIEW_H__CBDB5CB2_7563_4B5C_9691_0433DB6B818F__INCLUDED_)
#define AFX_LISTPREVIEW_H__CBDB5CB2_7563_4B5C_9691_0433DB6B818F__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include "ListView.h"

class CListPreviewPrv;
class CListPreviewData;
class CStatusBar;
class CMiniatureGen;
class CMiniatureGenData;



class CListPreview : public CListView
{
public:

	void SetNbMaxMiniature(const int &iMaxMiniature);
	void SetAffichageAlaVolee(const int &iVolee);

	HIMAGELIST GetImageListHandle();
	void ViderMemoire();
	void ResumePreview();
	void PausePreview();
	void SauverListe();

	void DefineThumbnailSize(const int &iWidth, const int &iHeight);
	void DefineStatusBar(CStatusBar * m_StatusBar);
	void ViderMemoireListe();
	CListPreview();
	virtual ~CListPreview();
	HWND CreateListView(HWND hwndParent, HINSTANCE hInst);
	int InitList(const char * cDir,const char * Localisation = "0");
	int InitList(IconeFileVector * m_IconeFileVector, const char * Localisation);

	HRESULT List_Refresh();
	LRESULT List_Notify(LPARAM lParam);

	int stopGenThumbnail();

	void SetParent(HWND hWnd);

private:

	void CreateListMiniatureHandle();
	void StopListMiniatureHandle();
	static ULONG __stdcall List_CreateMiniature(LPVOID param);
	static ULONG __stdcall List_StopMiniature(LPVOID param);

	void GestionMessageMiniature(bool &bEnd,int &iNumTerminerThread, int &iNumImage,const int &iNbThread);

	CMiniatureGenData * CreateHandleData();
	CMiniatureGen * CreateHandle(int iNumThread);
	ULONG List_CreatePicture(int &iNumImage, int nItem, HWND hWnd, HWND hWndMiniature, CMiniatureGen * pMiniatureGen, CMiniatureGen * pMiniatureGenVideo, CMiniatureGen * pMiniatureGenRaw, CMiniatureGen * pMiniatureGenFolder);

	HANDLE m_hStopThread;
	DWORD dwThreadIdStop;
	
	HANDLE m_hEndThread;
	HWND hParent;
	HANDLE hThreadPreview;
	DWORD dwThreadIdPreview; 
	HANDLE m_hProcessThread;
	HANDLE m_hWaitThread;
	CStatusBar * m_ListStatusBar;
	int m_iMaxMiniature;
	int iAlbum;
	bool m_bDefineThumbnailSize;
	IconeFileVector m_IconeFileVector;

	CListPreviewData * pimpl_;
	char cAlbumThumbmail[MAX_PATH];

	
};

#endif // !defined(AFX_LISTPREVIEW_H__CBDB5CB2_7563_4B5C_9691_0433DB6B818F__INCLUDED_)
