// stdafx.h : Fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//


#ifndef __STDAFX__LIBWINDOW__H__
#define __STDAFX__LIBWINDOW__H__


#define WIN32_LEAN_AND_MEAN		// Exclure les en-têtes Windows rarement utilisés

#include <windows.h>
#include <commctrl.h>
#include <windowsx.h>
#include <strsafe.h>
#include <objbase.h>
#ifndef WM_MOUSEWHEEL
#include "zmouse.h"
#endif
#include "..\include\global.h"

extern HINSTANCE hInstance;

// TODO : faites référence ici aux en-têtes supplémentaires nécessaires au programme
#endif