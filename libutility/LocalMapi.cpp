// Mapi.cpp: implementation of the CMapi class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LocalMapi.h"
#include <MAPI.H>
#include <MAPIUTIL.H>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapi::CMapi()
{

}

CMapi::~CMapi()
{

}
//////////////////////////////////////////////////////////////////////////////////////

HRESULT CMapi::SendEmail(const char * m_szBody,VectorAttachment * m_VectorAttachment)
{
     MapiMessage message ;
     MapiFileDesc * attachment = (MapiFileDesc* )malloc(sizeof(MapiFileDesc)*m_VectorAttachment->size()) ;
     MAPISENDMAIL *lpfnMAPISendMail ;
     HMODULE hModule ;
	 int i = 0;

	 VectorAttachment::iterator fit;

     hModule = LoadLibrary( "mapi32.dll" ) ;
     if( hModule==NULL ) {
          printf( "ERROR: Unable to load mapi32.dll\n" ) ;
          return false;
     }

     lpfnMAPISendMail = (MAPISENDMAIL *)GetProcAddress( hModule, "MAPISendMail" ) ;
     if( lpfnMAPISendMail==NULL ) {
          printf( "ERROR: Unable to locate MAPISendMail entry point\n" ) ;
          return false;
     }

     ZeroMemory(attachment, sizeof(MapiFileDesc) * m_VectorAttachment->size()) ;

	 for(fit = m_VectorAttachment->begin();fit != m_VectorAttachment->end();fit++,i++)
	 {
		 attachment[i].nPosition = i ;
		 attachment[i].lpszPathName = (char*)(*fit).c_str();
	 }

     
     memset( &message, 0, sizeof(message) ) ;
     message.lpszNoteText = (char *)m_szBody;
     message.nFileCount = m_VectorAttachment->size();
     message.lpFiles = attachment ;

     lpfnMAPISendMail( 0, 0, &message, MAPI_DIALOG, 0 ) ;

	 FreeLibrary(hModule);

	 free(attachment);

	 return true;
}

