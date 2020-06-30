/***********************************************************************
Copyright (c) 1999 - 2002, Microsoft Corporation
All Rights Reserved.
***********************************************************************/

#ifndef NORTHWIND_OLEDB_SAMPLE_QUERY_INCLUDED
#define NORTHWIND_OLEDB_SAMPLE_QUERY_INCLUDED	

#include "..\resource.h"

struct COLUMNDATA
{
	DWORD     dwLength;      // length of data
	DBSTATUS   dwStatus;      // status of column
	SHORT      wPadding;
	BYTE       bData[1];      // variable-length data
};

// Database REGARDS table name
//
const WCHAR DATABASE_REGARDS[]			= L"regards.sdf";

// SQl to create and drop employee table
//
//const WCHAR SQL_CREATE_EMPLOYEES_INDEX[] = L"CREATE INDEX PK_Employees on Employees (EmployeeID ASC)";

const WCHAR SQL_CREATE_ICONEFILE_TABLE[] = L"CREATE TABLE ICONEFILEFOLDER (FolderName NVARCHAR(512), NumIcone INT, Informations NVARCHAR(512), FullPathName NVARCHAR(512), TimeCreate NVARCHAR(32), Size NVARCHAR(20))";
const WCHAR SQL_DROP_ICONEFILE[] = L"DROP TABLE ICONEFILEFOLDER";
//const WCHAR SQL_CREATE_ICONEFILE_INDEX[]	= L"CREATE INDEX FolderName on ICONEFILEFOLDER (FolderName ASC)";

const WCHAR SQL_CREATE_ALBUMICONEFILE_TABLE[] = L"CREATE TABLE ICONEFILEALBUM (AlbumName NVARCHAR(512), NumIcone INT, Informations NVARCHAR(512), FullPathName NVARCHAR(512), TimeCreate NVARCHAR(32), Size NVARCHAR(20), ImageSize INTEGER, ImageWith INTEGER, ImageHeight INTEGER, ImageDate INTEGER)";
const WCHAR SQL_DROP_ALBUMICONEFILE[] = L"DROP TABLE ICONEFILEALBUM";

const WCHAR SQL_CREATE_CATALOGICONEFILE_TABLE[] = L"CREATE TABLE ICONEFILECATALOG (CatalogName NVARCHAR(512), NumIcone INT, Informations NVARCHAR(512), FullPathName NVARCHAR(512), TimeCreate NVARCHAR(32), Size NVARCHAR(20))";
const WCHAR SQL_DROP_CATALOGICONEFILE[] = L"DROP TABLE ICONEFILECATALOG";

const WCHAR SQL_CREATE_ALBUM_TABLE[] = L"CREATE TABLE ALBUM (AlbumName NVARCHAR(512))";
const WCHAR SQL_DROP_ALBUM[] = L"DROP TABLE ALBUM";

const WCHAR SQL_CREATE_CATALOG_TABLE[] = L"CREATE TABLE CATALOG (CatalogName NVARCHAR(512))";
const WCHAR SQL_DROP_CATALOG[] = L"DROP TABLE CATALOG";

const WCHAR SQL_CREATE_ALBUMATTRIBUT_TABLE[] = L"CREATE TABLE ALBUMATTRIBUT (NumAttribut INT, Libelle NVARCHAR(255))";
const WCHAR SQL_DROP_ALBUMATTRIBUT[] = L"DROP TABLE ALBUMATTRIBUT";

const WCHAR SQL_CREATE_CATALOGATTRIBUT_TABLE[] = L"CREATE TABLE CATALOGATTRIBUT (NumAttribut INT, Libelle NVARCHAR(255))";
const WCHAR SQL_DROP_CATALOGATTRIBUT[] = L"DROP TABLE CATALOGATTRIBUT";

const WCHAR SQL_CREATE_ALBUM_ICONEFILE_ATTRIBUT_TABLE[] = L"CREATE TABLE ALBUM_ICONEFILE_ATTRIBUT (AlbumName NVARCHAR(512), FileName NVARCHAR(512), NumAttribut INT, ValueAttribut NVARCHAR(512))";
const WCHAR SQL_DROP_ALBUM_ICONEFILE_ATTRIBUT[] = L"DROP TABLE ALBUM_ICONEFILE_ATTRIBUT";

const WCHAR SQL_CREATE_CATALOG_ICONEFILE_ATTRIBUT_TABLE[] = L"CREATE TABLE CATALOG_ICONEFILE_ATTRIBUT (CatalogName NVARCHAR(512), FileName NVARCHAR(512), NumAttribut INT, ValueAttribut NVARCHAR(512))";
const WCHAR SQL_DROP_CATALOG_ICONEFILE_ATTRIBUT[] = L"DROP TABLE CATALOG_ICONEFILE_ATTRIBUT";




// constants
//
const int MENU_HEIGHT				= 26;
const int MAX_LOADSTRING			= 100;		// Maximun lenght for resource strings
const int MAX_DATABASENAME_LENGHT	= 256;		// Maximun lenght for a database name
const int MAX_TABLENAME_LENGHT		= 256;		// Maximun lenght for a table name
const int MAX_INDEXNAME_LENGHT		= 256;		// Maximun lenght for a index name
const int MAX_QUERY_LENGTH			= 256;		// Maximun lenght for a query
const int COLUMN_ALIGNVAL           = 8;		// Column alignment value
 
// Return a properly aligned size
//
inline DWORD ROUND_UP(DWORD dwSize, DWORD dwAmount) { return ((dwSize + (dwAmount-1)) & ~(dwAmount-1));}

#endif