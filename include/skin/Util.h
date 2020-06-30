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

#include <windows.h>

#include "Bitmap.h"

HBITMAP ClipBitmap(HBITMAP bmpSrc, int xSrc, int ySrc, 
				   int nWidth, int nHeight);
Bitmap LoadImage(char *szFile);
void DrawBitmap(HDC hDC, Bitmap &bMap);
char * getPath(char *Path);
HRGN BitmapToRegion (HBITMAP hBmp, COLORREF cTransparentColor = 0, 
	COLORREF cTolerance = 0x101010);