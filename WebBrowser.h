// WebBrowser.h: interface for the CWebBrowser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WEBBROWSER_H__0A4AF544_BE92_4824_81A0_3BAE2ED89FAA__INCLUDED_)
#define AFX_WEBBROWSER_H__0A4AF544_BE92_4824_81A0_3BAE2ED89FAA__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include "libWindow/LocalWindow.h"
#include "iefunction.h"

class CWebBrowser : public CLocalWindow, public CIEFunction
{
public:

	LRESULT OnCommand(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	//LRESULT OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void ProcessElementCollection(IHTMLElementCollection* pElemColl);

	int RemoveAllItem();
	long AddItem(const char * Value, const int &iNumAttribut);
	
	long LocalDisplayHTMLRessource();

	CWebBrowser();
	virtual ~CWebBrowser();

	
private:

	int GetRecherche(char * Value, int iSize);
	

};

#endif // !defined(AFX_WEBBROWSER_H__0A4AF544_BE92_4824_81A0_3BAE2ED89FAA__INCLUDED_)
