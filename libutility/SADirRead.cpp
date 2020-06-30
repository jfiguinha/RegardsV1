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

// SADirRead.cpp: implementation of the CSADirRead class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SADirRead.h"
#include "..\resource.h"
#include <algorithm>
#include "time.h"
extern HINSTANCE hInstance;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//extern HINSTANCE hInstance;

// some helper functions we'll use here
bool asc_alpha_dir_sort(const CSADirEntry &a, const CSADirEntry &b);
bool asc_write_date_file_sort(const CSAFileEntry &a, const CSAFileEntry &b);
bool asc_create_date_file_sort(const CSAFileEntry &a, const CSAFileEntry &b);
bool asc_access_date_file_sort(const CSAFileEntry &a, const CSAFileEntry &b);
bool asc_size_file_sort(const CSAFileEntry &a, const CSAFileEntry &b);
bool asc_alpha_file_sort(const CSAFileEntry &a, const CSAFileEntry &b);
bool dsc_write_date_file_sort(const CSAFileEntry &a, const CSAFileEntry &b);
bool dsc_create_date_file_sort(const CSAFileEntry &a, const CSAFileEntry &b);
bool dsc_access_date_file_sort(const CSAFileEntry &a, const CSAFileEntry &b);
bool dsc_size_file_sort(const CSAFileEntry &a, const CSAFileEntry &b);
bool dsc_alpha_file_sort(const CSAFileEntry &a, const CSAFileEntry &b);

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
//////////////////////////////////////////////////////////////////////

