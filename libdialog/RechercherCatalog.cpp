#include "StdAfx.h"
#include ".\recherchercatalog.h"
#include <CatalogueData.h>
#include <ItemInfo.h>
#include "..\resource.h"
#include "..\objet.h"
#include "..\ImageCalalogueRecherche.h"
#include "lancerrechercher.h"

CRechercherCatalog::CRechercherCatalog(HINSTANCE hInstance)
{
	iPointeur = -1;
	iTabCritere = NULL;
	this->hInstance = hInstance;
}

CRechercherCatalog::~CRechercherCatalog(void)
{

	if(iTabCritere != NULL)
		delete[] iTabCritere;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

UINT CRechercherCatalog::CreateColonneCatalog(HWND hDlg, const int &iLibelle)
{
    char szText[256];     // temporary buffer 
    LVCOLUMN lvc; 
    int iCol; 
 
    // Initialize the LVCOLUMN structure.
    // The mask specifies that the format, width, text, and subitem
    // members of the structure are valid. 
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
     
    // Add the columns. 
    for (iCol = 0; iCol < 1; iCol++) 
	{ 
        lvc.iSubItem = iCol;
        lvc.pszText = szText;	
        lvc.cx = 500;           // width of column in pixels
		lvc.fmt = LVCFMT_LEFT;  // left-aligned column

        LoadString(hInstance, iLibelle, szText, sizeof(szText));

        if (ListView_InsertColumn(hDlg, iCol, &lvc) == -1) 
            return FALSE; 
    } 


	ListView_SetExtendedListViewStyle(hDlg, LVS_EX_FULLROWSELECT );

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////
//Recherche si item est sélectionné
///////////////////////////////////////////////////////////////////////////////////////

UINT CRechercherCatalog::VerifPresenceItem(HWND hDlg, const char * m_cFilename)
{
	if(hDlg == NULL)
		return 0;

	int iPos = ListView_GetNextItem(hDlg,-1,LVNI_ALL);

	while(iPos != -1)
	{
		CItemInfo * pItem;
		LVITEM   lvItem;

		lvItem.mask = LVIF_PARAM;
		lvItem.iItem = iPos;

		ListView_GetItem(hDlg,&lvItem);

		pItem = (CItemInfo *)lvItem.lParam;

		if(_stricmp(pItem->GetParseName(),m_cFilename) == 0)
			return 1;


		iPos = ListView_GetNextItem(hDlg,iPos,LVNI_ALL);

	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

UINT CRechercherCatalog::InsertItemCatalog(HWND hDlg, HWND hDlgComp)
{
	for(int i = 0;i < m_CatalogueData->m_AlbumDataVector.size();i++)
	{
		LVITEM   lvItem;
		CItemInfo * pItem;

		if(!VerifPresenceItem(hDlgComp,m_CatalogueData->m_AlbumDataVector[i].m_szAlbumName) && !VerifPresenceItem(hDlg,m_CatalogueData->m_AlbumDataVector[i].m_szAlbumName))
		{
			ZeroMemory(&lvItem, sizeof(lvItem));

			//fill in the TV_ITEM structure for this item
			lvItem.mask = LVIF_PARAM | LVIF_TEXT;

			//put the private information in the lParam
			pItem = new CItemInfo();
			pItem->SetParseName(m_CatalogueData->m_AlbumDataVector[i].m_szAlbumName);
			pItem->SetIPos(i);
			lvItem.lParam = (LPARAM)pItem;

			//text and images are done on a callback basis
			lvItem.pszText = pItem->GetParseName();

			int dIndex = ListView_InsertItem(hDlg, &lvItem);		
		}
	}
	return 0;
}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

UINT CRechercherCatalog::InsertItemCatalog(HWND hDlgSrc, HWND hDlgDst, HWND hDlgComp)
{
	LVITEM   lvItem;
	int iItem;

	iItem = ListView_GetNextItem(hDlgSrc,-1,LVNI_SELECTED);

	if(iItem != -1)
	{

		CItemInfo * pItem2, * pItem;
		LVITEM   lvItem2;

		lvItem.mask = LVIF_PARAM;
		lvItem.iItem = iItem;

		ListView_GetItem(hDlgSrc,&lvItem);

		pItem = (CItemInfo *)lvItem.lParam;

		if(!VerifPresenceItem(hDlgComp,pItem->GetParseName()))
		{

			ZeroMemory(&lvItem2, sizeof(lvItem));

			//fill in the TV_ITEM structure for this item
			lvItem2.mask = LVIF_PARAM | LVIF_TEXT;

			//put the private information in the lParam
			pItem2 = new CItemInfo(pItem);

			//memcpy(pItem2,pItem,sizeof(CItemInfo));

			lvItem2.lParam = (LPARAM)pItem2;

			//text and images are done on a callback basis
			lvItem2.pszText = pItem2->GetParseName();

			int dIndex = ListView_InsertItem(hDlgDst, &lvItem2);

			delete pItem;
			//free(pItem);

			ListView_DeleteItem(hDlgSrc,iItem);

			return 1;
		}

	}

	return -1;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

UINT CRechercherCatalog::InsertItemCatalog(HWND hDlgSrc, HWND hDlgDst, HWND hDlgComp, const int &iNumItem)
{
	LVITEM   lvItem;

	if(iNumItem != -1)
	{

		CItemInfo * pItem2, * pItem;
		LVITEM   lvItem2;

		lvItem.mask = LVIF_PARAM;
		lvItem.iItem = iNumItem;

		ListView_GetItem(hDlgSrc,&lvItem);

		pItem = (CItemInfo *)lvItem.lParam;

		if(!VerifPresenceItem(hDlgComp,pItem->GetParseName()))
		{

			ZeroMemory(&lvItem2, sizeof(lvItem));

			//fill in the TV_ITEM structure for this item
			lvItem2.mask = LVIF_PARAM | LVIF_TEXT;

			//put the private information in the lParam
			pItem2 = new CItemInfo(pItem);

			//memcpy(pItem2,pItem,sizeof(CItemInfo));

			lvItem2.lParam = (LPARAM)pItem2;

			//text and images are done on a callback basis
			lvItem2.pszText = pItem2->GetParseName();

			int dIndex = ListView_InsertItem(hDlgDst, &lvItem2);

			if(pItem != NULL)
				delete pItem;

			pItem = NULL;

			ListView_DeleteItem(hDlgSrc,iNumItem);

			return 1;
		}

	}

	return -1;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

UINT CRechercherCatalog::SuppAllItemCatalog(HWND hDlgSrc)
{
	LVITEM   lvItem;
	int iPos = ListView_GetItemCount(hDlgSrc);

	for(int i = 0;i < iPos;i++)
	{
		int iItem;
		CItemInfo * pItem;

		lvItem.mask = LVIF_PARAM;
		lvItem.iItem = 0;

		iItem = ListView_GetItem(hDlgSrc,&lvItem);

		pItem = (CItemInfo *)lvItem.lParam;

		if(pItem != NULL)
			delete pItem;

		pItem = NULL;

		ListView_DeleteItem(hDlgSrc,0);
	}
	
	return 0;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

UINT CRechercherCatalog::AddItemCritere(HWND hDlgSrc)
{
	//Récupération de la liste des critères disponibles
	char * clpText = new char[255];
	string m_stgChaineRecherche;
	int iPos = 0;
	int iOldiPos = 0;
	int i = 0;

	LoadString(hInstance,IDS_CRITERECATALOGUE,clpText,255);

	m_stgChaineRecherche = clpText;

	delete[] clpText;

	do
	{
		iPos = m_stgChaineRecherche.find(";",iPos);

		if(iPos != -1)
		{

			LVITEM   lvItem;
			CItemInfo * pItem;

			ZeroMemory(&lvItem, sizeof(lvItem));

			//fill in the TV_ITEM structure for this item
			lvItem.mask = LVIF_PARAM | LVIF_TEXT;

			//put the private information in the lParam
			pItem = new CItemInfo();
			pItem->SetParseName(m_stgChaineRecherche.substr(iOldiPos,iPos - iOldiPos).c_str());
			pItem->SetIPos(i);
			lvItem.lParam = (LPARAM)pItem;

			//text and images are done on a callback basis
			lvItem.pszText = pItem->GetParseName();

			lvItem.iItem = i;

			int dIndex = ListView_InsertItem(hDlgSrc, &lvItem);

			iPos++;

			iOldiPos = iPos;i++;
		}

	}while(iPos != -1);

	return i+1;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

UINT CRechercherCatalog::AffichageCritereBouton(HWND hDlg,int * iTabCritere, const int &iPointeur)
{
	if(iPointeur >= 0)
		EnableWindow(GetDlgItem(hDlg, IDC_SUPCRITERE),true);

	if(iPointeur < 0)
	{
		EnableWindow(GetDlgItem(hDlg, IDC_SUPCRITERE),false);
		EnableWindow(GetDlgItem(hDlg, IDC_BTOU),false);
		EnableWindow(GetDlgItem(hDlg, IDC_BTET),false);
		EnableWindow(GetDlgItem(hDlg, IDC_BTPARANTHESEOUV),true);
		EnableWindow(GetDlgItem(hDlg, IDC_BTAJOUTCRITERE),true);
		EnableWindow(GetDlgItem(hDlg, IDC_BTPARANTHESEFERM),false);
		return 0;
	}

	if(iTabCritere[iPointeur] == 1)
	{
		EnableWindow(GetDlgItem(hDlg, IDC_BTOU),true);
		EnableWindow(GetDlgItem(hDlg, IDC_BTET),true);
		EnableWindow(GetDlgItem(hDlg, IDC_BTPARANTHESEOUV),false);
		EnableWindow(GetDlgItem(hDlg, IDC_BTAJOUTCRITERE),false);
		EnableWindow(GetDlgItem(hDlg, IDC_BTPARANTHESEFERM),false);

		int iNbOuverture = 0;
		int iNbFermeture = 0;
		for(int i = 0;i < iPointeur;i++)
		{
			if(iTabCritere[i] == 2)
				iNbOuverture++;

			if(iTabCritere[i] == 3)
				iNbFermeture++;
		}

		if(iNbOuverture > iNbFermeture)
			EnableWindow(GetDlgItem(hDlg, IDC_BTPARANTHESEFERM),true);

		return 0;
	}

	if(iTabCritere[iPointeur] == 2)
	{
		EnableWindow(GetDlgItem(hDlg, IDC_BTOU),false);
		EnableWindow(GetDlgItem(hDlg, IDC_BTET),false);
		EnableWindow(GetDlgItem(hDlg, IDC_BTPARANTHESEOUV),false);
		EnableWindow(GetDlgItem(hDlg, IDC_BTAJOUTCRITERE),true);
		EnableWindow(GetDlgItem(hDlg, IDC_BTPARANTHESEFERM),false);
		return 0;
	}


	if(iTabCritere[iPointeur] == 4 || iTabCritere[iPointeur] == 5)
	{
		EnableWindow(GetDlgItem(hDlg, IDC_BTOU),false);
		EnableWindow(GetDlgItem(hDlg, IDC_BTET),false);
		EnableWindow(GetDlgItem(hDlg, IDC_BTPARANTHESEOUV),true);
		EnableWindow(GetDlgItem(hDlg, IDC_BTAJOUTCRITERE),true);
		EnableWindow(GetDlgItem(hDlg, IDC_BTPARANTHESEFERM),false);
		return 0;
	}

	return 0;
}



LRESULT CRechercherCatalog::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_stgChaineRechercheAff = "";
	m_stgChaineRecherche = "";

	iPointeur = -1;
	
	int iNbCritere = AddItemCritere(GetDlgItem(hDlg, IDC_LISTCRITERE));

	iTabCritere = new int[iNbCritere * 2];

	//Insertion des listes dans les listes box

	CreateColonneCatalog(GetDlgItem(hDlg, IDC_LISTSOURCEDISPO),IDS_CATALOGUENAME);
	CreateColonneCatalog(GetDlgItem(hDlg, IDC_LISTSOURCE),IDS_CATALOGUENAME);
	CreateColonneCatalog(GetDlgItem(hDlg, IDC_LISTCOMPARAISONDISPO),IDS_CATALOGUENAME);
	CreateColonneCatalog(GetDlgItem(hDlg, IDC_LISTCOMPARAISON),IDS_CATALOGUENAME);

	CreateColonneCatalog(GetDlgItem(hDlg, IDC_LISTCRITERE),IDS_CRITERELIST);

	InsertItemCatalog(GetDlgItem(hDlg, IDC_LISTSOURCEDISPO),GetDlgItem(hDlg, IDC_LISTCOMPARAISON));
	InsertItemCatalog(GetDlgItem(hDlg, IDC_LISTCOMPARAISONDISPO),GetDlgItem(hDlg, IDC_LISTSOURCE));

	return 0;
}

LRESULT CRechercherCatalog::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//int bResult = PostThreadMessage(dwThreadId, WM_NULL, 0, 0);
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{
		case IDC_AJOUTCATSRC:
			{

				if(InsertItemCatalog(GetDlgItem(hDlg, IDC_LISTSOURCEDISPO),GetDlgItem(hDlg, IDC_LISTSOURCE),GetDlgItem(hDlg, IDC_LISTCOMPARAISON)))
				{
					if(!IsWindowEnabled(GetDlgItem(hDlg, IDC_SUPPCATSRC)))
					{
						EnableWindow(GetDlgItem(hDlg, IDC_SUPPCATSRC),true);
						EnableWindow(GetDlgItem(hDlg, IDC_SUPALLCATSRC),true);
					}

					if(ListView_GetItemCount(GetDlgItem(hDlg, IDC_LISTSOURCEDISPO)) == 0)
					{
						EnableWindow(GetDlgItem(hDlg, IDC_AJOUTALLCATSRC),false);
						EnableWindow(GetDlgItem(hDlg, IDC_AJOUTCATSRC),false);
					}
				}
			}
			break;

		case IDC_AJOUTALLCATSRC:
			{
				int iNbItem = ListView_GetItemCount(GetDlgItem(hDlg, IDC_LISTSOURCEDISPO));

				if(iNbItem > 0)
				{
					int j = 0;
					for(int i = 0;i < iNbItem;i++)
					{
						if(InsertItemCatalog(GetDlgItem(hDlg, IDC_LISTSOURCE),GetDlgItem(hDlg, IDC_LISTSOURCEDISPO),GetDlgItem(hDlg, IDC_LISTCOMPARAISON),j) == -1)
							j++;
					}


					//SuppAllItemCatalog(GetDlgItem(hDlg, IDC_LISTSOURCEDISPO));
					//InsertItemCatalog(GetDlgItem(hDlg, IDC_LISTSOURCE),m_CatalogueData,GetDlgItem(hDlg, IDC_LISTCOMPARAISON));
					
					EnableWindow(GetDlgItem(hDlg, IDC_AJOUTALLCATSRC),false);
					EnableWindow(GetDlgItem(hDlg, IDC_AJOUTCATSRC),false);

					if(!IsWindowEnabled(GetDlgItem(hDlg, IDC_SUPPCATSRC)))
					{
						EnableWindow(GetDlgItem(hDlg, IDC_SUPPCATSRC),true);
						EnableWindow(GetDlgItem(hDlg, IDC_SUPALLCATSRC),true);
					}

				}
			}
			break;

		case IDC_SUPPCATSRC:
			{
				if(InsertItemCatalog(GetDlgItem(hDlg, IDC_LISTSOURCE),GetDlgItem(hDlg, IDC_LISTSOURCEDISPO),NULL))
				{

					if(!IsWindowEnabled(GetDlgItem(hDlg, IDC_AJOUTALLCATSRC)))
					{
						EnableWindow(GetDlgItem(hDlg, IDC_AJOUTALLCATSRC),true);
						EnableWindow(GetDlgItem(hDlg, IDC_AJOUTCATSRC),true);
					}

					if(ListView_GetItemCount(GetDlgItem(hDlg, IDC_LISTSOURCE)) == 0)
					{
						EnableWindow(GetDlgItem(hDlg, IDC_SUPPCATSRC),false);
						EnableWindow(GetDlgItem(hDlg, IDC_SUPALLCATSRC),false);
					}
				}
			}
			break;

		case IDC_SUPALLCATSRC:
			{
				if(ListView_GetItemCount(GetDlgItem(hDlg, IDC_LISTSOURCE)) > 0)
				{
					SuppAllItemCatalog(GetDlgItem(hDlg, IDC_LISTSOURCE));
					InsertItemCatalog(GetDlgItem(hDlg, IDC_LISTSOURCEDISPO),GetDlgItem(hDlg, IDC_LISTSOURCEDISPO));

					EnableWindow(GetDlgItem(hDlg, IDC_SUPPCATSRC),false);
					EnableWindow(GetDlgItem(hDlg, IDC_SUPALLCATSRC),false);


					if(!IsWindowEnabled(GetDlgItem(hDlg, IDC_AJOUTALLCATSRC)))
					{
						EnableWindow(GetDlgItem(hDlg, IDC_AJOUTALLCATSRC),true);
						EnableWindow(GetDlgItem(hDlg, IDC_AJOUTCATSRC),true);
					}
				}
			}
			break;

		case IDC_AJOUTCATDEST:
			{
				if(InsertItemCatalog(GetDlgItem(hDlg, IDC_LISTCOMPARAISONDISPO),GetDlgItem(hDlg, IDC_LISTCOMPARAISON),GetDlgItem(hDlg, IDC_LISTSOURCE)))
				{

					if(!IsWindowEnabled(GetDlgItem(hDlg, IDC_SUPCATDEST)))
					{
						EnableWindow(GetDlgItem(hDlg, IDC_SUPCATDEST),true);
						EnableWindow(GetDlgItem(hDlg, IDC_SUPALLCATDEST),true);
					}

					if(ListView_GetItemCount(GetDlgItem(hDlg, IDC_LISTCOMPARAISONDISPO)) == 0)
					{
						EnableWindow(GetDlgItem(hDlg, IDC_AJOUTALLCATDEST),false);
						EnableWindow(GetDlgItem(hDlg, IDC_AJOUTCATDEST),false);
					}
				}

			}
			break;

		case IDC_AJOUTALLCATDEST:
			{
				int iNbItem = ListView_GetItemCount(GetDlgItem(hDlg, IDC_LISTCOMPARAISONDISPO));
				if(iNbItem > 0)
				{
					int j = 0;
					for(int i = 0;i < iNbItem;i++)
					{
						if(InsertItemCatalog(GetDlgItem(hDlg, IDC_LISTCOMPARAISONDISPO),GetDlgItem(hDlg, IDC_LISTCOMPARAISON),GetDlgItem(hDlg, IDC_LISTSOURCE),j) == -1)
							j++;
					}

					//SuppAllItemCatalog(GetDlgItem(hDlg, IDC_LISTCOMPARAISONDISPO));
					//InsertItemCatalog(GetDlgItem(hDlg, IDC_LISTCOMPARAISON),m_CatalogueData,GetDlgItem(hDlg, IDC_LISTSOURCE));

					EnableWindow(GetDlgItem(hDlg, IDC_AJOUTALLCATDEST),false);
					EnableWindow(GetDlgItem(hDlg, IDC_AJOUTCATDEST),false);

					if(!IsWindowEnabled(GetDlgItem(hDlg, IDC_SUPCATDEST)))
					{
						EnableWindow(GetDlgItem(hDlg, IDC_SUPCATDEST),true);
						EnableWindow(GetDlgItem(hDlg, IDC_SUPALLCATDEST),true);
					}

				}
			}
			break;

		case IDC_SUPCATDEST:
			{
				if(InsertItemCatalog(GetDlgItem(hDlg, IDC_LISTCOMPARAISON),GetDlgItem(hDlg, IDC_LISTCOMPARAISONDISPO),NULL))
				{

					if(!IsWindowEnabled(GetDlgItem(hDlg, IDC_AJOUTALLCATDEST)))
					{
						EnableWindow(GetDlgItem(hDlg, IDC_AJOUTALLCATDEST),true);
						EnableWindow(GetDlgItem(hDlg, IDC_AJOUTCATDEST),true);
					}

					if(ListView_GetItemCount(GetDlgItem(hDlg, IDC_LISTCOMPARAISON)) == 0)
					{
						EnableWindow(GetDlgItem(hDlg, IDC_SUPCATDEST),false);
						EnableWindow(GetDlgItem(hDlg, IDC_SUPALLCATDEST),false);
					}
				}

			}
			break;
		
		case IDC_SUPALLCATDEST:
			{
				if(ListView_GetItemCount(GetDlgItem(hDlg, IDC_LISTCOMPARAISON)) > 0)
				{
					SuppAllItemCatalog(GetDlgItem(hDlg, IDC_LISTCOMPARAISON));
					InsertItemCatalog(GetDlgItem(hDlg, IDC_LISTCOMPARAISONDISPO),GetDlgItem(hDlg, IDC_LISTCOMPARAISONDISPO));

					EnableWindow(GetDlgItem(hDlg, IDC_SUPCATDEST),false);
					EnableWindow(GetDlgItem(hDlg, IDC_SUPALLCATDEST),false);


					if(!IsWindowEnabled(GetDlgItem(hDlg, IDC_AJOUTALLCATDEST)))
					{
						EnableWindow(GetDlgItem(hDlg, IDC_AJOUTCATDEST),true);
						EnableWindow(GetDlgItem(hDlg, IDC_AJOUTALLCATDEST),true);
					}
				}
			}
			break;

		case IDC_BTOU:
			{
				char m_cTemp[10];
				LoadString(hInstance,IDS_OU,m_cTemp,10);
				
				m_stgChaineRechercheAff += " '";
				m_stgChaineRechercheAff += m_cTemp;
				m_stgChaineRechercheAff += "' ";
				m_stgChaineRecherche += "op=5;";
				iPointeur++;
				iTabCritere[iPointeur] = 5;

				SendMessage(GetDlgItem(hDlg, IDC_EDITRECHERCHE), WM_SETTEXT, 0, (LPARAM) m_stgChaineRechercheAff.c_str());

				AffichageCritereBouton(hDlg,iTabCritere, iPointeur);
			}
			break;

		case IDC_BTET:
			{
				iPointeur++;

				iTabCritere[iPointeur] = 4;

				char m_cTemp[10];
				LoadString(hInstance,IDS_ET,m_cTemp,10);
				
				m_stgChaineRechercheAff += " '";
				m_stgChaineRechercheAff += m_cTemp;
				m_stgChaineRechercheAff += "' ";

				m_stgChaineRecherche += "op=4;";

				SendMessage(GetDlgItem(hDlg, IDC_EDITRECHERCHE), WM_SETTEXT, 0, (LPARAM) m_stgChaineRechercheAff.c_str());

				AffichageCritereBouton(hDlg,iTabCritere, iPointeur);
			}
			break;

		case IDC_BTPARANTHESEOUV:
			{
				iPointeur++;
				iTabCritere[iPointeur] = 2;

				m_stgChaineRechercheAff += " '(' ";
				m_stgChaineRecherche += "op=2;";

				SendMessage(GetDlgItem(hDlg, IDC_EDITRECHERCHE), WM_SETTEXT, 0, (LPARAM) m_stgChaineRechercheAff.c_str());

				AffichageCritereBouton(hDlg,iTabCritere, iPointeur);
			}
			break;

		case IDC_BTPARANTHESEFERM:
			{
				iPointeur++;
				iTabCritere[iPointeur] = 3;

				m_stgChaineRechercheAff += " ')' ";
				m_stgChaineRecherche += "op=3;";

				SendMessage(GetDlgItem(hDlg, IDC_EDITRECHERCHE), WM_SETTEXT, 0, (LPARAM) m_stgChaineRechercheAff.c_str()); 

				AffichageCritereBouton(hDlg,iTabCritere, iPointeur);

			}
			break;

		case IDC_BTAJOUTCRITERE:
			{
				iPointeur++;
				iTabCritere[iPointeur] = 1;
				
				LVITEM   lvItem;
				int iItem;

				iItem = ListView_GetNextItem(GetDlgItem(hDlg, IDC_LISTCRITERE),-1,LVNI_SELECTED);

				if(iItem != -1)
				{
					char Temp[255];
					CItemInfo * pItem;
					int iPos = -1;

					lvItem.mask = LVIF_PARAM;
					lvItem.iItem = iItem;

					iItem = ListView_GetItem(GetDlgItem(hDlg, IDC_LISTCRITERE),&lvItem);

					pItem = (CItemInfo *)lvItem.lParam;

					iPos = m_stgChaineRechercheAff.find(pItem->GetParseName());

					if(iPos == -1)
					{
						StringCchPrintf(Temp,255," '%s' ",pItem->GetParseName());

						m_stgChaineRechercheAff += Temp;

						StringCchPrintf(Temp,255,"critere=%d;",pItem->GetIPos());

						m_stgChaineRecherche += Temp;

						SendMessage(GetDlgItem(hDlg, IDC_EDITRECHERCHE), WM_SETTEXT, 0, (LPARAM) m_stgChaineRechercheAff.c_str());

						AffichageCritereBouton(hDlg,iTabCritere, iPointeur);
					}
					else
					{
						CObjet::AffichageMessage(IDS_CRITEREPRESENT,IDS_INFOS);
					}
				}
				
				
			}
			break;

		case IDC_SUPCRITERE:
			{
				iPointeur--;
				int iPos = 0;
				int iOldPos = 0;

				m_stgChaineRecherche = m_stgChaineRecherche.substr(0,m_stgChaineRecherche.length() - 1);
				m_stgChaineRechercheAff = m_stgChaineRechercheAff.substr(0,m_stgChaineRechercheAff.length() - 2);

				do
				{
					iPos++;
					iPos = m_stgChaineRecherche.find(";",iPos);
					if(iPos != -1)
					{
						iOldPos = iPos-1;
					}
				}while(iPos != -1);

				if(iOldPos != 0)
				{
					m_stgChaineRecherche = m_stgChaineRecherche.substr(0,iOldPos);
				}

				do
				{
					iPos++;
					iPos = m_stgChaineRechercheAff.find("'",iPos);
					if(iPos != -1)
					{
						iOldPos = iPos;
					}
				}while(iPos != -1);							

				if(iOldPos != 0)
				{
					m_stgChaineRechercheAff = m_stgChaineRechercheAff.substr(0,iOldPos-1);
				}

				SendMessage(GetDlgItem(hDlg, IDC_EDITRECHERCHE), WM_SETTEXT, 0, (LPARAM) m_stgChaineRechercheAff.c_str());

				AffichageCritereBouton(hDlg,iTabCritere, iPointeur);
			}
			break;

		case IDRECHERCHE:
			{
				int iValid = 0;
				//Valider procédure de recherche
				if(iTabCritere[iPointeur] == 1 || iTabCritere[iPointeur] == 3)
				{
					int iNbOuverture,iNbFermeture;

					iNbOuverture = iNbFermeture = 0;

					//Vérification de la validité des parenthéses
					for(int i = 0;i <= iPointeur;i++)
					{
						if(iTabCritere[i] == 2)
							iNbOuverture++;

						if(iTabCritere[i] == 3)
							iNbFermeture++;
					}

					if(iNbOuverture == iNbFermeture && ListView_GetItemCount(GetDlgItem(hDlg, IDC_LISTCOMPARAISON)) > 0 && ListView_GetItemCount(GetDlgItem(hDlg, IDC_LISTSOURCE)) > 0)
					{
						iValid = 1;
						int i = 0;
						
						//Récupération de la liste des albums Sources et de la listes des albums Destinations
						CImageCalalogueRecherche m_catalogueRecherche;
						m_catalogueRecherche.iNbAlbumDst = ListView_GetItemCount(GetDlgItem(hDlg, IDC_LISTCOMPARAISON));
						m_catalogueRecherche.iNbAlbumSrc = ListView_GetItemCount(GetDlgItem(hDlg, IDC_LISTSOURCE));
						m_catalogueRecherche.iTabAlbumDest = new int[m_catalogueRecherche.iNbAlbumDst + 1];
						m_catalogueRecherche.iTabAlbumSrc = new int[m_catalogueRecherche.iNbAlbumSrc + 1];
						m_catalogueRecherche.m_stgChaineRecherche = m_stgChaineRecherche;
						m_catalogueRecherche.m_CatalogueData = m_CatalogueData;

						for(i = 0;i < m_catalogueRecherche.iNbAlbumDst;i++)
						{
							int iItem;
							CItemInfo * pItem;
							LVITEM lvItem;

							lvItem.mask = LVIF_PARAM;
							lvItem.iItem = i;

							iItem = ListView_GetItem(GetDlgItem(hDlg, IDC_LISTCOMPARAISON),&lvItem);

							pItem = (CItemInfo *)lvItem.lParam;

							m_catalogueRecherche.iTabAlbumDest[i] = pItem->GetIPos();
						}

						for(i = 0;i < m_catalogueRecherche.iNbAlbumSrc;i++)
						{
							int iItem;
							CItemInfo * pItem;
							LVITEM lvItem;

							lvItem.mask = LVIF_PARAM;
							lvItem.iItem = i;

							iItem = ListView_GetItem(GetDlgItem(hDlg, IDC_LISTSOURCE),&lvItem);

							pItem = (CItemInfo *)lvItem.lParam;

							m_catalogueRecherche.iTabAlbumSrc[i] = pItem->GetIPos();
						}

						CLancerRechercher * m_cLancerRecherche = new CLancerRechercher(hInstance);
						m_cLancerRecherche->SetParam(&m_catalogueRecherche);
						m_cLancerRecherche->CreateDlg(hInstance, (LPCTSTR)IDD_FICHIER, hDlg);
						m_iReturnValue = m_cLancerRecherche->GetReturnValue();
						delete m_cLancerRecherche;
						EndDialog(hDlg, m_iReturnValue);

						/*
						if(DialogBoxParam(hInstance, (LPCTSTR)IDD_FICHIER, hDlg, (DLGPROC)CImageCalalogueRecherche::LancerRechercher,(LPARAM)&m_catalogueRecherche) == 1)
						{
							m_iReturnValue = 1;
							EndDialog(hDlg, 1);
						}
						else
						{
							m_iReturnValue = 0;
							EndDialog(hDlg, 0);
						}*/

					}
					

				}

				if(!iValid)
					CObjet::AffichageMessage(IDS_CRITEREERREUR,IDS_INFOS);
				else
				{
					//Lancement de la procédure de recherche
				}

			}
			break;

		case IDOK:
			{
				m_iReturnValue = 1;
				EndDialog(hDlg, 1);
			}
			return TRUE;

		case IDCANCEL:
			{
				m_iReturnValue = 0;
				EndDialog(hDlg, 0);
			}
			return TRUE;
	}
	return 0;
}

LRESULT CRechercherCatalog::OnDestroy(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SuppAllItemCatalog(GetDlgItem(hDlg, IDC_LISTSOURCEDISPO));
	SuppAllItemCatalog(GetDlgItem(hDlg, IDC_LISTSOURCE));
	SuppAllItemCatalog(GetDlgItem(hDlg, IDC_LISTCOMPARAISONDISPO));
	SuppAllItemCatalog(GetDlgItem(hDlg, IDC_LISTCOMPARAISON));
	SuppAllItemCatalog(GetDlgItem(hDlg, IDC_LISTCRITERE));
	return 0;
}