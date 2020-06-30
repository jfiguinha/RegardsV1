

#ifndef __CSQLLIB__
#define __CSQLLIB__

namespace LIBSQLSERVERCE
{

	class CSqlLib
	{
	public:
		CSqlLib(void);
		~CSqlLib(void);

		HRESULT InitDatabase(LPCWSTR lpFilename);
		HRESULT CompactDatabase();
		
		HRESULT ExecuteSQL(WCHAR * pwszQuery, IRowset* & pIRowset);
		HRESULT ExecuteSQLWithNoResult(WCHAR * pwszQuery);

		IDBCreateSession * GetSession()
		{
			return m_pIDBCreateSession;
		};

		BOOL GetColumnOrdinal(DBCOLUMNINFO* pDBColumnInfo, DWORD dwNumCols, WCHAR* pwszColName, DWORD* pOrdinal, DWORD* pColumn);

	private:

		HRESULT ExecuteSQL(ICommandText *pICmdText, WCHAR * pwszQuery);
		HRESULT OpenDatabase();
		HRESULT CreateDatabase();
		HRESULT KillSession();

		BOOL pSuccess;
		WCHAR m_wFilename[MAX_PATH];

	protected:
		IDBCreateSession *m_pIDBCreateSession;  // The IDBCreateSession interface
		HMODULE sql_dll;
		CComPtr<IClassFactory> classfactory;
	};

}

#endif