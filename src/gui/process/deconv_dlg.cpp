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
#include "deconv_dlg.h"

BEGIN_EVENT_TABLE( CProcessDeconvolutionDlg, CImageProcBaseDlg )
END_EVENT_TABLE( )
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CProcessDeconvolutionDlg
// Purpose:	Initialize my 3d view config dialog 
// Input:	pointer to reference window 
// Output:	nothing
////////////////////////////////////////////////////////////////////
CProcessDeconvolutionDlg::CProcessDeconvolutionDlg( wxWindow *parent, int nType, int nViewType, 
										const wxString& strTitle ) : CImageProcBaseDlg( parent, strTitle, nViewType )
{
	m_nType = nType;
	Create( );
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CProcessDeconvolutionDlg
// Purpose:	destroy my  dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CProcessDeconvolutionDlg::~CProcessDeconvolutionDlg( ) 
{

}

////////////////////////////////////////////////////////////////////
// Method:	Create
// Class:	CProcessDeconvolutionDlg
// Purpose:	create  dialog by type
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CProcessDeconvolutionDlg::Create( )
{
	if( m_nType == IMAGE_PROCESS_TYPE_DECONV_RLUCY )
	{
		SetTitle( wxT("Deconvolution :: Richardson-Lucy") );
		AddSlider( "Radius", 0, 200, 1.0, 10.0, 3.0 );
		AddSlider( "Sigma", 0, 200, 0.05, 1.0, 0.5 );

	} else if( m_nType == IMAGE_PROCESS_TYPE_DECONV_WIENER )
	{
		SetTitle( wxT("Deconvolution :: Wiener") );

	} else if( m_nType == IMAGE_PROCESS_TYPE_DECONV_DCTEM )
	{
		SetTitle( wxT("Deconvolution :: DCT-EM") );

	}

	Layout( );
	Fit( );
	Refresh( );
}

////////////////////////////////////////////////////////////////////
// Method:	Process
// Class:	CProcessDeconvolutionDlg
// Purpose:	process brightness contrast gamma dialog
// Input:	elx image
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CProcessDeconvolutionDlg::Process( ImageVariant& rElxImage )
{
	if( m_nType == IMAGE_PROCESS_TYPE_DECONV_RLUCY )
		rElxImage.DeconvolveRL( m_vectSliderCurrentReal[0], m_vectSliderCurrentReal[1], 1, BF_Nearest, CM_All );
	else if( m_nType == IMAGE_PROCESS_TYPE_DECONV_WIENER )
	{
		// add here cutom

	} else if( m_nType == IMAGE_PROCESS_TYPE_DECONV_DCTEM )
	{
		// add here cutom
	}
}
