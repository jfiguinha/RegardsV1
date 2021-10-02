#include "stdafx.h"
#include ".\listpreviewdata.h"
#include "objet.h"
#include "statusbar.h"
#include "effet.h"
#include "LoadImage.h"
#include <ItemInfo.h>
#include <files.h>
#include "parameter.h"
#include <GeneralHashFunctions.h>
#ifdef SQLSERVERCE

#include <SqlIconeFileFolder.h>
using namespace Regards::Sqlite;
#endif

CListPreviewData::CListPreviewData(void)
{
	iLargeur = 90;
	iHauteur = 70;
	iMarge = 1;
	hImgThumbmail = NULL;
	iAffichageAlaVolee = 0;
	iNumFolder = 0;
	m_cParameter = new CParameter();
	m_cParameter->GetOptionMiniature(iLargeur,iHauteur);
	m_bLoadData = true;
}

void CListPreviewData::SetThumbnailSize(const int &iWidth, const int &iHeight)
{
	m_bLoadData = false;
	iLargeur = iWidth;
	iHauteur = iHeight;
}

void CListPreviewData::UpdateData()
{
	if(m_bLoadData)
		m_cParameter->GetOptionMiniature(iLargeur,iHauteur);
}

CListPreviewData::~CListPreviewData(void)
{
	if(hImgThumbmail != NULL)
	{
		ImageList_RemoveAll(hImgThumbmail);
		ImageList_Destroy(hImgThumbmail);
	}
	delete m_cParameter;
}


void CListPreviewData::CreateHImageList(IconeFileVector &iconeFileVector)
{
	if(hImgThumbmail != NULL)
	{
		ImageList_RemoveAll(hImgThumbmail);
		ImageList_Destroy(hImgThumbmail);
	}

	hImgThumbmail = NULL;

	CreateFolderIcone();
	iconeFileVector.erase(iconeFileVector.begin(),iconeFileVector.end());
}

////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////

void CListPreviewData::CreateFolderIcone()
{
	if(m_bLoadData)
		m_cParameter->GetOptionMiniature(iLargeur,iHauteur);

	if(hImgThumbmail != NULL)
	{
		ImageList_RemoveAll(hImgThumbmail);
		ImageList_Destroy(hImgThumbmail);
	}

	hImgThumbmail = NULL;

	CEffet cEffet;
	HBITMAP hBitmapResult = NULL;
	HBITMAP hBitmap = NULL;
	BITMAP bitmap;
	int iNumLocal;
	UINT iWidth = 0;
	UINT iHeight = 0;


	hImgThumbmail = ImageList_Create(iLargeur,iHauteur, ILC_COLOR24 | ILC_MASK,1,1);

	ImageList_SetBkColor(hImgThumbmail,CLR_NONE);

	hBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_PICTBORDER));
	hBitmapResult = cEffet.GenerateIcone(hBitmap, iLargeur, iHauteur, iWidth, iHeight, 1, RGB(255,255,255),RGB(230,230,230));
	iNumLocal = ImageList_AddMasked(hImgThumbmail,hBitmapResult,RGB(255,255,255));
	DeleteObject(hBitmapResult);
	DeleteObject(hBitmap);

	hBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_FOLDER));
	hBitmapResult = cEffet.GenerateIcone(hBitmap, iLargeur, iHauteur, iWidth, iHeight, 1, RGB(255,255,255),RGB(230,230,230));
	iNumFolder = ImageList_Add(hImgThumbmail,hBitmapResult,NULL);
	DeleteObject(hBitmapResult);	
	DeleteObject(hBitmap);

	hBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_PICTBORDER));
	hBitmapResult = cEffet.GenerateIcone(hBitmap, iLargeur, iHauteur, iWidth, iHeight, 3, RGB(255,255,255), RGB(199,205,250));
	iNumLocal = ImageList_AddMasked(hImgThumbmail,hBitmapResult,RGB(255,255,255));
	DeleteObject(hBitmapResult);
	ImageList_SetOverlayImage(hImgThumbmail,iNumLocal,1);
	DeleteObject(hBitmap);
	DeleteObject(hBitmapResult);	

}

