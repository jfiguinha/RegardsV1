#include "StdAfx.h"
#include ".\attributrepertoire.h"
#include ".\albumcriterenotify.h"
#include <dialogcataloguedata.h>
#include "../resource.h"
#include <attribut.h>

CAttributRepertoire::CAttributRepertoire(HINSTANCE hInstance)
{
	this->hInstanceLocal = hInstance;
	m_iNumItem = 0;
	m_iSelectItem = 0;
	m_cAlbumCritereNotify = new CAlbumCritereNotify(hInstance);
}

CAttributRepertoire::~CAttributRepertoire(void)
{
	if(m_cAlbumCritereNotify != NULL)
		delete m_cAlbumCritereNotify;

	m_cAlbumCritereNotify = NULL;
}

LRESULT CAttributRepertoire::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//IDS_ATTRIBUTPICTUREREP
	char cTemp[20];
	LoadString(hInstanceLocal,IDS_ATTRIBUTPICTUREREP,cTemp,20);

	SetWindowText(hDlg,cTemp);

	InitAttributDialog(hDlg,0,m_iNumItem);
	return 0;
}

LRESULT CAttributRepertoire::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{
		case IDC_OK:
			//Mise à jour des informations dans les attributs de l'image
			{

#ifdef SQLSERVERCE

				//Mise à jour de la liste des attributs global pour l'album
				LVITEM lvItem;
				ZeroMemory(&lvItem,sizeof(LVITEM));
				lvItem.stateMask = LVIS_SELECTED;
				lvItem.state = LVIS_SELECTED;
				SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_SETITEMSTATE, -1, (LPARAM) (LPLVITEM) &lvItem);
				int m_iSelectItem = -1;
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

				//Mise à jour des attributs pour les fichiers de l'album
				//Mise à jour de la liste des attributs global pour l'album
				m_iSelectItem = -1;
				m_cDialogCatalogueData->m_AttributVectorPicture->clear();
				while((m_iSelectItem = SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_GETNEXTITEM,(WPARAM) m_iSelectItem, MAKELPARAM((UINT) LVNI_SELECTED, 0))) != -1)
				{
						CAttribut m_AttributTemp;
						char cTemp[255];
						LVITEM lvItem;
		
						ZeroMemory(&lvItem,sizeof(LVITEM));
						lvItem.cchTextMax = 254;
						lvItem.iSubItem = 0;
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

						m_cDialogCatalogueData->m_AttributVectorPicture->push_back(m_AttributTemp);
				}


#else
				IconeFileVector::iterator dit;

				for (dit = m_cDialogCatalogueData->m_Album->m_IconeFileVector.begin(); dit != m_cDialogCatalogueData->m_Album->m_IconeFileVector.end(); dit++)
				{
					int iNumAttribut = 0;
					LVITEM lvItem;
					
					IconeFileVector::iterator m_IconeFile = dit;
					//IconeFile * m_IconeFile = dit;

					ZeroMemory(&lvItem,sizeof(LVITEM));
					lvItem.stateMask = LVIS_SELECTED;
					lvItem.state = LVIS_SELECTED;

					m_IconeFile->m_AttributVector.clear();

					SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_SETITEMSTATE, -1, (LPARAM) (LPLVITEM) &lvItem);
					m_iSelectItem = -1;
					while((m_iSelectItem = SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_GETNEXTITEM,(WPARAM) m_iSelectItem, MAKELPARAM((UINT) LVNI_SELECTED, 0))) != -1)
					{
						CAttribut m_AttributTemp;
						GetAttributValue(m_AttributTemp,hDlg);
						m_IconeFile->m_AttributVector.push_back(m_AttributTemp);
						iNumAttribut++;

					}
					m_IconeFile->iNbAttribut = iNumAttribut;

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
#endif
			}
			m_iReturnValue = 1;
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;

		case IDC_CANCEL:
			m_iReturnValue = 0;
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;

		case IDCANCEL:
			m_iReturnValue = 0;
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;

		case IDC_NEWCRITERE:
			{
				char Temp[20];
				LoadString(hInstanceLocal,IDS_NEWCRITERE,Temp,20);

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