#include "StdAfx.h"
#include "SqlFindCatalogIconeFile.h"

using namespace Regards::Sqlite;

CSqlFindCatalogIconeFile::CSqlFindCatalogIconeFile(void)
	: CSqlExecuteRequest("RegardsDB")
{
}

CSqlFindCatalogIconeFile::~CSqlFindCatalogIconeFile(void)
{
}

//-------------------------------------------------------------------------------------------------
// Execution de la requete de recherche de données
//-------------------------------------------------------------------------------------------------
HRESULT CSqlFindCatalogIconeFile::SearchIconeFileCatalog(IconeFileVector * iconeFileVector, TCHAR * lpSQLRequest)
{
	m_iconeFileVector = iconeFileVector;
	TCHAR _pwszRequeteSQL[512];
	sprintf(_pwszRequeteSQL, "SELECT CatalogName, NumIcone, Informations, FullPathName, TimeCreate, Size, NbAttribut FROM ICONEFILECATALOG WHERE %s ", lpSQLRequest);
	return ExecuteRequest(_pwszRequeteSQL);

}



//--------------------------------------------------------
//Traitement du Chargement de toutes les données d'un album
//--------------------------------------------------------
int CSqlFindCatalogIconeFile::TraitementResult(CSqlResult * sqlResult)
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
				//char FullPath[512];
				//strcpy_s(FullPath,(CHAR*)CSqlEngine::UnicodeToAnsi((TCHAR*)(pCol->bData)));
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
			case 6:
				_cIconeFile.iNbAttribut = sqlResult->ColumnDataInt(i);
				break;
			}
		}
		m_iconeFileVector->push_back(_cIconeFile);
		nbResult++;
	}
	return nbResult;
}
