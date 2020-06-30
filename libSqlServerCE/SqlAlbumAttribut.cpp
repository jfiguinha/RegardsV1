#include "StdAfx.h"
#include "SqlAlbumAttribut.h"
#include "dbcommon.h"
#include "sqlengine.h"

using namespace LIBSQLSERVERCE;

CSqlAlbumAttribut::CSqlAlbumAttribut(void)
{
	wcscpy(pwzTableName,L"ALBUMATTRIBUT");
}

CSqlAlbumAttribut::~CSqlAlbumAttribut(void)
{
}

/////////////////////////////////////////////////////////////////////////////
//Sauvegardes des attributs pour les albums
/////////////////////////////////////////////////////////////////////////////
HRESULT CSqlAlbumAttribut::SaveAlbumAttribut(AttributVector * attributVector)
{
	WCHAR *	pwszIconeFileFolder[] =	{L"NumAttribut",L"Libelle"};
	_iNbElement = sizeof(pwszIconeFileFolder)/sizeof(pwszIconeFileFolder[0]);
	CopyChampsElement(pwszIconeFileFolder);
	m_AttributVector = attributVector;
	return ExecuteInsert();
}

/////////////////////////////////////////////////////////////////////////////
//Chargement des attributs pour les albums
/////////////////////////////////////////////////////////////////////////////
HRESULT CSqlAlbumAttribut::LoadAlbumAttribut(AttributVector * attributVector)
{
	WCHAR *	pwszIconeFileFolder[] =	{L"NumAttribut",L"Libelle"};
	_iNbElement = sizeof(pwszIconeFileFolder)/sizeof(pwszIconeFileFolder[0]);
	CopyChampsElement(pwszIconeFileFolder);
	m_AttributVector = attributVector;
	return ExecuteRequest(L"SELECT NumAttribut, Libelle FROM ALBUMATTRIBUT");
}

/////////////////////////////////////////////////////////////////////////////
//Suppression de tous les attributs pour les albums
/////////////////////////////////////////////////////////////////////////////
HRESULT CSqlAlbumAttribut::DeleteAlbumAttribut()
{
	return ExecuteRequestWithNoResult(L"DELETE FROM ALBUMATTRIBUT");
}

/////////////////////////////////////////////////////////////////////////////
//Suppression de tous les attributs pour les albums
/////////////////////////////////////////////////////////////////////////////
HRESULT CSqlAlbumAttribut::DeleteAlbumAttribut(int iNumAttribut)
{
	WCHAR _pwszRequeteSQL[512];
	swprintf(_pwszRequeteSQL,512,L"DELETE ALBUMATTRIBUT WHERE NumAttribut = %d",iNumAttribut);
	return ExecuteRequestWithNoResult(_pwszRequeteSQL);
}

/////////////////////////////////////////////////////////////////////////////
//Suppression de tous les attributs pour les albums
/////////////////////////////////////////////////////////////////////////////
HRESULT CSqlAlbumAttribut::TraitementResult(IRowset * & pIRowset,HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding)
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
				case 0:
					_AttributData.iNumAttribut = *(LONG*)(pCol->bData);
					break;
				case 1:
					strcpy_s(_AttributData.cLibelle,(CHAR*)CSqlEngine::UnicodeToAnsi((WCHAR*)(pCol->bData)));
					break;
			}

			if(wTemp != NULL)
			{
				delete[] wTemp;
				wTemp = NULL;
			}

						
		}
		
		m_AttributVector->push_back(_AttributData);

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

/////////////////////////////////////////////////////////////////////////////
//Suppression de tous les attributs pour les albums
/////////////////////////////////////////////////////////////////////////////
HRESULT CSqlAlbumAttribut::TraitementInsert(IRowset * & pIRowset,IRowsetChange * & pIRowsetChange, HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding)
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

	for (dit = m_AttributVector->begin(); dit != m_AttributVector->end(); dit++)
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
				case 0:
					sprintf_s(temp,"%d",m_AttributData.iNumAttribut);
					break;
				case 1:
					mbstowcs_s(&m_sizeTConvert,_wcTemp,1024, m_AttributData.cLibelle, 255);
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

