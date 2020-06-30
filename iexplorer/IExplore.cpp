/*****************************************************************************\
IExplore.cpp
Copyright (c) 2002 Matthew Blagden <info@nocturnalnetwork.com>
You may use this code in your applications at no cost, so long as credit
is given to Matthew Blagden, nocturnalnetwork.com, or The Nocturnal Network.
\*****************************************************************************/
#include "stdafx.h"
#include "IExplore.h"
#include "global.h"

#define Alert(x) { return E_NOTIMPL;} //MessageBox(NULL, x, x, MB_OK); return E_NOTIMPL; }



//inline void * __cdecl operator new(size_t size) { return (PBYTE)LocalAlloc(LPTR, size); }
//inline void __cdecl operator delete(void *ptr)  { if (ptr) LocalFree(ptr); }

ClientSite::ClientSite(Host* pFrameSite) { m_pHost = pFrameSite; }
ClientSite::~ClientSite() {}

// IUnknown
STDMETHODIMP ClientSite::QueryInterface(REFIID iid, void ** ppvObject)
    { return m_pHost->QueryInterface(iid, ppvObject); }

ULONG STDMETHODCALLTYPE ClientSite::AddRef()
    { return m_pHost->AddRef(); }

ULONG STDMETHODCALLTYPE ClientSite::Release()
    { return m_pHost->Release(); }

// IOleClientSite
STDMETHODIMP ClientSite::SaveObject()
    { Alert("ClientSite::SaveObject") }

STDMETHODIMP ClientSite::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker ** ppmk)
    { Alert("ClientSite::GetMoniker") }

STDMETHODIMP ClientSite::OnShowWindow(BOOL fShow)
    { Alert("ClientSite::OnShowWindow") }

STDMETHODIMP ClientSite::RequestNewObjectLayout()
    { Alert("ClientSite::RequestNewObjectLayout") }

STDMETHODIMP ClientSite::GetContainer(LPOLECONTAINER FAR* ppContainer)
    { return E_NOTIMPL; }

STDMETHODIMP ClientSite::ShowObject()
    { return S_OK; }

InPlaceSite::InPlaceSite(Host* pFrameSite) { m_pHost = pFrameSite; }
InPlaceSite::~InPlaceSite() {}

// IUnknown
STDMETHODIMP InPlaceSite::QueryInterface(REFIID iid, void ** ppvObject)
    { return m_pHost->QueryInterface(iid, ppvObject); }

ULONG STDMETHODCALLTYPE InPlaceSite::AddRef()
    { return m_pHost->AddRef(); }

ULONG STDMETHODCALLTYPE InPlaceSite::Release()
    { return m_pHost->Release(); }

// IOleWindow
HRESULT STDMETHODCALLTYPE InPlaceSite::GetWindow(HWND * phwnd)
    { *phwnd = m_pHost->m_hwndParent; return S_OK; }

HRESULT STDMETHODCALLTYPE InPlaceSite::ContextSensitiveHelp(BOOL fEnterMode)
    { Alert("InPlaceSite::ContextSensitiveHelp") }

// IOleInPlaceSite
HRESULT STDMETHODCALLTYPE InPlaceSite::CanInPlaceActivate(void)
    { return S_OK; }

HRESULT STDMETHODCALLTYPE InPlaceSite::OnInPlaceActivate(void)
    { return S_OK; }

HRESULT STDMETHODCALLTYPE InPlaceSite::OnUIActivate(void)
    { return S_OK; }

HRESULT STDMETHODCALLTYPE InPlaceSite::GetWindowContext(IOleInPlaceFrame **ppFrame,
                                                        IOleInPlaceUIWindow **ppDoc,
                                                        LPRECT lprcPosRect, LPRECT lprcClipRect,
                                                        LPOLEINPLACEFRAMEINFO lpFrameInfo)
    { return S_OK; }

HRESULT STDMETHODCALLTYPE InPlaceSite::Scroll(SIZE scrollExtant)
    { Alert("InPlaceSite::Scroll") }

HRESULT STDMETHODCALLTYPE InPlaceSite::OnUIDeactivate(BOOL fUndoable)
    { Alert("InPlaceSite::OnUIDeactivate") }

HRESULT STDMETHODCALLTYPE InPlaceSite::OnInPlaceDeactivate(void)
    { Alert("InPlaceSite::OnInPlaceDeactivate") }

HRESULT STDMETHODCALLTYPE InPlaceSite::DiscardUndoState(void)
    { Alert("InPlaceSite::DiscardUndoState") }

HRESULT STDMETHODCALLTYPE InPlaceSite::DeactivateAndUndo(void)
    { Alert("InPlaceSite::DeactivateAndUndo") }

