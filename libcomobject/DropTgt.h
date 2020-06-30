/**************************************************************************
   THIS CODE AND INFORMATION IS PROVIDED 'AS IS' WITHOUT WARRANTY OF
   ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
   PARTICULAR PURPOSE.

   Copyright 1999 - 2000 Microsoft Corporation.  All Rights Reserved.
**************************************************************************/

/**************************************************************************

   File:          DropTgt.h

   Description:   

**************************************************************************/

/**************************************************************************
   #include statements
**************************************************************************/

#ifndef __DROPTARGET_H__
#define __DROPTARGET_H__

//#include <windows.h>
//#include <ole2.h>
//#include <shlobj.h>


/**************************************************************************
   class definitions
**************************************************************************/

class CDropTarget : public IDropTarget
{
private:

	ULONG m_cRefCount;  
	BOOL m_fAcceptFmt;
	IDropTargetHelper *m_pDropTargetHelper;
	static HWND g_hwndMain;

	//Utility function to read type of drag from key state
	BOOL QueryDrop(DWORD grfKeyState, LPDWORD pdwEffect);
	void DisplayFileNames(HWND, HGLOBAL);

	LPSTR UnicodeToAnsi(LPCWSTR s);


public:    
   CDropTarget(void);
   ~CDropTarget(void);

   void SetWindow(HWND hWnd);
   static int iCount;

   // IUnknown methods
   STDMETHOD(QueryInterface)(REFIID, LPVOID*);
   STDMETHOD_(ULONG, AddRef)(void);
   STDMETHOD_(ULONG, Release)(void);

   // IDropTarget methods
   STDMETHOD(DragEnter)(LPDATAOBJECT, DWORD, POINTL, LPDWORD);
   STDMETHOD(DragOver)(DWORD, POINTL, LPDWORD);
   STDMETHOD(DragLeave)(void);
   STDMETHOD(Drop)(LPDATAOBJECT, DWORD, POINTL, LPDWORD); 
};

#endif