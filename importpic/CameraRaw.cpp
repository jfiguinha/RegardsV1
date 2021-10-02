// Raw.cpp: implementation of the CRaw class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CameraRaw.h"
#include <objet.h>
#include <resource.h>
//#include "raw.h"
typedef int (*GetDimensionsRaw)(const char * ,UINT *,UINT *);
typedef int (*WriteIBitmap)(const char * ,HWND, BYTE * *);

extern HINSTANCE hInstance;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCameraRaw::CCameraRaw()
{
	ModId = LoadLibrary("librawdll.dll");
}

CCameraRaw::~CCameraRaw()
{
	if(ModId != NULL)
		FreeLibrary(ModId);
}

void CCameraRaw::GetDimensions(const char * fileName,UINT *width,UINT *height)
{
	#pragma omp critical(y)
	{
		GetDimensionsRaw Fn_Ptr;
		int m_iStatus;

		//GetDimensionsRaw(fileName,width,height);


		if(ModId != NULL)
		{
			Fn_Ptr = (GetDimensionsRaw)GetProcAddress(ModId, "GetDimensionsRaw");
			if(Fn_Ptr != NULL)
				m_iStatus = Fn_Ptr(fileName,width,height);

			//FreeLibrary(ModId);
		}
	}
}

ULONG __stdcall CCameraRaw::ReadRawPrivate(LPVOID parametre)
{
	char m_szMessage[255];
	LPRAWDATAIMPORT m_lpRawData = (LPRAWDATAIMPORT)parametre;

	HWND hWnd2 = NULL;
	HWND hWnd1 = NULL;
	HWND hDlg = m_lpRawData->hParent;

	if(hDlg != NULL)
	{
		hWnd1 = GetDlgItem(m_lpRawData->hParent, IDC_FILEIN);
		SetWindowText(hWnd1,m_lpRawData->szFileName);
		hWnd2 = GetDlgItem(m_lpRawData->hParent, IDC_FILEOUT);

		//MSG msg;
		//if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		//	return 0;

		SendDlgItemMessage(hDlg, IDC_PRGFILE, PBM_STEPIT , 0, 0);
		LoadString(hInstance,IDS_STGOPENRAWFILE,m_szMessage,255);
		SetWindowText(hWnd2,m_szMessage);
	}

	#pragma omp critical(y)
	{
		//HMODULE ModId;
		WriteIBitmap Fn_Ptr;
		GetDimensionsRaw Fn_Ptr2;
		int m_iStatus;

		//GetDimensionsRaw(fileName,width,height);
		
		//ModId = LoadLibrary("librawdll.dll");
		if(m_lpRawData->ModId != NULL)
		{
			UINT width = 0;
			UINT height = 0;
			Fn_Ptr2 = (GetDimensionsRaw)GetProcAddress(m_lpRawData->ModId, "GetDimensionsRaw");
			if(Fn_Ptr2 != NULL)
				m_iStatus = Fn_Ptr2(m_lpRawData->szFileName,&width,&height);

			Fn_Ptr = (WriteIBitmap)GetProcAddress(m_lpRawData->ModId, "WriteIBitmap");
			if(Fn_Ptr != NULL)
			{
				int iTaille = 32 >> 3;
				long lsize = iTaille * width * height;

				BYTE * m_bByte = (BYTE *)malloc(lsize * sizeof(BYTE));
				Fn_Ptr(m_lpRawData->szFileName,hDlg,&m_bByte);
				if(m_bByte != NULL)
				{
					//m_ciBitmap->CreateBitmap(width, height);
					m_lpRawData->m_ciBitmap->SetBitmapData(m_bByte,width, height);
					m_lpRawData->m_ciBitmap->ReCreateHBitmap(false);
					m_lpRawData->m_ciBitmap->VertFlipBuf();
					free(m_bByte);
				}
			}

		}
	}
	if(hDlg != NULL)
		EndDialog(m_lpRawData->hParent,1);

	return 0;									// Returns Number Of Bytes Read In
}

///////////////////////////////////////////////////////////////////////////////////////////////
//Fonction permettant de déplacer les fichiers
///////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CCameraRaw::OpenFichier(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	static DWORD dwThreadId; 
	static HANDLE hThread;
	static LPRAWDATAIMPORT m_lpRawData;

	switch (message)
	{
		case WM_INITDIALOG:
			{
				m_lpRawData = (LPRAWDATAIMPORT)lParam;
				m_lpRawData->hParent = hDlg;

				SendDlgItemMessage(hDlg, IDC_PRGFILE, PBM_SETRANGE , (WPARAM)0, MAKELPARAM(0,5));
				SendDlgItemMessage(hDlg, IDC_PRGFILE, PBM_SETSTEP , (WPARAM)1, 0);

				hThread = (HANDLE)	CreateThread(NULL, 1024,
							ReadRawPrivate, m_lpRawData, 0, &dwThreadId);

				SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);

			}
			break;

		case WM_COMMAND:
			{
				int wmId, wmEvent;
				wmId    = LOWORD(wParam); 
				wmEvent = HIWORD(wParam); 
				int bResult;
				// Parse the menu selections:

				bResult = PostThreadMessage(dwThreadId, WM_NULL, 0, 0);

				Sleep(1000);
			}
			break;

		case WM_CLOSE:
			CloseHandle(hThread);
			EndDialog(hDlg,1);
			break;

	}
    return FALSE;
}

int CCameraRaw::ReadRaw(CIBitmap * m_ciBitmap, const char *szFileName, bool bFullScreen)
{

	/*
	CRaw m_cRaw;
	m_cRaw.OpenRawFile(m_ciBitmap,szFileName);
	*/

	CObjet::StartWait();
	RAWDATAIMPORT m_RawData;
	ZeroMemory(&m_RawData,sizeof(RAWDATAIMPORT));
	m_RawData.m_ciBitmap = m_ciBitmap;
	m_RawData.szFileName = szFileName;
	m_RawData.ModId = ModId;
	if(!bFullScreen && m_ciBitmap->hWnd != NULL)
	{
		DialogBoxParam(hInstance, (LPCTSTR)IDD_DLGPROGRESSBAR, NULL, (DLGPROC)OpenFichier,(LPARAM)&m_RawData);
		BringWindowToTop(m_ciBitmap->hWnd);
	}
	else
	{
		//::MessageBox(NULL,"FullScreen","FullScreen",0);
		m_RawData.hParent = NULL;
		ReadRawPrivate(&m_RawData);
	}

	CObjet::EndWait();
	return 0;									// Returns Number Of Bytes Read In
}


int CCameraRaw::ReadRawThumbnail(CIBitmap * m_ciBitmap, const char *szFileName,const int &ThumbWidth, const int &ThumbHeight)
{
	/*
	CRaw m_cRaw;

	m_cRaw.SetOutputSize(3,1);

	m_cRaw.OpenRawFile(szFileName,0);

	m_cRaw.ScaleScolors();

	m_cRaw.ConvertToRGB();

	m_cRaw.WriteIBitmap(m_ciBitmap);
	*/

	return 0;									// Returns Number Of Bytes Read In
}