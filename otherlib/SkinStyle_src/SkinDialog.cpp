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

// SkinDialog.cpp: implementation of the SkinDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "SkinDialog.h"
#include "SkinButton.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strsafe.h>
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SkinDialog::SkinDialog()
{
	_sticky = true;
	m_Loading = FALSE;
}

SkinDialog::~SkinDialog()
{
	Free();
}

//
// SkinFile Must Be a Full Path Name!
//
void SkinDialog::LoadSkin(char *SkinFile)
{
	m_Loading = TRUE;
	m_SkinFile.Load(SkinFile);

	_snprintf(m_SkinPath, MAX_PATH, "%s\0", SkinFile);
	char *tmp = strrchr(m_SkinPath, '\\');
	if(tmp != NULL)
		*(tmp + 1) = 0;

	Free();

	char *pmNormal		= m_SkinFile.ReadString("SCREEN", "Main", "");
	char *pmMask		= m_SkinFile.ReadString("SCREEN", "Mask", pmNormal);
	char *pmOver		= m_SkinFile.ReadString("SCREEN", "Over", pmNormal);
	char *pmDown		= m_SkinFile.ReadString("SCREEN", "Down", pmNormal);
	char *pmDisabled	= m_SkinFile.ReadString("SCREEN", "Disabled", pmNormal);

	char mNormal[MAX_PATH];
	char mMask[MAX_PATH];
	char mOver[MAX_PATH];
	char mDown[MAX_PATH];
	char mDisabled[MAX_PATH];

	_snprintf(mNormal, MAX_PATH, "%s%s\0", m_SkinPath, pmNormal);
	_snprintf(mMask, MAX_PATH, "%s%s\0", m_SkinPath, pmMask);
	_snprintf(mOver, MAX_PATH, "%s%s\0", m_SkinPath, pmOver);
	_snprintf(mDown, MAX_PATH, "%s%s\0", m_SkinPath, pmDown);
	_snprintf(mDisabled, MAX_PATH, "%s%s\0", m_SkinPath, pmDisabled);

	m_Main		= LoadImage(mNormal);
	m_Mask		= LoadImage(mMask);
	m_Selected	= LoadImage(mDown);
	m_Over		= LoadImage(mOver);
	m_Disabled	= LoadImage(mDisabled);

	::GlobalFree(pmNormal);
	::GlobalFree(pmMask);
	::GlobalFree(pmOver);
	::GlobalFree(pmDown);
	::GlobalFree(pmDisabled);

	BITMAP bm;

	::GetObject(m_Main, sizeof(bm), &bm);

	// resize the blank window to fit the bitmap
	if(!::SetWindowPos(m_hWnd, 0, 0, 0, bm.bmWidth, bm.bmHeight, SWP_NOMOVE | SWP_NOZORDER))
	{
		//::MessageBox(0, "SetWindowPos Failed", "Error", MB_OK);
	}

	HRGN h = BitmapToRegion((HBITMAP)m_Mask, RGB(255,255,255), RGB(0,0,0));
	if (h)
		::SetWindowRgn(m_hWnd, h, TRUE);

	// After a successful call to SetWindowRgn, the system owns the region specified 
	// by the region handle hRgn. The system does not make a copy of the region. 
	// Thus, you should not make any further function calls with this region handle. 
	// In particular, do not delete this region handle. 
	// The system deletes the region handle when it no longer needed.
	// ::DeleteObject(h);

	LoadButtons();
	LoadStatic();
	m_Loading = FALSE;
}



void SkinDialog::OnDraw(HDC hDC)
{
	if(!_has_focus) 
		DrawBitmap(hDC, m_Disabled);
	else
		DrawBitmap(hDC, m_Main);
}

LRESULT SkinDialog::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_Loading)
		return FALSE;

	switch( uMsg )
	{
		case WM_COMMAND:
			break;

		case WM_ACTIVATEAPP:
		{
			if(wParam) 
				_has_focus = true;
			else if(!wParam) 
				_has_focus = false;

			DWORD size = m_Buttons.GetSize();
			for(DWORD i = 0; i < size; i++)
				(m_Buttons[i])->Enable(wParam);

			::InvalidateRect(m_hWnd, NULL, FALSE);
		}
		return FALSE;

		case WM_NCHITTEST:
			if(_sticky)
				return HTCAPTION;
			return FALSE;

		case SB_LBUTTONUP:
			return OnButtonPressed((char *)lParam);
	}

	return KWindow::WndProc(hWnd, uMsg, wParam, lParam);
}

HWND SkinDialog::CreateEx(LPCTSTR lpszName, int x, int y, HINSTANCE hInst, LPCTSTR lpszSkinFile)
{
	HWND ret = KWindow::CreateEx(0, lpszName, lpszName, WS_POPUP, x, y, 100, 100, NULL, NULL, hInst); 
	m_hInst = hInst;

	//LoadSkin((char *)lpszSkinFile);

	return ret;
}

