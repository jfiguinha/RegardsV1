#include "stdafx.h"
#include ".\iteminfo.h"
#include <pidl.h>

CItemInfo::CItemInfo(void)
{
	m_szParseName = NULL;
	m_szFilename = NULL;
	pidlRel = NULL;
	pidlFQ = NULL;
	pParentFolder = NULL;

	m_lSize[0] = '\0';
	m_lDate[0] = '\0';
	m_lDimension[0] = '\0';
}

CItemInfo::CItemInfo(CItemInfo * p2)
{
	m_szParseName = NULL;
	m_szFilename = NULL;
	pidlRel = NULL;
	pidlFQ = NULL;
	pParentFolder = NULL;

	//pParentFolder = p2->pParentFolder;
	//SetPidlFQ(p2->pidlFQ);
	//SetPidlRel( p2->pidlRel);

	SetParseName(p2->m_szParseName);
	SetFileName(p2->m_szFilename);

	StringCchCopy(m_lSize,20,p2->m_lSize);
	StringCchCopy(m_lDate,20,p2->m_lDate);
	StringCchCopy(m_lDimension,20,p2->m_lDimension);

	iImage = p2->iImage;
	m_iPos = p2->m_iPos;
	m_iTypeDonne = p2->m_iTypeDonne;
}

CItemInfo::~CItemInfo(void)
{
	CPidl * m_cPidl = new CPidl();

	if(m_szParseName != NULL)
		delete[] m_szParseName;

	m_szParseName = NULL;

	if(m_szFilename != NULL)
		delete[] m_szFilename;

	m_szFilename = NULL;

	if(pidlFQ != NULL)
		m_cPidl->Pidl_Free(pidlFQ);

	pidlFQ = NULL;
	
	if(pidlRel != NULL)
		m_cPidl->Pidl_Free(pidlRel);

	pidlRel = NULL;

	if(pParentFolder != NULL)
		pParentFolder->Release();


	delete m_cPidl;
}

bool CItemInfo::operator == (const CItemInfo &p2) const
{
	if(pParentFolder != p2.pParentFolder)
		return false;

	if(pidlFQ != p2.pidlFQ)
		return false;

	if(pidlRel != p2.pidlRel)
		return false;

	if(strcmp(p2.m_szParseName,m_szParseName) != 0)
		return false;

	if(strcmp(p2.m_szFilename,m_szFilename) != 0)
		return false;

	if(strcmp(p2.m_lSize,m_lSize) != 0)
		return false;

	if(strcmp(p2.m_lDate,m_lDate) != 0)
		return false;

	if(strcmp(p2.m_lDimension,m_lDimension) != 0)
		return false;

	if(iImage != p2.iImage)
		return false;

	if(m_iPos != p2.m_iPos)
		return false;

	if(m_iTypeDonne != p2.m_iTypeDonne)
		return false;

	return true;
}

void CItemInfo::SetPidlFQ(LPITEMIDLIST lpItem)
{
	CPidl * m_cPidl = new CPidl();
	if(pidlFQ != NULL)
		m_cPidl->Pidl_Free(pidlFQ);
	
	pidlFQ = m_cPidl->Pidl_Copy(lpItem);
	delete m_cPidl;
}

void CItemInfo::SetPidlRel(LPITEMIDLIST lpItem)
{
	CPidl * m_cPidl = new CPidl();
	if(pidlRel != NULL)
		m_cPidl->Pidl_Free(pidlRel);
	
	pidlRel = m_cPidl->Pidl_Copy(lpItem);
	delete m_cPidl;
}