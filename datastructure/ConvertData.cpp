#include "stdafx.h"
#include ".\convertdata.h"

CConvertData::CConvertData(void)
{
	m_szRepOrigin[0] = '\0';
	m_szRepDest[0] = '\0';
	m_szLibelle[0] = '\0';
	iConverFormat = 0;
	iPourcentage = 0;
	iValeurPourcentage = 0;
	iNumFormat = 0;
	iDelImage = 0;
	iRedimensionne = 0;
	iRapport = 0;
	iWidth = 0;
	iHeight = 0;
	iTailleNumero = 0;
	iIncrementation = 0;
	iPosNumero = 0;
}

CConvertData::~CConvertData(void)
{
}
