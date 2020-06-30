// Tif.h: interface for the CTif class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIF_H__5CD669D4_5E20_4C38_B512_C5FFDCA4E562__INCLUDED_)
#define AFX_TIF_H__5CD669D4_5E20_4C38_B512_C5FFDCA4E562__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

//#include <windows.h>
#include "tiffio.h"
//#include "IBitmap.h"

class CIBitmap;

class CTif  
{
public:
	void GetDimensions(const char * fileName,UINT *width,UINT *height);
	int ReadTif(CIBitmap * m_ciBitmap,const char * szFileName);
	bool SaveTif(CIBitmap &m_ciBitmap,const char * szFileName,const bool &bCompress,const int &iJpegQuality);

	CTif();
	virtual ~CTif();

private:

	bool EncodeBody(TIFF *m_tif, CIBitmap &m_ciBitmap,const bool &bCompress,const int &iJpegQuality);

};

#endif // !defined(AFX_TIF_H__5CD669D4_5E20_4C38_B512_C5FFDCA4E562__INCLUDED_)
