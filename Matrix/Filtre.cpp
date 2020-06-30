// Filtre.cpp: implementation of the CFiltre class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Filtre.h"
#include "FFt.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFiltre::CFiltre()
{

}

CFiltre::~CFiltre()
{

}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CMatrix CFiltre::Filter2Conv(CMatrix &G, CMatrix &F)
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

		memcpy_optimized(m_dOutData + iPos1,m_dInData + iPos2 + iRowStart,m_iGN * sizeof(double));
	}

	return stencil;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CMatrix CFiltre::Filter2(CMatrix &G, CMatrix &F)
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


	#pragma omp parallel private(i,j)
	{
		for(int i = 0;i < m_iGM;i++)
		{
			#pragma omp for schedule(dynamic,1) nowait

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
	}

	return out;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Calcul du filtre inverse par la méthode de Wiener
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

CMatrixDoubleComplex CFiltre::calcul_filtrewiener(const int &iHeightImage, const int &iWidthImage,const int &iHeightFiltre,const int &iWidthFiltre, const double &rho)
{
	CMatrix h;
	CMatrix hh;
	CMatrixDoubleComplex H, H2;
	
	h.ones(iHeightFiltre,iWidthFiltre);

	double m_dValue = h.Prod();

	h = h / m_dValue;
	//h.show();

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

CMatrix CFiltre::restdct(CMatrix &m_MatrixImage,CMatrixDoubleComplex &G)
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


CMatrix CFiltre::restWiener(CMatrix &m_MatrixImage,CMatrixDoubleComplex &G)
{
	CMatrix m_MatrixImageRest;

	CMatrixDoubleComplex dctIdct;

	dctIdct = CFFt::fft2(m_MatrixImage);

	dctIdct.ArrayLocalMulti(G);

	CFFt::ifft2(dctIdct);

	return dctIdct.GetRealMatrix();
}
