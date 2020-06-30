////////////////////////////////////////////////////////////
//	JpegFile - A C++ class to allow reading and writing of
//	RGB and Grayscale JPEG images.
//	It is based on the IJG V.6 code.
//
//	This class Copyright 1997, Chris Losinger
//	This is free to use and modify provided my name is 
//	included.
//
//	See jpegfile.h for usage.
//
////////////////////////////////////////////////////////////

#include "stdafx.h"

#undef FAR        // gets rid of a warning
#define XMD_H     // fixes a type redefiniton issue
#include "JpegFile.h"
#include "ijl.h"
#include "jpeg2000.h"
#include "image.h"

RGBQUAD QuadFromWord(WORD b16);

////////////////////////////////////////
//This function turns a 16-bit pixel
//into an RGBQUAD value.
////////////////////////////////////////

RGBQUAD QuadFromWord(WORD b16)
{
   BYTE bytVals[] =
   {
     0,  16, 24, 32,  40, 48, 56, 64,
     72, 80, 88, 96, 104,112,120,128,
     136,144,152,160,168,176,184,192,
     200,208,216,224,232,240,248,255
   };

   WORD wR = b16;
   WORD wG = b16;
   WORD wB = b16;

   wR <<= 1; wR >>= 11;
   wG <<= 6; wG >>= 11;
   wB <<= 11; wB >>= 11;

   RGBQUAD rgb;

   rgb.rgbReserved = 0;
   rgb.rgbBlue     = bytVals[wB];
   rgb.rgbGreen    = bytVals[wG];
   rgb.rgbRed      = bytVals[wR];

   return rgb;
}


/*
 * <setjmp.h> is used for the optional error recovery mechanism shown in
 * the second part of the example.
 */

#include <setjmp.h>

// error handler, to avoid those pesky exit(0)'s

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

//
//
//

METHODDEF(void) my_error_exit (j_common_ptr cinfo);

//
//	to handle fatal errors.
//	the original JPEG code will just exit(0). can't really
//	do that in Windows....
//


METHODDEF(void) my_error_exit (j_common_ptr cinfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	my_error_ptr myerr = (my_error_ptr) cinfo->err;

	char buffer[JMSG_LENGTH_MAX];

	/* Create the message */
	(*cinfo->err->format_message) (cinfo, buffer);

	/* Always display the message. */
	//MessageBox(NULL,buffer,"JPEG Fatal Error",MB_ICONSTOP);


	/* Return control to the setjmp point */
	longjmp(myerr->setjmp_buffer, 1);
}

//
//	constructor doesn't do much - there's no real class here...
//

JpegFile::JpegFile()
{
}

//

//

JpegFile::~JpegFile()
{
}

BOOL JpegFile::JpegFromDib(CIBitmap &m_ciBitmap,     //Handle to DIB
                 int        nQuality, //JPEG quality (0-100)
                 const char * csJpeg,   //Pathname to jpeg file
                 char *   pcsMsg)   //Error msg to return
{

    //Basic sanity checks...
    if (nQuality < 0 || nQuality > 100 ||
        m_ciBitmap.GetPtBitmap()   == NULL ||
        pcsMsg == NULL ||
        csJpeg == "")
    {
        //if (pcsMsg != NULL)
         //   StringCchCopy(pcsMsg,50,"Invalid input data");

        return FALSE;
    }

    //LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)hDib;

    BYTE *buf2 = 0;

    //Use libjpeg functions to write scanlines to disk in JPEG format
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr       jerr;

    FILE*      pOutFile;     //Target file 
    int        nSampsPerRow; //Physical row width in image buffer 
    JSAMPARRAY jsmpArray;    //Pixel RGB buffer for JPEG file

    cinfo.err = jpeg_std_error(&jerr); //Use default error handling (ugly!)

    jpeg_create_compress(&cinfo);

	errno_t err;
	err = fopen_s(&pOutFile, csJpeg, "wb");

    if (err != 0)
    {
		//StringCchCopy(pcsMsg,50,"Cannot open file");
        jpeg_destroy_compress(&cinfo);
        return FALSE;
    }

    jpeg_stdio_dest(&cinfo, pOutFile);

    cinfo.image_width      = m_ciBitmap.GetBitmapWidth();  //Image width and height, in pixels 
    cinfo.image_height     = m_ciBitmap.GetBitmapHeigth();
    cinfo.input_components = 3;              //Color components per pixel
                                             //(RGB_PIXELSIZE - see jmorecfg.h)
    cinfo.in_color_space   = JCS_RGB; 	     //Colorspace of input image

    jpeg_set_defaults(&cinfo);

    jpeg_set_quality(&cinfo,
                     nQuality, //Quality: 0-100 scale
                     TRUE);    //Limit to baseline-JPEG values

    jpeg_start_compress(&cinfo, TRUE);

    //JSAMPLEs per row in output buffer
    nSampsPerRow = cinfo.image_width * cinfo.input_components; 

    //Allocate array of pixel RGB values
    jsmpArray = (*cinfo.mem->alloc_sarray)
                ((j_common_ptr) &cinfo,
                 JPOOL_IMAGE,
                 nSampsPerRow,
                 cinfo.image_height);

    if (DibToSamps(m_ciBitmap,
                   nSampsPerRow,
                   cinfo,
                   jsmpArray,
                   pcsMsg))
    {
        //Write the array of scan lines to the JPEG file
        (void)jpeg_write_scanlines(&cinfo,
                                   jsmpArray,
                                   cinfo.image_height);
    }

    jpeg_finish_compress(&cinfo); //Always finish

    fclose(pOutFile);

    jpeg_destroy_compress(&cinfo); //Free resources

    if (strlen(pcsMsg) > 0)
        return FALSE;

    else
        return TRUE;
}