HRESULT STDMETHODCALLTYPE InPlaceSite::OnPosRectChange(LPCRECT lprcPosRect)
{ 
	IOleInPlaceObject * ieIP;
	HRESULT hr = S_OK;
		
	//ATLTRACE("new width and height for this WebOC instance are %d and %d\n", lprcPosRect->right, lprcPosRect->bottom);
	if (m_pHost->pInPlaceObject) 
	{
		ieIP = m_pHost->pInPlaceObject;
		if (ieIP) 
		{
            hr = ieIP->SetObjectRects(lprcPosRect, lprcPosRect);
            if (SUCCEEDED(hr))
			{
                // Change the height of the window to match the webOC's height.
                // ResizeClient() calls the Win32 function
                // Set WindowPos()
			   HWND hWnd;
			   SIZE m_Size;
			   m_Size.cx = lprcPosRect->right;
			   m_Size.cy = lprcPosRect->bottom;

			   ieIP->GetWindow(&hWnd);
			   m_pHost->pObject->SetExtent(DVASPECT_CONTENT,&m_Size); 
			   //pOleObject->SetExtent(DVASPECT_CONTENT,&size); 

			   //BOOL bRet = MoveWindow(hWnd,0,0,lprcPosRect->right, lprcPosRect->bottom, TRUE);
               //BOOL bRet = ResizeClient(lprcPosRect->right, lprcPosRect->bottom, TRUE);
               //if (!bRet) 
			   //{
               //        hr = E_FAIL;
               //}
            }
		} 
		else 
		{
			hr = E_FAIL;
		}
	}
	delete ieIP;
	return hr;

}

/////////////////////////////////////////////////////////////////////////////////////////////
//BrowserEvents2
/////////////////////////////////////////////////////////////////////////////////////////////

BrowserEvents::BrowserEvents() 
{ 
}

BrowserEvents::~BrowserEvents() 
{
}

// IUnknown
STDMETHODIMP BrowserEvents::QueryInterface(REFIID iid, void ** ppvObject)
    { return m_pHost->QueryInterface(iid, ppvObject); }

ULONG STDMETHODCALLTYPE BrowserEvents::AddRef()
    { return m_pHost->AddRef(); }

ULONG STDMETHODCALLTYPE BrowserEvents::Release()
{ 
	return m_pHost->Release(); 
}

// IDispatch
HRESULT STDMETHODCALLTYPE BrowserEvents::GetTypeInfoCount(unsigned int FAR* pctinfo)
    { Alert("WebBrowserEvents2::GetTypeInfoCount") }

HRESULT STDMETHODCALLTYPE BrowserEvents::GetTypeInfo(unsigned int iTInfo, LCID  lcid,
                                                         ITypeInfo FAR* FAR*  ppTInfo)
    { Alert("WebBrowserEvents2::GetTypeInfo") }

HRESULT STDMETHODCALLTYPE BrowserEvents::GetIDsOfNames(REFIID riid,
                                                           OLECHAR FAR* FAR* rgszNames,
                                                           unsigned int cNames,
                                                           LCID lcid,
                                                           DISPID FAR* rgDispId)
    { Alert("WebBrowserEvents2::GetIDsOfNames") }



HRESULT STDMETHODCALLTYPE BrowserEvents::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid,
                                                    WORD wFlags, DISPPARAMS FAR* pDispParams,
                                                    VARIANT FAR* parResult,
                                                    EXCEPINFO FAR* pExcepInfo,
                                                    unsigned int FAR* puArgErr) 
{
    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//WebBrowserEvents2
/////////////////////////////////////////////////////////////////////////////////////////////

WebBrowserEvents2::WebBrowserEvents2(Host* pFrameSite) 
{ 
	m_pHost = pFrameSite; 
}

WebBrowserEvents2::~WebBrowserEvents2() 
{

}

HRESULT STDMETHODCALLTYPE WebBrowserEvents2::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid,
                                                    WORD wFlags, DISPPARAMS FAR* pDispParams,
                                                    VARIANT FAR* parResult,
                                                    EXCEPINFO FAR* pExcepInfo,
                                                    unsigned int FAR* puArgErr) 
{
    VARIANT * vURL;
    _bstr_t bstrURL;
	//char value[2048];
    
	switch(dispIdMember)
	{
		case DISPID_DOCUMENTCOMPLETE:
			{

				vURL = pDispParams->rgvarg[0].pvarVal;
				if (vURL->vt & VT_BYREF)
					bstrURL = *vURL->pbstrVal;
				else
					bstrURL = vURL->bstrVal;

				SendMessage(m_pHost->m_hwndParent,WM_COMMAND,MAKEWPARAM(ID_DISPLAYCOMPLETED,0),(LPARAM)(char *)bstrURL);
			}
			break;

		default:
			break;

	}

    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//HTMLBrowserEvents2
/////////////////////////////////////////////////////////////////////////////////////////////

HTMLBrowserEvents2::HTMLBrowserEvents2(Host* pFrameSite) 
{ 
	m_pHost = pFrameSite; 
}
HTMLBrowserEvents2::~HTMLBrowserEvents2() 
{
}


HRESULT STDMETHODCALLTYPE HTMLBrowserEvents2::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid,
                                                    WORD wFlags, DISPPARAMS FAR* pDispParams,
                                                    VARIANT FAR* parResult,
                                                    EXCEPINFO FAR* pExcepInfo,
                                                    unsigned int FAR* puArgErr) 
{
    switch ( dispIdMember )
    {
		case DISPID_HTMLELEMENTEVENTS_ONCLICK:
			SendMessage(m_pHost->m_hwndParent,WM_COMMAND,MAKEWPARAM(ID_RECHERCHE,0),0);
			break;

		default:
			break;
    }
    return S_OK;

}

