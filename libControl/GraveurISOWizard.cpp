#include "stdafx.h"
#include ".\graveurisowizard.h"
#include "objet.h"
#include <editredirector.h>
#include "AspiDriver.h"
#include <process.h>
#include "fichier.h"
#include ".\sharewizdatagrav.h"
#include ".\graveurisofunction.h"


///////////////////////////////////////////////////////////////////////////////////////
//Constructeur 
///////////////////////////////////////////////////////////////////////////////////////

CGraveurISOWizard::CGraveurISOWizard(void)
{
}

///////////////////////////////////////////////////////////////////////////////////////
//Destructeur
///////////////////////////////////////////////////////////////////////////////////////

CGraveurISOWizard::~CGraveurISOWizard(void)
{
}


///////////////////////////////////////////////////////////////////////////
//Procedure de gravure d'un album
///////////////////////////////////////////////////////////////////////////

void CGraveurISOWizard::CreateAlbumWizard(HWND hParent, const string &m_stgFilename, const int &iType, IconeFileVector m_IconeFileVector)
{
	CreatePropertySheet(hParent,m_stgFilename,iType,m_IconeFileVector);
}

///////////////////////////////////////////////////////////////////////////
//Procedure de gravure d'un fichier ISO
///////////////////////////////////////////////////////////////////////////

void CGraveurISOWizard::BurnIsoWizard(HWND hParent, const string &m_stgFilename)
{
	CreatePropertySheetIso(hParent,m_stgFilename);
}

void CGraveurISOWizard::CreateFolderWizard(HWND hParent, const string &m_stgFilename)
{
	CreatePropertySheetFolder(hParent,m_stgFilename);
}

int CGraveurISOWizard::CreatePropertySheet(HWND hwndOwner, const string &m_stgFilename, const int &iType, IconeFileVector m_IconeFileVector)
{
    PROPSHEETPAGE   psp =       {0}; //defines the property sheet pages
    HPROPSHEETPAGE  ahpsp[5] =  {0}; //an array to hold the page's HPROPSHEETPAGE handles
    PROPSHEETHEADER psh =       {0}; //defines the property sheet
    CShareWizDataGrav * wizdata = new CShareWizDataGrav(); //the shared data structure
    //
    //Create the Wizard pages
    //

    //Opening page - Introduction

	StringCchCopy(wizdata->cFilename,MAX_PATH,m_stgFilename.c_str());
	wizdata->m_IconeFileVector = &m_IconeFileVector;


    psp.dwSize =        sizeof(psp);
    psp.dwFlags =       PSP_DEFAULT | PSP_HIDEHEADER ;
    psp.hInstance =     hInstance;
    psp.lParam =        (LPARAM) wizdata; //The shared data structure
    psp.pfnDlgProc =    InitDlgProc;
    psp.pszTemplate =   MAKEINTRESOURCE(IDD_DEBUTGRAVURE);
    ahpsp[0] =          CreatePropertySheetPage(&psp);

	//////////////////////////////////////////////////////////////////
    //First interior page - Menu de selection
	//////////////////////////////////////////////////////////////////

    psp.dwFlags =           PSP_DEFAULT|PSP_USEHEADERTITLE;//|PSP_USEHEADERSUBTITLE;
    psp.pszHeaderTitle =    MAKEINTRESOURCE(IDS_GRAVUREMENU);
    psp.pszTemplate =       MAKEINTRESOURCE(IDD_DLGSELECTGRAVEUR);
    psp.pfnDlgProc =        SelectBurner;
    ahpsp[1] =              CreatePropertySheetPage(&psp);

	//////////////////////////////////////////////////////////////////
    //First interior page - Creation de l'image ISO
	//////////////////////////////////////////////////////////////////

    psp.dwFlags =           PSP_DEFAULT|PSP_USEHEADERTITLE;//|PSP_USEHEADERSUBTITLE;
    psp.pszHeaderTitle =    MAKEINTRESOURCE(IDS_GRAVUREMENU);
    psp.pszTemplate =       MAKEINTRESOURCE(IDD_DLGISOCREATE);
    psp.pfnDlgProc =        CreateIsoFile;
    ahpsp[2] =              CreatePropertySheetPage(&psp);

	//////////////////////////////////////////////////////////////////
    //First interior page - Gravure de l'image ISO
	//////////////////////////////////////////////////////////////////

    psp.dwFlags =           PSP_DEFAULT|PSP_USEHEADERTITLE;//|PSP_USEHEADERSUBTITLE;
    psp.pszHeaderTitle =    MAKEINTRESOURCE(IDS_GRAVUREMENU);
    psp.pszTemplate =       MAKEINTRESOURCE(IDD_BURNISO);
    psp.pfnDlgProc =        GravureDlgProc;
    ahpsp[3] =              CreatePropertySheetPage(&psp);


	//////////////////////////////////////////////////////////////////
    //Final page - Fin du Wizard
	//////////////////////////////////////////////////////////////////

    psp.dwFlags =       PSP_DEFAULT|PSP_HIDEHEADER;
    psp.pszTemplate =   MAKEINTRESOURCE(IDD_FINGRAVURE);
    psp.pfnDlgProc =    EndDlgProc;
    ahpsp[4] =          CreatePropertySheetPage(&psp);

	//////////////////////////////////////////////////////////////////
    //Create the property sheet
	//////////////////////////////////////////////////////////////////


    psh.dwSize =            sizeof(psh);
    psh.hInstance =         hInstance;
    psh.hwndParent =        hwndOwner;
    psh.phpage =            ahpsp;
    psh.dwFlags =           PSH_WIZARD97 | PSH_WATERMARK | PSH_HEADER | PSH_WIZARDHASFINISH;
    psh.pszbmWatermark =    MAKEINTRESOURCE(IDB_WATERMARK256);
    psh.pszbmHeader =       MAKEINTRESOURCE(IDB_BANNER256);
    psh.nStartPage =        0;
    psh.nPages =            5;


	wizdata->CreateFont();

    //Display the wizard

    PropertySheet(&psh);

	if(wizdata->TempFile[0] != '\0')
	{
		CFichier m_cFichier;
		m_cFichier.DeleteFolder(wizdata->TempFile);
	}

    
    // Destroy the fonts
    delete wizdata;
	return 0;
}

