
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
#include "colors_dlg.h"

BEGIN_EVENT_TABLE( CProcessColorsDlg, CImageProcBaseDlg )
END_EVENT_TABLE( )
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CProcessColorsDlg
// Purpose:	Initialize my 3d view config dialog 
// Input:	pointer to reference window 
// Output:	nothing
////////////////////////////////////////////////////////////////////
CProcessColorsDlg::CProcessColorsDlg( wxWindow *parent, int nType, int nViewType, 
										const wxString& strTitle ) : CImageProcBaseDlg( parent, strTitle, nViewType )
{
	m_nType = nType;
	Create( );
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CProcessColorsDlg
// Purpose:	destroy my  dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CProcessColorsDlg::~CProcessColorsDlg( ) 
{

}

////////////////////////////////////////////////////////////////////
// Method:	Create
// Class:	CProcessColorsDlg
// Purpose:	create  dialog by type
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CProcessColorsDlg::Create( )
{
	if( m_nType == IMAGE_PROCESS_TYPE_BCG )
	{
		SetTitle( wxT("Brightness/Contrast/Gamma") );
		AddSlider( "Brightness", -50, 50, -1.0, 1.0, 0.0 );
		AddSlider( "Contrast", -50, 50, -1.0, 1.0, 0.0 );
		AddSlider( "Gamma", 1, 101, 0.001, 4.0, 1.0 );

	} else if( m_nType == IMAGE_PROCESS_TYPE_MIDTONE )
	{
		SetTitle( wxT("Midtone") );
		AddSlider( "Midtone", 1, 99, 0.001, 0.999, 0.5 );

	} else if( m_nType == IMAGE_PROCESS_TYPE_BALANCE )
	{
		SetTitle( wxT("White Balance") );
		AddSlider( "Red", 0, 100, 0.0, 2.0, 0.8 );
		AddSlider( "Green", 0, 100, 0.0, 2.0, 1.1 );
		AddSlider( "Blue", 0, 100, 0.0, 2.0, 0.5 );

	} else if( m_nType == IMAGE_PROCESS_TYPE_COLORIZE )
	{
		SetTitle( wxT("Colorize") );
		AddSlider( "Hue", 0, 100, 0.0, 1.0, 0.5 );
		AddSlider( "Saturation", 0, 100, 0.0, 1.0, 0.5 );

	} else if( m_nType == IMAGE_PROCESS_TYPE_DESATURATE )
	{
		SetTitle( wxT("Desaturate") );
		AddSlider( "Factor", 0, 100, 0.0, 1, 0.5 );

	} else if( m_nType == IMAGE_PROCESS_TYPE_HUESATURATION )
	{
		SetTitle( wxT("Adjust Hue Saturation") );
		AddSlider( "Hue", 0, 100, 0.0, 1.0, 0.5 );
		AddSlider( "Saturation", 0, 100, 0.0, 1.0, 0.5 );
	}

	Layout( );
	Fit( );
	Refresh( );
}

////////////////////////////////////////////////////////////////////
// Method:	Process
// Class:	CProcessColorsDlg
// Purpose:	process brightness contrast gamma dialog
// Input:	elx image
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CProcessColorsDlg::Process( ImageVariant& rElxImage )
{
	if( m_nType == IMAGE_PROCESS_TYPE_BCG )
		rElxImage.AdjustBCG( m_vectSliderCurrentReal[0], m_vectSliderCurrentReal[1], m_vectSliderCurrentReal[2], CM_All );
	else if( m_nType == IMAGE_PROCESS_TYPE_MIDTONE )
		rElxImage.AdjustMidtone( m_vectSliderCurrentReal[0], CM_All );
	else if( m_nType == IMAGE_PROCESS_TYPE_BALANCE )
		rElxImage.Balance( m_vectSliderCurrentReal[0], m_vectSliderCurrentReal[1], m_vectSliderCurrentReal[2] );
	else if( m_nType == IMAGE_PROCESS_TYPE_COLORIZE )
		rElxImage.Colorize( m_vectSliderCurrentReal[0], m_vectSliderCurrentReal[1] );
	else if( m_nType == IMAGE_PROCESS_TYPE_DESATURATE )
		rElxImage.Desaturate( m_vectSliderCurrentReal[0], CM_All );
	else if( m_nType == IMAGE_PROCESS_TYPE_HUESATURATION )
		rElxImage.AdjustHueSaturation( m_vectSliderCurrentReal[0], m_vectSliderCurrentReal[1] );
}