void SkinDialog::Free()
{
	m_ToolTips.RemoveAll();
	m_Buttons.RemoveAll();
	m_Labels.RemoveAll();

	m_Selected.Free();
	m_Over.Free();
	m_Disabled.Free();
}

void SkinDialog::LoadButtons()
{
	int i = 1;
	char *Buffer;
	char Num[256];

	_snprintf(Num, 256, "%d\0", i);
	while((Buffer = m_SkinFile.ReadString("BUTTONINFO", Num, "")) != NULL)
	{
		// parse buffer for Button information
		// then create a new button and add it to m_Buttons
		// BUTTON_NAME, x, y, width, height, tooltip, ENABLED 
		char * BtnName	= strtok(Buffer, ",");
		char * xPos		= strtok(NULL, ",");
		char * yPos		= strtok(NULL, ",");
		char * nWidth	= strtok(NULL, ",");
		char * nHeight	= strtok(NULL, ",");
		char * ToolTip	= strtok(NULL, ",");
		char * Enabled	= strtok(NULL, ",");

		SkinButton *tmp = new SkinButton;
		KToolTip *tltp = new KToolTip;

		tltp->CreateEx(
			tmp->CreateEx(BtnName, 0, atoi(xPos), atoi(yPos), m_hWnd, 
			m_hInst, 
			ClipBitmap(m_Mask, atoi(xPos), atoi(yPos), atoi(nWidth), atoi(nHeight)), 
			ClipBitmap(m_Main, atoi(xPos), atoi(yPos), atoi(nWidth), atoi(nHeight)),
			ClipBitmap(m_Selected, atoi(xPos), atoi(yPos), atoi(nWidth), atoi(nHeight)), 
			ClipBitmap(m_Selected, atoi(xPos), atoi(yPos), atoi(nWidth), atoi(nHeight)),
			ClipBitmap(m_Disabled, atoi(xPos), atoi(yPos), atoi(nWidth), atoi(nHeight))
			), m_hInst, ToolTip);
		
		m_Buttons.Add(tmp);
		m_ToolTips.Add(tltp);

		_snprintf(Num, 256, "%d\0", ++i);
		::GlobalFree(Buffer);
	}
}

void SkinDialog::LoadStatic()
{
	int i = 1;
	char *Buffer;
	char Num[256];

	_snprintf(Num, 256, "%d\0", i);
	while((Buffer = m_SkinFile.ReadString("TEXTINFO", Num, "")) != NULL)
	{
		// parse buffer for Button information
		// then create a new button and add it to m_Buttons
		// BUTTON_NAME, x, y, width, height, tooltip, ENABLED 
		char * BtnName	= strtok(Buffer, ",");
		char * Font		= strtok(NULL, ",");
		char * Bold		= strtok(NULL, ",");
		char * Italic	= strtok(NULL, ",");
		char * Fsize	= strtok(NULL, ",");
		char * Fcolor	= strtok(NULL, ",");
		char * xPos		= strtok(NULL, ",");
		char * yPos		= strtok(NULL, ",");
		char * nWidth	= strtok(NULL, ",");
		char * nHeight	= strtok(NULL, ",");
		char * Text		= strtok(NULL, ",");
		char * ToolTip	= strtok(NULL, ",");

		TransLabel *tmp = new TransLabel;
		KToolTip *tltp = new KToolTip;

		tltp->CreateEx(
			tmp->CreateEx(0, BtnName, BtnName, 0, atoi(xPos) + 6, atoi(yPos), 
			atoi(nWidth), atoi(nHeight), m_hWnd, m_hInst, Font, 
			atoi(Fsize), (ULONG)atol(Fcolor)), m_hInst, ToolTip);

		tmp->SetWindowText(Text);
		
		m_Labels.Add(tmp);
		m_ToolTips.Add(tltp);

		_snprintf(Num, 256, "%d\0", ++i);
		::GlobalFree(Buffer);
	}
}

void SkinDialog::AddSkinBitmap(HINSTANCE hInst,HBITMAP m_cMain,HBITMAP m_cMask,
							   HBITMAP m_cSelected,HBITMAP m_cOver,HBITMAP m_cDisabled)
{
	int i = 0;


	m_Main._hBitmap	= m_cMain;
	m_Mask._hBitmap		= m_cMask;
	m_Selected._hBitmap	= m_cSelected;
	m_Over._hBitmap		= m_cOver;
	m_Disabled._hBitmap	= m_cDisabled;


	BITMAP bm;

	::GetObject(m_Main, sizeof(bm), &bm);

	// resize the blank window to fit the bitmap
	if(!::SetWindowPos(m_hWnd, 0, 0, 0, bm.bmWidth, bm.bmHeight, SWP_NOMOVE | SWP_NOZORDER))
	{
		//::MessageBox(0, "SetWindowPos Failed", "Error", MB_OK);
	}

	HRGN h = BitmapToRegion((HBITMAP)m_Mask, RGB(255,255,255), RGB(0,0,0));
	if (h)
		::SetWindowRgn(m_hWnd, h, TRUE);
}

