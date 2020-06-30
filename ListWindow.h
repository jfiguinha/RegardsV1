// ListWindow.h: interface for the CListWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTWINDOW_H__185F6D0D_D625_4DEC_AA0E_C4D72C133B84__INCLUDED_)
#define AFX_LISTWINDOW_H__185F6D0D_D625_4DEC_AA0E_C4D72C133B84__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include "libWindow\LocalWindow.h"

class CListWindowData;
class CParameter;

class CListWindow : public CLocalWindow
{
public:

	CListWindow();
	virtual ~CListWindow();

	// The lParam value contains the event SHCNE_xxxx
	// The wParam value supplies the SHNOTIFYSTRUCT

	typedef struct {
		DWORD dwItem1;    // dwItem1 contains the previous PIDL or name of the folder. 
		DWORD dwItem2;    // dwItem2 contains the new PIDL or name of the folder. 
	} SHNOTIFYSTRUCT;

private:

	LRESULT OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnTimer(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnContextMenu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnDrawItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnMenuChar(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnMeasureItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	//LRESULT OnSetFocus(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnMediaChange(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static UINT _stdcall MonitorDirectoryProc(LPVOID pvThis);

	UINT RegisterDriveNotification(HWND hWnd);
	UINT DeRegisterDriveNotification(HWND hWnd);
	string GetPathFromPIDL(DWORD pidl);

	CListWindowData * m_pListData;
	CParameter * m_cParameter;
	ULONG m_ulSHChangeNotifyRegister;

};

#endif // !defined(AFX_LISTWINDOW_H__185F6D0D_D625_4DEC_AA0E_C4D72C133B84__INCLUDED_)
