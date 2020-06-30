#include "StdAfx.h"
#include "SqlIconeFileFolder.h"
#include "SqlExecuteRequest.h"
#include "SqlEngine.h"
#include "SqlLib.h"
#include "dbcommon.h"

using namespace LIBSQLSERVERCE;

CSqlIconeFileFolder::CSqlIconeFileFolder(void)
{
	pwszTableResult = NULL;
}

CSqlIconeFileFolder::~CSqlIconeFileFolder(void)
{

}

//-------------------------------------------------------------------------------------------------
// Suppression des données
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileFolder::DeleteIconeFileFolder(WCHAR * lpFolderName)
{
	WCHAR pwszSQLRequest[255];
	wsprintf(pwszSQLRequest,L"DELETE FROM ICONEFILEFOLDER WHERE FolderName = '%s'",lpFolderName);
	return ExecuteRequestWithNoResult(pwszSQLRequest);
}

//-------------------------------------------------------------------------------------------------
// Chargement des données
//-------------------------------------------------------------------------------------------------

HRESULT CSqlIconeFileFolder::LoadIconeFileFolder(IconeFileVector * iconeFileVector, WCHAR *  lpFolderName)
{
	WCHAR pwszSQLRequest[255];
	WCHAR *	pwszIconeFileFolder[] =	{L"FolderName",L"NumIcone",L"Informations",L"FullPathName",L"TimeCreate",L"Size"};
	_iNbElement = sizeof(pwszIconeFileFolder)/sizeof(pwszIconeFileFolder[0]);
	CopyChampsElement(pwszIconeFileFolder);
	m_iconeFileVector = iconeFileVector;
	wcscpy_s(pwzTableName,L"ICONEFILEFOLDER");
	wcscpy_s(_pwzDirName,lpFolderName);

	wsprintf(pwszSQLRequest,L"SELECT FolderName, NumIcone, Informations, FullPathName, TimeCreate, Size FROM ICONEFILEFOLDER WHERE FolderName = '%s'",lpFolderName);
	return ExecuteRequest(pwszSQLRequest);
}

//-------------------------------------------------------------------------------------------------
// Traitement Result
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileFolder::TraitementResult(IRowset * & pIRowset,HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding)
{
	HRESULT hr = S_OK;
	BYTE*pData = (BYTE*)CoTaskMemAlloc(dwOffset);
	DWORD dwBindingSize = _iNbElement;//sizeof(pwszTableResult)/sizeof(pwszTableResult[0]);
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
		_cIconeFile.eTypeIconeFile = CIconeFile::ETYPEFILE::FILE;

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


//-------------------------------------------------------------------------------------------------
// Sauvegarde des données
//-------------------------------------------------------------------------------------------------

HRESULT CSqlIconeFileFolder::SaveIconeFileFolder(IconeFileVector * iconeFileVector, WCHAR * lpFolderName)
{
	WCHAR *	pwszIconeFileFolder[] =	{L"FolderName",L"NumIcone",L"Informations",L"FullPathName",L"TimeCreate",L"Size",L"NbAttribut"};
	_iNbElement = sizeof(pwszIconeFileFolder)/sizeof(pwszIconeFileFolder[0]);
	CopyChampsElement(pwszIconeFileFolder);

	m_iconeFileVector = iconeFileVector;
	wcscpy_s(pwzTableName,L"ICONEFILEFOLDER");
	wcscpy_s(_pwzDirName,lpFolderName);
	return ExecuteInsert();
}


//-------------------------------------------------------------------------------------------------
// Traitement Insert 
//-------------------------------------------------------------------------------------------------

HRESULT CSqlIconeFileFolder::TraitementInsert(IRowset * & pIRowset,IRowsetChange * & pIRowsetChange, HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding)
{
	// Save the data
	// Allocate data buffer for seek and retrieve operation.
	//
	HRESULT hr;
	IconeFileVector::iterator dit;
	DWORD dwBindingSize = _iNbElement;//sizeof(pwszTableResult)/sizeof(pwszTableResult[0]);
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
				lpwszInfo = _pwzDirName;
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
