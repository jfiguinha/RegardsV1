
#include "IExplore.h"

const SAFEARRAYBOUND ArrayBound = {1, 0};

class CIEFunction
{
public:

	typedef struct WEBBROWSER
	{
		Host * pHost;
		HTMLBrowserEvents2 * pSink;
	}WEBBROWSERDATA, * LPWEBBROWSER;


	CIEFunction(void);
	~CIEFunction(void);
	long DisplayHTMLStr(HWND hwnd, LPCTSTR strHtml, int iSize);
	long AttachEvent();
	void ProcessDocument(IHTMLDocument2* pDoc);
	virtual void ProcessElementCollection(IHTMLElementCollection* pElemColl){};
	long DisplayHTMLRessource(const char * szResourceName, int iSize);
	long Refresh();
	bool TestIfNewVersionAvailable(const char * szSiteAdress,const char * szVersion);

protected:

	WEBBROWSERDATA m_WEBBROWSER;

};
