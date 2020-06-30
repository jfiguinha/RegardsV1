// Filtre.cpp: implementation of the CFiltre class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Filtre.h"
#include "IBitmap.h"
#include "DialogFiltreEffet.h"
#include "FiltreMatrix.h"
#include <time.h>
#include <MatrixDoubleComplex.h>
#include <xmmintrin.h>
#include <Matrix.h>
#include "effet.h"
#include "color.h"
#include "perlin.h"
#include "FiltrePrv.h"
#include "perlinnoise.h"
#include "floattoint.h"

#include "transfoasm.h"

#include <GpGpuEffect.h>

#define MagickSQ2PI 2.50662827463100024161235523934010416269302368164062

short m_dDataByte[] = {0,1,2,3,4,5,6,7,8,9,
					  10,11,12,13,14,15,16,17,18,19,
					  20,21,22,23,24,25,26,27,28,29,
					  30,31,32,33,34,35,36,37,38,39,
					  40,41,42,43,44,45,46,47,48,49,
					  50,51,52,53,55,55,56,57,58,59,
					  60,61,62,63,64,65,66,67,68,69,
					  70,71,72,73,74,75,76,77,78,79,
					  80,81,82,83,84,85,86,87,88,89,
					  90,91,92,93,94,95,96,97,98,99,
					  100,101,102,103,104,105,106,107,108,109,
					  110,111,112,113,114,115,116,117,118,119,
					  120,121,122,123,124,125,126,127,128,129,
					  130,131,132,133,134,135,136,137,138,139,
					  140,141,142,143,144,145,146,147,148,149,
					  150,151,152,153,154,155,156,157,158,159,
					  160,161,162,163,164,165,166,167,168,169,
					  170,171,172,173,174,175,176,177,178,179,
					  180,181,182,183,184,185,186,187,188,189,
					  190,191,192,193,194,195,196,197,198,199,
					  200,201,202,203,204,205,206,207,208,209,
					  210,211,212,213,214,215,216,217,218,219,
					  220,221,222,223,224,225,226,227,228,229,
					  230,231,232,233,234,235,236,237,238,239,
					  240,241,242,243,244,245,246,247,248,249,
					  250,251,252,253,254,255};

#include <algorithm>
#include <iterator>
#include <functional>

template <typename T>
void qsort(T begin, T end)
{
  if (begin != end)
	{
	  T middle = partition(begin, end, bind2nd(less<iterator_traits<T>::value_type>(), *begin))
	  qsort(begin, middle);
	  qsort(max(begin + 1, middle), end);
	}
}


double CFiltre::alpha = 1.0f;
double CFiltre::beta = 1.0f;
double CFiltre::gamma = 1.0f;








//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFiltre::CFiltre()
{
	m_bArreter = false;
	m_lpFiltreEffet = NULL;
}

int CFiltre::GetMotionBlurKernel(int width,const double sigma,double **kernel)
{
	const int KernelRank = 3;
	double alpha, normalize;
	int bias;
	register long i;

	/*
	Generate a 1-D convolution matrix.  Calculate the kernel at higher
	resolution than needed and average the results as a form of numerical
	integration to get the best accuracy.
	*/
	if (width <= 0)
		width=3;

	*kernel=(double *)malloc(width*sizeof(double));

	if (*kernel == (double *) NULL)
		return(0);

	for (i=0; i < width; i++)
		(*kernel)[i]=0.0;

	bias=KernelRank*width;

	double m_dValue = (2.0*KernelRank*KernelRank*sigma*sigma);
	double m_dValue2 = MagickSQ2PI*sigma;

	for (i=0; i < bias; i++)
	{
		alpha=exp(-((double) i*i)/m_dValue);
		(*kernel)[i/KernelRank]+=alpha/m_dValue2;
	}

	normalize=0;
	for (i=0; i < width; i++)
		normalize+=(*kernel)[i];

	for (i=0; i < width; i++)
		(*kernel)[i]/=normalize;
	return(width);
}

#define MaxRGB  255UL

bool CFiltre::MotionBlur(HWND hwnd, CIBitmap * m_ciBitmap, const double &radius,const double &sigma,const double &angle)
{
	double *kernel;

	typedef struct _PointInfo
	{
	  double
		x,
		y;
	} PointInfo;

	int width;

	long y;

	PointInfo *offsets;

	long x, u, v;

	if (sigma == 0.0)
		return 0;

	kernel=(double *) NULL;
	if (radius > 0)
	{
		width=GetMotionBlurKernel((int) (2.0*ceil(radius)+1.0),sigma,&kernel);
	}
	else
	{
		double * last_kernel = NULL;

		width=GetMotionBlurKernel(3,sigma,&kernel);
		while ((MaxRGB*kernel[width-1]) > 0.0)
		{
			if (last_kernel != NULL)
				free(last_kernel);
			last_kernel=kernel;
			kernel = NULL;
			width = GetMotionBlurKernel(width+2,sigma,&kernel);
		}
		if (last_kernel != NULL)
		{
			free(kernel);
			width-=2;
			kernel=last_kernel;
		}
	}
	if (width < 3)
		return false;

	offsets=(PointInfo *)malloc(width*sizeof(PointInfo));
	if (offsets == NULL)
		return false;

  
	if (m_ciBitmap->GetPtBitmap() != NULL)
	{
		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize;
		m_ciBitmap->GetBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize);
		long i = 0;
		long lWidthSize = m_ciBitmap->GetWidthSize();

		BYTE * lData = new BYTE[m_lSize * sizeof(BYTE)];

		memset(lData,0,m_lSize * sizeof(BYTE));

		long r,g,b,opacity;

		long ptImage;

		//blur_image->storage_class=DirectClass;
		x=(long) (width*sin(DegreesToRadians(angle)));
		y=(long) (width*cos(DegreesToRadians(angle)));

		for (i=0; i < width; i++)
		{
			offsets[i].x=i*x/sqrt((double)x*x+y*y);
			offsets[i].y=i*y/sqrt((double)x*x+y*y);
		}

		ptImage = 0;

		for (y=0; y < (long)bmHeight; y++)
		{
			for (x=0; x < (long)bmWidth; x++)
			{
				r=g=b=opacity=0;
				
				for (i=0; i < width; i++)
				{
					u=x+(long)offsets[i].x;
					v=y+(long)offsets[i].y;
					if ((u < 0) || (u >= (long)bmWidth) || (v < 0) || (v >= (long)bmHeight))
						continue;

					//pixel=AcquireOnePixel(image,u,v,exception);
					
					ptImage = (v * lWidthSize) + (u * 4);

					r += kernel[i] * *(pBitsSrc + ptImage);
					g += kernel[i] * *(pBitsSrc + ptImage + 1);
					b += kernel[i] * *(pBitsSrc + ptImage + 2);
					
					//opacity+=kernel[i]*pixel.opacity;
				}

				//ptImage = (y * lWidthSize) + (x * 4);
				
				if(r < 0)
					r = 0;
				else if(r > 255)
					r = 255;

				*(lData + ptImage) = (BYTE)r;
			
				if(g < 0)
					g = 0;
				else if(g > 255)
					g = 255;

				*(lData + ptImage + 1) = (BYTE)g;

			
				if(b < 0)
					b = 0;
				else if(b > 255)
					b = 255;

				*(lData + ptImage + 2)  = (BYTE)b;

				*(lData + ptImage + 3)  = 0;

				ptImage += 4;
			}

			
		}
	
		m_ciBitmap->SetBitmap(lData, bmWidth, bmHeight);

		delete[] pBitsSrc;
		delete[] lData;
		
	}

	free(kernel);
	free(offsets);

	return true;
}

CFiltre::~CFiltre()
{
	m_lpFiltreEffet = NULL;
}

void CFiltre::Clouds(HWND hwnd, CIBitmap * m_ciBitmap,const COLORREF &m_color1, const COLORREF &m_color2,const float &freq,const float &pers,const int &octaves)
{

	OldCtrlWord = Changex87FPToTrunc();

	if (m_ciBitmap->GetPtBitmap() != NULL)
	{
		CPerlinNoise m_PerlinNoise;
		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize;
		m_ciBitmap->GetBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize);

		//long k = 0;
		long m;
	
		int nbtour = octaves >> 2;

		int i,j,k;

		float * tabFrequence = new float[octaves];
		float * tabAmplitude = new float[octaves];
	
		tabFrequence[0] = freq;
		tabAmplitude[0] = 1;

		for(k=1; k<octaves; k++) 
		{
			tabFrequence[k] = tabFrequence[k - 1] * 2;
			tabAmplitude[k] = tabAmplitude[k - 1] * pers;
		}

		if(m_lpFiltreEffet != NULL)
		{
			SendDlgItemMessage(m_lpFiltreEffet->hDlg,m_lpFiltreEffet->iHandlePrgBar,PBM_SETRANGE,0,MAKELPARAM(0,bmHeight));
			SendDlgItemMessage(m_lpFiltreEffet->hDlg,m_lpFiltreEffet->iHandlePrgBar,PBM_SETSTEP, (WPARAM) 1, 0); 
		}


		for(i=0; i< bmHeight; i++) 
		{
			m = m_ciBitmap->GetWidthSize() * i;
	
			if(m_lpFiltreEffet != NULL)
			{
				SendDlgItemMessage(m_lpFiltreEffet->hDlg,m_lpFiltreEffet->iHandlePrgBar,PBM_STEPIT, 0, 0); 
			}

			for(j=0; j< bmWidth; j++)
			{

				int l = m + j * 4;

				float total = 0.0f;
				float total1 = 0.0f;
				float total2 = 0.0f;
				float total3 = 0.0f;
				float total4 = 0.0f;		
				
				for(int k=0; k< octaves; k+=4) 
				{
					total1 = m_PerlinNoise.GetValue(j*(*(tabFrequence + k)), i*(*(tabFrequence + k))) * (*(tabAmplitude + k));					
					total2 = m_PerlinNoise.GetValue(j*(*(tabFrequence + k + 1)), i*(*(tabFrequence + k + 1))) * (*(tabAmplitude + k + 1));
					total3 = m_PerlinNoise.GetValue(j*(*(tabFrequence + k + 2)), i*(*(tabFrequence + k + 2))) * (*(tabAmplitude + k + 2));
					total4 = m_PerlinNoise.GetValue(j*(*(tabFrequence + k + 3)), i*(*(tabFrequence + k + 3))) * (*(tabAmplitude + k + 3));
					total += total1 + total2 + total3 + total4;
					}

				total = total*0.5f + 0.5f;

				if(total<0) 
					total = 0.0f;
				if(total>1) 
					total = 1.0f;

				*(pBitsSrc + l) = GetBValue(m_color1) * total + GetBValue(m_color2)*(1-total);
				*(pBitsSrc + l + 1) = GetGValue(m_color1) * total + GetGValue(m_color2)*(1-total);
				*(pBitsSrc + l + 2) = GetRValue(m_color1) * total + GetRValue(m_color2)*(1-total);
				*(pBitsSrc + l + 3) = 0;

			}
		}

		m_ciBitmap->SetBitmap(pBitsSrc, bmWidth, bmHeight);

		delete[] pBitsSrc;
		delete[] tabFrequence;
		delete[] tabAmplitude;
	}

	//Setx87ControlWord(OldCtrlWord);
}

