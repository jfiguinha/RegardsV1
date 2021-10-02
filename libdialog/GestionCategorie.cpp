#include "StdAfx.h"
#include ".\gestioncategorie.h"
#include "resource.h"
#include <dialogcataloguedata.h>
#include ".\albumcriterenotify.h"

CGestionCategorie::CGestionCategorie(HINSTANCE hInstance)
{
	m_cAlbumCritereNotify = new CAlbumCritereNotify(hInstance);
	hInstanceLocal = hInstance;
	m_iNumItem = 0;
	m_iSelectItem = 0;
	m_iNumLast = 0;
	m_bReturnValue = false;
}

CGestionCategorie::~CGestionCategorie(void)
{
	delete m_cAlbumCritereNotify;
}


LRESULT CGestionCategorie::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT,LVM_SETEXTENDEDLISTVIEWSTYLE, 0,(LPARAM)LVS_EX_FULLROWSELECT |LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);

	//Définition de l'entete de colonne
	LVCOLUMN lvColumn;

	char Temp[20];
	LoadString(hInstanceLocal,IDS_CATEGORYNUMERO,Temp,20);

	lvColumn.mask = LVCF_TEXT|LVCF_WIDTH;
	lvColumn.cx = 55;
	lvColumn.pszText = Temp;
	SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_INSERTCOLUMN , (WPARAM)1, (LPARAM)&lvColumn);

	LoadString(hInstanceLocal,IDS_CATEGORYLIBELLE,Temp,20);

	// Add columns
	lvColumn.mask = LVCF_TEXT|LVCF_WIDTH;
	lvColumn.cx = 200;
	lvColumn.pszText = Temp;
	SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_INSERTCOLUMN , (WPARAM)0, (LPARAM)&lvColumn);


	//Initialisation de la boite de dialogue (Liste Box)
	for (AttributVector::iterator AttributIt = m_lpDialogData->m_AttributVector->begin(); AttributIt!= m_lpDialogData->m_AttributVector->end(); AttributIt++)
	{
		CAttribut m_AttributTemp;
		int m_iIndex;
		char cTemp[50];
		m_AttributTemp = *AttributIt;
		LVITEM lvItem;
		ZeroMemory(&lvItem,sizeof(LVITEM));
		lvItem.mask = LVIF_STATE | LVIF_IMAGE | LVIF_INDENT | LVIF_PARAM | LVIF_TEXT;
		lvItem.iItem = m_iNumItem;
		lvItem.lParam = (LPARAM)m_iNumItem;
		lvItem.iSubItem = 0;
		lvItem.pszText = m_AttributTemp.cLibelle;

		m_iIndex = SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_INSERTITEM , (WPARAM)0, (LPARAM) (const LPLVITEM)&lvItem);

		_itoa_s(m_AttributTemp.iNumAttribut,cTemp,50,10);

		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = m_iIndex;
		lvItem.iSubItem = 1;
		lvItem.pszText = cTemp;

		m_iNumLast = m_AttributTemp.iNumAttribut;

		SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_SETITEMTEXT , (WPARAM)m_iIndex, (LPARAM) (const LPLVITEM)&lvItem);

		m_iNumItem++;
	}
	return 0;
}

LRESULT CGestionCategorie::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{
		case IDC_NEW:
			{
				char Temp[20];
				LoadString(hInstanceLocal,IDS_NEWCRITERE,Temp,20);

				char cTemp[255];
				LVITEM lvItem;
				int m_iIndex;

				m_iNumLast++;

				ZeroMemory(&lvItem,sizeof(LVITEM));
				lvItem.mask = LVIF_STATE | LVIF_TEXT;
				lvItem.iItem = m_iNumItem;
				lvItem.iSubItem = 0;
				lvItem.pszText = Temp;

				m_iIndex = SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_INSERTITEM , (WPARAM)0, (LPARAM) (const LPLVITEM)&lvItem);

				_itoa_s(m_iNumLast,cTemp,255,10);

				ZeroMemory(&lvItem,sizeof(LVITEM));
				lvItem.mask = TVIF_TEXT;
				lvItem.iItem = m_iIndex;
				lvItem.iSubItem = 1;
				lvItem.pszText = cTemp;

				m_iIndex = SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_SETITEMTEXT , (WPARAM)m_iIndex, (LPARAM) (const LPLVITEM)&lvItem);

				m_iNumItem++;
			}
			return 0;

		case IDC_DELETE:
			m_iSelectItem = -1;
			while((m_iSelectItem = SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_GETNEXTITEM,(WPARAM) m_iSelectItem, MAKELPARAM((UINT) LVNI_SELECTED, 0))) != -1)
			{
				SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_DELETEITEM ,(WPARAM) m_iSelectItem, 0);
				m_iNumItem -= 1;
			}
			break;

		case IDC_OK:
			{
				LVITEM lvItem;
				ZeroMemory(&lvItem,sizeof(LVITEM));
				lvItem.stateMask = LVIS_SELECTED;
				lvItem.state = LVIS_SELECTED;

				//Sauvegarde de la liste des attributs dans le vecteur
				m_lpDialogData->m_AttributVector->clear();
				SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_SETITEMSTATE, -1, (LPARAM) (LPLVITEM) &lvItem);
				m_iSelectItem = -1;
				while((m_iSelectItem = SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_GETNEXTITEM,(WPARAM) m_iSelectItem, MAKELPARAM((UINT) LVNI_SELECTED, 0))) != -1)
				{
					CAttribut m_AttributTemp;
					char cTemp[255];
	
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
					lvItem.iSubItem = 1;
					lvItem.pszText = cTemp;
					SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_GETITEMTEXT,(WPARAM) m_iSelectItem,(LPARAM) (LPLVITEM) &lvItem);
					
					m_AttributTemp.iNumAttribut = atoi(cTemp);

					m_lpDialogData->m_AttributVector->push_back(m_AttributTemp);

				};
			}

			EndDialog(hDlg, LOWORD(wParam));
			m_bReturnValue = true;
			return TRUE;

		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			m_bReturnValue = false;
			return TRUE;

	}
	return 0;
}

LRESULT CGestionCategorie::OnNotify(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_cAlbumCritereNotify->SetParam(m_iSelectItem,1);
	m_cAlbumCritereNotify->TraitementMessage(hDlg, uMsg, wParam, lParam);
	return 0;
}