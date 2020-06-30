// TreeWindow.h: interface for the CTreeWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TREEWINDOW_H__EEA4D7B9_D6CE_415F_8B22_D813C9330BD9__INCLUDED_)
#define AFX_TREEWINDOW_H__EEA4D7B9_D6CE_415F_8B22_D813C9330BD9__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include "libWindow\splitter.h"

#include <IconeFile.h>

class CStatusBar;
class CTreeWindowData;
class CParameter;

class CTreeWindow : public CWndSplitter
{
public:

	CTreeWindow();
	virtual ~CTreeWindow();
	
	int GetOrderBy();
	void SetStatusBar(CStatusBar * m_StatusBar);

private:


	LRESULT OnKillFocus(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnContextMenu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	//LRESULT OnSetFocus(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnDrawItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnMenuChar(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnMeasureItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnEraseBackground(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT FinGenThumbnail();


	static int CALLBACK MyCompProc(LPARAM lp1, LPARAM lp2, LPARAM lp3);
	static ULONG _stdcall MonitorDirectoryProc(LPVOID pvThis);
	//static LRESULT CALLBACK DialogFindProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	
	int ShowStyleListe(const long &Style);

	void DesHighlightItem(const int &iNumItem,const bool &m_bFlip = true);
	int DeleteListFile(HWND hWnd);

	int ProcessNotifyDoubleClick(const int &iItem);

	//void MoveSplitter(CSplitter * m_Splitter, HWND &hWnd1, HWND &hWnd2);
	int TestRepertoireFile(const char * File1, const char * File2);
	void SelectListImage(int iType);
	void SelectListImage();
	void DeplacerListImage(int iType, char * destPath);
	LRESULT ProcessCustomDraw(LPARAM lParam);
	int DeplacementCurseur(int iNumItem, UINT flags);
	int CreatePaneWindow(HWND hWnd);
	int CreatePreview(HWND hWnd);
	void Actualiser(LPARAM lParam = NULL);

	CParameter * m_cParameter;
	CStatusBar * m_TreeWindStatusBar;
	int iOrderBy; //0 Par Nom Asc, 1 Par Nom Desc, 2 Par Taille Asc, 3 Par Taille Desc, 4 Par Date Asc, 5 Par Date Desc
	CTreeWindowData * m_pTreeData;
	char cNumAlbum[5];
	int iChoix;
	int iOldYPos;


};

#endif // !defined(AFX_TREEWINDOW_H__EEA4D7B9_D6CE_415F_8B22_D813C9330BD9__INCLUDED_)
