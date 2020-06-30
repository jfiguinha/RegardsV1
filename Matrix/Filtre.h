// Filtre.h: interface for the CFiltre class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILTRE_H__ACB5BE83_FA20_476B_B592_FC3FFE734F2F__INCLUDED_)
#define AFX_FILTRE_H__ACB5BE83_FA20_476B_B592_FC3FFE734F2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "matrixdoublecomplex.h"
#include "fft.h"

class CFiltre  
{
public:

	CFiltre();

	virtual ~CFiltre();

	static CMatrix Filter2(CMatrix &G, CMatrix &F);

	static CMatrix Filter2Conv(CMatrix &G, CMatrix &F);


	static CMatrixDoubleComplex calcul_filtrewiener(const int &iHeightImage, const int &iWidthImage,const int &iHeightFiltre,const int &iWidthFiltre, const double &rho);

	static CMatrix restdct(CMatrix &m_MatrixImage,CMatrixDoubleComplex &G);

	static CMatrix restWiener(CMatrix &m_MatrixImage,CMatrixDoubleComplex &G);
};

#endif // !defined(AFX_FILTRE_H__ACB5BE83_FA20_476B_B592_FC3FFE734F2F__INCLUDED_)
