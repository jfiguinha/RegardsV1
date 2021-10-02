// WinMain.h: interface for the CWinMain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINMAIN_H__694B43E3_C4F8_498E_99D6_9A791CD97BDD__INCLUDED_)
#define AFX_WINMAIN_H__694B43E3_C4F8_498E_99D6_9A791CD97BDD__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include <MainWindow.h>

class CWinMainData;
#ifdef COOLMENU
class CCoolMenu;
#endif

class CWinMain : public CMainWindow 
{
public:

	CWinMain();
	virtual ~CWinMain();

	void ShowIntro(const BOOL &m_bShow);
	void SetIntroWindow(HWND hWnd);
	void SetCommandLine(char * szCommandLine);
	bool GetExplorer()
	{
		return m_bExplorer;
	}
	HWND GetWnd();

private:


	LRESULT OnEraseBackground(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnNotify(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnContextMenu(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnMeasureItem(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDrawItem(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseMove(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnActivate(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void SelectAlbum(HWND hwnd);

	static ULONG __stdcall AffichageBMP(LPVOID parametre);
	static ULONG __stdcall TraitementBMP(LPVOID parametre);

	bool m_bExplorer;
	bool m_bCommandLine;

#ifdef COOLMENU
	CCoolMenu * m_CCoolMenu;
#endif

	CWinMainData * m_cWinMainData;
	HWND m_hWnd;
	DWORD dwThreadId; 
	HANDLE hThread; 
	DWORD dwThreadTraitementId; 
	HANDLE hThreadTraitement; 
};

#endif // !defined(AFX_WINMAIN_H__694B43E3_C4F8_498E_99D6_9A791CD97BDD__INCLUDED_)
