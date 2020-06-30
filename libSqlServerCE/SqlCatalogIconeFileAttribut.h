

#ifndef __CSQLCATALOGICONEFILEATTRIBUT__
#define __CSQLCATALOGICONEFILEATTRIBUT__

#include <attribut.h>
#include "SqlExecuteRequest.h"


namespace LIBSQLSERVERCE
{
	class CSqlCatalogIconeFileAttribut : public CSqlExecuteRequest
	{
	public:
		CSqlCatalogIconeFileAttribut(void);
		~CSqlCatalogIconeFileAttribut(void);

		HRESULT SaveCatalogIconeFileAttribut(AttributVector * attributVector, WCHAR * lpCatalogName, int NumFile);
		HRESULT LoadCatalogIconeFileAttribut(AttributVector * attributVector, WCHAR * lpCatalogName, int NumFile);
		HRESULT DeleteCatalogIconeFileAttribut(WCHAR * lpAlbumName, int NumFile);

	private:

		virtual HRESULT TraitementResult(IRowset * & pIRowset,HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding);
		virtual HRESULT TraitementInsert(IRowset * & pIRowset,IRowsetChange * & pIRowsetChange, HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding);

		AttributVector * m_attributVector;
		WCHAR * _lpCatalogName;
		int _iNumFile;
	};
}

#endif