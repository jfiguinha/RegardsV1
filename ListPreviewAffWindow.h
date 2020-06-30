// ListPreviewAffWindow.h: interface for the CListPreviewAffWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTPREVIEWAFFWINDOW_H__E800051B_14F5_49F5_9FFA_EF38734AD57A__INCLUDED_)
#define AFX_LISTPREVIEWAFFWINDOW_H__E800051B_14F5_49F5_9FFA_EF38734AD57A__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include "libWindow\LocalWindow.h"

class CListPreview;

class CListPreviewAffWindow : public CLocalWindow
{
public:

	int Run();
	
	LRESULT OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnEraseBackground(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	CListPreviewAffWindow();
	virtual ~CListPreviewAffWindow();

	inline void SetDirectory(const char * cDirectory)
	{
		if(m_cDirectory != NULL)
			delete[] m_cDirectory;

		m_cDirectory = new char[strlen(cDirectory) + 1];
		strcpy_s(m_cDirectory,strlen(cDirectory)+1,cDirectory);
	}

	inline char * GetDirectory()
	{
		char * m_cCopy = new char[strlen(m_cDirectory) + 1];
		strcpy_s(m_cCopy,strlen(m_cDirectory)+1,m_cDirectory);
		return m_cCopy;
	}

	inline HWND GetHWnd()
	{
		return m_hWnd;
	}

	inline void SetNavigueWindow(HWND hWnd)
	{
		m_hWndNavigue = hWnd;
	}

private:

	void DesHighlightItem(HWND hWnd,const int &iNumItem,const bool &m_bFlip);

	CListPreview * m_ListPreview;
	int iItem;
	int iOldYPos;
	char * m_cDirectory;
	HWND m_hWnd;
	HWND m_hWndNavigue;

};

#endif // !defined(AFX_LISTPREVIEWAFFWINDOW_H__E800051B_14F5_49F5_9FFA_EF38734AD57A__INCLUDED_)
