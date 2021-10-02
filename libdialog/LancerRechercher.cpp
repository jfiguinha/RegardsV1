#include "StdAfx.h"
#include ".\lancerrechercher.h"
#include "imagecalaloguerecherche.h"
#include "resource.h"
#include <AlbumData.h>
#include <CatalogueData.h>

#ifdef SQLSERVERCE
#include <SqlCatalog.h>
#include <SqlIconeFileCatalogue.h>
using namespace Regards::Sqlite;
#endif

CLancerRechercher::CLancerRechercher(HINSTANCE hInstance)
{
	hThread = NULL;
	m_iReturnValue = 0;
	hInstanceLocal = hInstance;
}

CLancerRechercher::~CLancerRechercher(void)
{
	if(hThread != NULL)
		CloseHandle(hThread);

	hThread = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

ULONG CLancerRechercher::RechercherFichier(void * lParam)
{
	CLancerRechercher * m_lpLancerRechercher = (CLancerRechercher * )lParam;
	//Chargement dans un vecteur l'ensemble des fichiers à rechercher
	IconeFileVector m_IconeFileVectorSrc;
	IconeFileVector m_IconeFileVectorDst;
	char FichierHTML[255];
	int iFin = 1;
	MSG msg;
	int iNbElement = 0;
#ifdef SQLSERVERCE
	IconeFileVector m_iconeFileVector;
	size_t m_sizeTConvert;
	TCHAR m_wAlbumName[MAX_PATH];
	CSqlIconeFileCatalogue * m_cIconeFileCatalogue = new CSqlIconeFileCatalogue();
#endif
	int i = 0, j = 0;

	//Calcul de la taille du select

	for(j = 0;j < m_lpLancerRechercher->m_lpCatalogueRecherche->iNbAlbumDst;j++)
	{
		i+= m_lpLancerRechercher->m_lpCatalogueRecherche->m_CatalogueData->m_AlbumDataVector[m_lpLancerRechercher->m_lpCatalogueRecherche->iTabAlbumDest[j]].iNbFiles;
	}

	SendDlgItemMessage(m_lpLancerRechercher->hWnd, IDC_PRGFILE, PBM_SETRANGE, (WPARAM)0, MAKELPARAM(0, i));

	char m_cTempMessage[255];
	LoadString(m_lpLancerRechercher->hInstanceLocal,IDS_RUNSEARCHMSG1,m_cTempMessage,255);
	SendDlgItemMessage(m_lpLancerRechercher->hWnd, IDC_FILEIN, WM_SETTEXT , (WPARAM)0, (LPARAM)m_cTempMessage);



	for(i = 0;i < m_lpLancerRechercher->m_lpCatalogueRecherche->iNbAlbumSrc;i++)
	{

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			iFin = 0;
			goto FIN;
		}

		LoadString(m_lpLancerRechercher->hInstanceLocal,IDS_RUNSEARCHMSG4,m_cTempMessage,255);
		string m_stgChaine = m_cTempMessage;
		m_stgChaine += m_lpLancerRechercher->m_lpCatalogueRecherche->m_CatalogueData->m_AlbumDataVector[m_lpLancerRechercher->m_lpCatalogueRecherche->iTabAlbumSrc[i]].m_szAlbumName;

		SendDlgItemMessage(m_lpLancerRechercher->hWnd, IDC_FILEOUT, WM_SETTEXT , (WPARAM)0, (LPARAM)m_stgChaine.c_str());

		CAlbumData m_AlbumTemp =  m_lpLancerRechercher->m_lpCatalogueRecherche->m_CatalogueData->m_AlbumDataVector[m_lpLancerRechercher->m_lpCatalogueRecherche->iTabAlbumSrc[i]];

#ifdef SQLSERVERCE
		m_iconeFileVector.clear();
		m_cIconeFileCatalogue->LoadIconeFileCatalog(&m_iconeFileVector, m_AlbumTemp.m_szAlbumName);
		m_IconeFileVectorSrc.insert(m_IconeFileVectorSrc.begin(),m_iconeFileVector.begin(),m_iconeFileVector.end());

#else
		m_AlbumTemp.m_IconeFileVector.clear();
		m_lpLancerRechercher->m_lpCatalogueRecherche->m_CatalogueData->ChargerCatalogue(m_AlbumTemp,m_lpLancerRechercher->m_lpCatalogueRecherche->iTabAlbumSrc[i]);
#ifdef _DEBUG
		iNbElement = m_AlbumTemp.m_IconeFileVector.size();
#endif

		m_IconeFileVectorSrc.insert(m_IconeFileVectorSrc.begin(),m_AlbumTemp.m_IconeFileVector.begin(),m_AlbumTemp.m_IconeFileVector.end());

#ifdef _DEBUG
		iNbElement = m_IconeFileVectorSrc.size();
#endif
#endif

	}

	LoadString(m_lpLancerRechercher->hInstanceLocal,IDS_RUNSEARCHMSG2,m_cTempMessage,255);
	SendDlgItemMessage(m_lpLancerRechercher->hWnd, IDC_FILEIN, WM_SETTEXT , (WPARAM)0, (LPARAM)m_cTempMessage);

	//Boucle sur les fichiers d'index à stocker
	j = 1;
	for(i = 0;i < m_lpLancerRechercher->m_lpCatalogueRecherche->iNbAlbumDst;i++)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			iFin = 0;
			goto FIN;
		}

		CAlbumData m_AlbumTemp = m_lpLancerRechercher->m_lpCatalogueRecherche->m_CatalogueData->m_AlbumDataVector[m_lpLancerRechercher->m_lpCatalogueRecherche->iTabAlbumDest[i]];

