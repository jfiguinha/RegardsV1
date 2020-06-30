// Objet.cpp: implementation of the CObjet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Objet.h"
#include "loadimage.h"
#include <SAFileEntry.h>
#include <IconeFile.h>
#include ".\registryaccess.h"
#include <association.h>
#include <ibitmap.h>
#include <exif.h>


HCURSOR CObjet::hcurSave;

void CObjet::StartWait()
{
	HCURSOR hcur = ::LoadCursor(NULL, IDC_WAIT);
	//SetSystemCursor(::LoadCursor(NULL, IDC_WAIT), OCR_NORMAL);
	hcurSave = SetCursor(hcur);
}

void CObjet::EndWait()
{
	HCURSOR hcur = ::LoadCursor(NULL, IDC_ARROW);
	//SetSystemCursor(CopyCursor(hcur), 32512);
	SetCursor(hcur);
	//SetSystemCursor(::LoadCursor(NULL, IDC_ARROW), OCR_NORMAL);
}

void CObjet::TestOrientation(CIBitmap * ciBitmap, const char * cFilename)
{
	int m_iOrientation = 1;
	int iFormatImage = TestImageFormat(cFilename);
	if(iFormatImage == 1)
	{
		Cexif * m_cExif = new Cexif();
		FILE *hSource;
		errno_t err;
		err = fopen_s(&hSource, cFilename, "rb");

		if(err == 0)
		{
			m_cExif->DecodeExif(hSource);
			if(m_cExif->m_exifinfo->IsExif)
				m_iOrientation = m_cExif->m_exifinfo->Orientation;

			fclose(hSource);
		}
		delete m_cExif;
	}

	if(m_iOrientation == 6)
	{
		ciBitmap->HorzFlipBuf();
		ciBitmap->Rotation90();
	}
	if(m_iOrientation == 8)
	{
		ciBitmap->Rotation90();
	}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

LRESULT CObjet::TestFilePresent(const char * m_szFilename)
{
	int fichier_source = 0;
	if(_sopen_s(&fichier_source, m_szFilename, _O_RDONLY, _SH_DENYNO, _S_IREAD | _S_IWRITE) == -1)
		return 0;

	if(fichier_source != -1)
	{
		_close(fichier_source);
	}
	else
		return 0;

	return 1;
}

void CObjet::GetVirtualDesktopRect(LPRECT lprc )
{
	::SetRect( lprc, 
	::GetSystemMetrics( SM_XVIRTUALSCREEN ),
	::GetSystemMetrics( SM_YVIRTUALSCREEN ),
	::GetSystemMetrics( SM_CXVIRTUALSCREEN ),
	::GetSystemMetrics( SM_CYVIRTUALSCREEN ) );
}

//////////////////////////////////////////////////////////////////////
//Centrage de la fenêtre
//////////////////////////////////////////////////////////////////////

void CObjet::CenterWindow(HWND hwndDlg)
{
	WINDOWPLACEMENT wp;
	int YVal, XVal, X, Y, DbHeight , DbWidth, RetValue;
	ZeroMemory(&wp,sizeof(WINDOWPLACEMENT));
	RetValue = GetWindowPlacement(hwndDlg, &wp);
	YVal = GetSystemMetrics(SM_CYSCREEN);
	XVal = GetSystemMetrics(SM_CXSCREEN);
	DbHeight = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
	DbWidth = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
	X = (XVal - DbWidth) / 2;
	Y = (YVal - DbHeight) / 2;
	RetValue = SetWindowPos(hwndDlg, 0, X, Y, 0, 0, SWP_NOSIZE + SWP_NOACTIVATE + SWP_NOZORDER);
}

CObjet::CObjet()
{
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CObjet::AffichageMessage(UINT iStringText, UINT iStringCaption, const int &iOption)
{
	char clpText[255];
	char clpCaption[255];

	LoadString(hInstance,iStringText,clpText,255);
	LoadString(hInstance,iStringCaption,clpCaption,255);

	MessageBox(NULL,clpText,clpCaption,iOption);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CObjet::UncheckMenuStatePreviewQuality(HWND hwnd)
{
	CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_NORMAL,MF_UNCHECKED);
	CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BESSEL,MF_UNCHECKED);
	CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BICUBIC,MF_UNCHECKED);
	CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BILINEAR,MF_UNCHECKED);
	CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BLACKMAN,MF_UNCHECKED);
	CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BLACKMANBESSEL,MF_UNCHECKED);
	CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_BOX,MF_UNCHECKED);
	CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_CATROM,MF_UNCHECKED);
	CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_GAUSSIAN,MF_UNCHECKED);
	CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_HAMMING,MF_UNCHECKED);
	CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_HANNING,MF_UNCHECKED);
	CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_HERMITE,MF_UNCHECKED);
	CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_LANCZOS,MF_UNCHECKED);
	CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_MITCHELL,MF_UNCHECKED);
	CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_QUADRATIC,MF_UNCHECKED);
	CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_SINC,MF_UNCHECKED);
	CheckMenuItem(GetMenu(hwnd),ID_AFFICHAGE_PRVISUALISATIONENHAUTEQUALIT_TRIANGLE,MF_UNCHECKED);
}




