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

#if !defined(AFX_STDAFX_H__48C9E972_76B0_11D1_B293_0040053089AE__INCLUDED_)
#define AFX_STDAFX_H__48C9E972_76B0_11D1_B293_0040053089AE__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#ifndef STRICT
#define STRICT
#endif

/*/
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif

#define _WIN32_WINNT 0x0400
#define _ATL_APARTMENT_THREADED
*/

#ifndef ARRAYSIZE
#define ARRAYSIZE(x) (sizeof(x)/sizeof(x[0]))
#endif
#define OEMRESOURCE

#include <windows.h>
//#include <atlbase.h>
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
#include "resource.h"
#include "include\global.h"
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
#include <afxres.h>
#include <comdef.h>


#ifndef WM_MOUSEWHEEL
#include "zmouse.h"
#endif

#ifndef __VECTITEM__H__
#define __VECTITEM__H__
typedef std::vector<int> VECTITEM;
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__48C9E972_76B0_11D1_B293_0040053089AE__INCLUDED)
