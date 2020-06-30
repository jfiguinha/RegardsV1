// WndAfficheBitmap.h: interface for the CWndAfficheBitmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDAFFICHEBITMAP_H__61F64BA3_1C17_4DC4_A5A5_8995139BF745__INCLUDED_)
#define AFX_WNDAFFICHEBITMAP_H__61F64BA3_1C17_4DC4_A5A5_8995139BF745__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include "WndBitmap.h"


class CLoadImage;
class CDessin;
class CMargeData;

class CWndAfficheBitmap : public CWndBitmap  
{
public:
	
	CWndAfficheBitmap();
	virtual ~CWndAfficheBitmap();
	//ATOM MyRegisterClass(HINSTANCE hInstance);

	LRESULT OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnEraseBackground(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnRButtonDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseWheel(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnVScroll(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnHScroll(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnDblClick(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnTimer(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void VerifModification();
	void Windows98Update(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:

	typedef struct tabBITMAP
	{
		CIBitmap * m_ciBitmap;
	}BITMAPDATA, * LPBITMAPDATA;

	inline void InitDessin();

	//static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//static int m_iAction;
	static UINT __stdcall  AffichageMiniature(LPVOID parametre);
	void AddBitmap();
	void RemoveBitmap();
	void VidageMemoire();
	void SetDessinRatio(const float &m_fRatio);



	int m_iAction;
	CLoadImage * m_ChargementImage;
	CDessin * m_cDessin;
	CMargeData * m_MargeData;
	LPBITMAPDATA m_lpBitmapData;
	HWND hWndLevel;
};

#endif // !defined(AFX_WNDAFFICHEBITMAP_H__61F64BA3_1C17_4DC4_A5A5_8995139BF745__INCLUDED_)
