

#ifndef __SCALEBITMAPINT__H__
#define __SCALEBITMAPINT__H__

class CScaleBitmapInt
{
public:
	CScaleBitmapInt(void);
	~CScaleBitmapInt(void);

	HBITMAP ScaleBitmap(HBITMAP hBmp, WORD wNewWidth, WORD wNewHeight);

private:

	BITMAPINFO * PrepareRGBBitmapInfo(WORD wWidth, WORD wHeight);
	int * CreateCoeff(int nLen, int nNewLen, BOOL bShrink);
	void ShrinkData(BYTE *pInBuff, WORD wWidth, WORD wHeight,BYTE *pOutBuff, WORD wNewWidth, WORD wNewHeight);
	void EnlargeData(BYTE *pInBuff, WORD wWidth, WORD wHeight, BYTE *pOutBuff, WORD wNewWidth, WORD wNewHeight);

};

#endif
