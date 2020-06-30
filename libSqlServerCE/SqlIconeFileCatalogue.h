

#ifndef __CSQLICONEFILECATALOG__
#define __CSQLICONEFILECATALOG__

#include <iconefile.h>
#include "SqlExecuteRequest.h"

namespace LIBSQLSERVERCE
{
	class CSqlIconeFileCatalogue : public CSqlExecuteRequest
	{
	public:
		CSqlIconeFileCatalogue(void);
		~CSqlIconeFileCatalogue(void);

		HRESULT SaveIconeFileCatalog(IconeFileVector * iconeFileVector, WCHAR * lpCatalogName);
		HRESULT LoadIconeFileCatalog(IconeFileVector * iconeFileVector, WCHAR * lpCatalogName);
		HRESULT DeleteIconeFileCatalog(WCHAR * lpCatalogName);

	private:

		virtual HRESULT TraitementResult(IRowset * & pIRowset,HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding);
		virtual HRESULT TraitementInsert(IRowset * & pIRowset,IRowsetChange * & pIRowsetChange, HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding);

		IconeFileVector * m_iconeFileVector;
		WCHAR _pwzCatalogName[256];
	};
}

#endif