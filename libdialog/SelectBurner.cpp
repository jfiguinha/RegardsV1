#include "StdAfx.h"
#include ".\selectburner.h"
#include "resource.h"
#include <aspidriver.h>

CSelectBurner::CSelectBurner(HINSTANCE hInstance)
{
	m_hInstance = hInstance;
	m_cAspiDriver = new CAspiDriver();
}

CSelectBurner::~CSelectBurner(void)
{
	if(m_cAspiDriver != NULL)
		delete m_cAspiDriver;
	
	m_cAspiDriver = NULL;
}

LRESULT CSelectBurner::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hWnd;
	LoadString(m_hInstance,IDS_STGSELECTBURNER,m_szMessage,255);
	//Initialisation de la liste des graveurs
	SetWindowText(hDlg,m_szMessage);

	hWnd = GetDlgItem(hDlg,IDC_STLIBELLE);
	LoadString(m_hInstance,IDS_STGSELBURN,m_szMessage,255);
	SetWindowText(hWnd,m_szMessage);

	int iCount = 0;
	int iNbDevice = m_cAspiDriver->GetDeviceCount();

	hWnd = GetDlgItem(hDlg,IDC_CMBALBUM);

	for(int i = 0;i < iNbDevice;i++)
	{
		char m_cValue[256];
		string m_stgVendor,m_stgProduct,m_stgRevision,m_stgBusAddress;
		m_cAspiDriver->SetDevice(i);
		if(m_cAspiDriver->ItCanWrite())
		{
			iTabIndice[iCount] = i;
			m_cAspiDriver->GetDeviceString(m_stgVendor,m_stgProduct,m_stgRevision,m_stgBusAddress);
			//Test si une des chaines suivantes apparait pour savoir si c'est un graveur ou non
			StringCchPrintf(m_cValue,256,"%s %s %s",m_stgVendor.c_str(),m_stgProduct.c_str(),m_stgRevision.c_str());
			SendMessage(hWnd, CB_INSERTSTRING , iCount, (LPARAM)(LPCTSTR)m_cValue);
			SendMessage(hWnd, WM_SETTEXT , (WPARAM)0, (LPARAM)m_cValue);

			iCount++;
		}
	}
	SendMessage(hWnd, CB_SETCURSEL, 0, 0);

	if(iCount == 0)
		EndDialog(hDlg,0);

	return 0;
}

LRESULT CSelectBurner::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{
		case IDC_BUTTON1:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;

		case IDOK:
			{
				HWND hWnd = GetDlgItem(hDlg,IDC_CMBALBUM);
				//Recupération du graveur sélectionné
				int iSelection = SendMessage(hWnd, CB_GETCURSEL, 0, 0);
				iSelection = iTabIndice[iSelection];
				m_cAspiDriver->SetDevice(iSelection);

				HCURSOR  hCur = LoadCursor(NULL, IDC_WAIT);
				hCur = SetCursor(hCur);
				//Vérification du media
				if(m_cAspiDriver->InfosMediaErasable())
				{
					m_cAspiDriver->EraseMedia(true);
					m_cAspiDriver->EjectCD(true);
				}
				else
				{
					LoadString(m_hInstance,IDS_ERREUR,m_szCaption,255);
					LoadString(m_hInstance,IDS_STGCDNORW,m_szMessage,255);
					MessageBox(NULL,m_szMessage,m_szCaption,0);
				}

				SetCursor(LoadCursor(NULL, IDC_ARROW));
				EndDialog(hDlg, 1);
			}
			return TRUE;

		case IDCANCEL:
			EndDialog(hDlg, 0);
			return TRUE;
	}
	return 0;
}