void CFiltre::AppliquerFiltre(CFiltreEffet * m_lpFiltreEffet)
{
	//BOOL m_bGpGpu = true;
	//DWORD tickCount = GetTickCount();				// Get The Tick Count

	switch(m_lpFiltreEffet->NumFiltre)
	{
		case FILTRE_INTENSITY:
			{
 				Lightness(NULL,m_lpFiltreEffet->m_ciBitmap, m_lpFiltreEffet->m_lParam[0]);
			}
 			break;

		case FILTRE_PHOTO:
			{
 				PhotoFiltre(NULL,m_lpFiltreEffet->m_ciBitmap, m_lpFiltreEffet->m_lParam[0], (int)m_lpFiltreEffet->m_lParam[1]);
			}
 			break;

		case FILTRE_MEDIAN:
			{
				CGpGpuEffect * m_gpGpuEffect = new CGpGpuEffect();
				if(m_gpGpuEffect->Initialize() && m_lpFiltreEffet->m_bGpGpu)
				{
					m_gpGpuEffect->ComputeEffect(m_lpFiltreEffet->m_ciBitmap, m_lpFiltreEffet->NumFiltre, m_lpFiltreEffet->hDlg, m_lpFiltreEffet->iHandlePrgBar);
				}
				else
				{
					Median(NULL,m_lpFiltreEffet->m_ciBitmap);
				}
				delete m_gpGpuEffect;
			}
 			break;


		case FILTRE_CONTRAST:
			{
 				long lValue;
 				double offset;

 				//Ajustement du paramètre de contrast
 				if(m_lpFiltreEffet->m_lParam[0] == 0)
 					Contrast(NULL,m_lpFiltreEffet->m_ciBitmap,1,128);

 				if(m_lpFiltreEffet->m_lParam[0] > 0)
 				{
 					offset = m_lpFiltreEffet->m_lParam[0];
 					offset /= 100;
 					offset = 1 + offset;
 					Contrast(NULL,m_lpFiltreEffet->m_ciBitmap,offset,128);
 				}

 				if(m_lpFiltreEffet->m_lParam[0] < 0)
 				{
 					offset = -m_lpFiltreEffet->m_lParam[0];
 					offset /= 100;
 					offset = 1 - offset;
 					lValue = 128 + m_lpFiltreEffet->m_lParam[0];
 				
					Contrast(NULL,m_lpFiltreEffet->m_ciBitmap,offset,128);
 				}
			}
 			break;

		case FILTRE_GAUSSIANBLUR:
			{
				CGpGpuEffect * m_gpGpuEffect = new CGpGpuEffect();
				if(m_gpGpuEffect->Initialize() && m_lpFiltreEffet->m_bGpGpu)
				{
					m_gpGpuEffect->ComputeEffect(m_lpFiltreEffet->m_ciBitmap, m_lpFiltreEffet->NumFiltre, m_lpFiltreEffet->hDlg, m_lpFiltreEffet->iHandlePrgBar);
				}
				else
				{
					short kernel[9]={5,15,31,49,56,49,31,15,5};
					int gauss_sum=256;
					Filter2Pass(NULL,m_lpFiltreEffet->m_ciBitmap,kernel,9,gauss_sum,0);
				}
				delete m_gpGpuEffect;
			}
			break;


		case FILTRE_MOSAIC:
			{
				Mosaic(NULL,m_lpFiltreEffet->m_ciBitmap,m_lpFiltreEffet->m_lParam[0]);
			}
			break;

		case FILTRE_BLUR:
			{
				CGpGpuEffect * m_gpGpuEffect = new CGpGpuEffect();
				if(m_gpGpuEffect->Initialize() && m_lpFiltreEffet->m_bGpGpu)
				{
					m_gpGpuEffect->ComputeEffect(m_lpFiltreEffet->m_ciBitmap, m_lpFiltreEffet->NumFiltre, m_lpFiltreEffet->hDlg, m_lpFiltreEffet->iHandlePrgBar);
				}
				else
				{
					short kernel[]={1,1,1,1,1,1,1,1,1};
					Filter(NULL,m_lpFiltreEffet->m_ciBitmap,kernel,3,9,0);
				}
				delete m_gpGpuEffect;


			}
			break;

		case FILTRE_RGB:
			{
				ModifyRGB(NULL,m_lpFiltreEffet->m_ciBitmap,m_lpFiltreEffet->m_lParam[0], m_lpFiltreEffet->m_lParam[1]);
			}
			break;

		case FILTRE_BLACKANDWHITE:
			{
				BlackAndWhite(NULL, m_lpFiltreEffet->m_ciBitmap);
			}
			break;

		case FILTRE_GRAYSCALE:
			{
				GrayScale(NULL, m_lpFiltreEffet->m_ciBitmap);
			}
			break;

		case FILTRE_SEPIA:
			{
				GrayScale(NULL, m_lpFiltreEffet->m_ciBitmap);
				int m_iDiff = 255 - 172;
				this->ApplyColor(NULL,m_lpFiltreEffet->m_ciBitmap,RGB(172+m_iDiff,122+m_iDiff,51+m_iDiff));
				//GrayScale(NULL, m_lpFiltreEffet->m_ciBitmap);
			}
			break;

		case FILTRE_DILATE:
			{
				Dilate(NULL, m_lpFiltreEffet->m_ciBitmap);
			}
			break;

		case FILTRE_NEGATIF:
			{
				Negatif(NULL, m_lpFiltreEffet->m_ciBitmap);
			}
			break;

		case FILTRE_ERODE:
			{
				Erode(NULL, m_lpFiltreEffet->m_ciBitmap);
			}
			break;

		case FILTRE_NOISE:
			{
				Noise(NULL, m_lpFiltreEffet->m_ciBitmap, 50);
			}
			break;

		case FILTRE_SHARPEN:
			{
				CGpGpuEffect * m_gpGpuEffect = new CGpGpuEffect();
				if(m_gpGpuEffect->Initialize() && m_lpFiltreEffet->m_bGpGpu)
				{
					m_gpGpuEffect->ComputeEffect(m_lpFiltreEffet->m_ciBitmap, m_lpFiltreEffet->NumFiltre, m_lpFiltreEffet->hDlg, m_lpFiltreEffet->iHandlePrgBar);
				}
				else
				{
					short kernel[]={-1,-1,-1,-1,16,-1,-1,-1,-1};
					SimpleFilter(NULL, m_lpFiltreEffet->m_ciBitmap,kernel,3,8,0);
				}
				delete m_gpGpuEffect;
			}
			break;

		case FILTRE_SHARPENSTRONG:
			{
				CGpGpuEffect * m_gpGpuEffect = new CGpGpuEffect();
				if(m_gpGpuEffect->Initialize() && m_lpFiltreEffet->m_bGpGpu)
				{
					m_gpGpuEffect->ComputeEffect(m_lpFiltreEffet->m_ciBitmap, m_lpFiltreEffet->NumFiltre, m_lpFiltreEffet->hDlg, m_lpFiltreEffet->iHandlePrgBar);
				}
				else
				{
					short kernel[]={-1,-1,-1,-1,9,-1,-1,-1,-1};
					SimpleFilter(NULL, m_lpFiltreEffet->m_ciBitmap,kernel,3,1,0);
				}
				delete m_gpGpuEffect;
			}
			break;

		case FILTRE_SOFTEN:
			{
				CGpGpuEffect * m_gpGpuEffect = new CGpGpuEffect();
				if(m_gpGpuEffect->Initialize() && m_lpFiltreEffet->m_bGpGpu)
				{
					m_gpGpuEffect->ComputeEffect(m_lpFiltreEffet->m_ciBitmap, m_lpFiltreEffet->NumFiltre, m_lpFiltreEffet->hDlg, m_lpFiltreEffet->iHandlePrgBar);
				}
				else
				{
					short kernel[]={1,1,1,1,8,1,1,1,1};
					Filter(NULL, m_lpFiltreEffet->m_ciBitmap,kernel,3,16,0);
				}
				delete m_gpGpuEffect;
			}
			break;

		case FILTRE_EMBOSS:
			{
				CGpGpuEffect * m_gpGpuEffect = new CGpGpuEffect();
				if(m_gpGpuEffect->Initialize() && m_lpFiltreEffet->m_bGpGpu)
				{
					m_gpGpuEffect->ComputeEffect(m_lpFiltreEffet->m_ciBitmap, m_lpFiltreEffet->NumFiltre, m_lpFiltreEffet->hDlg, m_lpFiltreEffet->iHandlePrgBar);
				}
				else
				{
					short kernel[]={-1,0,0,0,0,0,0,0,1};
					Filter(NULL, m_lpFiltreEffet->m_ciBitmap,kernel,3,0,127);
				}
				delete m_gpGpuEffect;

			}
			break;

		case FILTRE_EDGE:
			{
				CGpGpuEffect * m_gpGpuEffect = new CGpGpuEffect();
				if(m_gpGpuEffect->Initialize() && m_lpFiltreEffet->m_bGpGpu)
				{
					m_gpGpuEffect->ComputeEffect(m_lpFiltreEffet->m_ciBitmap, m_lpFiltreEffet->NumFiltre, m_lpFiltreEffet->hDlg, m_lpFiltreEffet->iHandlePrgBar);
				}
				else
				{
					short kernel[]={-1,-1,-1,-1,8,-1,-1,-1,-1};
					Filter(NULL, m_lpFiltreEffet->m_ciBitmap,kernel,3,0,0);
				}
				delete m_gpGpuEffect;
			}
			break;

		case ID_FILTRES_NUAGES:
			{
				//m_lpFiltreEffet->
				//Clouds(NULL,m_lpFiltreEffet->m_ciBitmap,
			}
			break;
	}


	//DWORD LasttickCount = GetTickCount();				// Get The Tick Count
	//DWORD Result = LasttickCount - tickCount;

	//char Temp[10];
	//sprintf(Temp,"%d",Result);
	//::MessageBox(NULL,Temp,"Temps d'execution",0);


}


