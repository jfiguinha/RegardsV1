#include "StdAfx.h"
#include ".\insertcatalog.h"
#include <SADirRead.h>
#include "../loadimage.h"
#include <ibitmap.h>
#include <security.h>
#include <filtre.h>
#include "../objet.h"
#include <CatalogueData.h>
#include <ItemInfo.h>
#include "../resource.h"

class CSADirReadLocal : public CSADirRead
{
	public:
		
		bool AffichageDirectoryInformation(const string &m_stgInformation, HINSTANCE hInstance) 
		{
			char m_cTempMessage[255];
			LoadString(hInstance,IDS_SADEARMSG1,m_cTempMessage,255);
			SendDlgItemMessage(hWnd, IDC_FILEIN, WM_SETTEXT , (WPARAM)0, (LPARAM)m_cTempMessage);
			SendDlgItemMessage(hWnd, IDC_FILEOUT, WM_SETTEXT , (WPARAM)0, (LPARAM)m_stgInformation.c_str());
			return false;
		};

		bool AffichageFichierInformation(const string &m_stgInformation, HINSTANCE hInstance) 
		{
			char m_cTempMessage[255];
			LoadString(hInstance,IDS_SADEARMSG2,m_cTempMessage,255);
			SendDlgItemMessage(hWnd, IDC_FILEIN, WM_SETTEXT , (WPARAM)0, (LPARAM)m_cTempMessage);
			SendDlgItemMessage(hWnd, IDC_FILEOUT, WM_SETTEXT , (WPARAM)0, (LPARAM)m_stgInformation.c_str());
			return false;
		};


		bool TestFin()
		{
			MSG msg;
			if (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
				return true;
			else
				return false;
		}

	HWND hWnd;
};



CInsertCatalog::CInsertCatalog(HINSTANCE hInstance)
{
	hThread = NULL;
	m_AlbumTemp = NULL;
	this->hInstance = hInstance;
	m_iReturnValue = 0;
}

CInsertCatalog::~CInsertCatalog(void)
{
	if(hThread != NULL)
		CloseHandle(hThread);

	hThread = NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

UINT _stdcall CInsertCatalog::AjoutImageThread(LPVOID parametre)
{
	CoInitialize(NULL);

	CInsertCatalog * m_InsertCatalog = (CInsertCatalog *)parametre;
	CSADirReadLocal m_cCSADirRead;
	SAFileVector files;
	int iFin = 1;
	MSG msg;
	BYTE * m_bData = NULL;
	UINT Width = 0,Height = 0;
	ULONG lSize;

	m_cCSADirRead.hWnd = m_InsertCatalog->hWnd;
	m_cCSADirRead.m_bRecurse = true;
	m_cCSADirRead.m_iNbNiveaux = 1;
	m_cCSADirRead.GetDirs(m_InsertCatalog->m_AlbumTemp->m_szAlbumName);
	m_cCSADirRead.GetImageFile();
	int iSelCount = 0;

	files = m_cCSADirRead.Files();

	SendDlgItemMessage(m_InsertCatalog->hWnd, IDC_PRGFILE, PBM_SETRANGE, (WPARAM)0, MAKELPARAM(0, files.size()));

	for (SAFileVector::iterator fit = files.begin(); fit != files.end(); fit++)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			iFin = 0;
			//free(m_bData);
			break;
		}
		else
		{
			CObjet m_cObjet;
			CFiltre m_cFiltre;
			CLoadImage m_cLoadImage;
			CIBitmap * m_ciBitmap = new CIBitmap();
			CIconeFile m_IconeFile;
			CSecurity m_cSecurity;
			UINT m_LocalWidth, m_LocalHeight;
			ULONG m_LocalSize;

			string m_stgHashCode;
			CSAFileEntry dit = *(fit);
			char Temp[255];

			m_cObjet.AlbumGestionAddItem(dit,m_IconeFile);

			HBITMAP hBitmapResult;

			hBitmapResult = m_cLoadImage.ChargementIconeStatic(m_IconeFile.m_stgFullPathName, 15, 15, 0, 0, 1, RGB(255,255,255),RGB(230,230,230));

			if(hBitmapResult != NULL)
			{

				m_ciBitmap->SetBitmap(hBitmapResult);

				m_LocalWidth = m_ciBitmap->GetBitmapWidth();
				m_LocalHeight = m_ciBitmap->GetBitmapHeigth();

				if(m_LocalWidth != Width || Height != m_LocalHeight)
				{
					Width = m_LocalWidth;
					Height = m_LocalHeight;
					lSize = Width * Height;
					if(m_bData != NULL)
						free(m_bData);

					m_bData = (BYTE *)malloc(Width * Height);
					if (m_bData == NULL)
					{
						CObjet::ErreurInformation();
					}
				}

				m_ciBitmap->GetBandWBitmap(m_bData,m_LocalWidth,m_LocalHeight,m_LocalSize);

				//Sauvegarde dans un fichier

				if(lSize > 0)
				{
					//Récupération du Hash de l'image en N&B
					m_cSecurity.HashData(m_bData,lSize,m_stgHashCode);
					StringCchPrintf(Temp,255,"signature=%s;",m_stgHashCode.c_str());
					StringCbCat(m_IconeFile.informations,256,Temp);
#ifdef SQLSERVERCE
					m_InsertCatalog->m_iconeFileVector->push_back(m_IconeFile);
#else
					m_InsertCatalog->m_AlbumTemp->m_IconeFileVector.push_back(m_IconeFile);
#endif
					SendDlgItemMessage(m_InsertCatalog->hWnd, IDC_FILEIN, WM_SETTEXT , (WPARAM)0, (LPARAM)dit.m_sName.c_str());
					SendDlgItemMessage(m_InsertCatalog->hWnd, IDC_FILEOUT, WM_SETTEXT , (WPARAM)0, (LPARAM)m_InsertCatalog->m_AlbumTemp->m_szAlbumName);
					SendDlgItemMessage(m_InsertCatalog->hWnd, IDC_PRGFILE, PBM_SETPOS , (WPARAM)iSelCount, 0);
					iSelCount++;
					m_InsertCatalog->m_AlbumTemp->iNbFiles = iSelCount;
				}

				//m_ciBitmap.EraseData();

			}

			delete m_ciBitmap;
			
		}
	}

	free(m_bData);
	
	m_InsertCatalog->m_iReturnValue = iFin;
	EndDialog(m_InsertCatalog->hWnd, iFin);

	::CoUninitialize();
	return 0;
}


LRESULT CInsertCatalog::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char * clpText = new char[255];
	LoadString(hInstance,IDS_WINDOWCATCREATE,clpText,255);

	SetWindowText(hDlg,clpText);

	hWnd = hDlg;
	SendDlgItemMessage(hDlg, IDC_PRGFILE, PBM_SETSTEP , (WPARAM)1, 0);

	hThread = CreateThread(NULL, cuStackSize, 
		(LPTHREAD_START_ROUTINE)AjoutImageThread, (void *)this, 0, &dwThreadId);

	delete[] clpText;
	return 0;
}

LRESULT CInsertCatalog::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int bResult = PostThreadMessage(dwThreadId, WM_NULL, 0, 0);
	return 0;
}

LRESULT CInsertCatalog::OnDestroy(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(hThread != NULL)
		CloseHandle(hThread);

	hThread = NULL;
	return 0;
}