//
//	read a JPEG file
//

BYTE * JpegFile::JpegFileToRGB(const char * fileName,
							   UINT *width,
							   UINT *height)

{

	// basic code from IJG Jpeg Code v6 example.c

	*width=0;
	*height=0;

	/* This struct contains the JPEG decompression parameters and pointers to
	* working space (which is allocated as needed by the JPEG library).
	*/
	struct jpeg_decompress_struct cinfo;
	/* We use our private extension JPEG error handler.
	* Note that this struct must live as long as the main JPEG parameter
	* struct, to avoid dangling-pointer problems.
	*/
	struct my_error_mgr jerr;
	/* More stuff */
	FILE * infile=NULL;		/* source file */

	JSAMPARRAY buffer;		/* Output row buffer */
	int row_stride;		/* physical row width in output buffer */
	char buf[250];

	/* In this example we want to open the input file before doing anything else,
	* so that the setjmp() error recovery below can assume the file is open.
	* VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
	* requires it in order to read binary files.
	*/

	errno_t err;
	err = fopen_s(&infile, fileName, "rb");

	if (err != 0) {
		//StringCchCopy(buf,50,"Cannot open file");
		return NULL;
	}

	/* Step 1: allocate and initialize JPEG decompression object */

	/* We set up the normal JPEG error routines, then virtualerror_exit. */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;


	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer)) {
		/* If we get here, the JPEG code has signaled an error.
		 * We need to clean up the JPEG object, close the input file, and return.
		 */

		jpeg_destroy_decompress(&cinfo);

		if (infile!=NULL)
			fclose(infile);
		return NULL;
	}

	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(&cinfo);

	/* Step 2: specify data source (eg, a file) */

	jpeg_stdio_src(&cinfo, infile);

	/* Step 3: read file parameters with jpeg_read_header() */

	(void) jpeg_read_header(&cinfo, TRUE);
	/* We can ignore the return value from jpeg_read_header since
	*   (a) suspension is not possible with the stdio data source, and
	*   (b) we passed TRUE to reject a tables-only JPEG file as an error.
	* See libjpeg.doc for more info.
	*/

	/* Step 4: set parameters for decompression */

	/* In this example, we don't need to change any of the defaults set by
	* jpeg_read_header(), so we do nothing here.
	*/

	/* Step 5: Start decompressor */

	(void) jpeg_start_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	* with the stdio data source.
	*/

	/* We may need to do some setup of our own at this point before reading
	* the data.  After jpeg_start_decompress() we have the correct scaled
	* output image dimensions available, as well as the output colormap
	* if we asked for color quantization.
	* In this example, we need to make an output work buffer of the right size.
	*/ 

	// get our buffer set to hold data
	BYTE *dataBuf;

	////////////////////////////////////////////////////////////
	// alloc and open our new buffer
	dataBuf=(BYTE *)new BYTE[cinfo.output_width * 3 * cinfo.output_height];
	if (dataBuf==NULL) {

		jpeg_destroy_decompress(&cinfo);
		
		fclose(infile);

		return NULL;
	}

	// how big is this thing gonna be?
	*width = cinfo.output_width;
	*height = cinfo.output_height;
	
	/* JSAMPLEs per row in output buffer */
	row_stride = cinfo.output_width * cinfo.output_components;

	/* Make a one-row-high sample array that will go away when done with image */
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	/* Step 6: while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */

	/* Here we use the library's state variable cinfo.output_scanline as the
	* loop counter, so that we don't have to keep track ourselves.
	*/
	while (cinfo.output_scanline < cinfo.output_height) {
		/* jpeg_read_scanlines expects an array of pointers to scanlines.
		 * Here the array is only one element long, but you could ask for
		 * more than one scanline at a time if that's more convenient.
		 */
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);
		/* Assume put_scanline_someplace wants a pointer and sample count. */

		// asuumer all 3-components are RGBs
		if (cinfo.out_color_components==3) {
			
			j_putRGBScanline(buffer[0], 
								*width,
								dataBuf,
								cinfo.output_scanline-1);

		} else if (cinfo.out_color_components==1) {

			// assume all single component images are grayscale
			j_putGrayScanlineToRGB(buffer[0], 
								*width,
								dataBuf,
								cinfo.output_scanline-1);

		}

	}

	/* Step 7: Finish decompression */

	(void) jpeg_finish_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	* with the stdio data source.
	*/

	/* Step 8: Release JPEG decompression object */

	/* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_decompress(&cinfo);

	/* After finish_decompress, we can close the input file.
	* Here we postpone it until after no more JPEG errors are possible,
	* so as to simplify the setjmp error logic above.  (Actually, I don't
	* think that jpeg_destroy can do an error exit, but why assume anything...)
	*/
	fclose(infile);

	/* At this point you may want to check to see whether any corrupt-data
	* warnings occurred (test whether jerr.pub.num_warnings is nonzero).
	*/

	return dataBuf;
}