int CGraveurISOWizard::CreatePropertySheetFolder(HWND hwndOwner, const string &m_stgFilename)
{
    PROPSHEETPAGE   psp =       {0}; //defines the property sheet pages
    HPROPSHEETPAGE  ahpsp[5] =  {0}; //an array to hold the page's HPROPSHEETPAGE handles
    PROPSHEETHEADER psh =       {0}; //defines the property sheet
    CShareWizDataGrav * wizdata = new CShareWizDataGrav(); //the shared data structure
    //
    //Create the Wizard pages
    //

    //Opening page - Introduction

	wizdata->iTypeGravure = 3;
	StringCchCopy(wizdata->cFilename,MAX_PATH,m_stgFilename.c_str());
	
    psp.dwSize =        sizeof(psp);
    psp.dwFlags =       PSP_DEFAULT | PSP_HIDEHEADER ;
    psp.hInstance =     hInstance;
    psp.lParam =        (LPARAM) wizdata; //The shared data structure
    psp.pfnDlgProc =    InitDlgProc;
    psp.pszTemplate =   MAKEINTRESOURCE(IDD_DEBUTGRAVURE);
    ahpsp[0] =          CreatePropertySheetPage(&psp);

	//////////////////////////////////////////////////////////////////
    //First interior page - Menu de selection
	//////////////////////////////////////////////////////////////////

    psp.dwFlags =           PSP_DEFAULT|PSP_USEHEADERTITLE;//|PSP_USEHEADERSUBTITLE;
    psp.pszHeaderTitle =    MAKEINTRESOURCE(IDS_GRAVUREMENU);
    psp.pszTemplate =       MAKEINTRESOURCE(IDD_DLGSELECTGRAVEUR);
    psp.pfnDlgProc =        SelectBurner;
    ahpsp[1] =              CreatePropertySheetPage(&psp);

	//////////////////////////////////////////////////////////////////
    //First interior page - Creation de l'image ISO
	//////////////////////////////////////////////////////////////////

    psp.dwFlags =           PSP_DEFAULT|PSP_USEHEADERTITLE;//|PSP_USEHEADERSUBTITLE;
    psp.pszHeaderTitle =    MAKEINTRESOURCE(IDS_GRAVUREMENU);
    psp.pszTemplate =       MAKEINTRESOURCE(IDD_DLGISOCREATE);
    psp.pfnDlgProc =        CreateIsoFile;
    ahpsp[2] =              CreatePropertySheetPage(&psp);

	//////////////////////////////////////////////////////////////////
    //First interior page - Gravure de l'image ISO
	//////////////////////////////////////////////////////////////////

    psp.dwFlags =           PSP_DEFAULT|PSP_USEHEADERTITLE;//|PSP_USEHEADERSUBTITLE;
    psp.pszHeaderTitle =    MAKEINTRESOURCE(IDS_GRAVUREMENU);
    psp.pszTemplate =       MAKEINTRESOURCE(IDD_BURNISO);
    psp.pfnDlgProc =        GravureDlgProc;
    ahpsp[3] =              CreatePropertySheetPage(&psp);


	//////////////////////////////////////////////////////////////////
    //Final page - Fin du Wizard
	//////////////////////////////////////////////////////////////////

    psp.dwFlags =       PSP_DEFAULT|PSP_HIDEHEADER;
    psp.pszTemplate =   MAKEINTRESOURCE(IDD_FINGRAVURE);
    psp.pfnDlgProc =    EndDlgProc;
    ahpsp[4] =          CreatePropertySheetPage(&psp);

	//////////////////////////////////////////////////////////////////
    //Create the property sheet
	//////////////////////////////////////////////////////////////////


    psh.dwSize =            sizeof(psh);
    psh.hInstance =         hInstance;
    psh.hwndParent =        hwndOwner;
    psh.phpage =            ahpsp;
    psh.dwFlags =           PSH_WIZARD97 | PSH_WATERMARK | PSH_HEADER | PSH_WIZARDHASFINISH;
    psh.pszbmWatermark =    MAKEINTRESOURCE(IDB_WATERMARK256);
    psh.pszbmHeader =       MAKEINTRESOURCE(IDB_BANNER256);
    psh.nStartPage =        0;
    psh.nPages =            5;


	wizdata->CreateFont();
    //Display the wizard

    PropertySheet(&psh);

	delete wizdata;
	return 0;
}

