////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _OPENCV_IMAGE_SHIFT
#define _OPENCV_IMAGE_SHIFT

// opencv headers
#include "cv.h"
#include "highgui.h"

class CImageShift
{
// public methods
public:
	CImageShift( );
	~CImageShift( );
	void Reset( );
	void SetCameraSource( int nCameraId );

	void SetTarget( int x, int y );
	void Track( IplImage* pFrame, double& x, double& y );

// public data
public:

	IplImage* m_pImage;
	IplImage* m_pHSV;
	IplImage* m_pHUE;
	IplImage* m_pMask; 
	IplImage* m_pBackproject;
	CvHistogram* m_pHist;

	int m_bInitTrackObject;
	int m_bTargetSet;
	int m_nCameraId;

//	CvPoint origin;
	CvRect selection;
	CvRect track_window;
	CvBox2D track_box;
	CvConnectedComp track_comp;

	int hdims;
	float hranges_arr[2];
	float* hranges;

	int vmin;
	int vmax;
	int smin;

};

#endif
