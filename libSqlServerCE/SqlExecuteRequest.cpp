#include "StdAfx.h"
#include "SqlExecuteRequest.h"
#include "SqlEngine.h"
#include "SqlLib.h"
#include "dbcommon.h"

using namespace LIBSQLSERVERCE;

CSqlExecuteRequest::CSqlExecuteRequest(void)
{
	_iNbElement = 0;
	pwszTableResult = NULL;
}

CSqlExecuteRequest::~CSqlExecuteRequest(void)
{
	ViderChampsElement();
}

void CSqlExecuteRequest::ViderChampsElement()
{
	if(pwszTableResult != NULL)
	{
		for(int i=0;i<_iNbElement;i++)
			free(pwszTableResult[i]);
		free(pwszTableResult);

		pwszTableResult = NULL;
	}
}

void CSqlExecuteRequest::CopyChampsElement(WCHAR * * pszChamp)
{
	//ViderChampsElement();

	pwszTableResult = (WCHAR**)malloc(_iNbElement * sizeof(WCHAR*));
	for(int i=0;i<_iNbElement;i++)
	{
		pwszTableResult[i] = (WCHAR*)malloc(64 * sizeof(WCHAR));
		wcscpy(pwszTableResult[i],pszChamp[i]);
	}
}

//-------------------------------------------------------------------------------------------------
// Execution d'une requete qui n'attend pas de résultat
//-------------------------------------------------------------------------------------------------
HRESULT CSqlExecuteRequest::ExecuteRequestWithNoResult(WCHAR * lpRequestSQL)
{
	HRESULT	hr = NOERROR;			// Error code reporting

	_sqlLib = CSqlEngine::getInstance();

	// Validate IDBCreateSession interface
	//
	if (NULL == _sqlLib->GetSession())
	{
		hr = E_POINTER;
		goto Exit;
	}

	hr = _sqlLib->ExecuteSQLWithNoResult(lpRequestSQL);

Exit:

	return hr;
}

