/****************************************************************************

    PROGRAM: SSAVER.C     

    PURPOSE: A bare bones shell for a Win32 screen saver
 

****************************************************************************/

#include <windows.h>
#include <scrnsave.h>
#include "mp3.h"
#include "fichier.h"
#include "resource.h"
#include "catalogue1.h"
#include "loadimage.h"
#include "affichagebitmap.h"

HANDLE hInst;                   /* current instance              */
UINT wTimer = 0;    // Timer id

void EcrireRegistre(const long &intervall, const long &lNumAlbum,const char * Repertoire,const char * Mp3Rep);
void LireRegistre(long &intervall, long &lNumAlbum,char * Repertoire,char * Mp3Rep);

/*
 * ScreenSaverConfigureDialog -- Process the screen saver configuration
 *                               messages for the config dialog
 *
 */
BOOL CALLBACK ScreenSaverConfigureDialog(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	static CCatalogue * m_CCatalogue;
	static CCatalogue::Album m_Album;
	CCatalogue::AlbumVector::iterator AlbumIt;
	static CCatalogue::AlbumVector * m_AlbumVector;

    switch(Msg)
    {
		case WM_INITDIALOG:
			{
				long intervall,lNumAlbum;
				char Repertoire[255];
				char cMp3[255];


				LireRegistre(intervall, lNumAlbum,Repertoire,cMp3);
				SetDlgItemInt(hWnd, IDC_INTERVAL, intervall, FALSE);
				SetDlgItemInt(hWnd, IDC_ALBUM, lNumAlbum, FALSE);
				SetDlgItemText(hWnd, IDC_ALBUMREP, Repertoire);
				SetDlgItemText(hWnd, IDC_MP3REP, cMp3); 

				m_CCatalogue = new CCatalogue(Repertoire);
				m_CCatalogue->RecupPtVectorAlbum(&m_AlbumVector);

				int iNumber = 0;
				//Initialisation de la combo Box
				for (AlbumIt = m_AlbumVector->begin(); AlbumIt!= m_AlbumVector->end(); AlbumIt++)
				{
					CCatalogue::Album m_AlbumTemp = *AlbumIt;
					SendDlgItemMessage(hWnd, IDC_CBLIST, CB_INSERTSTRING , iNumber, (LPARAM)(LPCTSTR)m_AlbumTemp.m_szAlbumName);
					SendDlgItemMessage(hWnd, IDC_CBLIST, WM_SETTEXT , (WPARAM)0, (LPARAM)m_AlbumTemp.m_szAlbumName);
					iNumber++;

				}
				SendDlgItemMessage(hWnd, IDC_CBLIST, CB_SETCURSEL, --lNumAlbum, 0);
				delete m_CCatalogue;

			}
			break;

        case WM_COMMAND:
			{
				switch(LOWORD(wParam))
				{
					case IDOK:  // Exit the dialog
					case IDC_EXIT:
						{
							int intervall = GetDlgItemInt(hWnd, IDC_INTERVAL, NULL, FALSE);
							int album = SendDlgItemMessage(hWnd, IDC_CBLIST, CB_GETCURSEL, 0, 0);
							album++;
							//int album = GetDlgItemInt(hWnd, IDC_ALBUM, NULL, FALSE);
							char cRep[255];
							GetDlgItemText(hWnd, IDC_ALBUMREP,cRep,255);
							//MessageBox(NULL,cRep,"toto",0);
							char cMp3[255];
							GetDlgItemText(hWnd, IDC_MP3REP,cMp3,255);
							//MessageBox(NULL,cMp3,"toto",0);
							EcrireRegistre(intervall,album,cRep,cMp3);
							EndDialog(hWnd, 42);
						}
						break;

					case IDC_BTLISTEREP:
						{
							static OPENFILENAME ofn;
							static TCHAR szFilter[] = TEXT ("Fichiers MP3  (*.MP3)\0*.mp3\0");
							static TCHAR pstrTitleName[MAX_PATH];
							static TCHAR pstrFileName[MAX_PATH];
							
							ofn.lStructSize       = sizeof (OPENFILENAME) ;
							ofn.hwndOwner         = hWnd ;
							ofn.hInstance         = NULL ;
							ofn.lpstrFilter       = szFilter ;
							ofn.lpstrCustomFilter = NULL ;
							ofn.nMaxCustFilter    = 0 ;
							ofn.nFilterIndex      = 0 ;
							ofn.nMaxFile          = MAX_PATH ;
							ofn.lpstrFileTitle    = NULL ;          // Défini dans les fonctions Ouvrir et Fermer
							ofn.nMaxFileTitle     = MAX_PATH ;
							ofn.lpstrInitialDir   = NULL ;
							ofn.lpstrTitle        = NULL ;
							ofn.Flags             = 0 ;             // Défini dans les fonctions Ouvrir et Fermer
							ofn.nFileOffset       = 0 ;
							ofn.nFileExtension    = 0 ;
							ofn.lpstrDefExt       = TEXT ("MP3") ;
							ofn.lCustData         = 0L ;
							ofn.lpfnHook          = NULL ;
							ofn.lpTemplateName    = NULL ;
							ofn.lpstrFile         = pstrFileName ;
							ofn.lpstrFileTitle    = pstrTitleName ;
							ofn.Flags             = OFN_OVERWRITEPROMPT;

							if (GetOpenFileName(&ofn))
								SetDlgItemText(hWnd, IDC_MP3REP, pstrFileName); 

							break;
						}

					case IDC_BTLISTEREP2:
						{
							static OPENFILENAME ofn;
							static TCHAR szFilter[] = TEXT ("Fichiers DB  (*.DB)\0*.db\0");
							static TCHAR pstrTitleName[MAX_PATH];
							static TCHAR pstrFileName[MAX_PATH];
							
							ofn.lStructSize       = sizeof (OPENFILENAME) ;
							ofn.hwndOwner         = hWnd ;
							ofn.hInstance         = NULL ;
							ofn.lpstrFilter       = szFilter ;
							ofn.lpstrCustomFilter = NULL ;
							ofn.nMaxCustFilter    = 0 ;
							ofn.nFilterIndex      = 0 ;
							ofn.nMaxFile          = MAX_PATH ;
							ofn.lpstrFileTitle    = NULL ;          // Défini dans les fonctions Ouvrir et Fermer
							ofn.nMaxFileTitle     = MAX_PATH ;
							ofn.lpstrInitialDir   = NULL ;
							ofn.lpstrTitle        = NULL ;
							ofn.Flags             = 0 ;             // Défini dans les fonctions Ouvrir et Fermer
							ofn.nFileOffset       = 0 ;
							ofn.nFileExtension    = 0 ;
							ofn.lpstrDefExt       = TEXT ("db") ;
							ofn.lCustData         = 0L ;
							ofn.lpfnHook          = NULL ;
							ofn.lpTemplateName    = NULL ;
							ofn.lpstrFile         = pstrFileName ;
							ofn.lpstrFileTitle    = pstrTitleName ;
							ofn.Flags             = OFN_OVERWRITEPROMPT;

							if (GetOpenFileName(&ofn))
							{
								SetDlgItemText(hWnd, IDC_ALBUMREP, pstrFileName); 
								//Mise à jour de la liste
								m_CCatalogue = new CCatalogue(pstrFileName);
								m_CCatalogue->RecupPtVectorAlbum(&m_AlbumVector);

								SendDlgItemMessage(hWnd, IDC_CBLIST, CB_RESETCONTENT,0,0);

								int iNumber = 0;
								//Initialisation de la combo Box
								for (AlbumIt = m_AlbumVector->begin(); AlbumIt!= m_AlbumVector->end(); AlbumIt++)
								{
									CCatalogue::Album m_AlbumTemp = *AlbumIt;
									SendDlgItemMessage(hWnd, IDC_CBLIST, CB_INSERTSTRING , iNumber, (LPARAM)(LPCTSTR)m_AlbumTemp.m_szAlbumName);
									SendDlgItemMessage(hWnd, IDC_CBLIST, WM_SETTEXT , (WPARAM)0, (LPARAM)m_AlbumTemp.m_szAlbumName);
									iNumber++;

								}
								SendDlgItemMessage(hWnd, IDC_CBLIST, CB_SETCURSEL, 0, 0);
								delete m_CCatalogue;

							}

							break;
						}
					}
            }
        break;

        default:
		    return FALSE;
        break;
    }
    return TRUE;
}

				       
/****************************************************************************

    FUNCTION: ScreenSaverProc(HWND, UINT, WPARAM, LPARAM)

    PURPOSE:  Processes screen saver messages

****************************************************************************/

