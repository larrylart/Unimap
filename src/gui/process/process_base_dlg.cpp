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
#include "../dynamic_img_view.h"
#include "../astro_img_view.h"

// main header
#include "process_base_dlg.h"


// class:	CDynamicImgPreview
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CDynamicImgPreview
// Purpose:	Initialize my 3d view config dialog 
// Input:	pointer to reference window 
// Output:	nothing
////////////////////////////////////////////////////////////////////
CDynamicImgPreview::CDynamicImgPreview( CImageProcBaseDlg *pImgPreviewDlg,
									   const wxPoint& pos, const wxSize& size ) : 
										CDynamicImgView( pImgPreviewDlg, pos, size )
{
	m_pImgPreviewDlg = pImgPreviewDlg;
}

////////////////////////////////////////////////////////////////////
// Method:	UpdateOnImageCut
// Class:	CDynamicImgPreview
// Purpose:	add this to my image view class to handle what to do when
//			I do zoom or drag
// Input:	pointer to reference window 
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDynamicImgPreview::UpdateOnImageCut( )
{
	// only do when drag is over
	if( m_bDragging == 0 ) m_pImgPreviewDlg->UpdateImage( );
}

// class:	CImageProcBaseDlg
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( CImageProcBaseDlg, wxDialog )
	EVT_SCROLL( CImageProcBaseDlg::OnSlideChange )
	EVT_CHOICE( -1, CImageProcBaseDlg::OnChoiceChange )
END_EVENT_TABLE( )
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CImageProcBaseDlg
// Purpose:	Initialize my 3d view config dialog 
// Input:	pointer to reference window 
// Output:	nothing
////////////////////////////////////////////////////////////////////
CImageProcBaseDlg::CImageProcBaseDlg( wxWindow *parent, const wxString& strTitle, int nViewType )
               : wxDialog(parent, -1,
							strTitle,
							wxDefaultPosition,
							wxSize(200, -1),
							wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	m_nSlider = 0;
	m_nChoice = 0;
	m_bHasChanged = 0;
	m_nViewType = nViewType;
	m_pImagePreview = NULL;

	wxFont smallFont( 6, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT );

	// main dialog sizer
	wxFlexGridSizer *pTopSizer = new wxFlexGridSizer( 2, 1, 5, 5 );
	// sliders sizer
	m_sizerSliders = new wxFlexGridSizer( 4, 2, 10, 2 );
	m_sizerSliders->AddGrowableCol( 1 );
//	m_sizerSliders->Hide();
	// sliders sizer
	m_sizerChoices = new wxFlexGridSizer( 3, 2, 10, 2 );
	m_sizerChoices->AddGrowableCol( 1 );
	m_sizerChoices->AddGrowableCol( 0 );
//	m_sizerChoices->Hide();

	// if view type = 1 do add image preview and extra stuff
	if( m_nViewType == 1 )
	{
		m_pImagePreview = new CDynamicImgPreview( this, wxDefaultPosition, wxSize( 200, 100 ) );
		pTopSizer->AddGrowableRow( 0 );
		pTopSizer->Add( m_pImagePreview, 0, wxGROW | wxALL|wxEXPAND, 10 );
	}

	pTopSizer->Add( m_sizerChoices, 0, wxGROW |wxEXPAND|wxLEFT|wxRIGHT, 10 );
	pTopSizer->Add( m_sizerSliders, 0, wxGROW |wxEXPAND|wxLEFT|wxRIGHT, 10 );
	pTopSizer->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 10 );

	SetSizer(pTopSizer);
	SetAutoLayout(TRUE);
	pTopSizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CImageProcBaseDlg
// Purpose:	destroy my  dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CImageProcBaseDlg::~CImageProcBaseDlg( ) 
{
	int i=0;
	// clear sliders
	for( i=0; i<m_nSlider; i++ ) delete( m_vectSlider[i] );
	// clear choices
	for( i=0; i<m_nChoice; i++ )
	{ 
		delete( m_vectChoice[i] ); 
		delete( m_vectChoiceValue[i] ); 
	} 
	if( m_pImagePreview ) delete( m_pImagePreview );
}

