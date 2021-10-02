// WndNavigueBitmap.h: interface for the CWndNavigueBitmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDNAVIGUEBITMAP_H__95091752_4FB0_43E4_8F8B_5B488CD01D03__INCLUDED_)
#define AFX_WNDNAVIGUEBITMAP_H__95091752_4FB0_43E4_8F8B_5B488CD01D03__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include "WndBitmap.h"

class CDiaporama;
class CLoadImage;
class CMargeData;
class CDefineRepMonitor;
class CParameter;

class CWndNavigueBitmap  : public CWndBitmap 
{
public:
	void Windows98Update(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	CWndNavigueBitmap();
	virtual ~CWndNavigueBitmap();
	//ATOM MyRegisterClass(HINSTANCE hInstance);

	LRESULT OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnEraseBackground(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseWheel(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnHScroll(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnVScroll(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	//LRESULT OnMeasureItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	//LRESULT OnDrawItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnTimer(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnRButtonDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnDblClick(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	


private:

	//static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void LoadingImageFile(CIBitmap * m_ciBitmap,HWND hWnd, char * cFile,CBitmap &m_WndBitmap);
	
	void AffichageImage(CIBitmap * m_ciBitmap,HWND hWnd, char * cFile,CBitmap &m_WndBitmap);
	void SetHauteQualitePreview(HWND hWnd, int iQualite, int iChoix);
	static ULONG __stdcall AffichageMiniature(LPVOID parametre);
	static ULONG _stdcall MonitorDirectoryProc(LPVOID pvThis);
	static ULONG _stdcall ProcessTransitionEffect(LPVOID pvThis);

	BOOL MonitorThreadProc(HWND hWnd, char * cDirectory);
	void CloseMonitor();
	void GetOptionDiaporama();
	//void TestOrientation(HWND hWnd, CIBitmap * ciBitmap);
	void OuvrirVideo();
	void OuvrirImage();
	LRESULT ShowInfos(HWND hWnd, HDC hDC);

	int iTrie;
	HANDLE hThread; 
	CMargeData * m_MargeData;
	DWORD dwThreadTraitementId; 
	CDiaporama * m_cDiaporama;
	CLoadImage * m_ChargementImage;
	CDefineRepMonitor * m_define;
	DWORD dwThreadIdMonitor; 
	HANDLE hThreadMonitor;
	CParameter * m_cParameter;
	bool m_bTransition;
	
};

#endif // !defined(AFX_WNDNAVIGUEBITMAP_H__95091752_4FB0_43E4_8F8B_5B488CD01D03__INCLUDED_)
