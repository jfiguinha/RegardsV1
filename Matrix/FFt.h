// FFt.h: interface for the CFFt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FFT_H__5317793A_1245_44B7_BC4E_581AA9CAA9CD__INCLUDED_)
#define AFX_FFT_H__5317793A_1245_44B7_BC4E_581AA9CAA9CD__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

class CMatrixDoubleComplex;
class CMatrix;

class CFFt  
{
public:
	CFFt();
	virtual ~CFFt();




	static void fft(CMatrixDoubleComplex &m_Matrix);
	static CMatrixDoubleComplex fft(const CMatrix &m_Matrix);
	static CMatrixDoubleComplex fft2(const CMatrix &m_Matrix);
	static void fft2(CMatrixDoubleComplex &m_MatrixComplex);

	static void ifft2(CMatrixDoubleComplex &m_MatrixComplex);
	static void ifft(CMatrixDoubleComplex &m_MatrixComplex);


	static CMatrix dct2(const CMatrix &m_Matrix);

	static CMatrix dct(const CMatrix &m_Matrix);

	static CMatrix idct2(const CMatrix &m_Matrix);

	static CMatrix idct(const CMatrix &m_Matrix);


	static CMatrix conv2fft(const CMatrix &m_MatrixSource,const CMatrix &m_MatrixFilter);


private:

	static void fftSimpleCooleyTukeyType2Complex(double * ComplexData, double * & ComplexDataOut,const int &row,const int &col, const int &iSens);
	static void four1(double * & data, const int &isign,const int &iDataSize);

};

#endif // !defined(AFX_FFT_H__5317793A_1245_44B7_BC4E_581AA9CAA9CD__INCLUDED_)
