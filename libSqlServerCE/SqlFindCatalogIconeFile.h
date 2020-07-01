

#ifndef __CSQLFINDCATALOGICONEFILE__
#define __CSQLFINDCATALOGICONEFILE__

#include <iconefile.h>
#include "SqlExecuteRequest.h"

namespace Regards
{
	namespace Sqlite
	{
		class CSqlFindCatalogIconeFile : public CSqlExecuteRequest
		{
		public:
			CSqlFindCatalogIconeFile(void);
			~CSqlFindCatalogIconeFile(void);
			HRESULT SearchIconeFileCatalog(IconeFileVector * iconeFileVector, TCHAR * lpSQLRequest);

		private:
			int TraitementResult(CSqlResult * sqlResult);
			IconeFileVector * m_iconeFileVector;
		};
	}
}

#endif