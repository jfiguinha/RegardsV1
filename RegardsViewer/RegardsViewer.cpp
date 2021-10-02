#include <windows.h>
#include <atlbase.h>
#include <atlwin.h>
#include <global.h>
#include <WinMain.h>
#include <video.h>
#include "Resource.h"
#include <commctrl.h>
#include <strsafe.h>
//#include "floattoint.h"
#include <AffichageBitmap.h>
#include <WndIntro.h>
#include <objet.h>
#include <parameter.h>
#include <iefunction.h>
#include <JpegFile.h>
#include <ibitmap.h>
#include <global.h>
// Media Foundation 
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#ifdef SQLSERVERCE
#include <SqlEngine.h>
#include <SqlLibRegards.h>
using namespace Regards::Sqlite;
#endif



#ifdef WMP9

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

#endif

HINSTANCE hInstance;
INT NBSCREEN = 0;
HWND mainWindow = NULL;
/////////////////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////////////////
ULONG __stdcall ShowIntro(LPVOID parametre)
{
	OleInitialize(NULL);
	CWndIntro* m_cWndIntro = (CWndIntro*)parametre;
	m_cWndIntro->Run(NULL);
	ExitThread(0);
	OleUninitialize();
	return 0;

}

/////////////////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////////////////
ULONG __stdcall VerifNewVersion(LPVOID parametre)
{
	//CoInitialize(NULL);
	CIEFunction* m_cIEFunction = new CIEFunction();

	char m_Version[255];
	char m_AdresseWeb[255];

	LoadString(hInstance, IDS_REGARDSVERSION, m_Version, 255);
	LoadString(hInstance, IDS_ADRESSEWEBVERSION, m_AdresseWeb, 255);

	if (m_cIEFunction->TestIfNewVersionAvailable(m_AdresseWeb, m_Version))
	{
		char m_Message[255];
		char m_Caption[255];
		char m_Site[255];
		LoadString(hInstance, IDS_STGINFOSDOWNLOAD, m_Message, 255);
		LoadString(hInstance, IDS_INFOS, m_Caption, 255);
		LoadString(hInstance, IDS_ADRESSEWEB, m_Site, 255);
		if (MessageBox(NULL, m_Message, m_Caption, MB_OKCANCEL) == IDOK)
		{
			ShellExecute(NULL, "open", "iexplore", m_Site, NULL, SW_SHOW);
		}
	}
	delete m_cIEFunction;
	//::CoUninitialize();
	return 0;

}
/////////////////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////////////////
int CreateWinMainWindow(int iShowIntro, HINSTANCE hInst, HWND hWndIntro, char* szCommandLine, bool bCommandLine, bool& bExplorer)
{

	CWinMain* m_CWinMain = new CWinMain();
	RECT rc;
	const char	szAppName[] = "Regards";
	DWORD dwWndStyle;
	DWORD dwStyle = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	if (bCommandLine)
		dwWndStyle = WS_OVERLAPPEDWINDOW;
	else
		dwWndStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_MAXIMIZE | WS_MAXIMIZEBOX;

	GetWindowRect(GetDesktopWindow(), &rc);
	rc.left = 0;
	rc.top = 0;
	m_CWinMain->ShowIntro(iShowIntro);
	m_CWinMain->SetIntroWindow(hWndIntro);
	if (bCommandLine)
		m_CWinMain->SetCommandLine(szCommandLine);
	m_CWinMain->CreateEx(NULL, hInst, dwWndStyle, dwStyle, 0, "Regards", IDM_REGARDS, IDM_REGARDS, IDC_ARROW, rc);
	mainWindow = m_CWinMain->GetWnd();
	m_CWinMain->Run(IDR_REGARDS);
	//
	bExplorer = m_CWinMain->GetExplorer();
	delete m_CWinMain;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////////////////
int CreateIntroWindow(CWndIntro* cWndIntro, HINSTANCE hInst)
{

	int iWidth = (GetDeviceCaps(GetDC(NULL), HORZRES) - 414) / 2;
	int iHeight = (GetDeviceCaps(GetDC(NULL), VERTRES) - 141) / 2;
	DWORD dwStyle = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	DWORD dwWndStyle = WS_POPUPWINDOW | WS_VISIBLE;
	RECT rc = { iWidth,iHeight,414,141 };
	cWndIntro->CreateEx(NULL, hInstance, dwWndStyle, dwStyle, 0, "Intro", NULL, NULL, IDC_ARROW, rc);

	DWORD dwThreadId;

	HANDLE hThread = (HANDLE)CreateThread(NULL, 1024,
		ShowIntro, cWndIntro, 0, &dwThreadId);

	SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);

	while (cWndIntro->m_hWnd == NULL)
	{
		Sleep(50);
	}

	return 0;
}

