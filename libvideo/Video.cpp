#include "stdafx.h"
#include ".\video.h"
#include "Thumbnail.h"

ID2D1HwndRenderTarget * CVideo::g_pRT = NULL;
extern HWND mainWindow;

CVideo::CVideo(void)
{
	HRESULT hr;
    
    //hr = CoCreateInstance( CLSID_MediaDet, NULL, CLSCTX_INPROC_SERVER, 
    //                       IID_IMediaDet, (void**) &pDet );
}

CVideo::~CVideo(void)
{
}

void CVideo::Cleanup()
{
	SafeRelease(&g_pRT);
}

//-------------------------------------------------------------------
// CreateDrawingResources: Creates Direct2D resources.
//-------------------------------------------------------------------

HRESULT CVideo::CreateDrawingResources(HWND hwnd)
{
	HRESULT hr = S_OK;
	RECT rcClient = { 0 };

	ID2D1Factory *pFactory = NULL;
	ID2D1HwndRenderTarget *pRenderTarget = NULL;

	GetClientRect(hwnd, &rcClient);

	hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&pFactory
	);


	if (SUCCEEDED(hr))
	{
		hr = pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(
				hwnd,
				D2D1::SizeU(rcClient.right, rcClient.bottom)
			),
			&pRenderTarget
		);
	}

	if (SUCCEEDED(hr))
	{
		g_pRT = pRenderTarget;
		g_pRT->AddRef();
	}

	SafeRelease(&pFactory);
	SafeRelease(&pRenderTarget);
	return hr;
}

////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////

HBITMAP CVideo::DoExtractVideoFrame(const char *szFileName,long &Width, long &Height)
{
	HBITMAP hBitmap;
	ThumbnailGenerator thumbnail;
	Sprite  g_pSprites[1];

	const WCHAR *pwcsName;
	// required size
	int nChars = MultiByteToWideChar(CP_ACP, 0, szFileName, -1, NULL, 0);
	// allocate it
	pwcsName = new WCHAR[nChars];
	MultiByteToWideChar(CP_ACP, 0, szFileName, -1, (LPWSTR)pwcsName, nChars);
	// use it....
	HRESULT hr = thumbnail.OpenFile(pwcsName);
	if (FAILED(hr))
	{
		return NULL;
	}

	if (g_pRT == NULL)
	{
		// Create the Direct2D resources.
		hr = CreateDrawingResources(mainWindow);
	}

	// Generate new sprites.
	if (SUCCEEDED(hr))
	{
		assert(g_pRT != NULL);

		hr = thumbnail.CreateBitmaps(g_pRT, 1, g_pSprites);
	}

	if (SUCCEEDED(hr))
	{
		hBitmap = g_pSprites->GetBitmap();
	}

	// delete it
	delete[] pwcsName;

	g_pSprites[0].Clear();
	
	return hBitmap;
}

HRESULT CVideo::GetVideoSize(const char *szFileName,long &Width, long &Height)
{
	HBITMAP hBitmap;
	ThumbnailGenerator thumbnail;
	Sprite  g_pSprites[1];

	const WCHAR *pwcsName;
	// required size
	int nChars = MultiByteToWideChar(CP_ACP, 0, szFileName, -1, NULL, 0);
	// allocate it
	pwcsName = new WCHAR[nChars];
	MultiByteToWideChar(CP_ACP, 0, szFileName, -1, (LPWSTR)pwcsName, nChars);
	// use it....
	HRESULT hr = thumbnail.OpenFile(pwcsName);
	if (FAILED(hr))
	{
		return NULL;
	}

	if (g_pRT == NULL)
	{
		// Create the Direct2D resources.
		hr = CreateDrawingResources(mainWindow);
	}

	// Generate new sprites.
	if (SUCCEEDED(hr))
	{
		assert(g_pRT != NULL);

		hr = thumbnail.CreateBitmaps(g_pRT, 1, g_pSprites);
	}

	// Generate new sprites.
	if (SUCCEEDED(hr))
	{
		Width = g_pSprites[0].m_width;
		Height = g_pSprites[0].m_height;
	}

	// delete it
	delete[] pwcsName;

	g_pSprites[0].Clear();

    return 0;
}

