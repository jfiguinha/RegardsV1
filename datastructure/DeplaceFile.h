

#ifndef __DEPLACEFILE__H__
#define __DEPLACEFILE__H__

#include "iconefile.h"

class CDeplaceFile
{
public:
	CDeplaceFile(void);
	~CDeplaceFile(void);

	IconeFileVector * m_IconeFile;
	char * cDir;
	int iDepl;
	int iCopierFichier;
	HWND hWndParent;
};

#endif