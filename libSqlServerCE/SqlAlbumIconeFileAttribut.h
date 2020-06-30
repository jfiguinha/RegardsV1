

#ifndef __CSQLALBUMICONEFILEATTRIBUT__
#define __CSQLALBUMICONEFILEATTRIBUT__

#include <attribut.h>
#include "SqlExecuteRequest.h"

namespace LIBSQLSERVERCE
{
	class CSqlAlbumIconeFileAttribut : public CSqlExecuteRequest
	{
	public:
		CSqlAlbumIconeFileAttribut(void);
		~CSqlAlbumIconeFileAttribut(void);

		HRESULT SaveAlbumIconeFileAttribut(AttributVector * attributVector, WCHAR * lpAlbumName, WCHAR * lpFileName);
		HRESULT LoadAlbumIconeFileAttribut(AttributVector * attributVector, WCHAR * lpAlbumName, WCHAR * lpFileName);
		HRESULT DeleteAlbumIconeFileAttribut(WCHAR * lpAlbumName, WCHAR * lpFileName);
		HRESULT DeleteAlbumIconeFileAttribut(WCHAR * lpAlbumName);
		HRESULT DeleteAlbumIconeFileAttribut(int iNumAttribut);
		HRESULT UpdateAlbum(WCHAR * lpNewAlbumName,WCHAR * lpOldAlbumName);

	private:

		virtual HRESULT TraitementResult(IRowset * & pIRowset,HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding);
		virtual HRESULT TraitementInsert(IRowset * & pIRowset,IRowsetChange * & pIRowsetChange, HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding);

		AttributVector * m_attributVector;
		WCHAR * _lpAlbumName;
		WCHAR * _lpFileName;
	};

}
#endif