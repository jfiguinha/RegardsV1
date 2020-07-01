

#ifndef __CSQLICONEFILECATALOG__
#define __CSQLICONEFILECATALOG__

#include <iconefile.h>
#include "SqlExecuteRequest.h"

namespace Regards
{
	namespace Sqlite
	{
		class CSqlIconeFileCatalogue : public CSqlExecuteRequest
		{
		public:
			CSqlIconeFileCatalogue(void);
			~CSqlIconeFileCatalogue(void);

			HRESULT SaveIconeFileCatalog(IconeFileVector * iconeFileVector, TCHAR * lpCatalogName);
			HRESULT LoadIconeFileCatalog(IconeFileVector * iconeFileVector, TCHAR * lpCatalogName);
			HRESULT DeleteIconeFileCatalog(TCHAR * lpCatalogName);

		private:

			int TraitementResult(CSqlResult * sqlResult);

			IconeFileVector * m_iconeFileVector;
			TCHAR _pwzCatalogName[256];
		};
	}
}

#endif