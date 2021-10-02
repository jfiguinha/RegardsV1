#include "stdafx.h"
#include ".\infosfichierproc.h"
#include ".\previewdata.h"
#include "webinfofichier.h"
#include "objet.h"
#include <exif.h>
#include <ibitmap.h>
#include <VideoAPI.h>
#ifdef SQLSERVERCE
#include <SqlAlbumIconeFileAttribut.h>
#include <SqlAlbumAttribut.h>
using namespace Regards::Sqlite;
#endif

void CInfosFichierProc::SetParam(CPreviewData * pPreviewData)
{
	m_pPreviewData = pPreviewData;
}

CInfosFichierProc::CInfosFichierProc(void)
{
	g_hbrBackground = CreateSolidBrush(RGB(255,255,255));
	hFocus = NULL;
	m_cWebInfoFichier = new CWebInfoFichier();
	m_pPreviewData = NULL;

}

CInfosFichierProc::~CInfosFichierProc(void)
{
	if(m_cWebInfoFichier != NULL)
		delete m_cWebInfoFichier;

	m_cWebInfoFichier = NULL;
}

LRESULT CInfosFichierProc::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//Initialisation du composant Web
	DWORD dwStyle = CS_HREDRAW | CS_VREDRAW;
	DWORD dwWndStyle = WS_CHILD;
	DWORD dwExStyle = 0;
	RECT rc = {200, 0, 200, 0};
	m_cWebInfoFichier->CreateEx(hDlg,hInstance,dwWndStyle,dwStyle,dwExStyle,"WEBBROWSER2",0,0,IDC_ARROW,rc);
	::ShowWindow(m_cWebInfoFichier->m_hWnd,SW_SHOW);
	return 0;
}

