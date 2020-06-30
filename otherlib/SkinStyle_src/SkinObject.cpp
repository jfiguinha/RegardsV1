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

// SkinObject.cpp: implementation of the SkinObject class.
//
//////////////////////////////////////////////////////////////////////

#include "SkinObject.h"
#include "Util.h"

void SkinObject::OnDraw(HDC hDC)
{
	if ( m_Main )
		DrawBitmap(hDC, m_Main);
}

HWND SkinObject::CreateEx(DWORD dwExStyle, LPCTSTR lpszClass, LPCTSTR lpszName, DWORD dwStyle, 
		int x, int y, HWND hParent, HINSTANCE hInst, Bitmap Mask, Bitmap Main)
{
	BITMAP bm;
	HWND ret;

	m_Main = Main;

	::GetObject(m_Main, sizeof(bm), &bm);

	m_Height = bm.bmHeight;
	m_Width = bm.bmWidth;

	ret = KWindow::CreateEx(dwExStyle, lpszClass, lpszName, dwStyle, 
				x, y, m_Width, m_Height, hParent, NULL, hInst);

	if (Mask != NULL) {
		m_Mask = Mask;
		HRGN h = BitmapToRegion((HBITMAP)m_Mask, RGB(255,255,255), RGB(0,0,0));
		if (h)
			::SetWindowRgn(m_hWnd, h, TRUE);

		// After a successful call to SetWindowRgn, the system owns the region specified 
		// by the region handle hRgn. The system does not make a copy of the region. 
		// Thus, you should not make any further function calls with this region handle. 
		// In particular, do not delete this region handle. 
		// The system deletes the region handle when it no longer needed.
		// ::DeleteObject(h);
	}

	return ret;
}

HWND SkinObject::CreateEx(DWORD dwExStyle, LPCTSTR lpszClass, LPCTSTR lpszName, DWORD dwStyle, 
		int x, int y, HWND hParent, HINSTANCE hInst, LPCTSTR lpszMask, LPCTSTR lpszMain)
{
	BITMAP bm;
	HWND ret;

	m_Mask.Load((char *)lpszMask);
	m_Main.Load((char *)lpszMain);

	::GetObject(m_Main, sizeof(bm), &bm);

	m_Height = bm.bmHeight;
	m_Width = bm.bmWidth;

	ret = KWindow::CreateEx(dwExStyle, lpszClass, lpszName, dwStyle, 
				x, y, m_Width, m_Height, hParent, NULL, hInst);

	if (lpszMask != NULL) {
		HRGN h = BitmapToRegion((HBITMAP)m_Mask, RGB(255,255,255), RGB(0,0,0));
		if (h)
			::SetWindowRgn(m_hWnd, h, TRUE);

		// After a successful call to SetWindowRgn, the system owns the region specified 
		// by the region handle hRgn. The system does not make a copy of the region. 
		// Thus, you should not make any further function calls with this region handle. 
		// In particular, do not delete this region handle. 
		// The system deletes the region handle when it no longer needed.
		// ::DeleteObject(h);
	}

	return ret;
}
