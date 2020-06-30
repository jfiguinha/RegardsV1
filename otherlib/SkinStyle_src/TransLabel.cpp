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

// TransLabel.cpp: implementation of the TransLabel class.
//
//////////////////////////////////////////////////////////////////////

#include "TransLabel.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TransLabel::TransLabel()
{
	m_Font = NULL;
}

TransLabel::~TransLabel()
{
	if(m_Font != NULL)
		::DeleteObject(m_Font);
}

void TransLabel::OnDraw(HDC hDC)
{
	char szText[ 128 ];
	RECT Rect;

	::GetClientRect( m_hWnd, &Rect );

	if( ::GetWindowText( m_hWnd, szText, 128 ) )//  If there's text to show
	{
		::SetBkMode( hDC, TRANSPARENT );		//  No text background
		::SelectObject( hDC,  m_Font);
		::SetTextColor( hDC, m_FontColor );
		Rect.left += 4;							//  Move text in a little bit
												//  Draw it to display any underscore
		::DrawText( hDC, szText, lstrlen( szText ), &Rect, 
			DT_LEFT | DT_VCENTER | DT_SINGLELINE );
	}
}

HWND TransLabel::CreateEx(DWORD dwExStyle, LPCTSTR lpszClass, 
		LPCTSTR lpszName, DWORD dwStyle, int x, int y, int nWidth, 
		int nHeight, HWND hParent, HINSTANCE hInst, LPCTSTR lpszFont, 
		int nFontSize, ULONG fColor)
{
	HWND ret = KWindow::CreateEx(dwExStyle, lpszClass, lpszName, 
		dwStyle | WS_CHILD | WS_VISIBLE, x, y, nWidth, nHeight, 
		hParent, NULL, hInst);

	SetFont(lpszFont, nFontSize, fColor);

	return ret;
}

void TransLabel::SetWindowText(LPCTSTR lpszText)
{
	::SetWindowText(m_hWnd, lpszText);
	::InvalidateRect( GetParent( m_hWnd ), NULL, TRUE );
	::InvalidateRect(m_hWnd, NULL, FALSE);
}

void TransLabel::SetFont(LPCTSTR lpszFont, int nFontSize, ULONG nColor)
{
	if(m_Font != NULL)
		::DeleteObject(m_Font);

	m_Font =  ::CreateFont(
		nFontSize,				// height of font
		0,						// average character width
		0,						// angle of escapement
		0,						// base-line orientation angle
		FW_NORMAL,				// font weight
		0,						// italic attribute option
		0,						// underline attribute option
		0,						// strikeout attribute option
		DEFAULT_CHARSET,		// character set identifier
		OUT_TT_PRECIS,			// output precision
		CLIP_DEFAULT_PRECIS,	// clipping precision
		ANTIALIASED_QUALITY,	// output quality
		DEFAULT_PITCH,			// pitch and family
		lpszFont				// typeface name
	);

	m_FontColor = nColor;
}
