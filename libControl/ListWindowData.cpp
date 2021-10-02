#include "stdafx.h"
#include ".\listwindowdata.h"
#include "tree.h"
#include "statusbar.h"
#include "catalogue1.h"
#include "WebBrowser.h"
#include "AlbumWizard.h"
#include "ImageCatalogue.h"
#include "Album.h"
#include <AttributFichier.h>
#include <dialogcataloguedata.h>
#include <GestionCategorie.h>
#include <ItemInfo.h>
#include "objet.h"
#ifdef SQLSERVERCE	
#include <SqlAlbumAttribut.h>
#include <SqlIconeFileAlbum.h>
#include <SqlAlbumIconeFileAttribut.h>
using namespace Regards::Sqlite;
#endif

CListWindowData::CListWindowData(void)
{
	m_CAlbumCatalogue = NULL;
	m_CImageCatalogue = NULL;
	classTree = NULL;
	g_pcm2 = NULL;
	m_CWebBrowser = NULL;

	cFile[0] = '\0';
	cDirectory[0] = '\0';
	cDirectoryTemp[0] = '\0';

	g_fDragging = 0;
	iListDragging = 0;

	himl = NULL;    // handle to image list 
	m_IconeFileVector = NULL;
	m_bDblClick = false;
	m_Album = NULL;
	m_ItemOld = NULL;

	iNumAlbum = 0;
	iInitializeFolder = 0;
	iRecherche = 0;
	g_pMalloc = NULL;

}

