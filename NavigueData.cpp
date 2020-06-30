#include "stdafx.h"
#include ".\naviguedata.h"
#include <ibitmap.h>
#include <Regardsbitmap.h>
#include <BitmapEffect.h>
#include "scroll.h"
#include "Fichier.h"
#include "LoadImage.h"
#include "Dessin.h"
#include "ListPreviewAffWindow.h"
#include "Parameter.h"
using namespace Regards::libbitmap;

/////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////
CNavigueData::CNavigueData(void)
{
	m_ciBitmap = new CIBitmap();
	m_cScroll = new CScroll();
	m_cBitmap = new Regards::libbitmap::CBitmap(hInstance);
	m_cFichier = new CFichier();
	m_ChargementImage = new CLoadImage();
	cDirectory = NULL;
	cFile = NULL;
	cFirstFile = NULL;
	m_cDessin = new CDessin();
	m_cListPreviewAffWindow = NULL;
	m_hWnd = NULL;
	m_hWndPreview = NULL;
	m_bFullScreen = false;
	m_bRawFile = false;
	m_b3dDiaporama = false;
	m_iAction = 0;
	m_iHauteQualite = 0;
	m_iSequence = 0; // 0 : Pour Avant // 1: Pour Arriere
	m_bShrink = true; //Variable pour savoir si l'image sera pleine écran
	m_iDuree = 0;
	m_iSelect = 0;
	m_bCreateMiniature = false;
	m_bVideo =false;
	m_b3d = false;
	m_bIncludeSubFolder = false;
	m_bShowInfos = false;
	m_bGpGpu = CParameter::GetGpGpu();



}

void CNavigueData::SetGpGpu(bool bValue)
{
	m_bGpGpu = bValue;
}
bool CNavigueData::GetGpGpu()
{
	return m_bGpGpu;
}

Regards::libbitmap::CBitmap * CNavigueData::GetCBitmap()
{
	return m_cBitmap;
}

CIBitmap * CNavigueData::GetCIBitmap()
{
	return m_ciBitmap;
}

CScroll * CNavigueData::GetCScroll()
{
	return m_cScroll;
}

CDessin * CNavigueData::GetDessin()
{
	return m_cDessin;
}

CFichier * CNavigueData::GetCFichier()
{
	return m_cFichier;
}

int CNavigueData::GetSelect()
{
	return m_iSelect;
}

bool CNavigueData::GetVideo()
{
	return m_bVideo;
}

void CNavigueData::SetVideo(bool bValue)
{
	m_bVideo = bValue;
}


bool CNavigueData::GetFullScreen()
{
	return m_bFullScreen;
}

void CNavigueData::SetFullScreen(bool bValue)
{
	m_bFullScreen = bValue;
}

char * CNavigueData::GetCFile()
{
	char * out = NULL;
	#pragma omp critical(CFile)
	{
		out = cFile;
	}
	return out;
}

void CNavigueData::SetCFile(char * cValue)
{
	#pragma omp critical(CFile)
	{
		InitCFile(strlen(cValue) + 1);
		strcpy_s(cFile,strlen(cValue)+1,cValue);
	}
}

void CNavigueData::InitCFirstFile(int iSize)
{
	if(cFirstFile != NULL)
		delete[] cFirstFile;

	cFirstFile = new char[iSize];
}

char * CNavigueData::GetCFirstFile()
{
	char * out = NULL;
	#pragma omp critical(FirstFile)
	{
		out = cFirstFile;
	}
	return out;
}

void CNavigueData::SetCFirstFile(char * cValue)
{
	#pragma omp critical(FirstFile)
	{
		InitCFirstFile(strlen(cValue) + 1);
		strcpy_s(cFirstFile,strlen(cValue)+1,cValue);
	}
}

void CNavigueData::InitCFile(int iSize)
{
	if(cFile != NULL)
		delete[] cFile;

	cFile = new char[iSize];
}

void CNavigueData::InitCDirectory(int iSize)
{
	if(cDirectory != NULL)
		delete[] cDirectory;

	cDirectory = new char[iSize];
}

char * CNavigueData::GetCDirectory()
{
	char * cOut = NULL;
	#pragma omp critical(Directory)
	{
		cOut = cDirectory;
		
	}
	return cOut;
}

