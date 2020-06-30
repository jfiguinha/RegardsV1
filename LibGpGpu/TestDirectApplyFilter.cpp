//---------------------------------------------------------------------------
//                               www.GPGPU.org 
//                                Sample Code
//---------------------------------------------------------------------------
// Copyright (c) 2004 Mark J. Harris and GPGPU.org
//---------------------------------------------------------------------------
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any
// damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any
// purpose, including commercial applications, and to alter it and
// redistribute it freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you
//    must not claim that you wrote the original software. If you use
//    this software in a product, an acknowledgment in the product
//    documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and
//    must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//---------------------------------------------------------------------------
// Author: Mark Harris (harrism@gpgpu.org)
//---------------------------------------------------------------------------
// GPGPU Lesson 0: "helloGPGPU".
//---------------------------------------------------------------------------
//
// GPGPU CONCEPTS Introduced: 
//
//      1.) Texture = Array
//      2.) Fragment Program = Computational Kernel.
//      3.) One-to-one Pixel to Texel Mapping: 
//          a) Data-Dimensioned Viewport, and
//          b) Orthographic Projection.
//      4.) Viewport-Sized Quad = Data Stream Generator.
//      5.) Copy To Texture = feedback.
//
//      For details of each of these concepts, see the explanations in the 
//      inline "GPGPU CONCEPT" comments in the code below.
//
// APPLICATION Demonstrated: A simple post-process edge detection filter. 
//
//---------------------------------------------------------------------------
#include <stdafx.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#define GLEW_STATIC 1
#include <gl/glew.h>
#include <gl/glaux.h>											// Header File For The GLaux Library
#include <gl/glut.h>
#include <cg/cgGL.h>

//#define GL_TEXTURE_2D_USED

// forward declarations
class HelloGPGPU;
void reshape(int w, int h);

// globals
bool		g_bFirstTime = true;
CGcontext   g_cgContext;
CGprofile   g_cgProfile;
HelloGPGPU  *g_pHello;

//////////////////////////////////////////////////////////////////////
//Load Bmp File to create a texture
//////////////////////////////////////////////////////////////////////
AUX_RGBImageRec * LoadBMP(char *Filename)
{
	FILE *File=NULL;									// File Handle

	if (!Filename)										// Make Sure A Filename Was Given
	{
		return NULL;									// If Not Return NULL
	}

	File=fopen(Filename,"r");							// Check To See If The File Exists

	if (File)											// Does The File Exist?
	{
		fclose(File);									// Close The Handle
		return auxDIBImageLoad(Filename);				// Load The Bitmap And Return A Pointer
	}

	return NULL;
}

void SaveBitmapToFile( BYTE* pBitmapBits, LONG lWidth, LONG lHeight,WORD wBitsPerPixel, LPCTSTR lpszFileName )
{
    BITMAPINFOHEADER bmpInfoHeader = {0};
    // Set the size
    bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    // Bit count
    bmpInfoHeader.biBitCount = wBitsPerPixel;
    // Use all colors
    bmpInfoHeader.biClrImportant = 0;
    // Use as many colors according to bits per pixel
    bmpInfoHeader.biClrUsed = 0;
    // Store as un Compressed
    bmpInfoHeader.biCompression = BI_RGB;
    // Set the height in pixels
    bmpInfoHeader.biHeight = lHeight;
    // Width of the Image in pixels
    bmpInfoHeader.biWidth = lWidth;
    // Default number of planes
    bmpInfoHeader.biPlanes = 1;
    // Calculate the image size in bytes
    bmpInfoHeader.biSizeImage = lWidth* lHeight * (wBitsPerPixel/8); 

    BITMAPFILEHEADER bfh = {0};
    // This value should be values of BM letters i.e 0×4D42
    // 0×4D = M 0×42 = B storing in reverse order to match with endian
    bfh.bfType=0x4D42;
    /* or
    bfh.bfType = ‘B’+(’M’ << 8);
    // <<8 used to shift ‘M’ to end
    */
    // Offset to the RGBQUAD
    bfh.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
    // Total size of image including size of headers
    bfh.bfSize = bfh.bfOffBits + bmpInfoHeader.biSizeImage;
    // Create the file in disk to write
    HANDLE hFile = CreateFile( lpszFileName,GENERIC_WRITE, 0,NULL,

                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL); 

    if( !hFile ) // return if error opening file
    {
        return;
    } 

    DWORD dwWritten = 0;
    // Write the File header
    WriteFile( hFile, &bfh, sizeof(bfh), &dwWritten , NULL );
    // Write the bitmap info header
    WriteFile( hFile, &bmpInfoHeader, sizeof(bmpInfoHeader), &dwWritten, NULL );
    // Write the RGB Data
    WriteFile( hFile, pBitmapBits, bmpInfoHeader.biSizeImage, &dwWritten, NULL );
    // Close the file handle
    CloseHandle( hFile );
}

