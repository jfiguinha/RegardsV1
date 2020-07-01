#include "stdafx.h"
#include ".\pagecriterealbumdlg.h"
#include "objet.h"
#include <DialogCatalogueData.h>
#include "albumwizard.h"
#include <attributdata.h>
#include <insertalbum.h>
#include <albumcriterenotify.h>

#ifdef SQLSERVERCE
#include <SqlAlbumUtility.h>
using namespace Regards::Sqlite;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
CPageCritereAlbumDlg::CPageCritereAlbumDlg(HINSTANCE hInstance)
{
	this->hInstance = hInstance;
	m_cAlbumCritereNotify = new CAlbumCritereNotify(hInstance);
	iNumItem = 0;
	iSelectItem = -1;
	m_cAlbumWizard = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
CPageCritereAlbumDlg::~CPageCritereAlbumDlg(void)
{

	if(m_cAlbumCritereNotify != NULL)
		delete m_cAlbumCritereNotify;
	
	m_cAlbumCritereNotify = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CPageCritereAlbumDlg::OnInitDialog(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CDialogCatalogueData m_DialogData;
	m_DialogData.m_Album = m_cAlbumWizard->RecupPtAlbum();
	m_DialogData.m_AttributVector = m_cAlbumWizard->RecupPtAttribut();
	
	CAttributData * m_cAttributData = new CAttributData(hInstance);
	m_cAttributData->SetParam(&m_DialogData);
	//m_cAttributData->SetNumItem(iNumItem);
	m_cAttributData->InitAttributDialog(hwndDlg,0,iNumItem);
	delete m_cAttributData;

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CPageCritereAlbumDlg::OnDestroy(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CPageCritereAlbumDlg::OnCommand(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_NEXT);
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam); 

	switch(wmId)
	{
		case IDC_NEWCRITERE:
			{
				char Temp[20];
				LoadString(hInstance,IDS_NEWCRITERE,Temp,20);
				char cTemp[255];
				LVITEM lvItem;
				int m_iIndex;


				ZeroMemory(&lvItem,sizeof(LVITEM));
				lvItem.mask = LVIF_STATE | LVIF_TEXT;
				lvItem.iItem = iNumItem;
				lvItem.iSubItem = 0;
				lvItem.pszText = Temp;

				m_iIndex = SendDlgItemMessage(hwndDlg, IDC_LISTATTRIBUT, LVM_INSERTITEM , (WPARAM)0, (LPARAM) (const LPLVITEM)&lvItem);

				iNumItem++;

				lvItem.mask = LVIF_TEXT;
				lvItem.iItem = m_iIndex;
				lvItem.iSubItem = 1;
				lvItem.pszText = Temp;


				SendDlgItemMessage(hwndDlg, IDC_LISTATTRIBUT, LVM_SETITEMTEXT , (WPARAM)m_iIndex, (LPARAM) (const LPLVITEM)&lvItem);

				_itoa_s(iNumItem,cTemp,255,10);

				ZeroMemory(&lvItem,sizeof(LVITEM));
				lvItem.mask = TVIF_TEXT;
				lvItem.iItem = m_iIndex;
				lvItem.iSubItem = 2;
				lvItem.pszText = cTemp;

				SendDlgItemMessage(hwndDlg, IDC_LISTATTRIBUT, LVM_SETITEMTEXT , (WPARAM)m_iIndex, (LPARAM) (const LPLVITEM)&lvItem);

				
			}
			break;
		case IDC_SUPCRITERE:
			{
				int iSelectItem = -1;
				while((iSelectItem = SendDlgItemMessage(hwndDlg, IDC_LISTATTRIBUT, LVM_GETNEXTITEM,(WPARAM) iSelectItem, MAKELPARAM((UINT) LVNI_SELECTED, 0))) != -1)
				{
					SendDlgItemMessage(hwndDlg, IDC_LISTATTRIBUT, LVM_DELETEITEM ,(WPARAM) iSelectItem, 0);
					iNumItem -= 1;
				}					
			}
			break;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CPageCritereAlbumDlg::OnNotify(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPNMHDR lpnm = (LPNMHDR) lParam;

	switch (lpnm->code)
		{
		case PSN_SETACTIVE : //Enable the appropriate buttons.
			PropSheet_SetWizButtons(GetParent(hwndDlg), PSWIZB_NEXT);
			break;

		case PSN_WIZNEXT :
			{
				CObjet::StartWait();
				
				CAlbumData * m_cAlbumData;
				AttributVector * m_attributVector;

				m_cAlbumData = m_cAlbumWizard->RecupPtAlbum();
				m_attributVector = m_cAlbumWizard->RecupPtAttribut();

				//AJOUTIMAGEDATA m_AjoutImageData;
				//m_AjoutImageData.hWnd = hwndDlg;
				//m_AjoutImageData.iCas = 4;
				//m_AjoutImageData.iValueChecked = pdata->iValueChecked;

				CInsertAlbum * m_cInsertCatalogue = new CInsertAlbum(hInstance);
				m_cInsertCatalogue->SetWindow(hwndDlg);
				m_cInsertCatalogue->SetCas(4);
				m_cInsertCatalogue->SetValueChecked(m_cAlbumWizard->GetValueChecked());
#ifdef SQLSERVERCE
				m_cInsertCatalogue->SetAlbum(m_cAlbumData, NULL);
#else
				m_cInsertCatalogue->SetAlbum(m_cAlbumData);
#endif
				m_cInsertCatalogue->SetAttributVector(m_attributVector);

				m_cInsertCatalogue->CreateDlg(hInstance, (LPCTSTR)IDD_FICHIER, hwndDlg);
				delete m_cInsertCatalogue;

#ifdef SQLSERVERCE

				//Mise à jour des attributs pour les albums
				CSqlAlbumUtility::UpdateCategorie(m_attributVector);

#endif
				//Appel de la boite de dialogue
				//DialogBoxParam(hInstance, (LPCTSTR)IDD_FICHIER, hwndDlg, (DLGPROC)InsertCatalog,(LPARAM)&m_AjoutImageData);

				CObjet::EndWait();
			}
			SetWindowLong(hwndDlg, DWL_MSGRESULT, IDD_FIN);
			return TRUE;
			break;

		default :
			//CDialog::AlbumCritereNotify(hwndDlg, uMsg, wParam, lParam, iSelectItem,2);
			m_cAlbumCritereNotify->SetParam(iSelectItem,2);
			m_cAlbumCritereNotify->TraitementMessage(hwndDlg, uMsg, wParam, lParam);
			break;
		}
	return 0;
}