///////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////
void CFiltre::Mosaic(HWND hwnd, CIBitmap * m_ciBitmap,const long &m_lValue)
{
	if (m_ciBitmap->GetPtBitmap() != NULL)
	{
		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize;
		m_ciBitmap->GetBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize);
		
		long lWidthSize = m_ciBitmap->GetWidthSize();
		long i = 0;
		long k =0, j= 0;
		long xMax = bmWidth;
		long yMax = bmHeight;
		long r,g,b;


		for(long y=0; y< bmHeight; y += m_lValue)
		{
			for(long x=0; x<bmWidth; x += m_lValue)
			{
				
				yMax = (m_lValue + y);
				if(yMax > bmHeight)
					yMax = bmHeight;

				xMax = (m_lValue + x);
				if(xMax > bmWidth)
					xMax = bmWidth;

				r = g = b = 0;
				
				for(k = y;k < yMax;k++)
				{
					for(j = x;j < xMax;j++)
					{
						i = (k * lWidthSize) + (j * 4);
						//Faire une moyenne
						r += *(pBitsSrc + i);
						g += *(pBitsSrc + i + 1);
						b += *(pBitsSrc + i + 2);
					}
				}

				//Moyenne 
				r /= (m_lValue * m_lValue);
				g /= (m_lValue * m_lValue);
				b /= (m_lValue * m_lValue);


				for(k = y;k < yMax;k++)
				{
					for(j = x;j < xMax;j++)
					{
						i = (k * lWidthSize) + (j * 4);
						//Faire une moyenne
						*(pBitsSrc + i) = r;
						*(pBitsSrc + i + 1) = g;
						*(pBitsSrc + i + 2) = b;
					}
				}
			}
		}

		m_ciBitmap->SetBitmap(pBitsSrc, bmWidth, bmHeight);
		delete[] pBitsSrc;
	}
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CFiltre::ModifyRGB(HWND hwnd, CIBitmap * m_ciBitmap,const long &m_lValue, const int &m_iCouleur)
{
	OldCtrlWord = Changex87FPToTrunc();

	short m_dTemp[256];
	long i = 0;

	#pragma omp parallel for private(i)
		for(i=0; i< 256; i++)
		{
			m_dTemp[i] = m_lValue + i;
			if(m_dTemp[i] < 0 || m_dTemp[i] > 255)
				m_dTemp[i] = (m_dTemp[i] > 255) ? 255 : 0;
		}

	if (m_ciBitmap->GetPtBitmap() != NULL)
	{
		
		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize;
		m_ciBitmap->GetPtBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize);

		int iColorPt = m_iCouleur - 1;

		long size = bmWidth * bmHeight * 4;

		#pragma omp parallel for private(i)
			for(i=0; i< size; i+=4)
				*(pBitsSrc + i + iColorPt) = m_dTemp[(int)pBitsSrc[i + iColorPt]];

		m_ciBitmap->ReCreateHBitmap(false);	
	}
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CFiltre::Dilate(HWND hwnd, CIBitmap * m_ciBitmap,const long &Ksize)
{
	if (m_ciBitmap->GetPtBitmap() != NULL)
	{
	
		long k2 = Ksize/2;
		long kmax= Ksize-k2;
		BYTE r,g,b;
		long k = 0;
		long lWidthSize = m_ciBitmap->GetWidthSize();

		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize;
		m_ciBitmap->GetBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize);

		BYTE * pData= new BYTE[m_lSize];

		long y,x,j;
		int pos;

			for(y=0; y<bmHeight; y++)
			{
				for(x=0; x<bmWidth; x++)
				{
					r=b=g=0;
					for(j=-k2;j<kmax;j++)
					{
						for(k=-k2;k<kmax;k++)
						{
							pos = ((y+k) * lWidthSize) + ((x+j) * 4);
							if(pos < 0)
							{
								r=g=b=255;
								continue;
							}

							if (*(pBitsSrc + pos) > r) 
								r=*(pBitsSrc + pos);
							if (*(pBitsSrc + pos + 1) > g) 
								g=*(pBitsSrc + pos + 1);
							if (*(pBitsSrc + pos + 2) > b) 
								b=*(pBitsSrc + pos + 2);
						}
					}
					pos = (y * lWidthSize) + (x * 4);
					*(pData + pos) = r;
					*(pData + pos + 1) = g;
					*(pData + pos + 2) = b;

				}
			}

		m_ciBitmap->SetBitmap(pData, bmWidth, bmHeight);

		delete[] pBitsSrc;
		delete[] pData;
		
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

bool CFiltre::Filter2Pass(HWND hwnd, CIBitmap * m_ciBitmap, short * kernel,const short &Ksize,const short &Kfactor,const short &Koffset)
{
#if DEBUG
	DWORD tickCount = GetTickCount ();
#endif
	if (m_ciBitmap->GetPtBitmap() != NULL)
	{
		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize2;
		m_ciBitmap->GetPtBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize2);

		BYTE * pBitsCopy = new BYTE[m_lSize2];

		
		unsigned short m_FiltreValue[4];
		BYTE m_bResult[8];

		//Test si Athlon ou Intel
		memcpy(pBitsCopy,pBitsSrc,m_lSize2);

		int iMiddleFilter = Ksize >> 1;

		int sumr = 0, sumg = 0, sumb = 0;

		int bmWidth4 = bmWidth << 2;

		long k2 = 0;

		int i,j,k;

		// 8 pixels are processed in one loop
		unsigned short * m_KernelValue = new unsigned short[Ksize << 8];
		memset(m_KernelValue,0,(Ksize << 8) * sizeof(short));
		
		unsigned short * m_ptKernelValue = m_KernelValue;

		for(i = 0;i < Ksize;i++)
		{
			short m_FiltreValue[] = {kernel[i],kernel[i],kernel[i],kernel[i]};

			mulByteFilter(m_FiltreValue, m_dDataByte, m_ptKernelValue, 256);

			m_ptKernelValue += 256;
		}

		if(m_lpFiltreEffet != NULL)
		{
			SendDlgItemMessage(m_lpFiltreEffet->hDlg,m_lpFiltreEffet->iHandlePrgBar,PBM_SETRANGE,0,MAKELPARAM(0,bmWidth*2));
			SendDlgItemMessage(m_lpFiltreEffet->hDlg,m_lpFiltreEffet->iHandlePrgBar,PBM_SETSTEP, (WPARAM) 1, 0); 
		}

		////////////////////////////////////////////////////////////////////////////////////////
		//1ére pass horizontal
		////////////////////////////////////////////////////////////////////////////////////////

		for(i=0;i<bmWidth;i++)
		{

			long iLocalisation2 = (i << 2);

			if(m_lpFiltreEffet != NULL)
			{
				SendDlgItemMessage(m_lpFiltreEffet->hDlg,m_lpFiltreEffet->iHandlePrgBar,PBM_STEPIT, 0, 0); 
			}

			for(j=0;j<bmHeight;j++)
			{
				int iStart = 0;
				int iEnd = Ksize;
				
				ZeroMemory(&m_FiltreValue,8);

				int iPosX = i - ((Ksize-1)>>1);
				long iLocalisation = (j * (bmWidth << 2))  + (iPosX << 2);

				unsigned short * ptKernel = m_KernelValue;

				int iNb = 0;

				int iPosFin = iPosX + Ksize;

				if(iPosX < 0)
				{
					iNb = 0 - iPosX;
					iStart += iNb;
					iLocalisation += (iNb << 2);
					ptKernel += (iNb << 8);
				}

				if(iPosFin >= bmWidth)
				{
					iEnd = Ksize - 1 - (iPosFin - bmWidth);
				}

				iPosFin = iEnd - iStart;

			
				for(k=0;k < iPosFin;k++)
				{
					m_FiltreValue[0] +=ptKernel[*(pBitsCopy + iLocalisation)];
					m_FiltreValue[1] +=ptKernel[*(pBitsCopy + iLocalisation + 1)];
					m_FiltreValue[2] +=ptKernel[*(pBitsCopy + iLocalisation + 2)];

					iLocalisation+=4;
					ptKernel += 256;
				}


				if (Kfactor==0)
				{
					m_FiltreValue[0] += Koffset;
					m_FiltreValue[1] += Koffset;
					m_FiltreValue[2] += Koffset;
				} 
				else 
				{
					m_FiltreValue[0] /= Kfactor;
					m_FiltreValue[1] /= Kfactor;
					m_FiltreValue[2] /= Kfactor;
					m_FiltreValue[0] += Koffset;
					m_FiltreValue[1] += Koffset;
					m_FiltreValue[2] += Koffset;
				}

				fonct16to8(m_FiltreValue,m_bResult);


				*(pBitsSrc + iLocalisation2) = m_bResult[0];
				*(pBitsSrc + iLocalisation2 + 1) = m_bResult[1];
				*(pBitsSrc + iLocalisation2 + 2) = m_bResult[2];

				iLocalisation2 += bmWidth4;
			} 
		}

		////////////////////////////////////////////////////////////////////////////////////////
		//2éme pass vertical
		////////////////////////////////////////////////////////////////////////////////////////

		for(i=0;i < bmWidth;i++)
		{
			if(m_lpFiltreEffet != NULL)
			{
				SendDlgItemMessage(m_lpFiltreEffet->hDlg,m_lpFiltreEffet->iHandlePrgBar,PBM_STEPIT, 0, 0); 
			}

			long iLocalisation2 = (i << 2);

			for(j=0;j<bmHeight;j++)
			{
				ZeroMemory(&m_FiltreValue,8);

				
				int iStart = 0;
				int iEnd = Ksize;

				int iPosY = j-((Ksize-1)>>1);
				long iLocalisation = (iPosY * (bmWidth << 2))  + (i << 2);
				unsigned short * ptKernel = m_KernelValue;

				int iPosFin = iPosY + Ksize;

				if(iPosY < 0)
				{
					int iNb = 0 - iPosY;
					iStart += iNb;
					iLocalisation += bmWidth4 * iNb;
					ptKernel += iNb << 8;
				}

				if(iPosFin >= bmHeight)
				{
					iEnd = Ksize - 1 - (iPosFin - bmHeight);
				}

				for(k=iStart;k < iEnd;k++)
				{

					m_FiltreValue[0] +=ptKernel[*(pBitsSrc + iLocalisation)];
					m_FiltreValue[1] +=ptKernel[*(pBitsSrc + iLocalisation + 1)];
					m_FiltreValue[2] +=ptKernel[*(pBitsSrc + iLocalisation + 2)];

					iLocalisation += bmWidth4;
					ptKernel += 256;
				}


				if (Kfactor==0)
				{
					m_FiltreValue[0] += Koffset;
					m_FiltreValue[1] += Koffset;
					m_FiltreValue[2] += Koffset;
				} 
				else 
				{
					m_FiltreValue[0] /= Kfactor;
					m_FiltreValue[1] /= Kfactor;
					m_FiltreValue[2] /= Kfactor;
					m_FiltreValue[0] += Koffset;
					m_FiltreValue[1] += Koffset;
					m_FiltreValue[2] += Koffset;
				}

				fonct16to8(m_FiltreValue,m_bResult);

				*(pBitsCopy + iLocalisation2) = m_bResult[0];
				*(pBitsCopy + iLocalisation2 + 1) = m_bResult[1];
				*(pBitsCopy + iLocalisation2 + 2) = m_bResult[2];

				iLocalisation2 += bmWidth4;
			} 
		} 

		memcpy(pBitsSrc,pBitsCopy,m_lSize2);

		m_ciBitmap->ReCreateHBitmap(false);
		delete[] pBitsCopy;
		delete[] m_KernelValue;
	}

#if DEBUG
	DWORD LasttickCount = GetTickCount();				// Get The Tick Count
	DWORD Result = LasttickCount - tickCount;

	char Temp[10];
	sprintf(Temp,"%d",Result);
	::MessageBox(NULL,Temp,"Temps d'execution",0);
#endif
	return 0;

}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