char * filename = "C:\\developpement\\gpgpu\\hello_gpgpu\\helloGPGPU-1.0.2\\Debug\\data\\ZC4F1059.bmp";

/*----------------------------------------------------------------------*/
// This shader performs a 9-tap Laplacian edge detection filter.
//
//	This is the actual cg code that will be compiled at runtime
//	and loaded into the graphic card, fragment processor
//
//	tex2D returns the value of the texture at coordinates
//
//	the laplacian filter implemented is a 3x3 filter with the 
//	following coefficients:
//
//		1	1	1		tl	t	tr
//		1	8	1		l	c	r
//		1	1	1		bl	b	br
//
//
static const char *strongBlurFragSource = 
"void tonemappingProgram(                                   \n"
" float2 texcoord :sdfsdf TEXCOORD0,                     \n"
"out float4 color : COLOR,                                                                 \n"
"uniform samplerRECT sceneTexture){                        \n"       
"float4 scene = texRECT(sceneTexture, texcoord / 4.0);                             \n"
"color = scene;   \n"
"if(color.x ==0 && color.y == 0 && color.z ==0){   \n"
"color.x = 0;   \n"
"color.y = 1;   \n"
"color.z = 0;   \n"
"}   \n"
"}                                                                  \n";

/*----------------------------------------------------------------------*/
// This shader performs a 9-tap Laplacian edge detection filter.
//
//	This is the actual cg code that will be compiled at runtime
//	and loaded into the graphic card, fragment processor
//
//	tex2D returns the value of the texture at coordinates
//
//	the laplacian filter implemented is a 3x3 filter with the 
//	following coefficients:
//
//		-1	-1	-1		tl	t	tr
//		-1	 8	-1		l	c	r
//		-1	-1	-1		bl	b	br
//
//
//
// fragment program: out = accum + vec
//
const char* kernelSource = "float4 saxpy (		\n"
"    float2 coords : TEXCOORD0,					\n"
"    uniform samplerRECT texture) : COLOR0{		\n"
"    float4 result;								\n"
"    float4 y = texRECT(texture,coords);		\n"
"    result = y / 2.0f;							\n"
"    return result;								\n"
"}												\n";

static const char * BlurFragSource = 
"float4 blur(float2 coords : TEXCOORD0,									\n"
"uniform float width,													\n"
"uniform float height,													\n"
"uniform samplerRECT texture) : COLOR0									\n"
"{																		\n"
"    static const float wOffset = 1.0;							\n"       
"    static const float hOffset = 1.0;							\n"    
"    float4 c  = texRECT(texture, coords);                              \n"
"    float4 bl = texRECT(texture, coords + float2(-wOffset, -hOffset)); \n"
"    float4 l  = texRECT(texture, coords + float2(-wOffset,       0));  \n"
"    float4 tl = texRECT(texture, coords + float2(-wOffset,  hOffset)); \n"
"    float4 t  = texRECT(texture, coords + float2(      0,  hOffset));  \n"
"    float4 ur = texRECT(texture, coords + float2( wOffset,  hOffset)); \n"
"    float4 r  = texRECT(texture, coords + float2( wOffset,       0));  \n"
"    float4 br = texRECT(texture, coords + float2( wOffset,  hOffset)); \n"
"    float4 b  = texRECT(texture, coords + float2(      0, -hOffset));  \n"
"    // scale by 8 to brighten the edges								\n"
"    //return 8 * (c + -0.125 * (bl + l + tl + t + ur + r + br + b));	\n"
"    return (c + bl + l + tl + t + ur + r + br + b) / 8.0;	\n"
"}																		\n";

