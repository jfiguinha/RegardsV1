#ifndef __STDAFX_FILTREEFFET_H__
#define __STDAFX_FILTREEFFET_H__

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#ifndef STRICT
#define STRICT
#endif

#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>

#include <windows.h>
#include <commctrl.h>
#include <windowsx.h>

#include <math.h>
#include <omp.h>


#include <stdlib.h>
#include <stdio.h>
#include <strsafe.h>
#include <global.h>
#include "resource.h"


#include <vector>
using namespace std;

#endif