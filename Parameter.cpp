#include "stdafx.h"
#include ".\parameter.h"
#include ".\registryaccess.h"
#include "objet.h"
#include <omp.h>

CParameter::CParameter(void)
{
}

CParameter::~CParameter(void)
{
}

//////////////////////////////////////////////////////////////////////
//Information concernant le diaporama
//////////////////////////////////////////////////////////////////////

long CParameter::GetStartDiaporama()
{
	int iFiltre = 0;

	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->OpenKeyValue("SOFTWARE\\Regards\\diaporama") == ERROR_SUCCESS)
	{
		iFiltre = m_cRegistryAccess->ReadIntValue("start");
		if(iFiltre == -1)
			iFiltre = 0;
	}
	delete m_cRegistryAccess;

	return iFiltre;
}

void CParameter::SetStartDiaporama(const bool &m_bValue)
{
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->WriteOrCreateKeyValue("SOFTWARE\\Regards\\diaporama") == ERROR_SUCCESS)
	{
		m_cRegistryAccess->WriteIntValue(m_bValue,"start");
	}
	delete m_cRegistryAccess;

}

//////////////////////////////////////////////////////////////////////
//Information concernant le diaporama
//////////////////////////////////////////////////////////////////////

long CParameter::GetGpGpu()
{
	int gpGpu = 0;
	bool m_bGpGpu = true;

	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->OpenKeyValue("SOFTWARE\\Regards\\traitement") == ERROR_SUCCESS)
	{
		gpGpu = m_cRegistryAccess->ReadIntValue("gpgpu");
		if(gpGpu == 0)
			m_bGpGpu = false;
	}
	delete m_cRegistryAccess;

	return m_bGpGpu;
}

void CParameter::SetGpGpu(const bool &m_bValue)
{
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->WriteOrCreateKeyValue("SOFTWARE\\Regards\\traitement") == ERROR_SUCCESS)
	{
		m_cRegistryAccess->WriteIntValue(m_bValue,"gpgpu");
	}
	delete m_cRegistryAccess;

}

//////////////////////////////////////////////////////////////////////
//Information concernant les écrans
//////////////////////////////////////////////////////////////////////

int CParameter::GetNbScreen()
{
	int m_iNbScreen = 0;

	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->OpenKeyValue("SOFTWARE\\Regards\\screen") == ERROR_SUCCESS)
	{
		m_iNbScreen = m_cRegistryAccess->ReadIntValue("nbscreen");
		if(m_iNbScreen == -1)
			m_iNbScreen = 1;
	}
	delete m_cRegistryAccess;

	return m_iNbScreen;
}

void CParameter::SetNbScreen(const int &iNbScreen)
{
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->WriteOrCreateKeyValue("SOFTWARE\\Regards\\screen") == ERROR_SUCCESS)
	{
		m_cRegistryAccess->WriteIntValue(iNbScreen,"nbscreen");
	}
	delete m_cRegistryAccess;

}

//////////////////////////////////////////////////////////////////////
//Information concernant les écrans
//////////////////////////////////////////////////////////////////////

int CParameter::GetInfosScreen(int iNumScreen,RECT &rectScreen)
{
	int m_iReturnValue = 0;
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	char m_cKey[256];
	sprintf_s(m_cKey,256,"SOFTWARE\\Regards\\screen\\ecran%d",iNumScreen);

	if(m_cRegistryAccess->OpenKeyValue(m_cKey) == ERROR_SUCCESS)
	{
		rectScreen.left = m_cRegistryAccess->ReadIntValue("left");
		rectScreen.right = m_cRegistryAccess->ReadIntValue("right");
		rectScreen.top = m_cRegistryAccess->ReadIntValue("top");
		rectScreen.bottom = m_cRegistryAccess->ReadIntValue("bottom");
		if(rectScreen.bottom == -1)
			m_iReturnValue = -1;
	}
	delete m_cRegistryAccess;

	return m_iReturnValue;
}

