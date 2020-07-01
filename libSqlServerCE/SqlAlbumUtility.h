

#ifndef __CSQLALBUMUTILTIY___
#define __CSQLALBUMUTILTIY___

#include <albumdata.h>

namespace Regards
{
	namespace Sqlite
	{
		class CSqlAlbumUtility
		{
		public:
			CSqlAlbumUtility(void);
			~CSqlAlbumUtility(void);

			static LRESULT UpdateCategorie(AttributVector * attributVectorAlbum);
			static LRESULT UpdateFileAlbum(IconeFileVector * iconeFileVector, TCHAR * albumName);
			static LRESULT UpdateFileAttribut(AttributVector * attributVectorAlbum, TCHAR * albumName);
			static LRESULT DeleteAlbum(TCHAR * albumName);
			static LRESULT DeleteAlbumFile(TCHAR * albumName, TCHAR * fileName);
			static LRESULT SaveAlbum(CAlbumData * pAlbumData);
			static LRESULT RenameAlbum(TCHAR * albumOldName, TCHAR * albumNewName);
		};
	}

}

#endif