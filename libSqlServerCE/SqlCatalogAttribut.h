

#ifndef __CSQLCATALOGATTRIBUT__
#define __CSQLCATALOGATTRIBUT__

#include "SqlExecuteRequest.h"
#include <Attribut.h>

namespace Regards
{
	namespace Sqlite
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

			int TraitementResult(CSqlResult * sqlResult);

			AttributVector * m_AttributVector;
		};
	}
}

#endif