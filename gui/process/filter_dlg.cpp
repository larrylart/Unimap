
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
#include "filter_dlg.h"

BEGIN_EVENT_TABLE( CProcessFiltersDlg, CImageProcBaseDlg )
END_EVENT_TABLE( )
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CProcessFiltersDlg
// Purpose:	Initialize my 3d view config dialog 
// Input:	pointer to reference window 
// Output:	nothing
////////////////////////////////////////////////////////////////////
CProcessFiltersDlg::CProcessFiltersDlg( wxWindow *parent, int nType, int nViewType, 
										const wxString& strTitle ) : CImageProcBaseDlg( parent, strTitle, nViewType )
{
	m_nType = nType;
	Create( );
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CProcessFiltersDlg
// Purpose:	destroy my  dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CProcessFiltersDlg::~CProcessFiltersDlg( ) 
{

}

////////////////////////////////////////////////////////////////////
// Method:	Create
// Class:	CProcessFiltersDlg
// Purpose:	create  dialog by type
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CProcessFiltersDlg::Create( )
{
	if( m_nType == IMAGE_PROCESS_TYPE_FILTER_MORPHOLOGICAL )
	{
		SetTitle( wxT("Apply Filter :: Morphological") );
		// MFT_Erode, MFT_Dilate, MFT_Median, MFT_Open, MFT_Close, MFT_TopHat
		wxString vectMethods[6] = {wxT("Erode"), wxT("Dilate"), wxT("Median"), wxT("Open"), wxT("Close"), wxT("TopHat") };
		int vectValues[6] = {MFT_Erode, MFT_Dilate, MFT_Median, MFT_Open, MFT_Close, MFT_TopHat};
		AddSelector( "Method:", 6, vectMethods, vectValues );
		AddSlider( "Radius", 0, 10, 1.0, 10.0, 1.0 );
		AddSlider( "Blending", 0, 200, 0.0, 1.0, 1.0 );

	} else if( m_nType == IMAGE_PROCESS_TYPE_FILTER_DIGITAL )
	{
		SetTitle( wxT("Apply Filter :: Digital") );
		AddSlider( "Background", 0, 200, 0.0, 1.0, 0.0 );
		AddSlider( "Cross Over", 0, 200, 0.0, 1.0, 0.90 );
		AddSlider( "Scale", 0, 200, 0.2, 2.8, 1.0 );
		AddSlider( "Variance", 0, 200, 0.1, 3.0, 1.5 );
		// also add here EColorEmphasis

	} else if( m_nType == IMAGE_PROCESS_TYPE_PROCESS_DENOISING )
	{
		SetTitle( wxT("Apply Filter :: Denoising") );
		AddSlider( "Radius", 0, 10, 1.0, 10.0, 1.0 );

	} else if( m_nType == IMAGE_PROCESS_TYPE_PROCESS_DEBLOOMER )
	{
		SetTitle( wxT("Apply Filter :: Debloomer") );
		AddSlider( "Radius", 0, 10, 1.0, 10.0, 1.0 );
		AddSlider( "Variance", 0, 200, 0.05, 5.0, 1.5 );
	}

	Layout( );
	Fit( );
	Refresh( );
}

////////////////////////////////////////////////////////////////////
// Method:	Process
// Class:	CProcessFiltersDlg
// Purpose:	process brightness contrast gamma dialog
// Input:	elx image
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CProcessFiltersDlg::Process( ImageVariant& rElxImage )
{
	if( m_nType == IMAGE_PROCESS_TYPE_FILTER_MORPHOLOGICAL )	
		rElxImage.ApplyMorphological( (EMorphologicalFilterType) m_vectChoiceSelected[0], (int) m_vectSliderCurrentReal[0], 
										true, true, 1, m_vectSliderCurrentReal[1], CM_All );
	else if( m_nType == IMAGE_PROCESS_TYPE_FILTER_DIGITAL )
		rElxImage.ApplyDigitalDevelopment( m_vectSliderCurrentReal[0], m_vectSliderCurrentReal[1], 
											m_vectSliderCurrentReal[2], m_vectSliderCurrentReal[3], CE_Default, CM_All );
	else if( m_nType == IMAGE_PROCESS_TYPE_PROCESS_DENOISING )
		rElxImage.Despeckle( m_vectSliderCurrentReal[0], true, true, 1, CM_All );
	else if( m_nType == IMAGE_PROCESS_TYPE_PROCESS_DEBLOOMER )
		rElxImage.Debloom( CM_All );

}
