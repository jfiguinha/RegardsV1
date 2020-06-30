

#ifndef __CSQLEXECUTEREQUEST__
#define __CSQLEXECUTEREQUEST__

namespace LIBSQLSERVERCE
{
	class CSqlLib;

	class CSqlExecuteRequest
	{
	public:

		CSqlExecuteRequest(void);
		~CSqlExecuteRequest(void);
		HRESULT ExecuteRequest(WCHAR * lpRequestSQL);
		HRESULT ExecuteInsert();
		HRESULT ExecuteRequestWithNoResult(WCHAR * lpRequestSQL);

	protected:

		void ViderChampsElement();
		void CopyChampsElement(WCHAR * * pszChamp);

		virtual HRESULT TraitementResult(IRowset * & pIRowset,HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding){ return S_OK;};
		virtual HRESULT TraitementInsert(IRowset * & pIRowset,IRowsetChange * & pIRowsetChange, HROW * prghRows, HACCESSOR hAccessor, DWORD dwOffset, DBBINDING	*prgBinding){ return S_OK;};

		WCHAR * * pwszTableResult;
		int _iNbElement;
		WCHAR pwzTableName[255];
		CSqlLib * _sqlLib;
	};
}

#endif