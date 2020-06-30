#include "StdAfx.h"
#include "mp3dialog.h"
#include "resource.h"
#include "Fichier.h"
#include <mp3input.h>
#include <mp3.h>

CMP3Dialog::CMP3Dialog(void)
{
	m_cMP3 = new CMP3();
	m_cFichier = new CFichier();
	m_bPause = false;
	szFileName[0] = '\0';
	m_iSeconde = 0;
}

CMP3Dialog::~CMP3Dialog(void)
{
	if(m_cMP3 != NULL)
	{
		m_cMP3->xmMP3_close();
		m_cMP3->xmMP3_free();
		delete m_cMP3;
	}

	m_cMP3 = NULL;

	if(m_cFichier != NULL)
		delete m_cFichier;

	m_cFichier = NULL;
}

void CMP3Dialog::SecondToTime(const int &m_iSecond, char * szTime)
{
	int m_iSec = m_iSecond % 60;
	int m_iMinute = m_iSecond / 60;

	if(m_iMinute < 10 && m_iSec < 10)
	{
		StringCchPrintf(szTime,50,"0%d:0%d",m_iMinute,m_iSec);
	}
	else if(m_iMinute < 10)
	{
		StringCchPrintf(szTime,50,"0%d:%2d",m_iMinute,m_iSec);
	}
	else if(m_iSec < 10)
	{
		StringCchPrintf(szTime,50,"%2d:0%d",m_iMinute,m_iSec);
	}
	else
	{
		StringCchPrintf(szTime,50,"%2d:%2d",m_iMinute,m_iSec);
	}
}	

void CMP3Dialog::DrawPicture(HWND hDlg)
{
	HIMAGELIST himlHot3 = ImageList_LoadImage(hInstance,MAKEINTRESOURCE(IDB_BTPLAYER),17,5,RGB(255,0,255),IMAGE_BITMAP,LR_CREATEDIBSECTION);

	SendDlgItemMessage(hDlg, IDC_BTPLAY,  BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)ImageList_GetIcon(himlHot3,0,ILD_TRANSPARENT));
	SendDlgItemMessage(hDlg, IDC_BTSTOP,  BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)ImageList_GetIcon(himlHot3,1,ILD_TRANSPARENT));
	SendDlgItemMessage(hDlg, IDC_BTPAUSE,  BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)ImageList_GetIcon(himlHot3,2,ILD_TRANSPARENT));
	SendDlgItemMessage(hDlg, IDC_BTSUIVANT,  BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)ImageList_GetIcon(himlHot3,3,ILD_TRANSPARENT));
	SendDlgItemMessage(hDlg, IDC_BTPRECEDENT,  BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)ImageList_GetIcon(himlHot3,4,ILD_TRANSPARENT));

	ImageList_Destroy(himlHot3);
}

LRESULT CMP3Dialog::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//ZeroMemory(&pInfo,sizeof(CMP3Input::InputInfo));
	m_cMP3->xmMP3_init(0);
	m_iSeconde = 0;

	SendDlgItemMessage(hDlg, IDC_FILENAME, WM_SETTEXT , (WPARAM)0, (LPARAM)"");
	SendDlgItemMessage(hDlg, IDC_DUREEINFO, WM_SETTEXT , (WPARAM)0, (LPARAM)"00:00");
	SendDlgItemMessage(hDlg, IDC_SLDDUREE, TBM_SETRANGEMAX, (WPARAM) 1, (LPARAM) m_iSeconde); 

	DrawPicture(hDlg);
	return 0;
}

