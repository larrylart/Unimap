
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
#include "edge_dlg.h"

BEGIN_EVENT_TABLE( CProcessEdgeDlg, CImageProcBaseDlg )
END_EVENT_TABLE( )
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CProcessEdgeDlg
// Purpose:	Initialize my 3d view config dialog 
// Input:	pointer to reference window 
// Output:	nothing
////////////////////////////////////////////////////////////////////
CProcessEdgeDlg::CProcessEdgeDlg( wxWindow *parent, int nType, int nViewType, 
										const wxString& strTitle ) : CImageProcBaseDlg( parent, strTitle, nViewType )
{
	m_nType = nType;
	Create( );
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CProcessEdgeDlg
// Purpose:	destroy my  dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CProcessEdgeDlg::~CProcessEdgeDlg( ) 
{

}

////////////////////////////////////////////////////////////////////
// Method:	Create
// Class:	CProcessEdgeDlg
// Purpose:	create  dialog by type
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CProcessEdgeDlg::Create( )
{
	if( m_nType == IMAGE_PROCESS_TYPE_EDGE_GRADIENT )
	{
		SetTitle( wxT("Apply Edge :: Gradient") );
		// add here cutom select option
		wxString vectMethods[8] = {wxT("Pixel Differenc"), wxT("Basic"), wxT("Roberts"), wxT("Sobel"), wxT("Prewitt"), wxT("FreiChen"), wxT("Max") };
		//int vectValues[8] = {ED_Sobel, ED_Prewitt, ED_Kirsch, ED_FreiChen, ED_Laplacian, ED_LoG, ED_MarrHidreth, EG_Accurate};
		int vectValues[8] = {ED_PixelDifference, ED_Basic, ED_Roberts, ED_Sobel, ED_Prewitt, ED_FreiChen, ED_Max};
		AddSelector( "Method:", 7, vectMethods, vectValues );

	} else if( m_nType == IMAGE_PROCESS_TYPE_EDGE_ROBERSCROSS )
	{
		SetTitle( wxT("Apply Edge :: Robert Cross") );
		AddSlider( "Blending", 0, 200, 0.0, 1.0, 1.0, 1 );

	} else if( m_nType == IMAGE_PROCESS_TYPE_EDGE_LAPLACIAN )
	{
		SetTitle( wxT("Apply Edge :: Laplacian/Shape") );
		AddSlider( "Shape", 0, 200, 0.0, 1.0, 0.20 );
		AddSlider( "Threshold Min", 0, 200, 0.0, 1.0, 0.0 );
		AddSlider( "Threshold Max", 0, 200, 0.0, 1.0, 1.0 );
		AddSlider( "Blending", 0, 200, 0.0, 1.0, 1.0 );

	} else if( m_nType == IMAGE_PROCESS_TYPE_EDGE_MARRHILDRETH )
	{
		SetTitle( wxT("Apply Edge :: Marr Hildreth") );
		AddSlider( "Blending", 0, 200, 0.0, 1.0, 1.0, 1 );

	} else if( m_nType == IMAGE_PROCESS_TYPE_EDGE_CANNY )
	{
		SetTitle( wxT("Apply Edge :: Canny") );
		AddSlider( "Variance", 0, 200, 1.0, 6.0, 3.0 );
		AddSlider( "Threshold Low", 0, 200, 0.0, 1.0, 0.0 );
		AddSlider( "Threshold High", 0, 200, 0.0, 1.0, 1.0 );
	}

	Layout( );
	Fit( );
	Refresh( );
}

////////////////////////////////////////////////////////////////////
// Method:	Process
// Class:	CProcessEdgeDlg
// Purpose:	process brightness contrast gamma dialog
// Input:	elx image
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CProcessEdgeDlg::Process( ImageVariant& rElxImage )
{
	if( m_nType == IMAGE_PROCESS_TYPE_EDGE_GRADIENT )
		// 1: ED_Sobel, ED_Prewitt, ED_Kirsch, ED_FreiChen, ED_Laplacian, ED_LoG, ED_MarrHidreth 
		// 2: EG_Accurate
		rElxImage.ApplyGradient( (EEdgeDetector) m_vectChoiceSelected[0], EG_Fast, CM_All );
	else if( m_nType == IMAGE_PROCESS_TYPE_EDGE_ROBERSCROSS )
	{
		ImageVariant rTempElxImage = rElxImage;
		rTempElxImage.ApplyRoberts( true, CM_All );
		rElxImage.Blend( rTempElxImage, m_vectSliderCurrentReal[0] );

	} else if( m_nType == IMAGE_PROCESS_TYPE_EDGE_LAPLACIAN )
		rElxImage.ApplyLaplacian3x3( m_vectSliderCurrentReal[0], m_vectSliderCurrentReal[1], 
									m_vectSliderCurrentReal[2], m_vectSliderCurrentReal[3], BF_Nearest, 1, CM_All );
	else if( m_nType == IMAGE_PROCESS_TYPE_EDGE_MARRHILDRETH )
	{
		ImageVariant rTempElxImage = rElxImage;
		rTempElxImage.ApplyMarrHildreth( BF_Nearest, CM_All );
		rElxImage.Blend( rTempElxImage, m_vectSliderCurrentReal[0] );

	} else if( m_nType == IMAGE_PROCESS_TYPE_EDGE_CANNY )
	{
		// GDS_Fast, GDS_Accurate, GDS_Max2, GDS_Max4.
		rElxImage.ApplyCanny( ED_Sobel, EG_Fast, m_vectSliderCurrentReal[0], m_vectSliderCurrentReal[1], m_vectSliderCurrentReal[2], CM_All );
	}
}
