#include "StdAfx.h"
#include "SqlFindAlbumIconeFile.h"
using namespace Regards::Sqlite;
CSqlFindAlbumIconeFile::CSqlFindAlbumIconeFile(void)
	: CSqlExecuteRequest("RegardsDB")
{
}

CSqlFindAlbumIconeFile::~CSqlFindAlbumIconeFile(void)
{
}

/////////////////////////////////////////////////////////////////
//Chargement des informations sur les attributs
/////////////////////////////////////////////////////////////////
HRESULT CSqlFindAlbumIconeFile::SearchIconeFileAlbum(IconeFileVector * iconeFileVector, TCHAR * lpSQLRequestCriterium, TCHAR * lpSQLRequest, TCHAR * lpSpecialLiaison)
{
	m_iconeFileVector = iconeFileVector;
	TCHAR _pwszRequeteSQL[512];

	if(strlen(lpSQLRequestCriterium) > 0)
	{
		if(strlen(lpSQLRequest) > 0)
			sprintf(_pwszRequeteSQL,"SELECT IFA.AlbumName, IFA.NumIcone, IFA.Informations, IFA.FullPathName, IFA.TimeCreate, IFA.Size FROM ICONEFILEALBUM as IFA INNER JOIN ALBUM_ICONEFILE_ATTRIBUT as AIA ON IFA.FullPathName = AIA.FileName WHERE %s %s %s",lpSQLRequestCriterium,lpSpecialLiaison,lpSQLRequest);
		else
			sprintf(_pwszRequeteSQL,"SELECT IFA.AlbumName, IFA.NumIcone, IFA.Informations, IFA.FullPathName, IFA.TimeCreate, IFA.Size FROM ICONEFILEALBUM as IFA INNER JOIN ALBUM_ICONEFILE_ATTRIBUT as AIA ON IFA.FullPathName = AIA.FileName WHERE %s",lpSQLRequestCriterium);
	}
	else
		sprintf(_pwszRequeteSQL,"SELECT AlbumName, NumIcone, Informations, FullPathName, TimeCreate, Size FROM ICONEFILEALBUM WHERE %s",lpSQLRequest);
	return ExecuteRequest(_pwszRequeteSQL);
}


//--------------------------------------------------------
//Traitement du Chargement de toutes les données d'un album
//--------------------------------------------------------
int CSqlFindAlbumIconeFile::TraitementResult(CSqlResult * sqlResult)
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
