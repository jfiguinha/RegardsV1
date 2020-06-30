#include "StdAfx.h"
#include "GpGpuEffect.h"
#ifdef GPGPU_CG
	#include "GpGpuEngine.h"
	#include "GpGpuShaderFile.h"
#else
	#include "GpGpuEngineGL.h"
	#include "GpGpuShaderFileGL.h"
#endif
#include <IBitmap.h>
#include <commctrl.h>

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
CGpGpuEffect::CGpGpuEffect(void)
{
	m_cGpGpuEngine = NULL;
	m_iBlockSize = 256;
	m_iColorSize = 4;
	m_iNbTime = 1;
	m_kSize = 1;
	m_kFactor = 1.0f;
	m_kOffset = 0.0f;
}

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
CGpGpuEffect::~CGpGpuEffect(void)
{
	//glutDestroyWindow(m_iNumWindow);
	if(m_cGpGpuEngine != NULL)
		delete m_cGpGpuEngine;

	m_cGpGpuEngine = NULL;
}

///////////////////////////////////////////////////////////////////////
// Called at startup
///////////////////////////////////////////////////////////////////////
bool CGpGpuEffect::Initialize()
{

	//glutInitWindowSize(m_iBlockSize, m_iBlockSize);
	//m_iNumWindow = glutCreateWindow("Hello, GPGPU!");
	// Initialize the "OpenGL Extension Wrangler" library
	

	// Ensure we have the necessary OpenGL Shading Language extensions.
	if (glewGetExtension("GL_ARB_fragment_shader")      != GL_TRUE ||
	  glewGetExtension("GL_ARB_vertex_shader")        != GL_TRUE ||
	  glewGetExtension("GL_ARB_shader_objects")       != GL_TRUE ||
	  glewGetExtension("GL_ARB_shading_language_100") != GL_TRUE)    
	{
		//fprintf(stderr, "Driver does not support OpenGL Shading Language\n");
		return false;
	}

	if (glewGetExtension("GL_EXT_framebuffer_object") != GL_TRUE) 
	{
		//fprintf(stderr, "Driver does not support Framebuffer Objects (GL_EXT_framebuffer_object)\n");
		return false;
	}

	//

	return true;
}

///////////////////////////////////////////////////////////////////////
// Called at startup
///////////////////////////////////////////////////////////////////////
int CGpGpuEffect::GetPixelPosition(int x, int y, int width)
{
	return (x  + (y * width)) * m_iColorSize;
}

///////////////////////////////////////////////////////////////////////
// Called at startup
///////////////////////////////////////////////////////////////////////
void CGpGpuEffect::ApplyEffect(int iNbY, int iNbX, int iYSizeBlock, int iXSizeBlock,int m_iXPos, int m_iYPos, BYTE * & bPictureData, int width)
{
	if(m_cGpGpuEngine == NULL)
	{
#ifdef GPGPU_CG
		m_cGpGpuEngine = new CGpGpuEngine();
#else
		m_cGpGpuEngine = new GpGpuEngineGL();
#endif
	}
	
	//int size = iXSizeBlock * iYSizeBlock * m_iColorSize * sizeof(BYTE);
	int size = m_iBlockSize * m_iBlockSize * m_iColorSize * sizeof(BYTE);
	int linesize = iXSizeBlock * m_iColorSize * sizeof(BYTE);

	//for(int i = 0;i < m_iNbTime;i++)
	//{
		BYTE * data = new BYTE[size];

		memset(data,0,size);

		for(int y = 0;y < iYSizeBlock;y++)
		{
			//int iPos1 = GetPixelPosition(0,y,iXSizeBlock);
			int iPos1 = GetPixelPosition(0,y,m_iBlockSize);
			int iPos2 = GetPixelPosition(m_iXPos,m_iYPos+y,width);
			memcpy(data + iPos1,bPictureData + iPos2,linesize);
		}
		
		//m_cGpGpuEngine->InitValue(iXSizeBlock, iYSizeBlock, m_iColorSize, data);
		m_cGpGpuEngine->InitValue(m_iBlockSize, m_iBlockSize, m_iColorSize, data);
		
		m_cGpGpuEngine->Compute(m_iNbTime);  // update the scene and run the edge detect filter

		for(int y = 0;y < iYSizeBlock;y++)
		{
			//int iPos1 = GetPixelPosition(0,y,iXSizeBlock);
			int iPos1 = GetPixelPosition(0,y,m_iBlockSize);
			int iPos2 = GetPixelPosition(m_iXPos,m_iYPos+y,width);
			memcpy(bPictureData + iPos2,m_cGpGpuEngine->GetOutputData() + iPos1,linesize);
		}

		m_cGpGpuEngine->DeleteGLTextures();

		delete[] data;
	//}
}

