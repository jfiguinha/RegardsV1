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

// SkinButton.cpp: implementation of the SkinButton class.
//
//////////////////////////////////////////////////////////////////////

#include "SkinButton.h"

#include <stdio.h>
#include <windowsx.h>
#include <winuser.h>
#include <commctrl.h>

#include "Util.h"

HWND SkinButton::CreateEx(LPCTSTR lpszName, DWORD dwStyle, int x, int y, HWND hParent, 
		HINSTANCE hInst, Bitmap Mask, Bitmap Main, Bitmap Click, Bitmap Over, Bitmap Disabled)
{
	HWND ret = SkinObject::CreateEx(0, lpszName, lpszName, WS_CHILD | WS_VISIBLE 
		| CS_VREDRAW | CS_HREDRAW | CS_GLOBALCLASS, 
		x, y, hParent, hInst, Mask, Main);

	m_hWndParent = hParent;
	m_hInst = hInst;

	m_BtnName = (char *)::GlobalAlloc(0, strlen(lpszName) + 1);
	::_snprintf(m_BtnName, strlen(lpszName) + 1, "%s\0", lpszName);
	
	m_Clicked = Click;
	m_Over = Over;
	m_Disabled = Disabled;

	return ret;
}

HWND SkinButton::CreateEx(LPCTSTR lpszName, DWORD dwStyle, int x, int y, HWND hParent, 
	HINSTANCE hInst, LPCTSTR lpszMask, LPCTSTR lpszMain, LPCTSTR lpszClick, LPCTSTR lpszOver, LPCTSTR lpszDisabled)
{

	HWND ret = SkinObject::CreateEx(0, lpszName, lpszName, WS_CHILD | WS_VISIBLE 
		| CS_VREDRAW | CS_HREDRAW | CS_GLOBALCLASS, 
		x, y, hParent, hInst, lpszMask, lpszMain);

	m_hWndParent = hParent;
	m_hInst = hInst;

	m_BtnName = (char *)::GlobalAlloc(0, strlen(lpszName) + 1);
	::_snprintf(m_BtnName, strlen(lpszName) + 1, "%s\0", lpszName);
	
	m_Clicked.Load((char *)lpszClick);

	if ( dwStyle & SBS_MOUSEOVER ) {
		m_Over.Load((char *)lpszOver);
	}

	m_Disabled.Load((char *)lpszDisabled);

	return ret;
}

LRESULT SkinButton::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg )
	{
		case WM_LBUTTONDOWN:
			return OnLButtonDown(wParam, lParam);
			
		case WM_LBUTTONUP:
			return OnLButtonUp(wParam, lParam);

		case WM_RBUTTONDOWN:
			return OnRButtonDown(wParam, lParam);
			
		case WM_RBUTTONUP:
			return OnRButtonUp(wParam, lParam);

		case WM_MOUSEMOVE: 
			return OnMouseMove(wParam, lParam);

		case WM_MOUSELEAVE:
		{
			_mouse_over = false;
			_left_button_down = false;
			::InvalidateRect( hWnd, NULL, false );
			m_BeingTracked = FALSE;
		}
		return FALSE;
	}

	return KWindow::WndProc(hWnd, uMsg, wParam, lParam);
}

void SkinButton::OnDraw(HDC hDC)
{
		if (_left_button_down)
			DrawBitmap(hDC, m_Clicked);
		else if (_mouse_over)
			DrawBitmap(hDC, m_Over);
		else if (!_has_focus)
			DrawBitmap(hDC, m_Disabled);
		else 
			DrawBitmap(hDC, m_Main);
}

BOOL SkinButton::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	::PostMessage( m_hWndParent, SB_LBUTTONDOWN, wParam, (LPARAM)m_BtnName );
	_left_button_down = true;
	::InvalidateRect( m_hWnd, NULL, false );

	return FALSE;
}

BOOL SkinButton::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
	::PostMessage( m_hWndParent, SB_RBUTTONDOWN, wParam, (LPARAM)m_BtnName );
	return FALSE;
}

BOOL SkinButton::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	::PostMessage( m_hWndParent, SB_LBUTTONUP, wParam, (LPARAM)m_BtnName );
	_left_button_down = false;
	::InvalidateRect( m_hWnd, NULL, false );
	return FALSE;
}

BOOL SkinButton::OnRButtonUp(WPARAM wParam, LPARAM lParam)
{
	::PostMessage( m_hWndParent, SB_RBUTTONUP, wParam, (LPARAM)m_BtnName );
	return FALSE;
}

BOOL SkinButton::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if(!m_BeingTracked)
	{
		TRACKMOUSEEVENT lpEventTrack;

		lpEventTrack.cbSize      = sizeof( TRACKMOUSEEVENT );
		lpEventTrack.dwFlags     = TME_LEAVE;
		lpEventTrack.hwndTrack   = m_hWnd;

		if(!::_TrackMouseEvent(&lpEventTrack))
		{
			//::MessageBox(m_hWndParent, "TrackMouseEvent Failed", "Error", MB_OK);
		}
		m_BeingTracked = TRUE;
	}
	if( wParam != MK_LBUTTON )
	{	
		_left_button_down = false;
		_mouse_over = true;
		::InvalidateRect( m_hWnd, NULL, false );
	}

	return FALSE;
}

void SkinButton::Enable(BOOL set)
{
	_has_focus = set;	
	::InvalidateRect(m_hWnd, NULL, FALSE);
}
