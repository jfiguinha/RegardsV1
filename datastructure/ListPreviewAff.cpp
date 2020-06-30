#include "stdafx.h"
#include ".\listpreviewaff.h"

CListPreviewAff::CListPreviewAff(void)
{
	cDirectory[0] = '\0';
	iTypeData = 0;
	cNumAlbum = 0;
	m_IconeFileVector = NULL;
	hWnd = NULL;
	hParent = NULL;
}

CListPreviewAff::~CListPreviewAff(void)
{
}
