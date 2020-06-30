// Gif.h: interface for the CGif class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIF_H__A6A62543_694E_445F_917F_B2A7F6A00CF9__INCLUDED_)
#define AFX_GIF_H__A6A62543_694E_445F_917F_B2A7F6A00CF9__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include <windows.h>
#include "IBitmap.h"

extern "C" 
{
#define DOEXTERNGIF

// Workaround for namespace clash. This has been fixed in newer versions
// of libungif.
#ifdef _WINDOWS
	#undef DrawText
#endif

#include "gif_lib.h"
}

class CGif  
{
public:
	void GetDimensions(const char * fileName,UINT *width,UINT *height);
	CGif();
	virtual ~CGif();

	void DumpScreen2RGB(BYTE * Buffer,
				   GifRowType *ScreenBuffer,
				   int ScreenWidth, int ScreenHeight);

	int ReadGif(CIBitmap * m_ciBitmap,const char * szFileName);
	
private:
	bool ValidGif(const char * szFileName);
	ColorMapObject * ColorMap;
};

#endif // !defined(AFX_GIF_H__A6A62543_694E_445F_917F_B2A7F6A00CF9__INCLUDED_)
