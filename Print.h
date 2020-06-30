// Print.h: interface for the CPrint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRINT_H__F08DD227_F277_46B1_A23B_64BF23C3E3B6__INCLUDED_)
#define AFX_PRINT_H__F08DD227_F277_46B1_A23B_64BF23C3E3B6__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#define DEFAUTPRINTDISTANCE 0.4f

class CMargeData;
class CIBitmap;
class CPrintPreviewData;

class CPrintPrv
{
public:
	BOOL ProcessPrint(LPVOID param, CMargeData * m_MargeData);
	BOOL PrintPreview(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL GetDefaultPrintName(TCHAR * * buffer, int &iOrientation);
	BOOL RedrawPrintPage(HWND hDlg, CIBitmap * m_ciBitmap,CMargeData * m_MargeData);
	BOOL PrintPreviewPaint(HWND hDlg,CPrintPreviewData * prtPrevData);
	BOOL DetermineRatio(CPrintPreviewData * prtPrevData);
	BOOL DPGetDefaultPrinter(LPTSTR pPrinterName, LPDWORD pdwBufferSize);
	BOOL DPSetDefaultPrinter(LPTSTR pPrinterName);
};

class CPrint  
{
public:
	CPrint();
	virtual ~CPrint();

	public:

	static BOOL _stdcall AbortProc(HDC hdcPrn, int iCode);
	static BOOL _stdcall PrintDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL ProcessPrint(LPVOID param, CMargeData * m_MargeData);
	static HRESULT DisplayPrintPropertySheet(HINSTANCE hInst,HWND &hWnd, CMargeData * m_MargeData, const int &iTypePrint = 0);  // Window that owns the property sheet
	static void DisplayPreview(HINSTANCE hInst,HWND hWnd, CMargeData * m_MargeData);

	static BOOL bUserAbort ;
	static HWND hDlgPrint ;
};

#endif // !defined(AFX_PRINT_H__F08DD227_F277_46B1_A23B_64BF23C3E3B6__INCLUDED_)
