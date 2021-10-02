#include "stdafx.h"
#include ".\deplacefichier.h"
#include "objet.h"
#include <DeplaceFile.h>
#include <IconeFile.h>
#include <SADirRead.h>
#include "resource.h"

CDeplaceFichier::CDeplaceFichier(void)
{
}

CDeplaceFichier::~CDeplaceFichier(void)
{
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

void CDeplaceFichier::CopyDirectory(const char * m_szFilenameSource, const char * m_szFilenameDestination, HWND hWnd)
{
	CSADirRead m_cCSADirRead;
	SADirVector dirs;
	SADirVector::iterator dit;

	m_cCSADirRead.m_bRecurse = true;

	m_cCSADirRead.GetDirs(m_szFilenameSource);

	dirs = m_cCSADirRead.Dirs();

	for(dit = dirs.begin(); dit != dirs.end();dit++)
	{
		char cDirTemp[MAX_PATH];

		CSADirEntry m_dirValue = *dit;

		StringCchPrintf(cDirTemp,MAX_PATH,"%s\\%s",m_szFilenameDestination,m_dirValue.m_sNameOnly.c_str());

		//Création du répertoire
		CreateDirectory(cDirTemp,NULL);
		CopyFileDirectory(m_dirValue.m_sName.c_str(),cDirTemp,hWnd);
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

void CDeplaceFichier::CopyFileDirectory(const char * m_szFilenameSource, const char * m_szFilenameDestination, HWND hWnd)
{
	//Création du répertoire parent

	CSADirRead m_cCSADirRead;
	SAFileVector files;
	SAFileVector::iterator fit;

	m_cCSADirRead.m_bRecurse = false;

	m_cCSADirRead.GetDirs(m_szFilenameSource);
	m_cCSADirRead.GetFiles("*.*");

	files = m_cCSADirRead.Files();

	int iNbElement = SendDlgItemMessage(hWnd, IDC_PRGFILE, PBM_GETRANGE , (WPARAM)FALSE, 0);
	iNbElement += files.size();
	SendDlgItemMessage(hWnd, IDC_PRGFILE, PBM_SETRANGE , (WPARAM)0, MAKELPARAM(0,iNbElement));

	for(fit = files.begin(); fit != files.end();fit++)
	{
		char cFileDestination[MAX_PATH];

		CSAFileEntry  m_FileValue = *fit;

		StringCchPrintf(cFileDestination,MAX_PATH,"%s\\%s",m_szFilenameDestination,m_FileValue.m_sNameOnly.c_str());


		SendDlgItemMessage(hWnd, IDC_FILEIN, WM_SETTEXT , (WPARAM)0, (LPARAM)m_FileValue.m_sNameOnly.c_str());
		SendDlgItemMessage(hWnd, IDC_FILEOUT, WM_SETTEXT , (WPARAM)0, (LPARAM)m_szFilenameDestination);
		SendDlgItemMessage(hWnd, IDC_PRGFILE, PBM_STEPIT , (WPARAM)0, 0);


		//Création du répertoire
		if(CopyFile(m_FileValue.m_sName.c_str(),cFileDestination,TRUE) != TRUE)
			return;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

ULONG __stdcall CDeplaceFichier::CopierFichier(LPVOID parametre)
{
	OleInitialize(NULL);
	CObjet m_CObjet;
	char Temp[MAX_PATH];
	char m_chrFilenameSource[MAX_PATH];
	CIconeFile m_IconeFile;
	CDeplaceFichier * m_lpDeplaceFile = (CDeplaceFichier *) parametre;

	int iNbFile = 0;

	for (IconeFileVector::const_iterator dit = m_lpDeplaceFile->m_cDeplaceFile->m_IconeFile->begin(); dit!= m_lpDeplaceFile->m_cDeplaceFile->m_IconeFile->end(); dit++)
	{
		m_IconeFile = *dit;
		MSG msg;
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			break;

		if(!m_lpDeplaceFile->m_cDeplaceFile->iCopierFichier)
		{
			iNbFile++;

			m_CObjet.GetFileName(m_IconeFile.m_stgFullPathName, Temp,MAX_PATH);
			StringCchPrintf(m_chrFilenameSource,MAX_PATH,"%s\\%s",m_lpDeplaceFile->m_cDeplaceFile->cDir, Temp);

			SendDlgItemMessage(m_lpDeplaceFile->m_cDeplaceFile->hWndParent, IDC_FILEIN, WM_SETTEXT , (WPARAM)0, (LPARAM)m_IconeFile.m_stgFullPathName);
			SendDlgItemMessage(m_lpDeplaceFile->m_cDeplaceFile->hWndParent, IDC_FILEOUT, WM_SETTEXT , (WPARAM)0, (LPARAM)m_chrFilenameSource);
			SendDlgItemMessage(m_lpDeplaceFile->m_cDeplaceFile->hWndParent, IDC_PRGFILE, PBM_STEPIT , (WPARAM)0, 0);

			//Test si le fichier est un répertoire 
			if(m_CObjet.TestRepertoire(m_IconeFile.m_stgFullPathName))
			{
				CopyDirectory(m_IconeFile.m_stgFullPathName,m_lpDeplaceFile->m_cDeplaceFile->cDir,m_lpDeplaceFile->m_cDeplaceFile->hWndParent);
				continue;
			}

			if(m_lpDeplaceFile->m_cDeplaceFile->iDepl == 0)
				CopyFile(m_IconeFile.m_stgFullPathName,m_chrFilenameSource,TRUE);
			else
				rename(m_IconeFile.m_stgFullPathName, m_chrFilenameSource);
		}
	
	}

	m_lpDeplaceFile->m_cDeplaceFile->iCopierFichier = 1;

	EndDialog(m_lpDeplaceFile->m_cDeplaceFile->hWndParent,0);

	ExitThread(0);
	OleUninitialize();
	return true;
}

LRESULT CDeplaceFichier::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_cDeplaceFile != NULL)
	{
		m_cDeplaceFile->hWndParent = hDlg;
		m_cDeplaceFile->iCopierFichier = 0;

		SendDlgItemMessage(hDlg, IDC_PRGFILE, PBM_SETRANGE , (WPARAM)0, MAKELPARAM(0,m_cDeplaceFile->m_IconeFile->size()));
		SendDlgItemMessage(hDlg, IDC_PRGFILE, PBM_SETSTEP , (WPARAM)1, 0);

		hThread = (HANDLE)	CreateThread(NULL, cuStackSize,	CopierFichier, this, 0, &dwThreadId);

		SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);
	}
	return 0;
}

LRESULT CDeplaceFichier::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	wmId    = LOWORD(wParam); 
	wmEvent = HIWORD(wParam); 
	int bResult;
	// Parse the menu selections:

	bResult = PostThreadMessage(dwThreadId, WM_NULL, 0, 0);

	Sleep(1000);
	return 0;
}