BOOL JpegFile::GetJPGDimensions(const char * fileName,
								UINT *width,
								UINT *height)

{
  int                  pad_bytes;
  int                  dib_line_size;
  int                  size;
  BOOL                 bres = TRUE;
  char                 comments[1024];
  IJLERR               jerr;
  JPEG_CORE_PROPERTIES jcprops;

	try
	{

	comments[0] = 0;

	jerr = ijlInit(&jcprops);

	if(IJL_OK != jerr)
	{
		bres = FALSE;
		goto Exit;
	}

	jcprops.JPGFile = fileName;

	jcprops.jprops.jpeg_comment      = &comments[0];
	jcprops.jprops.jpeg_comment_size = sizeof(comments);

	jerr = ijlRead(&jcprops,IJL_JFILE_READPARAMS);

	if(IJL_OK != jerr && IJL_ERR_COM_BUFFER != jerr)
	{
		bres = FALSE;
		goto Exit;
	}


	*width  = jcprops.JPGWidth;
	*height = jcprops.JPGHeight;

	Exit:
		ijlFree(&jcprops);
}
catch(...)
{
}



  return TRUE;
}

//
//
//

BOOL JpegFile::RGBToJpegFile(const char * fileName, 
							BYTE *dataBuf,
							UINT widthPix,
							UINT height,
							BOOL color, 
							int quality)
{
	if (dataBuf==NULL)
		return FALSE;
	if (widthPix==0)
		return FALSE;
	if (height==0)
		return FALSE;

	LPBYTE tmp;
	if (!color) {
		tmp = (BYTE*)new BYTE[widthPix*height];
		if (tmp==NULL) {
			return FALSE;
		}

		UINT row,col;
		for (row=0;row<height;row++) {
			for (col=0;col<widthPix;col++) {
				LPBYTE pRed, pGrn, pBlu;
				pRed = dataBuf + row * widthPix * 3 + col * 3;
				pGrn = dataBuf + row * widthPix * 3 + col * 3 + 1;
				pBlu = dataBuf + row * widthPix * 3 + col * 3 + 2;

				// luminance
				int lum = (int)(.299 * (double)(*pRed) + .587 * (double)(*pGrn) + .114 * (double)(*pBlu));
				LPBYTE pGray;
				pGray = tmp + row * widthPix + col;
				*pGray = (BYTE)lum;
			}
		}
	}

	struct jpeg_compress_struct cinfo;
	/* More stuff */
	FILE * outfile=NULL;			/* target file */
	int row_stride;			/* physical row widthPix in image buffer */

	struct my_error_mgr jerr;

	/* Step 1: allocate and initialize JPEG compression object */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer)) {
		/* If we get here, the JPEG code has signaled an error.
		 * We need to clean up the JPEG object, close the input file, and return.
		 */

		jpeg_destroy_compress(&cinfo);

		if (outfile!=NULL)
			fclose(outfile);

		if (!color) {
			delete [] tmp;
		}
		return FALSE;
	}

	/* Now we can initialize the JPEG compression object. */
	jpeg_create_compress(&cinfo);

	/* Step 2: specify data destination (eg, a file) */
	/* Note: steps 2 and 3 can be done in either order. */

	errno_t err;
	err = fopen_s(&outfile, fileName, "wb");

	if (err != 0) {
		//char buf[250];
		//StringCchCopy(buf,50,"Cannot open file");
		return FALSE;
	}

	jpeg_stdio_dest(&cinfo, outfile);

	/* Step 3: set parameters for compression */
												    
	/* First we supply a description of the input image.
	* Four fields of the cinfo struct must be filled in:
	*/
	cinfo.image_width = widthPix; 	/* image widthPix and height, in pixels */
	cinfo.image_height = height;
	if (color) {
		cinfo.input_components = 3;		/* # of color components per pixel */
		cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */
	} else {
		cinfo.input_components = 1;		/* # of color components per pixel */
		cinfo.in_color_space = JCS_GRAYSCALE; 	/* colorspace of input image */
	}

 
/* Now use the library's routine to set default compression parameters.
   * (You must set at least cinfo.in_color_space before calling this,
   * since the defaults depend on the source color space.)
   */

  jpeg_set_defaults(&cinfo);
  /* Now you can set any non-default parameters you wish to.
   * Here we just illustrate the use of quality (quantization table) scaling:
   */
  jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

  /* Step 4: Start compressor */

  /* TRUE ensures that we will write a complete interchange-JPEG file.
   * Pass TRUE unless you are very sure of what you're doing.
   */
  jpeg_start_compress(&cinfo, TRUE);

  /* Step 5: while (scan lines remain to be written) */
  /*           jpeg_write_scanlines(...); */

  /* Here we use the library's state variable cinfo.next_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   * To keep things simple, we pass one scanline per call; you can pass
   * more if you wish, though.
   */
  row_stride = widthPix * 3;	/* JSAMPLEs per row in image_buffer */

  while (cinfo.next_scanline < cinfo.image_height) {
    /* jpeg_write_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could pass
     * more than one scanline at a time if that's more convenient.
     */
	LPBYTE outRow;
	if (color) {
		outRow = dataBuf + (cinfo.next_scanline * widthPix * 3);
	} else {
		outRow = tmp + (cinfo.next_scanline * widthPix);
	}

    (void) jpeg_write_scanlines(&cinfo, &outRow, 1);
  }

  /* Step 6: Finish compression */

  jpeg_finish_compress(&cinfo);

  /* After finish_compress, we can close the output file. */
  fclose(outfile);

  /* Step 7: release JPEG compression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_compress(&cinfo);

  if (!color)
	  delete [] tmp;
  /* And we're done! */

  return TRUE;
}


