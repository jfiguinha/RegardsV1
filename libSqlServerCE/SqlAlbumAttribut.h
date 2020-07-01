

#ifndef __CSQLALBUMATTRIBUT___
#define __CSQLALBUMATTRIBUT___

#include "SqlExecuteRequest.h"
#include <Attribut.h>

namespace Regards
{
	namespace Sqlite
	{
		class CSqlAlbumAttribut : public CSqlExecuteRequest
		{
		public:
			CSqlAlbumAttribut(void);
			~CSqlAlbumAttribut(void);

			HRESULT SaveAlbumAttribut(AttributVector * attributVector);
			HRESULT LoadAlbumAttribut(AttributVector * attributVector);
			HRESULT DeleteAlbumAttribut();
			HRESULT DeleteAlbumAttribut(int iNumAttribut);

		private:

			int TraitementResult(CSqlResult * sqlResult);
			AttributVector * m_AttributVector;

		};
	}
}

#endif