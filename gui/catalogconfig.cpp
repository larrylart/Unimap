////////////////////////////////////////////////////////////////////
// Package:		Catalog Config  implementation
// File:		catalogconfig.cpp
// Purpose:		define a config view window for catalogs
//
// Created by:	Larry Lart on 02-07-2007
// Updated by:	Larry Lart on 22/03/2010
//
// Copyright:	(c) 2007-2010 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

// wxwidgets headers
#include "wx/wxprec.h"
#include "wx/statline.h"
#include "wx/minifram.h"
#include "wx/thread.h"
#include <wx/regex.h>
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// local headers
#include "../util/folders.h"
#include "../config/config.h"
#include "../config/mainconfig.h"
#include "../sky/catalog_types.h"
#include "../sky/sky.h"
#include "../sky/catalog_stars.h"
#include "../sky/catalog_dso.h"
#include "../sky/catalog_dso_ngc.h"
#include "../sky/catalog_dso_ic.h"
#include "../sky/catalog_dso_messier.h"
#include "../sky/catalog_aes.h"
#include "../image/astroimage.h"
#include "catalog_download.h"
#include "../unimap_worker.h"
#include "catalog_view.h"
#include "waitdialog.h"

// main header
#include "catalogconfig.h"

//		***** CLASS CCatalogConfig *****
////////////////////////////////////////////////////////////////////
// CCatalogConfig EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(CCatalogConfig, wxDialog)
	// use time frame handlers
	EVT_CHECKBOX( wxID_AES_CAT_USE_DATE_RANGE_START, CCatalogConfig::OnUseDateRange )
	EVT_CHECKBOX( wxID_AES_CAT_USE_DATE_RANGE_END, CCatalogConfig::OnUseDateRange )
	// use remote handlers
	EVT_CHECKBOX( wxID_STARS_CAT_USE_REMOTE, CCatalogConfig::OnUseRemote )
	EVT_CHECKBOX( wxID_DSO_CAT_USE_REMOTE, CCatalogConfig::OnUseRemote )
	EVT_CHECKBOX( wxID_RADIO_CAT_USE_REMOTE, CCatalogConfig::OnUseRemote )
	EVT_CHECKBOX( wxID_XGAMMA_CAT_USE_REMOTE, CCatalogConfig::OnUseRemote )
	EVT_CHECKBOX( wxID_SUPERNOVAS_CAT_USE_REMOTE, CCatalogConfig::OnUseRemote )
	EVT_CHECKBOX( wxID_BHOLES_CAT_USE_REMOTE, CCatalogConfig::OnUseRemote )
	EVT_CHECKBOX( wxID_MSTARS_CAT_USE_REMOTE, CCatalogConfig::OnUseRemote )
	EVT_CHECKBOX( wxID_EXP_CAT_USE_REMOTE, CCatalogConfig::OnUseRemote )
	EVT_CHECKBOX( wxID_AST_CAT_USE_REMOTE, CCatalogConfig::OnUseRemote )
	EVT_CHECKBOX( wxID_COM_CAT_USE_REMOTE, CCatalogConfig::OnUseRemote )
	EVT_CHECKBOX( wxID_AES_CAT_USE_REMOTE, CCatalogConfig::OnUseRemote )
	// catalog selection handlers
	EVT_CHOICE( wxID_STARS_CATALOG_TO_USE, CCatalogConfig::OnSetCatalog )
	EVT_CHOICE( wxID_DSO_CATALOG_TO_USE, CCatalogConfig::OnSetCatalog )
	EVT_CHOICE( wxID_RADIO_CATALOG_TO_USE, CCatalogConfig::OnSetCatalog )
	EVT_CHOICE( wxID_XGAMMA_CATALOG_TO_USE, CCatalogConfig::OnSetCatalog )
	EVT_CHOICE( wxID_SUPERNOVAS_CATALOG_TO_USE, CCatalogConfig::OnSetCatalog )
	EVT_CHOICE( wxID_BHOLES_CATALOG_TO_USE, CCatalogConfig::OnSetCatalog )
	EVT_CHOICE( wxID_MSTARS_CATALOG_TO_USE, CCatalogConfig::OnSetCatalog )
	EVT_CHOICE( wxID_EXP_CATALOG_TO_USE, CCatalogConfig::OnSetCatalog )
	EVT_CHOICE( wxID_AST_CATALOG_TO_USE, CCatalogConfig::OnSetCatalog )
	EVT_CHOICE( wxID_COM_CATALOG_TO_USE, CCatalogConfig::OnSetCatalog )
	EVT_CHOICE( wxID_AES_CATALOG_TO_USE, CCatalogConfig::OnSetCatalog )
	// on download
	EVT_BUTTON( wxID_BUTTON_DOWNLOAD_STARS, CCatalogConfig::OnCatalogDownload )
	EVT_BUTTON( wxID_BUTTON_DOWNLOAD_DSO, CCatalogConfig::OnCatalogDownload )
	EVT_BUTTON( wxID_BUTTON_DOWNLOAD_RADIO, CCatalogConfig::OnCatalogDownload )
	EVT_BUTTON( wxID_BUTTON_DOWNLOAD_XGAMMA, CCatalogConfig::OnCatalogDownload )
	EVT_BUTTON( wxID_BUTTON_DOWNLOAD_SUPERNOVAS, CCatalogConfig::OnCatalogDownload )
	EVT_BUTTON( wxID_BUTTON_DOWNLOAD_BHOLES, CCatalogConfig::OnCatalogDownload )
	EVT_BUTTON( wxID_BUTTON_DOWNLOAD_MSTARS, CCatalogConfig::OnCatalogDownload )
	EVT_BUTTON( wxID_BUTTON_DOWNLOAD_EXP, CCatalogConfig::OnCatalogDownload )
	EVT_BUTTON( wxID_BUTTON_DOWNLOAD_AST, CCatalogConfig::OnCatalogDownload )
	EVT_BUTTON( wxID_BUTTON_DOWNLOAD_COM, CCatalogConfig::OnCatalogDownload )
	EVT_BUTTON( wxID_BUTTON_DOWNLOAD_AES, CCatalogConfig::OnCatalogDownload )
	// on view
	EVT_BUTTON( wxID_BUTTON_CATVIEW_STARS, CCatalogConfig::OnCatalogView )
	EVT_BUTTON( wxID_BUTTON_CATVIEW_DSO, CCatalogConfig::OnCatalogView )
	EVT_BUTTON( wxID_BUTTON_CATVIEW_RADIO, CCatalogConfig::OnCatalogView )
	EVT_BUTTON( wxID_BUTTON_CATVIEW_XGAMMA, CCatalogConfig::OnCatalogView )
	EVT_BUTTON( wxID_BUTTON_CATVIEW_SUPERNOVAS, CCatalogConfig::OnCatalogView )
	EVT_BUTTON( wxID_BUTTON_CATVIEW_BHOLES, CCatalogConfig::OnCatalogView )
	EVT_BUTTON( wxID_BUTTON_CATVIEW_MSTARS, CCatalogConfig::OnCatalogView )
	EVT_BUTTON( wxID_BUTTON_CATVIEW_EXP, CCatalogConfig::OnCatalogView )
	EVT_BUTTON( wxID_BUTTON_CATVIEW_AST, CCatalogConfig::OnCatalogView )
	EVT_BUTTON( wxID_BUTTON_CATVIEW_COM, CCatalogConfig::OnCatalogView )
	EVT_BUTTON( wxID_BUTTON_CATVIEW_AES, CCatalogConfig::OnCatalogView )
	// validate catalogs on ok
	EVT_BUTTON( wxID_OK, CCatalogConfig::OnValidate )
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CCatalogConfig
// Purpose:	Initialize add devicedialog
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
CCatalogConfig::CCatalogConfig( wxWindow *parent, const wxString& title ) : wxDialog(parent, -1,
								title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	int i=0;

//	wxFont smallFont( 6, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT );
	m_pAstroImage = NULL;

	m_nCatalogPanel = 0;
	// reset all panels
	for( i=0; i<20; i++ ) memset( &(m_vectCatalogPanel[i]), 0, sizeof(DefPanelCatalogType) );

	// create main sizer
	wxBoxSizer *pTopSizer = new wxBoxSizer( wxVERTICAL );

	// create notebook
	m_pNotebook = new wxNotebook( this, -1, wxDefaultPosition, wxDefaultSize, wxNB_MULTILINE ); 
	m_pNotebook->Freeze();

	//-----------
	// footprint base bmp - just one ???
	m_bmpFootprint.Create( 180, 90 );
	m_bmpNoneFootprint.LoadFile( unMakeAppPath(wxT("data/catalog/none_footprint.gif")), wxBITMAP_TYPE_GIF );

	//////////////////
	// CREATE PANELS
	// :: STARS
	AddCatalogPanel( CATALOG_TYPE_STARS );
	// :: DSO
	AddCatalogPanel( CATALOG_TYPE_DSO );
	// :: RADIO
	AddCatalogPanel( CATALOG_TYPE_RADIO );
	// :: X-RAY/GAMMA
	AddCatalogPanel( CATALOG_TYPE_XGAMMA );
	// :: SUPERNOVAS
	AddCatalogPanel( CATALOG_TYPE_SUPERNOVAS );
	// :: BLACKHOLES
	AddCatalogPanel( CATALOG_TYPE_BLACKHOLES );
	// :: MULTIPLE STARS/BINARY 
	AddCatalogPanel( CATALOG_TYPE_MSTARS );
//	// :: SOLAR
//	AddCatalogPanel( CATALOG_TYPE_SOLAR );
	// :: EXOPLANETS
	AddCatalogPanel( CATALOG_TYPE_EXOPLANETS );
	// :: ASTEROIDS
	AddCatalogPanel( CATALOG_TYPE_ASTEROIDS );
	// :: COMETS
	AddCatalogPanel( CATALOG_TYPE_COMETS );
	// :: AES
	AddCatalogPanel( CATALOG_TYPE_AES );

	m_pNotebook->Thaw();
	pTopSizer->Add( m_pNotebook, 1, wxEXPAND | wxALL, 10 );
	pTopSizer->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxCENTRE | wxALL, 10 );

	// activate
	SetSizer( pTopSizer );
	SetAutoLayout(TRUE);
	pTopSizer->SetSizeHints(this);	
	Fit();