/////////////////////////////////////////////////////////////////////////////////////////////
//HTMLLinkBrowserEvents2
/////////////////////////////////////////////////////////////////////////////////////////////

HTMLALinkBrowserEvents2::HTMLALinkBrowserEvents2(Host* pFrameSite)
{
	m_pHost = pFrameSite;
}

HTMLALinkBrowserEvents2::~HTMLALinkBrowserEvents2()
{
}

HRESULT STDMETHODCALLTYPE HTMLALinkBrowserEvents2::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
                                 DISPPARAMS FAR* pDispParams, VARIANT FAR* parResult,
                                 EXCEPINFO FAR* pExcepInfo, unsigned int FAR* puArgErr)
{
    _bstr_t bstrURL;
    switch ( dispIdMember )
    {
		case DISPID_HTMLELEMENTEVENTS2_ONCLICK:
			SendMessage(m_pHost->m_hwndParent,WM_COMMAND,MAKEWPARAM(ID_AIDEINFOS,0),0);
			break;

		default:
			break;
    }
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//HOST
/////////////////////////////////////////////////////////////////////////////////////////////

Host::Host(HWND hwndParent)
{
    #define Assert(x) if(!x) MessageBox(NULL, "IE ERROR!", "Error", MB_ICONSTOP)

    HRESULT                     hret;
    IUnknown                   *pUnknown;            
    
    IConnectionPoint           *pConnectionPoint;
    IConnectionPointContainer  *pConnectionPointContainer;   
    
    DWORD                       dwAdviseCookie;

    // Initialize COM library
    //OleInitialize(NULL);

    // Setup member variables
    m_dwRefCount = 1;
    m_hwndParent = hwndParent;

    // Instantiate all needed interfaces
    m_pClientSite = new ClientSite(this);
    m_pInPlaceSite = new InPlaceSite(this);
    m_pWebBrowserEvents2 = new WebBrowserEvents2(this);
	m_pHTMLBrowserEvents2 = new HTMLBrowserEvents2(this);
	m_pHTMLALinkEvents2 = new HTMLALinkBrowserEvents2(this);

    // Instantiate Web Browser control
    hret = CoCreateInstance(CLSID_WebBrowser, NULL, CLSCTX_ALL, IID_IUnknown, (void**)(&pUnknown));
    Assert(SUCCEEDED(hret));

    // Set the client site
    hret = pUnknown->QueryInterface(IID_IOleObject, (void**)(&pObject));
    Assert(SUCCEEDED(hret));
    hret = pObject->SetClientSite(m_pClientSite);
    Assert(SUCCEEDED(hret));

    // Set the site limits
    RECT rWindowPos;
    GetClientRect(hwndParent, &rWindowPos);
    
    rWindowPos.right -= rWindowPos.left;
    rWindowPos.bottom -= rWindowPos.top;
    rWindowPos.left = 0;
    rWindowPos.top = 0;
    
    hret = pUnknown->QueryInterface(IID_IOleInPlaceObject, (void**)(&pInPlaceObject));
    Assert(SUCCEEDED(hret));
    hret = pInPlaceObject->SetObjectRects(&rWindowPos, &rWindowPos);
    Assert(SUCCEEDED(hret));
    //pInPlaceObject->Release();

    // Activate the site

    hret = pObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, m_pClientSite, 0, hwndParent, &rWindowPos);
    Assert(SUCCEEDED(hret));
    //pObject->Release();

    // Set event handler
    hret = pUnknown->QueryInterface(IID_IConnectionPointContainer, (void**)(&pConnectionPointContainer));
    Assert(SUCCEEDED(hret));
    hret = pConnectionPointContainer->FindConnectionPoint(DIID_DWebBrowserEvents2, &pConnectionPoint);
    Assert(SUCCEEDED(hret));
    hret = pConnectionPoint->Advise(m_pWebBrowserEvents2, &dwAdviseCookie);
    Assert(SUCCEEDED(hret));
    pConnectionPointContainer->Release();
    pConnectionPoint->Release();

	// Set event handler for HTML event
	
    // Set event handler

    // Get Web Browser interface
    hret = pUnknown->QueryInterface(IID_IWebBrowser2, (void**)&pWebBrowser);
    Assert(SUCCEEDED(hret));
    if(pUnknown) pUnknown->Release();

    // Go to URL
    //hret = pWebBrowser->Navigate(L"http://www.google.com", NULL, NULL, NULL, NULL);
    //Assert(SUCCEEDED(hret));
    //if(pWebBrowser) pWebBrowser->Release();
}

