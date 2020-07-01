#include "StdAfx.h"
#include "SqlIconeFileAlbum.h"
#include "../loadimage.h"
using namespace Regards::Sqlite;

CSqlIconeFileAlbum::CSqlIconeFileAlbum(void)
	: CSqlExecuteRequest("RegardsDB")
{
}

CSqlIconeFileAlbum::~CSqlIconeFileAlbum(void)
{
}

//-------------------------------------------------------------------------------------------------
// Sauvegarde des données
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileAlbum::SaveIconeFileAlbum(IconeFileVector * iconeFileVector, TCHAR * lpAlbumName)
{
	for (CIconeFile m_IconeFileLocal : *iconeFileVector)
	{
		char temp[20];
		UINT width, height;
		CLoadImage m_cLoadImage;
		m_cLoadImage.GetDimensionImageStatic(m_IconeFileLocal.m_stgFullPathName, &width, &height);
		
		std::string m_stgValue = m_IconeFileLocal.szTimeCreate;
		sprintf_s(temp, 20, "%s%s%s", m_stgValue.substr(6, 4).c_str(), m_stgValue.substr(3, 2).c_str(), m_stgValue.substr(0, 2).c_str());

		string sql = "INSERT INTO ICONEFILEALBUM (AlbumName, NumIcone, Informations, FullPathName, TimeCreate, Size, ImageSize, ImageWith, ImageHeight, ImageDate) VALUES ('";
		sql += string(_pwzAlbumName) + "',";
		sql += to_string(m_IconeFileLocal.lNumIcone) + ",'";
		sql += string(m_IconeFileLocal.informations) + "','";
		sql += string(m_IconeFileLocal.m_stgFullPathName) + "','";
		sql += string(m_IconeFileLocal.szTimeCreate) + "','";
		sql += string(m_IconeFileLocal.szSize) + "',";
		sql += string(m_IconeFileLocal.szSize) + ",";
		sql += to_string(width) + ",";
		sql += to_string(height) + ",";
		sql += string(temp) + ")";
		ExecuteRequestWithNoResult(sql);
	}
	return S_OK;

}

//-------------------------------------------------------------------------------------------------
// Chargement des données
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileAlbum::LoadIconeFileAlbum(IconeFileVector * iconeFileVector, TCHAR * lpAlbumName)
{
	m_iconeFileVector = iconeFileVector;
	TCHAR pwszSQLRequest[255];
	sprintf(pwszSQLRequest,"SELECT AlbumName, NumIcone, Informations, FullPathName, TimeCreate, Size FROM ICONEFILEALBUM WHERE AlbumName = '%s'",lpAlbumName);
	return ExecuteRequest(pwszSQLRequest);

}

//-------------------------------------------------------------------------------------------------
// Chargement de toutes les images
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileAlbum::LoadIconeFileAlbum(IconeFileVector * iconeFileVector)
{
	m_iconeFileVector = iconeFileVector;
	return ExecuteRequest("SELECT AlbumName, NumIcone, Informations, FullPathName, TimeCreate, Size FROM ICONEFILEALBUM");
}

//-------------------------------------------------------------------------------------------------
// Suppression des données
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileAlbum::DeleteIconeFileAlbum(TCHAR * lpAlbumName)
{
	TCHAR pwszSQLRequest[255];
	sprintf(pwszSQLRequest,"DELETE FROM ICONEFILEALBUM WHERE AlbumName = '%s'",lpAlbumName);
	return ExecuteRequestWithNoResult(pwszSQLRequest);
}

//-------------------------------------------------------------------------------------------------
// Suppression d'une entrée
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileAlbum::DeleteIconeFileAlbum(TCHAR * lpAlbumName, TCHAR * lpFilename)
{
	TCHAR pwszSQLRequest[255];
	sprintf(pwszSQLRequest,"DELETE FROM ICONEFILEALBUM WHERE AlbumName = '%s' and FullPathName = '%s'",lpAlbumName, lpFilename);
	return ExecuteRequestWithNoResult(pwszSQLRequest);
}

//-------------------------------------------------------------------------------------------------
// Renommage du nom de l'album
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileAlbum::UpdateAlbum(TCHAR * lpNewAlbumName,TCHAR * lpOldAlbumName)
{
	TCHAR pwszSQLRequest[255];
	sprintf(pwszSQLRequest,"UPDATE ICONEFILEALBUM SET AlbumName = '%s' WHERE AlbumName = '%s'", lpNewAlbumName, lpOldAlbumName);
	return ExecuteRequestWithNoResult(pwszSQLRequest);
}



//--------------------------------------------------------
//Traitement du Chargement de toutes les données d'un album
//--------------------------------------------------------
int CSqlIconeFileAlbum::TraitementResult(CSqlResult * sqlResult)
{
	int nbResult = 0;
	while (sqlResult->Next())
	{
		CIconeFile _cIconeFile;
		for (auto i = 0; i < sqlResult->GetColumnCount(); i++)
		{
			switch (i)
			{
			case 0:
				strcpy_s(_cIconeFile.szKeyName, sqlResult->ColumnDataText(i).c_str());
				break;
			case 1:
				_cIconeFile.lNumIcone = sqlResult->ColumnDataInt(i);
				break;
			case 2:
				strcpy_s(_cIconeFile.informations, sqlResult->ColumnDataText(i).c_str());
				break;
			case 3:
				strcpy_s(_cIconeFile.m_stgFullPathName, sqlResult->ColumnDataText(i).c_str());
				break;
			case 4:
				strcpy_s(_cIconeFile.szTimeCreate, sqlResult->ColumnDataText(i).c_str());
				break;
			case 5:
				strcpy_s(_cIconeFile.szSize, sqlResult->ColumnDataText(i).c_str());
				break;
			}
		}
		m_iconeFileVector->push_back(_cIconeFile);
		nbResult++;
	}
	return nbResult;
}
