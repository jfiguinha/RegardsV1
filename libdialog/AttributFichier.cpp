#include "StdAfx.h"
#include ".\attributfichier.h"
#include ".\albumcriterenotify.h"
#include <dialogcataloguedata.h>
#include "../resource.h"
#include <attribut.h>

CAttributFichier::CAttributFichier(HINSTANCE hInstance)
{
	hInstanceLocal = hInstance;
	m_cAlbumCritereNotify = new CAlbumCritereNotify(hInstance);
	m_iSelectItem = 0;
	m_iNumItem = 0;
	m_iReturnValue = 0;
}

CAttributFichier::~CAttributFichier(void)
{
	if(m_cAlbumCritereNotify != NULL)
		delete m_cAlbumCritereNotify;

	m_cAlbumCritereNotify = NULL;
}



//////////////////////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////////////////////
LRESULT CAttributFichier::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	InitAttributDialog(hDlg,1,m_iNumItem);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////////////////////
LRESULT CAttributFichier::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{
		case IDC_OK:
			//Mise à jour des informations dans les attributs de l'image
			{
				int iNumAttribut = 0;
				LVITEM lvItem;
				ZeroMemory(&lvItem,sizeof(LVITEM));
				lvItem.stateMask = LVIS_SELECTED;
				lvItem.state = LVIS_SELECTED;

				//Sauvegarde de la liste des attributs dans le vecteur

#ifdef SQLSERVERCE
				m_cDialogCatalogueData->m_AttributVectorPicture->clear();
#else
				m_cDialogCatalogueData->m_IconeFile->m_AttributVector.clear();
#endif
				SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_SETITEMSTATE, -1, (LPARAM) (LPLVITEM) &lvItem);
				m_iSelectItem = -1;
				while((m_iSelectItem = SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_GETNEXTITEM,(WPARAM) m_iSelectItem, MAKELPARAM((UINT) LVNI_SELECTED, 0))) != -1)
				{
					CAttribut m_AttributTemp;
					GetAttributValue(m_AttributTemp,hDlg);
#ifdef SQLSERVERCE
					m_cDialogCatalogueData->m_AttributVectorPicture->push_back(m_AttributTemp);
#else
					m_cDialogCatalogueData->m_IconeFile->m_AttributVector.push_back(m_AttributTemp);
#endif
					iNumAttribut++;

				}
				m_cDialogCatalogueData->m_IconeFile->iNbAttribut = iNumAttribut;

				//Mise à jour de la liste des attributs
				m_iSelectItem = -1;
				m_cDialogCatalogueData->m_AttributVector->clear();
				while((m_iSelectItem = SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_GETNEXTITEM,(WPARAM) m_iSelectItem, MAKELPARAM((UINT) LVNI_SELECTED, 0))) != -1)
				{
						CAttribut m_AttributTemp;
						char cTemp[255];
						LVITEM lvItem;
		
						ZeroMemory(&lvItem,sizeof(LVITEM));
						lvItem.cchTextMax = 254;
						lvItem.iSubItem = 1;
						lvItem.pszText = cTemp;
						SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_GETITEMTEXT,(WPARAM) m_iSelectItem,(LPARAM) (LPLVITEM) &lvItem);

						StringCchCopy(m_AttributTemp.cLibelle,255,cTemp);

						ZeroMemory(&lvItem,sizeof(LVITEM));
						lvItem.mask = TVIF_TEXT;
						lvItem.cchTextMax = 254;
						lvItem.iItem = m_iSelectItem;
						lvItem.iSubItem = 2;
						lvItem.pszText = cTemp;
						SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_GETITEMTEXT,(WPARAM) m_iSelectItem,(LPARAM) (LPLVITEM) &lvItem);
						
						m_AttributTemp.iNumAttribut = atoi(cTemp);

						m_cDialogCatalogueData->m_AttributVector->push_back(m_AttributTemp);
				}
			}
			EndDialog(hDlg, LOWORD(wParam));
			m_iReturnValue = 1;
			return TRUE;

		case IDC_CANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			m_iReturnValue = 0;
			return TRUE;

		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			m_iReturnValue = 0;
			return TRUE;

		case IDC_NEWCRITERE:
			{
				char Temp[255];
				LoadString(hInstanceLocal,IDS_NEWCRITERE,Temp,255);

				//char Temp[] = "Entrée une valeur";
				char cTemp[255];
				LVITEM lvItem;
				int m_iIndex;


				ZeroMemory(&lvItem,sizeof(LVITEM));
				lvItem.mask = LVIF_STATE | LVIF_TEXT;
				lvItem.iItem = m_iNumItem;
				lvItem.iSubItem = 0;
				lvItem.pszText = Temp;

				m_iIndex = SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_INSERTITEM , (WPARAM)0, (LPARAM) (const LPLVITEM)&lvItem);

				m_iNumItem++;

				lvItem.mask = LVIF_TEXT;
				lvItem.iItem = m_iIndex;
				lvItem.iSubItem = 1;
				lvItem.pszText = Temp;


				SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_SETITEMTEXT , (WPARAM)m_iIndex, (LPARAM) (const LPLVITEM)&lvItem);

				_itoa_s(m_iNumItem,cTemp,255,10);

				ZeroMemory(&lvItem,sizeof(LVITEM));
				lvItem.mask = TVIF_TEXT;
				lvItem.iItem = m_iIndex;
				lvItem.iSubItem = 2;
				lvItem.pszText = cTemp;

				SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_SETITEMTEXT , (WPARAM)m_iIndex, (LPARAM) (const LPLVITEM)&lvItem);

				
			}
			break;
		case IDC_SUPCRITERE:
			{
				while((m_iSelectItem = SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_GETNEXTITEM,(WPARAM) m_iSelectItem, MAKELPARAM((UINT) LVNI_SELECTED, 0))) != -1)
				{
					SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_DELETEITEM ,(WPARAM) m_iSelectItem, 0);
					m_iNumItem -= 1;
				}					
			}
			break;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////////////////////
LRESULT CAttributFichier::OnNotify(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//AlbumCritereNotify(hDlg, message, wParam, lParam, iSelectItem,2);
	m_cAlbumCritereNotify->SetParam(m_iSelectItem,2);
	m_cAlbumCritereNotify->TraitementMessage(hDlg, uMsg, wParam, lParam);
	return 1;
}

