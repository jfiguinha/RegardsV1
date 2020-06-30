#include "StdAfx.h"
#include ".\convertetrenommage.h"
#include "..\objet.h"
#include "..\resource.h"
#include ".\conversionfichier.h"
#include <convertdata.h>

CConvertEtRenommage::CConvertEtRenommage(HINSTANCE hInstance)
{
	m_hInstance = hInstance;
	m_cConvertData = new CConvertData();
}

CConvertEtRenommage::~CConvertEtRenommage(void)
{
	if(m_cConvertData != NULL)
		delete m_cConvertData;

	m_cConvertData = NULL;
}

void CConvertEtRenommage::InsertImageFormat(HWND hDlg, const char * cFormat, int iNumberFormat, int iDialogItem)
{
	SendDlgItemMessage(hDlg, iDialogItem, CB_INSERTSTRING , iNumberFormat, (LPARAM)(LPCTSTR)cFormat); //2
	SendDlgItemMessage(hDlg, iDialogItem, WM_SETTEXT , (WPARAM)0, (LPARAM)(LPCTSTR)cFormat);

}

//////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////
LRESULT CConvertEtRenommage::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int iNumberIncre = 0;

	InsertImageFormat(hDlg,"BMP",iNumberIncre++,IDC_CBLISTFORMAT);
	InsertImageFormat(hDlg,"JPEG",iNumberIncre++,IDC_CBLISTFORMAT);
	InsertImageFormat(hDlg,"TGA",iNumberIncre++,IDC_CBLISTFORMAT);
	InsertImageFormat(hDlg,"TIF",iNumberIncre++,IDC_CBLISTFORMAT);
	InsertImageFormat(hDlg,"PDF",iNumberIncre++,IDC_CBLISTFORMAT);

	SendDlgItemMessage(hDlg, IDC_CBLISTFORMAT, WM_SETTEXT , (WPARAM)0, (LPARAM)(LPCTSTR)"JPEG");

	iNumberIncre = 0;

	InsertImageFormat(hDlg,"1000",iNumberIncre++,IDC_CBINCREMEN);
	InsertImageFormat(hDlg,"100",iNumberIncre++,IDC_CBINCREMEN);
	InsertImageFormat(hDlg,"10",iNumberIncre++,IDC_CBINCREMEN);
	InsertImageFormat(hDlg,"1",iNumberIncre++,IDC_CBINCREMEN);

	SendDlgItemMessage(hDlg, IDC_CBINCREMEN, WM_SETTEXT , (WPARAM)0, (LPARAM)(LPCTSTR)"10");

	iNumberIncre = 0;

	InsertImageFormat(hDlg,"0000",iNumberIncre++,IDC_CBTAILLENUMERO);
	InsertImageFormat(hDlg,"000",iNumberIncre++,IDC_CBTAILLENUMERO);
	InsertImageFormat(hDlg,"00",iNumberIncre++,IDC_CBTAILLENUMERO);
	InsertImageFormat(hDlg,"0",iNumberIncre++,IDC_CBTAILLENUMERO);

	SendDlgItemMessage(hDlg, IDC_CBTAILLENUMERO, WM_SETTEXT , (WPARAM)0, (LPARAM)(LPCTSTR)"00");

	CheckRadioButton(hDlg,IDC_RDPOSDROITE,IDC_RDPOSGAUCHE,IDC_RDPOSDROITE);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////