int CGraveurISOWizard::CreatePropertySheetIso(HWND hwndOwner, const string &m_stgFilename)
{
    PROPSHEETPAGE   psp =       {0}; //defines the property sheet pages
    HPROPSHEETPAGE  ahpsp[4] =  {0}; //an array to hold the page's HPROPSHEETPAGE handles
    PROPSHEETHEADER psh =       {0}; //defines the property sheet
    CShareWizDataGrav * wizdata = new CShareWizDataGrav(); //the shared data structure
    //
    //Create the Wizard pages
    //

    //Opening page - Introduction

	StringCchCopy(wizdata->cFilename,MAX_PATH,m_stgFilename.c_str());
	wizdata->iTypeGravure = 1;

    psp.dwSize =        sizeof(psp);
    psp.dwFlags =       PSP_DEFAULT | PSP_HIDEHEADER ;
    psp.hInstance =     hInstance;
    psp.lParam =        (LPARAM) wizdata; //The shared data structure
    psp.pfnDlgProc =    InitDlgProc;
    psp.pszTemplate =   MAKEINTRESOURCE(IDD_DEBUTGRAVURE);
    ahpsp[0] =          CreatePropertySheetPage(&psp);

	//////////////////////////////////////////////////////////////////
    //First interior page - Menu de selection
	//////////////////////////////////////////////////////////////////

    psp.dwFlags =           PSP_DEFAULT|PSP_USEHEADERTITLE;//|PSP_USEHEADERSUBTITLE;
    psp.pszHeaderTitle =    MAKEINTRESOURCE(IDS_GRAVUREMENU);
    psp.pszTemplate =       MAKEINTRESOURCE(IDD_DLGSELECTGRAVEUR);
    psp.pfnDlgProc =        SelectBurner;
    ahpsp[1] =              CreatePropertySheetPage(&psp);

	//////////////////////////////////////////////////////////////////
    //First interior page - Gravure de l'image ISO
	//////////////////////////////////////////////////////////////////

    psp.dwFlags =           PSP_DEFAULT|PSP_USEHEADERTITLE;//|PSP_USEHEADERSUBTITLE;
    psp.pszHeaderTitle =    MAKEINTRESOURCE(IDS_GRAVUREMENU);
    psp.pszTemplate =       MAKEINTRESOURCE(IDD_BURNISO);
    psp.pfnDlgProc =        GravureDlgProc;
    ahpsp[2] =              CreatePropertySheetPage(&psp);


	//////////////////////////////////////////////////////////////////
    //Final page - Fin du Wizard
	//////////////////////////////////////////////////////////////////

    psp.dwFlags =       PSP_DEFAULT|PSP_HIDEHEADER;
    psp.pszTemplate =   MAKEINTRESOURCE(IDD_FINGRAVURE);
    psp.pfnDlgProc =    EndDlgProc;
    ahpsp[3] =          CreatePropertySheetPage(&psp);

	//////////////////////////////////////////////////////////////////
    //Create the property sheet
	//////////////////////////////////////////////////////////////////


    psh.dwSize =            sizeof(psh);
    psh.hInstance =         hInstance;
    psh.hwndParent =        hwndOwner;
    psh.phpage =            ahpsp;
    psh.dwFlags =           PSH_WIZARD97 | PSH_WATERMARK | PSH_HEADER | PSH_WIZARDHASFINISH;
    psh.pszbmWatermark =    MAKEINTRESOURCE(IDB_WATERMARK256);
    psh.pszbmHeader =       MAKEINTRESOURCE(IDB_BANNER256);
    psh.nStartPage =        0;
    psh.nPages =            4;


    //Set up the font for the titles on the intro and ending pages
	wizdata->CreateFont();

    //Display the wizard

    PropertySheet(&psh);

	delete wizdata;
	return 0;
}

