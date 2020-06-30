// WebBrowser.cpp: implementation of the CWebBrowser class.
//
//////////////////////////////////////////////////////////////////////

//#define NOTIMPLEMENTED _ASSERT(0); return(E_NOTIMPL)

#include "stdafx.h"
#include "WebBrowser.h"
#include "catalogue1.h"
#include "objet.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWebBrowser::CWebBrowser()
{
}

CWebBrowser::~CWebBrowser()
{


}

LRESULT CWebBrowser::OnCommand(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
 	int wmEvent = HIWORD(wParam); 
 	// Parse the menu selections:
 	switch (wmId)
 	{
 		case ID_AIDEINFOS:
 			{
 				SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(ID_AIDEINFOS,0),(LPARAM)0);
 			}
 			break;

 		case ID_RECHERCHE:
 			{
 				char cValue[1024];
 				GetRecherche(cValue,1024);
 				SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(ID_RECHERCHE,0),(LPARAM)&cValue);
 			}
 			break;

 		case ID_REMOVEALLITEM:
 			{
 				RemoveAllItem();
 			}
 			break;

 			
 		case ID_ADDITEM:
 			{
 				CAttribut * m_LocalAttribut = (CAttribut * )lParam;
 				AddItem(m_LocalAttribut->cLibelle,m_LocalAttribut->iNumAttribut);
 			}
 			break;

 		case ID_DISPLAYCOMPLETED:
 			AttachEvent();
			RemoveAllItem();
			SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(ID_ADDITEM,0),0);
 			break;

 		case ID_STOPIE:
 			m_WEBBROWSER.pHost->pWebBrowser->Stop();
 			break;

 		case ID_NEWURLIE:
 			{
				char * m_pValue = (char *)lParam;
 				BSTR bstr;
 				wchar_t cFilePath[MAX_PATH];
 				//mbstowcs(cFilePath, (const char *)lParam,  512);
				size_t m_sizeTConvert;
				mbstowcs_s(&m_sizeTConvert,cFilePath,MAX_PATH,m_pValue, strlen(m_pValue)+1);


 				bstr = SysAllocString(cFilePath);
 				m_WEBBROWSER.pHost->pWebBrowser->Navigate(bstr, NULL, NULL, NULL, NULL);

 			}
 			break;

 		case ID_HTML:
 			{
				LocalDisplayHTMLRessource();
 			}
 			break;
 	}

	return 0;
}

long CWebBrowser::LocalDisplayHTMLRessource()
{
	TCHAR    szTemp[MAX_PATH*2];
	StringCchCopy(szTemp,MAX_PATH*2,TEXT("res://"));
	GetModuleFileName(hInstance, szTemp + lstrlen(szTemp), ARRAYSIZE(szTemp) - lstrlen(szTemp));

#ifdef ENGLISH
	StringCbCat(szTemp,MAX_PATH*2,TEXT("/IDR_HTML_SEARCH"));
#endif
#ifdef SPANISH
	StringCbCat(szTemp,MAX_PATH*2,TEXT("/IDR_HTML_BUSCAR"));
#endif
#ifdef FRENCH
	StringCbCat(szTemp,MAX_PATH*2,TEXT("/IDR_HTML_RECHERCHE"));
#endif

 	return DisplayHTMLRessource(szTemp,MAX_PATH*2);
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

void CWebBrowser::ProcessElementCollection(IHTMLElementCollection* pElemColl)
{
	IDispatch* pElemDisp = NULL;
	IHTMLElement* pElem = NULL;

	////////////////////////////////////////////////////////
    //   enumerate DHTML element 
	////////////////////////////////////////////////////////
    
	LONG celem;

	pElemColl->get_length(&celem);

    
	for(int i = 0; i < celem; i++)
	{
		VARIANT varIndex;
        varIndex.vt = VT_UINT;
        varIndex.lVal = i;
        
		VARIANT var2;
        VariantInit( &var2 );
        
		IDispatch* pDisp; 

        //////////////////////////
		HRESULT hr = pElemColl->item( varIndex, var2, &pDisp );
        
		if ( hr == S_OK && pDisp != NULL )
        {
             IHTMLElement* pElem;

             hr = pDisp->QueryInterface(IID_IHTMLElement,(void **)&pElem);
             
			 if ( hr == S_OK )
             {
                 
				 char Temp[255];

				 BSTR bstrId = 0;
				 _bstr_t bstrValue;
                 
				 hr = pElem->get_id(&bstrId);

				 bstrValue = bstrId;

				 if(bstrValue.length() > 0)
				 {
					StringCchCopy(Temp,255,(char *)bstrValue);

					if(strcmp(Temp,"idRecherche") == 0)
					{
						IHTMLButtonElement* pButtonElem = NULL;
						hr = pDisp->QueryInterface(IID_IHTMLButtonElement, (void **)&pButtonElem); 
						pButtonElem->put_name(L"idRecherche");
						m_WEBBROWSER.pHost->ConnectHTMLButtonEvents(pButtonElem);
					} 
					
					if(strcmp(Temp,"idManuel") == 0)
					{
						IHTMLAnchorElement * pLinkElem = NULL;
						hr = pDisp->QueryInterface(IID_IHTMLAnchorElement, (void **)&pLinkElem); 
						m_WEBBROWSER.pHost->ConnectHTMLALinkEvents(pLinkElem);
					}  
				 }
				 
				 
				 ::SysFreeString(bstrId);
				 
				  pElem->Release(); 
				 
				 
			 }
		     pDisp->Release();
		}

    }
}

LRESULT CWebBrowser::OnCreate(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	m_WEBBROWSER.pHost = new Host(hWnd);
	if (m_WEBBROWSER.pHost == NULL)
	{
		CObjet::ErreurInformation();
	}
	return 0;
}

LRESULT CWebBrowser::OnSize(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
 	RECT rc;
 	GetClientRect(hWnd,&rc);

 	if (m_WEBBROWSER.pHost != NULL)
 	{
 		m_WEBBROWSER.pHost->pInPlaceObject->SetObjectRects(&rc, &rc);
 		GetClientRect(hWnd,&rc);
 		m_WEBBROWSER.pHost->pObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, m_WEBBROWSER.pHost->m_pClientSite, 0, hWnd, &rc);
 	}

	return 0;
}