////////////////////////////////////////////////////////////////////
// Method:	AddSlider
// Class:	CImageProcBaseDlg
// Purpose:	add a custom slider
// Input:	name, min max visualize, curent pos vis, min/max real, 
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CImageProcBaseDlg::AddSlider( const char* strName, int min, int max, double f_min, double f_max, double f_current, int bRev )
{
//	m_sizerSliders->Show();

	// store vars
	m_vectSliderDataOrder[m_nSlider] = bRev;
	m_vectSliderStartVisual[m_nSlider] = min;
	m_vectSliderEndVisual[m_nSlider] = max;
	m_vectSliderStartReal[m_nSlider] = f_min;
	m_vectSliderEndReal[m_nSlider] = f_max;
	m_vectSliderCurrentReal[m_nSlider] = f_current;
	// calculate ratio and store
	double nRangeVisual = max-min;
	double nRangeReal = f_max-f_min;
	m_vectSlideVisualToRealRatio[m_nSlider] = (double) (nRangeReal/nRangeVisual);
	int current = (int) (f_current/m_vectSlideVisualToRealRatio[m_nSlider]);
	m_vectSliderCurrentVisual[m_nSlider] = current; 

	// create gui element
	m_vectSlider[m_nSlider] = new wxSlider( this, -1, current, min, max, wxPoint(-1, -1),
										wxSize(-1, 14 ), wxSL_HORIZONTAL /*|wxSL_LABELS*/ );
	// add to sizers
	m_sizerSliders->Add( new wxStaticText( this, -1, wxString( strName,wxConvUTF8 )), 0, wxALIGN_RIGHT | wxALIGN_TOP );
	m_sizerSliders->Add( m_vectSlider[m_nSlider], 1, wxALIGN_LEFT | wxALIGN_BOTTOM |wxEXPAND );

	// increment no of sliders
	m_nSlider++;
}

