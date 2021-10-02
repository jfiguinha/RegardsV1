// WndBitmap.h: interface for the CWndBitmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDBITMAP_H__F001DC6A_6FA9_4FC9_9076_7DAFB1C87178__INCLUDED_)
#define AFX_WNDBITMAP_H__F001DC6A_6FA9_4FC9_9076_7DAFB1C87178__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include <LocalWindow.h>

class CNavigueData;
class CIBitmap;
#include <Regardsbitmap.h>
using namespace Regards::libbitmap;


class CWndBitmap : public CLocalWindow
{

public:

	void ZoomOut(HWND hWnd,LPARAM lParam);

	void SetFullScreen(bool bValue);
	void SetShrinkImage(bool bValue);

	void ZoomIn(HWND hWnd,LPARAM lParam);

	CWndBitmap();
	virtual ~CWndBitmap();

protected:
	//Opengl
	int MySetPixelFormat();
	void KillGLWindow();



	LRESULT ProcessCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT MouseAndKeyCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT EraseBgrCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT SizeCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void DeplacementCurseur(HWND &hWnd,RECT &rc, const long &m_lDeplacement, const bool &m_bType, LPARAM lParam, CBitmap * m_WndBitmap, CIBitmap * m_ciBitmap);
	void PositionnementImage(HWND &hwnd, DWORD lParam,CBitmap * m_WndBitmap, CIBitmap * m_ciBitmap);

	CNavigueData * m_cNavigueData;
	HGLRC           hRC;							// Permanent Rendering Context
	HDC             hDC;							// Private GDI Device Context

private:
	void ApplyZoom(HWND hWnd,int iXImage, int iYImage);
};

#endif // !defined(AFX_WNDBITMAP_H__F001DC6A_6FA9_4FC9_9076_7DAFB1C87178__INCLUDED_)
