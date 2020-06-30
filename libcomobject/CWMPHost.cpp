// CWMPHost.cpp : Implementation of the CWMPHost
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include "stdafx.h"
#include "CWMPHost.h"
#include "CWMPEventDispatch.h"
#include <share.h>
//#include <crtdbg.h>


bool FAILMSG(HRESULT hr)
{
    if (FAILED(hr))
    {
        TCHAR   szError[1024];

        StringCchPrintf(szError,1024, _T("Error code = %08X"), hr);
        ::MessageBox(NULL, szError, _T("Error"), MB_OK | MB_ICONERROR);
    }

    return FAILED(hr);
}


/////////////////////////////////////////////////////////////////////////////
// CWMPHost

void CWMPHost::OnFinalMessage(HWND /*hWnd*/)
{
    ::PostQuitMessage(0);
}

LRESULT CWMPHost::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    AtlAxWinInit();
    CComPtr<IAxWinHostWindow>           spHost;
    CComPtr<IConnectionPointContainer>  spConnectionContainer;
    CComWMPEventDispatch                *pEventListener = NULL;
    CComPtr<IWMPEvents>                 spEventListener;
    HRESULT                             hr;
    RECT                                rcClient;

    m_dwAdviseCookie = 0;

    // create window

	
    GetClientRect(&rcClient);
    m_wndView.Create(m_hWnd, rcClient, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
    if (NULL == m_wndView.m_hWnd)
        goto FAILURE;
    


    // load OCX in window

    hr = m_wndView.QueryHost(&spHost);
    if (FAILMSG(hr))
        goto FAILURE;

    hr = spHost->CreateControl(CComBSTR(_T("{6BF52A52-394A-11d3-B153-00C04F79FAA6}")), m_wndView, 0);
    if (FAILMSG(hr))
        goto FAILURE;

    hr = m_wndView.QueryControl(&m_spWMPPlayer);
    if (FAILMSG(hr))
        goto FAILURE;

    // start listening to events

    hr = CComWMPEventDispatch::CreateInstance(&pEventListener);
    spEventListener = pEventListener;
    if (FAILMSG(hr))
        goto FAILURE;

	pEventListener->SetHost(this);

    hr = m_spWMPPlayer->QueryInterface(&spConnectionContainer);
    if (FAILMSG(hr))
        goto FAILURE;

    // See if OCX supports the IWMPEvents interface
    hr = spConnectionContainer->FindConnectionPoint(__uuidof(IWMPEvents), &m_spConnectionPoint);
    if (FAILED(hr))
    {
        // If not, try the _WMPOCXEvents interface, which will use IDispatch
        hr = spConnectionContainer->FindConnectionPoint(__uuidof(_WMPOCXEvents), &m_spConnectionPoint);
        if (FAILMSG(hr))
            goto FAILURE;
    }

    hr = m_spConnectionPoint->Advise(spEventListener, &m_dwAdviseCookie);
    if (FAILMSG(hr))
        goto FAILURE;

    return 0;

FAILURE:
    ::PostQuitMessage(0);
    return 0;
}

LRESULT CWMPHost::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    // stop listening to events

    if (m_spConnectionPoint)
    {
        if (0 != m_dwAdviseCookie)
            m_spConnectionPoint->Unadvise(m_dwAdviseCookie);
        m_spConnectionPoint.Release();
    }

    // close the OCX

    if (m_spWMPPlayer)
    {
        m_spWMPPlayer->close();
        m_spWMPPlayer.Release();
    }

    bHandled = FALSE;

	PostQuitMessage(0);
    return 0;
}

LRESULT CWMPHost::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    // stop listening to events

	//MessageBox("Key Down 2","Infos",0);

    bHandled = FALSE;
    return 0;
}

LRESULT CWMPHost::OnErase(UINT /* uMsg */, WPARAM /* wParam */, LPARAM /* lParam */, BOOL& bHandled)
{
    return 1;
}

LRESULT CWMPHost::GetCurrentPosition(double &dValue)
{
    HRESULT     hr;
    void        *pUnknown;
    //double dValue;
    CComPtr<IWMPControls> spWMPControls;
    hr = m_spWMPPlayer->get_controls(&spWMPControls);
    if (spWMPControls)
    {
        spWMPControls->QueryInterface(__uuidof(IWMPControls), &pUnknown);

		hr = spWMPControls->get_currentPosition(&dValue);
		if (FAILMSG(hr))
		{
			dValue = -1.0;
			return -1;
		}

    }
    return 0;
}

LRESULT CWMPHost::SetCurrentPosition(const double &dValue)
{
    HRESULT     hr;
    void        *pUnknown;
    //double dValue;
    CComPtr<IWMPControls> spWMPControls;
    hr = m_spWMPPlayer->get_controls(&spWMPControls);
    if (spWMPControls)
    {
        spWMPControls->QueryInterface(__uuidof(IWMPControls), &pUnknown);

		hr = spWMPControls->put_currentPosition(dValue);
		if (FAILMSG(hr))
		{
			return -1;
		}

    }
    return 0;
}

