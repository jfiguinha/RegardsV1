#include "stdafx.h"
#include ".\graveurisofunction.h"
#include ".\sharewizdatagrav.h"
#include <AspiDriver.h>
#include "objet.h"
#include "fichier.h"
#include "parameter.h"
#include <editredirector.h>

CGraveurISOFunction::CGraveurISOFunction(void)
{
}

CGraveurISOFunction::~CGraveurISOFunction(void)
{
}

LRESULT CGraveurISOFunction::InitGraveurList(HWND hWnd,CAspiDriver * m_cAspiDriver,int * iTabIndice)
{
	int iCount = 0;
	int iNbDevice = m_cAspiDriver->GetDeviceCount();

	for(int i = 0;i < iNbDevice;i++)
	{
		char m_cValue[256];
		string m_stgVendor,m_stgProduct,m_stgRevision,m_stgBusAddress;
		m_cAspiDriver->SetDevice(i);
		if(m_cAspiDriver->ItCanWrite())
		{
			iTabIndice[iCount] = i;
			m_cAspiDriver->GetDeviceString(m_stgVendor,m_stgProduct,m_stgRevision,m_stgBusAddress);
			//Test si une des chaines suivantes apparait pour savoir si c'est un graveur ou non
			StringCchPrintf(m_cValue,256,"%s %s %s",m_stgVendor.c_str(),m_stgProduct.c_str(),m_stgRevision.c_str());
			SendMessage(hWnd, CB_INSERTSTRING , iCount, (LPARAM)(LPCTSTR)m_cValue);
			SendMessage(hWnd, WM_SETTEXT , (WPARAM)0, (LPARAM)m_cValue);

			iCount++;
		}
	}

	SendMessage(hWnd, CB_SETCURSEL, 0, 0);
	return iCount;
}

LRESULT CGraveurISOFunction::TestCDSpace(CAspiDriver * m_cAspiDriver,const INT64 &m_lIsoFileSize)
{
	char m_szMessage[255];
	char m_szCaption[255];
	int iReturn;
	long NbBlocks = 0;
	long sizeBlocks = 0;
	//Obtention de la taille de la place restante
	long m_lBlockMax = m_cAspiDriver->GetLeadOutPos();
	long m_lCDCapacity = m_cAspiDriver->CDROMCapacity(NbBlocks,sizeBlocks);

	INT64 m_lFreeSpace = ((m_lBlockMax - NbBlocks) * sizeBlocks) - m_lIsoFileSize;
							
	if(m_lFreeSpace > 0)
	{
		iReturn = 1;
	}
	else
	{
		LoadString(hInstance,IDS_STGCHGCD,m_szMessage,255);
		LoadString(hInstance,IDS_INFOS,m_szCaption,255);

		if(MessageBox(NULL,m_szMessage,m_szCaption,MB_YESNO)==IDYES)
		{
			m_cAspiDriver->EjectCD(true);
			Sleep(3000);
			iReturn = 2;
		}
		else
		{
			iReturn = 0;
		}
	}

	return iReturn;
}

LRESULT CGraveurISOFunction::TestIsCDOK(CAspiDriver * m_cAspiDriver,const INT64 &m_lIsoFileSize,const int &iTypeGravure)
{
	char m_szMessage[255];
	char m_szCaption[255];
	int iReturn = 0;
	for(;;)
	{
		if(!m_cAspiDriver->IsCDReady())
		{
			//GetInfosDisc : + 
			int iNbTrack = 0;
			int iMediaInfos = m_cAspiDriver->InfosMedia(iNbTrack);

			if((iMediaInfos != 0 && iMediaInfos != 1) || iTypeGravure == 1)
			{
				if(m_cAspiDriver->InfosMediaErasable())
				{
					LoadString(hInstance,IDS_STGERASECD,m_szMessage,255);
					LoadString(hInstance,IDS_INFOS,m_szCaption,255);

					if(MessageBox(NULL,m_szMessage,m_szCaption,MB_YESNO)==IDYES)
					{
						m_cAspiDriver->EraseMedia(true);
						continue;
					}
					else
					{
						iReturn = 0;
						break;
					}
				}
				else
				{
					LoadString(hInstance,IDS_STGCONTINUE,m_szMessage,255);
					LoadString(hInstance,IDS_INFOS,m_szCaption,255);

					if(MessageBox(NULL,m_szMessage,m_szCaption,MB_YESNO)==IDYES)
					{
						continue;
					}
					else
					{
						iReturn = 0;
						break;
					}
				}
			}
			else
			{
				iReturn = TestCDSpace(m_cAspiDriver,m_lIsoFileSize);
				if(iReturn == 2)
					continue;
				else
					break;
			}

		}
		else
		{
			iReturn = TestCDSpace(m_cAspiDriver,m_lIsoFileSize);
			if(iReturn == 2)
				continue;
			else
				break;
		}
	}

	return iReturn;
}

