
// opencv headers
#include "cv.h"
#include "highgui.h"

//  main header
#include "image_shift.h"

////////////////////////////////////////////////////////////////////
CImageShift::CImageShift( )
{
	m_pImage = NULL;
	m_pHSV = NULL;
	m_pHUE = NULL;
	m_pMask = NULL;
	m_pBackproject = NULL;
	m_pHist = NULL;

	m_bTargetSet = 0;
	m_bInitTrackObject = 0;
	m_nCameraId = -1;

	hdims = 16;
	hranges_arr[0] = 0.0;
	hranges_arr[1] = 180.0;
	hranges = hranges_arr;

	vmin = 160; //200; // 40
	vmax = 256;
	smin = 45; //50; // 44
}

////////////////////////////////////////////////////////////////////
CImageShift::~CImageShift( )
{
	Reset( );
}

////////////////////////////////////////////////////////////////////
void CImageShift::Reset( )
{
	if( m_pImage ) cvReleaseImage( &m_pImage );
	if( m_pHSV ) cvReleaseImage( &m_pHSV );
	if( m_pHUE ) cvReleaseImage( &m_pHUE );
	if( m_pMask ) cvReleaseImage( &m_pMask );
	if( m_pBackproject ) cvReleaseImage( &m_pBackproject );
	if( m_pHist ) cvReleaseHist( &m_pHist );

	m_pImage = NULL;
	m_pHSV = NULL;
	m_pHUE = NULL;
	m_pMask = NULL;
	m_pBackproject = NULL;
	m_pHist = NULL;

	m_bTargetSet = 0;
	m_bInitTrackObject = 0;
}

////////////////////////////////////////////////////////////////////
void CImageShift::SetCameraSource( int nCameraId )
{
	m_nCameraId = nCameraId;
	Reset( );
}	

////////////////////////////////////////////////////////////////////
void CImageShift::SetTarget( int x, int y )
{
	int w=10, h=10;

	selection.x = x-w;
	selection.y = y-h;
	selection.width = 2*w;
	selection.height = 2*h;

	m_bTargetSet = 1;
	m_bInitTrackObject = 0;
}

////////////////////////////////////////////////////////////////////
void CImageShift::Track( IplImage* pFrame, double& x, double& y )
{
	if( !m_bTargetSet || !pFrame ) return;

	// if no image initialized 
	if( !m_pImage )
	{
		// allocate all the buffers
		m_pImage = cvCreateImage( cvGetSize(pFrame), 8, 3 );
		m_pImage->origin = pFrame->origin;
		m_pHSV = cvCreateImage( cvGetSize(pFrame), 8, 3 );
		m_pHUE = cvCreateImage( cvGetSize(pFrame), 8, 1 );
		m_pMask = cvCreateImage( cvGetSize(pFrame), 8, 1 );
		m_pBackproject = cvCreateImage( cvGetSize(pFrame), 8, 1 );
		m_pHist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
	}

	// copy local image
	cvCopy( pFrame, m_pImage, 0 );
	cvCvtColor( m_pImage, m_pHSV, CV_BGR2HSV );

	// calculate shift
    int _vmin = vmin, _vmax = vmax;

    cvInRangeS( m_pHSV, cvScalar(0,smin,MIN(_vmin,_vmax),0), cvScalar(180,256,MAX(_vmin,_vmax),0), m_pMask );
    cvSplit( m_pHSV, m_pHUE, 0, 0, 0 );

	// if first time track - do the init
    if( m_bInitTrackObject == 0 )
    {
        float max_val = 0.f;
        cvSetImageROI( m_pHUE, selection );
        cvSetImageROI( m_pMask, selection );
        cvCalcHist( &m_pHUE, m_pHist, 0, m_pMask );
        cvGetMinMaxHistValue( m_pHist, 0, &max_val, 0, 0 );
        cvConvertScale( m_pHist->bins, m_pHist->bins, max_val ? 255. / max_val : 0., 0 );
        cvResetImageROI( m_pHUE );
        cvResetImageROI( m_pMask );
        track_window = selection;
        m_bInitTrackObject = 1;
	}

	// calculate backprojectio/image shift
	cvCalcBackProject( &m_pHUE, m_pBackproject, m_pHist );
	cvAnd( m_pBackproject, m_pMask, m_pBackproject, 0 );
	cvCamShift( m_pBackproject, track_window, cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ), &track_comp, &track_box );
	track_window = track_comp.rect;

	// adjust angle
	if( !m_pImage->origin ) track_box.angle = -track_box.angle;

	// xor image active region ??
/*	if( select_object && selection.width > 0 && selection.height > 0 )
	{
		cvSetImageROI( m_pImage, selection );
		cvXorS( m_pImage, cvScalarAll(255), m_pImage, 0 );
		cvResetImageROI( m_pImage );
	}
*/

	//cvEllipseBox( image, track_box, CV_RGB(255,0,0), 3, CV_AA, 0 );
	x = track_box.center.x;
	y = track_box.center.y;

	return;
}
