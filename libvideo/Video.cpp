#include "stdafx.h"
#include ".\video.h"
#include <Qedit.h>
#include ".\grabbercallback.h"


CVideo::CVideo(void)
{
	HRESULT hr;
    
    //hr = CoCreateInstance( CLSID_MediaDet, NULL, CLSCTX_INPROC_SERVER, 
    //                       IID_IMediaDet, (void**) &pDet );
}

CVideo::~CVideo(void)
{
}

void CVideo::MyFreeMediaType(AM_MEDIA_TYPE& mt)
{
    if (mt.cbFormat != 0)
    {
        CoTaskMemFree((PVOID)mt.pbFormat);
        mt.cbFormat = 0;
        mt.pbFormat = NULL;
    }
    if (mt.pUnk != NULL)
    {
        // Unecessary because pUnk should not be used, but safest.
        mt.pUnk->Release();
        mt.pUnk = NULL;
    }
}

////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////

HBITMAP CVideo::DoExtractVideoFrame(const char *szFileName,long &Width, long &Height)
{

	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CComPtr< IMediaDet > pDet;
	CComPtr< ISampleGrabber > pGrabber;
	USES_CONVERSION;
	HBITMAP hBitmap;
	try
	{
		HRESULT hr;
		WCHAR wFile[MAX_PATH];
		MultiByteToWideChar( CP_ACP, 0, szFileName, -1, wFile, MAX_PATH );

		hr = CoCreateInstance( CLSID_MediaDet, NULL, CLSCTX_INPROC_SERVER, 
		                       IID_IMediaDet, (void**) &pDet );

		if (FAILED(hr))
			return NULL;
		
		

		long Streams = 0;
		BOOL bFoundVideo = FALSE;

		if(pDet == NULL)
			return NULL;

		hr = pDet->put_Filename( T2W( szFileName ));
		if (FAILED(hr))
		{
			pDet.Release();
			return NULL;
		}

		hr = pDet->get_OutputStreams( &Streams );
		if (FAILED(hr))
		{
			pDet.Release();
			return NULL;
		}

		hr = pDet->EnterBitmapGrabMode( 0.0 );
		if (FAILED(hr))
		{
			pDet.Release();
			return NULL;
		}

		hr = pDet->GetSampleGrabber( &pGrabber );
		if (FAILED(hr))
		{
			pDet.Release();
			return NULL;
		}

		// set the callback (our COM object callback)
		//
		CComQIPtr< IBaseFilter, &IID_IBaseFilter > pFilter( pGrabber );
		if(pFilter == NULL)
		{
			pGrabber.Release();
			pDet.Release();
			return NULL;
		}


		// Find the current bit depth.
		HDC hdc = GetDC(NULL);
		int iBitDepth = GetDeviceCaps(hdc, BITSPIXEL);
		ReleaseDC(NULL, hdc);

		AM_MEDIA_TYPE mt;
		ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
		mt.majortype = MEDIATYPE_Video;
		mt.formattype = FORMAT_VideoInfo; 

		// Set the media type.
		mt.majortype = MEDIATYPE_Video;
		switch (iBitDepth)
		{
		case 8:
			mt.subtype = MEDIASUBTYPE_RGB8;
			break;
		case 16:
			mt.subtype = MEDIASUBTYPE_RGB555;
			break;
		case 24:
			mt.subtype = MEDIASUBTYPE_RGB24;
			break;
		case 32:
			mt.subtype = MEDIASUBTYPE_RGB32;
			break;
		default:
			{
				pGrabber.Release();
				pDet.Release();
				return NULL;
			}
		}


		hr = pGrabber->SetMediaType(&mt);        
		if (FAILED(hr))
			{
				pGrabber.Release();
				pDet.Release();
				return NULL;
			}

		// Set up one-shot mode.
		hr = pGrabber->SetBufferSamples(TRUE);
		if (FAILED(hr))
			{
				pGrabber.Release();
				pDet.Release();
				return NULL;
			}

		hr = pGrabber->SetOneShot(TRUE);
		if (FAILED(hr))
			{
				pGrabber.Release();
				pDet.Release();
				return NULL;
			}

		
		// The graph will have been paused by entering bitmap grab mode.
		// We'll need to seek back to 0 to get it to deliver correctly.
		//

		FILTER_INFO fi;
		memset( &fi, 0, sizeof( fi ) );
		hr = pFilter->QueryFilterInfo( &fi );
		if (FAILED(hr))
			{
				pGrabber.Release();
				pDet.Release();
				return NULL;
			}
		// Release the filter's graph reference
		if( fi.pGraph ) 
			fi.pGraph->Release( );

		if(fi.pGraph != NULL)
		{
			IFilterGraph * pGraph = fi.pGraph;

			/*
			CComQIPtr< IMediaSeeking, &IID_IMediaSeeking > pSeeking( pGraph );
			if (pSeeking == NULL)
			{
				pGrabber.Release();
				pDet.Release();
				return NULL;
			}

			REFERENCE_TIME Start = 60;
			REFERENCE_TIME Duration = 30;
			hr = pSeeking->GetDuration( &Duration );
			if (FAILED(hr))
			{
				pGrabber.Release();
				pDet.Release();
				return NULL;
			}
			hr = pSeeking->SetPositions( &Start,    AM_SEEKING_AbsolutePositioning, 
										 &Duration, AM_SEEKING_AbsolutePositioning );
			if (FAILED(hr))
			{
				pGrabber.Release();
				pDet.Release();
				return NULL;
			}
			*/

			// run the graph
			//
			CComQIPtr< IMediaEvent, &IID_IMediaEvent > pEvent( pGraph );
			CComQIPtr< IMediaControl, &IID_IMediaControl > pControl( pGraph );
			hr = pControl->Run( );
			if (FAILED(hr))
			{
				pGrabber.Release();
				pDet.Release();
				return NULL;
			}

			long evCode;
			hr = pEvent->WaitForCompletion(INFINITE, &evCode);
			if (FAILED(hr))
			{
				pGrabber.Release();
				pDet.Release();
				return NULL;
			}
						
			// Find the required buffer size.
			long cbBuffer = 0;
			hr = pGrabber->GetCurrentBuffer(&cbBuffer, NULL);

			char *pBuffer = new char[cbBuffer];
			if (!pBuffer) 
			{
				pGrabber.Release();
				pDet.Release();
				return NULL;
			}
			hr = pGrabber->GetCurrentBuffer(&cbBuffer, (long*)pBuffer);
			if (FAILED(hr))
			{
				pGrabber.Release();
				pDet.Release();
				delete[] pBuffer;
				return NULL;
			}
			//AM_MEDIA_TYPE mt;

			hr = pGrabber->GetConnectedMediaType(&mt);
			if (FAILED(hr))
			{
				pGrabber.Release();
				pDet.Release();
				delete[] pBuffer;
				return NULL;
			}
			// Examine the format block.
			VIDEOINFOHEADER *pVih;
			if ((mt.formattype == FORMAT_VideoInfo) && 
				(mt.cbFormat >= sizeof(VIDEOINFOHEADER)) &&
				(mt.pbFormat != NULL) ) 
			{
				pVih = (VIDEOINFOHEADER*)mt.pbFormat;
			}
			else 
			{
				// Wrong format. Free the format block and return an error.
				MyFreeMediaType(mt);
				pGrabber.Release();
				pDet.Release();
				delete[] pBuffer;
				return NULL; 
			}

			hBitmap = CreateDIBitmap(GetDC(NULL),              
						&pVih->bmiHeader,
						CBM_INIT,
						pBuffer,
						(BITMAPINFO*)&pVih->bmiHeader,
						DIB_RGB_COLORS) ;


			// Free the format block when you are done:
			MyFreeMediaType(mt);
			delete[] pBuffer;
		}
	}
	catch(...)
	{
		pGrabber.Release();
		pDet.Release();
		return NULL;
	}

	pGrabber.Release();
	pDet.Release();
	CoUninitialize();
	return hBitmap;
}

