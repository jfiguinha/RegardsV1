

#ifndef __CATALOGUEDATA__H__
#define __CATALOGUEDATA__H__

#include "attribut.h"
#include "iconefile.h"
#include "albumdata.h"

class CParameter;

class CCatalogueData
{
	public:

		CCatalogueData(void);
		~CCatalogueData(void);

#ifndef SQLSERVERCE
		int ChargerCatalogue(CAlbumData &m_AlbumTemp,const int &iNumAlbum,const char * m_cFilename = "catalogue.db");
		int SauverUnCatalogue(CAlbumData &m_AlbumTemp,const int &iNumAlbum,const char * m_cFilename = "catalogue.db");
#endif

		AlbumDataVector m_AlbumDataVector;
		AttributVector m_AttributVector;
		IconeFileVector m_IconeFileRecherche;
		int iNbAlbum;
		HTREEITEM hItemAlbum;
		int iIconeAlbum;
		//char cDirectory[MAX_PATH];
		char m_cDefaultDirectory[MAX_PATH];
		int iTypeDonnee;

		CParameter * m_cParameter;
};


#endif