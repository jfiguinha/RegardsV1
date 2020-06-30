// Graveur.cpp: implementation of the CGraveur class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Graveur.h"
#include "..\objet.h"
#include <SADirRead.h>
#include ".\idiscmasterprogresseventsimpl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGraveur::CGraveur()
{

}

CGraveur::~CGraveur()
{

}


//////////////////////////////////////////////////////////////////
//Effacement du cd réinscriptible
///////////////////////////////////////////////////////////////////

int CGraveur::EraseCD(const bool &m_bMethod, HWND hWnd)
{
	IDiscMaster* pDiskMaster;

	UINT_PTR pnCookie;

	IDiscMasterProgressEventsImpl *pEvents = new IDiscMasterProgressEventsImpl(hWnd);

	HRESULT hr = CoCreateInstance(CLSID_MSDiscMasterObj, NULL,CLSCTX_LOCAL_SERVER, 
				   IID_IDiscMaster, (void**)&pDiskMaster);

	hr = pDiskMaster->Open();

	if(pDiskMaster->ProgressAdvise(pEvents,&pnCookie) != S_OK )
	{
		return 0;
	}

    IEnumDiscRecorders* recordersEnum;
    hr = pDiskMaster->EnumDiscRecorders(&recordersEnum);

    ULONG num;
    IDiscRecorder* recorder;
    hr = recordersEnum->Next(1,&recorder,&num);

	//Quick erase
	recorder->OpenExclusive();
	recorder->Erase(m_bMethod);
	recorder->Close();

    pDiskMaster->Close(); 

	delete pEvents;
	return hr;
}


//////////////////////////////////////////////////////////////////
//Effacement du cd réinscriptible
///////////////////////////////////////////////////////////////////

int CGraveur::EjectCD()
{
	IDiscMaster* pDiskMaster;

	HRESULT hr = CoCreateInstance(CLSID_MSDiscMasterObj, NULL,CLSCTX_LOCAL_SERVER, 
				   IID_IDiscMaster, (void**)&pDiskMaster);

	hr = pDiskMaster->Open();

    IEnumDiscRecorders* recordersEnum;
    hr = pDiskMaster->EnumDiscRecorders(&recordersEnum);

    ULONG num;
    IDiscRecorder* recorder;
    hr = recordersEnum->Next(1,&recorder,&num);

	//Quick erase
	recorder->OpenExclusive();
	recorder->Eject();
	recorder->Close();

    pDiskMaster->Close(); 

	return hr;
}

///////////////////////////////////////////////////////////////////
//Gravure des fichiers
///////////////////////////////////////////////////////////////////

int CGraveur::GraverFichierRep(const string &m_stgDirectory, HWND hWnd)
{
	CSADirRead m_CSADirRead;
	SAFileVector::const_iterator fit;
	SAFileVector files;
	IDiscMaster* pDiskMaster;
	IStorage * pStorage;
	UINT_PTR pnCookie;
	HRESULT hr;
	DWORD nBufferLength = 512;
	wchar_t cFilePath[512];
	char cFile[512];


	//GetTempPath(nBufferLength,NULL);

	char * cTemp = new char[nBufferLength + 1];
	if (cTemp == NULL)
	{
		CObjet::ErreurInformation();
	}
	GetTempPath(nBufferLength,cTemp);

	StringCchPrintf(cFile,512,"%swork2",cTemp);

	size_t m_sizeTConvert;
	mbstowcs_s(&m_sizeTConvert,cFilePath,512, cFile, 512);

	IDiscMasterProgressEventsImpl *pEvents = new IDiscMasterProgressEventsImpl(hWnd);
	if (pEvents == NULL)
	{
		CObjet::ErreurInformation();
	}
	//Récupération du chemin par défaut

	m_CSADirRead.m_iNbNiveaux = 1;
	m_CSADirRead.GetDirs(m_stgDirectory.c_str());
	m_CSADirRead.GetFiles("*.*");
	files = m_CSADirRead.Files();
		
	 
	hr = StgCreateStorageEx(cFilePath,
										STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE,
										STGFMT_STORAGE,
										0, NULL, NULL,
										IID_IStorage,
										reinterpret_cast<void**>(&pStorage) );


	hr = CoCreateInstance(CLSID_MSDiscMasterObj, NULL,CLSCTX_LOCAL_SERVER, 
				   IID_IDiscMaster, (void**)&pDiskMaster);

	hr = pDiskMaster->Open();

	if(pDiskMaster->ProgressAdvise(pEvents,&pnCookie) != S_OK )
	{
		return 0;
	}

    IJolietDiscMaster* joilet;
    hr = pDiskMaster->SetActiveDiscMasterFormat(IID_IJolietDiscMaster,(void**)&joilet );

    IEnumDiscRecorders* recordersEnum;
    hr = pDiskMaster->EnumDiscRecorders(&recordersEnum);

    ULONG num;
    IDiscRecorder* recorder;
    hr = recordersEnum->Next(1,&recorder,&num);

	for (fit = files.begin(); fit!=files.end(); fit++)
	{

		//IStream * pStream,* iStream;

		//IStorage* pStg; 
		char * data = new char[32001];
		if (data == NULL)
		{
			CObjet::ErreurInformation();
		}
		IStream* pStream;
		wchar_t cFilePath[512];
		string m_stgFilename;

		TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
		_splitpath_s((*fit).m_sName.c_str(), drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);	
		//_splitpath((*fit).m_sName.c_str(), drive, dir, fname, ext);

		m_stgFilename = fname;
		m_stgFilename += ext;

		size_t m_sizeTConvert;
		mbstowcs_s(&m_sizeTConvert,cFilePath,512, m_stgFilename.c_str(), m_stgFilename.length()+1);

		hr = pStorage->CreateStream(cFilePath,STGM_READWRITE|STGM_SHARE_EXCLUSIVE,0,0,&pStream);
		
		int fichier_source = 0;// _open((*fit).m_sName.c_str(), _O_RDONLY | _O_BINARY);

		if (_sopen_s(&fichier_source,(*fit).m_sName.c_str(),_O_RDONLY | _O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE)== -1) 
			return -1;
		if(fichier_source == -1)
			return -1;

		long octet_lus = _read(fichier_source, data, 32000);
		ULONG written;
		while(octet_lus > 0)
		{
			pStream->Write(data,octet_lus,&written);
			octet_lus = _read(fichier_source, data, 32000);
		}
		_close(fichier_source);

		delete[] data;

		pStream->Release();
	}

	m_CSADirRead.ClearDirs();

	hr = pDiskMaster->SetActiveDiscRecorder(recorder);

///////////////////////////////////
    hr = joilet->AddData(pStorage,1);  // HERE IT FAILS !!!


///////////////////////////////////
    hr = pDiskMaster->RecordDisc(FALSE,FALSE);

	pStorage->Release();

	//recorder->Eject();

    pDiskMaster->Close(); 

	delete pEvents;

	delete[] cTemp;
	return hr;

}