//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CObjet::~CObjet()
{
}



///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

int CObjet::TestRepertoire(const char * szFileName)
{
  struct _finddata_t  c_file;
  long fhandle;

  if ((fhandle=_findfirst(szFileName, &c_file ))!=-1) 
  {
     // we only care about subdirs
     if ((c_file.attrib & _A_SUBDIR)==_A_SUBDIR) 
     {
		 return true;
	 }
  }
  _findclose(fhandle);
  return false;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

int CObjet::TestImage(const char * szFileNameMain, const bool &m_bAffichage)
{
	if(strcmp(szFileNameMain,"") == 0)
	{
		if(m_bAffichage)
		{
			AffichageMessage(IDS_ERRORFICHIER,IDS_ERREUR,MB_OK | MB_ICONERROR);
		}
		return 0;
	}
	else
	{
#ifdef WMP9
		if(TestImageFormat(szFileNameMain) == 0 && TestMultimediaFormat(szFileNameMain) == 0 && Test3DFormat(szFileNameMain) == 0)
		{
			if(m_bAffichage)
			{
				//MessageBox(NULL,"Le fichier sélectionné n'est pas reconnu par l'application !","Erreur",MB_OK | MB_ICONERROR);
				ShellExecute(NULL, "open", szFileNameMain, NULL, NULL, SW_SHOWNORMAL );
			}
			return 0;
		}
	}
#else
		if(CObjet::TestImageFormat(szFileNameMain) == 0)
		{
			if(m_bAffichage)
			{
				//MessageBox(NULL,"Le fichier sélectionné n'est pas reconnu par l'application !","Erreur",MB_OK | MB_ICONERROR);
				ShellExecute(NULL, "open", szFileNameMain, NULL, NULL, SW_SHOWNORMAL );
			}
			return 0;
		}
	}
#endif
	return 1;
}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void CObjet::SetWallpaper(const char * FileName, const char * cType, const char * cTile, HWND hWndTreeWindow)
{
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	UINT m_iValue = 0;
	m_cRegistryAccess->SetKeyValue(HKEY_CURRENT_USER, "Control Panel\\Desktop\\", "Wallpaper", FileName);
	m_cRegistryAccess->SetKeyValue(HKEY_CURRENT_USER, "Control Panel\\Desktop\\", "WallpaperStyle", cType);
	m_cRegistryAccess->SetKeyValue(HKEY_CURRENT_USER, "Control Panel\\Desktop\\", "TileWallpaper", cTile);

	TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
	_splitpath_s(FileName, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

	errno_t erreur;
	erreur = _strlwr_s(ext,_MAX_EXT);
	if(erreur != 0)
		return;

	if(strcmp(ext,".bmp") == 0)
		SystemParametersInfo(SPI_SETDESKWALLPAPER,0,(void *)FileName, SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
	else
	{
		char Temp[MAX_PATH];
		StringCchCopy(Temp,MAX_PATH,"c:\\wallpaper_regards.bmp");
		SendMessage(hWndTreeWindow, WM_COMMAND, MAKEWPARAM(IDM_SAVEBMPFILE,0),(LPARAM)Temp);
		SystemParametersInfo(SPI_SETDESKWALLPAPER,0,(void *)Temp, SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
	}

	delete m_cRegistryAccess;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

VOID APIENTRY CObjet::DisplayContextMenu(HWND hwnd, POINT pt, HINSTANCE hInstance, const char * szMenuName, int iSubMenu) 
{ 
    HMENU hmenu;            // top-level menu 
    HMENU hmenuTrackPopup;  // shortcut menu 
	RECT rc;
 
    // Load the menu resource. 
 
    if ((hmenu = LoadMenu(hInstance, szMenuName)) == NULL) 
        return; 
 
    // TrackPopupMenu cannot display the menu bar so get 
    // a handle to the first shortcut menu. 
 
    hmenuTrackPopup = GetSubMenu(hmenu, iSubMenu); 
 
    // Display the shortcut menu. Track the right mouse 
    // button. 

	GetMenuItemRect(hwnd,hmenuTrackPopup,0,&rc);

    TrackPopupMenu(hmenuTrackPopup, 
            TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
			pt.x + rc.left, pt.y + rc.bottom, 0, hwnd, NULL); 

    // Destroy the menu. 
 
    DestroyMenu(hmenu); 
} 

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void CObjet::GetInfosTaille(const INT64 &lSize, char * cInfos,const int &iInfosSize)
{
	double fAffichage;

	//lTaille = lTaille >> 10;
	fAffichage = (double)lSize/(double)(1024);
	StringCchPrintf(cInfos,iInfosSize,"%.2f Ko",fAffichage);
}

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

int CObjet::GetSysImgIcon(string strTemp)
{

	SHFILEINFO shFinfo;

	try
	{

		if ( !SHGetFileInfo(strTemp.c_str(),
							0,
							&shFinfo,
							sizeof( shFinfo ),
							SHGFI_ICON | 
							SHGFI_SMALLICON ) )
		{
			return 0;
		}

	}
	catch(...)
	{
		return -1;
	}

	int iIcon = shFinfo.iIcon;

	// we only need the index from the system image list

	DestroyIcon( shFinfo.hIcon );

	
	return iIcon;
}

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

int CObjet::GetSysImgOpenIcon(string strTemp)
{

	SHFILEINFO shFinfo;

	try
	{

		if ( !SHGetFileInfo(strTemp.c_str(),
							0,
							&shFinfo,
							sizeof( shFinfo ),
							SHGFI_ICON | SHGFI_OPENICON | 
							SHGFI_SMALLICON ) )
		{
			return 0;
		}

	}
	catch(...)
	{
		return -1;
	}

	int iIcon = shFinfo.iIcon;

	// we only need the index from the system image list

	DestroyIcon( shFinfo.hIcon );

	
	return iIcon;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void CObjet::GetFileName(const char *szFilePath, char *FileName,const int &iFileNameSize)
{
	if(szFilePath != NULL)
	{
		TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
		_splitpath_s(szFilePath, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

		StringCchCopy(FileName,iFileNameSize,fname);
		StringCbCat(FileName,iFileNameSize,ext);
	}
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void CObjet::GetFolderName(const char *szFilePath, char *FileName,const int &iFileNameSize)
{
	if(szFilePath != NULL)
	{
		TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
		_splitpath_s(szFilePath, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

		StringCchCopy(FileName,iFileNameSize,drive);
		StringCbCat(FileName,iFileNameSize,dir);
	}
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void CObjet::GetInfosFile(const char *szFileName, char *cInfos,const int &iInfosSize)
{
	char Temp[20];
	_finddata_t  c_file;

	struct tm currenttime;
	errno_t err;

	//GetInfosFichier(szFileName,lSize,Temp);
	long fhandle = _findfirst(szFileName, &c_file);

	if(fhandle == -1)
	{
		cInfos = NULL;
		return;
	}
	//newtime = localtime_s(&c_file.time_create,*newtime); 

	err = _localtime64_s(&currenttime, &c_file.time_create);
	if(err == 0)
	{
		strftime(Temp,20,"%d/%m/%Y %H:%M:%S", &currenttime);
	}
	StringCchPrintf(cInfos,iInfosSize,"%s, %dKo, %s",c_file.name,c_file.size/1024,Temp);
	_findclose(fhandle);

}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void CObjet::GetInfosDateFile(const char *szFileName, char *cInfos,const int &iInfosSize)
{
	char Temp[20];
	_finddata_t  c_file;
	struct tm currenttime;
	errno_t err;

	//GetInfosFichier(szFileName,lSize,Temp);
	long fhandle = _findfirst(szFileName, &c_file);
	if(fhandle == -1)
	{
		cInfos = NULL;
		return;
	}

	//newtime = localtime(&c_file.time_create); 

	err = _localtime64_s(&currenttime, &c_file.time_create);

	if(err == 0)
	{
		strftime(Temp,45,"%d/%m/%Y %H:%M:%S",&currenttime);
	}
	StringCchPrintf(cInfos,40,"%s",Temp);
	_findclose(fhandle);

}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void CObjet::GetDiskSpaceInfos(const char * m_szFilename,char * cInfos,const int &iInfosSize)
{
	if(m_szFilename == NULL)
		return;

	UINT m_iValue = 0;
	TCHAR drive[_MAX_DRIVE+2], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
	_splitpath_s(m_szFilename, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

	
	DWORD SectorsPerCluster;
	DWORD BytesPerSector;
	DWORD NumberOfFreeClusters;
	DWORD TotalNumberOfClusters;
	
	StringCchCat(drive,_MAX_DRIVE + 2,"\\");

	//GetDiskFreeSpace(drive,&SectorsPerCluster,&BytesPerSector,&NumberOfFreeClusters,&TotalNumberOfClusters);
	ULARGE_INTEGER FreeBytesAvailable;
	ULARGE_INTEGER TotalNumberOfBytes;
	ULARGE_INTEGER TotalNumberOfFreeBytes;

	GetDiskFreeSpaceEx(drive,&FreeBytesAvailable,&TotalNumberOfBytes,&TotalNumberOfFreeBytes);

	//INT64 m_FreeSpace = TotalNumberOfFreeBytes;
	double m_dFreeSpace = (double)TotalNumberOfFreeBytes.QuadPart / 1024.0;
	m_dFreeSpace = m_dFreeSpace / 1024.0;
	if(m_dFreeSpace > 1024.0)
	{
		char m_cTempMessage[255];
		LoadString(hInstance,IDS_MESSAGEDISKGO,m_cTempMessage,255);
		m_dFreeSpace = m_dFreeSpace / 1024;
		StringCchPrintf(cInfos,iInfosSize,m_cTempMessage,drive,m_dFreeSpace);
	}
	else
	{
		char m_cTempMessage[255];
		LoadString(hInstance,IDS_MESSAGEDISKMO,m_cTempMessage,255);
		StringCchPrintf(cInfos,iInfosSize,m_cTempMessage,drive,m_dFreeSpace);
	}
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void CObjet::GetFileSize(const char *szFileName, char *cInfos,const int &iInfosSize)
{
	_finddata_t  c_file;

	//GetInfosFichier(szFileName,lSize,Temp);
	long fhandle = _findfirst(szFileName, &c_file);
	if(fhandle == -1)
	{
		cInfos = NULL;
		return;
	}
	StringCchPrintf(cInfos,iInfosSize,"%d Ko",c_file.size/1024);
	_findclose(fhandle);

}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

int CObjet::Test3DFormat(const char *szFileName)
{
	UINT m_iValue = 0;
	TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
	_splitpath_s(szFileName, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

	errno_t erreur;
	erreur = _strlwr_s(ext,_MAX_EXT);
	if(erreur != 0)
		return -1;

	if(strcmp(ext,".3ds") == 0)
		return 1;

	if(strcmp(ext,".md2") == 0)
		return 2;

	return m_iValue;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

int CObjet::TestGravureFormat(const char *szFileName)
{
	UINT m_iValue = 0;
	TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
	_splitpath_s(szFileName, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

	errno_t erreur;
	erreur = _strlwr_s(ext,_MAX_EXT);
	if(erreur != 0)
		return -1;

	if(strcmp(ext,".iso") == 0)
		return 1;

	return m_iValue;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
int CObjet::TestMultimediaFormat(const char *szFileName)
{
	UINT m_iValue = 0;
	TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
	_splitpath_s(szFileName, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

	errno_t erreur;
	erreur = _strlwr_s(ext,_MAX_EXT);
	if(erreur != 0)
		return -1;

	if(strcmp(ext,".avi") == 0)
		return 1;

	if(strcmp(ext,".divx") == 0)
		return 1;


	if(strcmp(ext,".mp3") == 0)
		return 2;


	if(strcmp(ext,".mp2") == 0)
		return 3;

	if(strcmp(ext,".mpv") == 0)
		return 3;

	if(strcmp(ext,".mv2") == 0)
		return 3;

	if(strcmp(ext,".mpg") == 0)
		return 3;

	if(strcmp(ext,".mpeg") == 0)
		return 3;

	if(strcmp(ext,".wav") == 0)
		return 4;

	if(strcmp(ext,".wmv") == 0)
		return 5;

	return m_iValue;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
int CObjet::TestVideoFormat(const char *szFileName)
{
	//return 0;

	UINT m_iValue = 0;
	TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
	_splitpath_s(szFileName, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

	errno_t erreur;
	erreur = _strlwr_s(ext,_MAX_EXT);
	if(erreur != 0)
		return -1;

	if(strcmp(ext,".avi") == 0)
		return 100;

	if(strcmp(ext,".divx") == 0)
		return 101;

	if(strcmp(ext,".mpv") == 0)
		return 102;

	if(strcmp(ext,".mv2") == 0)
		return 103;

	if(strcmp(ext,".mpg") == 0)
		return 104;

	if(strcmp(ext,".mpeg") == 0)
		return 105;

	if(strcmp(ext,".wmv") == 0)
		return 106;

	return m_iValue;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

int CObjet::TestCompressionFormat(const char *szFileName)
{
	UINT m_iValue = 0;
	TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
	_splitpath_s(szFileName, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

	errno_t erreur;
	erreur = _strlwr_s(ext,_MAX_EXT);
	if(erreur != 0)
		return -1;

	if(strcmp(ext,".zip") == 0)
		return 1;

	//if(strcmp(strlwr(ext),".rar") == 0)
	//	return 2;

	return m_iValue;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


int CObjet::TestZipFormat(const char *szFileName)
{
	UINT m_iValue = 0;
	TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
	_splitpath_s(szFileName, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

	errno_t erreur;
	erreur = _strlwr_s(ext,_MAX_EXT);
	if(erreur != 0)
		return -1;

	if(strlen(ext) < 3)
		return -1;

	if(_stricmp(ext,".zip") == 0)
		return 1;

	return m_iValue;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
int CObjet::TestImageFormat(const char *szFileName)
{
	if(TestRepertoire(szFileName))
		return 0;

	UINT m_iValue = 0;
	TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
	_splitpath_s(szFileName, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

	errno_t erreur;
	erreur = _strlwr_s(ext,_MAX_EXT);
	if(erreur != 0)
		return 0;

	if(strlen(ext) < 3)
		return 0;

	char m_cRaw[255];
	LoadString(hInstance,IDS_STGRAWFILE,m_cRaw,255);

	string m_stgData = m_cRaw;
	int m_iIndex = m_stgData.find(ext);

	if(m_iIndex != -1)
		return 12;


	/*
	if(_stricmp(ext,".crw") == 0)
		return 12;
	*/

	if(_stricmp(ext,".ppm") == 0)
		return 11;

	if(_stricmp(ext,".pgm") == 0)
		return 11;

	if(_stricmp(ext,".bmp") == 0)
		return 2;

	if(_stricmp(ext,".pcx") == 0)
		return 7;

	if(_stricmp(ext,".jpg") == 0)
		return 1;

	if(_stricmp(ext,".jpe") == 0)
		return 1;

	if(_stricmp(ext,".jpeg") == 0)
		return 1;

	if(_stricmp(ext,".jfif") == 0)
		return 1;

	if(_stricmp(ext,".jif") == 0)
		return 1;

	if(_stricmp(ext,".tif") == 0)
		return 3;

	if(_stricmp(ext,".tiff") == 0)
		return 3;

	if(_stricmp(ext,".gif") == 0)
		return 4;

	if(_stricmp(ext,".png") == 0)
		return 5;

	if(_stricmp(ext,".tga") == 0)
		return 6;

	if(_stricmp(ext,".pcd") == 0)
		return 8;

	if(_stricmp(ext,".jp2") == 0)
		return 9;

	if(_stricmp(ext,".jpc") == 0)
		return 9;

	if(_stricmp(ext,".j2k") == 0)
		return 9;

	if(_stricmp(ext,".psd") == 0)
		return 10;

	if(_stricmp(ext,".pdd") == 0)
		return 10;

	return m_iValue;
}



///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
int CObjet::BrowseCallbackProc(HWND hWnd, UINT message, long wParam, LPARAM lParam)
{
 
  //Callback for the Browse PIDL method.
 
  //On initialization, set the dialog's
  //pre-selected folder using the pidl
  //set as the bi.lParam, and passed back
  //to the callback as lpData param. 
  switch(message)
  {
		case BFFM_INITIALIZED:
			{
				SendMessage(hWnd, BFFM_SETSELECTIONA, TRUE, lParam);
			}
			break;
                          
         default:
			 break;
         
  }

  return FALSE;

	
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
int CObjet::GetDirectory(HWND hWnd, char * cDir)
{
	BROWSEINFOA bi;
	LPITEMIDLIST lpIDList;
	char Temp[MAX_PATH];
	char m_cMessage[255];

	LoadString(hInstance,IDS_SELECTDIR,m_cMessage,255);

	StringCchCopy(Temp,MAX_PATH,cDir);

	ZeroMemory(&bi,sizeof(BROWSEINFOA));
    bi.hwndOwner = hWnd;
    bi.lpszTitle = m_cMessage;//"Sélectionner un répertoire";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	bi.lParam = (LPARAM)&Temp;
	bi.lpfn = (BFFCALLBACK)BrowseCallbackProc;

	lpIDList = SHBrowseForFolder(&bi);

   if(lpIDList)
   {
		int nResult;
        nResult = SHGetPathFromIDList(lpIDList, cDir);
        CoTaskMemFree(lpIDList);
		return 1;
   }
   
   return 0;

}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
int CObjet::GetWindowsVersion()
{

	DWORD dwBuild;
	//Détection de la version de Windows
	DWORD dwVersion = GetVersion();
	// Get the Windows version.

	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));

	// Get the build number.

	if (dwVersion < 0x80000000)              // Windows NT/2000/XP
		return 1;
	else if (dwWindowsMajorVersion < 4)      // Win32s
		dwBuild = (DWORD)(HIWORD(dwVersion) & ~0x8000);
	else                                     // Windows 95/98/Me
		return 0;

	return dwBuild;
	
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
int CObjet::GetWindowsVersionAvance()
{

	//Détection de la version de Windows
	DWORD dwVersion = GetVersion();
	// Get the Windows version.

	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));

	// Get the build number.

	if(dwWindowsMajorVersion >= 5)
	{
		if(dwWindowsMajorVersion == 5)
		{
			switch(dwWindowsMinorVersion)
			{
				case 0:
					return 50;
				default:
					return 51;
			}
		}
		if(dwWindowsMajorVersion >= 6)
		{
			switch(dwWindowsMinorVersion)
			{
				case 0:
					return 60;
				default:
					return 61;
			}
		}
	}
	else
	{
		switch(dwWindowsMinorVersion)
		{
			case 10:
				//98
				return 410;
			case 90:
				//Me
				return 490;
		}
	}

	return -1;
	
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void CObjet::CreateAssociation(HWND hWnd)
{
	//Appel de la fonction d'affichage de la boite de dialogue de sélection
	//DialogBox(hInstance, (LPCTSTR)IDD_ASSOCIATE, hWnd, (DLGPROC)Association);



	CAssociation * m_cAssociation = new CAssociation(hInstance);
	m_cAssociation->CreateDlg(hInstance, (LPCTSTR)IDD_ASSOCIATE, hWnd);
	delete m_cAssociation;
}


//////////////////////////////////////////////////////////////////////
//Information concernant le diaporama
//////////////////////////////////////////////////////////////////////

void CObjet::AlbumGestionAddItem(CSAFileEntry &fit,CIconeFile &m_IconeFile)
{
	//char FileName[MAX_PATH];
	CLoadImage m_cLoadImage;
	CObjet m_cObjet;
	struct tm newtime;
	UINT width, height;
	char Infos[512];
	char Temp[255];
	errno_t err;

	StringCchCopy(Infos,512,"version=1.0;");

	//Ajout de l'image dans le vecteur image
	StringCchCopy(m_IconeFile.m_stgFullPathName,512,fit.m_sName.c_str());
	//newtime = localtime(&fit.time_write); 
	err = _localtime64_s(&newtime, &fit.time_write);

	if(err == 0)
		strftime(m_IconeFile.szTimeCreate,32,"%d/%m/%Y %H:%M:%S",&newtime);

	//Récupération des informations sur la taille de l'image
	m_cLoadImage.GetDimensionImageStatic(m_IconeFile.m_stgFullPathName,&width, &height);

	StringCchPrintf(Temp,255,"dimension=%dx%d;",width,height);

	StringCbCat(Infos,512,Temp);
	
	StringCchCopy(m_IconeFile.informations,256,Infos);

	//Récupération des informations sur le poids du fichier
	__int64 iSize = fit.size >> 10;
	StringCchPrintf(m_IconeFile.szSize,20,"%d Ko",iSize);

	m_IconeFile.lNumIcone = 0;
}

//////////////////////////////////////////////////////////////////////
//Information concernant le diaporama
//////////////////////////////////////////////////////////////////////

void CObjet::AlbumGestionAddItem(const char * m_stgFullPathFile,CIconeFile &m_IconeFile)
{
	CLoadImage m_cLoadImage;
	CObjet m_cObjet;
	struct tm newtime;
	UINT width, height;
	char Infos[512];
	char Temp[255];
	errno_t err;

	StringCchCopy(Infos,512,"version=1.0;");

	struct _finddata_t  c_file;
	long fhandle;

	if ((fhandle=_findfirst(m_stgFullPathFile, &c_file ))!=-1) 
	{
		//m_cObjet.GetFileName(m_IconeFile.m_stgFullPathName,m_IconeFile.m_stgFileName);
		//Ajout de l'image dans le vecteur image
		StringCchCopy(m_IconeFile.m_stgFullPathName,512,m_stgFullPathFile);

		err = _localtime64_s(&newtime, &c_file.time_write);

		if(err == 0)
			strftime(m_IconeFile.szTimeCreate,32,"%d/%m/%Y %H:%M:%S",&newtime);

		//newtime = localtime(&c_file.time_write); 
		//strftime(m_IconeFile.szTimeCreate,32,"%d/%m/%Y %H:%M:%S",newtime);

		//Récupération des informations sur la taille de l'image
		m_cLoadImage.GetDimensionImageStatic(m_IconeFile.m_stgFullPathName,&width, &height);

		StringCchPrintf(Temp,255,"dimension=%dx%d;",width,height);
		StringCbCat(Infos,512,Temp);
		StringCchCopy(m_IconeFile.informations,256,Infos);

		//Récupération des informations sur le poids du fichier
		int iSize = c_file.size >> 10;
		StringCchPrintf(m_IconeFile.szSize,20,"%d Ko",iSize);

		m_IconeFile.lNumIcone = 0;
	}
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CObjet::GetFormatName(const int &iFormat, char * cName,const int &iNameSize)
{
	switch(iFormat)
	{
		case 1:
			StringCchCopy(cName,iNameSize,"JPEG");
			break;
		case 2:
			StringCchCopy(cName,iNameSize,"BMP");
			break;
		case 3:
			StringCchCopy(cName,iNameSize,"TIFF");
			break;
		case 4:
			StringCchCopy(cName,iNameSize,"GIF");
			break;
		case 5:
			StringCchCopy(cName,iNameSize,"PNG");
			break;
		case 6:
			StringCchCopy(cName,iNameSize,"TGA");
			break;
		case 7:
			StringCchCopy(cName,iNameSize,"PCX");
			break;
		case 8:
			StringCchCopy(cName,iNameSize,"KODAK PHOTO-CD");
			break;
		case 9:
			StringCchCopy(cName,iNameSize,"JPEG 2000");
			break;

		case 10:
			StringCchCopy(cName,iNameSize,"Photoshop");
			break;

		case 11:
			StringCchCopy(cName,iNameSize,"PPM - PGM");
			break;

		case 12:
			StringCchCopy(cName,iNameSize,"Photo Raw");
			break;
	}
}
//////////////////////////////////////////////////////////////////////////
//Test si c'est un répertoire valide
//////////////////////////////////////////////////////////////////////////
int CObjet::IsValidateFolder(const char *m_Directory)
{
	IShellFolder *psfDeskTop = NULL;
	ULONG chEaten;
	OLECHAR       olePath[MAX_PATH];
	char cDirectory[MAX_PATH];
	ULONG pdwAttributes = SFGAO_VALIDATE;
	LPITEMIDLIST pidlDocFiles = NULL;

	StringCchPrintf(cDirectory,MAX_PATH,"%s\\",m_Directory);


	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, cDirectory, -1,
					   olePath, MAX_PATH);

	//pzSourceFiles[0] = '\0';
	HRESULT hr = SHGetDesktopFolder(&psfDeskTop);

	hr = psfDeskTop->ParseDisplayName(NULL, NULL, olePath, 
		 &chEaten, &pidlDocFiles, &pdwAttributes);

	if(hr != 0)
	{
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_Directory, -1,
						   olePath, MAX_PATH);

		hr = psfDeskTop->ParseDisplayName(NULL, NULL, olePath, 
			 &chEaten, &pidlDocFiles, &pdwAttributes);
	}

	if(pdwAttributes & SFGAO_VALIDATE)
		return 0;
	else
		return 1;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
int CObjet::BSTRToLocal(LPTSTR pLocal, BSTR pWide, DWORD dwChars)
{
	*pLocal = 0;

	#ifdef UNICODE
	lstrcpy_sn(pLocal, pWide, dwChars);
	#else
	WideCharToMultiByte( CP_ACP, 
						0, 
						pWide, 
						-1, 
						pLocal, 
								dwChars, 
						NULL, 
						NULL);
	#endif

	return lstrlen(pLocal);
}

/**************************************************************************

   LocalToBSTR()
   
**************************************************************************/

int CObjet::LocalToBSTR(BSTR pWide, LPTSTR pLocal, DWORD dwChars)
{
	*pWide = 0;

	#ifdef UNICODE
	lstrcpy_sn(pWide, pLocal, dwChars);
	#else
	MultiByteToWideChar( CP_ACP, 
						0, 
						pLocal, 
						-1, 
						pWide, 
						dwChars); 
	#endif

	return lstrlenW(pWide);
}