Host::~Host()
{
    // Clean up hosted classes
    if(m_pClientSite) delete m_pClientSite;
    if(m_pInPlaceSite) delete m_pInPlaceSite;
    if(m_pWebBrowserEvents2) delete m_pWebBrowserEvents2;
	if(m_pHTMLBrowserEvents2) delete m_pHTMLBrowserEvents2;
	if(m_pHTMLALinkEvents2) delete m_pHTMLALinkEvents2;

	if(pInPlaceObject)
		pInPlaceObject->Release();
	
	if(pWebBrowser)
		pWebBrowser->Release();

	if(pObject)
		pObject->Release();

    // Release COM library
    //OleUninitialize();
}

ULONG STDMETHODCALLTYPE Host::AddRef()
    { return ++m_dwRefCount; }

ULONG STDMETHODCALLTYPE Host::Release()
{
    if(!--m_dwRefCount)
        delete this;
    return m_dwRefCount;
}

STDMETHODIMP Host::QueryInterface(REFIID riid, void ** ppvObject)
{
    if(ppvObject == NULL) return E_INVALIDARG;
    *ppvObject = NULL;

    if(riid == IID_IUnknown)
        *ppvObject = this;
    else if(riid == IID_IOleClientSite)
        *ppvObject = m_pClientSite;
    else if(riid == IID_IOleInPlaceSite)
        *ppvObject = m_pInPlaceSite;
    else if(riid == DIID_DWebBrowserEvents2)
        *ppvObject = m_pWebBrowserEvents2;
	else if(riid == DIID_HTMLButtonElementEvents2)
		*ppvObject = m_pHTMLBrowserEvents2;
	else if(riid == DIID_HTMLAnchorEvents2)
		*ppvObject = m_pHTMLALinkEvents2;

    if(*ppvObject == NULL) return E_NOINTERFACE;
    AddRef();
    return S_OK;
}

void Host::ConnectHTMLButtonEvents(IHTMLButtonElement *pElem)
{
	HRESULT hr;
	IConnectionPointContainer* pCPC = NULL;
	IConnectionPoint* pCP = NULL;
	DWORD dwCookie;

	if(pElem != NULL)
	{

		// Check that this is a connectable object.
		hr = pElem->QueryInterface( IID_IConnectionPointContainer, (void**)&pCPC );
		if ( SUCCEEDED(hr) )        
		{
  			// Find the connection point.
  			hr = pCPC->FindConnectionPoint(DIID_HTMLButtonElementEvents2, &pCP );
  			if ( SUCCEEDED(hr) )           
  			{
  				// Advise the connection point.
  				// pUnk is the IUnknown interface pointer for your event sink
  				hr = pCP->Advise( m_pHTMLBrowserEvents2, &dwCookie );
  				pCP->Release();
  			}
  			pCPC->Release();  
		}

	}

}

void Host::ConnectHTMLALinkEvents(IHTMLAnchorElement *pElem)
{
	HRESULT hr;
	IConnectionPointContainer* pCPC = NULL;
	IConnectionPoint* pCP = NULL;
	DWORD dwCookie;

	if(pElem != NULL)
	{

		// Check that this is a connectable object.
		hr = pElem->QueryInterface( IID_IConnectionPointContainer, (void**)&pCPC );
		if ( SUCCEEDED(hr) )        
		{
  			// Find the connection point.
  			hr = pCPC->FindConnectionPoint(DIID_HTMLAnchorEvents2, &pCP );
  			if ( SUCCEEDED(hr) )           
  			{
  				// Advise the connection point.
  				// pUnk is the IUnknown interface pointer for your event sink
  				hr = pCP->Advise(m_pHTMLALinkEvents2, &dwCookie );
  				pCP->Release();
  			}
  			pCPC->Release();  
		}

	}

}
