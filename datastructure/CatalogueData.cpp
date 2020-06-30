#include "stdafx.h"
#include ".\cataloguedata.h"
#include "..\objet.h"
#include <security.h>
#include "..\parameter.h"

CCatalogueData::CCatalogueData(void)
{
	iNbAlbum = 0;
	hItemAlbum = NULL;
	iIconeAlbum = 0;
	iTypeDonnee = 0;
	m_cParameter = new CParameter();
	m_cDefaultDirectory[0] = '\0';
	m_cParameter->GetOptionAlbumDir(m_cDefaultDirectory,MAX_PATH);
}

CCatalogueData::~CCatalogueData(void)
{
	if(m_cParameter != NULL)
		delete m_cParameter;

	m_cParameter = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef SQLSERVERCE

int CCatalogueData::ChargerCatalogue(CAlbumData &m_AlbumTemp,const int &iNumAlbum,const char * m_cFilename)
{
	IStorage* pStg; 
	IStream* pStream;
	char cFile[512];
	char cTemp[512];
	
	wchar_t cFilePath[512];

	//wchar_t cCataloguePath[20];

	char cIntValue[10];

	StringCchPrintf(cFile,512,"%s\\%s",m_cDefaultDirectory,m_cFilename);

	size_t m_sizeTConvert;
	mbstowcs_s(&m_sizeTConvert,cFilePath,512, cFile, 512);

	CSecurity m_cSecurity;
	string m_stgChaine;
	m_cSecurity.HashData((BYTE *)m_AlbumTemp.m_szAlbumName,strlen(m_AlbumTemp.m_szAlbumName),m_stgChaine);
	
	m_stgChaine = m_stgChaine.substr(2,16);

	StringCchCopy(cFile,512,m_stgChaine.substr(2,16).c_str());

	int i = 0, j= 0;

	for(i = 0;i < strlen(cFile);i++)
	{
		if((cFile[i] >= 65 && cFile[i] <= 90) || (cFile[i] >= 48 && cFile[i] <= 57) || (cFile[i] >= 97 && cFile[i] <= 122))
			cTemp[j++] = cFile[i];

	}
	cTemp[j] = '\0';

	wchar_t cCataloguePath[512];

	mbstowcs_s(&m_sizeTConvert,cCataloguePath,512, cTemp, 512);

	HRESULT hr = StgOpenStorage(cFilePath, NULL, STGM_READWRITE|STGM_DIRECT|STGM_SHARE_EXCLUSIVE, NULL, 0, &pStg);

	if(hr == S_OK)
	{
		ULONG pcbWritten;

		hr = pStg->OpenStream(cCataloguePath,NULL, STGM_READWRITE|STGM_DIRECT|STGM_SHARE_EXCLUSIVE, 0, &pStream );

		if(hr == S_OK)
		{

			//m_AlbumDataVector.clear();

			//Sauvegarde de la liste des images
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

			pStream->Release();

		}
		 
		pStream = NULL;
	}

	pStg->Release();
	pStg = NULL;

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

int CCatalogueData::SauverUnCatalogue(CAlbumData &m_AlbumTemp,const int &iNumAlbum,const char * m_cFilename)
{
	IStorage* pStr;
	char cFile[512];
	char cTemp[512];
	ULONG pcbWritten;
	wchar_t cFilePath[512];
	char cIntValue[10];
	AttributVector::const_iterator AttributIt;
	IconeFileVector::const_iterator fit;


	cIntValue[0] = '\0';

	int iNbFiles = 0,i = 0, j = 0;

	StringCchPrintf(cFile,512,"%s\\%s",m_cDefaultDirectory,m_cFilename);

	size_t m_sizeTConvert;
	mbstowcs_s(&m_sizeTConvert,cFilePath,512, cFile, 512);

	CSecurity m_cSecurity;
	string m_stgChaine;
	m_cSecurity.HashData((BYTE *)m_AlbumTemp.m_szAlbumName,strlen(m_AlbumTemp.m_szAlbumName),m_stgChaine);
	
	m_stgChaine = m_stgChaine.substr(2,16);

	StringCchCopy(cFile,512,m_stgChaine.substr(2,16).c_str());

	for(i = 0;i < strlen(cFile);i++)
	{
		if((cFile[i] >= 65 && cFile[i] <= 90) || (cFile[i] >= 48 && cFile[i] <= 57) || (cFile[i] >= 97 && cFile[i] <= 122))
			cTemp[j++] = cFile[i];

	}
	cTemp[j] = '\0';

	wchar_t cCataloguePath[512];

	mbstowcs_s(&m_sizeTConvert,cCataloguePath,512, cTemp, 512);

	IStream* pStream;


	HRESULT hr = StgOpenStorage(cFilePath, NULL, STGM_READWRITE|STGM_DIRECT|STGM_SHARE_EXCLUSIVE, NULL, 0, &pStr);
	
	if(hr != S_OK)
	{
		hr = StgCreateDocfile(cFilePath, STGM_DIRECT|STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE, NULL, &pStr);
		if(hr != S_OK)
		{
			return -1;
		}

		hr = pStr->CreateStream( L"CatalogueData",STGM_DIRECT|STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE,0, 0, &pStream );
		pStream->Release(); 
		pStream = NULL;
	}

	

	hr = pStr->CreateStream(cCataloguePath, STGM_DIRECT|STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE, 0, 0, &pStream );

	if(hr == S_OK)
	{
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

			iNbFiles++;

			
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

		m_AlbumTemp.iNbFiles = iNbFiles;

		pStream->Release(); 
		pStream = NULL;
	}


	pStr->Release();
	pStr = NULL;

	return 0;
}
#endif