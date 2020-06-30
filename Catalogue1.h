// Catalogue1.h: interface for the CCatalogue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CATALOGUE1_H__FDE0F235_E7E9_4D5A_BF3E_E72800B423E9__INCLUDED_)
#define AFX_CATALOGUE1_H__FDE0F235_E7E9_4D5A_BF3E_E72800B423E9__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include <IconeFile.h>
#include <AlbumData.h>

class CTree;
class CCatalogueData;

class CCatalogue  
{
public:

	//Fonction de recherche d'image
	int RechercheImage(IconeFileVector * * m_IconeFileRecherche, const char * szStringToFind, HWND hWndPrgBar);

	//Modification du nom d'un album
	int NewAlbumName(const char * cNewName, const int &iNumAlbum);

	//Suppression d'un album
	virtual int SuppAlbum(HWND hWndTree, CTree * m_cTree, const int &iNumAlbum);

	//Initialisation du catalogue
	virtual int InitCatalogue(HWND hWndTree, CTree * m_cTree){return 0;};

#ifndef SQLSERVERCE
	//Fonction de trie
	int TrieVecteur(const int &iNumAlbum);
#endif

	//Fonctions d'ajout de nouveaux albums
	int AddAlbum(HWND hWndTree, CTree * m_cTree, const char *szName);
	int AddAlbum(HWND hWndTree, CTree * m_cTree, const CAlbumData &m_AlbumLocal);

	//Fonctions de récupération de pointeurs sur les données
	AlbumDataVector * RecupPtVectorAlbum();
	virtual CAlbumData * RecupPtAlbum(const int &iNumAlbum);
#ifndef SQLSERVERCE
	int RecupPtImage(CIconeFile **m_IconeFile, const int &m_iNumImage, const int &m_iNumAlbum);
#endif
	AttributVector * RecupPtAttribut();

#ifndef SQLSERVERCE
	//Récupération du numéro d'une image à partir de son nom
	int GetNumImage(const int &iNumAlbum, const char *szFileName);
#endif

	//Suppression d'une image
	int RemoveImage(const int &iNumAlbum, const int &iNumImage);

#ifndef SQLSERVERCE
	//Ajout d'une image à un album
	int AddImageToAlbum(const int &iNumAlbum, CIconeFile &m_IconeFile);
#endif
	//Vérification du nom d'un album
	int VerifNomAlbum(const char * szAlbumName);

#ifndef SQLSERVERCE
	//Suppression du fichier des albums
	int DeleteAlbumFile(const int &iNumAlbum, HWND &hWnd,VECTITEM * m_vectItem);
#endif

	int AddItemToTree(HWND hWndTree, CTree *m_cTree, const char * m_szLibelle);

	HTREEITEM InsertItem(HWND hwndParent, const char * lpszFileName);

	virtual int SaveAlbum(){return 0;};
	virtual int LoadAlbum(){return 0;};

	CCatalogue();
	virtual ~CCatalogue();

protected:

	int ParcoursString(int &iNum, char * cValue, int iSize, string &szFind);
	int FindCritere(IconeFileVector &m_IconeFileRecherche,const IconeFileVector &m_IconeFile, const int &iNumCritere, const char * cTabAttribut,int &iNbFileFind);
	int FindCritereFichier(IconeFileVector &m_IconeFileRecherche,const IconeFileVector &m_IconeFile, const int &iNumCritere, const char * cTabAttribut, int &iNbFileFind);

	int ConstructionRequeteSQL( const char *szStringToFind, string &stgRequeteSQLCriteria, string &stgRequeteSQL, int &iNumFormat);

	static bool asc_alpha_Image_sort(const CIconeFile &a, const CIconeFile &b);

	CCatalogueData * m_CatalogueData;

};

#endif // !defined(AFX_CATALOGUE1_H__FDE0F235_E7E9_4D5A_BF3E_E72800B423E9__INCLUDED_)
