// OpenGL.h: interface for the COpenGL class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __OPENGL_DIAPORAMA_H__
#define __OPENGL_DIAPORAMA_H__

#include <windows.h>								// Header File For Windows
#include <memory>
#include <gl\gl.h>								// Header File For The OpenGL32 Library
#include <gl\glu.h>								// Header File For The GLu32 Library
//#include <gl\glaux.h>	
#include "c3ds.h"
#include "md2.h"
#include <IBitmap.h>
#include "..\libWindow\LocalWindow.h"

#define IDM_3D_FILE 0x0580
class CParameter;
class CFichier;

class COpenGLDiaporama : public CLocalWindow
{

public:


		typedef struct OPENGLDATA
		{
			HGLRC           hRC;							// Permanent Rendering Context
			HDC             hDC;							// Private GDI Device Context
			HWND            hWndParent;							// Handle Parent Window
			HINSTANCE       hInstance;							// Holds The Instance Of The Application
			HWND			hWndStatic;
			int				screen_width;
			int				screen_height;
			BOOL			keys[256];								// Array Used For The Keyboard Routine
			BOOL			active;								// Window Active Flag Set To TRUE By Default
			BOOL			fullscreen;							// Fullscreen Flag Set To Fullscreen Mode By Default


		}MOPENGLDATA,* LPOPENGLDATA;





	COpenGLDiaporama();
	virtual ~COpenGLDiaporama();

	////////////////////////////////////////////////////////////////////////////////////////
	//Déclaration des fonctions
	////////////////////////////////////////////////////////////////////////////////////////

	GLvoid KillGLWindow();

	BOOL DrawScene(BOOL &done);

	int ProcessKey(HWND	hWnd,				// Handle For This Window
			UINT	uMsg,					// Message For This Window
			WPARAM	wParam,					// Additional Message Information
			LPARAM	lParam);					// Additional Message Information

	LRESULT OnCreate(HWND hWnd,UINT uMsg, WPARAM wParam,LPARAM lParam);
	LRESULT OnActivate(HWND hWnd,UINT uMsg, WPARAM wParam,LPARAM lParam);
	LRESULT OnShowWindow(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	LRESULT OnSysCommand(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	LRESULT OnCommand(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	LRESULT OnEraseBackground(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	LRESULT OnPaint(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	LRESULT OnClose(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	LRESULT OnKeyDown(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	LRESULT OnKeyUp(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	LRESULT OnLButtonDown(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	LRESULT OnLButtonUp(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	LRESULT OnMouseWheel(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	LRESULT OnMouseMove(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	LRESULT OnSize(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	LRESULT OnDestroy(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

	int ChargerRepertoire(CFichier * pFichier, int DefineQuality);
	int ReloadRepertoire();
	int StopDiaporama();

	int GetNumTexture()
	{
		return iNumTexture;
	};

	void SetNumTexture(const int &iValue)
	{
		iNumTexture = iValue;
	};

private:

	void GetOptionDiaporama(int &iDelay,int &iQuality3D);
	int SetGLTextures(int iNum, const char * filename);
	void DeleteGLTextures(int iNum);
	void DeleteAllGLTextures();
	void PrechargePhoto();
	void DrawGLQuad(float alpha);
	void DrawGLPicturePlus(float depX, float depZ, int iNumTexture, float alpha = 1.0f, bool bRotation = false, bool bFirst = false);
	void DrawGLPictureMoins(float depX, float depZ, int iNumTexture, float alpha = 1.0f);
	void DrawObject();
	void LoadGLTextures(int iNum, CIBitmap &m_ciBitmap);
	int InitGL();

	static ULONG __stdcall DiaporamaThread(LPVOID parametre);

	void DrawGLScene();

	int MySetPixelFormat();

	void resize();
	int Avance();
	int Recule();
	void InitRotationValue();
	void InitNavigationValue();

	char m_cFolderName[MAX_PATH];
	LPOPENGLDATA m_stOpenGLData;
	float points[60][45][3];    // The Array For The Points On The Grid Of Our "Wave"
	GLuint * texture;			// Storage For One Texture ( NEW )
	int iNumTexture;
	int haut;
	int bas;
	float left;
	float up;
	int iNbFrame;
	int m_iNbFrameSup;
	bool bAvance;
	bool bRecule;
	bool m_bModifieZoom;

	bool m_bSensRotation;
	bool m_bRotationEnd;
	bool m_bRotation;

	bool m_bZoom;
	float m_fZoomDepart;
	float m_fZoomFin;

	float m_fPasRotation;
	float m_fPasZoom;

	float m_fDegreStart;
	float m_fDegreEnd;

	int m_iNiveauZoom;

	int iNumFrame;
	int iNbFile;
	int iNbPrecharge;
	float angle;	
	DWORD lastTickCount;
	CParameter * m_cParameter;
	CFichier * m_cFichier;
	DWORD threadID;
	HANDLE m_hThread;
	CIBitmapVector m_cBitmapVector;
	bool m_bBuffer;
	int m_iDefineQuality;

	HWND hWndParent;
	bool m_bDiaporamaAvance;
	bool m_bDiaporama;
	int m_iNbSeconds;
};


#endif // !defined(AFX_OPENGL_H__F386BFEA_20E7_4833_885E_66DA519439B0__INCLUDED_)
