// Fichier.h: interface for the CFichier class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FICHIER_H__186C6F21_9B20_44B8_B189_2E5DBBE07EFE__INCLUDED_)
#define AFX_FICHIER_H__186C6F21_9B20_44B8_B189_2E5DBBE07EFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include "catalogue1.h"
#include "SADirRead.h"

class CFichier  
{
public:
	void DefinirListeImage(const char *szFileName, CCatalogue::IconeFileVector * m_IconeFileVector);
	void AffichageImageDerniere(TCHAR *szFileName);
	void AffichageImagePremiere(TCHAR *szFileName);
	int GetNbFiles();
	void DefinirRepertoire(const char * szFileName, const int m_iTypeTrie = 0);
	void DefinirRepertoire(const char *szFileName, const int m_iTypeTrie, const char * m_cExtension);

	int SetTrieFichier(int m_iTypeTrie);
	int ChargerFichier(TCHAR * szFileName, HWND &hwnd);
	CFichier();
	virtual ~CFichier();
	int AffichageImageSuivante(TCHAR *szFileName);
	int AffichageImagePrecedente(TCHAR *szFileName);
	int SauverFichier(TCHAR *szFileName, HBITMAP hbmp, HWND &hwnd, const HINSTANCE &hInst = NULL);
	int DeleteDirectory(LPCTSTR lpszName);
	int SupprimerFichier(TCHAR *szFileName);

private:

	static LRESULT CALLBACK JpegSaving(HWND, UINT, WPARAM, LPARAM);
	BOOL TakeOwnership(LPCTSTR lpszFile);
	BOOL SetPrivilege(HANDLE hToken,LPCTSTR lpszPrivilege,BOOL bChange);
	BOOL SetPermission(LPCTSTR lpszFile, LPCTSTR lpszAccess, DWORD dwAccessMask);


	CSADirRead m_cCSADirRead;
	CSADirRead::SAFileVector files;
	CSADirRead::SAFileVector::iterator fit;

	CCatalogue::IconeFileVector * m_IconeFileVector;
	CCatalogue::IconeFileVector::iterator Liste;

	TCHAR dir[_MAX_DIR];
	bool m_bAscendant;
	static long lQuality;
	int iDataType;
};

#endif // !defined(AFX_FICHIER_H__186C6F21_9B20_44B8_B189_2E5DBBE07EFE__INCLUDED_)
