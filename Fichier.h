// Fichier.h: interface for the CFichier class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FICHIER_H__186C6F21_9B20_44B8_B189_2E5DBBE07EFE__INCLUDED_)
#define AFX_FICHIER_H__186C6F21_9B20_44B8_B189_2E5DBBE07EFE__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

//#include <windows.h>
//#include "catalogueheader.h"
//#include <ibitmap.h>

#include <IconeFile.h>
#include <SAFileEntry.h> 

class CSADirRead;
class CBitmap;
class CIBitmap;

class CFichier  
{
public:
	const int GetPos();
	void DefinirListeImage(const char *szFileName, IconeFileVector * m_IconeFileVector);
	void AffichageImageDerniere(char *szFileName, int iSize);
	void AffichageImagePremiere(char *szFileName, int iSize);
	void AffichageImagePosition(char *szFileName, int iSize, int iPos);

	int GetNbFiles();
	void DefinirRepertoire(const char * szFileName, const int &m_iTypeTrie = 0,const bool &m_bIncludeSubFolder = false,const bool &bRawFile = true);
	void DefinirRepertoire(const char *szFileName, const int m_iTypeTrie, const char * m_cExtension);

	int SetTrieFichier(const int & m_iTypeTrie);
	int ChargerFichier(char * szFileName,int iSize, HWND &hwnd);
	CFichier();
	virtual ~CFichier();
	int AffichageImageSuivante(char * szFileName,int iSize);
	int AffichageImagePrecedente(char * szFileName,int iSize);
	int SauverFichier(TCHAR *szFileName, CIBitmap &m_ciBitmap, HWND &hwnd, const HINSTANCE &hInst = NULL);
	int DeleteDirectory(LPCTSTR lpszName);
	int SupprimerFichier(char * szFileName,int iSize);
	void GetFichierTime(const char * m_stgFilePath, char * DateTime,const int &iDateTimeSize);

	int DeleteFolder(const char * m_szFolder);
	int DeleteFile(const char * m_szFilename);
	int DeleteDir(LPCTSTR lpszName);

	TCHAR * GetDirName()
	{
		return dir;
	}

private:
	BOOL TakeOwnership(LPCTSTR lpszFile);
	BOOL SetPrivilege(HANDLE hToken,LPCTSTR lpszPrivilege,BOOL bChange);
	BOOL SetPermission(LPCTSTR lpszFile, LPCTSTR lpszAccess, DWORD dwAccessMask);

	CSADirRead * m_cCSADirRead;
	SAFileVector files;
	SAFileVector::iterator fit;

	IconeFileVector * m_IconeFileVector;
	IconeFileVector::iterator Liste;

	TCHAR dir[_MAX_DIR];
	bool m_bAscendant;
	long lQuality;
	int iDataType;
	int iPos;
};

#endif // !defined(AFX_FICHIER_H__186C6F21_9B20_44B8_B189_2E5DBBE07EFE__INCLUDED_)
