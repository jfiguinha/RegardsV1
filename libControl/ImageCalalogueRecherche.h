

#ifndef __IMAGECATALOGUERECHERCHE__H__
#define __IMAGECATALOGUERECHERCHE__H__

#include <IconeFile.h>

class CCatalogueData;

class CImageCalalogueRecherche
{
public:

	CImageCalalogueRecherche(void);
	~CImageCalalogueRecherche(void);

	int DataComparaison(CIconeFile &m_IconeFile,IconeFileVector &m_IconeFileVectorSrc,IconeFileVector &m_IconeFileVectorDst);
	int TraitementParenthese(int * iResult,int * iOp,int &j,int &i, const int& iNbOp, const int &iNbCritere);
	int GenerateHTMLFile(IconeFileVector &m_IconeFileVectorSrc, char * m_cHTMLFile);
	int ShowHTML(TCHAR * szTemp,const int &iTempSize,const int &iWidth = 800, const int &iHeight = 600);

public:

	int * iTabAlbumSrc;
	int iNbAlbumSrc;
	int * iTabAlbumDest;
	int iNbAlbumDst;
	string m_stgChaineRecherche;
	CCatalogueData * m_CatalogueData;
	//HWND hWnd;

};

#endif