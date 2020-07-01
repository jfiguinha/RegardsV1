#include "StdAfx.h"
#include ".\insertalbum.h"
#include <AlbumData.h>
#include "..\resource.h"
#include "..\objet.h"
#include "attributdata.h"
#ifdef SQLSERVERCE
#include <SqlAlbumUtility.h>
#include <SqlIconeFileAlbum.h>
using namespace Regards::Sqlite;

#endif

CInsertAlbum::CInsertAlbum(HINSTANCE hInstance)
{
	hThread = NULL;
	this->hInstance = hInstance;
}

CInsertAlbum::~CInsertAlbum(void)
{
	if(hThread != NULL)
	{
		CloseHandle(hThread);
		hThread = NULL;
	}
}

LRESULT CInsertAlbum::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_hDlg = hDlg;
	m_bTerminer = 0;

	switch(iCas)
	{
		case 1:
			SendDlgItemMessage(hDlg, IDC_PRGFILE, PBM_SETRANGE , (WPARAM)0, MAKELPARAM(0,m_IconeFileVector->size()));
			break;
	}
		
	SendDlgItemMessage(hDlg, IDC_PRGFILE, PBM_SETSTEP , (WPARAM)1, 0);

	hThread = CreateThread(NULL, cuStackSize, (LPTHREAD_START_ROUTINE)AjoutImageThread, this, 0, &dwThreadId);

	return 0;
}

LRESULT CInsertAlbum::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	wmId    = LOWORD(wParam); 
	wmEvent = HIWORD(wParam); 
	//DWORD ExitCode;

	int bResult = PostThreadMessage(dwThreadId, WM_NULL, 0, 0);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////