bool CFiltre::SimpleFilter(HWND hwnd, CIBitmap * m_ciBitmap, short * kernel,const short &Ksize,const short &Kfactor,const short &Koffset)
{
	if (m_ciBitmap->GetPtBitmap() != NULL)
	{
		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize2;
		m_ciBitmap->GetPtBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize2);

		BYTE * pBitsCopy = new BYTE[m_lSize2];

		memcpy(pBitsCopy,pBitsSrc,m_lSize2);
		
		int iMiddleFilter = Ksize >> 1;

		int sumr = 0, sumg = 0, sumb = 0;

		int m_FiltreValue[4];
		BYTE m_bResult[8];

		long k2 = 0;

		int i,j,k;

		int * m_KernelValue = new int[Ksize * (Ksize << 8)];

		memset(m_KernelValue,0,(Ksize * (Ksize << 8)) * sizeof(short));
	
		k = 0;

		int * m_ptKernelValue = m_KernelValue;

		for(i = 0;i < Ksize * Ksize;i++)
		{
			for(int j = 0;j < 256;j+=4)
			{
				m_ptKernelValue[j] = kernel[i] * j;
				m_ptKernelValue[j+1] = kernel[i] * (j+1);
				m_ptKernelValue[j+2] = kernel[i] * (j+2);
				m_ptKernelValue[j+3] = kernel[i] * (j+3);
			}

			m_ptKernelValue += 256;
		}

		if(m_lpFiltreEffet != NULL)
		{
			SendDlgItemMessage(m_lpFiltreEffet->hDlg,m_lpFiltreEffet->iHandlePrgBar,PBM_SETRANGE,0,MAKELPARAM(0,bmHeight));
			SendDlgItemMessage(m_lpFiltreEffet->hDlg,m_lpFiltreEffet->iHandlePrgBar,PBM_SETSTEP, (WPARAM) 1, 0); 
		}
	
		for(i = 0;i < bmHeight;i++)
		{
			if(m_lpFiltreEffet != NULL)
			{
				SendDlgItemMessage(m_lpFiltreEffet->hDlg,m_lpFiltreEffet->iHandlePrgBar,PBM_STEPIT, 0, 0); 
			}


			for(j = 0;j < bmWidth;j++)
			{		

				//ptKernel = kernel;
				int * ptKernel2 = m_KernelValue;

				ZeroMemory(&m_FiltreValue,16);
			
				int iPosY = i - iMiddleFilter;

				sumr = sumg = sumb = 0;

				int iPosMinY = i - iMiddleFilter;
				int iPosMaxY = i + iMiddleFilter;
				
				int iPosMinX = j - iMiddleFilter;
				int iPosMaxX = j + iMiddleFilter;

				if(iPosMinY < 0 || iPosMaxY >= bmHeight || iPosMinX < 0 || iPosMaxX >= bmWidth)
				{
					int y = 0;
					int iNbY = Ksize;

					//short * ptKernel = m_KernelValue;

					if(iPosMinY < 0)
					{
						ptKernel2 += ((Ksize * (0 - iPosMinY)) << 8);
						iNbY = Ksize - (0 - iPosMinY);
						iPosY = 0;
					}

					if(iPosMaxY >= bmHeight)
					{
						iNbY = Ksize - (iPosMaxY - bmHeight + 1);
					}

					for(int k=0;k < iNbY;k++,iPosY++)
					{
						int iPosX = j - iMiddleFilter;

						long iLocalisation = (iPosY * (bmWidth << 2))  + (iPosX << 2);

						for(int l = 0;l < Ksize;l++,iPosX++)
						{
							if(iPosX >= 0 && iPosX < bmWidth)
							{							
								m_FiltreValue[0] += ptKernel2[*(pBitsCopy + iLocalisation)];
								m_FiltreValue[1] += ptKernel2[*(pBitsCopy + iLocalisation+1)];
								m_FiltreValue[2] += ptKernel2[*(pBitsCopy + iLocalisation+2)];
							}

							ptKernel2+=256;iLocalisation+=4;
						}
					}
				}
				else
				{
					//short * ptKernel = m_KernelValue;

					for(int k=0,y = 0;k < Ksize;k++,iPosY++)
					{
						long iLocalisation = (iPosY * (bmWidth << 2))  + ((j - iMiddleFilter) << 2);

						for(int l = 0;l < Ksize;l++)
						{
							m_FiltreValue[0] += ptKernel2[*(pBitsCopy + iLocalisation)];
							m_FiltreValue[1] += ptKernel2[*(pBitsCopy + iLocalisation+1)];
							m_FiltreValue[2] += ptKernel2[*(pBitsCopy + iLocalisation+2)];

							ptKernel2+=256;iLocalisation+=4;
						}
					}
				}

				if (Kfactor==0)
				{
					m_FiltreValue[0] += Koffset;
					m_FiltreValue[1] += Koffset;
					m_FiltreValue[2] += Koffset;
				} 
				else 
				{
					m_FiltreValue[0] /= Kfactor;
					m_FiltreValue[1] /= Kfactor;
					m_FiltreValue[2] /= Kfactor;
					m_FiltreValue[0] += Koffset;
					m_FiltreValue[1] += Koffset;
					m_FiltreValue[2] += Koffset;
				}

				if(m_FiltreValue[0] > 255)
					m_FiltreValue[0] = 255;

				if(m_FiltreValue[1] > 255)
					m_FiltreValue[1] = 255;

				if(m_FiltreValue[2] > 255)
					m_FiltreValue[2] = 255;

				if(m_FiltreValue[0] < 0)
					m_FiltreValue[0] = 0;

				if(m_FiltreValue[1] < 0)
					m_FiltreValue[1] = 0;

				if(m_FiltreValue[2] < 0)
					m_FiltreValue[2] = 0;
		
				*(pBitsSrc + k2) = m_FiltreValue[0];
				*(pBitsSrc + k2+1) = m_FiltreValue[1];
				*(pBitsSrc + k2+2) = m_FiltreValue[2];
				*(pBitsSrc + k2+3) = 0;
				
				k2+=4;
			}
		}

		m_ciBitmap->ReCreateHBitmap(false);
		delete[] pBitsCopy;
		delete[] m_KernelValue;

	}


	return true;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

bool CFiltre::Filter(HWND hwnd, CIBitmap * m_ciBitmap, short * kernel,const short &Ksize,const short &Kfactor,const short &Koffset)
{
#if DEBUG
	DWORD tickCount = GetTickCount();
#endif

	if (m_ciBitmap->GetPtBitmap() != NULL)
	{
		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize2;
		m_ciBitmap->GetPtBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize2);

		BYTE * pBitsCopy = new BYTE[m_lSize2];

		memcpy(pBitsCopy,pBitsSrc,m_lSize2);
		
		int iMiddleFilter = Ksize >> 1;

		int sumr = 0, sumg = 0, sumb = 0;

		unsigned short m_FiltreValue[4];
		BYTE m_bResult[8];

		long k2 = 0;

		int k;

		short * m_KernelValue = new short[Ksize * (Ksize << 8)];

		memset(m_KernelValue,0,(Ksize * (Ksize << 8)) * sizeof(short));
	
		k = 0;

		short * m_ptKernelValue = m_KernelValue;

		for(int i = 0;i < Ksize * Ksize;i++)
		{
			short m_FiltreValue[] = {kernel[i],kernel[i],kernel[i],kernel[i]};

			mulByteFilter(m_FiltreValue, m_dDataByte, m_ptKernelValue, 256);

			m_ptKernelValue += 256;
		}

		if(m_lpFiltreEffet != NULL)
		{
			SendDlgItemMessage(m_lpFiltreEffet->hDlg,m_lpFiltreEffet->iHandlePrgBar,PBM_SETRANGE,0,MAKELPARAM(0,bmHeight));
			SendDlgItemMessage(m_lpFiltreEffet->hDlg,m_lpFiltreEffet->iHandlePrgBar,PBM_SETSTEP, (WPARAM) 1, 0); 
		}
	

        for(int i = 0;i < bmHeight;i++)
		{
			if(m_lpFiltreEffet != NULL)
			{
				SendDlgItemMessage(m_lpFiltreEffet->hDlg,m_lpFiltreEffet->iHandlePrgBar,PBM_STEPIT, 0, 0); 
			}

			for(int j = 0;j < bmWidth;j++)
			{		

				//ptKernel = kernel;
				short * ptKernel2 = m_KernelValue;

				ZeroMemory(&m_FiltreValue,8);
			
				int iPosY = i - iMiddleFilter;

				sumr = sumg = sumb = 0;

				int iPosMinY = i - iMiddleFilter;
				int iPosMaxY = i + iMiddleFilter;
				
				int iPosMinX = j - iMiddleFilter;
				int iPosMaxX = j + iMiddleFilter;

				if(iPosMinY < 0 || iPosMaxY >= bmHeight || iPosMinX < 0 || iPosMaxX >= bmWidth)
				{
					int y = 0;
					int iNbY = Ksize;

					//short * ptKernel = m_KernelValue;

					if(iPosMinY < 0)
					{
						ptKernel2 += ((Ksize * (0 - iPosMinY)) << 8);
						iNbY = Ksize - (0 - iPosMinY);
						iPosY = 0;
					}

					if(iPosMaxY >= bmHeight)
					{
						iNbY = Ksize - (iPosMaxY - bmHeight + 1);
					}
					
					for(int k=0;k < iNbY;k++,iPosY++)
					{
						int iPosX = j - iMiddleFilter;

						long iLocalisation = (iPosY * (bmWidth << 2))  + (iPosX << 2);

						for(int l = 0;l < Ksize;l++,iPosX++)
						{
							if(iPosX >= 0 && iPosX < bmWidth)
							{							
								m_FiltreValue[0] += ptKernel2[*(pBitsCopy + iLocalisation)];
								m_FiltreValue[1] += ptKernel2[*(pBitsCopy + iLocalisation+1)];
								m_FiltreValue[2] += ptKernel2[*(pBitsCopy + iLocalisation+2)];
							}

							ptKernel2+=256;iLocalisation+=4;
						}
					}
				}
				else
				{
					//short * ptKernel = m_KernelValue;

					for(int k=0,y = 0;k < Ksize;k++,iPosY++)
					{
						long iLocalisation = (iPosY * (bmWidth << 2))  + ((j - iMiddleFilter) << 2);

						for(int l = 0;l < Ksize;l++)
						{
							m_FiltreValue[0] += ptKernel2[*(pBitsCopy + iLocalisation)];
							m_FiltreValue[1] += ptKernel2[*(pBitsCopy + iLocalisation+1)];
							m_FiltreValue[2] += ptKernel2[*(pBitsCopy + iLocalisation+2)];

							ptKernel2+=256;iLocalisation+=4;
						}
					}
				}

				if (Kfactor==0)
				{
					m_FiltreValue[0] += Koffset;
					m_FiltreValue[1] += Koffset;
					m_FiltreValue[2] += Koffset;
				} 
				else 
				{
					m_FiltreValue[0] /= Kfactor;
					m_FiltreValue[1] /= Kfactor;
					m_FiltreValue[2] /= Kfactor;
					m_FiltreValue[0] += Koffset;
					m_FiltreValue[1] += Koffset;
					m_FiltreValue[2] += Koffset;
				}

				fonct16to8(m_FiltreValue,m_bResult);
		
				*(pBitsSrc + k2) = m_bResult[0];
				*(pBitsSrc + k2+1) = m_bResult[1];
				*(pBitsSrc + k2+2) = m_bResult[2];
				
				k2+=4;
			}
		}

		m_ciBitmap->ReCreateHBitmap(false);
		delete[] pBitsCopy;
		delete[] m_KernelValue;

	}




	return true;
}



