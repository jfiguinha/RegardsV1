#include "StdAfx.h"
#include ".\editredirector.h"
#include <AspiDriver.h>
#include <Edit.h>
#include "resource.h"
#include "objet.h"
#include <sadirread.h>
#include "parameter.h"

CEditRedirector::CEditRedirector(void)
{
	m_stgValue = "";
	iIsoCreating = iIsoWriting = 0;
	hWndDialogBox = NULL;
	hThread = NULL;
	m_pWnd = new CEdit();
	m_cParameter = new CParameter();
}

CEditRedirector::~CEditRedirector(void)
{
	if(m_pWnd != NULL)
		delete m_pWnd;

	m_pWnd = NULL;

	delete m_cParameter;
}


LRESULT CEditRedirector::CreateIsoFile(HWND hWnd,CAspiDriver * m_cAspiDriver,char * TempIsoFile)
{
	hWndDialogBox = hWnd;
	iIsoCreating = 1;
	char m_cLigneCommande[1024];

	int iNbTrack = 0;
	int iMediaInfos = m_cAspiDriver->InfosMedia(iNbTrack);

	if(iNbTrack > 1 && (iMediaInfos == 0 || iMediaInfos == 1))
	{
		int iTrackBegin = 0,iValueTemp = 0;
		int iTrackEnding = 0;
		m_cAspiDriver->ReadTrackInfo(iNbTrack-1,iTrackBegin,iValueTemp);
		m_cAspiDriver->ReadTrackInfo(iNbTrack,iTrackEnding,iValueTemp);
		StringCchPrintf(m_cLigneCommande,1024,"%s -v -C %d,%d -M %d,%d,%d -J -R -o %s %s",m_cParameter->GetMKisofsFile().c_str(),iTrackBegin,iTrackEnding,m_cAspiDriver->GetHaId(),m_cAspiDriver->GetTarget(),m_cAspiDriver->GetLunId(),
			GetTempIsoFile().c_str(),TempIsoFile);
	}
	else
		StringCchPrintf(m_cLigneCommande,1024,"%s -v -J -R -o %s %s",m_cParameter->GetMKisofsFile().c_str(),GetTempIsoFile().c_str(),TempIsoFile);

	Open(m_cLigneCommande);
	m_pWnd->SetWindowText("");		// add the message to the end of Edit control
	
	SendDlgItemMessage(hWnd, IDC_PRGFILE, PBM_SETRANGE , (WPARAM)0, MAKELPARAM(0,100));
	SendDlgItemMessage(hWnd, IDC_PRGFILE, PBM_SETSTEP , (WPARAM)1, 0);

	return 1;
}

LRESULT CEditRedirector::BurnIsoFile(HWND hWnd,char * m_szCommand)
{
	hWndDialogBox = hWnd;
	iIsoWriting = 1;
	Open(m_szCommand);
	m_pWnd->SetWindowText("");		// add the message to the end of Edit control

	SendDlgItemMessage(hWnd, IDC_PRGDATA, PBM_SETSTEP , (WPARAM)1, 0);
	SendDlgItemMessage(hWnd, IDC_PRGBUFFER, PBM_SETRANGE , (WPARAM)0, MAKELPARAM(0,100));
	SendDlgItemMessage(hWnd, IDC_PRGBUFFER, PBM_SETSTEP , (WPARAM)1, 0);

	return 0;
}


LRESULT CEditRedirector::OnInitDialog(HWND LocalhWnd)
{
	m_pWnd->SetWindow(LocalhWnd);

	LOGFONT m_lf;
	//	Initialize LOGFONT structure
	memset(&m_lf, 0, sizeof(m_lf));
	m_lf.lfWeight = FW_NORMAL;
	m_lf.lfCharSet = GB2312_CHARSET;//ANSI_CHARSET;
	m_lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	m_lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	m_lf.lfQuality = DEFAULT_QUALITY;
	m_lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	StringCchCopy(m_lf.lfFaceName,32,"FixedSys");

	DWORD dwStyle = GetWindowLong(LocalhWnd,GWL_STYLE );
	if( dwStyle )
	{
		dwStyle |= DS_LOCALEDIT;
		SetWindowLong(LocalhWnd,GWL_STYLE,dwStyle );
	}

	HFONT m_defFont = CreateFontIndirect( &m_lf );

	if(m_defFont != NULL)
		SendMessage(LocalhWnd,WM_SETFONT,(WPARAM)m_defFont,1);

	return TRUE;
}

