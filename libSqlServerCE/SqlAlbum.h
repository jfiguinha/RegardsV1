

#ifndef __CSQLALBUM___
#define __CSQLALBUM___

#include "SqlExecuteRequest.h"
#include <albumdata.h>

namespace Regards
{
	namespace Sqlite
	{
		class CSqlAlbum : public CSqlExecuteRequest
		{
		public:
			CSqlAlbum(void);
			~CSqlAlbum(void);

			HRESULT SaveAlbum(CAlbumData * pAlbumData);
			HRESULT LoadAlbum(AlbumDataVector * albumDataVector);
			HRESULT DeleteAlbum();
			HRESULT DeleteAlbum(TCHAR * lpAlbumName);
			HRESULT UpdateAlbum(TCHAR * lpNewAlbumName, TCHAR * lpOldAlbumName);

		private:

			int TraitementResult(CSqlResult * sqlResult);

			AlbumDataVector * m_AlbumDataVector;
			CAlbumData * m_AlbumData;
		};
	}
}

#endif