void CParameter::SetInfosScreen(const int &iNumScreen,RECT &rectScreen)
{
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	char m_cKey[256];
	sprintf_s(m_cKey,256,"SOFTWARE\\Regards\\screen\\ecran%d",iNumScreen);
	if(m_cRegistryAccess->WriteOrCreateKeyValue(m_cKey) == ERROR_SUCCESS)
	{
		m_cRegistryAccess->WriteIntValue(rectScreen.left,"left");
		m_cRegistryAccess->WriteIntValue(rectScreen.right,"right");
		m_cRegistryAccess->WriteIntValue(rectScreen.top,"top");
		m_cRegistryAccess->WriteIntValue(rectScreen.bottom,"bottom");
	}
	delete m_cRegistryAccess;
}

///////////////////////////////////////////////////////////////////////////////////////
//Récupère les informations sur le fait de sauver ou non les miniatures
///////////////////////////////////////////////////////////////////////////////////////

int CParameter::GetSaveOptionMiniature(int &iSave)
{
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->OpenKeyValue("SOFTWARE\\Regards\\exploreur") == ERROR_SUCCESS)
	{
		iSave = m_cRegistryAccess->ReadIntValue("savethumbnail");
		if(iSave == -1)
			iSave = 1;
	}
	delete m_cRegistryAccess;
	return 0;
}

int CParameter::SetSaveOptionMiniature(const int &iSave)
{
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->WriteOrCreateKeyValue("SOFTWARE\\Regards\\exploreur") == ERROR_SUCCESS)
	{
		m_cRegistryAccess->WriteIntValue(iSave,"savethumbnail");
	}
	delete m_cRegistryAccess;
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//Liste des options pour le diaporama
//////////////////////////////////////////////////////////////////////////



//Lecture

int CParameter::GetOptionDiaporama(int &iSequence,int &iDelai, bool &bAutoSize, int &bQualite,bool &bFullscreen, bool &b3D, int &i3DQuality)
{
	int m_iTemp = 0;
	int iFiltre = 0;

	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->OpenKeyValue("SOFTWARE\\Regards\\diaporama") == ERROR_SUCCESS)
	{
		iSequence = m_cRegistryAccess->ReadIntValue("Sequence");
		if(iSequence == -1)
			iSequence = 1;
		iDelai = m_cRegistryAccess->ReadIntValue("Delai");
		if(iDelai == -1)
			iDelai = 2;
		m_iTemp = m_cRegistryAccess->ReadIntValue("AutoSize");
		if(m_iTemp == -1)
			bAutoSize = true;
		else
			bAutoSize = m_iTemp;

		bQualite = m_cRegistryAccess->ReadIntValue("Qualite");
		if(bQualite == -1)
			bQualite = 0;
		
		m_iTemp = m_cRegistryAccess->ReadIntValue("Fullscreen");
		if(m_iTemp == -1)
			bFullscreen = false;
		else
			bFullscreen = m_iTemp;

		m_iTemp = m_cRegistryAccess->ReadIntValue("3D");
		if(m_iTemp == -1)
			b3D = false;
		else
			b3D = m_iTemp;

		m_iTemp = m_cRegistryAccess->ReadIntValue("3DQuality");
		if(m_iTemp == -1)
			i3DQuality = 2;
		else
			i3DQuality = m_iTemp;
	}
	else
	{
		iSequence = 1;
		iDelai = 2;
		bAutoSize = true;
		bQualite = 0;
		bFullscreen = false;
		b3D = false;
		i3DQuality = 2; //Moyen

	}
	delete m_cRegistryAccess;

	return 0;
}

//Sauvegarde

int CParameter::SetOptionDiaporama(const int &iSequence,const int &iDelai, const bool &bAutoSize, const int &bQualite,const bool &bFullscreen,const bool &b3D,const int &i3DQuality)
{
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->WriteOrCreateKeyValue("SOFTWARE\\Regards\\diaporama") == ERROR_SUCCESS)
	{
		m_cRegistryAccess->WriteIntValue(iSequence,"Sequence");
		m_cRegistryAccess->WriteIntValue(iDelai,"Delai");
		m_cRegistryAccess->WriteIntValue(bAutoSize,"AutoSize");
		m_cRegistryAccess->WriteIntValue(bQualite,"Qualite");
		m_cRegistryAccess->WriteIntValue(bFullscreen,"Fullscreen");
		m_cRegistryAccess->WriteIntValue(b3D,"3D");
		m_cRegistryAccess->WriteIntValue(i3DQuality,"3DQuality");
	}
	delete m_cRegistryAccess;

	return 0;
}

