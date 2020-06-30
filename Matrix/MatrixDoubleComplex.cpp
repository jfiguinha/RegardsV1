// MatrixDoubleComplex.cpp: implementation of the CMatrixDoubleComplex class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MatrixDoubleComplex.h"

/////////////////////////////////////////////////////////////////////////
//Fonction déplacant les données dans un tableau de complexe
/////////////////////////////////////////////////////////////////////////

inline void CMatrixDoubleComplex::deplacementComplex(double * & data,const int &iPosDepart,const int &iPosFin)
{
	if(iPosDepart > iPosFin)
	{
		for(int i = iPosDepart;i > iPosFin;i-=2)
		{
			SWAP(data[i],data[i-2]);
			SWAP(data[i+1],data[i-1]);
		}
	}
	else
	{
		for(int i = iPosFin;i<iPosDepart;i+=2)
		{
			SWAP(data[i],data[i+2]);
			SWAP(data[i+1],data[i+3]);
		}
	}
}

/////////////////////////////////////////////////////////////////////////
//Fonction calculant la conjuguée d'une matrice dans les nombres complexes
/////////////////////////////////////////////////////////////////////////

void CMatrixDoubleComplex::primeComplex(double * & data,const int &iRow, const int &iCol)
{
	int iColDeux = iCol << 1;
	if(iRow == iCol)
	{
		int iPos1;
		int iPos2;
		for(int k = 0;k < iRow-1;k++)
		{
			for(int i = 0;i < iRow - 1 - k;i++)
			{
				iPos1 = (i * iColDeux) + (k << 1);
				iPos2 = iPos1 + iColDeux - 2;
				for(int j = (k << 1) +2;j < iColDeux;j+=2)
				{
					iPos1+=2;
					iPos2+=2;
					SWAP(data[iPos1],data[iPos2]);
					SWAP(data[iPos1+1],data[iPos2+1]);
				}
			}
		}
	}
	else
	{

		int iDeplacement = 0;
		int iPos2 = 0;

		int iPos = 0;
		for(int i = 1;i < iRow;i++)
		{
			iPos += iColDeux;
			iPos2+=2;
			deplacementComplex(data,iPos,iPos2);
		}

		iDeplacement += (iRow << 1) - 2;iPos2+=2;

		for(int k = 1;k < iCol-1;k++)
		{
			int iPos3 = 0;

			for(int i = 1;i < iRow;i++)
			{
				iPos3 += iColDeux;
				int iPos = iPos3 + k * iDeplacement;
				iPos2+=2;
				deplacementComplex(data,iPos,iPos2);
				iDeplacement-=2;
			}

			iDeplacement += (iRow << 1) - 2;iPos2+=2;
		}
	}

}

/////////////////////////////////////////////////////////////////////////
//Inversion par colonne dans les nombres complexes
/////////////////////////////////////////////////////////////////////////

void CMatrixDoubleComplex::inverseComplex(double * & data,const int &iRow, const int &iCol)
{
	int iNbOccurenceComplex = (iRow >> 1) << 1;
	int iValueFin = (iRow-1)*iCol;
	for(int i = 0;i < iCol;i++)
	{
		int iPosDebut = i << 1;
		int iPosFin = (iValueFin << 1) + iPosDebut;

		for(int j = 0;j < iNbOccurenceComplex;j+=2)
		{
			SWAP(data[iPosDebut],data[iPosFin]);
			SWAP(data[iPosDebut+1],data[iPosFin+1]);
			
			iPosFin -= iCol << 1;
			iPosDebut += iCol << 1;
		}
	}
}


/////////////////////////////////////////////////////////////////////////
//Fonction de rotation d'une matrice dans les nombres complexes
/////////////////////////////////////////////////////////////////////////

