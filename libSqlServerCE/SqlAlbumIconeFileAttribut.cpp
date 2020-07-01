#include "StdAfx.h"
#include "SqlAlbumIconeFileAttribut.h"

using namespace Regards::Sqlite;

/////////////////////////////////////////////////////////////////
//Constructeur
/////////////////////////////////////////////////////////////////
CSqlAlbumIconeFileAttribut::CSqlAlbumIconeFileAttribut(void)
	: CSqlExecuteRequest("RegardsDB")
{

}

/////////////////////////////////////////////////////////////////
//Destructeur
/////////////////////////////////////////////////////////////////
CSqlAlbumIconeFileAttribut::~CSqlAlbumIconeFileAttribut(void)
{
}

/////////////////////////////////////////////////////////////////
//Sauvegarde des informations sur les attributs
/////////////////////////////////////////////////////////////////
HRESULT CSqlAlbumIconeFileAttribut::SaveAlbumIconeFileAttribut(AttributVector * attributVector, TCHAR * lpAlbumName, TCHAR * lpFileName)
{
	for (CAttribut m_AttributData : *attributVector)
	{
		string sql = "INSERT INTO ALBUM_ICONEFILE_ATTRIBUT (AlbumName,FileName,NumAttribut,ValueAttribut) VALUES ('" + string(lpAlbumName) +"','" + string(lpFileName) + "'," + to_string(m_AttributData.iNumAttribut) + ",'" + string(m_AttributData.cLibelle) + "')";
		ExecuteRequestWithNoResult(sql);
	}
	return S_OK;

}

/////////////////////////////////////////////////////////////////
//Chargement des informations sur les attributs
/////////////////////////////////////////////////////////////////
HRESULT CSqlAlbumIconeFileAttribut::LoadAlbumIconeFileAttribut(AttributVector * attributVector, TCHAR * lpAlbumName, TCHAR * lpFileName)
{
	m_attributVector = attributVector;
	TCHAR _pwszRequeteSQL[512];
	sprintf(_pwszRequeteSQL,"SELECT NumAttribut, ValueAttribut FROM ALBUM_ICONEFILE_ATTRIBUT WHERE AlbumName = '%s' and FileName = '%s'",lpAlbumName,lpFileName);
	return ExecuteRequest(_pwszRequeteSQL);
}

/////////////////////////////////////////////////////////////////
//Suppression des informations sur les attributs
/////////////////////////////////////////////////////////////////
HRESULT CSqlAlbumIconeFileAttribut::DeleteAlbumIconeFileAttribut(TCHAR * lpAlbumName, TCHAR * lpFileName)
{
	TCHAR _pwszRequeteSQL[512];
	sprintf(_pwszRequeteSQL,"DELETE FROM ALBUM_ICONEFILE_ATTRIBUT WHERE AlbumName = '%s' and FileName = '%s'",lpAlbumName,lpFileName);
	return ExecuteRequestWithNoResult(_pwszRequeteSQL);
}

HRESULT CSqlAlbumIconeFileAttribut::DeleteAlbumIconeFileAttribut(TCHAR * lpAlbumName)
{
	TCHAR _pwszRequeteSQL[512];
	sprintf(_pwszRequeteSQL,"DELETE FROM ALBUM_ICONEFILE_ATTRIBUT WHERE AlbumName = '%s'",lpAlbumName);
	return ExecuteRequestWithNoResult(_pwszRequeteSQL);
}

HRESULT CSqlAlbumIconeFileAttribut::DeleteAlbumIconeFileAttribut(int iNumAttribut)
{
	TCHAR _pwszRequeteSQL[512];
	sprintf(_pwszRequeteSQL,"DELETE FROM ALBUM_ICONEFILE_ATTRIBUT WHERE NumAttribut = %d",iNumAttribut);
	return ExecuteRequestWithNoResult(_pwszRequeteSQL);
}

//-------------------------------------------------------------------------------------------------
// Renommage du nom de l'album
//-------------------------------------------------------------------------------------------------
HRESULT CSqlAlbumIconeFileAttribut::UpdateAlbum(TCHAR * lpNewAlbumName,TCHAR * lpOldAlbumName)
{
	TCHAR pwszSQLRequest[255];
	wsprintf(pwszSQLRequest, "UPDATE ALBUM_ICONEFILE_ATTRIBUT SET AlbumName = '%s' WHERE AlbumName = '%s'", lpNewAlbumName, lpOldAlbumName);
	return ExecuteRequestWithNoResult(pwszSQLRequest);
}

//--------------------------------------------------------
//Traitement du Chargement de toutes les données d'un album
//--------------------------------------------------------
int CSqlAlbumIconeFileAttribut::TraitementResult(CSqlResult * sqlResult)
{
	int nbResult = 0;
	while (sqlResult->Next())
	{
		CAttribut _AttributData;
		for (auto i = 0; i < sqlResult->GetColumnCount(); i++)
		{
			
			switch (i)
			{
			case 0: //NumAttribut
				_AttributData.iNumAttribut = sqlResult->ColumnDataInt(i);
				break;
			case 1: //Libelle
				strcpy(_AttributData.cLibelle, sqlResult->ColumnDataText(i).c_str());
				break;
			}
		}
		m_attributVector->push_back(_AttributData);
		nbResult++;
	}
	return nbResult;
}