int CInsertAlbum::VerifPresenceImage(CIconeFile &m_IconeFile,IconeFileVector * m_IconeFileVector)
{
	IconeFileVector::iterator dit;
	MSG msg;

	for (dit = m_IconeFileVector->begin(); dit != m_IconeFileVector->end(); dit++)
	{

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//printf("Thread Two Received a message! Looped %d times\n", nIdleCount);
			//fflush(stdout);
			return 2;
		}
		else
		{
			CIconeFile m_IconeFileLocal = *dit;
			if(_stricmp(m_IconeFileLocal.m_stgFullPathName,m_IconeFile.m_stgFullPathName) == 0)
				return 1;

			if(_stricmp(m_IconeFileLocal.m_stgFullPathName,m_IconeFile.m_stgFullPathName) > 0)
				return 0;
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//Thread d'ajout d'image à un catalogue
///////////////////////////////////////////////////////////////////////////////////////////////////////////

UINT _stdcall CInsertAlbum::AjoutImageThread(LPVOID parametre)
{
	CInsertAlbum * m_LPAjoutImageData = (CInsertAlbum *)parametre;
	//m_LPAjoutImageData->pInfo->SetInitialized();

	char clpCaption[255];
	LoadString(m_LPAjoutImageData->hInstance,IDS_SELECTIONALBUM,clpCaption,255);
	MSG msg;
	int nIdleCount;
	long m_lTimer = 0L;

	switch(m_LPAjoutImageData->iCas)
	{
		case 1:
			{
				char m_cTempMessage[255];
				LoadString(m_LPAjoutImageData->hInstance,IDS_INSERTALBUMMSG1,m_cTempMessage,255);

				SendMessage(m_LPAjoutImageData->m_hDlg,WM_SETTEXT,(WPARAM)0, (LPARAM)m_cTempMessage);
				IconeFileVector::iterator dit;
				int iSelCount = 0;
				//Test si Mode Mise à jour ou creation
				if(m_LPAjoutImageData->m_AlbumconeFileVector->size() > 0)
					m_LPAjoutImageData->m_AlbumconeFileVector->clear();

				for (dit = m_LPAjoutImageData->m_IconeFileVector->begin(); dit != m_LPAjoutImageData->m_IconeFileVector->end(); dit++)
				{
					//Sleep(m_lTimer);

					if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
					{
						nIdleCount = 0;
						break;
					}
					else
					{
						//IconeFile m_IconeFile = *dit;
						m_LPAjoutImageData->m_AlbumconeFileVector->push_back(*dit);

						//Récupération du nom de fichier
						CObjet m_cObjet;
						char Filename[255];
						m_cObjet.GetFileName(dit->m_stgFullPathName,Filename,255);
			
						SendDlgItemMessage(m_LPAjoutImageData->m_hDlg, IDC_FILEIN, WM_SETTEXT , (WPARAM)0, (LPARAM)Filename);
						SendDlgItemMessage(m_LPAjoutImageData->m_hDlg, IDC_FILEOUT, WM_SETTEXT , (WPARAM)0, (LPARAM)m_LPAjoutImageData->m_Album->m_szAlbumName);
						SendDlgItemMessage(m_LPAjoutImageData->m_hDlg, IDC_PRGFILE, PBM_SETPOS , (WPARAM)iSelCount, 0);
						iSelCount++;
						m_LPAjoutImageData->m_Album->iNbFiles = iSelCount;
					}

				}
					
				
			}
			break;

		case 2:
			{
				char m_cTempMessage[255];
				LoadString(m_LPAjoutImageData->hInstance,IDS_INSERTALBUMMSG2,m_cTempMessage,255);
				SendMessage(m_LPAjoutImageData->m_hDlg,WM_SETTEXT,(WPARAM)0, (LPARAM)m_cTempMessage);

				int iSelCount;
				iSelCount = SendDlgItemMessage(m_LPAjoutImageData->m_hWndExtern, IDC_LSIMAGEDIR, LB_GETCOUNT, 0, 0);

				SendDlgItemMessage(m_LPAjoutImageData->m_hDlg, IDC_PRGFILE, PBM_SETRANGE , (WPARAM)0, MAKELPARAM(0,iSelCount));

				try
				{
					// let our creator know we're initialized
					//m_LPAjoutImageData->pInfo->SetInitialized();

					MSG msg;int i = 0;
					int nIdleCount = 0;
					IconeFileVector::iterator dit;

					IconeFileVector * m_IconeFileVector = m_LPAjoutImageData->m_IconeFileVector;

					dit = m_LPAjoutImageData->m_IconeFileVector->begin();


					while (i < iSelCount)
					{
						if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
						{
							//printf("Thread Two Received a message! Looped %d times\n", nIdleCount);
							//fflush(stdout);
							nIdleCount = 0;
							break;
						}
						else
						{
							CIconeFile m_IconeFile;
							CSAFileEntry fit;
							CObjet m_cObjet;

							fit = m_LPAjoutImageData->files->at(i);
							m_cObjet.AlbumGestionAddItem(fit,m_IconeFile);

							//AlbumGestionAddItem(m_LPAjoutImageData->m_hWndExtern,i,fit,m_IconeFile);
							//Vérifie si l'image existe ou non dans la liste

							int iReturnValue = 0;
							iReturnValue = m_LPAjoutImageData->VerifPresenceImage(m_IconeFile, m_IconeFileVector);

							if(iReturnValue == 0)
							{
								char FileName[MAX_PATH];
								SendDlgItemMessage(m_LPAjoutImageData->m_hWndExtern, IDC_LSIMAGEDIR, LB_GETTEXT , i, (LPARAM)FileName);
								SendDlgItemMessage(m_LPAjoutImageData->m_hWndExtern, IDC_LBALBUMIMAGE, LB_INSERTSTRING , i, (LPARAM)FileName);
								SendDlgItemMessage(m_LPAjoutImageData->m_hWndExtern, IDC_LBALBUMIMAGE, WM_SETTEXT , (WPARAM)0, (LPARAM)FileName);
								m_LPAjoutImageData->m_IconeFileVector->push_back(m_IconeFile);

								SendDlgItemMessage(m_LPAjoutImageData->m_hDlg, IDC_FILEIN, WM_SETTEXT , (WPARAM)0, (LPARAM)FileName);
								SendDlgItemMessage(m_LPAjoutImageData->m_hDlg, IDC_FILEOUT, WM_SETTEXT , (WPARAM)0, (LPARAM)clpCaption);
								SendDlgItemMessage(m_LPAjoutImageData->m_hDlg, IDC_PRGFILE, PBM_SETPOS , (WPARAM)i, 0);
							}
							else
							{
								if(iReturnValue == 2)
									break;
							}

							i++;
						}

					}
				}
				catch (...)
				{
				}

				break;
			}

		case 3:
			{
				char m_cTempMessage[255];
				LoadString(m_LPAjoutImageData->hInstance,IDS_INSERTALBUMMSG3,m_cTempMessage,255);
				SendMessage(m_LPAjoutImageData->m_hDlg,WM_SETTEXT,(WPARAM)0, (LPARAM)m_cTempMessage);

				int iSelCount;

				IconeFileVector m_IconeFileVector = *m_LPAjoutImageData->m_IconeFileVector;

				iSelCount = SendDlgItemMessage(m_LPAjoutImageData->m_hWndExtern, IDC_LSIMAGEDIR, LB_GETSELCOUNT , 0, 0);

				SendDlgItemMessage(m_LPAjoutImageData->m_hDlg, IDC_PRGFILE, PBM_SETRANGE , (WPARAM)0, MAKELPARAM(0,iSelCount));

				int * iNumSelected = new int[iSelCount + 1];
				if (iNumSelected == NULL)
				{
					CObjet::ErreurInformation();
				}

				SendDlgItemMessage(m_LPAjoutImageData->m_hWndExtern, IDC_LSIMAGEDIR, LB_GETSELITEMS , iSelCount, (LPARAM)iNumSelected);
					
				IconeFileVector::iterator dit;

				dit = m_LPAjoutImageData->m_IconeFileVector->begin();
				

				for(int i = 0;i < iSelCount;i++)
				{
					if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
					{
						nIdleCount = 0;
						break;
					}
					else
					{
						CIconeFile m_IconeFile;
						CSAFileEntry fit;
						CObjet m_cObjet;

						fit = m_LPAjoutImageData->files->at(iNumSelected[i]);
						m_cObjet.AlbumGestionAddItem(fit,m_IconeFile);
						//AlbumGestionAddItem(m_LPAjoutImageData->m_hWndExtern,i,fit,m_IconeFile);
						//Vérifie si l'image existe ou non dans la liste
						int iReturnValue = 0;
						iReturnValue = m_LPAjoutImageData->VerifPresenceImage(m_IconeFile, &m_IconeFileVector);

						if(iReturnValue == 0)
						{
							char FileName[MAX_PATH];
							SendDlgItemMessage(m_LPAjoutImageData->m_hWndExtern, IDC_LSIMAGEDIR, LB_GETTEXT , iNumSelected[i], (LPARAM)FileName);
							SendDlgItemMessage(m_LPAjoutImageData->m_hWndExtern, IDC_LBALBUMIMAGE, LB_INSERTSTRING , i, (LPARAM)FileName);
							SendDlgItemMessage(m_LPAjoutImageData->m_hWndExtern, IDC_LBALBUMIMAGE, WM_SETTEXT , (WPARAM)0, (LPARAM)FileName);
							m_LPAjoutImageData->m_IconeFileVector->push_back(m_IconeFile);
							SendDlgItemMessage(m_LPAjoutImageData->m_hDlg, IDC_FILEIN, WM_SETTEXT , (WPARAM)0, (LPARAM)FileName);
							SendDlgItemMessage(m_LPAjoutImageData->m_hDlg, IDC_FILEOUT, WM_SETTEXT , (WPARAM)0, (LPARAM)clpCaption);
							SendDlgItemMessage(m_LPAjoutImageData->m_hDlg, IDC_PRGFILE, PBM_SETPOS , (WPARAM)i, 0);
						}
						else
						{
							if(iReturnValue == 2)
								break;
						}
					}
				}
				delete[] iNumSelected;

				break;
			}

		case 4:
			{
#ifdef SQLSERVERCE

				char m_cTempMessage[255];
				LoadString(m_LPAjoutImageData->hInstance,IDS_INSERTALBUMMSG4,m_cTempMessage,255);
				SendMessage(m_LPAjoutImageData->m_hDlg,WM_SETTEXT,(WPARAM)0, (LPARAM)m_cTempMessage);

				int iSelectItem;
				//Sélection de tous les éléments de la liste
				LVITEM lvItem;
				ZeroMemory(&lvItem,sizeof(LVITEM));
				lvItem.stateMask = LVIS_SELECTED;
				lvItem.state = LVIS_SELECTED;
				nIdleCount = 0;
				SendDlgItemMessage(m_LPAjoutImageData->m_hWndExtern, IDC_LISTATTRIBUT, LVM_SETITEMSTATE, -1, (LPARAM) (LPLVITEM) &lvItem);

				AttributVector m_attributVector;
				//On récupère la mise à jour des attributs
				iSelectItem = -1;
				while((iSelectItem = SendDlgItemMessage(m_LPAjoutImageData->m_hWndExtern, IDC_LISTATTRIBUT, LVM_GETNEXTITEM,(WPARAM) iSelectItem, MAKELPARAM((UINT) LVNI_SELECTED, 0))) != -1)
				{
					CAttributData * m_cAttributData = new CAttributData(m_LPAjoutImageData->hInstance);
					CAttribut m_AttributTemp;
					m_cAttributData->SetSelectItem(iSelectItem);
					m_cAttributData->GetAttributValue(m_AttributTemp,m_LPAjoutImageData->m_hWndExtern);
					m_attributVector.push_back(m_AttributTemp);
					delete m_cAttributData;
				}
	
				//Mise à jour des attributs pour les images
				size_t m_sizeTConvert;
				CSqlAlbumUtility::UpdateFileAttribut(&m_attributVector, m_LPAjoutImageData->m_Album->m_szAlbumName);

				if(nIdleCount == 1)
				{
					break;
				}

				//Mise à jour de la liste des attributs
				iSelectItem = -1;
				m_LPAjoutImageData->m_AttributVector->clear();
				while((iSelectItem = SendDlgItemMessage(m_LPAjoutImageData->m_hWndExtern, IDC_LISTATTRIBUT, LVM_GETNEXTITEM,(WPARAM) iSelectItem, MAKELPARAM((UINT) LVNI_SELECTED, 0))) != -1)
				{
					if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
					{
						nIdleCount = 0;
						break;
					}
					else
					{
							CAttribut m_AttributTemp;
							char cTemp[255];
							LVITEM lvItem;
			
							ZeroMemory(&lvItem,sizeof(LVITEM));
							lvItem.cchTextMax = 254;
							lvItem.iSubItem = 1;
							lvItem.pszText = cTemp;
							SendDlgItemMessage(m_LPAjoutImageData->m_hWndExtern, IDC_LISTATTRIBUT, LVM_GETITEMTEXT,(WPARAM) iSelectItem,(LPARAM) (LPLVITEM) &lvItem);

							StringCchCopy(m_AttributTemp.cLibelle,255,cTemp);
							ZeroMemory(&lvItem,sizeof(LVITEM));
							lvItem.mask = TVIF_TEXT;
							lvItem.cchTextMax = 254;
							lvItem.iItem = iSelectItem;
							lvItem.iSubItem = 2;
							lvItem.pszText = cTemp;
							SendDlgItemMessage(m_LPAjoutImageData->m_hWndExtern, IDC_LISTATTRIBUT, LVM_GETITEMTEXT,(WPARAM) iSelectItem,(LPARAM) (LPLVITEM) &lvItem);
							
							m_AttributTemp.iNumAttribut = atoi(cTemp);

							m_LPAjoutImageData->m_AttributVector->push_back(m_AttributTemp);
					}
				}

#else
				char m_cTempMessage[255];
				LoadString(m_LPAjoutImageData->hInstance,IDS_INSERTALBUMMSG4,m_cTempMessage,255);
				SendMessage(m_LPAjoutImageData->m_hDlg,WM_SETTEXT,(WPARAM)0, (LPARAM)m_cTempMessage);

				int iSelectItem;
				IconeFileVector::iterator dit;
				//Sélection de tous les éléments de la liste
				LVITEM lvItem;
				ZeroMemory(&lvItem,sizeof(LVITEM));
				lvItem.stateMask = LVIS_SELECTED;
				lvItem.state = LVIS_SELECTED;
				nIdleCount = 0;
				SendDlgItemMessage(m_LPAjoutImageData->m_hWndExtern, IDC_LISTATTRIBUT, LVM_SETITEMSTATE, -1, (LPARAM) (LPLVITEM) &lvItem);

				for (dit = m_LPAjoutImageData->m_Album->m_IconeFileVector.begin(); dit != m_LPAjoutImageData->m_Album->m_IconeFileVector.end(); dit++)
				{
					if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
					{
						nIdleCount = 1;
						break;
					}
					else
					{
						int iNumAttribut = 0;
						//IconeFile * m_IconeFile = (IconeFile *)dit;

						//Sauvegarde de la liste des attributs dans le vecteur
						if(m_LPAjoutImageData->iValueChecked != 1)
						{
							dit->m_AttributVector.clear();
						}

						iSelectItem = -1;
						while((iSelectItem = SendDlgItemMessage(m_LPAjoutImageData->m_hWndExtern, IDC_LISTATTRIBUT, LVM_GETNEXTITEM,(WPARAM) iSelectItem, MAKELPARAM((UINT) LVNI_SELECTED, 0))) != -1)
						{
							CAttributData * m_cAttributData = new CAttributData(m_LPAjoutImageData->hInstance);
							CAttribut m_AttributTemp;
							m_cAttributData->SetSelectItem(iSelectItem);
							m_cAttributData->GetAttributValue(m_AttributTemp,m_LPAjoutImageData->m_hWndExtern);
							dit->m_AttributVector.push_back(m_AttributTemp);
							iNumAttribut++;
							delete m_cAttributData;

						}
						dit->iNbAttribut = iNumAttribut;
					}
				}

				if(nIdleCount == 1)
				{
					break;
				}


				//Mise à jour de la liste des attributs
				iSelectItem = -1;
				m_LPAjoutImageData->m_AttributVector->clear();
				while((iSelectItem = SendDlgItemMessage(m_LPAjoutImageData->m_hWndExtern, IDC_LISTATTRIBUT, LVM_GETNEXTITEM,(WPARAM) iSelectItem, MAKELPARAM((UINT) LVNI_SELECTED, 0))) != -1)
				{
					if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
					{
						nIdleCount = 0;
						break;
					}
					else
					{
							CAttribut m_AttributTemp;
							char cTemp[255];
							LVITEM lvItem;
			
							ZeroMemory(&lvItem,sizeof(LVITEM));
							lvItem.cchTextMax = 254;
							lvItem.iSubItem = 1;
							lvItem.pszText = cTemp;
							SendDlgItemMessage(m_LPAjoutImageData->m_hWndExtern, IDC_LISTATTRIBUT, LVM_GETITEMTEXT,(WPARAM) iSelectItem,(LPARAM) (LPLVITEM) &lvItem);

							StringCchCopy(m_AttributTemp.cLibelle,255,cTemp);
							ZeroMemory(&lvItem,sizeof(LVITEM));
							lvItem.mask = TVIF_TEXT;
							lvItem.cchTextMax = 254;
							lvItem.iItem = iSelectItem;
							lvItem.iSubItem = 2;
							lvItem.pszText = cTemp;
							SendDlgItemMessage(m_LPAjoutImageData->m_hWndExtern, IDC_LISTATTRIBUT, LVM_GETITEMTEXT,(WPARAM) iSelectItem,(LPARAM) (LPLVITEM) &lvItem);
							
							m_AttributTemp.iNumAttribut = atoi(cTemp);

							m_LPAjoutImageData->m_AttributVector->push_back(m_AttributTemp);
					}
				}
#endif
			}
			break;

	}

	EndDialog(m_LPAjoutImageData->m_hDlg, 0);

	return 0;
}