HRESULT CVideo::GetVideoSize(const char *szFileName,long &Width, long &Height)
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CComPtr< IMediaDet > pDet;
    USES_CONVERSION;
    HRESULT hr;

	hr = CoCreateInstance( CLSID_MediaDet, NULL, CLSCTX_INPROC_SERVER, 
	                       IID_IMediaDet, (void**) &pDet );

	if (FAILED(hr))
		return NULL;

    //printf("Frame extraction from %s.\r\n", pVideoName);

    // create a media detector
    //

	if(pDet == NULL)
		return -1;

    // set filename and look for a video stream
    //
    long Streams = 0;
    BOOL bFoundVideo = FALSE;
    hr = pDet->put_Filename( T2W( szFileName ));
	if (FAILED(hr))
		return NULL;

    hr = pDet->get_OutputStreams( &Streams );
	if (FAILED(hr))
		return NULL;

	bFoundVideo = TRUE;

    if( !bFoundVideo )
    {
        //printf( "Couldn't find a video stream\r\n" );
        return -1;
    }

    // this method will change the MediaDet to go into 
    // "sample grabbing mode" at time 0.
    //
    hr = pDet->EnterBitmapGrabMode( 0.0 );
	if (FAILED(hr))
		return NULL;
    // ask for the sample grabber filter that we know lives inside the
    // graph made by the MediaDet
    //
    CComPtr< ISampleGrabber > pGrabber;
    hr = pDet->GetSampleGrabber( &pGrabber );

	if(pGrabber == NULL)
		return 0;

    // set the callback (our COM object callback)
    //
    //CComQIPtr< IBaseFilter, &IID_IBaseFilter > pFilter( pGrabber );

    // ask for the connection media type so we know how big the images
    // are, so we can write out bitmaps
    //
    AM_MEDIA_TYPE mt;
    hr = pGrabber->GetConnectedMediaType( &mt );
    
    VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*) mt.pbFormat;
    Width    = vih->bmiHeader.biWidth;
    Height   = vih->bmiHeader.biHeight;

    // Set the output bmp file names and output range
    //
    MyFreeMediaType( mt );

	pGrabber.Release();
	pDet.Release();

	CoUninitialize();
    //printf("Frame extraction complete.\r\n");
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
HRESULT CVideo::VideoToBmp( const char * pVideoName, const char * pBmpName, double start, int iWidth, int iHeight)
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CComPtr< IMediaDet > pDet;
	CComPtr< ISampleGrabber > pGrabber;
	USES_CONVERSION;
	HBITMAP hBitmap;
	try
	{
		HRESULT hr;

		hr = CoCreateInstance( CLSID_MediaDet, NULL, CLSCTX_INPROC_SERVER, 
		                       IID_IMediaDet, (void**) &pDet );

		if (FAILED(hr))
			return NULL;
		
		

		long Streams = 0;
		BOOL bFoundVideo = FALSE;

		if(pDet == NULL)
			return NULL;

		hr = pDet->put_Filename( T2W( pVideoName ));
		if (FAILED(hr))
		{
			pDet.Release();
			return NULL;
		}

		hr = pDet->get_OutputStreams( &Streams );
		if (FAILED(hr))
		{
			pDet.Release();
			return NULL;
		}
		hr = pDet->WriteBitmapBits(start,iWidth,iHeight,T2W(pBmpName));
	}
	catch(...)
	{
	}

	pDet.Release();
	CoUninitialize();
	return 0;
}

