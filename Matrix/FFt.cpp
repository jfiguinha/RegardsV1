// FFt.cpp: implementation of the CFFt class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Matrix.h"
#include "MatrixDoubleComplex.h"
#include "FFt.h"
#include "fftw3.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFFt::CFFt()
{

}

CFFt::~CFFt()
{

}



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

CMatrix CFFt::conv2fft(const CMatrix &m_MatrixSource,const CMatrix &m_MatrixFilter)
{
	int z1x = m_MatrixSource.GetNValue();
	int z1y = m_MatrixSource.GetTheMValue();
	int z2x = m_MatrixFilter.GetNValue();
	int z2y = m_MatrixFilter.GetTheMValue();

	int col = z1x+z2x-1;
	int row = z1y+z2y-1;

	CMatrixDoubleComplex a,b;
	CMatrix RealA(row,col), RealB(row,col);

	RealB.SetMatrixValue(0,0,m_MatrixFilter);
	RealA.SetMatrixValue(0,0,m_MatrixSource);

	b = fft2(RealA);

	a = fft2(RealB);

	b.ArrayLocalMulti(a);

	ifft2(b);

	return b.GetRealMatrix();
}

/*
  Calculates the Fourier Transform of a set of 2n real-valued data points
  stored in array data[1..2n] replacing it by the positive frequency half
  of it's complex Fourier Transform if isign is set to +1.
  The real-valued first and last components of the complex transform
  are returned as elements data[1] and data[2] respectively.
  n must be a power of 2.
  Calculates the inverse transform of a complex data array
  which is the transform of real data if isign is set to -1.
  But the inverse transform array should be multiplied by 1/n.
*/
void CFFt::fft2(CMatrixDoubleComplex &m_MatrixComplex)
{
#if !(defined(__ICC) || defined(__ICL) || defined(__ECC) || defined(__ECL))

	fft(m_MatrixComplex);
	m_MatrixComplex.Prime();
	fft(m_MatrixComplex);
	m_MatrixComplex.Prime();
#else

	int row = m_MatrixComplex.GetTheMValue();
	int col = m_MatrixComplex.GetNValue();

	m_MatrixComplex.Prime();

	double * ComplexData = m_MatrixComplex.GetDataPt2();
	double * ComplexDataOut = m_MatrixComplex.GetDataPt2();

	fftw_plan p;

	p = fftw_plan_dft_2d(row,col, (fftw_complex *)ComplexData, (fftw_complex *)ComplexDataOut, 1, FFTW_ESTIMATE);

	fftw_execute(p);

	fftw_destroy_plan(p);	

	m_MatrixComplex.GetConjMatrix();

	m_MatrixComplex.Prime();

#endif
}


