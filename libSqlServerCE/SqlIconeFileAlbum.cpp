#include "StdAfx.h"
#include "SqlIconeFileAlbum.h"
#include "../loadimage.h"
using namespace LIBSQLSERVERCE;

CSqlIconeFileAlbum::CSqlIconeFileAlbum(void)
{
}

CSqlIconeFileAlbum::~CSqlIconeFileAlbum(void)
{
}

//-------------------------------------------------------------------------------------------------
// Sauvegarde des données
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileAlbum::SaveIconeFileAlbum(IconeFileVector * iconeFileVector, WCHAR * lpAlbumName)
{
	WCHAR *	pwszIconeFileFolder[] =	{L"AlbumName",L"NumIcone",L"Informations",L"FullPathName",L"TimeCreate",L"Size", L"ImageSize", L"ImageWith", L"ImageHeight", L"ImageDate"};
	_iNbElement = sizeof(pwszIconeFileFolder)/sizeof(pwszIconeFileFolder[0]);
	CopyChampsElement(pwszIconeFileFolder);
	m_iconeFileVector = iconeFileVector;
	wcscpy_s(pwzTableName,L"ICONEFILEALBUM");
	wcscpy_s(_pwzAlbumName,lpAlbumName);
	return ExecuteInsert();
}

//-------------------------------------------------------------------------------------------------
// Chargement des données
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileAlbum::LoadIconeFileAlbum(IconeFileVector * iconeFileVector, WCHAR * lpAlbumName)
{
	WCHAR pwszSQLRequest[255];
	WCHAR *	pwszIconeFileFolder[] =	{L"AlbumName",L"NumIcone",L"Informations",L"FullPathName",L"TimeCreate",L"Size"};

	_iNbElement = sizeof(pwszIconeFileFolder)/sizeof(pwszIconeFileFolder[0]);
	CopyChampsElement(pwszIconeFileFolder);
	m_iconeFileVector = iconeFileVector;
	wcscpy_s(pwzTableName,L"ICONEFILEALBUM");
	wcscpy_s(_pwzAlbumName,lpAlbumName);
	
	wsprintf(pwszSQLRequest,L"SELECT AlbumName, NumIcone, Informations, FullPathName, TimeCreate, Size FROM ICONEFILEALBUM WHERE AlbumName = '%s'",lpAlbumName);
	return ExecuteRequest(pwszSQLRequest);

}

//-------------------------------------------------------------------------------------------------
// Chargement de toutes les images
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileAlbum::LoadIconeFileAlbum(IconeFileVector * iconeFileVector)
{
	WCHAR pwszSQLRequest[255];
	WCHAR *	pwszIconeFileFolder[] =	{L"AlbumName",L"NumIcone",L"Informations",L"FullPathName",L"TimeCreate",L"Size"};

	_iNbElement = sizeof(pwszIconeFileFolder)/sizeof(pwszIconeFileFolder[0]);
	CopyChampsElement(pwszIconeFileFolder);
	m_iconeFileVector = iconeFileVector;
	wcscpy_s(pwzTableName,L"ICONEFILEALBUM");
	return ExecuteRequest(L"SELECT AlbumName, NumIcone, Informations, FullPathName, TimeCreate, Size FROM ICONEFILEALBUM");
}

//-------------------------------------------------------------------------------------------------
// Suppression des données
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileAlbum::DeleteIconeFileAlbum(WCHAR * lpAlbumName)
{
	WCHAR pwszSQLRequest[255];
	wsprintf(pwszSQLRequest,L"DELETE FROM ICONEFILEALBUM WHERE AlbumName = '%s'",lpAlbumName);
	return ExecuteRequestWithNoResult(pwszSQLRequest);
}

//-------------------------------------------------------------------------------------------------
// Suppression d'une entrée
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileAlbum::DeleteIconeFileAlbum(WCHAR * lpAlbumName, WCHAR * lpFilename)
{
	WCHAR pwszSQLRequest[255];
	wsprintf(pwszSQLRequest,L"DELETE FROM ICONEFILEALBUM WHERE AlbumName = '%s' and FullPathName = '%s'",lpAlbumName, lpFilename);
	return ExecuteRequestWithNoResult(pwszSQLRequest);
}

//-------------------------------------------------------------------------------------------------
// Renommage du nom de l'album
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileAlbum::UpdateAlbum(WCHAR * lpNewAlbumName,WCHAR * lpOldAlbumName)
{
	WCHAR pwszSQLRequest[255];
	wsprintf(pwszSQLRequest,L"UPDATE ICONEFILEALBUM SET AlbumName = '%s' WHERE AlbumName = '%s'", lpNewAlbumName, lpOldAlbumName);
	return ExecuteRequestWithNoResult(pwszSQLRequest);
}

//-------------------------------------------------------------------------------------------------
// Traitement de la récupération des données
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileAlbum::TraitementResult(IRowset * & pIRowset,HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding)
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

		_cIconeFile.eTypeIconeFile = CIconeFile::ETYPEFILE::ALBUM;

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
// Traitement de la sauvegarde des données
//-------------------------------------------------------------------------------------------------
HRESULT CSqlIconeFileAlbum::TraitementInsert(IRowset * & pIRowset,IRowsetChange * & pIRowsetChange, HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding)
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

		UINT width, height;
		CLoadImage m_cLoadImage;
		m_cLoadImage.GetDimensionImageStatic(m_IconeFileLocal.m_stgFullPathName,&width,&height);

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
				lpwszInfo = _pwzAlbumName;
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
			case 6: //ImageSize
				sprintf_s(temp,"%d",atol(m_IconeFileLocal.szSize));
				break;
			case 7: //ImageWith
				sprintf_s(temp,"%d",width);
				break;
			case 8: //ImageHeight
				sprintf_s(temp,"%d",height);
				break;
			case 9: //ImageDate
				{
					std::string m_stgValue = m_IconeFileLocal.szTimeCreate;
					sprintf_s(temp,20,"%s%s%s",m_stgValue.substr(6,4).c_str(),m_stgValue.substr(3,2).c_str(),m_stgValue.substr(0,2).c_str());
				}
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
