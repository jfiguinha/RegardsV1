#include "stdafx.h"
#include ".\iconefile.h"

CIconeFile::CIconeFile(void)
{
	informations[0] = '\0';
	m_stgFullPathName[0] = '\0';
	szTimeCreate[0] = '\0';
	szSize[0] = '\0';
	iNbAttribut = 0;
	lNumIcone = -1;
	eTypeIconeFile = ETYPEFILE::NONE;
	szKeyName[0] = '\0';
	szSignature[0] = '\0';
	szDimension[0] = '\0';
}

CIconeFile::~CIconeFile(void)
{
}

bool CIconeFile::operator == (const CIconeFile &p2) const 
{ 
	if(strcmp(m_stgFullPathName,p2.m_stgFullPathName) == 0)
		return true;
	
	return false;
}