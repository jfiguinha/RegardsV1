// stdafx.h : Fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//



#ifndef __LIBDIALOG_STDAFX_H__
#define __LIBDIALOG_STDAFX_H__

#define WIN32_LEAN_AND_MEAN		// Exclure les en-têtes Windows rarement utilisés

#include <vector>
#include <windows.h>
#include <shobjidl.h>
#include <commctrl.h>
#include <windowsx.h>
#include <stdio.h>
#include <io.h>
#include <tchar.h>
#include <process.h>
#include <time.h>
#include <Shlwapi.h>
#include <string>
#include <objbase.h>
#include <shlobj.h>
#include <algorithm>
#include <strsafe.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys\stat.H>
#include "global.h"
#include <string>
using namespace std;


const unsigned int cuStackSize = 128 * 1024;

#ifndef __VECTITEM__H__
#define __VECTITEM__H__
typedef std::vector<int> VECTITEM;
#endif

#endif
// TODO : faites référence ici aux en-têtes supplémentaires nécessaires au programme