BOOL CGraveurISOWizard::InitDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CShareWizDataGrav * pdata = (CShareWizDataGrav *) GetWindowLong(hwndDlg, GWL_USERDATA);

    switch (uMsg)
    {
    case WM_INITDIALOG :
        { 

			//Centrage de la fenetre
			//Modification des font utilisées
            pdata = (CShareWizDataGrav *) ((LPPROPSHEETPAGE) lParam) -> lParam;
			SendDlgItemMessage(hwndDlg, IDC_TITREGRAVURE, WM_SETFONT , (WPARAM)pdata->hTitleFont, (LPARAM)0);
            SetWindowLong(hwndDlg, GWL_USERDATA, (DWORD_PTR) pdata);
			CObjet::CenterWindow(GetParent(hwndDlg));
            break;
        }
		
    case WM_COMMAND :
        PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_NEXT | PSWIZB_FINISH);
        break;

    case WM_NOTIFY :
        {
        LPNMHDR lpnm = (LPNMHDR) lParam;

        switch (lpnm->code)
            {

            case PSN_SETACTIVE : //Enable the appropriate buttons.
                PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_NEXT | PSWIZB_FINISH);
				CObjet::CenterWindow(GetParent(hwndDlg));
                break;

            case PSN_WIZNEXT :
                break;

			case PSN_WIZFINISH :
				break;

            default :
                break;
            }
        }
        break;

    default:
        break;
    }
    return 0;
}

