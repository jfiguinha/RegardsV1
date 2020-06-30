

#ifndef __WEBINFOFICHIER__H__
#define __WEBINFOFICHIER__H__

#include "libWindow/LocalWindow.h"
#include "iefunction.h"

class Cexif;

class CWebInfoFichier  : public CLocalWindow, public CIEFunction
{
public:
	CWebInfoFichier(void);
	~CWebInfoFichier(void);

	LRESULT OnCommand(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnActivate(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	long LocalDisplayHTMLRessource();
	void ProcessElementCollection(IHTMLElementCollection* pElemColl);

private:

	HRESULT DefineCharInfo(IHTMLElement* pElem, char * cInfo);
	HRESULT DefineIntInfo(IHTMLElement* pElem, int iInfo);
	HRESULT DefineFloatInfo(IHTMLElement* pElem, float fInfo);
	long LocalDisplayEXIFHTMLRessource();
	void ProcessExifDocument();
	void ProcessExifDocument(IHTMLDocument2* pDoc);
	void ProcessExifElementCollection(IHTMLElementCollection* pElemColl);

	HWND hFocus;
	Cexif * m_cExif;
	int m_iType;
	string m_stgFichier;
};

#endif