////////////////////////////////////////////////////////////////////
// Method:	AddSelector
// Class:	CImageProcBaseDlg
// Purpose:	add a selector box
// Input:	label, of of options, array of options string, array of values
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CImageProcBaseDlg::AddSelector( const char* strLabel, int nChoices, 
									const wxString choices[], const int values[] )
{
	int i=0;

	m_vectChoiceCount[m_nChoice] = nChoices;
	// allocate/set values
	m_vectChoiceValue[m_nChoice] = (int*) malloc( nChoices*sizeof(int) );
	for( i=0; i<nChoices; i++ ) m_vectChoiceValue[m_nChoice][i] = values[i];
	
	// create gui element
	m_vectChoice[m_nChoice] = new wxChoice( this, -1, wxDefaultPosition, wxDefaultSize, 
											nChoices, choices );
	m_vectChoice[m_nChoice]->SetSelection( 0 );
	m_vectChoiceSelected[m_nChoice] = m_vectChoiceValue[m_nChoice][0];

	// add to sizers
	m_sizerChoices->Add( new wxStaticText( this, -1, wxString( strLabel,wxConvUTF8 )), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	m_sizerChoices->Add( m_vectChoice[m_nChoice], 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	m_nChoice++;
}

////////////////////////////////////////////////////////////////////
// Method:	SetImage
// Class:	CImageProcBaseDlg
// Purpose:	set date image/view local - for now in ImageVariant format
// Input:	pointer to event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CImageProcBaseDlg::SetImage( wxImage& pImage, CAstroImgView* pImgView )
{
	m_pImgView = pImgView;

	// get abstract image info
	const uint32 w = pImage.GetWidth();
	const uint32 h = pImage.GetHeight();
    const uint32 size = w*h*3;

	// check if to set my image
	if( m_nViewType == 1 )
	{
		double rat = (double) 300.0/(double) w;
		int n_h = (int) (h*rat + 0.5);
		m_pImagePreview->SetMinSize( wxSize( 300, n_h+1 ) );
		m_pImagePreview->SetWxImage( &pImage, 1 );
		
		// update layout
		m_sizerSliders->Layout( );
		m_sizerChoices->Layout( );
		Layout( );
		Fit( );
		Refresh( );
	}

/***** initial all copy  ---- replaced by call copy
	// create rgb image
	boost::shared_ptr<ImageRGBub> spImage( new ImageRGBub(w, h) );
    ubyte* prDst = (ubyte*) spImage->GetPixel(); 
    ubyte* prSrc = (ubyte*) pImage.GetData();

	/////////
    // copy from wxImage to ImageVariant
    ::memcpy( (void*) prDst, (const void*) prSrc, size );
	// set back image
	m_pElxImage = *spImage;
*/

	// update image
	UpdateImage( );
}

////////////////////////////////////////////////////////////////////
// Method:	UpdateImage
void CImageProcBaseDlg::UpdateImage( )
{
	if( m_nViewType == 1 )
	{
		m_pElxImage = m_pImagePreview->GetElxBitmap( );
		//m_pImagePreview->m_bCutUpdated = 0;

	} else
	{
		m_pElxImage = m_pImgView->GetElxBitmap( );
		//m_pImgView->m_bCutUpdated = 0;
	}
	// also call process here
	DoProcess( );
}

////////////////////////////////////////////////////////////////////
// Method:	DoProcess
// Class:	CImageProcBaseDlg
// Purpose:	process image with my curent filter
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CImageProcBaseDlg::DoProcess( )
{
	m_bHasChanged = 1;
	int i=0;

	// check sliders
	for( i=0; i<m_nSlider; i++ ) 
	{
		int val = m_vectSlider[i]->GetValue( );
		// check data order - reverse values if bla bla 
		if( m_vectSliderDataOrder[i] == 1 )
			val = m_vectSliderEndVisual[i]-(val-m_vectSliderStartVisual[i]);

		double f_val = (double) val*m_vectSlideVisualToRealRatio[i];
		m_vectSliderCurrentReal[i] = f_val;
	}

	// check choices
	for( i=0; i<m_nChoice; i++ ) 
	{
		int sel_id = m_vectChoice[i]->GetSelection( );
		m_vectChoiceSelected[i] = m_vectChoiceValue[i][sel_id];
	}


	// check if there was an update in the view - zoom or drag
/*	if( m_nViewType == 1 )
	{
		if( m_pImagePreview->m_bCutUpdated )
		{
			m_pElxImage = m_pImagePreview->GetElxBitmap( );
			//m_pImagePreview->m_bCutUpdated = 0;
		}

	} else
	{
		if( m_pImgView->m_bCutUpdated )
		{
			m_pElxImage = m_pImgView->GetElxBitmap( );
			//m_pImgView->m_bCutUpdated = 0;
		}
	}
*/

	ImageVariant rElxImage = m_pElxImage; //m_pImgView->GetElxImage( );

//	ImageVariant rElxImage = m_pImgView->GetElxBitmap( );

	//rElxImage.AdjustBCG( m_nBrightness, m_nContrast, m_nGamma, CM_All );
	Process( rElxImage );

//	m_pImgView->SetElxImage( &rElxImage );
	if( m_nViewType == 1 )
		m_pImagePreview->SetElxBitmap( &rElxImage );
	else
		m_pImgView->SetElxBitmap( &rElxImage );

/*	// get abstract image info
	const uint w = m_pImgView->m_pImage.GetWidth();
	const uint h = m_pImgView->m_pImage.GetHeight();
    const uint size = w*h*3;

	boost::shared_ptr<ImageRGBub> spImage = elxCreateImageRGBub( *m_pElxImage->GetImpl() );
	ubyte* prDst = (ubyte*) spImage->GetPixel(); 
	ubyte* prSrc = (ubyte*) m_pImgView->m_pImage.GetData();

	::memcpy( prSrc, (void*) prDst, size );

	m_pImgView->UpdateImage( );
*/
}

////////////////////////////////////////////////////////////////////
// Method:	OnScrollChange
// Class:	CImageProcBaseDlg
// Purpose:	when one of the sliders was changed
// Input:	pointer to event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CImageProcBaseDlg::OnSlideChange( wxScrollEvent& pEvent )
{
	DoProcess( );
}

////////////////////////////////////////////////////////////////////
// Method:	OnChoiceChange
// Class:	CImageProcBaseDlg
// Purpose:	when one of the choices was changed
// Input:	pointer to event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CImageProcBaseDlg::OnChoiceChange( wxCommandEvent& pEvent )
{
	DoProcess( );
}


////////////////////////////////////////////////////////////////////
// Method:	ProcessImage
// Class:	CImageProcBaseDlg
// Purpose:	process full image - set remotely with current data
// Input:	pointer to event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CImageProcBaseDlg::ProcessImage( wxImage& pImage )
{
	ImageVariant rElxImage = m_pImgView->GetElxImage( );
	Process( rElxImage );
	m_pImgView->SetElxImage( &rElxImage );
}
