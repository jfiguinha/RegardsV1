
#include "../libWindow/LocalDialog.h"

#ifndef __RESIZEWINDOW__H__
#define __RESIZEWINDOW__H__

class CIBitmap;
class CParameter;

class CResizeWindow : public CLocalDialog
{
public:
	CResizeWindow(void);
	~CResizeWindow(void);

	void SetIBitmap(CIBitmap * ciBitmap)
	{
		m_ciBitmap = ciBitmap;
	}

	int GetReturnValue()
	{
		return m_iReturnValue;
	}

	int GetWidth()
	{
		return m_iWidth;
	}

	int GetHeight()
	{
		return m_iHeight;
	}

	int GetFiltre()
	{
		return m_iFiltre;
	}

private:

	int IntToFiltre(int iNumPos);
	void InitDialogText(HWND hDlg);
	void SetFiltreItemString(HWND hDlg,int iFiltre, LPCTSTR lpValue);
	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	int m_iOriginalWidth, m_iOriginalHeight;
	int m_iWidth, m_iHeight;
	int m_iFiltre;
	int iCheckOn;
	double m_dRapport;
	int m_iNumFiltre;
	int iModify;

	int m_iReturnValue;

	CIBitmap * m_ciBitmap;
	CParameter * m_cParameter;
	HWND hWndParent;


};

#endif
