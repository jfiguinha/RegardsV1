// Ppm.h: interface for the CPpm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PPM_H__F9274501_BFF7_456E_B35C_A62114AB77DC__INCLUDED_)
#define AFX_PPM_H__F9274501_BFF7_456E_B35C_A62114AB77DC__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

class CIBitmap;

#define uint32 long
class CPpm  
{
public:
	CPpm();
	virtual ~CPpm();

	int ReadPPM(CIBitmap * m_ciBitmap, const char *szFileName);
	bool decodePPM(CIBitmap * m_ciBitmap,const string &data,char * m_cData);
	void GetDimensions(const char *szFileName, UINT *width, UINT *height);
};

#endif // !defined(AFX_PPM_H__F9274501_BFF7_456E_B35C_A62114AB77DC__INCLUDED_)
