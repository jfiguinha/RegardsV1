// OpenGL.cpp: implementation of the COpenGLDiaporama class.
//
//////////////////////////////////////////////////////////////////////

#include "gl/glew.h"
#include "OpenGLDiaporama.h"
#include <strsafe.h>
//#include "ATIExtensions.h"
#include "resource.h"
#include "parameter.h"
#include "fichier.h"
#include "global.h"
#include <effet.h>
#include <IBitmap.h>
#include "LoadImage.h"
#ifndef WM_MOUSEWHEEL
#include "zmouse.h"
#endif

#define WM_FILENAME 0x01000
#define WM_REINITTIMER 0x01001

#define PI		3.14159
const int WIDTH[] = {320,640,800,1024};
const int HEIGHT[] = {240,480,600,768};
GLfloat rgbSharpen[3][3][3];

extern HINSTANCE hInstance;

//---------------------------------------------------------
//Thread de création d'une fenêtre de traitement
//---------------------------------------------------------
ULONG __stdcall COpenGLDiaporama::DiaporamaThread(LPVOID parametre)
{
	OleInitialize(NULL);

	COpenGLDiaporama * m_OpenGLDiaporama = (COpenGLDiaporama *)parametre;
					// Get The Tick Count

	
	try
	{
		bool m_bEnd = false;
		bool m_bStandBy = true;
		MSG msg;
		DWORD lastTickCount = GetTickCount();

		while (!m_bEnd)
		{
			DWORD tickCount = GetTickCount();
			DWORD milliseconds = (tickCount - lastTickCount);

			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				switch(msg.message)
				{
					case WM_QUIT:
						{
							m_bEnd = true;
							break;
						}

					case WM_REINITTIMER:
						{
							lastTickCount = GetTickCount();
							break;
						}

					case IDM_IMAGES_SUIVANTE:
						{
							lastTickCount = GetTickCount();
							m_bStandBy = false;
							break;
						}

					case WM_FILENAME:
						{
							char m_szFileName[MAX_PATH];
							int iNumFichier = (int)msg.lParam;

							if(m_OpenGLDiaporama->bAvance)
								iNumFichier = (iNumFichier + 1)%m_OpenGLDiaporama->iNbFile;
							else
							{
								iNumFichier = iNumFichier - 1;
								if(iNumFichier < 0)
									iNumFichier = m_OpenGLDiaporama->iNbFile + iNumFichier;
							}
							m_OpenGLDiaporama->m_cFichier->AffichageImagePosition(m_szFileName, MAX_PATH, iNumFichier);					
							SendMessage(m_OpenGLDiaporama->GetParent(),WM_COMMAND,MAKEWPARAM(IDM_UPDATESTATUSFILEINFO,0),(LPARAM)m_szFileName);
							break;
						}

					default :
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					break;
				}
			}
			else if(milliseconds > m_OpenGLDiaporama->m_iNbSeconds && m_OpenGLDiaporama->m_bDiaporama && !m_bStandBy)
			{
				if(m_OpenGLDiaporama->m_bDiaporamaAvance)
				{
					SendMessage(m_OpenGLDiaporama->m_hWnd,WM_COMMAND,MAKEWPARAM(IDM_IMAGES_SUIVANTE,0),0);
					m_bStandBy = true;
				}
				else
				{
					SendMessage(m_OpenGLDiaporama->m_hWnd,WM_COMMAND,MAKEWPARAM(IDM_IMAGES_PRCDENTE,0),0);
					m_bStandBy = true;
				}
				lastTickCount = GetTickCount();
			}
			else if(m_bStandBy)
			{
				lastTickCount = GetTickCount();
			}

			Sleep(100);
		}
	}
	catch (...)
	{

	}

	ExitThread(0);
	OleUninitialize();
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COpenGLDiaporama::GetOptionDiaporama(int &iDelay,int &iQuality3D)
{
	int m_iSequence = 0;
	bool m_bAutoSize = false;
	int m_iQualite = 0;
	bool m_bFullScreen = false;
	bool m_b3D = false;

	CParameter::GetOptionDiaporama(m_iSequence,iDelay,m_bAutoSize,m_iQualite,m_bFullScreen,m_b3D,iQuality3D);

	iDelay = iDelay * 1000;

}

