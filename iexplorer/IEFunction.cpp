#include "stdafx.h"
#include ".\iefunction.h"
#include "GenericHTTPClient.h"
#include "Mshtml.h"

CIEFunction::CIEFunction(void)
{
	m_WEBBROWSER.pSink = NULL;
	m_WEBBROWSER.pHost = NULL;

}

CIEFunction::~CIEFunction(void)
{
 	if(m_WEBBROWSER.pHost != NULL)
 		m_WEBBROWSER.pHost->Release();

 	if(m_WEBBROWSER.pSink != NULL)
 		m_WEBBROWSER.pSink->Release();

	m_WEBBROWSER.pSink = NULL;
	m_WEBBROWSER.pHost = NULL;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

long CIEFunction::Refresh()
{
	if (m_WEBBROWSER.pHost != NULL)
 	{
		m_WEBBROWSER.pHost->pWebBrowser->Refresh();
	}
	return 0;
}

long CIEFunction::DisplayHTMLStr(HWND hwnd, LPCTSTR strHtml, int iSize)
{
	LPDISPATCH		lpDispatch;
	IHTMLDocument2	*htmlDoc2;
	SAFEARRAY		*sfArray;
	VARIANT_BOOL pBool;
	
	VARIANT			*pVar;
	BSTR			bstr;
	wchar_t * cFilePath = new wchar_t[32768];
	ZeroMemory(cFilePath,32768 * sizeof(wchar_t));

	// Assume an error.
	bstr = 0;


	if (!m_WEBBROWSER.pHost->pWebBrowser->get_Document(&lpDispatch))
	{


		if (!lpDispatch->QueryInterface(IID_IHTMLDocument2, (void**)&htmlDoc2))
		{
			if ((sfArray = SafeArrayCreate(VT_VARIANT, 1, (SAFEARRAYBOUND *)&ArrayBound)))
			{
				if (!SafeArrayAccessData(sfArray, (void**)&pVar))
				{
					pVar->vt = VT_BSTR;
					size_t m_sizeTConvert;
					mbstowcs_s(&m_sizeTConvert,cFilePath,32768, strHtml, iSize);

					bstr = SysAllocString(cFilePath);

					// Store our BSTR pointer in the VARIENT.
					if ((pVar->bstrVal = bstr))
					{					
						htmlDoc2->close();
						htmlDoc2->write(sfArray);
					}

				}
				SafeArrayDestroy(sfArray);
			}

			htmlDoc2->Release();
		}
	}

	lpDispatch->Release();

	delete[] cFilePath;
	// No error?
	if (bstr) return(0);


	// An error
	return(-1);
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

long CIEFunction::AttachEvent()
{
	IDispatch * pDocDisp = NULL;
	HRESULT hr;
	IHTMLDocument2	*htmlDoc2;

	m_WEBBROWSER.pHost->pWebBrowser->get_Document(&pDocDisp);

	if (pDocDisp != NULL) 
	{
		// Obtained the document object by specifying the IHTMLDocument2 Interface.
		hr = pDocDisp->QueryInterface( IID_IHTMLDocument2, (void**)&htmlDoc2 );
		if ( SUCCEEDED(hr) )
		{
		  // Obtained the IHTMLDocument2 interface for the document object
		  ProcessDocument(htmlDoc2);
		}
		pDocDisp->Release();
	}   

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

void CIEFunction::ProcessDocument(IHTMLDocument2* pDoc)
{
	HRESULT hr;
	IHTMLElementCollection* pElemColl = NULL;

	  hr = pDoc->get_all( &pElemColl );
	  if ( SUCCEEDED(hr) )
	  {
		  // Obtained element collection.
		  ProcessElementCollection( pElemColl);
		  pElemColl->Release();
	  }
}



//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

long CIEFunction::DisplayHTMLRessource(const char * szResourceName, int iSize)
{
	VARIANT			myURL;
	wchar_t cFilePath[MAX_PATH*2];

	VariantInit(&myURL);
	myURL.vt = VT_BSTR;
	
	size_t m_sizeTConvert;
	mbstowcs_s(&m_sizeTConvert,cFilePath,520, szResourceName, iSize);

	//mbstowcs(cFilePath, szResourceName,  MAX_PATH*2);
	myURL.bstrVal = SysAllocString(cFilePath);

	m_WEBBROWSER.pHost->pWebBrowser->Navigate2(&myURL, 0, 0, 0, 0);

	VariantClear(&myURL);
	
	return(0);
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

bool CIEFunction::TestIfNewVersionAvailable(const char * szSiteAdress,const char * szVersion)
{
	GenericHTTPClient httpClient;

	if(httpClient.Request(szSiteAdress,strlen(szSiteAdress)+1))
	{
		LPCTSTR szHTML=httpClient.QueryHTTPResponse();
		if(strcmp(szVersion,szHTML) >= 0)
		{
			return false;
		}
	}
	else
	{
		LPVOID     lpMsgBuffer;
		DWORD dwRet=FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				httpClient.GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				reinterpret_cast<LPTSTR>(&lpMsgBuffer),
				0,
				NULL);                         

		//MessageBox(NULL,reinterpret_cast<LPTSTR>(lpMsgBuffer), "ERROR", MB_OK);
		LocalFree(lpMsgBuffer);
		return false;
	}
	return true;
}