//
//	stash a scanline
//

void JpegFile::j_putRGBScanline(BYTE *jpegline, 
					 int widthPix,
					 BYTE *outBuf,
					 int row)
{
	try
	{
		int offset = row * widthPix * 3;
		int count;
		for (count=0;count<widthPix;count++) 
		{
			*(outBuf + offset + count * 3 + 0) = *(jpegline + count * 3 + 0);
			*(outBuf + offset + count * 3 + 1) = *(jpegline + count * 3 + 1);
			*(outBuf + offset + count * 3 + 2) = *(jpegline + count * 3 + 2);
		}
	}
	catch(...)
	{}
}

//
//	stash a gray scanline
//

void JpegFile::j_putGrayScanlineToRGB(BYTE *jpegline, 
							 int widthPix,
							 BYTE *outBuf,
							 int row)
{
	int offset = row * widthPix * 3;
	int count;

	try
	{

		for (count=0;count<widthPix;count++) {

			BYTE iGray;

			// get our grayscale value
			iGray = *(jpegline + count);

			*(outBuf + offset + count * 3 + 0) = iGray;
			*(outBuf + offset + count * 3 + 1) = iGray;
			*(outBuf + offset + count * 3 + 2) = iGray;
		}
	}
	catch(...)
	{

	}
}

