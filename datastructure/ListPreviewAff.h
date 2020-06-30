#ifndef __LISTPREVIEWAFF_H__
#define __LISTPREVIEWAFF_H__

#include "iconefile.h"


class CListPreviewAff
{
public:
	CListPreviewAff(void);
	~CListPreviewAff(void);

	int iTypeData;
	char cDirectory[MAX_PATH];
	int cNumAlbum;
	IconeFileVector * m_IconeFileVector;
	HWND hWnd;
	HWND hParent;
};

#endif