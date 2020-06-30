// Edit.cpp: implementation of the CEdit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Edit.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEdit::CEdit()
{
	m_hWnd = NULL;
}

CEdit::~CEdit()
{
	if(m_hWnd != NULL)
		DestroyWindow(m_hWnd);
}

int CEdit::CreateEx(HWND hParent, DWORD dExStyle, DWORD dWndStyle, RECT rc)
{
	m_hWnd = CreateWindowEx(dExStyle,"Edit","", dWndStyle,
					 rc.left,rc.top,rc.right,rc.bottom,
					hParent,
					 (HMENU)NULL,
					 hInstance,
					 NULL);

	return 0;
}

void CEdit::SetWindow(HWND hWnd)
{
	m_hWnd = hWnd;
}