BOOL JpegFile::DibToSamps(CIBitmap &m_ciBitmap,
                int                         nSampsPerRow,
                struct jpeg_compress_struct cinfo,
                JSAMPARRAY                  jsmpPixels,
                char *                    pcsMsg)
{
   //Sanity...
   if (m_ciBitmap.GetPtBitmap() == NULL    ||
     nSampsPerRow <= 0 || pcsMsg == NULL) 
   { 
     //if (pcsMsg !=NULL) 
     //   StringCchCopy(pcsMsg,50,"Invalid input data"); 
     return FALSE; 
   } 

   int r=0, p=0, q=0, b=0, n=0, 
       nUnused=0, nBytesWide=0, nUsed=0, nLastBits=0, nLastNibs=0, nCTEntries=0,
       nRow=0, nByte=0, nPixel=0;
   BYTE bytCTEnt=0;
   //LPBITMAPINFOHEADER pbBmHdr= (LPBITMAPINFOHEADER)hDib; //The bit count tells you the format of the bitmap: //Decide how many entries will be in the color table (if any) 

   switch (m_ciBitmap.GetBitmapDepth())
   {
	   /*
      case 1:
         nCTEntries = 2;   //Monochrome
         break;

      case 4:
         nCTEntries = 16;  //16-color
         break;

      case 8:
         nCTEntries = 256; //256-color
         break;*/

      case 16:
      case 24:
      case 32:
         nCTEntries = 0;   //No color table needed
         break;

      default:
         //StringCchCopy(pcsMsg,50,"Invalid bitmap bit count");
         return FALSE; //Unsupported format
   }

   /*
   //Point to the color table and pixels
   DWORD     dwCTab = (DWORD)pbBmHdr + pbBmHdr->biSize;
   LPRGBQUAD pCTab  = (LPRGBQUAD)(dwCTab);
   LPSTR     lpBits = (LPSTR)pbBmHdr +
                      (WORD)pbBmHdr->biSize +
                      (WORD)(nCTEntries * sizeof(RGBQUAD));

   //Different formats for the image bits
   LPBYTE   lpPixels = (LPBYTE)  lpBits;
   RGBQUAD* pRgbQs   = (RGBQUAD*)lpBits;
   WORD*    wPixels  = (WORD*)   lpBits;*/

   //Set up the jsamps according to the bitmap's format.
   //Note that rows are processed bottom to top, because
   //that's how bitmaps are created.
   switch (m_ciBitmap.GetBitmapDepth())
   {
	   /*
      case 1:
         nUsed      = (pbBmHdr->biWidth + 7) / 8;
         nUnused    = (((nUsed + 3) / 4) * 4) - nUsed;
         nBytesWide = nUsed + nUnused;
         nLastBits  = 8 - ((nUsed * 8) - pbBmHdr->biWidth);

         for (r=0; r < pbBmHdr->biHeight; r++)
         {
            for (p=0,q=0; p < nUsed; p++) 
            { 
               nRow=(pbBmHdr->biHeight-r-1) * nBytesWide;
               nByte =  nRow + p;

               int nBUsed = (p <(nUsed - 1)) ? 8 : nLastBits; //Modification apportée
			   
			   for(b=0; b < nBUsed;b++) 
               { 
                  bytCTEnt = lpPixels[nByte] << b; 
                  bytCTEnt = bytCTEnt >> 7;

                  jsmpPixels[r][q+0] = pCTab[bytCTEnt].rgbRed;
                  jsmpPixels[r][q+1] = pCTab[bytCTEnt].rgbGreen;
                  jsmpPixels[r][q+2] = pCTab[bytCTEnt].rgbBlue;

                  q += 3;
               }
            }
         }
         break;

      case 4:
         nUsed      = (pbBmHdr->biWidth + 1) / 2;
         nUnused    = (((nUsed + 3) / 4) * 4) - nUsed;
         nBytesWide = nUsed + nUnused;
         nLastNibs  = 2 - ((nUsed * 2) - pbBmHdr->biWidth);

         for (r=0; r < pbBmHdr->biHeight;r++)
         {
            for (p=0,q=0; p < nUsed;p++) 
            { 
               nRow=(pbBmHdr->biHeight-r-1) * nBytesWide;
               nByte = nRow + p;

               int nNibbles = (p  > (4-(n*4)));

                  jsmpPixels[r][q+0] = pCTab[bytCTEnt].rgbRed;
                  jsmpPixels[r][q+1] = pCTab[bytCTEnt].rgbGreen;
                  jsmpPixels[r][q+2] = pCTab[bytCTEnt].rgbBlue;

                  q += 3;
               //}
            }
         }
         break;

      default:
      case 8: //Each byte is a pointer to a pixel color
         nUnused = (((pbBmHdr->biWidth + 3) / 4) * 4) -
                   pbBmHdr->biWidth;

         for (r=0;r < pbBmHdr->biHeight; r++)
         {
            for (p=0,q=0; p < pbBmHdr->biWidth; p++,q+=3)
            {
               nRow   = (pbBmHdr->biHeight-r-1) * (pbBmHdr->biWidth + nUnused);
               nPixel =  nRow + p;

               jsmpPixels[r][q+0] = pCTab[lpPixels[nPixel]].rgbRed;
               jsmpPixels[r][q+1] = pCTab[lpPixels[nPixel]].rgbGreen;
               jsmpPixels[r][q+2] = pCTab[lpPixels[nPixel]].rgbBlue;
            }
         }
         break;

      case 16: //Hi-color (16 bits per pixel)
         for (r=0;r < pbBmHdr->biHeight; r++)
         {
            for (p=0,q=0; p < pbBmHdr->biWidth; p++,q+=3)
            {
               nRow    = (pbBmHdr->biHeight-r-1) * pbBmHdr->biWidth;
               nPixel  = nRow + p;

               RGBQUAD quad = QuadFromWord(wPixels[nPixel]);

               jsmpPixels[r][q+0] = quad.rgbRed;
               jsmpPixels[r][q+1] = quad.rgbGreen;
               jsmpPixels[r][q+2] = quad.rgbBlue;
            }
         }
         break;

      case 24:
         nBytesWide =  (pbBmHdr->biWidth*3);
         nUnused    =  (((nBytesWide + 3) / 4) * 4) -
                       nBytesWide;
         nBytesWide += nUnused;

         for (r=0;r < pbBmHdr->biHeight;r++)
         {
            for (p=0,q=0;p < (nBytesWide-nUnused); p+=3,q+=3)
            { 
               nRow = (pbBmHdr->biHeight-r-1) * nBytesWide;
               nPixel  = nRow + p;

               jsmpPixels[r][q+0] = lpPixels[nPixel+2]; //Red
               jsmpPixels[r][q+1] = lpPixels[nPixel+1]; //Green
               jsmpPixels[r][q+2] = lpPixels[nPixel+0]; //Blue
            }
         }
         break;*/

      case 32:

	     

		 BYTE * ptData = m_ciBitmap.GetPtBitmap();

		 RGBQUAD* pRgbQs   = (RGBQUAD*)ptData;

         for (r=0; r < m_ciBitmap.GetBitmapHeigth(); r++)
         {
            for (p=0,q=0; p < m_ciBitmap.GetBitmapWidth(); p++,q+=3)
            {
               nRow    = (m_ciBitmap.GetBitmapHeigth()-r-1) *  m_ciBitmap.GetBitmapWidth();
               nPixel  = nRow + p;

               jsmpPixels[r][q+0] = pRgbQs[nPixel].rgbRed;
               jsmpPixels[r][q+1] = pRgbQs[nPixel].rgbGreen;
               jsmpPixels[r][q+2] = pRgbQs[nPixel].rgbBlue;
			}
         }
         break;
   }   //end switch

return TRUE;
}
 

