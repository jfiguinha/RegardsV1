// OpenGL.h: interface for the COpenGL class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGL_H__F386BFEA_20E7_4833_885E_66DA519439B0__INCLUDED_)
#define AFX_OPENGL_H__F386BFEA_20E7_4833_885E_66DA519439B0__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include <windows.h>								// Header File For Windows
#include <memory>
#include <gl\gl.h>								// Header File For The OpenGL32 Library
#include <gl\glu.h>								// Header File For The GLu32 Library
//#include <gl\glaux.h>	
#include "c3ds.h"
#include "md2.h"

#include "..\libWindow\LocalWindow.h"

#define IDM_3D_FILE 0x0580
class CParameter;


class COpenGL : public CLocalWindow
{

public:


		typedef struct OPENGLDATA
		{
			HGLRC           hRC;							// Permanent Rendering Context
			HDC             hDC;							// Private GDI Device Context
			HWND            hWndParent;							// Handle Parent Window
			HINSTANCE       hInstance;							// Holds The Instance Of The Application
			HWND			hWndStatic;

			BOOL			keys[256];								// Array Used For The Keyboard Routine
			BOOL			active;								// Window Active Flag Set To TRUE By Default
			BOOL			fullscreen;							// Fullscreen Flag Set To Fullscreen Mode By Default

			BOOL			m_bLumiere;
			C3ds			* m_c3ds;
			CMd2			* m_cMd2;
			
			float		xRot;
			float		yRot;
			float		zRot;
			int			iEdge;
			int			nCmdShow;
			float		distance;
			bool		m_bFichierCharge;
			bool		m_bMouseMove;
			float		zoom;


			int iFormat; //0 -> 3ds 1 -> MD2
			int iAffichage;
			int screen_width;
			int screen_height;
			char m_Filename[512];
			int showStats;

			GLuint	base;				// Base Display List For The Font
			GLuint	texture[2];			// Storage For Our Font Texture

			float m_fRed;
			float m_fGreen;
			float m_fBlue;

			
			GLYPHMETRICSFLOAT gmf[256];	// Storage For Information About Our Outline Font Characters

		}MOPENGLDATA,* LPOPENGLDATA;





	COpenGL();
	virtual ~COpenGL();

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

	int ChargerFichier(const char * m_szFilename);

private:

	
	int Determine3DFormatNumber(const char * m_szFilename);

	GLvoid DrawObject();

	GLvoid BuildFont();

	GLvoid KillFont();

	int LoadGLTextures();

	int InitGL();

	int DrawGLScene();

	int MySetPixelFormat();

	void resize();

	GLvoid glPrint(const char *fmt, ...);	// Where The Printing Happens

	
	int xOldx;
	int yOldy;
	LPOPENGLDATA m_stOpenGLData;
	
	float angle;				// camera angle 
	float radians;			// camera angle in radians

	////// Mouse/Camera Variables
	int mouseX, mouseY;			// mouse coordinates
	float cameraX, cameraY, cameraZ;		// camera coordinates
	float lookX, lookY, lookZ;			// camera look-at coordinates
	int oldMouseX, oldMouseY;
	CParameter * m_cParameter;
};

#endif // !defined(AFX_OPENGL_H__F386BFEA_20E7_4833_885E_66DA519439B0__INCLUDED_)
