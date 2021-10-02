
#ifndef __MP3DIALOG__H__
#define __MP3DIALOG__H__

#include "CreateDialog.h"

class CMP3;
class CFichier;
class CMP3Input;

class CMP3Dialog : public CCreateDialog
{
public:
	CMP3Dialog(void);
	~CMP3Dialog(void);

private:


	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnHScroll(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void SecondToTime(const int &m_iSecond, char * szTime);
	void DrawPicture(HWND hDlg);

	CMP3 * m_cMP3;
	TCHAR szFileName[MAX_PATH];
	bool m_bPause;
	int m_iSeconde;
	CFichier * m_cFichier;
};

#endif