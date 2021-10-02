#include "StdAfx.h"
#include ".\dialogfindproc.h"
#include "treewindowdata.h"
#include "resource.h"
#include "objet.h"
#include "list.h"

CDialogFindProc::CDialogFindProc(void)
{
	iOldYPos = 0;
	iNumItem = -1;
	iPos = -1;
}

CDialogFindProc::~CDialogFindProc(void)
{
}

void CDialogFindProc::SetParam(CTreeWindowData * pTreeData)
{
	m_pTreeData = pTreeData;
}

LRESULT CDialogFindProc::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CheckDlgButton(hDlg,IDC_RDBAS,BST_CHECKED);
	SetFocus(GetDlgItem(hDlg,IDC_EDRECHERCHER));
	iPos = iNumItem = iOldYPos = m_pTreeData->iNumItem;
	return 0;
}

LRESULT CDialogFindProc::OnGetDlgCode(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DLGC_WANTALLKEYS;
}

LRESULT CDialogFindProc::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	wmId    = LOWORD(wParam); 
	wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{
		case IDCANCEL:
			{
				EndDialog(hDlg, LOWORD(wParam));
			}
			return TRUE;


		case IDSUIVANT:
			{
				POINT pt;
				char Temp[MAX_PATH];
				char m_stgFullPathName[MAX_PATH];
				char m_stgFilenameTemp[MAX_PATH];
				CObjet m_cObjet;
				int iIndex = -1;
				UINT iFlags = LVNI_BELOW;
				int iMotComplet = 0;
				int iCasse = 0;

				if(IsDlgButtonChecked(hDlg,IDC_CHKMOTSEUL))
				{
					iMotComplet = 1;
				}

				if(IsDlgButtonChecked(hDlg,IDC_CHKRESPCASSE))
				{
					iCasse = 1;
				}


				GetDlgItemText(hDlg,IDC_EDRECHERCHER,Temp,255);

				if(iPos != -1)
				{
					if(IsDlgButtonChecked(hDlg,IDC_RDHAUT))
					{
						iFlags = LVNI_ABOVE;
					}
				}
				else
				{
					iFlags = LVNI_ALL;
				}


				//Rechercher fichier correspondant

				iPos = ListView_GetNextItem(m_pTreeData->cList->m_hWnd, iPos, iFlags);
				while (iPos != -1) 
				{
					m_pTreeData->cList->List_GetItemText(iPos, m_stgFullPathName, MAX_PATH);

					m_cObjet.GetFileName(m_stgFullPathName,m_stgFilenameTemp,MAX_PATH);

					if(iMotComplet)
					{
						if(iCasse)
							iIndex = _stricmp(Temp,m_stgFilenameTemp);
						else
							iIndex = _stricmp(Temp,m_stgFilenameTemp);

						if(iIndex == 0)
							break;
					}
					else
					{
						string m_stgFilename;
						if(iCasse)
						{
							m_stgFilename = m_stgFilenameTemp;
							iIndex = m_stgFilename.find(Temp);
						}
						else
						{
							errno_t erreur;
							erreur = _strlwr_s(m_stgFilenameTemp,MAX_PATH);
							if(erreur != -1)
								m_stgFilename = m_stgFilenameTemp;
							else
								break;

							erreur = _strlwr_s(Temp,MAX_PATH);
							if(erreur != -1)
								iIndex = m_stgFilename.find(Temp);
							else
								break;
						}

						

						if(iIndex != -1)
							break;
					}



					iPos = ListView_GetNextItem(m_pTreeData->cList->m_hWnd, iPos, iFlags);
				}

				if(iIndex != -1)
				{

					//HWND hWnd = m_pTreeData->cList->m_hWnd;

					//Déselectionnée le précédent
					if(iOldYPos != -1)
						ListView_SetItemState(m_pTreeData->cList->m_hWnd,iOldYPos,0,LVIS_SELECTED | LVIS_DROPHILITED);

					ListView_GetItemPosition(m_pTreeData->cList->m_hWnd,iPos,&pt);

					m_pTreeData->cList->List_EnsureVisible(iPos,iOldYPos);

					ListView_SetItemState(m_pTreeData->cList->m_hWnd,iPos,LVIS_SELECTED | LVIS_DROPHILITED, LVIS_SELECTED);

					iOldYPos = iPos;

					m_pTreeData->iNumItem = iPos;
				}
				else
				{
					m_cObjet.AffichageMessage(IDS_FICHIERNONTROUVE,IDS_INFOS);
				}
			}
			break;

	}
	return 0;
}