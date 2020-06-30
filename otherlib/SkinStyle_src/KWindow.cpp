/* SkinStyle - Win32 Skinning Library 
 * Author: John Roark <jroark@cs.usfca.edu>
 */

/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

// KWindow.cpp: implementation of the KWindow class.
//
//////////////////////////////////////////////////////////////////////
#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <stdio.h>
#include "KWindow.h"

LRESULT KWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg )
	{
		case WM_KEYDOWN:
			OnKeyDown(wParam, lParam);
			return 0;

		case WM_PAINT:
		{
			PAINTSTRUCT ps;

			::BeginPaint(m_hWnd, &ps);
			OnDraw(ps.hdc);
			::EndPaint(m_hWnd, &ps);
		}
		return 0;

		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
	}

	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK KWindow::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	KWindow * pWindow;

	if ( uMsg == WM_NCCREATE )
	{
		assert( ! ::IsBadReadPtr((void *) lParam,
			sizeof(CREATESTRUCT)) );
		MDICREATESTRUCT * pMDIC = (MDICREATESTRUCT *)
			((LPCREATESTRUCT) lParam)->lpCreateParams;
		pWindow = (KWindow *) (pMDIC->lParam);

		assert( ! ::IsBadReadPtr(pWindow, sizeof(KWindow)) );
		::SetWindowLong(hWnd, GWL_USERDATA, (LONG) pWindow);
	}
	else
		pWindow = (KWindow *)::GetWindowLong(hWnd, GWL_USERDATA);

	if ( (pWindow) && (!pWindow->m_Decon) )
		return pWindow->WndProc(hWnd, uMsg, wParam, lParam);
	else
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void KWindow::GetWndClassEx(WNDCLASSEX &wc)
{
	memset(&wc, 0, sizeof(wc));

	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = NULL;
	wc.hIconSm       = NULL;
}

HWND KWindow::CreateEx(DWORD dwExStyle, LPCTSTR lpszClass, LPCTSTR lpszName, DWORD dwStyle, 
				int x, int y, int nWidth, int nHeight, HWND hParent, HMENU hMenu, HINSTANCE hInst)
{
	if ( ! RegisterClass(lpszClass, hInst) )
		return false;

	MDICREATESTRUCT mdic;
	::memset(&mdic, 0, sizeof(mdic));
	mdic.lParam = (LPARAM) this;
	m_hWnd = ::CreateWindowEx(dwExStyle, lpszClass, lpszName, 
		dwStyle, x, y, nWidth, nHeight, 
		hParent, hMenu, hInst, &mdic);

	m_hInst = hInst;
	m_ClassName = (char *)::GlobalAlloc(0, (strlen(lpszClass) + 1));
	_snprintf(m_ClassName, (strlen(lpszClass) + 1), "%s\0", lpszClass);

	return m_hWnd;
}

bool KWindow::RegisterClass(LPCTSTR lpszClass, HINSTANCE hInst)
{
	WNDCLASSEX wc;

	if ( ! ::GetClassInfoEx(hInst, lpszClass, &wc) )
	{
		GetWndClassEx(wc);

		wc.hInstance     = hInst;
		wc.lpszClassName = lpszClass;

		if ( ! ::RegisterClassEx(&wc) )
			return false;
	}

	return true;
}

WPARAM KWindow::MessageLoop()
{
	MSG msg;

	while ( ::GetMessage(&msg, NULL, 0, 0) )
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	return msg.wParam;
}
