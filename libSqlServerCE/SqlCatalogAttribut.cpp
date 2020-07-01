#include "StdAfx.h"
#include "SqlCatalogAttribut.h"

using namespace Regards::Sqlite;

CSqlCatalogAttribut::CSqlCatalogAttribut(void)
	: CSqlExecuteRequest("RegardsDB")
{

}

CSqlCatalogAttribut::~CSqlCatalogAttribut(void)
{
}


/////////////////////////////////////////////////////////////////////////////
//Sauvegardes des attributs pour les albums
/////////////////////////////////////////////////////////////////////////////
HRESULT CSqlCatalogAttribut::SaveCatalogAttribut(AttributVector * attributVector)
{
	for (CAttribut m_AttributData : *attributVector)
	{
		string sql = "INSERT INTO CATALOGATTRIBUT (NumAttribut, Libelle) VALUES (" + to_string(m_AttributData.iNumAttribut); +",'" + string(m_AttributData.cLibelle) + "')";
		ExecuteRequestWithNoResult(sql);
	}
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//Chargement des attributs pour les albums
/////////////////////////////////////////////////////////////////////////////
HRESULT CSqlCatalogAttribut::LoadCatalogAttribut(AttributVector * attributVector)
{
	m_AttributVector = attributVector;
	return ExecuteRequest("SELECT NumAttribut, Libelle FROM CATALOGATTRIBUT");
}

/////////////////////////////////////////////////////////////////////////////
//Suppression de tous les attributs pour les albums
/////////////////////////////////////////////////////////////////////////////
HRESULT CSqlCatalogAttribut::DeleteCatalogAttribut()
{
	return ExecuteRequestWithNoResult("DELETE FROM CATALOGATTRIBUT");
}


//--------------------------------------------------------
//Traitement du Chargement de toutes les données d'un album
//--------------------------------------------------------
int CSqlCatalogAttribut::TraitementResult(CSqlResult * sqlResult)
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
				//listFace.push_back(sqlResult->ColumnDataInt(i));
				strcpy(_AttributData.cLibelle, sqlResult->ColumnDataText(i).c_str());
				break;
			}
		}
		m_AttributVector->push_back(_AttributData);
		nbResult++;
	}
	return nbResult;
}
