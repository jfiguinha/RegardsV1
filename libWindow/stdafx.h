// stdafx.h : Fichier Include pour les fichiers Include syst�me standard,
// ou les fichiers Include sp�cifiques aux projets qui sont utilis�s fr�quemment,
// et sont rarement modifi�s
//


#ifndef __STDAFX__LIBWINDOW__H__
#define __STDAFX__LIBWINDOW__H__


#define WIN32_LEAN_AND_MEAN		// Exclure les en-t�tes Windows rarement utilis�s

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

// TODO : faites r�f�rence ici aux en-t�tes suppl�mentaires n�cessaires au programme
#endif