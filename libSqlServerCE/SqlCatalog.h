

#ifndef __CSQLCATALOG__
#define __CSQLCATALOG__

#include "SqlExecuteRequest.h"
#include <AlbumData.h>

namespace Regards
{
	namespace Sqlite
	{
		class CSqlCatalog : public CSqlExecuteRequest
		{
		public:
			CSqlCatalog(void);
			~CSqlCatalog(void);

			HRESULT SaveCatalog(AlbumDataVector * albumDataVector);
			HRESULT SaveCatalog(TCHAR * lpAlbumName);
			HRESULT LoadCatalog(AlbumDataVector * albumDataVector);
			HRESULT DeleteCatalog();

		private:

			int TraitementResult(CSqlResult * sqlResult);

			AlbumDataVector * m_AlbumDataVector;
			TCHAR * m_lpAlbumName;
		};
	}
}

#endif
