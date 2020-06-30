#include "stdafx.h"
#include ".\webinfofichier.h"
#include "objet.h"
#include <exif.h>

CWebInfoFichier::CWebInfoFichier(void)
{
	m_cExif = NULL;
	m_iType = 0;
	hFocus = NULL;
	//LocalDisplayEXIFHTMLRessource();
}

CWebInfoFichier::~CWebInfoFichier(void)
{
	if(m_cExif != NULL)
		delete m_cExif;

	m_cExif = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

LRESULT CWebInfoFichier::OnCommand(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
 	int wmEvent = HIWORD(wParam); 
 	// Parse the menu selections:
 	switch (wmId)
 	{
 		case ID_RECHERCHE:
 			{
 				char cValue[1024];
 				SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(IDC_BTMODIF,0),(LPARAM)&cValue);
 			}
 			break;

 		case ID_DISPLAYCOMPLETED:
			if(m_iType == 1)
			{
				if(m_cExif != NULL)
				{
					ProcessExifDocument();
					AttachEvent();
				}
			}
			else
			{
				if(m_iType == 2)
				{
					DisplayHTMLStr(hWnd, (LPCTSTR)m_stgFichier.c_str(),m_stgFichier.length()+1);
				}
 				AttachEvent();
			}
 			break;

		case IDM_HTMLEXIF:
			{
				Cexif * m_cExifLocal = (Cexif *)lParam;
				if(m_cExifLocal != NULL)
				{
					if(m_cExif != NULL) 
						delete m_cExif;

					m_cExif = new Cexif(m_cExifLocal->m_exifinfo);
					if(m_cExif != NULL)
					{
						//if(m_iType != 1)
						LocalDisplayEXIFHTMLRessource();
						//else
						//	SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(ID_DISPLAYCOMPLETED,0),0);
						m_iType = 1;
					}
				}
				break;
			}

 		case ID_STOPIE:
 			m_WEBBROWSER.pHost->pWebBrowser->Stop();
 			break;

 		case ID_HTML:
 			{
				m_iType = 2;
				m_stgFichier = (LPCTSTR)lParam;
				LocalDisplayEXIFHTMLRessource();
 			}
 			break;
 	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

long CWebInfoFichier::LocalDisplayEXIFHTMLRessource()
{
	TCHAR    szTemp[MAX_PATH*2];
	StringCchCopy(szTemp,MAX_PATH*2,TEXT("res://"));
	GetModuleFileName(hInstance, szTemp + lstrlen(szTemp), ARRAYSIZE(szTemp) - lstrlen(szTemp));
#ifdef ENGLISH
	StringCbCat(szTemp,MAX_PATH*2,TEXT("/IDR_HTML_EXIF_EN"));
#endif
#ifdef SPANISH
	StringCbCat(szTemp,MAX_PATH*2,TEXT("/IDR_HTML_EXIF_ES"));
#endif
#ifdef FRENCH
	StringCbCat(szTemp,MAX_PATH*2,TEXT("/IDR_HTML_EXIF2"));
#endif
 	return DisplayHTMLRessource(szTemp,MAX_PATH*2);
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

void CWebInfoFichier::ProcessExifDocument()
{
	IDispatch * pDocDisp = NULL;
	HRESULT hr;
	IHTMLDocument2	*htmlDoc2;

	//BSTR FullName;
	//m_WEBBROWSER.pHost->pWebBrowser->get_FullName(&FullName);
	m_WEBBROWSER.pHost->pWebBrowser->get_Document(&pDocDisp);

	if (pDocDisp != NULL) 
	{
		// Obtained the document object by specifying the IHTMLDocument2 Interface.
		hr = pDocDisp->QueryInterface( IID_IHTMLDocument2, (void**)&htmlDoc2 );
		if ( SUCCEEDED(hr) )
		{
		  // Obtained the IHTMLDocument2 interface for the document object
		  ProcessExifDocument(htmlDoc2);
		}
		pDocDisp->Release();
	}   
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

void CWebInfoFichier::ProcessExifDocument(IHTMLDocument2* pDoc)
{
	HRESULT hr;
	IHTMLElementCollection* pElemColl = NULL;

	  hr = pDoc->get_all( &pElemColl );
	  if ( SUCCEEDED(hr) )
	  {
		  // Obtained element collection.
		  ProcessExifElementCollection( pElemColl);
		  pElemColl->Release();
	  }
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

HRESULT CWebInfoFichier::DefineCharInfo(IHTMLElement* pElem, char * cInfo)
{
	HRESULT hr;
	//OLECHAR  m_bstr[MAX_PATH];
	//CObjet::LocalToBSTR(m_bstr, cInfo, ARRAYSIZE(m_bstr));
	_bstr_t m_bstrValue(cInfo);
	hr = pElem->put_outerHTML(m_bstrValue.copy(true));
	return hr;
}

HRESULT CWebInfoFichier::DefineIntInfo(IHTMLElement* pElem, int iInfo)
{
	char cTemp[255];
	sprintf_s(cTemp,255,"%d",iInfo);
	HRESULT hr;
	_bstr_t m_bstrValue = cTemp;
	hr = pElem->put_outerHTML(m_bstrValue.copy());
	return hr;
}

HRESULT CWebInfoFichier::DefineFloatInfo(IHTMLElement* pElem, float fInfo)
{
	char cTemp[255];
	sprintf_s(cTemp,255,"%f",fInfo);
	HRESULT hr;
	_bstr_t m_bstrValue = cTemp;
	hr = pElem->put_outerHTML(m_bstrValue.copy());
	return hr;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

void CWebInfoFichier::ProcessExifElementCollection(IHTMLElementCollection* pElemColl)
{
	IDispatch* pElemDisp = NULL;
	IHTMLElement* pElem = NULL;
	CObjet m_cObjet;

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

					//File Informations
					if(strcmp(Temp,"idFilenameInfos") == 0)
					{
						char cTemp[MAX_PATH];
						m_cObjet.GetFileName(m_cExif->m_exifinfo->FileName,cTemp,MAX_PATH);
						DefineCharInfo(pElem,cTemp);
					} 
					else if(strcmp(Temp,"idDateCreInfos") == 0)
					{
						char cTemp[MAX_PATH];
						m_cObjet.GetInfosDateFile(m_cExif->m_exifinfo->FileName,cTemp,MAX_PATH);
						DefineCharInfo(pElem,cTemp);
					}
					else if(strcmp(Temp,"idDateModInfos") == 0)
					{
						char cTemp[MAX_PATH];
						m_cObjet.GetInfosDateFile(m_cExif->m_exifinfo->FileName,cTemp,MAX_PATH);
						DefineCharInfo(pElem,cTemp);
					}
					else if(strcmp(Temp,"idFormatInfos") == 0)
					{
						char cTemp[MAX_PATH];
						int iFormat = m_cObjet.TestImageFormat(m_cExif->m_exifinfo->FileName);
						if(iFormat == 0)
						{
							strcpy_s(cTemp,MAX_PATH,"Video");
						}	
						else
							m_cObjet.GetFormatName(iFormat,cTemp,MAX_PATH);
						DefineCharInfo(pElem,cTemp);
					}
					else if(strcmp(Temp,"idLargeurInfos") == 0)
					{
						hr = DefineIntInfo(pElem,m_cExif->m_exifinfo->Width);
					}
					else if(strcmp(Temp,"idHauteurInfos") == 0)
					{
						hr = DefineIntInfo(pElem,m_cExif->m_exifinfo->Height);
					}
					else if(strcmp(Temp,"idTailleInfos") == 0)
					{
						char cTemp[MAX_PATH];
						m_cObjet.GetFileSize(m_cExif->m_exifinfo->FileName,cTemp,MAX_PATH);
						DefineCharInfo(pElem,cTemp);
					}

					//Exif Informations
					else if(strcmp(Temp,"idFilename") == 0)
						hr = DefineCharInfo(pElem,m_cExif->m_exifinfo->FileName);
					else if(strcmp(Temp,"idVersion") == 0)
						hr = DefineCharInfo(pElem,m_cExif->m_exifinfo->Version);
					else if(strcmp(Temp,"idCameraMake") == 0)
						hr = DefineCharInfo(pElem,m_cExif->m_exifinfo->CameraMake);
					else if(strcmp(Temp,"idCameraModel") == 0)
						hr = DefineCharInfo(pElem,m_cExif->m_exifinfo->CameraModel);
					else if(strcmp(Temp,"idDateTime") == 0)
						hr = DefineCharInfo(pElem,m_cExif->m_exifinfo->DateTime);
					else if(strcmp(Temp,"idHeight") == 0)
						hr = DefineIntInfo(pElem,m_cExif->m_exifinfo->Height);
					else if(strcmp(Temp,"idWidth") == 0)
						hr = DefineIntInfo(pElem,m_cExif->m_exifinfo->Width);
					else if(strcmp(Temp,"idOrientation") == 0)
						hr = DefineIntInfo(pElem,m_cExif->m_exifinfo->Orientation);
					else if(strcmp(Temp,"idIsColor") == 0)
						hr = DefineIntInfo(pElem,m_cExif->m_exifinfo->IsColor);
					else if(strcmp(Temp,"idProcess") == 0)
						hr = DefineIntInfo(pElem,m_cExif->m_exifinfo->Process);
					else if(strcmp(Temp,"idFlashUsed") == 0)
						hr = DefineIntInfo(pElem,m_cExif->m_exifinfo->FlashUsed);
					else if(strcmp(Temp,"idFocalLength") == 0)
						hr = DefineCharInfo(pElem,m_cExif->m_exifinfo->CameraMake);
					else if(strcmp(Temp,"idExposureTime") == 0)
						hr = DefineFloatInfo(pElem,m_cExif->m_exifinfo->FocalLength);
					else if(strcmp(Temp,"idApertureFNumber") == 0)
						hr = DefineFloatInfo(pElem,m_cExif->m_exifinfo->ApertureFNumber);
					else if(strcmp(Temp,"idDistance") == 0)
						hr = DefineFloatInfo(pElem,m_cExif->m_exifinfo->Distance);
					else if(strcmp(Temp,"idCCDWidth") == 0)
						hr = DefineFloatInfo(pElem,m_cExif->m_exifinfo->CCDWidth);
					else if(strcmp(Temp,"idExposureBias") == 0)
						hr = DefineFloatInfo(pElem,m_cExif->m_exifinfo->ExposureBias);
					else if(strcmp(Temp,"idWhitebalance") == 0)
						hr = DefineIntInfo(pElem,m_cExif->m_exifinfo->Whitebalance);
					else if(strcmp(Temp,"idExposureProgram") == 0)
						hr = DefineFloatInfo(pElem,m_cExif->m_exifinfo->ExposureBias);
					else if(strcmp(Temp,"idISOequivalent") == 0)
						hr = DefineIntInfo(pElem,m_cExif->m_exifinfo->ISOequivalent);
					else if(strcmp(Temp,"idCompressionLevel") == 0)
						hr = DefineIntInfo(pElem,m_cExif->m_exifinfo->CompressionLevel);
					else if(strcmp(Temp,"idFocalplaneXRes") == 0)
						hr = DefineFloatInfo(pElem,m_cExif->m_exifinfo->FocalplaneXRes);
					else if(strcmp(Temp,"idFocalplaneYRes") == 0)
						hr = DefineFloatInfo(pElem,m_cExif->m_exifinfo->FocalplaneYRes);
					else if(strcmp(Temp,"idFocalplaneUnits") == 0)
						hr = DefineFloatInfo(pElem,m_cExif->m_exifinfo->FocalplaneUnits);
					else if(strcmp(Temp,"idXresolution") == 0)
						hr = DefineFloatInfo(pElem,m_cExif->m_exifinfo->Xresolution);
					else if(strcmp(Temp,"idYresolution") == 0)
						hr = DefineFloatInfo(pElem,m_cExif->m_exifinfo->Yresolution);
					else if(strcmp(Temp,"idResolutionUnit") == 0)
						hr = DefineFloatInfo(pElem,m_cExif->m_exifinfo->ResolutionUnit);
					else if(strcmp(Temp,"idBrightness") == 0)
						hr = DefineFloatInfo(pElem,m_cExif->m_exifinfo->Brightness);
					else if(strcmp(Temp,"idComments") == 0)
						hr = DefineCharInfo(pElem,m_cExif->m_exifinfo->Comments);

					//Mot clés
					else if(strcmp(Temp,"idkeywordvalue") == 0)
					{
						hr = DefineCharInfo(pElem,(char *)m_cExif->m_exifinfo->m_stgHTMLDisplay.c_str());
					}
				 }		 
				 
				 ::SysFreeString(bstrId);
				 
				  pElem->Release(); 
				 
				 
			 }
		     pDisp->Release();
		}

    }
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

void CWebInfoFichier::ProcessElementCollection(IHTMLElementCollection* pElemColl)
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
        
		if ( hr == S_OK )
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

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

LRESULT CWebInfoFichier::OnActivate(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int iMessage = LOWORD(wParam);

	if(iMessage == WA_CLICKACTIVE)
		hFocus = GetFocus();
	
	if(iMessage == WA_INACTIVE)
		SetFocus(hFocus);

	return 0;
}

long CWebInfoFichier::LocalDisplayHTMLRessource()
{
	TCHAR    szTemp[MAX_PATH*2];
	StringCchCopy(szTemp,MAX_PATH*2,TEXT("res://"));
	GetModuleFileName(hInstance, szTemp + lstrlen(szTemp), ARRAYSIZE(szTemp) - lstrlen(szTemp));
	StringCbCat(szTemp,MAX_PATH*2,TEXT("/IDR_HTML_INFOS"));
 	return DisplayHTMLRessource(szTemp,MAX_PATH*2);
}

LRESULT CWebInfoFichier::OnCreate(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	
	m_WEBBROWSER.pHost = new Host(hWnd);
	if (m_WEBBROWSER.pHost== NULL)
	{
		CObjet::ErreurInformation();
	}
	LocalDisplayEXIFHTMLRessource();
	return 0;
}

LRESULT CWebInfoFichier::OnSize(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

LRESULT CWebInfoFichier::OnDestroy(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
