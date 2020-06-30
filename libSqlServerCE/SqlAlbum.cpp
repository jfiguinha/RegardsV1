#include "StdAfx.h"
#include "SqlAlbum.h"

using namespace LIBSQLSERVERCE;

CSqlAlbum::CSqlAlbum(void)
{
	wcscpy(pwzTableName,L"ALBUM");
}

CSqlAlbum::~CSqlAlbum(void)
{
}

//--------------------------------------------------------
//Suppression de toutes les donn�es d'un album
//--------------------------------------------------------
HRESULT CSqlAlbum::SaveAlbum(CAlbumData * pAlbumData)
{
	WCHAR *	pwszIconeFileFolder[] =	{L"AlbumName"};
	_iNbElement = sizeof(pwszIconeFileFolder)/sizeof(pwszIconeFileFolder[0]);
	CopyChampsElement(pwszIconeFileFolder);
	m_AlbumData = pAlbumData;
	return ExecuteInsert();
}

//--------------------------------------------------------
//Chargement de toutes les donn�es d'un album
//--------------------------------------------------------
HRESULT CSqlAlbum::LoadAlbum(AlbumDataVector * albumDataVector)
{
	WCHAR *	pwszIconeFileFolder[] =	{L"AlbumName"};
	_iNbElement = sizeof(pwszIconeFileFolder)/sizeof(pwszIconeFileFolder[0]);
	CopyChampsElement(pwszIconeFileFolder);
	m_AlbumDataVector = albumDataVector;
	return ExecuteRequest(L"SELECT AlbumName FROM ALBUM");
}

//--------------------------------------------------------
//Suppression de toutes les donn�es d'un album
//--------------------------------------------------------
HRESULT CSqlAlbum::DeleteAlbum()
{
	return ExecuteRequestWithNoResult(L"DELETE FROM ALBUM");
}

//--------------------------------------------------------
//Suppression de toutes les donn�es d'un album
//--------------------------------------------------------
HRESULT CSqlAlbum::DeleteAlbum(WCHAR * lpAlbumName)
{
	WCHAR pwszSQLRequest[255];
	wsprintf(pwszSQLRequest,L"DELETE FROM ALBUM WHERE AlbumName = '%s'",lpAlbumName);
	return ExecuteRequestWithNoResult(pwszSQLRequest);
}

//--------------------------------------------------------
//Mise � jour du nom d'un album
//--------------------------------------------------------
HRESULT CSqlAlbum::UpdateAlbum(WCHAR * lpNewAlbumName,WCHAR * lpOldAlbumName)
{
	WCHAR pwszSQLRequest[255];
	wsprintf(pwszSQLRequest,L"UPDATE ALBUM SET AlbumName = '%s' WHERE AlbumName = '%s'", lpNewAlbumName, lpOldAlbumName);
	return ExecuteRequestWithNoResult(pwszSQLRequest);
}

//--------------------------------------------------------
//Traitement du Chargement de toutes les donn�es d'un album
//--------------------------------------------------------
HRESULT CSqlAlbum::TraitementResult(IRowset * & pIRowset,HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding)
{
	HRESULT hr = S_OK;
	BYTE*pData = (BYTE*)CoTaskMemAlloc(dwOffset);
	DWORD dwBindingSize = sizeof(pwszTableResult)/sizeof(pwszTableResult[0]);
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
		CAlbumData _AlbumData;

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
				strcpy_s(_AlbumData.m_szAlbumName,(CHAR*)CSqlEngine::UnicodeToAnsi((WCHAR*)(pCol->bData)));
				break;
			}

			if(wTemp != NULL)
			{
				delete[] wTemp;
				wTemp = NULL;
			}

						
		}
		
		m_AlbumDataVector->push_back(_AlbumData);

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

//--------------------------------------------------------
//Insertion de toutes les donn�es dans un album
//--------------------------------------------------------
HRESULT CSqlAlbum::TraitementInsert(IRowset * & pIRowset,IRowsetChange * & pIRowsetChange, HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding)
{
	// Save the data
	// Allocate data buffer for seek and retrieve operation.
	//
	HRESULT hr;
	AlbumDataVector::iterator dit;
	DWORD dwBindingSize = sizeof(pwszTableResult)/sizeof(pwszTableResult[0]);
	BYTE*pData = (BYTE*)CoTaskMemAlloc(dwOffset);

	if (NULL == pData)
	{
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	//for (dit = m_AlbumDataVector->begin(); dit != m_AlbumDataVector->end(); dit++)
	//{
		DWORD	dwPhotoCol;
		DWORD   dwInfoSize;
		LPWSTR	lpwszInfo;

		// Set data buffer to zero
		//
		memset(pData, 0, dwOffset);
		//CAlbumData m_AlbumData = *dit;

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
				mbstowcs_s(&m_sizeTConvert,_wcTemp,1024, m_AlbumData->m_szAlbumName, 255);
				lpwszInfo = _wcTemp;
				break;
			}


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
	//}

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
