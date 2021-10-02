#include <stdafx.h>
#include "diaporama.h"
#include "resource.h"
#include "parameter.h"

CDiaporama::CDiaporama(HINSTANCE hInstance)
{
	m_hLocalInstance = hInstance;
}

CDiaporama::~CDiaporama()
{
}

LRESULT CDiaporama::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CParameter::GetOptionDiaporama(iSequence, iDuree, m_bShrink, m_bHauteQualite, m_bFullScreen, m_b3D, m_i3DQuality);

	//Initiation de l'affichage de la durée

	hWndAffichage = GetParent(hDlg);

	StringCchPrintf(cDuree,50,"%d", iDuree);

	SendDlgItemMessage(hDlg, 
	IDC_EDITDELAI, 
	WM_SETTEXT, 
	(WPARAM) strlen(cDuree), 
	(LPARAM) cDuree); 

	//Check en avant
	if(iSequence)
		CheckDlgButton(hDlg,IDC_SEQRADAVANT,BST_CHECKED);
	else
		CheckDlgButton(hDlg,IDC_SEQRADARRIERRE,BST_CHECKED);

	//Shrink Image
	if(m_bShrink)
		CheckDlgButton(hDlg,IDC_CHECKSHRINK,BST_CHECKED);

				//Shrink Image
	if(m_bHauteQualite)
		CheckDlgButton(hDlg,IDC_QUALITE,BST_CHECKED);


	//FullScreen
	if(m_bFullScreen)
		CheckDlgButton(hDlg,IDC_FULLSCREEN,BST_CHECKED);

	if(m_b3D)
		CheckDlgButton(hDlg,IDC_CHECKVISIONAGE3D,BST_CHECKED);

	switch(m_i3DQuality)
	{
		case 3:
			CheckDlgButton(hDlg,IDC_RDTRESHAUTE,BST_CHECKED);
			break;
		case 2:
			CheckDlgButton(hDlg,IDC_RDHAUTE,BST_CHECKED);
			break;
		case 1:
			CheckDlgButton(hDlg,IDC_RDMOYENNE,BST_CHECKED);
			break;
		case 0:
			CheckDlgButton(hDlg,IDC_RDBASSE,BST_CHECKED);
			break;
	}

	//Initiation du Slider

	SendDlgItemMessage(hDlg, 
		IDC_SLIDERDELAI, 
		TBM_SETRANGEMAX, 
		(WPARAM) 1, 
		(LPARAM) 100); 

	SendDlgItemMessage(hDlg, 
		IDC_SLIDERDELAI, 
		TBM_SETRANGEMIN , 
		(WPARAM) 1, 
		(LPARAM) 1); 

	SendDlgItemMessage(hDlg, 
		IDC_SLIDERDELAI, 
		TBM_SETPOS , 
		(WPARAM) 1, 
		(LPARAM) iDuree); 

	return TRUE;

}

LRESULT CDiaporama::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{
		case IDC_BTSTART:
		case IDOK:
			{
				//Récupération des données:
				SendDlgItemMessage(hDlg, 
				IDC_EDITDELAI, 
				WM_GETTEXT, 
				(WPARAM) 255, 
				(LPARAM) cDuree); 

				iDuree = atoi(cDuree);

				iSequence = (IsDlgButtonChecked(hDlg,IDC_SEQRADAVANT) == BST_CHECKED) ? 1 : 2;

				m_bHauteQualite = (IsDlgButtonChecked(hDlg,IDC_QUALITE) == BST_CHECKED) ? 1 : 0;

				m_bShrink = (IsDlgButtonChecked(hDlg,IDC_CHECKSHRINK) == BST_CHECKED) ? true : false;

				m_bFullScreen = (IsDlgButtonChecked(hDlg,IDC_FULLSCREEN) == BST_CHECKED) ? 1 : 0;

				m_b3D = (IsDlgButtonChecked(hDlg,IDC_CHECKVISIONAGE3D) == BST_CHECKED) ? 1 : 0;

				if(IsDlgButtonChecked(hDlg,IDC_RDTRESHAUTE) == BST_CHECKED)
				{
					m_i3DQuality = 3;
				}
				else if(IsDlgButtonChecked(hDlg,IDC_RDHAUTE) == BST_CHECKED)
				{
					m_i3DQuality = 2;
				}
				else if(IsDlgButtonChecked(hDlg,IDC_RDMOYENNE) == BST_CHECKED)
				{
					m_i3DQuality = 1;
				}
				else if(IsDlgButtonChecked(hDlg,IDC_RDBASSE) == BST_CHECKED)
				{
					m_i3DQuality = 0;
				}


				CParameter::SetOptionDiaporama(iSequence,iDuree,m_bShrink,m_bHauteQualite,m_bFullScreen,m_b3D,m_i3DQuality);

				if(wmId == IDC_BTSTART)
					SendMessage(hWndAffichage,WM_COMMAND,MAKEWPARAM(ID_IMAGES_STARTDIAPORAMA,0),0);

				EndDialog(hDlg, LOWORD(wParam));
			}
			return TRUE;

		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;

	}

	return TRUE;
}

LRESULT CDiaporama::OnHScroll(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//Mise à jour des données de l'affichage
    // User moved the slider.
    switch(LOWORD(wParam))
    {
		case TB_PAGEDOWN:
		case SB_THUMBTRACK:
		case TB_PAGEUP:
			iDuree = SendDlgItemMessage(hDlg, IDC_SLIDERDELAI,
				TBM_GETPOS, 0, 0);

			StringCchPrintf(cDuree,50,"%d",iDuree);

			SendDlgItemMessage(hDlg, 
			IDC_EDITDELAI, 
			WM_SETTEXT, 
			(WPARAM) strlen(cDuree), 
			(LPARAM) cDuree); 

			break;
	}

	return TRUE;
}
    
