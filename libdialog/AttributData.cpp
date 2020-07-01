#include "stdafx.h"
#include ".\attributdata.h"
#include "..\resource.h"
#include <attribut.h>
#include <dialogcataloguedata.h>
#ifdef SQLSERVERCE
#include <SqlAlbumIconeFileAttribut.h>
using namespace Regards::Sqlite;
#endif

CAttributData::CAttributData(HINSTANCE hInstance)
{
	hInstanceLocal = hInstance;
	m_iSelectItem = 0;
	//m_iNumItem = 0;
}

CAttributData::~CAttributData(void)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

void CAttributData::GetAttributValue(CAttribut &m_AttributTemp, HWND hDlg)
{
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

}

//////////////////////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////////////////////
void CAttributData::InitAttributDialog(HWND &hDlg, const int &iDataType, int &iNumItem)
{
	SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT,LVM_SETEXTENDEDLISTVIEWSTYLE, 0,(LPARAM)LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	//Définition de l'entete de colonne
	LVCOLUMN lvColumn;

	//IDS_CATEGORYVALEUR
	char cTemp[20];
	LoadString(hInstanceLocal,IDS_CATEGORYVALEUR,cTemp,20);

	lvColumn.mask = LVCF_TEXT|LVCF_WIDTH;
	lvColumn.cx = 135;
	lvColumn.pszText = cTemp;
	SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_INSERTCOLUMN , (WPARAM)0, (LPARAM)&lvColumn);


	//IDS_CATEGORYNUMERO

	LoadString(hInstanceLocal,IDS_CATEGORYNUMERO,cTemp,20);

	lvColumn.mask = LVCF_TEXT|LVCF_WIDTH;
	lvColumn.cx = 55;
	lvColumn.pszText = cTemp;
	SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_INSERTCOLUMN , (WPARAM)2, (LPARAM)&lvColumn);

	//IDS_CATEGORYATTRIBUT

	LoadString(hInstanceLocal,IDS_CATEGORYATTRIBUT,cTemp,20);


	// Add columns
	lvColumn.mask = LVCF_TEXT|LVCF_WIDTH;
	lvColumn.cx = 135;
	lvColumn.pszText = cTemp;
	SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_INSERTCOLUMN , (WPARAM)1, (LPARAM)&lvColumn);

	//Initialisation de la boite de dialogue (Liste Box)
	for (AttributVector::iterator AttributIt = m_cDialogCatalogueData->m_AttributVector->begin(); AttributIt!= m_cDialogCatalogueData->m_AttributVector->end(); AttributIt++)
	{
		CAttribut m_AttributTemp;
		int m_iIndex;
		char cTemp[50];
		m_AttributTemp = *AttributIt;
		LVITEM lvItem;
		ZeroMemory(&lvItem,sizeof(LVITEM));
		int iInfos = 0;

		if(iDataType == 1)
		{

#ifdef SQLSERVERCE

			for (AttributVector::iterator AttributItFile = m_cDialogCatalogueData->m_AttributVectorPicture->begin(); AttributItFile!= m_cDialogCatalogueData->m_AttributVectorPicture->end(); AttributItFile++)
			{
				CAttribut m_AttributTempLocal = *AttributItFile;
				if(m_AttributTempLocal.iNumAttribut == m_AttributTemp.iNumAttribut)
				{
					lvItem.mask = LVIF_TEXT;
					lvItem.iItem = iNumItem;
					lvItem.iSubItem = 0;
					lvItem.pszText = m_AttributTempLocal.cLibelle;
					m_iIndex = SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_INSERTITEM , (WPARAM)iNumItem, (LPARAM) (const LPLVITEM)&lvItem);
					iInfos = 1;
					break;
				}
			}

#else

			for (AttributVector::iterator AttributItFile = m_cDialogCatalogueData->m_IconeFile->m_AttributVector.begin(); AttributItFile!= m_cDialogCatalogueData->m_IconeFile->m_AttributVector.end(); AttributItFile++)
			{
				CAttribut m_AttributTempLocal = *AttributItFile;
				if(m_AttributTempLocal.iNumAttribut == m_AttributTemp.iNumAttribut)
				{
					lvItem.mask = LVIF_TEXT;
					lvItem.iItem = iNumItem;
					lvItem.iSubItem = 0;
					lvItem.pszText = m_AttributTempLocal.cLibelle;
					m_iIndex = SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_INSERTITEM , (WPARAM)iNumItem, (LPARAM) (const LPLVITEM)&lvItem);
					iInfos = 1;
					break;
				}
			}

#endif

		}

		if(iDataType == 0 || !iInfos)
		{
				lvItem.mask = LVIF_TEXT;
				lvItem.iItem = iNumItem;
				lvItem.iSubItem = 0;
				lvItem.pszText = "";
				m_iIndex = SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_INSERTITEM , (WPARAM)iNumItem, (LPARAM) (const LPLVITEM)&lvItem);
		}

		_itoa_s(m_AttributTemp.iNumAttribut,cTemp,50,10);

		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = m_iIndex;
		lvItem.iSubItem = 2;
		lvItem.pszText = cTemp;
		
		SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_SETITEMTEXT , (WPARAM)m_iIndex, (LPARAM) (const LPLVITEM)&lvItem);

		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = m_iIndex;
		lvItem.iSubItem = 1;
		lvItem.pszText = m_AttributTemp.cLibelle;

		//iNumLast = m_AttributTemp.iNumAttribut;

		SendDlgItemMessage(hDlg, IDC_LISTATTRIBUT, LVM_SETITEMTEXT , (WPARAM)m_iIndex, (LPARAM) (const LPLVITEM)&lvItem);

		//Recherche dans le vecteur attribut de l'image si une valeur correspond
		iNumItem++;
	}
}
