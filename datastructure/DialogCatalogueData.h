

#ifndef __DIALOGCATALOGUEDATA__H__
#define __DIALOGCATALOGUEDATA__H__

#include "attribut.h"
#include "iconefile.h"
#include "albumdata.h"

class CDialogCatalogueData
{
public:
	CDialogCatalogueData(void);
	~CDialogCatalogueData(void);

	AttributVector * m_AttributVector;
	AttributVector * m_AttributVectorPicture;
	CIconeFile * m_IconeFile;
	CAlbumData * m_Album;
	IconeFileVector * m_IconeFileTemp;
};

#endif