///////////////////////////////////////////////////////////////////
//Gravure des fichiers d'un album
///////////////////////////////////////////////////////////////////

int CGraveur::GraverFichierAlbum(IconeFileVector &m_IconeFileVector, const string &m_stgAlbumName, HWND hWnd)
{
	IDiscMaster* pDiskMaster;
	IStorage * pStorage;
	IStorage * pRepStorage;
	IconeFileVector::const_iterator fit;
	HRESULT hr;
	DWORD nBufferLength = 512;
	wchar_t cFilePath[512];
	char cFile[512];


	//GetTempPath(nBufferLength,NULL);

	char * cTemp = new char[nBufferLength + 1];
	GetTempPath(nBufferLength,cTemp);

	StringCchPrintf(cFile,512,"%swork2",cTemp);

	size_t m_sizeTConvert;
	mbstowcs_s(&m_sizeTConvert,cFilePath,512, cFile, 512);


	UINT_PTR pnCookie;
	IDiscMasterProgressEventsImpl *pEvents = new IDiscMasterProgressEventsImpl(hWnd);
	//Récupération du chemin par défaut

	 
	hr = StgCreateStorageEx(cFilePath,
										STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE,
										STGFMT_STORAGE,
										0, NULL, NULL,
										IID_IStorage,
										reinterpret_cast<void**>(&pStorage) );


	mbstowcs_s(&m_sizeTConvert,cFilePath,512, m_stgAlbumName.c_str(), m_stgAlbumName.length()+1);
	//mbstowcs(cFilePath, m_stgAlbumName.c_str(),  512);
	
	pStorage->CreateStorage(cFilePath,STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE, NULL, NULL,&pRepStorage);

	for (fit = m_IconeFileVector.begin(); fit!=m_IconeFileVector.end(); fit++)
	{

		//IStream * pStream,* iStream;

		//IStorage* pStg; 
		char * data = new char[32001];
		if (data == NULL)
		{
			CObjet::ErreurInformation();
		}
		IStream* pStream;

		string m_stgFilename;

		TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
		_splitpath_s((*fit).m_stgFullPathName, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);	

		//_splitpath((*fit).m_stgFullPathName, drive, dir, fname, ext);

		m_stgFilename = fname;
		m_stgFilename += ext;

		size_t m_sizeTConvert;
		mbstowcs_s(&m_sizeTConvert,cFilePath,512, m_stgFilename.c_str(), m_stgFilename.length()+1);
		//mbstowcs(cFilePath, m_stgFilename.c_str(),  512);

		hr = pRepStorage->CreateStream(cFilePath,STGM_READWRITE|STGM_SHARE_EXCLUSIVE,0,0,&pStream);
		
		//int fichier_source = _open((*fit).m_stgFullPathName, _O_RDONLY | _O_BINARY);

		int fichier_source = 0;

		if (_sopen_s(&fichier_source,(*fit).m_stgFullPathName,_O_RDONLY | _O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE)== -1) 
			return -1;
		if(fichier_source == -1)
			return -1;

		long octet_lus = _read(fichier_source, data, 32000);
		ULONG written;
		while(octet_lus > 0)
		{
			pStream->Write(data,octet_lus,&written);
			octet_lus = _read(fichier_source, data, 32000);
		}
		_close(fichier_source);

		delete[] data;

		pStream->Release();
	}

	pRepStorage->Release();

	hr = CoCreateInstance(CLSID_MSDiscMasterObj, NULL,CLSCTX_LOCAL_SERVER, 
				   IID_IDiscMaster, (void**)&pDiskMaster);

	hr = pDiskMaster->Open();

	if(pDiskMaster->ProgressAdvise(pEvents,&pnCookie) != S_OK )
	{
		return 0;
	}

    IJolietDiscMaster* joilet;
    hr = pDiskMaster->SetActiveDiscMasterFormat(IID_IJolietDiscMaster,(void**)&joilet );

    IEnumDiscRecorders* recordersEnum;
    hr = pDiskMaster->EnumDiscRecorders(&recordersEnum);

    ULONG num;
    IDiscRecorder* recorder;
    hr = recordersEnum->Next(1,&recorder,&num);


	hr = pDiskMaster->SetActiveDiscRecorder(recorder);

///////////////////////////////////
    hr = joilet->AddData(pStorage,1);  // HERE IT FAILS !!!


///////////////////////////////////
    hr = pDiskMaster->RecordDisc(FALSE,FALSE);

	pStorage->Release();

    pDiskMaster->Close(); 

	delete pEvents;

	delete[] cTemp;

	return hr;

}
///////////////////////////////////////////////////////////////////
//Gravure des fichiers
///////////////////////////////////////////////////////////////////

