// stdafx.h�: fichier Include pour les fichiers Include syst�me standard,
// ou les fichiers Include sp�cifiques aux projets qui sont utilis�s fr�quemment,
// et sont rarement modifi�s
//

#pragma once

#ifndef _WIN32_WINNT		// Autorise l'utilisation des fonctionnalit�s sp�cifiques � Windows�XP ou version ult�rieure.                   
#define _WIN32_WINNT 0x0501	// Attribuez la valeur appropri�e � cet �l�ment pour cibler d'autres versions de Windows.
#endif						

#define WIN32_LEAN_AND_MEAN		// Exclure les en-t�tes Windows rarement utilis�s

//#define GLEW_STATIC 1
#include <Windows.h>
#include <gl/glew.h>

//#include <gl/glaux.h>											// Header File For The GLaux Library
#ifdef GPGPU_CG
	#include <cg/cgGL.h>
#endif

#include "../include/global.h"
// TODO�: faites r�f�rence ici aux en-t�tes suppl�mentaires n�cessaires au programme