/*
LRESULT CWebBrowser::OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
 	RECT rc;
 	GetClientRect(hWnd,&rc);

 	if (m_WEBBROWSER.pHost != NULL)
 	{
		m_WEBBROWSER.pHost->pWebBrowser->Refresh();
	}

	return 0;
}*/

LRESULT CWebBrowser::OnDestroy(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
 	if(m_WEBBROWSER.pHost != NULL)
 		m_WEBBROWSER.pHost->Release();

 	if(m_WEBBROWSER.pSink != NULL)
 		m_WEBBROWSER.pSink->Release();

	m_WEBBROWSER.pSink = NULL;
	m_WEBBROWSER.pHost = NULL;

	PostQuitMessage(0);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

long CWebBrowser::AddItem(const char * Value, const int &iNumAttribut)
{
	VARIANT_BOOL bBusy;
	wchar_t cFilePath[MAX_PATH];
	wchar_t cNumPath[20];	
	char cTemp[20];

	_itoa_s(iNumAttribut, cTemp,20, 10);

	m_WEBBROWSER.pHost->pWebBrowser->get_Busy(&bBusy);

	if(bBusy)
		return false;

	//mbstowcs(cFilePath, Value,  255);
	//mbstowcs(cNumPath, cTemp,  20);

	size_t m_sizeTConvert;
	mbstowcs_s(&m_sizeTConvert,cFilePath,MAX_PATH, Value, strlen(Value)+1);
	mbstowcs_s(&m_sizeTConvert,cNumPath,20,cTemp, 20);

	IHTMLDocument2	*htmlDoc2;
	LPDISPATCH lpDispatch;
	if (m_WEBBROWSER.pHost->pWebBrowser->get_Document(&lpDispatch) == S_OK)
	{
		if (lpDispatch->QueryInterface(IID_IHTMLDocument2, (void**)&htmlDoc2) == S_OK)
		{
			HRESULT hr;

			LPDISPATCH pDisp;
			hr = htmlDoc2->get_Script(&pDisp);
			htmlDoc2->Release();

			if(pDisp == NULL)
				return false;

			BSTR bstrMember = SysAllocString(L"AddCategorie");
			DISPID dispid = NULL;
			hr = pDisp->GetIDsOfNames(IID_NULL,&bstrMember,1, LOCALE_SYSTEM_DEFAULT,&dispid);

			if(FAILED(hr))
				return false;

			const int arraySize = 2;

			DISPPARAMS dispparams;
			memset(&dispparams, 0, sizeof dispparams);
			dispparams.cArgs = arraySize;
			dispparams.rgvarg = new VARIANT[dispparams.cArgs];
	
			dispparams.rgvarg[0].bstrVal = SysAllocString(cFilePath);
			dispparams.rgvarg[0].vt = VT_BSTR;

			dispparams.rgvarg[1].bstrVal = SysAllocString(cNumPath);
			dispparams.rgvarg[1].vt = VT_BSTR;


			dispparams.cNamedArgs = 0;

			EXCEPINFO excepInfo;
			memset(&excepInfo, 0, sizeof excepInfo);
   			VARIANT vaResult;
			VariantInit(&vaResult);
			UINT nArgErr = (UINT)-1;  // initialize to invalid arg
         
			hr = pDisp->Invoke(dispid,IID_NULL,0,
									DISPATCH_METHOD,&dispparams,&vaResult,&excepInfo,&nArgErr);

			delete [] dispparams.rgvarg;
			if(FAILED(hr))
				return false;
			
			pDisp->Release();
		}
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

int CWebBrowser::RemoveAllItem()
{
	VARIANT_BOOL bBusy;

	m_WEBBROWSER.pHost->pWebBrowser->get_Busy(&bBusy);

	if(bBusy)
		return false;

	IHTMLDocument2	*htmlDoc2;
	LPDISPATCH lpDispatch;
	if (m_WEBBROWSER.pHost->pWebBrowser->get_Document(&lpDispatch) == S_OK)
	{
		if (lpDispatch->QueryInterface(IID_IHTMLDocument2, (void**)&htmlDoc2) == S_OK)
		{
			HRESULT hr;

			LPDISPATCH pDisp;
			hr = htmlDoc2->get_Script(&pDisp);
			htmlDoc2->Release();

			if(pDisp == NULL)
				return false;

			BSTR bstrMember = SysAllocString(L"RemoveAll");
			DISPID dispid = NULL;
			hr = pDisp->GetIDsOfNames(IID_NULL,&bstrMember,1, LOCALE_SYSTEM_DEFAULT,&dispid);

			if(FAILED(hr))
				return false;

			const int arraySize = 0;

			DISPPARAMS dispparams;
			memset(&dispparams, 0, sizeof dispparams);
			dispparams.cArgs = arraySize;
			dispparams.rgvarg = new VARIANT[dispparams.cArgs];

			dispparams.cNamedArgs = 0;

			EXCEPINFO excepInfo;
			memset(&excepInfo, 0, sizeof excepInfo);
   			VARIANT vaResult;
			VariantInit(&vaResult);
			UINT nArgErr = (UINT)-1;  // initialize to invalid arg
         
			hr = pDisp->Invoke(dispid,IID_NULL,0,
									DISPATCH_METHOD,&dispparams,&vaResult,&excepInfo,&nArgErr);

			delete [] dispparams.rgvarg;
			if(FAILED(hr))
				return false;
			
			pDisp->Release();
		}
	}

	return 0;	
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

int CWebBrowser::GetRecherche(char * Value, int iSize)
{
	VARIANT_BOOL bBusy;
	_bstr_t bstRecherche;

	m_WEBBROWSER.pHost->pWebBrowser->get_Busy(&bBusy);

	if(bBusy)
		return false;

	IHTMLDocument2	*htmlDoc2;
	LPDISPATCH lpDispatch;
	if (m_WEBBROWSER.pHost->pWebBrowser->get_Document(&lpDispatch) == S_OK)
	{
		if (lpDispatch->QueryInterface(IID_IHTMLDocument2, (void**)&htmlDoc2) == S_OK)
		{
			HRESULT hr;

			LPDISPATCH pDisp;
			hr = htmlDoc2->get_Script(&pDisp);
			htmlDoc2->Release();

			if(pDisp == NULL)
				return false;

			BSTR bstrMember = SysAllocString(L"GetRecherche");
			DISPID dispid = NULL;
			hr = pDisp->GetIDsOfNames(IID_NULL,&bstrMember,1, LOCALE_SYSTEM_DEFAULT,&dispid);

			if(FAILED(hr))
				return false;

			const int arraySize = 0;

			DISPPARAMS dispparams;
			memset(&dispparams, 0, sizeof dispparams);
			dispparams.cArgs = arraySize;
			dispparams.rgvarg = new VARIANT[dispparams.cArgs];
			dispparams.cNamedArgs = 0;

			EXCEPINFO excepInfo;
			memset(&excepInfo, 0, sizeof excepInfo);
   			VARIANT vaResult;
			VariantInit(&vaResult);
			UINT nArgErr = (UINT)-1;  // initialize to invalid arg
         
			hr = pDisp->Invoke(dispid,IID_NULL,0,
									DISPATCH_METHOD,&dispparams,&vaResult,&excepInfo,&nArgErr);

			bstRecherche = vaResult.bstrVal;

			StringCchCopy(Value,iSize,(char *)bstRecherche);

			delete [] dispparams.rgvarg;

			if(FAILED(hr))
				return false;
			
			pDisp->Release();
		}
	}

	return 0;	
}
