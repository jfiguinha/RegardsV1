
#ifndef __ABOUTDIALOG__H__
#define __ABOUTDIALOG__H__

#include "../libWindow/LocalDialog.h"

class CIBitmap;
class CMatrix;

class CWienerFilter : public CLocalDialog
{
public:
	CWienerFilter(void);
	~CWienerFilter(void);

	void Init(CIBitmap * Source);
	int GetReturnValue()
	{
		return m_iReturnValue;
	}

	CMatrix * GetMatrix()
	{
		return m_cMatrixFiltre;
	}

	double GetRho()
	{
		return m_rho;
	}

	int GetMethode()
	{
		return m_iMethode;
	}


private:

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnActivate(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnNotify(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	CIBitmap * m_ciBitmapTestBitmap;
	CIBitmap * m_ciBitmapOldBitmap;
	//char Temp[50];
	int m_iTailleFiltreL;
	int m_iTailleFiltreH;
	double m_rho;
	int m_iMethode;
	CMatrix * m_cMatrixFiltre;
	int m_iReturnValue;


};

#endif