/*
  Calculates the Fourier Transform of a set of 2n real-valued data points
  stored in array data[1..2n] replacing it by the positive frequency half
  of it's complex Fourier Transform if isign is set to +1.
  The real-valued first and last components of the complex transform
  are returned as elements data[1] and data[2] respectively.
  n must be a power of 2.
  Calculates the inverse transform of a complex data array
  which is the transform of real data if isign is set to -1.
  But the inverse transform array should be multiplied by 1/n.
*/
CMatrixDoubleComplex CFFt::fft2(const CMatrix &m_Matrix)
{
	CMatrixDoubleComplex m_MatrixComplexIn;
	m_MatrixComplexIn = m_Matrix;
	fft2(m_MatrixComplexIn);
	return m_MatrixComplexIn;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void CFFt::four1(double * & data, const int &isign,const int &iDataSize)
{
	int n,mmax,m,j,istep,i;
	double wtemp,wr,wpr,wpi,wi,theta,tempr,tempi;

	int nn = (iDataSize) / 2;
	n = nn << 1;
	j = 1;

	for(i=1;i<n;i+=2)
	{
		if(j > i)
		{
			SWAP(data[j-1],data[i-1]);
			SWAP(data[j],data[i]);
		}
		m = nn;
		while(m >= 2 && j > m)
		{
			j -=m;
			m >>= 1;
		}
		j +=m;
	}

	mmax = 2;
	while(n > mmax)
	{
		istep = mmax << 1;
		theta	= isign*(PI2/mmax);
		wtemp = sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi = sin(theta);
		wr = 1.0f;
		wi = 0.0f;

		for(m=1;m<mmax;m+=2)
		{
			for(i=m;i<=n;i+=istep)
			{
				j=i+mmax;
				if(j >= iDataSize)
				{
					if(i >= iDataSize)
					{
						tempr=0;
						tempi=0;
					}
					else
					{
						tempr=0;
						tempi=0;
						data[i-1] += tempr;
						data[i] += tempi;
					}
				}
				else if(i >= iDataSize)
				{
					tempr=wr*data[j-1]-wi*data[j];
					tempi=wr*data[j]+wi*data[j-1];
					data[j-1] = data[i-1]-tempr;
					data[j] = data[i] - tempi;
				}
				else
				{
					tempr=wr*data[j-1]-wi*data[j];
					tempi=wr*data[j]+wi*data[j-1];
					data[j-1] = data[i-1]-tempr;
					data[j] = data[i] - tempi;
					data[i-1] += tempr;
					data[i] += tempi;
				}
			}
			wr = (wtemp = wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;

		}
		mmax=istep;

	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

CMatrixDoubleComplex CFFt::fft(const CMatrix &m_Matrix)
{
	CMatrixDoubleComplex m_MatrixComplexIn;

	m_MatrixComplexIn = m_Matrix;

	fft(m_MatrixComplexIn);

	return m_MatrixComplexIn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


void CFFt::fft(CMatrixDoubleComplex &m_Matrix)
{

	int row = m_Matrix.GetTheMValue();
	int col = m_Matrix.GetNValue();

	double width = row;
	int m_dValue = log2(row);


	#if !(defined(__ICC) || defined(__ICL) || defined(__ECC) || defined(__ECL))

		if(row != col || pow(2,m_dValue) != row)
		{
			CMatrixDoubleComplex m_MatrixComplexIn;
			
			m_MatrixComplexIn = m_Matrix;
			
			m_MatrixComplexIn.Prime();

			double * ComplexData = m_MatrixComplexIn.GetDataPt2();
			double * ComplexDataOut = m_Matrix.GetDataPt2();

			fftSimpleCooleyTukeyType2Complex(ComplexData,ComplexDataOut,row,col,1);
			
			m_Matrix.Prime();

			m_MatrixComplexIn.DesAllocationMemoire();

		}
		else
		{

			m_Matrix.Prime();

			int deuxcol = col << 1;

			double * ComplexDataOut = m_Matrix.GetDataPt2();

			int iPos = 0,iPos4 = 0;
			int nn[1];

			nn[0] = col;

			for(int i = 0;i < row;i++)
			{
				iPos4 = iPos << 1;
				double * ComplexData = ComplexDataOut + iPos4;

				four1(ComplexData,1,deuxcol);

				iPos += col;
			}

			m_Matrix.GetConjMatrix();
			
			m_Matrix.Prime();
		}

	#else

		CMatrixDoubleComplex m_MatrixComplexIn;
		
		m_MatrixComplexIn = m_Matrix;
		
		m_MatrixComplexIn.Prime();

		double * ComplexData = m_MatrixComplexIn.GetDataPt2();
		double * ComplexDataOut = m_Matrix.GetDataPt2();

		fftSimpleCooleyTukeyType2Complex(ComplexData,ComplexDataOut,row,col,1);
		
		m_Matrix.Prime();

		m_MatrixComplexIn.DesAllocationMemoire();

	#endif
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

void CFFt::ifft(CMatrixDoubleComplex &m_Matrix)
{

	int row = m_Matrix.GetTheMValue();
	int col = m_Matrix.GetNValue();

	double width = row;
	int m_dValue = log2(row);

#if !(defined(__ICC) || defined(__ICL) || defined(__ECC) || defined(__ECL))


	if(row != col || pow(2,m_dValue) != row)
	{
		CMatrixDoubleComplex m_MatrixComplexIn;
		
		m_MatrixComplexIn = m_Matrix;
		
		m_MatrixComplexIn.Prime();

		double * ComplexData = m_MatrixComplexIn.GetDataPt2();
		double * ComplexDataOut = m_Matrix.GetDataPt2();

		fftSimpleCooleyTukeyType2Complex(ComplexData,ComplexDataOut,row,col,-1);
		
		m_Matrix.ArrayDivi(col);

		m_Matrix.Prime();

		m_MatrixComplexIn.DesAllocationMemoire();

	}
	else
	{

		m_Matrix.Prime();

		m_Matrix.GetConjMatrix();

		double * ComplexDataOut = m_Matrix.GetDataPt2();

		int iPos = 0,iPos4 = 0;
		int nn[1];

		nn[0] = col;

		int deuxcol = col << 1;

		for(int i = 0;i < row;i++)
		{
			iPos4 = (iPos << 1);

			double * ComplexData = ComplexDataOut + iPos4;

			four1(ComplexData,-1,deuxcol);

			iPos += col;
		}

		m_Matrix.ArrayDivi(col);
		
		m_Matrix.Prime();
	}

#else

		CMatrixDoubleComplex m_MatrixComplexIn = m_Matrix;
		
		m_MatrixComplexIn.Prime();

		m_MatrixComplexIn.GetConjMatrix();

		double * ComplexData = m_MatrixComplexIn.GetDataPt2();
		double * ComplexDataOut = m_Matrix.GetDataPt2();

		fftSimpleCooleyTukeyType2Complex(ComplexData,ComplexDataOut,row,col,-1);

		m_Matrix.ArrayDivi(col);
		
		m_Matrix.Prime();

#endif

}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

CMatrix CFFt::dct(const CMatrix &m_Matrix)
{
	
	int M = m_Matrix.GetTheMValue();
	int N = m_Matrix.GetNValue();
	int do_trans = 0;
	int k = 0;
	CMatrixDoubleComplex H(1,M);
	CMatrixDoubleComplex H2(M,N);
	CMatrix y(M,N);

	double * dataTemp = m_Matrix.GetDataPt();
	
	y.zeros(M,N);

	double * m_dValue = y.GetDataPt();

	//Re-order the elements of the columns of x

	for(int i = 0;i < M;i+=2)
	{
		int iPos = i * N;
		for(int j = 0;j < N;j++)
		{
			m_dValue[k++] = dataTemp[iPos + j];
		}
	}

	k = M-1;

	for(int i = 1;i < M;i+=2)
	{
		int iPos = i * N;
		int iPos2 = k * N;
		for(int j = 0;j < N;j++)
		{
			m_dValue[iPos2 + j] = dataTemp[iPos + j];
		}

		k--;
	}

	

	//Compute weights to multiply DFT coefficients
	double * ptData = H.GetDataPt2();
	double * ptData2 = H2.GetDataPt2();

	k = 0;

	double m_dDiv = sqrt((double)2*M);

	for(int j = 0;j < M;j++)
	{
		double theta = j*PI/(2*M);
		ptData[k] = (2 * cos(theta)) / m_dDiv;
		ptData[k+1] = (-2 * sin(theta)) / m_dDiv;
		k+=2;
	}

	ptData[0] /= sqrt((double)2);

	for(int i = 0,k=0;i < M;i++,k+=2)
	{
		int iPos = (i * (N << 1));
		for(int j = 0;j < N << 1;j+=2)
		{
			ptData2[iPos + j] = ptData[k];
			ptData2[iPos + j + 1] = ptData[k+1];
		}
	}
	
	H = fft(y);

	H2.ArrayLocalMulti(H);

	return H2.GetRealMatrix();
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

CMatrix CFFt::idct(const CMatrix &m_Matrix)
{
	int M = m_Matrix.GetTheMValue();
	int N = m_Matrix.GetNValue();
	int k = 0;

	double * dataTemp = m_Matrix.GetDataPt();
	
	CMatrix y;
	y.zeros(M,N);
	CMatrixDoubleComplex H(1,M);
	CMatrixDoubleComplex H2(M,N);

	double * m_dValue = y.GetDataPt();
	double * ptData = H.GetDataPt2();
	double * ptData2 = H2.GetDataPt2();

	k = 0;

	int iDblM = M << 1;

	double m_dDiv = sqrt((double)iDblM);

	for(int j = 0;j < M;j++)
	{
		double theta = j*PI/(iDblM);
		ptData[k] = m_dDiv * cos(theta) ;
		ptData[k+1] = m_dDiv * sin(theta);
		k +=2;
	}

	H.Prime();

	*(ptData) /= sqrt((double)2);
	*(ptData + 1) /= sqrt((double)2);

	k = 0;

	for(int i = 0;i < M;i++,k+=2)
	{
		int iPos = i * (N << 1);

		for(int j = 0;j < N << 1;j+=2)
		{
			*(ptData2 + iPos + j) = *(ptData + k);
			*(ptData2 + iPos + j + 1) = *(ptData + k + 1);
		}
	}

	H2.ArrayLocalMulti(m_Matrix);

	ifft(H2);

	ptData = H2.GetDataPt2();

	H.zeros(M,N);

	ptData2 = H.GetDataPt2();

	int iPos = 0;

	int iFin = M >> 1;

	if(M%2 == 1)
		iFin++;

	for(int i = 0;i < iFin;i++)
	{
		for(int j = 0;j < N << 1;j+=2)
		{
			int iPos2 = iPos * (N << 1) + j;
			int iPos3 = i * (N << 1)  + j;
			ptData2[iPos2] = ptData[iPos3];
			ptData2[iPos2 + 1] = ptData[iPos3 + 1];
		}

		iPos += 2;
	}

	iPos = 1;

	for(int i = M-1;i >= iFin;i--)
	{
		for(int j = 0;j < N << 1;j+=2)
		{
			int iPos2 = iPos * (N << 1) + j;
			int iPos3 = i * (N << 1)  + j;
			ptData2[iPos2] = ptData[iPos3];
			ptData2[iPos2 + 1] = ptData[iPos3 + 1];
		}
		iPos+=2;
	}

	return H.GetRealMatrix();
}

//////////////////////////////////////////////////////////////////////
//Calcul des coefficients
//////////////////////////////////////////////////////////////////////

CMatrix CFFt::dct2(const CMatrix &m_Matrix)
{
	CMatrix m_Matrix2(m_Matrix.GetTheMValue(),m_Matrix.GetNValue());

	m_Matrix2 = dct(m_Matrix);

	m_Matrix2.prime();

	m_Matrix2 = dct(m_Matrix2);

	m_Matrix2.prime();

	return m_Matrix2;

}

/////////////////////////////////////////////////////////////////
//Inverse Discrete Cosine Transform (DCT) 2 
/////////////////////////////////////////////////////////////////

CMatrix CFFt::idct2(const CMatrix &m_Matrix)
{

	int L = m_Matrix.GetNValue() * m_Matrix.GetTheMValue();

	CMatrix m_Matrix2(m_Matrix.GetTheMValue(),m_Matrix.GetNValue());

	int M = m_Matrix.GetTheMValue();
	int N = m_Matrix.GetNValue();

	m_Matrix2 = idct(m_Matrix);
	m_Matrix2.prime();
	m_Matrix2 = idct(m_Matrix2);
	m_Matrix2.prime();


	return m_Matrix2;

}

/////////////////////////////////////////////////////////////////
//Transformation inverse de fournier
/////////////////////////////////////////////////////////////////

void CFFt::ifft2(CMatrixDoubleComplex &m_MatrixComplex)
{
#if !(defined(__ICC) || defined(__ICL) || defined(__ECC) || defined(__ECL))

	ifft(m_MatrixComplex);
	m_MatrixComplex.Prime();
	ifft(m_MatrixComplex);
	m_MatrixComplex.Prime();

#else

	int row = m_MatrixComplex.GetTheMValue();
	int col = m_MatrixComplex.GetNValue();

	m_MatrixComplex.Prime();

	m_MatrixComplex.GetConjMatrix();

	double * ComplexData = m_MatrixComplex.GetDataPt2();
	double * ComplexDataOut = m_MatrixComplex.GetDataPt2();

	fftw_plan p;

	p = fftw_plan_dft_2d(row,col, (fftw_complex *)ComplexData, (fftw_complex *)ComplexDataOut, -1, FFTW_ESTIMATE);

	fftw_execute(p);

	fftw_destroy_plan(p);	

	m_MatrixComplex.ArrayDivi(col*row);

	m_MatrixComplex.Prime();

#endif

}

/////////////////////////////////////////////////////////////////
//Transformation inverse de fournier
/////////////////////////////////////////////////////////////////

#if !(defined(__ICC) || defined(__ICL) || defined(__ECC) || defined(__ECL))


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CFFt::fftSimpleCooleyTukeyType2Complex(double * ComplexDataIn, double * &ComplexDataOut, const int &col,const int &row,const int &iSens)
{
	int iPos = 0;
	double m_dLocalValue = iSens * (PI / col);
	double w,b;
 	double y1,y0,y2;
	double x1,x0,x2,p;
	int i,j,k;
	double m_dTemp;
	double m_dTemp2;
	double * ptData;

	for(i = 0;i < row;i++)
	{
		

		int iPos4 = (iPos << 1);
		int iPos3 = iPos4;
		int inb = 0;

		for(k = 0;k < col;k++)
		{
			ptData = ComplexDataIn + iPos4;
			//int * iIncre = valeurIncre + k * col;

			b = w = inb * m_dLocalValue;

 			y0 = 0;
 			y1 = sin(b);
 			x0 = 1;
 			x1 = cos(b);


			m_dTemp = *ptData + *(ptData + 2) * x1 + *(ptData + 3) * y1;
			m_dTemp2 = *(ptData + 1) + - *(ptData + 2) * y1 + *(ptData + 3) * x1;

			ptData+=4;

			p = 2.0 * cos(w);

			for(j = 2;j < col;j++)
			{

 				y2 = p*y1 - y0;
				x2 = p*x1 - x0;

				m_dTemp += *ptData * x2 + *(ptData + 1) * y2;
				m_dTemp2 += - *ptData * y2 + *(ptData + 1) * x2;
				
				ptData+=2;

 				y0 = y1;
 				y1 = y2;
	 			x0 = x1;
 				x1 = x2;
			}


			*(ComplexDataOut + iPos3) = m_dTemp;
			*(ComplexDataOut + iPos3+1) = m_dTemp2;

			iPos3+=2;
			inb += 2;
		}

		iPos += col;
	}
	
}

#else
	


	void CFFt::fftSimpleCooleyTukeyType2Complex(double * ComplexData, double * & ComplexDataOut,const int &row,const int &col, const int &iSens)
	{
		int i,k,l;
		int iPos = 0;
		
		double * data;
		double * out;


		for(i = 0;i < row;i++)
		{
			data = ComplexData + iPos;
			out = ComplexDataOut + iPos;

			fftw_plan p;

			p = fftw_plan_dft_1d(col, (fftw_complex *)data, (fftw_complex *)out, -1, FFTW_ESTIMATE);

			fftw_execute(p);

			fftw_destroy_plan(p);

			iPos += col << 1;
			
		}

		
	}

#endif
	