void CEditRedirector::WriteStdOut(LPCSTR pszOutput)
{
	if (m_pWnd->m_hWnd != NULL)
	{
		int nSize = m_pWnd->GetWindowTextLength();
		string m_stgOuput = CarriageReturnUnixToWindows(pszOutput);
		
		if(hWndDialogBox != NULL)
			SendMessage(hWndDialogBox,WM_COMMAND,MAKEWPARAM(IDM_SENDTEXT,0),(LPARAM)pszOutput);

		m_pWnd->SetSel(m_stgValue.length()+1, m_stgValue.length()+1);
		//Reformatage de la sortie 
		m_pWnd->ReplaceSel(m_stgOuput.c_str());		// add the message to the end of Edit control

		m_stgValue += m_stgOuput;
	}
}

void CEditRedirector::WriteStdError(LPCSTR pszError)
{
	WriteStdOut("\r\nError message: ");
	WriteStdOut(pszError);
}

string CEditRedirector::GetTempIsoDirectory()
{
	char puffer[MAX_PATH];
	m_cParameter->GetOptionAlbumDir(puffer,MAX_PATH);
	CSADirRead m_cCSADirRead;

	string m_stgFolder = puffer;
	if(puffer[strlen(puffer) - 1] != '\\')
		m_stgFolder += "\\temp";
	else
		m_stgFolder += "temp";


	

	return m_stgFolder;
}

string CEditRedirector::GetTempIsoFile()
{
	char puffer[MAX_PATH];
	m_cParameter->GetOptionAlbumDir(puffer,MAX_PATH);
	
	string m_stgFolder = puffer;
	if(puffer[strlen(puffer) - 1] != '\\')
		m_stgFolder += "\\temp.iso";
	else
		m_stgFolder += "temp.iso";

	GetShortPathName(m_stgFolder.c_str(),puffer,512);

	return m_stgFolder;
}


INT64 CEditRedirector::GetIsoFileSize()
{
	string m_stgFile = GetTempIsoFile();
	INT64 length;
	HANDLE fileh;

	
    //get the text of the file into a string buffer
    if ( (fileh=CreateFile(m_stgFile.c_str() ,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
                           FILE_FLAG_SEQUENTIAL_SCAN,NULL))==INVALID_HANDLE_VALUE ) 
	{
       return -1;
    }

	if ( GetFileType(fileh)!=FILE_TYPE_DISK )
	{
       return -1;
    }

    length = GetFileSize( fileh,NULL );

	CloseHandle(fileh);

	return length;

}

INT64 CEditRedirector::GetIsoFileSize(string m_stgFile)
{
	INT64 length;
	HANDLE fileh;

	
    //get the text of the file into a string buffer
    if ( (fileh=CreateFile(m_stgFile.c_str() ,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
                           FILE_FLAG_SEQUENTIAL_SCAN,NULL))==INVALID_HANDLE_VALUE ) 
	{
       return -1;
    }

	if ( GetFileType(fileh)!=FILE_TYPE_DISK )
	{
       return -1;
    }

    length = GetFileSize( fileh,NULL );

	CloseHandle(fileh);

	return length;

}

void CEditRedirector::SignalApplicationEnding()
{
	m_stgValue = "";
	iIsoCreating = iIsoWriting = 0;

	if(hWndDialogBox != NULL)
	{
		SendMessage(::GetParent(hWndDialogBox),PSM_PRESSBUTTON,PSBTN_NEXT,0);
	}
}
