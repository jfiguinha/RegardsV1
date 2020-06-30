

#ifndef __MARGEDATA__H__
#define __MARGEDATA__H__

class CIBitmap;

class CMargeData
{
public:
	CMargeData(void);
	~CMargeData(void);

	float m_fHaut;
	float m_fBas;
	float m_fDroite;
	float m_fGauche;
	int iPos;
	CIBitmap * m_ciBitmap;
};

#endif