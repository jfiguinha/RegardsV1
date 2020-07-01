#include <stdafx.h>
#include "SqlLibRegards.h"
#include "dbcommon.h"
#include "Shlwapi.h"
using namespace Regards::Sqlite;

CSqlLibRegards::CSqlLibRegards(const bool &readOnly, const bool &load_inmemory)
 : CSqlLib()
{
	this->readonly = readOnly;
	this->load_inmemory = load_inmemory;
}


CSqlLibRegards::~CSqlLibRegards()
{
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
bool CSqlLibRegards::InitDatabase(const string &lpFilename)
{
	int hr = -1;	// Error code reporting

	if (!PathFileExists(lpFilename.c_str()))
	{
		hr = CreateDatabase(lpFilename,load_inmemory);
	}
	else
	{
		hr = OpenConnection(lpFilename,false,load_inmemory);
		hr = ExecuteSQLWithNoResult("VACUUM;");
		hr = ExecuteSQLWithNoResult("PRAGMA auto_vacuum = FULL;");
	}

	return (hr != -1);
}

bool CSqlLibRegards::CheckVersion(const string &lpFilename)
{
    return S_OK;
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
bool CSqlLibRegards::CreateDatabase(const string &databasePath, const bool &load_inmemory)
{
	string libelle;
	string query;
	HRESULT hr;

	if (!OpenConnection(databasePath, false, load_inmemory))
		return false;

	// Create ICONEFILE table
		//
	hr = ExecuteSQLWithNoResult(SQL_CREATE_ICONEFILE_TABLE);
	if (FAILED(hr))
	{
		goto Exit;
	}

	// Create ALBUM ICONEFILE table
	//
	hr = ExecuteSQLWithNoResult(SQL_CREATE_ALBUMICONEFILE_TABLE);
	if (FAILED(hr))
	{
		goto Exit;
	}

	// Create CATALOG ICONEFILE table
	//
	hr = ExecuteSQLWithNoResult(SQL_CREATE_CATALOGICONEFILE_TABLE);
	if (FAILED(hr))
	{
		goto Exit;
	}

	// Create ALBUM table
	//
	hr = ExecuteSQLWithNoResult(SQL_CREATE_ALBUM_TABLE);
	if (FAILED(hr))
	{
		goto Exit;
	}

	// Create Catalog table
	//
	hr = ExecuteSQLWithNoResult(SQL_CREATE_CATALOG_TABLE);
	if (FAILED(hr))
	{
		goto Exit;
	}

	// Create ALBUM ATTRIBUT table
	//
	hr = ExecuteSQLWithNoResult(SQL_CREATE_ALBUMATTRIBUT_TABLE);
	if (FAILED(hr))
	{
		goto Exit;
	}

	// Create CATALOG ATTRIBUT table
	//
	hr = ExecuteSQLWithNoResult(SQL_CREATE_CATALOGATTRIBUT_TABLE);
	if (FAILED(hr))
	{
		goto Exit;
	}

	// Create ALBUM ICONEFILE ATTRIBUT table
	//
	hr = ExecuteSQLWithNoResult(SQL_CREATE_ALBUM_ICONEFILE_ATTRIBUT_TABLE);
	if (FAILED(hr))
	{
		goto Exit;
	}

	// Create CATALOG ICONEFILE ATTRIBUT table
	//
	hr = ExecuteSQLWithNoResult(SQL_CREATE_CATALOG_ICONEFILE_ATTRIBUT_TABLE);
	if (FAILED(hr))
	{
		goto Exit;
	}

Exit:


	return (hr != -1);
}
