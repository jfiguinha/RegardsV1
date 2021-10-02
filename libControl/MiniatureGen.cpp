#include "stdafx.h"
#include ".\miniaturegen.h"
#include "LoadImage.h"
#include "MiniatureGenData.h"
#include <queue>
#include <deque>
using namespace std;

#define TIMER_KILLERTHREAD 0x01001
#define TIME_MAX 2000
#define TIMER_MAXSTANDBY 0x01002
#define WM_ENDAFTEREMPTY 0x01003

typedef deque<CMiniatureGenData *, allocator<CMiniatureGenData *> > PTMINIATUREGENDATADEQUE;
typedef queue<CMiniatureGenData *,PTMINIATUREGENDATADEQUE> PTMINIATUREGENDATAQUEUE;

CMiniatureGen::CMiniatureGen(void)
{
	m_hWaitThread = NULL;
	m_hThread = NULL;
}

CMiniatureGen::~CMiniatureGen(void)
{
	EndDialog(m_hWnd,0);

	if(m_hWaitThread != NULL)
	{
		::CloseHandle(m_hWaitThread);
		m_hWaitThread = NULL;
	}

	if(m_hThread != NULL)
	{
		::CloseHandle(m_hThread);
		m_hThread = NULL;
	}

}

LRESULT CMiniatureGen::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_hWaitThread = CreateEvent(0, TRUE, FALSE, 0);
	m_hThread = CreateThread(0, 0, threadProc, this, 0, &threadID);
	return 0;
}

 unsigned long __stdcall CMiniatureGen::threadProc(void* ptr)
 {
    OleInitialize(NULL);
	PTMINIATUREGENDATAQUEUE m_ptMiniatureGenDataQueue;
	CMiniatureGen * m_cMiniatureGen = (CMiniatureGen *)ptr;

	bool m_bQuitIfEmpty = false;
	bool m_bEnd = false;
	bool m_bStop = false;
	try
	{
		MSG msg;

		while (!m_bEnd && !(m_bQuitIfEmpty && m_ptMiniatureGenDataQueue.empty()))
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				switch(msg.message)
				{
					case WM_ENDAFTEREMPTY:
						{
							m_bQuitIfEmpty = true;
							break;
						}

					case WM_QUIT:
						{
							// signale l'objet event d'attente et sort.
							m_bEnd = true;
							break;
						}

					case WM_STOP_DOWNLOAD_AUTO:
						{
							m_bStop = true;
							break;
						}

					case WM_DOWNLOAD_ONESHOT:
						{
							CMiniatureGenData * cMiniatureGenData = (CMiniatureGenData *)msg.lParam;
							m_ptMiniatureGenDataQueue.push(cMiniatureGenData);
						}
						break;
					default :
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					break;
				}
			}
			else
			{
				if(!m_ptMiniatureGenDataQueue.empty())
				{
					CMiniatureGenData * cMiniatureGenData = (CMiniatureGenData * )m_ptMiniatureGenDataQueue.front();
					int m_iResult = m_cMiniatureGen->Run(cMiniatureGenData);
					PostThreadMessage(cMiniatureGenData->GetThreadId(),WM_DOWNLOAD_ONESHOT,0,(LPARAM)cMiniatureGenData);
					Sleep(100);
					m_ptMiniatureGenDataQueue.pop();
				}
				else if(m_bStop)
				{
					m_bEnd = true;
				}
			}	
		}
	}
	catch (...)
	{

	}


	//Nettoyage
	while(!m_ptMiniatureGenDataQueue.empty())
	{
		CMiniatureGenData * cMiniatureGenData = (CMiniatureGenData * )m_ptMiniatureGenDataQueue.front();
		m_ptMiniatureGenDataQueue.pop();
		delete cMiniatureGenData;
	}

	::SetEvent(m_cMiniatureGen->m_hWaitThread);

	PostThreadMessage(m_cMiniatureGen->m_dwThreadID,WM_TERMINER_THREAD,0,0);

	OleUninitialize();

	return 99;
 }

int CMiniatureGen::Run(CMiniatureGenData * cMiniatureGenData)
{
	HBITMAP hBitmap;
	char m_szFilename[MAX_PATH];
	strcpy_s(m_szFilename,cMiniatureGenData->GetFilename().c_str());
	try
	{
		CLoadImage m_ChargementImage;
		hBitmap = m_ChargementImage.ChargementIconeStatic(m_szFilename, cMiniatureGenData->GetLargeur(), cMiniatureGenData->GetHauteur(), 0, 0, 1, RGB(255,255,255),RGB(230,230,230),cMiniatureGenData->GetIfFolder());			
		if(hBitmap != NULL)
		{
			cMiniatureGenData->SetBitmapResult(hBitmap);
			DeleteObject(hBitmap);
			return 1;
		}
	}
	catch(...)
	{
		//char Temp[255];
		//LoadString(hInstance,IDS_ERREUR,Temp,255);
		//::MessageBox(NULL,m_szFilename,Temp,0);
	}
	return 0;
}

LRESULT CMiniatureGen::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int	wmId    = LOWORD(wParam);
	int	wmEvent = HIWORD(wParam);

	switch (wmId)
	{
		case IDC_TRAITEMENTDATA:
			{
				CMiniatureGenData * cMiniatureGenData = (CMiniatureGenData *)lParam;
				PostThreadMessage(threadID,WM_DOWNLOAD_ONESHOT,0,(LPARAM)cMiniatureGenData);
				KillTimer(hDlg,TIMER_MAXSTANDBY);
				Sleep(20);
				SetTimer(hDlg,TIME_MAX,TIME_MAX,NULL);
				break;
			}

		case WM_STOP_DOWNLOAD_AUTO:
			{
				CMiniatureGenData * cMiniatureGenData = (CMiniatureGenData *)lParam;
				PostThreadMessage(threadID,WM_STOP_DOWNLOAD_AUTO,0,0);
				Sleep(20);
				break;
			}

		case IDC_ENDTRAITEMENTDATA:
			{
				PostThreadMessage(threadID,WM_QUIT,0,0);
				KillTimer(hDlg,TIMER_MAXSTANDBY);
				::WaitForSingleObject(m_hWaitThread, 2000);
				break;
			}

		default:
			return DefWindowProc(hDlg, uMsg, wParam, lParam);
	}
	return 0;
}

LRESULT CMiniatureGen::OnTimer(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
		case TIMER_KILLERTHREAD:
			KillTimer(hDlg,TIMER_KILLERTHREAD);
			if(m_hThread != NULL)
			{
				TerminateThread(m_hThread,0);
			}
			break;

		case TIMER_MAXSTANDBY:
			{
				PostThreadMessage(threadID,WM_ENDAFTEREMPTY, 0, 0);
				KillTimer(hDlg,TIMER_MAXSTANDBY);
				break;
			}
	}

	return 0;
}
