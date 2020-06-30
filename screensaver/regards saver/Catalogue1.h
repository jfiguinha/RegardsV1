// Catalogue1.h: interface for the CCatalogue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CATALOGUE1_H__FDE0F235_E7E9_4D5A_BF3E_E72800B423E9__INCLUDED_)
#define AFX_CATALOGUE1_H__FDE0F235_E7E9_4D5A_BF3E_E72800B423E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

class CCatalogue  
{
public:

	typedef struct Attribut
	{
		int iNumAttribut;
		char cLibelle[255];
	}Attribut;

	typedef std::vector<Attribut> AttributVector;

	typedef struct IconeFile
	{
		long lNumIcone;
		char m_stgFileName[512];
		char m_stgFullPathName[512];
		char szTimeCreate[32];
		char szSize[20];
		int iNbAttribut;
		AttributVector m_AttributVector;
	}IconeFile;

	typedef std::vector<IconeFile> IconeFileVector;

	typedef struct Album
	{
		char m_szAlbumName[255];
		int iNbFiles;
		IconeFileVector m_IconeFileVector;
	}Album;

	typedef std::vector<Album> AlbumVector;

	int RecupPtVectorAlbum(AlbumVector **m_AlbumVectorLocal);
	int RecupPtAlbum(Album * * m_AlbumLocal,const int &iNumAlbum);

	CCatalogue(char * Repertoire);
	virtual ~CCatalogue();

private:

	AlbumVector m_AlbumVector;
	int iNbAlbum;
	int LoadAlbum(char * Repertoire);


};

#endif // !defined(AFX_CATALOGUE1_H__FDE0F235_E7E9_4D5A_BF3E_E72800B423E9__INCLUDED_)