void CNavigueData::SetCDirectory(char * cValue)
{
	#pragma omp critical(Directory)
	{
		InitCDirectory(strlen(cValue) + 1);
		strcpy_s(cDirectory,strlen(cValue)+1,cValue);
	}
}


void CNavigueData::SetHauteQualite(int iValue)
{
	if(iValue >= 0)
		m_iHauteQualite = iValue;
}

int CNavigueData::GetHauteQualite()
{
	return m_iHauteQualite;
}

void CNavigueData::SetCreateMiniature(bool bValue)
{
	m_bCreateMiniature = bValue;
}

bool CNavigueData::GetCreateMiniature()
{
	return m_bCreateMiniature;
}

void CNavigueData::Set3D(bool bValue)
{
	m_b3d = bValue;
}

bool CNavigueData::Get3D()
{
	return m_b3d;
}

void CNavigueData::Set3DDiaporama(bool bValue)
{
	m_b3dDiaporama = bValue;
}

bool CNavigueData::Get3DDiaporama()
{
	return m_b3dDiaporama;
}

void CNavigueData::SetShrink(bool bValue)
{
	m_bShrink = bValue;
}

bool CNavigueData::GetShrink()
{
	return m_bShrink;
}

void CNavigueData::SetIncludeSubFolder(bool bValue)
{
	m_bIncludeSubFolder = bValue;
}

void CNavigueData::SetSelect(int iValue)
{
	m_iSelect = iValue;
}

void CNavigueData::SetHWnd(HWND hWnd)
{
	m_hWnd = hWnd;
}

HWND CNavigueData::GetHWnd()
{
	return m_hWnd;
}

void CNavigueData::SetPreviewHWnd(HWND hWnd)
{
	m_hWndPreview = hWnd;
}

HWND CNavigueData::GetPreviewHWnd()
{
	return m_hWndPreview;
}

CListPreviewAffWindow * CNavigueData::GetListPreview()
{
	return m_cListPreviewAffWindow;
}

CFichier * CNavigueData::GetFichier()
{
	return m_cFichier;
}

int CNavigueData::GetSequence()
{
	return m_iSequence;
}

void CNavigueData::SetSequence(int iValue)
{
	m_iSequence = iValue;
}

void CNavigueData::SetSubFolder(bool bValue)
{
	m_bIncludeSubFolder = bValue;
}

bool CNavigueData::GetSubFolder()
{
	return m_bIncludeSubFolder;
}

int CNavigueData::GetDuree()
{
	return m_iDuree;
}

void CNavigueData::SetDuree(int iValue)
{
	m_iDuree = iValue;
}

void CNavigueData::SetRawFile(bool bValue)
{
	m_bRawFile = bValue;
}

bool CNavigueData::GetRawFile()
{
	return m_bRawFile;
}

void CNavigueData::SetShowInfos(bool bValue)
{
	m_bShowInfos = bValue;
}

bool CNavigueData::GetShowInfos()
{
	return m_bShowInfos;
}
/////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////
CNavigueData::~CNavigueData(void)
{
	if(m_ciBitmap != NULL)
		delete m_ciBitmap;

	m_ciBitmap = NULL;

	if(m_cScroll != NULL)
		delete m_cScroll;

	m_cScroll = NULL;

	if(m_cBitmap != NULL)
		delete m_cBitmap;

	m_cBitmap = NULL;

	if(m_cFichier != NULL)
		delete m_cFichier;

	m_cFichier = NULL;

	if(m_ChargementImage != NULL)
		delete m_ChargementImage;

	m_ChargementImage = NULL;

	if(cDirectory != NULL)
		delete[] cDirectory;

	cDirectory = NULL;

	if(cFile != NULL)
		delete[] cFile;

	cFile = NULL;

	if(cFirstFile != NULL)
		delete[] cFirstFile;

	cFirstFile = NULL;

	if(m_cDessin != NULL)
		delete m_cDessin;

	m_cDessin = NULL;

	if(m_cListPreviewAffWindow != NULL)
		delete m_cListPreviewAffWindow;

	m_cListPreviewAffWindow = NULL;
}

void CNavigueData::SetCIBitmap(CIBitmap * ciBitmap)
{
	//CIBitmap * m_ciBitmapTemp = m_ciBitmap;
	
	#pragma omp critical(CIBITMAP)
	{
		if(m_ciBitmap != NULL)
			delete m_ciBitmap;
		
		m_ciBitmap = NULL;

		m_ciBitmap = ciBitmap;
	}
}