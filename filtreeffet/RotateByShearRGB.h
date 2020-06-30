#ifndef __ROTATEBYSHEARRGB__H__
#define __ROTATEBYSHEARRGB__H__

#include "RotateByShear.h"


//
// Sample implementation :
// -----------------------
// This implementation uses COLORREF for a pixel.
// This implementation fits the DIB data structure.
//
class CRotateByShearRGB : public CRotateByShear<COLORREF>
{
public: 

	static BOOL Progress (BYTE bPercentComplete)
	{
		//printf ("%d percent completed...\r", bPercentComplete);
		return TRUE;
		// If we return FALSE here, the rotation will be immediately aborted
	}

    CRotateByShearRGB (ProgressAnbAbortCallBack callback = NULL) : 
        CRotateByShear<COLORREF> (callback) {}

    virtual ~CRotateByShearRGB() {}

    COLORREF GetRGB (COLORREF *pImage,      // Pointer to image
                     SIZE  sImage,      // Size of image
                     UINT  x,           // X coordinate
                     UINT  y            // Y coordinate
                    )
    {
        return pImage [x + y * sImage.cx];
    }

        // Set RGB value at given pixel coordinates
    void SetRGB (COLORREF  *pImage,   // Pointer to image
                 SIZE       sImage,   // Size of image
                 UINT       x,        // X coordinate
                 UINT       y,        // Y coordinate
                 COLORREF   clr       // New color to set
                )
    {
        pImage [x + y * sImage.cx] = clr;
    }

        // Create a new bitmap, given a bitmap dimensions
    COLORREF *CreateNewBitmap (SIZE  sImage) // Size of image
    {
        return new COLORREF [sImage.cx * sImage.cy];
    }

        // Create a new bitmap which is an identical copy of the source bitmap
    COLORREF *CopyBitmap (COLORREF *pImage,     // Pointer to source image
                          SIZE      sImage      // Size of source (and destination) image
                         )
    {
        COLORREF *pDst =  CreateNewBitmap (sImage);
        if (NULL != pDst)
        {
			memcpy(pDst, pImage, sizeof (COLORREF) * sImage.cx * sImage.cy);
        }
        return pDst;
    }          

        // Destroy a bitmap previously created in call to CreateNewBitmap(..)
        // or to CopyBitmap (...)
    void DestroyBitmap (COLORREF *pImage,   // Pointer to image
                        SIZE                // Size of image
                       )
    {
        delete [] pImage;
    }

};

#endif