#include "stdafx.h"
#include ".\conversionfichier.h"
#include "resource.h"
#include "LoadImage.h"
#include <sadirread.h>
#include <convertdata.h>
#include <safileentry.h>
#include <ibitmap.h>
#include <effet.h>
#include <LoadPicture.h>


CConversionFichier::CConversionFichier(void)
{
}

CConversionFichier::~CConversionFichier(void)
{
	CloseHandle(hThread);
}

void CConversionFichier::SetParam(CConvertData * cConvertData)
{
	m_cConvertData = cConvertData;
}


LRESULT CConversionFichier::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SendDlgItemMessage(hDlg, IDC_PRGFILE, PBM_SETRANGE , (WPARAM)0, 0);
	SendDlgItemMessage(hDlg, IDC_PRGFILE, PBM_SETSTEP , (WPARAM)1, 0);

	m_cConvertData->hWnd = hDlg;


	hThread = CreateThread(NULL, cuStackSize,
				ConvertFichier, m_cConvertData, 0, &dwThreadId);


	SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);
	return 0;
}

LRESULT CConversionFichier::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////


ULONG __stdcall CConversionFichier::ConvertFichier(LPVOID parametre)
{
	OleInitialize(NULL);
	CConvertData * m_CONVERTDATA = (CConvertData *)parametre;
	//Récupération des fichiers images
	CSADirRead m_cCSADirRead;
	SAFileVector files;
	SAFileVector::iterator fit;

	m_cCSADirRead.ClearDirs();
	m_cCSADirRead.ClearFiles();
	m_cCSADirRead.GetDirs(m_CONVERTDATA->m_szRepOrigin);
	m_cCSADirRead.GetImageFile();
	files = m_cCSADirRead.Files();
	fit = files.begin();

	int iNumFiles = 0;

	SendDlgItemMessage(m_CONVERTDATA->hWnd, IDC_PRGFILE, PBM_SETRANGE , (WPARAM)0, MAKELPARAM(0,(int)files.size()));

	MSG msg;
	for (fit = files.begin(); fit!=files.end(); fit++)
	{

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			break;
		}
		else
		{

			//Génération du nom de l'image
			iNumFiles++;
			char m_szFilename[MAX_PATH];
			char Temp[20];
			long lNumFile = iNumFiles * m_CONVERTDATA->iIncrementation;
			TCHAR drive[_MAX_DRIVE],dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];

			_splitpath_s((*fit).m_sName.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);


			if(m_CONVERTDATA->iPosNumero == 1)
				StringCchPrintf(m_szFilename,MAX_PATH,"%s\\%s",m_CONVERTDATA->m_szRepDest,m_CONVERTDATA->m_szLibelle);
			else
				StringCchPrintf(m_szFilename,MAX_PATH,"%s\\",m_CONVERTDATA->m_szRepDest);

			StringCchPrintf(Temp,20,"%d",lNumFile);

			//Taille de l'incrémentation
			if(strlen(Temp) < m_CONVERTDATA->iTailleNumero)
			{
				int j = m_CONVERTDATA->iTailleNumero - strlen(Temp);
				for(int k = 0;k < j;k++)
				{
					StringCbCat(m_szFilename,MAX_PATH,"0");
				}

			}
			StringCbCat(m_szFilename,MAX_PATH,Temp);

			if(m_CONVERTDATA->iPosNumero == 2)
			{
				StringCbCat(m_szFilename,MAX_PATH,m_CONVERTDATA->m_szLibelle);
			}


			if(m_CONVERTDATA->iConverFormat != 0)
			{
				StringCbCat(m_szFilename,MAX_PATH,".");
			}
			

			//Vérification si conversion ou renommage
			if(m_CONVERTDATA->iConverFormat == 0)
			{
				StringCbCat(m_szFilename,MAX_PATH,ext);
				CopyFile((*fit).m_sName.c_str(),m_szFilename,TRUE);
			}
			else
			{
				CLoadImage m_cChargementImage;
				CIBitmap * m_ciBitmap = new CIBitmap();
				m_cChargementImage.ChargementImageStatic(m_ciBitmap,(*fit).m_sName.c_str());
				//Conversion de l'image

				//Test si modification de format (resize)
				if(m_CONVERTDATA->iRedimensionne && ((m_CONVERTDATA->iWidth > 0 && m_CONVERTDATA->iHeight > 0) || m_CONVERTDATA->iPourcentage))
				{
					CEffet m_cEffet;


					if(m_CONVERTDATA->iPourcentage)
					{
						UINT iWidthLocal = (m_ciBitmap->GetBitmapWidth() * m_CONVERTDATA->iValeurPourcentage) / 100;
						UINT iHeightLocal = (m_ciBitmap->GetBitmapHeigth() * m_CONVERTDATA->iValeurPourcentage) / 100;
						m_cEffet.ScaleBitmap(m_ciBitmap,iWidthLocal,iHeightLocal,true);
					}
					else
					{
						if(m_CONVERTDATA->iRapport)
						{
							//Avec ratio
							HBITMAP hBitmap;
							UINT iWidth = 0;
							UINT iHeight = 0;
							hBitmap = m_cEffet.GenerateIcone(m_ciBitmap->hBitmap, m_CONVERTDATA->iWidth, m_CONVERTDATA->iHeight, iWidth, iHeight, 0, RGB(0,0,0), RGB(0,0,0));
							m_ciBitmap->SetBitmap(hBitmap,false);
							DeleteObject(hBitmap);
						}
						else
						{
							m_cEffet.ScaleBitmap(m_ciBitmap,m_CONVERTDATA->iWidth,m_CONVERTDATA->iHeight,true);
						}
					}
				}

				//Sauvegarde de l'image
				switch(m_CONVERTDATA->iNumFormat)
				{
					case 1:
						{
							//JPEG
							StringCbCat(m_szFilename,MAX_PATH,"jpg");
							CLoadPicture::SaveImage(CLoadPicture::JPEG,*m_ciBitmap,m_szFilename,0,90);
							break;
						}
					case 2:
						{
							//BMP
							StringCbCat(m_szFilename,MAX_PATH,"bmp");
							CLoadPicture::SaveImage(CLoadPicture::BMP,*m_ciBitmap,m_szFilename);
							break;
						}

					case 3:
						{
							//TGA
							StringCbCat(m_szFilename,MAX_PATH,"tga");
							CLoadPicture::SaveImage(CLoadPicture::TGA,*m_ciBitmap,m_szFilename);
							break;

						}

					case 4:
						{
							//TIF
							StringCbCat(m_szFilename,MAX_PATH,"tif");
							CLoadPicture::SaveImage(CLoadPicture::TIFF,*m_ciBitmap,m_szFilename,0,90);
							break;
						}

					case 5:
						{
							//PDF
							StringCbCat(m_szFilename,MAX_PATH,"pdf");
							CLoadPicture::SaveImage(CLoadPicture::PDF,*m_ciBitmap,m_szFilename,2,0);
						}
				}
				
				delete m_ciBitmap;
			}

			//Vérification si suppression fichier
			if(m_CONVERTDATA->iDelImage)
				DeleteFile((*fit).m_sName.c_str());

			SendDlgItemMessage(m_CONVERTDATA->hWnd, IDC_FILEIN, WM_SETTEXT , (WPARAM)0, (LPARAM)(*fit).m_sName.c_str());
			SendDlgItemMessage(m_CONVERTDATA->hWnd, IDC_FILEOUT, WM_SETTEXT , (WPARAM)0, (LPARAM)m_szFilename);


			SendDlgItemMessage(m_CONVERTDATA->hWnd, IDC_PRGFILE, PBM_SETPOS , (WPARAM)iNumFiles, 0);

		}
		//Vérification si conversion à faire

		//Vérification si Réechantillonage
		//(*fit).m_sName.c_str();
	}

	EndDialog(m_CONVERTDATA->hWnd, 0);
	ExitThread(0);
	OleUninitialize();
	return 0;
}