//	pTopSizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CCatalogConfig
// Purpose:	Delete my object
// Input:	mothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CCatalogConfig::~CCatalogConfig( )
{
	int i=0;

	for( i=0; i<m_nCatalogPanel; i++ )
	{
		// delete vector panel objects
		delete( m_vectCatalogPanel[i].pRecords );
		delete( m_vectCatalogPanel[i].pVersion );
		delete( m_vectCatalogPanel[i].pDate );
		delete( m_vectCatalogPanel[i].pAuthor );
		delete( m_vectCatalogPanel[i].pOrganization );
		delete( m_vectCatalogPanel[i].pCatalogToUse );
		delete( m_vectCatalogPanel[i].pCatalogDownload );
		delete( m_vectCatalogPanel[i].pCatalogView );

		// properties, remote
		if( m_vectCatalogPanel[i].pCatLoadTimeFrame ) delete( m_vectCatalogPanel[i].pCatLoadTimeFrame );
		delete( m_vectCatalogPanel[i].pCatLoadRegion );
		delete( m_vectCatalogPanel[i].pCatUseRemote );
		delete( m_vectCatalogPanel[i].pCatRemoteLocation );
		if( m_vectCatalogPanel[i].pCatRemoteUpdateLocal ) delete( m_vectCatalogPanel[i].pCatRemoteUpdateLocal );
		// foot print
		if( m_vectCatalogPanel[i].pFootprint ) delete( m_vectCatalogPanel[i].pFootprint );
		// description
		delete( m_vectCatalogPanel[i].pDesc );
		// filter
		if( m_vectCatalogPanel[i].pCatObjCat ) delete( m_vectCatalogPanel[i].pCatObjCat );
		if( m_vectCatalogPanel[i].pCatObjType ) delete( m_vectCatalogPanel[i].pCatObjType );
		if( m_vectCatalogPanel[i].pCatDontUseType ) delete( m_vectCatalogPanel[i].pCatDontUseType );
		if( m_vectCatalogPanel[i].pCatRecUseDateStart ) delete( m_vectCatalogPanel[i].pCatRecUseDateStart );
		if( m_vectCatalogPanel[i].pCatRecDateStart ) delete( m_vectCatalogPanel[i].pCatRecDateStart );
		if( m_vectCatalogPanel[i].pCatRecUseDateEnd ) delete( m_vectCatalogPanel[i].pCatRecUseDateEnd );
		if( m_vectCatalogPanel[i].pCatRecDateEnd ) delete( m_vectCatalogPanel[i].pCatRecDateEnd );
	}

	//////////////////////
	// SOME CUSTOM
	// :: STARS
	delete( m_pStarsCatMaxMagLimit );
	// :: DSO
	delete( m_pUseMessierCat );
	delete( m_pUseNgcCat );
	delete( m_pUseIcCat );

	// delete notebook
	delete( m_pNotebook );
}

