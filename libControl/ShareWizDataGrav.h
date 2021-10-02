

#ifndef __SHAREWIZDATAGRAV__H__
#define __SHAREWIZDATAGRAV__H__

#include <IconeFile.h>

class CAspiDriver;
class CEditRedirector;
class CParameter;

class CShareWizDataGrav
{
public:
	CShareWizDataGrav(void);
	~CShareWizDataGrav(void);

	void CreateFont();
	void TraitementTextFromBurner(HWND hwndDlg,char * pzOutput);

	HFONT hTitleFont; //The title font for the Welcome and Completion pages
	BOOL IsBoxChecked; //The state of the first interior page's check box
	BOOL IsButtonClicked; //The state of the first interior page's group box
	int iValueChecked; //Valeur du bouton radio appuyez
	//other shared data added here
	int iSelectAlbum;
	int nEstimatedSeconds;
	int iErreur;

	HWND hParent;
	HWND hWnd;
	int iTabIndice[20];
	int iTypeGravure;
	char cFilename[512];
	char TempFile[512];
	int iDeviceSelect;
	IconeFileVector * m_IconeFileVector;
	CEditRedirector * m_cEditRedirector;
	CAspiDriver * m_cAspiDriver;
	CParameter * m_cParameter;
	DWORD dwThreadId; 
	HANDLE hThread; 
	
};

#endif