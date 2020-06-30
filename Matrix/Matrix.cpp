// Matrix.cpp: implementation of the CMatrix class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Matrix.h"
#include <string.h>
#include <stdio.h>


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

inline void CMatrix::deplacement(double * & data,const int &iPosDepart,const int &iPosFin)
{
	if(iPosDepart > iPosFin)
	{
		for(int i = iPosDepart;i > iPosFin;i--)
			SWAP(data[i],data[i-1]);
	}
	else
	{
		for(int i = iPosFin;i<iPosDepart;i++)
			SWAP(data[i],data[i+1]);
	}
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

void CMatrix::inverse(double * & data,const int &iRow, const int &iCol)
{
	int iNbOccurence = iRow >> 1;
	int iValueFin = (iRow-1)*iCol;
	for(int i = 0;i < iCol;i++)
	{
		int iPosDebut = i;
		int iPosFin = iValueFin + i;

		for(int j = 0;j < iNbOccurence;j++)
		{
			SWAP(data[iPosDebut],data[iPosFin]);
			
			iPosFin -= iCol;
			iPosDebut += iCol;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

void CMatrix::LocalPrime(const int &iRow, const int &iCol)
{
	if(iRow == iCol)
	{
		int iPos1;
		int iPos2;
		for(int k = 0;k < iRow-1;k++)
		{
			for(int i = 0;i < iRow - 1 - k;i++)
			{
				iPos1 = i * iCol + k;
				iPos2 = iPos1 + iCol - 1;
				for(int j = k+1;j < iCol;j++)
				{
					iPos1++;
					iPos2++;
					SWAP(X[iPos1],X[iPos2]);
				}
			}

			//show(data,iRow,iCol);
		}
	}
	else
	{

		int iDeplacement = 0;
		int iPos2 = 0;
		for(int k = 0;k < iCol-1;k++)
		{
			if(iDeplacement == 0)
			{
				int iPos = 0;
				for(int i = 1;i < iRow;i++)
				{
					iPos += iCol;
					iPos2++;

					deplacement(X,iPos,iPos2);
				}
			}
			else
			{
				int iPos3 = 0;

				for(int i = 1;i < iRow;i++)
				{
					iPos3 += iCol;
					int iPos = iPos3 + k * iDeplacement;
					iPos2++;

					deplacement(X,iPos,iPos2);

					iDeplacement--;

				}
			}

			iDeplacement += iRow-1;iPos2++;
		}
	}

}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

void CMatrix::prime()
{
	if(X != NULL)
	{
		/*
		LocalPrime(m_iM,m_iN);
		SWAP(m_iM,m_iN);
		*/
		
		int iTemp = m_iN;
		m_iN = m_iM;
		m_iM = iTemp;

		CMatrix m_Temp = *this;

		double * m_dComplexPt = m_Temp.GetDataPt();

		for(int j = 0;j < m_iM;j++)
		{
			for(int i = 0;i < m_iN;i++)
			{
				X[j * m_iN + i] = m_dComplexPt[i * m_iN + j];
			}
			
		}
	}
}

/////////////////////////////////////////////////////////////////////////
//Fonction de rotation d'une matrice
/////////////////////////////////////////////////////////////////////////

void CMatrix::rotate90(double * &data,int &iRow, int &iCol)
{
	prime();
	inverse(data,iRow,iCol);
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

void CMatrix::AllocationMemoire()
{
	AllocationMemoire(m_iM * m_iN);
}


void CMatrix::AllocationMemoire(const int &M, const int &N)
{
	m_iM = M;
	m_iN = N;
	m_iL = M * N;
	AllocationMemoire(m_iL);
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

void CMatrix::AllocationMemoire(const long &iSize)
{
	if(X != NULL)
		DesAllocationMemoire();

	iAllocationType = 0;
	X = new double[iSize];

	if(X == NULL)
	{
		//Probléme d'allocation mémoire passage par un heap
		iAllocationType = 1;
		X = (double *) HeapAlloc (GetProcessHeap (),
				HEAP_ZERO_MEMORY, iSize * sizeof(double)) ;
	}
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

void CMatrix::DesAllocationMemoire()
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

		X = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

CMatrix CMatrix::operator * (CMatrix& b)
{
	CMatrix m_newMatrix;
	int i = 0,j = 0;

	m_newMatrix.zeros(m_iM,b.m_iN);

	if(m_newMatrix.X != NULL)
	{
		if(b.m_iM == m_iN && b.m_iN == m_iM)
		{

			//#pragma omp parallel private(i,j)
			//{	
				for(i = 0;i < m_iM;i++)
				{
					//#pragma omp for schedule(dynamic,1) nowait
					for(j = 0;j < b.m_iN;j++)
					{
						
						double sum1 = 0.0, sum2 = 0.0, sum3 = 0.0, sum4 = 0.0;

						for(int k = 0;k < m_iN;k+=4)
						{
							sum1 += GetValuePos(i,k) * b.GetValuePos(k,j);//X[i * N + k] * b.X[k * b.N + j];
							sum2 += GetValuePos(i,k+1) * b.GetValuePos(k+1,j);//X[i * N + k+1] * b.X[(k + 1) * b.N + j];
							sum3 += GetValuePos(i,k+2) * b.GetValuePos(k+2,j);//X[i * N + k+2] * b.X[(k + 2) * b.N + j];
							sum4 += GetValuePos(i,k+3) * b.GetValuePos(k+3,j);//X[i * N + k+3] * b.X[(k + 3) * b.N + j];
						}

						m_newMatrix.X[i * b.m_iN + j] = sum1 + sum2 + sum3 + sum4;
					}
				}
			//}
		}
	}

	return m_newMatrix;
}

const double CMatrix::GetValuePos(const int &y, const int &x) 
{
	if(y < m_iM && x < m_iN && y >= 0 && x >= 0)
		return X[y * m_iN + x];
	else
		return 0.0;
}

void CMatrix::SetValuePos(const int &y, const int &x, const double &value)
{
	if(y < m_iM && x < m_iN && y >= 0 && x >= 0)
	{
		X[y * m_iN + x] = value;
	}
}



//Uniquement valable pour les matrix carrés

CMatrix CMatrix::GetMatrixInverse()
{

	CMatrix m_newMatrix(m_iM,m_iN);

	if(m_iM == m_iN)
	{
		
		CMatrix m_Temp = *this;
		double * m_dCol = new double[m_iM];
		double * m_dZeroCol = new double[m_iM];
		int * indx = new int[m_iM];
		double d;
		int i,j;

		ludcmp(m_Temp,indx,d);

		for(i = 0;i < m_iN;i++)
			m_dZeroCol[i] = 0.0;


		for(j = 0;j < m_iM;j++)
		{
			memcpy(m_dCol,m_dZeroCol,m_iM * sizeof(double));

			m_dCol[j] = 1.0f;

			lubksb(m_Temp,indx,m_dCol);

			for(i = 0;i < m_iN;i++)
				m_newMatrix.SetValuePos(i,j,m_dCol[i]);
			
		}

		delete[] m_dZeroCol;
		delete[] m_dCol;
		delete[] indx;
	}
	else
	{
		m_newMatrix.zeros(m_iM,m_iN);
	}

	return m_newMatrix;
}


//Uniquement valable pour les matrix carrés
void CMatrix::SetMatrixInverse()
{
	if(m_iM == m_iN)
	{
		double * m_dCol = new double[m_iM];
		double * m_dZeroCol = new double[m_iM];
		int * indx = new int[m_iM];
		double d;
		int i,j;

		ludcmp(*this,indx,d);

		for(i = 0;i < m_iN;i++)
			m_dZeroCol[i] = 0.0;


		for(j = 0;j < m_iM;j++)
		{
			memcpy(m_dCol,m_dZeroCol,m_iM * sizeof(double));

			m_dCol[j] = 1.0f;

			lubksb(*this,indx,m_dCol);

			for(i = 0;i < m_iN;i++)
				SetValuePos(i,j,m_dCol[i]);
			
		}

		delete[] m_dZeroCol;
		delete[] m_dCol;
		delete[] indx;
	}
	else
	{
		zeros(m_iM,m_iN);
	}
}

CMatrix CMatrix::ArrayMulti (const CMatrix& b)
{
	CMatrix m_newMatrix(m_iM,b.m_iN);
	int i = 0, j = 0;

	if(m_newMatrix.X != NULL)
	{
		if(b.m_iM == m_iN && b.m_iN == m_iM)
		{
			for(i = 0;i < m_iM;i++)
			{
				for(j = 0;j < m_iN;j++)
				{
					int iPos = i * m_iN + j;
					m_newMatrix.X[iPos] = X[iPos] * b.X[iPos];
				}
			}
		}
	}

	return m_newMatrix;
}


void CMatrix::ArrayLocalMulti(const CMatrix& b)
{
	int i = 0, j = 0;

	if(X != NULL)
	{
		if(b.m_iM == m_iN && b.m_iN == m_iM)
		{
			for(j = 0;j < m_iL;j++)
			{
				X[j] *= b.X[j];
			}
		}
	}
}

void CMatrix::ArrayLocalAdd(const CMatrix& b)
{
	int i = 0, j = 0;

	if(X != NULL)
	{
		if(b.m_iM == m_iN && b.m_iN == m_iM)
		{
			for(j = 0;j < m_iL;j++)
			{
				X[j] += b.X[j];
			}
		}
	}
}

void CMatrix::ArrayLocalSqrt()
{
	int i = 0, j = 0;

	if(X != NULL)
	{
		for(j = 0;j < m_iL;j++)
		{
			X[j] = sqrt(X[j]);
		}
	}
}

CMatrix CMatrix::ArraySqrt(const CMatrix& b)
{
	CMatrix m_newMatrix(m_iM,b.m_iN);
	int i = 0, j = 0;

	if(m_newMatrix.X != NULL)
	{
		if(b.m_iM == m_iN && b.m_iN == m_iM)
		{
			for(i = 0;i < m_iM;i++)
			{
				for(j = 0;j < m_iN;j++)
				{
					int iPos = i * m_iN + j;
					m_newMatrix.X[iPos] = sqrt(b.X[iPos]);
				}
			}
		}
	}

	return m_newMatrix;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

CMatrix CMatrix::ArrayDivi(const CMatrix& b)
{
	CMatrix m_newMatrix(m_iM,b.m_iN);
	int i = 0, j = 0;

	if(m_newMatrix.X != NULL)
	{
		if(b.m_iM == m_iN && b.m_iN == m_iM)
		{

			for(i = 0;i < m_iM;i++)
			{

				for(j = 0;j < m_iN;j++)
				{
					int iPos = i * m_iN + j;
					m_newMatrix.X[iPos] = X[iPos] / b.X[iPos];
				}
			}
		}
	}

	return m_newMatrix;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void CMatrix::ArrayLocalDivi(const double& b)
{
	int i = 0, j = 0;

	if(X != NULL)
	{
		for(i = 0;i < m_iL;i++)
		{
			X[i] /= b;
		}
	}
}

CMatrix::CMatrix(const CMatrix & Source)
{
	m_iL = Source.m_iL;
	m_iM = Source.m_iM;
	m_iN = Source.m_iN;
	AllocationMemoire();
	memcpy(X,Source.X,m_iL * sizeof(double));

}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

CMatrix::CMatrix(const int &M, const int &N)
{
	
	this->m_iM = M;
	this->m_iN = N;
	m_iL = M * N;
	AllocationMemoire();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

CMatrix&  CMatrix::operator = (const CMatrix& a)
{
	m_iN = a.m_iN;
	m_iM = a.m_iM;
	m_iL = m_iM * m_iN;

	DesAllocationMemoire();

	AllocationMemoire();

	memcpy(X,a.X,m_iL * sizeof(double));

	return *this;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

int CMatrix::Prod()
{
	return m_iL;
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

double CMatrix::Value()
{
	int i = 0;
	double m_dValue = 0.0;
	for(i = 0;i < m_iL;i++)
	{
		m_dValue += X[i];
	}

	return m_dValue;
};



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void CMatrix::fftshift()
{
	CMatrix m_Local(m_iM,m_iN);

	int x = m_iN;
	int y = m_iM;
	int xx = (int)ceil(x/2.0f);
	int yy = (int)ceil(y/2.0f);

	m_Local.zeros(m_iM,m_iN);

	double * data = m_Local.GetDataPt();

	int i = 0;

	for(i = 0;i < yy+1;i++)
	{
		for(int j = 0;j < xx+1;j++)
		{
			data[(yy - i) * m_iN + (xx - j)] = X[i * m_iN + j];
		}
	}

	for(i = yy+1;i < y;i++)
	{
		for(int j = 0;j < xx+1;j++)
		{
			data[(y - i + yy) * m_iN + (xx - j)] = X[i * m_iN + j];
		}
	}

	for(i = 0;i < yy+1;i++)
	{
		for(int j = xx+1;j < x;j++)
		{
			data[(yy - i) * m_iN + (x - j + xx)] = X[i * m_iN + j];
		}
	}

	for(i = yy+1;i < y;i++)
	{
		for(int j = xx+1;j < x;j++)
		{
			data[(y - i + yy) * m_iN + (x - j + xx)] = X[i * m_iN + j];
		}
	}

	memcpy(X,data,m_iL*sizeof(double));
}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

CMatrix CMatrix::operator * (const double& b)
{
	CMatrix m_newMatrix(m_iM,m_iN);

	if(m_newMatrix.X != NULL)
	{
		for(int i = 0;i < m_iM;i++)
		{
			for(int j = 0;j < m_iN;j++)
			{
				m_newMatrix.X[i * m_iN + j] = X[i * m_iN + j] * b;
			}
		}
	}

	return m_newMatrix;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

CMatrix CMatrix::operator / (const double& b)
{
	CMatrix m_newMatrix(m_iM,m_iN);

	if(m_newMatrix.X != NULL)
	{
		for(int i = 0;i < m_iM;i++)
		{
			for(int j = 0;j < m_iN;j++)
			{
				m_newMatrix.X[i * m_iN + j] = X[i * m_iN + j] / b;
			}
		}
	}

	return m_newMatrix;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

CMatrix CMatrix::operator / (const CMatrix& b)
{
	CMatrix m_newMatrix = *this;
	CMatrix m_newMatrixDiv = b;

	m_newMatrix = m_newMatrix * m_newMatrixDiv.GetMatrixInverse();

	return m_newMatrix;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

CMatrix CMatrix::operator + (const CMatrix& b)
{
	CMatrix m_newMatrix(m_iM,m_iN);

	if(m_newMatrix.X != NULL)
	{
		for(int i = 0;i < m_iM;i++)
		{
			for(int j = 0;j < m_iN;j++)
			{
				int iPos = i * m_iN + j;
				m_newMatrix.X[iPos] = X[iPos] + b.X[iPos];
			}
		}
	}

	return m_newMatrix;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void CMatrix::Div(const double& b)
{
	if(X != NULL)
	{
		for(int i = 0;i < m_iM;i++)
		{
			for(int j = 0;j < m_iN;j++)
			{
				X[i * m_iN + j] /= b;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void CMatrix::rot90(const int &iNb)
{
	if(X != NULL)
	{
		double * m_dTemp = new double[m_iL];

		int iNbValue = iNb%4;

		for(int k = 0;k < iNbValue;k++)
		{
			
			for(int i = 0;i < m_iM;i++)
			{
				for(int j = 0;j < m_iN;j++)
				{
					m_dTemp[(m_iN - 1 - j) * m_iM + i] = X[i * m_iN + j];
				}
			}

			memcpy(X,m_dTemp,m_iL* sizeof(double));
			int iNTemp = m_iN;
			m_iN = m_iM;
			m_iM = iNTemp;

			//rotate90(X,m_iM,m_iN);


		}


		delete[] m_dTemp;
	}
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

CMatrix::CMatrix()
{
	X = NULL;
	m_iM = 0;
	m_iN = 0;
	m_iL = 0;

	iAllocationType = 0;
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

CMatrix::~CMatrix()
{
	DesAllocationMemoire();
}


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

void CMatrix::zeros(const int &M, const int &N)
{
	if(m_iM != M || m_iN != N)
	{
		this->m_iM = M;
		this->m_iN = N;
		m_iL = M * N;

		if(X != NULL)
		{
			DesAllocationMemoire();
		}

		AllocationMemoire();
	}

	memset(X,0,m_iL);

	/*
	for(int i = 0;i < m_iL;i++)
	{
		X[i] = 0.0f;
	}*/
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

void CMatrix::ones(const int &M, const int &N)
{
	this->m_iM = M;
	this->m_iN = N;
	m_iL = M * N;

	AllocationMemoire();
	
	for(int i = 0;i < m_iL;i++)
	{
		X[i] = 1.0f;
	}
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

void CMatrix::ones(const int &iColDepart, const int &iLigneDepart,const int &iSize)
{
	if(X != NULL)
	{
		int iColFin = iColDepart + iSize; 
		int iLigneFin = iLigneDepart + iSize; 

		if(iColFin < m_iM && iLigneFin < m_iN)
		{
			for(int i = iColDepart;i < iColFin;i++)
			{
				for(int j = iLigneDepart;j < iLigneFin;j++)
				{
					X[i * m_iN + j] = 1.0f;
				}
			}
		}
	}
}	

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

void CMatrix::show()
{
	if(X != NULL)
	{
		for(int i = 0;i < m_iM;i++)
		{
			printf("|");
			for(int j = 0;j < m_iN;j++)
			{
				printf(" %.3f ",X[i * m_iN + j]);
			}
			printf("|\n");
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

void CMatrix::moins()
{
	if(X != NULL)
	{
		int iTemp = m_iN;
		m_iN = m_iM;
		m_iM = iTemp;

		CMatrix m_Temp = *this;

		double * m_dComplexPt = m_Temp.GetDataPt();

		for(int i = 0;i < m_iM;i++)
		{
			for(int j = 0;j < m_iN;j++)
			{
				X[i * m_iN + j] = m_dComplexPt[i * m_iN + (m_iN - 1 - j)];
			}
			
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

void CMatrix::freqspace(const int &iTaille)
{
	if(iTaille%2 == 0)
		return;

	m_iM = 1;
	m_iN = iTaille;

	AllocationMemoire();

	int iMilieu = (iTaille - 1) >> 1;

	for(int i = 0;i < iMilieu;i++)
	{
		X[m_iN - i - 1] = (float)(iTaille - 1 - (i << 1)) / (float)iTaille;
		X[i] = -X[m_iN - i - 1];
	}

	X[iMilieu] = 0;
}

void CMatrix::SetMatrixValue(const int &iHauteurStart, const int &iLargeurStart, CMatrix m_Matrix)
{
	double * data = m_Matrix.GetDataPt();
	int iHauteurFin = iHauteurStart + m_Matrix.GetTheMValue();
	int iLargeurFin = iLargeurStart + m_Matrix.GetNValue();

	int k = 0;

	for(int i = iHauteurStart;i < iHauteurFin;i++)
	{
		for(int j = iLargeurStart;j < iLargeurFin;j++,k++)
		{
			X[i * m_iN + j] = data[k];
		}
	}
}

void CMatrix::ludcmp(CMatrix &m_MatrixSource, int * &indx, double &d)
{
	const double tiny = 1.0e-20;
	int i,imax,j,k;
	double big, dum, sum ,temp;
	
	int n = m_MatrixSource.m_iN;

	double * vv = new double[n];

	d = 1.0f;

	for(i = 0;i < n;i++)
	{
		big = 0.0f;
		for(j = 0;j < n;j++)
		{
			temp = fabs(m_MatrixSource.GetValuePos(i,j));
			if(temp > big)
				big = temp;
		}

		if(big == 0.0)
			goto END;

		vv[i] = 1.0f/big;
	}

	for(j = 0;j < n;j++)
	{
		for(i=0;i<j;i++)
		{
			sum = m_MatrixSource.GetValuePos(i,j);
			for(k = 0;k < i;k++)
				sum -= m_MatrixSource.GetValuePos(i,k) * m_MatrixSource.GetValuePos(k,j);

			m_MatrixSource.SetValuePos(i,j,sum);

		}

		big = 0.0f;

		for(i = j;i < n;i++)
		{
			sum = m_MatrixSource.GetValuePos(i,j);
			for(k = 0;k < j;k++)
				sum -= m_MatrixSource.GetValuePos(i,k) * m_MatrixSource.GetValuePos(k,j);

			m_MatrixSource.SetValuePos(i,j,sum);

			dum = vv[i] * fabs(sum);
			if(dum >= big)
			{
				big = dum;
				imax = i;
			}
		}

		if(j != imax)
		{
			for(k = 0;k < n;k++)
			{
				dum = m_MatrixSource.GetValuePos(imax,k);
				m_MatrixSource.SetValuePos(imax,k,m_MatrixSource.GetValuePos(j,k));
				m_MatrixSource.SetValuePos(j,k,dum);
			}

			d = -d;
			vv[imax] = vv[j];
		}

		indx[j] = imax;

		if(m_MatrixSource.GetValuePos(j,j) == 0.0)
			m_MatrixSource.SetValuePos(j,j,TINY);

		if(j != n - 1)
		{
			dum = 1.0 / m_MatrixSource.GetValuePos(j,j);
			for(i = j + 1;i < n;i++)
				m_MatrixSource.SetValuePos(i,j,m_MatrixSource.GetValuePos(i,j) * dum);

		}

	}

END:
	delete[] vv;
}

void CMatrix::lubksb(CMatrix &m_MatrixSource, int * indx, double * b)
{
	int i,ii=0,ip,j;
	double sum;

	int n = m_MatrixSource.m_iN;

	for(i = 0;i < n;i++)
	{
		ip = indx[i];
		sum = b[ip];
		b[ip] = b[i];
		if(ii != 0)
		{
			for(j = ii - 1;j < i;j++)
				sum -= m_MatrixSource.GetValuePos(i,j) * b[j];
		}
		else if(sum != 0.0)
			ii = i + 1;

		b[i] = sum;
	}

	for(i = n - 1; i >=0;i--)
	{
		sum = b[i];
		for(j = i+1;j < n;j++)
			sum -= m_MatrixSource.GetValuePos(i,j) * b[j];
		b[i] = sum / m_MatrixSource.GetValuePos(i,i);
	}
}