int CGraveur::GraverFichier(const string &m_stgFilename, HWND hWnd)
{
	IDiscMaster* pDiskMaster;
	IStorage * pStorage;
	UINT_PTR pnCookie;
	HRESULT hr;
	DWORD nBufferLength = 512;
	wchar_t cFilePath[512];
	char cFile[512];


	//GetTempPath(nBufferLength,NULL);

	char * cTemp = new char[nBufferLength + 1];
	GetTempPath(nBufferLength,cTemp);

	StringCchPrintf(cFile,512,"%swork2",cTemp);

	//mbstowcs(cFilePath, cFile,  512);
	size_t m_sizeTConvert;
	mbstowcs_s(&m_sizeTConvert,cFilePath,512, cFile, 512);

	//Récupération du chemin par défaut

	IDiscMasterProgressEventsImpl *pEvents = new IDiscMasterProgressEventsImpl(hWnd);
 
	hr = StgCreateStorageEx(cFilePath,
										STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE,
										STGFMT_STORAGE,
										0, NULL, NULL,
										IID_IStorage,
										reinterpret_cast<void**>(&pStorage) );


	hr = CoCreateInstance(CLSID_MSDiscMasterObj, NULL,CLSCTX_LOCAL_SERVER, 
				   IID_IDiscMaster, (void**)&pDiskMaster);

	hr = pDiskMaster->Open();

	if(pDiskMaster->ProgressAdvise(pEvents,&pnCookie) != S_OK )
	{
		return 0;
	}

    IJolietDiscMaster* joilet;
    hr = pDiskMaster->SetActiveDiscMasterFormat(IID_IJolietDiscMaster,(void**)&joilet );

    IEnumDiscRecorders* recordersEnum;
    hr = pDiskMaster->EnumDiscRecorders(&recordersEnum);

    ULONG num;
    IDiscRecorder* recorder;
    hr = recordersEnum->Next(1,&recorder,&num);

	char * data = new char[32001];
	if (data == NULL)
	{
		CObjet::ErreurInformation();
	}	
	IStream* pStream;
	string m_stgFilenameLocal;

	TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
	_splitpath_s(m_stgFilename.c_str(), drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);	

	m_stgFilenameLocal = fname;
	m_stgFilenameLocal += ext;

	mbstowcs_s(&m_sizeTConvert,cFilePath,512, m_stgFilenameLocal.c_str(), m_stgFilenameLocal.length()+1);

	hr = pStorage->CreateStream(cFilePath,STGM_READWRITE|STGM_SHARE_EXCLUSIVE,0,0,&pStream);
	
	int fichier_source = 0;//_open(m_stgFilename.c_str(), _O_RDONLY | _O_BINARY);

	if (_sopen_s(&fichier_source,m_stgFilename.c_str(),_O_RDONLY | _O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE)== -1) 
		return -1;
		if(fichier_source == -1)
			return -1;
	long octet_lus = _read(fichier_source, data, 32000);
	ULONG written;
	while(octet_lus > 0)
	{
		pStream->Write(data,octet_lus,&written);
		octet_lus = _read(fichier_source, data, 32000);
	}
	_close(fichier_source);

	delete[] data;

	pStream->Release();

	hr = pDiskMaster->SetActiveDiscRecorder(recorder);

///////////////////////////////////
    hr = joilet->AddData(pStorage,1);  // HERE IT FAILS !!!


///////////////////////////////////
    hr = pDiskMaster->RecordDisc(FALSE,FALSE);

	pStorage->Release();

    pDiskMaster->Close(); 


	delete pEvents;

	delete[] cTemp;

	return hr;

}