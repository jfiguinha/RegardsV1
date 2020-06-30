#pragma once

#ifndef __IDiscMasterProgressEventsImpl__H__
#define __IDiscMasterProgressEventsImpl__H__

#include "Imapi.h"
#include "..\resource.h"
#include "..\include\global.h"
#include <commctrl.h>
#include <strsafe.h>

class IDiscMasterProgressEventsImpl : public IDiscMasterProgressEvents
{
public:

	IDiscMasterProgressEventsImpl(void);
	~IDiscMasterProgressEventsImpl(void);
	IDiscMasterProgressEventsImpl(HWND hWnd);

	STDMETHOD(QueryInterface)( REFIID riid, void** ppv )
	{
		if( riid == IID_IUnknown )
			*ppv = static_cast<IDiscMasterProgressEvents*>(this);
		else if( riid == IID_IDiscMasterProgressEvents )
			*ppv = static_cast<IDiscMasterProgressEvents*>(this);
		else
		{
			*ppv = NULL;
			return E_NOINTERFACE;
		}

		static_cast<IUnknown*>(*ppv)->AddRef();

		return S_OK;
	}

	STDMETHOD_(ULONG,AddRef)()
	{
		return InterlockedIncrement(&m_lRefCount);
	}

	STDMETHOD_(ULONG,Release)()
	{
		ULONG ul = InterlockedDecrement(&m_lRefCount);

		if( ul == 0 )
			delete this;

		return ul;
	}

	STDMETHOD(QueryCancel)( boolean* pbCancel )
	{
		return E_NOTIMPL;
	}

	STDMETHOD(NotifyPnPActivity)()
	{
		return E_NOTIMPL;
	}

	STDMETHOD(NotifyAddProgress)( long nCompletedSteps, long nTotalSteps )
	{
		char Temp[250];
		StringCchPrintf(Temp,250,"Position : %d / %d",nCompletedSteps,nTotalSteps);

		SendDlgItemMessage(hDlg, IDC_PROGRESSBAR, PBM_SETRANGE, (WPARAM)0, MAKELPARAM(0, nTotalSteps));
		SendDlgItemMessage(hDlg, IDC_PROGRESSBAR, PBM_SETPOS, (WPARAM)nCompletedSteps, (LPARAM)0);

		SendDlgItemMessage(hDlg, IDC_INFOSGRAVURE, WM_SETTEXT , (WPARAM)0, (LPARAM)Temp);
		return S_OK;
	}

	STDMETHOD(NotifyBlockProgress)( long nCompleted, long nTotal )
	{
		char Temp[250];
		StringCchPrintf(Temp,250,"Block Progression : %d / %d",nCompleted,nTotal);


		SendDlgItemMessage(hDlg, IDC_PROGRESSBAR, PBM_SETRANGE, (WPARAM)0, MAKELPARAM(0, nTotal));
		SendDlgItemMessage(hDlg, IDC_PROGRESSBAR, PBM_SETPOS, (WPARAM)nCompleted, (LPARAM)0);

		SendDlgItemMessage(hDlg, IDC_INFOSGRAVURE, WM_SETTEXT , (WPARAM)0, (LPARAM)Temp);
		return S_OK;
	}

	STDMETHOD(NotifyTrackProgress)( long nCurrentTrack, long nTotalTracks )
	{
		SendDlgItemMessage(hDlg, IDC_PROGRESSBAR, PBM_SETRANGE, (WPARAM)0, MAKELPARAM(0, nTotalTracks));
		SendDlgItemMessage(hDlg, IDC_PROGRESSBAR, PBM_SETPOS, (WPARAM)nCurrentTrack, (LPARAM)0);

		return S_OK;
	}

	STDMETHOD(NotifyPreparingBurn)( long nEstimatedSeconds )
	{
		char Temp[250];
		StringCchPrintf(Temp,250,"Préparation : %d s",nEstimatedSeconds);
		SendDlgItemMessage(hDlg, IDC_INFOSGRAVURE, WM_SETTEXT , (WPARAM)0, (LPARAM)Temp);

		this->nEstimatedSeconds = nEstimatedSeconds;

		SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(ID_DEFINE_DUREE,0),(LPARAM)nEstimatedSeconds);

		SetTimer(hDlg,TIMER_DATETIME,1000,NULL);
		return S_OK;
	}

	STDMETHOD(NotifyClosingDisc)( long nEstimatedSeconds )
	{
		char Temp[250];
		StringCchPrintf(Temp,250,"Fermeture : %d s",nEstimatedSeconds);
		SendDlgItemMessage(hDlg, IDC_INFOSGRAVURE, WM_SETTEXT , (WPARAM)0, (LPARAM)Temp);


		SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(ID_DEFINE_DUREE,0),(LPARAM)nEstimatedSeconds);

		SetTimer(hDlg,TIMER_DATETIME,1000,NULL);

		this->nEstimatedSeconds = nEstimatedSeconds;
		return S_OK;
	}

	STDMETHOD(NotifyBurnComplete)( HRESULT status )
	{
		SendDlgItemMessage(hDlg, IDC_INFOSGRAVURE, WM_SETTEXT , (WPARAM)0, (LPARAM)"Gravure terminée avec succés.");
		return S_OK;
	}

	STDMETHOD(NotifyEraseComplete)( HRESULT status )
	{
		return E_NOTIMPL;
	}

public:
	long m_lRefCount;

private:
	HWND hDlg;
	long nEstimatedSeconds;
};

#endif