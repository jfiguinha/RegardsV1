

#ifndef __CSQLICONEFILEFOLDER__
#define __CSQLICONEFILEFOLDER__

#include <iconefile.h>
#include "SqlExecuteRequest.h"

namespace Regards
{
	namespace Sqlite
	{
		class CSqlIconeFileFolder : public CSqlExecuteRequest
		{
		public:
			CSqlIconeFileFolder(void);
			~CSqlIconeFileFolder(void);

			HRESULT SaveIconeFileFolder(IconeFileVector * iconeFileVector, TCHAR * lpFolderName);
			HRESULT LoadIconeFileFolder(IconeFileVector * iconeFileVector, TCHAR * lpFolderName);
			HRESULT DeleteIconeFileFolder(TCHAR * lpFolderName);

		private:

			int TraitementResult(CSqlResult * sqlResult);

			IconeFileVector * m_iconeFileVector;
			TCHAR _pwzDirName[256];

		};
	}
}

#endif