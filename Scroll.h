// Scroll.h: interface for the CScroll class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCROLL_H__C42113EC_6425_4328_BFCB_524BA2AB1934__INCLUDED_)
#define AFX_SCROLL_H__C42113EC_6425_4328_BFCB_524BA2AB1934__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

//#include <windows.h>

class CScrollPrv;

class CScroll  
{
public:

	void DisabledScrollBar();
	int GetCurrentVScroll();
	int GetCurrentHScroll();
	void Hscroll(HWND hWnd,int Param,int Param2, int &fScroll);
	void Vscroll(HWND hWnd,int Param,int Param2, int &fScroll);
	void DefineSize(const int &xMax,const int &yMax,const int &xMin,const int &yMin);
	void Size(HWND &hWnd,const int &xNewSize, const int &yNewSize);
	void Initialize();
	void CreateScrollWindow(HWND hWndParent,HINSTANCE hInst);
	CScroll();
	virtual ~CScroll();

private:

	CScrollPrv * pimpl_;


};

#endif // !defined(AFX_SCROLL_H__C42113EC_6425_4328_BFCB_524BA2AB1934__INCLUDED_)