//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CFiltre::Erode(HWND hwnd, CIBitmap * m_ciBitmap,const long &Ksize)
{

	if (m_ciBitmap->GetPtBitmap() != NULL)
	{
		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize;

		m_ciBitmap->GetBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize);

		BYTE * pData= new BYTE[m_lSize];

		long k2 = Ksize/2;
		long kmax= Ksize-k2;
		BYTE r,g,b;
		int pos;
		long k = 0;
		long lWidthSize = m_ciBitmap->GetWidthSize();


		for(long y=0; y<bmHeight; y++)
		{
			for(long x=0; x<bmWidth; x++)
			{
				{
					r=b=g=255;
					for(long j=-k2;j<kmax;j++)
					{
						for(long k=-k2;k<kmax;k++)
						{
							pos = ((y+k) * lWidthSize) + ((x+j) * 4);
							if(pos < 0)
							{
								r=g=b=0;
								continue;
							}
	
							if (*(pBitsSrc + pos) < r) 
								r=*(pBitsSrc + pos);
							if (*(pBitsSrc + pos + 1) < g) 
								g=*(pBitsSrc + pos + 1);
							if (*(pBitsSrc + pos + 2) < b) 
								b=*(pBitsSrc + pos + 2);
						}
					}

					pos = (y * lWidthSize) + (x * 4);
					*(pData + pos) = r;
					*(pData + pos + 1) = g;
					*(pData + pos + 2) = b;

				}
			}
		}


		m_ciBitmap->SetBitmap(pData, bmWidth, bmHeight);

		delete[] pBitsSrc;
		delete[] pData;
		
	}

}

/////////////////////////////////////////////////////////////
//
//Filtre d'ajout de bruit
/////////////////////////////////////////////////////////////

void CFiltre::Noise(HWND hwnd, CIBitmap * m_ciBitmap,const long &level)
{
	if (m_ciBitmap->GetPtBitmap() != NULL)
	{

		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize;
		m_ciBitmap->GetPtBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize);

		long k = 0;
		long n;

		for(long y=0; y< bmHeight; y++)
		{
			for(long x=0; x< bmWidth; x++)
			{
					n=(long)((rand()/(float)RAND_MAX - 0.5)*level);
					*(pBitsSrc + k) = (BYTE)max(0,min(255,(int)(*(pBitsSrc + k) + n)));
					n=(long)((rand()/(float)RAND_MAX - 0.5)*level);
					*(pBitsSrc + k+1) = (BYTE)max(0,min(255,(int)(*(pBitsSrc + k+1) + n)));
					n=(long)((rand()/(float)RAND_MAX - 0.5)*level);
					*(pBitsSrc + k+2) = (BYTE)max(0,min(255,(int)(*(pBitsSrc + k+2) + n)));
					k+=4;
			}
		}

		//m_ciBitmap->SetBitmap(pBitsSrc, bmWidth, bmHeight);
		//delete[] pBitsSrc;

		m_ciBitmap->ReCreateHBitmap(false);
		
	}

}

/////////////////////////////////////////////////////////////
//
//Filtre d'ajout de bruit
/////////////////////////////////////////////////////////////

void CFiltre::FiltreWiener(HWND hwnd, CIBitmap * m_ciBitmap,const CMatrix &FiltreDegradation,const float &rho, const int &iMethod)
{
	if (m_ciBitmap->GetPtBitmap() != NULL)
	{
		
		CMatrix ImageR,ImageG,ImageB;
		CFiltreMatrix m_cFiltreMatrix;

		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize;
		m_ciBitmap->GetPtBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize);

		

		ImageR.zeros(bmHeight,bmWidth);
		ImageG.zeros(bmHeight,bmWidth);
		ImageB.zeros(bmHeight,bmWidth);

		double * m_dDataR = ImageR.GetDataPt();
		double * m_dDataG = ImageG.GetDataPt();
		double * m_dDataB = ImageB.GetDataPt();

		for(long i = 0,k=0;i < m_lSize;i+=4,k++)
		{
			*(m_dDataR + k) = pBitsSrc[i] / 255.0;
			*(m_dDataG + k) = pBitsSrc[i + 1] / 255.0; 
			*(m_dDataB + k) = pBitsSrc[i + 2] / 255.0;
		}	

		switch(iMethod)
		{
			case 1:
				{
					//Restauration par la méthode de wiener
					CMatrixDoubleComplex m_MatrixFiltre;
					m_MatrixFiltre = m_cFiltreMatrix.calcul_filtrewiener(bmHeight,bmWidth,FiltreDegradation,rho);
					ImageR = m_cFiltreMatrix.restWiener(ImageR,m_MatrixFiltre);
					ImageG = m_cFiltreMatrix.restWiener(ImageG,m_MatrixFiltre);
					ImageB = m_cFiltreMatrix.restWiener(ImageB,m_MatrixFiltre);
				}
				break;

			case 2:
				{

					//Restauration par la méthode de la transformée cosinus

					CMatrixDoubleComplex G,G2(bmHeight,bmWidth);
					//G2 = m_cFiltreMatrix.calcul_filtrewiener(bmHeight,bmWidth,FiltreDegradation,rho);

				

					G = m_cFiltreMatrix.calcul_filtrewiener(bmHeight*2,bmWidth*2,FiltreDegradation,rho);

					double * complexData = G.GetDataPt();
					double * complexDestData = G2.GetDataPt();

					int k = 0;

					for(int i = 0;i < ImageR.GetTheMValue();i++)
					{
						int iPos = i * ImageR.GetNValue() * 4;

						for(int j = 0;j < ImageR.GetNValue() * 2;j+=2)
						{
							complexDestData[k++] = complexData[iPos + j];
							complexDestData[k++] = complexData[iPos + j + 1];
						}
					}

					G.DesAllocationMemoire();

					ImageR = m_cFiltreMatrix.restdct(ImageR,G2);
					ImageG = m_cFiltreMatrix.restdct(ImageG,G2);
					ImageB = m_cFiltreMatrix.restdct(ImageB,G2);

					G2.DesAllocationMemoire();
				}
				break;
		}


		m_dDataR = ImageR.GetDataPt();
		m_dDataG = ImageG.GetDataPt();
		m_dDataB = ImageB.GetDataPt();

		BYTE * pData= new BYTE[m_lSize];

		memcpy(pData,pBitsSrc,m_lSize);

		int k=0;

		if(iMethod == 1)
		{
			#if !(defined(__ICC) || defined(__ICL) || defined(__ECC) || defined(__ECL))
				for(int y = bmHeight - 1;y >= 0;y--)
				{
					for(int x = 0;x < bmWidth;x++,k++)
					{
						BYTE valueR,valueG,valueB;

						long iPos2 = y * bmWidth * 4 + (bmWidth - x - 1) * 4;

						if(*(m_dDataR + k) > 1.0 || *(m_dDataR + k) < 0.0)
							valueR = *(pBitsSrc + iPos2);
						else
							valueR = *(m_dDataR + k) * 255.0;

							
						if(*(m_dDataG + k) > 1.0 || *(m_dDataR + k) < 0.0)
							valueG = *(pBitsSrc + iPos2 + 1);
						else
							valueG = *(m_dDataG + k) * 255.0;

						if(*(m_dDataB + k) > 1.0 || *(m_dDataB + k) < 0.0)
							valueB = *(pBitsSrc + iPos2 + 2);
						else
							valueB = *(m_dDataB + k) * 255.0;

						*(pBitsSrc + iPos2) = valueR;
						*(pBitsSrc + iPos2 + 1) = valueG;
						*(pBitsSrc + iPos2 + 2) = valueB;
					}
				}

			#else

				for(int i = 0,k=0;i < m_lSize;i+=4,k++)
				{
					BYTE valueR,valueG,valueB;		
				
					if(*(m_dDataR + k) < 0.0)
						valueR = 0;

					if(*(m_dDataR + k) > 1.0)
						valueR = 255;
					else
						valueR = *(m_dDataR + k) * 255.0;

						
					if(*(m_dDataG + k) < 0.0)
						valueG = 0;

					if(*(m_dDataG + k) > 1.0)
						valueG = 255;
					else
						valueG = *(m_dDataG + k) * 255.0;

						
					if(*(m_dDataB + k) < 0.0)
						valueB = 0;

					if(*(m_dDataB + k) > 1.0)
						valueB = 255;
					else
						valueB = *(m_dDataB + k) * 255.0;


					*(pBitsSrc + i) = valueR;
					*(pBitsSrc + i + 1) = valueG;
					*(pBitsSrc + i + 2) = valueB;
				}

			#endif

		}
		else
		{
			for(int i = 0,k=0;i < m_lSize;i+=4,k++)
			{
				BYTE valueR,valueG,valueB;		
			
				if(*(m_dDataR + k) < 0.0)
					valueR = 0;

				if(*(m_dDataR + k) > 1.0)
					valueR = 255;
				else
					valueR = *(m_dDataR + k) * 255.0;

					
				if(*(m_dDataG + k) < 0.0)
					valueG = 0;

				if(*(m_dDataG + k) > 1.0)
					valueG = 255;
				else
					valueG = *(m_dDataG + k) * 255.0;

					
				if(*(m_dDataB + k) < 0.0)
					valueB = 0;

				if(*(m_dDataB + k) > 1.0)
					valueB = 255;
				else
					valueB = *(m_dDataB + k) * 255.0;


				*(pBitsSrc + i) = valueR;
				*(pBitsSrc + i + 1) = valueG;
				*(pBitsSrc + i + 2) = valueB;
			}
		}

		////////////////////////////////////////////////////////////////
		//Fonction corrigeant les bruits 
		////////////////////////////////////////////////////////////////

		
		
		for(int y = 0;y < bmHeight;y++)
		{
			for(int x = 0;x < bmWidth;x++)
			{
				long iPos = y * bmWidth * 4 + x * 4;

				int iDiffR,iDiffG,iDiffB;

				BYTE valueR,valueG,valueB;
				BYTE valueRMoy,valueGMoy,valueBMoy;

				valueR = *(pBitsSrc + iPos);
				valueG = *(pBitsSrc + iPos + 1);
				valueB = *(pBitsSrc + iPos + 2);

				valueRMoy = *(pData + iPos);
				valueGMoy = *(pData + iPos + 1);
				valueBMoy = *(pData + iPos + 2);

				iDiffR = abs(valueRMoy - valueR);
				iDiffG = abs(valueGMoy - valueG);
				iDiffB = abs(valueBMoy - valueB);

				if(iDiffR > 40)
				{
					*(pBitsSrc + iPos) = valueRMoy;
				}

				if(iDiffG > 40)
				{
					*(pBitsSrc + iPos + 1) = valueGMoy;
				}
				
				if(iDiffB > 40)
				{
					*(pBitsSrc + iPos + 2) = valueBMoy;
				}

			}
		}

		delete[] pData;
		m_ciBitmap->ReCreateHBitmap(false);

		
	}

}


