#ifndef __DIAPORAMA__H__
#define __DIAPORAMA__H__

#include "LocalDialog.h"

class CParameter;

class CDiaporama : public CLocalDialog
{
public:
	CDiaporama(HINSTANCE hInstance);
	~CDiaporama();

private:

	HINSTANCE m_hLocalInstance;
	TCHAR cDuree[50];
	int wmId, wmEvent;
	HWND hWndAffichage;
	int iSequence; // 0 : Pour Avant // 1: Pour Arriere
	bool m_bShrink; //Variable pour savoir si l'image sera pleine écran
	int iDuree;
	bool m_bFullScreen;
	int m_bHauteQualite;
	bool m_b3D;
	int m_i3DQuality;

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnHScroll(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

};

#endif