////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////

int CListPreviewData::ChargeThumbmail(const char * Localisation,IconeFileVector &iconeFileVector)
{
	char cFile[MAX_PATH];
	char cDirectory[MAX_PATH];
	int m_iSaveMiniature = 0;

	wchar_t cFilePath[MAX_PATH];
	wchar_t cStreamPath[MAX_PATH];
	TCHAR folder[MAX_PATH];
	wchar_t cDataPath[MAX_PATH];

	if(strcmp(Localisation,"-1") == 0)
	{
		iFileDatabase = 0;
		return -1;
	}

	StringCchPrintf(cFile,MAX_PATH,"%s\\Image%sAlbum.db",m_cParameter->GetThumbnailDirectory().c_str(),Localisation);

	size_t m_sizeTConvert;
	mbstowcs_s(&m_sizeTConvert,cFilePath,MAX_PATH, cFile, MAX_PATH);

	StringCchPrintf(folder,MAX_PATH,"Image%sAlbum",Localisation);

	mbstowcs_s(&m_sizeTConvert,cStreamPath,MAX_PATH, folder, MAX_PATH);

	StringCchPrintf(cFile,MAX_PATH,"Image%sData",Localisation);

	mbstowcs_s(&m_sizeTConvert,cDataPath,MAX_PATH, cFile, MAX_PATH);

	m_cParameter->GetSaveOptionMiniature(m_iSaveMiniature);

	if(m_iSaveMiniature)
	{
		char m_cFileZip[MAX_PATH];
#ifdef SAVEZIP
		StringCchPrintf(m_cFileZip,MAX_PATH,"%s\\Image%sAlbum.zip",m_cParameter->GetThumbnailDirectory().c_str(),Localisation);
		if(CObjet::TestFilePresent(m_cFileZip))
		{
			int m_iReturn = 0;
			CFiles m_CFiles;
			string m_stgFilename;
			m_CFiles.Open(m_cFileZip);
			CFichierZip m_CFichierZip = m_CFiles.ExtractFile(0);
			m_CFichierZip.Extract(m_cParameter->GetThumbnailDirectory().c_str(),&m_stgFilename);
			m_CFichierZip.Close();
			m_CFiles.Close();
			m_iReturn = ChargeData(cFilePath, cStreamPath, cStreamPath,cDataPath,1,iconeFileVector);
			remove(m_stgFilename.c_str());
			return m_iReturn;
		}
#else
		StringCchPrintf(m_cFileZip,MAX_PATH,"%s\\Image%sAlbum.db",m_cParameter->GetThumbnailDirectory().c_str(),Localisation);
		if(CObjet::TestFilePresent(m_cFileZip))
			return ChargeData(cFilePath, cStreamPath, folder, cStreamPath,cDataPath,1,iconeFileVector);
#endif

	}

	return -1;

}

////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////