// This class encapsulates all of the GPGPU functionality of the example.
class HelloGPGPU
{
public: // methods
    HelloGPGPU()
    {
        // Create a simple 2D texture.  This example does not use
        // render to texture -- it just copies from the framebuffer to the
        // texture.

        // GPGPU CONCEPT 1: Texture = Array.
        // Textures are the GPGPU equivalent of arrays in standard 
        // computation. Here we allocate a texture large enough to fit our
        // data (which is arbitrary in this example).
       
        // GPGPU CONCEPT 2: Fragment Program = Computational Kernel.
        // A fragment program can be thought of as a small computational 
        // kernel that is applied in parallel to many fragments 
        // simultaneously.  Here we load a kernel that performs an edge 
        // detection filter on an image.

		cgSetErrorCallback(cgErrorCallback);
		g_cgContext = cgCreateContext();
	    
		// get the best profile for this hardware
		g_cgProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
		assert(g_cgProfile != CG_PROFILE_UNKNOWN);
		cgGLSetOptimalOptions(g_cgProfile); 

        // Create the edge detection fragment program
        _fragmentProgram = cgCreateProgram(g_cgContext, CG_SOURCE,
                                           BlurFragSource, g_cgProfile,
                                           "blur", NULL);

        // Create the texture parameter for the fragment program
        if(_fragmentProgram != NULL)
        {
            cgGLLoadProgram(_fragmentProgram);
            _textureParam = cgGetNamedParameter(_fragmentProgram, "texture");
			_widthParam = cgGetNamedParameter(_fragmentProgram, "width");
			_heightParam = cgGetNamedParameter(_fragmentProgram, "height");
        }

		

		_outputdata = NULL;
    }

	~HelloGPGPU()
    {
        cgDestroyProgram(_fragmentProgram);
		delete[] _outputdata;
		//suppression de la texture
    }

	int InitValue(int w, int h, BYTE * data)
	{
		_iWidth = w;
		_data = data;
		_iHeight = h;

		if(_outputdata != NULL)
			delete[] _outputdata;

		_outputdata = new BYTE[3*_iWidth*_iHeight];

		//Chargement de la texture pour l'image
		SetGLTextures();
	}


	/////////////////////////////////////////////////////////////////////
	//Application de la texture
	//////////////////////////////////////////////////////////////////////
	int SetGLTextures()
	{
		glEnable(GL_TEXTURE_RECTANGLE_ARB);

		glGenTextures (1, &texture[0]); 
		// bind texture x
		glBindTexture( GL_TEXTURE_RECTANGLE_ARB, texture[0] );

		// set texture parameters for x
		glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        
		// transfer data x to texture x
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB8, _iWidth, _iHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, _data);

		return 0;
	}

	/////////////////////////////////////////////////////////////////////
	//Suppression de la texture
	//////////////////////////////////////////////////////////////////////
	void DeleteGLTextures()
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
    void update()
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
        cgGLEnableProfile(g_cgProfile);
            
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
        cgGLDisableProfile(g_cgProfile);

        
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
		glGetTexImage(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, GL_UNSIGNED_BYTE,_outputdata);
		
		int iPos = 0;
		for(int y = 0;y < _iHeight;y++)
		{
			for(int x = 0;x < _iWidth;x++)
			{
				BYTE R = _outputdata[iPos];
				BYTE G = _outputdata[iPos+1];
				BYTE B = _outputdata[iPos+2];

				_outputdata[iPos] = B;
				_outputdata[iPos+1] = G;
				_outputdata[iPos+2] = R;

				iPos += 3;
			}
		}
    }

	BYTE * GetOutputData()
	{
		return _outputdata;
	}

	// Called when Cg detects an error
	static void cgErrorCallback()
	{
	  CGerror lastError = cgGetError();
	  
	  if(lastError)
	  {
		printf("%s\n\n", cgGetErrorString(lastError));
		printf("%s\n", cgGetLastListing(g_cgContext));
		printf("Cg error!\n");
	  }
	} 

