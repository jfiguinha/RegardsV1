#include "StdAfx.h"
#include "SqlFindAlbumIconeFile.h"


using namespace LIBSQLSERVERCE;

CSqlFindAlbumIconeFile::CSqlFindAlbumIconeFile(void)
{
}

CSqlFindAlbumIconeFile::~CSqlFindAlbumIconeFile(void)
{
}

/////////////////////////////////////////////////////////////////
//Chargement des informations sur les attributs
/////////////////////////////////////////////////////////////////
HRESULT CSqlFindAlbumIconeFile::SearchIconeFileAlbum(IconeFileVector * iconeFileVector, WCHAR * lpSQLRequestCriterium, WCHAR * lpSQLRequest, WCHAR * lpSpecialLiaison)
{
	WCHAR *	pwszIconeFileFolder[] =	{L"AlbumName",L"NumIcone",L"Informations",L"FullPathName",L"TimeCreate",L"Size"};
	_iNbElement = sizeof(pwszIconeFileFolder)/sizeof(pwszIconeFileFolder[0]);
	CopyChampsElement(pwszIconeFileFolder);
	m_iconeFileVector = iconeFileVector;
	wcscpy_s(pwzTableName,L"ICONEFILEALBUM");
	WCHAR _pwszRequeteSQL[512];

	if(wcslen(lpSQLRequestCriterium) > 0)
	{
		if(wcslen(lpSQLRequest) > 0)
			swprintf(_pwszRequeteSQL,512,L"SELECT IFA.AlbumName, IFA.NumIcone, IFA.Informations, IFA.FullPathName, IFA.TimeCreate, IFA.Size FROM ICONEFILEALBUM as IFA INNER JOIN ALBUM_ICONEFILE_ATTRIBUT as AIA ON IFA.FullPathName = AIA.FileName WHERE %s %s %s",lpSQLRequestCriterium,lpSpecialLiaison,lpSQLRequest);
		else
			swprintf(_pwszRequeteSQL,512,L"SELECT IFA.AlbumName, IFA.NumIcone, IFA.Informations, IFA.FullPathName, IFA.TimeCreate, IFA.Size FROM ICONEFILEALBUM as IFA INNER JOIN ALBUM_ICONEFILE_ATTRIBUT as AIA ON IFA.FullPathName = AIA.FileName WHERE %s",lpSQLRequestCriterium);
	}
	else
		swprintf(_pwszRequeteSQL,512,L"SELECT AlbumName, NumIcone, Informations, FullPathName, TimeCreate, Size FROM ICONEFILEALBUM WHERE %s",lpSQLRequest);
	return ExecuteRequest(_pwszRequeteSQL);
}

/////////////////////////////////////////////////////////////////
//Chargement des informations sur les attributs
/////////////////////////////////////////////////////////////////
HRESULT CSqlFindAlbumIconeFile::TraitementResult(IRowset * & pIRowset,HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding)
{
	HRESULT hr = S_OK;
	BYTE*pData = (BYTE*)CoTaskMemAlloc(dwOffset);
	DWORD dwBindingSize = _iNbElement;
	DWORD cRowsObtained;

	if (NULL == pData)
	{
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

    // Retrieve a row handle for the row resulting from the seek
    //
    hr = pIRowset->GetNextRows(DB_NULL_HCHAPTER, 0, 1, &cRowsObtained, &prghRows);
	if(FAILED(hr))
	{
		goto Exit;	
	}

	while(DB_S_ENDOFROWSET != hr)
	{
		// Fetch actual data
		//
		CIconeFile _cIconeFile;

		hr = pIRowset->GetData(prghRows[0], hAccessor, pData);
		if (FAILED(hr))
		{
			return hr;
		}

		for (int i = 0; i < dwBindingSize; i++)
		{
			WCHAR * wTemp = NULL;
			COLUMNDATA * pCol = (COLUMNDATA*)(pData + prgBinding[i].obLength);
			if (prgBinding[i].wType == DBTYPE_WSTR)
			{
				wTemp = new WCHAR[(pCol->dwLength/sizeof(WCHAR))+1];
				memset(wTemp, 0, pCol->dwLength);
				memcpy_s(wTemp,pCol->dwLength,pCol->bData,pCol->dwLength);
				wTemp[(pCol->dwLength/sizeof(WCHAR))] = L'\0';

			}
			else // numeric column
			{
				pCol->dwStatus = DBSTATUS_S_ISNULL;
			}

			switch(i)
			{
			case 0:
				strcpy_s(_cIconeFile.szKeyName,(CHAR*)CSqlEngine::UnicodeToAnsi((WCHAR*)(pCol->bData)));
				break;
			case 1:
				_cIconeFile.lNumIcone = *(LONG*)(pCol->bData);
				break;
			case 2:
				strcpy_s(_cIconeFile.informations,(CHAR*)CSqlEngine::UnicodeToAnsi((WCHAR*)(pCol->bData)));
				break;
			case 3:
				strcpy_s(_cIconeFile.m_stgFullPathName,(CHAR*)CSqlEngine::UnicodeToAnsi((WCHAR*)(pCol->bData)));
				break;
			case 4:
				strcpy_s(_cIconeFile.szTimeCreate,(CHAR*)CSqlEngine::UnicodeToAnsi((WCHAR*)(pCol->bData)));
				break;
			case 5:
				strcpy_s(_cIconeFile.szSize,(CHAR*)CSqlEngine::UnicodeToAnsi((WCHAR*)(pCol->bData)));
				break;
			}

			if(wTemp != NULL)
			{
				delete[] wTemp;
				wTemp = NULL;
			}

						
		}
		
		m_iconeFileVector->push_back(_cIconeFile);

		// Release the rowset.
		//
		hr = pIRowset->ReleaseRows(1, prghRows, NULL, NULL, NULL);
		if(FAILED(hr))
		{
			return hr;
		}

		// Fetches next row.
		hr = pIRowset->GetNextRows(DB_NULL_HCHAPTER, 0, 1, &cRowsObtained, &prghRows);

	}

	hr = pIRowset->ReleaseRows(1, prghRows, NULL, NULL, NULL);

Exit:

   // Free data record buffer
    //
	if (pData)
	{
        CoTaskMemFree(pData);
		pData = NULL;
	}


	return hr;

}