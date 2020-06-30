

#ifndef __CSQLALBUMUTILTIY___
#define __CSQLALBUMUTILTIY___

#include <albumdata.h>

namespace LIBSQLSERVERCE
{

	class CSqlAlbumUtility
	{
	public:
		CSqlAlbumUtility(void);
		~CSqlAlbumUtility(void);

		static LRESULT UpdateCategorie(AttributVector * attributVectorAlbum);
		static LRESULT UpdateFileAlbum(IconeFileVector * iconeFileVector, WCHAR * albumName);
		static LRESULT UpdateFileAttribut(AttributVector * attributVectorAlbum, WCHAR * albumName);
		static LRESULT DeleteAlbum(WCHAR * albumName);
		static LRESULT DeleteAlbumFile(WCHAR * albumName,WCHAR * fileName);
		static LRESULT SaveAlbum(CAlbumData * pAlbumData);
		static LRESULT RenameAlbum(WCHAR * albumOldName,WCHAR * albumNewName);
	};

}

#endif