BOOL CGraveurISOWizard::SelectBurner (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG :
        { 
			CGraveurISOFunction m_cGraveurFunction;
			CShareWizDataGrav * pdata = (CShareWizDataGrav *) ((LPPROPSHEETPAGE) lParam) -> lParam;
			SetWindowLong(hwndDlg, GWL_USERDATA, (DWORD_PTR) pdata);
			pdata->iValueChecked = 1;
			pdata->IsButtonClicked = TRUE;
			pdata->hWnd = hwndDlg;
			pdata->iErreur = 0;
			
			HWND hDlg = GetDlgItem(hwndDlg,IDC_CBGRAVEUR);
			if(m_cGraveurFunction.InitGraveurList(hDlg,pdata->m_cAspiDriver,pdata->iTabIndice) == 0)
			{
				pdata->iErreur = 1;
				//SendMessage(GetParent(hwndDlg),PSM_PRESSBUTTON,PSBTN_NEXT,0);
				EndDialog(hwndDlg,0);
			}
			
            break;
        }
	
    case WM_COMMAND :
		{
			CShareWizDataGrav * pdata = (CShareWizDataGrav *) GetWindowLong (hwndDlg, GWL_USERDATA);
			PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_NEXT | PSWIZB_FINISH);

			int wmId, wmEvent;
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			switch(wmId)
			{
				case IDM_ERREUR:
					pdata->iErreur = 1;
					SendMessage(GetParent(hwndDlg),PSM_PRESSBUTTON,PSBTN_NEXT,0);
					break;
			}
		}
		break;

	case WM_DESTROY:
		break;
        

    case WM_NOTIFY :
        {
        LPNMHDR lpnm = (LPNMHDR) lParam;
		CShareWizDataGrav * pdata = (CShareWizDataGrav *) GetWindowLong (hwndDlg, GWL_USERDATA);
        switch (lpnm->code)
            {

            case PSN_SETACTIVE : //Enable the appropriate buttons.
				PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_NEXT | PSWIZB_FINISH);
                break;

            case PSN_WIZNEXT :
				{
					////////////////////////////////////////////////////////////
					//Test si la gravure peut continuer
					////////////////////////////////////////////////////////////
					HWND hDlg = GetDlgItem(hwndDlg,IDC_CBGRAVEUR);
					//Recupération du graveur sélectionné
					int iSelection = SendMessage(hDlg, CB_GETCURSEL, 0, 0);
					iSelection = pdata->iTabIndice[iSelection];
					pdata->m_cAspiDriver->SetDevice(iSelection);


					if(pdata->iErreur)
					{
						SetWindowLong(hwndDlg, DWL_MSGRESULT, IDD_FINGRAVURE);
						return true;
					}

					if(pdata->iTypeGravure == 0 || pdata->iTypeGravure == 3)
					{
						CGraveurISOFunction m_cGraveurFunction;
						//Vérification pour la création d'un fichier ISO
						m_cGraveurFunction.TestIsISOToolsOK(pdata);
					}
					else
					{
						CGraveurISOFunction m_cGraveurFunction;
						//Vérification pour la gravure d'un fichier ISO
						m_cGraveurFunction.TestIsBurningToolsOK(pdata);
					}

					if(pdata->iErreur)
					{
						SetWindowLong(hwndDlg, DWL_MSGRESULT, IDD_FINGRAVURE);
						return true;
					}
				}

                break;

			case PSN_WIZFINISH :
				break;

            default :
                break;
            }
        }
        break;

    default:
        break;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//Procedure de creation de l'image ISO
///////////////////////////////////////////////////////////////////////////////////////////////

