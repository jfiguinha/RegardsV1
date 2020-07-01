

#ifndef __CSQLALBUMICONEFILEATTRIBUT__
#define __CSQLALBUMICONEFILEATTRIBUT__

#include <attribut.h>
#include "SqlExecuteRequest.h"

namespace Regards
{
	namespace Sqlite
	{
		class CSqlAlbumIconeFileAttribut : public CSqlExecuteRequest
		{
		public:
			CSqlAlbumIconeFileAttribut(void);
			~CSqlAlbumIconeFileAttribut(void);

			HRESULT SaveAlbumIconeFileAttribut(AttributVector * attributVector, TCHAR * lpAlbumName, TCHAR * lpFileName);
			HRESULT LoadAlbumIconeFileAttribut(AttributVector * attributVector, TCHAR * lpAlbumName, TCHAR * lpFileName);
			HRESULT DeleteAlbumIconeFileAttribut(TCHAR * lpAlbumName, TCHAR * lpFileName);
			HRESULT DeleteAlbumIconeFileAttribut(TCHAR * lpAlbumName);
			HRESULT DeleteAlbumIconeFileAttribut(int iNumAttribut);
			HRESULT UpdateAlbum(TCHAR * lpNewAlbumName, TCHAR * lpOldAlbumName);

		private:

			int TraitementResult(CSqlResult * sqlResult);

			AttributVector * m_attributVector;
			TCHAR * _lpAlbumName;
			TCHAR * _lpFileName;
		};
	}

}
#endif