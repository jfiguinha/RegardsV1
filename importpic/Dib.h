// Dib.h: interface for the CDib class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIB_H__15CC8101_0600_43E5_8F7B_B430ED56AE8B__INCLUDED_)
#define AFX_DIB_H__15CC8101_0600_43E5_8F7B_B430ED56AE8B__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include <windows.h>
#include "ibitmap.h"

class CDib  
{
public:
	void GetDimensions(const char * fileName,UINT *width,UINT *height);
	CDib();
	virtual ~CDib();
	HANDLE OpenDIB (LPSTR szFile);
	HANDLE ReadDibBitmapInfo (INT fh);
	BOOL DibInfo(HANDLE hbi,  LPBITMAPINFOHEADER lpbi);
	HPALETTE CreateDibPalette (HANDLE hbi);
	WORD PaletteSize (VOID FAR * pv);
	HPALETTE CreateBIPalette (LPBITMAPINFOHEADER lpbi);
	HANDLE DibFromBitmap (const CIBitmap &m_ciBitmap, DWORD biStyle, WORD biBits,  HPALETTE  hpal);
	HBITMAP BitmapFromDib (HANDLE hdib,HPALETTE hpal);
	HANDLE DDBToDIB(const CIBitmap &m_ciBitmap, DWORD dwCompression);


private:
	VOID WriteMapFileHeaderandConvertFromDwordAlignToPacked(HFILE fh, LPBITMAPFILEHEADER pbf);
	VOID ReadBitMapFileHeaderandConvertToDwordAlign(HFILE fh, LPBITMAPFILEHEADER pbf, LPDWORD lpdwoff);
	DWORD PASCAL lread (INT fh,VOID FAR *pv, DWORD ul);
	DWORD PASCAL lwrite (INT fh,VOID FAR *pv, DWORD ul);
	WORD DibNumColors (VOID FAR * pv);
};

#endif // !defined(AFX_DIB_H__15CC8101_0600_43E5_8F7B_B430ED56AE8B__INCLUDED_)
