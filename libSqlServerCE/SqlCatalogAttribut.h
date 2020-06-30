

#ifndef __CSQLCATALOGATTRIBUT__
#define __CSQLCATALOGATTRIBUT__

#include "SqlExecuteRequest.h"
#include <Attribut.h>

namespace LIBSQLSERVERCE
{
	class CSqlCatalogAttribut : public CSqlExecuteRequest
	{
	public:
		CSqlCatalogAttribut(void);
		~CSqlCatalogAttribut(void);

		HRESULT SaveCatalogAttribut(AttributVector * attributVector);
		HRESULT LoadCatalogAttribut(AttributVector * attributVector);
		HRESULT DeleteCatalogAttribut();

	private:

		virtual HRESULT TraitementResult(IRowset * & pIRowset,HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding);
		virtual HRESULT TraitementInsert(IRowset * & pIRowset,IRowsetChange * & pIRowsetChange, HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding);

		AttributVector * m_AttributVector;
	};
}

#endif