////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////

void CParameter::SetOptionTraitement(int iFiltre)
{
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->WriteOrCreateKeyValue("SOFTWARE\\Regards\\traitement") == ERROR_SUCCESS)
	{
		m_cRegistryAccess->WriteIntValue(iFiltre,"filtre");
	}
	delete m_cRegistryAccess;
}

////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////

int CParameter::GetOptionTraitement()
{
	int iFiltre = 0;
	
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->OpenKeyValue("SOFTWARE\\Regards\\traitement") == ERROR_SUCCESS)
	{
		iFiltre = m_cRegistryAccess->ReadIntValue("filtre");
		if(iFiltre == -1)
			iFiltre = 0;
	}
	delete m_cRegistryAccess;

	return iFiltre;
}



////////////////////////////////////////////////////////////////////////////
//Ensemble des options pour la navigation
////////////////////////////////////////////////////////////////////////////

int CParameter::GetOptionAffichage3D()
{
	int m_iRendu = 0;
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->OpenKeyValue("SOFTWARE\\Regards\\3d") == ERROR_SUCCESS)
	{
		m_iRendu = m_cRegistryAccess->ReadIntValue("rendu");
		if(m_iRendu == -1)
			m_iRendu = 0;
	}
	else
		m_iRendu = 0;
	delete m_cRegistryAccess;

	return m_iRendu;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

int CParameter::SetOptionAffichage3D(const int &iFiltre)
{
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->WriteOrCreateKeyValue("SOFTWARE\\Regards\\3d") == ERROR_SUCCESS)
	{
		m_cRegistryAccess->WriteIntValue(iFiltre,"rendu");
	}
	delete m_cRegistryAccess;
	return 0;
}


////////////////////////////////////////////////////////////////////////////
//Ensemble des options pour la navigation
////////////////////////////////////////////////////////////////////////////

int CParameter::GetOptionAffichage(int &m_bHauteQualite,bool &m_bAutoSize, bool &bRawFile)
{
	int iTemp = 0;
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->OpenKeyValue("SOFTWARE\\Regards\\navigation") == ERROR_SUCCESS)
	{
		m_bHauteQualite = m_cRegistryAccess->ReadIntValue("Qualite");
		if(m_bHauteQualite == -1)
			m_bHauteQualite = 0;

		iTemp = m_cRegistryAccess->ReadIntValue("AutoSize");
		if(iTemp == -1)
			m_bAutoSize = true;
		else
			m_bAutoSize = iTemp;

		int m_iValue = m_cRegistryAccess->ReadIntValue("RawFile");
		if(m_iValue == -1)
			bRawFile = false;
		else
			bRawFile = m_iValue;
	}
	else
	{
		m_bHauteQualite = 0;
		m_bAutoSize = 0;
		bRawFile = false;
	}
	delete m_cRegistryAccess;

	return 0;
}

////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////

int CParameter::SetOptionAffichage(const int &m_bHauteQualite,const bool &m_bAutoSize,const bool &bRawFile)
{
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->WriteOrCreateKeyValue("SOFTWARE\\Regards\\navigation") == ERROR_SUCCESS)
	{
		m_cRegistryAccess->WriteIntValue(m_bHauteQualite,"Qualite");
		m_cRegistryAccess->WriteIntValue(m_bAutoSize,"AutoSize");
		m_cRegistryAccess->WriteIntValue(bRawFile,"RawFile");
	}
	delete m_cRegistryAccess;

	return 0;
}

////////////////////////////////////////////////////////////////////////////
//Ensemble des options pour la navigation
////////////////////////////////////////////////////////////////////////////

