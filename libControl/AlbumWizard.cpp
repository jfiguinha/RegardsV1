// AlbumWizard.cpp: implementation of the CAlbumWizard class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AlbumWizard.h"
#include "catalogue1.h"
#include "prsht.h"
#include "PhotoDigital.h"
#include <SADirRead.h>
#include "objet.h"
#include <dialogcataloguedata.h>
#include <albumcriterenotify.h>
#include <attributdata.h>
#include <insertalbum.h>
#include ".\initdlgprocalbumwizard.h"
#include ".\pagecriterealbumdlg.h"
#include ".\pagemodifyalbumdlg.h"

#ifdef SQLSERVERCE
#include <SqlAlbum.h>
#include <SqlAlbumUtility.h>
#include <SqlAlbumAttribut.h>
#include <SqlAlbumIconeFileAttribut.h>
#include <SqlIconeFileAlbum.h>
using namespace Regards::Sqlite;
#endif

CAlbumData * CAlbumWizard::RecupPtAlbum()
{
	return m_CCatalogue->RecupPtAlbum(m_iSelectAlbum);
}

AttributVector * CAlbumWizard::RecupPtAttribut()
{
	return m_CCatalogue->RecupPtAttribut();
}

int CAlbumWizard::VerifNomAlbum(const char * cValue)
{
	return m_CCatalogue->VerifNomAlbum(cValue);
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAlbumWizard::CAlbumWizard()
{
	m_cInitDlgProc = new CInitDlgProcAlbumWizard();
	m_cInitDlgProc->SetParam(this);

	m_cPageCritere = new CPageCritereAlbumDlg(hInstance);
	m_cPageCritere->SetParam(this);

	m_cPageModify = new CPageModifyAlbumDlg();
	m_cPageModify->SetParam(this);

	hTitleFont = NULL; //The title font for the Welcome and Completion pages
	IsBoxChecked = false; //The state of the first interior page's check box
	IsButtonClicked = false; //The state of the first interior page's group box
	iValueChecked = 0; //Valeur du bouton radio appuyez
	//other shared data added here
	m_CCatalogue = NULL;
	m_iSelectAlbum = 0;
	m_hWndTree = NULL;
	m_classTree = NULL;
	m_hWndParent = NULL;
}

CAlbumWizard::~CAlbumWizard()
{
	if(m_cInitDlgProc != NULL)
		delete m_cInitDlgProc;

	m_cInitDlgProc = NULL;

	if(m_cPageCritere != NULL)
		delete m_cPageCritere;

	m_cPageCritere = NULL;

	if(m_cPageModify != NULL)
		delete m_cPageModify;

	m_cPageModify = NULL;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////
//1ére page du Wizard
////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK CAlbumWizard::InitDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CAlbumWizard * pdata = (CAlbumWizard *) GetWindowLong(hwndDlg, GWL_USERDATA);

    switch (uMsg)
    {
    case WM_INITDIALOG :
        { 
            pdata = (CAlbumWizard *) ((LPPROPSHEETPAGE) lParam) -> lParam;
            SetWindowLong(hwndDlg, GWL_USERDATA, (DWORD_PTR) pdata);
			pdata->m_cInitDlgProc->OnInitDialog(hwndDlg,uMsg,wParam,lParam);
            break;
        }
		
    case WM_COMMAND :
        pdata->m_cInitDlgProc->OnCommand(hwndDlg,uMsg,wParam,lParam);
        break;

    case WM_NOTIFY :
        {
			pdata->m_cInitDlgProc->OnNotify(hwndDlg,uMsg,wParam,lParam);
        }
        break;


    default:
        break;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//2éme page du Wizard
//Menu de choix
////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK CAlbumWizard::PageMenuDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CAlbumWizard * pdata = (CAlbumWizard *) GetWindowLong(hwndDlg, GWL_USERDATA);

    switch (uMsg)
    {
    case WM_INITDIALOG :
        { 
			//Initialisation des variables d'affichage

            pdata = (CAlbumWizard *) ((LPPROPSHEETPAGE) lParam) -> lParam;
            SetWindowLong(hwndDlg, GWL_USERDATA, (DWORD_PTR) pdata);
			CheckDlgButton(hwndDlg,IDC_RDALBUM,BST_CHECKED);
			pdata->iValueChecked = 1;
			pdata->IsButtonClicked = TRUE;
            break;
        }
		
    case WM_COMMAND :
        switch(LOWORD (wParam))
        {
			case IDC_RDALBUM :
				pdata->iValueChecked = 1;
				pdata->IsButtonClicked = TRUE;
				break;

			case IDC_RDATTRIBUTS :
				pdata->iValueChecked = 2;
				pdata->IsButtonClicked = TRUE;
				break;

			case IDC_RDRECHERCHE :
				pdata->iValueChecked = 3;
				pdata->IsButtonClicked = TRUE;
				break;
			case IDC_RDPHOTO :
				pdata->iValueChecked = 4;
				pdata->IsButtonClicked = TRUE;
				break;				

			default :
				break;
        }

        if(pdata->IsBoxChecked || pdata->IsButtonClicked)
        {
            PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_BACK | PSWIZB_NEXT  | PSWIZB_FINISH);
        }
        else
        {
            PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_BACK | PSWIZB_FINISH);
        }
        break;

    case WM_NOTIFY :
        {
        LPNMHDR lpnm = (LPNMHDR) lParam;

        switch (lpnm->code)
            {
            case PSN_SETACTIVE : //Enable the appropriate buttons.
	
                if(pdata->IsBoxChecked || pdata->IsButtonClicked) 
                {
                    PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_BACK | PSWIZB_NEXT  | PSWIZB_FINISH);
                }
                else //Otherwise, only enable Back.
                {
                    PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_BACK | PSWIZB_FINISH);
                }
                break;

            case PSN_WIZNEXT :
                if(pdata->iValueChecked == 2 || pdata->iValueChecked == 3)
                {
					switch(pdata->iValueChecked)
					{
						case 2:
							SendMessage(pdata->m_hWndParent,WM_COMMAND,MAKEWPARAM(ID_CATALOGUE_GESTIONDESCATGORIES,0),0);
							break;
						case 3:
							SendMessage(pdata->m_hWndParent,WM_COMMAND,MAKEWPARAM(ID_OUTILS_AFFICHAGERECHERCHE,0),0);
							break;
					}

					EndDialog(GetParent(hwndDlg), LOWORD(0));
                    return true;
                }

				if(pdata->iValueChecked == 4)
				{
					SetWindowLong(hwndDlg, DWL_MSGRESULT, IDD_SELECTPHOTO);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////


BOOL CALLBACK CAlbumWizard::PageAlbumMenuDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CAlbumWizard * pdata = (CAlbumWizard *) GetWindowLong(hwndDlg, GWL_USERDATA);

    switch (uMsg)
    {
    case WM_INITDIALOG :
        { 
            pdata = (CAlbumWizard *) ((LPPROPSHEETPAGE) lParam) -> lParam;
            SetWindowLong(hwndDlg, GWL_USERDATA, (DWORD_PTR) pdata);
			CheckDlgButton(hwndDlg,IDC_RDCREER,BST_CHECKED);
			pdata->IsButtonClicked = TRUE;
            break;
        }
		
    case WM_COMMAND :
        switch(LOWORD (wParam))
        {

        case IDC_RDCREER :
			pdata->IsButtonClicked = TRUE;
			pdata->iValueChecked = 1;
			break;

        case IDC_RDMODIFIER :
			pdata->IsButtonClicked = TRUE;
			pdata->iValueChecked = 2;
			break;

        case IDC_RDSUPPRIMER :
            pdata->IsButtonClicked = TRUE;
			pdata->iValueChecked = 3;
            break;
			
			default :
				break;
        }

		if(pdata->IsBoxChecked || pdata->IsButtonClicked)
		{
			PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_BACK | PSWIZB_NEXT);
		}
		else
		{
		   PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_BACK);
		}
        break;

    case WM_NOTIFY :
        {
        LPNMHDR lpnm = (LPNMHDR) lParam;

        switch (lpnm->code)
            {
            case PSN_SETACTIVE : //Enable the appropriate buttons.
	
                if(pdata->IsBoxChecked || pdata->IsButtonClicked) 
                {
                    PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_BACK | PSWIZB_NEXT);
                }
                else //Otherwise, only enable Back.
                {
                    PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_BACK);
				}
                break;

			case PSN_WIZBACK:
				{
                    SetWindowLong(hwndDlg, DWL_MSGRESULT, IDD_MENUALBUM);
                    return TRUE;
					break;
				}

            case PSN_WIZNEXT :
                if(pdata->iValueChecked == 1)
                {
					pdata->m_iSelectAlbum = -1;
                    SetWindowLong(hwndDlg, DWL_MSGRESULT, IDD_GSTALBUM);
                    return TRUE;
                }
				else
					pdata->m_iSelectAlbum = 0;
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

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK CAlbumWizard::PageSelectAlbumDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	AlbumDataVector::iterator AlbumIt;
	static AlbumDataVector * m_VectorAlbum;

    CAlbumWizard * pdata = (CAlbumWizard *) GetWindowLong(hwndDlg, GWL_USERDATA);

    switch (uMsg)
    {
    case WM_INITDIALOG :
        { 
            pdata = (CAlbumWizard *) ((LPPROPSHEETPAGE) lParam) -> lParam;
            SetWindowLong(hwndDlg, GWL_USERDATA, (DWORD_PTR) pdata);
				
			int iNumber = 0;

			m_VectorAlbum = pdata->m_CCatalogue->RecupPtVectorAlbum();

			//Initialisation de la combo Box
			for (AlbumIt = m_VectorAlbum->begin(); AlbumIt!= m_VectorAlbum->end(); AlbumIt++)
			{
				CAlbumData m_AlbumTemp = *AlbumIt;
				SendDlgItemMessage(hwndDlg, IDC_LISTALBUM, LB_INSERTSTRING , iNumber, (LPARAM)(LPCTSTR)m_AlbumTemp.m_szAlbumName);
				SendDlgItemMessage(hwndDlg, IDC_LISTALBUM, WM_SETTEXT , (WPARAM)0, (LPARAM)m_AlbumTemp.m_szAlbumName);
				iNumber++;

			}
			SendDlgItemMessage(hwndDlg, IDC_LISTALBUM, LB_SETCURSEL, 0, 0);
            break;
        }
		
    case WM_COMMAND :
        PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_BACK | PSWIZB_NEXT);
        break;

    case WM_NOTIFY :
        {
        LPNMHDR lpnm = (LPNMHDR) lParam;

        switch (lpnm->code)
            {
            case PSN_SETACTIVE : //Enable the appropriate buttons.
				{
					int iSelCount = SendDlgItemMessage(hwndDlg, IDC_LISTALBUM, LB_GETCURSEL , 0, 0);
					if(iSelCount != -1) 
					{
						PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_BACK | PSWIZB_NEXT);
					}
					else //Otherwise, only enable Back.
					{
						PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_BACK);
					}
				}
                break;

            case PSN_WIZNEXT :
				{
					int iSelCount = SendDlgItemMessage(hwndDlg, IDC_LISTALBUM, LB_GETCURSEL , 0, 0);
					pdata->m_iSelectAlbum = iSelCount;
					if(pdata->iValueChecked == 3)
					{
						char clpText[255];
						char clpCaption[255];

						LoadString(hInstance,IDS_SUPPALBUM,clpText,255);
						LoadString(hInstance,IDS_INFOS,clpCaption,255);

						if(MessageBox(NULL,clpText,clpCaption,MB_YESNO | MB_ICONWARNING) == IDYES)
						{
							pdata->m_CCatalogue->SuppAlbum(*pdata->m_hWndTree, pdata->m_classTree, iSelCount);
							SetWindowLong(hwndDlg, DWL_MSGRESULT, IDD_FIN);
							return TRUE;
						}
						else
						{
							SetWindowLong(hwndDlg, DWL_MSGRESULT, IDD_SELECTALBUMWIZARD);
							return TRUE;
						}
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

int CAlbumWizard::FinishPageModifyAlbum(HWND hwndDlg,CAlbumData * m_Album,IconeFileVector &m_IconeFileVector)
{
	//Ajout du libelle
	char cTemp[255];
	SendDlgItemMessage(hwndDlg, IDC_LBLALBUM, WM_GETTEXT, 255, (LPARAM)cTemp);

	if(strcmp(cTemp,"") == 0)
	{
		CObjet::AffichageMessage(IDS_ALBUMNAME,IDS_INFOS);
		return FALSE;
	}
	else
	{
		StringCchCopy(m_Album->m_szAlbumName,255,cTemp);

#ifdef SQLSERVERCE
		IconeFileVector m_IconeFileVectorAlbum;
		CSqlIconeFileAlbum * m_cSqlIconeFileAlbum = new CSqlIconeFileAlbum();
		m_cSqlIconeFileAlbum->LoadIconeFileAlbum(&m_IconeFileVectorAlbum, m_Album->m_szAlbumName);
		delete m_cSqlIconeFileAlbum;
#endif

		CInsertAlbum * m_cInsertCatalogue = new CInsertAlbum(hInstance);
#ifdef SQLSERVERCE
		m_cInsertCatalogue->SetAlbum(m_Album,&m_IconeFileVectorAlbum);
#else
		m_cInsertCatalogue->SetAlbum(m_Album);
#endif
		m_cInsertCatalogue->SetCas(1);
		m_cInsertCatalogue->SetIconeFileVector(&m_IconeFileVector);
		m_cInsertCatalogue->CreateDlg(hInstance, (LPCTSTR)IDD_FICHIER, hwndDlg);
		delete m_cInsertCatalogue;

		if(m_iSelectAlbum == -1)
		{
			m_iSelectAlbum = m_CCatalogue->AddAlbum(*m_hWndTree, m_classTree,*m_Album);

#ifdef SQLSERVERCE
		
			//Création d'un album et Sauvegarde dans la base de données
			CSqlAlbum * m_cSqlAlbum = new CSqlAlbum();
			m_cSqlAlbum->SaveAlbum(m_Album);
			delete m_cSqlAlbum;

			CSqlIconeFileAlbum * m_cSqlIconeFileAlbum = new CSqlIconeFileAlbum();
			m_cSqlIconeFileAlbum->SaveIconeFileAlbum(&m_IconeFileVector, m_Album->m_szAlbumName);
			delete m_cSqlIconeFileAlbum;

#endif
		}
		else
		{
			//Mise à jour de l'album
#ifdef SQLSERVERCE
			//Sauvegarde dans la base de données - Faire une comparaison des éléments
			CSqlAlbumUtility::UpdateFileAlbum(&m_IconeFileVector, m_Album->m_szAlbumName);

#endif
		}

	}
    return TRUE;
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK CAlbumWizard::PageModifyAlbumDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CAlbumWizard * pdata = (CAlbumWizard *) GetWindowLong(hwndDlg, GWL_USERDATA);				

    switch (uMsg)
    {
		case WM_KILLFOCUS:
			SetFocus(GetParent(hwndDlg));
			break;

    case WM_INITDIALOG :
        { 
            pdata = (CAlbumWizard *) ((LPPROPSHEETPAGE) lParam) -> lParam;
            SetWindowLong(hwndDlg, GWL_USERDATA, (DWORD_PTR) pdata);
			pdata->m_cPageModify->OnInitDialog(hwndDlg,uMsg,wParam,lParam);
			break;
        }
	
    case WM_COMMAND :
		{
			return pdata->m_cPageModify->OnCommand(hwndDlg,uMsg,wParam,lParam);
		}
        break;

    case WM_NOTIFY :
        {
			return pdata->m_cPageModify->OnNotify(hwndDlg,uMsg,wParam,lParam);
        }
        break;

    default:
        break;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//Affichage des critère Album
////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK CAlbumWizard::PageCritereAlbumDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CAlbumWizard * pdata = (CAlbumWizard *) GetWindowLong(hwndDlg, GWL_USERDATA);

    switch (uMsg)
    {
    case WM_INITDIALOG :
        { 
            pdata = (CAlbumWizard *) ((LPPROPSHEETPAGE) lParam) -> lParam;
            SetWindowLong(hwndDlg, GWL_USERDATA, (DWORD_PTR) pdata);
			pdata->m_cPageCritere->OnInitDialog(hwndDlg,uMsg,wParam,lParam);
            break;
        }
		
	case WM_DESTROY:
		{
			//delete m_cAlbumCritereNotify;
			return pdata->m_cPageCritere->OnDestroy(hwndDlg,uMsg,wParam,lParam);
		}
		break;

    case WM_COMMAND :
		{
			return pdata->m_cPageCritere->OnCommand(hwndDlg,uMsg,wParam,lParam);
		}
        break;

    case WM_NOTIFY :
        {
			return pdata->m_cPageCritere->OnNotify(hwndDlg,uMsg,wParam,lParam);
        }
        break;

    default:
        break;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK CAlbumWizard::EndDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CAlbumWizard * pdata = (CAlbumWizard *) GetWindowLong(hwndDlg, GWL_USERDATA);

    switch (uMsg)
    {
    case WM_INITDIALOG :
        { 
            pdata = (CAlbumWizard *) ((LPPROPSHEETPAGE) lParam) -> lParam;
            SetWindowLong(hwndDlg, GWL_USERDATA, (DWORD_PTR) pdata);
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
				PropSheet_SetWizButtons(GetParent(hwndDlg), NULL);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK CAlbumWizard::PagePhotoDigitalDlgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CAlbumWizard * pdata = (CAlbumWizard *) GetWindowLong(hwndDlg, GWL_USERDATA);

	static int iCase = 0;
	static int iNumberFormat = 0;
	static int iNumberIncre = 0;
	static CPhotoDigital::PHOTODIGITALDATA m_PHOTODIGITALDATA;

    switch (uMsg)
    {
    case WM_INITDIALOG :
        { 
			iNumberFormat = 0;
			iNumberIncre = 0;

            pdata = (CAlbumWizard *) ((LPPROPSHEETPAGE) lParam) -> lParam;
            SetWindowLong(hwndDlg, GWL_USERDATA, (DWORD_PTR) pdata);
			//Insertion des données dans les listes déroulantes

			//Initialisation des variables

			SendDlgItemMessage(hwndDlg, IDC_CBFORMAT, CB_INSERTSTRING , iNumberFormat, (LPARAM)(LPCTSTR)"BMP"); //2
			SendDlgItemMessage(hwndDlg, IDC_CBFORMAT, WM_SETTEXT , (WPARAM)0, (LPARAM)(LPCTSTR)"BMP");
			iNumberFormat++;
			SendDlgItemMessage(hwndDlg, IDC_CBFORMAT, CB_INSERTSTRING , iNumberFormat, (LPARAM)(LPCTSTR)"JPEG"); //1
			SendDlgItemMessage(hwndDlg, IDC_CBFORMAT, WM_SETTEXT , (WPARAM)0, (LPARAM)(LPCTSTR)"JPEG");
			iNumberFormat++;
			SendDlgItemMessage(hwndDlg, IDC_CBFORMAT, CB_INSERTSTRING , iNumberFormat, (LPARAM)(LPCTSTR)"TGA"); //3
			SendDlgItemMessage(hwndDlg, IDC_CBFORMAT, WM_SETTEXT , (WPARAM)0, (LPARAM)(LPCTSTR)"TGA");
			iNumberFormat++;
			SendDlgItemMessage(hwndDlg, IDC_CBFORMAT, CB_INSERTSTRING , iNumberFormat, (LPARAM)(LPCTSTR)"TIF"); //4
			SendDlgItemMessage(hwndDlg, IDC_CBFORMAT, WM_SETTEXT , (WPARAM)0, (LPARAM)(LPCTSTR)"TIF");
			iNumberFormat++;
			SendDlgItemMessage(hwndDlg, IDC_CBFORMAT, CB_INSERTSTRING , iNumberFormat, (LPARAM)(LPCTSTR)"PDF"); //5
			SendDlgItemMessage(hwndDlg, IDC_CBFORMAT, WM_SETTEXT , (WPARAM)0, (LPARAM)(LPCTSTR)"PDF");
			iNumberFormat++;

			SendDlgItemMessage(hwndDlg, IDC_CBFORMAT, CB_SETCURSEL, 1, 0);

			SendDlgItemMessage(hwndDlg, IDC_CBINCREMENT, CB_INSERTSTRING , iNumberIncre, (LPARAM)(LPCTSTR)"1000");
			SendDlgItemMessage(hwndDlg, IDC_CBINCREMENT, WM_SETTEXT , (WPARAM)0, (LPARAM)(LPCTSTR)"1000");
			iNumberIncre++;
			SendDlgItemMessage(hwndDlg, IDC_CBINCREMENT, CB_INSERTSTRING , iNumberIncre, (LPARAM)(LPCTSTR)"100");
			SendDlgItemMessage(hwndDlg, IDC_CBINCREMENT, WM_SETTEXT , (WPARAM)0, (LPARAM)(LPCTSTR)"100");
			iNumberIncre++;
			SendDlgItemMessage(hwndDlg, IDC_CBINCREMENT, CB_INSERTSTRING , iNumberIncre, (LPARAM)(LPCTSTR)"10");
			SendDlgItemMessage(hwndDlg, IDC_CBINCREMENT, WM_SETTEXT , (WPARAM)0, (LPARAM)(LPCTSTR)"10");
			iNumberIncre++;
			SendDlgItemMessage(hwndDlg, IDC_CBINCREMENT, CB_INSERTSTRING , iNumberIncre, (LPARAM)(LPCTSTR)"1");
			SendDlgItemMessage(hwndDlg, IDC_CBINCREMENT, WM_SETTEXT , (WPARAM)0, (LPARAM)(LPCTSTR)"1");
			iNumberIncre++;



			//Initialisation des données
			ZeroMemory(&m_PHOTODIGITALDATA,sizeof(CPhotoDigital::PHOTODIGITALDATA));

			m_PHOTODIGITALDATA.iTypeIncrementation = 1;
			m_PHOTODIGITALDATA.iFormatImage = 1;

            break;
        }
		
    case WM_COMMAND :
		{
			int wmId    = LOWORD(wParam); 
			int wmEvent = HIWORD(wParam); 

			switch(wmId)
			{
				case IDC_CMDSTOCKAGE:
					{
						CObjet m_cObjet;
						m_cObjet.GetDirectory(hwndDlg,m_PHOTODIGITALDATA.m_szDirectory);
						SetDlgItemText(hwndDlg, IDC_REPSTOCK, m_PHOTODIGITALDATA.m_szDirectory);
						if(strcmp(m_PHOTODIGITALDATA.m_szFilename,"") != 0 && strcmp(m_PHOTODIGITALDATA.m_szDirectory,"") != 0)
							iCase = 1;
						else
							iCase = 0;
					}
					break;

				case IDC_LBLIMAGE:
					{
						GetDlgItemText(hwndDlg, IDC_LBLIMAGE,m_PHOTODIGITALDATA.m_szFilename,255);
						if(strcmp(m_PHOTODIGITALDATA.m_szFilename,"") != 0 && strcmp(m_PHOTODIGITALDATA.m_szDirectory,"") != 0)
							iCase = 1;
						else
							iCase = 0;
						
					}
					break;

				case IDC_CBINCREMENT:
					{
						char cRep[255];
						GetDlgItemText(hwndDlg, IDC_CBINCREMENT,cRep,255);
						m_PHOTODIGITALDATA.iTypeIncrementation = atoi(cRep);

					}
					break;

				case IDC_CBFORMAT:
					{
						char cRep[255];
						GetDlgItemText(hwndDlg, IDC_CBFORMAT,cRep,255);
						if(_stricmp(cRep,"JPEG") == 0)
						{
							m_PHOTODIGITALDATA.iFormatImage = 1;
							break;
						}
						if(_stricmp(cRep,"BMP") == 0)
						{
							m_PHOTODIGITALDATA.iFormatImage = 2;
							break;
						}
						if(_stricmp(cRep,"TGA") == 0)
						{
							m_PHOTODIGITALDATA.iFormatImage = 3;
							break;
						}
						if(_stricmp(cRep,"TIF") == 0)
						{
							m_PHOTODIGITALDATA.iFormatImage = 4;
							break;
						}
						if(_stricmp(cRep,"PDF") == 0)
						{
							m_PHOTODIGITALDATA.iFormatImage = 5;
							break;
						}
					}
					break;

				default:
					break;
			}

			if(iCase)
				PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_BACK | PSWIZB_NEXT);
			else
				PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_BACK);
		}
        break;

    case WM_NOTIFY :
        {
        LPNMHDR lpnm = (LPNMHDR) lParam;

        switch (lpnm->code)
            {
            case PSN_SETACTIVE : //Enable the appropriate buttons.
				{
					PropSheet_SetWizButtons(GetParent(hwndDlg), NULL);
				}
                break;

            case PSN_WIZNEXT :
				{
					//Récupération de toutes les données
					CPhotoDigital m_cPhotoDigital;
					m_cPhotoDigital.SetParametre(m_PHOTODIGITALDATA);
					DWORD dStyle = 0;
					DWORD dExStyle = 0;
					DWORD dWndStyle = WS_OVERLAPPEDWINDOW | WS_MAXIMIZE;

					RECT rc = {0,0,0,0};

					m_cPhotoDigital.CreateEx(hwndDlg,hInstance,dWndStyle,dStyle,dExStyle,"PHOTO",0,0,IDC_ARROW,rc);

					m_cPhotoDigital.Run();

					//m_cPhotoDigital->CreatePhotoWindow(hwndDlg,SW_HIDE);
					//delete m_cPhotoDigital;


					SetWindowLong(hwndDlg, DWL_MSGRESULT, 0);
					return TRUE;					
				}
                break;

			case PSN_WIZBACK :
				{
					SetWindowLong(hwndDlg, DWL_MSGRESULT, IDD_MENUWIZARD);
					return TRUE;
				}
				break;

            default :
                break;
            }
        }
        break;

	case WM_DESTROY:
		iCase = 0;
		break;

    default:
        break;
    }
    return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//Fonction de création de la fenêtre du Wizard
////////////////////////////////////////////////////////////////////////////////////////////////////////

void CAlbumWizard::CreateAlbumWizard(HWND hParent, HINSTANCE hInstance,int nCmdShow,CCatalogue * cCatalogue, HWND * hWndTree,CTree * classTree)
{
    PROPSHEETPAGE   psp =       {0}; //defines the property sheet pages
    HPROPSHEETPAGE  ahpsp[8] =  {0}; //an array to hold the page's HPROPSHEETPAGE handles
    PROPSHEETHEADER psh =       {0}; //defines the property sheet

	m_CCatalogue = NULL;
	m_hWndTree = hWndTree;
	m_classTree = classTree;
	m_iSelectAlbum = -1;
	m_hWndParent = hParent;
    //
    //Create the Wizard pages
    //

    //Opening page - Introduction

    psp.dwSize =        sizeof(psp);
    psp.dwFlags =       PSP_DEFAULT | PSP_HIDEHEADER ;
    psp.hInstance =     hInstance;
    psp.lParam =        (LPARAM)this; //The shared data structure
    psp.pfnDlgProc =    InitDlgProc;
    psp.pszTemplate =   MAKEINTRESOURCE(IDD_CATALOGUEMENU);
    ahpsp[0] =          CreatePropertySheetPage(&psp);

	//////////////////////////////////////////////////////////////////
    //First interior page - Menu de selection
	//////////////////////////////////////////////////////////////////

    psp.dwFlags =           PSP_DEFAULT|PSP_USEHEADERTITLE;//|PSP_USEHEADERSUBTITLE;
    psp.pszHeaderTitle =    MAKEINTRESOURCE(IDS_MENUGESTIONALBUM);
    //psp.pszHeaderSubTitle = MAKEINTRESOURCE(IDS_REPERTOIRESUP);
    psp.pszTemplate =       MAKEINTRESOURCE(IDD_MENUWIZARD);
    psp.pfnDlgProc =        PageMenuDlgProc;

    ahpsp[1] =              CreatePropertySheetPage(&psp);

	//////////////////////////////////////////////////////////////////
    //Second interior page - Menu de selection pour un Album
	//////////////////////////////////////////////////////////////////

    psp.dwFlags =           PSP_DEFAULT|PSP_USEHEADERTITLE;
    psp.pszHeaderTitle =    MAKEINTRESOURCE(IDS_MENUGESTIONALBUM);
    psp.pszTemplate =       MAKEINTRESOURCE(IDD_MENUALBUM);
    psp.pfnDlgProc =        PageAlbumMenuDlgProc;

    ahpsp[2] =              CreatePropertySheetPage(&psp);

	//////////////////////////////////////////////////////////////////
    //3 interior page - Selection d'un album
	//////////////////////////////////////////////////////////////////

    psp.dwFlags =           PSP_DEFAULT|PSP_USEHEADERTITLE;
    psp.pszHeaderTitle =    MAKEINTRESOURCE(IDS_MENUGESTIONALBUM);
    psp.pszTemplate =       MAKEINTRESOURCE(IDD_SELECTALBUMWIZARD);
    psp.pfnDlgProc =        PageSelectAlbumDlgProc;
    ahpsp[3] =              CreatePropertySheetPage(&psp);

	//////////////////////////////////////////////////////////////////
    //4 interior page - Modification des données d'un album
	//////////////////////////////////////////////////////////////////

    psp.dwFlags =           PSP_DEFAULT|PSP_USEHEADERTITLE;
    psp.pszHeaderTitle =    MAKEINTRESOURCE(IDS_MENUGESTIONALBUM);
    psp.pszTemplate =       MAKEINTRESOURCE(IDD_GSTALBUM);
    psp.pfnDlgProc =        PageModifyAlbumDlgProc;
    ahpsp[4] =              CreatePropertySheetPage(&psp);

	//////////////////////////////////////////////////////////////////
    //5 interior page - Ajout des critères à un album
	//////////////////////////////////////////////////////////////////

    psp.dwFlags =           PSP_DEFAULT|PSP_USEHEADERTITLE;
    psp.pszHeaderTitle =    MAKEINTRESOURCE(IDS_MENUGESTIONALBUM);
    psp.pszTemplate =       MAKEINTRESOURCE(IDD_AJOUTCRITEREALBUM);
    psp.pfnDlgProc =        PageCritereAlbumDlgProc;
    ahpsp[5] =              CreatePropertySheetPage(&psp);

	//////////////////////////////////////////////////////////////////
    //6 interior page - Gestion des appareils photos numériques
	//////////////////////////////////////////////////////////////////

    psp.dwFlags =           PSP_DEFAULT|PSP_USEHEADERTITLE;
    psp.pszHeaderTitle =    MAKEINTRESOURCE(IDS_MENUPHOTONUMERIQUE);
    psp.pszTemplate =       MAKEINTRESOURCE(IDD_SELECTPHOTO);
    psp.pfnDlgProc =        PagePhotoDigitalDlgProc;
    ahpsp[6] =              CreatePropertySheetPage(&psp);


	//////////////////////////////////////////////////////////////////
    //Final page - Fin du Wizard
	//////////////////////////////////////////////////////////////////

    psp.dwFlags =       PSP_DEFAULT|PSP_HIDEHEADER;
    psp.pszTemplate =   MAKEINTRESOURCE(IDD_FIN);
    psp.pfnDlgProc =    EndDlgProc;
    ahpsp[7] =          CreatePropertySheetPage(&psp);

	//////////////////////////////////////////////////////////////////
    //Create the property sheet
	//////////////////////////////////////////////////////////////////


    psh.dwSize =            sizeof(psh);
    psh.hInstance =         hInstance;
    psh.hwndParent =        hParent;
    psh.phpage =            ahpsp;
    psh.dwFlags =           PSH_WIZARD97 | PSH_WATERMARK | PSH_HEADER | PSH_WIZARDHASFINISH;
    psh.pszbmWatermark =    MAKEINTRESOURCE(IDB_WATERMARK256);
    psh.pszbmHeader =       MAKEINTRESOURCE(IDB_BANNER256);
    psh.nStartPage =        0;
    psh.nPages =            8;


    //Set up the font for the titles on the intro and ending pages

    NONCLIENTMETRICS ncm = {0};
    ncm.cbSize = sizeof(ncm);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

    //Create the intro/end title font
    LOGFONT TitleLogFont = ncm.lfMessageFont;
    TitleLogFont.lfWeight = FW_BOLD;
	StringCchCopy(TitleLogFont.lfFaceName,32,TEXT("Verdana Bold"));

    HDC hdc = GetDC(NULL); //gets the screen DC
    INT FontSize = 12;
    TitleLogFont.lfHeight = 0 - GetDeviceCaps(hdc, LOGPIXELSY) * FontSize / 72;
    hTitleFont = CreateFontIndirect(&TitleLogFont);
    ReleaseDC(NULL, hdc);

	m_CCatalogue = cCatalogue;
	classTree = classTree;
	hWndTree = hWndTree;

    //Display the wizard

    PropertySheet(&psh);
    
    // Destroy the fonts
    DeleteObject(hTitleFont);
}