/*
HRESULT CMapi::Initialize()
{
	try
	{
		hr = MAPIInitialize(NULL);
		
	}
	catch (...)
	{
		return hr;
	}

	return hr;
}

HRESULT CMapi::AdminProfiles()
{
	try
	{
		hr = MAPIAdminProfiles(0, &pProfAdmin);
		
	}
	catch (...)
	{
		return hr;
	}

	return hr;
}

HRESULT CMapi::CreateNewProfile(LPSTR szProfile)
{
	try
	{
		hr = pProfAdmin->CreateProfile(szProfile, NULL, 0, 0);
		
	}
	catch (...)
	{
		return hr;
	}

	return hr;
}

HRESULT CMapi::LogonDefaultEx()
{
	try
	{
		hr = MAPILogonEx(0, NULL, NULL, MAPI_NO_MAIL | MAPI_USE_DEFAULT,
			&pSession);
		
	}
	catch (...)
	{
		return hr;
	}

	return hr;
}

HRESULT CMapi::LogonNewEx()
{
	try
	{
		hr = MAPILogonEx(0, NULL, NULL, MAPI_NO_MAIL | MAPI_NEW_SESSION,
                           &pSession);
		
	}
	catch (...)
	{
		return hr;
	}

	return hr;
}

HRESULT CMapi::OpenAdminServices()
{
	try
	{
		hr = pSession->AdminServices(0, &pSvcAdmin);
		
	}
	catch (...)
	{
		return hr;
	}

	return hr;
}

HRESULT CMapi::CreateMessagingService(LPTSTR lpszService, LPTSTR lpszDisplayName)
{
	try
	{
		hr = pSvcAdmin->CreateMsgService(lpszService, lpszDisplayName, 0, 0);
		
	}
	catch (...)
	{
		return hr;
	}

	return hr;
}


HRESULT CMapi::ConfigureMessagingService(LPTSTR lpszService, LPTSTR lpszExchangeNameFL, LPTSTR lpszServer)
{
	enum {iSvcName, iSvcUID, cptaSvc};
    SizedSPropTagArray (cptaSvc, ptaSvc) = { cptaSvc, 
                                            {   PR_SERVICE_NAME,
                                                PR_SERVICE_UID } };

	try
	{
		hr = pSvcAdmin->GetMsgServiceTable(0, &pMsgSvcTable);
		if (HR_FAILED(hr))
		{
			return hr;
		}

		sres.rt = RES_CONTENT;
		sres.res.resContent.ulFuzzyLevel = FL_FULLSTRING;
		sres.res.resContent.ulPropTag = PR_SERVICE_NAME;
		sres.res.resContent.lpProp = &pSvcProps;
	
		pSvcProps.ulPropTag = PR_SERVICE_NAME;
		pSvcProps.Value.lpszA = lpszService; //"MSEMS"

		hr = HrQueryAllRows(pMsgSvcTable, (LPSPropTagArray)&ptaSvc, &sres, NULL, 0, &prows);
		if (HR_FAILED(hr))
		{
			return hr;
		}
	
		// Configure Microsoft Exchange Service Here
		rgval[0].ulPropTag		= PR_PROFILE_UNRESOLVED_NAME;
		rgval[0].Value.lpszA	= lpszExchangeNameFL;	// Now we use the real name

		rgval[1].ulPropTag		= PR_PROFILE_UNRESOLVED_SERVER;
		rgval[1].Value.lpszA	= lpszServer;

		hr = pSvcAdmin->ConfigureMsgService((LPMAPIUID) prows->aRow->lpProps[iSvcUID].Value.bin.lpb,
			                            0, 0, 2, rgval);
		
	}
	catch (...)
	{
		return hr;
	}

	return hr;
}


HRESULT CMapi::DeleteMessagingService(LPTSTR lpszService)
{
	enum {iSvcName, iSvcUID, cptaSvc};
    SizedSPropTagArray (cptaSvc, ptaSvc) = { cptaSvc, 
                                            {   PR_SERVICE_NAME,
                                                PR_SERVICE_UID } };
	pSvcProps.ulPropTag = PR_SERVICE_NAME;
	pSvcProps.Value.lpszA = lpszService;
	
	try
	{
		hr = HrQueryAllRows(pMsgSvcTable, (LPSPropTagArray)&ptaSvc, &sres, NULL, 0, &prows);
    	hr = pSvcAdmin->DeleteMsgService((LPMAPIUID) prows->aRow->lpProps[iSvcUID].Value.bin.lpb);
	}
	catch (...) 
	{
		return hr;
	}

	return hr;
}

HRESULT CMapi::GetMessageStoresTable()
{
	try 
	{
		hr = pSession->GetMsgStoresTable(0, &pMsgSvcTable);
		
	}
	catch (...)
	{
		return hr;
	}

	return hr;
}

HRESULT CMapi::SetDefaultMessagingStore(char * sMailbox)
{
	enum {iSvcName, iSvcUID, cptaSvc};
    SizedSPropTagArray (cptaSvc, ptaSvc) = { cptaSvc, 
                                            {   PR_DISPLAY_NAME,
                                                PR_SERVICE_UID } };

	sres.res.resProperty.ulPropTag = PR_DISPLAY_NAME;

	
	
	pSvcProps.ulPropTag = PR_DISPLAY_NAME;
	pSvcProps.Value.lpszA = sMailbox;
	
	hr = pMsgSvcTable->FindRow(&sres, BOOKMARK_BEGINNING, 0);
	if (SUCCEEDED(hr))
	{
		LPSPropValue	lpProp = NULL;
		
		hr = pMsgSvcTable->QueryRows(1, 0, &prows);
		if(FAILED(hr))
		{
			return hr;
		}

		if (prows->cRows != 1)
		{
			return hr;
		}

		lpProp = &prows->aRow[0].lpProps[0];

		if(lpProp->ulPropTag != PR_ENTRYID)
		{
			return hr;
		}

		try 
		{
			hr = pSession->SetDefaultStore(MAPI_DEFAULT_STORE,//Default
				lpProp->Value.bin.cb, (LPENTRYID) lpProp->Value.bin.lpb);
		}
		catch (...)
		{
			return hr;
		}
	}
	
	return hr;
}

void CMapi::CloseMapi()
{
	UlRelease(pMsgSvcTable);
    UlRelease(pSvcAdmin);
    FreeProws(prows);
   	pSvcAdmin = NULL;
	pMsgSvcTable = NULL;
	prows = NULL;
	UlRelease(pSession);
    UlRelease(pProfAdmin);
    MAPIUninitialize();
	return;
}*/