// Catalogue1.cpp: implementation of the CCatalogue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Catalogue1.h"
#include "loadimage.h"
#include "objet.h"
#include <ItemInfo.h>
#include <CatalogueData.h>
#include <Attribut.h>
using namespace std;

#ifdef SQLSERVERCE	
#include <SqlAlbum.h>
#include <SqlAlbumAttribut.h>
#include <SqlAlbumIconeFileAttribut.h>
#include <SqlFindAlbumIconeFile.h>
#include <SqlIconeFileAlbum.h>
#include <SqlAlbumUtility.h>
using namespace Regards::Sqlite;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCatalogue::CCatalogue()
{
	m_CatalogueData = new CCatalogueData();
	m_CatalogueData->iNbAlbum = 0;
}

CCatalogue::~CCatalogue()
{
	if(m_CatalogueData != NULL)
		delete m_CatalogueData;

	m_CatalogueData = NULL;
}

//////////////////////////////////////////////////////////////////////
//Insertion d'une entrée dans l'arborescence
//////////////////////////////////////////////////////////////////////
HTREEITEM CCatalogue::InsertItem(HWND hwndParent, const char * lpszFileName)
{
    TVITEM tvi; 
    TVINSERTSTRUCT tvins; 
    HTREEITEM hPrev;

    tvi.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN; 
 

	CItemInfo * pItem = new CItemInfo();
	pItem->SetTypeDonne(m_CatalogueData->iTypeDonnee);
	tvi.lParam = (LPARAM)pItem;


   //fill in the TVITEM structure for this item

    tvi.pszText = (char *)lpszFileName; 
    tvi.cchTextMax = lstrlen(lpszFileName); 

 
    // Assume the item is not a parent item, so give it a 
    // document image. 
    tvi.iImage = m_CatalogueData->iIconeAlbum; 
    tvi.iSelectedImage = m_CatalogueData->iIconeAlbum; 
	//tvi.lParam =

	tvi.cChildren = 0;
 
    // Save the heading level in the item's application-defined 
    // data area. 
    
 
    tvins.item = tvi; 
    tvins.hInsertAfter = TVI_LAST; 
 
    // Set the parent item based on the specified level. 

	tvins.hParent = m_CatalogueData->hItemAlbum;

    // Add the item to the tree-view control. 
    hPrev = (HTREEITEM) SendMessage(hwndParent, TVM_INSERTITEM, 0, 
         (LPARAM) (LPTVINSERTSTRUCT) &tvins); 

 
	return hPrev;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//Ajout d'image à un album
/////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef SQLSERVERCE
int CCatalogue::AddImageToAlbum(const int &iNumAlbum, CIconeFile &m_IconeFile)
{
	CIconeFile m_IconeFileTemp;
	for (IconeFileVector::iterator dit = m_CatalogueData->m_AlbumDataVector[iNumAlbum].m_IconeFileVector.begin(); dit!= m_CatalogueData->m_AlbumDataVector[iNumAlbum].m_IconeFileVector.end(); dit++)
	{
		m_IconeFileTemp = *dit;
		if(_stricmp(m_IconeFileTemp.m_stgFullPathName,m_IconeFile.m_stgFullPathName) == 0)
			return -1;
	}

	//Récupération du numéro de la dernière image 

	if(m_CatalogueData->m_AlbumDataVector[iNumAlbum].iNbFiles == 0)
		m_IconeFile.lNumIcone = 0;
	else
	{
		m_IconeFileTemp = m_CatalogueData->m_AlbumDataVector[iNumAlbum].m_IconeFileVector[m_CatalogueData->m_AlbumDataVector[iNumAlbum].iNbFiles - 1];
		m_IconeFile.lNumIcone = 0;
	}

	m_IconeFile.iNbAttribut = 0;

	m_CatalogueData->m_AlbumDataVector[iNumAlbum].m_IconeFileVector.push_back(m_IconeFile);
	m_CatalogueData->m_AlbumDataVector[iNumAlbum].iNbFiles++;

	return 0;
}
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////
//Suppression d'une imagge de l'album
/////////////////////////////////////////////////////////////////////////////////////////////////

int CCatalogue::RemoveImage(const int &iNumAlbum, const int &iNumImage)
{
	m_CatalogueData->m_AlbumDataVector[iNumAlbum].iNbFiles -= iNumImage;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//Récupération d'un pointeur sur les attributs
/////////////////////////////////////////////////////////////////////////////////////////////////

AttributVector * CCatalogue::RecupPtAttribut()
{
#ifdef SQLSERVERCE

	m_CatalogueData->m_AttributVector.clear();

	//On récupère la liste des attributs pour les albums
	CSqlAlbumAttribut * m_cSqlAlbumAttribut = new CSqlAlbumAttribut();
	m_cSqlAlbumAttribut->LoadAlbumAttribut(&m_CatalogueData->m_AttributVector);
	delete m_cSqlAlbumAttribut;

#endif

	return &m_CatalogueData->m_AttributVector;

}

/////////////////////////////////////////////////////////////////////////////////////////////////
//Récupération du nombre d'image contenue dans un album
/////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef SQLSERVERCE
int CCatalogue::GetNumImage(const int &iNumAlbum, const char *szFileName)
{
	int iNumImage = 0;
	CAlbumData m_Album = m_CatalogueData->m_AlbumDataVector[iNumAlbum];
	for (IconeFileVector::iterator fit = m_Album.m_IconeFileVector.begin(); fit!=m_Album.m_IconeFileVector.end(); fit++)
	{
		CIconeFile IconeFileTemp = *fit;
		if(_stricmp(IconeFileTemp.m_stgFullPathName, szFileName) == 0)
			return iNumImage;
		else
			iNumImage++;
	}
	return -1;
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
//Récupération d'un pointeur sur une image
/////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef SQLSERVERCE
int CCatalogue::RecupPtImage(CIconeFile **m_IconeFile, const int &m_iNumImage, const int &m_iNumAlbum)
{
	*m_IconeFile = &m_CatalogueData->m_AlbumDataVector[m_iNumAlbum].m_IconeFileVector[m_iNumImage];
	return 0;
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
//Récupération d'un pointeur sur un album
/////////////////////////////////////////////////////////////////////////////////////////////////

CAlbumData * CCatalogue::RecupPtAlbum(const int &iNumAlbum)
{
	return &m_CatalogueData->m_AlbumDataVector[iNumAlbum];
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//Récupération d'un pointeur sur l'ensemble des albums
/////////////////////////////////////////////////////////////////////////////////////////////////

AlbumDataVector * CCatalogue::RecupPtVectorAlbum()
{
	return &m_CatalogueData->m_AlbumDataVector;
}	

/////////////////////////////////////////////////////////////////////////////////////////////////
//Ajout d'un album
/////////////////////////////////////////////////////////////////////////////////////////////////

int CCatalogue::AddAlbum(HWND hWndTree, CTree * m_cTree, const char *szName)
{
	CAlbumData m_AlbumTemp;
	m_AlbumTemp.iNbFiles = 0;
	
	StringCchCopy(m_AlbumTemp.m_szAlbumName,255,szName);
	m_CatalogueData->m_AlbumDataVector.push_back(m_AlbumTemp);

	InsertItem(hWndTree, szName);

	//-----------------------------------------------------------
	//Ajout de l'album dans la base de données
	//-----------------------------------------------------------

#ifdef SQLSERVERCE

	CSqlAlbumUtility::SaveAlbum(&m_AlbumTemp);

#endif

	m_CatalogueData->iNbAlbum++;
	return 0;

}

/////////////////////////////////////////////////////////////////////////////////////////////////
//Ajout d'un album
/////////////////////////////////////////////////////////////////////////////////////////////////

int CCatalogue::AddAlbum(HWND hWndTree, CTree * m_cTree, const CAlbumData &m_AlbumLocal)
{
	m_CatalogueData->m_AlbumDataVector.push_back(m_AlbumLocal);
	InsertItem(hWndTree, m_AlbumLocal.m_szAlbumName);
	//m_cTree->AddItem(hWndTree, hItemAlbum, m_AlbumLocal.m_szAlbumName);
	m_CatalogueData->iNbAlbum = m_CatalogueData->m_AlbumDataVector.size();
	return m_CatalogueData->m_AlbumDataVector.size() - 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

bool CCatalogue::asc_alpha_Image_sort(const CIconeFile &a, const CIconeFile &b)
{
	return (_stricmp(a.m_stgFullPathName, b.m_stgFullPathName) < 0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//Fonction de trie du vecteur des catalogues
/////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef SQLSERVERCE
int CCatalogue::TrieVecteur(const int &iNumAlbum)
{
	std::sort(m_CatalogueData->m_AlbumDataVector[iNumAlbum].m_IconeFileVector.begin(), m_CatalogueData->m_AlbumDataVector[iNumAlbum].m_IconeFileVector.end(), asc_alpha_Image_sort);
	return 0;
}
#endif

int CCatalogue::AddItemToTree(HWND hWndTree, CTree *m_cTree, const char * m_szLibelle)
{
	TVITEM tvi; 
	TVINSERTSTRUCT tvins; 
	ULONG pdwAttributes = SFGAO_VALIDATE;

	tvi.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN; 

	// Set the text of the item. 

	//Recherche du nom à afficher

	CItemInfo * pItem = new CItemInfo();
	pItem->SetTypeDonne(m_CatalogueData->iTypeDonnee);
	
	tvi.lParam = (LPARAM)pItem;


   //fill in the TVITEM structure for this item

	tvi.pszText = (char *)m_szLibelle; 
	tvi.cchTextMax = lstrlen(m_szLibelle); 
	tvi.cChildren = 1;

	// Assume the item is not a parent item, so give it a 
	// document image. 
	tvi.iImage = m_CatalogueData->iIconeAlbum; 
	tvi.iSelectedImage = m_CatalogueData->iIconeAlbum; 

	tvins.item = tvi; 
	tvins.hInsertAfter = TVI_LAST; 

	// Set the parent item based on the specified level. 

	tvins.hParent = TVI_ROOT;

	// Add the item to the tree-view control. 
	m_CatalogueData->hItemAlbum = (HTREEITEM) SendMessage(hWndTree, TVM_INSERTITEM, 0, 
		 (LPARAM) (LPTVINSERTSTRUCT) &tvins); 

	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//Suppression d'un album
/////////////////////////////////////////////////////////////////////////////////////////////////

int CCatalogue::SuppAlbum(HWND hWndTree, CTree *m_cTree, const int &iNumAlbum)
{
	string sName = m_CatalogueData->m_AlbumDataVector[iNumAlbum].m_szAlbumName;
	m_CatalogueData->m_AlbumDataVector.erase(m_CatalogueData->m_AlbumDataVector.begin() + iNumAlbum);

	//-----------------------------------------------------------
	//Suppression de l'album dans la base de données
	//-----------------------------------------------------------

#ifdef SQLSERVERCE

	CSqlAlbumUtility::DeleteAlbum((TCHAR *)sName.c_str());

#endif


	HTREEITEM hItem = m_CatalogueData->hItemAlbum;
	HTREEITEM hChild = TreeView_GetChild(hWndTree, hItem );

	while ( hChild ) 
	{
		//Récupération du nom de l'item
		string sValue;
		TV_ITEM tItem;
		int iTypeAllocation = 1;
		tItem.hItem = hChild;
		tItem.mask = TVIF_TEXT;
		tItem.pszText = (LPTSTR)malloc(255);
		//Test si allocation réussie ou non
		if(tItem.pszText == NULL)
		{
			//Probléme d'allocation mémoire
			//Essai d'allocation dans le tas
			iTypeAllocation = 2;
			tItem.pszText = (LPTSTR) HeapAlloc (GetProcessHeap (), 0, 255);
			if (tItem.pszText == NULL)
			{
				CObjet::ErreurInformation();
			}

		}

		
		tItem.cchTextMax = 255;
		TreeView_GetItem(hWndTree, &tItem);
		sValue = tItem.pszText;

		switch(iTypeAllocation)
		{
			case 2:
				HeapFree(GetProcessHeap (), 0,tItem.pszText);
				break;

			default:
				free(tItem.pszText);
		}

		if(sName == sValue)
		{
			TreeView_DeleteItem(hWndTree,  hChild );
			break;
		}
		
		hChild = TreeView_GetNextItem(hWndTree, hChild, TVGN_NEXT );

	}

	m_CatalogueData->iNbAlbum--;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//Modification du nom d'un album
/////////////////////////////////////////////////////////////////////////////////////////////////

int CCatalogue::NewAlbumName(const char *cNewName, const int &iNumAlbum)
{
#ifdef SQLSERVERCE

	CSqlAlbumUtility::RenameAlbum((TCHAR *)cNewName, (TCHAR *)m_CatalogueData->m_AlbumDataVector[iNumAlbum].m_szAlbumName);
	
#endif

	StringCchCopy(m_CatalogueData->m_AlbumDataVector[iNumAlbum].m_szAlbumName,255,cNewName);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//Recherche de critère propre aux images
/////////////////////////////////////////////////////////////////////////////////////////////////

int CCatalogue::FindCritere(IconeFileVector &m_IconeFileRecherche,const IconeFileVector &m_IconeFile, const int &iNumCritere, const char * cTabAttribut, int &iNbFileFind)
{
	IconeFileVector::const_iterator fit;
	AttributVector::iterator AttributIt;

	for (fit = m_IconeFile.begin(); fit!= m_IconeFile.end(); fit++)
	{
		CIconeFile IconeFileTemp = *fit;
		for (AttributIt = IconeFileTemp.m_AttributVector.begin(); AttributIt!= IconeFileTemp.m_AttributVector.end(); AttributIt++)
		{
			CAttribut m_AttributTemp;
			m_AttributTemp = *AttributIt;

			//Test si l'attribut existe dans l'image
			if(m_AttributTemp.iNumAttribut == iNumCritere)
			{
				int iResult;
				string m_stgLibelle = m_AttributTemp.cLibelle;
				iResult = m_stgLibelle.find(cTabAttribut,0);

				if(iResult >= 0)
				{
					iNbFileFind++;
					m_IconeFileRecherche.push_back(IconeFileTemp);
				}

				break;
			}
		}
		
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//Recherche de critère spécifique aux propriétés du fichier
/////////////////////////////////////////////////////////////////////////////////////////////////
int CCatalogue::FindCritereFichier(IconeFileVector &m_IconeFileRecherche,const IconeFileVector &m_IconeFile, const int &iNumCritere, const char * cTabAttribut, int &iNbFileFind)
{
	IconeFileVector::const_iterator fit;
	CObjet m_cObjet;

	for (fit = m_IconeFile.begin(); fit!= m_IconeFile.end(); fit++)
	{
		CIconeFile IconeFileTemp = *fit;
		switch(iNumCritere)
		{
			case 11:
				//Format de fichier
				{
					int iValue = atoi(cTabAttribut);
					if(m_cObjet.TestImageFormat(IconeFileTemp.m_stgFullPathName) == iValue  || m_cObjet.TestVideoFormat(IconeFileTemp.m_stgFullPathName) == iValue)
					{
						m_IconeFileRecherche.push_back(IconeFileTemp);
						iNbFileFind++;
					}
					
				}
				break;

			case 12:
				//Taille du fichier supérieur
				{
						//Taille du fichier inférieur
						long m_lSize = atol(IconeFileTemp.szSize);
						long m_lTailleFichier = atol(cTabAttribut);

						if(m_lSize >= m_lTailleFichier)
						{
							m_IconeFileRecherche.push_back(IconeFileTemp);
							iNbFileFind++;
						}
				}
				break;

			case 13:
				{
						//Taille du fichier inférieur
						long m_lSize = atol(IconeFileTemp.szSize);
						long m_lTailleFichier = atol(cTabAttribut);

						if(m_lSize <= m_lTailleFichier)
						{
							m_IconeFileRecherche.push_back(IconeFileTemp);
							iNbFileFind++;
						}
				}
				break;

			case 15:
				{
						//Width du fichier
						UINT width;
						UINT height;
						long m_lWdithFichier = atol(cTabAttribut);
						CLoadImage m_cLoadImage;

						m_cLoadImage.GetDimensionImageStatic(IconeFileTemp.m_stgFullPathName,&width,&height);

						if(width == m_lWdithFichier)
						{
							m_IconeFileRecherche.push_back(IconeFileTemp);
							iNbFileFind++;
						}
				}
				break;

			case 16:
				{
						//Height du fichier
						long m_lHeightFichier = atol(cTabAttribut);
						UINT width;
						UINT height;
						CLoadImage m_cLoadImage;
						m_cLoadImage.GetDimensionImageStatic(IconeFileTemp.m_stgFullPathName,&width,&height);

						if(height == m_lHeightFichier)
						{
							m_IconeFileRecherche.push_back(IconeFileTemp);
							iNbFileFind++;
						}
				}
				break;


			case 14:
				{
					//Transformation en AnneeMoisJour de la date pour comparaison
					string m_stgValue = IconeFileTemp.szTimeCreate;
					string m_stgValueTemp = cTabAttribut;
					char cTempValue[20];
					int i = m_stgValueTemp.find("-");
					long m_lValue;
					long m_lValueDateDe = atol(m_stgValueTemp.substr(0,i).c_str());
					long m_lValueDateA = atol(m_stgValueTemp.substr(i+1,m_stgValueTemp.length() - i - 1).c_str());
					StringCchPrintf(cTempValue,20,"%s%s%s",m_stgValue.substr(6,4).c_str(),m_stgValue.substr(3,2).c_str(),m_stgValue.substr(0,2).c_str());
					m_lValue = atol(cTempValue);

					if(m_lValueDateDe > 0 && m_lValueDateA > 0)
					{
						if(m_lValue >= m_lValueDateDe && m_lValue <= m_lValueDateA)
						{
							m_IconeFileRecherche.push_back(IconeFileTemp);
							iNbFileFind++;
						}
					}
					else
					{
						if(m_lValueDateDe > 0 && m_lValue >= m_lValueDateDe)
						{
							m_IconeFileRecherche.push_back(IconeFileTemp);
							iNbFileFind++;
						}
						else if(m_lValue <= m_lValueDateA && m_lValueDateA > 0)
						{
							m_IconeFileRecherche.push_back(IconeFileTemp);
							iNbFileFind++;
						}

					}
					
				}
				break;
		}
	}

	return 0;
}

int CCatalogue::ConstructionRequeteSQL(const char *szStringToFind, string &stgRequeteSQLCriteria, string &stgRequeteSQL, int &iNumFormat)
{
	int iTabNumAttribut[20];
	char cTabAttribut[20][255];
	int i = 0, iNbAttribut = 0;
	string szFind = szStringToFind;
	char m_cSpecialCharacter[] = "%";
	string m_stgSpecialLiaisonCharacter = "OR";

	for(i=0;i < 11;i++)
	{
		if(ParcoursString(iTabNumAttribut[iNbAttribut], cTabAttribut[iNbAttribut],255, szFind))
			iNbAttribut++;
	}


	//Test si recherche par OU ou ET
	i = szFind.find("et=1",0);
	if(i >= 0)
		m_stgSpecialLiaisonCharacter = "AND";

	//Recherche par OU
	//On extrait l'ensemble des critères pour l'exécuter en une seule requête SQL
	for(i= 0;i < iNbAttribut;i++)
	{
		if(iTabNumAttribut[i] <= 10)
		{
			char temp[255];
			sprintf_s(temp,"(NumAttribut = %d and ValueAttribut like '%s%s%s') %s ",iTabNumAttribut[i],m_cSpecialCharacter,cTabAttribut[i],m_cSpecialCharacter, m_stgSpecialLiaisonCharacter.c_str());
			stgRequeteSQLCriteria += temp;
		}
		else
		{
			switch(iTabNumAttribut[i])
			{
			case 11:
				//Format de fichier
				iNumFormat = atoi(cTabAttribut[i]);
				break;

				case 12:
					//Taille du fichier supérieur
					{
						char temp[255];
						long m_lTailleFichier = atol(cTabAttribut[i]);
						sprintf_s(temp,"ImageSize > %d %s ",m_lTailleFichier,m_stgSpecialLiaisonCharacter.c_str());
						stgRequeteSQL += temp;
					}
					break;

				case 13:
					{
						char temp[255];
						long m_lTailleFichier = atol(cTabAttribut[i]);
						sprintf_s(temp,"ImageSize < %d %s ",m_lTailleFichier,m_stgSpecialLiaisonCharacter.c_str());
						stgRequeteSQL += temp;
					}
					break;

				case 15:
					{
						char temp[255];
						long m_lWdithFichier = atol(cTabAttribut[i]);
						sprintf_s(temp,"ImageWith = %d %s ",m_lWdithFichier,m_stgSpecialLiaisonCharacter.c_str());
						stgRequeteSQL += temp;
					}
					break;

				case 16:
					{
						char temp[255];
						long m_lHeightFichier = atol(cTabAttribut[i]);
						sprintf_s(temp,"ImageHeight = %d %s ",m_lHeightFichier,m_stgSpecialLiaisonCharacter.c_str());
						stgRequeteSQL += temp;

					}
					break;


				case 14:
					{
						char temp[255];
						//Transformation en AnneeMoisJour de la date pour comparaison
						string m_stgValueTemp = cTabAttribut[i];
						char cTempValue[20];
						int i = m_stgValueTemp.find("-");
						long m_lValueDateDe = atol(m_stgValueTemp.substr(0,i).c_str());
						long m_lValueDateA = atol(m_stgValueTemp.substr(i+1,m_stgValueTemp.length() - i - 1).c_str());
						sprintf_s(temp,"(ImageDate >= %d and ImageDate <= %d) %s ",m_lValueDateDe,m_lValueDateA,m_stgSpecialLiaisonCharacter.c_str());
						stgRequeteSQL += temp;
					
					}
					break;
			}
		}
	}

	if(stgRequeteSQL.length() > 0)
		stgRequeteSQL = stgRequeteSQL.substr(0,stgRequeteSQL.length() - (m_stgSpecialLiaisonCharacter.length()+1));

	if(stgRequeteSQLCriteria.length() > 0)
		stgRequeteSQLCriteria = stgRequeteSQLCriteria.substr(0,stgRequeteSQLCriteria.length() - (m_stgSpecialLiaisonCharacter.length()+1));
	return 0;

}

/////////////////////////////////////////////////////////////////////////////////////////////////
//Procédure de Recherche d'Image dans le vecteur de stockage de donnée
//Pointeur vers un vecteur en sortie
/////////////////////////////////////////////////////////////////////////////////////////////////

int CCatalogue::RechercheImage(IconeFileVector * * m_IconeFileRechercheLocal, const char *szStringToFind, HWND hWndPrgBar)
{
#ifdef SQLSERVERCE

	int m_iNumFormat = -1;
	string m_stgRequeteSQL = "";
	string m_stgRequeteSQLAttribut = "";
	IconeFileVector m_iconeFileVector;
	CObjet m_cObjet;

	int iNbFileFind = 0;

	ShowWindow(hWndPrgBar,SW_SHOW);

	//Travail sur la chaine de recherche
	string szFind = szStringToFind;

	m_CatalogueData->m_IconeFileRecherche.clear();

	ConstructionRequeteSQL(szStringToFind,m_stgRequeteSQLAttribut,m_stgRequeteSQL,m_iNumFormat);


	//Execution de la requete SQL
	CSqlFindAlbumIconeFile * m_cSqlFindAlbumIconeFile = new CSqlFindAlbumIconeFile();
	int i = szFind.find("et=1",0);
	if(i >= 0)
		m_cSqlFindAlbumIconeFile->SearchIconeFileAlbum(&m_CatalogueData->m_IconeFileRecherche,(TCHAR *) m_stgRequeteSQLAttribut.c_str(), (TCHAR *)m_stgRequeteSQL.c_str(), (TCHAR*)"AND");
	else
		m_cSqlFindAlbumIconeFile->SearchIconeFileAlbum(&m_CatalogueData->m_IconeFileRecherche, (TCHAR *)m_stgRequeteSQLAttribut.c_str(), (TCHAR *)m_stgRequeteSQL.c_str(), (TCHAR*)"OR");

	delete m_cSqlFindAlbumIconeFile;


	iNbFileFind = m_CatalogueData->m_IconeFileRecherche.size();

	m_iconeFileVector.clear();

	//On supprime les images en doublon
	for (IconeFileVector::iterator IconeFileIt = m_CatalogueData->m_IconeFileRecherche.begin(); IconeFileIt!= m_CatalogueData->m_IconeFileRecherche.end(); IconeFileIt++)
	{
		CIconeFile m_IconeFile= *IconeFileIt;
		IconeFileVector::iterator m_IconeFileFind = find(m_iconeFileVector.begin(),m_iconeFileVector.end(),m_IconeFile);
		if(m_IconeFileFind == m_iconeFileVector.end())
		{
			if(m_iNumFormat != -1)
			{
				if(m_cObjet.TestImageFormat(m_IconeFile.m_stgFullPathName) == m_iNumFormat  || m_cObjet.TestVideoFormat(m_IconeFile.m_stgFullPathName) == m_iNumFormat)
					m_iconeFileVector.push_back(m_IconeFile);				
			}
			else
			{
				m_iconeFileVector.push_back(m_IconeFile);
			}
		}
	}

	m_CatalogueData->m_IconeFileRecherche.clear();
	m_CatalogueData->m_IconeFileRecherche = m_iconeFileVector;

	if(iNbFileFind > 0)
	{
		*m_IconeFileRechercheLocal = &m_CatalogueData->m_IconeFileRecherche;
		ShowWindow(hWndPrgBar,SW_HIDE);

		return 1;
	}
	else
		CObjet::AffichageMessage(IDS_ALBUMIMAGECRITERE,IDS_INFOS);

	ShowWindow(hWndPrgBar,SW_HIDE);

#else

	AlbumDataVector::const_iterator dit;
	int iTabNumAttribut[20];
	char cTabAttribut[20][255];

	//TypeRecherche :
	//0 : recherche par ou
	//1 : recherche par et

	int i, iTrouve = 0, iNbFileFind = 0, iNbAttribut = 0, iTypeRecherche = 0;//, iBoucle;

	ShowWindow(hWndPrgBar,SW_SHOW);



	//Travail sur la chaine de recherche
	string szFind = szStringToFind;

	m_CatalogueData->m_IconeFileRecherche.clear();

	for(i=0;i < 11;i++)
	{
		if(ParcoursString(iTabNumAttribut[iNbAttribut], cTabAttribut[iNbAttribut],255, szFind))
			iNbAttribut++;
	}

	//Test si recherche par ou ou et
	i = szFind.find("et=1",0);
	if(i >= 0)
		iTypeRecherche = 1;

	SendMessage(hWndPrgBar, PBM_SETRANGE, 0, MAKELPARAM(0,iNbAttribut * m_CatalogueData->m_AlbumDataVector.size())); 
	SendMessage(hWndPrgBar, PBM_SETSTEP, (WPARAM) 1, 0); 


	if(iTypeRecherche == 0)
	{
		//Recherche par ou
		for(i= 0;i < iNbAttribut;i++)
		{
			for (dit = m_CatalogueData->m_AlbumDataVector.begin(); dit!= m_CatalogueData->m_AlbumDataVector.end(); dit++)
			{
				SendMessage(hWndPrgBar, PBM_STEPIT, 0, 0); 
				CAlbumData m_AlbumTemp = *dit;
				if(iTabNumAttribut[i] > 10)
					FindCritereFichier(m_CatalogueData->m_IconeFileRecherche,m_AlbumTemp.m_IconeFileVector, iTabNumAttribut[i], cTabAttribut[i],iNbFileFind);
				else
					FindCritere(m_CatalogueData->m_IconeFileRecherche,m_AlbumTemp.m_IconeFileVector, iTabNumAttribut[i], cTabAttribut[i],iNbFileFind);
			}
		}
	}
	else
	{
		IconeFileVector m_VecteurTemp1;
		IconeFileVector m_VecteurTemp2;
		//Recherche par et
		for(i= 0;i < iNbAttribut;i++)
		{
			iNbFileFind = 0;
			if(i == 0)
			{
				//Trie par rapport au premier attribut
				for (dit = m_CatalogueData->m_AlbumDataVector.begin(); dit!= m_CatalogueData->m_AlbumDataVector.end(); dit++)
				{
					SendMessage(hWndPrgBar, PBM_STEPIT, 0, 0);
					CAlbumData m_AlbumTemp = *dit;
					if(iTabNumAttribut[i] > 10)
						FindCritereFichier(m_VecteurTemp1,m_AlbumTemp.m_IconeFileVector, iTabNumAttribut[i], cTabAttribut[i],iNbFileFind);
					else
						FindCritere(m_VecteurTemp1,m_AlbumTemp.m_IconeFileVector, iTabNumAttribut[i], cTabAttribut[i],iNbFileFind);
				}

				continue;
			}

			//Recherche à partir 
			if(iTabNumAttribut[i] > 10)
				FindCritereFichier(m_VecteurTemp2,m_VecteurTemp1, iTabNumAttribut[i], cTabAttribut[i],iNbFileFind);
			else
				FindCritere(m_VecteurTemp2,m_VecteurTemp1, iTabNumAttribut[i], cTabAttribut[i],iNbFileFind);	
			//Copie des données de 2 vers 1
			m_VecteurTemp1 = m_VecteurTemp2;
			m_VecteurTemp2.clear();
		}
		m_CatalogueData->m_IconeFileRecherche = m_VecteurTemp1;
	}

	if(iNbFileFind > 0)
	{
		*m_IconeFileRechercheLocal = &m_CatalogueData->m_IconeFileRecherche;
		ShowWindow(hWndPrgBar,SW_HIDE);

		return 1;
	}
	else if(iNbAttribut == 0)
	{
		CObjet::AffichageMessage(IDS_ALBUMCRITERE,IDS_INFOS);
	}
	else
	{
		CObjet::AffichageMessage(IDS_ALBUMIMAGECRITERE,IDS_INFOS);
	}

	ShowWindow(hWndPrgBar,SW_HIDE);

#endif


	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

int CCatalogue::ParcoursString(int &iNum, char *cValue, int iSize, string &szFind)
{
	int iValue = szFind.find("=",0);
	iNum = atoi(szFind.substr(0,iValue).c_str());
	szFind = szFind.substr(iValue + 1,szFind.length() - iValue - 1);
	iValue = szFind.find(";",0);
	StringCchCopy(cValue,iSize,szFind.substr(0,iValue).c_str());

	szFind = szFind.substr(iValue + 1,szFind.length() - iValue - 1);

	if(iNum == -1)
		return 0;
	else
		return 1;

}

/////////////////////////////////////////////////////////////////////
//Suppression des images d'un album
/////////////////////////////////////////////////////////////////////
#ifndef SQLSERVERCE
int CCatalogue::DeleteAlbumFile(const int &iNumAlbum, HWND &hWnd,VECTITEM * m_vectItem)
{
	char clpText[255];
	char Temp[255];
	char clpCaption[255];
	int iNbCount,iNbEncours;
	iNbEncours = 0;

	//iNbCount = ListView_GetSelectedCount(hWnd);

	iNbCount = m_vectItem->size();

	LoadString(hInstance,IDS_SUPPIMAGE,clpText,255);
	LoadString(hInstance,IDS_INFOS,clpCaption,255);
	StringCchPrintf(Temp,255,clpText,iNbCount);
	if(MessageBox(NULL,Temp,clpCaption,MB_YESNO | MB_ICONWARNING) == IDYES)
	{
		for(int i = 0;i < iNbCount;i++)
		{

			//IconeFile * m_IconeFile;
			CIconeFile m_IconeFileTemp;

			IconeFileVector::iterator m_IconeFile;

			int nItem = m_vectItem->at(i);
						
			CItemInfo * pItem;
			LVITEM   lvItem;

			lvItem.mask = LVIF_PARAM;
			lvItem.iItem = nItem;

			ListView_GetItem(hWnd,&lvItem);

			pItem = (CItemInfo *)lvItem.lParam;

			StringCchCopy(m_IconeFileTemp.m_stgFullPathName,512,pItem->GetParseName());

			m_IconeFile = find(m_CatalogueData->m_AlbumDataVector[iNumAlbum].m_IconeFileVector.begin(),m_CatalogueData->m_AlbumDataVector[iNumAlbum].m_IconeFileVector.end(),m_IconeFileTemp);

			m_CatalogueData->m_AlbumDataVector[iNumAlbum].m_IconeFileVector.erase(m_IconeFile);

			//ListView_DeleteItem(hWnd, nItem);
		}

		int nItem = m_vectItem->at(0);
		for(int i = 0;i < iNbCount;i++)
		{
			
			ListView_DeleteItem(hWnd, nItem);
		}

		m_CatalogueData->m_AlbumDataVector[iNumAlbum].iNbFiles -= iNbCount;

		m_vectItem->clear();
	}

	SendMessage(hWnd,LVM_ARRANGE,0,0);

	return iNbCount;
}
#endif
/////////////////////////////////////////////////////////////////////////
//Vérifie si le nom de l'album existe déjà
/////////////////////////////////////////////////////////////////////////

int CCatalogue::VerifNomAlbum(const char *szAlbumName)
{
	string Temp = szAlbumName;
	int iNumAlbum = 0;
		
	for (AlbumDataVector::const_iterator dit = m_CatalogueData->m_AlbumDataVector.begin(); dit!= m_CatalogueData->m_AlbumDataVector.end(); dit++)
	{
		CAlbumData m_AlbumTemp;
		m_AlbumTemp = *dit;
		if(_stricmp(Temp.c_str(),m_AlbumTemp.m_szAlbumName) == 0)
			return iNumAlbum;

		iNumAlbum++;
	}

	return -2;
}