LONG CALLBACK ScreenSaverProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{              
	static CCatalogue * m_CCatalogue;
	static CCatalogue::AlbumVector * m_AlbumVector;
	static CCatalogue::AlbumVector::const_iterator dit;
	static CCatalogue::IconeFileVector::iterator fit;
	static CCatalogue::Album m_Album;
	static CMP3 m_cMP3;
	static CFichier m_cFichier;
	static long interval;

	switch (msg) 
	{
		case WM_CREATE:
		{		
			// On Create
			CMP3Input::InputInfo pInfo;

			
			long lNumAlbum;
			char Repertoire[MAX_PATH];
			char m_szMp3[MAX_PATH];

			m_cMP3.xmMP3_setWindow(hWnd);
			m_cMP3.xmMP3_init(0);

			LireRegistre(interval,lNumAlbum,Repertoire,m_szMp3);

			if(strcmp(Repertoire,"") != 0)
			{
				interval *= 1000;

				if(strcmp(m_szMp3,"") != 0)
				{

					m_cFichier.DefinirRepertoire(m_szMp3,0,"*.mp3");

					ZeroMemory(&pInfo,sizeof(CMP3Input::InputInfo));
					m_cMP3.xmMP3_open(m_szMp3, &pInfo);

				}

				m_CCatalogue = new CCatalogue(Repertoire);
				m_CCatalogue->RecupPtVectorAlbum(&m_AlbumVector);
				dit = m_AlbumVector->begin();

				if(dit != NULL)
				{
					m_Album = *dit;
					
					for(int i = 1;i < lNumAlbum;i++)
					{
						dit++;
						m_Album = *dit;
					}
					fit = m_Album.m_IconeFileVector.begin();
					wTimer = SetTimer(hWnd, 0, interval, NULL);  
				}

				if(strcmp(m_szMp3,"") != 0)
					m_cMP3.xmMP3_play();
			}
			break;
		}

		case IDC_FINFICHIER:
			{
				CMP3Input::InputInfo pInfo;
				TCHAR szFileName[255];
				ZeroMemory(&pInfo,sizeof(CMP3Input::InputInfo));
				if(m_cFichier.GetNbFiles() > 0)
				{
					m_cFichier.AffichageImageSuivante(szFileName);
					m_cMP3.xmMP3_stop();
					m_cMP3.xmMP3_close();
					m_cMP3.xmMP3_open(szFileName, &pInfo);
					m_cMP3.xmMP3_play();
				}
			}
			break;

		case WM_CLOSE:
		{	
			if(m_cFichier.GetNbFiles() > 0)
			{
				m_cMP3.xmMP3_stop();
				m_cMP3.xmMP3_close();
			}
			m_cMP3.xmMP3_free();
			DestroyWindow(hWnd);
			PostQuitMessage( 0 );													
			break;
		}

	case WM_TIMER:
		{
			CIBitmap m_ciBitmap;
			CLoadImage m_ChargementImage;
			CAffichageBitmap m_AffichageBitmap;
			// Show a frame of our animation
			CCatalogue::IconeFile m_IconeFile;
			m_IconeFile = *fit;
			m_ChargementImage.ChargementImageStatic(m_ciBitmap,m_IconeFile.m_stgFullPathName);

			m_AffichageBitmap.AffichageBitmap(hWnd,m_ciBitmap.hBitmap,m_IconeFile.m_stgFullPathName);

			fit++;
			if(fit == m_Album.m_IconeFileVector.end())
			{	
				fit = m_Album.m_IconeFileVector.begin();
			}
			//DrawFrame(hWnd);
		}
		return 0;
	
    case WM_ERASEBKGND:        
		// Seed the random number generator
        //srand(GetTickCount());
		// Tell windows to send us timer messages at 50ms intervals
		break;       

    case WM_DESTROY:            /* message: window being destroyed */
		delete m_CCatalogue;
		if (wTimer) // Stop our timer if it was created
			KillTimer(hWnd, wTimer); 
		break;              
    }  
    
    return DefScreenSaverProc(hWnd, msg, wParam, lParam); 
}

