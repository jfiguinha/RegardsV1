

#ifndef __ICONEFILE__H__
#define __ICONEFILE__H__

#include <vector>
#include "attribut.h"
#include <windows.h>

class CIconeFile
{
public:
	CIconeFile(void);
	~CIconeFile(void);

	enum ETYPEFILE{NONE,FILE,ALBUM,CATALOG};

	ETYPEFILE eTypeIconeFile;
	char szKeyName[256];
	long lNumIcone;
	char informations[256];
	char m_stgFullPathName[513];
	char szTimeCreate[32];
	char szSize[20];
	int iNbAttribut;
	char szSignature[256];
	char szDimension[256];

	AttributVector m_AttributVector;

	bool operator == (const CIconeFile &p2) const ;
};

typedef std::vector<CIconeFile> IconeFileVector;

#endif