int CListPreviewData::SaveThumbmail(const char * Localisation,IconeFileVector &iconeFileVector)
{

	char m_cFile[MAX_PATH];
	char m_cFileZip[MAX_PATH];
	char cDirectory[MAX_PATH];
	int m_iSaveMiniature = 0;
	wchar_t cFilePath[MAX_PATH];
	wchar_t cStreamPath[MAX_PATH];
	wchar_t cDataPath[MAX_PATH];
	TCHAR folder[MAX_PATH];

	if(strcmp(Localisation,"-1") == 0 || iFileDatabase == 33)
		return -1;

	StringCchPrintf(m_cFile,MAX_PATH,"%s\\Image%sAlbum.db",m_cParameter->GetThumbnailDirectory().c_str(),Localisation);

	size_t m_sizeTConvert;
	mbstowcs_s(&m_sizeTConvert,cFilePath,MAX_PATH, m_cFile, MAX_PATH);

	StringCchPrintf(folder,MAX_PATH,"Image%sAlbum",Localisation);

	mbstowcs_s(&m_sizeTConvert,cStreamPath,MAX_PATH, folder, MAX_PATH);

	StringCchPrintf(m_cFile,MAX_PATH,"Image%sData",Localisation);

	mbstowcs_s(&m_sizeTConvert,cDataPath,MAX_PATH, m_cFile, MAX_PATH);

	m_cParameter->GetSaveOptionMiniature(m_iSaveMiniature);

	if(m_iSaveMiniature)
	{
		SaveData(cFilePath, cStreamPath, folder, cStreamPath,cDataPath,1,iconeFileVector);
#ifdef SAVEZIP
		StringCchPrintf(m_cFileZip,MAX_PATH,"%s\\Image%sAlbum.zip",m_cParameter->GetThumbnailDirectory().c_str(),Localisation);
		StringCchPrintf(m_cFile,MAX_PATH,"%s\\Image%sAlbum.db",m_cParameter->GetThumbnailDirectory().c_str(),Localisation);
		CFiles m_CFiles;
		m_CFiles.Create(m_cFileZip);
		m_CFiles.AddFileByName(m_cFile,1,false,true);
		m_CFiles.Close();
		remove(m_cFile);
#endif
	}

	return 0;
}



////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////

int  CListPreviewData::SaveData(const wchar_t * cFilePath, const wchar_t * FolderPath, const TCHAR * Folder, const wchar_t * m_chStorageName, const wchar_t * m_chStreamName, const int &iTypeData,IconeFileVector &iconeFileVector)
{

	IStorage* pStr;
	ULONG pcbWritten;
	CIconeFile m_IconeFile;
	HRESULT hr;

	hr = StgOpenStorage(cFilePath, NULL, STGM_READWRITE|STGM_DIRECT|STGM_SHARE_EXCLUSIVE, NULL, 0, &pStr);

	if(hr != S_OK)
	{
		hr = StgCreateDocfile(cFilePath, STGM_DIRECT|STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE, NULL, &pStr);
	}
	else
	{
		pStr->DestroyElement(m_chStreamName);
		pStr->DestroyElement(m_chStorageName);
	}

	if(hr == S_OK)
	{

		//delete[] cFilePath;
		IStream* pStream;
		
		hr = pStr->CreateStream(m_chStorageName, STGM_DIRECT|STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE, 0, 0, &pStream );

		ImageList_Write(hImgThumbmail, pStream);

		pStream->Release(); 
		pStream = NULL;

#ifndef SQLSERVERCE

		if(iTypeData == 1)
		{
			hr = pStr->CreateStream(m_chStreamName, STGM_DIRECT|STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE, 0, 0, &pStream );

			for (IconeFileVector::iterator dit = iconeFileVector.begin(); dit!=iconeFileVector.end(); dit++)
			{
				char Value[255];
				m_IconeFile = *dit;
				_ltoa_s(m_IconeFile.lNumIcone,Value, 255,  10);

				hr = pStream->Write(m_IconeFile.m_stgFullPathName, 512, &pcbWritten);
				hr = pStream->Write(Value, sizeof(long), &pcbWritten);
			}

			pStream->Release(); 
		}
#endif
		pStr->Release(); 

#ifdef SQLSERVERCE

		CSqlIconeFileFolder * _sqlIconeFileFolder = new CSqlIconeFileFolder();
		_sqlIconeFileFolder->DeleteIconeFileFolder((TCHAR *)Folder);
		_sqlIconeFileFolder->SaveIconeFileFolder(&iconeFileVector, (TCHAR *)Folder);
		delete _sqlIconeFileFolder;

#endif

//Sauvegarde des données en ZIP



		return 0;

	}

	return -1;
}


////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////

