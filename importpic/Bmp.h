// Bmp.h: interface for the CBmp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BMP_H__0C2E582B_3BF3_444C_9B0D_4548BD25FA57__INCLUDED_)
#define AFX_BMP_H__0C2E582B_3BF3_444C_9B0D_4548BD25FA57__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

class CIBitmap;

class CBmp  
{
public:
	void GetJDimensions(const char * fileName,UINT *width,UINT *height);
	int SaveBMP(CIBitmap &m_ciBitmap, const char * filename);
	int ReadBmp(CIBitmap * m_ciBitmap, const char *szFileName);
	int ReadBmp2(CIBitmap * m_ciBitmap, const char *szFileName);
	CBmp();
	virtual ~CBmp();

};

#endif // !defined(AFX_BMP_H__0C2E582B_3BF3_444C_9B0D_4548BD25FA57__INCLUDED_)