////////////////////////////////////////////////////////////////////
// Method:	AddCatalogPanel
// Class:	CCatalogConfig
// Purpose:	add a new catalog panel to the notebook 
// Input:	catalog type id
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCatalogConfig::AddCatalogPanel( int nCatType )
{
	if( nCatType < 1 ) return;

	int nId = m_nCatalogPanel;
	m_vectCatalogPanel[nId].cat_type = nCatType;

	wxString strCatalogSelectLabel = _("Choose a catalog:");
	wxString strPanelTitle = _("Other");
	// some gui custom options
	wxFlexGridSizer* sizerCustomCats=NULL;
	// gui element ids
	int nCatalogToUseId=-1, nCatalogDownloadId=-1, nCatalogViewId=-1, nCatLoadRegionId=-1, nCatLoadTimeFrameId=-1,
		nCatUseRemoteId=-1, nCatRemoteLocationId=-1, nCatObjCatId=-1, 
		nCatObjTypeId=-1, nCatDontUseTypeId=-1, nCatRecUseDateStartId=-1,
		nCatRecDateStartId=-1, nCatRecUseDateEndId=-1, nCatRecDateEndId=-1,
		nCatRemoteUpdateLocalId=-1;

	wxString vectStrUseCatalog[10];
	wxString vectStrCatRemoteLocation[10];
	wxString vectStrCatObjCat[100];
	wxString vectStrCatObjType[100];
	// init defaults
	vectStrUseCatalog[0] = wxT("None");
	vectStrCatRemoteLocation[0] = wxT("Vizier");
	vectStrCatObjCat[0] = wxT("All Categories");
	vectStrCatObjType[0] = wxT("All Types");

	///////
	// set by type
	if( nCatType == CATALOG_TYPE_STARS )
	{
		strPanelTitle = wxT("Stars");
		strCatalogSelectLabel = wxT("Main Stars Catalog:");
		nCatalogToUseId = wxID_STARS_CATALOG_TO_USE;
		nCatalogDownloadId = wxID_BUTTON_DOWNLOAD_STARS;
		nCatalogViewId = wxID_BUTTON_CATVIEW_STARS;
		nCatLoadRegionId = wxID_STARS_CAT_LOAD_REGION;
		nCatUseRemoteId = wxID_STARS_CAT_USE_REMOTE;
		nCatRemoteLocationId = wxID_STARS_CATALOG_REMOTE_LOCATIONS;

	} else if( nCatType == CATALOG_TYPE_DSO )
	{
		strPanelTitle = wxT("DSO");
		strCatalogSelectLabel = wxT("Main Dso Catalog:");
		nCatalogToUseId = wxID_DSO_CATALOG_TO_USE;
		nCatalogDownloadId = wxID_BUTTON_DOWNLOAD_DSO;
		nCatalogViewId = wxID_BUTTON_CATVIEW_DSO;
		nCatLoadRegionId = wxID_DSO_CAT_LOAD_REGION;
		nCatUseRemoteId = wxID_DSO_CAT_USE_REMOTE;
		nCatRemoteLocationId = wxID_DSO_CATALOG_REMOTE_LOCATIONS;

	} else if( nCatType == CATALOG_TYPE_RADIO )
	{
		strPanelTitle = wxT("Radio");
		strCatalogSelectLabel = wxT("Radio Sources Catalog:");
		nCatalogToUseId = wxID_RADIO_CATALOG_TO_USE;
		nCatalogDownloadId = wxID_BUTTON_DOWNLOAD_RADIO;
		nCatalogViewId = wxID_BUTTON_CATVIEW_RADIO;
		nCatLoadRegionId = wxID_RADIO_CAT_LOAD_REGION;
		nCatUseRemoteId = wxID_RADIO_CAT_USE_REMOTE;
		nCatRemoteLocationId = wxID_RADIO_CATALOG_REMOTE_LOCATIONS;

	} else if( nCatType == CATALOG_TYPE_XGAMMA )
	{
		strPanelTitle = wxT("X-Ray/Gamma");
		strCatalogSelectLabel = wxT("X-Ray/Gamma Catalog:");
		nCatalogToUseId = wxID_XGAMMA_CATALOG_TO_USE;
		nCatalogDownloadId = wxID_BUTTON_DOWNLOAD_XGAMMA;
		nCatalogViewId = wxID_BUTTON_CATVIEW_XGAMMA;
		nCatLoadRegionId = wxID_XGAMMA_CAT_LOAD_REGION;
		nCatUseRemoteId = wxID_XGAMMA_CAT_USE_REMOTE;
		nCatRemoteLocationId = wxID_XGAMMA_CATALOG_REMOTE_LOCATIONS;

	} else if( nCatType == CATALOG_TYPE_SUPERNOVAS )
	{
		strPanelTitle = wxT("Supernovas");
		strCatalogSelectLabel = wxT("Supernovas Catalog:");
		nCatalogToUseId = wxID_SUPERNOVAS_CATALOG_TO_USE;
		nCatalogDownloadId = wxID_BUTTON_DOWNLOAD_SUPERNOVAS;
		nCatalogViewId = wxID_BUTTON_CATVIEW_SUPERNOVAS;
		nCatLoadRegionId = wxID_SUPERNOVAS_CAT_LOAD_REGION;
		nCatUseRemoteId = wxID_SUPERNOVAS_CAT_USE_REMOTE;
		nCatRemoteLocationId = wxID_SUPERNOVAS_CATALOG_REMOTE_LOCATIONS;

	} else if( nCatType == CATALOG_TYPE_BLACKHOLES )
	{
		strPanelTitle = wxT("Black Holes");
		strCatalogSelectLabel = wxT("Black Holes Catalog:");
		nCatalogToUseId = wxID_BHOLES_CATALOG_TO_USE;
		nCatalogDownloadId = wxID_BUTTON_DOWNLOAD_BHOLES;
		nCatalogViewId = wxID_BUTTON_CATVIEW_BHOLES;
		nCatLoadRegionId = wxID_BHOLES_CAT_LOAD_REGION;
		nCatUseRemoteId = wxID_BHOLES_CAT_USE_REMOTE;
		nCatRemoteLocationId = wxID_BHOLES_CATALOG_REMOTE_LOCATIONS;

	} else if( nCatType == CATALOG_TYPE_MSTARS )
	{
		strPanelTitle = wxT("Double/Multiple Stars");
		strCatalogSelectLabel = wxT("D/M Stars Catalog:");
		nCatalogToUseId = wxID_MSTARS_CATALOG_TO_USE;
		nCatalogDownloadId = wxID_BUTTON_DOWNLOAD_MSTARS;
		nCatalogViewId = wxID_BUTTON_CATVIEW_MSTARS;
		nCatLoadRegionId = wxID_MSTARS_CAT_LOAD_REGION;
		nCatUseRemoteId = wxID_MSTARS_CAT_USE_REMOTE;
		nCatRemoteLocationId = wxID_MSTARS_CATALOG_REMOTE_LOCATIONS;

	} else if( nCatType == CATALOG_TYPE_EXOPLANETS )
	{
		strPanelTitle = wxT("Extra-Solar Planets");
		strCatalogSelectLabel = wxT("Extra-Solar Panets Catalog:");
		nCatalogToUseId = wxID_EXP_CATALOG_TO_USE;
		nCatalogDownloadId = wxID_BUTTON_DOWNLOAD_EXP;
		nCatalogViewId = wxID_BUTTON_CATVIEW_EXP;
		nCatLoadRegionId = wxID_EXP_CAT_LOAD_REGION;
		nCatUseRemoteId = wxID_EXP_CAT_USE_REMOTE;
		nCatRemoteLocationId = wxID_EXP_CATALOG_REMOTE_LOCATIONS;

	} else if( nCatType == CATALOG_TYPE_ASTEROIDS )
	{
		strPanelTitle = wxT("Asteroids");
		strCatalogSelectLabel = wxT("Asteroids Catalog:");
		nCatalogToUseId = wxID_AST_CATALOG_TO_USE;
		nCatalogDownloadId = wxID_BUTTON_DOWNLOAD_AST;
		nCatalogViewId = wxID_BUTTON_CATVIEW_AST;
		nCatLoadRegionId = wxID_AST_CAT_LOAD_REGION;
		nCatUseRemoteId = wxID_AST_CAT_USE_REMOTE;
		nCatRemoteLocationId = wxID_AST_CATALOG_REMOTE_LOCATIONS;

	} else if( nCatType == CATALOG_TYPE_COMETS )
	{
		strPanelTitle = wxT("Comets");
		strCatalogSelectLabel = wxT("Comets Catalog:");
		nCatalogToUseId = wxID_COM_CATALOG_TO_USE;
		nCatalogDownloadId = wxID_BUTTON_DOWNLOAD_COM;
		nCatalogViewId = wxID_BUTTON_CATVIEW_COM;
		nCatLoadRegionId = wxID_COM_CAT_LOAD_REGION;
		nCatUseRemoteId = wxID_COM_CAT_USE_REMOTE;
		nCatRemoteLocationId = wxID_COM_CATALOG_REMOTE_LOCATIONS;

	} else if( nCatType == CATALOG_TYPE_AES )
	{
		strPanelTitle = wxT("Artificial Earth Satellites");
		strCatalogSelectLabel = wxT("Artificial Earth Satellites:");
		nCatalogToUseId = wxID_AES_CATALOG_TO_USE;
		nCatalogDownloadId = wxID_BUTTON_DOWNLOAD_AES;
		nCatalogViewId = wxID_BUTTON_CATVIEW_AES;
		nCatLoadRegionId = wxID_AES_CAT_LOAD_REGION;
		nCatUseRemoteId = wxID_AES_CAT_USE_REMOTE;
		nCatRemoteLocationId = wxID_AES_CATALOG_REMOTE_LOCATIONS;
		// new ids
		nCatObjCatId = wxID_AES_CATALOG_SAT_OBJECT_CAT;
		nCatRecUseDateStartId = wxID_AES_CAT_USE_DATE_RANGE_START;
		nCatRecUseDateEndId = wxID_AES_CAT_USE_DATE_RANGE_END;

	}

/*
 else if( nCatType == CATALOG_TYPE_SOLAR )
	{
		strPanelTitle = _("Solar");
		strCatalogSelectLabel = _("Solar Objects Catalog:");
		nCatalogToUseId = wxID_SOLAR_CATALOG_TO_USE;
		nCatalogDownloadId = wxID_BUTTON_DOWNLOAD_SOLAR;
		nCatLoadRegionId = wxID_SOLAR_CAT_LOAD_REGION;
		nCatUseRemoteId = wxID_SOLAR_CAT_USE_REMOTE;
		nCatRemoteLocationId = wxID_SOLAR_CATALOG_REMOTE_LOCATIONS;

	}
*/

	// create new panel
	wxPanel* pPanel = new wxPanel( m_pNotebook );

	/////////////////////////////
	// PANEL SIZER - layout of elements
	wxFlexGridSizer* sizerPage = new wxFlexGridSizer( 4, 1, 0, 0 );
	sizerPage->AddGrowableCol( 0 );
	// only dso has extra inner field
	if( nCatType == CATALOG_TYPE_DSO )
		sizerPage->AddGrowableRow( 5 );
	else
		sizerPage->AddGrowableRow( 4 );
	wxFlexGridSizer* sizerCatalog = new wxFlexGridSizer( 1, 4, 3, 10 );
	// size header A line
	m_vectCatalogPanel[nId].sizerHeaderA = new wxFlexGridSizer( 1, 6, 3, 3 );
	m_vectCatalogPanel[nId].sizerHeaderA->AddGrowableCol( 0 );
	m_vectCatalogPanel[nId].sizerHeaderA->AddGrowableCol( 5 );
	// size header B line
	m_vectCatalogPanel[nId].sizerHeaderB = new wxFlexGridSizer( 1, 4, 3, 3 );
	m_vectCatalogPanel[nId].sizerHeaderB->AddGrowableCol( 0 );
	m_vectCatalogPanel[nId].sizerHeaderB->AddGrowableCol( 3 );
	// sizer for data and image
	wxFlexGridSizer* sizerDataAndImg = new wxFlexGridSizer( 1, 2, 0, 0 );
	sizerDataAndImg->AddGrowableCol( 0 );
	wxFlexGridSizer* sizerData = new wxFlexGridSizer( 5, 2, 5, 3 );
	sizerData->AddGrowableCol( 0 );
	sizerData->AddGrowableCol( 1 );
	sizerData->AddGrowableRow( 0 );

	///////////////
	// case here by some cutom types - with 4 params or 3 
	if( nCatType == CATALOG_TYPE_STARS || nCatType == CATALOG_TYPE_AES )
	{
		sizerData->AddGrowableRow( 0 );
		sizerData->AddGrowableRow( 3 );
		sizerData->AddGrowableRow( 1 );
		sizerData->AddGrowableRow( 2 );

	} else
	{
		sizerData->AddGrowableRow( 0 );
		sizerData->AddGrowableRow( 2 );
	}

	// static boxes
	// :: catalog select
	sizerPage->Add( sizerCatalog, 0, wxALIGN_CENTER | wxALIGN_BOTTOM | wxTOP, 10 );
	// :: DETAILS
	sizerPage->Add( m_vectCatalogPanel[nId].sizerHeaderA, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL |wxGROW|wxEXPAND| wxTOP, 5 );
	sizerPage->Add( m_vectCatalogPanel[nId].sizerHeaderB, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL |wxGROW|wxEXPAND| wxTOP, 5 );
	// :: DATA
	sizerPage->Add( sizerDataAndImg, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW|wxTOP, 5 );
	wxStaticBox* pBoxData = new wxStaticBox( pPanel, -1, wxT("Parameters"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* sizerBoxData = new wxStaticBoxSizer( pBoxData, wxHORIZONTAL );
	sizerBoxData->Add( sizerData, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL |wxLEFT|wxGROW|wxRIGHT, 10 );
	sizerDataAndImg->Add( sizerBoxData, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL |wxGROW| wxRIGHT, 3 );

	///////////////////////
	// this panel could be either image or data/filters etc
	wxFlexGridSizer* sizerDataRight=NULL;
	wxStaticBoxSizer* sizerBoxImg;
	if( nCatType == CATALOG_TYPE_AES )
	{
		sizerDataRight = new wxFlexGridSizer( 5, 2, 5, 3 );
		sizerData->AddGrowableCol( 0 );
		sizerData->AddGrowableCol( 1 );

		// :: FILTER
		wxStaticBox* pBoxImg = new wxStaticBox( pPanel, -1, wxT("Filters"), wxPoint(-1,-1), wxDefaultSize );
		sizerBoxImg = new wxStaticBoxSizer( pBoxImg, wxHORIZONTAL );
		sizerBoxImg->Add( sizerDataRight, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL |wxLEFT|wxGROW|wxRIGHT, 10 );
		sizerDataAndImg->Add( sizerBoxImg, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL |wxGROW| wxLEFT, 3 );

	} else
	{
		// :: IMAGE
		wxStaticBox* pBoxImg = new wxStaticBox( pPanel, -1, wxT("Footprint"), wxPoint(-1,-1), wxDefaultSize );
		sizerBoxImg = new wxStaticBoxSizer( pBoxImg, wxHORIZONTAL );
		sizerDataAndImg->Add( sizerBoxImg, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL |wxGROW| wxLEFT, 3 );
	}

	///////////////
	// case here by some cutom types
	if( nCatType == CATALOG_TYPE_DSO )
	{
		// :: CUSTOM CATALOGS
		sizerCustomCats = new wxFlexGridSizer( 1, 3, 0, 0 );
		sizerCustomCats->AddGrowableCol( 0 );
		sizerCustomCats->AddGrowableCol( 2 );
		sizerPage->Add( sizerCustomCats, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL |wxGROW|wxTOP, 5 );
	}

	// :: DESCRIPTION
	wxStaticBox* pBoxDesc = new wxStaticBox( pPanel, -1, wxT("Description"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* sizerBoxDesc = new wxStaticBoxSizer( pBoxDesc, wxHORIZONTAL );
	sizerPage->Add( sizerBoxDesc, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL |wxGROW );

	///////////////////////
	// panel elements
	// header
	m_vectCatalogPanel[nId].pRecords = new wxStaticText( pPanel, -1, wxT("0"));
	m_vectCatalogPanel[nId].pRecords->SetForegroundColour( *wxRED );
	m_vectCatalogPanel[nId].pVersion = new wxStaticText( pPanel, -1, wxT("0.0"));
	m_vectCatalogPanel[nId].pVersion->SetForegroundColour( *wxRED );
	m_vectCatalogPanel[nId].pDate = new wxStaticText( pPanel, -1, wxT("00-00-0000"));
	m_vectCatalogPanel[nId].pDate->SetForegroundColour( *wxRED );
	m_vectCatalogPanel[nId].pAuthor = new wxStaticText( pPanel, -1, wxT("none"));
	m_vectCatalogPanel[nId].pAuthor->SetForegroundColour( *wxRED );
	m_vectCatalogPanel[nId].pOrganization = new wxStaticText( pPanel, -1, wxT("none"));
	m_vectCatalogPanel[nId].pOrganization->SetForegroundColour( *wxRED );

	//////////////////////////
	// properties
	m_vectCatalogPanel[nId].pCatalogToUse = new wxChoice( pPanel, nCatalogToUseId,
										wxDefaultPosition, wxSize(315,-1), 1, 
										vectStrUseCatalog, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_vectCatalogPanel[nId].pCatalogToUse->SetSelection( 0 );
	m_vectCatalogPanel[nId].pCatalogDownload = new wxButton( pPanel, nCatalogDownloadId, wxT("Download"), wxPoint( -1, -1 ), wxSize( 60, -1 ) );
	m_vectCatalogPanel[nId].pCatalogView = new wxButton( pPanel, nCatalogViewId, wxT("View"), wxPoint( -1, -1 ), wxSize( 40, -1 ) );
	m_vectCatalogPanel[nId].pCatalogView->Disable();

	// data
	if( nCatType == CATALOG_TYPE_AES ) 
		m_vectCatalogPanel[nId].pCatLoadTimeFrame = new wxCheckBox( pPanel, nCatLoadTimeFrameId, wxT("") );
	else
		m_vectCatalogPanel[nId].pCatLoadTimeFrame = NULL;

	m_vectCatalogPanel[nId].pCatLoadRegion = new wxCheckBox( pPanel, nCatLoadRegionId, wxT("") );
	m_vectCatalogPanel[nId].pCatUseRemote = new wxCheckBox( pPanel, nCatUseRemoteId, wxT("") );
	m_vectCatalogPanel[nId].pCatRemoteLocation = new wxChoice( pPanel, nCatRemoteLocationId,
										wxDefaultPosition, wxSize(100,-1), 1, 
										vectStrCatRemoteLocation, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_vectCatalogPanel[nId].pCatRemoteLocation->SetSelection( 0 );
	// custom
	if( nCatType == CATALOG_TYPE_AES )
	{
		m_vectCatalogPanel[nId].pCatRemoteUpdateLocal = new wxCheckBox( pPanel, nCatRemoteUpdateLocalId, wxT("") );

	} else
	{
		m_vectCatalogPanel[nId].pCatRemoteUpdateLocal = NULL;
	}

	///////////////////////
	// footprint coverage bitmap/or filters
	if( nCatType == CATALOG_TYPE_AES )
	{
		// :: object category
		m_vectCatalogPanel[nId].pCatObjCat = new wxChoice( pPanel, nCatObjCatId,
										wxDefaultPosition, wxSize(210,-1), 1, 
										vectStrCatObjCat, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
		// :: object type
		m_vectCatalogPanel[nId].pCatObjType = new wxChoice( pPanel, nCatObjTypeId,
										wxDefaultPosition, wxSize(210,-1), 1, 
										vectStrCatObjType, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
		// :: not to use type
		m_vectCatalogPanel[nId].pCatDontUseType = new wxCheckBox( pPanel, nCatDontUseTypeId, wxT("Ignore Space Debris Objects") );
		// :: start date
		m_vectCatalogPanel[nId].pCatRecUseDateStart = new wxCheckBox( pPanel, nCatRecUseDateStartId, wxT("Start date:") );
		m_vectCatalogPanel[nId].pCatRecDateStart = new wxDatePickerCtrl( pPanel, nCatRecDateStartId, wxDefaultDateTime,
									wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN );
		//m_vectCatalogPanel[nId].pCatRecDateStart->SetRange( wxDateTime( 2415020.0 ), wxDateTime( 2488069.5 ) );
		m_vectCatalogPanel[nId].pCatRecDateStart->SetValue( wxDateTime( 2415020.5 ) );
		// :: end date
		m_vectCatalogPanel[nId].pCatRecUseDateEnd = new wxCheckBox( pPanel, nCatRecUseDateEndId, wxT("End date:") );
		m_vectCatalogPanel[nId].pCatRecDateEnd = new wxDatePickerCtrl( pPanel, nCatRecDateEndId, wxDefaultDateTime,
									wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN );
		//m_vectCatalogPanel[nId].pCatRecDateEnd->SetRange( wxDateTime( 2415020.0 ), wxDateTime( 2488069.5 ) );

		// reset other to null
		m_vectCatalogPanel[nId].pFootprint = NULL;

	} else
	{
		m_vectCatalogPanel[nId].pCatObjCat = NULL;
		m_vectCatalogPanel[nId].pCatObjType = NULL;
		m_vectCatalogPanel[nId].pCatDontUseType = NULL;
		m_vectCatalogPanel[nId].pCatRecUseDateStart = NULL;
		m_vectCatalogPanel[nId].pCatRecDateStart = NULL;
		m_vectCatalogPanel[nId].pCatRecUseDateEnd = NULL;
		m_vectCatalogPanel[nId].pCatRecDateEnd = NULL;
		m_vectCatalogPanel[nId].pFootprint = new wxStaticBitmap( pPanel, -1, m_bmpFootprint );
	}

	///////////////////////////////
	// text control - with CUSTOM
	if( nCatType == CATALOG_TYPE_DSO )
		m_vectCatalogPanel[nId].pDesc = new wxTextCtrl( pPanel, -1, wxT(""), wxDefaultPosition, wxSize(500,100), wxTE_MULTILINE|wxTE_RICH2|wxTE_READONLY );
	else
		m_vectCatalogPanel[nId].pDesc = new wxTextCtrl( pPanel, -1, wxT(""), wxDefaultPosition, wxSize(500,120), wxTE_MULTILINE|wxTE_RICH2|wxTE_READONLY );

	///////////////
	// add some extra custom elements
	if( nCatType == CATALOG_TYPE_STARS )
	{
		m_pStarsCatMaxMagLimit = new wxTextCtrl( pPanel, -1, wxT(""), wxDefaultPosition, wxSize(80,-1) );

	} else if( nCatType == CATALOG_TYPE_DSO )
	{
		//  custom dso catalogs
		m_pUseMessierCat = new wxCheckBox( pPanel, wxID_CHECK_USE_CAT_MESSIER, wxT("Use MESSIER") );
		m_pUseNgcCat = new wxCheckBox( pPanel, wxID_CHECK_USE_CAT_NGC, wxT("Use NGC") );
		m_pUseIcCat = new wxCheckBox( pPanel, wxID_CHECK_USE_CAT_IC, wxT("Use IC")  );
	}

	///////////////////////////////////
	// Populate panel
	// :: records
	m_vectCatalogPanel[nId].sizerHeaderA->Add( new wxStaticText(pPanel, -1, wxT("Records:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	m_vectCatalogPanel[nId].sizerHeaderA->Add( m_vectCatalogPanel[nId].pRecords, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxRIGHT, 10  );
	// :: date
	m_vectCatalogPanel[nId].sizerHeaderA->Add( new wxStaticText(pPanel, -1, wxT("Date:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	m_vectCatalogPanel[nId].sizerHeaderA->Add( m_vectCatalogPanel[nId].pDate, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxRIGHT, 10  );
	// :: version
	m_vectCatalogPanel[nId].sizerHeaderA->Add( new wxStaticText(pPanel, -1, wxT("Version:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	m_vectCatalogPanel[nId].sizerHeaderA->Add( m_vectCatalogPanel[nId].pVersion, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: author
	m_vectCatalogPanel[nId].sizerHeaderB->Add( new wxStaticText(pPanel, -1, wxT("Author:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	m_vectCatalogPanel[nId].sizerHeaderB->Add( m_vectCatalogPanel[nId].pAuthor, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxRIGHT, 10  );
	// :: organizations
	m_vectCatalogPanel[nId].sizerHeaderB->Add( new wxStaticText(pPanel, -1, wxT("Organization:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	m_vectCatalogPanel[nId].sizerHeaderB->Add( m_vectCatalogPanel[nId].pOrganization, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	// main/catalog to use
	sizerCatalog->Add(new wxStaticText( pPanel, -1, strCatalogSelectLabel ), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerCatalog->Add( m_vectCatalogPanel[nId].pCatalogToUse, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerCatalog->Add( m_vectCatalogPanel[nId].pCatalogDownload, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerCatalog->Add( m_vectCatalogPanel[nId].pCatalogView, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	// :: if to load time frame or not
	if( nCatType == CATALOG_TYPE_AES ) 
	{
		sizerData->Add( new wxStaticText(pPanel, -1, wxT("Load Only Time-Frame in Use:")), 0,
								wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
		sizerData->Add( m_vectCatalogPanel[nId].pCatLoadTimeFrame, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	}

	// flag to set if to use all catalog (if local) or just loa region
	sizerData->Add( new wxStaticText(pPanel, -1, wxT("Load Only Region of Interest:")), 0,
							wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerData->Add( m_vectCatalogPanel[nId].pCatLoadRegion, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	/////////////////////////////
	//  >>> CUSTOM INSERT <<<
	if( nCatType == CATALOG_TYPE_STARS )
	{
		// :: max limit magnituted
		sizerData->Add( new wxStaticText(pPanel, -1, wxT("Load up to Magnitude:")), 0,
								wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
		sizerData->Add( m_pStarsCatMaxMagLimit, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	}

	// if to load remote or local
	sizerData->Add( new wxStaticText(pPanel, -1, wxT("Load from Remote Location:")), 0,
							wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerData->Add( m_vectCatalogPanel[nId].pCatUseRemote, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: download remote location
	sizerData->Add( new wxStaticText(pPanel, -1, wxT("Remote Location To Query:")), 0,
							wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerData->Add( m_vectCatalogPanel[nId].pCatRemoteLocation, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// custom - update local
	if( nCatType == CATALOG_TYPE_AES )
	{
		sizerData->Add( new wxStaticText(pPanel, -1, wxT("Update Local Database:")), 0,
								wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
		sizerData->Add( m_vectCatalogPanel[nId].pCatRemoteUpdateLocal, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	}

	/////////////////////////////
	//  >>> CUSTOM INSERT <<<
	if( nCatType == CATALOG_TYPE_DSO )
	{
		// use merssier catalog
		sizerCustomCats->Add( m_pUseMessierCat, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL|wxRIGHT, 15  );
		// use ngc cat
		sizerCustomCats->Add( m_pUseNgcCat, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxRIGHT, 15  );
		// use ic cat
		sizerCustomCats->Add( m_pUseIcCat, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	}
	
	//////////////////
	// :: add footprint image /OR/ filter
	if( nCatType == CATALOG_TYPE_AES )
	{
		// :: load satellite type
		sizerDataRight->Add( new wxStaticText(pPanel, -1, wxT("Category:")), 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
		sizerDataRight->Add( m_vectCatalogPanel[nId].pCatObjCat, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
		// :: object types
		sizerDataRight->Add( new wxStaticText(pPanel, -1, wxT("Object Type:")), 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
		sizerDataRight->Add( m_vectCatalogPanel[nId].pCatObjType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
		// :: ignore types
		sizerDataRight->AddStretchSpacer();
		sizerDataRight->Add( m_vectCatalogPanel[nId].pCatDontUseType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
		// :: start date
		sizerDataRight->Add( m_vectCatalogPanel[nId].pCatRecUseDateStart, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
		sizerDataRight->Add( m_vectCatalogPanel[nId].pCatRecDateStart, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
		// :: end date
		sizerDataRight->Add( m_vectCatalogPanel[nId].pCatRecUseDateEnd, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
		sizerDataRight->Add( m_vectCatalogPanel[nId].pCatRecDateEnd, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	} else
	{
		sizerBoxImg->Add( m_vectCatalogPanel[nId].pFootprint, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL  );
	}
	// :: text 
	sizerBoxDesc->Add( m_vectCatalogPanel[nId].pDesc, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW  );

	// SET MY PANEL IN NOTEBOOK
	pPanel->SetSizer( sizerPage );
	m_pNotebook->AddPage( pPanel, strPanelTitle );

	m_nCatalogPanel++;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetConfig
// Class:	CCatalogConfig
// Purpose:	Set and get my reference from the config object
// Input:	pointer to star catalog and main config
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCatalogConfig::SetConfig( CSky* pSky, CConfigMain* pMainConfig, CAstroImage* pAstroImage )
{
	m_pSky = pSky;
	m_pUnimapWorker = pSky->m_pUnimapWorker;
	m_pAstroImage = pAstroImage;

	int i=0, j=0, nConfigCatId=0, bCatLoadLocalRegion=0, bCatLoadTimeFrame=0,
		bCatLoadRemote=0, nCatLoadRemoteLocation=0, bCatRemoteUpdateLocal=0,
		nCatObjCat=0, nCatObjType=0, bCatDontUseType=0, bCatRecUseDateStart=0,
		bCatRecUseDateEnd=0;
	double nVarFloat = 0.0, nCatRecDateStart=0.0, nCatRecDateEnd=0.0;
	wxString strTmp=wxT("");
	
	/////////////////////////////////
	// :: populate catalogs in my selects
	for( i=0; i<m_nCatalogPanel; i++ )
	{
		m_vectCatalogPanel[i].pCatalogToUse->Freeze();
		m_vectCatalogPanel[i].pCatalogToUse->Clear();
		////////////////////////
		// >>> CUSTOM CASE <<<
		if( m_vectCatalogPanel[i].cat_type == CATALOG_TYPE_STARS )
		{
			m_vectCatalogPanel[i].nMapCatalogId = 0;

		} else
		{
			m_vectCatalogPanel[i].pCatalogToUse->Append( wxT("None") );
			m_vectCatalogPanel[i].nMapCatalogId=1;
			m_vectCatalogPanel[i].vectMapCatalogId[0]=-1;
		}

		// special case ???
		if( m_vectCatalogPanel[i].cat_type == CATALOG_TYPE_AES )
		{
			//m_vectCatalogPanel[i].pCatObjType->Clear();

		} 

		// set selectio change flag to zero
		m_vectCatalogPanel[i].bSelectionChanged = 0;
		m_vectCatalogPanel[i].pCatalogToUse->Thaw();
	}

	//////////////
	// for all loaded cats
	for( i=0; i<m_pSky->m_nCatalogDef; i++ )
	{
		// check if not custom
		if( !m_pSky->m_vectCatalogDef[i].m_nCustomUse )
		{
			int nPanelId = GetPanelIdByCatalogType( m_pSky->m_vectCatalogDef[i].m_nObjectType );
			if( nPanelId < 0 || nPanelId >= m_nCatalogPanel ) continue;

			m_vectCatalogPanel[nPanelId].pCatalogToUse->Freeze();

			// set my cat list and map vectors
			int nMapId = m_vectCatalogPanel[nPanelId].nMapCatalogId;
			m_vectCatalogPanel[nPanelId].pCatalogToUse->Append( m_pSky->m_vectCatalogDef[i].m_strName );
			m_vectCatalogPanel[nPanelId].vectMapCatalogId[nMapId] = m_pSky->m_vectCatalogDef[i].m_nId;
			m_vectCatalogPanel[nPanelId].nMapCatalogId++;

			m_vectCatalogPanel[nPanelId].pCatalogToUse->Thaw();
		}
	}

	/////////////
	// Now set/config all panels data
	for( i=0; i<m_nCatalogPanel; i++ )
	{
		m_vectCatalogPanel[i].pCatalogToUse->Freeze();

		// set default selection to zero
		m_vectCatalogPanel[i].pCatalogToUse->SetSelection( 0 );

		//////////////////////////////////
		// CUSTOM :: GET CONFIG CAT IDs
		if( m_vectCatalogPanel[i].cat_type == CATALOG_TYPE_STARS )
		{
			if( m_pAstroImage && m_pAstroImage->m_nCatalogForMatching >= 0 )
				nConfigCatId = m_pAstroImage->m_nCatalogForMatching;
			else
				nConfigCatId = pMainConfig->m_nCatalogForMatching;

			bCatLoadLocalRegion = pMainConfig->m_bStarsCatLoadLocalRegion;
			bCatLoadRemote = pMainConfig->m_bStarsCatLoadRemote;
			nCatLoadRemoteLocation = pMainConfig->m_nStarsCatLoadRemoteLocation;

		} else if( m_vectCatalogPanel[i].cat_type == CATALOG_TYPE_DSO )
		{
			nConfigCatId = pMainConfig->m_nDsoCatalogToUse;
			bCatLoadLocalRegion = pMainConfig->m_bDsoCatLoadLocalRegion;
			bCatLoadRemote = pMainConfig->m_bDsoCatLoadRemote;
			nCatLoadRemoteLocation = pMainConfig->m_nDsoCatLoadRemoteLocation;

		} else if( m_vectCatalogPanel[i].cat_type == CATALOG_TYPE_RADIO )
		{
			nConfigCatId = pMainConfig->m_nRadioCatalogToUse;
			bCatLoadLocalRegion = pMainConfig->m_bRadioCatLoadLocalRegion;
			bCatLoadRemote = pMainConfig->m_bRadioCatLoadRemote;
			nCatLoadRemoteLocation = pMainConfig->m_nRadioCatLoadRemoteLocation;

		} else if( m_vectCatalogPanel[i].cat_type == CATALOG_TYPE_XGAMMA )
		{
			nConfigCatId = pMainConfig->m_nXGammaCatalogToUse;
			bCatLoadLocalRegion = pMainConfig->m_bXGammaCatLoadLocalRegion;
			bCatLoadRemote = pMainConfig->m_bXGammaCatLoadRemote;
			nCatLoadRemoteLocation = pMainConfig->m_nXGammaCatLoadRemoteLocation;

		} else if( m_vectCatalogPanel[i].cat_type == CATALOG_TYPE_SUPERNOVAS )
		{
			nConfigCatId = pMainConfig->m_nSNCatalogToUse;
			bCatLoadLocalRegion = pMainConfig->m_bSNCatLoadLocalRegion;
			bCatLoadRemote = pMainConfig->m_bSNCatLoadRemote;
			nCatLoadRemoteLocation = pMainConfig->m_nSNCatLoadRemoteLocation;

		} else if( m_vectCatalogPanel[i].cat_type == CATALOG_TYPE_BLACKHOLES )
		{
			nConfigCatId = pMainConfig->m_nBHCatalogToUse;
			bCatLoadLocalRegion = pMainConfig->m_bBHCatLoadLocalRegion;
			bCatLoadRemote = pMainConfig->m_bBHCatLoadRemote;
			nCatLoadRemoteLocation = pMainConfig->m_nBHCatLoadRemoteLocation;

		} else if( m_vectCatalogPanel[i].cat_type == CATALOG_TYPE_MSTARS )
		{
			nConfigCatId = pMainConfig->m_nMSCatalogToUse;
			bCatLoadLocalRegion = pMainConfig->m_bMSCatLoadLocalRegion;
			bCatLoadRemote = pMainConfig->m_bMSCatLoadRemote;
			nCatLoadRemoteLocation = pMainConfig->m_nMSCatLoadRemoteLocation;

		} else if( m_vectCatalogPanel[i].cat_type == CATALOG_TYPE_EXOPLANETS )
		{
			nConfigCatId = pMainConfig->m_nEXPCatalogToUse;
			bCatLoadLocalRegion = pMainConfig->m_bEXPCatLoadLocalRegion;
			bCatLoadRemote = pMainConfig->m_bEXPCatLoadRemote;
			nCatLoadRemoteLocation = pMainConfig->m_nEXPCatLoadRemoteLocation;

		} else if( m_vectCatalogPanel[i].cat_type == CATALOG_TYPE_ASTEROIDS )
		{
			nConfigCatId = pMainConfig->m_nASTCatalogToUse;
			bCatLoadLocalRegion = pMainConfig->m_bASTCatLoadLocalRegion;
			bCatLoadRemote = pMainConfig->m_bASTCatLoadRemote;
			nCatLoadRemoteLocation = pMainConfig->m_nASTCatLoadRemoteLocation;

		} else if( m_vectCatalogPanel[i].cat_type == CATALOG_TYPE_COMETS )
		{
			nConfigCatId = pMainConfig->m_nCOMCatalogToUse;
			bCatLoadLocalRegion = pMainConfig->m_bCOMCatLoadLocalRegion;
			bCatLoadRemote = pMainConfig->m_bCOMCatLoadRemote;
			nCatLoadRemoteLocation = pMainConfig->m_nCOMCatLoadRemoteLocation;

		} else if( m_vectCatalogPanel[i].cat_type == CATALOG_TYPE_AES )
		{
			nConfigCatId = pMainConfig->m_nAESCatalogToUse;
			bCatLoadTimeFrame = pMainConfig->m_bAESCatLoadTimeFrame;
			bCatLoadLocalRegion = pMainConfig->m_bAESCatLoadLocalRegion;
			bCatLoadRemote = pMainConfig->m_bAESCatLoadRemote;
			nCatLoadRemoteLocation = pMainConfig->m_nAESCatLoadRemoteLocation;
			// new
			bCatRemoteUpdateLocal = pMainConfig->m_bAESCatRemoteUpdateLocal;
			nCatObjCat = pMainConfig->m_nAESCatObjCat;
			nCatObjType = pMainConfig->m_nAESCatObjType;
			bCatDontUseType = pMainConfig->m_bAESCatDontUseType;
			bCatRecUseDateStart = pMainConfig->m_bAESCatRecUseDateStart;
			nCatRecDateStart = pMainConfig->m_nAESCatRecDateStart;
			bCatRecUseDateEnd = pMainConfig->m_bAESCatRecUseDateEnd;
			nCatRecDateEnd = pMainConfig->m_nAESCatRecDateEnd;
		}

		/////////////////////////////////
		// get catalog id
		int nCatId = GetSelectedCatId( i, nConfigCatId );
		if( nCatId >= 0 )
			m_vectCatalogPanel[i].pCatalogToUse->SetSelection( nCatId );
		else
			m_vectCatalogPanel[i].pCatalogToUse->SetSelection( 0 );

		m_vectCatalogPanel[i].pCatalogToUse->Thaw();

		////////////////
		// now call values/defaults 
		CheckCatalogByType( m_vectCatalogPanel[i].cat_type );

		// get if to load only time frame
		if( m_vectCatalogPanel[i].pCatLoadTimeFrame ) m_vectCatalogPanel[i].pCatLoadTimeFrame->SetValue( (bool) bCatLoadTimeFrame );

		// get if to load region or all
		m_vectCatalogPanel[i].pCatLoadRegion->SetValue( (bool) bCatLoadLocalRegion );

		// this check to see if set in remote mode
		if( m_vectCatalogPanel[i].pCatUseRemote->IsEnabled() )
		{
			// get if to load remotely or use loacal if any
			SetUseRemoteCat( m_vectCatalogPanel[i].cat_type, (bool) bCatLoadRemote );
		}
		// set remote query location
		m_vectCatalogPanel[i].pCatRemoteLocation->SetSelection( GetQuerySourceSelectionId( m_vectCatalogPanel[i].cat_type, nCatLoadRemoteLocation ) );
		// other new 
		if( m_vectCatalogPanel[i].pCatRemoteUpdateLocal ) m_vectCatalogPanel[i].pCatRemoteUpdateLocal->SetValue( (bool) bCatRemoteUpdateLocal );
		if( m_vectCatalogPanel[i].pCatDontUseType ) m_vectCatalogPanel[i].pCatDontUseType->SetValue( (bool) bCatDontUseType );

		// set filter date start
		if( m_vectCatalogPanel[i].pCatRecUseDateStart )
		{
			m_vectCatalogPanel[i].pCatRecUseDateStart->SetValue( (bool) bCatRecUseDateStart );
			SetUseFilterDateRange( m_vectCatalogPanel[i].cat_type, 0, bCatRecUseDateStart );
		}
		if( m_vectCatalogPanel[i].pCatRecDateStart ) m_vectCatalogPanel[i].pCatRecDateStart->SetValue( wxDateTime( nCatRecDateStart ) );

		// set filter date end
		if( m_vectCatalogPanel[i].pCatRecUseDateEnd ) 
		{
			m_vectCatalogPanel[i].pCatRecUseDateEnd->SetValue( (bool) bCatRecUseDateEnd );
			SetUseFilterDateRange( m_vectCatalogPanel[i].cat_type, 1, bCatRecUseDateEnd );
		}
		if( m_vectCatalogPanel[i].pCatRecDateEnd ) m_vectCatalogPanel[i].pCatRecDateEnd->SetValue( wxDateTime( nCatRecDateEnd ) );

		///////////////////////////////////////
		// CUSTOM SETS
		if( m_vectCatalogPanel[i].cat_type == CATALOG_TYPE_STARS )
		{
			// get max mag limit
			strTmp.Printf( wxT("%.2f"), pMainConfig->m_nStarsCatMaxMagLimit );
			m_pStarsCatMaxMagLimit->SetValue( strTmp );

		} else if( m_vectCatalogPanel[i].cat_type == CATALOG_TYPE_AES )
		{
			m_vectCatalogPanel[i].pCatObjCat->Freeze();
			// satellite type
			for( j=0; j<m_pSky->m_pCatalogAes->m_nSatType; j++ )
			{
				m_vectCatalogPanel[i].pCatObjCat->Append( m_pSky->m_pCatalogAes->m_vectSatType[j].desc );
			}
			if( m_vectCatalogPanel[i].pCatObjCat ) m_vectCatalogPanel[i].pCatObjCat->SetSelection( nCatObjCat );
			m_vectCatalogPanel[i].pCatObjCat->Thaw();
			
			m_vectCatalogPanel[i].pCatObjType->Freeze();
			// object type
			for( j=0; j<m_pSky->m_pCatalogAes->m_nObjType; j++ )
			{
				m_vectCatalogPanel[i].pCatObjType->Append( m_pSky->m_pCatalogAes->m_vectObjType[j].desc );
			}
			if( m_vectCatalogPanel[i].pCatObjType ) m_vectCatalogPanel[i].pCatObjType->SetSelection( nCatObjType );
			m_vectCatalogPanel[i].pCatObjType->Thaw();

		} else if( m_vectCatalogPanel[i].cat_type == CATALOG_TYPE_DSO )
		{
			// get if to use messier catalog
			m_pUseMessierCat->SetValue( (bool) pMainConfig->m_bUseMessier );
			// get if to use ngc catalog
			m_pUseNgcCat->SetValue( (bool) pMainConfig->m_bUseNgc );
			// get if to use IC catalog
			m_pUseIcCat->SetValue( (bool) pMainConfig->m_bUseIc );
		}
	}

	GetSizer()->Layout();
	Layout();
	Refresh( FALSE );
	Update( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	GetPanelIdByCatalogType
// Class:	CCatalogConfig
// Purpose:	get panel id by cat type
// Input:	cat type
// Output:	panel id
////////////////////////////////////////////////////////////////////
int CCatalogConfig::GetPanelIdByCatalogType( int nCatType )
{
	int i=0;
	for( i=0; i<m_nCatalogPanel; i++ )
	{
		if( m_vectCatalogPanel[i].cat_type == nCatType ) return( i );
	}

	return( -1 );
}

////////////////////////////////////////////////////////////////////
// Method:	GetPanelByCatalogType
// Class:	CCatalogConfig
// Purpose:	get panel id by cat type
// Input:	cat type
// Output:	pointer to panel structure
////////////////////////////////////////////////////////////////////
DefPanelCatalogType* CCatalogConfig::GetPanelByCatalogType( int nCatType )
{
	int i=0;
	for( i=0; i<m_nCatalogPanel; i++ )
	{
		if( m_vectCatalogPanel[i].cat_type == nCatType ) return( &(m_vectCatalogPanel[i]) );
	}

	return( NULL );
}

////////////////////////////////////////////////////////////////////
// Method:	GetCatIndexFromSelection
// Class:	CCatalogConfig
// Purpose:	get select catalog id from map
// Input:	nothing
// Output:	dso cat id
////////////////////////////////////////////////////////////////////
int CCatalogConfig::GetCatIndexFromSelection( int nCatType )
{
	int nPanelId = GetPanelIdByCatalogType( nCatType );
	if( nPanelId < 0 || nPanelId >= m_nCatalogPanel ) return( -1 );

	int nSelected = m_vectCatalogPanel[nPanelId].pCatalogToUse->GetSelection();
	// now get catalog number for selectd
	int nId = m_vectCatalogPanel[nPanelId].vectMapCatalogId[nSelected];

	int nFound = -1;
	// get catalog number which matches - and set found index
	for( int i=0; i<m_pSky->m_nCatalogDef; i++ )
		if( m_pSky->m_vectCatalogDef[i].m_nObjectType == nCatType && m_pSky->m_vectCatalogDef[i].m_nId == nId ) nFound = i;

	return( nFound );
}

////////////////////////////////////////////////////////////////////
int CCatalogConfig::GetCatIdFromSelection( int nCatType )
{
	int nPanelId = GetPanelIdByCatalogType( nCatType );
	if( nPanelId < 0 || nPanelId >= m_nCatalogPanel ) return( -1 );

	int nSelected = m_vectCatalogPanel[nPanelId].pCatalogToUse->GetSelection();
	return( m_vectCatalogPanel[nPanelId].vectMapCatalogId[nSelected] );
}

////////////////////////////////////////////////////////////////////
int CCatalogConfig::GetSelectedCatId( int nPanelId, int nCatId )
{
	int nFound = -1;
	for( int i=0; i<m_vectCatalogPanel[nPanelId].nMapCatalogId; i++ )
		if( m_vectCatalogPanel[nPanelId].vectMapCatalogId[i] == nCatId ) nFound = i;

	return( nFound );
}

////////////////////////////////////////////////////////////////////
// Method:	OnSetCatalog
// Class:	CCatalogConfig
// Purpose:	call to select a catalog 
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCatalogConfig::OnSetCatalog( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();

	if( nId == wxID_STARS_CATALOG_TO_USE )
		CheckCatalogByType( CATALOG_TYPE_STARS );
	else if( nId == wxID_DSO_CATALOG_TO_USE )
		CheckCatalogByType( CATALOG_TYPE_DSO );
	else if( nId == wxID_RADIO_CATALOG_TO_USE )
		CheckCatalogByType( CATALOG_TYPE_RADIO );
	else if( nId == wxID_XGAMMA_CATALOG_TO_USE )
		CheckCatalogByType( CATALOG_TYPE_XGAMMA );
	else if( nId == wxID_SUPERNOVAS_CATALOG_TO_USE )
		CheckCatalogByType( CATALOG_TYPE_SUPERNOVAS );
	else if( nId == wxID_BHOLES_CATALOG_TO_USE )
		CheckCatalogByType( CATALOG_TYPE_BLACKHOLES );
	else if( nId == wxID_MSTARS_CATALOG_TO_USE )
		CheckCatalogByType( CATALOG_TYPE_MSTARS );
	else if( nId == wxID_EXP_CATALOG_TO_USE )
		CheckCatalogByType( CATALOG_TYPE_EXOPLANETS );
	else if( nId == wxID_AST_CATALOG_TO_USE )
		CheckCatalogByType( CATALOG_TYPE_ASTEROIDS );
	else if( nId == wxID_COM_CATALOG_TO_USE )
		CheckCatalogByType( CATALOG_TYPE_COMETS );
	else if( nId == wxID_AES_CATALOG_TO_USE )
		CheckCatalogByType( CATALOG_TYPE_AES );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnCatalogDownload
// Class:	CCatalogConfig
// Purpose:	popup window catalog download
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCatalogConfig::OnCatalogDownload( wxCommandEvent& pEvent )
{
	int nButtonId = pEvent.GetId();
	int nCatId = -1;
	int nCatType = -1;

	// check by originating button type
	if( nButtonId == wxID_BUTTON_DOWNLOAD_STARS )
		nCatType = CATALOG_TYPE_STARS;
	else if( nButtonId == wxID_BUTTON_DOWNLOAD_DSO )
		nCatType = CATALOG_TYPE_DSO;
	else if( nButtonId == wxID_BUTTON_DOWNLOAD_RADIO )
		nCatType = CATALOG_TYPE_RADIO;
	else if( nButtonId == wxID_BUTTON_DOWNLOAD_XGAMMA )
		nCatType = CATALOG_TYPE_XGAMMA;
	else if( nButtonId == wxID_BUTTON_DOWNLOAD_SUPERNOVAS )
		nCatType = CATALOG_TYPE_SUPERNOVAS;
	else if( nButtonId == wxID_BUTTON_DOWNLOAD_BHOLES )
		nCatType = CATALOG_TYPE_BLACKHOLES;
	else if( nButtonId == wxID_BUTTON_DOWNLOAD_MSTARS )
		nCatType = CATALOG_TYPE_MSTARS;
	else if( nButtonId == wxID_BUTTON_DOWNLOAD_EXP )
		nCatType = CATALOG_TYPE_EXOPLANETS;
	else if( nButtonId == wxID_BUTTON_DOWNLOAD_AST )
		nCatType = CATALOG_TYPE_ASTEROIDS;
	else if( nButtonId == wxID_BUTTON_DOWNLOAD_COM )
		nCatType = CATALOG_TYPE_COMETS;
	else if( nButtonId == wxID_BUTTON_DOWNLOAD_AES )
		nCatType = CATALOG_TYPE_AES;
	else
		return;

	// set cat id
	nCatId = GetCatIndexFromSelection( nCatType );

	// create the download dialog
	CCatalogDownloadDlg rDialog( this, &(m_pSky->m_vectCatalogDef[nCatId]), wxT("Download catalog ...") );
	
	// show dialog and if ok procced to download
	if( rDialog.ShowModal() == wxID_OK )
	{
		// set catalog type 
//		m_pSky->m_nCatalogTypeForDownloading = nCatType;
		// set what catalog to download - set the id not the index
//		m_pSky->m_nCatalogIdForDownloading = m_pSky->m_vectCatalogDef[nCatId].m_nId;
		// set what catalog to download
//		m_pSky->m_nCatalogSourceForDownloading = rDialog.GetSourceId( );

		/////////
		// set my worker thread
//		DefCmdAction cmd;
//		cmd.id = THREAD_ACTION_CATALOG_DOWNLOAD;
//		m_pUnimapWorker->SetAction( cmd );

		///////////
		// SET THREAD TO DOWNLOAD CATALOG
		DefCmdAction act;
		act.id = THREAD_ACTION_CATALOG_DOWNLOAD;
		// set catalog type 
		act.vectInt[0] = nCatType;
		// set what catalog to download - set the id not the index
		act.vectInt[1] = m_pSky->m_vectCatalogDef[nCatId].m_nId;
		// set what catalog to download
		act.vectInt[2] = rDialog.GetSourceId( );
		/////////////////
		// show wait dialog
		CWaitDialog* pWaitDialog = new CWaitDialog( this, wxT("Download Catalog Data"), 
							wxT("Fetching data from remote location ..."), 1, m_pUnimapWorker, &act );
		pWaitDialog->ShowModal();
		// delete/reset wait dialog
		delete( pWaitDialog );
		pWaitDialog = NULL;

		// activate view button :: todo: here i should check if either this was downloaded or not
		DefPanelCatalogType* pPanelData = GetPanelByCatalogType( nCatType );
		pPanelData->pCatalogView->Enable();
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnCatalogView
// Class:	CCatalogConfig
// Purpose:	popup a dialog window to view/search current catalog 
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCatalogConfig::OnCatalogView( wxCommandEvent& pEvent )
{
	int nButtonId = pEvent.GetId();
	int nCatId = -1;
	int nCatType = -1;
	wxString strTitle;

	// check by originating button type
	if( nButtonId == wxID_BUTTON_CATVIEW_STARS )
		nCatType = CATALOG_TYPE_STARS;
	else if( nButtonId == wxID_BUTTON_CATVIEW_DSO )
		nCatType = CATALOG_TYPE_DSO;
	else if( nButtonId == wxID_BUTTON_CATVIEW_RADIO )
		nCatType = CATALOG_TYPE_RADIO;
	else if( nButtonId == wxID_BUTTON_CATVIEW_XGAMMA )
		nCatType = CATALOG_TYPE_XGAMMA;
	else if( nButtonId == wxID_BUTTON_CATVIEW_SUPERNOVAS )
		nCatType = CATALOG_TYPE_SUPERNOVAS;
	else if( nButtonId == wxID_BUTTON_CATVIEW_BHOLES )
		nCatType = CATALOG_TYPE_BLACKHOLES;
	else if( nButtonId == wxID_BUTTON_CATVIEW_MSTARS )
		nCatType = CATALOG_TYPE_MSTARS;
	else if( nButtonId == wxID_BUTTON_CATVIEW_EXP )
		nCatType = CATALOG_TYPE_EXOPLANETS;
	else if( nButtonId == wxID_BUTTON_CATVIEW_AST )
		nCatType = CATALOG_TYPE_ASTEROIDS;
	else if( nButtonId == wxID_BUTTON_CATVIEW_COM )
		nCatType = CATALOG_TYPE_COMETS;
	else if( nButtonId == wxID_BUTTON_CATVIEW_AES )
		nCatType = CATALOG_TYPE_AES;
	else 
		return;

	// set cat id
	nCatId = GetCatIndexFromSelection( nCatType );

	// create the download dialog
	strTitle.Printf( wxT("View catalog: %s"), m_pSky->m_vectCatalogDef[nCatId].m_strName );
	CCatalogViewDlg rDialog( this, m_pSky, strTitle );
	rDialog.SetCatalogTable( nCatId, m_pSky->m_vectCatalogDef[nCatId].m_nId );
	
	// show dialog and if ok procced to download
	if( rDialog.ShowModal() == wxID_OK )
	{
		// do something ???
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetUseRemoteCat
// Class:	CCatalogConfig
// Purpose:	when set to use remote enable/disable query source
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCatalogConfig::SetUseRemoteCat( int nCatType, int bRemote )
{
	int nPanelId = GetPanelIdByCatalogType( nCatType );
	if( nPanelId < 0 || nPanelId >= m_nCatalogPanel ) return;

	if( bRemote )
	{
		m_vectCatalogPanel[nPanelId].pCatUseRemote->SetValue( 1 );
		m_vectCatalogPanel[nPanelId].pCatRemoteLocation->Enable( );
		if( m_vectCatalogPanel[nPanelId].pCatRemoteUpdateLocal ) m_vectCatalogPanel[nPanelId].pCatRemoteUpdateLocal->Enable();

	} else
	{
		m_vectCatalogPanel[nPanelId].pCatUseRemote->SetValue( 0 );
		m_vectCatalogPanel[nPanelId].pCatRemoteLocation->Disable( );
		if( m_vectCatalogPanel[nPanelId].pCatRemoteUpdateLocal ) m_vectCatalogPanel[nPanelId].pCatRemoteUpdateLocal->Disable();
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnUseRemote
// Class:	CCatalogConfig
// Purpose:	set if to use or not remote and enable/disable location
// Input:	pointer to event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCatalogConfig::OnUseRemote( wxCommandEvent& pEvent )
{
	int nCtrlId = pEvent.GetId();
	int nCtrlVal = pEvent.IsChecked();

	if( nCtrlId == wxID_STARS_CAT_USE_REMOTE )
		SetUseRemoteCat( CATALOG_TYPE_STARS, nCtrlVal );
	else if( nCtrlId == wxID_DSO_CAT_USE_REMOTE )
		SetUseRemoteCat( CATALOG_TYPE_DSO, nCtrlVal );
	else if( nCtrlId == wxID_RADIO_CAT_USE_REMOTE )
		SetUseRemoteCat( CATALOG_TYPE_RADIO, nCtrlVal );
	else if( nCtrlId == wxID_XGAMMA_CAT_USE_REMOTE )
		SetUseRemoteCat( CATALOG_TYPE_XGAMMA, nCtrlVal );
	else if( nCtrlId == wxID_SUPERNOVAS_CAT_USE_REMOTE )
		SetUseRemoteCat( CATALOG_TYPE_SUPERNOVAS, nCtrlVal );
	else if( nCtrlId == wxID_BHOLES_CAT_USE_REMOTE )
		SetUseRemoteCat( CATALOG_TYPE_BLACKHOLES, nCtrlVal );
	else if( nCtrlId == wxID_MSTARS_CAT_USE_REMOTE )
		SetUseRemoteCat( CATALOG_TYPE_MSTARS, nCtrlVal );
	else if( nCtrlId == wxID_EXP_CAT_USE_REMOTE )
		SetUseRemoteCat( CATALOG_TYPE_EXOPLANETS, nCtrlVal );
	else if( nCtrlId == wxID_AST_CAT_USE_REMOTE )
		SetUseRemoteCat( CATALOG_TYPE_ASTEROIDS, nCtrlVal );
	else if( nCtrlId == wxID_COM_CAT_USE_REMOTE )
		SetUseRemoteCat( CATALOG_TYPE_COMETS, nCtrlVal );
	else if( nCtrlId == wxID_AES_CAT_USE_REMOTE )
		SetUseRemoteCat( CATALOG_TYPE_AES, nCtrlVal );

	return;
}

////////////////////////////////////////////////////////////////////
void CCatalogConfig::SetUseFilterDateRange( int nCatType, int nStartEnd, int bUse )
{
	int nPanelId = GetPanelIdByCatalogType( nCatType );
	if( nPanelId < 0 || nPanelId >= m_nCatalogPanel ) return;

	wxCheckBox* pCatRecUseDate = m_vectCatalogPanel[nPanelId].pCatRecUseDateStart;
	wxDatePickerCtrl* pCatRecDate = m_vectCatalogPanel[nPanelId].pCatRecDateStart;
	// get right pointer
	if( nStartEnd == 1 )
	{
		pCatRecUseDate = m_vectCatalogPanel[nPanelId].pCatRecUseDateEnd;
		pCatRecDate = m_vectCatalogPanel[nPanelId].pCatRecDateEnd;
	}
		
	// set if enabled or not
	if( bUse )
	{
		pCatRecUseDate->SetValue( 1 );
		pCatRecDate->Enable( );

	} else
	{
		pCatRecUseDate->SetValue( 0 );
		pCatRecDate->Disable( );
	}

	return;
}

// Method:	OnUseDateRange
////////////////////////////////////////////////////////////////////
void CCatalogConfig::OnUseDateRange( wxCommandEvent& pEvent )
{
	int nCtrlId = pEvent.GetId();
	int nCtrlVal = pEvent.IsChecked();

	if( nCtrlId == wxID_AES_CAT_USE_DATE_RANGE_START )
		SetUseFilterDateRange( CATALOG_TYPE_AES, 0, nCtrlVal );
	else if( nCtrlId == wxID_AES_CAT_USE_DATE_RANGE_END )
		SetUseFilterDateRange( CATALOG_TYPE_AES, 1, nCtrlVal );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	CheckCatalogByType
// Class:	CCatalogConfig
// Purpose:	validate my catalog entries by type
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCatalogConfig::CheckCatalogByType( int nCatType )
{
	// first enable both options
	int i=0, j=0;
	wxString strTmp;
	int nCatId = -1;

	int nPanelId = GetPanelIdByCatalogType( nCatType );
	if( nPanelId < 0 || nPanelId >= m_nCatalogPanel ) return;

	// freeze elements for speed
	m_vectCatalogPanel[nPanelId].pDesc->Freeze();

	nCatId = GetCatIndexFromSelection( nCatType );

	// check if catalog id less then 0
	if( nCatId >= 0 )
	{
		// set header data
		// :: records
		strTmp.Printf( wxT("%d"), m_pSky->m_vectCatalogDef[nCatId].m_nRecordsNo );
		m_vectCatalogPanel[nPanelId].pRecords->SetLabel( strTmp );
		// :: date
		if( m_pSky->m_vectCatalogDef[nCatId].m_strUpdateDate.Length() <= 0 )
			m_vectCatalogPanel[nPanelId].pDate->SetLabel( m_pSky->m_vectCatalogDef[nCatId].m_strCreationDate );
		else
		{
			strTmp.Printf( wxT("%s-%s"), m_pSky->m_vectCatalogDef[nCatId].m_strCreationDate, m_pSky->m_vectCatalogDef[nCatId].m_strUpdateDate );
			m_vectCatalogPanel[nPanelId].pDate->SetLabel( strTmp );
		}
		// :: version
		m_vectCatalogPanel[nPanelId].pVersion->SetLabel( m_pSky->m_vectCatalogDef[nCatId].m_strVersion );
		// :: author
		m_vectCatalogPanel[nPanelId].pAuthor->SetLabel( m_pSky->m_vectCatalogDef[nCatId].m_strAuthor );
		// :: organization
		m_vectCatalogPanel[nPanelId].pOrganization->SetLabel( m_pSky->m_vectCatalogDef[nCatId].m_strOrganization );

		// case when is only remote possible
		if( m_pSky->m_vectCatalogDef[nCatId].m_nSourcesFull > 0 )
		{
			m_vectCatalogPanel[nPanelId].pCatalogDownload->Enable( );
			// check if file local not exist
			if( !wxFileExists( m_pSky->m_vectCatalogDef[nCatId].m_strFile ) )
			{
				// enable area and disable control
				m_vectCatalogPanel[nPanelId].pCatLoadRegion->SetValue( 1 );
				m_vectCatalogPanel[nPanelId].pCatLoadRegion->Disable( );
				// enable remote and disable control
				SetUseRemoteCat( nCatType, 1 );
				m_vectCatalogPanel[nPanelId].pCatUseRemote->Disable( );

			} else
			{
				// also set default to load local
				if( !m_vectCatalogPanel[nPanelId].pCatUseRemote->GetValue() || m_vectCatalogPanel[nPanelId].bSelectionChanged ) 
					SetUseRemoteCat( nCatType, 0 );
				else 
					SetUseRemoteCat( nCatType, 1 );
				m_vectCatalogPanel[nPanelId].pCatUseRemote->Enable( );
				// :: here we check by size - if less then 500k set default to load all
				m_vectCatalogPanel[nPanelId].pCatLoadRegion->Enable( );
				if( m_pSky->m_vectCatalogDef[nCatId].m_nRecordsNo < 500000 )
				{
					m_vectCatalogPanel[nPanelId].pCatLoadRegion->SetValue( 0 );

				} else
				{
					m_vectCatalogPanel[nPanelId].pCatLoadRegion->SetValue( 1 );
				}
			}

		// case when no remote source .. maybe only local
		} else
		{
			m_vectCatalogPanel[nPanelId].pCatalogDownload->Disable( );
			// enable area and disable control
			m_vectCatalogPanel[nPanelId].pCatLoadRegion->SetValue( 1 );
			m_vectCatalogPanel[nPanelId].pCatLoadRegion->Disable( );
			// enable remote and disable control
			SetUseRemoteCat( nCatType, 1 );
			m_vectCatalogPanel[nPanelId].pCatUseRemote->Disable( );
		}

		////////////////////
		// :: VIEW BUTTON :: check if file exists for view
		if( wxFileExists( m_pSky->m_vectCatalogDef[nCatId].m_strFile ) )
		{
			////////////////////////////
			// ENABLE VIEW BY TYPE
			int n_id = m_pSky->m_vectCatalogDef[nCatId].m_nId;
			if( n_id == CATALOG_ID_EXOPLANET_EU ||
				// stars
				n_id == CATALOG_ID_SAO || n_id == CATALOG_ID_HIPPARCOS ||
				n_id == CATALOG_ID_TYCHO_2 || n_id == CATALOG_ID_2MASS || 
				n_id == CATALOG_ID_GSC || n_id == CATALOG_ID_USNO_B1 || 
				// dso
				n_id == CATALOG_ID_PGC || n_id == CATALOG_ID_HYPERLEDA ||
				n_id == CATALOG_ID_UGC || n_id == CATALOG_ID_MCG ||
				n_id == CATALOG_ID_MGC || n_id == CATALOG_ID_PGC_ || 
				// radio
				n_id == CATALOG_ID_SPECFIND || n_id == CATALOG_ID_VLSS ||
				n_id == CATALOG_ID_NVSS || n_id == CATALOG_ID_MSL ||
				// gamma/x-ray
				n_id == CATALOG_ID_ROSAT || n_id == CATALOG_ID_BATSE ||
				n_id == CATALOG_ID_INTEGRAL || n_id == CATALOG_ID_BEPPOSAX_GRBM ||
				n_id == CATALOG_ID_2XMMi || n_id == CATALOG_ID_HEAO_A1 ||
				// supernovas
				n_id == CATALOG_ID_ASC || n_id == CATALOG_ID_SSC ||
				// black holes
				n_id == CATALOG_ID_SDSS_DR5 || n_id == CATALOG_ID_RXE_AGN ||
				// multiple/double stars
				n_id == CATALOG_ID_WDS || n_id == CATALOG_ID_CCDM ||
				// asteroids
				n_id == CATALOG_ID_OMP ||
				// comets
				n_id == CATALOG_ID_COCOM ||
				// AES
				n_id == CATALOG_ID_CELESTRAK
				)
			{
				m_vectCatalogPanel[nPanelId].pCatalogView->Enable( );
			} else
				m_vectCatalogPanel[nPanelId].pCatalogView->Disable( );
		} else
			m_vectCatalogPanel[nPanelId].pCatalogView->Disable( );

		///////////
		// data section
		// sources for query
		m_vectCatalogPanel[nPanelId].nMapSourceId = 0;
		m_vectCatalogPanel[nPanelId].pCatRemoteLocation->Clear();
		m_vectCatalogPanel[nPanelId].pCatRemoteLocation->Freeze();
		for( i=0; i<m_pSky->m_vectCatalogDef[nCatId].m_nSourcesQuery; i++ )
		{
			int nSourceId = m_pSky->m_vectCatalogDef[nCatId].m_vectSourcesQuery[i];
			if( nSourceId == CATALOG_QUERY_SOURCE_VIZIER )
				m_vectCatalogPanel[nPanelId].pCatRemoteLocation->Append( wxT("Vizier") );
			else if( nSourceId == CATALOG_QUERY_SOURCE_NOFS_NAVY_MIL )
				m_vectCatalogPanel[nPanelId].pCatRemoteLocation->Append( wxT("NOSF-Navy.Mil") );
			else if( nSourceId == CATALOG_QUERY_SOURCE_LEDA )
				m_vectCatalogPanel[nPanelId].pCatRemoteLocation->Append( wxT("Leda") );
			else if( nSourceId == CATALOG_QUERY_SOURCE_EXOPLANET_EU )
				m_vectCatalogPanel[nPanelId].pCatRemoteLocation->Append( wxT("Exoplanet.EU") );
			else if( nSourceId == CATALOG_QUERY_SOURCE_CELESTRAK )
				m_vectCatalogPanel[nPanelId].pCatRemoteLocation->Append( wxT("Celestrak.Com") );

			// set map source to select
			m_vectCatalogPanel[nPanelId].vectMapSourceId[i] = nSourceId;
			m_vectCatalogPanel[nPanelId].nMapSourceId++;
		}
		m_vectCatalogPanel[nPanelId].pCatRemoteLocation->Thaw();
		m_vectCatalogPanel[nPanelId].pCatRemoteLocation->SetSelection( 0 );
			
		/////////
		// footprint image OR filter panel
		if( nCatType == CATALOG_TYPE_AES )
		{
			m_vectCatalogPanel[nPanelId].pCatObjCat->Enable();
			m_vectCatalogPanel[nPanelId].pCatObjType->Enable();
			m_vectCatalogPanel[nPanelId].pCatDontUseType->Enable();
			m_vectCatalogPanel[nPanelId].pCatRecUseDateStart->Enable();
			m_vectCatalogPanel[nPanelId].pCatRecDateStart->Enable();
			m_vectCatalogPanel[nPanelId].pCatRecUseDateEnd->Enable();
			m_vectCatalogPanel[nPanelId].pCatRecDateEnd->Enable();

			// satelite types
			//m_vectCatalogPanel[nPanelId].pCatObjCat->SetSelection( 0 );
			// object type
			//m_vectCatalogPanel[nPanelId].pCatObjType->SetSelection( 0 );

		} else
		{
			m_vectCatalogPanel[nPanelId].pFootprint->Enable();
			m_bmpFootprint.LoadFile( m_pSky->m_vectCatalogDef[nCatId].m_strFootPrintImg, wxBITMAP_TYPE_GIF );
			m_vectCatalogPanel[nPanelId].pFootprint->SetBitmap( m_bmpFootprint );
		}
		////////////
		// :: set description
		m_vectCatalogPanel[nPanelId].pDesc->Enable();
		m_vectCatalogPanel[nPanelId].pDesc->SetValue( m_pSky->m_vectCatalogDef[nCatId].m_strDescription );

		//////////////////////////////
		// CUSTOM CATALOGS -- todo :: remove custom here
		if( nCatType == CATALOG_TYPE_DSO )
		{
			// :: check messier
			if( !wxFileExists( DEFAULT_MESSIER_BINARY_FILENAME ) )
			{
				m_pUseMessierCat->SetValue( 0 );
				m_pUseMessierCat->Disable( );
			}
			// also check the default ngc/ic
			if( !wxFileExists( DEFAULT_NGC_BINARY_FILENAME ) )
			{
				m_pUseNgcCat->SetValue( 0 );
				m_pUseNgcCat->Disable( );
			}
			if( !wxFileExists( DEFAULT_IC_BINARY_FILENAME ) )
			{
				m_pUseIcCat->SetValue( 0 );
				m_pUseIcCat->Disable( );
			}
		}

	} else
	{
		// fill with empty data
		// set header data
		m_vectCatalogPanel[nPanelId].pRecords->SetLabel( wxT("0") );
		m_vectCatalogPanel[nPanelId].pDate->SetLabel( wxT("00-00-0000") );
		m_vectCatalogPanel[nPanelId].pVersion->SetLabel( wxT("0.0") );
		m_vectCatalogPanel[nPanelId].pAuthor->SetLabel( wxT("none") );
		m_vectCatalogPanel[nPanelId].pOrganization->SetLabel( wxT("none") );
		// data section
		m_vectCatalogPanel[nPanelId].pCatalogDownload->Disable( );
		m_vectCatalogPanel[nPanelId].pCatalogView->Disable( );
		m_vectCatalogPanel[nPanelId].pCatLoadRegion->SetValue( 0 );
		m_vectCatalogPanel[nPanelId].pCatLoadRegion->Disable( );
		m_vectCatalogPanel[nPanelId].pCatUseRemote->SetValue( 0 );
		m_vectCatalogPanel[nPanelId].pCatUseRemote->Disable( );
		// sources for query
		m_vectCatalogPanel[nPanelId].pCatRemoteLocation->Clear();
		m_vectCatalogPanel[nPanelId].pCatRemoteLocation->Disable();			
		/////////
		// footprint image
		if( nCatType == CATALOG_TYPE_AES )
		{
			m_vectCatalogPanel[nPanelId].pCatObjCat->Disable();
			m_vectCatalogPanel[nPanelId].pCatObjType->Disable();
			m_vectCatalogPanel[nPanelId].pCatDontUseType->Disable();
			m_vectCatalogPanel[nPanelId].pCatRecUseDateStart->Disable();
			m_vectCatalogPanel[nPanelId].pCatRecDateStart->Disable();
			m_vectCatalogPanel[nPanelId].pCatRecUseDateEnd->Disable();
			m_vectCatalogPanel[nPanelId].pCatRecDateEnd->Disable();

		} else
		{
			//m_bmpFootprint.LoadFile( unMakeAppPath(wxT("data/catalog/none_footprint.gif")), wxBITMAP_TYPE_GIF );
			m_vectCatalogPanel[nPanelId].pFootprint->SetBitmap( m_bmpNoneFootprint );
			m_vectCatalogPanel[nPanelId].pFootprint->Disable();
		}
		////////////
		// :: set description
		m_vectCatalogPanel[nPanelId].pDesc->SetValue( wxT("") );
		m_vectCatalogPanel[nPanelId].pDesc->Disable();

	}

	// Enable back elements for speed
	m_vectCatalogPanel[nPanelId].pDesc->Thaw();

	// update layout
	m_vectCatalogPanel[nPanelId].sizerHeaderA->Layout();
	m_vectCatalogPanel[nPanelId].sizerHeaderB->Layout();
/*	GetSizer()->Layout();
	Layout();
	Refresh( FALSE );
	Update( );
*/

	// mark selection changed
	m_vectCatalogPanel[nPanelId].bSelectionChanged = 1;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	GetQuerySourceId
// Class:	CCatalogConfig
// Purpose:	return from map between select box and real source id
// Input:	catalog type
// Output:	catalog query source id
////////////////////////////////////////////////////////////////////
int CCatalogConfig::GetQuerySourceId( int nCatType )
{

	int nPanelId = GetPanelIdByCatalogType( nCatType );
	if( nPanelId < 0 || nPanelId >= m_nCatalogPanel ) return( -1 );

	int nSelectId = m_vectCatalogPanel[nPanelId].pCatRemoteLocation->GetSelection( );
	return( m_vectCatalogPanel[nPanelId].vectMapSourceId[nSelectId] );

//	return( -1 );
}

////////////////////////////////////////////////////////////////////
// Method:	GetQuerySourceSelectionId
// Class:	CCatalogConfig
// Purpose:	return from map between select box and real selection id
// Input:	catalog type, source id
// Output:	source selection id
////////////////////////////////////////////////////////////////////
int CCatalogConfig::GetQuerySourceSelectionId( int nCatType, int nQuerySourceId )
{
//	wxString strTmp;
//	strTmp.Printf( wxT("%d, %d\n"), nCatType, nQuerySourceId );
//	wxMessageBox( strTmp, _T("Debug") );

	int i = 0;

	int nPanelId = GetPanelIdByCatalogType( nCatType );
	if( nPanelId < 0 || nPanelId >= m_nCatalogPanel ) return( -1 );

//	strTmp.Printf( wxT("%d, %d\n"), nPanelId, m_vectCatalogPanel[nPanelId].nMapSourceId );
//	wxMessageBox( strTmp, _T("Debug2") );

	for( i=0; i<m_vectCatalogPanel[nPanelId].nMapSourceId; i++ )
		if( m_vectCatalogPanel[nPanelId].vectMapSourceId[i] == nQuerySourceId ) return( i );


	return( -1 );
}

////////////////////////////////////////////////////////////////////
// Method:	OnValidate
// Class:	CCatalogConfig
// Purpose:	validate my data on ok
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CCatalogConfig::OnValidate( wxCommandEvent& pEvent )
{
/*
	wxString strMsg = "";
	wxString strTmp = "";
	wxRegEx reName( "^[a-zA-Z0-9_\\ \\+\\-\\.\\B[:punct:]]+$" );
	wxRegEx reStar( "[*,]+" );
	int bErrorFound = 0;
	long nTmp = 0;
	int bFoundNumber = 0;

	// common checks
	// check hardware name
	strTmp = m_pDeviceNameEntry->GetLineText(0);
	if( !reName.Matches( strTmp ) || reStar.Matches( strTmp ) )
	{
		if( strTmp.IsEmpty() )
			strMsg += "ERROR :: please enter hardware name.\n";
		else
			strMsg += "ERROR :: illegal characters found in software name.\n";
		bErrorFound = 1;
	}
	// check on delay
	strTmp = m_pOnDelayEntry->GetLineText(0);
	bFoundNumber = strTmp.ToLong( &nTmp );
	if( !bFoundNumber || nTmp < 0 )
	{
		if( strTmp.IsEmpty() )
			strMsg += "ERROR :: please enter ON delay.\n";
		else
			strMsg += "ERROR :: ON delay should be a positive integer.\n";
		bErrorFound = 1;
	}
	// check off delay
	strTmp = m_pOffDelayEntry->GetLineText(0);
	bFoundNumber = strTmp.ToLong( &nTmp );
	if( !bFoundNumber || nTmp < 0 )
	{
		if( strTmp.IsEmpty() )
			strMsg += "ERROR :: please enter OFF delay.\n";
		else
			strMsg += "ERROR :: OFF delay should be a positive integer.\n";
		bErrorFound = 1;
	}

	//////////////////////////////////////////////
	// now, in the end, check to see if there was an error
	if( bErrorFound == 1 )
	{
		wxMessageBox( strMsg, _T("Data Error!"), wxOK|wxICON_ERROR );
	} else
		event.Skip();
*/
	pEvent.Skip();
	return;
}
