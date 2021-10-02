#include "stdafx.h"
#include "GestionTraitementWindow.h"
#include "traitementBitmap.h"

//---------------------------------------------------------
//Suppression de l'ensemble des fenêtres
//---------------------------------------------------------
void CGestionTraitementWindow::DestroyAllTraitementWindow()
{
	TraitementInstanceVector::const_iterator fit;
	
	for (fit = m_TraitementIntanceVector.begin(); fit!= m_TraitementIntanceVector.end(); fit++)
	{
		DestroyWindow((*fit).hWnd);
		CloseHandle((*fit).hThreadTraitement);
	}

	m_TraitementIntanceVector.clear();
}

//---------------------------------------------------------
//Création d'une fenêtre de traitement
//---------------------------------------------------------
void CGestionTraitementWindow::CreateTraitementWindow(const char * filename,HWND hWndParent)
{
	CTraitementInstance m_TraitementInstance;

	hwndAffichageBitmap = hWndParent;
	strcpy_s(m_cFileName,MAX_PATH,filename);

	m_TraitementInstance.hWnd = NULL;
	m_TraitementInstance.hThreadTraitement = (HANDLE)CreateThread(NULL, 1024,
				TraitementBMP, this, 0, &m_TraitementInstance.dwThreadId);

	SetThreadPriority(m_TraitementInstance.hThreadTraitement, THREAD_PRIORITY_NORMAL);

	m_TraitementIntanceVector.push_back(m_TraitementInstance);
}

//---------------------------------------------------------
//Affectation d'une fenêtre de traitement
//---------------------------------------------------------
void CGestionTraitementWindow::SetTraitementWindow(HWND hWnd)
{
	TraitementInstanceVector::iterator fit;
	fit = m_TraitementIntanceVector.begin();
	fit += m_iNbTraitementInstance;
	fit->hWnd = hWnd;
	m_iNbTraitementInstance++;
}

//---------------------------------------------------------
//Suppression d'une fenêtre
//---------------------------------------------------------
void CGestionTraitementWindow::DeleteSelectTraitementWindow(HWND hWndLocal)
{
	TraitementInstanceVector::iterator fit;
	
	for (fit = m_TraitementIntanceVector.begin(); fit!=m_TraitementIntanceVector.end(); fit++)
	{
		if((*fit).hWnd == hWndLocal)
		{
			if((*fit).hThreadTraitement != NULL)
			{
				CloseHandle((*fit).hThreadTraitement);
				m_iNbTraitementInstance--;
			}
			m_TraitementIntanceVector.erase(fit);
			break;
		}
	}
}

//---------------------------------------------------------
//Thread de création d'une fenêtre de traitement
//---------------------------------------------------------
ULONG __stdcall CGestionTraitementWindow::TraitementBMP(LPVOID parametre)
{
	OleInitialize(NULL);
	char m_cTemp[255];
	CGestionTraitementWindow * m_AffichageBitmap = (CGestionTraitementWindow *)parametre;
	CTraitementBitmap * m_cTraitementBMP = new CTraitementBitmap();
	m_cTraitementBMP->SetFilename(m_AffichageBitmap->m_cFileName);
	RECT  rc = {0, 0, GetDeviceCaps(GetDC(NULL),HORZRES), GetDeviceCaps(GetDC(NULL),VERTRES)};
	DWORD dwndStlyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_MAXIMIZE;
	DWORD dStyle = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	DWORD dExStyle = 0;

	LoadString(hInstance,IDS_TITLE_TRTBITMAP,m_cTemp,255);

	m_cTraitementBMP->SetBackgroundColor((HBRUSH)GetStockObject(BLACK_BRUSH));
	m_cTraitementBMP->CreateEx(NULL,hInstance,dwndStlyle,dStyle,dExStyle,m_cTemp,IDM_REGARDS,IDM_TRAITEMENTIMAGE,IDC_ARROW,rc);
	m_cTraitementBMP->SetParent(m_AffichageBitmap->hwndAffichageBitmap);
	m_cTraitementBMP->Run(IDR_TRTBITMAP);
	delete m_cTraitementBMP;
	ExitThread(0);
	OleUninitialize();
	return 0;
}