int CParameter::GetOptionMiniature(int &iLargeur,int &iHauteur)
{
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->OpenKeyValue("SOFTWARE\\Regards\\exploreur") == ERROR_SUCCESS)
	{
		iLargeur = m_cRegistryAccess->ReadIntValue("Largeur");
		iHauteur = m_cRegistryAccess->ReadIntValue("Hauteur");

		if(iLargeur <= 0)
			iLargeur = 90;

		if(iHauteur <= 0)
			iHauteur = 70;
	}
	else
	{
		iLargeur = 90;
		iHauteur = 70;
		SetOptionMiniature(iLargeur,iHauteur);
	}
	delete m_cRegistryAccess;
	return 0;
}

////////////////////////////////////////////////////////////////////////////
//Ensemble des options pour la navigation
////////////////////////////////////////////////////////////////////////////

int CParameter::SetOptionMiniature(const int &iLargeur, const int &iHauteur)
{
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->WriteOrCreateKeyValue("SOFTWARE\\Regards\\exploreur") == ERROR_SUCCESS)
	{
		m_cRegistryAccess->WriteIntValue(iLargeur,"Largeur");
		m_cRegistryAccess->WriteIntValue(iHauteur,"Hauteur");
	}
	delete m_cRegistryAccess;
	return 0;
}

////////////////////////////////////////////////////////////////////////////
//Ensemble des options pour l'exploration
////////////////////////////////////////////////////////////////////////////

//Lecture

int CParameter::GetOptionExploration(int &m_bHauteQualite,bool &m_bImage, char * m_szDirectory,const int &iDirectorySize, int &iModeAffichage)
{
	int iTemp = 0;
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->OpenKeyValue("SOFTWARE\\Regards\\exploreur") == ERROR_SUCCESS) 
	{
		m_bHauteQualite = m_cRegistryAccess->ReadIntValue("Qualite");
		if(m_bHauteQualite == -1)
			m_bHauteQualite = 0;

		iTemp = m_cRegistryAccess->ReadIntValue("ImageUniquement");
		if(iTemp == -1)
			m_bImage = false;
		else
			m_bImage = iTemp;

		iModeAffichage = m_cRegistryAccess->ReadIntValue("ModeAffichage");
		if(iModeAffichage == -1)
			iModeAffichage = 4;
		
		if(m_cRegistryAccess->ReadStringValue(m_szDirectory,iDirectorySize,"Repertoire") == -1)
		{
			StringCchCopy(m_szDirectory,iDirectorySize,"C:");
		}
	}
	else
	{
		m_bHauteQualite = 0;
		m_bImage = 0;
		StringCchCopy(m_szDirectory,iDirectorySize,"C:");
		iModeAffichage = 1;
	}
	delete m_cRegistryAccess;

	return 0;
}


////////////////////////////////////////////////////////////////////////////
//Définition du nombre de thread pour les miniatures
////////////////////////////////////////////////////////////////////////////

int CParameter::GetNbThread(int &iNbThread)
{
	int iTemp = 0;
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->OpenKeyValue("SOFTWARE\\Regards\\exploreur") == ERROR_SUCCESS) 
	{
		iNbThread = m_cRegistryAccess->ReadIntValue("Thread");
		if(iNbThread == -1)
			iNbThread = omp_get_num_procs()*2;
	}
	else
	{
		iNbThread = omp_get_num_procs()*2;
	}
	delete m_cRegistryAccess;

	return 0;
}

int CParameter::SetNbThread(int &iNbThread)
{
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->WriteOrCreateKeyValue("SOFTWARE\\Regards\\exploreur") == ERROR_SUCCESS)
	{
		m_cRegistryAccess->WriteIntValue(iNbThread,"Thread");
	}
	delete m_cRegistryAccess;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////
