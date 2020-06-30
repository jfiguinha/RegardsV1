/*********************************************************************

   SADirRead

   Copyright (C) 2001 Smaller Animals Software, Inc.

   This software is provided 'as-is', without any express or implied
   warranty.  In no event will the authors be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.

   3. This notice may not be removed or altered from any source distribution.

   http://www.smalleranimals.com
   smallest@smalleranimals.com

**********************************************************************/

/**********************************************************************

  Sample use:
 
#include "SADirRead.h"

...

	CSADirRead dr;

	dr.Recurse() = bScanSub;    // scan subdirs ?

	dr.ClearDirs();         // start clean
	dr.GetDirs("c:\\temp"); // get all folders under c:\temp

	// dump the directory list to the debug trace window:

	// get the dir array
	CSADirRead::SADirVector &dirs = dr.Dirs();

	// loop over it
	for (CSADirRead::SADirVector::const_iterator dit = dirs.begin(); dit!=dirs.end(); dit++)
	{
		TRACE("%s\n", (*dit).m_sName);
	}

	dr.ClearFiles();        // start clean

	dr.GetFiles("*.jpg");   // get all *.JPG files in c:\temp and below

	// get the file array
	CSADirRead::SAFileVector &files = dr.Files();   

	// dump it...
	for (CSADirRead::SAFileVector::const_iterator fit = files.begin(); fit!=files.end(); fit++)
	{
		TRACE("%s\n", (*fit).m_sName);
	}

	dr.ClearFiles();		// start clean, again

	dr.GetFiles("*.txt");	// get all *.txt files in c:\temp and below

	// dump them, too
	for (fit = files.begin(); fit!=files.end(); fit++)
	{
		TRACE("%s : %d bytes\n", (*fit).m_sName, (*fit).size);
	}

**********************************************************************/

// SADirRead.cpp: implementation of the CSADirRead class.
//
//////////////////////////////////////////////////////////////////////

#include "SADirRead.h"

#include <time.h>
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// some helper functions we'll use here
bool asc_alpha_dir_sort(const CSADirRead::CSADirEntry &a, const CSADirRead::CSADirEntry &b);
bool asc_write_date_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);
bool asc_create_date_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);
bool asc_access_date_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);
bool asc_size_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);
bool asc_alpha_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);
bool dsc_write_date_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);
bool dsc_create_date_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);
bool dsc_access_date_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);
bool dsc_size_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);
bool dsc_alpha_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSADirRead::CSADirRead()
{
	m_bRecurse = false;
	m_bIncludeSubFolder = false;
	m_iNbNiveaux = 0;
}

//////////////////////////////////////////////////////////////////////

CSADirRead::~CSADirRead()
{

}


//////////////////////////////////////////////////////////////////////

bool CSADirRead::GetDirs(const char * pDirPath, const HWND &hWndList)
{
	m_sSourceDir = pDirPath;

	//FormatPath(m_sSourceDir);

	// find tree starting from pDirPath
	if (m_bRecurse)
	{
		GetSubDirs(m_dirs, m_sSourceDir,hWndList);
	}
	else
	{
		// add the main directory. 
		m_dirs.push_back(CSADirEntry(m_sSourceDir));
	}

	return true;	
}


//////////////////////////////////////////////////////////////////////