BOOL JpegFile::GetImageFromJPG(const char * csFileName, HBITMAP &hBitmap,const long &JPGThumbWidth, const long &JPGThumbHeight)
{
  int                  pad_bytes;
  int                  dib_line_size;
  int                  size;
  BOOL                 bres = TRUE;
  char                 comments[1024];
  IJLERR               jerr;
  JPEG_CORE_PROPERTIES jcprops;
  string m_color;
  string m_subsampling;
  BYTE * m_imageData = NULL;

	// Keep a local copy of the filename for usage with the IJL functions.
	// Note that since IJL does not support Unicode we must perform the
	// Unicode to Ascii conversion
	char lpszAsciiFileName[MAX_PATH];
	strcpy_s(lpszAsciiFileName, MAX_PATH, csFileName);

	ZeroMemory (&jcprops, sizeof(JPEG_CORE_PROPERTIES));


	try
	{

	comments[0] = 0;

	jerr = ijlInit(&jcprops);

	if(IJL_OK != jerr)
	{
		bres = FALSE;
		goto Exit;
	}

	jcprops.JPGFile = (LPTSTR) lpszAsciiFileName;

	jcprops.jprops.jpeg_comment      = &comments[0];
	jcprops.jprops.jpeg_comment_size = sizeof(comments);

	jerr = ijlRead(&jcprops,IJL_JFILE_READPARAMS);

	if(IJL_OK != jerr && IJL_ERR_COM_BUFFER != jerr)
	{
		bres = FALSE;
		goto Exit;
	}

	switch(jcprops.JPGColor)
	{
	case IJL_G:          m_color = "Gray";    break;
	case IJL_RGB:        m_color = "RGB";     break;
	case IJL_YCBCR:      m_color = "YCbCr";   break;
	case IJL_RGBA_FPX:   m_color = "CMYK";    break;
	case IJL_YCBCRA_FPX: m_color = "YCCK";    break;
	default:             m_color = "Unknown"; break;
	}

	switch(jcprops.JPGSubsampling)
	{
	case IJL_NONE: m_subsampling = "None";  break;
	case IJL_422:  m_subsampling = "422";   break;
	case IJL_411:  m_subsampling = "411";   break;
	case IJL_4224: m_subsampling = "4224";  break;
	case IJL_4114: m_subsampling = "4114";  break;
	default:       m_subsampling = "Other"; break;
	}

	if(JPGThumbWidth != 0 && JPGThumbHeight != 0)
	{
		jcprops.DIBWidth  =  JPGThumbWidth;
		jcprops.DIBHeight = JPGThumbHeight;

		pad_bytes = IJL_DIB_PAD_BYTES(jcprops.DIBWidth,3);
		dib_line_size = jcprops.DIBWidth * jcprops.DIBChannels + pad_bytes;
		size = dib_line_size * abs(jcprops.DIBHeight) + sizeof(BITMAPINFO);

		m_imageData = new BYTE [size];

		if(NULL == m_imageData)
		{
		bres = FALSE;
		goto Exit;
		}

		BYTE* buffer = m_imageData + sizeof(BITMAPINFO);

		jcprops.DIBBytes    = buffer;
		jcprops.DIBPadBytes = pad_bytes;

		jerr = ijlRead(&jcprops,IJL_JFILE_READTHUMBNAIL);

		if(IJL_OK != jerr)
		{
		bres = FALSE;
		goto Exit;
		}

		BITMAPINFO* bmi = (BITMAPINFO*)m_imageData;

		bmi->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
		bmi->bmiHeader.biWidth         = jcprops.DIBWidth;
		bmi->bmiHeader.biHeight        = abs(jcprops.DIBHeight);
		bmi->bmiHeader.biCompression   = BI_RGB;
		bmi->bmiHeader.biBitCount      = (jcprops.DIBChannels == 3) ? (WORD)24 : (WORD)32;
		bmi->bmiHeader.biPlanes        = 1;
		bmi->bmiHeader.biClrUsed       = 0;
		bmi->bmiHeader.biClrImportant  = 0;
		bmi->bmiHeader.biSizeImage     = 0;
		bmi->bmiHeader.biXPelsPerMeter = 0;
		bmi->bmiHeader.biYPelsPerMeter = 0;
		bmi->bmiColors[0].rgbRed       = 0;
		bmi->bmiColors[0].rgbGreen     = 0;
		bmi->bmiColors[0].rgbBlue      = 0;
		bmi->bmiColors[0].rgbReserved  = 0;


		hBitmap = CreateDIBitmap (GetDC(NULL),              
						&bmi->bmiHeader,
						CBM_INIT,
						m_imageData + sizeof(BITMAPINFO),
						(LPBITMAPINFO)bmi,
						DIB_RGB_COLORS) ;
	}
	else
	{
		jcprops.DIBWidth  =  IJL_DIB_SCALE_SIZE(jcprops.JPGWidth,8);
		jcprops.DIBHeight = -IJL_DIB_SCALE_SIZE(jcprops.JPGHeight,8);


		switch(jcprops.JPGChannels)
		{
		case 1:
		jcprops.DIBChannels = 3;
		jcprops.DIBColor    = IJL_BGR;
		break;

		case 3:
		jcprops.DIBChannels = 3;
		jcprops.DIBColor    = IJL_BGR;
		break;

		case 4:
		jcprops.DIBChannels = jcprops.JPGChannels;
		jcprops.DIBColor    = IJL_RGBA_FPX;
		break;
		}

		pad_bytes = IJL_DIB_PAD_BYTES(jcprops.DIBWidth,jcprops.DIBChannels);
		dib_line_size = jcprops.DIBWidth * jcprops.DIBChannels + pad_bytes;
		size = dib_line_size * abs(jcprops.DIBHeight) + sizeof(BITMAPINFO);

		m_imageData = new BYTE [size];

		if(NULL == m_imageData)
		{
		bres = FALSE;
		goto Exit;
		}

		BYTE* buffer = m_imageData + sizeof(BITMAPINFO);

		jcprops.DIBBytes    = buffer;
		jcprops.DIBPadBytes = pad_bytes;

		jerr = ijlRead(&jcprops,IJL_JFILE_READONEEIGHTH);

		if(IJL_OK != jerr)
		{
		bres = FALSE;
		goto Exit;
		}

		BITMAPINFO* bmi = (BITMAPINFO*)m_imageData;

		bmi->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
		bmi->bmiHeader.biWidth         = jcprops.DIBWidth;
		bmi->bmiHeader.biHeight        = abs(jcprops.DIBHeight);
		bmi->bmiHeader.biCompression   = BI_RGB;
		bmi->bmiHeader.biBitCount      = (jcprops.DIBChannels == 3) ? (WORD)24 : (WORD)32;
		bmi->bmiHeader.biPlanes        = 1;
		bmi->bmiHeader.biClrUsed       = 0;
		bmi->bmiHeader.biClrImportant  = 0;
		bmi->bmiHeader.biSizeImage     = 0;
		bmi->bmiHeader.biXPelsPerMeter = 0;
		bmi->bmiHeader.biYPelsPerMeter = 0;
		bmi->bmiColors[0].rgbRed       = 0;
		bmi->bmiColors[0].rgbGreen     = 0;
		bmi->bmiColors[0].rgbBlue      = 0;
		bmi->bmiColors[0].rgbReserved  = 0;

		hBitmap = CreateDIBitmap (GetDC(NULL),              
						&bmi->bmiHeader,
						CBM_INIT,
						buffer,
						(LPBITMAPINFO)bmi,
						DIB_RGB_COLORS) ;



	}




	}
	catch(...)
	{
	}

	Exit:

	ijlFree(&jcprops);

	delete[] m_imageData;
	m_imageData = NULL;

  return bres;
}

