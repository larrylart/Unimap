////////////////////////////////////////////////////////////////////
// Name:		video camera header
// File:		camera_video.h
// Purpose:		interface for video camera processing
//
// Created by:	Larry Lart on 04/07/2006
// Updated by:	Larry Lart on 10/09/2009
//
////////////////////////////////////////////////////////////////////

#ifndef _CAMERA_VIDEO_H
#define _CAMERA_VIDEO_H

// opencv includes
#include "highgui.h"
//#include <vfw.h>
#include "cv.h"
#include "cvaux.h"
//#include "cvcam.h"
// custom define to use windows
#define _USE_WIN

// wx
//precompiled headers
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif 

// define camera filters
#define CAMERA_FILTER_NONE		0
#define CAMERA_FILTER_CONTOUR	1
#define CAMERA_FILTER_SHADOW	2
#define CAMERA_FILTER_SMOOTH	3
#define CAMERA_FILTER_EDGE		4

// filters constants
#define FILTER_EDGE_THRESHOLD	75

//define externall classes
class CCamera;
//class CVideoWorker;
class CCameraView;
class CLogger;
class CGUIFrame;
// system custom
//class IBaseFilter;
//class IGraphBuilder;

class CCameraVideo 
{

// public methods
public:
	// contructor
	CCameraVideo( );
	~CCameraVideo( );
	// get frame method
	IplImage *GetIFrame( );

	// initialize camera frame def
	int Init(  );
	void SetConfig( CCamera* pCamera, CGUIFrame* pCameraDlg, DefHWDevice* pHWDevice );
	void SetFilter( int nFilter );

	int Run();
	int Start( );
	void Stop( );
	void PauseResume( );
	int GetSize( );
	void IsChanged( );

	void ShowSettingsDlg( );
	void ShowFormatDlg( );

//	void ShowSettingsDlg( HWND hwndParent, wxString  cameraName );
//	IBaseFilter* getFilterByName(IGraphBuilder *pGraph, wxString name );

// data
public:
	// external objects
	CCamera*	m_pCamera;
	DefHWDevice* m_pHWDevice;

	CLogger*	m_pLogger;
	CvCapture*		m_pCapture;

	CGUIFrame*	m_pCameraDlg;
	CCameraView*	m_pCameraView;

//	CGUIFrame*		m_pFrame;
//	CUmouseWorker*	m_pWorker;

	// status flags
	bool m_isRunning;
	bool m_isAvi;
	bool m_isPause;
	bool m_isBusy;
	bool m_isSetToStart;
	bool m_isSetToStop;
	bool m_isVideoShow;

	bool m_bProcBusy;
	int m_bIsChange;

	int			m_nWidth;
	int			m_nHeight;

	int			m_nFilter;

	double		m_nAspectRatio;

// protected data
protected:
	IplImage*	m_pVideoImg;
	wxImage		m_rWxImage;

	IplImage*	m_pTempProc;
	IplImage*	m_pTempProcGray;
	IplImage*	m_pVideoImgGray;

	double		m_nFpsAlpha;
	double		m_nFps;
	char		m_strFps[255];
	double		m_timePrevFrameStamp;
	double		m_timeCurrFrameStamp;
	int			m_nTotalFrames;

// Implementation
protected:
	int GetNextFrame( void* );
	
//	DECLARE_MESSAGE_MAP()
};

#endif 
