

#ifndef __CSQLFINDALBUMICONEFILE__
#define __CSQLFINDALBUMICONEFILE__
#include <iconefile.h>
#include "SqlExecuteRequest.h"

namespace LIBSQLSERVERCE
{
	class CSqlFindAlbumIconeFile : public CSqlExecuteRequest
	{
	public:
		CSqlFindAlbumIconeFile(void);
		~CSqlFindAlbumIconeFile(void);
		
		HRESULT SearchIconeFileAlbum(IconeFileVector * iconeFileVector, WCHAR * lpSQLRequestCriterium, WCHAR * lpSQLRequest, WCHAR * lpSpecialLiaison);

	private:

		virtual HRESULT TraitementResult(IRowset * & pIRowset,HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding);
		IconeFileVector * m_iconeFileVector;

	};
}

#endif