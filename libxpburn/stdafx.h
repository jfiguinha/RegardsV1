// stdafx.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//

#pragma once

#ifndef _WIN32_WINNT		// Autorise l'utilisation des fonctionnalités spécifiques à Windows XP ou version ultérieure.                   
#define _WIN32_WINNT 0x0501	// Attribuez la valeur appropriée à cet élément pour cibler d'autres versions de Windows.
#endif						

#define WIN32_LEAN_AND_MEAN		// Exclure les en-têtes Windows rarement utilisés

#include <windows.h>
#include <stdio.h>
#include <rpcsal.h>
#include <io.h>
#include <tchar.h>
#include <process.h>
#include <time.h>
#include <Shlwapi.h>
#include <objbase.h>
#include <algorithm>
#include <strsafe.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys\stat.H>
#include <string>
using namespace std;

// TODO : faites référence ici aux en-têtes supplémentaires nécessaires au programme
