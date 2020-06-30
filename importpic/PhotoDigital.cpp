// PhotoDigital.cpp: implementation of the CPhotoDigital class.
//
//////////////////////////////////////////////////////////////////////

#include "PhotoDigital.h"
#include "LoadImage.h"

int CPhotoDigital::iNumImage;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPhotoDigital::CPhotoDigital()
{
	
}

CPhotoDigital::~CPhotoDigital()
{
}

LRESULT CALLBACK CPhotoDigital::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
	{ 
		case WM_CREATE: 
			break; 

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case TWCPP_ENDTRANSFER:
					PostQuitMessage(0);
			}
			break;

		case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
			break;


		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		return 0;
}

HWND CPhotoDigital::CreatePhotoWindow(HWND hWndParent, int nCmdShow)
{
	WNDCLASSEX	wc;
	MSG			msg;
	HWND hwndMain;
	
	const char	szAppName[] = "Photo";

	//
	//	Register our main window class
	//
	wc.cbSize			= sizeof(wc);
	wc.style			= 0;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra		= 0;

	wc.cbWndExtra = 0;

	wc.hInstance		= GetModuleHandle(NULL);
	wc.hIcon			= NULL;
	wc.hCursor			= LoadCursor (NULL, IDC_SIZEWE);
	wc.hbrBackground	= (HBRUSH)(COLOR_3DFACE+1);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= szAppName;
	wc.hIconSm			= NULL;

	RegisterClassEx(&wc);

	//hInstance = hInst;



	//
	//	Create the main window. This window
	//  will host two child controls.
	//
	hwndMain = CreateWindowEx(0,		// extended style (not needed)
				szAppName,				// window class name
				szAppName,				// window caption
				WS_OVERLAPPEDWINDOW|
				WS_VISIBLE | WS_MAXIMIZE,		// window style
				0,			// initial x position
				0,			// initial y position
				0,			// initial x size
				0,			// initial y size
				NULL,					// parent window handle
				NULL,					// use window class menu
				GetModuleHandle(NULL),					// program instance handle
				NULL);					// creation parameters


	ShowWindow(hwndMain, nCmdShow);

	InitTwain(hwndMain);

	if(SelectSource())
		Acquire();

	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		ProcessMessage(msg);
		DispatchMessage(&msg);
	}
	
	ReleaseTwain();
	return hwndMain;
}

//InitTwain(hWnd);

void CPhotoDigital::CopyImage(HWND hWnd, HANDLE hBitmap,TW_IMAGEINFO& info)
{
	//Traitement des images suivant les paramètres
	CLoadImage m_cChargementImage;
	CIBitmap m_ciBitmap;
	HBITMAP hBmp;
	hBmp = m_ciBitmap.HandleToBitmap(hBitmap,info.BitsPerPixel);

	//Génération du nom de l'image;

	iNumImage++;

	char m_szTemp[10];
	char m_szPath[1024];
	long m_lNumImage = iNumImage * m_PHOTODIGITALDATA.iTypeIncrementation;
	sprintf(m_szTemp,"%d",m_lNumImage);

	switch(strlen(m_szTemp))
	{
		case 1:
			sprintf(m_szPath,"%s\\%s00000%d",m_PHOTODIGITALDATA.m_szDirectory,m_PHOTODIGITALDATA.m_szFilename,m_lNumImage); 
			break;
		case 2:
			sprintf(m_szPath,"%s\\%s0000%d",m_PHOTODIGITALDATA.m_szDirectory,m_PHOTODIGITALDATA.m_szFilename,m_lNumImage); 
			break;
		case 3:
			sprintf(m_szPath,"%s\\%s000%d",m_PHOTODIGITALDATA.m_szDirectory,m_PHOTODIGITALDATA.m_szFilename,m_lNumImage); 
			break;
		case 4:
			sprintf(m_szPath,"%s\\%s00%d",m_PHOTODIGITALDATA.m_szDirectory,m_PHOTODIGITALDATA.m_szFilename,m_lNumImage); 
			break;
		case 5:
			sprintf(m_szPath,"%s\\%s0%d",m_PHOTODIGITALDATA.m_szDirectory,m_PHOTODIGITALDATA.m_szFilename,m_lNumImage); 
			break;
		default:
			sprintf(m_szPath,"%s\\%s%d",m_PHOTODIGITALDATA.m_szDirectory,m_PHOTODIGITALDATA.m_szFilename,m_lNumImage); 
	}

	switch(m_PHOTODIGITALDATA.iFormatImage)
	{
		case 1:
			strcat(m_szPath,".jpg");
			m_cChargementImage.JPEGSAVE(hBmp,m_szPath,90);
			break;
		case 2:
			strcat(m_szPath,".bmp");
			m_cChargementImage.BMPSAVE(hBmp,m_szPath);
			break;
	}
	
	m_ciBitmap.EraseData();
	

}

void CPhotoDigital::SetParametre(PHOTODIGITALDATA &m_PHOTODIGITALDATA)
{
	iNumImage = 0;
	this->m_PHOTODIGITALDATA = m_PHOTODIGITALDATA;
}
