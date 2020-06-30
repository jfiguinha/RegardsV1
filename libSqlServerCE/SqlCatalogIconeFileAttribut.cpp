#include "StdAfx.h"
#include "SqlCatalogIconeFileAttribut.h"

using namespace LIBSQLSERVERCE;

CSqlCatalogIconeFileAttribut::CSqlCatalogIconeFileAttribut(void)
{
	wcscpy_s(pwzTableName,L"CATALOG_ICONEFILE_ATTRIBUT");
}

CSqlCatalogIconeFileAttribut::~CSqlCatalogIconeFileAttribut(void)
{
}
/////////////////////////////////////////////////////////////////
//Sauvegarde des informations sur les attributs
/////////////////////////////////////////////////////////////////
HRESULT CSqlCatalogIconeFileAttribut::SaveCatalogIconeFileAttribut(AttributVector * attributVector, WCHAR * lpCatalogName, int NumFile)
{
	WCHAR *	pwszIconeFileAttribut[] = {L"CatalogName",L"NumFile",L"NumAttribut"};
	_iNbElement = sizeof(pwszIconeFileAttribut)/sizeof(pwszIconeFileAttribut[0]);
	CopyChampsElement(pwszIconeFileAttribut);
	m_attributVector = attributVector;
	_lpCatalogName = lpCatalogName;
	_iNumFile = NumFile;
	return ExecuteInsert();
}

/////////////////////////////////////////////////////////////////
//Chargement des informations sur les attributs
/////////////////////////////////////////////////////////////////
HRESULT CSqlCatalogIconeFileAttribut::LoadCatalogIconeFileAttribut(AttributVector * attributVector, WCHAR * lpCatalogName, int NumFile)
{
	WCHAR *	pwszIconeFileAttribut[] = {L"NumAttribut",L"Libelle"};
	_iNbElement = sizeof(pwszIconeFileAttribut)/sizeof(pwszIconeFileAttribut[0]);
	CopyChampsElement(pwszIconeFileAttribut);
	m_attributVector = attributVector;
	WCHAR _pwszRequeteSQL[512];
	swprintf(_pwszRequeteSQL,512,L"SELECT AIA.NumAttribut, AA.Libelle FROM CATALOG_ICONEFILE_ATTRIBUT CIA, CATALOGMATTRIBUT CA WHERE CIA.NumAttribut = CA.NumAttribut and AlbumName = '%s' and NumFile = %d",lpCatalogName,NumFile);
	return ExecuteRequest(_pwszRequeteSQL);
}

/////////////////////////////////////////////////////////////////
//Suppression des informations sur les attributs
/////////////////////////////////////////////////////////////////
HRESULT CSqlCatalogIconeFileAttribut::DeleteCatalogIconeFileAttribut(WCHAR * lpCatalogName, int NumFile)
{
	WCHAR _pwszRequeteSQL[512];
	swprintf(_pwszRequeteSQL,512,L"DELETE FROM CATALOG_ICONEFILE_ATTRIBUT WHERE CatalogName = '%s' and NumFile = %d",lpCatalogName,NumFile);
	return ExecuteRequestWithNoResult(_pwszRequeteSQL);
}

/////////////////////////////////////////////////////////////////
//Traitement du chargement des informations sur les attributs
/////////////////////////////////////////////////////////////////
HRESULT CSqlCatalogIconeFileAttribut::TraitementResult(IRowset * & pIRowset,HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding)
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
		CAttribut _AttributData;

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
				case 0: //NumAttribut
					_AttributData.iNumAttribut = *(LONG*)(pCol->bData);
					break;
				case 1: //Libelle
					strcpy_s(_AttributData.cLibelle,(CHAR*)CSqlEngine::UnicodeToAnsi((WCHAR*)(pCol->bData)));
					break;
			}

			if(wTemp != NULL)
			{
				delete[] wTemp;
				wTemp = NULL;
			}

						
		}
		
		m_attributVector->push_back(_AttributData);

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

/////////////////////////////////////////////////////////////////
//Traitement de la sauvegarde des informations sur les attributs
/////////////////////////////////////////////////////////////////
HRESULT CSqlCatalogIconeFileAttribut::TraitementInsert(IRowset * & pIRowset,IRowsetChange * & pIRowsetChange, HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding)
{
	// Save the data
	// Allocate data buffer for seek and retrieve operation.
	//
	HRESULT hr;
	AttributVector::iterator dit;
	DWORD dwBindingSize = _iNbElement;
	BYTE*pData = (BYTE*)CoTaskMemAlloc(dwOffset);

	if (NULL == pData)
	{
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	for (dit = m_attributVector->begin(); dit != m_attributVector->end(); dit++)
	{
		DWORD	dwPhotoCol;
		DWORD   dwInfoSize;
		LPWSTR	lpwszInfo;

		// Set data buffer to zero
		//
		memset(pData, 0, dwOffset);
		CAttribut m_AttributData = *dit;

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
				case 0: //AlbumName
					lpwszInfo = _lpCatalogName;
					break;
				case 1: //NumFile
					sprintf_s(temp,"%d",_iNumFile);
					break;
				case 2: //NumAttribut
					sprintf_s(temp,"%d",m_AttributData.iNumAttribut);
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