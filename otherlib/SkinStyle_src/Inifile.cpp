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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#include "IniFile.H"

IniFile::IniFile(char *file)
{
	Load(file);
}

void IniFile::Load(char *file)
{
	Free();
	m_Name = (char *)::GlobalAlloc(0, MAX_PATH);
	memset(m_Name, 0, MAX_PATH);

	_snprintf(m_Name, MAX_PATH, "%s\0", file);
}

char *IniFile::ReadString(char * m_Sec, char * m_Ident, char * m_Def)
{
	char *Buffer = (char *)::GlobalAlloc(0, 2048);
	memset(Buffer, 0, 2048);

	//Chargement des données
	::GetPrivateProfileString(m_Sec, m_Ident, m_Def, Buffer, 2047, m_Name);

	if(Buffer[0] == 0) {
		::GlobalFree(Buffer);
		Buffer = NULL;
	}

	return Buffer; 
}

BOOL IniFile::WriteString(char * m_Sec, char * m_Ident, char * m_Val)
{
	return ::WritePrivateProfileString(m_Sec, m_Ident, m_Val, m_Name);
}