int JpegFile::ReadJpeg(CIBitmap * m_ciBitmap,const char * szFileName)
{
#if _DEBUG
	ReadJpeg_Intel(m_ciBitmap,szFileName);
	ReadJpeg_Std(m_ciBitmap,szFileName);
#else
	if(ReadJpeg_Intel(m_ciBitmap,szFileName) == -1)
		ReadJpeg_Std(m_ciBitmap,szFileName);

#endif

	return 0;
}

int JpegFile::ReadJpeg_Intel(CIBitmap * m_ciBitmap,const char * szFileName)
{

#if _DEBUG
	DWORD tickCount = GetTickCount();
#endif

	try
	{
		JPEG_CORE_PROPERTIES image;
		memset( &image, 0, sizeof( JPEG_CORE_PROPERTIES ) );
		BYTE* imageData;

		if( ijlInit( &image ) != IJL_OK )
		{
			return -1;
		}


		image.JPGFile = szFileName;
		if( ijlRead( &image, IJL_JFILE_READPARAMS ) != IJL_OK )
		{
			ijlFree( &image );
			return -1;
		}


		// !dudnik: to fix bug case 584680, [OT:287A305B]
		// Set the JPG color space ... this will always be
		// somewhat of an educated guess at best because JPEG
		// is "color blind" (i.e., nothing in the bit stream
		// tells you what color space the data was encoded from).
		// However, in this example we assume that we are
		// reading JFIF files which means that 3 channel images
		// are in the YCbCr color space and 1 channel images are
		// in the Y color space.
		switch(image.JPGChannels)
		{
		case 1:
		  image.JPGColor    = IJL_G;
		  image.DIBChannels = 3;
		  image.DIBColor    = IJL_BGR;
		  break;

		case 3:
		  image.JPGColor    = IJL_YCBCR;
		  image.DIBChannels = 3;
		  image.DIBColor    = IJL_BGR;
		  break;

		case 4:
		  image.JPGColor    = IJL_YCBCRA_FPX;
		  image.DIBChannels = 4;
		  image.DIBColor    = IJL_RGBA_FPX;
		  break;

		default:
		  // This catches everything else, but no
		  // color twist will be performed by the IJL.
		  image.DIBColor = (IJL_COLOR)IJL_OTHER;
		  image.JPGColor = (IJL_COLOR)IJL_OTHER;
		  image.DIBChannels = image.JPGChannels;
		  break;
		}

		image.DIBWidth    = image.JPGWidth;
		image.DIBHeight   = image.JPGHeight;
		image.DIBPadBytes = IJL_DIB_PAD_BYTES(image.DIBWidth,image.DIBChannels);

		int imageSize = (image.DIBWidth * image.DIBChannels + image.DIBPadBytes) *
		  image.DIBHeight;

		imageData = new BYTE[ imageSize ];
		if( imageData == NULL )
		{
			ijlFree( &image );
			return -1;
		}

		image.DIBBytes = imageData;

		if( ijlRead( &image, IJL_JFILE_READWHOLEIMAGE ) != IJL_OK )
		{
			ijlFree( &image );
			delete[] imageData;
			return -1;
		}


		if(image.DIBColor == IJL_RGBA_FPX)
		{
		  RGBA_FPX_to_BGRA(imageData,image.DIBWidth,image.DIBHeight);
		}


		// initializing incapsulated image with correct values
		

		switch(image.DIBChannels)
		{
		case 3:
			{
				m_ciBitmap->CreateBitmap(image.DIBWidth, image.DIBHeight);

				BYTE * outbuf = m_ciBitmap->GetPtBitmap();


				long lWidthSize = ((((image.DIBWidth * 24) + 31) & ~31) >> 3);
				long l = 0;

				//outbuf = (BYTE *)malloc(image.DIBWidth * image.DIBHeight * 5);
				//#pragma omp parallel 
				//{
				int * iTabK = new int[image.DIBWidth];
				int * iTabPos = new int[image.DIBWidth];
				int iPosK = 0;
				int iPos = 0;
				for(int x = 0;x < image.DIBWidth;x++)
				{
					iTabK[x] = iPosK;
					iTabPos[x] = iPos;
					iPosK+=4;
					iPos+=3;
				}

					for(int y = image.DIBHeight - 1,l = 0 ;y >= 0;y--, l++)
					{
						int iPos = y * lWidthSize;
						int k = image.DIBWidth * l * 4;
						for(int x = 0;x < image.DIBWidth;x++)
						{
							memcpy(outbuf+k+iTabK[x],imageData+iPos+iTabPos[x],3);
							//outbuf[k] = imageData[iPos];
							//outbuf[k+1] = imageData[iPos+1];
							//outbuf[k+2] = imageData[iPos+2];
							//k+=4;
							//iPos += 3;
						}
					}
				//}

				delete[] iTabK;
				delete[] iTabPos;
				m_ciBitmap->ReCreateHBitmap(false);
			}
		  break;

		case 4:

			CImage::VertFlipBuf(imageData,image.DIBWidth*4,image.DIBHeight);
			m_ciBitmap->SetBitmap(imageData,image.DIBWidth, image.DIBHeight);
		  break;

		default:
		  return -1;
		  break;
		}


		///Conversion de 24bits -> 32bits

		delete[] imageData;
		ijlFree(&image);

		// now we have
		//   m_imageData  containing image data, and
		//   m_imageDims  with image dimensions, and
		//   m_imageChannels with image number of channels


	}
	catch(...)
	{

	}

#if _DEBUG
	DWORD LasttickCount = GetTickCount();				// Get The Tick Count
	DWORD Result = LasttickCount - tickCount;

	char Temp[10];
	sprintf(Temp,"%d",Result);
	::MessageBox(NULL,Temp,"Temps d'execution",0);
#endif

	return 0;
}

