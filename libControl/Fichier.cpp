// Fichier.cpp: implementation of the CFichier class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Fichier.h"
#include "LoadImage.h"
#include <SADirRead.h>
#include "objet.h"
#include <ibitmap.h>
#include <JpegSaving.h>
#include <LoadPicture.h>

void CFichier::GetFichierTime(const char * m_stgFilePath, char * DateTime,const int &iDateTimeSize)
{

	WORD m_modTime, m_modDate;
	WIN32_FIND_DATA FindData;
	SYSTEMTIME SystemTime;

	FindFirstFile( m_stgFilePath, &FindData );
	FileTimeToDosDateTime(&FindData.ftLastWriteTime, &m_modDate, &m_modTime);
	FileTimeToSystemTime(&FindData.ftLastWriteTime, &SystemTime);
	//DosDateTimeToVariantTime(m_modDate, m_modTime, &m_varfichier.m_dtDate);

	//Conversion de la date en string
	StringCchPrintf(DateTime,iDateTimeSize,"%d/%d/%d %d:%d:%d", SystemTime.wDay, SystemTime.wMonth, SystemTime.wYear,
		SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond);

}

int CFichier::DeleteDir(LPCTSTR lpszName)
{
	int ret=0;
	char name1[256];
	WIN32_FIND_DATA info;
    HANDLE hp;
    int n;
    char *cp;
	
	StringCchPrintf(name1,256, "%s\\*.*",lpszName);
    hp = FindFirstFile(name1,&info);
    if(!hp || hp==INVALID_HANDLE_VALUE)
        return(ret);
    do
    {
		cp = info.cFileName;
        if(cp[1]==0 && *cp=='.')
            continue;
        else if(cp[2]==0 && *cp=='.' && cp[1]=='.')
            continue;
        StringCchPrintf(name1,256,"%s\\%s",lpszName,info.cFileName);
		if(info.dwFileAttributes&FILE_ATTRIBUTE_READONLY)
		{
			SetFileAttributes(name1,info.dwFileAttributes&~FILE_ATTRIBUTE_READONLY);
		}
		if(info.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			//TakeOwnership(name1);
			//SetPermission(name1,"everyone",GENERIC_ALL);
			DeleteDir(name1);
		}
		else
		{
			//TakeOwnership(name1);
			//SetPermission(name1,"everyone",GENERIC_ALL);
			//_chmod(name1,_S_IREAD | _S_IWRITE);
			DeleteFile(name1);
		}

    }
    while(FindNextFile(hp,&info));
	FindClose(hp);
	if(info.dwFileAttributes&FILE_ATTRIBUTE_READONLY)
	{
		SetFileAttributes(lpszName,info.dwFileAttributes&~FILE_ATTRIBUTE_READONLY);
	}
	if(RemoveDirectory(lpszName))
	{
		printf("success\n");
		ret=1;
	}	
	else
	{
		printf("error %d\n",GetLastError());
	}
	return(ret);
}

int CFichier::DeleteFolder(const char * m_szFolder)
{
	CObjet m_cObjet;
	if(!m_cObjet.IsValidateFolder(m_szFolder))
		return 0;

	CSADirRead m_cCSADirReadLocal;
	//Obtention de l'ensemble des fichiers
	m_cCSADirReadLocal.m_bRecurse = true;
	SAFileVector m_files;

	//m_sSourceDir = m_szFolder;

	m_cCSADirReadLocal.ClearFiles();
	m_cCSADirReadLocal.GetDirs(m_szFolder);
	m_cCSADirReadLocal.GetFiles("*.*");

	m_files = m_cCSADirReadLocal.Files();

	SAFileVector::iterator fit;
	for(fit = m_files.begin();fit != m_files.end();fit++)
	{
		_chmod(fit->m_sName.c_str(),_S_IREAD | _S_IWRITE);
		if(DeleteFile(fit->m_sName.c_str()) == 0)
		{
			TakeOwnership(fit->m_sName.c_str());
			SetPermission(fit->m_sName.c_str(),"everyone",GENERIC_ALL);
			DeleteFile(fit->m_sName.c_str());
		}
	}

	if(RemoveDirectory(m_szFolder) == 0)
	{
		//DeleteDir(m_szFolder);
	}
	

	return 0;
}

