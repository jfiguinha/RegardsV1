// stdafx.h�: fichier Include pour les fichiers Include syst�me standard,
// ou les fichiers Include sp�cifiques aux projets qui sont utilis�s fr�quemment,
// et sont rarement modifi�s
//



#ifndef _WIN32_WINNT		// Autorise l'utilisation des fonctionnalit�s sp�cifiques � Windows�XP ou version ult�rieure.                   
#define _WIN32_WINNT 0x0501	// Attribuez la valeur appropri�e � cet �l�ment pour cibler d'autres versions de Windows.
#endif						

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
#include <ssceoledb30.h>
// TODO�: faites r�f�rence ici aux en-t�tes suppl�mentaires n�cessaires au programme
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// certains constructeurs CString seront explicites

#include <atlbase.h>
#include <atlstr.h>

#include "dbcommon.h"
#include "SqlEngine.h"