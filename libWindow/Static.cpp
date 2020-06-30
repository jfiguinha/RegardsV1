// Static.cpp: implementation of the CStatic class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Static.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStatic::CStatic()
{
	m_hWnd = NULL;
}

CStatic::~CStatic()
{
	if(m_hWnd != NULL)
		DestroyWindow(m_hWnd);
}

int CStatic::CreateEx(HWND hParent, DWORD dExStyle, DWORD dWndStyle, RECT rc)
{

	m_hWnd = CreateWindowEx(dExStyle,"STATIC","", dWndStyle,
					 0,0,0,0,
					hParent,
					 (HMENU)NULL,
					 hInstance,
					 NULL);

	return 0;
}
