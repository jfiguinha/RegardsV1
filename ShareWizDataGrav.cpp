#include "stdafx.h"
#include ".\sharewizdatagrav.h"
#include <EditRedirector.h>
#include <AspiDriver.h>
#include "parameter.h"

CShareWizDataGrav::CShareWizDataGrav(void)
{
	m_cParameter = new CParameter();
	m_cEditRedirector = new CEditRedirector();
	m_cAspiDriver = new CAspiDriver();
	hTitleFont = NULL;
	TempFile[0] = '\0';
	iDeviceSelect = 0;
	iTypeGravure = 0;
	hThread = NULL;

	IsBoxChecked = false; //The state of the first interior page's check box
	IsButtonClicked = false; //The state of the first interior page's group box
	iValueChecked = 0; //Valeur du bouton radio appuyez
	//other shared data added here
	iSelectAlbum = 0;
	nEstimatedSeconds = 0;
	iErreur = 0;
	hParent = NULL;
	hWnd = NULL;
	cFilename[0] = '\0';
	m_IconeFileVector = NULL;
	dwThreadId = 0; 
}

CShareWizDataGrav::~CShareWizDataGrav(void)
{
	if(m_cEditRedirector != NULL)
		delete m_cEditRedirector;

	m_cEditRedirector = NULL;

	if(m_cAspiDriver != NULL)
		delete m_cAspiDriver;

	m_cAspiDriver = NULL;

	if(hTitleFont != NULL)
		DeleteObject(hTitleFont);

	hTitleFont = NULL;

	if(hThread != NULL)
	{
		CloseHandle(hThread);
		hThread = NULL;
	}

	if(m_cParameter != NULL)
		delete m_cParameter;

	m_cParameter = NULL;
}


void CShareWizDataGrav::CreateFont()
{
    //Set up the font for the titles on the intro and ending pages

    NONCLIENTMETRICS ncm = {0};
    ncm.cbSize = sizeof(ncm);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

    //Create the intro/end title font
    LOGFONT TitleLogFont = ncm.lfMessageFont;
    TitleLogFont.lfWeight = FW_BOLD;
	StringCchCopy(TitleLogFont.lfFaceName,32,TEXT("Verdana Bold"));

    HDC hdc = GetDC(NULL); //gets the screen DC
    INT FontSize = 12;
    TitleLogFont.lfHeight = 0 - GetDeviceCaps(hdc, LOGPIXELSY) * FontSize / 72;
    hTitleFont = CreateFontIndirect(&TitleLogFont);
    ReleaseDC(NULL, hdc);
}