LRESULT CMP3Dialog::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
		case IDC_BTPRECEDENT:
			{
				char FileName[MAX_PATH];
				m_cFichier->AffichageImagePrecedente(FileName,MAX_PATH);
				SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_BTFICHIER,0),(LPARAM)FileName);
				SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_BTPLAY,0),0);
			}
			break;
		case IDC_BTSUIVANT:
			{
				char FileName[MAX_PATH];
				m_cFichier->AffichageImageSuivante(FileName,MAX_PATH);
				SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_BTFICHIER,0),(LPARAM)FileName);
				SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_BTPLAY,0),0);
			}
			break;
		case IDOK:
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;

		case IDCANCEL:
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;

		case IDC_POS:
			{
				char Temp[10];
				m_iSeconde = (int)lParam;
				SecondToTime(m_iSeconde,Temp);
				SendDlgItemMessage(hDlg, IDC_DUREEINFO, WM_SETTEXT , (WPARAM)0, (LPARAM)Temp);
				SendDlgItemMessage(hDlg, IDC_SLDDUREE, TBM_SETPOS, (WPARAM) 1, (LPARAM) m_iSeconde);
			}
			break;

		case IDC_FINFICHIER:
			{
				CMP3Input::InputInfo pInfo;
				m_cFichier->AffichageImageSuivante(szFileName,MAX_PATH);
				SendDlgItemMessage(hDlg, IDC_SLDDUREE, TBM_SETPOS, (WPARAM) 1, (LPARAM) 0); 
				ZeroMemory(&pInfo,sizeof(CMP3Input::InputInfo));
				m_cMP3->xmMP3_stop();
				m_cMP3->xmMP3_close();

				TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
				_splitpath_s(szFileName, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);	
				
				m_cMP3->xmMP3_open(szFileName, &pInfo);

				StringCchPrintf(dir,255,"%s%s",fname,ext);
				SendDlgItemMessage(hDlg, IDC_FILENAME, WM_SETTEXT , (WPARAM)0, (LPARAM)dir);

				m_iSeconde = pInfo.totalSec;

				SendDlgItemMessage(hDlg, IDC_SLDDUREE, TBM_SETRANGEMAX, (WPARAM) 1, (LPARAM) m_iSeconde); 

				m_cMP3->xmMP3_play();
			}
			break;


		case IDC_BTFICHIER:
			{
				CMP3Input::InputInfo pInfo;
				SendDlgItemMessage(hDlg, IDC_SLDDUREE, TBM_SETPOS, (WPARAM) 1, (LPARAM) 0); 
				ZeroMemory(&pInfo,sizeof(CMP3Input::InputInfo));

				//hWnd = hDlg;
				m_cMP3->xmMP3_setWindow(hDlg);

				m_cMP3->xmMP3_stop();
				m_cMP3->xmMP3_close();

				StringCchCopy(szFileName,MAX_PATH,(const char *)lParam);

				m_cFichier->DefinirRepertoire(szFileName,0,"*.mp3");

				TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
				_splitpath_s(szFileName, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);	

				m_cMP3->xmMP3_open(szFileName, &pInfo);

				StringCchPrintf(dir,255,"%s%s",fname,ext);
				SendDlgItemMessage(hDlg, IDC_FILENAME, WM_SETTEXT , (WPARAM)0, (LPARAM)dir);

				m_iSeconde = pInfo.totalSec;

				SendDlgItemMessage(hDlg, IDC_SLDDUREE, TBM_SETRANGEMAX, (WPARAM) 1, (LPARAM) m_iSeconde); 
			}
			break;

		case IDC_BTPLAY:
			if(m_bPause)
			{
				m_cMP3->xmMP3_restart();
				m_bPause = false;
			}
			else
				m_cMP3->xmMP3_play();
			break;

		case IDC_BTSTOP:
			m_cMP3->xmMP3_stop();
			break;

		case IDC_BTPAUSE:
			m_bPause = true;
			m_cMP3->xmMP3_pause();
			break;
	}
	return 0;
}

LRESULT CMP3Dialog::OnPaint(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DrawPicture(hDlg);
	return 0;
}

LRESULT CMP3Dialog::OnHScroll(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//Mise à jour des données de l'affichage
    // User moved the slider.
    switch(LOWORD(wParam))
    {
		case TB_PAGEDOWN:
		case SB_THUMBTRACK:
		case TB_PAGEUP:
			m_iSeconde = SendDlgItemMessage(hDlg, IDC_SLDDUREE,	TBM_GETPOS, 0, 0);
			m_cMP3->xmMP3_seek(m_iSeconde);

			break;
	}

	return 0;
}