void SkinDialog::AddSkinButton(char * Name,...)
{
	//char Value[255];
	va_list marker;

	va_start(marker, Name);     /* Initialize variable arguments. */

	char * BtnName = new char[255];

	StringCchCopy(BtnName,255,Name);

	// parse buffer for Button information
	// then create a new button and add it to m_Buttons
	// BUTTON_NAME, x, y, width, height, tooltip, ENABLED 

	//char * BtnName	= va_arg( marker, char *);//strtok(Buffer, ",");
	char * xPos		= va_arg( marker, char *);//strtok(NULL, ",");
	char * yPos		= va_arg( marker, char *);//strtok(NULL, ",");
	char * nWidth	= va_arg( marker, char *);//strtok(NULL, ",");
	char * nHeight	= va_arg( marker, char *);//strtok(NULL, ",");
	char * ToolTip	= va_arg( marker, char *);//strtok(NULL, ",");
	char * Enabled	= va_arg( marker, char *);//strtok(NULL, ",");

	SkinButton *tmp = new SkinButton;
	KToolTip *tltp = new KToolTip;

	tltp->CreateEx(
		tmp->CreateEx(BtnName, 0, atoi(xPos), atoi(yPos), m_hWnd, 
		m_hInst, 
		ClipBitmap(m_Mask, atoi(xPos), atoi(yPos), atoi(nWidth), atoi(nHeight)), 
		ClipBitmap(m_Main, atoi(xPos), atoi(yPos), atoi(nWidth), atoi(nHeight)),
		ClipBitmap(m_Selected, atoi(xPos), atoi(yPos), atoi(nWidth), atoi(nHeight)), 
		ClipBitmap(m_Selected, atoi(xPos), atoi(yPos), atoi(nWidth), atoi(nHeight)),
		ClipBitmap(m_Disabled, atoi(xPos), atoi(yPos), atoi(nWidth), atoi(nHeight))
		), m_hInst, ToolTip);
	
	m_Buttons.Add(tmp);
	m_ToolTips.Add(tltp);

	va_end( marker );  

	delete[] BtnName;
}

void SkinDialog::AddSkinLabel(char * Name,...)
{
	int i = 0;
	va_list marker;

	va_start(marker, Name);     /* Initialize variable arguments. */

	char * BtnName = new char[255];
	StringCchCopy(BtnName,255,Name);

    
	// parse buffer for Button information
	// then create a new button and add it to m_Buttons
	// BUTTON_NAME, x, y, width, height, tooltip, ENABLED 
	//char * BtnName	= va_arg( marker, char *);//strtok(Buffer, ",");
	char * Font		= va_arg( marker, char *);//strtok(NULL, ",");
	char * Bold		= va_arg( marker, char *);//strtok(NULL, ",");
	char * Italic	= va_arg( marker, char *);//strtok(NULL, ",");
	char * Fsize	= va_arg( marker, char *);//strtok(NULL, ",");
	char * Fcolor	= va_arg( marker, char *);//strtok(NULL, ",");
	char * xPos		= va_arg( marker, char *);//strtok(NULL, ",");
	char * yPos		= va_arg( marker, char *);//strtok(NULL, ",");
	char * nWidth	= va_arg( marker, char *);//strtok(NULL, ",");
	char * nHeight	= va_arg( marker, char *);//strtok(NULL, ",");
	char * Text		= va_arg( marker, char *);//strtok(NULL, ",");
	char * ToolTip	= va_arg( marker, char *);//strtok(NULL, ",");

	TransLabel *tmp = new TransLabel;
	KToolTip *tltp = new KToolTip;

	tltp->CreateEx(
		tmp->CreateEx(0, BtnName, BtnName, 0, atoi(xPos) + 6, atoi(yPos), 
		atoi(nWidth), atoi(nHeight), m_hWnd, m_hInst, Font, 
		atoi(Fsize), (ULONG)atol(Fcolor)), m_hInst, ToolTip);

	tmp->SetWindowText(Text);
	
	m_Labels.Add(tmp);
	m_ToolTips.Add(tltp);

	//_snprintf(Num, 256, "%d\0", ++i);
	//::GlobalFree(Buffer);

	va_end( marker );  
	delete[] BtnName;
}
