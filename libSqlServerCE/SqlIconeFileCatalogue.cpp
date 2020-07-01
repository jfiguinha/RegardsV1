#include "StdAfx.h"
#include "SqlIconeFileCatalogue.h"
using namespace Regards::Sqlite;

CSqlIconeFileCatalogue::CSqlIconeFileCatalogue(void)
	: CSqlExecuteRequest("RegardsDB")
{
}

CSqlIconeFileCatalogue::~CSqlIconeFileCatalogue(void)
{
}

//-------------------------------------------------------------------------------------------------
// Sauvegarde des données
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileCatalogue::SaveIconeFileCatalog(IconeFileVector * iconeFileVector, TCHAR * lpCatalogName)
{
	for (CIconeFile m_IconeFileLocal : *iconeFileVector)
	{
		string sql = "INSERT INTO ICONEFILECATALOG (CatalogName, NumIcone, Informations, FullPathName, TimeCreate, Size) VALUES ('";
		sql += string(lpCatalogName) + "',";
		sql += to_string(m_IconeFileLocal.lNumIcone) + ",'";
		sql += string(m_IconeFileLocal.informations) + "','";
		sql += string(m_IconeFileLocal.m_stgFullPathName) + "','";
		sql += string(m_IconeFileLocal.szTimeCreate) + "','";
		sql += string(m_IconeFileLocal.szSize) + ")";
		ExecuteRequestWithNoResult(sql);
	}
	return S_OK;

}

//-------------------------------------------------------------------------------------------------
// Chargement des données
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileCatalogue::LoadIconeFileCatalog(IconeFileVector * iconeFileVector, TCHAR * lpCatalogName)
{
	TCHAR pwszSQLRequest[255];
	m_iconeFileVector = iconeFileVector;
	
	sprintf(pwszSQLRequest,"SELECT CatalogName, NumIcone, Informations, FullPathName, TimeCreate, Size FROM ICONEFILECATALOG WHERE CatalogName = '%s'",lpCatalogName);
	return ExecuteRequest(pwszSQLRequest);
}

//-------------------------------------------------------------------------------------------------
// Suppression des données
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileCatalogue::DeleteIconeFileCatalog(TCHAR * lpCatalogName)
{
	TCHAR pwszSQLRequest[255];
	sprintf(pwszSQLRequest,"DELETE FROM ICONEFILECATALOG WHERE CatalogName = '%s'",lpCatalogName);
	return ExecuteRequestWithNoResult(pwszSQLRequest);
}


//--------------------------------------------------------
//Traitement du Chargement de toutes les données d'un album
//--------------------------------------------------------
int CSqlIconeFileCatalogue::TraitementResult(CSqlResult * sqlResult)
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


