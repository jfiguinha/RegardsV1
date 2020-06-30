#pragma once

#include "libwindow\message.h"
#include <TraitementInstance.h>
#include <IconeFile.h>

class CWndNavigueBitmap;
class CStatusBar;
class CWMPHost;
class COpenGL;
class CToolbar;
class CDropTarget;
#ifdef COOLMENU
	class CCoolMenu;
#endif
class CParameter;
class COpenGLDiaporama;
class CGestionTraitementWindow;

class CAffichageBitmapWindow : CMessage
{
public:

    static const UINT FULLSCREENMODE = 0;
    static const UINT WINDOWMODE = 1;

	CAffichageBitmapWindow(void);
	virtual ~CAffichageBitmapWindow(void);

	LRESULT OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnMeasureItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnDrawItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseWheel(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnKillFocus(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnExitMenuLoop(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnEraseBackGround(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnWindowPosChanging(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


	void SetDone(bool bValue)
	{
		m_bDone = bValue;
	}

	bool GetDone()
	{
		return m_bDone;
	}

	void SetAffichageWindow(HWND hWnd)
	{
		hwndAffichageBitmap = hWnd;
	}

	bool GetFullScreen()
	{
		return m_bFullScreen;
	}

	bool Affichage3D()
	{
		return m_b3D;
	}

	LRESULT VideoGetFullScreenState();

	void SetExplorer(bool bExplorer)
	{
		m_bExplorer = bExplorer;
	}

	bool GetExplorer()
	{
		return m_bExplorer;
	}

	void SetFilename(const char * cFileName)
	{
		StringCchCopy(cFile,255,cFileName);
	}

	void SetDirectory(char * drive,char * dir)
	{
		StringCchCopy(cDirectory,255,drive);
		StringCchCat(cDirectory,255,dir);
		cDirectory[strlen(cDirectory) - 1] = '\0';
	}

	void DefineStartValue(HWND hParent,int iTrie)
	{
		this->hParent = hParent;
		m_iFileTrie = iTrie;
		m_bFullScreen = false;
		m_iAffichageAlbum = 0;
	}

	bool GetOnCreate()
	{
		return m_bCreate;
	}

	void SetOnCreate(bool bValue)
	{
		m_bCreate = bValue;
	}

	CParameter * m_cParameter;

	void SetWindowPlacement(HWND hWnd);


private:

	void StopVideo();
	void Stop3D();
	void StopDiaporama3D();
	void ShowFileInfos(HWND hWnd, const char * Filename);
	
	int SaveVideoFrame(HWND hWnd,char * cFilename, int iSize);
	void ExtractPictureFromVideo(HWND hWnd);

	char cDirectory[MAX_PATH];
	char cFile[MAX_PATH];

	HWND hwndAffichageBitmap;
	HWND hParent;
	HWND hWndToolbar;
	HWND hWndStatusWindow;
	HWND HwndBitmap;

	CWndNavigueBitmap * m_CWndBitmap;
	CStatusBar * m_StatusBar;
	CWMPHost * m_wmpHost;
	COpenGL * m_cOpenGL;
	COpenGLDiaporama * m_cOpenGLDiaporama;
 	CToolbar * m_cToolbar;
	CDropTarget * m_pDropTarget;

#ifdef COOLMENU
	CCoolMenu * m_CCoolMenu;
#endif

	CGestionTraitementWindow * m_cGestionTraitementWindow;

	int m_iNumTexture3D;
	int m_iAffichageAlbum;
	bool m_bFullScreen;
	UINT m_bOldState;
	bool m_bBlocking;
	int m_iFileTrie;
	bool m_bExplorer;

	bool m_bPlayVideo;
	bool m_b3D;
	bool m_b3DDiaporama;

	BOOL m_bDone;
	bool m_bCreate;
	bool m_bFirst;

	IconeFileVector * m_IconeFileVector;
	WINDOWPOS _windowPos;

};