int CFichier::DeleteFile(const char * m_szFilename)
{
	_chmod(m_szFilename,_S_IREAD | _S_IWRITE);
	return ::DeleteFile(m_szFilename);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFichier::CFichier()
{
	iPos = 1;
	lQuality = 10;
	m_cCSADirRead = new CSADirRead();
	m_IconeFileVector = NULL;
	dir[0] = '\0';
	m_bAscendant = false;
	iDataType = 0;
}

CFichier::~CFichier()
{
	if(m_cCSADirRead != NULL)
		delete m_cCSADirRead;
}

int CFichier::DeleteDirectory(LPCTSTR lpszName)
{
	int ret=0;
	char name1[256];
	WIN32_FIND_DATA info;
    HANDLE hp;
    char *cp;
	
	StringCchPrintf(name1,256, "%s\\*.*",lpszName);

    hp = FindFirstFile(name1,&info);

    if(!hp || hp==INVALID_HANDLE_VALUE)
        return(ret);
    do
    {
		cp = info.cFileName;
        if(cp[1]==0 && *cp=='.')
            continue;
        else if(cp[2]==0 && *cp=='.' && cp[1]=='.')
            continue;
        StringCchPrintf(name1,256,"%s\\%s",lpszName,info.cFileName);
		if(info.dwFileAttributes&FILE_ATTRIBUTE_READONLY)
		{
			SetFileAttributes(name1,info.dwFileAttributes&~FILE_ATTRIBUTE_READONLY);
		}
		if(info.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			TakeOwnership(name1);
			SetPermission(name1,"everyone",GENERIC_ALL);
			DeleteDirectory(name1);
		}
		else
		{
			TakeOwnership(name1);
			SetPermission(name1,"everyone",GENERIC_ALL);
			DeleteFile(name1);
		}

    }
    while(FindNextFile(hp,&info));
	FindClose(hp);
	if(info.dwFileAttributes&FILE_ATTRIBUTE_READONLY)
	{
		SetFileAttributes(lpszName,info.dwFileAttributes&~FILE_ATTRIBUTE_READONLY);
	}
	if(RemoveDirectory(lpszName))
	{
		ret=1;
	}	

	return(ret);
}

BOOL CFichier::TakeOwnership(LPCTSTR lpszFile)
{
	int file[256];
	DWORD description;
	SECURITY_DESCRIPTOR sd;
	SECURITY_INFORMATION info_owner=OWNER_SECURITY_INFORMATION;
		
	TOKEN_USER *owner = (TOKEN_USER*)file;
	HANDLE token;
	
	
	InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
	if(OpenProcessToken(GetCurrentProcess(),TOKEN_READ | TOKEN_ADJUST_PRIVILEGES,&token))
	{
		//To Get the Current Process Token & opening it to adjust the previleges
		if(SetPrivilege(token,SE_TAKE_OWNERSHIP_NAME,FALSE))
		{
			//Enabling the privilege
			if(GetTokenInformation(token,TokenUser,owner,sizeof(file),&description))
			{
				//Get the information on the opened token
				if(SetSecurityDescriptorOwner(&sd,owner->User.Sid,FALSE))
				{
					//replace any owner information present on the security descriptor
					if(SetFileSecurity(lpszFile,info_owner,&sd))
						return(TRUE);
				}
				//SetSecurityDescriptorOwner
			}
		}
	}

	SetPrivilege(token,SE_TAKE_OWNERSHIP_NAME,TRUE);//Disabling the set previlege

	return(FALSE);
}

BOOL CFichier::SetPrivilege(HANDLE hToken,LPCTSTR lpszPrivilege,BOOL bChange)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;
	BOOL bReturnValue = FALSE;

	if (lpszPrivilege != NULL && !bChange)
	{
		if (LookupPrivilegeValue( 
			NULL,            // lookup privilege on local system
			lpszPrivilege,   // privilege to lookup 
			&luid )) 
		{      // receives LUID of privilege
			tp.PrivilegeCount = 1;
			tp.Privileges[0].Luid = luid;
			tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		}
	}

	AdjustTokenPrivileges(hToken,bChange,&tp,
		sizeof(TOKEN_PRIVILEGES), 
		(PTOKEN_PRIVILEGES) NULL,
		(PDWORD) NULL);  
	// Call GetLastError to determine whether the function succeeded.
	
	if (GetLastError() == ERROR_SUCCESS) 
	{ 
		bReturnValue = TRUE;
	} 
	 
	return bReturnValue;
}