bool CSADirRead::GetSubDirs(std::vector<CSADirRead::CSADirEntry> &dir_array, const string &path, const HWND &hWndList)
{
	string newPath;

	string searchString;
	searchString = path;
	searchString+= "\\*.*";

	try 
	{
      struct _finddata_t  c_file;
      long fhandle;

      if ((fhandle=_findfirst( searchString.c_str(), &c_file ))!=-1) 
      {
         // we only care about subdirs
         if ((c_file.attrib & _A_SUBDIR)==_A_SUBDIR) 
         {
            // add c_file.name to the string array
           // CSADirEntry t;
            // we'll handle parents on our own
            if ((strcmp(c_file.name, ".")!=0) && (strcmp(c_file.name, "..")!=0)) 
            {
				AddDirectoryToVector(dir_array,c_file,path,hWndList);

			   if(m_iNbNiveaux)
			   {
					newPath = path;
					newPath += "\\";
					newPath += c_file.name;
					//Test si sous-répertoire
					GetSubDirs(dir_array,newPath,hWndList);
			   }
            }
         }

		  if(TestFin())
			  return false;

         
         // find the rest of them	
         while(_findnext( fhandle, &c_file ) == 0 ) 
         {
            //CSADirEntry t;
            if ((c_file.attrib & _A_SUBDIR)==_A_SUBDIR) 
            {
               // we'll handle parents on our own
               if ((strcmp(c_file.name, ".")!=0) && (strcmp(c_file.name, "..")!=0)) 
               {
				   AddDirectoryToVector(dir_array,c_file,path,hWndList);

				   if(m_iNbNiveaux)
				   {
						newPath = path;
						newPath += "\\";
						newPath += c_file.name;
						//Test si sous-répertoire
						GetSubDirs(dir_array,newPath,hWndList);
				   }
               }
            }

		  if(TestFin())
			  return false;

         }
         _findclose(fhandle);
      }
	} 
	catch (...) 
	{
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::ClearFiles()
{
	m_files.clear();
	NbFiles = 0;
	return true;
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::ClearDirs()
{
	m_dirs.clear();
	NbFiles = 0;
	return true;
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::GetFiles(const char *pFilemask, const HWND &hWndList)
{
	// get the files in each of our directories

	if(m_bRecurse)
	{
		for (std::vector<CSADirEntry>::iterator it = m_dirs.begin();it!=m_dirs.end(); it++) 
		{
			string curDir = (*it).m_sName;

		  // sanity check
			if (curDir.empty())
			{
				continue;
			}
			
			if (!FindFiles(curDir, pFilemask,hWndList))
			{
				return false;
			}
		}
	}
	else
	{
		if (m_sSourceDir.empty())
		{
			return false;
		}
		
		if (!FindFiles(m_sSourceDir, pFilemask,hWndList))
		{
			return false;
		}
	}

	return true;
}	

//////////////////////////////////////////////////////////////////////

bool CSADirRead::SortFiles(int iSortStyle, bool bReverse)
{
	switch (iSortStyle)
	{
	default:
		break;
	case eSortAlpha:
		std::sort(m_files.begin(), m_files.end(), (bReverse ? dsc_alpha_file_sort : asc_alpha_file_sort));
		break;
	case eSortSize:
		std::sort(m_files.begin(), m_files.end(), (bReverse ? dsc_size_file_sort : asc_size_file_sort));
		break;
	case eSortWriteDate:
		std::sort(m_files.begin(), m_files.end(), (bReverse ? dsc_write_date_file_sort : asc_write_date_file_sort));
		break;
	case eSortAccessDate:
		std::sort(m_files.begin(), m_files.end(), (bReverse ? dsc_access_date_file_sort : asc_access_date_file_sort));
		break;
	case eSortCreateDate:
		std::sort(m_files.begin(), m_files.end(), (bReverse ? dsc_create_date_file_sort : asc_create_date_file_sort));
		break;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////

// chop off trailing "\"
void CSADirRead::FormatPath(string &path)
{
	/*
	string inPath = path;
	inPath.TrimRight();
	string tmp;
	
	int iLastSlashPos = inPath.ReverseFind('\\');
	if (iLastSlashPos == -1)
	{
		iLastSlashPos = inPath.ReverseFind('/');
	}
	
	if (iLastSlashPos!=-1) 
	{
		if (iLastSlashPos==inPath.length()-1) 
		{
			path = inPath.Left(iLastSlashPos);

			FormatPath(path); // in case the incoming path is "C:\temp\\\..."
		}
	} */
}

void CSADirRead::AddFichierToVector(_finddata_t c_file,const string &baseName, const HWND &hWndList)
{
	string fileName = baseName;
	fileName += c_file.name;

	if(hWndList != NULL)
	{
		NbFiles++;
		InsertItem(hWndList, c_file.name, fileName, c_file.size, &c_file.time_write);
	}
	else
	{
		CSAFileEntry t;
		t.m_sName = fileName;
		t.time_write = c_file.time_write;
		t.time_create = c_file.time_create;
		t.time_access = c_file.time_access;
		t.size = c_file.size;
		m_files.push_back(t);
	}
}

void CSADirRead::AddDirectoryToVector(std::vector<CSADirRead::CSADirEntry> &dir_array, _finddata_t c_file,const string &path, const HWND &hWndList)
{
	string newPath = path;
	newPath+= "\\";
	newPath+= c_file.name;

	if(strcmp(path.c_str(),"") == 0)
		return;

	try
	{
		if(hWndList != NULL)
		{
			NbFiles++;
			InsertItem(hWndList, c_file.name, newPath, c_file.size, &c_file.time_write);
		}
		else
		{
			CSADirEntry t;

			t.m_sName = newPath;
			t.m_sNameOnly = c_file.name;
			t.time_write = c_file.time_write;
			t.time_create = c_file.time_create;
			t.time_access = c_file.time_access;
			t.size = c_file.size;

			dir_array.push_back(t);
		}
	}
	catch(...)
	{}
}
//////////////////////////////////////////////////////////////////////

UINT CSADirRead::FindFiles(const string & dir, const string & filter, const HWND &hWndList)
{
   // make sure the path ends in a single "\"
   string baseName = dir;
   FormatPath(baseName);
   baseName+='\\';
   
   string fullPath = baseName;
   fullPath += filter;
   
   string fileName;
   
   try 
   {
      // find first file in current directory
      struct _finddata_t  c_file;
      long fhandle;
      
      if ((fhandle=_findfirst( fullPath.c_str(), &c_file ))!=-1) 
      {
		  //CSAFileEntry t;

		  if(m_bIncludeSubFolder)
		  {
			  AddFichierToVector(c_file,baseName,hWndList);
		  }
		  else
		  {
			// we only care about files, not subdirs
			if ((c_file.attrib & _A_SUBDIR)!=_A_SUBDIR) 
			{
				AddFichierToVector(c_file,baseName,hWndList);
			}
		  }

		  if(TestFin())
			  return false;
         
         // find the rest of them	
         while(_findnext( fhandle, &c_file ) == 0 ) 
         {
			  if(m_bIncludeSubFolder)
			  {
				  AddFichierToVector(c_file,baseName,hWndList);
			  }
			  else
			  {
				if ((c_file.attrib & _A_SUBDIR)!=_A_SUBDIR) 
				{
					AddFichierToVector(c_file,baseName,hWndList);
				}
			  }

			  if(TestFin())
				  return false;
         }
         _findclose(fhandle);
      }
   } 
   catch (...) 
   {
      return false;
   }
   
   return true;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
/*
	these are the various sorting functions we use
*/
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


bool CSADirRead::asc_alpha_dir_sort(const CSADirRead::CSADirEntry &a, const CSADirRead::CSADirEntry &b)
{
	return (stricmp(a.m_sName.c_str(), b.m_sName.c_str()) < 0);
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::asc_alpha_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b)
{
	return (stricmp(a.m_sName.c_str(), b.m_sName.c_str()) < 0);
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::asc_write_date_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b)
{
#ifndef USE_WIN32_FINDFILE
	return (difftime(a.time_write, b.time_write) < 0);
#else
	return (difftime(a.time_create, b.time_create) < 0);
   //return (CompareFileTime(&a.time_write, &b.time_write) < 0);
#endif
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::asc_create_date_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b)
{
#ifndef USE_WIN32_FINDFILE
	return (difftime(a.time_create, b.time_create) < 0);
#else
	return (difftime(a.time_create, b.time_create) < 0);
   //return (CompareFileTime(&a.time_create, &b.time_create) < 0);
#endif
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::asc_access_date_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b)
{
#ifndef USE_WIN32_FINDFILE
	return (difftime(a.time_access, b.time_access) < 0);
#else
	return (difftime(a.time_create, b.time_create) < 0);
   //return (CompareFileTime(&a.time_access, &b.time_access) < 0);
#endif
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::asc_size_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b)
{
	return (a.size < b.size);
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::dsc_alpha_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b)
{
	return (stricmp(a.m_sName.c_str(), b.m_sName.c_str()) > 0);
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::dsc_write_date_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b)
{
#ifndef USE_WIN32_FINDFILE
	return (difftime(a.time_write, b.time_write) > 0);
#else
	return (difftime(a.time_create, b.time_create) > 0);
   //return (CompareFileTime(&a.time_write, &b.time_write) > 0);
#endif
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::dsc_create_date_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b)
{
#ifndef USE_WIN32_FINDFILE
	return (difftime(a.time_create, b.time_create) > 0);
#else
	return (difftime(a.time_create, b.time_create) > 0);
   //return (CompareFileTime(&a.time_create, &b.time_create) > 0);
#endif
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::dsc_access_date_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b)
{
#ifndef USE_WIN32_FINDFILE
	return (difftime(a.time_access, b.time_access) > 0);
#else
	return (difftime(a.time_access, b.time_access) > 0);
   //return (CompareFileTime(&a.time_access, &b.time_access) > 0);
#endif
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::dsc_size_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b)
{
	return (a.size > b.size);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CSADirRead::IncludeSubFolder(bool m_bValue)
{
	m_bIncludeSubFolder = m_bValue;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
/*
void CSADirRead::InsertItem(HWND hWndListView, string m_stgFileName, string m_sName,const long &lSize,const long * lTimeWrite)
{
	int dIndex;
	
	CHAR szLabel[255];
	struct tm *newtime;
	CObjet m_cObjet;
	char ValueTime[255];
	LVITEM lvItem;

	ZeroMemory(&lvItem,sizeof(LVITEM));


	lvItem.mask = LVIF_STATE | LVIF_IMAGE | LVIF_INDENT | LVIF_PARAM | LVIF_TEXT;
	lvItem.iItem = NbFiles;
	lvItem.lParam = (LPARAM)NbFiles;
	lvItem.iSubItem = 0;
	lvItem.pszText = (char *)m_stgFileName.c_str();
	lvItem.iImage = m_cObjet.GetSysImgIcon(m_sName);

	dIndex = ListView_InsertItem(hWndListView,&lvItem);

	//Ajout de l'information sur la Taille

	lvItem.mask = TVIF_TEXT;
	lvItem.iItem = dIndex;
	lvItem.iSubItem = 1;
	lvItem.pszText = szLabel;
	m_cObjet.GetInfosTaille(lSize,szLabel);

	ListView_SetItem(hWndListView,&lvItem);

	newtime = localtime(lTimeWrite ); 
	lvItem.mask = TVIF_TEXT;
	lvItem.iItem = dIndex;
	lvItem.iSubItem = 2;

	strftime(ValueTime,255,"%d/%m/%Y %H:%M:%S",newtime);

	lvItem.pszText = ValueTime;

	ListView_SetItem(hWndListView,&lvItem);

	lvItem.mask = TVIF_TEXT;
	lvItem.iItem = dIndex;
	lvItem.iSubItem = 3;
	lvItem.pszText = "";

	ListView_SetItem(hWndListView,&lvItem);
}*/

void CSADirRead::GetImageFile(const HWND &hWndList)
{
	GetFiles("*.jpg",hWndList);
	GetFiles("*.jpe",hWndList);
	GetFiles("*.jpeg",hWndList);
	GetFiles("*.jif",hWndList);
	GetFiles("*.jfif",hWndList);
	GetFiles("*.gif",hWndList);
	GetFiles("*.tif",hWndList);
	GetFiles("*.bmp",hWndList);
	GetFiles("*.png",hWndList);
	GetFiles("*.tga",hWndList);
	GetFiles("*.pcx",hWndList);
	GetFiles("*.pcd",hWndList);

	SortFiles(::CSADirRead::eSortAlpha,false);
}
