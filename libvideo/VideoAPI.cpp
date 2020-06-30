#include "StdAfx.h"
#include "VideoAPI.h"
#include "video.h"

CVideoAPI::CVideoAPI(void)
{
}

CVideoAPI::~CVideoAPI(void)
{
}

void CVideoAPI::GetVideoSize(const char * filename,long &iWidth, long &iHeight)
{
	CVideo * m_cVideo = new CVideo();
	m_cVideo->GetVideoSize(filename,iWidth,iHeight);
	delete m_cVideo;
}

void CVideoAPI::VideoToBmp(const char * filename, const char * tempfilename, double dValue,int iWidth, int iHeight)
{
	CVideo * m_cVideo = new CVideo();
	m_cVideo->VideoToBmp(filename,tempfilename,dValue,iWidth,iHeight);
	delete m_cVideo;
}