#include "StdAfx.h"
#include "SqlAlbumAttribut.h"
#include "dbcommon.h"
#include "sqlengine.h"

using namespace Regards::Sqlite;

CSqlAlbumAttribut::CSqlAlbumAttribut(void)
	: CSqlExecuteRequest("RegardsDB")
{
	//wcscpy(pwzTableName,L"ALBUMATTRIBUT");
}

CSqlAlbumAttribut::~CSqlAlbumAttribut(void)
{
}

/////////////////////////////////////////////////////////////////////////////
//Sauvegardes des attributs pour les albums
/////////////////////////////////////////////////////////////////////////////
HRESULT CSqlAlbumAttribut::SaveAlbumAttribut(AttributVector * attributVector)
{
	for (CAttribut m_AttributData : *attributVector)
	{
		string sql = "INSERT INTO ALBUMATTRIBUT (NumAttribut, Libelle) VALUES (" + to_string(m_AttributData.iNumAttribut); + ",'" + string(m_AttributData.cLibelle) + "')";
		ExecuteRequestWithNoResult(sql);
	}
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//Chargement des attributs pour les albums
/////////////////////////////////////////////////////////////////////////////
HRESULT CSqlAlbumAttribut::LoadAlbumAttribut(AttributVector * attributVector)
{
	m_AttributVector = attributVector;
	return ExecuteRequest("SELECT NumAttribut, Libelle FROM ALBUMATTRIBUT");
}

/////////////////////////////////////////////////////////////////////////////
//Suppression de tous les attributs pour les albums
/////////////////////////////////////////////////////////////////////////////
HRESULT CSqlAlbumAttribut::DeleteAlbumAttribut()
{
	return ExecuteRequestWithNoResult("DELETE FROM ALBUMATTRIBUT");
}

/////////////////////////////////////////////////////////////////////////////
//Suppression de tous les attributs pour les albums
/////////////////////////////////////////////////////////////////////////////
HRESULT CSqlAlbumAttribut::DeleteAlbumAttribut(int iNumAttribut)
{
	TCHAR _pwszRequeteSQL[512];
	sprintf(_pwszRequeteSQL,"DELETE ALBUMATTRIBUT WHERE NumAttribut = %d",iNumAttribut);
	return ExecuteRequestWithNoResult(_pwszRequeteSQL);
}

//--------------------------------------------------------
//Traitement du Chargement de toutes les données d'un album
//--------------------------------------------------------
int CSqlAlbumAttribut::TraitementResult(CSqlResult * sqlResult)
{
	int nbResult = 0;
	while (sqlResult->Next())
	{
		CAttribut _AttributData;
		for (auto i = 0; i < sqlResult->GetColumnCount(); i++)
		{
			
			switch (i)
			{
			case 0:
				//listFace.push_back(sqlResult->ColumnDataInt(i));
				_AttributData.iNumAttribut = sqlResult->ColumnDataInt(i);
				break;

			case 1:
				strcpy(_AttributData.cLibelle, sqlResult->ColumnDataText(i).c_str());
				break;
			}
		}
		m_AttributVector->push_back(_AttributData);
		nbResult++;
	}
	return nbResult;
}

