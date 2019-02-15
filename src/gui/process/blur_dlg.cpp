////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// wx includes
#include "wx/wxprec.h"
#include <wx/dialog.h>

// ELX
#include <elx/core/CoreMacros.h>
#include <elx/math/TransfertFunction.h>
#include <elx/image/AbstractImageManager.h>
#include <elx/image/ImageFileManager.h>
#include <elx/math/MathCore.h>
#include <elx/image/ImageVariant.h>
#include <elx/image/ImageOperatorsImpl.h>
#include <elx/image/PixelIterator.h>

// local headers
#include "../astro_img_view.h"

// main header
#include "blur_dlg.h"

BEGIN_EVENT_TABLE( CProcessBlurDlg, CImageProcBaseDlg )
END_EVENT_TABLE( )
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CProcessBlurDlg
// Purpose:	Initialize my 3d view config dialog 
// Input:	pointer to reference window 
// Output:	nothing
////////////////////////////////////////////////////////////////////
CProcessBlurDlg::CProcessBlurDlg( wxWindow *parent, int nType, int nViewType, 
										const wxString& strTitle ) : CImageProcBaseDlg( parent, strTitle, nViewType )
{
	m_nType = nType;
	Create( );
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CProcessBlurDlg
// Purpose:	destroy my  dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CProcessBlurDlg::~CProcessBlurDlg( ) 
{

}

////////////////////////////////////////////////////////////////////
// Method:	Create
// Class:	CProcessBlurDlg
// Purpose:	create  dialog by type
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CProcessBlurDlg::Create( )
{
	if( m_nType == IMAGE_PROCESS_TYPE_BLUR_SMOOTH )
	{
		SetTitle( wxT("Apply Smooth Blurring") );
		AddSlider( "Threshold Min", 0, 200, 0.0, 1.0, 0.0 );
		AddSlider( "Threshold Max", 0, 200, 0.0, 1.0, 1.0 );
		AddSlider( "Blending", 0, 200, 0.0, 1.0, 1.0 );

	} else if( m_nType == IMAGE_PROCESS_TYPE_BLUR_MEAN )
	{
		SetTitle( wxT("Apply Mean Blurring") );
		AddSlider( "Radius", 0, 200, 1.0, 31.0, 3.0 );
		AddSlider( "Blending", 0, 200, 0.0, 1.0, 1.0 );
		
	} else if( m_nType == IMAGE_PROCESS_TYPE_BLUR_LOWPASS )
	{
		SetTitle( wxT("Apply Low-Pass Blurring") );
		AddSlider( "Alpha", 0, 200, 1.0, 100.0, 3.0 );
		AddSlider( "Threshold Min", 0, 200, 0.0, 1.0, 0.0 );
		AddSlider( "Threshold Max", 0, 200, 0.0, 1.0, 1.0 );
		AddSlider( "Blending", 0, 200, 0.0, 1.0, 1.0 );

	} else if( m_nType == IMAGE_PROCESS_TYPE_BLUR_GAUSSIAN )
	{
		SetTitle( wxT("Apply Gaussian Blurring") );
		AddSlider( "Radius", 0, 200, 1.0, 31.0, 3.0 );
		AddSlider( "Variance", 0, 200, 1.0, 15.0, 2.0 );
		AddSlider( "Blending", 0, 200, 0.0, 1.0, 1.0 );

	} else if( m_nType == IMAGE_PROCESS_TYPE_BLUR_BILATERAL )
	{
		SetTitle( wxT("Apply Bilateral Blurring") );
		AddSlider( "Radius", 0, 200, 1.0, 10.0, 2.5 );
		AddSlider( "Spatial Variance", 0, 200, 1.0, 15.0, 2.0 );
		AddSlider( "Range Variance", 0, 200, 1.0, 100.0, 20.0 );
		AddSlider( "Blending", 0, 200, 0.0, 1.0, 1.0 );

	} else if( m_nType == IMAGE_PROCESS_TYPE_BLUR_SELECTIVE )
	{
		SetTitle( wxT("Apply Selective Blurring") );
		AddSlider( "Blending", 0, 200, 0.0, 1.0, 1.0 );

	} else if( m_nType == IMAGE_PROCESS_TYPE_BLUR_FFTLOW )
	{
		SetTitle( wxT("Apply FFT Low-Pass Blurring") );
		AddSlider( "Cutoff", 0, 200, 0.0, 0.5, 0.25 );
	}	

	Layout( );
	Fit( );
	Refresh( );
}

////////////////////////////////////////////////////////////////////
// Method:	Process
// Class:	CProcessBlurDlg
// Purpose:	process brightness contrast gamma dialog
// Input:	elx image
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CProcessBlurDlg::Process( ImageVariant& rElxImage )
{
	if( m_nType == IMAGE_PROCESS_TYPE_BLUR_SMOOTH )
		rElxImage.ApplySmooth( m_vectSliderCurrentReal[0], m_vectSliderCurrentReal[1], m_vectSliderCurrentReal[2], BF_Nearest, 1, CM_All );
	else if( m_nType == IMAGE_PROCESS_TYPE_BLUR_MEAN )
		rElxImage.ApplyMean( m_vectSliderCurrentReal[0], true, 0.0, 1.0, m_vectSliderCurrentReal[1], BF_Nearest, 1, CM_All );
	else if( m_nType == IMAGE_PROCESS_TYPE_BLUR_LOWPASS )
		rElxImage.ApplyLowpass( m_vectSliderCurrentReal[0], m_vectSliderCurrentReal[1], 
								m_vectSliderCurrentReal[2], m_vectSliderCurrentReal[3], BF_Nearest, 1, CM_All );
	else if( m_nType == IMAGE_PROCESS_TYPE_BLUR_GAUSSIAN )
		rElxImage.ApplyGaussian( m_vectSliderCurrentReal[0], m_vectSliderCurrentReal[1], m_vectSliderCurrentReal[2], BF_Nearest, 1, CM_All );
	else if( m_nType == IMAGE_PROCESS_TYPE_BLUR_BILATERAL )
		rElxImage.ApplyBilateral( m_vectSliderCurrentReal[0], m_vectSliderCurrentReal[1], 
								m_vectSliderCurrentReal[2], m_vectSliderCurrentReal[3], BF_Nearest, 1, CM_All );
	else if( m_nType == IMAGE_PROCESS_TYPE_BLUR_SELECTIVE )
		rElxImage.ApplySelectiveBlur( m_vectSliderCurrentReal[0], BF_Nearest, 1, CM_All );
	else if( m_nType == IMAGE_PROCESS_TYPE_BLUR_FFTLOW )
		rElxImage.ApplyFFTLowPass( m_vectSliderCurrentReal[0], 2, CM_All );
}
