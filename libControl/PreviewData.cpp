#include "stdafx.h"
#include ".\previewdata.h"
#include "fichier.h"

#include "webinfofichier.h"



CPreviewData::CPreviewData(void)
{
	iShowVideo = 0;
	iTypeData = 0;
	//m_ChargementImage = new CLoadImage();
	//m_ciBitmap = new CIBitmap();
	StringCchCopy(cDirectory,MAX_PATH,"c:");
	iAffichageInfos = 1;

	//m_cExif = NULL;
	hWndStaticBitmap = NULL;
	hWndPreview = NULL;
	hWndMP3 = NULL;
	hWndInfosFichier = NULL;
	hMCI = NULL;
	m_bSmooth = 0;
	HBITMAP hBitmap = NULL;

	cFile[0] = '\0';
	IconeFileTemp = NULL;
	m_AttributVector = NULL;
	iAlbum = 0;
}

CPreviewData::~CPreviewData(void)
{
	//if(m_cExif != NULL)
	//	delete m_cExif;

	//m_cExif = NULL;

	if(hBitmap != NULL)
		DeleteObject(hBitmap);

	hBitmap = NULL;


	if(hWndMP3 != NULL)
		DestroyWindow(hWndMP3);

	if(hWndInfosFichier != NULL)
		DestroyWindow(hWndInfosFichier);

	if(hWndStaticBitmap != NULL)
		DestroyWindow(hWndStaticBitmap);

	//if(m_ChargementImage != NULL)
	//	delete m_ChargementImage;
		
	//if(m_ciBitmap != NULL)
	//	delete m_ciBitmap;

	//m_ciBitmap = NULL;
	hWndMP3 = NULL;
	hWndStaticBitmap = NULL;
	//m_ChargementImage = NULL;
	//m_ciBitmap = NULL;

}

void CPreviewData::OnDestroy()
{
	if(hWndMP3 != NULL)
		DestroyWindow(hWndMP3);
	
	if(hWndInfosFichier != NULL)
		DestroyWindow(hWndInfosFichier);

	if(hWndStaticBitmap != NULL)
		DestroyWindow(hWndStaticBitmap);

	hWndMP3 = NULL;
	hWndStaticBitmap = NULL;
	hWndInfosFichier = NULL;
}