/*
 * RegisterDialogClasses -- Entry point for registering window classes 
 *                          required by configuration dialog box.
 */


BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
	return TRUE;
}

/***********************************/
/* Registrierung auslesen          */
/***********************************/

void LireRegistre(long &intervall, long &lNumAlbum,char * Repertoire, char * Mp3Rep)
{
	HKEY hKey;
	DWORD groesse;
	DWORD art;
	char puffer[255];


	if(RegOpenKeyEx(HKEY_CURRENT_USER,
		"SOFTWARE\\Screensaver\\Regards",
		0, 
		KEY_READ, 
		&hKey ) == ERROR_SUCCESS) 
	{
		groesse = sizeof(puffer);
		art = REG_SZ;
		RegQueryValueEx(hKey, "Intervall", 0, &art, (unsigned char *)puffer, &groesse);
		intervall = atoi(puffer);
		groesse = sizeof(puffer);
		art = REG_SZ;
		RegQueryValueEx(hKey, "Album", 0, &art, (unsigned char *)puffer, &groesse);
		lNumAlbum = atoi(puffer);
		art = REG_SZ;
		groesse = 255;
		RegQueryValueEx(hKey, "Repertoire", 0, &art, (unsigned char *)puffer, &groesse);
		strcpy(Repertoire,puffer);
		art = REG_SZ;
		groesse = 255;
		RegQueryValueEx(hKey, "mp3", 0, &art, (unsigned char *)puffer, &groesse);
		strcpy(Mp3Rep,puffer);

	}
	else	/* noch kein Eintrag in der Registrierung */
	{
		intervall = 4;
		lNumAlbum = 1;
		strcpy(Repertoire,"");
		strcpy(Mp3Rep,"");
	}
}