#ifdef SQLSERVERCE

void InitializeSQLServerDatabase()
{
	CSqlLibRegards* libExplorer = new CSqlLibRegards(false, true);
	size_t m_sizeTConvert;
	CParameter m_cParameter;
	char m_szDirectory[MAX_PATH];
	m_cParameter.GetOptionAlbumDir(m_szDirectory, MAX_PATH);
	StringCbCat(m_szDirectory, MAX_PATH, "\\regards.sdf");
	//CSqlEngine::Initialize(m_wFilePath);
	CSqlEngine::Initialize(m_szDirectory, "RegardsDB", libExplorer);
}

#endif

//------------------------------------------------------------------
//Détection du nombre de moniteur
//Insertion des infos dans la base de registre
//------------------------------------------------------------------
/*
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	RECT Rect;
	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	Rect = *lprcMonitor;
	GetMonitorInfo(hMonitor, &mi);
	//Sauvegarde des paramètres
	NBSCREEN++;

	CParameter::SetInfosScreen(NBSCREEN, mi.rcMonitor);

	return TRUE;
}
*/
/////////////////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd)
{
	char m_szCommandLine[MAX_PATH];
	bool m_bCommandLine = false;
	bool m_bExplorer = false;
	m_szCommandLine[0] = '\0';
	CWndIntro* m_cWndIntro = new CWndIntro();
	hInstance = hInst;

	// Initialize COM
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	if (SUCCEEDED(hr))
	{
		// Initialize Media Foundation.
		hr = MFStartup(MF_VERSION);
	}


	INITCOMMONCONTROLSEX iccex;

	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	iccex.dwICC = ICC_COOL_CLASSES;
	InitCommonControlsEx(&iccex);

#ifdef SQLSERVERCE

	InitializeSQLServerDatabase();

#endif

#ifdef WMP9
	_Module.Init(ObjectMap, hInstance, &LIBID_ATLLib);
#endif

	//Détection du nombre d'écran
	// work out how big we have to be to cover all the screens.
	//EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);

	CParameter::SetNbScreen(NBSCREEN);

	DWORD dwThreadId;

	HANDLE hThread = (HANDLE)CreateThread(NULL, 1024,
		VerifNewVersion, NULL, 0, &dwThreadId);

	SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);

	int m_iShowIntro = 1;

	if (lpCmdLine[0] == '\"')
	{
		int iSize = strlen(lpCmdLine);
		for (int i = 1; i < iSize - 1; i++)
		{
			lpCmdLine[i - 1] = lpCmdLine[i];
		}
		lpCmdLine[iSize - 2] = '\0';
	}

	//WORD OldCtrlWord = Changex87FPToTrunc();

	if (strcmp(lpCmdLine, "") != 0)
	{
		bool m_bExplorer = false;

		char* szFileLine;

		GetFullPathName(lpCmdLine, MAX_PATH, m_szCommandLine, &szFileLine);

		if (strcmp(m_szCommandLine, "") == 0)
		{
			memcpy(m_szCommandLine, lpCmdLine + 1, strlen(lpCmdLine) - 2);
			m_szCommandLine[strlen(lpCmdLine) - 2] = '\0';

		}

		m_bCommandLine = true;
		m_iShowIntro = 0;
	}


	if (m_iShowIntro)
		CreateIntroWindow(m_cWndIntro, hInst);

	CreateWinMainWindow(m_iShowIntro, hInst, m_cWndIntro->m_hWnd, m_szCommandLine, m_bCommandLine, m_bExplorer);
	if (m_bExplorer && m_bCommandLine)
	{
		CParameter m_cParameter;
		char m_szDirectory[MAX_PATH];
		m_cParameter.GetOptionAlbumDir(m_szDirectory, MAX_PATH);
		StringCbCat(m_szDirectory, MAX_PATH, "\\regards.exe");
		ShellExecute(NULL, "open", m_szDirectory, NULL, NULL, SW_SHOWNORMAL);
	}

	if (m_iShowIntro)
		DestroyWindow(m_cWndIntro->m_hWnd);


FIN:

#ifdef WMP9
	_Module.RevokeClassObjects();
	_Module.Term();
#endif	

#ifdef SQLSERVERCE
	CSqlEngine::kill("RegardsDB");
#endif

	TerminateThread(hThread, 0);
	delete m_cWndIntro;

	//OleUninitialize();

	CVideo::Cleanup();

	//Setx87ControlWord(OldCtrlWord);
	MFShutdown();
	CoUninitialize();


	return 0;
}