CListWindowData::~CListWindowData(void)
{
	if(m_CAlbumCatalogue != NULL)
		delete m_CAlbumCatalogue;

	if(classTree != NULL)
		delete classTree;

	if(m_CWebBrowser != NULL)
		delete m_CWebBrowser;

	if(m_CImageCatalogue != NULL)
		delete m_CImageCatalogue;

	m_CAlbumCatalogue = NULL;
	m_CImageCatalogue = NULL;
	m_CWebBrowser = NULL;
	classTree = NULL;

#ifdef SQLSERVERCE	
	m_IconeFileVectorAlbum.clear();
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//Mises à jour dans la base de données des catégories
///////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CListWindowData::UpdateCategorie(AttributVector * attributVectorAlbum)
{

#ifdef SQLSERVERCE	
	//Comparaison de la liste des attributs avant et aprés
	CSqlAlbumIconeFileAttribut * m_cSqlAlbumIconeFileAttribut = new CSqlAlbumIconeFileAttribut();
	CSqlAlbumAttribut * m_cSqlAlbumAttribut = new CSqlAlbumAttribut();
	AttributVector m_AttributVectorInDatabase;
	AttributVector m_AttributVectorNewItem;

	m_cSqlAlbumAttribut->LoadAlbumAttribut(&m_AttributVectorInDatabase);

	int _NumLast = 0;
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//Vérification des attributs ayant été supprimés
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	for (AttributVector::iterator AttributIt = m_AttributVectorInDatabase.begin(); AttributIt!= m_AttributVectorInDatabase.end(); AttributIt++)
	{
		CAttribut m_AttributDatabaseTemp;
		m_AttributDatabaseTemp = *AttributIt;
		bool m_bFind = false;
		//m_IconeFile = find(m_AttributDatabaseTemp.begin(),m_AttributDatabaseTemp.end(),m_AttributAlbumTemp);

		//Vérification des attributs ayant été ajoutés
		for (AttributVector::iterator AttributItNew = attributVectorAlbum->begin(); AttributItNew!= attributVectorAlbum->end(); AttributItNew++)
		{
			CAttribut m_AttributAlbumTemp;
			m_AttributAlbumTemp = *AttributItNew;		

			if(strcmp(m_AttributDatabaseTemp.cLibelle,m_AttributAlbumTemp.cLibelle) == 0)
			{
				m_bFind = true;
				break;
			}
		}

		_NumLast = m_AttributDatabaseTemp.iNumAttribut;

		if(!m_bFind)
		{
			m_cSqlAlbumAttribut->DeleteAlbumAttribut(m_AttributDatabaseTemp.iNumAttribut);
			m_cSqlAlbumIconeFileAttribut->DeleteAlbumIconeFileAttribut(m_AttributDatabaseTemp.iNumAttribut);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//On vérifie les éléments ayant été ajoutés
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	for (AttributVector::iterator AttributIt = attributVectorAlbum->begin(); AttributIt!= attributVectorAlbum->end(); AttributIt++)
	{
		CAttribut m_AttributAlbumTemp;
		m_AttributAlbumTemp = *AttributIt;
		bool m_bFind = false;

		for (AttributVector::iterator AttributItNew = m_AttributVectorInDatabase.begin(); AttributItNew!= m_AttributVectorInDatabase.end(); AttributItNew++)
		{
			CAttribut m_AttributDatabaseTemp;
			m_AttributDatabaseTemp = *AttributItNew;

			if(strcmp(m_AttributDatabaseTemp.cLibelle,m_AttributAlbumTemp.cLibelle) == 0)
			{
				m_bFind = true;
				break;
			}
		}

		if(!m_bFind)
			m_AttributVectorNewItem.push_back(m_AttributAlbumTemp);
	}
	
	if(m_AttributVectorNewItem.size() > 0)
		m_cSqlAlbumAttribut->SaveAlbumAttribut(&m_AttributVectorNewItem);

	delete m_cSqlAlbumIconeFileAttribut;
	delete m_cSqlAlbumAttribut;

	m_AttributVectorNewItem.clear();
	m_AttributVectorInDatabase.clear();
#endif
	return S_OK;
}

//----------------------------------------------------------------------------------------
//Gestion des catégories
//----------------------------------------------------------------------------------------
LRESULT CListWindowData::GestionDesCategories(HWND hWnd)
{
#ifdef SQLSERVERCE	

	CDialogCatalogueData m_dialogData;
	AttributVector m_AttributVectorAlbum;


	CSqlAlbumAttribut * m_cSqlAlbumAttribut = new CSqlAlbumAttribut();
	m_cSqlAlbumAttribut->LoadAlbumAttribut(&m_AttributVectorAlbum);

	m_dialogData.m_AttributVector = &m_AttributVectorAlbum;
	CGestionCategorie * m_cGestionCategorie = new CGestionCategorie(hInstance);
	m_cGestionCategorie->SetParam(&m_dialogData);
	m_cGestionCategorie->CreateDlg(hInstance, (LPCTSTR)IDD_GSTCATEGORIE, hWnd);
	if(m_cGestionCategorie->GetReturnValue())
	{
		UpdateCategorie(&m_AttributVectorAlbum);
	}

	m_AttributVectorAlbum.clear();

	
	delete m_cGestionCategorie;
	delete m_cSqlAlbumAttribut;



#else
	//Gestion des catégories
	//Pointeur sur les attributs
	CDialogCatalogueData m_dialogData;
	AttributVector * m_AttributVector;
	m_AttributVector = m_CAlbumCatalogue->RecupPtAttribut();
	m_dialogData.m_AttributVector = m_AttributVector;
	CGestionCategorie * m_cGestionCategorie = new CGestionCategorie(hInstance);
	m_cGestionCategorie->SetParam(&m_dialogData);
	m_cGestionCategorie->CreateDlg(hInstance, (LPCTSTR)IDD_GSTCATEGORIE, hWnd);
	delete m_cGestionCategorie;
#endif

	return S_OK;
}

//----------------------------------------------------------------------------------------
//Gestion des attributs pour une image
//----------------------------------------------------------------------------------------
LRESULT CListWindowData::GestionDesAttributs(HWND hWnd, const char * szFileName, HWND hWndParent)
{
#ifdef SQLSERVERCE	

	char _szAlbumName[MAX_PATH];
	classTree->TreeViewGetSelectName(_szAlbumName,MAX_PATH);
	int iNumAlbum = m_CAlbumCatalogue->DetermineNumAlbum(_szAlbumName);
	if(iNumAlbum != -1)
	{
		CAlbumData * m_AlbumLocal = m_CAlbumCatalogue->RecupPtAlbum(iNumAlbum);

		CDialogCatalogueData m_dialogData;

		/*
		size_t m_sizeTConvert;
		wchar_t _wAlbumName[MAX_PATH];
		wchar_t _wFileName[MAX_PATH];
		mbstowcs_s(&m_sizeTConvert,_wAlbumName, MAX_PATH, m_AlbumLocal->m_szAlbumName, MAX_PATH);
		mbstowcs_s(&m_sizeTConvert,_wFileName, MAX_PATH, szFileName, MAX_PATH);
		*/
		//Vérifie si l'album existe
		//On récupère les attributs pour un album
		AttributVector m_AttributVectorAlbum;
		CSqlAlbumAttribut * m_cSqlAlbumAttribut = new CSqlAlbumAttribut();
		m_cSqlAlbumAttribut->LoadAlbumAttribut(&m_AttributVectorAlbum);
		delete m_cSqlAlbumAttribut;

		//On récupère les attributs pour l'image
		AttributVector m_AttributVectorPicture;
		CSqlAlbumIconeFileAttribut * m_cSqlAlbumIconeFileAttribut = new CSqlAlbumIconeFileAttribut();
		m_cSqlAlbumIconeFileAttribut->LoadAlbumIconeFileAttribut(&m_AttributVectorPicture, m_AlbumLocal->m_szAlbumName, (TCHAR *)szFileName);
		delete m_cSqlAlbumIconeFileAttribut;

		m_dialogData.m_AttributVector = &m_AttributVectorAlbum;
		m_dialogData.m_AttributVectorPicture = &m_AttributVectorPicture;
		m_dialogData.m_IconeFile = new CIconeFile();
		strcpy_s(m_dialogData.m_IconeFile->m_stgFullPathName, 512, szFileName);
		strcpy_s(m_dialogData.m_IconeFile->szKeyName, MAX_PATH, m_AlbumLocal->m_szAlbumName);

		CAttributFichier * m_cAttributFichier = new CAttributFichier(hInstance);
		m_cAttributFichier->SetParam(&m_dialogData);
		m_cAttributFichier->CreateDlg(hInstance, (LPCTSTR)IDD_ATTRIBUTIMAGE, hWnd);
		if(m_cAttributFichier->GetReturnValue())
		{
			//Update des catégories si besoin
			UpdateCategorie(&m_AttributVectorAlbum);

			if(m_AttributVectorPicture.size() > 0)
			{
				//On sauvegarde les attributs de l'image
				CSqlAlbumIconeFileAttribut * m_cSqlAlbumIconeFileAttribut = new CSqlAlbumIconeFileAttribut();
				m_cSqlAlbumIconeFileAttribut->DeleteAlbumIconeFileAttribut(m_AlbumLocal->m_szAlbumName, (TCHAR *)szFileName);
				m_cSqlAlbumIconeFileAttribut->SaveAlbumIconeFileAttribut(&m_AttributVectorPicture, m_AlbumLocal->m_szAlbumName, (TCHAR *)szFileName);
				delete m_cSqlAlbumIconeFileAttribut;
			}
			
			SendMessage(hWndParent,WM_COMMAND,MAKEWPARAM(ID_AFFICHAGE_CONTROLE_INFOSFICHIER,99),0);
		}
		delete m_cAttributFichier;
	}

#else

	char Temp[MAX_PATH];
	int iNumAlbum;

	//Récupération du numero de l'album
	classTree->TreeViewGetSelectName(Temp,MAX_PATH);
	iNumAlbum = m_CAlbumCatalogue->DetermineNumAlbum(Temp);

	if(iNumAlbum != -1)
	{
		int iNumImage;
		CDialogCatalogueData m_dialogData;
		CIconeFile * m_IconeFile;
		//Récupération d'un pointeur sur la liste des attributs (VecteurAttribut)
		AttributVector * m_AttributVector;
		m_AttributVector = m_CAlbumCatalogue->RecupPtAttribut();
		//CDialog::SetAttributVector(m_AttributVector);

		m_dialogData.m_AttributVector = m_AttributVector;

		iNumImage = m_CAlbumCatalogue->GetNumImage(iNumAlbum, szFileName);
		
		if(iNumImage != -1)
		{
			//Récupération d'un pointeur sur l'entrée de l'image (IconeFile)
			m_CAlbumCatalogue->RecupPtImage(&m_IconeFile, iNumImage, iNumAlbum);
			//CDialog::SetIconeFilePt(m_IconeFile);

			m_dialogData.m_IconeFile = m_IconeFile;

			//DialogBoxParam(hInstance, (LPCTSTR)IDD_ATTRIBUTIMAGE, hWnd, (DLGPROC)CDialog::AttributFichier,(LPARAM)&m_dialogData);
			CAttributFichier * m_cAttributFichier = new CAttributFichier(hInstance);
			m_cAttributFichier->SetParam(&m_dialogData);
			m_cAttributFichier->CreateDlg(hInstance, (LPCTSTR)IDD_ATTRIBUTIMAGE, hWnd);
			delete m_cAttributFichier;
			//classTree.SaveAlbum();

			SendMessage(hWndParent,WM_COMMAND,MAKEWPARAM(ID_AFFICHAGE_CONTROLE_INFOSFICHIER,99),0);
		}
	}
#endif
	return S_OK;
}

//----------------------------------------------------------------------------------------
//Création d'un album
//----------------------------------------------------------------------------------------
LRESULT CListWindowData::CreateNewAlbum(const char * albumName)
{
	//Vérifie si le nom de l'album existe ou non
	if(m_CAlbumCatalogue->VerifNomAlbum(albumName) == -2)
		m_CAlbumCatalogue->AddAlbum(classTree->m_hWnd, classTree, albumName);
	else
		CObjet::AffichageMessage(IDS_ALBUMEXISTANT,IDS_INFOS);

	return S_OK;
}

//----------------------------------------------------------------------------------------
//Suppression d'un album
//----------------------------------------------------------------------------------------
LRESULT CListWindowData::DeleteAlbum(HWND hWndParent)
{
	char Temp[MAX_PATH];
	char clpText[255];
	char clpCaption[255];

	LoadString(hInstance,IDS_SUPPALBUM,clpText,255);
	LoadString(hInstance,IDS_INFOS,clpCaption,255);

	//Récupération du numéro de l'album
	classTree->TreeViewGetSelectName(Temp,MAX_PATH);
	iNumAlbum = m_CAlbumCatalogue->DetermineNumAlbum(Temp);
	//Affichage d'une boite de dialogue demandant la confirmation de la suppression
	if(MessageBox(NULL,clpText,clpCaption,MB_YESNO | MB_ICONWARNING) == IDYES)
	{
		m_CAlbumCatalogue->SuppAlbum(classTree->m_hWnd, classTree, iNumAlbum);
		SendMessage(hWndParent,WM_COMMAND,MAKEWPARAM(IDM_INITDIRECTORY,0),(LPARAM)"Album");
	}
	return S_OK;
}

//----------------------------------------------------------------------------------------
//Initialisation du répertoire
//----------------------------------------------------------------------------------------
LRESULT CListWindowData::InitializeFolder(HWND hWnd,const char * szFolder, HWND hWndParent)
{
	//Récupération de la sélection
	int i = 0;
	string m_szRep;
	string sValue;
	string m_stgCRep = "C:";
	HTREEITEM hTreeCItem;
	HTREEITEM hTreeItem;

	m_szRep = szFolder;
	sValue = szFolder;


	hTreeItem = TreeView_GetRoot(classTree->m_hWnd);

	iInitializeFolder = 1;			

	do
	{
		BOOL m_bSpecialFolder;
		if(hTreeItem == NULL)
			break;

		sValue = classTree->GetItemText(hTreeItem);

		if(sValue == m_stgCRep)
			hTreeCItem = hTreeItem;

		char SpecialFolder[MAX_PATH];

		m_bSpecialFolder = classTree->SpecialFolderToPath(sValue.c_str(),SpecialFolder,MAX_PATH);
		if(m_bSpecialFolder)
			sValue = SpecialFolder;

		if(sValue.length() <= m_szRep.length())
		{
			string m_stgTemp = m_szRep.substr(0,sValue.length());
			char * m_cTemp = _strdup(m_stgTemp.c_str());
			char * m_cValue = _strdup(sValue.c_str());

			_strlwr_s(m_cTemp,strlen(m_cTemp)+1);
			_strlwr_s(m_cValue,strlen(m_cValue)+1);
	
			
			string m_stgTemp2 = m_cTemp;
			string m_stgTemp3 = m_cValue;

			free(m_cTemp);
			free(m_cValue);

			if(m_stgTemp3 == m_stgTemp2)
			{
				HTREEITEM hTreeTemp;
				i = sValue.length() + 1;

				TreeView_Expand(classTree->m_hWnd,hTreeItem,TVE_EXPAND);
				do
				{
					i = m_szRep.find("\\",i);
					if(i != -1)
					{
						hTreeTemp = hTreeItem;
						//Expansion du répertoire
						hTreeItem = classTree->ExpandedItem(hTreeItem, m_szRep.substr(0,i).c_str());
						if(hTreeItem == NULL)
						{
							hTreeItem = hTreeTemp;
							break;
						}
						TreeView_Expand(classTree->m_hWnd,hTreeItem,TVE_EXPAND);
						i++;
					}
					else
					{
						hTreeItem = classTree->ExpandedItem(hTreeItem, m_szRep.c_str());
						if(!TreeView_Expand(classTree->m_hWnd,hTreeItem,TVE_EXPAND))
						{	

							TreeView_SelectItem(classTree->m_hWnd,hTreeItem);
							if(_stricmp(cDirectory,m_szRep.c_str()) != 0)
							{
								StringCchCopy(cDirectory,MAX_PATH,m_szRep.c_str());
							}

							
						} 
					}
				}while(i != -1);
				if(i == -1)
					break;

			}
		}

		hTreeItem = TreeView_GetNextItem(classTree->m_hWnd,hTreeItem,TVGN_NEXT);

	}while(hTreeItem != NULL);

	if(hTreeItem == NULL)
	{
		hTreeItem = hTreeCItem;

		#if !(defined(__ICC) || defined(__ICL) || defined(__ECC) || defined(__ECL))
			m_szRep = "c:";
		#else
			m_szRep = "C:";
		#endif

		
		hTreeItem = classTree->ExpandedItem(hTreeCItem, m_szRep.c_str());
		if(!TreeView_Expand(classTree->m_hWnd,hTreeItem,TVE_EXPAND))
		{	

			TreeView_SelectItem(classTree->m_hWnd,hTreeItem);
			if(_stricmp(cDirectory,m_szRep.c_str()) != 0)
			{
				StringCchCopy(cDirectory,MAX_PATH,m_szRep.c_str());
			}

			
		} 
	}


	iInitializeFolder = 0;

	TVITEM m_tvItem;
	m_tvItem.mask = TVIF_PARAM;
	m_tvItem.hItem = hTreeItem;

	TreeView_GetItem(classTree->m_hWnd,&m_tvItem);

	CItemInfo * pItem = (CItemInfo *)m_tvItem.lParam;

	int iNumAlbum = m_CAlbumCatalogue->DetermineNumAlbum(cDirectory);

	if(iNumAlbum == -1)
		iNumAlbum = m_CImageCatalogue->DetermineNumAlbum(cDirectory);

	cDirectoryTemp[0] = '\0';
	SendInitMessage(iNumAlbum,pItem->GetTypeDonne(), hWndParent);
	StringCchCopy(cDirectoryTemp,MAX_PATH,cDirectory);
	SendMessage(hWndParent,WM_COMMAND,MAKEWPARAM(ID_ENDINITIALIZE_FODER,0),0);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

void CListWindowData::SendInitMessage(const int &iNumAlbum,int iNumTypeDonne, HWND hWndParent)
{
	
	switch(iNumTypeDonne)
	{
		case 1:
			{
				if(strlen(cDirectory) > 0 && _stricmp(cDirectoryTemp, cDirectory) != 0)
					SendMessage(hWndParent,WM_COMMAND,MAKEWPARAM(IDM_INITDIRECTORY,0),(LPARAM)cDirectory);
			}
			break;

		case 2:
			{
				if(iNumAlbum != -1)
				{
					SendMessage(hWndParent,WM_COMMAND,MAKEWPARAM(IDM_DIRNAME,0),(LPARAM)cDirectory);
					m_Album = m_CAlbumCatalogue->RecupPtAlbum(iNumAlbum);
					SendMessage(hWndParent,WM_COMMAND,MAKEWPARAM(IDM_ALBUMIMAGELIST,0),(LPARAM)iNumAlbum);

#ifdef SQLSERVERCE		


					m_IconeFileVectorAlbum.clear();
					CSqlIconeFileAlbum * m_cSqlIconeFileAlbum = new CSqlIconeFileAlbum();
					m_cSqlIconeFileAlbum->LoadIconeFileAlbum(&m_IconeFileVectorAlbum, m_Album->m_szAlbumName);
					delete m_cSqlIconeFileAlbum;
					SendMessage(hWndParent,WM_COMMAND,MAKEWPARAM(IDM_INITALBUM,0),(LPARAM)&m_IconeFileVectorAlbum);
#else
					SendMessage(hWndParent,WM_COMMAND,MAKEWPARAM(IDM_INITALBUM,0),(LPARAM)&m_Album->m_IconeFileVector);
#endif

					
					
				}
				else
				{
					if(strlen(cDirectory) > 0 && _stricmp(cDirectoryTemp, cDirectory) != 0)
						SendMessage(hWndParent,WM_COMMAND,MAKEWPARAM(IDM_INITDIRECTORY,0),(LPARAM)cDirectory);
				}
			}
			break;

		case 3:
			{

				if(iNumAlbum != -1)
				{
					IconeFileVector * lpIconeFileFolder = NULL;
					iRecherche = 1; // Impossible de supprimer un fichier
					m_CImageCatalogue->RecupPtAlbum(iNumAlbum, lpIconeFileFolder);
					SendMessage(hWndParent,WM_COMMAND,MAKEWPARAM(IDM_ALBUMIMAGELIST,0),(LPARAM)-2);
					SendMessage(hWndParent,WM_COMMAND,MAKEWPARAM(IDM_INITCATALOGUE,0),(LPARAM)lpIconeFileFolder);
					SendMessage(hWndParent,WM_COMMAND,MAKEWPARAM(IDM_DIRNAME,0),(LPARAM)cDirectory);
				}
				else
				{
					if(strlen(cDirectory) > 0 && _stricmp(cDirectoryTemp, cDirectory) != 0)
						SendMessage(hWndParent,WM_COMMAND,MAKEWPARAM(IDM_INITDIRECTORY,0),(LPARAM)cDirectory);
				}

			}
			break;
	}

	StringCchCopy(cDirectoryTemp,MAX_PATH,cDirectory);

}