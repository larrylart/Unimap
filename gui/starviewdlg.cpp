////////////////////////////////////////////////////////////////////
// Package:		Star View Dialog implementation
// File:		starviewdlg.cpp
// Purpose:		create dialog to to show object/star details
//
// Created by:	Larry Lart on 16-09-2007
// Updated by:	Larry Lart on 29-01-2010 - clean/brak in parts
//
// Copyright:	(c) 2007-2010 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

// system libs
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

#include "wx/wxprec.h"
#include "wx/statline.h"
#include "wx/minifram.h"
#include "wx/thread.h"
#include "wx/notebook.h"
#include <wx/file.h>
#include <wx/string.h>
#include <wx/filesys.h>
#include <wx/html/htmlpars.h>
#include <wx/fs_inet.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/regex.h>
#include <wx/uri.h>
#include <wx/tokenzr.h>

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// other wxCode modules
#include "../include/plotctrl/plotctrl.h"

// local headers
#include "../util/func.h"
#include "../util/folders.h"
#include "../config/mainconfig.h"
#include "../sky/sky.h"
#include "../sky/catalog_stars_names.h"
#include "../sky/catalog_dso_names.h"
#include "../sky/simbad.h"
#include "../sky/bibdb.h"
#include "../sky/skyview_site.h"
#include "../image/astroimage.h"
#include "../spectrum/spectrum.h"
#include "../unimap_worker.h"
#include "waitdialog.h"
#include "HistogramView.h"
#include "RadialProfileView.h"
#include "waitdialog.h"
//#include "../httpengine/httpbuilder.h"

// main header
#include "starviewdlg.h"

// Class :: CCustomHtmlWindow
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CCustomHtmlWindow
// Purose:	build my CCustomHtmlWindow object  
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CCustomHtmlWindow::CCustomHtmlWindow( wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size ):
			wxHtmlWindow( parent, id, pos, size )
{
	SetMinSize( wxSize( -1, 130 ) );
}

////////////////////////////////////////////////////////////////////
// Method:	OnLinkClicked
// Class:	CCustomHtmlWindow
// Purose:	when link was clicked
// Input:	reference to link info
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCustomHtmlWindow::OnLinkClicked( const wxHtmlLinkInfo& link )
{
	wxString strMyUrl = link.GetHref();
	wxLaunchDefaultBrowser( strMyUrl, wxBROWSER_NEW_WINDOW );
	return;
}