LRESULT CConvertEtRenommage::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId    = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{
		case IDC_BTREPORIGINE:
			{
				CObjet m_cObjet;
				if(m_cObjet.GetDirectory(hDlg,m_cConvertData->m_szRepOrigin))
				{
					SetDlgItemText(hDlg, IDC_LBLREPORIGINE, m_cConvertData->m_szRepOrigin);
				}
			}
			return false;
	
		case IDC_BTREPDEST:
			{
				CObjet m_cObjet;
				if(m_cObjet.GetDirectory(hDlg,m_cConvertData->m_szRepDest))
					SetDlgItemText(hDlg, IDC_LBLREPDESTINATION, m_cConvertData->m_szRepDest);
			}
			return false;


		case IDOK:
			{


				//Test si toutes données sont correctes
				GetDlgItemText(hDlg, IDC_LBLLIBELLE, m_cConvertData->m_szLibelle,100);
				if(strcmp(m_cConvertData->m_szRepDest,"") != 0 && strcmp(m_cConvertData->m_szRepOrigin,"") != 0 && strcmp(m_cConvertData->m_szLibelle,"") != 0)
				{
					char cTemp[25];
					m_cConvertData->iConverFormat = 0;
					m_cConvertData->iDelImage = 0;
					m_cConvertData->iRedimensionne = 0;

					if(IsDlgButtonChecked(hDlg,IDC_RDPOSDROITE))
						m_cConvertData->iPosNumero = 1; //Libelle à droite
					else
						m_cConvertData->iPosNumero = 2; //Libelle à gauche
					
					GetDlgItemText(hDlg, IDC_CBTAILLENUMERO, cTemp,25);
					m_cConvertData->iTailleNumero = strlen(cTemp);
					GetDlgItemText(hDlg, IDC_CBINCREMEN, cTemp,25);
					m_cConvertData->iIncrementation = atoi(cTemp);

					if(IsDlgButtonChecked(hDlg,IDC_CHKCONVERTFORMAT))
					{

						m_cConvertData->iConverFormat = 1;
						GetDlgItemText(hDlg, IDC_CBLISTFORMAT, cTemp,25);
						if(_stricmp(cTemp,"JPEG") == 0)
							m_cConvertData->iNumFormat = 1;

						if(_stricmp(cTemp,"TIF") == 0)
							m_cConvertData->iNumFormat = 4;

						if(_stricmp(cTemp,"TGA") == 0)
							m_cConvertData->iNumFormat = 3;

						if(_stricmp(cTemp,"BMP") == 0)
							m_cConvertData->iNumFormat = 2;

						if(_stricmp(cTemp,"PDF") == 0)
							m_cConvertData->iNumFormat = 5;
					}

					if(IsDlgButtonChecked(hDlg,IDC_CHKDELPHOTO))
					{
						m_cConvertData->iDelImage = 1;
					}

					if(IsDlgButtonChecked(hDlg,IDC_CHKREDIM))
					{

						m_cConvertData->iRedimensionne = 1;
						m_cConvertData->iRapport = IsDlgButtonChecked(hDlg,IDC_CHKRAPPORT);
						BOOL m_bSuccess;

						if(IsDlgButtonChecked(hDlg,IDC_CHKPOURCENT))
						{
							m_cConvertData->iPourcentage = 1;
							m_cConvertData->iValeurPourcentage = GetDlgItemInt(hDlg,IDC_POURCENT,&m_bSuccess,TRUE);
						}
						else
						{
							m_cConvertData->iPourcentage = 0;
							m_cConvertData->iHeight = GetDlgItemInt(hDlg,IDC_LBLHAUTEUR,&m_bSuccess,TRUE);
							m_cConvertData->iWidth = GetDlgItemInt(hDlg,IDC_LARGEUR,&m_bSuccess,TRUE);
						}
					}

					CConversionFichier * m_cConversionFichier = new CConversionFichier();
					m_cConversionFichier->SetParam(m_cConvertData);
					m_cConversionFichier->CreateDlg(m_hInstance,(LPCTSTR)IDD_FICHIER, hDlg);
					delete m_cConversionFichier;

					//DialogBoxParam(hInstance, (LPCTSTR)IDD_FICHIER, hDlg, (DLGPROC)CDialogPrv::ConversionFichier,(LPARAM)&m_CONVERTDATA);
					EndDialog(hDlg, LOWORD(0));
					return TRUE;
				}
				else
				{
					CObjet m_cObjet;
					m_cObjet.AffichageMessage(IDS_RENSFORM,IDS_INFOS);
				}
			}
			return FALSE;

		case IDCANCEL:
			EndDialog(hDlg, LOWORD(0));
			return TRUE;

	}
	return 0;
}
