#include "StdAfx.h"
#include "SqlAlbum.h"

using namespace Regards::Sqlite;

CSqlAlbum::CSqlAlbum(void)
	: CSqlExecuteRequest("RegardsDB")
{
	//wcscpy(pwzTableName,L"ALBUM");
}

CSqlAlbum::~CSqlAlbum(void)
{
}

//--------------------------------------------------------
//Suppression de toutes les données d'un album
//--------------------------------------------------------
HRESULT CSqlAlbum::SaveAlbum(CAlbumData * pAlbumData)
{
	string sql = "INSERT INTO ALBUM (AlbumName) VALUES ('" + string(pAlbumData->m_szAlbumName) + "')";
	return ExecuteRequestWithNoResult(sql);
}

//--------------------------------------------------------
//Chargement de toutes les données d'un album
//--------------------------------------------------------
HRESULT CSqlAlbum::LoadAlbum(AlbumDataVector * albumDataVector)
{
	m_AlbumDataVector = albumDataVector;
	return ExecuteRequest("SELECT AlbumName FROM ALBUM");
}

//--------------------------------------------------------
//Suppression de toutes les données d'un album
//--------------------------------------------------------
HRESULT CSqlAlbum::DeleteAlbum()
{
	return ExecuteRequestWithNoResult("DELETE FROM ALBUM");
}

//--------------------------------------------------------
//Suppression de toutes les données d'un album
//--------------------------------------------------------
HRESULT CSqlAlbum::DeleteAlbum(TCHAR * lpAlbumName)
{
	TCHAR pwszSQLRequest[255];
	wsprintf(pwszSQLRequest,"DELETE FROM ALBUM WHERE AlbumName = '%s'",lpAlbumName);
	return ExecuteRequestWithNoResult(pwszSQLRequest);
}

//--------------------------------------------------------
//Mise à jour du nom d'un album
//--------------------------------------------------------
HRESULT CSqlAlbum::UpdateAlbum(TCHAR * lpNewAlbumName,TCHAR * lpOldAlbumName)
{
	TCHAR pwszSQLRequest[255];
	wsprintf(pwszSQLRequest,"UPDATE ALBUM SET AlbumName = '%s' WHERE AlbumName = '%s'", lpNewAlbumName, lpOldAlbumName);
	return ExecuteRequestWithNoResult(pwszSQLRequest);
}

//--------------------------------------------------------
//Traitement du Chargement de toutes les données d'un album
//--------------------------------------------------------
int CSqlAlbum::TraitementResult(CSqlResult * sqlResult)
{
	int nbResult = 0;
	while (sqlResult->Next())
	{
		CAlbumData _AlbumData;
		for (auto i = 0; i < sqlResult->GetColumnCount(); i++)
		{
			
			switch (i)
			{
			case 0:
				//listFace.push_back(sqlResult->ColumnDataInt(i));
				strcpy(_AlbumData.m_szAlbumName, sqlResult->ColumnDataText(i).c_str());
				break;
			}
		}
		m_AlbumDataVector->push_back(_AlbumData);
		nbResult++;
	}
	return nbResult;
}
