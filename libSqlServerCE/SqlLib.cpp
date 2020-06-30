#include "StdAfx.h"
#include "SqlLib.h"
#include "dbcommon.h"
#include <ca_merge30.h>
using namespace LIBSQLSERVERCE;

typedef HRESULT (STDAPICALLTYPE* DllGetClassObjectPtr)(REFCLSID rclsid, REFIID riid, LPVOID * ppv);

////////////////////////////////////////////////////////////////////////////////
//Constructeur
////////////////////////////////////////////////////////////////////////////////
CSqlLib::CSqlLib(void)
{
	m_wFilename[0] = L'\0'; 
	HRESULT hr = NOERROR;
	m_pIDBCreateSession = NULL;
	// Initialize environment
	//
	//hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if(FAILED(hr))
	{
         pSuccess = FALSE;
		 MessageBox(NULL, L"COM Initialization Failure.", L"Regards Database", MB_OK);
         return;
	}

	// Load SQL CE DLL.
	sql_dll = LoadLibrary(L"sqlceoledb30.dll");
	if (sql_dll)
	{
		DllGetClassObjectPtr dllgetclassobject = (DllGetClassObjectPtr)GetProcAddress(sql_dll, "DllGetClassObject");
		if (dllgetclassobject)
		{
			dllgetclassobject(CLSID_SQLSERVERCE_3_0, IID_IClassFactory,  (void **)&classfactory);
			if(classfactory)
			{
				pSuccess = TRUE;
				return;
			}
		}
	}

	MessageBox(NULL, L"COM Initialization Failure.", L"Regards Database", MB_OK);
	pSuccess = FALSE;
}