//--------------------------------------------------------------------
//Chargement des textures
//--------------------------------------------------------------------
int COpenGLDiaporama::SetGLTextures(int iNum, const char * filename)
{
	CLoadImage m_ChargementImage;

	if(m_bBuffer)
	{
		if(m_cBitmapVector[iNum].GetPtBitmap() == NULL)
		{
			HBITMAP hBitmap = m_ChargementImage.ChargementIconeStatic(filename, WIDTH[m_iDefineQuality], HEIGHT[m_iDefineQuality], 0, 0, 1, RGB(0,0,0),RGB(0,0,0),false,true);			
			if(hBitmap != NULL)
				m_cBitmapVector[iNum].SetBitmap(hBitmap,false);
		}

		LoadGLTextures(iNum, m_cBitmapVector[iNum]);
	}
	else
	{
		CIBitmap m_ciBitmap;
		HBITMAP hBitmap = m_ChargementImage.ChargementIconeStatic(filename, WIDTH[m_iDefineQuality], HEIGHT[m_iDefineQuality], 0, 0, 1, RGB(0,0,0),RGB(0,0,0),false,true);			
		if(hBitmap != NULL)
			m_ciBitmap.SetBitmap(hBitmap,false);

		LoadGLTextures(iNum, m_ciBitmap);
	}
	return 0;
}

//--------------------------------------------------------------------
//Chargement des textures
//--------------------------------------------------------------------
void COpenGLDiaporama::LoadGLTextures(int iNum, CIBitmap &m_ciBitmap)
{
	glGenTextures (1, &texture[iNum]); 
	glBindTexture (GL_TEXTURE_2D, texture[iNum]); 
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST); 
	glTexImage2D(GL_TEXTURE_2D, 0, 4, m_ciBitmap.GetBitmapWidth(), m_ciBitmap.GetBitmapHeigth(), 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, m_ciBitmap.GetPtBitmap());
}

//--------------------------------------------------------------------
//Chargement des textures
//--------------------------------------------------------------------
void COpenGLDiaporama::DeleteGLTextures(int iNum)
{
	glDeleteTextures( 1, &texture[iNum]);
}

//--------------------------------------------------------------------
//Chargement des textures
//--------------------------------------------------------------------
void COpenGLDiaporama::DeleteAllGLTextures()
{
	for(int i = 0;i < iNbFile;i++)
		glDeleteTextures( 1, &texture[i]);
}

