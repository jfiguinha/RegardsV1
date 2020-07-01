#include "StdAfx.h"
#include "SqlCatalog.h"

using namespace Regards::Sqlite;

CSqlCatalog::CSqlCatalog(void)
	: CSqlExecuteRequest("RegardsDB")
{
	//wcscpy(pwzTableName,L"CATALOG");
	m_AlbumDataVector = NULL;
	m_lpAlbumName = NULL;
}

CSqlCatalog::~CSqlCatalog(void)
{
}


//--------------------------------------------------------
//Suppression de toutes les données d'un album
//--------------------------------------------------------
HRESULT CSqlCatalog::SaveCatalog(AlbumDataVector * albumDataVector)
{
	for (CAlbumData album : *albumDataVector)
	{
		SaveCatalog(album.m_szAlbumName);
	}
	return S_OK;
}

//--------------------------------------------------------
//Suppression de toutes les données d'un album
//--------------------------------------------------------
HRESULT CSqlCatalog::SaveCatalog(TCHAR * lpAlbumName)
{
	string sql = "INSERT INTO CatalogName (CatalogName) VALUES ('" + string(m_lpAlbumName) + "')";
	return ExecuteRequestWithNoResult(sql);
}

//--------------------------------------------------------
//Chargement de toutes les données d'un album
//--------------------------------------------------------
HRESULT CSqlCatalog::LoadCatalog(AlbumDataVector * albumDataVector)
{
	m_AlbumDataVector = albumDataVector;
	return ExecuteRequest("SELECT CatalogName FROM CATALOG");
}

//--------------------------------------------------------
//Suppression de toutes les données d'un album
//--------------------------------------------------------
HRESULT CSqlCatalog::DeleteCatalog()
{
	return ExecuteRequestWithNoResult("DELETE FROM CATALOG");
}

//--------------------------------------------------------
//Traitement du Chargement de toutes les données d'un album
//--------------------------------------------------------
int CSqlCatalog::TraitementResult(CSqlResult * sqlResult)
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