#ifdef SQLSERVERCE

		m_iconeFileVector.clear();
		m_cIconeFileCatalogue->LoadIconeFileCatalog(&m_iconeFileVector, m_AlbumTemp.m_szAlbumName);

		//Boucle sur les fichiers d'index à comparer

		LoadString(m_lpLancerRechercher->hInstanceLocal,IDS_RUNSEARCHMSG5,m_cTempMessage,255);
		string m_stgChaine = m_cTempMessage;//"Traitement du catalogue ";
		m_stgChaine += m_lpLancerRechercher->m_lpCatalogueRecherche->m_CatalogueData->m_AlbumDataVector[m_lpLancerRechercher->m_lpCatalogueRecherche->iTabAlbumDest[i]].m_szAlbumName;

		SendDlgItemMessage(m_lpLancerRechercher->hWnd, IDC_FILEIN, WM_SETTEXT , (WPARAM)0, (LPARAM)m_stgChaine.c_str());

		for(IconeFileVector::const_iterator fit = m_iconeFileVector.begin(); fit!= m_iconeFileVector.end(); fit++,j++)
		{
			CIconeFile m_IconeFile = *fit;
			
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				iFin = 0;
				goto FIN;
			}			

			SendDlgItemMessage(m_lpLancerRechercher->hWnd, IDC_PRGFILE, PBM_SETPOS , (WPARAM)j, 0);
			SendDlgItemMessage(m_lpLancerRechercher->hWnd, IDC_FILEOUT, WM_SETTEXT , (WPARAM)0, (LPARAM)m_IconeFile.m_stgFullPathName);
			m_lpLancerRechercher->m_lpCatalogueRecherche->DataComparaison(m_IconeFile,m_IconeFileVectorSrc,m_IconeFileVectorDst);

		}

