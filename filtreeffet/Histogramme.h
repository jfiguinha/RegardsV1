

#ifndef __HISTOGRAMME_H__
#define __HISTOGRAMME_H__

class CIBitmap;

class CHistogramme
{
public:

	CHistogramme(void);
	~CHistogramme(void);

	void SetBitmap(CIBitmap * ciBitmap)
	{
		m_ciBitmap = ciBitmap;
	}

	void ParcoursBitmap();

	HBITMAP GenerateHistogrammeBitmap(int iWidth, int iHeight, int iColor,COLORREF colorFond = RGB(226,230,238));

private:

	int GetValue(int iColor, int iYMax, int iPos);

	long m_lMoyenneValue[256];
	long m_lMaxMoyenneValue;

	long m_lRedValue[256];
	long m_lMaxRedValue;

	long m_lGreenValue[256];
	long m_lMaxGreenValue;

	long m_lBlueValue[256];
	long m_lMaxBlueValue;

	CIBitmap * m_ciBitmap;

};

#endif