BOOL CGraveurISOWizard::CreateIsoFile (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG :
        { 
			CShareWizDataGrav * pdata = (CShareWizDataGrav *) ((LPPROPSHEETPAGE) lParam) -> lParam;
			SetWindowLong(hwndDlg, GWL_USERDATA, (DWORD_PTR) pdata);
			pdata->iValueChecked = 1;
			pdata->IsButtonClicked = TRUE;
			pdata->hWnd = hwndDlg;
			pdata->iErreur = 0;
			pdata->hThread = NULL;
			
			if (pdata->m_cEditRedirector == NULL)
				pdata->m_cEditRedirector = new CEditRedirector();

			//Génération du nom du répertoire temporaire
			SYSTEMTIME SystemTime;
			GetSystemTime(&SystemTime);

			if(pdata->iTypeGravure == 3)
				StringCchCopy(pdata->TempFile,512,pdata->cFilename);
			else
				StringCchPrintf(pdata->TempFile,512,"%s%d%d%d%d%d%d",pdata->m_cEditRedirector->GetTempIsoDirectory().c_str(), SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay,
					SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond);

			SendMessage(GetParent(hwndDlg),PSM_CANCELTOCLOSE,0,0);

			pdata->hThread = (HANDLE)CreateThread(NULL, 1024,
				CGraveurISOFunction::CreateIsoImage, pdata, 0, &pdata->dwThreadId);

            break;
        }
	
    case WM_COMMAND :
		{
			CShareWizDataGrav * pdata = (CShareWizDataGrav *) GetWindowLong (hwndDlg, GWL_USERDATA);
			PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_DISABLEDFINISH);

			int wmId, wmEvent;
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 

			char m_szMessage[255];

			switch (wmId)
			{
				case IDM_ERREUR:
					pdata->iErreur = 1;
					SendMessage(GetParent(hwndDlg),PSM_PRESSBUTTON,PSBTN_NEXT,0);
					break;

				case IDM_SENDTEXT:
					{
						int iPos;
						char * pzOutput = (char *)lParam;

						HWND hWnd = GetDlgItem(hwndDlg,IDC_FILEIN);

						LoadString(hInstance,IDS_STGISOCREATION,m_szMessage,255);
						SetWindowText(hWnd,m_szMessage);

						hWnd = GetDlgItem(hwndDlg,IDC_FILEOUT);

						LoadString(hInstance,IDS_STGENDTHE,m_szMessage,255);
						string m_stgMessage = m_szMessage;
						string m_stgOutput = pzOutput;

						iPos = m_stgOutput.find("Invalid");
						if(iPos != -1)
						{
							LoadString(hInstance,IDS_ERREUR,m_szMessage,255);
							MessageBox(NULL,pzOutput,m_szMessage,0);
							pdata->iErreur = 1;
							break;
						}

						iPos = m_stgOutput.find("finish");

						m_stgOutput = m_stgOutput.substr(iPos+7,m_stgOutput.length() - (iPos + 7));

						m_stgMessage += m_stgOutput;

						SetWindowText(hWnd,m_stgMessage.c_str());

						iPos = atoi(pzOutput);
						SendDlgItemMessage(hwndDlg, IDC_PRGFILE, PBM_SETPOS , iPos, 0);

						break;
					}
			}

		}
		break;

	case WM_DESTROY:
		break;
        

    case WM_NOTIFY :
        {
		CShareWizDataGrav * pdata = (CShareWizDataGrav *) GetWindowLong (hwndDlg, GWL_USERDATA);
        LPNMHDR lpnm = (LPNMHDR) lParam;

        switch (lpnm->code)
            {

			case PSN_QUERYCANCEL:
				{
					SetWindowLong(hwndDlg, DWL_MSGRESULT, 0 );
					return TRUE;
				}
				break;

            case PSN_SETACTIVE : //Enable the appropriate buttons.
				PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_DISABLEDFINISH);//PSWIZB_NEXT | PSWIZB_FINISH);
                break;

            case PSN_WIZNEXT :
				{
					CGraveurISOFunction m_cGraveurFunction;
					//Test si la gravure peut se faire
					m_cGraveurFunction.TestIsBurningToolsOK(pdata);

					//Forcer à finir
					if(pdata->m_cEditRedirector != NULL)
					{
						delete pdata->m_cEditRedirector;
						pdata->m_cEditRedirector = NULL;
					}

					if(pdata->hThread != NULL)
					{
						CloseHandle(pdata->hThread);
						pdata->hThread = NULL;
					}

					if(pdata->iErreur)
					{
						remove(pdata->m_cEditRedirector->GetTempIsoFile().c_str());

						SetWindowLong(hwndDlg, DWL_MSGRESULT, IDD_FINGRAVURE);
					}
					else
						SetWindowLong(hwndDlg, DWL_MSGRESULT, IDD_BURNISO);

					return TRUE;

				}
                break;

			case PSN_WIZFINISH :
				{

					remove(pdata->m_cEditRedirector->GetTempIsoFile().c_str());

					if(pdata->m_cEditRedirector != NULL)
					{
						delete pdata->m_cEditRedirector;
						pdata->m_cEditRedirector = NULL;
					}

					if(pdata->hThread != NULL)
					{
						CloseHandle(pdata->hThread);
						pdata->hThread = NULL;
					}
					return TRUE;
				}
				break;

            default :
                break;
            }
        }
        break;

    default:
        break;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//Procedure de gravure de l'image ISO
