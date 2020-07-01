

#ifndef __CSQLICONEFILEALBUM__
#define __CSQLICONEFILEALBUM__

#include <iconefile.h>
#include "SqlExecuteRequest.h"

namespace Regards
{
	namespace Sqlite
	{
		class CSqlIconeFileAlbum : public CSqlExecuteRequest
		{
		public:
			CSqlIconeFileAlbum(void);
			~CSqlIconeFileAlbum(void);

			HRESULT SaveIconeFileAlbum(IconeFileVector * iconeFileVector, TCHAR * lpAlbumName);
			HRESULT LoadIconeFileAlbum(IconeFileVector * iconeFileVector, TCHAR * lpAlbumName);
			HRESULT LoadIconeFileAlbum(IconeFileVector * iconeFileVector);
			HRESULT DeleteIconeFileAlbum(TCHAR * lpAlbumName);
			HRESULT DeleteIconeFileAlbum(TCHAR * lpAlbumName, TCHAR * lpFilename);
			HRESULT UpdateAlbum(TCHAR * lpNewAlbumName, TCHAR * lpOldAlbumName);

		private:

			int TraitementResult(CSqlResult * sqlResult);

			IconeFileVector * m_iconeFileVector;
			TCHAR _pwzAlbumName[256];
		};
	}
}

#endif