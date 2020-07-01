// stdafx.h�: fichier Include pour les fichiers Include syst�me standard,
// ou les fichiers Include sp�cifiques aux projets qui sont utilis�s fr�quemment,
// et sont rarement modifi�s
//
		

#define WIN32_LEAN_AND_MEAN		// Exclure les en-t�tes Windows rarement utilis�s
// Fichiers d'en-t�te Windows�:
#include <windows.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <strsafe.h>
#include <objbase.h>
#include <oledberr.h>

// TODO�: faites r�f�rence ici aux en-t�tes suppl�mentaires n�cessaires au programme
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// certains constructeurs CString seront explicites

#include "dbcommon.h"
#include <SqlEngine.h>
#include <SqlResult.h>
#include <string>
#include <vector>
using namespace std;
using namespace Regards::Sqlite;