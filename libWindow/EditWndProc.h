#pragma once

#ifndef __EDITWNDPROC__H__
#define __EDITWNDPROC__H__

#include "LocalWindow.h"

class CEditWndProc : public CLocalWindow  
{
public:
	CEditWndProc(void);
	~CEditWndProc(void);

	int CreateEx(HWND hParent, DWORD dExStyle, DWORD dWndStyle, RECT rc);

	LRESULT OnChar(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyDown(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDown(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnGetDlgCode(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnKillFocus(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void SetRow(int iValue)
	{
		m_EditRow = iValue;
	}

	void SetCol(int iValue)
	{
		m_EditCol = iValue;
	}

private:
	int m_iKey;
	int m_EditRow;
	int m_EditCol;
};

#endif