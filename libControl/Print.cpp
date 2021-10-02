// Print.cpp: implementation of the CPrint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Print.h"
#include "objet.h"
#include <IBitmap.h>
#include <MargeData.h>
#include <Effet.h>
#include <PrintPreviewData.h>
#include ".\printpreviewdialog.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void CPrint::DisplayPreview(HINSTANCE hInst, HWND hWnd, CMargeData * m_MargeData)
{
	//Copie de sauvegarde
	CIBitmap m_ciBitmap;
	m_ciBitmap.SetBitmap(m_MargeData->m_ciBitmap->GetPtBitmap(),m_MargeData->m_ciBitmap->GetBitmapWidth(),m_MargeData->m_ciBitmap->GetBitmapHeigth());

	CPrintPreviewDialog * m_cPrintPreview = new CPrintPreviewDialog();
	m_cPrintPreview->SetParam(m_MargeData);
	m_cPrintPreview->CreateDlg(hInst, (LPCTSTR)IDD_DLGPROPIMP, NULL);
	int iRetour = m_cPrintPreview->GetReturnValue();

	//int iRetour = DialogBoxParam(hInst, (LPCTSTR)IDD_DLGPROPIMP, NULL, (DLGPROC)CPrint::PrintPreview,(LPARAM)m_MargeData);
	if(iRetour == 0)
	{
		m_MargeData->m_ciBitmap->SetBitmap(m_ciBitmap.GetPtBitmap(),m_ciBitmap.GetBitmapWidth(),m_ciBitmap.GetBitmapHeigth());
	}

	SendMessage(hWnd,WM_ERASEBKGND,MAKEWPARAM(m_MargeData->m_ciBitmap->GetBitmapWidth(), m_MargeData->m_ciBitmap->GetBitmapHeigth()),MAKELPARAM(IDM_ZOOM_OK,0));

	delete m_cPrintPreview;
}

//Lancement de la procédure d'impression

