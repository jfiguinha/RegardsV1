// Fichier.cpp: implementation of the CFichier class.
//
//////////////////////////////////////////////////////////////////////

#include "Fichier.h"
#include "resource.h"
#include <Commctrl.h>

long CFichier::lQuality = 10;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFichier::CFichier()
{

}

CFichier::~CFichier()
{

}

int CFichier::DeleteDirectory(LPCTSTR lpszName)
{
	int ret=0;
	char name1[256];
	WIN32_FIND_DATA info;
    HANDLE hp;
    char *cp;
	
	sprintf(name1, "%s\\*.*",lpszName);

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
        sprintf(name1,"%s\\%s",lpszName,info.cFileName);
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
	ACL *acl;
	SID_NAME_USE sidname;
	DWORD sid_size=sizeof(buff);
	
	InitializeSecurityDescriptor(&sd1,SECURITY_DESCRIPTOR_REVISION);
	//to get the SID 
	if(LookupAccountName(NULL,lpszAccess,sid,&sid_size,domain,&domain_size,&sidname))
	{
		acl_size=sizeof(ACL)+sizeof(ACCESS_ALLOWED_ACE)-sizeof(DWORD)+GetLengthSid(buff);
		acl = (ACL *)malloc(acl_size);
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

	free(acl);
	return(FALSE);
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

int CFichier::ChargerFichier(TCHAR *szFileName, HWND &hwnd)
{
	static OPENFILENAME ofn ;
	static TCHAR szFilter[] = TEXT("Fichiers Supportés\0*.jpg;*.jpe;*.jpeg;*.jfif;*.jif;*.bmp;*.gif;*.tif;*.png\0")
						TEXT ("Fichiers JPEG (*.JPG;*.JPEG;*.JPE;*.JFIF;*.JIF)\0*.jpg;*.jpeg;*.jpe;*.jfif;*.jif\0")
						TEXT ("Fichiers BMP  (*.BMP)\0*.bmp\0")
						TEXT ("Fichiers GIF  (*.GIF)\0*.gif\0")
						TEXT ("Fichiers TIF (*.TIF)\0*.tif\0")
						TEXT ("Fichiers PNG (*.PNG)\0*.png\0")
						//TEXT ("Fichiers PCX (*.PCX)\0*.pcx\0")
						TEXT ("Tous les fichiers (*.*)\0*.*\0\0") ;
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

	strcpy(szFileName,pstrFileName);

	DefinirRepertoire(szFileName);

	return 1;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

int CFichier::AffichageImageSuivante(TCHAR *szFileName)
{

	switch(iDataType)
	{
		case 0:
			try
			{
				fit++;
				if(fit == files.end())
				{
					fit=files.begin();
				}
			}
			catch(...)
			{
				fit=files.begin();
			}
			strcpy(szFileName,(*fit).m_sName.c_str());
			break;
		case 1:
			try
			{
				Liste++;
				if(Liste == m_IconeFileVector->end())
				{
					Liste=m_IconeFileVector->begin();
				}
			}
			catch(...)
			{
				Liste=m_IconeFileVector->begin();
			}
			strcpy(szFileName,(*Liste).m_stgFullPathName);
			break;
	}
	return 1;
}

int CFichier::SupprimerFichier(TCHAR *szFileName)
{
	switch(iDataType)
	{
		case 0:
			try
			{
				files.erase(fit);
			}
			catch(...)
			{
				fit=files.begin();
			}
			strcpy(szFileName,(*fit).m_sName.c_str());
			break;
		case 1:
			try
			{
				Liste++;
				m_IconeFileVector->erase(Liste);
				if(Liste == m_IconeFileVector->end())
				{
					Liste=m_IconeFileVector->begin();
				}
			}
			catch(...)
			{
				Liste=m_IconeFileVector->begin();
			}
			strcpy(szFileName,(*Liste).m_stgFullPathName);
			break;
	}
	return 1;	
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

int CFichier::AffichageImagePrecedente(TCHAR *szFileName)
{

	switch(iDataType)
	{
		case 0:
			try
			{
				if(fit != files.begin())
					fit--;
				else
				{
					fit = files.end();
					fit--;	
				}
			}
			catch(...)
			{
				fit = files.end();
				fit--;	
			}
			strcpy(szFileName,(*fit).m_sName.c_str());	
			break;
		case 1:
			try
			{
				if(Liste != m_IconeFileVector->begin())
					Liste--;
				else
				{
					Liste = m_IconeFileVector->end();
					Liste--;	
				}
			}
			catch(...)
			{
				Liste = m_IconeFileVector->end();
				Liste--;	
			}
			strcpy(szFileName,(*Liste).m_stgFullPathName);	
			break;
	}


	return 1;
}

int CFichier::SetTrieFichier(int m_iTypeTrie)
{
	switch(m_iTypeTrie)
	{
		case 0:
			m_cCSADirRead.SortFiles(::CSADirRead::eSortAlpha,true);
			break;

		case 1:
			m_cCSADirRead.SortFiles(::CSADirRead::eSortAlpha,false);
			break;

		case 2:
			m_cCSADirRead.SortFiles(::CSADirRead::eSortSize,true);
			break;

		case 3:
			m_cCSADirRead.SortFiles(::CSADirRead::eSortSize,false);
			break;

		case 4:
			m_cCSADirRead.SortFiles(::CSADirRead::eSortCreateDate,true);
			break;

		case 5:
			m_cCSADirRead.SortFiles(::CSADirRead::eSortCreateDate,false);
			break;
	}

	return 0;
}




//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CFichier::DefinirRepertoire(const char *szFileName, const int m_iTypeTrie)
{

	iDataType = 0;
	TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT];
	TCHAR temp[255];

	_splitpath(szFileName, drive, dir, fname, ext);

	strcpy(temp,drive);
	strcat(temp,dir);
	strcpy(dir,temp);

	dir[strlen(dir) - 1] = '\0';

	m_cCSADirRead.ClearDirs();
	m_cCSADirRead.ClearFiles();
	m_cCSADirRead.GetDirs(dir);
	m_cCSADirRead.GetImageFile();

	switch(m_iTypeTrie)
	{
		case 0:
			m_cCSADirRead.SortFiles(::CSADirRead::eSortAlpha,false);
			break;

		case 1:
			m_cCSADirRead.SortFiles(::CSADirRead::eSortAlpha,true);
			break;

		case 2:
			m_cCSADirRead.SortFiles(::CSADirRead::eSortSize,false);
			break;

		case 3:
			m_cCSADirRead.SortFiles(::CSADirRead::eSortSize,true);
			break;

		case 4:
			m_cCSADirRead.SortFiles(::CSADirRead::eSortCreateDate,false);
			break;

		case 5:
			m_cCSADirRead.SortFiles(::CSADirRead::eSortCreateDate,true);
			break;
	}

	files = m_cCSADirRead.Files();
	fit = files.begin();
	//Recherche fichier dans la liste
	try
	{
		do
		{
			if(strcmp(szFileName,(*fit).m_sName.c_str()) == 0)
				break;

			fit++;

		}while(fit != files.end());
	}
	catch(...)
	{}

}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CFichier::DefinirRepertoire(const char *szFileName, const int m_iTypeTrie, const char * m_cExtension)
{

	iDataType = 0;
	TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT];
	TCHAR temp[255];

	_splitpath(szFileName, drive, dir, fname, ext);

	strcpy(temp,drive);
	strcat(temp,dir);
	strcpy(dir,temp);

	dir[strlen(dir) - 1] = '\0';

	m_cCSADirRead.ClearDirs();
	m_cCSADirRead.ClearFiles();
	m_cCSADirRead.GetDirs(dir);
	m_cCSADirRead.GetFiles(m_cExtension);

	switch(m_iTypeTrie)
	{
		case 0:
			m_cCSADirRead.SortFiles(::CSADirRead::eSortAlpha,false);
			break;

		case 1:
			m_cCSADirRead.SortFiles(::CSADirRead::eSortAlpha,true);
			break;

		case 2:
			m_cCSADirRead.SortFiles(::CSADirRead::eSortSize,false);
			break;

		case 3:
			m_cCSADirRead.SortFiles(::CSADirRead::eSortSize,true);
			break;

		case 4:
			m_cCSADirRead.SortFiles(::CSADirRead::eSortCreateDate,false);
			break;

		case 5:
			m_cCSADirRead.SortFiles(::CSADirRead::eSortCreateDate,true);
			break;
	}

	files = m_cCSADirRead.Files();
	fit = files.begin();
	//Recherche fichier dans la liste
	try
	{
		do
		{
			if(strcmp(szFileName,(*fit).m_sName.c_str()) == 0)
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

void CFichier::AffichageImagePremiere(TCHAR *szFileName)
{
	switch(iDataType)
	{
		case 0:
			fit = files.begin();
			strcpy(szFileName,(*fit).m_sName.c_str());
			break;
		case 1:
			Liste = m_IconeFileVector->begin();
			strcpy(szFileName,(*Liste).m_stgFullPathName);
			break;
	}
}

void CFichier::AffichageImageDerniere(TCHAR *szFileName)
{
	switch(iDataType)
	{
		case 0:
			fit = files.end();
			fit--;
			strcpy(szFileName,(*fit).m_sName.c_str());
			break;
		case 1:
			Liste = m_IconeFileVector->end();
			Liste--;
			strcpy(szFileName,(*Liste).m_stgFullPathName);
			break;
	}
}


void CFichier::DefinirListeImage(const char *szFileName, CCatalogue::IconeFileVector *m_IconeFileLocalVector)
{
	m_IconeFileVector = m_IconeFileLocalVector;
	iDataType = 1;
	//Recherche fichier dans la liste
	try
	{
		Liste = m_IconeFileVector->begin();

		do
		{
			if(strcmp(szFileName,(*Liste).m_stgFullPathName) == 0)
				break;

			Liste++;

		}while(Liste != m_IconeFileVector->end());
	}
	catch(...)
	{}
}
