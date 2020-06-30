

#ifndef __CONVERTDATA__H__
#define __CONVERTDATA__H__

class CConvertData
{
public:
	CConvertData(void);
	~CConvertData(void);

	char m_szRepOrigin[MAX_PATH];
	char m_szRepDest[MAX_PATH];
	char m_szLibelle[100];
	int iConverFormat;
	int iPourcentage;
	int iValeurPourcentage;
	int iNumFormat;
	int iDelImage;
	int iRedimensionne;
	int iRapport;
	int iWidth;
	int iHeight;
	int iTailleNumero;
	int iIncrementation;
	int iPosNumero;
	HWND hWnd;
};

#endif