bool CSADirRead::GetDirs(const char * pDirPath, const HWND &hWndList)
{

	char Temp[MAX_PATH];

	StringCchCopy(Temp,MAX_PATH,pDirPath);

	struct _finddata_t  c_file;
	long fhandle;

	fhandle=_findfirst(pDirPath, &c_file );

	Temp[strlen(pDirPath) - strlen(c_file.name) - 1] = '\0';

	m_sSourceDir = Temp;

	AddDirectoryToVector(m_dirs,c_file,m_sSourceDir,hWndList);

	m_sSourceDir = pDirPath;

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
//////////////////////////////////////////////////////////////////////

bool CSADirRead::GetSubDirs(std::vector<CSADirEntry> &dir_array, const string &path, const HWND &hWndList)
{
	string newPath;

	string searchString;
	searchString = path;
	searchString+= "\\*.*";

	//AffichageDirectoryInformation(searchString, hInstance);

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
//////////////////////////////////////////////////////////////////////

bool CSADirRead::ClearFiles()
{
	m_files.clear();
	NbFiles = 0;
	return true;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

bool CSADirRead::ClearDirs()
{
	m_dirs.clear();
	NbFiles = 0;
	return true;
}

//////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////

void CSADirRead::AddFichierToVector(_finddata_t c_file,const string &baseName, const HWND &hWndList)
{
	string fileName = baseName;
	fileName += c_file.name;

	if(hWndList != NULL)
	{
		NbFiles++;
		//InsertItem(hWndList, c_file.name, fileName, c_file.size, &c_file.time_write);
	}
	else
	{
		CSAFileEntry t;
		t.m_sName = fileName;
		t.m_sNameOnly = c_file.name;
		t.time_write = c_file.time_write;
		t.time_create = c_file.time_create;
		t.time_access = c_file.time_access;
		t.size = c_file.size;
		m_files.push_back(t);
	}
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CSADirRead::AddDirectoryToVector(std::vector<CSADirEntry> &dir_array, _finddata_t c_file,const string &path, const HWND &hWndList)
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
			//InsertItem(hWndList, c_file.name, newPath, c_file.size, &c_file.time_write);
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
//////////////////////////////////////////////////////////////////////

UINT CSADirRead::FindFiles(const string & dir, const string & filter, const HWND &hWndList)
{
   // make sure the path ends in a single "\"
   string baseName = dir;
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
				// we only care about files, not subdirs
				if ((c_file.attrib & _A_SUBDIR)!=_A_SUBDIR) 
				{
					if(m_stgFilter.length() > 0)
					{
						errno_t err;
						string fileName = baseName;
						fileName += c_file.name;

						//Récupération de l'extension du fichier
						TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
						_splitpath_s(fileName.c_str(), drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);	

						err = _strlwr_s(ext,_MAX_EXT);

						int iPos = m_stgFilter.find(ext);

						if(iPos != -1)
						{
							AddFichierToVector(c_file,baseName,hWndList);
						}
					}
					else
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


bool CSADirRead::asc_alpha_dir_sort(const CSADirEntry &a, const CSADirEntry &b)
{
	return (_stricmp(a.m_sName.c_str(), b.m_sName.c_str()) < 0);
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::asc_alpha_file_sort(const CSAFileEntry &a, const CSAFileEntry &b)
{
	return (_stricmp(a.m_sName.c_str(), b.m_sName.c_str()) < 0);
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::asc_write_date_file_sort(const CSAFileEntry &a, const CSAFileEntry &b)
{
#ifndef USE_WIN32_FINDFILE
	return (difftime(a.time_write, b.time_write) < 0);
#else
	return (difftime(a.time_create, b.time_create) < 0);
#endif
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::asc_create_date_file_sort(const CSAFileEntry &a, const CSAFileEntry &b)
{
#ifndef USE_WIN32_FINDFILE
	return (difftime(a.time_create, b.time_create) < 0);
#else
	return (difftime(a.time_create, b.time_create) < 0);
#endif
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::asc_access_date_file_sort(const CSAFileEntry &a, const CSAFileEntry &b)
{
#ifndef USE_WIN32_FINDFILE
	return (difftime(a.time_access, b.time_access) < 0);
#else
	return (difftime(a.time_create, b.time_create) < 0);
   //return (CompareFileTime(&a.time_access, &b.time_access) < 0);
#endif
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::asc_size_file_sort(const CSAFileEntry &a, const CSAFileEntry &b)
{
	return (a.size < b.size);
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::dsc_alpha_file_sort(const CSAFileEntry &a, const CSAFileEntry &b)
{
	return (_stricmp(a.m_sName.c_str(), b.m_sName.c_str()) > 0);
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::dsc_write_date_file_sort(const CSAFileEntry &a, const CSAFileEntry &b)
{
#ifndef USE_WIN32_FINDFILE
	return (difftime(a.time_write, b.time_write) > 0);
#else
	return (difftime(a.time_create, b.time_create) > 0);
   //return (CompareFileTime(&a.time_write, &b.time_write) > 0);
#endif
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::dsc_create_date_file_sort(const CSAFileEntry &a, const CSAFileEntry &b)
{
#ifndef USE_WIN32_FINDFILE
	return (difftime(a.time_create, b.time_create) > 0);
#else
	return (difftime(a.time_create, b.time_create) > 0);
#endif
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::dsc_access_date_file_sort(const CSAFileEntry &a, const CSAFileEntry &b)
{
#ifndef USE_WIN32_FINDFILE
	return (difftime(a.time_access, b.time_access) > 0);
#else
	return (difftime(a.time_access, b.time_access) > 0);
#endif
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::dsc_size_file_sort(const CSAFileEntry &a, const CSAFileEntry &b)
{
	return (a.size > b.size);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CSADirRead::IncludeSubFolder(const bool &m_bValue)
{
	m_bIncludeSubFolder = m_bValue;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CSADirRead::GetImageFile(const HWND &hWndList,const bool &m_bTrie,const bool &m_bMultimedia,const bool &bRawFile,const bool &m_bVideo)
{

	char clpCaption[255];

	if(m_bMultimedia)
	{
		if(bRawFile)
		{
			LoadString(hInstance,IDS_STGIMPORTPICTURE,clpCaption,255);
		}
		else
		{
			LoadString(hInstance,IDS_PICTUREFORMATMULTIWITHOUTRAW,clpCaption,255);
		}
		
	}
	else
	{
		if(bRawFile)
			LoadString(hInstance,IDS_PICTUREFORMAT,clpCaption,255);
		else
			LoadString(hInstance,IDS_PICTUREFORMATWITHOUTRAW,clpCaption,255);
	}

	m_stgFilter = clpCaption;

	if(m_bVideo)
	{
		LoadString(hInstance,IDS_VIDEOFORMAT,clpCaption,255);
		m_stgFilter += clpCaption;
	}

	GetFiles("*.*");

	m_stgFilter = "";

	if(m_bTrie)
		SortFiles(::CSADirRead::eSortAlpha,false);
}
