#pragma once

class CEmployees
{
public:
	CEmployees(void);
	~CEmployees(void);

	HWND Create(HWND hWndParent, HINSTANCE hInstance);
	HRESULT CreateDatabase();
	HRESULT ExecuteSQL(ICommandText *pICmdText, WCHAR * pwszQuery);
	HRESULT OpenDatabase();

private:
	IDBCreateSession *m_pIDBCreateSession;  // The IDBCreateSession interface
	HMODULE sql_dll;
	CComPtr<IClassFactory> classfactory;
};