////////////////////////////////////////////////////////////////
//Recherche des informations
////////////////////////////////////////////////////////////////
void CShareWizDataGrav::TraitementTextFromBurner(HWND hwndDlg,char * pzOutput)
{
	char m_szMessage[255];
	string m_stgData;
	string m_stgData2;
	float m_fVitesseGravure;
	int iDataWrite;
	HWND hWnd;
	int iMaxDataWrite;
	int iPos, iPos2, iPos3;

	string m_stgOutput = pzOutput;
	iPos = m_stgOutput.find("Track");

	if(iPos != -1)
	{
		////////////////////////////////////////////////////////////////
		//Recherche des informations
		////////////////////////////////////////////////////////////////

		iPos += 6;
		iPos2 = m_stgOutput.find(":");
		if(iPos2 == -1)
			return;

		m_stgData = m_stgOutput.substr(iPos,iPos2 - iPos);
		hWnd = GetDlgItem(hwndDlg,IDC_TXTTRACK);
		SetWindowText(hWnd,m_stgData.c_str());

		////////////////////////////////////////////////////////////////
		//Recherche des données gravées :
		////////////////////////////////////////////////////////////////

		iPos2++;
		iPos = m_stgOutput.find("written");
		if(iPos == -1)
			return;

		m_stgData = m_stgOutput.substr(iPos2,iPos - iPos2);
		
		///////////////////////////////////////////////////////////////////
		//Décomposition des informations sur les tailles de données à graver
		iPos3 = m_stgData.find("of");
		if(iPos3 == -1)
			return;

		iDataWrite = atoi(m_stgData.substr(0,iPos3).c_str());

		iPos3 += 3;
		iPos2 = m_stgData.length();
		m_stgData2 = m_stgData.substr(iPos3,iPos2 - iPos3);
		iMaxDataWrite = atoi(m_stgData2.c_str());
		SendDlgItemMessage(hwndDlg, IDC_PRGDATA, PBM_SETRANGE , (WPARAM)0, MAKELPARAM(0,iMaxDataWrite));

		SendDlgItemMessage(hwndDlg, IDC_PRGDATA, PBM_SETPOS , iDataWrite, 0);

		hWnd = GetDlgItem(hwndDlg,IDC_TXTDATAINFOS);
		SetWindowText(hWnd,m_stgData.c_str());


		//////////////////////////////////////////////////////////////////
		//Taille du buffer
		//////////////////////////////////////////////////////////////////

		iPos = m_stgOutput.find("[buf");
		if(iPos == -1)
			return;

		iPos += 5;
		iPos2 = m_stgOutput.find("]");
		if(iPos2 == -1)
			return;

		m_stgData = m_stgOutput.substr(iPos,iPos2 - iPos);

		SendDlgItemMessage(hwndDlg, IDC_PRGBUFFER, PBM_SETPOS , atoi(m_stgData.c_str()), 0);

		//printf("%s\n",m_stgData.c_str());

		//////////////////////////////////////////////////////////////////
		//Vitesse de gravure
		//////////////////////////////////////////////////////////////////
		
		iPos = m_stgOutput.find("x.");
		if(iPos == -1)
			return;

		iPos2+=2;

		m_stgData = m_stgOutput.substr(iPos2,iPos - iPos2);

		hWnd = GetDlgItem(hwndDlg,IDC_TXTVITESSEGRAVURE);
		SetWindowText(hWnd,m_stgData.c_str());	

		//Calcul de la durée de gravure
		m_fVitesseGravure = atof(m_stgData.c_str());

		int iDonneeRestante = (iMaxDataWrite - iDataWrite) << 10;
		m_fVitesseGravure *= 150;
		int iDureeRestante = iDonneeRestante / m_fVitesseGravure;
		int iDureeEstimee = (iMaxDataWrite << 10) / m_fVitesseGravure; 

		char cConvertion[15];

		
		LoadString(hInstance,IDS_STGSECONDES,m_szMessage,255);

		StringCchPrintf(cConvertion,15,m_szMessage,iDureeRestante);

		hWnd = GetDlgItem(hwndDlg,IDC_TXTDUREERESTANTE);
		SetWindowText(hWnd,cConvertion);

		StringCchPrintf(cConvertion,15,m_szMessage,iDureeEstimee);

		hWnd = GetDlgItem(hwndDlg,IDC_TXTDUREEESTIMEE);
		SetWindowText(hWnd,cConvertion);	


		LoadString(hInstance,IDS_STGBURNON,m_szMessage,255);
		HWND hWnd = GetDlgItem(hwndDlg,IDC_TXTINFOS);
		SetWindowText(hWnd,m_szMessage);

	}
	else
	{
		iPos = m_stgOutput.find("Fixating"); 
		if(iPos != -1)
		{

			LoadString(hInstance,IDS_STGENDBURN,m_szMessage,255);
			HWND hWnd = GetDlgItem(hwndDlg,IDC_TXTINFOS);
			SetWindowText(hWnd,m_szMessage);
			SendDlgItemMessage(hwndDlg, IDC_PRGBUFFER, PBM_SETPOS , 0, 0);
		}
		else
		{
			iPos = m_stgOutput.find("Data may not fit on current disk.");
			if(iPos != -1)
			{
				char m_szCaption[255];
				LoadString(hInstance,IDS_STGCHGCD,m_szCaption,255);
				LoadString(hInstance,IDS_ERREUR,m_szMessage,255);
				if(MessageBox(NULL,m_szMessage,m_szCaption,MB_YESNO)==IDNO)
					iErreur = 1;
				else
					iErreur = 2;
			}
			else
			{
				iPos = m_stgOutput.find("seconds."); 
				if(iPos != -1)
				{
					char Temp[50];
					LoadString(hInstance,IDS_STGBURNBEGIN,m_szMessage,255);

					string m_stgParse = m_stgOutput.substr(iPos - 3,4);
					iPos = atoi(m_stgParse.c_str());
					StringCchPrintf(Temp,50,m_szMessage,iPos);
					HWND hWnd = GetDlgItem(hwndDlg,IDC_TXTINFOS);
					SetWindowText(hWnd,Temp);
				}
				else
				{
					LoadString(hInstance,IDS_STGINITBURNING,m_szMessage,255);
					HWND hWnd = GetDlgItem(hwndDlg,IDC_TXTINFOS);
					SetWindowText(hWnd,m_szMessage);
				}
			}
		}
	}
}
