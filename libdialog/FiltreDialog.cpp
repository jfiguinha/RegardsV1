#include "StdAfx.h"
#include ".\filtredialog.h"
#include <dialogfiltreeffet.h>
#include "resource.h"
#include <filtre.h>
#include <process.h>

CFiltreDialog::CFiltreDialog(CFiltreEffet * lpFiltreEffet)
{
	m_iReturnValue = 0;
	m_lpFiltreEffet = lpFiltreEffet;
}

CFiltreDialog::~CFiltreDialog(void)
{
}

unsigned long __stdcall CFiltreDialog::AppelTraitementDialog(void * param)
{
	OleInitialize(NULL);
	HWND hDlg = (HWND)param;
	SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(APPLIQFILTREEFFET,0),0);
	OleUninitialize();
	return 0;
}

LRESULT CFiltreDialog::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_lpFiltreEffet->hDlg = hDlg;
	if(m_lpFiltreEffet->iAffichage == 1)
	{
		hThread = CreateThread(NULL, cuStackSize, AppelTraitementDialog, hDlg, 0, &dwThreadId);
		SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);
	}
	else
		SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(APPLIQFILTREEFFET,0),0);

	return 0;
}

LRESULT CFiltreDialog::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	wmId    = LOWORD(wParam); 
	wmEvent = HIWORD(wParam); 
	
	
	switch (wmId)
	{
		case APPLIQFILTREEFFET:
			{
				CFiltre cFiltre(m_lpFiltreEffet);
				cFiltre.AppliquerFiltre(m_lpFiltreEffet);
			}

		case IDOK:
			m_iReturnValue = 1;
			CloseHandle(hThread);
			EndDialog(hDlg, LOWORD(1));
			return TRUE;

		case IDCANCEL:
			{
				m_iReturnValue = 0;
				DWORD ExitCode;
				GetExitCodeThread(hThread,&ExitCode);
				TerminateThread(hThread,ExitCode);
				CloseHandle(hThread);
				EndDialog(hDlg, LOWORD(0));
			}
			return TRUE;

	}
	return 0;
}