int JpegFile::ReadJpeg_Std(CIBitmap * m_ciBitmap,const char * szFileName)
{

	///////////////////////////////////////////////////////////////
	//JPEG METHOD
	//Si erreur avec Intel JPEG librairie test avec lib standard
	///////////////////////////////////////////////////////////////


	try
	{

#if _DEBUG
	DWORD tickCount = GetTickCount();
#endif

			UINT nWidth, nHeigth;
			BYTE * m_buf = NULL, * imageData = NULL;
			UINT m_widthDW;

			m_buf= JpegFileToRGB(szFileName, &nWidth, &nHeigth);

			BYTE * outbuf;

			imageData = CImage::MakeDwordAlignedBuf(m_buf, nWidth, nHeigth, &m_widthDW);

			//RGBA_FPX_to_BGRA(imageData,nWidth,nHeigth);

			long lWidthSize = ((((nWidth * 24) + 31) & ~31) >> 3);
			long k = 0;

			outbuf = (BYTE *)malloc(nWidth * nHeigth * 4);

			/*
			for(int y = nHeigth - 1;y >= 0;y--)
			{
				int iPos = y * lWidthSize;
				for(int x = 0;x < nWidth;x++,k+=4,iPos += 3)
				{
					outbuf[k] = m_tmp[iPos+2];
					outbuf[k+1] = m_tmp[iPos+1];
					outbuf[k+2] = m_tmp[iPos];
					outbuf[k+3] = 0;
				}
			}*/

			int * iTabK = new int[nWidth];
			int * iTabPos = new int[nWidth];
			int iPosK = 0;
			int iPos = 0;
			for(int x = 0;x < nWidth;x++)
			{
				iTabK[x] = iPosK;
				iTabPos[x] = iPos;
				iPosK+=4;
				iPos+=3;
			}

			for(int y = nHeigth - 1,l = 0 ;y >= 0;y--, l++)
			{
				int iPos = y * lWidthSize;
				int k = nWidth * l * 4;
				for(int x = 0;x < nWidth;x++)
				{
					outbuf[k+iTabK[x]] = imageData[iPos+2+iTabPos[x]];
					outbuf[k+1+iTabK[x]] = imageData[iPos+1+iTabPos[x]];
					outbuf[k+2+iTabK[x]] = imageData[iPos+iTabPos[x]];
				}
			}

			m_ciBitmap->SetBitmap(outbuf,nWidth, nHeigth);
			free(outbuf);


			delete[] m_buf;
			delete[] imageData;

#if _DEBUG
	DWORD LasttickCount = GetTickCount();				// Get The Tick Count
	DWORD Result = LasttickCount - tickCount;

	char Temp[10];
	sprintf(Temp,"%d",Result);
	::MessageBox(NULL,Temp,"Temps d'execution",0);
#endif

	}
	catch(...)
	{
	}

	return 0;
}


void JpegFile::RGBA_FPX_to_BGRA(BYTE* data,int width,int height)
{
  int   i;
  int   j;
  int   pad;
  int   line_width;
  BYTE  r, g, b, a;
  BYTE* ptr;

  ptr = data;
  pad = IJL_DIB_PAD_BYTES(width,4);
  line_width = width * 4 + pad;

  for(i = 0; i < height; i++)
  {
    ptr = data + line_width*i;
    for(j = 0; j < width; j++)
    {
      r = ptr[0];
      g = ptr[1];
      b = ptr[2];
      a = ptr[3];
      ptr[2] = (r*a+1) >> 8;
      ptr[1] = (g*a+1) >> 8;
      ptr[0] = (b*a+1) >> 8;
      ptr += 4;
    }
  }

  return;
} // CJPGViewDoc::RGBA_FPX_to_BGRA()