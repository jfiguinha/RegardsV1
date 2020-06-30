// Preview.h: interface for the CPreview class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PREVIEW_H__07A461D5_7D0C_41D4_8187_7F266B3CF328__INCLUDED_)
#define AFX_PREVIEW_H__07A461D5_7D0C_41D4_8187_7F266B3CF328__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

//#include <windows.h>
#include "libWindow\LocalWindow.h"

class CIBitmap;
class CMP3Dialog;
class CInfosFichierProc;
class CPreviewData;
class CWMPHost;

class CPreview : public CLocalWindow
{

public:
	int SetDirName(const char * m_szDirectory);
	int SaveBitmap(const char * m_szFilename);

	int SetQuality(const int &iSmooth);

	CPreview();
	virtual ~CPreview();


	void SetPictureQuality(int iQualite);

private:

	LRESULT OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnEraseBackground(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT OnDblClick(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	

	//static int _stdcall MP3Player(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static void SecondToTime(const int &m_iSecond, char * szTime);
	//static int _stdcall InfosFichierProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void ShowBitmap(const int &iRedraw = 1);
	void ShowMultimedia();
	void ShowMP3();

	int SaveWindowPosition();
	int LoadWindowPosition(int &iHauteurInfos);

	CIBitmap * m_ciBitmapTemp;
	CPreviewData * m_pPreviewData;
	int m_iRightTemp;
	int m_iBottomTemp;
	CMP3Dialog * m_cMp3Traitement;
	CInfosFichierProc * m_cInfosFichierProc;
	bool m_bShow;
	bool m_bOnSize;

#ifdef WMP9
	CWMPHost * m_wmpHost;
	int m_iRightFile;
	int m_iTopFile;
#endif

};

#endif // !defined(AFX_PREVIEW_H__07A461D5_7D0C_41D4_8187_7F266B3CF328__INCLUDED_)
