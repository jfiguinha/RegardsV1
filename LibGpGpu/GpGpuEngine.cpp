#include "StdAfx.h"
#include "GpGpuEngine.h"
#include <stdio.h>

#ifdef GPGPU_CG

CGcontext m_cgContext;
CGprofile m_cgProfile;

// Called when Cg detects an error
void CGpGpuEngine::cgErrorCallback()
{
  CGerror lastError = cgGetError();
  
  if(lastError)
  {
#ifdef _DEBUG
	  char Error[255];
	  sprintf_s(Error,"cgGetErrorString : %s \n", cgGetErrorString(lastError));
	  OutputDebugString(Error);
	  sprintf_s(Error,"cgGetLastListing : %s \n", cgGetLastListing(m_cgContext));
	  OutputDebugString(Error);
#endif
  }
} 

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
CGpGpuEngine::CGpGpuEngine(void)
{
	cgSetErrorCallback(cgErrorCallback);
	m_cgContext = cgCreateContext();
    
	// get the best profile for this hardware
	m_cgProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
	cgGLSetOptimalOptions(m_cgProfile); 

	_outputdata = NULL;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
CGpGpuEngine::~CGpGpuEngine(void)
{
    cgDestroyProgram(_fragmentProgram);
	if(_outputdata != NULL)
	{
		delete[] _outputdata;
		_outputdata = NULL;
	}
	DeleteGLTextures();
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
void CGpGpuEngine::InitializeEngine(const char * cgSource, const char * cgFunction)
{
    // Create the edge detection fragment program
    _fragmentProgram = cgCreateProgram(m_cgContext, CG_SOURCE,
                                       cgSource, m_cgProfile,
                                       cgFunction, NULL);

    // Create the texture parameter for the fragment program
    if(_fragmentProgram != NULL)
    {
        cgGLLoadProgram(_fragmentProgram);
        _textureParam = cgGetNamedParameter(_fragmentProgram, "texture");
    }
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
void CGpGpuEngine::InitValue(int w, int h, int colorSize, BYTE * data)
{
	_iWidth = w;
	_data = data;
	_iHeight = h;
	_iColorSize = colorSize;

	if(_outputdata != NULL)
		delete[] _outputdata;

	_outputdata = new BYTE[colorSize * _iWidth * _iHeight];

	//Chargement de la texture pour l'image
	glEnable(GL_TEXTURE_RECTANGLE_ARB);

	glGenTextures (1, &texture[0]); 
	// bind texture x
	glBindTexture( GL_TEXTURE_RECTANGLE_ARB, texture[0] );

	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);

	//glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 4, _iWidth, _iHeight, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	// define texture with floating point format
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB,0, GL_RGB8, _iWidth,_iHeight,0,GL_BGRA_EXT,GL_UNSIGNED_BYTE,0);
	// attach texture
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB,texture[0],0);
	// transfer data to texture
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB,0,0,0,_iWidth,_iHeight, GL_BGRA_EXT,GL_UNSIGNED_BYTE, data);
}

/////////////////////////////////////////////////////////////////////
//Suppression de la texture
//////////////////////////////////////////////////////////////////////
void CGpGpuEngine::DeleteGLTextures()
{
	glDeleteTextures( 1, &texture[0]);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
}


// This method updates the texture by rendering the geometry (a teapot 
// and 3 rotating tori) and copying the image to a texture.  
// It then renders a second pass using the texture as input to an edge 
// detection filter.  It copies the results of the filter to the texture.
// The texture is used in HelloGPGPU::display() for displaying the 
// results.
void CGpGpuEngine::Compute(const int & iNbTime)
{    
	// Set up the viewport for 1:1 pixel to texel mapping 
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	gluOrtho2D(0, _iWidth, 0, _iHeight); 
	glViewport(0, 0, _iWidth, _iHeight); 
	glEnable(GL_TEXTURE_RECTANGLE_ARB);      

    // run the edge detection filter over the geometry texture
    // Activate the edge detection filter program
    cgGLBindProgram(_fragmentProgram);
    cgGLEnableProfile(m_cgProfile);
        
	for(int i = 0;i < iNbTime;i++)
	{
		// bind the scene texture as input to the filter
		cgGLSetTextureParameter(_textureParam, texture[0]);
		cgGLEnableTextureParameter(_textureParam);
		cgSetParameter1f(_widthParam, (float)_iWidth);
		cgSetParameter1f(_heightParam, (float)_iHeight);

		// GPGPU CONCEPT 4: Viewport-Sized Quad = Data Stream Generator.
		// In order to execute fragment programs, we need to generate pixels.
		// Drawing a quad the size of our viewport (see above) generates a 
		// fragment for every pixel of our destination texture. Each fragment
		// is processed identically by the fragment program. Notice that in 
		// the reshape() function, below, we have set the frustum to 
		// orthographic, and the frustum dimensions to [-1,1].  Thus, our 
		// viewport-sized quad vertices are at [-1,-1], [1,-1], [1,1], and 
		// [-1,1]: the corners of the viewport.
		// run the program

		// make quad filled to hit every pixel/texel
		glPolygonMode(GL_FRONT,GL_FILL);
		// and render quad
		glBegin(GL_QUADS);
			glTexCoord2f(0.0, 0.0); 
			glVertex2f(0.0, 0.0);
			glTexCoord2f(_iWidth, 0.0); 
			glVertex2f(_iWidth, 0.0);
			glTexCoord2f(_iWidth, _iHeight); 
			glVertex2f(_iWidth, _iHeight);
			glTexCoord2f(0.0, _iHeight); 
			glVertex2f(0.0, _iHeight);
		glEnd();
	    
		// disable the filter
		cgGLDisableTextureParameter(_textureParam);
		cgGLDisableProfile(m_cgProfile);

	    
		// GPGPU CONCEPT 5: Copy To Texture (CTT) = Feedback.
		// We have just invoked our computation (edge detection) by applying 
		// a fragment program to a viewport-sized quad. The results are now 
		// in the frame buffer. To store them, we copy the data from the 
		// frame buffer to a texture.  This can then be fed back as input
		// for display (in this case) or more computation (see 
		// more advanced samples.)

		// update the texture again, this time with the filtered scene
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texture[0]);
		glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, 0, 0, _iWidth, _iHeight);
	}
	glGetTexImage(GL_TEXTURE_RECTANGLE_ARB, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE,_outputdata);
}

BYTE * CGpGpuEngine::GetOutputData()
{
	return _outputdata;
}

#endif