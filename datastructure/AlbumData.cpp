#include "stdafx.h"
#include ".\albumdata.h"

CAlbumData::CAlbumData(void)
{
	//initialisation du nom de l'album
	m_szAlbumName[0] = '\0';
	iNbFiles = 0;
}

CAlbumData::~CAlbumData(void)
{
}
