// ImageCatalogue.cpp: implementation of the CImageCatalogue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageCatalogue.h"
#include <SADirRead.h>
#include "loadimage.h"
#include <ibitmap.h>
#include "security.h"
#include "filtre.h"
#include "objet.h"
#include <CatalogueData.h>
#include <ItemInfo.h>
#include <RechercherCatalog.h>
#include <InsertCatalog.h>

#ifdef SQLSERVERCE	
#include <SqlAlbumUtility.h>
#include <SqlCatalog.h>
#include <SqlIconeFileCatalogue.h>
using namespace Regards::Sqlite;
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImageCatalogue::CImageCatalogue()
{
	m_CatalogueData->iTypeDonnee = 3; 
	m_CatalogueData->iNbAlbum = 0;
	LoadCatalogue();
}

CImageCatalogue::~CImageCatalogue()
{
#ifndef SQLSERVERCE
	SaveCatalogue();
#endif
}

int CImageCatalogue::DeleteRechercheFile(HWND hWnd,VECTITEM * m_vectItem)
{
	char clpText[255];
	char Temp[255];
	char clpCaption[255];
	int iNbCount,iNbEncours;
	iNbEncours = 0 ;
	SHFILEOPSTRUCT m_ShFileOp;
	m_ShFileOp.hwnd = hWnd;
	m_ShFileOp.wFunc = FO_DELETE;
	m_ShFileOp.fFlags = FOF_NOCONFIRMATION;
	m_ShFileOp.fAnyOperationsAborted = FALSE;
	m_ShFileOp.lpszProgressTitle = "Suppression des fichiers";

	iNbCount = m_vectItem->size();//ListView_GetSelectedCount(hWnd);

	LoadString(hInstance,IDS_SUPPIMAGE,clpText,255);
	LoadString(hInstance,IDS_INFOS,clpCaption,255);
	StringCchPrintf(Temp,255,clpText,iNbCount);

	if(MessageBox(NULL,Temp,clpCaption,MB_YESNO | MB_ICONWARNING) == IDYES)
	{

#ifdef SQLSERVERCE

		//WCHAR m_wAlbumName[MAX_PATH];
		//WCHAR m_wFilename[MAX_PATH];
		//size_t m_sizeTConvert;	
#endif

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

			m_IconeFile = find(m_CatalogueData->m_IconeFileRecherche.begin(),m_CatalogueData->m_IconeFileRecherche.end(),m_IconeFileTemp);

			m_ShFileOp.pFrom = m_IconeFile->m_stgFullPathName;
			m_ShFileOp.pTo = m_IconeFile->m_stgFullPathName;

			SHFileOperation(&m_ShFileOp);

			m_CatalogueData->m_IconeFileRecherche.erase(m_IconeFile);

#ifdef SQLSERVERCE

			
			//mbstowcs_s(&m_sizeTConvert,m_wAlbumName, MAX_PATH, m_IconeFile->szKeyName, MAX_PATH);
			//mbstowcs_s(&m_sizeTConvert,m_wFilename, MAX_PATH, m_IconeFile->m_stgFullPathName, MAX_PATH);

			CSqlAlbumUtility::DeleteAlbumFile(m_IconeFile->szKeyName, m_IconeFile->m_stgFullPathName);

#endif
		}



		int nItem = m_vectItem->at(0);
		for(int i = 0;i < iNbCount;i++)
		{	
			ListView_DeleteItem(hWnd, nItem);
		}

		m_vectItem->clear();
	}

	SendMessage(hWnd,LVM_ARRANGE,0,0);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

