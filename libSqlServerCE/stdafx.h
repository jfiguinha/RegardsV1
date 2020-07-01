// stdafx.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//
		

#define WIN32_LEAN_AND_MEAN		// Exclure les en-têtes Windows rarement utilisés
// Fichiers d'en-tête Windows :
#include <windows.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <strsafe.h>
#include <objbase.h>
#include <oledberr.h>

// TODO : faites référence ici aux en-têtes supplémentaires nécessaires au programme
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// certains constructeurs CString seront explicites

#include "dbcommon.h"
#include <SqlEngine.h>
#include <SqlResult.h>
#include <string>
#include <vector>
using namespace std;
using namespace Regards::Sqlite;