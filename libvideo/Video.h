
class ID2D1HwndRenderTarget;

class CVideo
{
public:
	CVideo(void);
	~CVideo(void);
	static void Cleanup();
	HBITMAP DoExtractVideoFrame(const char *szFileName,long &Width, long &Height);
	HRESULT VideoToBmp(const char * pVideoName, const char * pBmpName, double start, int iWidth, int iHeight);
	HRESULT GetVideoSize(const char *szFileName,long &Width, long &Height);
private:
	static ID2D1HwndRenderTarget   * g_pRT;      // Render target for D2D animation
	HRESULT CreateDrawingResources(HWND hwnd);
};
