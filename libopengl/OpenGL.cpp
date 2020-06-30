// OpenGL.cpp: implementation of the COpenGL class.
//
//////////////////////////////////////////////////////////////////////

#include "OpenGL.h"
#include <strsafe.h>
#include "../resource.h"
#include "../parameter.h"
#ifndef WM_MOUSEWHEEL
#include "zmouse.h"
#endif

#define PI		3.14159

extern HINSTANCE hInstance;


GLvoid COpenGL::BuildFont()								// Build Our Font Display List
{
	HFONT	font;										// Windows Font ID

	m_stOpenGLData->base = glGenLists(256);								// Storage For 256 Characters

	font = CreateFont(	-12,							// Height Of Font
						0,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_BOLD,						// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						"Comic Sans MS");				// Font Name

	SelectObject(GetDC(NULL), font);							// Selects The Font We Created

	wglUseFontOutlines(	GetDC(NULL),							// Select The Current DC
						0,								// Starting Character
						255,							// Number Of Display Lists To Build
						m_stOpenGLData->base,							// Starting Display Lists
						0.0f,							// Deviation From The True Outlines
						0.2f,							// Font Thickness In The Z Direction
						WGL_FONT_POLYGONS,				// Use Polygons, Not Lines
						m_stOpenGLData->gmf);

}

GLvoid COpenGL::KillFont()									// Delete The Font From Memory
{
	glDeleteLists(m_stOpenGLData->base,256);							// Delete All 256 Display Lists
}

GLvoid COpenGL::glPrint(const char *fmt, ...)	// Where The Printing Happens
{
	float		length=0;								// Used To Find The Length Of The Text
	char		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments

	if (fmt == NULL)									// If There's No Text
		return;											// Do Nothing

	va_start(ap, fmt);									// Parses The String For Variables
	    vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text

	for (unsigned int loop=0;loop<(strlen(text));loop++)	// Loop To Find Text Length
	{
		length+=m_stOpenGLData->gmf[text[loop]].gmfCellIncX;			// Increase Length By Each Characters Width
	}

	glTranslatef(-length/2,0.0f,0.0f);					// Center Our Text On The Screen

	glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
	glListBase(m_stOpenGLData->base);									// Sets The Base Character to 0
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
	glPopAttrib();	

}

int COpenGL::LoadGLTextures()                                    // Load Bitmaps And Convert To Textures
{
	
	BITMAP bitmap;
	HBITMAP hBitmap;
	BITMAPINFO bmiInfos;
	BYTE * m_bData;

	hBitmap = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_FONT));

	GetObject (hBitmap, sizeof (BITMAP), &bitmap);

	bmiInfos.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiInfos.bmiHeader.biWidth = bitmap.bmWidth;
	bmiInfos.bmiHeader.biHeight = bitmap.bmHeight;
	bmiInfos.bmiHeader.biPlanes = 1;
	bmiInfos.bmiHeader.biBitCount = 24;
	bmiInfos.bmiHeader.biCompression = BI_RGB;
	bmiInfos.bmiHeader.biSizeImage = ((3 * bitmap.bmWidth + 3) & ~3) * bitmap.bmHeight;

	m_bData = new BYTE[bmiInfos.bmiHeader.biSizeImage];

	GetDIBits(GetDC(NULL), hBitmap, 0, bitmap.bmHeight, 
			 m_bData, &bmiInfos, DIB_RGB_COLORS); 

	glBindTexture(GL_TEXTURE_2D, m_stOpenGLData->texture[0]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, bitmap.bmWidth, bitmap.bmHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, m_bData);


	// Return The Status

	delete[] m_bData;
	DeleteObject(hBitmap);

    return 0;                                  // Return The Status

}

