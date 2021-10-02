#include "stdafx.h"
#include ".\association.h"
#include <registryaccess.h>
#include "resource.h"
#include "objet.h"

CAssociation::CAssociation(HINSTANCE hInstance)
{
	this->hInstance = hInstance;
}

CAssociation::~CAssociation(void)
{
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

int CAssociation::AssociateRegards()
{
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();
	//Dim sPath As String
	char sPath[MAX_PATH];
	char cDirLong[MAX_PATH];
	char cDirShort[MAX_PATH];

	//Create the 'MyApp.Document' item under
	//HKEY_CLASSES_ROOT. This is where you'll put
	//the command line to execute or other shell
	//statements necessary.

	m_cRegistryAccess->CreateNewKey((LPTSTR)"Regards.Document", HKEY_CLASSES_ROOT);
	m_cRegistryAccess->CreateNewKey((LPTSTR)"Regards.Document\\shell\\open\\command", HKEY_CLASSES_ROOT);
	m_cRegistryAccess->CreateNewKey((LPTSTR)"Regards.Document\\DefaultIcon", HKEY_CLASSES_ROOT);


	m_cRegistryAccess->SetKeyValue(HKEY_CLASSES_ROOT,"Regards.Document", "", "Regards Document");
	
	GetCurrentDirectory(MAX_PATH,cDirLong);

	GetShortPathName(cDirLong,cDirShort,MAX_PATH);

	//Convertir LongPath en ShortPath

	StringCchPrintf(sPath,MAX_PATH,"%s\\regards.exe %%1",cDirShort);
	m_cRegistryAccess->SetKeyValue(HKEY_CLASSES_ROOT,"Regards.Document\\shell\\open\\command", "", sPath);

	StringCchPrintf(sPath,MAX_PATH,"%s\\regards.exe,1",cDirShort);

 	m_cRegistryAccess->SetKeyValue(HKEY_CLASSES_ROOT,"Regards.Document\\DefaultIcon", "", sPath);
	


	m_cRegistryAccess->CreateNewKey((LPTSTR)"Applications\\regards.exe", HKEY_CLASSES_ROOT);
	m_cRegistryAccess->CreateNewKey((LPTSTR)"Applications\\regards.exe\\shell\\open\\command", HKEY_CLASSES_ROOT);
	m_cRegistryAccess->CreateNewKey((LPTSTR)"Applications\\regards.exe\\DefaultIcon", HKEY_CLASSES_ROOT);

	m_cRegistryAccess->SetKeyValue(HKEY_CLASSES_ROOT,"Applications\\regards.exe", "", "Regards Document");

	StringCchPrintf(sPath,MAX_PATH,"%s\\regards.exe %%1",cDirShort);
	m_cRegistryAccess->SetKeyValue(HKEY_CLASSES_ROOT,"Applications\\regards.exe\\shell\\open\\command", "", sPath);

	//StringCchPrintf(sPath,"%s\\regards.exe,1",cDir);

	StringCchPrintf(sPath,MAX_PATH,"%s\\regards.exe,1",cDirShort);
 	m_cRegistryAccess->SetKeyValue(HKEY_CLASSES_ROOT,"Applications\\regards.exe\\DefaultIcon", "", sPath);

	delete m_cRegistryAccess;
	return 0;
}

int CAssociation::AssociateVistaRegards()
{
	return 0;
}

int CAssociation::AssociateFileExtVista(LPTSTR szFileExtension)
{
	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();

	//File Associations begin with a listing
	//of the default extension under HKEY_CLASSES_ROOT.
	//So the first step is to create that
	//root extension item
	
	char sPath[MAX_PATH];
	char cDirLong[MAX_PATH];
	char cDirShort[MAX_PATH];
	char TempKey[255];
	char Temp[255];

	StringCchPrintf(Temp,255,"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.%s",szFileExtension);
	m_cRegistryAccess->CreateNewKey(Temp, HKEY_CURRENT_USER);

	StringCchPrintf(TempKey,255,"Regards.Document.%s",szFileExtension);
	StringCchPrintf(Temp,255,"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.%s\\UserChoice",szFileExtension);
	m_cRegistryAccess->CreateNewKey(Temp, HKEY_CURRENT_USER);
	m_cRegistryAccess->SetKeyValue(HKEY_CURRENT_USER,Temp,"Progid","Applications\\Regards.exe");

	StringCchPrintf(TempKey,255,"Regards.Document.%s",szFileExtension);
	StringCchPrintf(Temp,255,"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.%s\\OpenWithProgids",szFileExtension);
	m_cRegistryAccess->CreateNewKey(Temp, HKEY_CURRENT_USER);
	m_cRegistryAccess->WriteBinValue("Regards.Document");

	delete m_cRegistryAccess;

    return 0;
} // AssignRMAsDefaultDotICS

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


int CAssociation::AssociateFileExt(LPTSTR szFileExtension)
{

	
	char Temp[255];
	char Temp2[255];

	
	
	int iVersion = CObjet::GetWindowsVersionAvance();
	if(iVersion >= 60)
		return AssociateFileExtVista(szFileExtension);

	StringCchPrintf(Temp,255,".%s",szFileExtension);

	CRegistryAccess * m_cRegistryAccess = new CRegistryAccess();

	//File Associations begin with a listing
	//of the default extension under HKEY_CLASSES_ROOT.
	//So the first step is to create that
	//root extension item
	m_cRegistryAccess->CreateNewKey(Temp, HKEY_CLASSES_ROOT);


	//To the extension just added, add a
	//subitem where the registry will look for
	//commands relating to the .xxx extension
	//("MyApp.Document"). Its type is String (REG_SZ)
	m_cRegistryAccess->SetKeyValue(HKEY_CLASSES_ROOT,Temp, "", "Regards.Document");
	StringCchPrintf(Temp2,255,"image/%s",szFileExtension);
	m_cRegistryAccess->SetKeyValue(HKEY_CLASSES_ROOT,Temp, "Content Type", Temp2);
	m_cRegistryAccess->SetKeyValue(HKEY_CLASSES_ROOT,Temp, "PerceivedType", "image");

	StringCchPrintf(Temp,255,"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.%s",szFileExtension);
	m_cRegistryAccess->CreateNewKey(Temp, HKEY_CURRENT_USER);
	m_cRegistryAccess->SetKeyValue(HKEY_CURRENT_USER,Temp,"Application","regards.exe");
	delete m_cRegistryAccess;
	return 0;
}

LRESULT CAssociation::OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CheckDlgButton(hDlg,IDC_CKGIF,BST_CHECKED);
	CheckDlgButton(hDlg,IDC_CKPCX,BST_CHECKED);
	CheckDlgButton(hDlg,IDC_CKPNG,BST_CHECKED);
	CheckDlgButton(hDlg,IDC_CKTGA,BST_CHECKED);
	CheckDlgButton(hDlg,IDC_CKJPEG,BST_CHECKED);
	CheckDlgButton(hDlg,IDC_CKTIFF,BST_CHECKED);
	CheckDlgButton(hDlg,IDC_CKBMP,BST_CHECKED);
	CheckDlgButton(hDlg,IDC_CKPCD,BST_CHECKED);
	CheckDlgButton(hDlg,IDC_CKPSD,BST_CHECKED);
	CheckDlgButton(hDlg,IDC_CKJP2,BST_CHECKED);
	CheckDlgButton(hDlg,IDC_CKAVI,BST_CHECKED);
	CheckDlgButton(hDlg,IDC_CKMPG,BST_CHECKED);
	CheckDlgButton(hDlg,IDC_CK3DS,BST_CHECKED);
	CheckDlgButton(hDlg,IDC_CKPPM,BST_CHECKED);
	CheckDlgButton(hDlg,IDC_CKPGM,BST_CHECKED);
	CheckDlgButton(hDlg,IDC_CKCRW,BST_CHECKED);
	return 0;
}

