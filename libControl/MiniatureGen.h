

#ifndef __MINIATUREGEN__H__
#define __MINIATUREGEN__H__

#include "CreateDialog.h"


#define WM_TERMINER_THREAD 0x2000
#define WM_STOP_DOWNLOAD_AUTO 0x2001
#define WM_START_DOWNLOAD_AUTO 0x2002
#define WM_DOWNLOAD_ONESHOT 0x2003
#define IDC_TRAITEMENTDATA 0x2004
#define IDC_ENDTRAITEMENTDATA 0x2005

class CMiniatureGenData;
class CLoadImage;

class CMiniatureGen : public CCreateDialog
{
public:
	CMiniatureGen(void);
	~CMiniatureGen(void);

	HWND GetWindowHandle()
	{
		return this->m_hWnd;
	}

	void SetThreadId(DWORD dwThreadID)
	{
		m_dwThreadID = dwThreadID;
	}	

private:

	int Run(CMiniatureGenData * cMiniatureGenData);
	static unsigned long __stdcall threadProc(void* ptr);
	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnTimer(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	DWORD m_dwThreadID;
	HANDLE m_hThread;
	DWORD threadID;
	HANDLE m_hWaitThread;

	//int m_iNumImage;
};

#endif