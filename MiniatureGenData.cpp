#include "stdafx.h"
#include ".\miniaturegendata.h"
#include <ItemInfo.h>

CMiniatureGenData::CMiniatureGenData(void)
{
	m_dwThreadID = 0;
	m_pItem = NULL;
	m_iNumItem = 0;
	m_iHauteur = 70;
	m_iLargeur = 90;
	hBitmap = NULL;
	m_bFolder = false;
}

CMiniatureGenData::~CMiniatureGenData(void)
{
	if(hBitmap != NULL)
		DeleteObject(hBitmap);

	hBitmap = NULL;
}

void CMiniatureGenData::SetItemInfo(CItemInfo * pItem)
{
	m_pItem = pItem;
}

CItemInfo * CMiniatureGenData::GetItemInfo()
{
	return m_pItem;
}