//--------------------------------------------------------------------
//Chargement des textures
//--------------------------------------------------------------------
void COpenGLDiaporama::PrechargePhoto()
{
	char m_szFilename[MAX_PATH];

	if(bAvance)
	{
		int iNumDelete = iNumTexture - 3;
		if(iNumDelete < 0)
			iNumDelete = iNbFile + iNumDelete;
		DeleteGLTextures(iNumDelete);

		int iNumFichier = (iNumTexture+4)%iNbFile;

		m_cFichier->AffichageImagePosition(m_szFilename,MAX_PATH,iNumFichier);

		SetGLTextures(iNumFichier,m_szFilename);
	}

	if(bRecule)
	{
		int iNumDelete = (iNumTexture + 3)%iNbFile;
		DeleteGLTextures(iNumDelete);
		
		int iNumAdd = iNumTexture - 4;
		if(iNumAdd < 0)
			iNumAdd = iNbFile + iNumAdd;

		m_cFichier->AffichageImagePosition(m_szFilename,MAX_PATH,iNumAdd);

		SetGLTextures(iNumAdd,m_szFilename);
	}

	PostThreadMessage(threadID,WM_FILENAME,0,(LPARAM)iNumTexture);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
COpenGLDiaporama::COpenGLDiaporama()
{
	m_bBuffer = false;
	m_cParameter = new CParameter();
	m_stOpenGLData = (OPENGLDATA *)malloc(sizeof(OPENGLDATA));

	m_cFichier = new CFichier();

	m_stOpenGLData->hRC=NULL;							// Permanent Rendering Context
	m_stOpenGLData->hDC=NULL;							// Private GDI Device Context
	m_stOpenGLData->hWndParent=NULL;							// Holds Our Window Handle
	m_stOpenGLData->active=TRUE;						// Window Active Flag Set To TRUE By Default
	m_stOpenGLData->fullscreen=FALSE;					// Fullscreen Flag Set To Fullscreen Mode By Default

	m_bModifieZoom = false;
	m_iDefineQuality = 2;
	iNumTexture = 0;
	m_fDegreStart = 0.0f;
	m_bRotation = false;
	m_bRotationEnd = true;
	m_bDiaporama = false;
	haut=0;
	bas=1;
	left=0.0f;
	up=-12.0f;
	iNbFrame = 60;
	bAvance = false;
	bRecule = false;
	iNumFrame = 0;
	iNbFile = 0;
	iNbPrecharge = 7;
	angle= 0.0f;
	m_iNiveauZoom = 0;
	texture = NULL;
	m_fZoomDepart = 0.0f;
	m_fZoomFin = 5.4f;
	m_fPasRotation = 0.5f;
	m_fPasZoom = 0.2f;
	ZeroMemory(m_stOpenGLData->keys,256);
	m_iNbFrameSup = 0;

	m_hThread = CreateThread(0, 0, DiaporamaThread, this, 0, &threadID);
}

//////////////////////////////////////////////////////////////////////
// Destructeur
//////////////////////////////////////////////////////////////////////
COpenGLDiaporama::~COpenGLDiaporama()
{
	PostThreadMessage(threadID,WM_QUIT,0,0);

	if(m_hThread != NULL)
	{
		::CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	m_cBitmapVector.clear();

	if(texture != NULL)
		delete[] texture;

	delete m_cParameter;
	free(m_stOpenGLData);
}

//////////////////////////////////////////////////////////////////////
// Rechargement d'un répertoire
//////////////////////////////////////////////////////////////////////
int COpenGLDiaporama::ReloadRepertoire()
{
	char m_szFilename[MAX_PATH];
	iNbFile = m_cFichier->GetNbFiles();

	if(texture != NULL)
		delete[] texture;

	texture = new GLuint[iNbFile];

	int Status=FALSE;									// Status Indicator

	int iStart = iNumTexture;

	for(int i = iStart;i < iStart+4;i++)
	{
		m_cFichier->AffichageImagePosition(m_szFilename,MAX_PATH,i);
		SetGLTextures(i%iNbFile,m_szFilename);
	}

	for(int i = 1;i < 4;i++)
	{
		int iNbFichier = iStart - i;
		if(iNbFichier < 0)
			iNbFichier = iNbFile + iNbFichier;
		m_cFichier->AffichageImagePosition(m_szFilename,MAX_PATH,iNbFichier);
		SetGLTextures(iNbFichier%iNbFile,m_szFilename);
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Chargement d'un répertoire
//////////////////////////////////////////////////////////////////////
int COpenGLDiaporama::ChargerRepertoire(CFichier * pFichier, int DefineQuality)
{

	GetOptionDiaporama(m_iNbSeconds,m_iDefineQuality);

	/*
	if(DefineQuality != m_iDefineQuality)
	{
		DeleteAllGLTextures();
		m_cBitmapVector.clear();
	}
	m_iDefineQuality = DefineQuality;*/

	if(strcmp(m_cFolderName,pFichier->GetDirName()) != 0)
	{
		m_cBitmapVector.clear();
		m_bBuffer = false;

		if(pFichier->GetNbFiles() > 20)
			m_bBuffer = false;
		else
		{
			for(int i = 0;i < pFichier->GetNbFiles();i++)
			{
				CIBitmap m_ciBitmap;
				m_cBitmapVector.push_back(m_ciBitmap);
			}
			m_bBuffer = true;
		}
		strcpy_s(m_cFolderName,MAX_PATH,pFichier->GetDirName());
	}

	m_cFichier = pFichier;
	ReloadRepertoire();

	return TRUE;
}

/*
//////////////////////////////////////////////////////////////////////
// Chargement d'un répertoire
//////////////////////////////////////////////////////////////////////
void COpenGLDiaporama::DrawGLQuad(float alpha)
{
	glPushMatrix();
	glBegin(GL_QUADS);
	for(int x = 0; x < 59; x++ )
	{
		for(int y = 0; y < 44; y++ )
		{
			float float_x = float(x)/59.0f;
			float float_y = float(y)/44.0f;
			float float_xb = float(x+1)/59.0f;
			float float_yb = float(y+1)/44.0f;

			glColor4f(alpha,alpha,alpha,alpha);

			glTexCoord2f( float_x, float_y);
			glVertex3f( points[x][y][0], points[x][y][1], points[x][y][2] );

			glTexCoord2f( float_x, float_yb );
			glVertex3f( points[x][y+1][0], points[x][y+1][1], points[x][y+1][2] );

			glTexCoord2f( float_xb, float_yb );
			glVertex3f( points[x+1][y+1][0], points[x+1][y+1][1], points[x+1][y+1][2] );

			glTexCoord2f( float_xb, float_y );
			glVertex3f( points[x+1][y][0], points[x+1][y][1], points[x+1][y][2] );
		}
	}
	glEnd();
	glPopMatrix();


}
*/
//////////////////////////////////////////////////////////////////////
// Chargement d'un répertoire
//////////////////////////////////////////////////////////////////////
void COpenGLDiaporama::DrawGLQuad(float alpha)
{
	glPushMatrix();
	
    // GPGPU CONCEPT 4: Viewport-Sized Quad = Data Stream Generator.
    // In order to execute fragment programs, we need to generate pixels.
    // Drawing a quad the size of our viewport (see above) generates a 
    // fragment for every pixel of our destination texture. Each fragment
    // is processed identically by the fragment program. Notice that in 
    // the reshape() function, below, we have set the frustum to 
    // orthographic, and the frustum dimensions to [-1,1].  Thus, our 
    // viewport-sized quad vertices are at [-1,-1], [1,-1], [1,1], and 
    // [-1,1]: the corners of the viewport.
    glBegin(GL_QUADS);
    {       
		//glColor4f(alpha,alpha,alpha,alpha);
		glColor4f(alpha,alpha,alpha,alpha);
        glTexCoord2f(0, 0); glVertex3f(-6.65, -5, -0.5f);
        glTexCoord2f(1, 0); glVertex3f( 6.65, -5, -0.5f);
        glTexCoord2f(1, 1); glVertex3f( 6.65,  5, -0.5f);
        glTexCoord2f(0, 1); glVertex3f(-6.65,  5, -0.5f);
    }
    glEnd();


	glPopMatrix();
}

//////////////////////////////////////////////////////////////////////
// Chargement d'un répertoire
//////////////////////////////////////////////////////////////////////
void COpenGLDiaporama::DrawGLPicturePlus(float depX, float depZ, int iNumTexture, float alpha, bool bRotation, bool bFirst)
{
	glLoadIdentity();

	if(bRotation && !bAvance && !bRecule)
	{
		if(m_bSensRotation)
		{
			if(m_fDegreStart < m_fDegreEnd)
				m_fDegreStart += m_fPasRotation;
			else
				m_bRotationEnd = true;
		}
		else
		{
			if(m_fDegreStart > m_fDegreEnd)
				m_fDegreStart -= m_fPasRotation;
			else
				m_bRotationEnd = true;
		}

		glRotatef(m_fDegreStart,0,0,1.0f);
	}

	if(!bAvance && !bRecule && bFirst && m_fZoomDepart <= m_fZoomFin && !m_bModifieZoom)
	{
		depZ = depZ + m_fZoomDepart;
		m_fZoomDepart += m_fPasZoom;
	}
	else if(bFirst)
	{
		if((bAvance || bRecule) && m_fZoomDepart > 0.0f && !m_bModifieZoom)
			m_fZoomDepart -= m_fPasZoom;
		else if((bAvance || bRecule) && !m_bModifieZoom)
			m_fZoomDepart = 0.0f;

		depZ = depZ + m_fZoomDepart;
	}

	glTranslatef(depX,0.0f,depZ);

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Set Blending Mode To Mix Based On SRC Alpha
	//glEnable(GL_ALPHA);

	glBindTexture(GL_TEXTURE_2D, texture[iNumTexture%iNbFile]);

	//glDisable(GL_ALPHA);

	DrawGLQuad(alpha);
}

//////////////////////////////////////////////////////////////////////
// Chargement d'un répertoire
//////////////////////////////////////////////////////////////////////
void COpenGLDiaporama::DrawGLPictureMoins(float depX, float depZ, int iNumTexture, float alpha)
{
	glLoadIdentity();

	glTranslatef(depX,0.0f,depZ);

	int iNumPhoto = iNumTexture;
	if(iNumPhoto < 0)
		iNumPhoto = iNbFile + iNumPhoto;

	glBindTexture(GL_TEXTURE_2D, texture[iNumPhoto]);

	DrawGLQuad(alpha);
}

//////////////////////////////////////////////////////////////////////
// Chargement d'un répertoire
//////////////////////////////////////////////////////////////////////
void COpenGLDiaporama::DrawObject()									// Here's Where We Do All The Drawing
{
	float niveau0 = -15.0f;
	float niveau1 = -30.0f;
	float niveau2 = -45.0f;
	float niveau3 = -60.0f;

	float xposNiveau0 = 0.0f;
	float xposNiveau1 = 15.0f;
	float xposNiveau2 = 30.0f;
	float xposNiveau3 = 45.0f;

	float fAlphaNiveau1 = 0.66f;
	float fAlphaNiveau2 = 0.33f;
	float fAlphaNiveau3 = 0.0f;
	
	float fAlphaNiveauZero = 1.0f;

	float fAlphaNiveauPlus3 = 0.0f;
	float fAlphaNiveauPlus1 = 0.66f;// + fAlphaNiveau3;
	float fAlphaNiveauPlus2 = 0.33f;// + fAlphaNiveau3;

	float fAlphaNiveauMoins3 = 0.0f;// - (0.33f * (float)iNumFrame) / (float)iNbFrame;
	float fAlphaNiveauMoins1 = 0.66f;// - fAlphaNiveau3;
	float fAlphaNiveauMoins2 = 0.33f;// - fAlphaNiveau3;

	float fFrameDepXValue = 0.0;
	float fFrameDepZValue = 0.0;
	float depX = xposNiveau0 + fFrameDepXValue;
	float depZ = niveau0 + fFrameDepZValue;

	glEnable(GL_ALPHA_TEST);
	glPushMatrix();

	if(bAvance)
	{
		fFrameDepXValue = (-15.0f * (float)iNumFrame) / (float)iNbFrame;
		fFrameDepZValue = (-15.0f * (float)iNumFrame) / (float)iNbFrame;
		depX = xposNiveau0 + fFrameDepXValue;
		depZ = niveau0 + fFrameDepZValue;
		fAlphaNiveauPlus3 = (0.33f * (float)iNumFrame) / (float)iNbFrame;
		fAlphaNiveauPlus1 = 0.66f + fAlphaNiveauPlus3;
		fAlphaNiveauPlus2 = 0.33f + fAlphaNiveauPlus3;

		fAlphaNiveauMoins3 = 0.0f;//(0.33f * (float)iNumFrame) / (float)iNbFrame;
		fAlphaNiveauMoins1 = 0.66f - fAlphaNiveauPlus3;
		fAlphaNiveauMoins2 = 0.33f - fAlphaNiveauPlus3;

		fAlphaNiveauZero = 1.0f - fAlphaNiveauPlus3;
	}

	if(bRecule)
	{
		fFrameDepXValue = (15.0f * (float)iNumFrame) / (float)iNbFrame;
		fFrameDepZValue = (15.0f * (float)iNumFrame) / (float)iNbFrame;
		depX = xposNiveau0 + fFrameDepXValue;
		depZ = niveau0 - fFrameDepZValue;
		fAlphaNiveauPlus3 = (0.33f * (float)iNumFrame) / (float)iNbFrame;
		fAlphaNiveauPlus1 = 0.66f - fAlphaNiveauPlus3;
		fAlphaNiveauPlus2 = 0.33f - fAlphaNiveauPlus3;

		fAlphaNiveauMoins3 = fAlphaNiveauPlus3;
		fAlphaNiveauMoins1 = 0.66f + fAlphaNiveauPlus3;
		fAlphaNiveauMoins2 = 0.33f + fAlphaNiveauPlus3;

		fAlphaNiveauZero = 1.0f - fAlphaNiveauPlus3;
		fAlphaNiveauPlus3 = 0.0f;
	}



	////////////////////////////////////////////////////////////////////
	//Affichage de la première image
	////////////////////////////////////////////////////////////////////

	//Effet de Rotation
	DrawGLPicturePlus(depX, depZ+m_iNiveauZoom, iNumTexture,fAlphaNiveauZero,m_bRotation,true);

	////////////////////////////////////////////////////////////////////
	//Photo - 1
	depX = -xposNiveau1 + fFrameDepXValue;
	depZ = niveau1 + fFrameDepZValue;
	DrawGLPictureMoins(depX, depZ, iNumTexture-1,fAlphaNiveauMoins1);


	////////////////////////////////////////////////////////////////////
	//Photo + 1
	depX = xposNiveau1 + fFrameDepXValue;
	depZ = niveau1 - fFrameDepZValue;
	DrawGLPicturePlus(depX, depZ, iNumTexture+1,fAlphaNiveauPlus1);

	////////////////////////////////////////////////////////////////////
	//Photo - 2

	depX = -xposNiveau2 + fFrameDepXValue;
	depZ = niveau2 + fFrameDepZValue;
	DrawGLPictureMoins(depX, depZ, iNumTexture-2,fAlphaNiveauMoins2);

	////////////////////////////////////////////////////////////////////
	//Photo + 2

	depX = xposNiveau2 + fFrameDepXValue;
	depZ = niveau2 - fFrameDepZValue;
	DrawGLPicturePlus(depX, depZ, iNumTexture+2,fAlphaNiveauPlus2);

	////////////////////////////////////////////////////////////////////
	//Photo - 3
	depX = -xposNiveau3 + fFrameDepXValue;
	depZ = niveau3 + fFrameDepZValue;
	DrawGLPictureMoins(depX, depZ, iNumTexture-3,fAlphaNiveauMoins3);

	////////////////////////////////////////////////////////////////////
	//Photo + 3

	depX = xposNiveau3 + fFrameDepXValue;
	depZ = niveau3 - fFrameDepZValue;
	DrawGLPicturePlus(depX, depZ, iNumTexture+3,fAlphaNiveauPlus3);

	glPopMatrix();

	if(bAvance && m_fZoomDepart == 0.0f)
	{
		//iNumFrame++;
		if(iNumFrame >= iNbFrame)
		{
			iNumFrame = 0;
			iNumTexture++;
			iNumTexture = iNumTexture % iNbFile;
			bAvance = false;
			if(m_bDiaporama)
				PostThreadMessage(threadID,IDM_IMAGES_SUIVANTE,0,0);

		}
	}
	else if(bRecule && m_fZoomDepart == 0.0f)
	{
		//iNumFrame++;
		if(iNumFrame >= iNbFrame)
		{
			iNumFrame = 0;
			bRecule = false;
			iNumTexture--;
			if(iNumTexture < 0)
				iNumTexture = iNbFile-1;	

			if(m_bDiaporama)
				PostThreadMessage(threadID,IDM_IMAGES_SUIVANTE,0,0);
		}
	}
	glDisable(GL_ALPHA_TEST);
									// Keep Going
}



////////////////////////////////////////////////////////////////////////////////////////
//Draw The Scene
////////////////////////////////////////////////////////////////////////////////////////

BOOL COpenGLDiaporama::DrawScene(BOOL &done)
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

int COpenGLDiaporama::InitGL()								
{
	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping ( NEW )

	glViewport(0 , 0, m_stOpenGLData->screen_width ,m_stOpenGLData->screen_height);	// Set Up A Viewport
	glMatrixMode(GL_PROJECTION);								// Select The Projection Matrix
	glLoadIdentity();											// Reset The Projection Matrix
	gluPerspective(50, (float)m_stOpenGLData->screen_width/(float)m_stOpenGLData->screen_height, 5,  2000); // Set Our Perspective
	glMatrixMode(GL_MODELVIEW);									// Select The Modelview Matrix
	glLoadIdentity();											// Reset The Modelview Matrix

	glEnable(GL_DEPTH_TEST);									// Enable Depth Testing
	glEnable(GL_SRC_ALPHA);
	glShadeModel(GL_SMOOTH);									// Select Smooth Shading

	glClearColor(0.0f, 0.0f, 0.0f, 0.5);						// Set The Clear Color To Black

	/*
	for(int x=0; x<60; x++)
	{
		for(int y=0; y<45; y++)
		{
			points[x][y][0]=float((x/5.0f)-4.5f)-1.5f;
			points[x][y][1]=float((y/5.0f)-4.5f);
			points[x][y][2]=0.0f;//float(sin((((x/5.0f)*40.0f)/360.0f)*3.141592654*2.0f));
			
		}
	}*/


	return TRUE;	
}

/**********************************************************
 *
 * SUBROUTINE resize(int,int)
 *
 * This routine must be called everytime we resize our window.
 * 
 *********************************************************/

void COpenGLDiaporama::resize()
{
	// Save previous context and make our context current
	HDC mhOldDC = wglGetCurrentDC();
	HGLRC mhOldGLRC = wglGetCurrentContext();
	wglMakeCurrent(m_stOpenGLData->hDC, m_stOpenGLData->hRC);

	// Reset the current viewport
	glViewport(0, 0, m_stOpenGLData->screen_width, m_stOpenGLData->screen_height);
	glMatrixMode (GL_PROJECTION);										// Select The Projection Matrix
	glLoadIdentity ();													// Reset The Projection Matrix
	gluPerspective(50, (float)m_stOpenGLData->screen_width/(float)m_stOpenGLData->screen_height, 5,  2000);
	glMatrixMode (GL_MODELVIEW);										// Select The Modelview Matrix
	glLoadIdentity ();	

	// Restore previous context
	wglMakeCurrent(mhOldDC, mhOldGLRC);

}

////////////////////////////////////////////////////////////////////////////////////////
// Fonction d'affichage
////////////////////////////////////////////////////////////////////////////////////////

void COpenGLDiaporama::DrawGLScene()								
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	DrawObject();
	glFlush();
}

////////////////////////////////////////////////////////////////////////////////////////
// Properly Kill The Window
////////////////////////////////////////////////////////////////////////////////////////

GLvoid COpenGLDiaporama::KillGLWindow()							
{
	DeleteAllGLTextures();
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(m_stOpenGLData->hRC);
	DestroyWindow(m_hWnd);

}

////////////////////////////////////////////////////////////////////////////////////////
// Create OpenGL Window
////////////////////////////////////////////////////////////////////////////////////////

// Set up the pixel format
int COpenGLDiaporama::MySetPixelFormat()
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

LRESULT COpenGLDiaporama::OnCreate(HWND hWnd,UINT uMsg, WPARAM wParam,LPARAM lParam)
{
	//m_stOpenGLData->active=TRUE;

	m_stOpenGLData->hWndStatic = CreateWindowEx(0,"STATIC","", WS_CHILD,
			 0, 0, 0,0,
			hWnd,
			 (HMENU)NULL,
			 hInstance,
			 NULL);

	this->m_hWnd = hWnd;

	m_stOpenGLData->hDC = GetDC(m_stOpenGLData->hWndStatic);

	MySetPixelFormat();

	InitGL();

	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGLDiaporama::OnActivate(HWND hWnd,UINT uMsg, WPARAM wParam,LPARAM	lParam)
{
	if (!HIWORD(wParam))					// Check Minimization State
		m_stOpenGLData->active=TRUE;					// Program Is Active
	else
		m_stOpenGLData->active=FALSE;					// Program Is No Longer Active

	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGLDiaporama::OnShowWindow(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
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
	
	return 1;

}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGLDiaporama::OnSysCommand(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (wParam)						// Check System Calls
	{
		case SC_SCREENSAVE:				// Screensaver Trying To Start?
		case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
		return 0;					// Prevent From Happening
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COpenGLDiaporama::InitRotationValue()
{
	m_bSensRotation = false;
	m_bRotation = false;
	m_bRotationEnd = true;
	m_bModifieZoom = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGLDiaporama::OnCommand(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	int wmId, wmEvent;
	wmId    = LOWORD(wParam); 
	wmEvent = HIWORD(wParam); 

	switch(wmId)
	{
		case IDM_IMAGES_ZOOM_ZOOMIN:
			{
				m_bModifieZoom = true;
				m_fZoomDepart += m_fPasZoom;
			}
			return 1;

		case IDM_IMAGES_ZOOM_ZOOMOUT:
			{
				m_bModifieZoom = true;
				m_fZoomDepart -= m_fPasZoom;
			}
			return 1;

		case IDM_IMAGES_SUIVANTE:
			Avance();
			return 1;

		case IDM_IMAGES_PRCDENTE:
			Recule();
			return 1;

		case ID_IMAGES_VISUALISATION_PREMIREIMAGE:
			{
				this->iNumTexture = 0;
				this->ReloadRepertoire();
			}
			return 1;

		case ID_IMAGES_VISUALISATION_DERNIREIMAGE:
			return 1;

		case ID_IMAGES_STARTDIAPORAMA:
			{
				m_bDiaporama = true;
				PostThreadMessage(threadID,IDM_IMAGES_SUIVANTE,0,0);
			}
			return 1;

		case ID_IMAGES_STOPDIAPORAMA:
			m_bDiaporama = false;
			return 1;

		case IDM_IMAGES_ROTATEIMAGE_180:
			{
				if(m_bRotationEnd)
				{
					m_fDegreEnd = (m_fDegreStart + 180.0f);
					InitRotationValue();
					m_bSensRotation = true;
					m_bRotation = true;
				}
			}
			return 1;

		case IDM_IMAGES_ROTATEIMAGE_90CW:
			{
				if(m_bRotationEnd)
				{
					m_fDegreEnd = (m_fDegreStart + 90.0f);
					InitRotationValue();
					m_bSensRotation = true;
					m_bRotation = true;
				}
			}
			return 1;

		case IDM_IMAGES_ROTATEIMAGE_90CCW:
			{
				if(m_bRotationEnd)
				{
					m_fDegreEnd = (m_fDegreStart - 90.0f);
					InitRotationValue();
					m_bSensRotation = false;
					m_bRotation = true;
				}
			}
			return 1;

		case IDM_IMAGES_ROTATEIMAGE_FLIPHORIZONTAL:
			{
			}
			return 1;

		case IDM_IMAGES_ROTATEIMAGE_FLIPVERTICAL:
			{
				if(m_bRotationEnd)
				{
					m_fDegreEnd = (m_fDegreStart + 180.0f);
					InitRotationValue();
					m_bSensRotation = true;
					m_bRotation = true;
				}
			}
			return 1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


LRESULT COpenGLDiaporama::OnEraseBackground(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	return OnPaint(hWnd,uMsg,wParam,lParam);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGLDiaporama::OnPaint(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	this->m_hWnd = hWnd;
	float CurrentTime = 0; //Temps courant pour le calcul de Frames

	DWORD tickCount = GetTickCount ();				// Get The Tick Count

	if(bAvance || bRecule)
	{
		DWORD milliseconds = tickCount - lastTickCount;

		//milliseconds = 200;

		//float m_dNbTimeFrame = (1000.0f / (float)iNbFrame);
		
		//iNumFrame += milliseconds / 16;

		char Temp[255];
		sprintf(Temp,"NbFrame : %d \n",milliseconds);
		OutputDebugString(Temp);

		//iNumFrame = iNumFrame % iNbFrame;
		//iNumFrame++;

		if(milliseconds < 15)
		{
			iNumFrame += 1 + m_iNbFrameSup;
			Sleep(15 - milliseconds);
		}
		else
		{
			iNumFrame += (milliseconds / 15) + m_iNbFrameSup;
		}
	}

	
	
	lastTickCount = tickCount;	

	//PAINTSTRUCT ps;

	HDC hdc = GetDC(hWnd);

	HBITMAP hBitmap;

	RECT rt;
	GetClientRect(hWnd, &rt);

	rt.bottom = 1;

	hBitmap = CreateCompatibleBitmap(hdc,rt.right,1);

	HDC sourceDC;
	sourceDC = CreateCompatibleDC(hdc);

	SetBkMode(sourceDC, TRANSPARENT); 

	HBITMAP hbmOldSource = (HBITMAP)::SelectObject( sourceDC, hBitmap);

	FillRect(sourceDC, &rt, (HBRUSH)GetStockObject(BLACK_BRUSH)); 

	BitBlt(hdc,0,0, rt.right,1, sourceDC, 0, 0,SRCCOPY);

	DeleteDC(sourceDC);

	ReleaseDC(hWnd,hdc);

	DeleteObject(hbmOldSource);

	DeleteObject(hBitmap);

    HDC mhOldDC = wglGetCurrentDC();
    HGLRC mhOldGLRC = wglGetCurrentContext();
	wglMakeCurrent(m_stOpenGLData->hDC, m_stOpenGLData->hRC);
	DrawGLScene();
	SwapBuffers(m_stOpenGLData->hDC);
	wglMakeCurrent(mhOldDC, mhOldGLRC);
	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGLDiaporama::OnClose(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	DestroyWindow(hWnd);
	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGLDiaporama::OnKeyDown(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	ProcessKey(hWnd,uMsg,wParam,lParam);
	m_stOpenGLData->keys[wParam] = TRUE;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGLDiaporama::OnKeyUp(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	m_stOpenGLData->keys[wParam] = FALSE;					// If So, Mark It As FALSE
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGLDiaporama::OnLButtonDown(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGLDiaporama::OnLButtonUp(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


LRESULT COpenGLDiaporama::OnMouseWheel(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	long zDelta;
	zDelta = HIWORD(wParam);
	if(zDelta == 120)
		Recule();
	else
		Avance();
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGLDiaporama::OnMouseMove(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGLDiaporama::OnSize(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
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

		MoveWindow(m_stOpenGLData->hWndStatic,0,1,m_stOpenGLData->screen_width,m_stOpenGLData->screen_height,true);
	
	}
	else
	{
		RECT rt;
		GetClientRect(hWnd, &rt);

		m_stOpenGLData->screen_width = rt.right;
		m_stOpenGLData->screen_height = rt.bottom;

		MoveWindow(m_stOpenGLData->hWndStatic,0,1,m_stOpenGLData->screen_width,m_stOpenGLData->screen_height,false);
	}

	if(::IsWindowVisible(m_stOpenGLData->hWndStatic) && !m_stOpenGLData->fullscreen)
		resize();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT COpenGLDiaporama::OnDestroy(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{

	ReleaseDC(m_stOpenGLData->hWndStatic, m_stOpenGLData->hDC);
	PostQuitMessage(0);	
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// On passe à l'image suivante
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int COpenGLDiaporama::Avance()
{
	InitNavigationValue();
	//m_fZoomDepart = 0;
	if(!bAvance)
	{
		if(iNumFrame != 0)
		{
			iNumFrame = iNbFrame - iNumFrame;
			if(iNumFrame != 0)
			{
				iNumTexture = iNumTexture - 1;
				if(iNumTexture < 0)
					iNumTexture = iNbFile - 1;
			}
		}

		bAvance = true;
		bRecule = false;
		
		//if(iNumFrame == 0)
		PrechargePhoto();
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// On passe à l'image précédente
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COpenGLDiaporama::InitNavigationValue()
{
	m_bModifieZoom = false;
	m_bRotation = false;
	m_fDegreStart = 0;
	m_iNiveauZoom = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// On passe à l'image précédente
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int COpenGLDiaporama::Recule()
{
	InitNavigationValue();
	//m_fZoomDepart = 0;
	if(!bRecule)
	{
		if(iNumFrame != 0)
		{
			iNumFrame = iNbFrame - iNumFrame;
			if(iNumFrame != 0)
			{
				iNumTexture = iNumTexture + 1;
				iNumTexture = iNumTexture % iNbFile;
			}
		}
		bRecule = true;
		bAvance = false;
		
		//if(iNumFrame == 0)
		PrechargePhoto();
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gestion des touches
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int COpenGLDiaporama::ProcessKey(HWND	hWnd,				// Handle For This Window
		UINT	uMsg,					// Message For This Window
		WPARAM	wParam,					// Additional Message Information
		LPARAM	lParam)					// Additional Message Information
{
	switch(wParam)
	{
		case VK_UP:
			//m_stOpenGLData->xRot+=0.5f;
			break;

		case VK_DOWN:
			//m_stOpenGLData->xRot-=0.5f;
			break;

		case VK_LEFT:
			{
				if(m_stOpenGLData->keys[wParam] == TRUE)
				{
					//Augmentation de la vitesse
					m_iNbFrameSup++;
				}
				else
				{
					m_iNbFrameSup = 0;
				}

				char Temp[255];
				sprintf(Temp,"m_iNbFrameSup : %d \n",m_iNbFrameSup);
				OutputDebugString(Temp);

				Recule();
			}
			break;

		case VK_RIGHT:
			{
				if(m_stOpenGLData->keys[wParam] == TRUE)
				{
					//Augmentation de la vitesse
					m_iNbFrameSup++;
				}
				else
				{
					m_iNbFrameSup = 0;
				}

				char Temp[255];
				sprintf(Temp,"m_iNbFrameSup : %d \n",m_iNbFrameSup);
				OutputDebugString(Temp);

				Avance();
			}
			break;
	
		default:
			return 0;

	}
	return 1;
}