// This function create a video file based on given still image sequence
//   firstImage : the file name of the first image
//   outAvi     : output video name
//   length     : the frame number of the output video
//   dFrameRate  : the frame rate of the video
//
HRESULT CVideo::BmpToVideo( const char * firstImage, const char * outAvi, int length, double dFrameRate)
{
    HRESULT hr;
	CComPtr< IMediaDet > pDet;
    // Start by making an empty timeline.
    //
    IAMTimeline    *pTL = NULL;
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    CoCreateInstance(CLSID_AMTimeline, NULL, CLSCTX_INPROC_SERVER, 
        IID_IAMTimeline, (void**)&pTL);

    // GROUP: Add a video group to the timeline. 
    //
    IAMTimelineGroup    *pGroup = NULL;
    IAMTimelineObj      *pGroupObj = NULL;
    pTL->CreateEmptyNode(&pGroupObj, TIMELINE_MAJOR_TYPE_GROUP);
    pGroupObj->QueryInterface(IID_IAMTimelineGroup, (void **)&pGroup);

	if(pDet == NULL)
		return -1;
    // set group properties (width and height to the image width and height)
    //
    hr = pDet->put_Filename( T2BSTR(firstImage) );
    AM_MEDIA_TYPE mtGroup;
    hr = pDet->get_StreamMediaType(&mtGroup);
    // Now use this media type for the group.
    hr = pGroup->SetMediaType(&mtGroup);
    hr = pGroup->SetOutputFPS(dFrameRate);

    hr = pTL->AddGroup(pGroupObj);
    pGroupObj->Release();

    // TRACK: Add a track to the group. 

    IAMTimelineObj      *pTrackObj;
    IAMTimelineTrack    *pTrack;
    IAMTimelineComp     *pComp = NULL;

    hr = pTL->CreateEmptyNode(&pTrackObj, TIMELINE_MAJOR_TYPE_TRACK);
    hr = pGroup->QueryInterface(IID_IAMTimelineComp, (void **)&pComp);
    hr = pComp->VTrackInsBefore(pTrackObj, 0);
    hr = pTrackObj->QueryInterface(IID_IAMTimelineTrack, (void **)&pTrack);

    pTrackObj->Release();
    pComp->Release();    
    pGroup->Release();

    // SOURCE: Add a source to the track.

    IAMTimelineSrc *pSource = NULL;
    IAMTimelineObj *pSourceObj;
    hr = pTL->CreateEmptyNode(&pSourceObj, TIMELINE_MAJOR_TYPE_SOURCE);
    hr = pSourceObj->QueryInterface(IID_IAMTimelineSrc, (void **)&pSource);

    // Set the times and the file name.
    REFERENCE_TIME duration = REFERENCE_TIME(10000000. * length / dFrameRate);
    hr = pSourceObj->SetStartStop(0, duration);
    hr = pSource->SetMediaName( T2BSTR(firstImage));
    hr = pSource->SetDefaultFPS(dFrameRate);
    hr = pTrack->SrcAdd(pSourceObj);

    pSourceObj->Release();
    pSource->Release();
    pTrack->Release();

    // Preview the timeline.
    IRenderEngine       *pRenderEngine = NULL;
    CoCreateInstance(CLSID_RenderEngine, NULL, CLSCTX_INPROC_SERVER,
        IID_IRenderEngine, (void**) &pRenderEngine);

    ICaptureGraphBuilder2 *pBuilder = NULL; 
    IGraphBuilder         *pGraph = NULL;
    IMediaControl         *pControl = NULL;
    IMediaEvent           *pEvent = NULL;

    // Build the graph.
    hr = pRenderEngine->SetTimelineObject(pTL);
    hr = pRenderEngine->ConnectFrontEnd();

    if (outAvi) {  // capture to file
        hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC, 
            IID_ICaptureGraphBuilder2, (void **)&pBuilder);

        // Get a pointer to the graph front end.
        hr = pRenderEngine->GetFilterGraph(&pGraph);
        hr = pBuilder->SetFiltergraph(pGraph);

        // Create the file-writing section.
        IBaseFilter *pMux;
        hr = pBuilder->SetOutputFileName(&MEDIASUBTYPE_Avi, 
            T2BSTR(outAvi), &pMux, NULL);

        long NumGroups;
        hr = pTL->GetGroupCount(&NumGroups);

        // Loop through the groups and get the output pins.
        for (int i = 0; i < NumGroups; i++)
        {
            IPin *pPin;
            if (pRenderEngine->GetGroupOutputPin(i, &pPin) == S_OK) 
            {
                // Connect the pin.
                hr = pBuilder->RenderStream(NULL, NULL, pPin, NULL, pMux);
                pPin->Release();
            }
        }
    }
    else { // preview on screen
        hr = pRenderEngine->RenderOutputPins( );
        hr = pRenderEngine->GetFilterGraph(&pGraph);
    }

    // Run the graph.
    hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
    hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);
    hr = pControl->Run();
    
    // wait and print out the progress
    //
    CComQIPtr< IMediaSeeking, &IID_IMediaSeeking > pSeeking( pGraph );
    long evCode = 0;
    if (pSeeking)  // IMediaSeeking is supported
    {
        REFERENCE_TIME rtTotal, rtNow = 0;
        hr = pSeeking->GetDuration(&rtTotal);
        while ((pEvent->WaitForCompletion(500, &evCode)) == E_ABORT)
        {
            hr = pSeeking->GetCurrentPosition(&rtNow);
            //printf("%d%% finished\n", (rtNow * 100)/rtTotal);
        }
    }
    else  // Cannot update the progress.
    {
        pEvent->WaitForCompletion(INFINITE, &evCode);
    }

    // Clean up.
    pEvent->Release();
    pControl->Release();
    pGraph->Release();

    // Clean up.
    pRenderEngine->ScrapIt();
    pTL->ClearAllGroups();
    pRenderEngine->Release();
    pTL->Release();
   CoUninitialize();

    return S_OK;
}