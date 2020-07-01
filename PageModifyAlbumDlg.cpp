#include "stdafx.h"
#include ".\pagemodifyalbumdlg.h"
#include "albumwizard.h"
#include <albumdata.h>
#include "objet.h"
#include <SADirRead.h>
#include <insertalbum.h>

#ifdef SQLSERVERCE
#include <SqlIconeFileAlbum.h>
using namespace Regards::Sqlite;
#endif

CPageModifyAlbumDlg::CPageModifyAlbumDlg(void)
{
	iNumber = 0;
	iNumberImageAlbum = 0;
	iCase = 0;
	m_AlbumLocal = new CAlbumData();

	m_cAlbumWizard = NULL;
	sPath[0] = '\0';
	sTemp[0] = '\0';
	m_Album = NULL;

}

CPageModifyAlbumDlg::~CPageModifyAlbumDlg(void)
{
	if(m_AlbumLocal != NULL)
		delete m_AlbumLocal;

	m_AlbumLocal = NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CPageModifyAlbumDlg::OnInitDialog(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SendDlgItemMessage(hwndDlg, IDC_LBALBUMIMAGE, LB_RESETCONTENT  , 0, 0);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CPageModifyAlbumDlg::OnCommand(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD (wParam))
    {
		case IDC_LBLALBUM:
			{
				char cTemp[MAX_PATH];
				int iLocalAlbum;
				SendDlgItemMessage(hwndDlg, IDC_LBLALBUM, WM_GETTEXT, 255, (LPARAM)cTemp);

				if(_stricmp(sTemp,cTemp) != 0)
				{
					if(_stricmp(cTemp,"") == 0) 
					{
						iCase = 0;
						break;
					}

					iLocalAlbum = m_cAlbumWizard->VerifNomAlbum(cTemp);
					StringCchCopy(sTemp,MAX_PATH,cTemp);
					if(iLocalAlbum == -2 || (iLocalAlbum == m_cAlbumWizard->GetSelectedAlbum())) 
					{
						iCase = 1;
					}
					else //Otherwise, only enable Back.
					{
						iCase = 0;
						CObjet::AffichageMessage(IDS_ALBUMEXISTANT,IDS_INFOS);					
					}
					
				}
			}
			break;


		case IDC_LBLREP:
			{
				CObjet m_cObjet;

				
				//Récupération du nom du répertoire
				SendDlgItemMessage(hwndDlg, IDC_LBLREP, WM_GETTEXT, (WPARAM) 255, (LPARAM) sPath); 

				if(_stricmp(sTemp,sPath) == 0)
					break;

				//Test si répertoire 
				if(m_cObjet.TestRepertoire(sPath))
				{
					SAFileVector::const_iterator fit;
					CSADirRead dr;

					iNumber = 0;

					SendDlgItemMessage(hwndDlg, IDC_LSIMAGEDIR, LB_RESETCONTENT  , 0, 0);

					//Insertion des données dans la liste 

					dr.ClearDirs();         // start clean
					dr.m_bRecurse = false;
					dr.GetDirs(sPath); // get all folders under c:\temp
					dr.GetImageFile(0,true,false,false,true);

					// get the file array
					files = dr.Files();
					
					for (fit = files.begin(); fit!=files.end(); fit++)
					{
						string m_stgFileName = (*fit).m_sName;
						m_stgFileName = m_stgFileName.substr(strlen(sPath) + 1,m_stgFileName.length() - strlen(sPath) - 1);

						SendDlgItemMessage(hwndDlg, IDC_LSIMAGEDIR, LB_INSERTSTRING , iNumber, (LPARAM)(LPCTSTR)m_stgFileName.c_str());
						SendDlgItemMessage(hwndDlg, IDC_LSIMAGEDIR, WM_SETTEXT , (WPARAM)0, (LPARAM)m_stgFileName.c_str());
						iNumber++;
					}

				}
				else
				{
					StringCchCopy(sTemp,MAX_PATH,sPath);
				}

			}
			break;

		case IDC_BTREP:
			{
				CObjet m_cObjet;
				if(m_cObjet.GetDirectory(hwndDlg, sPath))
				{
					SendDlgItemMessage(hwndDlg, IDC_LBLREP, WM_SETTEXT, (WPARAM) 0, (LPARAM) sPath); 
				}
			}
			break;

		case IDC_AJOUTIMAGE:
			{
				//Récupération de la liste des fichiers
				int iSelCount = SendDlgItemMessage(hwndDlg, IDC_LSIMAGEDIR, LB_GETSELCOUNT , 0, 0);
				//int iNumSelected;
				if(iSelCount > 0)
				{
					CObjet::StartWait();

					CInsertAlbum * m_cInsertCatalogue = new CInsertAlbum(hInstance);
#ifdef SQLSERVERCE
					m_cInsertCatalogue->SetAlbum(m_Album, NULL);
#else
					m_cInsertCatalogue->SetAlbum(m_Album);
#endif
					m_cInsertCatalogue->SetCas(3);
					m_cInsertCatalogue->SetWindow(hwndDlg);
					m_cInsertCatalogue->SetSAFileVector(&files);
					m_cInsertCatalogue->SetIconeFileVector(&m_IconeFileVector);
					m_cInsertCatalogue->CreateDlg(hInstance, (LPCTSTR)IDD_FICHIER, hwndDlg);
					delete m_cInsertCatalogue;

					//Appel de la boite de dialogue
					//DialogBoxParam(hInstance, (LPCTSTR)IDD_FICHIER, hwndDlg, (DLGPROC)InsertCatalog,(LPARAM)&m_AjoutImageData);

					CObjet::EndWait();
				}
			}
			break;


		case IDC_AJOUTTOUT:
			{
				//Récupération de la liste des fichiers
				int iSelCount = SendDlgItemMessage(hwndDlg, IDC_LSIMAGEDIR, LB_GETCOUNT, 0, 0);
				//int iNumSelected;
				if(iSelCount > 0)
				{
					CObjet::StartWait();

					CInsertAlbum * m_cInsertCatalogue = new CInsertAlbum(hInstance);
					m_cInsertCatalogue->SetIconeFileVector(&m_IconeFileVector);
#ifdef SQLSERVERCE
					m_cInsertCatalogue->SetAlbum(m_Album, NULL);
#else
					m_cInsertCatalogue->SetAlbum(m_Album);
#endif
					m_cInsertCatalogue->SetWindow(hwndDlg);
					m_cInsertCatalogue->SetCas(2);
					m_cInsertCatalogue->SetSAFileVector(&files);
					m_cInsertCatalogue->CreateDlg(hInstance, (LPCTSTR)IDD_FICHIER, hwndDlg);
					delete m_cInsertCatalogue;

					CObjet::EndWait();
				}

			}
			break;

		case IDC_SUPIMAGE:
			{
				//Récupération de la liste des fichiers
				int iSelCount = SendDlgItemMessage(hwndDlg, IDC_LBALBUMIMAGE, LB_GETSELCOUNT , 0, 0);
				
				//int iNumSelected;
				if(iSelCount > 0)
				{
					int * iNumSelected = new int[iSelCount + 1];

					CObjet::StartWait();
					for(int i = 0;i < iSelCount;i++)
					{
						SendDlgItemMessage(hwndDlg, IDC_LBALBUMIMAGE, LB_GETSELITEMS , iSelCount, (LPARAM)iNumSelected);
						SendDlgItemMessage(hwndDlg, IDC_LBALBUMIMAGE, LB_DELETESTRING, iNumSelected[0], 0);
						m_IconeFileVector.erase(m_IconeFileVector.begin() + iNumSelected[0]);

					}
					delete[] iNumSelected;
					CObjet::EndWait();
				}
				
			}
			break;

		case IDC_SUPTOUT:
			{
				//Récupération de la liste des fichiers
				SendDlgItemMessage(hwndDlg, IDC_LBALBUMIMAGE, LB_RESETCONTENT  , 0, 0);
				m_IconeFileVector.clear();
			}
			break;
		
		default :
			break;
    }

	if(iCase)
		PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_BACK | PSWIZB_NEXT);
	else
		PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_BACK);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CPageModifyAlbumDlg::OnNotify(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LPNMHDR lpnm = (LPNMHDR) lParam;
	
    switch (lpnm->code)
        {

		case PSN_WIZFINISH :
			{
				if(!m_cAlbumWizard->FinishPageModifyAlbum(hwndDlg,m_Album,m_IconeFileVector))
				{
					SetWindowLong(hwndDlg, DWL_MSGRESULT, IDD_GSTALBUM);
					return TRUE;
				}
			}
			break;

        case PSN_SETACTIVE : //Enable the appropriate buttons.
			{
				
				PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_BACK);
				SendDlgItemMessage(hwndDlg, IDC_LBALBUMIMAGE, LB_RESETCONTENT  , 0, 0);
		
				if(m_cAlbumWizard->GetSelectedAlbum() == -1)
				{
					m_Album = m_AlbumLocal;
					SendDlgItemMessage(hwndDlg, IDC_LBALBUMIMAGE, LB_RESETCONTENT  , 0, 0);
				}

				//Création d'un thread

				iNumber = 0;
				iNumberImageAlbum = 0;
				files.clear();
				m_IconeFileVector.clear();

				CObjet::StartWait();


				if(m_cAlbumWizard->GetSelectedAlbum() != -1)
				{
					m_Album = m_cAlbumWizard->RecupPtAlbum();
#ifdef SQLSERVERCE


					CSqlIconeFileAlbum * m_cSqlIconeFileAlbum = new CSqlIconeFileAlbum();
					m_cSqlIconeFileAlbum->LoadIconeFileAlbum(&m_IconeFileVector, m_Album->m_szAlbumName);
					delete m_cSqlIconeFileAlbum;
#endif

					//Test si Mode Mise à jour ou creation
					for (IconeFileVector::iterator dit = m_IconeFileVector.begin(); dit != m_IconeFileVector.end(); dit++)
					{
						CIconeFile m_IconeFile = *dit;
						//Récupération du nom de fichier
						CObjet m_cObjet;
						char Filename[255];
						m_cObjet.GetFileName(dit->m_stgFullPathName,Filename,255);
						SendDlgItemMessage(hwndDlg, IDC_LBALBUMIMAGE, LB_INSERTSTRING , iNumberImageAlbum, (LPARAM)Filename);
						SendDlgItemMessage(hwndDlg, IDC_LBALBUMIMAGE, WM_SETTEXT , (WPARAM)0, (LPARAM)Filename);
						iNumberImageAlbum++;
					}
				}
				
				CObjet::EndWait();

				SendDlgItemMessage(hwndDlg, IDC_LBLALBUM, WM_SETTEXT , (WPARAM)0, (LPARAM)m_Album->m_szAlbumName);
				StringCchCopy(sTemp,255,m_Album->m_szAlbumName);
				break;
			}
            break;

		case PSN_QUERYINITIALFOCUS :
			SetWindowLong(hwndDlg,DWL_MSGRESULT, (LPARAM)GetDlgItem(hwndDlg, IDC_BTREP));
			return TRUE;

		case PSN_WIZBACK :
			{
				if(m_cAlbumWizard->GetSelectedAlbum() == -1)
				{
					SetWindowLong(hwndDlg, DWL_MSGRESULT, IDD_MENUALBUM);
					return TRUE;
				}
			}
			break;

        case PSN_WIZNEXT :
			{
				if(!m_cAlbumWizard->FinishPageModifyAlbum(hwndDlg,m_Album,m_IconeFileVector))
				{
					SetWindowLong(hwndDlg, DWL_MSGRESULT, IDD_GSTALBUM);
					return TRUE;
				}
			}
            break;

        default :
            break;
        }
	return 0;
}