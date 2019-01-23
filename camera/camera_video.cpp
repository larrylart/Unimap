////////////////////////////////////////////////////////////////////
// Name:		implementation of the Video Camera class 
// File:		camera_video.cpp
// Purpose:		camera system control methods
//
// Created by:	Larry Lart on 10/02/2004
// Updated by:	Larry Lart on 10/09/2009
//
// Copyright:	(c) 2004-2009 Larry Lart
////////////////////////////////////////////////////////////////////

// system includes
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <windows.h>
//#include <streams.h>

#include <vfw.h>

// opencv includes
#include "_highgui.h"
#include "highgui.h"
#include "cv.h"
#include "cvaux.h"
//#include "videoInput.h"

// wx includes
//precompiled headers
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif 

// other local includes
#include "../util/func.h"
#include "../observer/observer_hardware.h"
#include "camera.h"
//#include "../worker.h"
#include "../logger/logger.h"
//#include "../gui/camera_view.h"
#include "../gui/frame.h"

// Main header
#include "camera_video.h"

#ifndef _USE_WIN
        void testcallback(void* _image);
        IplImage* m_pLnxFrame;
#endif
                                                                                                                             
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CCameraVideo
// Purose:	build my Camera object  
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CCameraVideo::CCameraVideo(  )
{
	m_pCapture = NULL;
	m_pCamera = NULL;
	m_pLogger = NULL;
	m_pCameraView = NULL;
	m_pCameraDlg = NULL;
	m_isRunning = false;
	m_isPause = false;
	// the new camera implementation
	m_pVideoImg = NULL;
	m_nFps = -1;
	m_nFpsAlpha = 0.1;
	m_isAvi = false;
	
	m_bIsChange = 0;

	m_isSetToStart = 0;
	m_isSetToStop = 0;
	m_isVideoShow = 1; 

	m_isBusy = 0;

	m_nFilter = CAMERA_FILTER_NONE;

	//default camera aspect ratio
	m_nAspectRatio = (double) 480/640;

	m_pTempProcGray = NULL;
	m_pTempProc = NULL;
	m_pVideoImgGray = NULL;
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CCameraVideo
// Purose:	destroy my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CCameraVideo::~CCameraVideo( )
{
    //stop camera
	Stop( );

	if( m_pVideoImg )
	{
		cvReleaseImage( &m_pVideoImg );
	}
	if( m_pTempProc ) cvReleaseImage( &m_pTempProc );
	if( m_pTempProcGray ) cvReleaseImage( &m_pTempProcGray );
	if( m_pVideoImgGray ) cvReleaseImage( &m_pVideoImgGray );

	// reset pointer to null
	m_pTempProc = NULL;

	m_pVideoImg = NULL;
	m_pCapture = NULL;
	m_pCameraView = NULL;
//	m_pFrame = NULL;
//	m_pWorker = NULL;
}

////////////////////////////////////////////////////////////////////
// Method:	Init
// Class:	CCameraVideo
// Purose:	initialize my startup variables
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CCameraVideo::Init( )
{
	// get config
//	UpdateConfig( );
	// get init time stamp
	m_timeCurrFrameStamp = GetDTime( );
	m_timePrevFrameStamp = m_timeCurrFrameStamp;
	
	return( 1 );
}

////////////////////////////////////////////////////////////////////
void CCameraVideo::ShowSettingsDlg( )
{

#ifdef _CV110
	capDlgVideoSource( m_pCapture->capWnd );
#else
	// hack to get the windows handler
	typedef struct CvCaptureCAM_VFW
	{
		void* vtable;
		CAPDRIVERCAPS caps;
		HWND   capWnd;
		VIDEOHDR* hdr;
		DWORD  fourcc;
		HIC    hic;
		IplImage* rgb_frame;
		IplImage frame;
	} CvCaptureCAM_VFW;

	CvCaptureCAM_VFW* p = (CvCaptureCAM_VFW*) m_pCapture;
	capDlgVideoSource( p->capWnd );
#endif

	// first pause camera
	PauseResume( );
	// mark change and wait for thread to restart camera
	IsChanged( );

//		m_pCamera->m_pCameraVideo->ShowSettingsDlg( (HWND) this->GetHWND(), m_vectCamDevices[nDevId].strName );

//		getFilterByName( 
//		capDlgVideoSource( p->capWnd );

		//create a videoInput object
//		videoInput VI;
		//to get a settings dialog for the device
//		VI.showSettingsWindow(0);

/*
		wxString strDD = m_vectCamDevices[nDevId].strDevId;
		CameraDescription cd;
		//WIA::CommonDialog* dialog = new Interop::WIA::CommonDialogClass();
		int ncams = cvcamGetCamerasCount( );

		// DeviceDescription, device, ChannelDescription
		cvcamGetProperty( 0, CVCAM_DESCRIPTION, &cd );

		cvcamSetProperty(0,CVCAM_PROP_ENABLE, CVCAMTRUE);
		cvcamGetProperty(0, CVCAM_CAMERAPROPS, NULL);
		// cvcamGetProperty(0, CVCAM_CAMERAPROPS, NULL);   

*/

//		_cvcamSummonPinPropPage(0);

}

////////////////////////////////////////////////////////////////////
void CCameraVideo::ShowFormatDlg( )
{
	// first pause camera
	PauseResume( );

#ifdef _CV110
	capDlgVideoFormat( m_pCapture->capWnd );
#else
	// hack to get the windows handler
	typedef struct CvCaptureCAM_VFW
	{
		void* vtable;
		CAPDRIVERCAPS caps;
		HWND   capWnd;
		VIDEOHDR* hdr;
		DWORD  fourcc;
		HIC    hic;
		IplImage* rgb_frame;
		IplImage frame;
	} CvCaptureCAM_VFW;

	CvCaptureCAM_VFW* p = (CvCaptureCAM_VFW*) m_pCapture;
	capDlgVideoFormat( p->capWnd );
#endif

	// mark change and wait for thread to restart camera
	IsChanged( );
}

/*
////////////////////////////////////////////////////////////////////
void CCameraVideo::ShowSettingsDlg( HWND hwndParent, wxString  cameraName )
{
//	m_pCapture->VI.showSettingsWindow(0);

    IGraphBuilder *pGraph;
    IMediaControl *pMediaControl;
	pGraph= NULL;
	pMediaControl= NULL;

	if( !FAILED(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&pGraph)) ) 
	{
		pGraph->QueryInterface(IID_IMediaControl, (void **)&pMediaControl);
	}

	//////////////////////////
	// settings window
	IAMVfwCaptureDialogs *pVfw = 0;
	IBaseFilter* pCap = getFilterByName( pGraph, cameraName );

	if (pCap!=0 && SUCCEEDED(pCap->QueryInterface(IID_IAMVfwCaptureDialogs, (void**)&pVfw)))
	{
		// Check if the device supports this dialog box.
		if (S_OK == pVfw->HasDialog(VfwCaptureDialog_Format)) 
		{
			// Show the dialog box.
			pVfw->ShowDialog(VfwCaptureDialog_Format, hwndParent);
		}

		if (S_OK == pVfw->HasDialog(VfwCaptureDialog_Source))
		{
			// Show the dialog box.
//			pVfw->ShowDialog(VfwCaptureDialog_Source, hwndParent);
		}	

		pVfw->Release();
	}

	pMediaControl->Release();
	pGraph->Release();
*/

//    if(!cvcam_properties[camera]._enabled)
//        return;

/*    //Find the output pit that is connected to the next filter...
    CAUUID uuID;
    ISpecifyPropertyPages* pspp = 0;

    int fcvcamInit = 0;
    OAFilterState state = State_Stopped;
    //if _cvcamMediaControl is valid, it means Graph has already been builded
    //therefore we have to disconnect all filters first
    if(_cvcamMediaControl.is_valid())
    {
        fcvcamInit = 1;
        //keep graph state before disconnect it, os we can restart from current state
        _cvcamMediaControl->GetState(0, &state);
        //disconnect graph
        _cvcamTearDownGraph();
    }

    IBaseFilter* filter = _cvcamSource[camera].value();

    IPin* pPin = get_source_pin(_cvcamSource[camera].value(), PINDIR_OUTPUT);
    if(!pPin)
        pPin=get_pin(filter, PINDIR_OUTPUT);

    if(!pPin)
        return;

    pPin->QueryInterface(IID_ISpecifyPropertyPages, (void**)&pspp);
    if(!pspp)
        return;

    pspp->GetPages(&uuID);
    pspp->Release();


    OleCreatePropertyFrame(NULL, 0, 0, L"Video Source", 1,
        (IUnknown**)&pPin, uuID.cElems, uuID.pElems,
        0, 0, NULL);

    CoTaskMemFree(uuID.pElems);
    pPin->Release();

    //store this video source resolution
    IAMStreamConfig* pVSC;
    AM_MEDIA_TYPE *pmt = NULL;

    _cvcamCapGraphBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, _cvcamSource[camera].value(),
        IID_IAMStreamConfig, (void **)&pVSC);

    pVSC->GetFormat(&pmt);
    VIDEOINFOHEADER *pVih = reinterpret_cast<VIDEOINFOHEADER*>(pmt->pbFormat);
    cvcam_properties[(uint)camera].srcwidth = pVih->bmiHeader.biWidth;
    cvcam_properties[(uint)camera].srcheight = abs(pVih->bmiHeader.biHeight);
    DeleteMediaType(pmt);

    pVSC->Release();
}

// get a filter by name...
////////////////////////////////////////////////////////////////////
IBaseFilter* CCameraVideo::getFilterByName(IGraphBuilder *pGraph, wxString name ) 
{
	name = "Logitech QuickCam Express";


      IEnumFilters *pEnum = NULL;
      IBaseFilter *pFilter;
      ULONG cFetched;
	  bool found= false;

      pGraph->EnumFilters(&pEnum);
      while(!found && pEnum->Next(1, &pFilter, &cFetched) == S_OK)
      {
        FILTER_INFO FilterInfo;
        char szName[256];
        wxString fname;

        pFilter->QueryFilterInfo(&FilterInfo);
        WideCharToMultiByte(CP_ACP, 0, FilterInfo.achName, -1, szName, 256, 0, 0);
        fname= szName;

		FilterInfo.pGraph->Release();

		if( fname == name ) 
			found= true;
		else
			pFilter->Release();
      }

      pEnum->Release();

	  if (found)
		  return pFilter;
	  else
          return 0;
}

////////////////////////////////////////////////////////////////////
bool createFilterGraph() 
{
	IPin* pSourceOut[2];
	pSourceOut[0]= pSourceOut[1]= NULL;

	addFilterByEnum(CLSID_VideoInputDeviceCategory,cameraName,pGraph,pSourceOut,1);

	if (cameraName == "") 
	{
		// Add the decoding filters
		addFilter(CLSID_AviSplitter, L"Splitter", pGraph, pSourceOut);
		addFilter(CLSID_AVIDec, L"Decoder", pGraph, pSourceOut);

		// Insert the first Smart Tee
		addFilter(CLSID_SmartTee, L"SmartTee(1)", pGraph, pSourceOut,2);
	}

	// Add the ProxyTrans filter
	addFilter(CLSID_ProxyTransform, L"ProxyTrans", pGraph, pSourceOut);

	// Set the ProxyTrans callback     
	IBaseFilter* pProxyFilter = NULL;
	IProxyTransform* pProxyTrans = NULL;
	pGraph->FindFilterByName(L"ProxyTrans",&pProxyFilter);
	pProxyFilter->QueryInterface(IID_IProxyTransform, (void**)&pProxyTrans);
	pProxyTrans->set_transform(process, 0);
	SAFE_RELEASE(pProxyTrans);
	SAFE_RELEASE(pProxyFilter);

	if (cameraName == "") {
	// Render the original (decoded) sequence using 2nd SmartTee(1) output pin
	addRenderer(L"Renderer(1)", pGraph, pSourceOut+1);
	}

	// Insert the second Smart Tee
	if (ofilename != 0)
	addFilter(CLSID_SmartTee, L"SmartTee(2)", pGraph, pSourceOut,2);

	// Encode the processed sequence 
	if (compressorName != "") {

	addFilterByEnum(CLSID_VideoCompressorCategory,compressorName,pGraph,pSourceOut);
	changeCompressorOptions(pSourceOut, 1, keyFrames, pFrames, quality);
	}

	if (ofilename != 0) {

	addFilter(CLSID_AviDest, L"AVImux", pGraph, pSourceOut);
	// Write to file
	addFileWriter(ofilename, pGraph, pSourceOut);
	// Render the transformed sequence using 2nd SmartTee(2) output pin
	addRenderer(L"Renderer(2)", pGraph, pSourceOut+1);

	} else {

	// Render the transformed sequence 
	addRenderer(L"Renderer(2)", pGraph, pSourceOut);
	}

	return 1;
}
*/

////////////////////////////////////////////////////////////////////
// Method:	SetConfig
// Class:	CCameraVideo
// Purose:	set configuration objects
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCameraVideo::SetConfig( CCamera* pCamera, CGUIFrame* pCameraDlg, DefHWDevice* pHWDevice )
{
	m_pCamera = pCamera;
	m_pCameraDlg = pCameraDlg;
	m_pHWDevice = pHWDevice;
	//m_pCameraView = pCameraView;

	// do the default
	m_nFilter = CAMERA_FILTER_NONE;
}

////////////////////////////////////////////////////////////////////
// Method:	SetFilter
// Class:	CCameraVideo
// Purose:	set camera filter
// Input:	filter id
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCameraVideo::SetFilter( int nFilter )
{
	m_nFilter = nFilter;
	//m_pConfig->SetIntVar( SECTION_ID_CAMERA, CONF_CAMERA_FILTER, m_nFilter );
}

////////////////////////////////////////////////////////////////////
// Method:	GetSize
// Class:	CCameraVideo
// Purose:	initialize my startup variables
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CCameraVideo::GetSize( )
{
	// now get my properties
	m_nWidth = (int) cvGetCaptureProperty( m_pCapture, CV_CAP_PROP_FRAME_WIDTH );
	m_nHeight = (int) cvGetCaptureProperty( m_pCapture, CV_CAP_PROP_FRAME_HEIGHT );
	// set camview size
//	m_pCameraView->SetSize( m_nWidth, m_nHeight );
	// set aspect ratio
	m_nAspectRatio = (double) m_nWidth/m_nHeight;
	// re-arange layout
//	m_pFrame->ResetLayout( );

	return( 0 );
}

////////////////////////////////////////////////////////////////////
// Method:	Start
// Class:	CCameraVideo
// Purose:	Start capture
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CCameraVideo::Start( )
{
	wxString strMsg;

#ifdef _USE_WIN
//	cvcamSetProperty(0,CVCAM_PROP_ENABLE, (void*)CVCAMTRUE);
	m_pCapture = cvCaptureFromCAM( 0 );

	// check if success
	if( m_pCapture )
	{
		// grab first frame to initialize format
		IplImage* pFrame = cvQueryFrame( m_pCapture );
		// get camera's size
		GetSize( );
		// log
		if( m_pLogger )
		{
			strMsg.Printf( wxT("INFO :Camera: started successfully in %d by %d"), m_nWidth, m_nHeight );
			m_pLogger->Log( strMsg );
		}
		
	} else
	{
		if( m_pLogger )
		{
			m_pLogger->Log( wxT("ERROR :Camera: failed to start"), GUI_LOGGER_ATTENTION_STYLE );
			m_isRunning = false;
		}

		return( 0 );
	}

#else
	int ncameras = cvcamGetCamerasCount();
	printf("DEBUG :: Found cameras=%d\n", ncameras );
	int w = 320;
	int h = 240;
	int nCam = 1;
	int t=1;
	int p=1;
	
	cvcamSetProperty(nCam, CVCAM_RNDWIDTH,  &w );
	cvcamSetProperty(nCam, CVCAM_RNDHEIGHT, &h );

	cvcamSetProperty(nCam, CVCAM_PROP_ENABLE, &t );

	int width = 320;
	int height = 200;


	HWND MyWin = (HWND)m_pCameraView->GetHandle();
	cvcamSetProperty(nCam, CVCAM_PROP_WINDOW, &MyWin ); 
	cvcamSetProperty(nCam, CVCAM_PROP_RENDER, &p );  

	
	cvcamSetProperty(0, CVCAM_PROP_CALLBACK, (void*)testcallback );
	cvcamInit();
	Sleep( 5000 );
	cvcamStart();

#endif
	
	m_pVideoImgGray = cvCreateImage( cvSize( m_nWidth, m_nHeight ), IPL_DEPTH_8U, 1 );
	m_pTempProc = cvCreateImage( cvSize( m_nWidth, m_nHeight ), IPL_DEPTH_8U, 3 );
	m_pTempProcGray = cvCreateImage( cvSize( m_nWidth, m_nHeight ), IPL_DEPTH_8U, 1 );

	m_isRunning = true;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	Stop
// Class:	CCameraVideo
// Purose:	Stop capture
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCameraVideo::Stop( )
{
	m_isRunning = false;

#ifdef _USE_WIN
	if( m_pCapture ) cvReleaseCapture( &m_pCapture );
	wxThread::Sleep( 5 );
#else
	cvcamExit();
#endif

}

////////////////////////////////////////////////////////////////////
// Method:	PauseResume
// Class:	CCameraVideo
// Puprose:	pause/resume capture
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCameraVideo::PauseResume( )
{
	if( m_isPause )
	{
		m_isPause = false;
	} else
	{
		m_isPause = true;
	}
}

////////////////////////////////////////////////////////////////////
// Method:	IsChanged
// Class:	CCameraVideo
// Puprose:	mark flag change
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCameraVideo::IsChanged( )
{
	m_bIsChange = 1;
	m_isPause = true;
}

////////////////////////////////////////////////////////////////////
// feedback - only for linux
#ifndef _USE_WIN
void testcallback(void* _image)
{
	IplImage* image = (IplImage*)_image;
	cvLine(image, cvPoint(0, 0), cvPoint(image->width, image->height),CV_RGB(255, 0, 0), 1);
	m_pLnxFrame = image ;
}
#endif

////////////////////////////////////////////////////////////////////
// Method:	Get Next Frame
// Class:	CCameraVideo
// Purpose:	get next frame from camera buffer
// Input:	pointer to void
// Output:	Nothing
////////////////////////////////////////////////////////////////////
int CCameraVideo::GetNextFrame( void* )
{
	m_isBusy = 1;

    static int repositioning = 0;
    IplImage* pFrame = NULL;

	// get current frame time stamp
    m_timeCurrFrameStamp = GetDTime( );
	if( m_timeCurrFrameStamp - m_timePrevFrameStamp  < 5 )
	{
		m_isBusy = 0;
		return( 0 );

	} else
		m_timePrevFrameStamp = m_timeCurrFrameStamp;

#ifdef _USE_WIN	
	// get frame if any
 	pFrame = cvQueryFrame( m_pCapture );
#else
	// to test this - don't remmember how i got this working 
	// on linux 
	pFrame = m_pLnxFrame;
	// get frame in linux ?

//	cvcamPause();
//	cvcamGetProperty(0,CVCAM_PROP_RAW,&pFrame);
//	cvcamResume();

//	pFrame = cvCloneImage( fimg );

	Sleep(400);
#endif

	// check if no frame return
	if( !pFrame ) return( 0 );

	// if this was avi and frame is zero(end or none?) stop
    if( pFrame == 0 && m_isAvi )
    {
		m_isBusy = 0;
        //this->StopAvi( 0,0 );
        return( 0 );
	}


	// if no video image 
    if( !m_pVideoImg ) 
    {
        cvReleaseImage( &m_pVideoImg );
		m_pVideoImg = cvCreateImage( cvSize( m_nWidth, m_nHeight ), 8, 3 );
    }
	
	// check for the last origin of the frame
	if( pFrame->origin == 1 )
	{
		cvConvertImage( pFrame, m_pVideoImg, CV_CVTIMG_FLIP | CV_CVTIMG_SWAP_RB );
	} else
	{
		cvCopy( pFrame, m_pVideoImg, 0 );
	}		

	//////////////
	// FILTERS :: apply filter if any selected
	if( m_nFilter == CAMERA_FILTER_CONTOUR )
	{
		cvMorphologyEx( m_pVideoImg, m_pVideoImg, m_pTempProc, 0, 
						CV_MOP_GRADIENT, 1 );

	} else if( m_nFilter == CAMERA_FILTER_SHADOW )
	{
		// shadow
		cvThreshold( m_pVideoImg, m_pVideoImg, 80, 255, CV_THRESH_TOZERO );
		//cvThreshold( m_pVideoImg, m_pVideoImg, 0, 101, CV_THRESH_TOZERO );
	} else if( m_nFilter == CAMERA_FILTER_SMOOTH )
	{
		// blur smooth 
		cvSmooth( m_pVideoImg, m_pVideoImg, CV_BLUR, 7 );

	} else if( m_nFilter == CAMERA_FILTER_EDGE )
	{
		cvCvtColor( m_pVideoImg, m_pVideoImgGray, CV_RGB2GRAY);
		cvSmooth( m_pVideoImgGray, m_pTempProcGray, CV_BLUR, 3, 3, 0 );
		cvNot( m_pVideoImgGray, m_pTempProcGray );
		cvCanny(m_pVideoImgGray, m_pTempProcGray, (float) FILTER_EDGE_THRESHOLD, 
					(float) FILTER_EDGE_THRESHOLD*3, 3);
		cvZero( m_pTempProc );
		cvCopy( m_pVideoImg, m_pTempProc, m_pTempProcGray );
	}

	// if to update gui
	if( m_pCameraDlg && m_isVideoShow == 1 )
	{
		unsigned char *rawData;
		CvSize roiSize;
		int step = 0;

		// Update gui
		IplImage* pIplImage;
		if( m_nFilter == CAMERA_FILTER_EDGE )
		{
			pIplImage = m_pTempProc;
			cvGetRawData( m_pTempProc, &rawData, &step, &roiSize );

		} else
		{
			pIplImage = m_pVideoImg;
			cvGetRawData( m_pVideoImg, &rawData, &step, &roiSize );
		}
			
		// set to wx image buffer
		m_rWxImage = wxImage( m_nWidth, m_nHeight, rawData, TRUE );
		// set view
		//m_pCameraView->SetImage( &m_rWxImage, 1, 0 );
		m_pCameraDlg->SetCameraImage( &m_rWxImage, m_pVideoImg, m_pHWDevice->do_update_gui );
	}		

	////////////////
	// If camera started
    if( m_isRunning )
    {
		// get current frame time stamp
        m_timeCurrFrameStamp = GetDTime( );
        // update fps
        if( m_nFps < 0 )
		{
            m_nFps = 1000 / ( m_timeCurrFrameStamp - m_timePrevFrameStamp );
		} else
		{	
            m_nFps = ( 1 - m_nFpsAlpha ) * m_nFps + m_nFpsAlpha * 
						1000 / ( m_timeCurrFrameStamp - m_timePrevFrameStamp );
		}
		// set current time stamp as previuos
        m_timePrevFrameStamp = m_timeCurrFrameStamp;
		// get info of number of frames per second in a string
		// for debuging/etc
        sprintf( m_strFps, "FPS: %5.1f", m_nFps );
//		m_pFrame->SetStatusFps( m_strFps );

    }

	m_isBusy = 0;
	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	GetIFrame
// Class:	CCameraVideo
// Purose:	get last frame grabbed
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
IplImage *CCameraVideo::GetIFrame( )
{
	if( m_nFilter == CAMERA_FILTER_EDGE )
		return( m_pTempProc );
	else
		return( m_pVideoImg );
}


////////////////////////////////////////////////////////////////////
// Method:	Run
// Class:	CCameraVideo
// Purose:	Start to run my camera thread
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CCameraVideo::Run( ) 
{
	if( m_isBusy == 1 ) return ( 0 );

	int nStatus = 0;

	// PROCESS START/STOP COMMANDS
	if( m_isSetToStop && m_isRunning )
	{
		Stop( );
		m_isSetToStop = 0;

	} else if( m_isSetToStart && !m_isRunning )
	{
		int nStatus = Start( );
		// if failed to start
		if( nStatus == 0 )
		{
//			m_pFrame->GetToolBar()->ToggleTool( wxID_BUTTON_CTRL_CAMERA, 0 );
		}
		m_isSetToStart = 0;
	}

	////////////////////////////////////////
	// IF CAMERA RUNNING AND NOT PAUSED
	if( !m_isPause && m_isRunning )
	{
		// Get my next frame
		nStatus = this->GetNextFrame( NULL );

	} else
	{
		// if set to change
		if( m_bIsChange == 1 )
		{
			// release alloc images
			if( m_pVideoImg ) cvReleaseImage( &m_pVideoImg );
			if( m_pTempProc ) cvReleaseImage( &m_pTempProc );
			if( m_pTempProcGray ) cvReleaseImage( &m_pTempProcGray );
			if( m_pVideoImgGray ) cvReleaseImage( &m_pVideoImgGray );

			// check size
			Stop( );
			Start( );
			m_bIsChange = 0;
			m_isPause = 0;
		}
	}

	return( nStatus );
}
