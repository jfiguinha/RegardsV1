

#ifndef __ALBUMCRITERENOTIFY__H__
#define __ALBUMCRITERENOTIFY__H__

class CEditWndProc;

class CAlbumCritereNotify
{
public:
	CAlbumCritereNotify(HINSTANCE hInstance);
	~CAlbumCritereNotify(void);

	void SetParam(int iSelectItem, const int iNumNotEdit)
	{
		m_iSelectItem = iSelectItem;
		m_iNumNotEdit = iNumNotEdit;
	}

	void TraitementMessage(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:

	typedef struct tagEDIT
	{
		int editRow;
		int editCol;
		int iKey;
		WNDPROC origEditWndProc;
	}EDITDATA, * LPEDITDATA;

	LRESULT OnKeyDown(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnClick(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDblClick(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnBeginLabelEdit(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnEndLabelEdit(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	static LRESULT CALLBACK EditWndProc(HWND hwnd, UINT mMsg, WPARAM wParam, LPARAM lParam);

	LVITEM m_LVItem;
	char Libelle[255];
	HWND hEdit;
	HWND hLV;
	int m_iSelectItem;
	int m_iNumNotEdit;
	static EDITDATA m_EditData;
	HINSTANCE hInstanceLocal;
};

#endif