void EcrireRegistre(const long &intervall, const long &lNumAlbum,const char * Repertoire,const char * Mp3Rep)
{
	char  dummy[5];
	HKEY  hKey; 
	DWORD  dwDisposition; 

	RegCreateKeyEx(HKEY_CURRENT_USER,
		"SOFTWARE\\Screensaver\\Regards",
		0, 
		NULL, 
		REG_OPTION_NON_VOLATILE, 
		KEY_WRITE, 
		NULL, 
		&hKey, 
		&dwDisposition); 
	wsprintf(dummy, "%u", intervall);
	RegSetValueEx(hKey, "Intervall", 0, REG_SZ,(unsigned char *)dummy, sizeof(dummy));
	wsprintf(dummy, "%u", lNumAlbum);
	RegSetValueEx(hKey, "Album", 0, REG_SZ, (unsigned char *)dummy, sizeof(dummy));
	if(strcmp(Repertoire,"") != 0)
		RegSetValueEx(hKey, "Repertoire", 0, REG_SZ, (unsigned char *)Repertoire, strlen(Repertoire));
	if(strcmp(Mp3Rep,"") != 0)
		RegSetValueEx(hKey, "mp3", 0, REG_SZ, (unsigned char *)Mp3Rep, strlen(Mp3Rep));
}