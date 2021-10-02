#include "stdafx.h"
#include ".\treewindowdata.h"
#include "statusbar.h"
#include "list.h"
#include "preview.h"
#include "listpreview.h"
#include "pane.h"
#include "registryaccess.h"

CTreeWindowData::CTreeWindowData(void)
{
	bDragging = 0;
	iDragEnable = 1;
	cList = NULL;
	m_ListPreview = NULL;
	m_Preview = NULL;
	g_pcm2 = NULL;
	g_pMalloc = NULL;
	pDataObj = NULL;
	m_cPane = NULL;
	m_IconeFileVectorAlbum = NULL;
	iColumnUp = 0;
	m_bClick = false;
	m_bCaptureList = false;
	m_bKeyDown = false;
	m_bClickDown = false;
	iCount = 0;
	iCreerRepertoire = 0;
	iTypeData = 0; //0 : Fichier, 1 : Album;
	iDelete = 0;
	bDragging = 0;
	iSubItem = 0;
	iAffichageDimension = 0;
	iAffichageAfterMiniature = 0;
	iDemarre = 0;
	iTypeAffichage = 0;
	iNumItem = 0;
	iNumSelectedItem = 0;
	iInfosFichier = 0;
	hDragImageList = NULL;
	hThread = NULL;
	iMarquee = 0;
	iPauseMonitor = 0;

	cDirectory[0] = '\0';
	cFileTemp[0] = '\0';
	cTempDir[0] = '\0';
	cFile[0] = '\0';
}

CTreeWindowData::~CTreeWindowData(void)
{

	if(m_cPane != NULL)
		delete m_cPane;

	m_cPane = NULL;

	if(cList != NULL)
		delete cList;

	cList = NULL;

	if(m_ListPreview != NULL)
	{
		if(m_ListPreview->IsWindowVisible())
			m_ListPreview->SauverListe();

		delete m_ListPreview;

		m_ListPreview = NULL;
	}

	if(m_Preview != NULL)
		delete m_Preview;

	m_Preview = NULL;

	if(hThread != NULL)
	{
		TerminateThread(hThread,1);
		CloseHandle(hThread);
		hThread = NULL;
	}

	if(g_pMalloc != NULL)
	{
		g_pMalloc->Release();
		g_pMalloc = NULL;
	}

	m_vectItem.clear();
}

int CTreeWindowData::SaveWindowPosition()
{
	WINDOWINFO _windowInfo;
	int _iPreview = 0;
	int _iListSize = 0;
	GetWindowInfo(m_cPane->m_hWnd,&_windowInfo);
	_iPreview = _windowInfo.rcClient.right - _windowInfo.rcClient.left;
	GetWindowInfo(cList->m_hWnd,&_windowInfo);
	_iListSize = _windowInfo.rcClient.right - _windowInfo.rcClient.left;

	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->WriteOrCreateKeyValue("SOFTWARE\\Regards\\exploreur") == ERROR_SUCCESS)
	{
		m_cRegistryAccess->WriteIntValue(_iPreview,"PreviewWindowSize");
		m_cRegistryAccess->WriteIntValue(_iListSize,"ListPreviewWindowSize");
		
	}
	delete m_cRegistryAccess;
	return 0;
}

int CTreeWindowData::LoadWindowPosition(int &iPreview, int &iList)
{
	WINDOWINFO _windowInfo;
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	if(m_cRegistryAccess->OpenKeyValue("SOFTWARE\\Regards\\exploreur") == ERROR_SUCCESS)
	{
		iPreview = m_cRegistryAccess->ReadIntValue("PreviewWindowSize");
		if(iPreview == -1)
		{
			delete m_cRegistryAccess;
			return -1;
		}
		
		iList = m_cRegistryAccess->ReadIntValue("ListPreviewWindowSize");

	}
	else
	{
		delete m_cRegistryAccess;
		return -1;
	}
	delete m_cRegistryAccess;
	return 0;
}