// Create the filter graph to render the video and output each frame to 
// bmp files in the range of (start, end)
//   pVideoName : video file name;
//   pBmpName   : output bmp file names, e.g. Img%2.2d.bmp -> 
//                Img00.bmp, Img01.bmp, ...
//   start      : Starting frame number
//   end        : Ending frame number
//   dFrameRate : Framerate of output images
//
HRESULT CVideo::VideoToBmp(const char * pVideoName, const char * pBmpName, double start, int iWidth, int iHeight)
{
	HBITMAP hBitmap;
	ThumbnailGenerator thumbnail;
	Sprite  g_pSprites[1];

	const WCHAR *pwcsName;
	// required size
	int nChars = MultiByteToWideChar(CP_ACP, 0, pVideoName, -1, NULL, 0);
	// allocate it
	pwcsName = new WCHAR[nChars];
	MultiByteToWideChar(CP_ACP, 0, pVideoName, -1, (LPWSTR)pwcsName, nChars);
	// use it....
	HRESULT hr = thumbnail.OpenFile(pwcsName);
	if (FAILED(hr))
	{
		return NULL;
	}

	if (g_pRT == NULL)
	{
		// Create the Direct2D resources.
		hr = CreateDrawingResources(mainWindow);
	}

	// Generate new sprites.
	if (SUCCEEDED(hr))
	{
		assert(g_pRT != NULL);

		hr = thumbnail.CreateBitmaps(g_pRT, 1, g_pSprites);
	}

	if (SUCCEEDED(hr))
	{
		hBitmap = g_pSprites->GetBitmap();

		HDC hDC = GetDC(nullptr);
		WORD wBitCount;
		DWORD dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
		BITMAP Bitmap0;
		BITMAPFILEHEADER bmfHdr;
		BITMAPINFOHEADER bi;
		LPBITMAPINFOHEADER lpbi;
		HANDLE fh, hDib, hPal, hOldPal2 = nullptr;
		wBitCount = 24;
		GetObject(hBitmap, sizeof(Bitmap0), (LPSTR)&Bitmap0);
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = Bitmap0.bmWidth;
		bi.biHeight = -Bitmap0.bmHeight;
		bi.biPlanes = 1;
		bi.biBitCount = wBitCount;
		bi.biCompression = BI_RGB;
		bi.biSizeImage = 0;
		bi.biXPelsPerMeter = 0;
		bi.biYPelsPerMeter = 0;
		bi.biClrImportant = 0;
		bi.biClrUsed = 256;
		dwBmBitsSize = ((Bitmap0.bmWidth * wBitCount + 31) & ~31) / 8
			* Bitmap0.bmHeight;
		hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
		lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
		*lpbi = bi;

		hPal = GetStockObject(DEFAULT_PALETTE);
		if (hPal)
		{
			hDC = GetDC(nullptr);
			hOldPal2 = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
			RealizePalette(hDC);
		}


		GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap0.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
			+ dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);

		if (hOldPal2)
		{
			SelectPalette(hDC, (HPALETTE)hOldPal2, TRUE);
			RealizePalette(hDC);
			ReleaseDC(nullptr, hDC);
		}

		fh = CreateFile(pBmpName, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);

		if (fh == INVALID_HANDLE_VALUE)
			return NULL;

		bmfHdr.bfType = 0x4D42; // "BM"
		dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
		bmfHdr.bfSize = dwDIBSize;
		bmfHdr.bfReserved1 = 0;
		bmfHdr.bfReserved2 = 0;
		bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

		::WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, nullptr);

		::WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, nullptr);
		GlobalUnlock(hDib);
		GlobalFree(hDib);
		CloseHandle(fh);
	}

	// delete it
	delete[] pwcsName;

	g_pSprites[0].Clear();


	return 0;
}

// This function create a video file based on given still image sequence
//   firstImage : the file name of the first image
//   outAvi     : output video name
//   length     : the frame number of the output video
//   dFrameRate  : the frame rate of the video
//
