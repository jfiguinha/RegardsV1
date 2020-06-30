

#ifndef __CSQLICONEFILEALBUM__
#define __CSQLICONEFILEALBUM__

#include <iconefile.h>
#include "SqlExecuteRequest.h"

namespace LIBSQLSERVERCE
{
	class CSqlIconeFileAlbum : public CSqlExecuteRequest
	{
	public:
		CSqlIconeFileAlbum(void);
		~CSqlIconeFileAlbum(void);

		HRESULT SaveIconeFileAlbum(IconeFileVector * iconeFileVector, WCHAR * lpAlbumName);
		HRESULT LoadIconeFileAlbum(IconeFileVector * iconeFileVector, WCHAR * lpAlbumName);
		HRESULT LoadIconeFileAlbum(IconeFileVector * iconeFileVector);
		HRESULT DeleteIconeFileAlbum(WCHAR * lpAlbumName);
		HRESULT DeleteIconeFileAlbum(WCHAR * lpAlbumName, WCHAR * lpFilename);
		HRESULT UpdateAlbum(WCHAR * lpNewAlbumName,WCHAR * lpOldAlbumName);

	private:

		virtual HRESULT TraitementResult(IRowset * & pIRowset,HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding);
		virtual HRESULT TraitementInsert(IRowset * & pIRowset,IRowsetChange * & pIRowsetChange, HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding);

		IconeFileVector * m_iconeFileVector;
		WCHAR _pwzAlbumName[256];
	};
}

#endif