HRESULT CPrint::DisplayPrintPropertySheet(HINSTANCE hInst,HWND &hWnd, CMargeData * m_MargeData, const int &iTypePrint)  // Window that owns the property sheet
{
	HRESULT hResult;
	LPPRINTDLG pPDX = NULL;
	LPPRINTPAGERANGE pPageRanges = NULL;
	HWND hDlgPrint;
	HANDLE pPrinter;

	// Allocate the PRINTDLGEX structure.

	pPDX = (LPPRINTDLG)HeapAlloc(GetProcessHeap (),HEAP_ZERO_MEMORY,sizeof(PRINTDLG));
	if (!pPDX)
		return E_OUTOFMEMORY;

	// Allocate an array of PRINTPAGERANGE structures.

	pPageRanges = (LPPRINTPAGERANGE) HeapAlloc(GetProcessHeap (),HEAP_ZERO_MEMORY,
					   10 * sizeof(PRINTPAGERANGE));
	if (!pPageRanges)
	{
		HeapFree(GetProcessHeap(), 0, pPDX); 
		return E_OUTOFMEMORY;
	}

	//  Initialize the PRINTDLGEX structure.

	bool m_bReturn = true;

	pPDX->lStructSize = sizeof(PRINTDLG);
	pPDX->hwndOwner = hWnd;
	pPDX->hDevMode = NULL;
	pPDX->hDevNames = NULL;
	pPDX->hDC = NULL;
	pPDX->Flags = PD_RETURNDC | PD_COLLATE;
	pPDX->nMinPage = 1;
	pPDX->nMaxPage = 1000;
	pPDX->nCopies = 1;
	pPDX->hInstance = 0;
	pPDX->lpPrintTemplateName = NULL;
	//  Invoke the Print property sheet.

	DWORD size;
	PDEVMODE pDevMode = NULL;

	int iTypeAllocation = 1;


	GetDefaultPrinter(NULL, &size);
	TCHAR* buffer = new TCHAR[size];
	if (buffer == NULL)
	{
		CObjet::ErreurInformation();
	}
	switch(iTypePrint)
	{
		case 1:
				{
					DWORD m_dValue = 0;
					DWORD nSize;

					// Get the printer name.

					//buffer = new TCHAR[size]; 
					// Get the printer name.
					if(!GetDefaultPrinter(buffer, &size))
					{
						m_bReturn = false;
						goto MEMORYFREE;
					}


					// Get a printer handle.
					if(OpenPrinter(buffer, &pPrinter, NULL) == FALSE)
					{
						m_bReturn = false;
						goto MEMORYFREE;
					}

					//Retrieve the size of the DEVMODE.

					nSize = DocumentProperties(NULL, pPrinter, buffer, 0, 0, 0);

					if(nSize < m_dValue)
					{
						m_bReturn = false;
						goto MEMORYFREE;
					}

					//Fill the DEVMODE from the printer.

					pDevMode = (PDEVMODE)malloc(nSize);
					if(pDevMode == NULL)
					{
						//Probléme d'allocation mémoire
						//Essai d'allocation dans le tas
						iTypeAllocation = 2;
						pDevMode = (PDEVMODE) HeapAlloc (GetProcessHeap (), 0, nSize);
						if (pDevMode == NULL)
						{
							CObjet::ErreurInformation();
						}

					}
					//Récupération des informations sur la configuration de la page par défaut

					nSize = DocumentProperties(NULL, pPrinter, buffer, pDevMode, 0, DM_OUT_BUFFER);

					if(nSize < m_dValue)
					{
						m_bReturn = false;
						goto MEMORYFREE;
					}

					pPDX->hDevMode = pDevMode;

					pPDX->nToPage = -1;


					// Get a device context for the printer.
					pPDX->hDC = CreateDC(NULL, buffer, NULL, NULL);


					if(pPDX->hDC == NULL)
					{
						m_bReturn = false;
						goto MEMORYFREE;
					}

					
				  
				}
				break;

		default:

				hResult = PrintDlg(pPDX);

				if(pPDX->Flags != PD_PAGENUMS)
					pPDX->nToPage = 0;
				break;
	}

	//hResult = PrintDlg(pPDX);

	if(hResult)
	{
		
		// User clicked the Print button, so
		// use the DC and other information returned in the 
		// PRINTDLGEX structure to print the document

		char cTemp[MAX_PATH];
		m_MargeData->m_ciBitmap->GetFilename(cTemp);


		DOCINFO di = { sizeof (DOCINFO), cTemp} ;

		CPrint::bUserAbort = FALSE ;
		hDlgPrint = CreateDialog (hInst, MAKEINTRESOURCE(IDD_PRINTDLGBOX), hWnd, CPrint::PrintDlgProc) ;

		ShowWindow(hDlgPrint, SW_SHOW);

		SetAbortProc (pPDX->hDC, CPrint::AbortProc) ;


		for(int i = 0;i < pPDX->nCopies;i++)
		{
			if(CPrint::bUserAbort)
				break;


			if (StartDoc (pPDX->hDC, &di) > 0)
			 {
				  if (StartPage (pPDX->hDC) > 0)
				  {
					
					CPrint::ProcessPrint(pPDX,m_MargeData);
					
					if (EndPage (pPDX->hDC) > 0)
						EndDoc (pPDX->hDC) ;
				  }
			 }
		}
	}

MEMORYFREE:

    DestroyWindow (hDlgPrint) ;

	if (pPDX->hDC != NULL) 
		DeleteDC(pPDX->hDC);

	if(pPDX != NULL)
		HeapFree(GetProcessHeap(), 0, pPDX); 

	if(pDevMode != NULL)
	{
		switch(iTypeAllocation)
		{
			case 2:
				HeapFree(GetProcessHeap (), 0,pDevMode);
				break;

			default:
				free(pDevMode);
		}
		
	}

	return m_bReturn;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPrint::CPrint()
{

}

CPrint::~CPrint()
{

}

BOOL CPrint::bUserAbort ;
HWND CPrint::hDlgPrint ;


/////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////

BOOL CPrintPrv::DetermineRatio(CPrintPreviewData * prtPrevData)
{
	int m_iLargeurMax, m_iHauteurMax;
	m_iLargeurMax = prtPrevData->iPosWidth - prtPrevData->iPosRightPicture - prtPrevData->iPosLeftPicture;
	m_iHauteurMax = prtPrevData->iPosHeight - prtPrevData->iPosTopPicture - prtPrevData->iPosBottomPicture;
	

	if(prtPrevData->m_MargeData->iPos == 1)
	{
		prtPrevData->m_dRatio = (double)(m_iLargeurMax) / (double)prtPrevData->m_MargeData->m_ciBitmap->GetBitmapWidth();
	}
	else
	{
		prtPrevData->m_dRatio = (double)(m_iHauteurMax) / (double)prtPrevData->m_MargeData->m_ciBitmap->GetBitmapHeigth();
	}
	
	
	if(m_iHauteurMax < (prtPrevData->m_MargeData->m_ciBitmap->GetBitmapHeigth() * prtPrevData->m_dRatio))
	{
		prtPrevData->m_dRatio = (double)m_iHauteurMax / (double)prtPrevData->m_MargeData->m_ciBitmap->GetBitmapHeigth();
	}
	else
	{
		if(m_iLargeurMax < (prtPrevData->m_MargeData->m_ciBitmap->GetBitmapWidth() * prtPrevData->m_dRatio))
		{
			prtPrevData->m_dRatio = (double)m_iLargeurMax / (double)prtPrevData->m_MargeData->m_ciBitmap->GetBitmapWidth();			
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

BOOL CPrintPrv::PrintPreviewPaint(HWND hDlg,CPrintPreviewData * prtPrevData)
{
	int m_BitmapWidth = prtPrevData->m_MargeData->m_ciBitmap->GetBitmapWidth();
	int m_BitmapHeight = prtPrevData->m_MargeData->m_ciBitmap->GetBitmapHeigth();

	prtPrevData->m_ciBitmap->SetBitmap(prtPrevData->m_ciBitmapTemp->hBitmap);

	int m_iTailleIconeWidth = prtPrevData->m_MargeData->m_ciBitmap->GetBitmapWidth() * prtPrevData->m_dRatio;
	int m_iTailleIconeHeight = prtPrevData->m_MargeData->m_ciBitmap->GetBitmapHeigth() * prtPrevData->m_dRatio;


	if(prtPrevData->m_ciBitmapOriginalResize == NULL)
		prtPrevData->m_ciBitmapOriginalResize = new CIBitmap();
		

	if(m_iTailleIconeWidth != prtPrevData->m_ciBitmapOriginalResize->GetBitmapWidth() || m_iTailleIconeHeight != prtPrevData->m_ciBitmapOriginalResize->GetBitmapHeigth())
	{
		CEffet m_cEffet;
		prtPrevData->m_ciBitmapOriginalResize->SetBitmap(prtPrevData->m_MargeData->m_ciBitmap->GetPtBitmap(),m_BitmapWidth,m_BitmapHeight);
		m_cEffet.ScaleBitmap(prtPrevData->m_ciBitmapOriginalResize,m_iTailleIconeWidth,m_iTailleIconeHeight,TRIANGLEFILTER);

	}


	HDC destDC;
	destDC = CreateCompatibleDC( NULL );
	HBITMAP hbmOldDest = (HBITMAP)::SelectObject( destDC, prtPrevData->m_ciBitmap->hBitmap);

	int xValue = 0;
	int yValue = 0;

	if(prtPrevData->iOrientation == 1)
	{

		prtPrevData->m_ciBitmapOriginalResize->ShowBitmap(destDC,
			 prtPrevData->iPosLeft + prtPrevData->iPosLeftPicture,
			 prtPrevData->iPosTop + prtPrevData->iPosTopPicture,
			 xValue,
			 yValue,
			 0,0,
			 m_iTailleIconeWidth, m_iTailleIconeHeight,
			 1);

	}
	else
	{

		prtPrevData->m_ciBitmapOriginalResize->ShowBitmap(destDC,
			 prtPrevData->iPosLeft + prtPrevData->iPosLeftPicture,
			 prtPrevData->iPosTop + prtPrevData->iPosTopPicture + 1,
			 xValue,
			 yValue,
			 0,0,
			 m_iTailleIconeWidth, m_iTailleIconeHeight,
			 1);

	}

	DeleteObject(hbmOldDest);
	DeleteDC(destDC);

	return 0;
}

// Size of internal buffer used to hold "printername,drivername,portname"
// string. Increasing this may be necessary for huge strings.
#define MAXBUFFERSIZE 250

/*----------------------------------------------------------------*/ 
/* DPGetDefaultPrinter                                            */ 
/*                                                                */ 
/* Parameters:                                                    */ 
/*   pPrinterName: Buffer alloc'd by caller to hold printer name. */ 
/*   pdwBufferSize: On input, ptr to size of pPrinterName.        */ 
/*          On output, min required size of pPrinterName.         */ 
/*                                                                */ 
/* NOTE: You must include enough space for the NULL terminator!   */ 
/*                                                                */ 
/* Returns: TRUE for success, FALSE for failure.                  */ 
/*----------------------------------------------------------------*/ 

BOOL CPrintPrv::DPGetDefaultPrinter(LPTSTR pPrinterName, LPDWORD pdwBufferSize)
{
  BOOL bFlag;
  OSVERSIONINFO osv;
  TCHAR cBuffer[MAXBUFFERSIZE];
  PRINTER_INFO_2 *ppi2 = NULL;
  DWORD dwNeeded = 0;
  DWORD dwReturned = 0;
  
  // What version of Windows are you running?
  osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&osv);
  
  // If Windows 95 or 98, use EnumPrinters...
  if (osv.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
  {
    // The first EnumPrinters() tells you how big our buffer should
    // be in order to hold ALL of PRINTER_INFO_2. Note that this will
    // usually return FALSE. This only means that the buffer (the 4th
    // parameter) was not filled in. You don't want it filled in here...
    EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 2, NULL, 0, &dwNeeded, &dwReturned);
    if (dwNeeded == 0) 
      return FALSE;
    
    // Allocate enough space for PRINTER_INFO_2...
    ppi2 = (PRINTER_INFO_2 *)GlobalAlloc(GPTR, dwNeeded);
    if (!ppi2)
      return FALSE;
    
    // The second EnumPrinters() will fill in all the current information...
    bFlag = EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 2, (LPBYTE)ppi2, dwNeeded, &dwNeeded, &dwReturned);
    if (!bFlag)
    {
      GlobalFree(ppi2);
      return FALSE;
    }
    
    // If given buffer too small, set required size and fail...
    if ((DWORD)lstrlen(ppi2->pPrinterName) >= *pdwBufferSize)
    {
      *pdwBufferSize = (DWORD)lstrlen(ppi2->pPrinterName) + 1;
      GlobalFree(ppi2);
      return FALSE;
    }
    
    // Copy printer name into passed-in buffer...

	StringCchCopy(pPrinterName,*pdwBufferSize,ppi2->pPrinterName);
    
    // Set buffer size parameter to min required buffer size...
    *pdwBufferSize = (DWORD)lstrlen(ppi2->pPrinterName) + 1;
  }
  
  // If Windows NT, use the GetDefaultPrinter API for Windows 2000,
  // or GetProfileString for version 4.0 and earlier...
  else if (osv.dwPlatformId == VER_PLATFORM_WIN32_NT)
  {
#if(WINVER >= 0x0500)
    if (osv.dwMajorVersion >= 5) // Windows 2000 or later
    {
      bFlag = GetDefaultPrinter(pPrinterName, pdwBufferSize);
      if (!bFlag)
        return FALSE;
    }
    
    else // NT4.0 or earlier
#endif
    {
      // Retrieve the default string from Win.ini (the registry).
      // String will be in form "printername,drivername,portname".
      if (GetProfileString("windows", "device", ",,,", cBuffer, MAXBUFFERSIZE) <= 0)
        return FALSE;

     char * next_token1;
      // Printer name precedes first "," character...
      strtok_s(cBuffer, ",", &next_token1);
      
      // If given buffer too small, set required size and fail...
      if ((DWORD)lstrlen(cBuffer) >= *pdwBufferSize)
      {
        *pdwBufferSize = (DWORD)lstrlen(cBuffer) + 1;
        return FALSE;
      }
      
      // Copy printer name into passed-in buffer...
	  StringCchCopy(pPrinterName,*pdwBufferSize,cBuffer);
      
      // Set buffer size parameter to min required buffer size...
      *pdwBufferSize = (DWORD)lstrlen(cBuffer) + 1;
    }
  }
  
  // Cleanup...
  if (ppi2)
    GlobalFree(ppi2);
  
  return TRUE;
}

#undef MAXBUFFERSIZE

/*-----------------------------------------------------------------*/ 
/* DPSetDefaultPrinter                                             */ 
/*                                                                 */ 
/* Parameters:                                                     */ 
/*   pPrinterName: Valid name of existing printer to make default. */ 
/*                                                                 */ 
/* Returns: TRUE for success, FALSE for failure.                   */ 
/*-----------------------------------------------------------------*/ 
BOOL CPrintPrv::DPSetDefaultPrinter(LPTSTR pPrinterName)
{
  BOOL bFlag;
  OSVERSIONINFO osv;
  DWORD dwNeeded = 0;
  HANDLE hPrinter = NULL;
  PRINTER_INFO_2 *ppi2 = NULL;
  LPTSTR pBuffer = NULL;
  LONG lResult;
  
  // What version of Windows are you running?
  osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&osv);
  
  if (!pPrinterName)
    return FALSE;
  
  // If Windows 95 or 98, use SetPrinter...
  if (osv.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
  {
    // Open this printer so you can get information about it...
    bFlag = OpenPrinter(pPrinterName, &hPrinter, NULL);
    if (!bFlag || !hPrinter)
      return FALSE;
    
    // The first GetPrinter() tells you how big our buffer should
    // be in order to hold ALL of PRINTER_INFO_2. Note that this will
    // usually return FALSE. This only means that the buffer (the 3rd
    // parameter) was not filled in. You don't want it filled in here...
    GetPrinter(hPrinter, 2, 0, 0, &dwNeeded);
    if (dwNeeded == 0)
    {
      ClosePrinter(hPrinter);
      return FALSE;
    }
    
    // Allocate enough space for PRINTER_INFO_2...
    ppi2 = (PRINTER_INFO_2 *)GlobalAlloc(GPTR, dwNeeded);
    if (!ppi2)
    {
      ClosePrinter(hPrinter);
      return FALSE;
    }
    
    // The second GetPrinter() will fill in all the current information
    // so that all you need to do is modify what you're interested in...
    bFlag = GetPrinter(hPrinter, 2, (LPBYTE)ppi2, dwNeeded, &dwNeeded);
    if (!bFlag)
    {
      ClosePrinter(hPrinter);
      GlobalFree(ppi2);
      return FALSE;
    }
    
    // Set default printer attribute for this printer...
    ppi2->Attributes |= PRINTER_ATTRIBUTE_DEFAULT;
    bFlag = SetPrinter(hPrinter, 2, (LPBYTE)ppi2, 0);
    if (!bFlag)
    {
      ClosePrinter(hPrinter);
      GlobalFree(ppi2);
      return FALSE;
    }
    
    // Tell all open applications that this change occurred. 
    // Allow each application 1 second to handle this message.
    lResult = SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0L,
      (LPARAM)(LPCTSTR)"windows", SMTO_NORMAL, 1000, NULL);
  }
  
  // If Windows NT, use the SetDefaultPrinter API for Windows 2000,
  // or WriteProfileString for version 4.0 and earlier...
  else if (osv.dwPlatformId == VER_PLATFORM_WIN32_NT)
  {
#if(WINVER >= 0x0500)
    if (osv.dwMajorVersion >= 5) // Windows 2000 or later...
    {
      bFlag = SetDefaultPrinter(pPrinterName);
      if (!bFlag)
        return FALSE;
    }
    
    else // NT4.0 or earlier...
#endif
    {
      // Open this printer so you can get information about it...
      bFlag = OpenPrinter(pPrinterName, &hPrinter, NULL);
      if (!bFlag || !hPrinter)
        return FALSE;
      
      // The first GetPrinter() tells you how big our buffer should
      // be in order to hold ALL of PRINTER_INFO_2. Note that this will
      // usually return FALSE. This only means that the buffer (the 3rd
      // parameter) was not filled in. You don't want it filled in here...
      GetPrinter(hPrinter, 2, 0, 0, &dwNeeded);
      if (dwNeeded == 0)
      {
        ClosePrinter(hPrinter);
        return FALSE;
      }
      
      // Allocate enough space for PRINTER_INFO_2...
      ppi2 = (PRINTER_INFO_2 *)GlobalAlloc(GPTR, dwNeeded);
      if (!ppi2)
      {
        ClosePrinter(hPrinter);
        return FALSE;
      }
      
      // The second GetPrinter() fills in all the current<BR/>
      // information...
      bFlag = GetPrinter(hPrinter, 2, (LPBYTE)ppi2, dwNeeded, &dwNeeded);
      if ((!bFlag) || (!ppi2->pDriverName) || (!ppi2->pPortName))
      {
        ClosePrinter(hPrinter);
        GlobalFree(ppi2);
        return FALSE;
      }
      
      // Allocate buffer big enough for concatenated string.
      // String will be in form "printername,drivername,portname"...
      pBuffer = (LPTSTR)GlobalAlloc(GPTR,
        lstrlen(pPrinterName) +
        lstrlen(ppi2->pDriverName) +
        lstrlen(ppi2->pPortName) + 3);
      if (!pBuffer)
      {
        ClosePrinter(hPrinter);
        GlobalFree(ppi2);
        return FALSE;
      }
      
      // Build string in form "printername,drivername,portname"...
	  StringCchPrintf(pBuffer,255,"%s,%s,%s",pPrinterName,ppi2->pDriverName,ppi2->pPortName);
      
      // Set the default printer in Win.ini and registry...
      bFlag = WriteProfileString("windows", "device", pBuffer);
      if (!bFlag)
      {
        ClosePrinter(hPrinter);
        GlobalFree(ppi2);
        GlobalFree(pBuffer);
        return FALSE;
      }
    }
    
    // Tell all open applications that this change occurred. 
    // Allow each app 1 second to handle this message.
    lResult = SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0L, 0L,
      SMTO_NORMAL, 1000, NULL);
  }
  
  // Cleanup...
  if (hPrinter)
    ClosePrinter(hPrinter);
  if (ppi2)
    GlobalFree(ppi2);
  if (pBuffer)
    GlobalFree(pBuffer);
  
  return TRUE;
} 


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