inline void CMatrixDoubleComplex::rotate90Complex(double * & data,int &iRow,int &iCol)
{
	primeComplex(data,iRow,iCol);
	SWAP(iRow,iCol);
	inverseComplex(data,iRow,iCol);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMatrixDoubleComplex::CMatrixDoubleComplex()
{
	m_iM = 0; //Hauteur
	m_iN = 0; //Largeur
	m_iL = 0;

	iImaginaireNull = 0;
	iAllocationType = 0;

	X = NULL;

}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CMatrixDoubleComplex::~CMatrixDoubleComplex()
{
	DesAllocationMemoire();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CMatrixDoubleComplex::AllocationMemoire()
{
	if(X != NULL)
		DesAllocationMemoire();

	iAllocationType = 0;
	X = new double[m_iL * 2];

	if(X == NULL)
	{
		//Probléme d'allocation mémoire passage par un heap
		iAllocationType = 1;
		X = (double *) HeapAlloc (GetProcessHeap (),
				HEAP_ZERO_MEMORY, m_iL * 2 * sizeof(double)) ;
	}
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CMatrixDoubleComplex::AllocationMemoire(const int &M, const int &N)
{
	if(X != NULL)
		DesAllocationMemoire();

	m_iM = M;
	m_iN = N;
	m_iL = M * N;

	AllocationMemoire();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CMatrixDoubleComplex::DesAllocationMemoire()
{
	if(X != NULL)
	{
		switch(iAllocationType)
		{
			case 0:
				delete[] X;
				break;

			case 1:
				HeapFree (GetProcessHeap(), 0, X) ;
				break;
		}
	}
	X = NULL;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CMatrixDoubleComplex::CMatrixDoubleComplex(const int &M, const int &N)
{
	m_iM = 0; //Hauteur
	m_iN = 0; //Largeur
	m_iL = 0;

	iImaginaireNull = 0;
	iAllocationType = 0;

	X = NULL;


	m_iM = M;
	m_iN = N;
	m_iL =M * N;
	AllocationMemoire();
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CMatrixDoubleComplex::CMatrixDoubleComplex(const CMatrix &Source)
{
	m_iM = 0; //Hauteur
	m_iN = 0; //Largeur
	m_iL = 0;

	iImaginaireNull = 0;
	iAllocationType = 0;

	X = NULL;


	m_iM = Source.GetTheMValue();
	m_iN = Source.GetNValue();
	m_iL = m_iM * m_iN;
	AllocationMemoire();

	int L = m_iL << 1;

	//Affectation des données
	double * m_dDataMatrix = Source.GetDataPt();
	
	for(int i = 0,j = 0;i < L;i+=2,j++)
	{
		X[i] = m_dDataMatrix[j];
		X[i+1] = 0.0;
	}
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CMatrixDoubleComplex::CMatrixDoubleComplex(const CMatrixDoubleComplex & Source)
{
	m_iM = 0; //Hauteur
	m_iN = 0; //Largeur
	m_iL = 0;

	iImaginaireNull = 0;
	iAllocationType = 0;

	X = NULL;

	m_iL = Source.m_iL;
	m_iM = Source.m_iM;
	m_iN = Source.m_iN;
	AllocationMemoire();
	memcpy(X,Source.X,m_iL * 2 * sizeof(double));

}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

CMatrixDoubleComplex& CMatrixDoubleComplex::operator = (const CMatrixDoubleComplex& a)
{
	m_iN = a.m_iN;
	m_iM = a.m_iM;
	m_iL = m_iM * m_iN;

	DesAllocationMemoire();

	AllocationMemoire();

	memcpy(X,a.X,m_iL * 2 * sizeof(double));

	return *this;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CMatrixDoubleComplex::ArrayLocalMulti(const CMatrixDoubleComplex& b)
{
	int i = 0, j = 0;

	if(X != NULL)
	{
		for(i = 0;i < m_iL << 1;i+=2)
		{
			double m_dTemp = X[i];
			X[i] = m_dTemp * b.X[i] - X[i+1] * b.X[i+1];
			X[i+1] = X[i+1] * b.X[i] + b.X[i+1] * m_dTemp;
		}
	}
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CMatrixDoubleComplex::ArrayLocalMulti(CMatrix& b)
{
	int i = 0, j = 0;
	double * m_dData = b.GetDataPt();

	if(X != NULL)
	{
		for(i = 0;i < m_iL << 1;i+=2,j++)
		{
			X[i] *= m_dData[j];
			X[i+1] *= m_dData[j];
		}
	}
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CMatrixDoubleComplex::ArrayDivi (const double& b)
{
	if(X != NULL)
	{
		for(int i = 0;i < m_iL << 1;i+=2)
		{
			X[i] /= b;
			X[i+1] /= b;
		}
	}
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CMatrixDoubleComplex::ArrayAdd (const double& b)
{
	if(X != NULL)
	{
		for(int i = 0;i < m_iL << 1;i+=2)
		{
			X[i] += b;
		}
	}
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CMatrixDoubleComplex::ArrayDivi(CMatrixDoubleComplex& b)
{
	if(X != NULL)
	{
		for(int i = 0;i < m_iL << 1;i+=2)
		{
			double den = b.X[i] * b.X[i] + b.X[i+1] * b.X[i+1];
		
			double ac = X[i]* b.X[i];
			double bd = X[i+1]* b.X[i+1];
			
			// the order here is important since z might be the same as x or y
			X[i+1] = ((X[i] + X[i+1])*(b.X[i] - b.X[i+1]) - ac + bd)/den;
			X[i] = (ac + bd)/den;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void CMatrixDoubleComplex::rot90(const int &iNb)
{
	if(X != NULL)
	{
		double * m_dTemp = new double[m_iL * 2];

		int iNbValue = iNb%4;

		for(int k = 0;k < iNbValue;k++)
		{
			
			for(int i = 0;i < m_iM;i++)
			{
				for(int j = 0;j < m_iN * 2;j++)
				{
					m_dTemp[(m_iN - 1 - j) * m_iM + i] = X[i * m_iN + j];
				}
			}

			memcpy(X,m_dTemp,m_iL * 2 * sizeof(double));
			int iNTemp = m_iN;
			m_iN = m_iM;
			m_iM = iNTemp;

			//rotate90Complex(X,m_iM,m_iN);

		}


		delete[] m_dTemp;
	}
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CMatrixDoubleComplex::GetRealMatrix(CMatrix &m_Matrix)
{
	if(m_Matrix.GetLValue() != m_iL)
		m_Matrix.AllocationMemoire(m_iM,m_iN);

	double * m_dData = m_Matrix.GetDataPt();
	if(X != NULL)
	{
		for(int i = 0, k = 0;i < m_iL << 1;i+=2,k++)
		{
			m_dData[k] = X[i];
		}
	}

}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CMatrix CMatrixDoubleComplex::GetRealMatrix()
{
	CMatrix m_Matrix;
	m_Matrix.AllocationMemoire(m_iM,m_iN);

	double * m_dData = m_Matrix.GetDataPt();
	if(X != NULL)
	{
		for(int i = 0, k = 0;i < m_iL << 1;i+=2,k++)
		{
			m_dData[k] = X[i];
		}
	}
	return m_Matrix;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CMatrixDoubleComplex::show()
{
	if(X != NULL)
	{
		//for(int i = 0;i < m_iM;i++)
		for(int i = 0;i < m_iM;i++)
		{
			printf("|");
			int iPos = i * m_iN;
			for(int j = 0;j < m_iN << 1;j+=2)
			{
				int iPos2 = (iPos << 1) + j;
				printf(" %.3f + %.3f I",X[iPos2], X[iPos2 + 1]);
			}
			printf("|\n");
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void CMatrixDoubleComplex::Prime()
{
	if(X != NULL)
	{
		CMatrixDoubleComplex m_Temp = *this;

		double * m_dComplexPt = m_Temp.GetDataPt();

		for(int j = 0;j < m_iM;j++)
		{
			int iPos = j * (m_iN << 1);

			for(int i = 0;i < m_iN << 1;i+=2)
			{
				X[i * (m_iM) + (j << 1)] = m_dComplexPt[iPos + i];
				X[i * (m_iM) + (j << 1) + 1] = m_dComplexPt[iPos + i + 1];
			}
		}

		int iTemp = m_iN;
		m_iN = m_iM;
		m_iM = iTemp;


		//primeComplex(X,m_iM,m_iN);
		//SWAP(m_iM,m_iN);

	}
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CMatrixDoubleComplex CMatrixDoubleComplex::operator / (const double& b)
{
	CMatrixDoubleComplex m_newMatrix(m_iM,m_iN);

	if(m_newMatrix.X != NULL)
	{
		for(int i = 0;i < m_iL << 1;i+=2)
		{
			m_newMatrix.X[i] = X[i] / b;
			m_newMatrix.X[i+1] = X[i+1] / b;
		}
	}

	return m_newMatrix;
}



//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CMatrixDoubleComplex::inverseAll()
{
	if(X != NULL)
	{
		for(int i = 0;i < m_iM;i++)
		{
			int iLocal = i * m_iN * 2;
			
			for(int j = i*2;j < m_iN << 1;j+=2)
			{
				int yPos = j * m_iN + i*2;
				int yPos2 = iLocal + j;

				double m_dTemp = X[yPos2];
				double m_dTemp1 = X[yPos2 + 1];
				X[yPos2] = X[yPos];
				X[yPos2 + 1] = X[yPos + 1];
				X[yPos] = m_dTemp;
				X[yPos + 1] = m_dTemp1;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


void CMatrixDoubleComplex::GetConjMatrix()
{
	if(X != NULL)
	{
		for(int i = 0;i < m_iL*2;i+=2)
		{
			X[i+1] = -X[i+1];
		}
	}
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CMatrixDoubleComplex::ones(const int &M, const int &N)
{
	this->m_iM = M;
	this->m_iN = N;
	m_iL = M * N;

	AllocationMemoire();
	
	for(int i = 0;i < m_iL << 1;i+=2)
	{
		X[i] = 1.0f;
		X[i+1] = 1.0f;
	}
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CMatrixDoubleComplex::zeros(const int &M, const int &N)
{
	this->m_iM = M;
	this->m_iN = N;
	m_iL = M * N;

	AllocationMemoire();
	
	memset(X,0,m_iL);

}