int CImageCatalogue::Rechercher(HWND hWnd,IconeFileVector * * m_IconeFileRechercheLocal)
{
	CRechercherCatalog * m_cRechercherCatalog = new CRechercherCatalog(hInstance);
	m_cRechercherCatalog->SetParam(m_CatalogueData);
	m_cRechercherCatalog->CreateDlg(hInstance, (LPCTSTR)IDD_DLGCATASEARCH, hWnd);
	int m_iReturnValue = m_cRechercherCatalog->GetReturnValue();
	delete m_cRechercherCatalog;

	if(m_iReturnValue == 1)
	{
		*m_IconeFileRechercheLocal = &m_CatalogueData->m_IconeFileRecherche;
		return 1;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//Compactage du fichier des catalogues
/////////////////////////////////////////////////////////////////////////////////////////////////

int CImageCatalogue::CompacterFichierCatalogue()
{
#ifndef SQLSERVERCE
	char cDirectory[MAX_PATH];
	char cFile[MAX_PATH];
	char cFile2[MAX_PATH];
	const char * m_cFilename = "catalogue2.db";

	SaveCatalogue(m_cFilename);
	for(int i = 0;i < m_CatalogueData->iNbAlbum;i++)
	{
		m_CatalogueData->m_AlbumDataVector[i].m_IconeFileVector.clear();
		m_CatalogueData->ChargerCatalogue(m_CatalogueData->m_AlbumDataVector[i],i);
		m_CatalogueData->SauverUnCatalogue(m_CatalogueData->m_AlbumDataVector[i],i,m_cFilename);
	}

	StringCchPrintf(cFile,MAX_PATH,"%s\\catalogue.db",m_CatalogueData->m_cDefaultDirectory);
	StringCchPrintf(cFile2,MAX_PATH,"%s\\%s",m_CatalogueData->m_cDefaultDirectory,m_cFilename);

	remove(cFile);
	rename(cFile2,cFile);
#endif
	CObjet::AffichageMessage(IDS_COMPACTDATAREADY,IDS_INFOS);
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//Suppression d'un album
/////////////////////////////////////////////////////////////////////////////////////////////////

int CImageCatalogue::SuppAlbum(HWND hWndTree, CTree *m_cTree, const int &iNumAlbum)
{
	string sName = m_CatalogueData->m_AlbumDataVector[iNumAlbum].m_szAlbumName;
	m_CatalogueData->m_AlbumDataVector.erase(m_CatalogueData->m_AlbumDataVector.begin() + iNumAlbum);

	HTREEITEM hItem = m_CatalogueData->hItemAlbum;
	HTREEITEM hChild = TreeView_GetChild(hWndTree, hItem );

	while ( hChild ) 
	{
		//Récupération du nom de l'item
		int iTypeAllocation = 1;
		string sValue;
		TV_ITEM tItem;
		tItem.hItem = hChild;
		tItem.mask = TVIF_TEXT;
		tItem.pszText = (LPTSTR)malloc(255);
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

///////////////////////////////////////////////////////////////////////////////////////////////
//Mise à jour du catalogue
///////////////////////////////////////////////////////////////////////////////////////////////

int CImageCatalogue::MiseAJour(HWND hWndTree, const int &iNumAlbum)
{
	char cDirectory[MAX_PATH];
	CObjet m_cObjet;

	cDirectory[0] = '\0';

	//Création d'un album
	//Album m_AlbumTemp = m_CatalogueData->m_AlbumDataVector[iNumAlbum];

	
	CInsertCatalog * m_InsertCatalog = new CInsertCatalog(hInstance);

#ifdef SQLSERVERCE
	
	IconeFileVector m_iconeFileVector;
	m_InsertCatalog->SetParam(&m_CatalogueData->m_AlbumDataVector[iNumAlbum], &m_iconeFileVector);

#else
	
	m_CatalogueData->m_AlbumDataVector[iNumAlbum].m_IconeFileVector.clear();
	m_InsertCatalog->SetParam(&m_CatalogueData->m_AlbumDataVector[iNumAlbum]);

#endif

	m_InsertCatalog->CreateDlg(hInstance, (LPCTSTR)IDD_FICHIER, hWndTree);
	int iReturn = m_InsertCatalog->GetReturnValue();
	delete m_InsertCatalog;

	//Appel de la boite de dialogue
	if(iReturn)
	{
#ifdef SQLSERVERCE
	

		CSqlIconeFileCatalogue * m_cSqlIconeFileCatalogue = new CSqlIconeFileCatalogue();
		m_cSqlIconeFileCatalogue->DeleteIconeFileCatalog(m_CatalogueData->m_AlbumDataVector[iNumAlbum].m_szAlbumName);
		m_cSqlIconeFileCatalogue->SaveIconeFileCatalog(&m_iconeFileVector, m_CatalogueData->m_AlbumDataVector[iNumAlbum].m_szAlbumName);
		delete m_cSqlIconeFileCatalogue;

#else
		m_CatalogueData->SauverUnCatalogue(m_CatalogueData->m_AlbumDataVector[iNumAlbum],iNumAlbum);
#endif
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//Fonction d'initialisation d'un catalogue
///////////////////////////////////////////////////////////////////////////////////////////////

int CImageCatalogue::InitCatalogue(HWND hWndTree, CTree * m_cTree)
{
	int iVersion = CObjet::GetWindowsVersionAvance();
	if(iVersion > 50)
	{
		HIMAGELIST m_imgList;
		HBITMAP hBmp;
		m_imgList = (HIMAGELIST)SendMessage(hWndTree, TVM_GETIMAGELIST, TVSIL_NORMAL, 0);
		hBmp = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_CATALOGUE));
		m_CatalogueData->iIconeAlbum = ImageList_Add(m_imgList,hBmp,NULL);
		m_CatalogueData->iIconeAlbum = ImageList_Add(m_imgList,hBmp,NULL);
		DeleteObject(hBmp);
		SendMessage(hWndTree, TVM_SETIMAGELIST, TVSIL_NORMAL, (LPARAM)m_imgList );
	}
	else
	{
		char Temp[MAX_PATH];
		SHFILEINFO  sfi;
		GetModuleFileName(hInstance,Temp,MAX_PATH);
		SHGetFileInfo(Temp, 0, &sfi, sizeof(sfi), SHGFI_DISPLAYNAME | SHGFI_SYSICONINDEX);
		m_CatalogueData->iIconeAlbum = sfi.iIcon;
	}

	AddItemToTree(hWndTree, m_cTree, "Catalogue");

	//hItemAlbum = TVI_ROOT;

	CAlbumData m_AlbumTemp;
	for (AlbumDataVector::const_iterator dit = m_CatalogueData->m_AlbumDataVector.begin(); dit!= m_CatalogueData->m_AlbumDataVector.end(); dit++)
	{
		m_AlbumTemp = *dit;
		if(strcmp(m_AlbumTemp.m_szAlbumName,"") != 0)
		{
			InsertItem(hWndTree,m_AlbumTemp.m_szAlbumName);
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

int CImageCatalogue::DetermineNumAlbum(const char *szDirName)
{
	string Temp = szDirName;
	int iPos;
	long lNumEntree = 0;
	iPos = Temp.find("Catalogue");
	if(iPos != -1 && Temp != "Catalogue")
	{
		Temp = Temp.substr(strlen("Catalogue")+1, Temp.length() - strlen("Catalogue")-1);
		
		for (AlbumDataVector::const_iterator dit = m_CatalogueData->m_AlbumDataVector.begin(); dit!= m_CatalogueData->m_AlbumDataVector.end(); dit++)
		{
			CAlbumData m_AlbumTemp;
			m_AlbumTemp = *dit;
			if(_stricmp(Temp.c_str(),m_AlbumTemp.m_szAlbumName) == 0)
				return lNumEntree;

			lNumEntree++;

		}
	}

	return -1;
}



CAlbumData * CImageCatalogue::RecupPtAlbum(const int &iNumAlbum)
{
	//Chargement des images dans le vecteur
#ifndef SQLSERVERCE
	
	for(int i = 0;i < m_CatalogueData->iNbAlbum;i++)
		m_CatalogueData->m_AlbumDataVector[iNumAlbum].m_IconeFileVector.clear();

	m_CatalogueData->ChargerCatalogue(m_CatalogueData->m_AlbumDataVector[iNumAlbum],iNumAlbum);

#endif

	return &m_CatalogueData->m_AlbumDataVector[iNumAlbum];
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

int CImageCatalogue::LoadCatalogue()
{
#ifdef SQLSERVERCE

	m_CatalogueData->m_AlbumDataVector.clear();
	CSqlCatalog * m_cSqlCatalog = new CSqlCatalog();
	//m_cSqlCatalog->LoadCatalog(&m_CatalogueData->m_AlbumDataVector);
	delete m_cSqlCatalog;

#else
	IStorage* pStg; 
	IStream* pStream;
	char cFile[MAX_PATH];
	
	wchar_t cFilePath[MAX_PATH];
	char cIntValue[10];

	StringCchPrintf(cFile,MAX_PATH,"%s\\catalogue.db",m_CatalogueData->m_cDefaultDirectory);

	size_t m_sizeTConvert;
	mbstowcs_s(&m_sizeTConvert,cFilePath,MAX_PATH, cFile, MAX_PATH);

	HRESULT hr = StgOpenStorage(cFilePath, NULL, STGM_READWRITE|STGM_DIRECT|STGM_SHARE_EXCLUSIVE, NULL, 0, &pStg);

	if(hr == S_OK)
	{
		ULONG pcbWritten;

		hr = pStg->OpenStream( L"CatalogueData",NULL, STGM_READWRITE|STGM_DIRECT|STGM_SHARE_EXCLUSIVE, 0, &pStream );

		m_CatalogueData->m_AlbumDataVector.clear();
		do
		{

			CAlbumData m_AlbumTemp;

			hr = pStream->Read(m_AlbumTemp.m_szAlbumName, 255, &pcbWritten);
			if(pcbWritten == 0)
				break;

			hr = pStream->Read(cIntValue, sizeof(int), &pcbWritten);
			m_AlbumTemp.iNbFiles = atoi(cIntValue);

			m_CatalogueData->iNbAlbum++;

			m_CatalogueData->m_AlbumDataVector.push_back(m_AlbumTemp);

		}while(pcbWritten > 0);

		pStream->Release(); 
		pStream = NULL;

		hr = pStg->OpenStream( L"CatalogueAttribut",NULL, STGM_READWRITE|STGM_DIRECT|STGM_SHARE_EXCLUSIVE, 0, &pStream );
		m_CatalogueData->m_AttributVector.clear();

		if(hr == S_OK)
		{

			do
			{

				CAttribut m_AttributTemp;

				hr = pStream->Read(m_AttributTemp.cLibelle, 255, &pcbWritten);
				if(pcbWritten == 0)
					break;

				hr = pStream->Read(cIntValue, sizeof(int), &pcbWritten);
				m_AttributTemp.iNumAttribut = atoi(cIntValue);

				m_CatalogueData->m_AttributVector.push_back(m_AttributTemp);

			}while(pcbWritten > 0);

			pStream->Release(); 

		}
		pStg->Release(); 

	}
#endif
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

int CImageCatalogue::SaveCatalogue(const char * m_cFilename)
{
#ifndef SQLSERVERCE
	IStorage* pStr;
	char cFile[MAX_PATH];
	ULONG pcbWritten;
	CAlbumData m_AlbumTemp;
	wchar_t cFilePath[MAX_PATH];
	char cIntValue[10];
	AttributVector::const_iterator AttributIt;
	AlbumDataVector::iterator dit;

	StringCchPrintf(cFile,MAX_PATH,"%s\\%s",m_CatalogueData->m_cDefaultDirectory,m_cFilename);

	//mbstowcs(cFilePath, cFile, MAX_PATH);
	size_t m_sizeTConvert;
	mbstowcs_s(&m_sizeTConvert,cFilePath,MAX_PATH, cFile, MAX_PATH);

	HRESULT hr = StgOpenStorage(cFilePath, NULL, STGM_READWRITE|STGM_DIRECT|STGM_SHARE_EXCLUSIVE, NULL, 0, &pStr);
	
	if(hr != S_OK)
	{
		hr = StgCreateDocfile(cFilePath, STGM_DIRECT|STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE, NULL, &pStr);
		if(hr != S_OK)
		{
			return -1;
		}
	}

	IStream* pStream;

	hr = pStr->CreateStream( L"CatalogueData", STGM_DIRECT|STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE, 0, 0, &pStream );

	for (dit = m_CatalogueData->m_AlbumDataVector.begin(); dit!= m_CatalogueData->m_AlbumDataVector.end(); dit++)
	{
		//Sauvegarde des données des albums
		
		m_AlbumTemp = *dit;
		hr = pStream->Write(m_AlbumTemp.m_szAlbumName, 255, &pcbWritten);

		//Sauvegarde du nombre d'image disponible dans l'album
		_itoa_s(m_AlbumTemp.iNbFiles,cIntValue,10,10);
		hr = pStream->Write(cIntValue, sizeof(int), &pcbWritten);
	}

	pStream->Release(); 
	pStream = NULL;

	hr = pStr->CreateStream( L"CatalogueAttribut", STGM_DIRECT|STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE, 0, 0, &pStream );

	for (AttributIt = m_CatalogueData->m_AttributVector.begin(); AttributIt!= m_CatalogueData->m_AttributVector.end(); AttributIt++)
	{
	
		CAttribut m_AttributTemp;
		m_AttributTemp = *AttributIt;
		hr = pStream->Write(m_AttributTemp.cLibelle, 255, &pcbWritten);

		//Sauvegarde du numero d'entrée de l'album
		_itoa_s(m_AttributTemp.iNumAttribut,cIntValue,10,10);
		hr = pStream->Write(cIntValue, sizeof(int), &pcbWritten);

	}

	pStream->Release(); 
	pStr->Release(); 
#endif
	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CImageCatalogue::CreerCatalogue(HWND hWnd, HWND hWndTree)
{
	char cDirectory[MAX_PATH];
	CObjet m_cObjet;

	cDirectory[0] = '\0';

#ifdef SQLSERVERCE
	IconeFileVector m_IconeFileVector;
	//WCHAR m_wAlbumName[MAX_PATH];
	//size_t m_sizeTConvert;
	CSqlCatalog m_cSqlCatalog;
	CSqlIconeFileCatalogue m_cSqlIconeFileCatalogue;
#endif

	//DialogBoxParam(hInstance,(LPCTSTR)IDD_SELECTDIRCHECK,hWnd,(DLGPROC)CDialog::SelectFolderBox,NULL);

	if(m_cObjet.GetDirectory(NULL, cDirectory))
	{
		if(VerifNomAlbum(cDirectory) >= 0)
			return 0;

		//Création d'un album
		CAlbumData m_AlbumTemp;

		if(cDirectory[strlen(cDirectory) - 1] == '\\')
			cDirectory[strlen(cDirectory) - 1] = '\0';

		StringCchCopy(m_AlbumTemp.m_szAlbumName,255,cDirectory);

		CInsertCatalog * m_InsertCatalog = new CInsertCatalog(hInstance);

#ifdef SQLSERVERCE
		m_InsertCatalog->SetParam(&m_AlbumTemp,&m_IconeFileVector);
#else
		m_InsertCatalog->SetParam(&m_AlbumTemp);
#endif
		m_InsertCatalog->CreateDlg(hInstance, (LPCTSTR)IDD_FICHIER, hWndTree);
		int iReturn = m_InsertCatalog->GetReturnValue();
		delete m_InsertCatalog;

		//Appel de la boite de dialogue
		if(iReturn)
		{

			char Temp[MAX_PATH];
			//Test si périphérique amovible ?
			TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
			_splitpath_s(cDirectory, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);	
			
			//_splitpath(cDirectory, drive, dir, fname, ext);

			StringCchCopy(Temp,MAX_PATH,drive);
			StringCbCat(Temp,MAX_PATH,"\\");

			SHFILEINFO  sfi;
			DWORD dwFileAttributes = SFGAO_REMOVABLE;

			SHGetFileInfo(Temp, dwFileAttributes, &sfi, sizeof(sfi), SHGFI_ATTRIBUTES | SHGFI_DISPLAYNAME | SHGFI_SYSICONINDEX);

			if(SFGAO_REMOVABLE & sfi.dwAttributes)
			{
				StringCchPrintf(cDirectory,MAX_PATH,"%s%s",sfi.szDisplayName,dir);
				StringCchCopy(m_AlbumTemp.m_szAlbumName,255,cDirectory);
			}

#ifdef SQLSERVERCE


			m_cSqlIconeFileCatalogue.SaveIconeFileCatalog(&m_IconeFileVector, m_AlbumTemp.m_szAlbumName);

#else
			m_CatalogueData->SauverUnCatalogue(m_AlbumTemp,m_CatalogueData->iNbAlbum);
#endif

			m_CatalogueData->m_AlbumDataVector.push_back(m_AlbumTemp);
			InsertItem(hWndTree, m_AlbumTemp.m_szAlbumName);
			m_CatalogueData->iNbAlbum++;
			return m_CatalogueData->iNbAlbum - 1;
		}

	}



	return -1;
}

int CImageCatalogue::RecupPtAlbum(const int &iNumAlbum, IconeFileVector * & lpIconeFileFolder)
{
#ifdef SQLSERVERCE

	m_iconeFileFolder.clear();
	CSqlIconeFileCatalogue * m_cSqlIconeFileCatalogue = new CSqlIconeFileCatalogue();
	m_cSqlIconeFileCatalogue->LoadIconeFileCatalog(&m_iconeFileFolder, m_CatalogueData->m_AlbumDataVector[iNumAlbum].m_szAlbumName);
	delete m_cSqlIconeFileCatalogue;

	lpIconeFileFolder = &m_iconeFileFolder;
#endif
	return 0;
}