// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently
//
// This is a part of the Active Template Library.
// Copyright (C) 1996-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#pragma once

#ifndef STRICT
#define STRICT
#endif

#define OEMRESOURCE

#include <windows.h>
#include <commctrl.h>
#include <windowsx.h>
#include <stdio.h>
#include <io.h>
#include <tchar.h>
#include <wchar.h>
#include <process.h>
#include <time.h>
#include <Shlwapi.h>
#include <objbase.h>
#include <algorithm>
#include <strsafe.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys\stat.H>
#include <shlobj.h>
#include <share.h>
#include "resource.h"
#include "global.h"
#include <vector>
#include <string>
using std::string;  // comment if your compiler doesn't use namespaces

const unsigned int cuStackSize = 128 * 1024;

extern HINSTANCE hInstance;

#include <atlbase.h>

extern CComModule _Module;

#include <objbase.h>


#include <atlcom.h>
#include <atlhost.h>
#include <atlctl.h>
#include <comdef.h>

#ifndef WM_MOUSEWHEEL
#include "zmouse.h"
#endif


#ifndef __VECTITEM__H__
#define __VECTITEM__H__
typedef std::vector<int> VECTITEM;
#endif

#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#elif defined(__APPLE__) && !defined(GLEW_APPLE_GLX)
#include <AGL/agl.h>
#else
#include <GL/glxew.h>
#endif


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

static int log2(int zahl)
{
	int log;
	log = 0;
	if (zahl != 0)
	{
		while ((zahl & 1) == 0)
		{
			zahl >>= 1;
			log++;
		}
	}
	return log;
}

