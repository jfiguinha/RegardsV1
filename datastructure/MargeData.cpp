#include "stdafx.h"
#include ".\margedata.h"
#include <ibitmap.h>

CMargeData::CMargeData(void)
{
	m_fHaut = 0.0;
	m_fBas = 0.0;
	m_fDroite = 0.0;
	m_fGauche = 0.0;
	iPos = 0;
	m_ciBitmap = NULL;
}

CMargeData::~CMargeData(void)
{
}
