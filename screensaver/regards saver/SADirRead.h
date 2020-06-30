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

// SADirRead.h: interface for the CSADirRead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SADIRREAD_H__5BC7B291_F166_11D4_9393_0050DABB534C__INCLUDED_)
#define AFX_SADIRREAD_H__5BC7B291_F166_11D4_9393_0050DABB534C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <windows.h>

#include <string>
#include <vector>
#include <Commctrl.h>
#include <io.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;

/**********************************************************************
   USE_WIN32_FINDFILE

   if you comment this next #define, the class will use the C 
   library "findfirst", "findnext", "findclose" functions. Otherwise,
   it will use the Win32 functions "FindFirstFile", "FindNextFile" and "FindClose".

   The Win32 functions can use UNC paths, the C lib functions can't.

**********************************************************************/
#define USE_WIN32_FINDFILE



//////////////////////////////////////////////////////////////////////

class CSADirRead  
{
public:
	void GetImageFile(const HWND &hWndList = NULL);
	void IncludeSubFolder(bool m_bValue);

	// directories
	struct CSADirEntry
	{
		CSADirEntry(){};
		CSADirEntry(const string &s)
		{
			m_sName = s;
		};
		string m_sName;
		string m_sNameOnly;
		unsigned int attrib;
		time_t time_create;
		time_t time_access;
		time_t time_write;
		unsigned __int64 size;
	};

	struct CSAFileEntry
	{
		string	m_sName;
		unsigned int attrib;
		time_t time_create;
		time_t time_access;
		time_t time_write;
		unsigned __int64 size;

	  static void MakeFiletime(unsigned __int64 inTime, _FILETIME &outTime)
	  {
		 memcpy(&outTime, &inTime, sizeof(_FILETIME));
	  }
	};


	CSADirRead();
	virtual ~CSADirRead();

	// recurse subdirectories?
	bool&	Recurse()	{m_bRecurse=!m_bRecurse;return m_bRecurse;}

	// first, get the dirs
	bool	GetDirs(const char * pDirPath, const HWND &hWndList = NULL);

	// then get the files in those dirs
	bool	GetFiles(const char *pFilemask, const HWND &hWndList = NULL);

	// remove all entries
	bool	ClearDirs();
	bool	ClearFiles();

	// if you want to sort the data
	enum {eSortNone, eSortAlpha, eSortWriteDate, eSortCreateDate, eSortAccessDate, eSortSize};
	bool	SortFiles(int iSortStyle, bool bReverse);

	//////////////////////////////////////////////////////////////////////


	typedef std::vector<CSADirEntry> SADirVector;

	SADirVector &Dirs()	{return m_dirs;}

	//////////////////////////////////////////////////////////////////////

	typedef std::vector<CSAFileEntry> SAFileVector;

	SAFileVector &Files()	{return m_files;}

	//////////////////////////////////////////////////////////////////////


	bool	m_bRecurse;
	int m_iNbNiveaux;

	static bool asc_alpha_dir_sort(const CSADirRead::CSADirEntry &a, const CSADirRead::CSADirEntry &b);
	static bool asc_alpha_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);
	static bool asc_write_date_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);
	static bool asc_create_date_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);
	static bool asc_access_date_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);
	static bool asc_size_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);
	static bool dsc_alpha_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);
	static bool dsc_write_date_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);
	static bool dsc_create_date_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);
	static bool dsc_access_date_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);
	static bool dsc_size_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);

protected:

	bool	GetSubDirs(SADirVector &dir_array, const string &path, const HWND & hWndList = NULL);
	UINT	FindFiles(const string & dir, const string & filter, const HWND & hWndList = NULL);
	void	FormatPath(string &path);
	
	virtual void InsertItem(HWND hWndListView, string m_stgFileName, string m_sName,const long &lSize,const long * lTimeWrite){};

	virtual bool TestFin() {return false;};

	void	AddFichierToVector(_finddata_t c_file,const string &baseName, const HWND &hWndList = NULL);
	void	AddDirectoryToVector(SADirVector &dir_array,_finddata_t c_file,const string &newPath, const HWND &hWndList = NULL);

	bool m_bIncludeSubFolder;
	
	
	string	m_sSourceDir;

	SADirVector m_dirs;
	SAFileVector m_files;

	long NbFiles;
	bool m_bAffichageImage;

};

#endif // !defined(AFX_SADIRREAD_H__5BC7B291_F166_11D4_9393_0050DABB534C__INCLUDED_)
