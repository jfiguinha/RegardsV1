// J2K.cpp: implementation of the CJ2K class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "jpeg2000.h"
#include "Image.h"


//#define CEILDIV(a,b) ((a+b-1)/b)



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJ2K::CJ2K()
{

}

CJ2K::~CJ2K()
{

}

int CJ2K::ReadJ2K(CIBitmap * m_ciBitmap,const char * szFileName)
{
	jas_image_t *image=0;
	jas_stream_t *in=0;
	jas_matrix_t **bufs=0;
	long i,error=0;
	FILE * hFile = NULL;
	//FILE * hFile = fopen(szFileName, "rb" );

	errno_t err;
	err = fopen_s(&hFile, szFileName, "rb");

	if(err != 0)
		return -1;

	CxIOFile file(hFile);

  try
  {
	if (jas_init())
		throw "cannot initialize jasper";

	if (!(in = jas_stream_fdopen(0, "rb")))
		throw "error: cannot open standard input";

	CxFileJas src(&file,in);

	if (!(image = jas_image_decode(in, -1, 0))) 
		throw "error: cannot load image data";

	long x,y,w,h,depth,cmptno;

	w = jas_image_cmptwidth(image,0);
	h = jas_image_cmptheight(image,0);
	depth = jas_image_cmptprec(image,0);

	if (image->numcmpts_ > 64 || image->numcmpts_ < 0)
		throw "error: too much components";

	if (depth!=1 && depth!=4 && depth!=8){
		jas_image_t *newimage;
		jas_cmprof_t *outprof;
		//jas_eprintf("forcing conversion to sRGB\n");
		if (!(outprof = jas_cmprof_createfromclrspc(JAS_CLRSPC_SRGB))) {
			throw "cannot create sRGB profile";
		}
		if (!(newimage = jas_image_chclrspc(image, outprof, JAS_CMXFORM_INTENT_PER))) {
			throw "cannot convert to sRGB";
		}
		jas_image_destroy(image);
		jas_cmprof_destroy(outprof);
		image = newimage;
	}

	bufs = (jas_matrix_t **)calloc(image->numcmpts_, sizeof(jas_matrix_t**));
	for (i = 0; i < image->numcmpts_; ++i) {
		if (!(bufs[i] = jas_matrix_create(1, w))) {
			throw "error: cannot allocate memory";
		}
	}

	if (image->numcmpts_==3 &&
		image->cmpts_[0]->width_ == image->cmpts_[1]->width_ &&
		image->cmpts_[1]->width_ == image->cmpts_[2]->width_ &&
		image->cmpts_[0]->height_ == image->cmpts_[1]->height_ &&
		image->cmpts_[1]->height_ == image->cmpts_[2]->height_ &&
		image->cmpts_[0]->prec_  == image->cmpts_[1]->prec_ &&
		image->cmpts_[1]->prec_ == image->cmpts_[2]->prec_ )
	{

		//Conversion en 32 bits

		//UINT lWidthSize = 4*w;//((((w * 32) + 31) & ~31) >> 3);
		BYTE * Data = (BYTE *)malloc(h*w*5);
	
		//RGBQUAD c;

		long pos = 0;

        for (i=0,y=h-1; y>=0; y--)
		{
			for (cmptno = 0; cmptno < image->numcmpts_; ++cmptno) 
			{
				jas_image_readcmpt(image, cmptno, 0, y, w, 1, bufs[cmptno]);
			}
			for (x=0; x<w; x++,i++)
			{
				//UINT pos = (y * lWidthSize) + (x * 3);
				Data[pos]   = (jas_matrix_getv(bufs[2], x));
				Data[pos + 1] = (jas_matrix_getv(bufs[1], x));
				Data[pos + 2] = (jas_matrix_getv(bufs[0], x));
				Data[pos + 3] = 0;

				pos+=4;
				//SetPixelColor(x,h-1-y,c);
			}
		}

		//CImage::VertFlipBuf(Data,lWidthSize,h);

		m_ciBitmap->SetBitmap(Data,w,h);

		free(Data);



	} 
  } 
  catch (...) 
  {
	  m_ciBitmap->CreateBitmap(0,0);
  }

		if (bufs) {
			for (i = 0; i < image->numcmpts_; ++i){	if (bufs[i]) jas_matrix_destroy(bufs[i]);}
			free(bufs);
		}
		jas_cleanup();

		if (image) 
			jas_image_destroy(image);
		if (in) 
			jas_stream_close(in);

	//fclose(hFile);

	return 0;


}

void CJ2K::GetDimensions(const char *fileName, UINT *width, UINT *height)
{
	jas_image_t *image=0;
	jas_stream_t *in=0;
	long i,error=0;
	FILE * hFile = NULL;

	errno_t err;
	err = fopen_s(&hFile, fileName, "rb");

	if(err != 0)
		return;

	CxIOFile file(hFile);

  try
  {
	if (jas_init())
		throw "cannot initialize jasper";

	if (!(in = jas_stream_fdopen(0, "rb")))
		throw "error: cannot open standard input";

	CxFileJas src(&file,in);

	if (!(image = jas_image_decode(in, -1, 0))) 
		throw "error: cannot load image data";

	*width = jas_image_cmptwidth(image,0);
	*height = jas_image_cmptheight(image,0);

  }
  catch(...)
  {
  }

	jas_cleanup();
	if (image) 
		jas_image_destroy(image);

	if (in) 
		jas_stream_close(in);
	//fclose(hFile);
}