LRESULT CWMPHost::OnSize(UINT /* uMsg */, WPARAM /* wParam */, LPARAM /* lParam */, BOOL& /* lResult */)
{
    RECT rcClient;
    GetClientRect(&rcClient);
    m_wndView.MoveWindow(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
    return 0;
}


LRESULT CWMPHost::OnPlayFile(const char * filename, int iSize)
{
	VARIANT	myURL;
	VariantInit(&myURL);
	myURL.vt = VT_BSTR;
	
	size_t m_sizeTConvert;
	mbstowcs_s(&m_sizeTConvert,m_wFilename,MAX_PATH, filename, iSize);

	myURL.bstrVal = SysAllocString(m_wFilename);

	HRESULT  hr;

	hr = m_spWMPPlayer->put_URL(myURL.bstrVal);

	VariantClear(&myURL);
   			
	m_cFilename = filename;

	return 0;
}

LRESULT CWMPHost::OnClose()
{
    HRESULT     hr;

	if(m_spWMPPlayer != NULL)
	{
		hr = m_spWMPPlayer->close();
		if (FAILMSG(hr))
			return 0;
	}
    return 0;
}

LRESULT CWMPHost::OnWMPPlayer2StretchToFit()
{
    HRESULT      hr;
    VARIANT_BOOL fValue;
    CComPtr<IWMPPlayer2> spWMPPlayer2;

    hr = m_spWMPPlayer.QueryInterface(&spWMPPlayer2);
    if (FAILMSG(hr))
        return 0;

    hr = spWMPPlayer2->get_stretchToFit(&fValue);
    if (FAILMSG(hr))
        return 0;

	if(!fValue)
	{
		hr = spWMPPlayer2->put_stretchToFit(VARIANT_TRUE);
		if (FAILMSG(hr))
			return 0;
	}
	else
	{
		hr = spWMPPlayer2->put_stretchToFit(0);
		if (FAILMSG(hr))
			return 0;
	}
    return 0;
}

LRESULT CWMPHost::OnGetFullScreenState()
{
    HRESULT      hr;
    VARIANT_BOOL fValue;

    hr = m_spWMPPlayer->get_fullScreen(&fValue);
    if (FAILMSG(hr))
        return 0;

	return fValue;
}


LRESULT CWMPHost::OnWMPPlayerFullScreen()
{
    HRESULT      hr;
    VARIANT_BOOL fValue;

    hr = m_spWMPPlayer->get_fullScreen(&fValue);
    if (FAILMSG(hr))
        return 0;

	fValue = !fValue;
    hr = m_spWMPPlayer->put_fullScreen(VARIANT_TRUE);
    if (FAILMSG(hr))
        return 0;

    return 0;
}

LRESULT CALLBACK CWMPHost::CreateBookmarkFile()
{

	//Obtention du nom du fichier + ext .bkm

	double m_dValue;
	WCHAR FileName[1024];
	WCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
	_wsplitpath_s(m_wFilename, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);	

	StringCchCopy(FileName,MAX_PATH,drive);
	StringCbCat(FileName,MAX_PATH,dir);
	StringCbCat(FileName,MAX_PATH,fname);
	StringCbCat(FileName,MAX_PATH,L".bkm");

	int fichier_cible;

	if(GetCurrentPosition(m_dValue) == 0)
	{
		//fichier_cible = _open(FileName,_O_RDONLY); 
		if(_wsopen_s(&fichier_cible,FileName,_O_RDONLY, _SH_DENYNO, _S_IREAD | _S_IWRITE) == -1)
			return -1;

		if(fichier_cible != -1)
		{
			_close(fichier_cible);
			DeleteFile(FileName);
		}

		//fichier_cible = _open(FileName,_O_WRONLY | _O_CREAT | _O_BINARY);
		if(_wsopen_s(&fichier_cible,FileName,_O_WRONLY | _O_CREAT | _O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE) == -1)
			return -1;
		if(fichier_cible == -1)
			return -1;
		_write(fichier_cible,&m_dValue,sizeof(double));
		_close(fichier_cible);
	}

	return 0;
}

LRESULT CALLBACK CWMPHost::ReadBookmarkFile()
{

	WCHAR FileName[MAX_PATH];
	WCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];
	_wsplitpath_s(m_wFilename, drive,_MAX_DRIVE, dir,_MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);	
	
	//_splitpath(m_cFilename, drive, dir, fname, ext);

	StringCchCopy(FileName,MAX_PATH,drive);
	StringCbCat(FileName,MAX_PATH,dir);
	StringCbCat(FileName,MAX_PATH,fname);
	StringCbCat(FileName,MAX_PATH,L".bkm");

	double m_dValue;
	int fichier_cible;

	//fichier_cible = _open(FileName, _O_RDONLY);
	if(_wsopen_s(&fichier_cible,FileName,_O_RDONLY, _SH_DENYNO, _S_IREAD | _S_IWRITE) == -1)
		return -1;


	if(fichier_cible != -1)
	{

		_read(fichier_cible,&m_dValue,sizeof(double));
		_close(fichier_cible);
		SetCurrentPosition(m_dValue);
	}

	return 0;
}


LRESULT CWMPHost::FowardMsgToWMP(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    HRESULT hr;
    LRESULT llResult = 0;
    CComPtr<IOleInPlaceObjectWindowless> spSite = NULL;

    hr = m_spWMPPlayer->QueryInterface(&spSite);
    
	if( SUCCEEDED(hr) )
    {
        spSite->OnWindowMessage(uMsg, wParam, lParam, &llResult);
    }
    bHandled = TRUE;

    return llResult;
}
