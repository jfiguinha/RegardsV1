
#ifndef __DEFINESIZEICONE__H__
#define __DEFINESIZEICONE__H__

#include "../libWindow/LocalDialog.h"

class CDefineSizeIcone : public CLocalDialog
{
public:
	CDefineSizeIcone(void);
	~CDefineSizeIcone(void);

	int GetReturnCode()
	{
		return m_iReturnCode;
	}

	void SetParam(int iLargeur, int iHauteur)
	{
		m_iLargeur = iLargeur;
		m_iHauteur = iHauteur;
	}

	int GetLargeur()
	{
		return m_iLargeur;
	}

	int GetHauteur()
	{
		return m_iHauteur;
	}

private:
	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	int m_iReturnCode;
	int m_iLargeur;
	int m_iHauteur;
};

#endif