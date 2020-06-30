

#ifndef __CSQLFINDCATALOGICONEFILE__
#define __CSQLFINDCATALOGICONEFILE__

#include <iconefile.h>
#include "SqlExecuteRequest.h"

namespace LIBSQLSERVERCE
{
	class CSqlFindCatalogIconeFile : public CSqlExecuteRequest
	{
	public:
		CSqlFindCatalogIconeFile(void);
		~CSqlFindCatalogIconeFile(void);
		HRESULT SearchIconeFileCatalog(IconeFileVector * iconeFileVector, WCHAR * lpSQLRequest);

	private:

		virtual HRESULT TraitementResult(IRowset * & pIRowset,HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding);
		IconeFileVector * m_iconeFileVector;
	};
}

#endif