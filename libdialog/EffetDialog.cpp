#include "StdAfx.h"
#include ".\effetdialog.h"
#include <dialogfiltreeffet.h>
#include <ibitmap.h>
#include <effet.h>

unsigned long __stdcall CEffetDialog::AppelTraitementDialog(void * param)
{
	OleInitialize(NULL);
	HWND hDlg = (HWND)param;
	SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(APPLIQFILTREEFFET,0),0);
	OleUninitialize();
	return 0;
}

CEffetDialog::CEffetDialog(CFiltreEffet * lpFiltreEffet)
{
	m_iReturnValue = 0;
	m_lpFiltreEffet = lpFiltreEffet;
	hThread = NULL;
}

CEffetDialog::~CEffetDialog(void)
{
	if(hThread != NULL)
		CloseHandle(hThread);

	hThread = NULL;
}

LRESULT CEffetDialog::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	OutputDebugString("CEffetDialog::OnInitDialog");

	if(m_lpFiltreEffet->iAffichage == 1)
	{
		m_lpFiltreEffet->hDlg = hDlg;
		hThread = (HANDLE)	CreateThread(NULL, cuStackSize, AppelTraitementDialog, hDlg, 0, &dwThreadId);
		SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);
	}
	else
		SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(APPLIQFILTREEFFET,0),0);

	return 0;
}

LRESULT CEffetDialog::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	wmId    = LOWORD(wParam); 
	wmEvent = HIWORD(wParam); 
	
	switch (wmId)
	{
		case APPLIQFILTREEFFET:
			{
				HBITMAP hBitmap;
				CEffet cEffet(m_lpFiltreEffet);
				switch(m_lpFiltreEffet->NumFiltre)
				{
 					case EFFET_ROTATION:
						{
							if(m_lpFiltreEffet->m_lParam[1] == 0)
							{
								switch(m_lpFiltreEffet->m_lParam[0])
								{
									case 90:
										m_lpFiltreEffet->m_ciBitmap->Rotation90();
										m_lpFiltreEffet->m_ciBitmap->HorzFlipBuf();
										break;

									case 180:
										m_lpFiltreEffet->m_ciBitmap->VertFlipBuf();
										break;

									case 270:

										m_lpFiltreEffet->m_ciBitmap->HorzFlipBuf();
										m_lpFiltreEffet->m_ciBitmap->Rotation90();
										
										break;
								}
							}
							else
							{
								cEffet.GetFreeRotatedBitmap(m_lpFiltreEffet->m_ciBitmap,m_lpFiltreEffet->m_lParam[0],RGB(0,0,0));
							}
						}
 						break;

 					case EFFET_FLIP:
						{
							if(m_lpFiltreEffet->m_ciBitmap->GetPtBitmap() != NULL)
							{
								if(m_lpFiltreEffet->m_lParam[0])
									m_lpFiltreEffet->m_ciBitmap->HorzFlipBuf();
								else
									m_lpFiltreEffet->m_ciBitmap->VertFlipBuf();
							}
							break;
						}

 					case EFFET_SCALE:
						{
 							cEffet.ScaleBitmap(m_lpFiltreEffet->m_ciBitmap, m_lpFiltreEffet->m_lParam[0], m_lpFiltreEffet->m_lParam[1], true);
						}
 						break;

 					case EFFET_LOWSCALE:
						{
 							cEffet.ScaleBitmap(m_lpFiltreEffet->m_ciBitmap, m_lpFiltreEffet->m_lParam[0], m_lpFiltreEffet->m_lParam[1], true);
						}
 						break;
				}
			}


		case IDOK:
			m_iReturnValue = 1;
			if(hThread != NULL)
				CloseHandle(hThread);

			hThread = NULL;
			EndDialog(hDlg, LOWORD(1));
			return TRUE;

		case IDCANCEL:
			{
				m_iReturnValue = 0;
				DWORD ExitCode;
				if(hThread != NULL)
				{
					GetExitCodeThread(hThread,&ExitCode);
					TerminateThread(hThread,ExitCode);
					CloseHandle(hThread);
					hThread = NULL;
				}
				EndDialog(hDlg, LOWORD(0));
			}
			return TRUE;


	}
	return 0;
}