protected: // data
    int           _iWidth, _iHeight; // The dimensions of our array
    float         _rAngle;           // used for animation
    BYTE *		  _data;
	BYTE *		  _outputdata;
    unsigned int  _iTexture;         // The texture used as a data array
	GLuint	texture[1];			// Storage For One Texture ( NEW )
    CGprogram     _fragmentProgram;  // the fragment program used to update
    CGparameter   _textureParam, _widthParam, _heightParam;     // a parameter to the fragment program

};

//HelloGPGPU * g_pHello;// = new HelloGPGPU();

int GetPixelPosition(int x, int y, int width)
{
	return (x  + (y * width)) * 3;
}

void DoEffect(int iNbY, int iNbX, int iYSizeBlock, int iXSizeBlock,int m_iXPos, int m_iYPos, BYTE * & bPictureData, int width)
{
	
	int size = iXSizeBlock * iYSizeBlock * 3 * sizeof(BYTE);
	int linesize = iXSizeBlock * 3 * sizeof(BYTE);

	BYTE * data = new BYTE[size];

	for(int y = 0;y < iYSizeBlock;y++)
	{
		int iPos1 = GetPixelPosition(0,y,iXSizeBlock);
		int iPos2 = GetPixelPosition(m_iXPos,m_iYPos+y,width);
		memcpy(data + iPos1,bPictureData + iPos2,linesize);
	}

	
	g_pHello->InitValue(iXSizeBlock, iYSizeBlock, data);
	g_pHello->update();  // update the scene and run the edge detect filter

	for(int y = 0;y < iYSizeBlock;y++)
	{
		int iPos1 = GetPixelPosition(0,y,iXSizeBlock);
		int iPos2 = GetPixelPosition(m_iXPos,m_iYPos+y,width);
		memcpy(bPictureData + iPos2,g_pHello->GetOutputData() + iPos1,linesize);
	}

	//delete g_pHello;

	g_pHello->DeleteGLTextures();
	delete[] data;
}

// The main function
void main()
{
	DWORD tickCount = GetTickCount ();				// Get The Tick Count
	int sizeBlock = 512;
	int _iWidth = 0, _iHeight = 0;
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	AUX_RGBImageRec * TextureImage; 

	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if (TextureImage=LoadBMP(filename))
	{
		_iWidth = TextureImage->sizeX;
		_iHeight = TextureImage->sizeY;
	}

	glutInitWindowSize(sizeBlock, sizeBlock);
	glutCreateWindow("Hello, GPGPU!");

	g_pHello = new HelloGPGPU();

	bool m_bWPlusUn = false;
	bool m_bHPlusUn = false;

	int iNbW = _iWidth / sizeBlock;
	int iNbH = _iHeight / sizeBlock;

	int iNbWSize = iNbW * sizeBlock;
	int iNbHSize = iNbH * sizeBlock;

	if(iNbWSize < _iWidth)
	{
		iNbW++;m_bWPlusUn = true;
	}

	if(iNbHSize < _iHeight)
	{
		iNbH++;m_bHPlusUn = true;
	}

	for(int iNbY = 0; iNbY < iNbH;iNbY++)
	{
		int sizeBlockX = sizeBlock;
		int sizeBlockY = sizeBlock;
		int m_iYPos = iNbY * sizeBlock;
		if(iNbY == (iNbH - 1) && m_bHPlusUn)
			sizeBlockY = _iHeight - (sizeBlock * iNbY);

		for(int iNbX = 0;iNbX < iNbW;iNbX++)
		{
			int m_iXPos = iNbX * sizeBlock;

			if(m_bWPlusUn && iNbX == (iNbW - 1))
				sizeBlockX = _iWidth - (sizeBlock * iNbX);
			
			DoEffect(iNbY, iNbX, sizeBlockY, sizeBlockX, m_iXPos, m_iYPos, TextureImage->data, TextureImage->sizeX);

		}
	}

	SaveBitmapToFile(TextureImage->data, TextureImage->sizeX, TextureImage->sizeY, 24, "c:\\text.bmp");

	delete g_pHello;

	if (TextureImage)									// If Texture Exists
	{
		if (TextureImage->data)							// If Texture Image Exists
			free(TextureImage->data);					// Free The Texture Image Memory
		free(TextureImage);								// Free The Image Structure
	}

	DWORD LasttickCount = GetTickCount();				// Get The Tick Count
	DWORD Result = LasttickCount - tickCount;
	printf("Temps d'execution : %d \n",Result);
}

