
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
#include "sharp_dlg.h"

BEGIN_EVENT_TABLE( CProcessSharpDlg, CImageProcBaseDlg )
END_EVENT_TABLE( )
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CProcessSharpDlg
// Purpose:	Initialize my 3d view config dialog 
// Input:	pointer to reference window 
// Output:	nothing
////////////////////////////////////////////////////////////////////
CProcessSharpDlg::CProcessSharpDlg( wxWindow *parent, int nType, int nViewType, 
										const wxString& strTitle ) : CImageProcBaseDlg( parent, strTitle, nViewType )
{
	m_nType = nType;
	Create( );
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CProcessSharpDlg
// Purpose:	destroy my  dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CProcessSharpDlg::~CProcessSharpDlg( ) 
{

}

////////////////////////////////////////////////////////////////////
// Method:	Create
// Class:	CProcessSharpDlg
// Purpose:	create  dialog by type
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CProcessSharpDlg::Create( )
{
	if( m_nType == IMAGE_PROCESS_TYPE_SHARP_SHARPEN )
	{
		SetTitle( wxT("Apply Sharp :: Sharpen") );
		AddSlider( "Threshold Min", 0, 200, 0.0, 1.0, 0.0 );
		AddSlider( "Threshold Max", 0, 200, 0.0, 1.0, 1.0 );
		AddSlider( "Blend", 0, 200, 0.0, 1.0, 1.0 );

	} else if( m_nType == IMAGE_PROCESS_TYPE_SHARP_UNSHARP )
	{
		SetTitle( wxT("Apply Sharp :: Unsharp") );
		AddSlider( "Alpha", 0, 200, 0.0, 1.0, 0.20 );
		AddSlider( "ThresholdMin", 0, 200, 0.0, 1.0, 0.0 );
		AddSlider( "ThresholdMax", 0, 200, 0.0, 1.0, 1.0 );
		AddSlider( "Blend", 0, 200, 0.0, 1.0, 1.0 );

	} else if( m_nType == IMAGE_PROCESS_TYPE_SHARP_UNSHARPMASK )
	{
		SetTitle( wxT("Apply Sharp :: Unsharp-Mask") );
		AddSlider( "Radius", 0, 200, 1.0, 101.0, 20.0 );
		AddSlider( "Amount", 0, 200, 0.10, 2.10, 1.0 );
		AddSlider( "Threshold", 0, 200, 0.10, 2.10, 0.50 );
		// also add borders

	} else if( m_nType == IMAGE_PROCESS_TYPE_SHARP_LAPGAUS )
	{
		SetTitle( wxT("Apply Sharp :: Laplacian of Gaussian") );
		AddSlider( "Radius", 0, 200, 1.0, 31.0, 4.0 );
		AddSlider( "Amount", 0, 200, 0.05, 15.0, 1.5 );

	} else if( m_nType == IMAGE_PROCESS_TYPE_SHARP_EMBOSS )
	{
		SetTitle( wxT("Apply Sharp :: Emboss") );
		AddSlider( "Degree", 0, 200, 0.0, 360.0, 0.0 );
		AddSlider( "Threshold Min", 0, 200, 0.0, 1.0, 0.0 );
		AddSlider( "Threshold Max", 0, 200, 0.0, 1.0, 1.0 );
		AddSlider( "Blend", 0, 200, 0.0, 1.0, 1.0 );
	}

	Layout( );
	Fit( );
	Refresh( );
}

////////////////////////////////////////////////////////////////////
// Method:	Process
// Class:	CProcessSharpDlg
// Purpose:	process brightness contrast gamma dialog
// Input:	elx image
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CProcessSharpDlg::Process( ImageVariant& rElxImage )
{
	if( m_nType == IMAGE_PROCESS_TYPE_SHARP_SHARPEN )
		rElxImage.ApplySharpen( m_vectSliderCurrentReal[0], m_vectSliderCurrentReal[1], m_vectSliderCurrentReal[2], 1.0, BF_Nearest, 1, CM_All );
//	else if( m_nType == IMAGE_PROCESS_TYPE_SHARP_UNSHARP )
//		rElxImage.ApplyUnsharp( m_vectSliderCurrentReal[0], m_vectSliderCurrentReal[1], 
//								m_vectSliderCurrentReal[2], m_vectSliderCurrentReal[3], BF_Nearest, 1, CM_All );
	else if( m_nType == IMAGE_PROCESS_TYPE_SHARP_UNSHARPMASK )
		rElxImage.ApplyUnsharpMask( m_vectSliderCurrentReal[0], m_vectSliderCurrentReal[1], m_vectSliderCurrentReal[2], BF_Nearest, CM_All );
	else if( m_nType == IMAGE_PROCESS_TYPE_SHARP_LAPGAUS )
		rElxImage.ApplyLoG( m_vectSliderCurrentReal[0], m_vectSliderCurrentReal[1], BF_Nearest, CM_All );
	else if( m_nType == IMAGE_PROCESS_TYPE_SHARP_EMBOSS )
		rElxImage.ApplyEmboss( m_vectSliderCurrentReal[0], m_vectSliderCurrentReal[1], 
								m_vectSliderCurrentReal[2], m_vectSliderCurrentReal[3], BF_Nearest, 1, CM_All );
}
