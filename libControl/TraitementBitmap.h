// TraitementBitmap.h: interface for the CTraitementBitmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRAITEMENTBITMAP_H__BE370EF9_D50E_4780_9EF1_CDB1CBE37374__INCLUDED_)
#define AFX_TRAITEMENTBITMAP_H__BE370EF9_D50E_4780_9EF1_CDB1CBE37374__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000


#include <LocalWindow.h>
#include "twaincpp.h"

class CTraitementData;

class CTraitementBitmap : public CTwain, public CLocalWindow
{
public:



	void CopyImage(HWND hWnd, HANDLE hBitmap,TW_IMAGEINFO& info);

	int SetFilename(const char * m_szFilename);

	CTraitementBitmap();
	virtual ~CTraitementBitmap();
	//int CreateTraitementWindow(HWND hParent, HINSTANCE hInstance,int nCmdShow, const char * cFileName);

	int Run(int iAccelerator);

private:
	
	//Fonction du process
	LRESULT OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnMeasureItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnDrawItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseWheel(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnEraseBackground(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//HWND m_hWnd;
	TCHAR szWindowClass[MAX_PATH];
	TCHAR szTitle[MAX_PATH];
	TCHAR cFileName[MAX_PATH];
	CTraitementData * m_pTraitementData; 
	//CTraitementBitmapPrv * m_CTraitementBitmapPrv;
};

#endif // !defined(AFX_TRAITEMENTBITMAP_H__BE370EF9_D50E_4780_9EF1_CDB1CBE37374__INCLUDED_)
