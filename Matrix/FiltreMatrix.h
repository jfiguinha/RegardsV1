// Filtre.h: interface for the CFiltre class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILTRE_H__ACB5BE83_FA20_476B_B592_FC3FFE734F2F__INCLUDED_)
#define AFX_FILTRE_H__ACB5BE83_FA20_476B_B592_FC3FFE734F2F__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000


class CMatrix;
class CMatrixDoubleComplex;

class CFiltreMatrix 
{
public:

	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////

	CFiltreMatrix();

	virtual ~CFiltreMatrix();

	static CMatrix Filter2(const CMatrix &G,const CMatrix &F);

	static CMatrix Filter2Conv(const CMatrix &G,const CMatrix &F);

	//static CMatrix restdct(const CMatrix &m_MatrixImage,const int &iHeightFiltre,const int &iWidthFiltre, const double &rho);

	static CMatrixDoubleComplex calcul_filtrewiener(const int &iHeightImage, const int &iWidthImage,const CMatrix &FiltreDegradation, const double &rho);

	static CMatrix restdct(const CMatrix &m_MatrixImage,const CMatrixDoubleComplex &G);

	static CMatrix restWiener(const CMatrix &m_MatrixImage,const CMatrixDoubleComplex &G);


	///////////////////////////////////////////////////////////////////////
	//Liste des différents filtres
	///////////////////////////////////////////////////////////////////////

	static CMatrix GetGaussianMatrix();
	static CMatrix GetBlurMatrix();
	static CMatrix GetMotionBlurMatrix(const double &sigma,const int &Width = 3);
	static CMatrix GetSoftenMatrix();
	static CMatrix GetSharpenMatrix();
	static CMatrix GetEdgeMatrix();
	static CMatrix GetEmbossMatrix();
};

#endif // !defined(AFX_FILTRE_H__ACB5BE83_FA20_476B_B592_FC3FFE734F2F__INCLUDED_)
