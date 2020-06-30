

#ifndef __CSQLALBUM___
#define __CSQLALBUM___

#include "SqlExecuteRequest.h"
#include <albumdata.h>

namespace LIBSQLSERVERCE
{
	class CSqlAlbum : public CSqlExecuteRequest
	{
	public:
		CSqlAlbum(void);
		~CSqlAlbum(void);

		HRESULT SaveAlbum(CAlbumData * pAlbumData);
		HRESULT LoadAlbum(AlbumDataVector * albumDataVector);
		HRESULT DeleteAlbum();
		HRESULT DeleteAlbum(WCHAR * lpAlbumName);
		HRESULT UpdateAlbum(WCHAR * lpNewAlbumName,WCHAR * lpOldAlbumName);

	private:

		virtual HRESULT TraitementResult(IRowset * & pIRowset,HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding);
		virtual HRESULT TraitementInsert(IRowset * & pIRowset,IRowsetChange * & pIRowsetChange, HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding);

		AlbumDataVector * m_AlbumDataVector;
		CAlbumData * m_AlbumData;
	};
}

#endif