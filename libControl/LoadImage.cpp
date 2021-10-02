// LoadImage.cpp: implementation of the CLoadImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LoadImage.h"
#include "effet.h"
#include "objet.h"
#include <ibitmap.h>
#include "VideoAPI.h"
#include <avi.h>
#include "loadpicture.h"
CRITICAL_SECTION CLoadImage::CriticalSection;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoadImage::CLoadImage()
{
	hThread = NULL;
	dwThreadId = 0;
	pimpl_ = new CLoadPicture();
	ZeroMemory(&m_ImageThread,sizeof(LOADIMAGETHREAD));
	m_ImageThread.m_bHandleTerminer = true;
	m_cLoadVideo = new CVideoAPI();
}

CLoadImage::~CLoadImage()
{
	if(hThread != NULL)
	{
		CloseHandle(hThread);
	}

	if(pimpl_ != NULL)
		delete pimpl_;

	delete m_cLoadVideo;
}

/////////////////////////////////////////////////////////////////
//Chargement de l'image
/////////////////////////////////////////////////////////////////
int CLoadImage::ChargementImage(CIBitmap * m_ciBitmap, const char * szFileName, const HWND &hWnd, bool bFullScreen)
{
	if(!m_ImageThread.m_bHandleTerminer)
		return 0;

	m_ImageThread.m_bHandleTerminer = false;

	try
	{
		if(hThread != NULL)
		{
			CloseHandle(hThread);
			hThread = NULL;
		}

		m_ciBitmap->hWnd = hWnd;
		m_ciBitmap->SetFilename(szFileName);

		m_ImageThread.m_ciBitmap = m_ciBitmap;
		m_ImageThread.m_bFullScreen = bFullScreen;

		hThread = (HANDLE)CreateThread(NULL, cuStackSize,
						ChargerImage, &m_ImageThread, 0, &dwThreadId);	

	}
	catch(...)
	{
		m_ImageThread.m_bHandleTerminer = true;
		return 0;
	}

	return 1;
}

/////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////
ULONG __stdcall CLoadImage::ChargerImage(LPVOID parametre)
{
	OleInitialize(NULL);
	InitializeCriticalSection(&CriticalSection); 
	CObjet m_cObjet;
	LPLOADIMAGETHREAD m_lpImage = (LPLOADIMAGETHREAD)parametre;
	CIBitmap * m_ciBitmap = m_lpImage->m_ciBitmap;
	bool m_bSend = false;

	try
	{

		EnterCriticalSection(&CriticalSection); 

		
		char cFileName[255];
		m_ciBitmap->GetFilename(cFileName);
		int m_iValue = m_cObjet.TestImageFormat(cFileName);

		m_cObjet.StartWait();

		if(m_iValue > 0)
		{
			CLoadPicture::ChargementImage(m_iValue,cFileName, m_ciBitmap, m_lpImage->m_bFullScreen);
			
			if(m_ciBitmap->hBitmap != NULL || m_ciBitmap->GetPtBitmap() != NULL)
			{
				m_cObjet.TestOrientation(m_ciBitmap,cFileName);
				SendMessage(m_ciBitmap->hWnd, WM_ERASEBKGND, (WPARAM)m_ciBitmap,MAKELPARAM(IDM_CHGIMAGE_OK,0));
				m_bSend = true;
			}
		}

		m_cObjet.EndWait();


		LeaveCriticalSection(&CriticalSection);
	}
	catch(...)
	{
		SendMessage(m_ciBitmap->hWnd, WM_ERASEBKGND, 0,MAKELPARAM(IDM_CHGIMAGE_OK,1));
		LeaveCriticalSection(&CriticalSection);
		m_cObjet.EndWait();
	}

	if(!m_bSend)
	{

	}


	m_lpImage->m_bHandleTerminer = true;
	DeleteCriticalSection(&CriticalSection);
	ExitThread(0);
	OleUninitialize();
	return 0;
}

/////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////
int CLoadImage::ChargementImageStatic(CIBitmap * m_ciBitmap, const char *szFileName, bool bFullScreen)
{
	CObjet m_cObjet;

	int m_iValue = m_cObjet.TestImageFormat(szFileName);

	try
	{

		if(m_iValue > 0)
		{
			CLoadPicture::ChargementImage(m_iValue,szFileName, m_ciBitmap, bFullScreen);
			m_cObjet.TestOrientation(m_ciBitmap,szFileName);
		}

	}
	catch(...)
	{
	}
	return m_iValue;
}

