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

// Bitmap.cpp: implementation of the Bitmap class.
//
//////////////////////////////////////////////////////////////////////
#include "Bitmappic.h"

#include "SkinDialog.h"

// Load a Bitmap from a resource
BOOL Bitmap::Load (HINSTANCE hInst, char const * resName)
{
	Free ();

	_hBitmap = (HBITMAP) ::LoadImage (hInst, resName,
		IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	
	return _hBitmap != 0;
}

// Load a Bitmap from a resource
BOOL Bitmap::Load (HINSTANCE hInst, int id)
{
	Free ();
	
	_hBitmap = (HBITMAP) ::LoadImage (hInst, MAKEINTRESOURCE (id),
		IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	
	return _hBitmap != 0;
}

// Load a Bitmap Directly from a file
BOOL Bitmap::Load (char * path)
{
	Free ();

	_hBitmap = (HBITMAP) ::LoadImage (0, path, 
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	
	return _hBitmap != 0;
}

// Get The Size of the Bitmap
void Bitmap::GetSize (int & width, int & height)
{
	BITMAP bm;
	::GetObject (_hBitmap, sizeof (bm), & bm);
	width = bm.bmWidth;
	height = bm.bmHeight;
}
