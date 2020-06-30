// MatrixDoubleComplex.h: interface for the CMatrixDoubleComplex class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATRIXDOUBLECOMPLEX_H__510B67DD_6499_4C8A_ADA0_9E97E506ABFA__INCLUDED_)
#define AFX_MATRIXDOUBLECOMPLEX_H__510B67DD_6499_4C8A_ADA0_9E97E506ABFA__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include <windows.h>

#include "matrix.h"
#include <stdio.h>

class CMatrixDoubleComplex  
{
public:

	CMatrixDoubleComplex();

	virtual ~CMatrixDoubleComplex();

	CMatrixDoubleComplex(const int &M, const int &N);

	CMatrixDoubleComplex(const CMatrix &Source);

	CMatrixDoubleComplex(const CMatrixDoubleComplex & Source);

	void AllocationMemoire();
	void AllocationMemoire(const int &M, const int &N);
	void DesAllocationMemoire();

	//////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

	const int &GetTheMValue() const
	{
		return m_iM;
	}

	//////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

	double * GetDataPt() const
	{
		return X;
	}

	//////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

	double * GetDataPt2()
	{
		return X;
	}


	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	const int &GetNValue() const 
	{
		return m_iN;
	};

	//////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

	const long &GetLValue() const
	{
		return m_iL;
	};

	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	CMatrixDoubleComplex& operator = (const CMatrixDoubleComplex& a);

	void ArrayLocalMulti(const CMatrixDoubleComplex& b);

	void ArrayLocalMulti(CMatrix& b);

	void ArrayDivi (const double& b);

	void ArrayAdd (const double& b);

	void ArrayDivi(CMatrixDoubleComplex& b);

	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	void rot90(const int &iNb = 1);

	void GetRealMatrix(CMatrix &m_Matrix);

	CMatrix GetRealMatrix();

	void show();

	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	void Prime();

	CMatrixDoubleComplex operator / (const double& b);

	void inverseAll();

	void GetConjMatrix();

	void ones(const int &M, const int &N);

	void zeros(const int &M, const int &N);


private:

	inline void deplacementComplex(double * & data,const int &iPosDepart,const int &iPosFin);
	void primeComplex(double * & data,const int &iRow, const int &iCol);
	void inverseComplex(double * &data,const int &iRow, const int &iCol);
	inline void rotate90Complex(double * & data,int &iRow,int &iCol);

	int m_iM; //Hauteur
	int m_iN; //Largeur
	long m_iL;

	int iImaginaireNull;
	int iAllocationType;

	double * X;
};

#endif // !defined(AFX_MATRIXDOUBLECOMPLEX_H__510B67DD_6499_4C8A_ADA0_9E97E506ABFA__INCLUDED_)
