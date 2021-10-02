

#ifndef __PREVIEWDATA__H__
#define __PREVIEWDATA__H__

#include <attribut.h>
#include <iconefile.h>
#include <ibitmap.h>
#include <exif.h>
#include "LoadImage.h"
class CWebInfoFichier;


class CPreviewData
{
public:
	CPreviewData(void);
	~CPreviewData(void);
	void OnDestroy();


	CLoadImage m_ChargementImage;
	CIBitmap m_ciBitmap;
	//CWebInfoFichier * m_cWebInfoFichier;
	Cexif m_cExif;
	HWND hWndStaticBitmap;
	HWND hWndPreview;
	HWND hWndMP3;
	HWND hWndInfosFichier;
	HWND hMCI;
	int m_bSmooth;
	HBITMAP hBitmap;
	RECT rcTemp;
	RECT rcVideo;
	char cDirectory[MAX_PATH];
	char cFile[MAX_PATH];
	int iTypeData; //0 : Aucun, 1 : Image, 2: Video, 3 : MP3

	int iShowVideo;
	CIconeFile * IconeFileTemp;
	AttributVector * m_AttributVector;
	int iAlbum;
	int iAffichageInfos;
};

#endif