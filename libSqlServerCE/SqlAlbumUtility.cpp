#include "StdAfx.h"
#include "SqlAlbumUtility.h"
#include "SqlAlbum.h"
#include "SqlAlbumIconeFileAttribut.h"
#include "SqlIconeFileAlbum.h"
#include "SqlAlbumAttribut.h"
#include <algorithm>

using namespace LIBSQLSERVERCE;

CSqlAlbumUtility::CSqlAlbumUtility(void)
{
}

CSqlAlbumUtility::~CSqlAlbumUtility(void)
{
}

LRESULT CSqlAlbumUtility::SaveAlbum(CAlbumData * pAlbumData)
{
	CSqlAlbum * m_cSqlAlbum = new CSqlAlbum();
	m_cSqlAlbum->SaveAlbum(pAlbumData);
	delete m_cSqlAlbum;
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//Mise à jour des attributs pour les albums
///////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CSqlAlbumUtility::UpdateFileAttribut(AttributVector * attributVectorAlbum, WCHAR * albumName)
{
	//Suppression de l'ensemble des vieux attributs
	CSqlAlbumIconeFileAttribut * m_cSqlAlbumIconeFileAttribut = new CSqlAlbumIconeFileAttribut();
	CSqlIconeFileAlbum * m_cSqlIconeFileAlbum = new CSqlIconeFileAlbum();
	IconeFileVector m_IconeFileFolder;

	m_cSqlAlbumIconeFileAttribut->DeleteAlbumIconeFileAttribut(albumName);
	m_cSqlIconeFileAlbum->LoadIconeFileAlbum(&m_IconeFileFolder,albumName);

	for (IconeFileVector::iterator dit = m_IconeFileFolder.begin(); dit != m_IconeFileFolder.end(); dit++)
	{
		CIconeFile m_cIconeFile = *dit;
		size_t m_sizeTConvert;
		WCHAR m_wFileName[MAX_PATH];
		mbstowcs_s(&m_sizeTConvert,m_wFileName, MAX_PATH, m_cIconeFile.m_stgFullPathName, MAX_PATH);
		m_cSqlAlbumIconeFileAttribut->SaveAlbumIconeFileAttribut(attributVectorAlbum,albumName,m_wFileName);
	}

	delete m_cSqlAlbumIconeFileAttribut;
	delete m_cSqlIconeFileAlbum;
	m_IconeFileFolder.clear();

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//Mise à jour des attributs pour les albums
///////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CSqlAlbumUtility::UpdateCategorie(AttributVector * attributVectorAlbum)
{
	//Comparaison de la liste des attributs avant et aprés
	CSqlAlbumIconeFileAttribut * m_cSqlAlbumIconeFileAttribut = new CSqlAlbumIconeFileAttribut();
	CSqlAlbumAttribut * m_cSqlAlbumAttribut = new CSqlAlbumAttribut();
	AttributVector m_AttributVectorInDatabase;
	AttributVector m_AttributVectorNewItem;

	m_cSqlAlbumAttribut->LoadAlbumAttribut(&m_AttributVectorInDatabase);

	int _NumLast = 0;
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//Vérification des attributs ayant été supprimés
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	for (AttributVector::iterator AttributIt = m_AttributVectorInDatabase.begin(); AttributIt!= m_AttributVectorInDatabase.end(); AttributIt++)
	{
		CAttribut m_AttributDatabaseTemp;
		m_AttributDatabaseTemp = *AttributIt;
		bool m_bFind = false;
		//m_IconeFile = find(m_AttributDatabaseTemp.begin(),m_AttributDatabaseTemp.end(),m_AttributAlbumTemp);

		//Vérification des attributs ayant été ajoutés
		for (AttributVector::iterator AttributItNew = attributVectorAlbum->begin(); AttributItNew!= attributVectorAlbum->end(); AttributItNew++)
		{
			CAttribut m_AttributAlbumTemp;
			m_AttributAlbumTemp = *AttributItNew;		

			if(strcmp(m_AttributDatabaseTemp.cLibelle,m_AttributAlbumTemp.cLibelle) == 0)
			{
				m_bFind = true;
				break;
			}
		}

		_NumLast = m_AttributDatabaseTemp.iNumAttribut;

		if(!m_bFind)
		{
			m_cSqlAlbumAttribut->DeleteAlbumAttribut(m_AttributDatabaseTemp.iNumAttribut);
			m_cSqlAlbumIconeFileAttribut->DeleteAlbumIconeFileAttribut(m_AttributDatabaseTemp.iNumAttribut);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//On vérifie les éléments ayant été ajoutés
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	for (AttributVector::iterator AttributIt = attributVectorAlbum->begin(); AttributIt!= attributVectorAlbum->end(); AttributIt++)
	{
		CAttribut m_AttributAlbumTemp;
		m_AttributAlbumTemp = *AttributIt;
		bool m_bFind = false;

		for (AttributVector::iterator AttributItNew = m_AttributVectorInDatabase.begin(); AttributItNew!= m_AttributVectorInDatabase.end(); AttributItNew++)
		{
			CAttribut m_AttributDatabaseTemp;
			m_AttributDatabaseTemp = *AttributItNew;

			if(strcmp(m_AttributDatabaseTemp.cLibelle,m_AttributAlbumTemp.cLibelle) == 0)
			{
				m_bFind = true;
				break;
			}
		}

		if(!m_bFind)
			m_AttributVectorNewItem.push_back(m_AttributAlbumTemp);
	}
	
	if(m_AttributVectorNewItem.size() > 0)
		m_cSqlAlbumAttribut->SaveAlbumAttribut(&m_AttributVectorNewItem);

	delete m_cSqlAlbumIconeFileAttribut;
	delete m_cSqlAlbumAttribut;

	m_AttributVectorNewItem.clear();
	m_AttributVectorInDatabase.clear();

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//Suppression d'un album
///////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CSqlAlbumUtility::DeleteAlbum(WCHAR * albumName)
{
	//On supprime toutes les infos pour cette album

	CSqlAlbum * m_cSqlAlbum = new CSqlAlbum();
	CSqlAlbumIconeFileAttribut * m_cSqlAlbumIconeFileAttribut = new CSqlAlbumIconeFileAttribut();
	CSqlIconeFileAlbum * m_cSqlIconeFileAlbum = new CSqlIconeFileAlbum();
		
	m_cSqlAlbum->DeleteAlbum(albumName);
	m_cSqlAlbumIconeFileAttribut->DeleteAlbumIconeFileAttribut(albumName);
	m_cSqlIconeFileAlbum->DeleteIconeFileAlbum(albumName);
		
	delete m_cSqlAlbum;
	delete m_cSqlAlbumIconeFileAttribut;
	delete m_cSqlIconeFileAlbum;

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//Renommage d'un album
///////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CSqlAlbumUtility::RenameAlbum(WCHAR * albumOldName,WCHAR * albumNewName)
{
	//On met à jour toutes les tables avec le nouveau nom
	CSqlAlbum * m_cSqlAlbum = new CSqlAlbum();
	CSqlIconeFileAlbum * m_cSqlIconeFileAlbum = new CSqlIconeFileAlbum();
	CSqlAlbumIconeFileAttribut * m_cSqlAlbumIconeFileAttribut = new CSqlAlbumIconeFileAttribut();

	m_cSqlAlbum->UpdateAlbum(albumNewName,albumOldName);
	m_cSqlAlbumIconeFileAttribut->UpdateAlbum(albumNewName,albumOldName);
	m_cSqlIconeFileAlbum->UpdateAlbum(albumNewName,albumOldName);
	
	delete m_cSqlAlbum;
	delete m_cSqlIconeFileAlbum;
	delete m_cSqlAlbumIconeFileAttribut;
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//Mises à jour dans la base de données des catégories
///////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CSqlAlbumUtility::UpdateFileAlbum(IconeFileVector * iconeFileVector, WCHAR * albumName)
{
	IconeFileVector m_IconeFileAdd;
	IconeFileVector m_AlbumIconeFile;
	size_t m_sizeTConvert;	
	CSqlIconeFileAlbum * m_cSqlIconeFileAlbum = new CSqlIconeFileAlbum();
	CSqlAlbumIconeFileAttribut * m_SqlAlbumIconeFileAttribut = new CSqlAlbumIconeFileAttribut();
	m_cSqlIconeFileAlbum->LoadIconeFileAlbum(&m_AlbumIconeFile,albumName);

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//Vérification des attributs ayant été supprimés
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	for (IconeFileVector::iterator IconeFileIt = m_AlbumIconeFile.begin(); IconeFileIt!= m_AlbumIconeFile.end(); IconeFileIt++)
	{
		CIconeFile m_IconeFile;
		m_IconeFile = *IconeFileIt;

		IconeFileVector::iterator m_IconeFileFind = find(iconeFileVector->begin(),iconeFileVector->end(),m_IconeFile);
		if(m_IconeFileFind == iconeFileVector->end())
		{
			//L'image a été supprimé 
			//On supprime toutes les références
			WCHAR m_wFileName[MAX_PATH];
			mbstowcs_s(&m_sizeTConvert,m_wFileName, MAX_PATH, m_IconeFile.m_stgFullPathName, MAX_PATH);
			m_cSqlIconeFileAlbum->DeleteIconeFileAlbum(albumName,m_wFileName);
			m_SqlAlbumIconeFileAttribut->DeleteAlbumIconeFileAttribut(albumName,m_wFileName);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//On vérifie les éléments ayant été ajoutés
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	for (IconeFileVector::iterator IconeFileIt = iconeFileVector->begin(); IconeFileIt!= iconeFileVector->end(); IconeFileIt++)
	{
		CIconeFile m_IconeFile;
		m_IconeFile = *IconeFileIt;

		IconeFileVector::iterator m_IconeFileFind = find(m_AlbumIconeFile.begin(),m_AlbumIconeFile.end(),m_IconeFile);
		if(m_IconeFileFind == m_AlbumIconeFile.end())
			m_IconeFileAdd.push_back(m_IconeFile);
	}
	
	if(m_IconeFileAdd.size() > 0)
			m_cSqlIconeFileAlbum->SaveIconeFileAlbum(&m_IconeFileAdd,albumName);
	
	delete m_cSqlIconeFileAlbum;
	delete m_SqlAlbumIconeFileAttribut;

	m_IconeFileAdd.clear();
	m_AlbumIconeFile.clear();

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//Suppression d'un fichier de l'album
///////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CSqlAlbumUtility::DeleteAlbumFile(WCHAR * albumName,WCHAR * fileName)
{
	CSqlAlbumIconeFileAttribut * m_SqlAlbumIconeFileAttribut = new CSqlAlbumIconeFileAttribut();
	CSqlIconeFileAlbum * m_SqlIconeFileAlbum = new CSqlIconeFileAlbum();
	
	m_SqlAlbumIconeFileAttribut->DeleteAlbumIconeFileAttribut(albumName,fileName);
	m_SqlIconeFileAlbum->DeleteIconeFileAlbum(albumName,fileName);
	
	delete m_SqlAlbumIconeFileAttribut;
	delete m_SqlIconeFileAlbum;
	return S_OK;
}