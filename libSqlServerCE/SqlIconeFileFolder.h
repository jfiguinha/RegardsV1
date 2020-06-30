

#ifndef __CSQLICONEFILEFOLDER__
#define __CSQLICONEFILEFOLDER__

#include <iconefile.h>
#include "SqlExecuteRequest.h"

namespace LIBSQLSERVERCE
{
	class CSqlIconeFileFolder : public CSqlExecuteRequest
	{
	public:
		CSqlIconeFileFolder(void);
		~CSqlIconeFileFolder(void);

		HRESULT SaveIconeFileFolder(IconeFileVector * iconeFileVector, WCHAR * lpFolderName);
		HRESULT LoadIconeFileFolder(IconeFileVector * iconeFileVector, WCHAR * lpFolderName);
		HRESULT DeleteIconeFileFolder(WCHAR * lpFolderName);

	private:

		virtual HRESULT TraitementResult(IRowset * & pIRowset,HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding);
		virtual HRESULT TraitementInsert(IRowset * & pIRowset,IRowsetChange * & pIRowsetChange, HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding);

		IconeFileVector * m_iconeFileVector;
		WCHAR _pwzDirName[256];

	};
}

#endif