// stdafx.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//

#pragma once

#ifndef _WIN32_WINNT		// Autorise l'utilisation des fonctionnalités spécifiques à Windows XP ou version ultérieure.                   
#define _WIN32_WINNT 0x0501	// Attribuez la valeur appropriée à cet élément pour cibler d'autres versions de Windows.
#endif						

#define WIN32_LEAN_AND_MEAN		// Exclure les en-têtes Windows rarement utilisés

//#define GLEW_STATIC 1
#include <Windows.h>
#include <gl/glew.h>

//#include <gl/glaux.h>											// Header File For The GLaux Library
#ifdef GPGPU_CG
	#include <cg/cgGL.h>
#endif

#include "../include/global.h"
// TODO : faites référence ici aux en-têtes supplémentaires nécessaires au programme