// Class :: CObjectView
////////////////////////////////////////////////////////////////////
// implement message map
BEGIN_EVENT_TABLE(CObjectView, wxWindow)
	EVT_PAINT( CObjectView::OnPaint )
	// window events
	EVT_SIZE( CObjectView::OnSize ) 
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CObjectView
// Purose:	build my CObjectView object  
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CObjectView::CObjectView( wxWindow *pWindow, const wxPoint& pos, const wxSize& size ):
			wxWindow(pWindow, -1, pos, size, wxSIMPLE_BORDER, wxT( "Object view" ) )
{
	m_bSizeIsRunning = 0;
	m_bHasImage = 0;
	m_nWidth = size.GetWidth();
	m_nHeight = size.GetHeight();
	m_nPicWidth = m_nWidth;
	m_nPicHeight = m_nHeight;
	m_nScaleFactor = 1;
	m_nOrigPicWidth = -1;
	m_nOrigPicHeight = -1;
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CObjectView
// Purose:	destroy my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CObjectView::~CObjectView( )
{
//	m_pBitmap.Destroy( );
}

////////////////////////////////////////////////////////////////////
// Method:	OnPaint
// Class:	CObjectView
// Purose:	on paint event
// Input:	reference to paint event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CObjectView::OnPaint( wxPaintEvent& event )
{
	wxPaintDC dc(this);

	dc.BeginDrawing();
	dc.Clear( );

	dc.DrawBitmap( m_pBitmap, 0 , 0 );

	dc.EndDrawing();
	
	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetImage
// Class:	CObjectView
// Purose:	CObjectView set image
// Input:	reference to image object
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CObjectView::SetImage( wxImage& pImage )
{
	m_nOrigPicWidth = pImage.GetWidth();
	m_nOrigPicHeight = pImage.GetHeight();

	// copy image localy
	m_pBitmap = wxBitmap( pImage.Rescale( m_nPicWidth, m_nPicHeight ) );

	m_bHasImage = 1;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnSize
// Class:	CObjectView
// Purose:	adjust on windows resize
// Input:	reference to size event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CObjectView::OnSize( wxSizeEvent& even )
{
	even.Skip();

	int nWidth = even.GetSize().GetWidth();
	int nHeight = even.GetSize().GetHeight();

	if( m_bSizeIsRunning  == 1 ) return;
	m_bSizeIsRunning = 1;

	m_nWidth = nWidth;
	m_nHeight = nHeight;

	m_nPicWidth = m_nWidth;
	m_nPicHeight = m_nHeight;

	// if no image - don't adjust
	if( !m_bHasImage )
	{
		double nPicProp = (double) m_nOrigPicWidth/m_nOrigPicHeight;
		double nWinProp = (double) m_nWidth/m_nHeight;
		if( nPicProp <= nWinProp )
		{
			m_nScaleFactor = (double) m_nHeight/m_nOrigPicHeight;
			m_nPicWidth = (int) m_nOrigPicWidth*m_nScaleFactor;

			m_nPicHeight = m_nHeight;

	//		m_nPicOrigX = (int) ( (m_nWidth/2) - (m_nPicWidth/2) );
	//		m_nPicOrigY = 0;

		} else
		{
			m_nScaleFactor = (double) m_nWidth/m_nOrigPicWidth;
			m_nPicHeight = (int) m_nOrigPicHeight*m_nScaleFactor;

			m_nPicWidth = m_nWidth;

	//		m_nPicOrigY = (int) ( (m_nHeight/2) - (m_nPicHeight/2) );
	//		m_nPicOrigX = 0;

		}

		SetSize( -1, -1, m_nPicWidth, m_nPicHeight );
	}

	m_bSizeIsRunning = 0;

	// if no image has been set yet just return
	if( m_bHasImage == 0 )
	{
		return;
	}

	// rescape with new size
	m_pBitmap = wxBitmap( m_pBitmap.ConvertToImage().Rescale( m_nPicWidth, m_nPicHeight ) );


//	m_bSizeIsRunning = 0;
}

// Class :: CStarViewDlg
////////////////////////////////////////////////////////////////////
// CStarViewDlg EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( CStarViewDlg, wxDialog )
	EVT_CHOICE( wxID_SELECT_IMAGE_SOURCE, CStarViewDlg::OnSelectImageSource )
	EVT_CHOICE( wxID_SELECT_IMAGE_CHANNEL_TYPE, CStarViewDlg::OnSelectImageChannelType )
	EVT_CHOICE( wxID_SELECT_IMAGE_SURVEY_TYPE, CStarViewDlg::OnSelectImageSurveyType )
	EVT_CHOICE( wxID_SELECT_IMAGE_SURVEY, CStarViewDlg::OnSelectImageSurvey )
	EVT_CHOICE( wxID_SELECT_IMAGE_COLOR_TABLE, CStarViewDlg::OnSelectImageSurvey )
	EVT_CHOICE( wxID_SELECT_IMAGE_CATALOG_OVERLAY, CStarViewDlg::OnSelectCatalogOverlay )
	EVT_CHOICE( wxID_SELECT_IMAGE_CONTOUR_OVERLAY, CStarViewDlg::OnSelectImageSurvey )
	EVT_CHOICE( wxID_SELECT_IMAGE_GRID_OVERLAY, CStarViewDlg::OnSelectImageSurvey )
	// data stuff
	EVT_CHOICE( wxID_SELECT_OBJECT_SYSTEM_COORD_TYPE, CStarViewDlg::OnSelectCoordType )
	EVT_CHOICE( wxID_SELECT_REFERENCES_BIBCODE, CStarViewDlg::OnSelectBibcode )
	EVT_CHOICE( wxID_SELECT_MEASURE_NAME, CStarViewDlg::OnSelectMeasurement )
	EVT_CHOICE( wxID_SELECT_SPECTRUM_NAME, CStarViewDlg::OnSelectSpectrum )
	EVT_BUTTON( wxID_BUTTON_REFERENCES_VIEWFORMAT_BUTTON, CStarViewDlg::OnViewReferenceDoc )
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CStarViewDlg
// Purpose:	Initialize my about dialog // 
// Input:	pointer to reference window 
// Output:	nothing
////////////////////////////////////////////////////////////////////
CStarViewDlg::CStarViewDlg( CSky* pSky, wxWindow *parent, const wxString& title, CUnimapWorker* pUnimapWorker )
							: wxDialog(parent, -1, title, wxDefaultPosition,
								wxSize( 600, 400 ), wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	m_pSky = pSky;
	m_pUnimapWorker = pUnimapWorker;
	// set flags 
	m_bHasName = 0;

	// select default none
	wxString vectDefaultNone[1];
	vectDefaultNone[0] = wxT("none");
	// select image source
	wxString vectUseImageSource[2];
	vectUseImageSource[0] = wxT( "Local Image" );
	vectUseImageSource[1] = wxT( "Nasa SkyView" );
	// select image channel type
	wxString vectRemImageChannel[2];
	vectRemImageChannel[0] = wxT( "Single" );
	vectRemImageChannel[1] = wxT( "Color RGB" );
	// remote image params
	wxString vectRemImageType[8];
	vectRemImageType[0] = wxT( "Optical" );
	vectRemImageType[1] = wxT( "Infrared" );
	vectRemImageType[2] = wxT( "UV" );
	vectRemImageType[3] = wxT( "Radio" );
	vectRemImageType[4] = wxT( "X-ray" );
	vectRemImageType[5] = wxT( "Hard X-ray" );
	vectRemImageType[6] = wxT( "Diffuse X-ray" );
	vectRemImageType[7] = wxT( "Gamma Ray" );
	// survey image
	wxString vectSurveyImage[1];
	vectSurveyImage[0] = wxT( "None" );

	// grid overlay
	wxString vectRemImageGridOverlay[10];
	vectRemImageGridOverlay[0] = wxT( "No Grid" );
	vectRemImageGridOverlay[1] = wxT( "Same As Image" );
	vectRemImageGridOverlay[2] = wxT( "J2000" );
	vectRemImageGridOverlay[3] = wxT( "B1950" );
	vectRemImageGridOverlay[4] = wxT( "Galactic" );
	vectRemImageGridOverlay[5] = wxT( "E2000" );
	vectRemImageGridOverlay[6] = wxT( "ICRS" );
	// contour overlay
	wxString vectRemImageContourOverlay[1];
	vectRemImageContourOverlay[0] = wxT( "None" );
	// catalog overlay
	wxString vectRemImageCatalogOverlay[50];
	vectRemImageCatalogOverlay[0] = wxT( "None" );
	// rgb
	wxString vectRemImageRGBCompRed[1];
	vectRemImageRGBCompRed[0] = wxT( "None" );
	wxString vectRemImageRGBCompGreen[1];
	vectRemImageRGBCompGreen[0] = wxT( "None" );
	wxString vectRemImageRGBCompBlue[1];
	vectRemImageRGBCompBlue[0] = wxT( "None" );
	// color table
	wxString vectRemImageColorTable[1];
	vectRemImageColorTable[0] = wxT( "None" );
	// coord 
	wxString vectCoordType[1];
	vectCoordType[0] = wxT( "ICRS,ep=2000,eq=2000" );
	// measures
	wxString vectMeasuresName[1];
	vectMeasuresName[0] = wxT( "None" );

	// main dialog sizer
	wxFlexGridSizer *topsizer = new wxFlexGridSizer( 3, 1, 5, 1 );
//	topsizer->AddGrowableRow( 1 );

	// sizer head
	wxFlexGridSizer *sizerHead = new wxFlexGridSizer( 1, 2, 0, 0 );
	sizerHead->AddGrowableCol( 1 );
	topsizer->Add( sizerHead, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL |wxGROW|wxBOTTOM | wxTOP, 2 );

	// sizer head data
	m_pSizerHeadDataTop = new wxFlexGridSizer( 1, 1, 0, 0 );
	sizerHead->Add( m_pSizerHeadDataTop, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL |wxGROW|wxBOTTOM | wxTOP, 10 );

	m_pSizerHeadData = new wxGridSizer( 2, 2, 4, 10 );
	m_pSizerHeadDataTop->Add( m_pSizerHeadData, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL |wxGROW |wxLEFT|wxBOTTOM, 10 );
	
	//////////
	// create image head data elemens
	m_pUseImageSource = new wxChoice( this, wxID_SELECT_IMAGE_SOURCE,
										wxDefaultPosition,
										wxSize(100,-1), 2, vectUseImageSource, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pUseImageSource->SetSelection( 0 );
	// checkbox if to use RGB
	m_pRemImageChannel = new wxChoice( this, wxID_SELECT_IMAGE_CHANNEL_TYPE,
										wxDefaultPosition,
										wxSize(100,-1), 2, vectRemImageChannel, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pRemImageChannel->SetSelection( 0 );

	// image type
	m_pRemImageType = new wxChoice( this, wxID_SELECT_IMAGE_SURVEY_TYPE,
										wxDefaultPosition,
										wxSize(80,-1), 8, vectRemImageType, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pRemImageType->SetSelection( 0 );
	// image survey
	m_pRemImageSurvey = new wxChoice( this, wxID_SELECT_IMAGE_SURVEY,
										wxDefaultPosition,
										wxSize(150,-1), 1, vectSurveyImage, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pRemImageSurvey->SetSelection( 0 );
	// color table
	m_pRemImageColorTable = new wxChoice( this, wxID_SELECT_IMAGE_COLOR_TABLE,
										wxDefaultPosition,
										wxSize(150,-1), 1, vectRemImageColorTable, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pRemImageColorTable->SetSelection( 0 );
	// Grid Overlay
	m_pRemImageGridOverlay = new wxChoice( this, wxID_SELECT_IMAGE_GRID_OVERLAY,
										wxDefaultPosition,
										wxSize(150,-1), 7, vectRemImageGridOverlay, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pRemImageGridOverlay->SetSelection( 0 );
	// Contour Overlays
	m_pRemImageContourOverlay = new wxChoice( this, wxID_SELECT_IMAGE_CONTOUR_OVERLAY,
										wxDefaultPosition,
										wxSize(150,-1), 1, vectRemImageContourOverlay, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pRemImageContourOverlay->SetSelection( 0 );
	// Catalog Overlay
	m_pRemImageCatalogOverlay = new wxChoice( this, wxID_SELECT_IMAGE_CATALOG_OVERLAY,
										wxDefaultPosition,
										wxSize(150,-1), 1, vectRemImageCatalogOverlay, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pRemImageCatalogOverlay->SetSelection( 0 );
	// Rgb Image
	// red comp
	m_pRemImageRGBCompRed = new wxChoice( this, wxID_SELECT_IMAGE_CATALOG_OVERLAY,
										wxDefaultPosition,
										wxSize(140,-1), 1, vectRemImageRGBCompRed, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pRemImageRGBCompRed->SetSelection( 0 );
	// green comp
	m_pRemImageRGBCompGreen = new wxChoice( this, wxID_SELECT_IMAGE_CATALOG_OVERLAY,
										wxDefaultPosition,
										wxSize(140,-1), 1, vectRemImageRGBCompGreen, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pRemImageRGBCompGreen->SetSelection( 0 );
	// blue comp
	m_pRemImageRGBCompBlue = new wxChoice( this, wxID_SELECT_IMAGE_CATALOG_OVERLAY,
										wxDefaultPosition,
										wxSize(140,-1), 1, vectRemImageRGBCompBlue, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pRemImageRGBCompBlue->SetSelection( 0 );

	//////////
	// set head elements into the sizer
	// use local/remote
	m_pSizerHeadData->Add(new wxStaticText( this, -1, wxT("Image Source:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	m_pSizerHeadData->Add( m_pUseImageSource, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// choose display mode
	m_pSizerHeadData->Add(new wxStaticText( this, -1, wxT("Image Channels:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);	
	m_pSizerHeadData->Add( m_pRemImageChannel, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	///////
	// One Channel
	m_pImgDataSurveyBox = new wxStaticBox( this, -1, wxT("One Channel"), wxPoint(-1,-1), wxDefaultSize );
	m_pImgDataSurveyBoxSizer = new wxStaticBoxSizer( m_pImgDataSurveyBox, wxHORIZONTAL );
	m_pSizerHeadDataTop->Add( m_pImgDataSurveyBoxSizer, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxGROW|wxLEFT, 10 );
	// survey data sizer
	m_pSizerHeadDataSurvey = new wxFlexGridSizer( 3, 2, 4, 4 );
	m_pImgDataSurveyBoxSizer->Add( m_pSizerHeadDataSurvey, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL  );

	// image type
	m_pSizerHeadDataSurvey->Add(new wxStaticText( this, -1, wxT("       Survey Type:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	m_pSizerHeadDataSurvey->Add( m_pRemImageType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// image survey
	m_pSizerHeadDataSurvey->Add(new wxStaticText( this, -1, wxT("Survey Name:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	m_pSizerHeadDataSurvey->Add( m_pRemImageSurvey, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// color table
	m_pSizerHeadDataSurvey->Add(new wxStaticText( this, -1, wxT("Color Table:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	m_pSizerHeadDataSurvey->Add( m_pRemImageColorTable, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	////////////////////////////
	// rgb section - sizerHeadDataTop
	// rgb choser
	m_pImgDataRgbBox = new wxStaticBox( this, -1, wxT("RGB Channels"), wxPoint(-1,-1), wxDefaultSize );
	m_pImgDataRgbBoxSizer = new wxStaticBoxSizer( m_pImgDataRgbBox, wxHORIZONTAL );
	m_pSizerHeadDataTop->Add( m_pImgDataRgbBoxSizer, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxGROW|wxLEFT, 10  );
	// rgb data sizer
	m_pSizerHeadDataRgb = new wxFlexGridSizer( 3, 2, 4, 4 );
	m_pImgDataRgbBoxSizer->Add( m_pSizerHeadDataRgb, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL  );
	// rgb comp
	m_pSizerHeadDataRgb->Add(new wxStaticText( this, -1, wxT("                     Red:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);	
	m_pSizerHeadDataRgb->Add( m_pRemImageRGBCompRed, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	m_pSizerHeadDataRgb->Add(new wxStaticText( this, -1, wxT("Green:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);	
	m_pSizerHeadDataRgb->Add( m_pRemImageRGBCompGreen, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	m_pSizerHeadDataRgb->Add(new wxStaticText( this, -1, wxT("Blue:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);	
	m_pSizerHeadDataRgb->Add( m_pRemImageRGBCompBlue, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	
	m_pSizerHeadDataTop->Hide( m_pImgDataRgbBoxSizer, 1 );

	///////
	// Overlays
	m_pImgDataOverlayBox = new wxStaticBox( this, -1, wxT("Overlays"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* pImgDataOverlayBoxSizer = new wxStaticBoxSizer( m_pImgDataOverlayBox, wxHORIZONTAL );
	m_pSizerHeadDataTop->Add( pImgDataOverlayBoxSizer, 1, wxALIGN_CENTER | wxALIGN_BOTTOM | wxGROW|wxLEFT, 10  );
	// rgb data sizer
	m_pSizerHeadDataOverlay = new wxFlexGridSizer( 3, 2, 4, 4 );
	pImgDataOverlayBoxSizer->Add( m_pSizerHeadDataOverlay, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL  );

	// grid overlay
	m_pSizerHeadDataOverlay->Add(new wxStaticText( this, -1, wxT("Grid Overlay:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	m_pSizerHeadDataOverlay->Add( m_pRemImageGridOverlay, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// Contour Overlay
	m_pSizerHeadDataOverlay->Add(new wxStaticText( this, -1, wxT("Contour Overlay:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	m_pSizerHeadDataOverlay->Add( m_pRemImageContourOverlay, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// Catalog Overlay
	m_pSizerHeadDataOverlay->Add(new wxStaticText( this, -1, wxT("Catalog Overlay:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	m_pSizerHeadDataOverlay->Add( m_pRemImageCatalogOverlay, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	////////////////////////
	// by default disable all skyview panels
	DisableRemoteImagePanel( );

	// image view
	m_pObjView = new CObjectView( this, wxDefaultPosition,  wxSize( 250, 250 ) ); 
	sizerHead->Add( m_pObjView, 0, wxALIGN_CENTER |wxGROW| wxALL, 5 );

	//////////////////////////////////////////
	// Data panel 
	m_pNotebook = new wxNotebook( this, -1 );
	// CREATE PANELS
	// add Identifiers panel
	m_pPanelDetails = new wxPanel( m_pNotebook );
	// add Identifiers panel
	m_pPanelIdentifiers = new wxPanel( m_pNotebook );
	// add References panel
	m_pPanelReferences = new wxPanel( m_pNotebook );
	// add Measures panel
	m_pPanelMeasures = new wxPanel( m_pNotebook );
	// add Notes panel
	m_pPanelNotes = new wxPanel( m_pNotebook );
	// add Notes panel
	m_pPanelSpectrum = new wxPanel( m_pNotebook );
	// add Local panel
	m_pPanelLocal = new wxPanel( m_pNotebook );
	// add Histogram panel
	m_pPanelHist = new wxPanel( m_pNotebook );
	// add Radial Profile panel
	m_pPanelRadial = new wxPanel( m_pNotebook );

	//////////////////////////////////////////////////
	// create Details sizer
	wxGridSizer* pDetailsSizer = new wxGridSizer( 1, 1, 10, 10 );
	
	wxFlexGridSizer* pDetailsTopSizer = new wxFlexGridSizer( 1, 1, 10, 10 );
	pDetailsTopSizer->AddGrowableCol( 0 );
	pDetailsSizer->Add( pDetailsTopSizer, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL  );

	wxFlexGridSizer* pDetailsDataTopSizer = new wxFlexGridSizer( 1, 5, 5, 5 );
	pDetailsDataTopSizer->AddGrowableCol( 0 );
	pDetailsDataTopSizer->AddGrowableCol( 4 );
	pDetailsTopSizer->Add( pDetailsDataTopSizer, 1,
                wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL |wxGROW|wxTOP|wxBOTTOM, 5 );

	wxFlexGridSizer* pDetailsDataSizer = new wxFlexGridSizer( 10, 4, 5, 5 );
	pDetailsDataSizer->AddGrowableCol( 1 );
	pDetailsDataSizer->AddGrowableCol( 3 );
	pDetailsTopSizer->Add( pDetailsDataSizer, 1,
                wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL |wxBOTTOM, 5 );

	////////////////////////////////////
	// details sizer elements
	// coord elements
	m_pCoordType =  new wxChoice( m_pPanelDetails, wxID_SELECT_OBJECT_SYSTEM_COORD_TYPE,
										wxDefaultPosition,
										wxSize(150,-1), 1, vectCoordType, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pCoordType->SetSelection( 0 );
	m_pCoordValue = new wxStaticText( m_pPanelDetails, -1, wxT(""));
	// other
	m_pName = new wxStaticText( m_pPanelDetails, -1, wxT("na"));
	m_pType = new wxStaticText( m_pPanelDetails, -1, wxT("na"));
	m_pProperMotions = new wxStaticText( m_pPanelDetails, -1, wxT("na"));
	m_pParallax = new wxStaticText( m_pPanelDetails, -1, wxT("na"));
	m_pRadialVelocityLabel = new wxStaticText( m_pPanelDetails, -1, wxT("Radial Velocity:"));
	m_pRadialVelocity = new wxStaticText( m_pPanelDetails, -1, wxT("na"));
	m_pSpectralType = new wxStaticText( m_pPanelDetails, -1, wxT("na"));
	m_pRedshift = new wxStaticText( m_pPanelDetails, -1, wxT("na"));

	m_pMorphologicalType = new wxStaticText( m_pPanelDetails, -1, wxT("na"));
	m_pAngularSize = new wxStaticText( m_pPanelDetails, -1, wxT("na"));
	m_pFluxB = new wxStaticText( m_pPanelDetails, -1, wxT("na"));
	m_pFluxV = new wxStaticText( m_pPanelDetails, -1, wxT("na"));
	m_pFluxJ = new wxStaticText( m_pPanelDetails, -1, wxT("na"));
	m_pFluxH = new wxStaticText( m_pPanelDetails, -1, wxT("na"));
	m_pFluxK = new wxStaticText( m_pPanelDetails, -1, wxT("na"));


	//////////////////////////////////////
	// populate details sizer data
	// Coord:
	pDetailsDataTopSizer->AddStretchSpacer(1);
	pDetailsDataTopSizer->Add(new wxStaticText( m_pPanelDetails, -1, wxT("Coordinates:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	pDetailsDataTopSizer->Add( m_pCoordType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	pDetailsDataTopSizer->Add( m_pCoordValue, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	pDetailsDataTopSizer->AddStretchSpacer();
	// Name:
	pDetailsDataSizer->Add(new wxStaticText( m_pPanelDetails, -1, wxT("Name:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	pDetailsDataSizer->Add( m_pName, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// Morphological type:
	pDetailsDataSizer->Add(new wxStaticText( m_pPanelDetails, -1, wxT("Morphological type:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	pDetailsDataSizer->Add( m_pMorphologicalType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	// Type:
	pDetailsDataSizer->Add(new wxStaticText( m_pPanelDetails, -1, wxT("Type:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	pDetailsDataSizer->Add( m_pType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// Angular size:
	pDetailsDataSizer->Add(new wxStaticText( m_pPanelDetails, -1, wxT("Angular size:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	pDetailsDataSizer->Add( m_pAngularSize, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );


	// Proper motions:
	pDetailsDataSizer->Add(new wxStaticText( m_pPanelDetails, -1, wxT("Proper motions:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	pDetailsDataSizer->Add( m_pProperMotions, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// Flux B:
	pDetailsDataSizer->Add(new wxStaticText( m_pPanelDetails, -1, wxT("Flux B:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	pDetailsDataSizer->Add( m_pFluxB, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	// Parallax:
	pDetailsDataSizer->Add(new wxStaticText( m_pPanelDetails, -1, wxT("Parallax:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	pDetailsDataSizer->Add( m_pParallax, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// Flux V:
	pDetailsDataSizer->Add(new wxStaticText( m_pPanelDetails, -1, wxT("Flux V:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	pDetailsDataSizer->Add( m_pFluxV, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );


	// Radial Velocity:
	pDetailsDataSizer->Add( m_pRadialVelocityLabel, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	pDetailsDataSizer->Add( m_pRadialVelocity, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// Flux J:
	pDetailsDataSizer->Add(new wxStaticText( m_pPanelDetails, -1, wxT("Flux J:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	pDetailsDataSizer->Add( m_pFluxJ, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );


	// Spectral Type:
	pDetailsDataSizer->Add(new wxStaticText( m_pPanelDetails, -1, wxT("Spectral Type:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	pDetailsDataSizer->Add( m_pSpectralType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// Flux H:
	pDetailsDataSizer->Add(new wxStaticText( m_pPanelDetails, -1, wxT("Flux H:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	pDetailsDataSizer->Add( m_pFluxH, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	// Redshift:
	pDetailsDataSizer->Add(new wxStaticText( m_pPanelDetails, -1, wxT("Redshift:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	pDetailsDataSizer->Add( m_pRedshift, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// Flux K:
	pDetailsDataSizer->Add(new wxStaticText( m_pPanelDetails, -1, wxT("Flux K:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	pDetailsDataSizer->Add( m_pFluxK, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	// call layout
//	pDetailsDataSizer->Fit(pPanelDetails);
//	pDetailsDataSizer->RecalcSizes();

	//////////////////////////////////////////////////
	// create Identifiers sizer
	wxGridSizer* pIdentifiersSizer = new wxGridSizer( 1, 1, 10, 10 );
	// layout 
	m_pIdentifiersDataSizer = new wxFlexGridSizer( 10, 6, 5, 5 );
	m_pIdentifiersDataSizer->AddGrowableCol( 1 );
	m_pIdentifiersDataSizer->AddGrowableCol( 3 );
	m_pIdentifiersDataSizer->AddGrowableCol( 5 );

	pIdentifiersSizer->Add( m_pIdentifiersDataSizer, 1,
                wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL |wxTOP |wxBOTTOM, 5 );

	//////////////////////////////////////////////////
	// create refereces/bibcodes sizer
	pReferencesSizer = new wxGridSizer( 1, 1, 10, 10 );;
	// layout 
	wxFlexGridSizer* pReferencesDataSizer = new wxFlexGridSizer( 5, 1, 5, 5 );
	pReferencesDataSizer->AddGrowableCol( 0 );
	pReferencesDataSizer->AddGrowableRow( 2 );

	pReferencesSizer->Add( pReferencesDataSizer, 1,
                wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW );

	// create elements for references "panel"
	m_pRefBibcode = new wxChoice( m_pPanelReferences, wxID_SELECT_REFERENCES_BIBCODE,
										wxDefaultPosition,
										wxSize(140,-1), 1, vectDefaultNone, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pRefBibcode->SetSelection( 0 );

	m_pRefViewFormatButton = new wxButton( m_pPanelReferences, wxID_BUTTON_REFERENCES_VIEWFORMAT_BUTTON, wxT("View Document"),
										wxPoint( -1, -1 ),
										wxSize( -1, -1 ) );

	m_pRefViewFormat = new wxChoice( m_pPanelReferences, wxID_SELECT_REFERENCES_VIEWFORMAT,
										wxDefaultPosition,
										wxSize(150,-1), 1, vectDefaultNone, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pRefViewFormat->SetSelection( 0 );

	m_pRefTitle = new wxStaticText( m_pPanelReferences, -1, wxT("") );
	m_pRefJournal = new wxStaticText( m_pPanelReferences, -1, wxT("") );
	m_pRefAuthor = new wxStaticText( m_pPanelReferences, -1, wxT("") );

	// summary box
//	m_pRefSummary = new wxStaticText( m_pPanelReferences, -1, wxT("") );
//	m_pRefSummary->Wrap( 400 );
//	m_pRefSummary = new wxTextCtrl( m_pPanelReferences, -1, wxT(""),
//			wxPoint(-1, -1), wxSize(-1,130), wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH2|wxTE_AUTO_URL|wxTE_NOHIDESEL );
	
	m_pRefSummary = new wxHtmlWindow( m_pPanelReferences, -1, wxPoint(-1, -1), wxSize(-1,130) );

	// add elements to layout
	wxStaticBox* pRefBrowseBox = new wxStaticBox( m_pPanelReferences, -1, wxT("Browse"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* pRefBoxBrowseSizer = new wxStaticBoxSizer( pRefBrowseBox, wxHORIZONTAL );

	wxFlexGridSizer* pRefHeaderA = new wxFlexGridSizer( 1, 4, 10, 5 );
	pRefHeaderA->AddGrowableCol( 0 );
	pRefHeaderA->AddGrowableCol( 3 );

	pRefBoxBrowseSizer->Add( pRefHeaderA, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW );

	pReferencesDataSizer->Add( pRefBoxBrowseSizer, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW );
	// bibcodes
	wxBoxSizer* pRefDataBibcodes = new wxBoxSizer( wxHORIZONTAL );
	pRefDataBibcodes->Add( new wxStaticText( m_pPanelReferences, -1, wxT("Bibcode: ")), 0,
											wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pRefDataBibcodes->Add( m_pRefBibcode, 0,
											wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	pRefHeaderA->AddStretchSpacer(1);
	pRefHeaderA->Add( pRefDataBibcodes, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// format view
	wxBoxSizer* pRefDataViewDoc = new wxBoxSizer( wxHORIZONTAL );
	pRefDataViewDoc->Add( m_pRefViewFormatButton, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 10  );
	pRefDataViewDoc->Add( m_pRefViewFormat, 0,
											wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	pRefHeaderA->Add( pRefDataViewDoc, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	pRefHeaderA->AddStretchSpacer(1);
	// title, journal, author
	wxFlexGridSizer* pRefHeaderB = new wxFlexGridSizer( 4, 2, 10, 5 );
	pRefHeaderB->AddGrowableCol( 1 );
	pReferencesDataSizer->Add( pRefHeaderB, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW );
	// title
	pRefHeaderB->Add( new wxStaticText( m_pPanelReferences, -1, wxT("Title: ")), 0,
											wxALIGN_RIGHT | wxALIGN_TOP  );
	pRefHeaderB->Add( m_pRefTitle, 1,
											wxALIGN_LEFT | wxALIGN_TOP );
	// journal
	pRefHeaderB->Add( new wxStaticText( m_pPanelReferences, -1, wxT("Journal: ")), 0,
											wxALIGN_RIGHT | wxALIGN_TOP  );
	pRefHeaderB->Add( m_pRefJournal, 0,
											wxALIGN_LEFT | wxALIGN_TOP );
	// author
	pRefHeaderB->Add( new wxStaticText( m_pPanelReferences, -1, wxT("Author: ")), 0,
											wxALIGN_RIGHT | wxALIGN_TOP  );
	pRefHeaderB->Add( m_pRefAuthor, 0,
											wxALIGN_LEFT | wxALIGN_TOP );
	////////////
	// reference summary
	wxStaticBox* pRefSummaryBox = new wxStaticBox( m_pPanelReferences, -1, wxT("Summary"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* pRefBoxSummarySizer = new wxStaticBoxSizer( pRefSummaryBox, wxHORIZONTAL );
	pReferencesDataSizer->Add( pRefBoxSummarySizer, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW );
	pRefBoxSummarySizer->Add( m_pRefSummary, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL |wxGROW );
	
	//////////////////////////////////////////////////
	// create Measures sizer
	wxGridSizer* pMeasuresSizer = new wxGridSizer( 1, 1, 10, 10 );
	// layout
	wxFlexGridSizer* pMeasuresDataSizer = new wxFlexGridSizer( 2, 1, 5, 5 );
	pMeasuresDataSizer->AddGrowableRow( 1 );
	pMeasuresDataSizer->AddGrowableCol( 0 );
	pMeasuresSizer->Add( pMeasuresDataSizer, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW );
	// create header name select sizer
	wxFlexGridSizer* pMeasuresDataHeaderSizer = new wxFlexGridSizer( 1, 4, 5, 5 );
	pMeasuresDataHeaderSizer->AddGrowableCol( 0 );
	pMeasuresDataHeaderSizer->AddGrowableCol( 3 );
	pMeasuresDataSizer->Add( pMeasuresDataHeaderSizer, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW|wxTOP, 5 );

	// create elements for Measures "panel"
	// the masurements names
	m_pMeasuresName =  new wxChoice( m_pPanelMeasures, wxID_SELECT_MEASURE_NAME,
										wxDefaultPosition,
										wxSize(140,-1), 1, vectMeasuresName, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pMeasuresName->SetSelection( 0 );
	// the tables
	m_pMeasuresTable = new wxListCtrl( m_pPanelMeasures, -1,
			wxPoint(-1,-1), wxSize(-1,-1), wxLC_REPORT|wxLC_HRULES|wxLC_VRULES );
	/////
	// populate measure layout
	// header name select
	pMeasuresDataHeaderSizer->AddStretchSpacer(1);
	pMeasuresDataHeaderSizer->Add( new wxStaticText( m_pPanelMeasures, -1, wxT("Select measurement: ")), 0,
											wxALIGN_RIGHT | wxALIGN_CENTER  );
	pMeasuresDataHeaderSizer->Add( m_pMeasuresName, 0, wxALIGN_LEFT | wxALIGN_TOP );
	pMeasuresDataHeaderSizer->AddStretchSpacer(1);
	// measurement table 
	pMeasuresDataSizer->Add( m_pMeasuresTable, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW|wxEXPAND );

	//////////////////////////////////////////////////
	// create Notes sizer
	wxGridSizer* pNotesSizer = new wxGridSizer( 1, 1, 10, 10 );	
	wxGridSizer* pNotesDataSizer = new wxGridSizer( 1, 1, 10, 10 );
	pNotesSizer->Add( pNotesDataSizer, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW|wxALL, 20 );

	// layout
	// create elements for Notes "panel"
	m_pNotesWin = new CCustomHtmlWindow( m_pPanelNotes, -1, wxPoint(-1, -1), wxSize(-1,-1) );
	// add to notes panel 
	pNotesDataSizer->Add( m_pNotesWin, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW|wxEXPAND );


	/////////////////////////////////////////////////////////////
	// create Spectrum sizer
	wxGridSizer* pSpectrumSizer = new wxGridSizer( 1, 1, 10, 10 );
	// layout
	wxFlexGridSizer* pSpectrumDataSizer = new wxFlexGridSizer( 2, 1, 5, 5 );
	pSpectrumDataSizer->AddGrowableRow( 1 );
	pSpectrumDataSizer->AddGrowableCol( 0 );
	pSpectrumSizer->Add( pSpectrumDataSizer, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW );
	// create header name select sizer
	wxGridSizer* pSpectrumDataHeaderSizer = new wxGridSizer( 1, 2, 5, 5 );
//	pSpectrumDataHeaderSizer->AddGrowableCol( 0 );
//	pSpectrumDataHeaderSizer->AddGrowableCol( 1 );
	pSpectrumDataSizer->Add( pSpectrumDataHeaderSizer, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW|wxTOP, 5 );

	// data for spectrum
	m_pSpectrumList = new wxChoice( m_pPanelSpectrum, wxID_SELECT_SPECTRUM_NAME,
										wxDefaultPosition,
										wxSize(140,-1), 1, vectDefaultNone, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pSpectrumView = new wxPlotCtrl( m_pPanelSpectrum, -1 );
	m_pSpectrumView->SetShowXAxisLabel( 1 );
	m_pSpectrumView->SetShowYAxisLabel( 1 );
	m_pSpectrumView->SetShowPlotTitle( 1 );
	m_pSpectrumView->SetCrossHairCursor( 1 );
	m_pSpectrumView->SetDrawLines( 0 );
	m_pSpectrumView->SetDrawSpline( 1 );
	m_pSpectrumView->SetDrawSymbols( 0 );

	// layout
	pSpectrumDataHeaderSizer->Add( new wxStaticText( m_pPanelSpectrum, -1, wxT("Select spectral measurement: ")), 0,
											wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pSpectrumDataHeaderSizer->Add( m_pSpectrumList, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// spec graph
	pSpectrumDataSizer->Add( m_pSpectrumView, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW|wxEXPAND );

	//////////////////////////////////////////////////
	// create Local sizer
	wxGridSizer* pLocalSizer = new wxGridSizer( 1, 1, 10, 10 );
	wxGridSizer* pLocalDataSizer = new wxGridSizer( 4, 4, 5, 5 );
	pLocalSizer->Add( pLocalDataSizer, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxALL, 10 );

	// create elements for Local "panel"
	m_pObjectPosX = new wxStaticText( m_pPanelLocal, -1, wxT("na"));
	m_pObjectPosY = new wxStaticText( m_pPanelLocal, -1, wxT("na"));
	m_pObjectCoordRa = new wxStaticText( m_pPanelLocal, -1, wxT("na"));
	m_pObjectCoordDec = new wxStaticText( m_pPanelLocal, -1, wxT("na"));
	m_pObjectMag = new wxStaticText( m_pPanelLocal, -1, wxT("na"));
	m_pObjectKron = new wxStaticText( m_pPanelLocal, -1, wxT("na"));
	m_pObjectEllipseWidth = new wxStaticText( m_pPanelLocal, -1, wxT("na"));
	m_pObjectEllipseHeight = new wxStaticText( m_pPanelLocal, -1, wxT("na"));
	m_pObjectEllipseAngle = new wxStaticText( m_pPanelLocal, -1, wxT("na"));
	m_pObjectFwhm = new wxStaticText( m_pPanelLocal, -1, wxT("na"));
	// layout
	// :: pos x
	pLocalDataSizer->Add( new wxStaticText( m_pPanelLocal, -1, wxT("Object Center X: ")), 0,
											wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pLocalDataSizer->Add( m_pObjectPosX, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: pos y
	pLocalDataSizer->Add( new wxStaticText( m_pPanelLocal, -1, wxT("Object Center Y: ")), 0,
											wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pLocalDataSizer->Add( m_pObjectPosY, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: coord ra
	pLocalDataSizer->Add( new wxStaticText( m_pPanelLocal, -1, wxT("Object Coord RA: ")), 0,
											wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pLocalDataSizer->Add( m_pObjectCoordRa, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: coord dec
	pLocalDataSizer->Add( new wxStaticText( m_pPanelLocal, -1, wxT("Object Coord DEC: ")), 0,
											wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pLocalDataSizer->Add( m_pObjectCoordDec, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: detected mag ?? do detected vs real ?
	pLocalDataSizer->Add( new wxStaticText( m_pPanelLocal, -1, wxT("Detected Magnitude: ")), 0,
											wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pLocalDataSizer->Add( m_pObjectMag, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: Kron Factor
	pLocalDataSizer->Add( new wxStaticText( m_pPanelLocal, -1, wxT("Kron Factor: ")), 0,
											wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pLocalDataSizer->Add( m_pObjectKron, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: ellipse a
	pLocalDataSizer->Add( new wxStaticText( m_pPanelLocal, -1, wxT("Object Width: ")), 0,
											wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pLocalDataSizer->Add( m_pObjectEllipseWidth, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: ellipse b
	pLocalDataSizer->Add( new wxStaticText( m_pPanelLocal, -1, wxT("Object Height: ")), 0,
											wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pLocalDataSizer->Add( m_pObjectEllipseHeight, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: ellipse angle
	pLocalDataSizer->Add( new wxStaticText( m_pPanelLocal, -1, wxT("Object Angle: ")), 0,
											wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pLocalDataSizer->Add( m_pObjectEllipseAngle, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: object fwhm
	pLocalDataSizer->Add( new wxStaticText( m_pPanelLocal, -1, wxT("Object FWHM: ")), 0,
											wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pLocalDataSizer->Add( m_pObjectFwhm, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );


	//////////////////////////////////////////////////
	// create Histogram sizer
	wxGridSizer* pHistSizer = new wxGridSizer( 1, 1, 10, 10 );
	m_pImageHistogramView = new CHistogramView( m_pPanelHist, &m_rHistogram, NULL, true, -1 );
	pHistSizer->Add( m_pImageHistogramView, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 10 );

	//////////////////////////////////////////////////
	// create Radial sizer
	wxGridSizer* pRadialSizer = new wxGridSizer( 1, 1, 10, 10 );
	m_pImageRadialProfileView = new CRadialProfileView( m_pPanelRadial, -1 );
	pRadialSizer->Add( m_pImageRadialProfileView, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 10 );

	////////////////////
	// add Panel Details
	m_pPanelDetails->SetSizer( pDetailsSizer );
	// add Panel Identifiers
	m_pPanelIdentifiers->SetSizer( pIdentifiersSizer );
	// add Panel References
	m_pPanelReferences->SetSizer( pReferencesSizer );
	// add Panel Measures
	m_pPanelMeasures->SetSizer( pMeasuresSizer );
	// add Panel Notes
	m_pPanelNotes->SetSizer( pNotesSizer );
	// add panel spectrum
	m_pPanelSpectrum->SetSizer( pSpectrumSizer );
	// add Panel Local
	m_pPanelLocal->SetSizer( pLocalSizer );
	// add Panel Histogram
	m_pPanelHist->SetSizer( pHistSizer );
	// add Panel Radial
	m_pPanelRadial->SetSizer( pRadialSizer );

	// hide panels
	m_pPanelDetails->Hide( );
	m_pPanelIdentifiers->Hide( );
	m_pPanelReferences->Hide( );
	m_pPanelMeasures->Hide( );
	m_pPanelNotes->Hide( );
	m_pPanelSpectrum->Hide( );
	m_pPanelLocal->Hide( );
	m_pPanelHist->Hide( );
	m_pPanelRadial->Hide( );

	// set panel flags to zero 
	m_bPanelDetails = 0;
	m_bPanelIdentifiers = 0;
	m_bPanelReferences = 0;
	m_bPanelMeasures = 0;
	m_bPanelNotes = 0;
	m_bPanelSpectrum = 0;
	m_bPanelLocal = 1;
	m_bPanelHist = 1;
	m_bPanelRadial = 1;

	topsizer->Add( m_pNotebook, 1, wxALIGN_CENTER|wxALIGN_TOP|wxGROW | wxALL, 3 );

	// add dialog bottom buttons
//	topsizer->Add( CreateButtonSizer(wxOK), 0, wxALIGN_RIGHT | wxALL, 5 );

	topsizer->Layout( );
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	Layout( );
	//SetMinSize( wxSize( 499, 399 ) );
	//SetMaxSize( wxSize( 510, 410 ) );

	topsizer->SetSizeHints(this);
//	topsizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CStarViewDlg
// Purpose:	destroy my  dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CStarViewDlg::~CStarViewDlg( ) 
{
	// delete base objects
	delete( m_pSimbad );
	delete( m_pSkyObjSpectrum );
	delete( m_pBibDb );
	delete( m_pSkyview );

	delete( m_pImageHistogramView );
	delete( m_pImageRadialProfileView );
	// remote image
	delete( m_pImgDataSurveyBox );
	delete( m_pUseImageSource );
	delete( m_pRemImageType );
	delete( m_pRemImageSurvey );
	delete( m_pRemImageColorTable );

	delete( m_pImgDataOverlayBox );
	delete( m_pRemImageGridOverlay );
	delete( m_pRemImageContourOverlay );
	delete( m_pRemImageCatalogOverlay );
	delete( m_pRemImageChannel );

	delete( m_pImgDataRgbBox );
	delete( m_pRemImageRGBCompRed );
	delete( m_pRemImageRGBCompGreen );
	delete( m_pRemImageRGBCompBlue );
	// details
	delete( m_pCoordType );
	delete( m_pCoordValue );
	delete( m_pName );
	delete( m_pType );
	delete( m_pProperMotions );
	delete( m_pParallax );
	delete( m_pRadialVelocityLabel );
	delete( m_pRadialVelocity );
	delete( m_pObjView );
	delete( m_pSpectralType );
	delete( m_pRedshift );
	delete( m_pMorphologicalType );
	delete( m_pAngularSize );
	delete( m_pFluxB );
	delete( m_pFluxV );
	delete( m_pFluxJ );
	delete( m_pFluxH );
	delete( m_pFluxK );

	// indentifiers
//	delete( m_pIdentifiersCat );
//	delete( m_pIdentifiersName );
	// references
	delete( m_pRefTitle );
	delete( m_pRefJournal );
	delete( m_pRefAuthor );
	delete( m_pRefSummary );
	delete( m_pRefBibcode );
	delete( m_pRefViewFormat );
	delete( m_pRefViewFormatButton );
	// measures
	delete( m_pMeasuresTable );
	delete( m_pMeasuresName );
	// motes
	delete( m_pNotesWin );

	/////////////////////////////
	// spectrum
	delete( m_pSpectrumView );
	delete( m_pSpectrumList );

	///////////////////
	// local
	delete( m_pObjectPosX );
	delete( m_pObjectPosY );
	delete( m_pObjectCoordRa );
	delete( m_pObjectCoordDec );
	delete( m_pObjectMag );
	delete( m_pObjectKron );
	delete( m_pObjectEllipseWidth );
	delete( m_pObjectEllipseHeight );
	delete( m_pObjectEllipseAngle );
	delete( m_pObjectFwhm );

	delete( m_pPanelDetails );
	delete( m_pPanelIdentifiers );
	delete( m_pPanelReferences );
	delete( m_pPanelMeasures );
	delete( m_pPanelNotes );
	delete( m_pPanelLocal );
	delete( m_pPanelHist );
	delete( m_pPanelRadial );
	delete( m_pNotebook );
}

////////////////////////////////////////////////////////////////////
void CStarViewDlg::GetObjectData( ) 
{
	// get data from simbad
	m_pUnimapWorker->SetWaitDialogMsg( wxT("Loading Simbad data...") );
	
	m_pSimbad->GetObjectData( m_strObjectCatName );

	// get bibcodes if any 
	m_pUnimapWorker->SetWaitDialogMsg( wxT("Loading ADS references...") );
	m_pBibDb->GetBibcodesData( );

	// get spectrum
	m_pSkyObjSpectrum->GetSpectrum( m_nObjType, m_strObjectCatName );
}

////////////////////////////////////////////////////////////////////
// Method:	DisplayObjectData
// Class:	CStarViewDlg
// Purpose:	display object data in Gui
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CStarViewDlg::DisplayObjectData( ) 
{
	int i = 0;

	//////////
	// populat from other obj
	m_pCoordType->Clear( );

	// adjust title with object name
	wxString strTitle = this->GetLabel() + wxT(" :: ") + m_pSimbad->m_strObjectName;
	this->SetLabel( strTitle );

	m_pName->SetLabel( m_pSimbad->m_strObjectName );
	m_pType->SetLabel( m_pSimbad->m_strObjectType );
	// set coord types
	for( i=0; i<m_pSimbad->m_nCoordType; i++ ){ m_pCoordType->Append( m_pSimbad->m_vectCoordLabel[i] ); }
	m_bPanelDetails = m_pSimbad->m_bHasDetails;
	// set proper motions
	m_pProperMotions->SetLabel( m_pSimbad->m_strProperMotions );
	m_pParallax->SetLabel( m_pSimbad->m_strParallax );
	m_pRedshift->SetLabel( m_pSimbad->m_strRedshift );
	// set flux
	m_pFluxB->SetLabel( m_pSimbad->m_strFluxB );
	m_pFluxV->SetLabel( m_pSimbad->m_strFluxV );
	m_pFluxJ->SetLabel( m_pSimbad->m_strFluxJ );
	m_pFluxH->SetLabel( m_pSimbad->m_strFluxH );
	m_pFluxK->SetLabel( m_pSimbad->m_strFluxK );
	// spectral
	m_pSpectralType->SetLabel( m_pSimbad->m_strSpectralType );
	// morphological type
	if( !m_pSimbad->m_strMorphologicalType.IsEmpty() )
		m_pMorphologicalType->SetLabel( m_pSimbad->m_strMorphologicalType );
	// angular size
	m_pAngularSize->SetLabel( m_pSimbad->m_strAngSize );

	//////
	// SET BIBCODES
	// setdefault bibcodes
	SelectBibcode(  );
	m_pRefBibcode->Clear( );
	for( i=0; i<m_pBibDb->m_nBibCode; i++ )
	{
		// add to ctrl list
		m_pRefBibcode->Append( m_pBibDb->m_vectBibCode[i] );
	}
	m_pRefBibcode->SetSelection( 0 );
	if(  m_pBibDb->m_nBibCode > 0 ) m_bPanelReferences = 1;

	/////////
	// reset measures
	m_pMeasuresName->Clear( );
	for( i=0; i<m_pSimbad->m_vectMeasuresName.size(); i++ )
	{
		m_pMeasuresName->Append( m_pSimbad->m_vectMeasuresName[i] );
	}
	if( m_pSimbad->m_nMeasuresTable > 0 ) m_bPanelMeasures = 1;

	///////////////
	// set default selections
	m_pCoordType->SetSelection( 0 );
	m_pCoordValue->SetLabel( m_pSimbad->m_vectCoordValue[0] );

	//////
	// set panels flags
	m_bPanelIdentifiers = m_pSimbad->m_bHasIdentifiers;
	m_bPanelNotes = m_pSimbad->m_bHasNotes;
	m_bPanelSpectrum = m_pSkyObjSpectrum->m_bHasSpectrum;

	//////////////////
	// radial velocity/cz case
	if( m_pSimbad->m_strCZ.IsEmpty() )
	{
		m_pRadialVelocityLabel->SetLabel( wxT("Radial Velocity:") );
		m_pRadialVelocity->SetLabel( m_pSimbad->m_strRadialVelocity );

	} else
	{
		wxString strValue = m_pSimbad->m_strRadialVelocity + wxT(" ") + m_pSimbad->m_strCZ;
		m_pRadialVelocityLabel->SetLabel( wxT("Radial Velocity/CZ:") );
		m_pRadialVelocity->SetLabel( strValue );

	}
	// select measurments
	m_pMeasuresName->SetSelection( 0 );
	SelectMeasurement( );
	// if notes
	if( !m_pSimbad->m_strNotesPage.IsEmpty() )
	{
		m_pSimbad->m_strNotesPage += wxT("</UL></td></tr></table>\n</body></html>\n");
		m_pNotesWin->SetPage( m_pSimbad->m_strNotesPage );
	}

	//////////////////
	// populate identifiers
	for( i=0; i<m_pSimbad->m_nIdentifiers ; i++ )
	{
		m_pIdentifiersDataSizer->Add( new wxStaticText( m_pPanelIdentifiers, -1, m_pSimbad->m_vectIdentifiersCat[i] + wxT(": ")), 0,
									wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
		m_pIdentifiersDataSizer->Add( new wxStaticText( m_pPanelIdentifiers, -1, m_pSimbad->m_vectIdentifiersName[i]), 0,
									wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	}

	///////////////////
	// SPECTRUM
	m_pSpectrumList->Clear();
	for( i=0; i<m_pSkyObjSpectrum->m_nSpectrum ; i++ )
	{
		// append to the list
		m_pSpectrumList->Append( m_pSkyObjSpectrum->m_vectSpectrum[i].strLabel );
	}
	// select first spectrum
	m_pSpectrumList->SetSelection( 0 );
	SelectSpectrum( );

	////////////////////
	// here we add visible panels if data is set
	// add Panel Details
	if( m_bPanelDetails )
	{
		m_pPanelDetails->Show( );
		m_pNotebook->AddPage( m_pPanelDetails, wxT("Details") );
	}
	// add Panel Identifiers
	if( m_bPanelIdentifiers )
	{
		m_pPanelIdentifiers->Show( );
		m_pNotebook->AddPage( m_pPanelIdentifiers, wxT("Identifiers") );
	}
	// add Panel References
	if( m_bPanelReferences )
	{
		m_pPanelReferences->Show( );
		m_pNotebook->AddPage( m_pPanelReferences, wxT("References") );
	}
	// add Panel Measures
	if( m_bPanelMeasures )
	{
		m_pPanelMeasures->Show( );
		m_pNotebook->AddPage( m_pPanelMeasures, wxT("Measures") );
	}
	// add Panel Notes
	if( m_bPanelNotes )
	{
		m_pPanelNotes->Show( );
		m_pNotebook->AddPage( m_pPanelNotes, wxT("Notes") );
	}
	// add Panel Spectrum
	if( m_bPanelSpectrum )
	{
		m_pPanelSpectrum->Show( );
		m_pNotebook->AddPage( m_pPanelSpectrum, wxT("Spectrum") );
	}
	// add Panel Local
	if( m_bPanelLocal )
	{
		m_pPanelLocal->Show( );
		m_pNotebook->AddPage( m_pPanelLocal, wxT("Local") );
	}
	// add Panel histogram
	if( m_bPanelHist )
	{
		m_pPanelHist->Show( );
		m_pNotebook->AddPage( m_pPanelHist, wxT("Histogram") );
	}
	// add Panel radial
	if( m_bPanelRadial )
	{
		m_pPanelRadial->Show( );
		m_pNotebook->AddPage( m_pPanelRadial, wxT("Radial Plot") );
	}

	///?????????
	m_pNotebook->Layout();

//	m_pNotebook->Fit( );

	m_pPanelDetails->GetSizer()->Layout( );
	m_pPanelDetails->Layout( );
	// Indentifiers
	m_pPanelIdentifiers->GetSizer()->Layout( );
	m_pPanelIdentifiers->Layout( );
	// References
	m_pPanelReferences->GetSizer()->Layout( );
	m_pPanelReferences->Layout( );
	// measures
	m_pPanelMeasures->GetSizer()->Layout( );
	m_pPanelMeasures->Layout( );
	// notes
	m_pPanelNotes->GetSizer()->Layout( );
	m_pPanelNotes->Layout( );
	// local 
	m_pPanelLocal->GetSizer()->Layout( );
	m_pPanelLocal->Layout( );
	// histogram 
	m_pPanelHist->GetSizer()->Layout( );
	m_pPanelHist->Layout( );
	// radial profile 
	m_pPanelRadial->GetSizer()->Layout( );
	m_pPanelRadial->Layout( );

//	SelectBibcode(  );

	GetSizer()->Layout( );
	Fit( );

	Refresh( FALSE );
	Update( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetObjectHistogram
// Class:	CStarViewDlg
// Purpose:	set object histogram
// Input:	pointer to image and object
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CStarViewDlg::SetObjectHistogram( )
{
	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetObjectData
// Class:	CStarViewDlg
// Purpose:	setobject data
// Input:	pointer to image and object
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CStarViewDlg::SetObjectData( CAstroImage *pAstroImage, wxImage &pImage, 
									StarDef* pObject, unsigned char nObjType ) 
{
	int i=0;

	m_nObjType = nObjType;
	m_pAstroImage = pAstroImage;
	m_pObject = pObject;

	//////////////////////////////////
	// get object/star name from cat 1=dstar, 2=cstar, 3=dso
	if( nObjType == 1 )
		m_bHasName = m_pSky->m_pCatalogStarNames->GetStarSimpleCatName( m_pAstroImage->m_vectStar[pObject->img_id], m_strObjectCatName );
	else if( nObjType == 2 )
		m_bHasName = m_pSky->m_pCatalogStarNames->GetStarSimpleCatName( m_pAstroImage->m_vectCatStar[pObject->img_id], m_strObjectCatName );
	else if( nObjType == 3 )
		m_bHasName = m_pSky->m_pCatalogDsoNames->GetDsoSimpleCatName( m_pAstroImage->m_vectCatDso[pObject->img_id], m_strObjectCatName );

	//////////////////////
	// create base objects
	m_pBibDb = new CBibDb( m_pUnimapWorker );
	m_pSimbad = new CSimbad( pAstroImage, m_pUnimapWorker, m_pSky, m_pBibDb );
	m_pSkyObjSpectrum = new CSkyObjSpectrum( m_pUnimapWorker );
	m_pSkyview = new CSkyviewSite( m_pAstroImage, m_pObject, m_strObjectCatName );

	// init counters
	m_pBibDb->m_nBibCode = 0;
	m_pSimbad->m_nCoordType = 0;
	wxString strTmp;
	// get image size
	int nImgWidth = pImage.GetWidth();
	int nImgHeight = pImage.GetHeight();

	//////////////////////////////////
	// prepare folders normaly used
	if( !::wxDirExists( BIBCODES_SUMMARY_DATA_PATH ) )
	{
		::wxMkdir( BIBCODES_SUMMARY_DATA_PATH );
	}

	// get right a and/or b
	double a, b;
	if( pObject->kronfactor != 0 )
	{
		a = pObject->a*pObject->kronfactor;
		b = pObject->b*pObject->kronfactor;

	} else
	{
		a = pObject->a;
		b = pObject->b;
	}

	// if type 2
//	if( nObjType == 3 ) 
//	{
//		a = pObject->a != 0 ? pObject->a : pObject->b;
//		b = pObject->b != 0 ? pObject->b : pObject->a;
//	}

	// get ellipse bouding box
	double nMaxW, nMaxH;
	ellipseBoundingBox( pObject->x, pObject->y, a, b, pObject->theta, nMaxW, nMaxH );
	// set cu variables
	double nCutW = nMaxW;
	double nCutH = nMaxH;
	///////////
	// :: adjust cut :: if to small add extra 20 points
	if( nCutW < 20 && nCutH < 20 )
	{
		nCutW += 20;
		nCutH += 20;
	}
	// :: adjust cut ratio - if to tall or wide
	double nCutRatio = (double) nCutW/nCutH;
	if( nCutRatio > 1.1 )
		nCutH = nCutH*nCutRatio;
	else if( nCutRatio < 0.9 )
		nCutW = nCutW/nCutRatio;
	// :: finaly if either cut w or h is bigger then image set to max image
	int bFoundMax = 0;
	if( nCutW > (double) nImgWidth/2.0 ){ nCutW = (double) nImgWidth/2.0; bFoundMax=1; }
	if( nCutH > (double) nImgHeight/2.0 ){ nCutH = (double) nImgHeight/2.0; bFoundMax=1; }
	// :: if adjust to max
	if( bFoundMax )
	{
		nCutRatio = (double) nCutW/nCutH;
		// cut down from biggest one to ratio
		if( nCutRatio > 1.1 )
			nCutW = nCutW/nCutRatio;
		else if( nCutRatio < 0.9 )
			nCutH = nCutH*nCutRatio;
	}

	// calculate width height
	m_nObjWinWidth = (int) round(2.0*nCutW);
	m_nObjWinHeight = (int) round(2.0*nCutH);

	// calculate image cut
	int nCutOrigX = pObject->x-round(nCutW);
	int nCutOrigY = pObject->y-round(nCutH);

	// check window cut against image 
	// check on x
	if( pObject->x-round(nCutW) < 0 )
	{
		nCutOrigX = 0;
	} else if( pObject->x+round(nCutW) > nImgWidth )
	{
		nCutOrigX = nImgWidth - m_nObjWinWidth;
	}	
	// and on y
	if( pObject->y-round(nCutH) < 0 )
	{
		nCutOrigY = 0;
	} else if( pObject->y+round(nCutH) > nImgHeight )
	{
		nCutOrigY = nImgHeight - m_nObjWinHeight;
	}	

	// get object image as sub image
	m_pLocalImage = pImage.GetSubImage( wxRect(nCutOrigX, nCutOrigY, m_nObjWinWidth, m_nObjWinHeight) );
	m_pObjView->SetImage( m_pLocalImage );
	// debug 
//	if( !m_pLocalImage.IsOk() ) wxMessageBox( wxT("sub image not ok."), wxT("Debug") );
//	m_pLocalImage.SaveFile( wxT("./test.jpg") );
//	pImage.SaveFile( wxT("./test2.jpg") );
//	wxString strTmpLog = wxT("");
//	strTmpLog.Printf( wxT("x=%d,y=%d,w=%d,h=%d"), nCutOrigX, nCutOrigY, m_nObjWinWidth, m_nObjWinHeight );
//	wxMessageBox( strTmpLog, wxT("Debug") );

	/////////////////////////
	// make histogram
	m_rHistogram.ComputeHistogramAreaElps( pImage, pObject->x, pObject->y, a, b, pObject->theta );
	m_pImageHistogramView->UpdateHistogram( &m_rHistogram );

	//////////////////////////
	// set radial profile panel
	m_pImageRadialProfileView->SetData( pImage, pObject, nObjType );

	////////////////////////////////////
	// :: load skyview color tables
	m_pSkyview->LoadSkyViewColorTable( OBJECT_SKYVIEW_COLOR_TABLE_FILE );
	// populate
	m_pRemImageColorTable->Clear();
	m_pRemImageColorTable->Freeze();
	m_pRemImageColorTable->Append( wxT( "None" ) );
	for( i=0; i<m_pSkyview->m_nRemImageColorTable; i++ )
	{
		m_pRemImageColorTable->Append( m_pSkyview->m_vectRemImageColorTableLabel[i] );
	}
	// re-enable list box
	m_pRemImageColorTable->Thaw();
	m_pRemImageColorTable->SetSelection( 0 );

	////////////////////////////////////
	// :: load skyview catalog overlays
	m_pSkyview->LoadSkyViewCatalogOverlay( OBJECT_SKYVIEW_CATALOG_OVERLAY_FILE );
	// populate
	m_pRemImageCatalogOverlay->Clear();
	m_pRemImageCatalogOverlay->Freeze();
	m_pRemImageCatalogOverlay->Append( wxT( "None" ) );
	for( i=0; i<m_pSkyview->m_nRemImageCatalogOverlay; i++ )
	{
		m_pRemImageCatalogOverlay->Append( m_pSkyview->m_vectRemImageCatalogOverlayLabel[i] );
	}
	// re-enable list box
	m_pRemImageCatalogOverlay->Thaw();
	m_pRemImageCatalogOverlay->SetSelection( 0 );

	////////////////////////////////////
	// :: load skyview surveys
	m_pSkyview->LoadSkyViewSurveys( OBJECT_SKYVIEW_SURVEYS_FILE );
	// populate : reset contour
	m_pRemImageContourOverlay->Clear();
	m_pRemImageContourOverlay->Freeze();
	m_pRemImageContourOverlay->Append( wxT( "None" ) );
	// reset rgb
	m_pRemImageRGBCompRed->Clear( );
	m_pRemImageRGBCompRed->Freeze( );
	m_pRemImageRGBCompRed->Append( wxT( "None" ) );
	m_pRemImageRGBCompGreen->Clear( );
	m_pRemImageRGBCompGreen->Freeze( );
	m_pRemImageRGBCompGreen->Append( wxT( "None" ) );
	m_pRemImageRGBCompBlue->Clear( );
	m_pRemImageRGBCompBlue->Freeze( );
	m_pRemImageRGBCompBlue->Append( wxT( "None" ) );
	// for each ... populate
	for( i=0; i<m_pSkyview->m_vectImageContourOverlay.size(); i++ )
	{
		m_pRemImageContourOverlay->Append( m_pSkyview->m_vectImageContourOverlay[i] );
		// build the rgb components
		m_pRemImageRGBCompRed->Append( m_pSkyview->m_vectImageContourOverlay[i] );
		m_pRemImageRGBCompGreen->Append( m_pSkyview->m_vectImageContourOverlay[i] );
		m_pRemImageRGBCompBlue->Append( m_pSkyview->m_vectImageContourOverlay[i] );
	}
	// set survey
	wxString strSurveyType = wxT( "Optical" );
	SetImageSurveyType( strSurveyType );
	m_pRemImageSurvey->SetSelection( 0 );
	// re-enable list box
	m_pRemImageContourOverlay->Thaw();
	m_pRemImageContourOverlay->SetSelection( 0 );
	// set rgb to default none
	m_pRemImageRGBCompRed->Thaw();
	m_pRemImageRGBCompGreen->Thaw( );
	m_pRemImageRGBCompBlue->Thaw( );
	m_pRemImageRGBCompRed->SetSelection( 0 );
	m_pRemImageRGBCompGreen->SetSelection( 0 );
	m_pRemImageRGBCompBlue->SetSelection( 0 );

	/////////////////////////
	// set locals panel
	strTmp.sprintf( wxT("%d"), (int) pObject->x );
	m_pObjectPosX->SetLabel( strTmp );
	strTmp.sprintf( wxT("%d"), (int) pObject->y );
	m_pObjectPosY->SetLabel( strTmp );
	// ra/dec
	strTmp.sprintf( wxT("%.5f"), pObject->ra );
	m_pObjectCoordRa->SetLabel( strTmp );
	strTmp.sprintf( wxT("%.5f"), pObject->dec );
	m_pObjectCoordDec->SetLabel( strTmp );
	// mag/kron
	strTmp.sprintf( wxT("%.5f"), pObject->mag );
	m_pObjectMag->SetLabel( strTmp );
	strTmp.sprintf( wxT("%.5f"), pObject->kronfactor );
	m_pObjectKron->SetLabel( strTmp );
	// ellipse
	strTmp.sprintf( wxT("%.2f"), pObject->a );
	m_pObjectEllipseWidth->SetLabel( strTmp );
	strTmp.sprintf( wxT("%.2f"), pObject->b );
	m_pObjectEllipseHeight->SetLabel( strTmp );
	strTmp.sprintf( wxT("%.4f"), pObject->theta );
	m_pObjectEllipseAngle->SetLabel( strTmp );
	// fwhm
	strTmp.sprintf( wxT("%.5f"), pObject->fwhm );
	m_pObjectFwhm->SetLabel( strTmp );

	//////////////////////////////
	// set worket thread to display wait dialog
//	m_pUnimap->m_pUnimapWorker->m_pStarDlg = this;

	DefCmdAction act;
	act.id = THREAD_ACTION_LOAD_OBJECT_DATA;
	act.vectObj[0] = this;
//	m_pUnimap->m_pUnimapWorker->SetAction( cmd );

	/////////////////
	// show wait dialog
	CWaitDialog* pWaitDialog = new CWaitDialog( this, wxT("Load Object Data"), 
						wxT("loading data..."), 0, m_pUnimapWorker, &act );
	pWaitDialog->ShowModal();
	// delete/reset wait dialog
	delete( pWaitDialog );
	pWaitDialog = NULL;

	Refresh( FALSE );
	Update( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	GetRemoteImage
// Class:	CStarViewDlg
// Purpose:	Get remote image
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CStarViewDlg::GetRemoteImage( ) 
{
	// get current settings
	wxString strImageSource = m_pRemImageChannel->GetStringSelection();
	wxString strSurvey = m_pRemImageSurvey->GetStringSelection();
	wxString strRemImageType = m_pRemImageType->GetStringSelection();

	// color table - to use only when single channel ?
	int nColorTable = m_pRemImageColorTable->GetSelection();
	// grid overlay
	int nGridOverlay = m_pRemImageGridOverlay->GetSelection();
	// catalog overlay
	int nCatalogOverlay = m_pRemImageCatalogOverlay->GetSelection();
	// set contour overlay
	int nContourOverlay = m_pRemImageContourOverlay->GetSelection();

	/////
	// more vars
	wxString strRemImageRGBCompRed = m_pRemImageRGBCompRed->GetStringSelection();
	wxString strRemImageRGBCompGreen = m_pRemImageRGBCompGreen->GetStringSelection();
	wxString strRemImageRGBCompBlue = m_pRemImageRGBCompBlue->GetStringSelection();

	wxString strRemImageGridOverlay = m_pRemImageGridOverlay->GetStringSelection();
	wxString strRemImageContourOverlay = m_pRemImageContourOverlay->GetStringSelection();

	// call to skyview object
	m_pSkyview->GetRemoteImage( strImageSource, strSurvey, strRemImageType, nColorTable, nGridOverlay,
							nCatalogOverlay, nContourOverlay, strRemImageRGBCompRed, 
							strRemImageRGBCompGreen, strRemImageRGBCompBlue,
							strRemImageGridOverlay, strRemImageContourOverlay, m_bHasName );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		OnSelectCoordType
// Class:		CStarViewDlg
// Purpose:		when coord type is selected
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarViewDlg::OnSelectCoordType( wxCommandEvent& event )
{
	int nSelect = m_pCoordType->GetSelection();
	m_pCoordValue->SetLabel( m_pSimbad->m_vectCoordValue[nSelect] );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnSelectBibcode
// Class:		CStarViewDlg
// Purpose:		when bicode selected populate fields
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarViewDlg::OnSelectBibcode( wxCommandEvent& event )
{
	SelectBibcode( );

	pReferencesSizer->Layout();
	GetSizer()->Layout();
	Fit();

	Refresh( FALSE );
	Update( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnSelectMeasurement
// Class:		CStarViewDlg
// Purpose:		when measurment selected populate fields
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarViewDlg::OnSelectMeasurement( wxCommandEvent& event )
{
	SelectMeasurement( );

	GetSizer()->Layout( );
	Fit( );

	Refresh( FALSE );
	Update( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnSelectSpectrum
// Class:		CStarViewDlg
// Purpose:		when spectrum selected populate fields
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarViewDlg::OnSelectSpectrum( wxCommandEvent& event )
{
	SelectSpectrum( );

	Refresh( FALSE );
	Update( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		SelectBibcode
// Class:		CStarViewDlg
// Purpose:		get current selection and populate fields
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarViewDlg::SelectBibcode(  )
{
	FILE* pFileBibcode = NULL;
	wxChar strLine[10000];
	int w = 0, h = 0;
	int i = 0;
	// define regex
	wxRegEx reTitle( wxT( "^Title=(.+)$" ), wxRE_ADVANCED  );
	wxRegEx reJournal( wxT( "^Journal=(.+)$" ), wxRE_ADVANCED  );
	wxRegEx reAuthor( wxT( "^Author=(.+)$" ), wxRE_ADVANCED  );
	wxRegEx reSummary( wxT( "^Summary=(.+)$" ), wxRE_ADVANCED  );
	wxRegEx reDataLinks( wxT( "^DataLinks=(.+)$" ), wxRE_ADVANCED  );

	// calculate wrap size
//	wxSize nSize = m_pRefTitle->GetContainingSizer()->GetSize( );
	wxSize nSize = m_pNotebook->GetContainingSizer()->GetSize( );
	w = nSize.GetWidth() - 60;
	
	int nSelect = m_pRefBibcode->GetSelection();

	wxString strFileName = m_pBibDb->ProcessPathFromBibCode( m_pBibDb->m_vectBibCode[nSelect] );
	pFileBibcode = wxFopen( strFileName, wxT("rb") );
	if( !pFileBibcode ) return;

	// Reading lines from cfg file
	while( !feof( pFileBibcode ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 10000, pFileBibcode );
		wxString strWxLine = strLine;

		/////////////////
		// check for my patterns
		if( reTitle.Matches( strWxLine ) )
		{
			m_pRefTitle->SetLabel( reTitle.GetMatch( strWxLine, 1 ).Trim( 1 ).Trim( 0 ) );
			m_pRefTitle->Wrap( w );

		} else if( reJournal.Matches( strWxLine ) )
		{
			m_pRefJournal->SetLabel( reJournal.GetMatch( strWxLine, 1 ).Trim( 1 ).Trim( 0 ) );
			m_pRefJournal->Wrap( w );

		} else if( reAuthor.Matches( strWxLine ) )
		{
			m_pRefAuthor->SetLabel( reAuthor.GetMatch( strWxLine, 1 ).Trim( 1 ).Trim( 0 ) );
			m_pRefAuthor->Wrap( w );

		} else if( reSummary.Matches( strWxLine ) )
		{

		//	m_pRefSummary->SetValue( m_vectBibSummary[nSelect] );
			m_pRefSummary->SetPage( reSummary.GetMatch( strWxLine, 1 ).Trim( 1 ).Trim( 0 ) );
		//	m_pRefSummary->Wrap( w );

		} else if( reDataLinks.Matches( strWxLine ) )
		{
			wxString strDataLinks = reDataLinks.GetMatch( strWxLine, 1 ).Trim( 1 ).Trim( 0 );
			// and here we set the doc link types
			m_pRefViewFormat->Clear( );
			// split string
			int nBibDataLinks = 0;
			wxStringTokenizer tkz( strDataLinks, wxT(",") );
			while ( tkz.HasMoreTokens() )
			{
				wxString strDocType = tkz.GetNextToken();
				// check if emty - aka end of line
				if( strDocType.Length() != 0 )
				{
					// add to list
					m_pRefViewFormat->Append( strDocType );
					// increment counter
					nBibDataLinks++;
				}
			}
			// if found data link enable and select else disable my entries
			if( nBibDataLinks > 0 )
			{
				m_pRefViewFormat->SetSelection( 0 );
				m_pRefViewFormat->Enable();
				m_pRefViewFormatButton->Enable();

			} else
			{
				m_pRefViewFormat->Disable();
				m_pRefViewFormatButton->Disable();
			}
		}
	}

	fclose( pFileBibcode );

	GetSizer()->Layout( );
	Fit( );

	Refresh( FALSE );
	Update( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		SelectMeasurement
// Class:		CStarViewDlg
// Purpose:		get current selection and populate Measurement table
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarViewDlg::SelectMeasurement( )
{
	int i = 0, j = 0;
	int nMesTable = m_pMeasuresName->GetSelection();
	// clean up
	m_pMeasuresTable->ClearAll();

	// create header
	for( i=0; i<m_pSimbad->m_nMeasuresTableCol[nMesTable]; i++ )
	{
		m_pMeasuresTable->InsertColumn( i+1, m_pSimbad->m_vectMeasuresTableHead[nMesTable][i], wxLIST_FORMAT_CENTRE, 100 );
	}
	// add data
	for( i=0; i<m_pSimbad->m_nMeasuresTableRow[nMesTable]; i++ )
	{
		long nIndex = m_pMeasuresTable->InsertItem( 55551+i, wxT("") );

		for( j=0; j<m_pSimbad->m_nMeasuresTableCol[nMesTable]; j++ )
		{
			m_pMeasuresTable->SetItem( nIndex, j, m_pSimbad->m_vectMeasuresTableValues[nMesTable][i][j] );
		}
	}

	Refresh( FALSE );
	Update( );
}

////////////////////////////////////////////////////////////////////
// Method:		SelectSpectrum
// Class:		CStarViewDlg
// Purpose:		get current selection and plot spectrum
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarViewDlg::SelectSpectrum( )
{
	int nSpecId = m_pSpectrumList->GetSelection();
	if( nSpecId < 0 || nSpecId >= m_pSkyObjSpectrum->m_nSpectrum ) return;

	int nCurveId = m_pSpectrumView->GetActiveIndex();
	// free if used
	if( m_pSkyObjSpectrum->m_vectSpectrumDataX || m_pSkyObjSpectrum->m_vectSpectrumDataY ) m_pSpectrumView->DeleteCurve( 0, true );
//	while( m_pSpectrumView->GetActiveIndex() != -1 ) Sleep( 50 );

//	if( m_pSkyObjSpectrum->m_vectSpectrumDataX ) free( m_pSkyObjSpectrum->m_vectSpectrumDataX );
//	if( m_pSkyObjSpectrum->m_vectSpectrumDataY ) free( m_pSkyObjSpectrum->m_vectSpectrumDataY );
	m_pSkyObjSpectrum->m_nSpectrumDataSize = 0;

	///////////////////
	// allocate my data vectors - basic
	m_pSkyObjSpectrum->m_nSpectrumDataSizeAllocated = SPECTRUM_DATA_SIZE_INITIAL_ALLOCATION;
//	if( !m_pSkyObjSpectrum->m_vectSpectrumDataX ) 
	m_pSkyObjSpectrum->m_vectSpectrumDataX = (double*) malloc( m_pSkyObjSpectrum->m_nSpectrumDataSizeAllocated*sizeof(double) );
//	if( !m_pSkyObjSpectrum->m_vectSpectrumDataY ) 
	m_pSkyObjSpectrum->m_vectSpectrumDataY = (double*) malloc( m_pSkyObjSpectrum->m_nSpectrumDataSizeAllocated*sizeof(double) );
	// load spectrum data
	m_pSkyObjSpectrum->m_nSpectrumDataSize = m_pSkyObjSpectrum->LoadSpectrumFile( m_pSkyObjSpectrum->m_vectSpectrum[nSpecId].strFileName, 
										m_pSkyObjSpectrum->m_vectSpectrumDataX, m_pSkyObjSpectrum->m_vectSpectrumDataY );

	////////////////////////
	// set my spectrum graph
	wxPlotData plotData;
	plotData.Create( m_pSkyObjSpectrum->m_vectSpectrumDataX, m_pSkyObjSpectrum->m_vectSpectrumDataY, 
						m_pSkyObjSpectrum->m_nSpectrumDataSize, false );
	m_pSpectrumView->AddCurve( plotData, true, true);
	// set axis labels
	m_pSpectrumView->SetXAxisLabel( m_pSkyObjSpectrum->m_vectSpectrum[nSpecId].strXLabel );
	m_pSpectrumView->SetYAxisLabel( m_pSkyObjSpectrum->m_vectSpectrum[nSpecId].strYLabel );
	// set title
	wxString strTitle;
	int bStartLine = 0;
	// region
	if( wxStrlen( m_pSkyObjSpectrum->m_vectSpectrum[nSpecId].strRegion ) > 0 )
	{
		strTitle = m_pSkyObjSpectrum->m_vectSpectrum[nSpecId].strRegion;
		bStartLine = 1;
	}
	// telescope
	if( wxStrlen( m_pSkyObjSpectrum->m_vectSpectrum[nSpecId].strTelescope ) > 0 )
	{
		if( bStartLine == 1 )
			strTitle +=  wxT(", ") + m_pSkyObjSpectrum->m_vectSpectrum[nSpecId].strTelescope;
		else
		{
			strTitle = m_pSkyObjSpectrum->m_vectSpectrum[nSpecId].strTelescope;
			bStartLine = 1;
		}
	}
	// instrument
	if( wxStrlen( m_pSkyObjSpectrum->m_vectSpectrum[nSpecId].strInstrument ) > 0 )
	{
		if( bStartLine == 1 )
			strTitle +=  wxT(", ") + m_pSkyObjSpectrum->m_vectSpectrum[nSpecId].strInstrument;
		else
		{
			strTitle = m_pSkyObjSpectrum->m_vectSpectrum[nSpecId].strInstrument;
			bStartLine = 1;
		}
	}
	// band
	if( wxStrlen( m_pSkyObjSpectrum->m_vectSpectrum[nSpecId].strBand ) > 0 )
	{
		if( bStartLine == 1 )
			strTitle +=  wxT(", ") + m_pSkyObjSpectrum->m_vectSpectrum[nSpecId].strBand;
		else
		{
			strTitle = m_pSkyObjSpectrum->m_vectSpectrum[nSpecId].strBand;
			bStartLine = 1;
		}
	}
	// if none do a default
	if( bStartLine == 0 ) strTitle.Printf( wxT("Spectrum plot: %s"), this->m_strObjectCatName );

	m_pSpectrumView->SetPlotTitle( strTitle );

	// update interface
	Refresh( FALSE );
	Update( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnViewReferenceDoc
// Class:		CStarViewDlg
// Purpose:		view reference document
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarViewDlg::OnViewReferenceDoc( wxCommandEvent& event )
{
	int nBibCode = m_pRefBibcode->GetSelection();
	int nDocFormat = m_pRefViewFormat->GetSelection();
	wxString strUrl = wxT("");
	wxString strLinkType = wxT( "NONE" );
	wxString strSelectType = m_pRefViewFormat->GetStringSelection();

	// now check by link type
	if( strSelectType.CmpNoCase( wxT("Abstract") ) == 0 )
	{
		strLinkType = wxT( "ABSTRACT" );
	} else if( strSelectType.CmpNoCase( wxT("eJournal") ) == 0 )
	{
		strLinkType = wxT( "EJOURNAL" );
	} else if( strSelectType.CmpNoCase( wxT("Article (Pdf/Ps)") ) == 0 )
	{
		strLinkType = wxT( "ARTICLE" );
	} else if( strSelectType.CmpNoCase( wxT("Pre-Print") ) == 0 )
	{
		strLinkType = wxT( "PREPRINT" );
	} else if( strSelectType.CmpNoCase( wxT("Online Data") ) == 0 )
	{
		strLinkType = wxT( "DATA" );
	} else if( strSelectType.CmpNoCase( wxT("References") ) == 0 )
	{
		strLinkType = wxT( "REFERENCES" );
	} else if( strSelectType.CmpNoCase( wxT("Simbad") ) == 0 )
	{
		strLinkType = wxT( "SIMBAD" );
	} else if( strSelectType.CmpNoCase( wxT("Also Read") ) == 0 )
	{
		strLinkType = wxT( "AR" );
	} else if( strSelectType.CmpNoCase( wxT("Citations") ) == 0 )
	{
		strLinkType = wxT( "CITATIONS" );
	} else if( strSelectType.CmpNoCase( wxT("Associated") ) == 0 )
	{
		strLinkType = wxT( "ASSOCIATED" );
	}

			// eu = faster
			// http://cdsads.u-strasbg.fr/cgi-bin/nph-basic

//	strUrl.sprintf( "%s%s%s%s", "http://adsabs.harvard.edu/cgi-bin/nph-data_query?bibcode=", 
//						URLEncode( m_vectBibCode[nBibCode] ),
//						"&db_key=AST&link_type=", strLinkType );
	strUrl.sprintf( wxT("%s%s%s%s"), wxT("http://cdsads.u-strasbg.fr/cgi-bin/nph-data_query?bibcode="), 
						URLEncode( m_pBibDb->m_vectBibCode[nBibCode] ),
						wxT("&db_key=AST&link_type="), strLinkType );

	wxLaunchDefaultBrowser( strUrl, wxBROWSER_NEW_WINDOW );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnSelectImageSource
// Class:		CStarViewDlg
// Purpose:		use remote image(load) or local image
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarViewDlg::OnSelectImageSource( wxCommandEvent& event )
{
	wxString strImageSource = m_pUseImageSource->GetStringSelection();

	if( strImageSource.CmpNoCase( wxT("Nasa SkyView") ) == 0 )
	{
		EnableRemoteImagePanel( );

	} else
	{
		DisableRemoteImagePanel( );
	}

	DisplayImage( );

	// readjust layout
	GetSizer()->Layout( );
	Fit( );

	Refresh( FALSE );
	Update( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnSelectImageChannelType
// Class:		CStarViewDlg
// Purpose:		choose what structure image should have one channel
//				or RGB or etc
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarViewDlg::OnSelectImageChannelType( wxCommandEvent& event )
{
	wxString strImageSource = m_pRemImageChannel->GetStringSelection();

	if( strImageSource.CmpNoCase( wxT("Color RGB") ) == 0 )
	{
		m_pSizerHeadDataTop->Hide( m_pImgDataSurveyBoxSizer, 1 );
		m_pSizerHeadDataTop->Show( m_pImgDataRgbBoxSizer, 1 );
	} else 
	{
		m_pSizerHeadDataTop->Hide( m_pImgDataRgbBoxSizer, 1 );
		m_pSizerHeadDataTop->Show( m_pImgDataSurveyBoxSizer, 1 );
	}

	DisplayImage( );

	// readjust layout
	GetSizer()->Layout( );
	Fit( );

	Refresh( FALSE );
	Update( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		SetRemoteImage
// Class:		CStarViewDlg
// Purpose:		set remote image after was loaded
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarViewDlg::SetRemoteImage( )
{
	wxString strFile;

	// get current settings
	wxString strImageSource = m_pRemImageChannel->GetStringSelection();
	wxString strSurvey = m_pRemImageSurvey->GetStringSelection();
	wxString strSurveyRGB = wxT( "" );
	// check by source type
	if( strImageSource.CmpNoCase( wxT("Color RGB") ) == 0 )
	{
		strSurvey = m_pRemImageRGBCompRed->GetStringSelection() + wxT(",") +
					m_pRemImageRGBCompGreen->GetStringSelection() + wxT(",") +
					m_pRemImageRGBCompBlue->GetStringSelection();
	} 
	// color table - to use only when single channel ?
	int nColorTable = m_pRemImageColorTable->GetSelection();
	// grid overlay
	int nGridOverlay = m_pRemImageGridOverlay->GetSelection();
	// catalog overlay
	int nCatalogOverlay = m_pRemImageCatalogOverlay->GetSelection();
	// set contour overlay
	int nContourOverlay = m_pRemImageContourOverlay->GetSelection();

	// set object name if not matched do use image name to avoid dup zero
	wxString strObjectName = m_strObjectCatName;
	// if no name use id
	if( m_bHasName == 0 ) strObjectName.sprintf( wxT("%s_%d"), m_pAstroImage->m_strImageName, m_pObject->img_id );
	// setlocal file
	strFile.Printf( wxT("%s%s%s%s%s%d_%d_%d_%d%s"), m_pSkyview->GetSurveyImagePath( m_pRemImageType->GetStringSelection() ), 
						strObjectName, wxT("__"), CleanupFilename( strSurvey, 1, 0  ) ,wxT("__"), 
						nColorTable , nCatalogOverlay, nContourOverlay, nGridOverlay, wxT(".jpg") );


	// clean file name
	wxString strWxFile = strFile;
	// load image
	wxImage* pImage = new wxImage( strFile, wxBITMAP_TYPE_ANY );

	if( pImage->IsOk() )
	{
		int nHeight = pImage->GetHeight( );
		int nWidth = pImage->GetWidth( );

		// set image
		m_pObjView->SetImage( *pImage );

	} else
	{
		// todo: display error loading image???
	}

	// readjust layout
	GetSizer()->Layout( );
	Fit( );

	Refresh( FALSE );
	Update( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		DisplayImage
// Class:		CStarViewDlg
// Purpose:		display image(load) or local image
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarViewDlg::DisplayImage( )
{
	wxString strImageSource = m_pUseImageSource->GetStringSelection();

	if( strImageSource.CmpNoCase( wxT("Nasa SkyView") ) == 0 )
	{
		// check if color rgb is set and not all channels are set return
		if( m_pRemImageChannel->GetStringSelection().CmpNoCase( wxT("Color RGB") ) == 0 &&
			( m_pRemImageRGBCompRed->GetSelection() == 0 ||
			m_pRemImageRGBCompGreen->GetSelection() == 0 ||
			m_pRemImageRGBCompBlue->GetSelection() == 0 ) )
		{
			return;
		}

		///////////////////
		// call thread ... todo: replace with call to gui ..
		//m_pUnimap->m_pUnimapWorker->m_pStarDlg = this;
		DefCmdAction act;
		act.id = THREAD_ACTION_LOAD_OBJECT_IMAGE;
		act.vectObj[0] = this;

		//m_pUnimap->m_pUnimapWorker->SetAction( cmd );

		/////////////////
		// show wait dialog
		CWaitDialog* pWaitDialog = new CWaitDialog( this, wxT("Load Object Image"), 
							wxT("Loading image..."), 0, m_pUnimapWorker, &act );
		pWaitDialog->ShowModal();
		// delete/reset wait dialog
		delete( pWaitDialog );
		pWaitDialog = NULL;

		// now that's done call to set remote image
		SetRemoteImage( );

	} else
	{
		// load local image
		m_pObjView->SetImage( m_pLocalImage );
		// and disable remote image selectors
	}

	return;
}


////////////////////////////////////////////////////////////////////
// Method:		OnSelectImageSurvey
// Class:		CStarViewDlg
// Purpose:		display image when a new survey is selected
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarViewDlg::OnSelectImageSurvey( wxCommandEvent& event )
{
	DisplayImage( );

	// readjust layout
	GetSizer()->Layout( );
	Fit( );

	Refresh( FALSE );
	Update( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnSelectImageSurveyType
// Class:		CStarViewDlg
// Purpose:		display image when a new survey is selected
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarViewDlg::OnSelectImageSurveyType( wxCommandEvent& event )
{
//	int nSurveyType = m_pRemImageType->GetSelection();

	wxString strSelectType = m_pRemImageType->GetStringSelection();

	SetImageSurveyType( strSelectType );

	DisplayImage( );

	// readjust layout
	GetSizer()->Layout( );
	Fit( );

	Refresh( FALSE );
	Update( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		SetImageSurveyType
// Class:		CStarViewDlg
// Purpose:		set survey type list
// Input:		survey type string
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarViewDlg::SetImageSurveyType( wxString& strSelectType )
{
	int i = 0;

	m_pRemImageSurvey->Clear();

	// now check by link type
	if( strSelectType.CmpNoCase( wxT("Optical") ) == 0 )
	{
		for( i=0; i<m_pSkyview->m_nSurveyImageOptical; i++ ) m_pRemImageSurvey->Append( m_pSkyview->m_vectSurveyImageOptical[i] );
	} else if( strSelectType.CmpNoCase( wxT("Infrared") ) == 0 )
	{
		for( i=0; i<m_pSkyview->m_nSurveyImageInfrared; i++ ) m_pRemImageSurvey->Append( m_pSkyview->m_vectSurveyImageInfrared[i] );
	} else if( strSelectType.CmpNoCase( wxT("UV") ) == 0 )
	{
		for( i=0; i<m_pSkyview->m_nSurveyImageUV; i++ ) m_pRemImageSurvey->Append( m_pSkyview->m_vectSurveyImageUV[i] );
	} else if( strSelectType.CmpNoCase( wxT("Radio") ) == 0 )
	{
		for( i=0; i<m_pSkyview->m_nSurveyImageRadio; i++ ) m_pRemImageSurvey->Append( m_pSkyview->m_vectSurveyImageRadio[i] );
	} else if( strSelectType.CmpNoCase( wxT("X-ray") ) == 0 )
	{
		for( i=0; i<m_pSkyview->m_nSurveyImageXray; i++ ) m_pRemImageSurvey->Append( m_pSkyview->m_vectSurveyImageXray[i] );
	} else if( strSelectType.CmpNoCase( wxT("Hard X-ray") ) == 0 )
	{
		for( i=0; i<m_pSkyview->m_nSurveyImageHardXray; i++ ) m_pRemImageSurvey->Append( m_pSkyview->m_vectSurveyImageHardXray[i] );
	} else if( strSelectType.CmpNoCase( wxT("Diffuse X-ray") ) == 0 )
	{
		for( i=0; i<m_pSkyview->m_nSurveyImageDiffuseXray; i++ ) m_pRemImageSurvey->Append( m_pSkyview->m_vectSurveyImageDiffuseXray[i] );
	} else if( strSelectType.CmpNoCase( wxT("Gamma Ray") ) == 0 )
	{
		for( i=0; i<m_pSkyview->m_nSurveyImageGammaRay; i++ ) m_pRemImageSurvey->Append( m_pSkyview->m_vectSurveyImageGammaRay[i] );
	}
	// set default to zero
	m_pRemImageSurvey->SetSelection( 0 );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnSelectCatalogOverlay
// Class:		CStarViewDlg
// Purpose:		display image on select catalog overlay
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CStarViewDlg::OnSelectCatalogOverlay( wxCommandEvent& event )
{
	DisplayImage( );

	// readjust layout
	GetSizer()->Layout( );
	Fit( );

	Refresh( FALSE );
	Update( );

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	DisableRemoteImagePanel
// Class:	CStarViewDlg
// Purpose:	disable all elements in remote image panel
// Input:	file name
// Output:	status
/////////////////////////////////////////////////////////////////////
void CStarViewDlg::DisableRemoteImagePanel( )
{
	wxSizerItemList::iterator iter;
	wxSizerItemList vectList;

	// disable header part
	m_pSizerHeadData->GetItem( (size_t) 2 )->GetWindow()->Disable();
	m_pRemImageChannel->Disable( );

	// overlay panel
	m_pImgDataOverlayBox->Disable( );
	vectList = m_pSizerHeadDataOverlay->GetChildren( );
	for( iter = vectList.begin(); iter != vectList.end(); ++iter )
    {
        wxSizerItem *current = *iter;
		current->GetWindow()->Disable();
	}
	// one channel
	m_pImgDataSurveyBox->Disable( );
	vectList = m_pSizerHeadDataSurvey->GetChildren( );
	for( iter = vectList.begin(); iter != vectList.end(); ++iter )
    {
        wxSizerItem *current = *iter;
		current->GetWindow()->Disable();
	}
	// rgb
	m_pImgDataRgbBox->Disable( );
	vectList = m_pSizerHeadDataRgb->GetChildren( );
	for( iter = vectList.begin(); iter != vectList.end(); ++iter )
    {
        wxSizerItem *current = *iter;
		current->GetWindow()->Disable();
	}

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	EnableRemoteImagePanel
// Class:	CStarViewDlg
// Purpose:	enable all elements in remote image panel
// Input:	file name
// Output:	status
/////////////////////////////////////////////////////////////////////
void CStarViewDlg::EnableRemoteImagePanel( )
{
	wxSizerItemList::iterator iter;
	wxSizerItemList vectList;

	// disable header part
	m_pSizerHeadData->GetItem( (size_t) 2 )->GetWindow()->Enable();
	m_pRemImageChannel->Enable( );

	// overlay panel
	m_pImgDataOverlayBox->Enable( );
	vectList = m_pSizerHeadDataOverlay->GetChildren( );
	for( iter = vectList.begin(); iter != vectList.end(); ++iter )
    {
        wxSizerItem *current = *iter;
		current->GetWindow()->Enable();
	}
	// one channel
	m_pImgDataSurveyBox->Enable( );
	vectList = m_pSizerHeadDataSurvey->GetChildren( );
	for( iter = vectList.begin(); iter != vectList.end(); ++iter )
    {
        wxSizerItem *current = *iter;
		current->GetWindow()->Enable();
	}
	// rgb
	m_pImgDataRgbBox->Enable( );
	vectList = m_pSizerHeadDataRgb->GetChildren( );
	for( iter = vectList.begin(); iter != vectList.end(); ++iter )
    {
        wxSizerItem *current = *iter;
		current->GetWindow()->Enable();
	}

	return;
}
