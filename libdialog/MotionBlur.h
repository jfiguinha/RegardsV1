
#ifndef __MOTIONBLUR__H__
#define __MOTIONBLUR__H__

#include "../libWindow/LocalDialog.h"

class CIBitmap;

class CMotionBlur : public CLocalDialog
{
public:
	CMotionBlur(void);
	~CMotionBlur(void);

	void Init(CIBitmap * Source);
	
	inline int GetReturnValue()
	{
		return m_iReturnValue;
	}

	inline double GetSigma()
	{
		return m_dsigma;
	}

	inline double GetAngle()
	{
		return m_dAngle;
	}

	inline double GetRadius()
	{
		return m_dradius;
	}

private:

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnHScroll(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnActivate(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	CIBitmap * m_ciBitmapTestBitmap;
	CIBitmap * m_ciBitmapOldBitmap;
	
	HBITMAP hBitmap;

	int m_iReturnValue;

	double m_dradius;
	double m_dsigma;
	double m_dAngle;

	int m_iRadius;
	int m_iSigma;
	int m_iAngle;

};

#endif
