// Objet.h: interface for the CObjet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJET_H__B71D36FB_D2FC_4381_BC3A_79EE4C6788AE__INCLUDED_)
#define AFX_OBJET_H__B71D36FB_D2FC_4381_BC3A_79EE4C6788AE__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include <string>
using namespace std;
#include <Winuser.h>

class CObjetPrv;
class CObjetPrint;
class CSAFileEntry;
class CMargeData;
class CIconeFile;
class CIBitmap;

class CObjet  
{
public:

	static void ErreurInformation()
	{
		//Impossible d'allouer la mémoire fin du programme !
		MessageBox(NULL,(LPCSTR)"Erreur d'allocation mémoire", (LPCSTR)"Erreur",MB_ICONERROR);
		exit(0);		
	};

	void GetDiskSpaceInfos(const char * m_szFilename,char * cInfos,const int &iInfosSize);

	static int IsValidateFolder(const char *m_Directory);
	
	void CreateAssociation(HWND hWnd);

	int TestImage(const char * szFileNameMain, const bool &m_bAffichage = true);
	
	int TestImageFormat(const char * szFileName);

	int TestZipFormat(const char *szFileName);

	int TestVideoFormat(const char *szFileName);

	void GetInfosFile(const char * szFileName, char * cInfos,const int &iInfosSize);

	void GetFileName(const char * szFilePath, char * FileName,const int &iFileNameSize);

	int GetDirectory(HWND hWnd, char * cDir);

	static int TestRepertoire(const char * szFileName);

	int TestMultimediaFormat(const char *szFileName);

	int Test3DFormat(const char *szFileName);
	int TestGravureFormat(const char *szFileName);

	int TestCompressionFormat(const char *szFileName);

	static void GetVirtualDesktopRect(LPRECT lprc );

	void TestOrientation(CIBitmap * ciBitmap, const char * cFilename);


	CObjet();
	virtual ~CObjet();
	int GetSysImgIcon(string strTemp);
	int GetSysImgOpenIcon(string strTemp);
	void GetInfosTaille(const INT64 &lSize, char * cInfos,const int &iInfosSize);
	VOID APIENTRY DisplayContextMenu(HWND hwnd, POINT pt, HINSTANCE hInstance, const char * szMenuName, int iSubMenu);

	void SetWallpaper(const char * FileName, const char * cType, const char * cTile, HWND hWndTreeWindow);

	static int GetWindowsVersion();
	static int GetWindowsVersionAvance();

	void GetInfosDateFile(const char *szFileName, char *cInfos,const int &iInfosSize);
	void GetFileSize(const char *szFileName, char *cInfos,const int &iInfosSize);
	void GetFolderName(const char *szFilePath, char *FileName,const int &iFileNameSize);
	void GetFormatName(const int &iFormat, char * cName,const int &iNameSize);


	//BOOL MonitorThreadProc(HWND hWnd, char * cDirectory);

	static void AffichageMessage(UINT iStringText, UINT iStringCaption, const int &iOption = 0);

	static void CenterWindow(HWND hwndDlg);	

	static void StartWait();

	static void EndWait();

	void AlbumGestionAddItem(CSAFileEntry &fit,CIconeFile &m_IconeFile);
	void AlbumGestionAddItem(const char * m_stgFullPathFile,CIconeFile &m_IconeFile);

	static void UncheckMenuStatePreviewQuality(HWND hwnd);

	static LRESULT TestFilePresent(const char * m_szFilename);


	static int BSTRToLocal(LPTSTR pLocal, BSTR pWide, DWORD dwChars);
	static int LocalToBSTR(BSTR pWide, LPTSTR pLocal, DWORD dwChars);

private:

	static int _stdcall BrowseCallbackProc(HWND hWnd, UINT message, long wParam, LPARAM lParam);
	static HCURSOR hcurSave;
};


#endif // !defined(AFX_OBJET_H__B71D36FB_D2FC_4381_BC3A_79EE4C6788AE__INCLUDED_)