//////////////////////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////////////////////
LRESULT CGraveurISOFunction::TestIsISOToolsOK(CShareWizDataGrav * pdata)
{
	if(CObjet::TestFilePresent(pdata->m_cParameter->GetMKisofsFile().c_str()))
	{
		pdata->iErreur = 0;
	}
	else
	{
		pdata->iErreur = 1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////////////////////
LRESULT CGraveurISOFunction::TestIsBurningToolsOK(CShareWizDataGrav * pdata)
{
	INT64 m_lIsoFileSize;
	if(pdata->iTypeGravure == 1)
	{
		//Obtention de la taille du fichier ISO
		m_lIsoFileSize = pdata->m_cEditRedirector->GetIsoFileSize(pdata->cFilename);

		//Test si le fichier ISO est disponible
		if(!CObjet::TestFilePresent(pdata->cFilename))
		{
			pdata->iErreur = 1;
			return 0;
		}
	}
	else
	{
		//Obtention de la taille du fichier ISO
		m_lIsoFileSize = pdata->m_cEditRedirector->GetIsoFileSize();

		//Test si le fichier ISO est disponible
		if(!CObjet::TestFilePresent(pdata->m_cEditRedirector->GetTempIsoFile().c_str()))
		{
			pdata->iErreur = 1;
			return 0;
		}

	}


	//Test si CD valide
	if(TestIsCDOK(pdata->m_cAspiDriver,m_lIsoFileSize,pdata->iTypeGravure) == 0)
	{
		pdata->iErreur = 1;
		return 0;
	}


	//Test si Programme disponible
	if(CObjet::TestFilePresent(pdata->m_cParameter->GetCDRecordFile().c_str()))
	{
		pdata->iErreur = 0;
	}
	else
	{
		pdata->iErreur = 1;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////
//Procedure de gravure de l'image ISO
///////////////////////////////////////////////////////////////////////////////////////
ULONG __stdcall CGraveurISOFunction::BurnIsoImage(LPVOID parametre)
{
	char m_cNewLigne[1024];
	CShareWizDataGrav *  pdata = (CShareWizDataGrav * )parametre;
	HWND hwnd = GetDlgItem(pdata->hWnd,IDC_EDOUTPUT);
	pdata->m_cEditRedirector->OnInitDialog(hwnd);

	

	if(pdata->iTypeGravure == 1)
	{
		char puffer[512];
		GetShortPathName(pdata->cFilename,puffer,512);
		StringCchPrintf(m_cNewLigne,1024,"%s -v -data dev=%d,%d,%d %s",pdata->m_cParameter->GetCDRecordFile().c_str(),pdata->m_cAspiDriver->GetHaId(),pdata->m_cAspiDriver->GetTarget(),pdata->m_cAspiDriver->GetLunId(),puffer);
	}
	else
		StringCchPrintf(m_cNewLigne,1024,"%s -v -multi -data dev=%d,%d,%d %s",pdata->m_cParameter->GetCDRecordFile().c_str(),pdata->m_cAspiDriver->GetHaId(),pdata->m_cAspiDriver->GetTarget(),pdata->m_cAspiDriver->GetLunId(),pdata->m_cEditRedirector->GetTempIsoFile().c_str());

	pdata->m_cEditRedirector->BurnIsoFile(pdata->hWnd,m_cNewLigne);

	ExitThread(0);
	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////
//Procedure de creation de l'image ISO
///////////////////////////////////////////////////////////////////////////////////////
ULONG __stdcall CGraveurISOFunction::CreateIsoImage(LPVOID parametre)
{
	CShareWizDataGrav *  pdata = (CShareWizDataGrav * )parametre;
	char m_szMessage[255];
	char m_szCaption[255];

	int iErreurFichier = 0;

	HWND hwnd = GetDlgItem(pdata->hWnd,IDC_EDOUTPUT);

	pdata->m_cEditRedirector->OnInitDialog(hwnd);
	IconeFileVector::iterator dit;

	//////////////////////////////////////////////////////////////////////////////////////
	//Création du répertoire de stockage
	//////////////////////////////////////////////////////////////////////////////////////

	if(pdata->iTypeGravure == 0)
	{
		if (!CreateDirectory(pdata->TempFile, NULL)) 
		{ 
			CFichier m_cFichier;
			//Suppression du Répertoire
			//CSADirRead m_cCSADirRead;
			m_cFichier.DeleteFolder(pdata->TempFile);

			if (!CreateDirectory(pdata->TempFile, NULL)) 
			{
				SendMessage(pdata->hWnd,WM_COMMAND,MAKEWPARAM(IDM_ERREUR,0),0);
				return -1;
			}
		} 

		
		//////////////////////////////////////////////////////////////////////////////////////
		//Importation des fichiers de l'album
		//////////////////////////////////////////////////////////////////////////////////////

		
		LoadString(hInstance,IDS_STGIMPORTIMAGE,m_szMessage,255);
		hwnd = GetDlgItem(pdata->hWnd,IDC_FILEIN);
		::SetWindowText(hwnd,m_szMessage);

		SendDlgItemMessage(pdata->hWnd, IDC_PRGFILE, PBM_SETRANGE , (WPARAM)0, MAKELPARAM(0,pdata->m_IconeFileVector->size()));
		SendDlgItemMessage(pdata->hWnd, IDC_PRGFILE, PBM_SETSTEP , (WPARAM)1, 0);


		//Importation des fichiers
		for (dit = pdata->m_IconeFileVector->begin(); dit != pdata->m_IconeFileVector->end(); dit++)
		{
			//Copie des fichiers
			CObjet m_cObjet;
			char m_szFilename[MAX_PATH];
			m_cObjet.GetFileName(dit->m_stgFullPathName,m_szFilename,MAX_PATH);
			string m_stgFilename = pdata->TempFile;
			m_stgFilename += "\\";
			m_stgFilename += m_szFilename;
			if(!CopyFile(dit->m_stgFullPathName,m_stgFilename.c_str(),true) && iErreurFichier == 0)
			{
					LoadString(hInstance,IDS_STGPBGRAVE,m_szMessage,255);
					LoadString(hInstance,IDS_INFOS,m_szCaption,255);

					if(MessageBox(NULL,m_szMessage,m_szCaption,MB_YESNO)==IDNO)
					{

						//Suppression du Répertoire
						//CSADirRead m_cCSADirRead;
						CFichier m_cFichier;
						m_cFichier.DeleteFolder(pdata->TempFile);
						SendMessage(pdata->hWnd,WM_COMMAND,MAKEWPARAM(IDM_ERREUR,0),0);				
						return 1;
					}
					else
					{
						iErreurFichier = 1;
					}
			}
			SendDlgItemMessage(pdata->hWnd, IDC_PRGFILE, PBM_STEPIT , 0, 0);
			hwnd = GetDlgItem(pdata->hWnd,IDC_FILEOUT);
			::SetWindowText(hwnd,dit->m_stgFullPathName);
		}
	}

	///////////////////////////////////////////////////////////////////////////
	//Création de l'image ISO
	///////////////////////////////////////////////////////////////////////////
	
	pdata->m_cEditRedirector->CreateIsoFile(pdata->hWnd,pdata->m_cAspiDriver,pdata->TempFile);

	ExitThread(0);
	return 1;
}