//Effet GrayScale
void CFiltre::GrayScale(HWND hwnd, CIBitmap * m_ciBitmap)
{
	if (m_ciBitmap->GetPtBitmap() != NULL)
	{
		//BYTE Moyenne;
		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize;
		long i = 0;

		m_ciBitmap->GetPtBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize);

		//Conversion des données de Byte vers floating point

#pragma omp parallel for private (i)
		for(i = 0;i < m_lSize;i+=4)
		{
			BYTE Moyenne = ((pBitsSrc[i] * 0.299) + (pBitsSrc[i + 1] * 0.587) + (pBitsSrc[i + 2] * 0.114));
			*(pBitsSrc + i) = Moyenne;
			*(pBitsSrc + i + 1) = Moyenne;
			*(pBitsSrc + i + 2) = Moyenne;

		}	

		m_ciBitmap->ReCreateHBitmap(false);	
	}


}

//Effet Noir et Blanc
void CFiltre::BlackAndWhite(HWND hwnd, CIBitmap * m_ciBitmap)
{
	if (m_ciBitmap->GetPtBitmap() != NULL)
	{
		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize;
		long i;

		m_ciBitmap->GetPtBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize);

		for(i = 0;i < m_lSize;i+=4)
		{
			int iValue;
			double Moyenne = ((pBitsSrc[i] * 0.144f) + (pBitsSrc[i + 1] * 0.587f) + (pBitsSrc[i + 2] * 0.299f));
			Moyenne < 128.0 ? iValue = 0: iValue = 255;
			*(pBitsSrc + i) = iValue;
			*(pBitsSrc + i + 1) = iValue;
			*(pBitsSrc + i + 2) = iValue;
			*(pBitsSrc + i + 3) = iValue;
		}

		m_ciBitmap->ReCreateHBitmap(false);
		
	}
}

//Effet Noir et Blanc
void CFiltre::ApplyColor(HWND hwnd, CIBitmap * m_ciBitmap, COLORREF colorref)
{
	if (m_ciBitmap->GetPtBitmap() != NULL)
	{
		COLORREF m_color, m_color2;
		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize;
		long i;

		m_ciBitmap->GetPtBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize);

		for(int y = 0;y < bmHeight;y++)
		{
			for(int x = 0;x < bmWidth;x++)
			{
				m_ciBitmap->GetRGBColorValue(x, y, m_color);

				int m_iBlue = (float)(GetBValue(m_color) * GetBValue(colorref)) / (float)255;
				int m_iGreen = (float)(GetGValue(m_color) * GetGValue(colorref)) / (float)255;
				int m_iRed = (float)(GetRValue(m_color) * GetRValue(colorref)) / (float)255;

				m_color2 = RGB(m_iRed,m_iGreen,m_iBlue);

				m_ciBitmap->SetColorValue(x, y, m_color2);
			}
		}

		m_ciBitmap->ReCreateHBitmap(false);
		
	}
}

//Effet Photo Filtre
void CFiltre::PhotoFiltre(HWND hwnd, CIBitmap * m_ciBitmap, COLORREF colorValue, int iDensity)
{
	if (m_ciBitmap->GetPtBitmap() != NULL )
	{
		CIBitmap * m_ciBitmapTemp = new CIBitmap();
		m_ciBitmapTemp->SetBitmap(m_ciBitmap->GetPtBitmap(),m_ciBitmap->GetBitmapWidth(),m_ciBitmap->GetBitmapHeigth());

		GrayScale(NULL, m_ciBitmapTemp);
		int m_iMax = GetRValue(colorValue);
		if(m_iMax < GetGValue(colorValue))
			m_iMax = GetGValue(colorValue);
		if(m_iMax < GetBValue(colorValue))
			m_iMax = GetBValue(colorValue);
		int m_iDiff = 255 - m_iMax;
		ApplyColor(hwnd,m_ciBitmapTemp,RGB(GetRValue(colorValue) + m_iDiff,GetGValue(colorValue) + m_iDiff,GetBValue(colorValue) + m_iDiff));

		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize;
		long i;
		COLORREF m_color;
		COLORREF m_color2;
		float fAlpha = (float)1 - ((float)(iDensity) / (float)100);
		float fDifference = (float)1 - fAlpha;

		m_ciBitmap->GetPtBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize);

		for(int y = 0;y < bmHeight;y++)
		{
			for(int x = 0;x < bmWidth;x++)
			{
				m_ciBitmap->GetRGBColorValue(x, y, m_color);
				m_ciBitmapTemp->GetRGBColorValue(x, y, m_color2);

				m_color2 = RGB(GetRValue(m_color2) * fDifference + GetRValue(m_color) * fAlpha
								,GetGValue(m_color2) * fDifference + GetGValue(m_color) * fAlpha
								,GetBValue(m_color2) * fDifference  + GetBValue(m_color) * fAlpha);

				m_ciBitmap->SetColorValue(x, y, m_color2);
			}
		}

		m_ciBitmap->ReCreateHBitmap(false);
		delete m_ciBitmapTemp;
	}
}


long CalculPos(long x, long y, long width)
{
	return ((y * width * 3) + x * 3);
}

void AlphaBlending(BYTE * pBitsSrc,const int &iPosSrc, BYTE * pBitSrc2, const int &iPosSrc2, BYTE * pBitDest, const int &iPosDest, const int &alpha)
{
	if(*(pBitSrc2 + iPosSrc2) == 0 && *(pBitSrc2 + iPosSrc2 + 1) == 0 && *(pBitSrc2 + iPosSrc2 + 2) == 0)
		return;

	*(pBitDest + iPosDest) = *(pBitsSrc + iPosSrc) + ((*(pBitSrc2 + iPosSrc2) * alpha) >> 8);
	*(pBitDest + iPosDest + 1) = *(pBitsSrc + iPosSrc + 1) + ((*(pBitSrc2 + iPosSrc2 + 1) * alpha) >> 8);
	*(pBitDest + iPosDest + 2) = *(pBitsSrc + iPosSrc + 2) + ((*(pBitSrc2 + iPosSrc2 + 2) * alpha) >> 8);

	if(*(pBitDest + iPosDest) > 255)
		*(pBitDest + iPosDest) = 255;

	if(*(pBitDest + iPosDest + 1) > 255)
		*(pBitDest + iPosDest + 1) = 255;

	if(*(pBitDest + iPosDest + 2) > 255)
		*(pBitDest + iPosDest + 2) = 255;

}

