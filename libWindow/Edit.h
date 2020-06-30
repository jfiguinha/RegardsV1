// Edit.h: interface for the CEdit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EDIT_H__AE33726E_C90F_4851_A355_FDEF3F95CED8__INCLUDED_)
#define AFX_EDIT_H__AE33726E_C90F_4851_A355_FDEF3F95CED8__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include "LocalWindow.h"

class CEdit : public CLocalWindow   
{
public:
	void SetWindow(HWND hWnd);
	CEdit();
	virtual ~CEdit();

	int CreateEx(HWND hParent, DWORD dExStyle, DWORD dWndStyle, RECT rc);

	int GetWindowTextLength()
	{
		char cWindowText[1024];
		return GetWindowText(m_hWnd,cWindowText,1024);
	}

	int SetSel(int iStart,int iEnd)
	{
		SendMessage(m_hWnd,EM_SETSEL,iStart,iEnd);
		return 0;
	}
	
	int ReplaceSel(LPCTSTR pszOutput)		// add the message to the end of Edit control
	{
		SendMessage(m_hWnd,EM_REPLACESEL,0,(LPARAM)pszOutput);
		return 0;
	}

};

#endif // !defined(AFX_EDIT_H__AE33726E_C90F_4851_A355_FDEF3F95CED8__INCLUDED_)