BOOL CFichier::SetPermission(LPCTSTR lpszFile, LPCTSTR lpszAccess, DWORD dwAccessMask)
{
	int buff[512];
	char domain[512];
	
	DWORD domain_size=sizeof(domain);
	DWORD acl_size;

	SECURITY_DESCRIPTOR sd1;
	SECURITY_INFORMATION info_dacl=DACL_SECURITY_INFORMATION;
	PSID sid = (PSID*)buff;
	ACL *acl = NULL;
	SID_NAME_USE sidname;
	DWORD sid_size=sizeof(buff);
	int iTypeAllocation = 1;

	InitializeSecurityDescriptor(&sd1,SECURITY_DESCRIPTOR_REVISION);
	//to get the SID 
	if(LookupAccountName(NULL,lpszAccess,sid,&sid_size,domain,&domain_size,&sidname))
	{
		acl_size=sizeof(ACL)+sizeof(ACCESS_ALLOWED_ACE)-sizeof(DWORD)+GetLengthSid(buff);
		acl = (ACL *)malloc(acl_size);
		if(acl == NULL)
		{
			//Probléme d'allocation mémoire
			//Essai d'allocation dans le tas
			iTypeAllocation = 2;
			acl = (ACL *) HeapAlloc (GetProcessHeap (), 0, acl_size);
			if (acl == NULL)
			{
				CObjet::ErreurInformation();
			}

		}

		//To calculate the size of an ACL, 
		InitializeAcl(acl,acl_size,ACL_REVISION);

		if(AddAccessAllowedAce(acl,ACL_REVISION,dwAccessMask,sid))
		{
			if(SetSecurityDescriptorDacl(&sd1,TRUE,acl,FALSE))
			{
				if(SetFileSecurity(lpszFile,info_dacl,&sd1))
					return(TRUE);
			}
		}
	}

	if(acl != NULL)
	{

		switch(iTypeAllocation)
		{
			case 2:
				HeapFree(GetProcessHeap (), 0,acl);
				break;

			default:
				free(acl);
		}
		
	}

	return(FALSE);
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

int CFichier::ChargerFichier(char *szFileName, int iSize, HWND &hwnd)
{
	static OPENFILENAME ofn ;

#ifdef ENGLISH


	static TCHAR szFilter[] = TEXT("Files Supported\0*.jpg;*.jpe;*.jpeg;*.jfif;*.jif;*.bmp;*.gif;*.tif;*.png\0")
						TEXT ("Files JPEG (*.JPG;*.JPEG;*.JPE;*.JFIF;*.JIF)\0*.jpg;*.jpeg;*.jpe;*.jfif;*.jif\0")
						TEXT ("Files BMP  (*.BMP)\0*.bmp\0")
						TEXT ("Files GIF  (*.GIF)\0*.gif\0")
						TEXT ("Files TIF (*.TIF)\0*.tif\0")
						TEXT ("Files PNG (*.PNG)\0*.png\0")
						//TEXT ("Fichiers PCX (*.PCX)\0*.pcx\0")
						TEXT ("All Files (*.*)\0*.*\0\0") ;
#endif
#ifdef SPANISH


	static TCHAR szFilter[] = TEXT("Archivos soportado\0*.jpg;*.jpe;*.jpeg;*.jfif;*.jif;*.bmp;*.gif;*.tif;*.png\0")
						TEXT ("Archivos JPEG (*.JPG;*.JPEG;*.JPE;*.JFIF;*.JIF)\0*.jpg;*.jpeg;*.jpe;*.jfif;*.jif\0")
						TEXT ("Archivos BMP  (*.BMP)\0*.bmp\0")
						TEXT ("Archivos GIF  (*.GIF)\0*.gif\0")
						TEXT ("Archivos TIF (*.TIF)\0*.tif\0")
						TEXT ("Archivos PNG (*.PNG)\0*.png\0")
						//TEXT ("Fichiers PCX (*.PCX)\0*.pcx\0")
						TEXT ("Todos los Archivos (*.*)\0*.*\0\0") ;
#endif
#ifdef FRENCH


	static TCHAR szFilter[] = TEXT("Fichiers Supportés\0*.jpg;*.jpe;*.jpeg;*.jfif;*.jif;*.bmp;*.gif;*.tif;*.png\0")
						TEXT ("Fichiers JPEG (*.JPG;*.JPEG;*.JPE;*.JFIF;*.JIF)\0*.jpg;*.jpeg;*.jpe;*.jfif;*.jif\0")
						TEXT ("Fichiers BMP  (*.BMP)\0*.bmp\0")
						TEXT ("Fichiers GIF  (*.GIF)\0*.gif\0")
						TEXT ("Fichiers TIF (*.TIF)\0*.tif\0")
						TEXT ("Fichiers PNG (*.PNG)\0*.png\0")
						//TEXT ("Fichiers PCX (*.PCX)\0*.pcx\0")
						TEXT ("Tous les fichiers (*.*)\0*.*\0\0") ;
#endif

	static TCHAR pstrTitleName[MAX_PATH];
	static TCHAR pstrFileName[MAX_PATH];
	
	ofn.lStructSize       = sizeof (OPENFILENAME) ;
	ofn.hwndOwner         = hwnd ;
	ofn.hInstance         = NULL ;
	ofn.lpstrFilter       = szFilter ;
	ofn.lpstrCustomFilter = NULL ;
	ofn.nMaxCustFilter    = 0 ;
	ofn.nFilterIndex      = 0 ;
	ofn.nMaxFile          = MAX_PATH ;
	ofn.lpstrFileTitle    = NULL ;          // Défini dans les fonctions Ouvrir et Fermer
	ofn.nMaxFileTitle     = MAX_PATH ;
	ofn.lpstrInitialDir   = NULL ;
	ofn.lpstrTitle        = NULL ;
	ofn.Flags             = 0 ;             // Défini dans les fonctions Ouvrir et Fermer
	ofn.nFileOffset       = 0 ;
	ofn.nFileExtension    = 0 ;
	ofn.lpstrDefExt       = TEXT ("BMP") ;
	ofn.lCustData         = 0L ;
	ofn.lpfnHook          = NULL ;
	ofn.lpTemplateName    = NULL ;
	ofn.lpstrFile         = pstrFileName ;
	ofn.lpstrFileTitle    = pstrTitleName ;
	ofn.Flags             = OFN_HIDEREADONLY | OFN_CREATEPROMPT ;

	if (!GetOpenFileName (&ofn))
		return 0;

	StringCchCopy(szFileName,MAX_PATH,pstrFileName);

	DefinirRepertoire(szFileName);

	return 1;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

int CFichier::SauverFichier(TCHAR *szFileName, CIBitmap &m_ciBitmap, HWND &hwnd, const HINSTANCE &hInst)
{
	static OPENFILENAME ofn;

#ifdef ENGLISH

	static TCHAR szFilter[] = TEXT ("Files BMP  (*.BMP)\0*.bmp\0")
							  TEXT ("Files JPG  (*.JPG)\0*.jpg\0")
							  TEXT ("Files TGA  (*.TGA)\0*.tga\0")
							  TEXT ("Files TIF  (*.TIF)\0*.tif\0")
							  TEXT ("Files PDF  (*.PDF)\0*.pdf\0");
#endif
#ifdef SPANISH

	static TCHAR szFilter[] = TEXT ("Archivos BMP  (*.BMP)\0*.bmp\0")
							  TEXT ("Archivos JPG  (*.JPG)\0*.jpg\0")
							  TEXT ("Archivos TGA  (*.TGA)\0*.tga\0")
							  TEXT ("Archivos TIF  (*.TIF)\0*.tif\0")
							  TEXT ("Archivos PDF  (*.PDF)\0*.pdf\0");
#endif
#ifdef FRENCH

	static TCHAR szFilter[] = TEXT ("Fichiers BMP  (*.BMP)\0*.bmp\0")
							  TEXT ("Fichiers JPG  (*.JPG)\0*.jpg\0")
							  TEXT ("Fichiers TGA  (*.TGA)\0*.tga\0")
							  TEXT ("Fichiers TIF  (*.TIF)\0*.tif\0")
							  TEXT ("Fichiers PDF  (*.PDF)\0*.pdf\0");
#endif


	static TCHAR pstrTitleName[MAX_PATH];
	static TCHAR pstrFileName[MAX_PATH];
	CLoadImage m_cChargementImage;
	TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];

	_splitpath_s(szFileName, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

	StringCchCopy(pstrFileName,MAX_PATH,fname);

	ofn.lStructSize       = sizeof (OPENFILENAME) ;
	ofn.hwndOwner         = hwnd ;
	ofn.hInstance         = NULL ;
	ofn.lpstrFilter       = szFilter ;
	ofn.lpstrCustomFilter = NULL ;
	ofn.nMaxCustFilter    = 0 ;
	ofn.nFilterIndex      = 0 ;
	ofn.nMaxFile          = MAX_PATH ;
	ofn.lpstrFileTitle    = NULL ;          // Défini dans les fonctions Ouvrir et Fermer
	ofn.nMaxFileTitle     = MAX_PATH ;
	ofn.lpstrInitialDir   = NULL ;
	ofn.lpstrTitle        = NULL ;
	ofn.Flags             = 0 ;             // Défini dans les fonctions Ouvrir et Fermer
	ofn.nFileOffset       = 0 ;
	ofn.nFileExtension    = 0 ;
	ofn.lpstrDefExt       = TEXT ("BMP") ;
	ofn.lCustData         = 0L ;
	ofn.lpfnHook          = NULL ;
	ofn.lpTemplateName    = NULL ;
	ofn.lpstrFile         = pstrFileName ;
	ofn.lpstrFileTitle    = pstrTitleName ;
	ofn.Flags             = OFN_OVERWRITEPROMPT;

	if (!GetSaveFileName(&ofn))
		return 0;

	_splitpath_s(pstrFileName, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

	if(_stricmp(ext,".bmp") == 0)
		CLoadPicture::SaveImage(CLoadPicture::BMP,m_ciBitmap,pstrFileName);

	if(_stricmp(ext,".tga") == 0)
		CLoadPicture::SaveImage(CLoadPicture::TGA,m_ciBitmap,pstrFileName);

	if(_stricmp(ext,".tif") == 0)
	{
		CLoadPicture::SaveImage(CLoadPicture::TIFF,m_ciBitmap,pstrFileName);
	}

	if(_stricmp(ext,".pdf") == 0)
	{
		CLoadPicture::SaveImage(CLoadPicture::PDF,m_ciBitmap,pstrFileName,2,0);
	}
	if(_stricmp(ext,".jpg") == 0)
	{
		//DialogBox(hInst, (LPCTSTR)IDD_JPGSAVE, hwnd, (DLGPROC)JpegSaving);

		CJpegSaving * m_cJpegSaving = new CJpegSaving();
		m_cJpegSaving->SetQuality(lQuality);
		m_cJpegSaving->CreateDlg(hInst, (LPCTSTR)IDD_JPGSAVE, hwnd);
		lQuality = m_cJpegSaving->GetQuality();

		if(lQuality > 0)
		{
			CLoadPicture::SaveImage(CLoadPicture::JPEG,m_ciBitmap,pstrFileName,0,lQuality);
		}
		delete m_cJpegSaving;
	}

	return 1;
}

void CFichier::AffichageImagePosition(char *szFileName, int iSize, int iPos)
{
	//fit = files.at(iPos);
	this->iPos = iPos;
	StringCchCopy(szFileName,iSize,files.at(iPos).m_sName.c_str());
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

int CFichier::AffichageImageSuivante(char *szFileName, int iSize)
{
	if(iPos > files.size())
	{
		iPos = 1;
		fit=files.begin();
	}

	switch(iDataType)
	{
		case 0:
			try
			{
				iPos++;
				fit++;
				if(fit == files.end())
				{
					fit=files.begin();
					iPos = 1;
				}
			}
			catch(...)
			{
				iPos = 1;
				fit=files.begin();
			}
			StringCchCopy(szFileName,iSize,(*fit).m_sName.c_str());
			break;
		case 1:
			try
			{
				iPos++;
				Liste++;
				if(Liste == m_IconeFileVector->end())
				{
					Liste=m_IconeFileVector->begin();
					iPos = 1;
				}
			}
			catch(...)
			{
				iPos = 1;
				Liste=m_IconeFileVector->begin();
			}
			StringCchCopy(szFileName,iSize,(*Liste).m_stgFullPathName);
			break;
	}
	return 1;
}

int CFichier::SupprimerFichier(char *szFileName,int iSize)
{
	switch(iDataType)
	{
		case 0:
			try
			{
				iPos--;
				files.erase(fit);
			}
			catch(...)
			{
				iPos = 1;
				fit=files.begin();
			}
			StringCchCopy(szFileName,MAX_PATH,(*fit).m_sName.c_str());
			break;
		case 1:
			try
			{
				iPos++;
				Liste++;
				m_IconeFileVector->erase(Liste);
				if(Liste == m_IconeFileVector->end())
				{
					Liste=m_IconeFileVector->begin();
					iPos = 1;
				}
			}
			catch(...)
			{
				iPos = 1;
				Liste=m_IconeFileVector->begin();
			}
			StringCchCopy(szFileName,MAX_PATH,(*Liste).m_stgFullPathName);
			break;
	}
	return 1;	
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

int CFichier::AffichageImagePrecedente(char *szFileName,int iSize)
{

	switch(iDataType)
	{
		case 0:
			try
			{
				iPos--;
				if(fit != files.begin())
					fit--;
				else
				{
					fit = files.end();
					fit--;	
					iPos = GetNbFiles();
				}
			}
			catch(...)
			{
				fit = files.end();
				fit--;	
				iPos = GetNbFiles() - 1;
			}
			StringCchCopy(szFileName,iSize,(*fit).m_sName.c_str());
			break;
		case 1:
			try
			{
				iPos--;
				if(Liste != m_IconeFileVector->begin())
					Liste--;
				else
				{
					Liste = m_IconeFileVector->end();
					Liste--;	
					iPos = GetNbFiles();
				}
			}
			catch(...)
			{
				Liste = m_IconeFileVector->end();
				Liste--;	
				iPos = GetNbFiles();
			}
			StringCchCopy(szFileName,iSize,(*Liste).m_stgFullPathName);
			break;
	}


	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

int CFichier::SetTrieFichier(const int &m_iTypeTrie)
{
	switch(m_iTypeTrie)
	{
		case 0:
			m_cCSADirRead->SortFiles(::CSADirRead::eSortAlpha,true);
			break;

		case 1:
			m_cCSADirRead->SortFiles(::CSADirRead::eSortAlpha,false);
			break;

		case 2:
			m_cCSADirRead->SortFiles(::CSADirRead::eSortSize,true);
			break;

		case 3:
			m_cCSADirRead->SortFiles(::CSADirRead::eSortSize,false);
			break;

		case 4:
			m_cCSADirRead->SortFiles(::CSADirRead::eSortCreateDate,true);
			break;

		case 5:
			m_cCSADirRead->SortFiles(::CSADirRead::eSortCreateDate,false);
			break;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CFichier::DefinirRepertoire(const char *szFileName, const int &m_iTypeTrie,const bool &m_bIncludeSubFolder, const bool &bRawFile)
{

	iDataType = 0;

	TCHAR temp[MAX_PATH];

	TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
	_splitpath_s(szFileName, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

	StringCchCopy(temp,MAX_PATH,drive);
	StringCbCat(temp,MAX_PATH,dir);
	StringCchCopy(dir,_MAX_DIR,temp);

	dir[strlen(dir) - 1] = '\0';

	m_cCSADirRead->ClearDirs();
	m_cCSADirRead->ClearFiles();

	if(!m_bIncludeSubFolder)
	{
		m_cCSADirRead->m_bRecurse = m_bIncludeSubFolder;
		m_cCSADirRead->GetDirs(dir);
		m_cCSADirRead->m_iNbNiveaux = 0;

#ifdef WMP9
		m_cCSADirRead->GetImageFile(NULL,false,true,bRawFile);
#else
		m_cCSADirRead->GetImageFile(NULL,false,false,bRawFile);
#endif
		

		switch(m_iTypeTrie)
		{
			case 0:
				m_cCSADirRead->SortFiles(::CSADirRead::eSortAlpha,false);
				break;

			case 1:
				m_cCSADirRead->SortFiles(::CSADirRead::eSortAlpha,true);
				break;

			case 2:
				m_cCSADirRead->SortFiles(::CSADirRead::eSortSize,false);
				break;

			case 3:
				m_cCSADirRead->SortFiles(::CSADirRead::eSortSize,true);
				break;

			case 4:
				m_cCSADirRead->SortFiles(::CSADirRead::eSortCreateDate,false);
				break;

			case 5:
				m_cCSADirRead->SortFiles(::CSADirRead::eSortCreateDate,true);
				break;
		}
	}
	else
	{
		m_cCSADirRead->m_bRecurse = m_bIncludeSubFolder;
		m_cCSADirRead->m_iNbNiveaux = 1;
		m_cCSADirRead->GetDirs(dir);

#ifdef WMP9
		m_cCSADirRead->GetImageFile(NULL,false,true);
#else
		m_cCSADirRead->GetImageFile(NULL,false);
#endif

	}

	files = m_cCSADirRead->Files();
	fit = files.begin();
	iPos = 1;


	if(strlen(fname) > 0)
	{
		//Recherche fichier dans la liste
		try
		{
			do
			{
				if(_stricmp(szFileName,(*fit).m_sName.c_str()) == 0)
					break;

				iPos++;
				fit++;

			}while(fit != files.end());
		}
		catch(...)
		{}
	}

}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CFichier::DefinirRepertoire(const char *szFileName, const int m_iTypeTrie, const char * m_cExtension)
{

	iDataType = 0;
	TCHAR temp[MAX_PATH];

	//_splitpath(szFileName, drive, dir, fname, ext);
	TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
	_splitpath_s(szFileName, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

	StringCchCopy(temp,MAX_PATH,drive);
	StringCbCat(temp,MAX_PATH,dir);
	StringCchCopy(dir,_MAX_DIR,temp);

	dir[strlen(dir) - 1] = '\0';

	m_cCSADirRead->ClearDirs();
	m_cCSADirRead->ClearFiles();
	m_cCSADirRead->GetDirs(dir);
	m_cCSADirRead->GetFiles(m_cExtension);

	switch(m_iTypeTrie)
	{
		case 0:
			m_cCSADirRead->SortFiles(::CSADirRead::eSortAlpha,false);
			break;

		case 1:
			m_cCSADirRead->SortFiles(::CSADirRead::eSortAlpha,true);
			break;

		case 2:
			m_cCSADirRead->SortFiles(::CSADirRead::eSortSize,false);
			break;

		case 3:
			m_cCSADirRead->SortFiles(::CSADirRead::eSortSize,true);
			break;

		case 4:
			m_cCSADirRead->SortFiles(::CSADirRead::eSortCreateDate,false);
			break;

		case 5:
			m_cCSADirRead->SortFiles(::CSADirRead::eSortCreateDate,true);
			break;
	}

	files = m_cCSADirRead->Files();
	fit = files.begin();
	//Recherche fichier dans la liste
	try
	{
		do
		{
			if(_stricmp(szFileName,(*fit).m_sName.c_str()) == 0)
				break;

			fit++;

		}while(fit != files.end());
	}
	catch(...)
	{}

}

int CFichier::GetNbFiles()
{
	switch(iDataType)
	{
		case 0:
			return files.size();	
			break;
		case 1:
			return m_IconeFileVector->size();	
			break;
	}

	return -1;
}

void CFichier::AffichageImagePremiere(char *szFileName, int iSize)
{
	switch(iDataType)
	{
		case 0:
			fit = files.begin();
			StringCchCopy(szFileName,iSize,(*fit).m_sName.c_str());
			break;
		case 1:
			Liste = m_IconeFileVector->begin();
			StringCchCopy(szFileName,iSize,(*Liste).m_stgFullPathName);
			break;
	}
}

void CFichier::AffichageImageDerniere(char *szFileName, int iSize)
{
	switch(iDataType)
	{
		case 0:
			fit = files.end();
			fit--;
			StringCchCopy(szFileName,iSize,(*fit).m_sName.c_str());
			break;
		case 1:
			Liste = m_IconeFileVector->end();
			Liste--;
			StringCchCopy(szFileName,iSize,(*Liste).m_stgFullPathName);
			break;
	}
}


void CFichier::DefinirListeImage(const char *szFileName, IconeFileVector * m_IconeFileLocalVector)
{
	m_IconeFileVector = m_IconeFileLocalVector;
	iDataType = 1;
	//Recherche fichier dans la liste
	try
	{
		Liste = m_IconeFileVector->begin();

		do
		{
			if(_stricmp(szFileName,(*Liste).m_stgFullPathName) == 0)
				break;

			Liste++;

		}while(Liste != m_IconeFileVector->end());
	}
	catch(...)
	{}
}

const int CFichier::GetPos()
{
	return iPos;
}