int CListPreviewData::ChargeDatabaseFolder(const char * cDirectory,const char * Localisation,IconeFileVector &iconeFileVector)
{
	char cFile[MAX_PATH];
	wchar_t cFilePath[MAX_PATH];
	WCHAR pswFolder[MAX_PATH];
	int m_iSaveMiniature = 0;
	string m_stgFolder = cDirectory;
	if(cDirectory[strlen(cDirectory) - 1] != '\\')
		m_stgFolder += "\\";

	if(strcmp(Localisation,"-1") == 0)
	{
		iFileDatabase = 0;
		return -1;
	}

	if(!CObjet::IsValidateFolder(cDirectory))
	{
		return -1;
	}

	//Obtention du code en base64 de cDirectory
	unsigned int iHashValue = RSHash(m_stgFolder);
	StringCchPrintf(cFile,MAX_PATH,"%s\\%x.db",m_cParameter->GetThumbnailDirectory().c_str(),iHashValue);

	size_t m_sizeTConvert;
	mbstowcs_s(&m_sizeTConvert,cFilePath,MAX_PATH, cFile, MAX_PATH);
	mbstowcs_s(&m_sizeTConvert,pswFolder,MAX_PATH, m_stgFolder.c_str(), MAX_PATH);

	m_cParameter->GetSaveOptionMiniature(m_iSaveMiniature);

	if(m_iSaveMiniature)
	{
		char m_cFileZip[MAX_PATH];
#ifdef SAVEZIP
		StringCchPrintf(m_cFileZip,MAX_PATH,"%s\\%x.zip",m_cParameter->GetThumbnailDirectory().c_str(),iHashValue);
#else
		StringCchPrintf(m_cFileZip,MAX_PATH,"%s\\%x.db",m_cParameter->GetThumbnailDirectory().c_str(),iHashValue);
#endif
		//Test si le fichier existe ou pas
		if(CObjet::TestFilePresent(m_cFileZip))
		{
			int m_iReturn = 0;
#ifdef SAVEZIP
			
			CFichierZip m_CFichierZip;
			CFiles m_CFiles;
			string m_stgFilename;
			m_CFiles.Open(m_cFileZip);
			m_CFichierZip = m_CFiles.ExtractFile(0);
			m_CFichierZip.Extract(m_cParameter->GetThumbnailDirectory().c_str(),&m_stgFilename);
			m_CFichierZip.Close();
			m_CFiles.Close();
#endif
			m_iReturn = ChargeData(cFilePath,pswFolder,(const TCHAR *)m_stgFolder.c_str(), L"imageStream",L"imageData",1,iconeFileVector);
#ifdef SAVEZIP
			remove(m_stgFilename.c_str());
#endif
			return m_iReturn;
		}
	}

	return -1;

}

////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////