//Gestion et Sauvegarde des paramètres de placement des fenêtres
///////////////////////////////////////////////////////////////////////////////////////
int CParameter::SaveWindowPlacement(const char * RegistryKey,HWND hWnd)
{
	WINDOWPLACEMENT _wndPlacement;
	BOOL _bResult = ::GetWindowPlacement(hWnd, &_wndPlacement);

	if(_bResult && ::IsWindowVisible(hWnd))
	{
		CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
		if(m_cRegistryAccess->WriteOrCreateKeyValue(RegistryKey) == ERROR_SUCCESS)
		{
			m_cRegistryAccess->WriteIntValue(_wndPlacement.rcNormalPosition.left,"left");
			m_cRegistryAccess->WriteIntValue(_wndPlacement.rcNormalPosition.top,"top");
			m_cRegistryAccess->WriteIntValue(_wndPlacement.rcNormalPosition.right,"right");
			m_cRegistryAccess->WriteIntValue(_wndPlacement.rcNormalPosition.bottom,"bottom");
			m_cRegistryAccess->WriteIntValue(_wndPlacement.flags,"flags");
			m_cRegistryAccess->WriteIntValue(_wndPlacement.showCmd,"showCmd");
			
		}
		delete m_cRegistryAccess;
	}
	return 0;
}

int CParameter::LoadWindowPlacement(const char * RegistryKey,HWND hWnd)
{
	if(hWnd != NULL)
	{
		CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
		if(m_cRegistryAccess->OpenKeyValue(RegistryKey) == ERROR_SUCCESS)
		{
			WINDOWPLACEMENT _windowPlacement;
			::GetWindowPlacement(hWnd,&_windowPlacement);

			_windowPlacement.rcNormalPosition.left = m_cRegistryAccess->ReadIntValue("left");
			if(_windowPlacement.rcNormalPosition.left == -1)
			{
				delete m_cRegistryAccess;
				return -1;
			}

			//RECT  rc = {0, 0, GetDeviceCaps(GetDC(NULL),HORZRES), GetDeviceCaps(GetDC(NULL),VERTRES)};

			_windowPlacement.rcNormalPosition.bottom = m_cRegistryAccess->ReadIntValue("bottom");
			_windowPlacement.rcNormalPosition.right = m_cRegistryAccess->ReadIntValue("right");
			_windowPlacement.rcNormalPosition.top = m_cRegistryAccess->ReadIntValue("top");
			_windowPlacement.flags = m_cRegistryAccess->ReadIntValue("flags");
			_windowPlacement.showCmd = m_cRegistryAccess->ReadIntValue("showCmd");

			RECT rc;
			CObjet::GetVirtualDesktopRect(&rc);		

			//Vérification de la taille de windows
			if(_windowPlacement.rcNormalPosition.left > rc.right || _windowPlacement.rcNormalPosition.top > rc.bottom)
			{
				_windowPlacement.showCmd = SW_SHOWMAXIMIZED;
			}

			SetWindowPlacement(hWnd,&_windowPlacement);
		}
		else
		{
			delete m_cRegistryAccess;
			return -1;
		}
		delete m_cRegistryAccess;
	}
	return 0;
}

int CParameter::GetWindowPlacement(const char * RegistryKey,RECT &rectScreen)
{
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->OpenKeyValue(RegistryKey) == ERROR_SUCCESS)
	{
		rectScreen.left = m_cRegistryAccess->ReadIntValue("left");
		if(rectScreen.left == -1)
		{
			delete m_cRegistryAccess;
			return -1;
		}
		rectScreen.bottom = m_cRegistryAccess->ReadIntValue("bottom");
		rectScreen.right = m_cRegistryAccess->ReadIntValue("right");
		rectScreen.top = m_cRegistryAccess->ReadIntValue("top");
	}
	delete m_cRegistryAccess;

	return 0;
}

//Ecriture
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
int CParameter::SetOptionExploration(const int &m_bHauteQualite,const bool &m_bImage,const char * m_szDirectory,const int &iModeAffichage)
{
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->WriteOrCreateKeyValue("SOFTWARE\\Regards\\exploreur") == ERROR_SUCCESS)
	{
		if(m_bImage == 1 || m_bImage == 0)
			m_cRegistryAccess->WriteIntValue(m_bImage,"ImageUniquement");
		if(CObjet::IsValidateFolder(m_szDirectory))
			m_cRegistryAccess->WriteStringValue(m_szDirectory,"Repertoire");
		if(iModeAffichage < 6 && iModeAffichage >= 0)
			m_cRegistryAccess->WriteIntValue(iModeAffichage,"ModeAffichage");

		m_cRegistryAccess->WriteIntValue(m_bHauteQualite,"Qualite");

	}
	delete m_cRegistryAccess;

	return 0;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

