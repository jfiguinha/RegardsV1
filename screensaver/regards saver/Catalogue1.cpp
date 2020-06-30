// Catalogue1.cpp: implementation of the CCatalogue class.
//
//////////////////////////////////////////////////////////////////////

#include "Catalogue1.h"
#include <ocidl.h>
#include <olectl.h>

#include <algorithm>
#include <string>
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCatalogue::CCatalogue(char * Repertoire)
{
	LoadAlbum(Repertoire);
}

CCatalogue::~CCatalogue()
{
}


int CCatalogue::RecupPtAlbum(Album **m_AlbumLocal, const int &iNumAlbum)
{
	*m_AlbumLocal = &m_AlbumVector[iNumAlbum];
	return 0;
}


int CCatalogue::RecupPtVectorAlbum(AlbumVector **m_AlbumVectorLocal)
{
	*m_AlbumVectorLocal = &m_AlbumVector;
	return 0;
}	



int CCatalogue::LoadAlbum(char * Repertoire)
{
	IStorage* pStg; 
	IStream* pStream;
	//IStream* pStreamImageList;
	char cFile[512];
	char cDirectory[MAX_PATH];
	wchar_t cFilePath[512];
	char cIntValue[10];

	//GetCurrentDirectory(MAX_PATH,cDirectory);

	sprintf(cFile,"%s",Repertoire);

	//MessageBox(NULL,cFile,"Infos",0);

	mbstowcs(cFilePath, cFile,  512);

	HRESULT hr = StgOpenStorage(cFilePath, NULL, STGM_READWRITE|STGM_DIRECT|STGM_SHARE_EXCLUSIVE, NULL, 0, &pStg);

	if(hr == S_OK)
	{
		ULONG pcbWritten;

		hr = pStg->OpenStream( L"AlbumData",NULL, STGM_READWRITE|STGM_DIRECT|STGM_SHARE_EXCLUSIVE, 0, &pStream );

		m_AlbumVector.clear();
		do
		{

			Album m_AlbumTemp;

			hr = pStream->Read(m_AlbumTemp.m_szAlbumName, 255, &pcbWritten);
			if(pcbWritten == 0)
				break;

			hr = pStream->Read(cIntValue, sizeof(int), &pcbWritten);
			m_AlbumTemp.iNbFiles = atoi(cIntValue);


			//Sauvegarde de la liste des images
			for (int j=0;j < m_AlbumTemp.iNbFiles;j++)
			{
				IconeFile IconeFileTemp;

				hr = pStream->Read(cIntValue, sizeof(long), &pcbWritten);
				IconeFileTemp.lNumIcone = atol(cIntValue);
				hr = pStream->Read(IconeFileTemp.m_stgFileName, 255, &pcbWritten);
				hr = pStream->Read(IconeFileTemp.m_stgFullPathName, 255, &pcbWritten);
				hr = pStream->Read(IconeFileTemp.szSize, 20, &pcbWritten);
				hr = pStream->Read(IconeFileTemp.szTimeCreate, 32, &pcbWritten);
				hr = pStream->Read(cIntValue, sizeof(int), &pcbWritten);
				IconeFileTemp.iNbAttribut = atoi(cIntValue);

				for(int k=0;k < IconeFileTemp.iNbAttribut;k++)
				{
					Attribut m_AttributTemp;

					hr = pStream->Read(m_AttributTemp.cLibelle, 255, &pcbWritten);
					if(pcbWritten == 0)
						break;

					hr = pStream->Read(cIntValue, sizeof(int), &pcbWritten);
					m_AttributTemp.iNumAttribut = atoi(cIntValue);

					IconeFileTemp.m_AttributVector.push_back(m_AttributTemp);
				}

				m_AlbumTemp.m_IconeFileVector.push_back(IconeFileTemp);
			}

			iNbAlbum++;

			m_AlbumVector.push_back(m_AlbumTemp);

		}while(pcbWritten > 0);

		pStream->Release(); 
		pStream = NULL;

		pStg->Release(); 

	}

	return 0;
}


