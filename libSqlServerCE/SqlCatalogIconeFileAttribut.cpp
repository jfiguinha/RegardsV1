#include "StdAfx.h"
#include "SqlCatalogIconeFileAttribut.h"

using namespace Regards::Sqlite;

CSqlCatalogIconeFileAttribut::CSqlCatalogIconeFileAttribut(void)
	: CSqlExecuteRequest("RegardsDB")
{
}

CSqlCatalogIconeFileAttribut::~CSqlCatalogIconeFileAttribut(void)
{
}
/////////////////////////////////////////////////////////////////
//Sauvegarde des informations sur les attributs
/////////////////////////////////////////////////////////////////
HRESULT CSqlCatalogIconeFileAttribut::SaveCatalogIconeFileAttribut(AttributVector * attributVector, TCHAR * lpCatalogName, int NumFile)
{
	for (CAttribut m_AttributData : *attributVector)
	{
		string sql = "INSERT INTO ALBUMATTRIBUT (CatalogName, NumFile, NumAttribut) VALUES ('" + string(lpCatalogName) + "', " + to_string(NumFile) +"," + to_string(m_AttributData.iNumAttribut) + ")";
		ExecuteRequestWithNoResult(sql);
	}
	return S_OK;
}

/////////////////////////////////////////////////////////////////
//Chargement des informations sur les attributs
/////////////////////////////////////////////////////////////////
HRESULT CSqlCatalogIconeFileAttribut::LoadCatalogIconeFileAttribut(AttributVector * attributVector, TCHAR * lpCatalogName, int NumFile)
{
	m_attributVector = attributVector;
	TCHAR _pwszRequeteSQL[512];
	sprintf(_pwszRequeteSQL,"SELECT AIA.NumAttribut, AA.Libelle FROM CATALOG_ICONEFILE_ATTRIBUT CIA, CATALOGMATTRIBUT CA WHERE CIA.NumAttribut = CA.NumAttribut and AlbumName = '%s' and NumFile = %d",lpCatalogName,NumFile);
	return ExecuteRequest(_pwszRequeteSQL);
}

/////////////////////////////////////////////////////////////////
//Suppression des informations sur les attributs
/////////////////////////////////////////////////////////////////
HRESULT CSqlCatalogIconeFileAttribut::DeleteCatalogIconeFileAttribut(TCHAR * lpCatalogName, int NumFile)
{
	TCHAR _pwszRequeteSQL[512];
	sprintf(_pwszRequeteSQL,"DELETE FROM CATALOG_ICONEFILE_ATTRIBUT WHERE CatalogName = '%s' and NumFile = %d",lpCatalogName,NumFile);
	return ExecuteRequestWithNoResult(_pwszRequeteSQL);
}


//--------------------------------------------------------
//Traitement du Chargement de toutes les données d'un album
//--------------------------------------------------------
int CSqlCatalogIconeFileAttribut::TraitementResult(CSqlResult * sqlResult)
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
		m_attributVector->push_back(_AttributData);
		nbResult++;
	}
	return nbResult;
}
