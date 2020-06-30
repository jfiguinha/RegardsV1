

#include <atlbase.h>
#include <strmif.h>
#include "Qedit.h"

class CVideo
{
public:
	CVideo(void);
	~CVideo(void);

	HBITMAP DoExtractVideoFrame(const char *szFileName,long &Width, long &Height);
	HRESULT BmpToVideo( const char * firstImage, const char * outAvi, int length, double dFrameRate);
	HRESULT VideoToBmp( const char * pVideoName, const char * pBmpName, double start, int iWidth, int iHeight);
	HRESULT GetVideoSize(const char *szFileName,long &Width, long &Height);
private:

	void MyFreeMediaType(AM_MEDIA_TYPE& mt);
	//CComPtr< IMediaDet > pDet;
};
