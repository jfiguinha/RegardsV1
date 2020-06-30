#pragma once

#ifdef GPGPU_CG
class CGpGpuEngine;
#else
class GpGpuEngineGL;
#endif

class CIBitmap;

class CGpGpuEffect
{
public:
	CGpGpuEffect(void);
	~CGpGpuEffect(void);
	
	bool Initialize();
	void ComputeEffect(CIBitmap * ciBitmap, int iEffect, HWND hWnd, int iControl);

private:

	
	int GetPixelPosition(int x, int y, int width);
	void ApplyEffect(int iNbY, int iNbX, int iYSizeBlock, int iXSizeBlock,int m_iXPos, int m_iYPos, BYTE * & bPictureData, int width);
	int m_iNumWindow;
	int m_iBlockSize;
	int m_iColorSize;
	int m_iNbTime;
	int m_kSize;
	float m_kFactor;
	float m_kOffset;

#ifdef GPGPU_CG
	CGpGpuEngine * m_cGpGpuEngine;
#else
	GpGpuEngineGL * m_cGpGpuEngine;
#endif
};
