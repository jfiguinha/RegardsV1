

#ifndef __NAVIGUEDATA_H__
#define __NAVIGUEDATA_H__

class CIBitmap;
class CScroll;
class CFichier;
class CLoadImage;
class CDessin;
class CListPreviewAffWindow;
#include <Regardsbitmap.h>
using namespace Regards::libbitmap;

class CNavigueData
{
public:

	CNavigueData(void);
	~CNavigueData(void);


	CBitmap * GetCBitmap();
	CIBitmap * GetCIBitmap();

	void SetCIBitmap(CIBitmap * ciBitmap);

	CScroll * GetCScroll();

	CDessin * GetDessin();
	CFichier * GetCFichier();

	int GetSelect();

	bool GetVideo();
	void SetVideo(bool bValue);

	bool GetFullScreen();

	void SetFullScreen(bool bValue);
	char * GetCFile();

	void SetCFile(char * cValue);
	void InitCFirstFile(int iSize);

	char * GetCFirstFile();
	void SetCFirstFile(char * cValue);

	void InitCFile(int iSize);
	void InitCDirectory(int iSize);

	char * GetCDirectory();
	void SetCDirectory(char * cValue);

	void SetHauteQualite(int iValue);
	int GetHauteQualite();

	void SetCreateMiniature(bool bValue);
	bool GetCreateMiniature();

	void Set3D(bool bValue);
	bool Get3D();

	void Set3DDiaporama(bool bValue);
	bool Get3DDiaporama();

	void SetShrink(bool bValue);
	bool GetShrink();

	void SetIncludeSubFolder(bool bValue);
	void SetSelect(int iValue);

	void SetHWnd(HWND hWnd);
	HWND GetHWnd();

	void SetPreviewHWnd(HWND hWnd);
	HWND GetPreviewHWnd();

	CListPreviewAffWindow * GetListPreview();
	CFichier * GetFichier();

	int GetSequence();
	void SetSequence(int iValue);

	void SetSubFolder(bool bValue);
	bool GetSubFolder();

	int GetDuree();
	void SetDuree(int iValue);

	void SetRawFile(bool bValue);
	bool GetRawFile();

	void SetShowInfos(bool bValue);
	bool GetShowInfos();

	void SetGpGpu(bool bValue);
	bool GetGpGpu();

private:

	CIBitmap * m_ciBitmap;
	//
	CScroll * m_cScroll;
	CBitmap * m_cBitmap;
	CFichier  * m_cFichier;
	CLoadImage * m_ChargementImage;
	//LPBITMAPDATA m_ciBitmapTable;
	int m_iAction;
	//bool m_bDiaporama;
	int m_iHauteQualite;
	int m_iSequence; // 0 : Pour Avant // 1: Pour Arriere
	bool m_bShrink; //Variable pour savoir si l'image sera pleine écran
	int m_iDuree;
	bool m_bFullScreen;
	bool m_bRawFile;
	char * cDirectory;
	char * cFile;
	char * cFirstFile;
	CDessin * m_cDessin;
	int m_iSelect;
	bool m_bCreateMiniature;
	bool m_bVideo;
	bool m_b3d;
	bool m_b3dDiaporama;
	bool m_bIncludeSubFolder;
	HWND m_hWnd;
	HWND m_hWndPreview;
	CListPreviewAffWindow * m_cListPreviewAffWindow;
	bool m_bShowInfos;
	bool m_bGpGpu;
	//LPListPreviewAffDATA m_PreviewData;
};

#endif