/////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////
int CLoadImage::GetDimensionImageStatic(const char *szFileName, UINT * width, UINT * height)
{
	CObjet m_cObjet;

	int m_iValue = m_cObjet.TestImageFormat(szFileName);

	try
	{
		if(m_iValue > 0)
			pimpl_->GetDimensions(szFileName, width, height, m_iValue);
		else
		{
			m_iValue = m_cObjet.TestVideoFormat(szFileName);
			if(m_iValue > 0)
			{
				switch(m_iValue)
				{
					case 100:
						{
							CAvi * m_cAvi = new CAvi();
							int m_lwidth = 0;
							int m_lheight = 0;
							m_cAvi->GetDimensionAvi(szFileName,m_lwidth,m_lheight);
							*width = m_lwidth;
							*height = m_lheight;
							delete m_cAvi;
						}
						break;

					default:
						{
							long m_lwidth = 0;
							long m_lheight = 0;
							m_cLoadVideo->GetVideoSize(szFileName,m_lwidth,m_lheight);
							*width = m_lwidth;
							*height = m_lheight;
						}
						break;
				}

			}
		}

	}
	catch(...)
	{
	}
	return m_iValue;
}

/////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////
HBITMAP CLoadImage::ChargementIconeStatic(const char *szFileName, const int &iIconeWidth, const int &iIconeHeight, const unsigned int &Width, const unsigned int &Height, const int &iMarge, const COLORREF &m_color1, const COLORREF &m_color2, bool bFolder, bool bJpegQuality )
{
	CEffet m_cEffet;
	CObjet m_cObjet;
	HBITMAP hBitmap = NULL;

	int m_iValue = m_cObjet.TestImageFormat(szFileName);
	UINT iWidth = 0;
	UINT iHeight = 0;
	

	try
	{
		switch(m_iValue)
		{
			default:

				if(bFolder)
				{
					HBITMAP hBitmapFolder = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_FOLDERBIG));
					hBitmap = m_cEffet.GenerateFolderIcone(hBitmapFolder,szFileName,iIconeWidth,iIconeHeight, iMarge, m_color1, m_color2);
					DeleteObject(hBitmapFolder);
					break;
				}

				//Test if folder

				if(m_iValue > 0)		
				{
					try
					{
						CIBitmap m_ciBitmap;
						CObjet m_cObjet;
						m_ciBitmap.CreateBitmap(0,0);

						int m_iFormat = m_cObjet.TestImageFormat(szFileName);
	
						if(m_iFormat == 1 && !bJpegQuality)
						{
							HBITMAP m_hBitmapLocal;
							if(CLoadPicture::GetImageFromJPG(szFileName,m_hBitmapLocal,0,0))
							{
								m_ciBitmap.SetBitmap(m_hBitmapLocal,false);
								DeleteObject(m_hBitmapLocal);
							}
							else
							{
								CLoadPicture::ChargementImage(m_iValue,szFileName, &m_ciBitmap, true);
							}

						}
						else
						{
							CLoadPicture::ChargementImage(m_iValue,szFileName, &m_ciBitmap, true);
							
						}

						m_cObjet.TestOrientation(&m_ciBitmap,szFileName);

						if(m_ciBitmap.GetBitmapWidth() > 0 && m_ciBitmap.GetBitmapHeigth() > 0)
							hBitmap = m_cEffet.GenerateIcone(&m_ciBitmap, iIconeWidth, iIconeHeight, iWidth, iHeight, iMarge, m_color1, m_color2);
						else
							hBitmap = NULL;

					}
					catch(...)
					{
						char Temp[255];
						//LoadString(hInstance,IDS_ERREUR,Temp,255);
						//::MessageBox(NULL,szFileName,Temp,0);
						return NULL;
					}
				}
				else
				{

					m_iValue = m_cObjet.TestVideoFormat(szFileName);
					if(m_iValue > 0)
					{
						
						char cTempPath[MAX_PATH];
						TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
						_splitpath_s(szFileName, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

						GetTempPath(MAX_PATH,cTempPath);
						sprintf_s(cTempPath,MAX_PATH,"%s%s.bmp",cTempPath,fname);

						LONG m_iWidth = 640;
						LONG m_iHeight = 480;

						//GetDimensionImageStatic(szFileName,&m_iWidth,&m_iHeight);

						if(m_iWidth > 0 && m_iHeight > 0)
						{
							/*
							m_cLoadVideo->VideoToBmp(szFileName,cTempPath,1,m_iWidth,m_iHeight);
							CIBitmap m_ciBitmap;
							ChargementImageStatic(&m_ciBitmap,cTempPath);
							HBITMAP hBitmapVideo;
							m_ciBitmap.GetBitmap(hBitmapVideo);
							remove(cTempPath);
							*/
							HBITMAP hBitmapVideo = m_cLoadVideo->DoExtractVideoFrame(szFileName, m_iWidth, m_iHeight);

							if(hBitmapVideo != NULL)
							{
								hBitmap = m_cEffet.GenerateIcone(hBitmapVideo, iIconeWidth, iIconeHeight, iWidth, iHeight, iMarge, m_color1, m_color2);
								DeleteObject(hBitmapVideo);
							}
						}
					}

				}
				break;
		
		}
	}
	catch(...)
	{
	}
	
	return (HBITMAP)CopyImage(hBitmap, IMAGE_BITMAP, 0, 0, LR_COPYDELETEORG);
}


////////////////////////////////////////////////////////////////////////////////
//Fermeture du Thread
////////////////////////////////////////////////////////////////////////////////
void CLoadImage::CloseThread()
{
	CloseHandle(hThread);
}