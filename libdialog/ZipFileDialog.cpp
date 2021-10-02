#include "stdafx.h"
#include ".\zipfiledialog.h"
#include "toolbar.h"
#include "coolmenu.h"
#include <files.h>
#include "resource.h"
#include "aboutdialog.h"
#include <Commdlg.h>
#include <shellapi.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

CZipFileDialog::CZipFileDialog(HINSTANCE hInst)
{
	hInstance = hInst;
	m_CFiles = new CFiles();
#ifdef COOLMENU
	m_CCoolMenu = new CCoolMenu();
#endif
	m_cToolbar = new CToolbar();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

CZipFileDialog::~CZipFileDialog(void)
{
	if(m_CFiles != NULL)
		delete m_CFiles;

#ifdef COOLMENU
	if(m_CCoolMenu != NULL)
		delete m_CCoolMenu;
#endif
	if(m_cToolbar != NULL)
		delete m_cToolbar;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CZipFileDialog::OnMeasureItem(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#ifdef COOLMENU
	if(wParam == 0)
		return m_CCoolMenu->CoolMenu_MeasureItem(wParam, lParam);
#endif
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CZipFileDialog::OnDrawItem(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#ifdef COOLMENU
	if(wParam == 0)
		return m_CCoolMenu->CoolMenu_DrawItem(wParam, lParam);
#endif
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CZipFileDialog::OnDestroy(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#ifdef COOLMENU
	HMENU hmenu,hsub;

	hmenu = GetMenu(hDlg);
	for(int i = 0;i < 2;i++)
	{
		hsub  = GetSubMenu(hmenu, i);
		m_CCoolMenu->RemoveCoolMenu(hsub);
	}
#endif
	m_CFiles->Close();
	DestroyWindow(hWndToolbar);
	DestroyWindow(hWndStatusBar);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CZipFileDialog::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char Temp[20];
	char cTemp2[20];
	m_IntArray.clear();

	SetMenu(hDlg,LoadMenu(hInstance,MAKEINTRESOURCE(IDM_MENUZIP))); 

	LoadString(hInstance,IDS_ZIPFILE,cTemp2,20);

	StringCchPrintf(szFileName,MAX_PATH,"%s - %s",cTemp2,m_cCompressionFichier);
	SetWindowText(hDlg,szFileName);

	hWndToolbar = m_cToolbar->CreateRebar(hDlg,hInstance,4);

	hWndStatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, "", hDlg, 1);

	SendDlgItemMessage(hDlg, IDC_FILELIST,LVM_SETEXTENDEDLISTVIEWSTYLE, 0,(LPARAM)LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	//Définition de l'entete de colonne
	LVCOLUMN lvColumn;
	
	LoadString(hInstance,IDS_FIRSTCOLUMN,Temp,20);

	lvColumn.mask = LVCF_TEXT|LVCF_WIDTH;
	lvColumn.cx = 135;
	lvColumn.pszText = Temp;
	SendDlgItemMessage(hDlg, IDC_FILELIST, LVM_INSERTCOLUMN , (WPARAM)0, (LPARAM)&lvColumn);


	// Add columns
	
	LoadString(hInstance,IDS_SECONDCOLUMN,Temp,20);

	lvColumn.mask = LVCF_TEXT|LVCF_WIDTH;
	lvColumn.cx = 135;
	lvColumn.pszText = Temp;
	SendDlgItemMessage(hDlg, IDC_FILELIST, LVM_INSERTCOLUMN , (WPARAM)1, (LPARAM)&lvColumn);

	// Add columns
	LoadString(hInstance,IDS_COMPRESSSIZE,Temp,20);
	lvColumn.mask = LVCF_TEXT|LVCF_WIDTH;
	lvColumn.cx = 135;
	lvColumn.pszText = Temp;
	SendDlgItemMessage(hDlg, IDC_FILELIST, LVM_INSERTCOLUMN , (WPARAM)2, (LPARAM)&lvColumn);


	LoadString(hInstance,IDS_MODIFYTHE,Temp,20);
	lvColumn.mask = LVCF_TEXT|LVCF_WIDTH;
	lvColumn.cx = 135;
	lvColumn.pszText = Temp;
	SendDlgItemMessage(hDlg, IDC_FILELIST, LVM_INSERTCOLUMN , (WPARAM)3, (LPARAM)&lvColumn);

	// Add columns
	LoadString(hInstance,IDS_PATH,Temp,20);
	lvColumn.mask = LVCF_TEXT|LVCF_WIDTH;
	lvColumn.cx = 135;
	lvColumn.pszText = Temp;
	SendDlgItemMessage(hDlg, IDC_FILELIST, LVM_INSERTCOLUMN , (WPARAM)4, (LPARAM)&lvColumn);

	m_CFiles->Open(m_cCompressionFichier);
	iNbFiles = m_CFiles->getNbFiles();
	for(int i = 0;i < iNbFiles;i++)
	{
		if(AjoutFichierZip(hDlg, i))
			m_IntArray.push_back(i);
	}

	LoadString(hInstance,IDS_FILE,cTemp2,20);			

	StringCchPrintf(Temp,20,"%d %s",iNbFiles,cTemp2);
	SendMessage(hWndStatusBar, SB_SETTEXT,  0 | 0,  (LPARAM)Temp);

#ifdef COOLMENU
	HMENU hmenu = GetMenu(hDlg);
	for(int k = 0;k < 2;k++)
	{
		HMENU hsub  = GetSubMenu(hmenu, k);
		m_CCoolMenu->MakeCoolMenu(hsub, hInstance, MAKEINTRESOURCE(IDB_MENUBMP));
	}
#endif
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CZipFileDialog::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{

		case ID_AIDE_APROPOS:
			{
				CAboutDialog * m_cAboutDialog = new CAboutDialog(hInstance);
				m_cAboutDialog->CreateDlg(hInstance,(LPCTSTR)IDD_ABOUTBOX, hDlg);
				delete m_cAboutDialog;
			}
			break;

		case ID_ACTION_AJOUTERFICHIER:
			{
				OPENFILENAME ofn;       // common dialog box structure
				char szFile[260];       // buffer for file name
				char FilePath[260];       // buffer for file name
				char Temp[20];

				szFile[0]='\0';

				// Initialize OPENFILENAME
				ZeroMemory(&ofn, sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = hDlg;
				ofn.lpstrFile = szFile;
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST;

				if (!GetOpenFileName(&ofn))
					return 0;

				if ( ofn.nFileOffset < lstrlen(szFile) )
				{
					if(!m_CFiles->AddFileByName(szFile, 1, false, true))
					{
						if(AjoutFichierZip(hDlg,iNbFiles))
							iNbFiles++;
					}
				}
				else
				{
					StringCchCopy(FilePath,MAX_PATH,szFile);
					FilePath[ofn.nFileOffset] = 0;
					while ( szFile[ofn.nFileOffset] != 0 )
					{
						char Message[MAX_PATH];
						StringCchCopy(Message,MAX_PATH,FilePath);
						StringCbCat(Message,MAX_PATH,"\\");
						StringCbCat(Message,MAX_PATH,szFile+ofn.nFileOffset);

						if(!m_CFiles->AddFileByName(Message, 1, false, true))
						{
							if(AjoutFichierZip(hDlg,iNbFiles))
								iNbFiles++;
						}

						ofn.nFileOffset += (strlen(szFile+ofn.nFileOffset) + 1);
					}
				}

				char cTemp2[20];
				LoadString(hInstance,IDS_FILE,cTemp2,20);
				StringCchPrintf(Temp,20,"%d %s",iNbFiles,cTemp2);

				SendMessage(hWndStatusBar,SB_SETTEXT,0 | 0,(LPARAM)Temp);

			}
			return 0;
			break;

		case ID_ACTION_SUPPRIMERFICHIER:
			{
				int iSelCount = SendDlgItemMessage(hDlg, IDC_FILELIST, LVM_GETSELECTEDCOUNT , 0, 0);
				if(iSelCount > 0)
				{
					char Temp[120];
					char clpText[255];
					char clpCaption[255];

					LoadString(hInstance,IDS_SUPPFILE,clpText,255);
					LoadString(hInstance,IDS_INFOS,clpCaption,255);
					
					StringCchPrintf(Temp,120,clpText,iSelCount);
					if(MessageBox(hDlg,Temp,clpCaption,MB_YESNO | MB_ICONWARNING) == IDYES)
					{
						int nItem = SendDlgItemMessage(hDlg, IDC_FILELIST, LVM_GETNEXTITEM, -1, MAKELPARAM(LVNI_SELECTED,0)); 
						while (nItem != -1) 
						{ 
							m_CFiles->Remove(m_IntArray[nItem]);
							m_IntArray.erase(m_IntArray.begin() + nItem);
							SendDlgItemMessage(hDlg, IDC_FILELIST, LVM_DELETEITEM, nItem, 0); 

							nItem = SendDlgItemMessage(hDlg, IDC_FILELIST, LVM_GETNEXTITEM, -1, MAKELPARAM(LVNI_SELECTED,0)); 

							iNbFiles--;
						} 

						char Temp[20];

						StringCchPrintf(Temp,20,"%d Fichiers",iNbFiles);
						SendMessage(hWndStatusBar,SB_SETTEXT,0 | 0,(LPARAM)Temp);
					}
				}
			}
			break;

		case ID_ACTION_EXTRAIREFICHIER:
			{
				//Test du nombre de fichier sélectionné

				int iSelCount = SendDlgItemMessage(hDlg, IDC_FILELIST, LVM_GETSELECTEDCOUNT , 0, 0);
				//int iNumSelected;
				if(iSelCount > 0)
				{
					//Récupération du path du fichier zip
					TCHAR drive[_MAX_DRIVE],dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
					_splitpath_s(m_cCompressionFichier, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

					char sPath[MAX_PATH];
					StringCchCopy(sPath,MAX_PATH,drive);
					CObjet m_cObjet;
					if(m_cObjet.GetDirectory(hDlg, sPath))
					{
						int iNbEncours = 0;
						int nItem = SendDlgItemMessage(hDlg, IDC_FILELIST, LVM_GETNEXTITEM, -1, MAKELPARAM(LVNI_SELECTED,0)); 
						while (nItem != -1) 
						{ 
							CFichierZip m_CFichierZip;
							string m_stgFilename;
							m_CFichierZip = m_CFiles->ExtractFile(m_IntArray[nItem]);
							m_CFichierZip.Extract(sPath,&m_stgFilename);
								
							nItem = SendDlgItemMessage(hDlg, IDC_FILELIST, LVM_GETNEXTITEM, nItem, MAKELPARAM(LVNI_SELECTED,0)); 
						} 

					}
				}
			}
			break;

		case ID_ACTION_VOIRFICHIER:
			{
				int iSelCount = SendDlgItemMessage(hDlg, IDC_FILELIST, LVM_GETSELECTEDCOUNT , 0, 0);
				//int iNumSelected;
				if(iSelCount > 0)
				{
					/*
					//Récupération du path du fichier zip
					TCHAR drive[_MAX_DRIVE],dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];

					_splitpath(m_cCompressionFichier, drive, dir, fname, ext);

					int nItem = SendDlgItemMessage(hDlg, IDC_FILELIST, LVM_GETNEXTITEM, -1, MAKELPARAM(LVNI_SELECTED,0)); 

					char sPath[MAX_PATH];
					GetTempPath(MAX_PATH,sPath);

					CFichierZip m_CFichierZip;
					string m_stgFilename;
					m_CFichierZip = m_CFiles->ExtractFile(m_IntArray[nItem]);
					m_CFichierZip.Extract(sPath,&m_stgFilename);

					ShellExecute(NULL,"open",m_stgFilename.c_str(),NULL,NULL,SW_SHOWNORMAL);
					*/
					int nItem = SendDlgItemMessage(hDlg, IDC_FILELIST, LVM_GETNEXTITEM, -1, MAKELPARAM(LVNI_SELECTED,0)); 
					VoirFichier(nItem);
				}
			}
			break;

		case ID_ACTION_TOUTSLECTIONNER:
			{
				LVITEM m_LVITEM;
				m_LVITEM.state = LVIS_SELECTED | LVIS_DROPHILITED;
				m_LVITEM.stateMask = LVIS_SELECTED;
				SendDlgItemMessage(hDlg, IDC_FILELIST, LVM_SETITEMSTATE,-1,(LPARAM)&m_LVITEM);	
			}
			break;

		case IDCANCEL:
		case ID_ACTION_QUITTER:
			{
				DestroyWindow(hWndToolbar);
				EndDialog(hDlg, LOWORD(wParam));
			}
			break;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CZipFileDialog::OnNotify(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (((LPNMHDR) lParam)->code) 
	{
		case NM_DBLCLK:
			{
				LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE) lParam;
				VoirFichier(lpnmitem->iItem);
			}
			break;

		case TTN_GETDISPINFO: 
			{ 
			LPTOOLTIPTEXT lpttt; 

			lpttt = (LPTOOLTIPTEXT) lParam; 
			lpttt->hinst = hInstance; 

			// Specify the resource identifier of the descriptive 
			// text for the given button. 
			int idButton = lpttt->hdr.idFrom; 
			switch (idButton) 
			{ 
				case ID_ACTION_AJOUTERFICHIER: 
					lpttt->lpszText = MAKEINTRESOURCE(IDS_ZIPAJOUT); 
					break; 
				case ID_ACTION_SUPPRIMERFICHIER: 
					lpttt->lpszText = MAKEINTRESOURCE(IDS_ZIPSUP); 
					break; 
				case ID_ACTION_EXTRAIREFICHIER: 
					lpttt->lpszText = MAKEINTRESOURCE(IDS_ZIPEXTRAIRE); 
					break; 
				case ID_ACTION_VOIRFICHIER: 
					lpttt->lpszText = MAKEINTRESOURCE(IDS_ZIPVOIR); 
					break; 
				case ID_ACTION_TOUTSLECTIONNER: 
					lpttt->lpszText = MAKEINTRESOURCE(IDS_ZIPSELECTALL); 
					break; 
			}

		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////////


int CZipFileDialog::VoirFichier(int iNumItem)
{
	
	//Récupération du path du fichier zip
	TCHAR drive[_MAX_DRIVE],dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];

	//_splitpath(m_cCompressionFichier, drive, dir, fname, ext);
	_splitpath_s(m_cCompressionFichier, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

	char sPath[MAX_PATH];
	GetTempPath(MAX_PATH,sPath);

	CFichierZip m_CFichierZip;
	string m_stgFilename;
	m_CFichierZip = m_CFiles->ExtractFile(m_IntArray[iNumItem]);
	m_CFichierZip.Extract(sPath,&m_stgFilename);

	ShellExecute(NULL,"open",m_stgFilename.c_str(),NULL,NULL,SW_SHOWNORMAL);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

int CZipFileDialog::AjoutFichierZip(HWND &hDlg, const int &i)
{
	char cTemp[50];
	long m_lValue;
	DATE m_dDate;
	bool m_bDirectory;
	SYSTEMTIME  systime;
	LVITEM lvItem;
	int m_iIndex;

	ZeroMemory(&lvItem,sizeof(LVITEM));
	ZeroMemory(&systime,sizeof(SYSTEMTIME));

	string m_stgFileName;
	string m_stgDirectory;
	CFichierZip m_CFichierZip;

	if(i >= m_CFiles->getNbFiles())
		return 0;

	m_CFichierZip = m_CFiles->ExtractFile(i);

	m_CFichierZip.getDirectory(&m_bDirectory);
	//Test si répertoire
	if(m_bDirectory)
		return 0;

	m_CFichierZip.getFileNameInZip(&m_stgFileName);

	int j = 0;
	do
	{
		j = m_stgFileName.find("/",0);
		if(j >= 0)
		{
			m_stgDirectory += m_stgFileName.substr(0,j+1);
			m_stgFileName = m_stgFileName.substr(j + 1,m_stgFileName.length() - j - 1);
		}
	}while(j >= 0);

	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = i;
	lvItem.iSubItem = 0;
	lvItem.pszText = (char *)m_stgFileName.c_str();
	m_iIndex = SendDlgItemMessage(hDlg, IDC_FILELIST, LVM_INSERTITEM , (WPARAM)i, (LPARAM) (const LPLVITEM)&lvItem);

	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = m_iIndex;
	lvItem.iSubItem = 4;
	lvItem.pszText = (char *)m_stgDirectory.c_str();
	SendDlgItemMessage(hDlg, IDC_FILELIST, LVM_SETITEMTEXT , (WPARAM)m_iIndex, (LPARAM) (const LPLVITEM)&lvItem);


	m_CFichierZip.getUncompressedSize(&m_lValue);
	StringCchPrintf(cTemp,50,"%d octets",m_lValue);
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = m_iIndex;
	lvItem.iSubItem = 1;
	lvItem.pszText = cTemp;
	
	SendDlgItemMessage(hDlg, IDC_FILELIST, LVM_SETITEMTEXT , (WPARAM)m_iIndex, (LPARAM) (const LPLVITEM)&lvItem);
	
	m_CFichierZip.getModificationDate(&m_dDate);
	VariantTimeToSystemTime(m_dDate,&systime);
	char str[50];
	StringCchPrintf(str,50, "%u/%u/%u  %u:%u:%u", systime.wDay, systime.wMonth, systime.wYear,
	   systime.wHour, systime.wMinute, systime.wSecond);

	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = m_iIndex;
	lvItem.iSubItem = 3;
	lvItem.pszText = str;
	
	SendDlgItemMessage(hDlg, IDC_FILELIST, LVM_SETITEMTEXT , (WPARAM)m_iIndex, (LPARAM) (const LPLVITEM)&lvItem);

	m_CFichierZip.getCompressedSize(&m_lValue);
	StringCchPrintf(cTemp,50,"%d octets",m_lValue);
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = m_iIndex;
	lvItem.iSubItem = 2;
	lvItem.pszText = cTemp;
	
	SendDlgItemMessage(hDlg, IDC_FILELIST, LVM_SETITEMTEXT , (WPARAM)m_iIndex, (LPARAM) (const LPLVITEM)&lvItem);


	return 1;
}
