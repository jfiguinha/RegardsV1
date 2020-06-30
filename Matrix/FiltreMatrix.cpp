// Filtre.cpp: implementation of the CFiltre class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FiltreMatrix.h"
#include "Matrix.h"
#include "MatrixDoubleComplex.h"
#include "FFt.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFiltreMatrix::CFiltreMatrix()
{

}

CFiltreMatrix::~CFiltreMatrix()
{

}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CMatrix CFiltreMatrix::Filter2Conv(const CMatrix &G,const CMatrix &F)
{

	CMatrix in;
	CMatrix stencil;

	stencil = F;

	stencil.rot90(2);

	in = CFFt::conv2fft(G,stencil);

	stencil.AllocationMemoire(G.GetTheMValue(),G.GetNValue());

	double * m_dInData = in.GetDataPt();
	double * m_dOutData = stencil.GetDataPt();

	int m_iGN = G.GetNValue();
	int m_iFN = in.GetNValue();
	int m_iGM = G.GetTheMValue();
	int m_iFM = in.GetTheMValue();

	int iColStart = (m_iFM - m_iGM) >> 1;
	int iRowStart = (m_iFN - m_iGN) >> 1;

	for(int i = 0;i < G.GetTheMValue();i++)
	{
		int iPos1 = i * m_iGN;
		int iPos2 = (i + iColStart) * m_iFN;

		memcpy(m_dOutData + iPos1,m_dInData + iPos2 + iRowStart,m_iGN * sizeof(double));
	}

	return stencil;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CMatrix CFiltreMatrix::Filter2(const CMatrix &G,const CMatrix &F)
{
	
	CMatrix out(G.GetTheMValue(),G.GetNValue());
	double * m_dGData = G.GetDataPt();
	double * m_dFData = F.GetDataPt();
	double * m_dOutData = out.GetDataPt();

	int iMiddleYPoint = F.GetTheMValue() >> 1;
	int iMiddleXPoint = F.GetNValue() >> 1;


	int m_iGN = G.GetNValue();
	int m_iFN = F.GetNValue();
	int m_iGM = G.GetTheMValue();
	int m_iFM = F.GetTheMValue();

	int i = 0,j = 0;

	double m_MatriceValue = 0.0;


	//#pragma omp parallel private(i,j)
	//{
		for(int i = 0;i < m_iGM;i++)
		{
			//#pragma omp for schedule(dynamic,1) nowait

			for(int j = 0;j < m_iGN;j++)
			{
				double m_dValue = 0.0;
		
				int iYPos = i - iMiddleYPoint;

				for(int k = 0;k < m_iFM;k++)
				{
					int iXPos = j - iMiddleXPoint;

					int iPos1 = iYPos * m_iGN;
					int iPos2 = k * m_iFN;

					if(iYPos >= 0 && iYPos < m_iGM)
					{
						for(int l = 0;l < m_iFN;l++)
						{
							if(iXPos >= 0 && iXPos < m_iGN)
								m_dValue += m_dGData[iPos1 + iXPos] * m_dFData[iPos2 + l] ;

							iXPos++;
						}
					}

					iYPos++;
				}

				m_dOutData[i * m_iGN + j] = m_dValue;
			}
		}
	//}

	return out;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Calcul du filtre inverse par la méthode de Wiener
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

CMatrixDoubleComplex CFiltreMatrix::calcul_filtrewiener(const int &iHeightImage, const int &iWidthImage,const CMatrix &FiltreDegradation, const double &rho)
{
	CMatrix h;
	CMatrix hh;
	CMatrixDoubleComplex H, H2;

	int iHeightFiltre = FiltreDegradation.GetTheMValue();
	int iWidthFiltre = FiltreDegradation.GetNValue();

	h = FiltreDegradation;

	double m_dValue = h.Prod();

	h = h / m_dValue;

	hh.zeros(iHeightImage,iWidthImage); 

	int iCentreImageHauteur = iHeightImage >> 1;
	int iCentreImageLargeur = iWidthImage >> 1;

	int iCentreFiltreHauteur = iHeightFiltre >> 1;
	int iCentreFiltreLargeur = iWidthFiltre >> 1;

	int iHauteurStart = iCentreImageHauteur - iCentreFiltreHauteur;
	int iLargeurStart = iCentreImageLargeur - iCentreFiltreLargeur;

	hh.SetMatrixValue(iHauteurStart,iLargeurStart,h);

	hh.fftshift();

	//hh.show();

	//Calcul de la Transformé de fournier


	H = CFFt::fft2(hh);

	//H.show();

	H2 = H;


	//Calcul du filtre inverse

	H2.GetConjMatrix();

	//printf("Multiplication \n");

	H.ArrayLocalMulti(H2);

	//printf("Addition \n");

	H.ArrayAdd(rho);

	//printf("Multiplication \n");

	//Filtre de Wiener
	H2.ArrayDivi(H);

	//H2.show();

	return H2;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Restauration de l'image par la méthode cosinus
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

CMatrix CFiltreMatrix::restdct(const CMatrix &m_MatrixImage,const CMatrixDoubleComplex &G)
{
	CMatrixDoubleComplex dctIdct;
	CMatrix dctIf;

	dctIf = CFFt::dct2(m_MatrixImage);

	//dctIf.show();

	dctIdct = G;

	//printf("Application du filtre inverse \n");

	dctIdct.ArrayLocalMulti(dctIf);

	//dctIdct.show();

	dctIf = dctIdct.GetRealMatrix();

	//dctIf.show();
	
	//printf("Calcul de la idct du filtre obtenue \n");

	return CFFt::idct2(dctIf);
}

///////////////////////////////////////////////////////////////////////
//Liste des différents filtres
///////////////////////////////////////////////////////////////////////

CMatrix CFiltreMatrix::GetGaussianMatrix()
{
	CMatrix m_newMatrix(9,9);
	double * m_dData = m_newMatrix.GetDataPt2();
	double kernel[] = {0, 0, 1, 1, 1, 1, 1, 0, 0,
				0, 1, 2, 3, 3, 3, 2, 1, 0,
				1, 2, 3, 6, 7, 6, 3, 2, 1,
				1, 3, 6, 9, 11, 9, 6, 3, 1,
				1, 3, 7, 11, 12, 11, 7, 3, 1,
				1, 3, 6, 9, 11, 9, 6, 3, 1,
				1, 2, 3, 6, 7, 6, 3, 2, 1,
				0, 1, 2, 3, 3, 3, 2, 1, 0,
				0, 0, 1, 1, 1, 1, 1, 0, 0};

	memcpy(m_dData,kernel,81*sizeof(double));

	double m_dValue = m_newMatrix.Prod();

	m_newMatrix = m_newMatrix / m_dValue;

	return m_newMatrix;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

CMatrix CFiltreMatrix::GetBlurMatrix()
{
	CMatrix m_newMatrix(3,3);
	double * m_dData = m_newMatrix.GetDataPt2();
	double kernel[] = {1,1,1,1,1,1,1,1,1};

	memcpy(m_dData,kernel,9*sizeof(double));

	double m_dValue = m_newMatrix.Prod();

	m_newMatrix = m_newMatrix / m_dValue;


	return m_newMatrix;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

CMatrix CFiltreMatrix::GetMotionBlurMatrix(const double &sigma,const int &Width)
{
	CMatrix m_newMatrix(3,Width);

	const int KernelRank = 3;
	double alpha;
	int bias;
	register long i;

	double * kernel= m_newMatrix.GetDataPt2();

	for (i=0; i < Width; i++)
		kernel[i]=0.0;

	bias=KernelRank*Width;

	double m_dValue = (2.0*KernelRank*KernelRank*sigma*sigma);
	double m_dValue2 = MagickSQ2PI*sigma;

	for (i=0; i < bias; i++)
	{
		alpha=exp(-((double) i*i)/m_dValue);
		kernel[i/KernelRank]+=alpha/m_dValue2;
	}

	//normalize=0;
	//for (i=0; i < Width; i++)
	//	normalize+=kernel[i];

	//for (i=0; i < Width; i++)
	//	kernel[i]/=normalize;

	m_dValue = m_newMatrix.Prod();

	m_newMatrix = m_newMatrix / m_dValue;


	return m_newMatrix;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

CMatrix CFiltreMatrix::GetSoftenMatrix()
{
	CMatrix m_newMatrix(3,3);
	double * m_dData = m_newMatrix.GetDataPt2();
	double kernel[] = {1,1,1,1,8,1,1,1,1};

	memcpy(m_dData,kernel,9*sizeof(double));

	double m_dValue = m_newMatrix.Prod();

	m_newMatrix = m_newMatrix / m_dValue;


	return m_newMatrix;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

CMatrix CFiltreMatrix::GetSharpenMatrix()
{
	CMatrix m_newMatrix(3,3);
	double * m_dData = m_newMatrix.GetDataPt2();
	double kernel[] = {-1,-1,-1,-1,15,-1,-1,-1,-1};

	memcpy(m_dData,kernel,9*sizeof(double));

	double m_dValue = m_newMatrix.Prod();

	m_newMatrix = m_newMatrix / m_dValue;


	return m_newMatrix;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

CMatrix CFiltreMatrix::GetEdgeMatrix()
{
	CMatrix m_newMatrix(3,3);
	double * m_dData = m_newMatrix.GetDataPt2();
	double kernel[] = {-1,-1,-1,-1,8,-1,-1,-1,-1};

	memcpy(m_dData,kernel,9*sizeof(double));

	double m_dValue = m_newMatrix.Prod();

	m_newMatrix = m_newMatrix / m_dValue;


	return m_newMatrix;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

CMatrix CFiltreMatrix::GetEmbossMatrix()
{
	CMatrix m_newMatrix(3,3);
	double * m_dData = m_newMatrix.GetDataPt2();
	double kernel[] = {-1,0,0,0,0,0,0,0,1};

	memcpy(m_dData,kernel,9*sizeof(double));

	double m_dValue = m_newMatrix.Prod();

	m_newMatrix = m_newMatrix / m_dValue;


	return m_newMatrix;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Restauration de l'image par la méthode cosinus
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
CMatrix CFiltreMatrix::restdct(const CMatrix &m_MatrixImage,const int &iHeightFiltre,const int &iWidthFiltre, const double &rho)
{
	CMatrix m_MatrixImageRest, dctIf;
	CMatrixDoubleComplex G,dctIdct;
	CMatrixDoubleComplex G2(m_MatrixImage.GetTheMValue(),m_MatrixImage.GetNValue());

	G = calcul_filtrewiener(m_MatrixImage.GetTheMValue() * 2,m_MatrixImage.GetNValue() * 2,iHeightFiltre,iWidthFiltre, rho);

	double * complexData = G.GetDataPt();
	double * complexDestData = G2.GetDataPt();

	int k = 0;

	for(int i = 0;i < m_MatrixImage.GetTheMValue();i++)
	{
		for(int j = 0;j < m_MatrixImage.GetNValue() * 2;j+=2)
		{
			complexDestData[k++] = complexData[i * m_MatrixImage.GetNValue() * 2 + j];
			complexDestData[k++] = complexData[i * m_MatrixImage.GetNValue() * 2 + j + 1];
		}
	}

	G.DesAllocationMemoire();

	dctIf = CFFt::dct2(m_MatrixImage);

	//dctIf.show();
	
	G2.ArrayLocalMulti(dctIf);

	//dctIdct.show();

	m_MatrixImageRest = CFFt::idct2(G2.GetRealMatrix());

	return m_MatrixImageRest;
}*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


CMatrix CFiltreMatrix::restWiener(const CMatrix &m_MatrixImage,const CMatrixDoubleComplex &G)
{
	CMatrix m_MatrixImageRest;

	CMatrixDoubleComplex dctIdct;

	dctIdct = CFFt::fft2(m_MatrixImage);

	dctIdct.ArrayLocalMulti(G);

	CFFt::ifft2(dctIdct);

	return dctIdct.GetRealMatrix();
}
