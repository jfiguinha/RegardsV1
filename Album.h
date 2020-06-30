// Album.h: interface for the CAlbum class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALBUM_H__F80771AC_B8BE_410D_B153_FEA628DA95B6__INCLUDED_)
#define AFX_ALBUM_H__F80771AC_B8BE_410D_B153_FEA628DA95B6__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

class CCatalogue;
class CTree;

class CAlbum : public CCatalogue  
{
public:
	CAlbum();
	virtual ~CAlbum();

	int InitCatalogue(HWND hWndTree, CTree * m_cTree);
	int DetermineNumAlbum(const char *szDirName);
	int DeleteAlbum(const char * szAlbumName);

	int LoadAlbum();
	int SaveAlbum();
};

#endif // !defined(AFX_ALBUM_H__F80771AC_B8BE_410D_B153_FEA628DA95B6__INCLUDED_)
