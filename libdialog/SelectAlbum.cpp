#include "StdAfx.h"
#include ".\selectalbum.h"
#include "resource.h"

CSelectAlbum::CSelectAlbum(void)
{
	m_VectorAlbum = NULL;
	m_iReturnValue = 0;
}

CSelectAlbum::~CSelectAlbum(void)
{
}

/////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////
LRESULT CSelectAlbum::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	AlbumDataVector::iterator AlbumIt;
	int iNumber = 0;
	//Initialisation de la combo Box
	for (AlbumIt = m_VectorAlbum->begin(); AlbumIt!= m_VectorAlbum->end(); AlbumIt++)
	{
		CAlbumData m_AlbumTemp = *AlbumIt;
		SendDlgItemMessage(hDlg, IDC_CMBALBUM, CB_INSERTSTRING , iNumber, (LPARAM)(LPCTSTR)m_AlbumTemp.m_szAlbumName);
		SendDlgItemMessage(hDlg, IDC_CMBALBUM, WM_SETTEXT , (WPARAM)0, (LPARAM)m_AlbumTemp.m_szAlbumName);
		iNumber++;

	}
	SendDlgItemMessage(hDlg, IDC_CMBALBUM, CB_SETCURSEL, 0, 0);
	return 0;
}

/////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////
LRESULT CSelectAlbum::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{
		case IDC_BUTTON1:
			m_iReturnValue = LOWORD(wParam);
			EndDialog(hDlg, LOWORD(wParam));
			break;

		case IDOK:
			{
				int iValue, iPos = 0;
				CAlbumData m_AlbumTemp;
				AlbumDataVector::iterator AlbumIt;
				//Retour du nom de l'album
				
				iValue = SendDlgItemMessage(hDlg, IDC_CMBALBUM, CB_GETCURSEL, 0, 0);
				for (AlbumIt = m_VectorAlbum->begin(); AlbumIt!= m_VectorAlbum->end(); AlbumIt++)
				{
					//Album m_AlbumTemp = *AlbumIt;
					if(iPos == iValue)
					{
						m_stgAlbumName = AlbumIt->m_szAlbumName;
						iValue = iPos;
						break;
					}
					iPos++;
				}

				m_iReturnValue = LOWORD(iValue);
				EndDialog(hDlg, LOWORD(iValue));
			}
			break;

		case IDCANCEL:
			m_iReturnValue = -1;
			EndDialog(hDlg, -1);
			break;
	}
	return 0;
}