int COpenGL::ProcessKey(HWND	hWnd,				// Handle For This Window
		UINT	uMsg,					// Message For This Window
		WPARAM	wParam,					// Additional Message Information
		LPARAM	lParam)					// Additional Message Information
{
	switch(wParam)
	{
		case VK_UP:
			m_stOpenGLData->xRot+=0.5f;
			break;

		case VK_DOWN:
			m_stOpenGLData->xRot-=0.5f;
			break;

		case VK_LEFT:
			m_stOpenGLData->yRot+=0.5f;
			break;

		case VK_RIGHT:
			m_stOpenGLData->yRot-=0.5f;
			break;

		case VK_SUBTRACT:
			m_stOpenGLData->zoom += 2.0f;
			break;

		case VK_ADD:
			m_stOpenGLData->zoom -= 2.0f;
			break;

		case VK_END:
			m_stOpenGLData->zRot-=0.5f;
			break;

		case VK_HOME:
			m_stOpenGLData->zRot+=0.5f;
			break;

		case 76:
		case 108:
			{

				m_stOpenGLData->m_bLumiere = !m_stOpenGLData->m_bLumiere;
			}
			break;

		case 105:
		case 73:
			m_stOpenGLData->iAffichage++;
			m_stOpenGLData->iAffichage = m_stOpenGLData->iAffichage % 4;
			break;
	
		default:
			return 0;

	}
	return 1;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COpenGL::COpenGL()
{
	m_cParameter = new CParameter();
	m_stOpenGLData = (OPENGLDATA *)malloc(sizeof(OPENGLDATA));

	m_stOpenGLData->showStats = 1;
	m_stOpenGLData->xRot = 1.0f;
	m_stOpenGLData->yRot = 1.0f;
	m_stOpenGLData->zRot = 1.0f;
	m_stOpenGLData->zoom = 60.0f;
	m_stOpenGLData->m_fRed = 1.0f;
	m_stOpenGLData->m_fGreen = 1.0f;
	m_stOpenGLData->m_fBlue = 1.0f;
	m_stOpenGLData->m_bLumiere = true;
	m_stOpenGLData->iFormat = 0;

	m_stOpenGLData->iAffichage = m_cParameter->GetOptionAffichage3D();

	m_stOpenGLData->m_bMouseMove = false;
	m_stOpenGLData->distance = 0.0f;
	m_stOpenGLData->m_bFichierCharge = false;

	m_stOpenGLData->hRC=NULL;							// Permanent Rendering Context
	m_stOpenGLData->hDC=NULL;							// Private GDI Device Context
	m_stOpenGLData->hWndParent=NULL;							// Holds Our Window Handle
	m_stOpenGLData->active=TRUE;						// Window Active Flag Set To TRUE By Default
	m_stOpenGLData->fullscreen=FALSE;					// Fullscreen Flag Set To Fullscreen Mode By Default

	xOldx = 0;
	yOldy = 0;
	angle = 0.0f;				// camera angle 
	radians = 0.0f;			// camera angle in radians

	m_stOpenGLData->m_c3ds = new C3ds();
	m_stOpenGLData->m_cMd2 = new CMd2();
	ZeroMemory(m_stOpenGLData->keys,256);
}

COpenGL::~COpenGL()
{
	delete m_cParameter;
	if(m_stOpenGLData->m_c3ds != NULL)
	{
		delete m_stOpenGLData->m_c3ds;
		m_stOpenGLData->m_c3ds = NULL;
	}

	if(m_stOpenGLData->m_cMd2 != NULL)
	{
		delete m_stOpenGLData->m_cMd2;
		m_stOpenGLData->m_cMd2 = NULL;
	}

	m_cParameter->SetOptionAffichage3D(m_stOpenGLData->iAffichage);
	free(m_stOpenGLData);
}

////////////////////////////////////////////////////////////////////////////////////
//Affichage des objets suivant le format
////////////////////////////////////////////////////////////////////////////////////

GLvoid COpenGL::DrawObject()
{
	switch(m_stOpenGLData->iFormat)
	{
		case 1:
			{
				m_stOpenGLData->m_c3ds->DrawObject(m_stOpenGLData->m_fRed,m_stOpenGLData->m_fGreen,m_stOpenGLData->m_fBlue);
				break;
			}

		case 2:
			{
				m_stOpenGLData->m_cMd2->DisplayMD2(m_stOpenGLData->m_fRed,m_stOpenGLData->m_fGreen,m_stOpenGLData->m_fBlue);
				break;
			}
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//Draw The Scene
////////////////////////////////////////////////////////////////////////////////////////

BOOL COpenGL::DrawScene(BOOL &done)
{
	// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
	if (m_stOpenGLData->active)						// Program Active?
	{
		if (m_stOpenGLData->keys[VK_ESCAPE] == 1)				// Was ESC Pressed?
		{
			done=TRUE;				// ESC Signalled A Quit
		}
		else						// Not Time To Quit, Update Screen
		{
			DrawGLScene();				// Draw The Scene
			SwapBuffers(m_stOpenGLData->hDC);			// Swap Buffers (Double Buffering)
		}
	}

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////
// All Setup For OpenGL Goes Here
////////////////////////////////////////////////////////////////////////////////////////

int COpenGL::InitGL()								
{
	
	

	////////////////////////////////////////////////////////
	//Lumiere
	////////////////////////////////////////////////////////

    glClearColor(0.0f, 0.0f, 0.0f, 0.5f); // This clear the background color to black
	glClearDepth(1.0f);
    glShadeModel(GL_SMOOTH); // Type of shading for the polygons
   	
    glEnable(GL_DEPTH_TEST); // We enable the depth test (also called z buffer)
    
	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL); // Polygon rasterization mode (polygon filled)

	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT,GL_DONT_CARE);

	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	return TRUE;
}

/**********************************************************
 *
 * SUBROUTINE resize(int,int)
 *
 * This routine must be called everytime we resize our window.
 * 
 *********************************************************/

void COpenGL::resize()
{
	// Save previous context and make our context current
	HDC mhOldDC = wglGetCurrentDC();
	HGLRC mhOldGLRC = wglGetCurrentContext();
	wglMakeCurrent(m_stOpenGLData->hDC, m_stOpenGLData->hRC);

	// Reset the current viewport
	glViewport(0, 0, m_stOpenGLData->screen_width, m_stOpenGLData->screen_height);

	// Restore previous context
	wglMakeCurrent(mhOldDC, mhOldGLRC);

}


////////////////////////////////////////////////////////////////////////////////////////
// Fonction d'affichage
////////////////////////////////////////////////////////////////////////////////////////

int COpenGL::DrawGLScene()								
{
	if(!m_stOpenGLData->m_bFichierCharge)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		
		if (m_stOpenGLData->xRot > 359) m_stOpenGLData->xRot = 0;
		if (m_stOpenGLData->yRot > 359) m_stOpenGLData->yRot = 0;
		if (m_stOpenGLData->zRot > 359) m_stOpenGLData->zRot = 0;

		glRotatef(m_stOpenGLData->xRot,1.0,0.0,0.0); // Rotations of the object (the model matrix is multiplied by the rotation matrices)
		glRotatef(m_stOpenGLData->yRot,0.0,1.0,0.0);
		glRotatef(m_stOpenGLData->zRot,0.0,0.0,1.0);


		glBegin(GL_TRIANGLES);
		glColor3f(1, 0, 0);			
		glVertex3f(0, 0.8f, 0);			//top
		glColor3f(0, 1, 0);			
		glVertex3f(-0.5, -0.5, 0.5);	//left
		glColor3f(0, 0,1);			
		glVertex3f(0.5, -0.5, 0.5);		//right


		glColor3f(1.0f,0.0f,0.0f);			// Red
		glVertex3f( 0.0f, 0.8f, 0.0f);		// Top Of Triangle (Right)
		glColor3f(0.0f,0.0f,1.0f);			// Blue
		glVertex3f( .5f,-.5f, .5f);			// Left Of Triangle (Right)
		glColor3f(0.0f,0.5f,0.0f);			// Green
		glVertex3f( 0.5f,-0.5f, -0.5f);		// Right Of Triangle (Right)

		glColor3f(1.0f,0.0f,0.0f);			// Red
		glVertex3f( 0.0f, 0.8f, 0.0f);		// Top Of Triangle (Back)
		glColor3f(0.0f,1.0f,0.0f);			// Green
		glVertex3f( 0.5f,-0.5f, -0.5f);		// Left Of Triangle (Back)
		glColor3f(0.0f,0.0f,1.0f);			// Blue
		glVertex3f(-0.5f,-0.5f, -0.5f);		// Right Of Triangle (Back)

		glColor3f(1.0f,0.0f,0.0f);			// Red
		glVertex3f( 0.0f, 0.8f, 0.0f);		// Top Of Triangle (Left)
		glColor3f(0.0f,0.0f,1.0f);			// Blue
		glVertex3f(-0.5f,-0.5f,-0.5f);		// Left Of Triangle (Left)
		glColor3f(0.0f,1.0f,0.0f);			// Green
		glVertex3f(-0.5f,-0.5f, 0.5f);		// Right Of Triangle (Left)

		glEnd();

		//xrot+=1.5f;
		
	}
	else
	{	

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Viewport transformation
		glViewport(0,0,m_stOpenGLData->screen_width,m_stOpenGLData->screen_height);  


		glMatrixMode( GL_PROJECTION);
		glLoadIdentity();

		gluPerspective(m_stOpenGLData->zoom, (float)m_stOpenGLData->screen_width/(float)m_stOpenGLData->screen_height, m_stOpenGLData->distance/10.0f, m_stOpenGLData->distance*2.0f);

		glMatrixMode( GL_MODELVIEW);
		glLoadIdentity();


		//GLfloat LightPosition[]= { 0.0f, 0.0f, 2.0f, 1.0f };


		glTranslatef( 0.0f, 0.0f, -m_stOpenGLData->distance);
		
		glRotatef(m_stOpenGLData->xRot,1.0,0.0,0.0); // Rotations of the object (the model matrix is multiplied by the rotation matrices)
		glRotatef(m_stOpenGLData->yRot,0.0,1.0,0.0);
		glRotatef(m_stOpenGLData->zRot,0.0,0.0,1.0);

		
		//glActiveTextureARB( GL_TEXTURE0_ARB );

		switch(m_stOpenGLData->iAffichage)
		{
			case 0:
				{	
					if(m_stOpenGLData->m_bLumiere)
					{
						GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
						GLfloat LightPosition[]= {m_stOpenGLData->distance*m_stOpenGLData->zoom, 0.0,0.0, 1.0f };	
						GLfloat LightAmbient[]= { 1.0f, 1.0f, 1.0f, 1.0f }; 

						glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
						glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
						glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);
						
						glEnable(GL_LIGHT1);
						glEnable(GL_LIGHTING);
						glEnable(GL_CULL_FACE);

						glCullFace(GL_BACK);
						DrawObject();

						glDisable(GL_CULL_FACE);
						glDisable(GL_LIGHTING);
						glDisable(GL_LIGHT1);
					}
					else
						DrawObject();

										
				}
				break;

			case 1:


				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
				glEnable( GL_BLEND);
				DrawObject();
				glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
				glDisable( GL_BLEND);


				break;

			case 2:
				//hidden line
				glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
				glPolygonOffset( 1.0f, 1.0f);
				glEnable( GL_POLYGON_OFFSET_FILL);

				DrawObject();
				glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				glDisable( GL_POLYGON_OFFSET_FILL);

				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
				glEnable( GL_BLEND);
				DrawObject();
				glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
				glDisable( GL_BLEND);
				break;

			default:
				{
					//configure the drawing options
					glEnable(GL_LINE_SMOOTH);
					glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glLineWidth(1.0f);

					glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
					glEnable( GL_BLEND);
					DrawObject();
					glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
					glDisable( GL_BLEND);

					glDisable(GL_LINE_SMOOTH);
				}
				break;
		}

		//glActiveTextureARB( GL_TEXTURE0_ARB);

		glFlush();

	}

	return TRUE;
}