void AdditiveBlending(BYTE * pBitsSrc,const int &iPosSrc, BYTE * pBitSrc2, const int &iPosSrc2, BYTE * pBitDest, const int &iPosDest, const int &alpha)
{
	unsigned char alut [512];

	for (int i = 0; i < 512; i++)
	{
	  alut [i] = i;
	  if (i > 255) alut [i] = 255;
	}

	*(pBitDest + iPosDest) = alut[*(pBitsSrc + iPosSrc) + *(pBitSrc2 + iPosSrc2)];
	*(pBitDest + iPosDest + 1) = alut[*(pBitsSrc + iPosSrc + 1) + *(pBitSrc2 + iPosSrc2 + 1)];
	*(pBitDest + iPosDest + 2) = alut[*(pBitsSrc + iPosSrc + 2) + *(pBitSrc2 + iPosSrc2 + 2)];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Technique du point milieu Nouvelle version
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CFiltre::Cercle(HWND hwnd, CIBitmap * m_ciBitmap)
{
	int iWidth = m_ciBitmap->GetBitmapWidth();
	int iHeight = m_ciBitmap->GetBitmapHeigth();

	int xFrom = 150,yFrom = 400;
	int x = 0, y = 0, r2;
	int iOldy = 0;
	int iOldx = 0;
	float fAlpha = 0.5f;
	float fDifference2 = fAlpha;
	float fDifference = 1.0f - fAlpha;

	CFiltrePrv m_cFiltrePrv;
	m_cFiltrePrv.GetBitmapInfo(m_ciBitmap);

	//Copie de sauvegarde des données

	m_cFiltrePrv.SetHeightMax(iHeight);
	m_cFiltrePrv.SetWidthMax(iWidth);
	m_cFiltrePrv.SetTypeTransfert(1);

	int Rayon = 100;

	COLORREF m_color = RGB(255,0,0);

	int bFullCircle = 1;
	int bAntialiasing = 1;
	int iTailleCercle = 1;
	int bAntialiasingArriere = 1;

	int iRouge = GetRValue(m_color);
	int iVert = GetGValue(m_color);
	int iBleu = GetBValue(m_color);

	r2 = Rayon * Rayon;
	int iDemiRayon = Rayon>>1;

	iOldx = -Rayon;

	int i = 0;

	y = 0;

	do
	{
		
		float fValue = -sqrt((double)r2 - y*y);

		x = (int)fValue;
		
		iOldx = x;

		if(bFullCircle)
		{
			for(i = iDemiRayon;i <= -iOldx;i++)
			{
				m_cFiltrePrv.WriteCirclePoint2(xFrom,yFrom,y,i,fAlpha,m_color,m_ciBitmap);
				m_cFiltrePrv.WriteCirclePoint2(xFrom,yFrom,i,y,fAlpha,m_color,m_ciBitmap);
			}
		}
		else
		{
			//Longueur de cercle différent
			for(i = -iOldx - iTailleCercle + 1;i <= -iOldx;i++)
			{
				m_cFiltrePrv.WriteCirclePoint2(xFrom,yFrom,y,i,fAlpha,m_color,m_ciBitmap);
				m_cFiltrePrv.WriteCirclePoint2(xFrom,yFrom,i,y,fAlpha,m_color,m_ciBitmap);
			}
		}

		y++;

	}while(y < -iOldx);

	
	if(bFullCircle)
	{
		
		for(y = 0; y < iDemiRayon; y++)
		{
			for(x = 0;x < iDemiRayon;x++)
			{
				m_cFiltrePrv.WriteCirclePoint2(xFrom,yFrom,y,x,fAlpha,m_color,m_ciBitmap);
			}
		}
	}

	y = 0;

	if(bAntialiasing)
	{
		do
		{
			
			float fValue = -sqrt((double)r2 - y*y);

			/////////////////////////////////////////////////////////////////////
			//Antialiasing avant
			/////////////////////////////////////////////////////////////////////

			x = (int)fValue;

			int xLocal = xFrom - x;
			int xLocal2 = xFrom + x;
			int yLocal = yFrom + y;
			int yLocal2 = yFrom - y;

			m_cFiltrePrv.DefineZoneBilinear(xLocal,yLocal,m_ciBitmap,2);
			m_cFiltrePrv.DefineZoneBilinear(xLocal2,yLocal,m_ciBitmap,2);
			m_cFiltrePrv.DefineZoneBilinear(xLocal,yLocal2,m_ciBitmap,2);
			m_cFiltrePrv.DefineZoneBilinear(xLocal2,yLocal2,m_ciBitmap,2);

			xLocal = xFrom - y;
			xLocal2 = xFrom + y;
			yLocal = yFrom + x;
			yLocal2 = yFrom - x;

			m_cFiltrePrv.DefineZoneBilinear(xLocal,yLocal,m_ciBitmap,2);
			m_cFiltrePrv.DefineZoneBilinear(xLocal2,yLocal,m_ciBitmap,2);
			m_cFiltrePrv.DefineZoneBilinear(xLocal,yLocal2,m_ciBitmap,2);
			m_cFiltrePrv.DefineZoneBilinear(xLocal2,yLocal2,m_ciBitmap,2);

			y++;

		}while(y < -iOldx);
	}

	//Destruction de la sauvegarde
	m_cFiltrePrv.DeleteBitsSrv();

	////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////
}

//Effet LensFlare

void CFiltre::LensFlare(HWND hwnd, CIBitmap * m_ciBitmap, const int &iPosX, const int &iPosY, const int &iPuissance, const int &iType, const int &iIntensity, const int &iColor, const int &iColorIntensity)
{
	int iWidth = m_ciBitmap->GetBitmapWidth();
	int iHeight = m_ciBitmap->GetBitmapHeigth();
	LensFlare(hwnd, m_ciBitmap,iWidth, iHeight, iPosX, iPosY, iPuissance, iType, iIntensity, iColor, iColorIntensity);
}

void CFiltre::LensFlare(HWND hwnd, CIBitmap * m_ciBitmap,const int &iWidth, const int &iHeight, const int &iPosX, const int &iPosY, const int &iPuissance, const int &iType, const int &iIntensity, const int &iColor, const int &iColorIntensity)
{
	CFiltrePrv m_cFiltrePrv;

	//Chargement du lensflare 1
	if (m_ciBitmap->hBitmap)
	{
		//long Moyenne;
		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;//,bmWidth2, bmHeight2;
		unsigned long m_lSize;
	//	HBITMAP hBitmap;
		int i = 0;

		int x = iPosX;
		int y = iPosY;

		int iRayon = iPuissance;

		m_cFiltrePrv.SetBilinearFiltering(iType);

		int iMaxX = (iWidth + (m_ciBitmap->GetBitmapWidth() - iWidth)) >> 1;
		int iMaxY = (iHeight + (m_ciBitmap->GetBitmapHeigth() - iHeight)) >> 1;
	
		m_cFiltrePrv.SetHeightMax(iHeight);
		m_cFiltrePrv.SetWidthMax(iWidth);
		m_cFiltrePrv.SetIntensity(iColorIntensity);

		int iLargeur = (x - (iMaxX)) << 1;
			
		iLargeur = -iLargeur;

		int iHauteur = (y - (iMaxY)) << 1;

		iHauteur = -iHauteur;

		int x2 = iLargeur + x;
		int y2 = iHauteur + y;

 		float a = ((float)(y2 - y) / (float)(x2 - x));

 		float b = y - (a * x);

		int iTaille = iRayon;

		int iCouleur1 = iColor + 50;//(iColor + 50) % 360;
		if(iCouleur1 > 360)
			iCouleur1 -= 360;

		int iCouleur2 = iColor + 100;//(iColor + 100) % 360;
		if(iCouleur2 > 360)
			iCouleur2 -= 360;

		int iCouleur3 = iColor + 200;//(iColor + 200) % 360;
		if(iCouleur3 > 360)
			iCouleur3 -= 360;


		HSB m_value2 = {iCouleur3,100,100};
		COLORREF m_rgbValue3;
		CColor::HSBToRGB(m_value2,m_rgbValue3);
		
		HSB m_value = {iCouleur1,100,100};
		COLORREF m_rgbValue1;
		CColor::HSBToRGB(m_value,m_rgbValue1);

		HSB m_value3 = {iCouleur2,100,100};
		COLORREF m_rgbValue2;
		CColor::HSBToRGB(m_value3,m_rgbValue2);

		HSB m_value4 = {iColor,iColorIntensity,100};
		COLORREF m_rgbValue4;
		CColor::HSBToRGB(m_value4,m_rgbValue4);

		float fRayon = iRayon*(iIntensity / 10.0f);


		iRayon = iRayon * 0.75f;

		//return;

		////////////////////////////////////////////////////////////////
		


		//////////////////////////////////////////////////////////////////
		//1er Halo
		//////////////////////////////////////////////////////////////////

		x = iPosX + (iLargeur * 0.875f);
		y = iPosY + (iHauteur * 0.875f);

		iTaille = iRayon;
		
		//y = a * x + b;

		m_cFiltrePrv.Halo(x,y,m_ciBitmap,iCouleur1,iTaille,8,0.7f);

		//////////////////////////////////////////////////////////////////
		//1er Halo
		//////////////////////////////////////////////////////////////////

		x = iPosX - (iLargeur*0.125f);
		y = iPosY - (iHauteur*0.125f);

		iTaille = iRayon >> 1;
		
		//y = a * x + b;

		m_cFiltrePrv.Halo(x,y,m_ciBitmap,iCouleur2,iTaille,5,0.7f);


		////////////////////////////////////////////////////////////////
		//Small Burst
		////////////////////////////////////////////////////////////////
		
		x = iPosX + (iLargeur >>1);
		y = iPosY + (iHauteur >>1);

		iTaille = iRayon >> 3;

		m_cFiltrePrv.Burst(x, y,m_ciBitmap,iTaille,iColor,25,100);


		////////////////////////////////////////////////////////////////
		//Deuxieme Burst
		////////////////////////////////////////////////////////////////


		x = iPosX + (iLargeur * 0.625f);
		y = iPosY + (iHauteur * 0.625f);

		iTaille = iRayon / 10;

		m_cFiltrePrv.Burst(x, y,m_ciBitmap,iTaille,iCouleur2);

		x = (iPosX + (iLargeur * 0.75f));
		y = (iPosY + (iHauteur * 0.75f));

		iTaille = iRayon >> 3;

		m_cFiltrePrv.Burst(x, y,m_ciBitmap,iTaille,iCouleur3,25,100);


		////////////////////////////////////////////////////////////////

		x = (iPosX + (iLargeur * 1.125f));
		y = (iPosY + (iHauteur * 1.125f));

		iTaille = iRayon << 2;
		
		m_cFiltrePrv.HaloGradient(x2,y2,m_ciBitmap,iTaille,iWidth/20,0.7f);

		////////////////////////////////////////////////////////////////

		
		x = (iPosX + (iLargeur * 0.625f));
		y = (iPosY + (iHauteur * 0.625f));

		if(iLargeur > 0)
			x += iRayon >> 1;
		else
			x -= iRayon >> 1;

		y = a * x + b;

		m_cFiltrePrv.Circle(x,y,m_ciBitmap,m_rgbValue1,iRayon*0.75,0.8f);

		x = (iPosX + (iLargeur * 0.625f));
		//y = (iPosY + (iHauteur * 6 / 8));

		if(iLargeur > 0)
			x += iRayon >> 1;
		else
			x -= iRayon >> 1;

		y = a * x + b;

		m_cFiltrePrv.Circle(x,y,m_ciBitmap,m_rgbValue1,iRayon/2,0.8f);

		x = (iPosX + (iLargeur * 0.625f));
		//y = (iPosY + (iHauteur * 6 / 8));

		if(iLargeur > 0)
			x += iRayon*0.6f;
		else
			x -= iRayon*0.6f;

		y = a * x + b;

		m_cFiltrePrv.Circle(x,y,m_ciBitmap,m_rgbValue1,iRayon/4,0.8f);

		
		x = (iPosX + (iLargeur * 0.4f));
		y = (iPosY + (iHauteur * 0.4f));

		if(iLargeur > 0)
			x -= (iRayon >> 3);
		else
			x += (iRayon >> 3);

		y = a * x + b;

		m_cFiltrePrv.Circle(x,y,m_ciBitmap,m_rgbValue1,iRayon/4,0.8f);


		x = (iPosX + (iLargeur / 5));
		y = (iPosY + (iHauteur / 5));

		m_cFiltrePrv.Circle(x,y,m_ciBitmap,m_rgbValue3,iRayon*0.75,0.8f);

		if(iLargeur > 0)
			x += iRayon*0.2f;
		else
			x -= iRayon*0.2f;

		y = a * x + b;

		m_cFiltrePrv.Circle(x,y,m_ciBitmap,m_rgbValue3,iRayon*0.4,0.8f);

		x = (iPosX + (iLargeur / 5));
		y = (iPosY + (iHauteur / 5));

		if(iLargeur > 0)
			x -= iRayon*0.2f;
		else
			x += iRayon*0.2f;

		y = a * x + b;

		m_cFiltrePrv.Circle(x,y,m_ciBitmap,m_rgbValue3,iRayon*0.2,0.8f);

		x = (iPosX + (iLargeur * 0.75f));
		y = (iPosY + (iHauteur * 0.75f));

		m_cFiltrePrv.Circle(x,y,m_ciBitmap,m_rgbValue2,iRayon*0.2,0.8f);

		x = iPosX;
		y = iPosY;

		iTaille = fRayon * 0.5f;


		m_cFiltrePrv.CircleGradient(x,y,m_ciBitmap,m_rgbValue4,fRayon);

		//x = iPosX;
		//y = iPosY;

		m_cFiltrePrv.Halo(x,y,m_ciBitmap,iColor,iTaille,8,0.8f,0);


		m_cFiltrePrv.Burst(x, y,m_ciBitmap,iTaille*0.9f,iColor,iIntensity,iColorIntensity);

		//Trait lumineux

		int iIntRayon = (int)fRayon/2;
		
		for (i = 0; i <= 360; i++) 
		{
			
			float fxValue = cos(i * CONVRADIAN) * rand();
			float fyValue = sin(i * CONVRADIAN) * rand();

			fxValue = (int)fxValue % (iIntRayon);
			fyValue = (int)fyValue % (iIntRayon);
			
			m_cFiltrePrv.MidpointLine(x,y,x+fxValue,y+fyValue,m_ciBitmap,RGB(255,255,255),0.9f,true);
			m_cFiltrePrv.MidpointLine(x-fxValue,y-fyValue,x,y,m_ciBitmap,RGB(255,255,255),0.9f,true);
		}

		if(iLargeur > 0)
			x = iPosX - iRayon;
		else
			x = iPosX + iRayon;

		y = a*x + b;


		m_cFiltrePrv.Circle(x,y,m_ciBitmap,m_rgbValue1,iRayon*4,0.95f);
	
		m_ciBitmap->ReCreateHBitmap(false);

	}
}


void CFiltre::Negatif(HWND hwnd, CIBitmap * m_ciBitmap)
{
	if (m_ciBitmap->GetPtBitmap() != NULL)
	{
		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize;

		m_ciBitmap->GetPtBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize);

		__int64 i = 0;
		i = ~i;                                 // 0xffffffffffffffff    

		// 8 pixels are processed in one loop
		int nLoop = m_lSize/8;

		__m64* pIn = (__m64*) pBitsSrc;          // input pointer
		__m64* pOut = (__m64*) pBitsSrc;           // output pointer


		_mm_empty();                            // emms

		__m64 n1 = Get_m64(i);

		for (int j = 0; j < nLoop; j++ )
		{
			*pOut = _mm_subs_pu8 (n1 , *pIn);     // Unsigned subtraction with 
												// saturation.
												// tmp = n1 - *pIn  for each byte
			pIn++;                              // next 8 pixels
			pOut++;
		}

		_mm_empty();                            // emms

		m_ciBitmap->ReCreateHBitmap(false);
		
	}
}