BOOL CPrintPrv::GetDefaultPrintName(TCHAR * * buffer, int &iOrientation)
{
			//Récupération des informations sur l'imprimante par défaut
			DWORD size;
			DWORD m_dZeroValue = 0;
			HANDLE pPrinter;
			PDEVMODE pDevMode;
			int iReturn = 0;
			//char Temp[10];

			DPGetDefaultPrinter(NULL, &size);

			*buffer = new TCHAR[size];
			if (*buffer == NULL)
			{
				CObjet::ErreurInformation();
			}	

			DPGetDefaultPrinter(*buffer, &size);


			// Get a printer handle.
			if(OpenPrinter(*buffer, &pPrinter, NULL) == FALSE)
			{
				iReturn = 1;
				goto FIN;
			}

			//Retrieve the size of the DEVMODE.

			size = DocumentProperties(NULL, pPrinter, *buffer, 0, 0, 0);

			if(size < m_dZeroValue)
			{
				iReturn = 1;
				goto FIN;
			}

			//Fill the DEVMODE from the printer.

			pDevMode = (PDEVMODE)malloc(size);
			if (pDevMode == NULL)
			{
				CObjet::ErreurInformation();
			}
			//Récupération des informations sur la configuration de la page par défaut

			DocumentProperties(NULL, pPrinter, *buffer, pDevMode, 0, DM_OUT_BUFFER);

			iOrientation = pDevMode->dmOrientation;

FIN:

			free(pDevMode);

			ClosePrinter(pPrinter);

			return iReturn;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

BOOL CPrintPrv::RedrawPrintPage(HWND hDlg, CIBitmap * m_ciBitmap,CMargeData * m_MargeData)
{

	HWND hWnd = GetDlgItem(hDlg, IDC_PCTPREV);

	hWnd = GetDlgItem(hDlg,IDC_PCTPREV);
	HDC hdc = GetDC(hWnd);
	HDC hdcMem = CreateCompatibleDC(hdc); 

	SelectObject(hdcMem, m_ciBitmap->hBitmap);

	BitBlt(hdc,0,0, m_ciBitmap->GetBitmapWidth(), m_ciBitmap->GetBitmapHeigth(), hdcMem, 0, 0,SRCCOPY);

	DeleteDC(hdcMem);

	ReleaseDC(hWnd,hdc);

	//SendMessage(hDlg,WM_ERASEBKGND,0,0);

	return 0;
}

BOOL CPrint::ProcessPrint(LPVOID param, CMargeData * m_MargeData)
{

		LPPRINTDLG pPDX = (LPPRINTDLG)param;
			
		RECT rc;
		int xPage, yPage ;
		int iWidthPageSize, iHeightPageSize;

		xPage = GetDeviceCaps (pPDX->hDC, HORZRES) ;
		yPage = GetDeviceCaps (pPDX->hDC, VERTRES) ;
		iWidthPageSize = GetDeviceCaps (pPDX->hDC, HORZSIZE);
		iHeightPageSize = GetDeviceCaps (pPDX->hDC, VERTSIZE);

		rc.left = ((m_MargeData->m_fGauche * 10) * xPage) / iWidthPageSize;
		rc.right = xPage - rc.left - (((m_MargeData->m_fDroite * 10) * xPage) / iWidthPageSize);
		rc.top = ((m_MargeData->m_fHaut * 10) * yPage) / iHeightPageSize;
		rc.bottom = yPage - rc.top - (((m_MargeData->m_fBas * 10) * yPage) / iHeightPageSize);


		if(bUserAbort)
			return TRUE;

		float fLocalratio;
		int left, top;

		if(m_MargeData->m_ciBitmap->GetBitmapWidth() > m_MargeData->m_ciBitmap->GetBitmapHeigth())
			fLocalratio = (float)(rc.right) / (float)(m_MargeData->m_ciBitmap->GetBitmapWidth());
		else
			fLocalratio = (float)(rc.bottom) / (float)(m_MargeData->m_ciBitmap->GetBitmapHeigth());

		int nTailleAffichageWidth = m_MargeData->m_ciBitmap->GetBitmapWidth() * fLocalratio;
		int nTailleAffichageHeight =m_MargeData->m_ciBitmap->GetBitmapHeigth() * fLocalratio;

		if(nTailleAffichageHeight > (rc.bottom))
		{
			fLocalratio = (float)(rc.bottom)/ (float)(m_MargeData->m_ciBitmap->GetBitmapHeigth());
			nTailleAffichageWidth = m_MargeData->m_ciBitmap->GetBitmapWidth() * fLocalratio;
			nTailleAffichageHeight = m_MargeData->m_ciBitmap->GetBitmapHeigth() * fLocalratio;
		}
		else
		{
			if(nTailleAffichageWidth > (rc.right))
			{
				fLocalratio = (float)rc.right / (float)(m_MargeData->m_ciBitmap->GetBitmapWidth());
				nTailleAffichageWidth = m_MargeData->m_ciBitmap->GetBitmapWidth() * fLocalratio;
				nTailleAffichageHeight = m_MargeData->m_ciBitmap->GetBitmapHeigth() * fLocalratio;
			}
		}

		if(bUserAbort)
			return TRUE;

		// Use StretchDIBits to scale the bitmap and maintain 
		// its original proportions (that is, if the bitmap was square 
		// when it appeared in the application's client area, it should 
		// also appear square on the page). 


		int xValue = 0;
		int yValue = 0;

		m_MargeData->m_ciBitmap->ShowBitmap(pPDX->hDC,
			rc.left, rc.top,
			xValue,yValue,
			0,0,
			nTailleAffichageWidth, nTailleAffichageHeight,
			1);

		if(bUserAbort)
			return TRUE;

		// Retrieve the width of the string that specifies the full path 
		// and filename for the file that contains the bitmap. 

		tagSIZE szMetric;

		ZeroMemory(&szMetric,sizeof(tagSIZE));

		char cTemp[255];

		m_MargeData->m_ciBitmap->GetFilename(cTemp);

		GetTextExtentPoint32(pPDX->hDC, cTemp, strlen(cTemp), &szMetric); 

		// Compute the starting point for the text-output operation. The 
		// string will be centered horizontally and positioned three lines 
		// down from the top of the page. 

		left = ((GetDeviceCaps(pPDX->hDC, HORZRES) / 2) - (szMetric.cx / 2)); 
		top = (szMetric.cy * 3); 

		return TRUE;

		// Print the path and filename for the bitmap, centered at the top 
		// of the page. 

		//TextOut(pPDX->hDC, left, top, cTemp, strlen(cTemp));

		
}

//////////////////////////////////////////////////////////////////////
//Procédure d'impression
//////////////////////////////////////////////////////////////////////

//Fonction permettant d'arreter l'impression du bitmap

BOOL CALLBACK CPrint::AbortProc (HDC hdcPrn, int iCode)
{
     MSG msg ;
     
     while (!bUserAbort && PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
     {
          if (!hDlgPrint || !IsDialogMessage (hDlgPrint, &msg))
          {
               TranslateMessage (&msg) ;
               DispatchMessage (&msg) ;
          }
     }
     return !bUserAbort ;
}

///////////////////////////////////////////////////////////////////////////////////////////
//Affichage de la boite de dialogue d'information sur l'impression
///////////////////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK CPrint::PrintDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
     switch (message)
     {
     case WM_INITDIALOG:
          SetWindowText (hDlg, "Impression ...") ;
          EnableMenuItem (GetSystemMenu (hDlg, FALSE), SC_CLOSE, MF_GRAYED) ;
          return TRUE ;
          
     case WM_COMMAND:
		 {
			int wmId, wmEvent;
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			switch (wmId)
			{
				case IDOK:
					{
						bUserAbort = TRUE ;
						EnableWindow (GetParent (hDlg), TRUE) ;
						DestroyWindow (hDlg) ;
						hDlgPrint = NULL ;
						return TRUE ;
					}
			}
		 }
     }
     return FALSE ;
}

