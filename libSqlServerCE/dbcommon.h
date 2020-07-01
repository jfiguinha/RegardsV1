/***********************************************************************
Copyright (c) 1999 - 2002, Microsoft Corporation
All Rights Reserved.
***********************************************************************/

#ifndef NORTHWIND_OLEDB_SAMPLE_QUERY_INCLUDED
#define NORTHWIND_OLEDB_SAMPLE_QUERY_INCLUDED	

#include "..\resource.h"

// Database REGARDS table name
//
const TCHAR DATABASE_REGARDS[]			= "regards.sdf";

// SQl to create and drop employee table
//
//const TCHAR SQL_CREATE_EMPLOYEES_INDEX[] = L"CREATE INDEX PK_Employees on Employees (EmployeeID ASC)";

const TCHAR SQL_CREATE_ICONEFILE_TABLE[] = "CREATE TABLE ICONEFILEFOLDER (FolderName NVARCHAR(512), NumIcone INT, Informations NVARCHAR(512), FullPathName NVARCHAR(512), TimeCreate NVARCHAR(32), Size NVARCHAR(20))";
const TCHAR SQL_DROP_ICONEFILE[] = "DROP TABLE ICONEFILEFOLDER";
//const TCHAR SQL_CREATE_ICONEFILE_INDEX[]	= "CREATE INDEX FolderName on ICONEFILEFOLDER (FolderName ASC)";

const TCHAR SQL_CREATE_ALBUMICONEFILE_TABLE[] = "CREATE TABLE ICONEFILEALBUM (AlbumName NVARCHAR(512), NumIcone INT, Informations NVARCHAR(512), FullPathName NVARCHAR(512), TimeCreate NVARCHAR(32), Size NVARCHAR(20), ImageSize INTEGER, ImageWith INTEGER, ImageHeight INTEGER, ImageDate INTEGER)";
const TCHAR SQL_DROP_ALBUMICONEFILE[] = "DROP TABLE ICONEFILEALBUM";

const TCHAR SQL_CREATE_CATALOGICONEFILE_TABLE[] = "CREATE TABLE ICONEFILECATALOG (CatalogName NVARCHAR(512), NumIcone INT, Informations NVARCHAR(512), FullPathName NVARCHAR(512), TimeCreate NVARCHAR(32), Size NVARCHAR(20))";
const TCHAR SQL_DROP_CATALOGICONEFILE[] = "DROP TABLE ICONEFILECATALOG";

const TCHAR SQL_CREATE_ALBUM_TABLE[] = "CREATE TABLE ALBUM (AlbumName NVARCHAR(512))";
const TCHAR SQL_DROP_ALBUM[] = "DROP TABLE ALBUM";

const TCHAR SQL_CREATE_CATALOG_TABLE[] = "CREATE TABLE CATALOG (CatalogName NVARCHAR(512))";
const TCHAR SQL_DROP_CATALOG[] = "DROP TABLE CATALOG";

const TCHAR SQL_CREATE_ALBUMATTRIBUT_TABLE[] = "CREATE TABLE ALBUMATTRIBUT (NumAttribut INT, Libelle NVARCHAR(255))";
const TCHAR SQL_DROP_ALBUMATTRIBUT[] = "DROP TABLE ALBUMATTRIBUT";

const TCHAR SQL_CREATE_CATALOGATTRIBUT_TABLE[] = "CREATE TABLE CATALOGATTRIBUT (NumAttribut INT, Libelle NVARCHAR(255))";
const TCHAR SQL_DROP_CATALOGATTRIBUT[] = "DROP TABLE CATALOGATTRIBUT";

const TCHAR SQL_CREATE_ALBUM_ICONEFILE_ATTRIBUT_TABLE[] = "CREATE TABLE ALBUM_ICONEFILE_ATTRIBUT (AlbumName NVARCHAR(512), FileName NVARCHAR(512), NumAttribut INT, ValueAttribut NVARCHAR(512))";
const TCHAR SQL_DROP_ALBUM_ICONEFILE_ATTRIBUT[] = "DROP TABLE ALBUM_ICONEFILE_ATTRIBUT";

const TCHAR SQL_CREATE_CATALOG_ICONEFILE_ATTRIBUT_TABLE[] = "CREATE TABLE CATALOG_ICONEFILE_ATTRIBUT (CatalogName NVARCHAR(512), FileName NVARCHAR(512), NumAttribut INT, ValueAttribut NVARCHAR(512))";
const TCHAR SQL_DROP_CATALOG_ICONEFILE_ATTRIBUT[] = "DROP TABLE CATALOG_ICONEFILE_ATTRIBUT";




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