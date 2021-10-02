

#ifndef __CGRAVEURFUNCTION__H__
#define __CGRAVEURFUNCTION__H__

class CShareWizDataGrav;
class CAspiDriver;

class CGraveurISOFunction
{
public:
	CGraveurISOFunction(void);
	~CGraveurISOFunction(void);

	LRESULT InitGraveurList(HWND hWnd,CAspiDriver * m_cAspiDriver,int * iTabIndice);
	LRESULT TestCDSpace(CAspiDriver * m_cAspiDriver,const INT64 &m_lIsoFileSize);
	LRESULT TestIsCDOK(CAspiDriver * m_cAspiDriver,const INT64 &m_lIsoFileSize,const int &iTypeGravure = 0);

	static ULONG __stdcall CreateIsoImage(LPVOID parametre);
	static ULONG __stdcall BurnIsoImage(LPVOID parametre);
	LRESULT TestIsISOToolsOK(CShareWizDataGrav * pdata);
	LRESULT TestIsBurningToolsOK(CShareWizDataGrav * pdata);
};

#endif