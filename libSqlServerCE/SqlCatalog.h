

#ifndef __CSQLCATALOG__
#define __CSQLCATALOG__

#include "SqlExecuteRequest.h"
#include <AlbumData.h>

namespace LIBSQLSERVERCE
{
	class CSqlCatalog : public CSqlExecuteRequest
	{
	public:
		CSqlCatalog(void);
		~CSqlCatalog(void);

		HRESULT SaveCatalog(AlbumDataVector * albumDataVector);
		HRESULT SaveCatalog(WCHAR * lpAlbumName);
		HRESULT LoadCatalog(AlbumDataVector * albumDataVector);
		HRESULT DeleteCatalog();

	private:

		virtual HRESULT TraitementResult(IRowset * & pIRowset,HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding);
		virtual HRESULT TraitementInsert(IRowset * & pIRowset,IRowsetChange * & pIRowsetChange, HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding);

		AlbumDataVector * m_AlbumDataVector;
		WCHAR * m_lpAlbumName;
	};
}

#endif
