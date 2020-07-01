#include "StdAfx.h"
#include "SqlIconeFileFolder.h"
#include "SqlExecuteRequest.h"
#include "SqlEngine.h"
#include "SqlLib.h"
#include "dbcommon.h"
using namespace Regards::Sqlite;

CSqlIconeFileFolder::CSqlIconeFileFolder(void)
	: CSqlExecuteRequest("RegardsDB")
{
}

CSqlIconeFileFolder::~CSqlIconeFileFolder(void)
{

}

//-------------------------------------------------------------------------------------------------
// Sauvegarde des données
//-------------------------------------------------------------------------------------------------

HRESULT CSqlIconeFileFolder::SaveIconeFileFolder(IconeFileVector * iconeFileVector, TCHAR * lpFolderName)
{
	for (CIconeFile m_IconeFileLocal : *iconeFileVector)
	{
		string sql = "INSERT INTO ICONEFILEFOLDER (FolderName, NumIcone, Informations, FullPathName, TimeCreate, Size) VALUES ('";
		sql += string(lpFolderName) + "',";
		sql += to_string(m_IconeFileLocal.lNumIcone) + ",'";
		sql += string(m_IconeFileLocal.informations) + "','";
		sql += string(m_IconeFileLocal.m_stgFullPathName) + "','";
		sql += string(m_IconeFileLocal.szTimeCreate) + "','";
		sql += string(m_IconeFileLocal.szSize) + "')";
		ExecuteRequestWithNoResult(sql);
	}
	return S_OK;
}


//-------------------------------------------------------------------------------------------------
// Suppression des données
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileFolder::DeleteIconeFileFolder(TCHAR * lpFolderName)
{
	TCHAR pwszSQLRequest[255];
	sprintf(pwszSQLRequest, "DELETE FROM ICONEFILEFOLDER WHERE FolderName = '%s'",lpFolderName);
	return ExecuteRequestWithNoResult(pwszSQLRequest);
}

//-------------------------------------------------------------------------------------------------
// Chargement des données
//-------------------------------------------------------------------------------------------------

HRESULT CSqlIconeFileFolder::LoadIconeFileFolder(IconeFileVector * iconeFileVector, TCHAR *  lpFolderName)
{
	TCHAR pwszSQLRequest[255];
	m_iconeFileVector = iconeFileVector;
	sprintf(pwszSQLRequest,"SELECT FolderName, NumIcone, Informations, FullPathName, TimeCreate, Size FROM ICONEFILEFOLDER WHERE FolderName = '%s'",lpFolderName);
	return ExecuteRequest(pwszSQLRequest);
}


//--------------------------------------------------------
//Traitement du Chargement de toutes les données d'un album
//--------------------------------------------------------
int CSqlIconeFileFolder::TraitementResult(CSqlResult * sqlResult)
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
