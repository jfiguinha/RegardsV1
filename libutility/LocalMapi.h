// Mapi.h: interface for the CMapi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPI_H__38851032_7DDB_499A_81CB_55519BF156C4__INCLUDED_)
#define AFX_MAPI_H__38851032_7DDB_499A_81CB_55519BF156C4__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include <windows.h>
#include <mapix.h>
#include <vector>
#include <string>

using namespace std;

typedef vector<string> VectorAttachment;

class CMapi  
{
public:
	CMapi();
	virtual ~CMapi();

	/*
	HRESULT Initialize();
	HRESULT AdminProfiles();
	HRESULT CreateNewProfile(LPSTR szProfile);
	HRESULT LogonDefaultEx();
	HRESULT LogonNewEx();
	HRESULT OpenAdminServices();
	HRESULT CreateMessagingService(LPTSTR lpszService, LPTSTR lpszDisplayName);
	//HRESULT ConfigureMessagingService(LPTSTR lpszService, LPTSTR lpszExchangeNameFL, LPTSTR lpszServer);
	HRESULT DeleteMessagingService(LPTSTR lpszService);
	HRESULT GetMessageStoresTable();
	HRESULT SetDefaultMessagingStore(char * sMailbox);
	void CloseMapi();*/
	HRESULT CMapi::SendEmail(const char * m_szBody,VectorAttachment * m_VectorAttachment);

private:
	/*
	LPPROFADMIN         pProfAdmin;
    LPSERVICEADMIN      pSvcAdmin;
    LPMAPITABLE         pMsgSvcTable;
	LPSRow              prow;
	LPSRowSet           prows;
	SPropValue			rgval[2];
	LPMAPISESSION       pSession;
	SRestriction		sres;    
    HRESULT             hr;
	SPropValue			pSvcProps;*/
	
};

#endif // !defined(AFX_MAPI_H__38851032_7DDB_499A_81CB_55519BF156C4__INCLUDED_)
