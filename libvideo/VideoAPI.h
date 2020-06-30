

class CVideoAPI
{
public:
	CVideoAPI(void);
	~CVideoAPI(void);

	void GetVideoSize(const char * filename,long &iWidth, long &iHeight);
	void VideoToBmp(const char * filename, const char * tempfilename, double dValue,int iWidth, int iHeight);
};