///////////////////////////////////////////////////////////////////////////////////////////////

BOOL CGraveurISOWizard::GravureDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG :
        { 
			long m_lIsoFileSize = 0;
			CShareWizDataGrav * pdata = (CShareWizDataGrav *) ((LPPROPSHEETPAGE) lParam) -> lParam;
			SetWindowLong(hwndDlg, GWL_USERDATA, (DWORD_PTR) pdata);
			pdata->iValueChecked = 1;
			pdata->IsButtonClicked = TRUE;
			pdata->hWnd = hwndDlg;

			if(pdata->m_cEditRedirector == NULL)
				pdata->m_cEditRedirector = new CEditRedirector();

			pdata->hThread = (HANDLE)CreateThread(NULL, 1024,
				CGraveurISOFunction::BurnIsoImage, pdata, 0, &pdata->dwThreadId);

			SendMessage(GetParent(hwndDlg),PSM_CANCELTOCLOSE,0,0);

			break;
        }
	
    case WM_COMMAND :
		{
			CShareWizDataGrav *pdata = (CShareWizDataGrav *) GetWindowLong (hwndDlg, GWL_USERDATA);
			PropSheet_SetWizButtons(GetParent(hwndDlg), NULL);

			int wmId, wmEvent;
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			switch (wmId)
				{

					case IDM_ERREUR:
						pdata->iErreur = 1;
						SendMessage(GetParent(hwndDlg),PSM_PRESSBUTTON,PSBTN_NEXT,0);
						break;

					case IDM_SENDTEXT:
						{
							char * pzOutput = (char *)lParam;
							pdata->TraitementTextFromBurner(hwndDlg,pzOutput);
							break;
						}
				}
		}
		break;

	case WM_DESTROY:
		break;
        

    case WM_NOTIFY :
        {
		CShareWizDataGrav *pdata = (CShareWizDataGrav *) GetWindowLong (hwndDlg, GWL_USERDATA);
        LPNMHDR lpnm = (LPNMHDR) lParam;

        switch (lpnm->code)
            {

            case PSN_SETACTIVE : //Enable the appropriate buttons.
				PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_DISABLEDFINISH);
                break;

			case PSN_WIZFINISH :
            case PSN_WIZNEXT :
				{
					if(pdata->iTypeGravure == 0 || pdata->iTypeGravure == 3)
					{
						_chmod(pdata->m_cEditRedirector->GetTempIsoFile().c_str(),_S_IREAD | _S_IWRITE);
						DeleteFile(pdata->m_cEditRedirector->GetTempIsoFile().c_str());
					}

					//Forcer à finir
					if(pdata->m_cEditRedirector != NULL)
					{
						delete pdata->m_cEditRedirector;
						pdata->m_cEditRedirector = NULL;
					}

					if(pdata->hThread != NULL)
					{
						CloseHandle(pdata->hThread);
						pdata->hThread = NULL;
					}

					if(pdata->iErreur == 2)
					{
						SetWindowLong(hwndDlg, DWL_MSGRESULT, IDD_BURNISO);
						return TRUE;
					}
				}
                break;

            default :
                break;
            }
        }
        break;

    default:
        break;
    }
    return 0;
}

BOOL CGraveurISOWizard::EndDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG :
        { 
             CShareWizDataGrav * pdata = (CShareWizDataGrav *) ((LPPROPSHEETPAGE) lParam) -> lParam;
            SetWindowLong(hwndDlg, GWL_USERDATA, (DWORD_PTR) pdata);
			if(pdata->iErreur == 0)
				pdata->m_cAspiDriver->EjectCD(true);
            break;
        }
		
    case WM_COMMAND :
        PropSheet_SetWizButtons(GetParent(hwndDlg), NULL);
        break;

    case WM_NOTIFY :
        {
        LPNMHDR lpnm = (LPNMHDR) lParam;

        switch (lpnm->code)
            {
            case PSN_SETACTIVE : //Enable the appropriate buttons.
				PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_FINISH);
                break;

            case PSN_WIZNEXT :
                break;

            default :
                break;
            }
        }
        break;

    default:
        break;
    }
    return 0;
}