string CParameter::GetMKisofsFile()
{
	char puffer[512];
	GetOptionAlbumDir(puffer,512);
	
	string m_stgFolder = puffer;
	if(puffer[strlen(puffer) - 1] != '\\')
		m_stgFolder += "\\mkisofs.exe";
	else
		m_stgFolder += "mkisofs.exe";

	GetShortPathName(m_stgFolder.c_str(),puffer,512);

	return puffer;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

string CParameter::GetCDRecordFile()
{
	char puffer[512];
	GetOptionAlbumDir(puffer,512);
	
	string m_stgFolder = puffer;
	if(puffer[strlen(puffer) - 1] != '\\')
		m_stgFolder += "\\cdrecord.exe";
	else
		m_stgFolder += "cdrecord.exe";

	GetShortPathName(m_stgFolder.c_str(),puffer,512);

	return puffer;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

string CParameter::GetBurningDirectory()
{
	char puffer[512];
	GetOptionAlbumDir(puffer,512);
	string m_stgFolder = puffer;
	m_stgFolder += "\\BurningCD";

	//Test si le répertoire existe ou non
	if(!CObjet::TestRepertoire(m_stgFolder.c_str()))
		CreateDirectory(m_stgFolder.c_str(),NULL);

	return m_stgFolder;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

string CParameter::GetThumbnailDirectory()
{
	char puffer[512];
	GetOptionAlbumDir(puffer,512);
	string m_stgFolder = puffer;
	m_stgFolder += "\\thumbnail";

	//Test si le répertoire existe ou non
	if(!CObjet::TestRepertoire(m_stgFolder.c_str()))
		CreateDirectory(m_stgFolder.c_str(),NULL);

	return m_stgFolder;
}

//Ecriture
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
int CParameter::SetOptionDirectory(const char * m_szDirectory)
{
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->WriteOrCreateKeyValue("SOFTWARE\\Regards\\exploreur") == ERROR_SUCCESS)
		m_cRegistryAccess->WriteStringValue(m_szDirectory,"Repertoire");
	delete m_cRegistryAccess;

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//Répertoire de stockage par défaut des albums
//////////////////////////////////////////////////////////////////////////


//Ecriture
int CParameter::SetOptionAlbumDir(char * m_szDirectory)
{
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->WriteOrCreateKeyValue("SOFTWARE\\Regards\\navigation") == ERROR_SUCCESS)
	{
		m_cRegistryAccess->WriteStringValue(m_szDirectory,"Album");
	}
	delete m_cRegistryAccess;

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////////////////
int CParameter::GetDefaultAlbumDir(char * m_szDirectory,const int &iDirectorySize)
{
	char m_cCurrentDirectory[MAX_PATH];
	m_cCurrentDirectory[0] = '\0';
	GetCurrentDirectory(MAX_PATH,m_cCurrentDirectory);
	SetOptionAlbumDir(m_cCurrentDirectory);	
	StringCchCopy(m_szDirectory,MAX_PATH,m_cCurrentDirectory);
	return 0;
}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Lecture
int CParameter::GetOptionAlbumDir(char * m_szDirectory,const int &iDirectorySize)
{
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->OpenKeyValue("SOFTWARE\\Regards\\navigation") == ERROR_SUCCESS) 
	{
		m_cRegistryAccess->ReadStringValue(m_szDirectory,iDirectorySize,"Album");
		if(!CObjet::TestRepertoire(m_szDirectory))
		{
			GetDefaultAlbumDir(m_szDirectory,iDirectorySize);
		}
	}
	else
	{
		GetDefaultAlbumDir(m_szDirectory,iDirectorySize);
	}
	delete m_cRegistryAccess;
	return 0;
}