///////////////////////////////////////////////////////////////////////
//Application de l'effet
///////////////////////////////////////////////////////////////////////
void CGpGpuEffect::ComputeEffect(CIBitmap * ciBitmap, int iEffect, HWND hWnd, int iControl)
{

	if(m_cGpGpuEngine == NULL)
	{
#ifdef GPGPU_CG
		m_cGpGpuEngine = new CGpGpuEngine();
#else
		m_cGpGpuEngine = new GpGpuEngineGL();
#endif
	}

	bool m_bWPlusUn = false;
	bool m_bHPlusUn = false;

	switch(iEffect)
	{
#ifdef GPGPU_CG

		case FILTRE_SOFTEN:
			m_cGpGpuEngine->InitializeEngine(SoftenShaderSource,"Soften");
			break;

		case FILTRE_BLUR:
			m_cGpGpuEngine->InitializeEngine(BlurShaderSource,"Blur");
			break;

		case FILTRE_EDGE:
			m_cGpGpuEngine->InitializeEngine(EdgeShaderSource,"Edge");
			break;

		case FILTRE_SHARPEN:
			m_cGpGpuEngine->InitializeEngine(SharpenStrongShaderSource,"SharpenStrong");
			break;

		case FILTRE_SHARPENSTRONG:
			m_cGpGpuEngine->InitializeEngine(SharpenShaderSource,"Sharpen");
			break;

		case FILTRE_GAUSSIANBLUR:
			m_iNbTime = 2;
			m_cGpGpuEngine->InitializeEngine(GaussianShaderSource,"Gaussian");
			break;

		//case FILTRE_EMBOSS:
		//	m_cGpGpuEngine->InitializeEngine(EmbossShaderSource,"Emboss");
		//	break;
#else
		case FILTRE_SOFTEN:
			{
				float vecs[9] = {1.0f, 1.0f, 1.0f, 1.0f, 8.0f, 1.0f, 1.0f, 1.0f, 1.0f};
				m_kFactor = 16.0f;
				m_cGpGpuEngine->InitializeEngine(MatrixConvolution,vecs, m_kSize, m_kFactor, m_kOffset);
			}
			break;

		case FILTRE_BLUR:
			{
				m_kFactor = 9.0f;
				float vecs[9] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
				m_cGpGpuEngine->InitializeEngine(MatrixConvolution,vecs, m_kSize, m_kFactor, m_kOffset);
			}
			break;

		case FILTRE_EDGE:
			{
				float vecs[9] = {-1.0f, -1.0f, -1.0f, -1.0f, 8.0f, -1.0f, -1.0f, -1.0f, -1.0f};
				m_cGpGpuEngine->InitializeEngine(MatrixConvolution,vecs, m_kSize, m_kFactor, m_kOffset);
			}
			break;

		case FILTRE_SHARPEN:
			{
				m_kFactor = 8.0f;
				float vecs[9] = {-1.0f,-1.0f,-1.0f,-1.0f,16.0f,-1.0f,-1.0f,-1.0f,-1.0f};
				m_cGpGpuEngine->InitializeEngine(MatrixConvolution,vecs, m_kSize, m_kFactor, m_kOffset);
			}
			break;

		case FILTRE_SHARPENSTRONG:
			{
				float vecs[9] = {-1.0f,-1.0f,-1.0f,-1.0f,9.0f,-1.0f,-1.0f,-1.0f,-1.0f};
				m_cGpGpuEngine->InitializeEngine(MatrixConvolution,vecs, m_kSize, m_kFactor, m_kOffset);
			}
			break;

		case FILTRE_GAUSSIANBLUR:
			{
				m_iNbTime = 6;
				m_kFactor = 256.0f;
				float vecs[9] = {5.0f,15.0f,31.0f,49.0f,56.0f,49.0f,31.0f,15.0f,5.0f};
				m_cGpGpuEngine->InitializeEngine(MatrixConvolution, vecs, m_kSize, m_kFactor, m_kOffset);
			}
			break;

		case FILTRE_EMBOSS:
			{
				m_kOffset = 0.5f;
				float vecs[9] = {-1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
				m_cGpGpuEngine->InitializeEngine(MatrixConvolution, vecs, m_kSize, m_kFactor, m_kOffset);
			}
			break;

		case FILTRE_MEDIAN:
			{
				m_iNbTime = 2;
				m_cGpGpuEngine->InitializeEngine(MedianShaderSource, NULL, 0, 0, 0);
			}
			break;
#endif

	}


	int iNbW = ciBitmap->GetBitmapWidth() / m_iBlockSize;
	int iNbH = ciBitmap->GetBitmapHeigth() / m_iBlockSize;

	int iNbWSize = iNbW * m_iBlockSize;
	int iNbHSize = iNbH * m_iBlockSize;

	if(iNbWSize < ciBitmap->GetBitmapWidth())
		iNbW++;m_bWPlusUn = true;

	if(iNbHSize < ciBitmap->GetBitmapHeigth())
	{
		iNbH++;m_bHPlusUn = true;
	}

	if(hWnd != NULL)
	{
		SendDlgItemMessage(hWnd,iControl,PBM_SETRANGE,0,MAKELPARAM(0,iNbH));
		SendDlgItemMessage(hWnd,iControl,PBM_SETSTEP, (WPARAM) 1, 0); 
	}

	for(int iNbY = 0; iNbY < iNbH;iNbY++)
	{
		int sizeBlockX = m_iBlockSize;
		int sizeBlockY = m_iBlockSize;
		int m_iYPos = iNbY * m_iBlockSize;
		if(iNbY == (iNbH - 1) && m_bHPlusUn)
			sizeBlockY = ciBitmap->GetBitmapHeigth() - (m_iBlockSize * iNbY);

		if(hWnd != NULL)
			SendDlgItemMessage(hWnd,iControl,PBM_STEPIT, 0, 0); 

		for(int iNbX = 0;iNbX < iNbW;iNbX++)
		{
			int m_iXPos = iNbX * m_iBlockSize;

			if(m_bWPlusUn && iNbX == (iNbW - 1))
				sizeBlockX = ciBitmap->GetBitmapWidth() - (m_iBlockSize * iNbX);
			
			BYTE * m_data = ciBitmap->GetPtBitmap();
			ApplyEffect(iNbY, iNbX, sizeBlockY, sizeBlockX, m_iXPos, m_iYPos, m_data, ciBitmap->GetBitmapWidth());

		}
	}
}
