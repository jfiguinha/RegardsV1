

#ifndef __CSQLFINDALBUMICONEFILE__
#define __CSQLFINDALBUMICONEFILE__
#include <iconefile.h>
#include "SqlExecuteRequest.h"

namespace Regards
{
	namespace Sqlite
	{
		class CSqlFindAlbumIconeFile : public CSqlExecuteRequest
		{
		public:
			CSqlFindAlbumIconeFile(void);
			~CSqlFindAlbumIconeFile(void);

			HRESULT SearchIconeFileAlbum(IconeFileVector * iconeFileVector, TCHAR * lpSQLRequestCriterium, TCHAR * lpSQLRequest, TCHAR * lpSpecialLiaison);

		private:

			int TraitementResult(CSqlResult * sqlResult);

			IconeFileVector * m_iconeFileVector;

		};
	}
}

#endif