LRESULT CInfosFichierProc::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId,wmEvent;
	wmId    = LOWORD(wParam); 
	wmEvent = HIWORD(wParam); 
	// Parse the menu selections:
	switch (wmId)
	{
		case IDC_BTMODIF:
			{
				SendMessage(::GetParent(hDlg),WM_COMMAND,MAKEWPARAM(ID_LISTE_GESTIONSDESATTRIBUTS,0),0);
#ifdef SQLSERVERCE
				//Mise à jour de la liste des attributs
				CSqlAlbumAttribut * m_cSqlAlbumAttribut = new CSqlAlbumAttribut();
				m_cSqlAlbumAttribut->LoadAlbumAttribut(m_pPreviewData->m_AttributVector);
				delete m_cSqlAlbumAttribut;
#endif
			}
			break;

		case IDM_FILENAME:
		case IDC_UPDATE_FILEINFO:
			{
				CObjet m_cObjet;
				char m_szMessage[512];
				char m_szInfos[512];
				char m_szTemp[512];
				char cTemp[1024];
				string m_stgHTMLDisplay;

				int iFormatImage = m_cObjet.TestImageFormat(m_pPreviewData->cFile);
				if(iFormatImage == 1)
				{
					FILE *hSource;
					errno_t err;
					err  = fopen_s(&hSource, m_pPreviewData->cFile,"rb");

					if(err == 0)
						m_pPreviewData->m_cExif.DecodeExif(hSource);
					if(hSource)
						fclose(hSource);
				}

				//if(m_pPreviewData->m_cExif.m_exifinfo->IsExif)
				//{
					StringCchCopy(m_pPreviewData->m_cExif.m_exifinfo->FileName,MAX_PATH,m_pPreviewData->cFile);
					//Process Exif Infos
					RECT rc;
					GetClientRect(hDlg,&rc);
					MoveWindow(m_cWebInfoFichier->m_hWnd,0,0,rc.right-1,rc.bottom-1,false);
					MoveWindow(m_cWebInfoFichier->m_hWnd,0,0,rc.right+1,rc.bottom+1,false);
					//SendMessage(::GetParent(hWnd),WM_COMMAND,MAKEWPARAM(IDM_INFOSFICHIERSIZECHANGE,0),0);

					if(!m_pPreviewData->m_cExif.m_exifinfo->IsExif)
					{
						if(m_pPreviewData->iShowVideo == 0)
						{
							//Largeur de l'image
							m_pPreviewData->m_cExif.m_exifinfo->Width = m_pPreviewData->m_ciBitmap.GetBitmapWidth();
							
							//Hauteur de l'image
							m_pPreviewData->m_cExif.m_exifinfo->Height = m_pPreviewData->m_ciBitmap.GetBitmapHeigth();
						}
						else
						{
							CVideoAPI * m_cVideo = new CVideoAPI();
							long m_lWidth = 0;
							long m_lHeight = 0;
							m_cVideo->GetVideoSize(m_pPreviewData->cFile,m_lWidth,m_lHeight);

							//Largeur de l'image
							m_pPreviewData->m_cExif.m_exifinfo->Width = m_lWidth;
							
							//Hauteur de l'image
							m_pPreviewData->m_cExif.m_exifinfo->Height = m_lHeight;

							delete m_cVideo;
						}
					}

					if(m_pPreviewData->iAlbum == 1)
					{


						//Affichage du bouton mots clés
						LoadString(hInstance,IDS_STGTABLEINFOFILE,m_szInfos,512);
						m_pPreviewData->m_cExif.m_exifinfo->m_stgHTMLDisplay = m_szInfos;	

						LoadString(hInstance,IDS_STGAFFKEYWORD,m_szMessage,512);

#ifdef SQLSERVERCE

						AttributVector m_AttributVector;
						//Récupération des informations sur le fichier dans la base de données


						CSqlAlbumIconeFileAttribut * m_cSqlAlbumIconeFileAttribut = new CSqlAlbumIconeFileAttribut();
						m_cSqlAlbumIconeFileAttribut->LoadAlbumIconeFileAttribut(&m_AttributVector, m_pPreviewData->IconeFileTemp->szKeyName, m_pPreviewData->IconeFileTemp->m_stgFullPathName);
						delete m_cSqlAlbumIconeFileAttribut;
						 
						AttributVector::iterator AttributIt;
						for (AttributIt = m_AttributVector.begin(); AttributIt!= m_AttributVector.end(); AttributIt++)
						{
							CAttribut m_AttributTemp;
							CAttribut m_AttributTemp2;
							m_AttributTemp = *AttributIt;

							if(m_pPreviewData->m_AttributVector != NULL)
							{
								m_AttributTemp2 = m_pPreviewData->m_AttributVector->at(m_AttributTemp.iNumAttribut - 1);				
								StringCchPrintf(m_szTemp,512,m_szMessage,m_AttributTemp2.cLibelle,m_AttributTemp.cLibelle);
								m_pPreviewData->m_cExif.m_exifinfo->m_stgHTMLDisplay += m_szTemp;
							}
							else
							{
								char cLocalTemp[50];
								StringCchPrintf(cLocalTemp,50,"%d",m_AttributTemp.iNumAttribut);
								StringCchPrintf(m_szTemp,512,m_szMessage,cLocalTemp,m_AttributTemp.cLibelle);
								m_pPreviewData->m_cExif.m_exifinfo->m_stgHTMLDisplay += m_szTemp;

							}
						}
#else

						AttributVector::iterator AttributIt;
						for (AttributIt = m_pPreviewData->IconeFileTemp->m_AttributVector.begin(); AttributIt!= m_pPreviewData->IconeFileTemp->m_AttributVector.end(); AttributIt++)
						{
							CAttribut m_AttributTemp;
							CAttribut m_AttributTemp2;
							m_AttributTemp = *AttributIt;

							if(m_pPreviewData->m_AttributVector != NULL)
							{
								m_AttributTemp2 = m_pPreviewData->m_AttributVector->at(m_AttributTemp.iNumAttribut - 1);				
								StringCchPrintf(m_szTemp,512,m_szMessage,m_AttributTemp2.cLibelle,m_AttributTemp.cLibelle);
								m_pPreviewData->m_cExif.m_exifinfo->m_stgHTMLDisplay += m_szTemp;
							}
							else
							{
								char cLocalTemp[50];
								StringCchPrintf(cLocalTemp,50,"%d",m_AttributTemp.iNumAttribut);
								StringCchPrintf(m_szTemp,512,m_szMessage,cLocalTemp,m_AttributTemp.cLibelle);
								m_pPreviewData->m_cExif.m_exifinfo->m_stgHTMLDisplay += m_szTemp;

							}
						}

#endif



						m_pPreviewData->m_cExif.m_exifinfo->m_stgHTMLDisplay += "</table>";
						//Affichage des mots clés
						//Affichage du bouton mots clés
						LoadString(hInstance,IDS_STGFORMINFOFILE,m_szInfos,512);
						m_pPreviewData->m_cExif.m_exifinfo->m_stgHTMLDisplay += m_szInfos;
					}

					SendMessage(m_cWebInfoFichier->m_hWnd,WM_COMMAND,MAKEWPARAM(IDM_HTMLEXIF,m_pPreviewData->iAlbum),(LPARAM)&m_pPreviewData->m_cExif);
			}
			break;
	}
	return 0;
}

LRESULT CInfosFichierProc::OnSize(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	GetClientRect(hDlg,&rc);
	MoveWindow(m_cWebInfoFichier->m_hWnd,0,0,rc.right-1,rc.bottom-1,false);
	SendMessage(GetParent(hDlg),WM_COMMAND,MAKEWPARAM(IDM_INFOSFICHIERSIZECHANGE,0),lParam);
	return 0;
}

LRESULT CInfosFichierProc::OnDestroy(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_cWebInfoFichier != NULL)
	{
		DestroyWindow(m_cWebInfoFichier->m_hWnd);
		delete m_cWebInfoFichier;
	}
	m_cWebInfoFichier = NULL;
	return 0;
}
