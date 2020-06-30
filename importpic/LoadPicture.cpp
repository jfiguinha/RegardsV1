// ChargementImage.cpp: implementation of the CChargementImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "LoadPicture.h"
#include "image.h"
#include "CPCD.h"
#include "cTiff.h"
#include "tga.h"
#include "pcx.h"
#include "cpng.h"
#include "jpegfile.h"
#include "gif.h"
#include "bmp.h"
#include "dib.h"
#include "psd.h"
#include "Ppm.h"
#include "pdf.h"
#include "jpeg2000.h"
#include "CameraRaw.h"


const int CLoadPicture::BMP = 2;
const int CLoadPicture::JPEG = 1;
const int CLoadPicture::PDF = 13;
const int CLoadPicture::TIFF = 3;
const int CLoadPicture::TGA = 6;

JpegFile CLoadPicture::m_cJpeg;
CCameraRaw CLoadPicture::m_cRaw;
CBmp CLoadPicture::m_cBmp;
CTif CLoadPicture::m_cTif;
CGif CLoadPicture::m_cGif;
CPng CLoadPicture::m_cPng;
CTga CLoadPicture::m_cTga;
CPcx CLoadPicture::m_cPcx;
CCPCD CLoadPicture::m_cPcd;
CJ2K CLoadPicture::m_cJ2k;
CPsd CLoadPicture::m_cPsd;
CPpm CLoadPicture::m_cPpm;
CPdf CLoadPicture::m_cPdf;

BOOL CLoadPicture::GetImageFromJPG(const char * csFileName, HBITMAP &hBitmap,const long &JPGThumbWidth, const long &JPGThumbHeight)
{
	return m_cJpeg.GetImageFromJPG(csFileName,hBitmap,JPGThumbWidth,JPGThumbHeight);
}

BOOL CLoadPicture::GetImageFromRAW(const char * csFileName, CIBitmap * m_ciBitmap,const long &JPGThumbWidth, const long &JPGThumbHeight)
{
	return m_cRaw.ReadRawThumbnail(m_ciBitmap,csFileName,JPGThumbWidth,JPGThumbHeight);
}

/////////////////////////////////////////////////////////////////
//On récupère les dimensions pour les images
/////////////////////////////////////////////////////////////////
void CLoadPicture::GetDimensions(const char *fileName, UINT *width, UINT *height, const int &iFormat)
{
	*width = 0;
	*height = 0;
	switch(iFormat)
	{
		case 1:
			{
				m_cJpeg.GetJPGDimensions(fileName,width,height);
			}
			break;
		
		case 2:
			{
				m_cBmp.GetJDimensions(fileName,width,height);
			}
			break;
		case 3:
			{

				m_cTif.GetDimensions(fileName,width,height);
			}
			break;
		case 4:
			{

				m_cGif.GetDimensions(fileName,width,height);
			}
			break;
		case 5:
			{
				
				if(m_cPng.GetDimensions(fileName,width,height) == -1)
				{
					*width = 0;
					*height = 0;
				}
			}
			break;
		case 6:
			{
				
				m_cTga.GetDimensions(fileName,width,height);
			}
			break;
		case 7:
			{
				
				m_cPcx.GetDimensions(fileName,width,height);
			}
			break;

		case 8:
			{
				
				m_cPcd.GetJDimensions(fileName,width,height);
			}
			break;

		case 9:
			{
				
				m_cJ2k.GetDimensions(fileName,width,height);
			}
			break;

		case 10:
			{
				
				m_cPsd.GetDimensions(fileName,width,height);
			}
			break;


		case 11:
			{
				m_cPpm.GetDimensions(fileName,width,height);
			}
			break;

		case 12:
			{
				m_cRaw.GetDimensions(fileName,width,height);
			}
	}
}



/////////////////////////////////////////////////////////////////
//On charge les images
/////////////////////////////////////////////////////////////////
void CLoadPicture::ChargementImage(const int &m_iValue,const char *szFileName, CIBitmap * m_ciBitmap, bool bFullScreen)
{
		switch(m_iValue)
		{
			case 1:
				{
					m_cJpeg.ReadJpeg(m_ciBitmap,szFileName);
				}
				break;
			
			case 2:
				{
					m_cBmp.ReadBmp(m_ciBitmap,szFileName);
				}
				break;

			case 3:
				{
					m_cTif.ReadTif(m_ciBitmap,szFileName);
				}
				break;

			case 4:
				{
					m_cGif.ReadGif(m_ciBitmap,szFileName);
				}
				break;

			case 5:
				{
					m_cPng.ReadPng(m_ciBitmap,szFileName);
				}
				break;

			case 6:
				{
					m_cTga.TGA(m_ciBitmap,szFileName);
				}
				break;

			case 7:
				{
					m_cPcx.ReadPcx(m_ciBitmap,szFileName);	
					break;
				}

			case 8:
				{
					m_cPcd.readPCD(m_ciBitmap,szFileName);
				}
				break;

			case 9:
				{
					m_cJ2k.ReadJ2K(m_ciBitmap,szFileName);
				}
				break;

			case 10:
				{
					m_cPsd.ReadPsd(m_ciBitmap,szFileName);
				}
				break;

			case 11:
				{
					m_cPpm.ReadPPM(m_ciBitmap,szFileName);
				}
				break;

			case 12:
				{
					m_cRaw.ReadRaw(m_ciBitmap,szFileName,bFullScreen);
				}
				break;
		}
}

/////////////////////////////////////////////////////////////////
//On Sauvegarde les images
/////////////////////////////////////////////////////////////////
void CLoadPicture::SaveImage(const int &m_iValue, CIBitmap &m_ciBitmap,const char * szFileName,const int &compress,const int &iJpegQuality)
{
		switch(m_iValue)
		{
			case JPEG:
				{
					char pcsMsg[255];
					m_cJpeg.JpegFromDib(m_ciBitmap,iJpegQuality,szFileName,pcsMsg);
				}
				break;
			
			case BMP:
				{
					 m_cBmp.SaveBMP(m_ciBitmap,szFileName);
				}
				break;

			case TIFF:
				{
					m_cTif.SaveTif(m_ciBitmap,szFileName,(bool)compress,iJpegQuality);
				}
				break;

			case TGA:
				{
					m_cTga.SaveTGA(m_ciBitmap,szFileName);
				}
				break;

			case PDF: //PDF
				{
					
					m_cPdf.WritePDFImage(m_ciBitmap,szFileName,compress,iJpegQuality);
				}
				break;
		}
}