LRESULT CAssociation::OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId,wmEvent;
	wmId    = LOWORD(wParam); 
	wmEvent = HIWORD(wParam); 
	// Parse the menu selections:

	switch (wmId)
	{
		case IDOK:
			{
				AssociateRegards();
				if(IsDlgButtonChecked(hDlg,IDC_CKGIF) == BST_CHECKED)
					AssociateFileExt((LPTSTR)"gif");

				if(IsDlgButtonChecked(hDlg,IDC_CKPCX) == BST_CHECKED)
					AssociateFileExt((LPTSTR)"pcx");

				if(IsDlgButtonChecked(hDlg,IDC_CKPNG) == BST_CHECKED)
					AssociateFileExt((LPTSTR)"png");

				if(IsDlgButtonChecked(hDlg,IDC_CKTGA) == BST_CHECKED)
					AssociateFileExt((LPTSTR)"tga");

				if(IsDlgButtonChecked(hDlg,IDC_CKJPEG) == BST_CHECKED)
				{
					AssociateFileExt((LPTSTR)"jpe");
					AssociateFileExt((LPTSTR)"jpeg");
					AssociateFileExt((LPTSTR)"jpg");
				}

				if(IsDlgButtonChecked(hDlg,IDC_CKTIFF) == BST_CHECKED)
				{
					AssociateFileExt((LPTSTR)"tif");
					AssociateFileExt((LPTSTR)"tiff");
				}

				if(IsDlgButtonChecked(hDlg,IDC_CKBMP) == BST_CHECKED)
					AssociateFileExt((LPTSTR)"bmp");

				if(IsDlgButtonChecked(hDlg,IDC_CKPCD) == BST_CHECKED)
					AssociateFileExt((LPTSTR)"pcd");

				if(IsDlgButtonChecked(hDlg,IDC_CKPSD) == BST_CHECKED)
				{
					AssociateFileExt((LPTSTR)"psd");
					AssociateFileExt((LPTSTR)"pdd");
				}

				if(IsDlgButtonChecked(hDlg,IDC_CKJP2) == BST_CHECKED)
				{
					AssociateFileExt((LPTSTR)"jp2");
					AssociateFileExt((LPTSTR)"jpc");
				}

				if(IsDlgButtonChecked(hDlg,IDC_CKMPG) == BST_CHECKED)
				{
					AssociateFileExt((LPTSTR)"mpg");
					AssociateFileExt((LPTSTR)"mpv");
					AssociateFileExt((LPTSTR)"mv2");
				}

				if(IsDlgButtonChecked(hDlg,IDC_CKAVI) == BST_CHECKED)
				{
					AssociateFileExt((LPTSTR)"avi");
				}

				if(IsDlgButtonChecked(hDlg,IDC_CK3DS) == BST_CHECKED)
				{
					AssociateFileExt((LPTSTR)"3ds");
				}

				if(IsDlgButtonChecked(hDlg,IDC_CKPPM) == BST_CHECKED)
				{
					AssociateFileExt((LPTSTR)"ppm");
				}

				if(IsDlgButtonChecked(hDlg,IDC_CKPGM) == BST_CHECKED)
				{
					AssociateFileExt((LPTSTR)"pgm");
				}

				if(IsDlgButtonChecked(hDlg,IDC_CKCRW) == BST_CHECKED)
				{
					//Raw Format Association IDS_STGRAWFILE
					char cTemp[255];
					LoadString(hInstance,IDS_STGRAWFILE,cTemp,255);
					string m_stgData = cTemp;
					int m_iIndex = 0;
					try
					{
						while((m_iIndex = m_stgData.find(";",m_iIndex)) != -1)
						{
							if(m_stgData.length() > (m_iIndex + 5))
							{
								string m_stgFormat = m_stgData.substr(m_iIndex,5);
								m_iIndex++;
								strncpy_s(cTemp,255,m_stgFormat.substr(2,3).c_str(),3);
								cTemp[3] = '\0';
								AssociateFileExt(cTemp);
							}
							else
								break;
						}
					}
					catch(...)
					{
					}
				}

				EndDialog(hDlg, LOWORD(wParam));
			}
			break;

		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			break;
	}
	return 0;
}