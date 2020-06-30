// List.h: interface for the CList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIST_H__618357B4_EA7A_498A_85B6_A92258BC6232__INCLUDED_)
#define AFX_LIST_H__618357B4_EA7A_498A_85B6_A92258BC6232__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include "ListView.h"

class CList : public CListView
{
public:

	CList();
	virtual ~CList();

	void DefineAfficheImage(const bool &m_bVisible);
	void DefineStatusBar(CStatusBar * m_StatusBar);
	void DeleteItem(char * FileName);
	int AddItem(const char *FileName);

	HWND CreateListView (HWND hwndParent, HINSTANCE hInst, const char * szName, const int iShareImage = 0);
	VOID SetView(DWORD dwView);
	BOOL InitListViewImageLists();
	LRESULT List_Notify(LPARAM lParam, const int &iTypeData);
	HRESULT List_Refresh();
	void List_DoBackgroundMenu(LPPOINT pptScreen, IContextMenu2  *g_pcm2);

	void SetParent(HWND hWnd)
	{
		hParent = hWnd;
	};

	HWND GetParent()
	{
		return hParent;
	};
	//Nouvelle implémentation


protected:

	HWND hParent;
	HWND hWndMiniature;

};

#endif // !defined(AFX_LIST_H__618357B4_EA7A_498A_85B6_A92258BC6232__INCLUDED_)