HRESULT CSqlLib::CompactDatabase()
{
	/*
	// Load SQL CE DLL.
	ISSCEEngine *pISSCEEngine = NULL; 
	HMODULE sql_ce = LoadLibrary(L"sqlcese30.dll");
	if (sql_ce)
	{
		DllGetClassObjectPtr dllgetclassobject = (DllGetClassObjectPtr)GetProcAddress(sql_ce, "DllGetClassObject");
		if (dllgetclassobject)
		{
			dllgetclassobject(CLSID_Engine, IID_ISSCEEngine,  (void **)&pISSCEEngine);
			if(pISSCEEngine)
			{
				WCHAR *wszSrc=L"Data Source = src.sdf";
				WCHAR *wszDst=L"Data Source = dst.sdf";
    			pISSCEEngine->CompactDatabase(wszSrc,wszDst);
				pISSCEEngine->Release();
			}
		}
	}
	*/

	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
//Destructeur
////////////////////////////////////////////////////////////////////////////////
CSqlLib::~CSqlLib(void)
{
	KillSession();

	if(classfactory)
		classfactory.Release();

	if(sql_dll)
		FreeLibrary(sql_dll);

	// Uninitialize the environment
	//CoUninitialize();
}

HRESULT CSqlLib::KillSession()
{
	HRESULT        hr = NOERROR;
	// Release interfaces
	//
	if(m_pIDBCreateSession)
	{
        
        IDBInitialize *pIDBInitialize = NULL;

	    hr = m_pIDBCreateSession->QueryInterface(IID_IDBInitialize, (void **) &pIDBInitialize);
	    if(SUCCEEDED(hr))
	    {
            pIDBInitialize->Uninitialize();
            pIDBInitialize->Release();
        }
        
		m_pIDBCreateSession->Release();
	}
	return hr;
}

////////////////////////////////////////////////////////////////////////////////
// Function: InitDatabase()
//
// Description: Open a connection to database, 
//				then create a session object.
//
// Returns: NOERROR if succesfull
//
// Notes:
//
////////////////////////////////////////////////////////////////////////////////
HRESULT CSqlLib::InitDatabase(LPCWSTR lpFilename)
{
    HRESULT			   	hr				= NOERROR;	// Error code reporting
	HANDLE				hFind;							// File handle
	WIN32_FIND_DATA		FindFileData;					// The file structure description  

	if(pSuccess)
	{
		wcscpy_s(m_wFilename,MAX_PATH, lpFilename);
		// If database exists, open it,
		// Otherwise, create a new database, insert sample data.
		//
		hFind = FindFirstFile(m_wFilename, &FindFileData);
		if (INVALID_HANDLE_VALUE != hFind)
		{
			FindClose(hFind);
			hr = OpenDatabase();
		}
		else
		{
			hr = CreateDatabase();
		}
	}

	return hr;
}

////////////////////////////////////////////////////////////////////////////////
// Function: OpenDatabase
//
// Description:	Open a connection to database
//
// Returns: NOERROR if succesfull
//
////////////////////////////////////////////////////////////////////////////////
HRESULT CSqlLib::OpenDatabase()
{
    HRESULT			   	hr				= NOERROR;	// Error code reporting
	DBPROP				dbprop[1];					// property used in property set to initialize provider
	DBPROPSET			dbpropset[1];				// Property Set used to initialize provider

    IDBInitialize       *pIDBInitialize = NULL;		// Provider Interface Pointer
	IDBProperties       *pIDBProperties	= NULL;		// Provider Interface Pointer

	VariantInit(&dbprop[0].vValue);		

	hr = classfactory->CreateInstance(0, IID_IDBInitialize, (void **)&pIDBInitialize);

	if(FAILED(hr))
	{
		goto Exit;
	}

	// Initialize a property with name of database
	//
    dbprop[0].dwPropertyID	= DBPROP_INIT_DATASOURCE;
	dbprop[0].dwOptions		= DBPROPOPTIONS_REQUIRED;
    dbprop[0].vValue.vt		= VT_BSTR;
    dbprop[0].vValue.bstrVal= SysAllocString(m_wFilename);
	if(NULL == dbprop[0].vValue.bstrVal)
	{
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	// Initialize the property set
	//
	dbpropset[0].guidPropertySet = DBPROPSET_DBINIT;
	dbpropset[0].rgProperties	 = dbprop;
	dbpropset[0].cProperties	 = sizeof(dbprop)/sizeof(dbprop[0]);

	//Set initialization properties.
	//
	hr = pIDBInitialize->QueryInterface(IID_IDBProperties, (void **)&pIDBProperties);
    if(FAILED(hr))
    {
		goto Exit;
    }

	// Sets properties in the Data Source and initialization property groups
	//
    hr = pIDBProperties->SetProperties(1, dbpropset); 
	if(FAILED(hr))
    {
		goto Exit;
    }

	// Initializes a data source object 
	//
	hr = pIDBInitialize->Initialize();
	if(FAILED(hr))
    {
		goto Exit;
    }

    // Get IDBCreateSession interface
    //
  	hr = pIDBInitialize->QueryInterface(IID_IDBCreateSession, (void**)&m_pIDBCreateSession);

Exit:
    // Clear Variant
    //
	VariantClear(&dbprop[0].vValue);

	// Release interfaces
	//
	if(pIDBProperties)
	{
		pIDBProperties->Release();
	}

    if (pIDBInitialize)
    {
        pIDBInitialize->Release();
    }

	return hr;
}

////////////////////////////////////////////////////////////////////////////////
// Function: CreateDatabase
//
// Description:
//		Create Northwind Database through OLE DB
//		Create Employees table
//
// Returns: NOERROR if succesfull
//
////////////////////////////////////////////////////////////////////////////////
HRESULT CSqlLib::CreateDatabase()
{
	HRESULT				hr					 = NOERROR;	// Error code reporting
	DBPROPSET			dbpropset[1];					// Property Set used to initialize provider
	DBPROP				dbprop[1];						// property array used in property set to initialize provider

	IDBInitialize	    *pIDBInitialize      = NULL;    // Provider Interface Pointer
	IDBDataSourceAdmin	*pIDBDataSourceAdmin = NULL;	// Provider Interface Pointer
	IUnknown			*pIUnknownSession	 = NULL;	// Provider Interface Pointer
	IDBCreateCommand	*pIDBCrtCmd			 = NULL;	// Provider Interface Pointer
	ICommandText		*pICmdText			 = NULL;	// Provider Interface Pointer

	VariantInit(&dbprop[0].vValue);

	// Delete the DB if it already exists
	//
	hr = classfactory->CreateInstance(0, IID_IDBInitialize, (void **)&pIDBInitialize);
	if(FAILED(hr))
	  goto Exit;


	// Initialize a property with name of database
	//
	dbprop[0].dwPropertyID		= DBPROP_INIT_DATASOURCE;
	dbprop[0].dwOptions			= DBPROPOPTIONS_REQUIRED;
	dbprop[0].vValue.vt			= VT_BSTR;
	dbprop[0].vValue.bstrVal	= SysAllocString(m_wFilename);
	if(NULL == dbprop[0].vValue.bstrVal)
	{
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	// Initialize the property set
	//
	dbpropset[0].guidPropertySet = DBPROPSET_DBINIT;
	dbpropset[0].rgProperties	 = dbprop;
	dbpropset[0].cProperties	 = sizeof(dbprop)/sizeof(dbprop[0]);

	// Get IDBDataSourceAdmin interface
	//
	hr = pIDBInitialize->QueryInterface(IID_IDBDataSourceAdmin, (void **) &pIDBDataSourceAdmin);
	if(FAILED(hr))
	{
		goto Exit;
	}

	// Create and initialize data store
	//
	
	hr = pIDBDataSourceAdmin->CreateDataSource(1, dbpropset, NULL, IID_IUnknown, &pIUnknownSession);
	if(FAILED(hr))	
    {
		goto Exit;
    }

    // Get IDBCreateSession interface
    //
  	hr = pIDBInitialize->QueryInterface(IID_IDBCreateSession, (void**)&m_pIDBCreateSession);
	if(FAILED(hr))
	{
		goto Exit;
	}

	// Get IDBCreateCommand interface
	//
	hr = pIUnknownSession->QueryInterface(IID_IDBCreateCommand, (void**)&pIDBCrtCmd);
	if(FAILED(hr))
	{
		goto Exit;
	}

	// Create a command object
	//
	hr = pIDBCrtCmd->CreateCommand(NULL, IID_ICommandText, (IUnknown**)&pICmdText);
	if(FAILED(hr))
	{
		goto Exit;
	}

	// Create ICONEFILE table
	//
	hr = ExecuteSQL(pICmdText, (LPWSTR)SQL_CREATE_ICONEFILE_TABLE);
	if(FAILED(hr))
	{
		goto Exit;
	}

	// Create ALBUM ICONEFILE table
	//
	hr = ExecuteSQL(pICmdText, (LPWSTR)SQL_CREATE_ALBUMICONEFILE_TABLE);
	if(FAILED(hr))
	{
		goto Exit;
	}

	// Create CATALOG ICONEFILE table
	//
	hr = ExecuteSQL(pICmdText, (LPWSTR)SQL_CREATE_CATALOGICONEFILE_TABLE);
	if(FAILED(hr))
	{
		goto Exit;
	}

	// Create ALBUM table
	//
	hr = ExecuteSQL(pICmdText, (LPWSTR)SQL_CREATE_ALBUM_TABLE);
	if(FAILED(hr))
	{
		goto Exit;
	}

	// Create Catalog table
	//
	hr = ExecuteSQL(pICmdText, (LPWSTR)SQL_CREATE_CATALOG_TABLE);
	if(FAILED(hr))
	{
		goto Exit;
	}

	// Create ALBUM ATTRIBUT table
	//
	hr = ExecuteSQL(pICmdText, (LPWSTR)SQL_CREATE_ALBUMATTRIBUT_TABLE);
	if(FAILED(hr))
	{
		goto Exit;
	}

	// Create CATALOG ATTRIBUT table
	//
	hr = ExecuteSQL(pICmdText, (LPWSTR)SQL_CREATE_CATALOGATTRIBUT_TABLE);
	if(FAILED(hr))
	{
		goto Exit;
	}

	// Create ALBUM ICONEFILE ATTRIBUT table
	//
	hr = ExecuteSQL(pICmdText, (LPWSTR)SQL_CREATE_ALBUM_ICONEFILE_ATTRIBUT_TABLE);
	if(FAILED(hr))
	{
		goto Exit;
	}

	// Create CATALOG ICONEFILE ATTRIBUT table
	//
	hr = ExecuteSQL(pICmdText, (LPWSTR)SQL_CREATE_CATALOG_ICONEFILE_ATTRIBUT_TABLE);
	if(FAILED(hr))
	{
		goto Exit;
	}


Exit:
    // Clear Variant
    //
	VariantClear(&dbprop[0].vValue);

	// Release interfaces
	//
	if(pICmdText)
	{
		pICmdText->Release();
	}

	if(pIDBCrtCmd)
	{
		pIDBCrtCmd->Release();
	}

	if(pIUnknownSession)
	{
		pIUnknownSession->Release();
	}

	if(pIDBDataSourceAdmin)
	{
		pIDBDataSourceAdmin->Release();
	}

	if(pIDBInitialize)
	{
		pIDBInitialize->Release();
	}

	return hr;
}

////////////////////////////////////////////////////////////////////////////////
// Function: ExecuteSQL
//
// Description:
//		Executes a non row returning SQL statement
//
// Parameters
//		pICmdText	- a pointer to the ICommandText interface on the Command Object
//		pwszQuery	- the SQL statement to execute
//
// Returns: NOERROR if succesfull
//
////////////////////////////////////////////////////////////////////////////////
HRESULT CSqlLib::ExecuteSQL(ICommandText *pICmdText, WCHAR * pwszQuery)
{
	HRESULT hr = NOERROR;

	hr = pICmdText->SetCommandText(DBGUID_SQL, pwszQuery); 
	if(FAILED(hr))
	{
		goto Exit;
	}

	hr = pICmdText->Execute(NULL, IID_NULL, NULL, NULL, NULL);

Exit:

	return hr;
}

////////////////////////////////////////////////////////////////////////////////
// Function: ExecuteSQL
//
// Description:
//		Executes a non row returning SQL statement
//
// Parameters
//		pICmdText	- a pointer to the ICommandText interface on the Command Object
//		pwszQuery	- the SQL statement to execute
//
// Returns: NOERROR if succesfull
//
////////////////////////////////////////////////////////////////////////////////
HRESULT CSqlLib::ExecuteSQL(WCHAR * pwszQuery, IRowset * & pIRowset)
{
	HRESULT hr = NOERROR;

	IDBCreateCommand	*pIDBCrtCmd			 = NULL;	// Provider Interface Pointer
	ICommandText		*pICmdText			 = NULL;	// Provider Interface Pointer


	// Get IDBCreateCommand interface
	//
	// The session created next receives the SQL Server connection of
    // the data source object. No new connection is established.
    if (FAILED(m_pIDBCreateSession->CreateSession(NULL, IID_IDBCreateCommand, (IUnknown**) &pIDBCrtCmd)))
    {
        // Display error from pIDBCreateSession.
        goto Exit;
    }

	hr = pIDBCrtCmd->CreateCommand(NULL, IID_ICommandText, (IUnknown**)&pICmdText);
	if(FAILED(hr))
	{
		goto Exit;
	}

	hr = pICmdText->SetCommandText(DBGUID_SQL, pwszQuery); 
	if(FAILED(hr))
	{
		goto Exit;
	}

    hr = pICmdText->Execute(NULL, IID_IRowset, NULL, NULL, (IUnknown**)&pIRowset);

Exit:

	// Release interfaces
	//
	if(pICmdText)
	{
		pICmdText->Release();
	}

	if(pIDBCrtCmd)
	{
		pIDBCrtCmd->Release();
	}

	return hr;
}

////////////////////////////////////////////////////////////////////////////////
// Function: ExecuteSQL
//
// Description:
//		Executes a non row returning SQL statement
//
// Parameters
//		pICmdText	- a pointer to the ICommandText interface on the Command Object
//		pwszQuery	- the SQL statement to execute
//
// Returns: NOERROR if succesfull
//
////////////////////////////////////////////////////////////////////////////////
HRESULT CSqlLib::ExecuteSQLWithNoResult(WCHAR * pwszQuery)
{
	HRESULT hr = NOERROR;

	IDBCreateCommand	*pIDBCrtCmd			 = NULL;	// Provider Interface Pointer
	ICommandText		*pICmdText			 = NULL;	// Provider Interface Pointer


	// Get IDBCreateCommand interface
	//
	// The session created next receives the SQL Server connection of
    // the data source object. No new connection is established.
    if (FAILED(m_pIDBCreateSession->CreateSession(NULL, IID_IDBCreateCommand, (IUnknown**) &pIDBCrtCmd)))
    {
        // Display error from pIDBCreateSession.
        goto Exit;
    }

	hr = pIDBCrtCmd->CreateCommand(NULL, IID_ICommandText, (IUnknown**)&pICmdText);
	if(FAILED(hr))
	{
		goto Exit;
	}

	hr = pICmdText->SetCommandText(DBGUID_SQL, pwszQuery); 
	if(FAILED(hr))
	{
		goto Exit;
	}

    hr = pICmdText->Execute(NULL, IID_NULL, NULL, NULL, NULL);

Exit:

	// Release interfaces
	//
	if(pICmdText)
	{
		pICmdText->Release();
	}

	if(pIDBCrtCmd)
	{
		pIDBCrtCmd->Release();
	}

	return hr;
}



////////////////////////////////////////////////////////////////////////////////
// Function: GetColumnOrdinal()
//
// Description: Returns column ordinal for column name.
//
// Parameters
//		pDBColumnInfo	- a pointer to Database column info
//		dwNumCols		- number of columns
//		pwszColName		- column name
//		pOrdinal		- column ordinal
//
// Returns: TRUE if succesfull
//
////////////////////////////////////////////////////////////////////////////////
BOOL CSqlLib::GetColumnOrdinal(DBCOLUMNINFO* pDBColumnInfo, DWORD dwNumCols, WCHAR* pwszColName, DWORD* pOrdinal, DWORD* pColumn)
{
	for(DWORD dwCol = 0; dwCol< dwNumCols; ++dwCol)
	{
		if(NULL != pDBColumnInfo[dwCol].pwszName)
		{
			if(0 == wcsicmp(pDBColumnInfo[dwCol].pwszName, pwszColName))
			{
				*pOrdinal = pDBColumnInfo[dwCol].iOrdinal;
				*pColumn = dwCol;
				return TRUE;
			}
		}
	}

	return FALSE;
}