//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------
HRESULT CSqlExecuteRequest::ExecuteInsert()
{
	HRESULT				hr					= NOERROR;			// Error code reporting
	DBBINDING			*prgBinding			= NULL;				// Binding used to create accessor
    HROW				rghRows[1]          = {DB_NULL_HROW};   // Array of row handles obtained from the rowset object
	HROW				*prghRows			= rghRows;			// Row handle(s) pointer
	DBID				TableID;								// Used to open/create table
	DBID				IndexID;								// Used to create index
	DBPROPSET			rowsetpropset[1];						// Used when opening integrated index
	DBPROP				rowsetprop[1];							// Used when opening integrated index
   	ULONG				cRowsObtained		= 0;				// Number of rows obtained from the rowset object
    DBOBJECT			dbObject;								// DBOBJECT data.
	DBCOLUMNINFO		*pDBColumnInfo		= NULL;				// Record column metadata
	BYTE				*pData				= NULL;				// record data
	WCHAR				*pStringsBuffer		= NULL;
	DWORD				dwBindingSize		= 0;
	DWORD				dwIndex				= 0;
	DWORD				dwRow				= 0;
	DWORD				dwOrdinal			= 0;
	DWORD				dwCol				= 0;
	DWORD				dwOffset			= 0;
	ULONG				ulNumCols;

	IOpenRowset			*pIOpenRowset		= NULL;				// Provider Interface Pointer
	IRowset				*pIRowset			= NULL;				// Provider Interface Pointer
	ITransactionLocal	*pITxnLocal			= NULL;				// Provider Interface Pointer
	IRowsetChange		*pIRowsetChange		= NULL;				// Provider Interface Pointer
	IAccessor			*pIAccessor			= NULL;				// Provider Interface Pointer
	ISequentialStream	*pISequentialStream = NULL;				// Provider Interface Pointer
	IColumnsInfo		*pIColumnsInfo		= NULL;				// Provider Interface Pointer
	HACCESSOR			hAccessor			= DB_NULL_HACCESSOR;// Accessor handle

	VariantInit(&rowsetprop[0].vValue);

	_sqlLib = CSqlEngine::getInstance();

	// Validate IDBCreateSession interface
	//
	if (NULL == _sqlLib->GetSession())
	{
		hr = E_POINTER;
		goto Exit;
	}

    // Create a session object 
    //
    hr = _sqlLib->GetSession()->CreateSession(NULL, IID_IOpenRowset, (IUnknown**) &pIOpenRowset);
    if(FAILED(hr))
    {
        goto Exit;
    }

	hr = pIOpenRowset->QueryInterface(IID_ITransactionLocal, (void**)&pITxnLocal);
	if(FAILED(hr))
	{
		goto Exit;
	}

	// Set up information necessary to open a table 
	// using an index and have the ability to seek.
	//
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= pwzTableName;

	//IndexID.eKind			= DBKIND_NAME;
	//IndexID.uName.pwszName	= L"PK_Employees";

	// Request ability to use IRowsetChange interface
	// 
	rowsetpropset[0].cProperties	= 1;
	rowsetpropset[0].guidPropertySet= DBPROPSET_ROWSET;
	rowsetpropset[0].rgProperties	= rowsetprop;

	rowsetprop[0].dwPropertyID		= DBPROP_IRowsetChange;
	rowsetprop[0].dwOptions			= DBPROPOPTIONS_REQUIRED;
	rowsetprop[0].colid				= DB_NULLID;
	rowsetprop[0].vValue.vt			= VT_BOOL;
	rowsetprop[0].vValue.boolVal	= VARIANT_TRUE;

	// Open the table using the index
	//
	hr = pIOpenRowset->OpenRowset(	NULL,
									&TableID,
									NULL,
									IID_IRowset,
									sizeof(rowsetpropset)/sizeof(rowsetpropset[0]),
									rowsetpropset,
									(IUnknown**) &pIRowset);
	if(FAILED(hr))
	{
		goto Exit;
	}

    // Get IRowsetChange interface
	//
	hr = pIRowset->QueryInterface(IID_IRowsetChange, (void**)&pIRowsetChange);
	if(FAILED(hr))
	{
		goto Exit;
	}

    // Get IColumnsInfo interface
	//
    hr = pIRowset->QueryInterface(IID_IColumnsInfo, (void **)&pIColumnsInfo);
	if(FAILED(hr))
	{
		goto Exit;
	}

	// Get the column metadata 
	//
    hr = pIColumnsInfo->GetColumnInfo(&ulNumCols, &pDBColumnInfo, &pStringsBuffer);
	if(FAILED(hr) || 0 == ulNumCols)
	{
		goto Exit;
	}

    // Create a DBBINDING array.
    // The binding doesn't include the bookmark column (first column).
	//
	dwBindingSize = ulNumCols - 1;
	prgBinding = (DBBINDING*)CoTaskMemAlloc(sizeof(DBBINDING)*dwBindingSize);
	if (NULL == prgBinding)
	{
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	// Set initial offset for binding position
	//
	dwOffset = 0;

	// Prepare structures to create the accessor
	//
    for (dwIndex = 0; dwIndex < dwBindingSize; ++dwIndex)
    {
		DWORD dColumnNum;
		if (!_sqlLib->GetColumnOrdinal(pDBColumnInfo, ulNumCols, pwszTableResult[dwIndex], &dwOrdinal, &dColumnNum))
		{
			hr = E_FAIL;
			goto Exit;
		}

		prgBinding[dwIndex].iOrdinal	= dwOrdinal;
		prgBinding[dwIndex].pTypeInfo	= NULL;
		prgBinding[dwIndex].pBindExt	= NULL;
		prgBinding[dwIndex].dwMemOwner	= DBMEMOWNER_CLIENTOWNED;
		prgBinding[dwIndex].dwFlags		= 0;
		prgBinding[dwIndex].bPrecision	= pDBColumnInfo[dColumnNum].bPrecision;
		prgBinding[dwIndex].bScale		= pDBColumnInfo[dColumnNum].bScale;
		prgBinding[dwIndex].dwPart		= DBPART_VALUE | DBPART_STATUS | DBPART_LENGTH;
		prgBinding[dwIndex].obLength	= dwOffset;                                     
		prgBinding[dwIndex].obStatus	= prgBinding[dwIndex].obLength + sizeof(ULONG);  
		prgBinding[dwIndex].obValue		= prgBinding[dwIndex].obStatus + sizeof(DBSTATUS);

		switch(pDBColumnInfo[dwIndex + 1].wType)
		{
		case DBTYPE_BYTES:
			// Set up the DBOBJECT structure.
			//
			dbObject.dwFlags = STGM_WRITE;
			dbObject.iid = IID_ISequentialStream;

			prgBinding[dwIndex].pObject		= &dbObject;
			prgBinding[dwIndex].cbMaxLen	= sizeof(IUnknown*);
			prgBinding[dwIndex].wType		= DBTYPE_IUNKNOWN;
			break;

		case DBTYPE_WSTR:
			prgBinding[dwIndex].pObject		= NULL;
			prgBinding[dwIndex].wType		= pDBColumnInfo[dColumnNum].wType;
			prgBinding[dwIndex].cbMaxLen	= sizeof(WCHAR)*(pDBColumnInfo[dColumnNum].ulColumnSize + 1);	// Extra buffer for null terminator 
			break;

		default:
			prgBinding[dwIndex].pObject		= NULL;
			prgBinding[dwIndex].wType		= pDBColumnInfo[dColumnNum].wType;
			prgBinding[dwIndex].cbMaxLen	= pDBColumnInfo[dColumnNum].ulColumnSize; 
			break;
		}

		// Calculate new offset
		// 
		dwOffset = prgBinding[dwIndex].obValue + prgBinding[dwIndex].cbMaxLen;

		// Properly align the offset
		//
		dwOffset = ROUND_UP(dwOffset, COLUMN_ALIGNVAL);
	}

	// Get IAccessor interface
	//
	hr = pIRowset->QueryInterface(IID_IAccessor, (void**)&pIAccessor);
	if(FAILED(hr))
	{
		goto Exit;
	}

    // Create accessor.
	//
    hr = pIAccessor->CreateAccessor(DBACCESSOR_ROWDATA, 
									dwBindingSize, 
									prgBinding,
									0,
									&hAccessor,
									NULL);
    if(FAILED(hr))
    {
        goto Exit;
    }

	// Begins a new local transaction
	//
	hr = pITxnLocal->StartTransaction(ISOLATIONLEVEL_READCOMMITTED | ISOLATIONLEVEL_CURSORSTABILITY, 0, NULL, NULL);
	if(FAILED(hr))
	{
		goto Exit;
	}

	hr = TraitementInsert(pIRowset,pIRowsetChange, prghRows, hAccessor, dwOffset, prgBinding);
	if(FAILED(hr))
	{
		goto Abort;
	}

	// Commit the transaction
	//
	if (pITxnLocal)
	{
		pITxnLocal->Commit(FALSE, XACTTC_SYNC, 0);
	}

	goto Exit;

Abort:

    if (DB_NULL_HROW != prghRows[0])
    {
        pIRowset->ReleaseRows(1, prghRows, NULL, NULL, NULL);
    }

	// Abort the transaction
	//
	if (pITxnLocal)
	{
		pITxnLocal->Abort(NULL, FALSE, FALSE);
	}

Exit:
    // Clear Variants
    //
	VariantClear(&rowsetprop[0].vValue);

    // Free allocated DBBinding memory
    //
    if (prgBinding)
    {
        CoTaskMemFree(prgBinding);
        prgBinding = NULL;
    }

    // Free allocated column info memory
    //
    if (pDBColumnInfo)
    {
        CoTaskMemFree(pDBColumnInfo);
        pDBColumnInfo = NULL;
    }
	
	// Free allocated column string values buffer
    //
    if (pStringsBuffer)
    {
        CoTaskMemFree(pStringsBuffer);
        pStringsBuffer = NULL;
    }



	// Release interfaces
	//
    if(pISequentialStream)
    {
		pISequentialStream->Release();
    }

	if(pIAccessor)
	{
		pIAccessor->ReleaseAccessor(hAccessor, NULL); 
		pIAccessor->Release();
	}

	if (pIColumnsInfo)
	{
		pIColumnsInfo->Release();
	}

	if (pIRowsetChange)
	{
		pIRowsetChange->Release();
	}

	if (pITxnLocal)
	{
		pITxnLocal->Release();
	}

	if(pIRowset)
	{
		pIRowset->Release();
	}

	if(pIOpenRowset)
	{
		pIOpenRowset->Release();
	}

	return hr;
}


//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------
HRESULT CSqlExecuteRequest::ExecuteRequest(WCHAR * lpRequestSQL)
{
	HRESULT					hr					= NOERROR;			// Error code reporting
	DBID				    TableID;								// Used to open/create table
	DBID				    IndexID;								// Used to open/create index
	DBPROPSET			    rowsetpropset[1];						// Used when opening integrated index
	DBPROP				    rowsetprop[1];							// Used when opening integrated index
	DBBINDING				*prgBinding			= NULL;				// Binding used to create accessor
	HROW				    rghRows[1];								// Array of row handles obtained from the rowset object
	HROW*				    prghRows			= rghRows;			// Row handle(s) pointer
   	ULONG				    cRowsObtained;							// Number of rows obtained from the rowset object
	DBCOLUMNINFO			*pDBColumnInfo		= NULL;				// Record column metadata
	WCHAR					*pwszName			= NULL;				// Record employee name
	DWORD					dwIndex				= 0;
	DWORD					dwOffset			= 0;
	DWORD					dwBindingSize		= 0;
	DWORD					dwOrdinal			= 0;
	ULONG					ulNumCols			= 0;
	WCHAR					*pStringsBuffer		= NULL;

	IOpenRowset				*pIOpenRowset		= NULL;				// Provider Interface Pointer
	IRowset					*pIRowset			= NULL;				// Provider Interface Pointer
	IColumnsInfo			*pIColumnsInfo		= NULL;				// Provider Interface Pointer
	IAccessor*			    pIAccessor			= NULL;				// Provider Interface Pointer
	HACCESSOR			    hAccessor			= DB_NULL_HACCESSOR;// Accessor handle
	IRowsetChange*			pIRowsetChange      = NULL;

	_sqlLib = CSqlEngine::getInstance();

    VariantInit(&rowsetprop[0].vValue);

	// Validate IDBCreateSession interface
	//
	if (NULL == _sqlLib->GetSession())
	{
		hr = E_POINTER;
		goto Exit;
	}

	hr = _sqlLib->ExecuteSQL(lpRequestSQL,pIRowset);
    if(FAILED(hr))
    {
        goto Exit;
    }

    // Get IColumnsInfo interface
	//
    hr = pIRowset->QueryInterface(IID_IColumnsInfo, (void **)&pIColumnsInfo);
	if(FAILED(hr))
	{
		goto Exit;
	}

	// Get the column metadata 
	//
    hr = pIColumnsInfo->GetColumnInfo(&ulNumCols, &pDBColumnInfo, &pStringsBuffer);
	if(FAILED(hr) || 0 == ulNumCols)
	{
		goto Exit;
	}

    // Create a DBBINDING array.
	//
	dwBindingSize = _iNbElement;//sizeof(pwszTableResult)/sizeof(pwszTableResult[0]);
	prgBinding = (DBBINDING*)CoTaskMemAlloc(sizeof(DBBINDING)*(dwBindingSize+1));
	if (NULL == prgBinding)
	{
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	// Set initial offset for binding position
	//
	dwOffset = 0;

	// Prepare structures to create the accessor
	//

    for (dwIndex = 0; dwIndex < dwBindingSize; ++dwIndex)
    {
		DWORD dNumColumn = 0;
		if (!_sqlLib->GetColumnOrdinal(pDBColumnInfo, ulNumCols, pwszTableResult[dwIndex], &dwOrdinal, &dNumColumn))
		{
			hr = E_FAIL;
			goto Exit;
		}

		//dwOrdinal = dwIndex;

		prgBinding[dwIndex].iOrdinal	= dwOrdinal;
		prgBinding[dwIndex].dwPart		= DBPART_VALUE | DBPART_STATUS | DBPART_LENGTH;
		prgBinding[dwIndex].obLength	= dwOffset + offsetof(COLUMNDATA,dwLength);                                    
		prgBinding[dwIndex].obStatus	= dwOffset + offsetof(COLUMNDATA,dwStatus);
		prgBinding[dwIndex].obValue		= dwOffset + offsetof(COLUMNDATA,bData);
		prgBinding[dwIndex].wType		= pDBColumnInfo[dNumColumn].wType;
		prgBinding[dwIndex].pTypeInfo	= NULL;
		prgBinding[dwIndex].pObject		= NULL;
		prgBinding[dwIndex].pBindExt	= NULL;
		prgBinding[dwIndex].dwMemOwner	= DBMEMOWNER_CLIENTOWNED;
		prgBinding[dwIndex].dwFlags		= 0;
		prgBinding[dwIndex].bPrecision	= pDBColumnInfo[dNumColumn].bPrecision;
		prgBinding[dwIndex].bScale		= pDBColumnInfo[dNumColumn].bScale;

		switch(prgBinding[dwIndex].wType)
		{
		case DBTYPE_WSTR:		
			prgBinding[dwIndex].cbMaxLen = sizeof(WCHAR)*(pDBColumnInfo[dNumColumn].ulColumnSize + 1);	// Extra buffer for null terminator 
			break;
		default:
			prgBinding[dwIndex].cbMaxLen = pDBColumnInfo[dNumColumn].ulColumnSize; 
			break;
		}

		// Calculate the offset, and properly align it
		// 
		dwOffset = prgBinding[dwIndex].obValue + prgBinding[dwIndex].cbMaxLen;
		dwOffset = ROUND_UP(dwOffset, COLUMN_ALIGNVAL);
	}

	// Get IAccessor 
	//

	hr = pIRowset->QueryInterface(IID_IAccessor, (void**)&pIAccessor);
	if(FAILED(hr))
	{
		goto Exit;
	}

	// Create the accessor
	//
	hr = pIAccessor->CreateAccessor(DBACCESSOR_ROWDATA, 
									dwBindingSize, 
									prgBinding, 
									dwOffset, 
									&hAccessor, 
									NULL);
	if(FAILED(hr))
	{
		goto Exit;
	}


	
	hr = TraitementResult(pIRowset,prghRows,hAccessor, dwOffset, prgBinding);
	
Exit:
    // Clear Variants
    //
	VariantClear(&rowsetprop[0].vValue); 

    // Free allocated DBBinding memory
    //
    if (prgBinding)
    {
        CoTaskMemFree(prgBinding);
        prgBinding = NULL;
    }

    // Free allocated column info memory
    //
    if (pDBColumnInfo)
    {
        CoTaskMemFree(pDBColumnInfo);
        pDBColumnInfo = NULL;
    }
	
	// Free allocated column string values buffer
    //
    if (pStringsBuffer)
    {
        CoTaskMemFree(pStringsBuffer);
        pStringsBuffer = NULL;
    }


    // Free employee name buffer
    //
	if (pwszName)
	{
		CoTaskMemFree(pwszName);
		pwszName = NULL;
	}

	// Release interfaces
	//
	if(pIAccessor)
	{
		pIAccessor->ReleaseAccessor(hAccessor, NULL); 
		pIAccessor->Release();
	}

	if (pIColumnsInfo)
	{
		pIColumnsInfo->Release();
	}

	if(pIRowset)
	{
		pIRowset->Release();
	}

	if(pIOpenRowset)
	{
		pIOpenRowset->Release();
	}

	return hr;
}

