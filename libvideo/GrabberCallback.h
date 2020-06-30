

#include <atlbase.h>
#include <atlcom.h>
#include <atlhost.h>
#include <dShow.h>
#include "Qedit.h"

// This semi-COM object is a callback COM object for the sample grabber
// It's SampleCB() will be called on each frame, So we save the image
// frame to a bmp file in the SampleCB().
//
class CGrabberCallback : public ISampleGrabberCB 
{
protected: // attributes control the frames to save
    long   m_lStart;           // starting frame to save
    long   m_lEnd;             // ending frame to save
    double m_dFrameRate;       // Framerate of the output
    char   m_szBmpName[256];  // bitmap file name e.g. Test%3.3d.bmp
    long   counter;
    double dPreTime;
public:  // These parameters can not be get from the IMediaSample
         // it have to be passed in when assigning this callback to a 
         // ISampleGrabber Filter
    int  m_nWidth;           // image width
    int  m_nHeight;          // image height
    int  m_nBitCount;        // image bit per pixel
	bool m_bEnd;
public:
    // Initialize the Callback member
    //
    CGrabberCallback() : m_lStart(0), m_lEnd(0)
    {
        // No image file names to save
        memset(m_szBmpName, 0, sizeof(m_szBmpName));
        m_dFrameRate = 30;
		counter = 0;
		dPreTime = -1000;
		m_bEnd = false;
    }

    // Set output bitmap file names and range 
    //   pBmpName  : output file names (e.g. test%4.4d.bmp)
    //   start     : starting frame to save
    //   end       : ending frame to save (0 if to the end of the video)
    //   dFrameRate: framerate of output
    //
    void SetBitmapOutput(const char *pBmpName, int start, int end, double dFrameRate = 30)
    {
        strcpy_s(m_szBmpName, 256, pBmpName);
        m_lStart     = start;
        m_lEnd       = end;
        m_dFrameRate = dFrameRate;
    }

    // Callback function on each frame with the media time & data
    //
    STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample )
    {       
        char szFileName[MAX_PATH];

        // Is output filename set and current frame in the output range?
        // and correct frame rate
        // Note: the framerate will not be arbitrary but rounded to
        // the video framerate / (integer)k
        if (strlen(m_szBmpName) > 0 && !m_bEnd && SampleTime == m_lStart)
        {
            // Get the image buffer
            BYTE * pBuffer;
            pSample->GetPointer(&pBuffer);

            // Figure out where to save the image
            sprintf_s(szFileName,MAX_PATH, m_szBmpName, counter);
            // printf("Out put to file %s\n", szFileName);

            // Save to Bitmap file
            SaveBitmap( szFileName, m_nWidth, m_nHeight, m_nBitCount, pBuffer, pSample->GetActualDataLength() );

            // update the previous save image time
            dPreTime = SampleTime;
			m_bEnd = true;
			return 1;
        }

        counter++;
        return 0;
    }

    // Captured video data onto a bitmap on the user's disk.
    //     szBmpName   : bitmap file name
    //     w           : image width
    //     h           : image height
    //     c           : bit per pixel
    //     pBuffer     : image data
    //     lBufferSize : length of the image data
    //
    BOOL SaveBitmap( char* szBmpName, int w, int h, int c, BYTE * pBuffer, long lBufferSize )
    {
        // write out a BMP file
        HANDLE hf = CreateFile(
            szBmpName, GENERIC_WRITE, 0, NULL,
            CREATE_ALWAYS, NULL, NULL );

        if( hf == INVALID_HANDLE_VALUE )
            return 0;

        // write out the file header
        BITMAPFILEHEADER bfh;
        memset( &bfh, 0, sizeof( bfh ) );
        bfh.bfType = 'MB';
        bfh.bfSize = sizeof( bfh ) + lBufferSize + sizeof( BITMAPINFOHEADER );
        bfh.bfOffBits = sizeof( BITMAPINFOHEADER ) + sizeof( BITMAPFILEHEADER );

        DWORD dwWritten = 0;
        WriteFile( hf, &bfh, sizeof( bfh ), &dwWritten, NULL );

        // and the bitmap format
        BITMAPINFOHEADER bih;
        memset( &bih, 0, sizeof( bih ) );
        bih.biSize = sizeof( bih );
        bih.biWidth    = w;
        bih.biHeight   = h;
        bih.biBitCount = c;
        bih.biPlanes   = 1;

        dwWritten = 0;
        WriteFile( hf, &bih, sizeof( bih ), &dwWritten, NULL );

        // and the bits themselves
        dwWritten = 0;
        WriteFile( hf, pBuffer, lBufferSize, &dwWritten, NULL );
        CloseHandle( hf );

        return 0;
    }

    STDMETHODIMP BufferCB( double SampleTime, BYTE * pBuffer, long BufferLen )
    {
        return 0;
    }

    // Faked COM interface
    //
    STDMETHODIMP_(ULONG) AddRef() { return 2; }
    STDMETHODIMP_(ULONG) Release() { return 1; }

    STDMETHODIMP QueryInterface(REFIID riid, void ** ppv)
    {
        if (riid == IID_ISampleGrabberCB || riid == IID_IUnknown) 
        {
            *ppv = (void *) static_cast<ISampleGrabberCB *>(this);
            return NOERROR;
        }    
        return E_NOINTERFACE;
    }
};