int CListPreviewData::SauverDatabaseFolder(const char * cDirectory,IconeFileVector &iconeFileVector)
{

	if(ImageList_GetImageCount(hImgThumbmail) > 2 && iFileDatabase != 33)
	{

		char cFile[MAX_PATH];
		char m_cFileZip[MAX_PATH];
		CIconeFile m_IconeFile;
		wchar_t cFilePath[MAX_PATH];
		WCHAR pswFolder[MAX_PATH];
		int m_iSaveMiniature = 0;
		unsigned int iHashValue = RSHash(cDirectory);
		StringCchPrintf(cFile,MAX_PATH,"%s\\%x.db",m_cParameter->GetThumbnailDirectory().c_str(),iHashValue);

		if(!CObjet::IsValidateFolder(cDirectory))
		{
			return -1;
		}

		//mbstowcs(cFilePath, cFile, 512);
		size_t m_sizeTConvert;
		mbstowcs_s(&m_sizeTConvert,cFilePath,MAX_PATH, cFile, MAX_PATH);
		mbstowcs_s(&m_sizeTConvert,pswFolder,MAX_PATH, cDirectory, MAX_PATH);

		m_cParameter->GetSaveOptionMiniature(m_iSaveMiniature);

		if(m_iSaveMiniature)
		{
			SaveData(cFilePath, pswFolder, cDirectory,L"imageStream",L"imageData",1,iconeFileVector);
#ifdef SAVEZIP
			StringCchPrintf(m_cFileZip,MAX_PATH,"%s\\%x.zip",m_cParameter->GetThumbnailDirectory().c_str(),iHashValue);
			CFiles m_CFiles;
			m_CFiles.Create(m_cFileZip);
			m_CFiles.AddFileByName(cFile,1,false,true);
			m_CFiles.Close();
			remove(cFile);
#endif
		}
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////

int CListPreviewData::ChargeData(const wchar_t * FilePath, const wchar_t * FolderPath, const TCHAR * Folder, const wchar_t * m_chStorageName, const wchar_t * m_chStreamName, const int &iTypeData, IconeFileVector &iconeFileVector)
{

	IStorage* pStg; 
	IStream* pStream;

	//hImgThumbmail = ImageList_Create(iLargeur,iHauteur, ILC_COLOR24 | ILC_MASK,10,1);

	iFileDatabase = 0;

	//return -1;

	HRESULT hr = StgOpenStorage(FilePath, NULL, STGM_READWRITE|STGM_DIRECT|STGM_SHARE_EXCLUSIVE , NULL, 0, &pStg);

	if(hr == S_OK)
	{
		UpdateData();
		CIconeFile m_IconeFile;
		ULONG pcbWritten;

		hr = pStg->OpenStream(m_chStorageName,NULL, STGM_READWRITE|STGM_DIRECT|STGM_SHARE_EXCLUSIVE, 0, &pStream );

		if(hr == S_OK)
		{
			
			hImgThumbmail = ImageList_Read(pStream);

			int iImage = ImageList_GetImageCount(hImgThumbmail);
			char Temp[255];

			StringCchPrintf(Temp,255,"%d",iImage);

			//On vérifie que la taille correspond
			IMAGEINFO ImageInfo;
			ZeroMemory(&ImageInfo,sizeof(IMAGEINFO));
			ImageList_GetImageInfo(hImgThumbmail,0,&ImageInfo);

			SIZE size;
			ZeroMemory(&size,sizeof(SIZE));
			size.cx = abs(ImageInfo.rcImage.right - ImageInfo.rcImage.left);
			size.cy = abs(ImageInfo.rcImage.top - ImageInfo.rcImage.bottom);
			if(size.cx != GetLargeur() || size.cy != GetHauteur())
			{
				ImageList_RemoveAll(hImgThumbmail);
				ImageList_Destroy(hImgThumbmail);
				hImgThumbmail = NULL;
				pStream->Release();
				pStg->Release(); 
				return -1;
			}

			//MessageBox(NULL,Temp,"Infos",0);


			if(hImgThumbmail == NULL)
			{
				pStream->Release();
				pStg->Release(); 
				return -1;
			}

			iNumFolder = 1;

			iFileDatabase = 1;

			pStream->Release(); 

			pStream = NULL;

#ifndef SQLSERVERCE

			if(iTypeData == 1)
			{
				iconeFileVector.clear();

				hr = pStg->OpenStream(m_chStreamName,NULL, STGM_READWRITE|STGM_DIRECT|STGM_SHARE_EXCLUSIVE, 0, &pStream );

				try
				{
					do
					{
						char Value[255];
						hr = pStream->Read(m_IconeFile.m_stgFullPathName, 512, &pcbWritten);
						if(pcbWritten == 0)
							break;

						hr = pStream->Read(Value, sizeof(long), &pcbWritten);
						if(pcbWritten == 0)
							break;

						m_IconeFile.lNumIcone = atol(Value);
						iconeFileVector.push_back(m_IconeFile);

					}while(pcbWritten > 0);

					pStream->Release(); 
				}
				catch(...)
				{
					iconeFileVector.clear();
					return -1;
				}
			}
		
#endif

		}
		pStg->Release(); 

#ifdef SQLSERVERCE

		iconeFileVector.clear();
		CSqlIconeFileFolder * _sqlIconeFileFolder = new CSqlIconeFileFolder();
		_sqlIconeFileFolder->LoadIconeFileFolder(&iconeFileVector,(TCHAR *)FolderPath);
		delete _sqlIconeFileFolder;

#endif

		return 0;
	}

	return -1;
}