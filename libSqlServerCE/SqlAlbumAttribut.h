

#ifndef __CSQLALBUMATTRIBUT___
#define __CSQLALBUMATTRIBUT___

#include "SqlExecuteRequest.h"
#include <Attribut.h>

namespace LIBSQLSERVERCE
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

		virtual HRESULT TraitementResult(IRowset * & pIRowset,HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding);
		virtual HRESULT TraitementInsert(IRowset * & pIRowset,IRowsetChange * & pIRowsetChange, HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding);

		AttributVector * m_AttributVector;

	};
}

#endif