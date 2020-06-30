

#ifndef __SCALEBITMAP__H__
#define __SCALEBITMAP__H__

class CScaleBitmap
{
public:
	CScaleBitmap(void);
	~CScaleBitmap(void);

	HBITMAP ScaleBitmap(HBITMAP hBmp, WORD wNewWidth, WORD wNewHeight);

private:

	BITMAPINFO * PrepareRGBBitmapInfo(WORD wWidth, WORD wHeight);
	float * CreateCoeff(int nLen, int nNewLen, BOOL bShrink);
	void ShrinkData(BYTE *pInBuff, WORD wWidth, WORD wHeight,BYTE *pOutBuff, WORD wNewWidth, WORD wNewHeight);
	void EnlargeData(BYTE *pInBuff, WORD wWidth, WORD wHeight, BYTE *pOutBuff, WORD wNewWidth, WORD wNewHeight);
	HBITMAP CopyScreenToBitmap(HBITMAP hBitmap, int iWidth, int iHeight);
};

#endif