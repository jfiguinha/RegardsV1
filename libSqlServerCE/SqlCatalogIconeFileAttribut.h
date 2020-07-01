

#ifndef __CSQLCATALOGICONEFILEATTRIBUT__
#define __CSQLCATALOGICONEFILEATTRIBUT__

#include <attribut.h>
#include "SqlExecuteRequest.h"

namespace Regards
{
	namespace Sqlite
	{
		class CSqlCatalogIconeFileAttribut : public CSqlExecuteRequest
		{
		public:
			CSqlCatalogIconeFileAttribut(void);
			~CSqlCatalogIconeFileAttribut(void);

			HRESULT SaveCatalogIconeFileAttribut(AttributVector * attributVector, TCHAR * lpCatalogName, int NumFile);
			HRESULT LoadCatalogIconeFileAttribut(AttributVector * attributVector, TCHAR * lpCatalogName, int NumFile);
			HRESULT DeleteCatalogIconeFileAttribut(TCHAR * lpAlbumName, int NumFile);

		private:

			int TraitementResult(CSqlResult * sqlResult);

			AttributVector * m_attributVector;
			TCHAR * _lpCatalogName;
			int _iNumFile;
		};
	}
}

#endif