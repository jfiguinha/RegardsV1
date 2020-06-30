#include "StdAfx.h"
#include "Employees.h"
#include "dbcommon.h"

typedef HRESULT (STDAPICALLTYPE* DllGetClassObjectPtr)(REFCLSID rclsid, REFIID riid, LPVOID * ppv);

CEmployees::CEmployees(void)
{
	HRESULT hr = NOERROR;
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if(FAILED(hr))
	{
		 MessageBox(NULL, L"COM Initialization Failure.", L"Employees", MB_OK);
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
		}
	}

}

CEmployees::~CEmployees(void)
{
	if(classfactory)
		classfactory.Release();

	if(sql_dll)
		FreeLibrary(sql_dll);

	CoUninitialize();
}

HWND CEmployees::Create(HWND hWndParent, HINSTANCE hInstance)
{
	return NULL;
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
HRESULT CEmployees::CreateDatabase()
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
	DeleteFile(DATABASE_NORTHWIND);


	if(classfactory == NULL)
		goto Exit;

	hr = classfactory->CreateInstance(0, IID_IDBInitialize, (void **)&pIDBInitialize);
	if(FAILED(hr))
	  goto Exit;


   	// Create an instance of the OLE DB Provider
	//
	/*
	hr = CoCreateInstance(	CLSID_SQLSERVERCE_3_0, 
							0, 
							CLSCTX_INPROC_SERVER, 
							IID_IDBInitialize, 
							(void**)&pIDBInitialize);
	if(FAILED(hr))
	{
		goto Exit;
	}*/


	// Initialize a property with name of database
	//
	dbprop[0].dwPropertyID		= DBPROP_INIT_DATASOURCE;
	dbprop[0].dwOptions			= DBPROPOPTIONS_REQUIRED;
	dbprop[0].vValue.vt			= VT_BSTR;
	dbprop[0].vValue.bstrVal	= SysAllocString(DATABASE_NORTHWIND);
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

	// Drop "Employees" table if it exists ignoring errors
	//
	ExecuteSQL(pICmdText, (LPWSTR)SQL_DROP_EMPLOYEES);

	// Create Employees table
	//
	hr = ExecuteSQL(pICmdText, (LPWSTR)SQL_CREATE_EMPLOYEES_TABLE);
	if(FAILED(hr))
	{
		goto Exit;
	}

	// Create Index
	// Note: The sample table has small amount of demo data, the index is created here.
	// In your application, to improve performance, index shoule be created after 
	// inserting initial data. 
	//
	hr = ExecuteSQL(pICmdText, (LPWSTR)SQL_CREATE_EMPLOYEES_INDEX);
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
// Function: OpenDatabase
//
// Description:	Open a connection to database
//
// Returns: NOERROR if succesfull
//
////////////////////////////////////////////////////////////////////////////////
HRESULT CEmployees::OpenDatabase()
{
    HRESULT			   	hr				= NOERROR;	// Error code reporting
	DBPROP				dbprop[1];					// property used in property set to initialize provider
	DBPROPSET			dbpropset[1];				// Property Set used to initialize provider

    IDBInitialize       *pIDBInitialize = NULL;		// Provider Interface Pointer
	IDBProperties       *pIDBProperties	= NULL;		// Provider Interface Pointer

	VariantInit(&dbprop[0].vValue);		

    // Create an instance of the OLE DB Provider
	//
	if(classfactory == NULL)
		goto Exit;

	hr = classfactory->CreateInstance(0, IID_IDBInitialize, (void **)&pIDBInitialize);
	if(FAILED(hr))
	  goto Exit;


	// Initialize a property with name of database
	//
    dbprop[0].dwPropertyID	= DBPROP_INIT_DATASOURCE;
	dbprop[0].dwOptions		= DBPROPOPTIONS_REQUIRED;
    dbprop[0].vValue.vt		= VT_BSTR;
    dbprop[0].vValue.bstrVal= SysAllocString(DATABASE_NORTHWIND);
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
HRESULT CEmployees::ExecuteSQL(ICommandText *pICmdText, WCHAR * pwszQuery)
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