////////////////////////////////////////////////////////////////////////////////////////
// Properly Kill The Window
////////////////////////////////////////////////////////////////////////////////////////

GLvoid COpenGL::KillGLWindow()							
{
	DestroyWindow(m_hWnd);

}

////////////////////////////////////////////////////////////////////////////////////////
// Create OpenGL Window
////////////////////////////////////////////////////////////////////////////////////////

// Set up the pixel format
int COpenGL::MySetPixelFormat()
{

	PIXELFORMATDESCRIPTOR pfd = { 
	    sizeof(PIXELFORMATDESCRIPTOR),    // size of this pfd 
	    1,                                // version number 
	    PFD_DRAW_TO_WINDOW |              // support window 
	    PFD_SUPPORT_OPENGL |              // support OpenGL 
	    PFD_DOUBLEBUFFER,                 // double buffered 
	    PFD_TYPE_RGBA,                    // RGBA type 
	    32,                               // 24-bit color depth 
	    0, 0, 0, 0, 0, 0,                 // color bits ignored 
	    0,                                // no alpha buffer 
	    0,                                // shift bit ignored 
	    0,                                // no accumulation buffer 
	    0, 0, 0, 0,                       // accum bits ignored 
	    32,                               // 32-bit z-buffer     
	    0,                                // no stencil buffer 
	    0,                                // no auxiliary buffer 
	    PFD_MAIN_PLANE,                   // main layer 
	    0,                                // reserved 
	    0, 0, 0                           // layer masks ignored 
	}; 
	
	int  iPixelFormat; 
 
	// get the device context's best, available pixel format match 
	if((iPixelFormat = ChoosePixelFormat(m_stOpenGLData->hDC, &pfd)) == 0)
	{
		//MessageBox(NULL, "ChoosePixelFormat Failed", NULL, MB_OK);
		return 0;
	}
	 
	// make that match the device context's current pixel format 
	if(SetPixelFormat(m_stOpenGLData->hDC, iPixelFormat, &pfd) == FALSE)
	{
		//MessageBox(NULL, "SetPixelFormat Failed", NULL, MB_OK);
		return 0;
	}

    // Create the render context
	m_stOpenGLData->hRC = wglCreateContext(m_stOpenGLData->hDC);
    wglMakeCurrent(m_stOpenGLData->hDC, m_stOpenGLData->hRC);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGL::OnCreate(HWND hWnd,UINT uMsg, WPARAM wParam,LPARAM lParam)
{
	//m_stOpenGLData->active=TRUE;

	m_stOpenGLData->hWndStatic = CreateWindowEx(0,"STATIC","", WS_CHILD,
			 0, 0, 0,0,
			hWnd,
			 (HMENU)NULL,
			 hInstance,
			 NULL);

	m_stOpenGLData->hDC = GetDC(m_stOpenGLData->hWndStatic);

	MySetPixelFormat();

	InitGL();

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGL::OnActivate(HWND hWnd,UINT uMsg, WPARAM wParam,LPARAM	lParam)
{
	if (!HIWORD(wParam))					// Check Minimization State
		m_stOpenGLData->active=TRUE;					// Program Is Active
	else
		m_stOpenGLData->active=FALSE;					// Program Is No Longer Active

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGL::OnShowWindow(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	if(wParam)
	{
		//Affichage 
		::ShowWindow(m_stOpenGLData->hWndStatic,SW_SHOW);

		if(!m_stOpenGLData->fullscreen)
		{
			RECT rcPos;
			GetClientRect(hWnd,&rcPos);

			SendMessage(hWnd,WM_SIZE,0,MAKELPARAM(rcPos.right,rcPos.bottom));
		}

	}
	else
	{
		//Desaffichage
		::ShowWindow(m_stOpenGLData->hWndStatic,SW_HIDE);
	}
	
	return 0;

}

int COpenGL::Determine3DFormatNumber(const char * m_szFilename)
{
	UINT m_iValue = 0;
	TCHAR drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[255];

	_splitpath(m_szFilename, drive, dir, fname, ext);

	if(stricmp(ext,".3ds") == 0)
		return 1;

	if(stricmp(ext,".md2") == 0)
		return 2;

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGL::OnSysCommand(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (wParam)						// Check System Calls
	{
		case SC_SCREENSAVE:				// Screensaver Trying To Start?
		case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
		return 0;					// Prevent From Happening
	}
	return 0;
}

int COpenGL::ChargerFichier(const char * m_szFilename)
{
	m_stOpenGLData->iFormat = Determine3DFormatNumber(m_szFilename);
	//Test Format
	switch(m_stOpenGLData->iFormat)
	{
		
		case 1:
			{
				//Lecture du format 3ds

				m_stOpenGLData->m_bFichierCharge = m_stOpenGLData->m_c3ds->Load3dsFile(m_szFilename,m_stOpenGLData->distance);
				if(m_stOpenGLData->m_bFichierCharge)
				{
					glDepthFunc( GL_LEQUAL);
					glEnable( GL_DEPTH_TEST);
					m_stOpenGLData->xRot = 1.0f;
					m_stOpenGLData->yRot = 1.0f;
					m_stOpenGLData->zRot = 1.0f;
					m_stOpenGLData->zoom = 60.0f;
					m_stOpenGLData->m_bMouseMove = false;
				}
				break;
			}

		case 2:
			{
				//Lecture du format MD2
				m_stOpenGLData->m_bFichierCharge = m_stOpenGLData->m_cMd2->LoadMD2Model(m_szFilename,m_stOpenGLData->distance);
				if(m_stOpenGLData->m_bFichierCharge)
				{
					glDepthFunc( GL_LEQUAL);
					glEnable( GL_DEPTH_TEST);
					m_stOpenGLData->xRot = 1.0f;
					m_stOpenGLData->yRot = 1.0f;
					m_stOpenGLData->zRot = 1.0f;
					m_stOpenGLData->zoom = 60.0f;
					m_stOpenGLData->m_bMouseMove = false;
				}
				break;
			}

	}

	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGL::OnCommand(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	int wmId, wmEvent;
	wmId    = LOWORD(wParam); 
	wmEvent = HIWORD(wParam); 

	switch(wmId)
	{
		case IDM_3D_FILE:
			{
				m_stOpenGLData->iFormat = 0;
				char * cFile = (char *)lParam;
				ChargerFichier(cFile);
			}
			break;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


LRESULT COpenGL::OnEraseBackground(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	return OnPaint(hWnd,uMsg,wParam,lParam);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGL::OnPaint(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	if(m_stOpenGLData->showStats)
	{
		//PAINTSTRUCT ps;

		HDC hdc = GetDC(hWnd);

		char buffer[256];

		StringCchPrintf( buffer,256, "POS: x : %.3f y : %.3f z : %.3f", m_stOpenGLData->xRot,m_stOpenGLData->yRot,m_stOpenGLData->zRot);

		//Creation d'un double buffer;

		HBITMAP hBitmap;

		RECT rt;
		GetClientRect(hWnd, &rt);

		rt.bottom = 20;

		hBitmap = CreateCompatibleBitmap(hdc,rt.right,20);

		HDC sourceDC;
		sourceDC = CreateCompatibleDC(hdc);

		SetBkMode(sourceDC, TRANSPARENT); 

		HBITMAP hbmOldSource = (HBITMAP)::SelectObject( sourceDC, hBitmap);

		FillRect(sourceDC, &rt, (HBRUSH)GetStockObject(BLACK_BRUSH)); 

		SetTextColor(sourceDC,RGB(255,255,255));

		TextOut(sourceDC, rt.left, rt.top, buffer, strlen(buffer)); 
		
		BitBlt(hdc,0,0, rt.right,20, sourceDC, 0, 0,SRCCOPY);

		DeleteDC(sourceDC);

		ReleaseDC(hWnd,hdc);

		DeleteObject(hbmOldSource);
		DeleteObject(hBitmap);
	}

    HDC mhOldDC = wglGetCurrentDC();
    HGLRC mhOldGLRC = wglGetCurrentContext();
	wglMakeCurrent(m_stOpenGLData->hDC, m_stOpenGLData->hRC);
	DrawGLScene();
	SwapBuffers(m_stOpenGLData->hDC);
	wglMakeCurrent(mhOldDC, mhOldGLRC); 
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGL::OnClose(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	DestroyWindow(hWnd);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGL::OnKeyDown(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	ProcessKey(hWnd,uMsg,wParam,lParam);
	m_stOpenGLData->keys[wParam] = TRUE;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGL::OnKeyUp(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	m_stOpenGLData->keys[wParam] = FALSE;					// If So, Mark It As FALSE
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGL::OnLButtonDown(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	m_stOpenGLData->m_bMouseMove = true;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGL::OnLButtonUp(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	m_stOpenGLData->m_bMouseMove = false;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


LRESULT COpenGL::OnMouseWheel(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	long zDelta;
	zDelta = HIWORD(wParam);
	if(zDelta == 120)
		m_stOpenGLData->zoom -= 2.0f;
	else
		m_stOpenGLData->zoom += 2.0f;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGL::OnMouseMove(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	
	if(m_stOpenGLData->m_bMouseMove)
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		if(x > xOldx)
			m_stOpenGLData->yRot += 0.5f;
		else
			m_stOpenGLData->yRot -= 0.5f;

		if(y > yOldy)
			m_stOpenGLData->xRot += 0.5f;
		else
			m_stOpenGLData->xRot -= 0.5f;


		xOldx = x;
		yOldy = y;
	}


	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGL::OnSize(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	int yTop;
	yTop = 0;
 	int yNewSize = HIWORD (lParam); 
	int xNewSize = LOWORD (lParam);

	if(::IsWindowVisible(m_stOpenGLData->hWndStatic) && !m_stOpenGLData->fullscreen)
	{
		RECT rt;
		GetClientRect(hWnd, &rt);

		m_stOpenGLData->screen_width = rt.right;
		m_stOpenGLData->screen_height = rt.bottom;

		MoveWindow(m_stOpenGLData->hWndStatic,0,20,m_stOpenGLData->screen_width,m_stOpenGLData->screen_height,false);
	
	}
	else
	{
		RECT rt;
		GetClientRect(hWnd, &rt);

		m_stOpenGLData->screen_width = rt.right;
		m_stOpenGLData->screen_height = rt.bottom;

		MoveWindow(m_stOpenGLData->hWndStatic,0,20,m_stOpenGLData->screen_width,m_stOpenGLData->screen_height,false);
	}

	if(::IsWindowVisible(m_stOpenGLData->hWndStatic) && !m_stOpenGLData->fullscreen)
		resize();

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGL::OnDestroy(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	KillFont();

	ReleaseDC(m_stOpenGLData->hWndStatic, m_stOpenGLData->hDC) ; 
	wglDeleteContext(m_stOpenGLData->hRC); 

	DestroyWindow(m_stOpenGLData->hWndStatic);

	PostQuitMessage(0);	

	return 0;
}


