#include "StdAfx.h"
#include "SqlIconeFileCatalogue.h"

using namespace LIBSQLSERVERCE;

CSqlIconeFileCatalogue::CSqlIconeFileCatalogue(void)
{
}

CSqlIconeFileCatalogue::~CSqlIconeFileCatalogue(void)
{
}

//-------------------------------------------------------------------------------------------------
// Sauvegarde des données
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileCatalogue::SaveIconeFileCatalog(IconeFileVector * iconeFileVector, WCHAR * lpCatalogName)
{
	WCHAR *	pwszIconeFileFolder[] =	{L"CatalogName",L"NumIcone",L"Informations",L"FullPathName",L"TimeCreate",L"Size"};
	_iNbElement = sizeof(pwszIconeFileFolder)/sizeof(pwszIconeFileFolder[0]);
	CopyChampsElement(pwszIconeFileFolder);
	m_iconeFileVector = iconeFileVector;
	wcscpy_s(pwzTableName,L"ICONEFILECATALOG");
	wcscpy_s(_pwzCatalogName,lpCatalogName);
	return ExecuteInsert();
}

//-------------------------------------------------------------------------------------------------
// Chargement des données
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileCatalogue::LoadIconeFileCatalog(IconeFileVector * iconeFileVector, WCHAR * lpCatalogName)
{
	WCHAR pwszSQLRequest[255];
	WCHAR *	pwszIconeFileFolder[] =	{L"CatalogName",L"NumIcone",L"Informations",L"FullPathName",L"TimeCreate",L"Size"};
	_iNbElement = sizeof(pwszIconeFileFolder)/sizeof(pwszIconeFileFolder[0]);

	CopyChampsElement(pwszIconeFileFolder);
	m_iconeFileVector = iconeFileVector;

	wcscpy_s(pwzTableName,L"ICONEFILECATALOG");
	wcscpy_s(_pwzCatalogName,lpCatalogName);

	
	wsprintf(pwszSQLRequest,L"SELECT CatalogName, NumIcone, Informations, FullPathName, TimeCreate, Size FROM ICONEFILECATALOG WHERE CatalogName = '%s'",lpCatalogName);

	return ExecuteRequest(pwszSQLRequest);
}

//-------------------------------------------------------------------------------------------------
// Suppression des données
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileCatalogue::DeleteIconeFileCatalog(WCHAR * lpCatalogName)
{
	WCHAR pwszSQLRequest[255];
	wsprintf(pwszSQLRequest,L"DELETE FROM ICONEFILECATALOG WHERE CatalogName = '%s'",lpCatalogName);
	return ExecuteRequestWithNoResult(pwszSQLRequest);
}

//-------------------------------------------------------------------------------------------------
// Traitement de la récupération des données
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileCatalogue::TraitementResult(IRowset * & pIRowset,HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding)
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
		_cIconeFile.eTypeIconeFile = CIconeFile::ETYPEFILE::CATALOG;

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
				//char FullPath[512];
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

//-------------------------------------------------------------------------------------------------
// Traitement de la sauvegarde des données
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileCatalogue::TraitementInsert(IRowset * & pIRowset,IRowsetChange * & pIRowsetChange, HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding)
{
	// Save the data
	// Allocate data buffer for seek and retrieve operation.
	//
	HRESULT hr;
	IconeFileVector::iterator dit;
	DWORD dwBindingSize = _iNbElement;
	BYTE*pData = (BYTE*)CoTaskMemAlloc(dwOffset);

	if (NULL == pData)
	{
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	for (dit = m_iconeFileVector->begin(); dit != m_iconeFileVector->end(); dit++)
	{
		DWORD	dwPhotoCol;
		DWORD   dwInfoSize;
		LPWSTR	lpwszInfo;

		// Set data buffer to zero
		//
		memset(pData, 0, dwOffset);
		CIconeFile m_IconeFileLocal = *dit;

		for (DWORD dwCol = 0; dwCol < dwBindingSize; ++dwCol)
		{
			// Get column value in string
			//
			char temp[20];
			wchar_t _wcTemp[1024];
			size_t m_sizeTConvert;
			int iValue = 0;

			switch(dwCol)
			{
			case 0:
				lpwszInfo = _pwzCatalogName;
				break;
			case 1:
				sprintf_s(temp,"%d",m_IconeFileLocal.lNumIcone);
				break;
			case 2:
				mbstowcs_s(&m_sizeTConvert,_wcTemp,1024, m_IconeFileLocal.informations, 256);
				lpwszInfo = _wcTemp;
				break;
			case 3:
				mbstowcs_s(&m_sizeTConvert,_wcTemp,1024, m_IconeFileLocal.m_stgFullPathName, 513);
				lpwszInfo = _wcTemp;
				break;
			case 4:
				mbstowcs_s(&m_sizeTConvert,_wcTemp,1024, m_IconeFileLocal.szTimeCreate, 32);
				lpwszInfo = _wcTemp;
				break;
			case 5:
				mbstowcs_s(&m_sizeTConvert,_wcTemp,1024, m_IconeFileLocal.szSize, 20);
				lpwszInfo = _wcTemp;
				break;
			}

			//lpwszInfo = g_SampleEmployeeData[dwRow].wszEmployeeInfo[dwCol];

			switch(prgBinding[dwCol].wType)
			{
				case DBTYPE_WSTR:
					// Copy value to binding buffer, truncate the string if it is too long
					//
					dwInfoSize = prgBinding[dwCol].cbMaxLen/sizeof(WCHAR) - 1;
					if (wcslen(lpwszInfo) >= dwInfoSize)
					{
						wcsncpy((WCHAR*)(pData+prgBinding[dwCol].obValue), lpwszInfo, dwInfoSize);
						*(WCHAR*)(pData+prgBinding[dwCol].obValue+dwInfoSize*sizeof(WCHAR)) = WCHAR('\0');
					}
					else
					{
						wcscpy((WCHAR*)(pData+prgBinding[dwCol].obValue),lpwszInfo);
					}

					*(ULONG*)(pData+prgBinding[dwCol].obLength)		= wcslen((WCHAR*)(pData+prgBinding[dwCol].obValue))*sizeof(WCHAR);
					*(DBSTATUS*)(pData+prgBinding[dwCol].obStatus)	= DBSTATUS_S_OK;
					break;

				case DBTYPE_I4:
					*(int*)(pData+prgBinding[dwCol].obValue)		= atoi(temp);//_wtoi(lpwszInfo);
					*(ULONG*)(pData+prgBinding[dwCol].obLength)		= 4;
					*(DBSTATUS*)(pData+prgBinding[dwCol].obStatus)	= DBSTATUS_S_OK;
					break;

				default:
					break;
			}
		}

		// Insert data to database
		//
		hr = pIRowsetChange->InsertRow(DB_NULL_HCHAPTER, hAccessor, pData, prghRows);
		if (FAILED(hr))
		{
			goto Exit;
		}

        // Release the rowset
		//
		hr = pIRowset->ReleaseRows(1, prghRows, NULL, NULL, NULL);
        if(FAILED(hr))
        {
			goto Exit;
        }
	}

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