#else
		m_AlbumTemp.m_IconeFileVector.clear();
		m_lpLancerRechercher->m_lpCatalogueRecherche->m_CatalogueData->ChargerCatalogue(m_AlbumTemp,m_lpLancerRechercher->m_lpCatalogueRecherche->iTabAlbumDest[i]);

		//Boucle sur les fichiers d'index à comparer

		LoadString(m_lpLancerRechercher->hInstanceLocal,IDS_RUNSEARCHMSG5,m_cTempMessage,255);
		string m_stgChaine = m_cTempMessage;//"Traitement du catalogue ";
		m_stgChaine += m_lpLancerRechercher->m_lpCatalogueRecherche->m_CatalogueData->m_AlbumDataVector[m_lpLancerRechercher->m_lpCatalogueRecherche->iTabAlbumDest[i]].m_szAlbumName;

	#ifdef _DEBUG
		iNbElement = m_AlbumTemp.m_IconeFileVector.size();
	#endif

		SendDlgItemMessage(m_lpLancerRechercher->hWnd, IDC_FILEIN, WM_SETTEXT , (WPARAM)0, (LPARAM)m_stgChaine.c_str());


		IconeFileVector::const_iterator fit;

		for (fit = m_AlbumTemp.m_IconeFileVector.begin(); fit!= m_AlbumTemp.m_IconeFileVector.end(); fit++,j++)
		{
			CIconeFile m_IconeFile = *fit;
			
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				iFin = 0;
				goto FIN;
			}			

			SendDlgItemMessage(m_lpLancerRechercher->hWnd, IDC_PRGFILE, PBM_SETPOS , (WPARAM)j, 0);

			SendDlgItemMessage(m_lpLancerRechercher->hWnd, IDC_FILEOUT, WM_SETTEXT , (WPARAM)0, (LPARAM)m_IconeFile.m_stgFullPathName);

			////////////////////////////////////////////////////////////////////
			//Comparaison des données
			////////////////////////////////////////////////////////////////////

			m_lpLancerRechercher->m_lpCatalogueRecherche->DataComparaison(m_IconeFile,m_IconeFileVectorSrc,m_IconeFileVectorDst);

		}

#endif
	}


	m_lpLancerRechercher->m_lpCatalogueRecherche->m_CatalogueData->m_IconeFileRecherche.clear();

	LoadString(m_lpLancerRechercher->hInstanceLocal,IDS_RUNSEARCHMSG3,m_cTempMessage,255);
	SendDlgItemMessage(m_lpLancerRechercher->hWnd, IDC_FILEIN, WM_SETTEXT , (WPARAM)0, (LPARAM)m_cTempMessage);

	m_lpLancerRechercher->m_lpCatalogueRecherche->GenerateHTMLFile(m_IconeFileVectorSrc, FichierHTML);

	////////////////////////////////////////////////////
	//Affichage du rapport
	////////////////////////////////////////////////////

	m_lpLancerRechercher->m_lpCatalogueRecherche->ShowHTML(FichierHTML,255);

	////////////////////////////////////////////////////
	//Affichage de la liste des fichiers trouvés
	////////////////////////////////////////////////////
	

FIN:

	m_lpLancerRechercher->m_iReturnValue = iFin;

	EndDialog(m_lpLancerRechercher->hWnd,iFin);
	
	m_iconeFileVector.clear();
	m_IconeFileVectorSrc.clear();
	m_IconeFileVectorDst.clear();

#ifdef SQLSERVERCE
	delete m_cIconeFileCatalogue;
#endif

	return 0;


}

LRESULT CLancerRechercher::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SendDlgItemMessage(hDlg, IDC_PRGFILE, PBM_SETRANGE , (WPARAM)0, 0);
	SendDlgItemMessage(hDlg, IDC_PRGFILE, PBM_SETSTEP , (WPARAM)1, 0);

	hWnd = hDlg;

	hThread = CreateThread(NULL, cuStackSize,
				RechercherFichier, this, 0, &dwThreadId);


	SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);
	return 0;
}

LRESULT CLancerRechercher::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	wmId    = LOWORD(wParam); 
	wmEvent = HIWORD(wParam); 
	BOOL bResult;
	// Parse the menu selections:
	
	bResult = PostThreadMessage(dwThreadId, WM_NULL, 0, 0);

	Sleep(1000);
	return 0;
}

LRESULT CLancerRechercher::OnDestroy(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(hThread != NULL)
		CloseHandle(hThread);

	hThread = NULL;

	EndDialog(hDlg,0);
	return 0;
}