#define ELEM_SWAP(a,b) { register short t=(a);(a)=(b);(b)=t; }

short CFiltre::quick_select(short * arr, int n) 
{
    int low, high ;
    int median;
    int middle, ll, hh;

    low = 0 ; high = n-1 ; median = (low + high) / 2;
    for (;;) {
        if (high <= low) /* One element only */
            return arr[median] ;

        if (high == low + 1) {  /* Two elements only */
            if (arr[low] > arr[high])
                ELEM_SWAP(arr[low], arr[high]) ;
            return arr[median] ;
        }

    /* Find median of low, middle and high items; swap into position low */
    middle = (low + high) / 2;
    if (arr[middle] > arr[high])    ELEM_SWAP(arr[middle], arr[high]) ;
    if (arr[low] > arr[high])       ELEM_SWAP(arr[low], arr[high]) ;
    if (arr[middle] > arr[low])     ELEM_SWAP(arr[middle], arr[low]) ;

    /* Swap low item (now in position middle) into position (low+1) */
    ELEM_SWAP(arr[middle], arr[low+1]) ;

    /* Nibble from each end towards middle, swapping items when stuck */
    ll = low + 1;
    hh = high;
    for (;;) {
        do ll++; while (arr[low] > arr[ll]) ;
        do hh--; while (arr[hh]  > arr[low]) ;

        if (hh < ll)
        break;

        ELEM_SWAP(arr[ll], arr[hh]) ;
    }

    /* Swap middle item (in position low) back into correct position */
    ELEM_SWAP(arr[low], arr[hh]) ;

    /* Re-set active partition */
    if (hh <= median)
        low = ll;
        if (hh >= median)
        high = hh - 1;
    }
}

#undef ELEM_SWAP

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

void CFiltre::Median(HWND hwnd, CIBitmap * m_ciBitmap)
{
	if (m_ciBitmap->GetPtBitmap() != NULL)
	{
		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize;
		m_ciBitmap->GetBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize);
		
		long lWidthSize = m_ciBitmap->GetWidthSize();
		long i = 0, m_lLocalisation = 0;
		long k =0, j= 0;
		long xMax = bmWidth;
		long yMax = bmHeight;
		long m_lValue = 3;

		short m_ZeroTabValue[27];
		short m_TabRValue[9];
		short m_TabGValue[9];
		short m_TabBValue[9];

		for(j = 0;j < 9;j++)
			m_ZeroTabValue[j] = 0;

		int iSize = 9*sizeof(short);

		if(m_lpFiltreEffet != NULL)
		{
			SendDlgItemMessage(m_lpFiltreEffet->hDlg,m_lpFiltreEffet->iHandlePrgBar,PBM_SETRANGE,0,MAKELPARAM(0,bmHeight));
			SendDlgItemMessage(m_lpFiltreEffet->hDlg,m_lpFiltreEffet->iHandlePrgBar,PBM_SETSTEP, (WPARAM) 1, 0); 
		}

		for(long y=0; y< bmHeight;y++)
		{
			if(m_lpFiltreEffet != NULL)
			{
				SendDlgItemMessage(m_lpFiltreEffet->hDlg,m_lpFiltreEffet->iHandlePrgBar,PBM_STEPIT, 0, 0); 
			}


			for(long x=0; x<bmWidth;x++)
			{

				memcpy(m_TabRValue,m_ZeroTabValue,iSize);
				memcpy(m_TabGValue,m_ZeroTabValue,iSize);
				memcpy(m_TabBValue,m_ZeroTabValue,iSize);

				k = 0;
			
				for (i=0; i < 3; i++)
				{
					int v = y - 1 + i;
					int u = x - 1;

					int lLocalisation = (v * lWidthSize) + (u << 2);

					if(v >= 0 && v < bmHeight)
					{
						for (j=0; j < 3; j++)
						{
							if(u >= 0 && u < bmWidth)
							{
								m_TabRValue[k] = *(pBitsSrc + lLocalisation);
								m_TabGValue[k] = *(pBitsSrc + lLocalisation+1);
								m_TabBValue[k] = *(pBitsSrc + lLocalisation+2);
							}
							lLocalisation += 4;
							u++;k++;
						}
					}
					else
					{
						k += 3;
					}
				}

				*(pBitsSrc + m_lLocalisation) = quick_select(m_TabRValue,9);
				*(pBitsSrc + m_lLocalisation + 1) = quick_select(m_TabGValue,9);
				*(pBitsSrc + m_lLocalisation + 2) = quick_select(m_TabBValue,9);

				m_lLocalisation+=4;
			}

		}

		m_ciBitmap->SetBitmap(pBitsSrc, bmWidth, bmHeight);

		if(pBitsSrc != NULL)
			delete[] pBitsSrc;

		pBitsSrc = NULL;
	}
}


///////////////////////////////////////////////////////////////////
//Effet Lightness
///////////////////////////////////////////////////////////////////

void CFiltre::Lightness(HWND hwnd, CIBitmap * m_ciBitmap,int m_lightness)
{
	if (m_ciBitmap->GetPtBitmap() != NULL)
	{

		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize;

		m_ciBitmap->GetPtBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize);


		int nChange = 2.55 * (double) (m_lightness);

		if ( nChange > 255 )
			nChange = 255;
		else if ( nChange < -255 )
			nChange = -255;

		BYTE b = (BYTE) abs(nChange);

		// make 64 bits value with b in each byte
		__int64 c = b;

		for ( int i = 1; i <= 7; i++ )
		{
			c = c << 8;
			c |= b;
		}

		// 8 pixels are processed in one loop
		int nNumberOfLoops = m_lSize / 8;

		__m64* pIn = (__m64*) pBitsSrc;          // input pointer
		__m64* pOut = (__m64*) pBitsSrc;           // output pointer

		_mm_empty();                            // emms

		__m64 nChange64 = Get_m64(c);

		if ( nChange > 0 )
		{
			for (int i = 0; i < nNumberOfLoops; i++ )
			{
				*pOut = _mm_adds_pu8(*pIn, nChange64); // Unsigned addition 
													 // with saturation.
													 // tmp = *pIn + nChange64
													 // for each byte

				//*pOut = tmp;

				pIn++;                               // next 8 pixels
				pOut++;
			}
		}
		else
		{
			for (int i = 0; i < nNumberOfLoops; i++ )
			{
				*pOut = _mm_subs_pu8(*pIn, nChange64); // Unsigned subtraction 
													 // with saturation.
													 // tmp = *pIn - nChange64
													 // for each byte

				//*pOut = tmp;

				pIn++;                                      // next 8 pixels
				pOut++;
			}
		}

		m_ciBitmap->ReCreateHBitmap(false);	
		_mm_empty();                            // emms
	}

}

///////////////////////////////////////////////////////////////////
//Effet Contrast
///////////////////////////////////////////////////////////////////

void CFiltre::Contrast(HWND hwnd, CIBitmap * m_ciBitmap,double contrast, BYTE offset)
{
	//BITMAP bitmap;
	double m_contrast = contrast;
	register double csupp = contrast * ((double)offset - 250.0) + 125.0;

	OldCtrlWord = Changex87FPToTrunc();

	short m_dTemp[256];
	long i = 0;

	#pragma omp parallel for private(i)
		for(i=0; i< 256; i++)
		{
			m_dTemp[i] = FloatToInt(contrast * (i) + csupp);
			if(m_dTemp[i] < 0 || m_dTemp[i] > 255)
				m_dTemp[i] = (m_dTemp[i] > 255) ? 255 : 0;
		}

	if (m_ciBitmap->GetPtBitmap() != NULL)
	{
		
		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize;
		m_ciBitmap->GetPtBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize);

		long lWidthSize = m_ciBitmap->GetWidthSize();

		long size = bmWidth * bmHeight * 4;

		#pragma omp parallel for private(i)
			for(i=0; i< size; i++)
				*(pBitsSrc + i) = m_dTemp[(int)pBitsSrc[i]];

		m_ciBitmap->ReCreateHBitmap(false);	
	}
}

///////////////////////////////////////////////////////////////////
//Effet Contrast
///////////////////////////////////////////////////////////////////

void CFiltre::ReplaceColor(CIBitmap * m_ciBitmap, const COLORREF &m_color1, const COLORREF &m_color2, const int &iLarge)
{

	if (m_ciBitmap->GetPtBitmap() != NULL)
	{
		BYTE * pBitsSrc = NULL;
		UINT bmWidth, bmHeight;
		unsigned long m_lSize;
		m_ciBitmap->GetBitmap(&pBitsSrc, bmWidth, bmHeight, m_lSize);

		long i = 0;
		long lWidthSize = m_ciBitmap->GetWidthSize();

		BYTE red = GetRValue(m_color1);
		BYTE green = GetGValue(m_color1);
		BYTE blue = GetBValue(m_color1);

		BYTE red2 = GetRValue(m_color2);
		BYTE green2 = GetGValue(m_color2);
		BYTE blue2 = GetBValue(m_color2);

		for(long y=0; y< bmHeight; y++)
		{
			for(long x=0; x<bmWidth; x++)
			{
				//Recherche de la couleur à remplacer
				i = (y * lWidthSize) + (x * 3);
				if(*(pBitsSrc + i) == red && *(pBitsSrc + i + 1) == green && *(pBitsSrc + i + 2) == blue)
				{
					*(pBitsSrc + i) = red2;
					*(pBitsSrc + i + 1) = green2;
					*(pBitsSrc + i + 2) = blue2;
				}
	
			}
		}

		m_ciBitmap->SetBitmap(pBitsSrc, bmWidth, bmHeight);
		delete[] pBitsSrc;
		
	}
}