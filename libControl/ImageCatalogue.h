// ImageCatalogue.h: interface for the CImageCatalogue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGECATALOGUE_H__66D1E16D_E04F_4207_AD1E_1D5FCCA216AA__INCLUDED_)
#define AFX_IMAGECATALOGUE_H__66D1E16D_E04F_4207_AD1E_1D5FCCA216AA__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include "Catalogue1.h"
#include <IconeFile.h>

class CSADirReadLocal;
class CImageCataloguePrv;

class CImageCatalogue : public CCatalogue  
{
public:


	int CreerCatalogue(HWND hWnd, HWND hWndTree);
	CImageCatalogue();
	virtual ~CImageCatalogue();

	CAlbumData * RecupPtAlbum(const int &iNumAlbum);
	int InitCatalogue(HWND hWndTree, CTree * m_cTree);
	int LoadCatalogue();
	int SaveCatalogue(const char * m_cFilename = "catalogue.db");
	int DetermineNumAlbum(const char *szDirName);
	int Rechercher(HWND hWnd,IconeFileVector * * m_IconeFileRechercheLocal);
	int SuppAlbum(HWND hWndTree, CTree *m_cTree, const int &iNumAlbum);
	int MiseAJour(HWND hWndTree, const int &iNumAlbum);
	int DeleteRechercheFile(HWND hWnd,VECTITEM * m_vectItem);
	int CompacterFichierCatalogue();
	
#ifdef SQLSERVERCE
	int RecupPtAlbum(const int &iNumAlbum, IconeFileVector * & lpIconeFileFolder);

private:

	IconeFileVector m_iconeFileFolder;

#endif
};

#endif // !defined(AFX_IMAGECATALOGUE_H__66D1E16D_E04F_4207_AD1E_1D5FCCA216AA__INCLUDED_)
