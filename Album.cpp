// Album.cpp: implementation of the CAlbum class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "catalogue1.h"
#include "Album.h"
#include "objet.h"
#include <CatalogueData.h>

#ifdef SQLSERVERCE
#include <SqlAlbum.h>
#include <SqlAlbumUtility.h>
#include <SqlIconeFileAlbum.h>
#include <SqlAlbumIconeFileAttribut.h>
using namespace Regards::Sqlite;

#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAlbum::CAlbum()
{
	LoadAlbum();
	m_CatalogueData->iTypeDonnee = 2;
}

CAlbum::~CAlbum()
{
#ifndef SQLSERVERCE
	SaveAlbum();
#endif
}

int CAlbum::InitCatalogue(HWND hWndTree, CTree * m_cTree)
{
	int iVersion = CObjet::GetWindowsVersionAvance();

	if(iVersion > 50)
	{
		HIMAGELIST m_imgList;
		HBITMAP hBmp;
		m_imgList = (HIMAGELIST)SendMessage(hWndTree, TVM_GETIMAGELIST, TVSIL_NORMAL, 0);
		hBmp = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_ALBUM));
		m_CatalogueData->iIconeAlbum = ImageList_Add(m_imgList,hBmp,NULL);
		m_CatalogueData->iIconeAlbum = ImageList_Add(m_imgList,hBmp,NULL);
		DeleteObject(hBmp);
		SendMessage(hWndTree, TVM_SETIMAGELIST, TVSIL_NORMAL, (LPARAM)m_imgList );
	}
	else
	{
		char Temp[MAX_PATH];
		SHFILEINFO  sfi;

		GetModuleFileName(hInstance,Temp,MAX_PATH);

		//strcat(Temp,",3");
		
		SHGetFileInfo(Temp, 0, &sfi, sizeof(sfi), SHGFI_DISPLAYNAME | SHGFI_SYSICONINDEX);

		//int iIcon = Shell_GetCachedImageIndex(Temp,0,GIL_DONTCACHE);

		m_CatalogueData->iIconeAlbum = sfi.iIcon;
	}

	AddItemToTree(hWndTree, m_cTree, "Album");

	CAlbumData m_AlbumTemp;
	for (AlbumDataVector::const_iterator dit = m_CatalogueData->m_AlbumDataVector.begin(); dit!= m_CatalogueData->m_AlbumDataVector.end(); dit++)
	{
		m_AlbumTemp = *dit;
		if(strcmp(m_AlbumTemp.m_szAlbumName,"") != 0)
		{
			InsertItem(hWndTree,m_AlbumTemp.m_szAlbumName);
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//Suppression d'un album
/////////////////////////////////////////////////////////////////////////////////////////////////

int CAlbum::DeleteAlbum(const char * szAlbumName)
{
#ifdef SQLSERVERCE	


	CSqlAlbumUtility::DeleteAlbum((TCHAR *)szAlbumName);

#endif
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//Détermination du numéro de l'album
/////////////////////////////////////////////////////////////////////////////////////////////////

int CAlbum::DetermineNumAlbum(const char *szDirName)
{
	string Temp = szDirName;
	int iPos;
	long lNumEntree = 0;
	iPos = Temp.find("Album");
	if(iPos != -1 && Temp != "Album")
	{
		Temp = Temp.substr(strlen("Album")+1, Temp.length() - strlen("Album")-1);
		
		for (AlbumDataVector::const_iterator dit = m_CatalogueData->m_AlbumDataVector.begin(); dit!= m_CatalogueData->m_AlbumDataVector.end(); dit++)
		{
			CAlbumData m_AlbumTemp;
			m_AlbumTemp = *dit;
			if(_stricmp(Temp.c_str(),m_AlbumTemp.m_szAlbumName) == 0)
				return lNumEntree;

			lNumEntree++;

		}
	}

	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

int CAlbum::LoadAlbum()
{

#ifdef SQLSERVERCE

	m_CatalogueData->m_AlbumDataVector.clear();
	CSqlAlbum * m_cSqlAlbum = new CSqlAlbum();
	m_cSqlAlbum->LoadAlbum(&m_CatalogueData->m_AlbumDataVector);
	delete m_cSqlAlbum;

#else

	IStorage* pStg; 
	IStream* pStream;
	char cFile[MAX_PATH];
	
	wchar_t cFilePath[MAX_PATH];
	char cIntValue[10];

	StringCchPrintf(cFile,MAX_PATH,"%s\\album.db",m_CatalogueData->m_cDefaultDirectory);

	size_t m_sizeTConvert;
	mbstowcs_s(&m_sizeTConvert,cFilePath,MAX_PATH, cFile, MAX_PATH);

	HRESULT hr = StgOpenStorage(cFilePath, NULL, STGM_READWRITE|STGM_DIRECT|STGM_SHARE_EXCLUSIVE, NULL, 0, &pStg);

	if(hr == S_OK)
	{
		ULONG pcbWritten;

		hr = pStg->OpenStream( L"AlbumData",NULL, STGM_READWRITE|STGM_DIRECT|STGM_SHARE_EXCLUSIVE, 0, &pStream );

		m_CatalogueData->m_AlbumDataVector.clear();
		do
		{

			CAlbumData m_AlbumTemp;

			hr = pStream->Read(m_AlbumTemp.m_szAlbumName, 255, &pcbWritten);
			if(pcbWritten == 0)
				break;

			hr = pStream->Read(cIntValue, sizeof(int), &pcbWritten);
			m_AlbumTemp.iNbFiles = atoi(cIntValue);


			//Chargement de la liste des images
			for (int j=0;j < m_AlbumTemp.iNbFiles;j++)
			{
				CIconeFile IconeFileTemp;

				hr = pStream->Read(cIntValue, sizeof(long), &pcbWritten);
				IconeFileTemp.lNumIcone = atol(cIntValue);
				hr = pStream->Read(IconeFileTemp.informations, 255, &pcbWritten);
				hr = pStream->Read(IconeFileTemp.m_stgFullPathName, 255, &pcbWritten);
				hr = pStream->Read(IconeFileTemp.szSize, 20, &pcbWritten);
				hr = pStream->Read(IconeFileTemp.szTimeCreate, 32, &pcbWritten);
				hr = pStream->Read(cIntValue, sizeof(int), &pcbWritten);
				IconeFileTemp.iNbAttribut = atoi(cIntValue);

				for(int k=0;k < IconeFileTemp.iNbAttribut;k++)
				{
					CAttribut m_AttributTemp;

					hr = pStream->Read(m_AttributTemp.cLibelle, 255, &pcbWritten);
					if(pcbWritten == 0)
						break;

					hr = pStream->Read(cIntValue, sizeof(int), &pcbWritten);
					m_AttributTemp.iNumAttribut = atoi(cIntValue);

					IconeFileTemp.m_AttributVector.push_back(m_AttributTemp);
				}

				m_AlbumTemp.m_IconeFileVector.push_back(IconeFileTemp);
			}

			m_CatalogueData->iNbAlbum++;

			m_CatalogueData->m_AlbumDataVector.push_back(m_AlbumTemp);

		}while(pcbWritten > 0);

		pStream->Release(); 
		pStream = NULL;

		hr = pStg->OpenStream( L"AlbumAttribut",NULL, STGM_READWRITE|STGM_DIRECT|STGM_SHARE_EXCLUSIVE, 0, &pStream );
		m_CatalogueData->m_AttributVector.clear();
		do
		{

			CAttribut m_AttributTemp;

			hr = pStream->Read(m_AttributTemp.cLibelle, 255, &pcbWritten);
			if(pcbWritten == 0)
				break;

			hr = pStream->Read(cIntValue, sizeof(int), &pcbWritten);
			m_AttributTemp.iNumAttribut = atoi(cIntValue);

			m_CatalogueData->m_AttributVector.push_back(m_AttributTemp);

		}while(pcbWritten > 0);

		pStream->Release(); 
		pStg->Release(); 

	}

#endif

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

int CAlbum::SaveAlbum()
{
#ifndef SQLSERVERCE

	IStorage* pStr;
	char cFile[MAX_PATH];
	ULONG pcbWritten;
	CAlbumData m_AlbumTemp;
	wchar_t cFilePath[MAX_PATH];
	char cIntValue[10];
	AttributVector::const_iterator AttributIt;
	AlbumDataVector::const_iterator dit;
	IconeFileVector::const_iterator fit;

	StringCchPrintf(cFile,MAX_PATH,"%s\\album.db",m_CatalogueData->m_cDefaultDirectory);

	size_t m_sizeTConvert;
	mbstowcs_s(&m_sizeTConvert,cFilePath,MAX_PATH, cFile, MAX_PATH);

	HRESULT hr = StgOpenStorage(cFilePath, NULL, STGM_READWRITE|STGM_DIRECT|STGM_SHARE_EXCLUSIVE, NULL, 0, &pStr);
	
	if(hr != S_OK)
	{
		hr = StgCreateDocfile(cFilePath, STGM_DIRECT|STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE, NULL, &pStr);
		if(hr != S_OK)
		{
			return -1;
		}
	}

	IStream* pStream;

	hr = pStr->CreateStream( L"AlbumData", STGM_DIRECT|STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE, 0, 0, &pStream );

	for (dit = m_CatalogueData->m_AlbumDataVector.begin(); dit!= m_CatalogueData->m_AlbumDataVector.end(); dit++)
	{
		//Sauvegarde des données des albums
		
		m_AlbumTemp = *dit;
		hr = pStream->Write(m_AlbumTemp.m_szAlbumName, 255, &pcbWritten);

		//Sauvegarde du nombre d'image disponible dans l'album
		_itoa_s(m_AlbumTemp.iNbFiles,cIntValue,10,10);
		hr = pStream->Write(cIntValue, sizeof(int), &pcbWritten);

		//Sauvegarde de la liste des images
		for (fit = m_AlbumTemp.m_IconeFileVector.begin(); fit!=m_AlbumTemp.m_IconeFileVector.end(); fit++)
		{
			CIconeFile IconeFileTemp = *fit;
			_ltoa_s(IconeFileTemp.lNumIcone,cIntValue,10,10);
			hr = pStream->Write(cIntValue, sizeof(long), &pcbWritten);
			hr = pStream->Write(IconeFileTemp.informations, 255, &pcbWritten);
			hr = pStream->Write(IconeFileTemp.m_stgFullPathName, 255, &pcbWritten);
			hr = pStream->Write(IconeFileTemp.szSize, 20, &pcbWritten);
			hr = pStream->Write(IconeFileTemp.szTimeCreate, 32, &pcbWritten);
			
			//Sauvegarde du nombre d'attribut disponible dans l'image
			_itoa_s(IconeFileTemp.iNbAttribut,cIntValue,10,10);
			hr = pStream->Write(cIntValue, sizeof(int), &pcbWritten);


			for (AttributIt = IconeFileTemp.m_AttributVector.begin(); AttributIt!= IconeFileTemp.m_AttributVector.end(); AttributIt++)
			{
				CAttribut m_AttributTemp;
				m_AttributTemp = *AttributIt;
				hr = pStream->Write(m_AttributTemp.cLibelle, 255, &pcbWritten);

				//Sauvegarde du numero d'entrée de l'album
				_itoa_s(m_AttributTemp.iNumAttribut,cIntValue,10,10);
				hr = pStream->Write(cIntValue, sizeof(int), &pcbWritten);

			}
		}
	}

	pStream->Release(); 
	pStream = NULL;

	hr = pStr->CreateStream( L"AlbumAttribut", STGM_DIRECT|STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE, 0, 0, &pStream );

	for (AttributIt = m_CatalogueData->m_AttributVector.begin(); AttributIt!= m_CatalogueData->m_AttributVector.end(); AttributIt++)
	{
	
		CAttribut m_AttributTemp;
		m_AttributTemp = *AttributIt;
		hr = pStream->Write(m_AttributTemp.cLibelle, 255, &pcbWritten);

		//Sauvegarde du numero d'entrée de l'album
		_itoa_s(m_AttributTemp.iNumAttribut,cIntValue,10,10);
		hr = pStream->Write(cIntValue, sizeof(int), &pcbWritten);

	}

	pStream->Release(); 
	pStr->Release(); 

#endif

	return 0;
}
