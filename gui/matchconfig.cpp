////////////////////////////////////////////////////////////////////
// Package:		Match configuration implementation
// File:		matchconfig.cpp
// Purpose:		GUI dialog window to config matching process
//
// Created by:	Larry Lart on 11/02/2007
// Updated by:  Larry Lart on 05/02/2010
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
#include "../sky/sky.h"
#include "../sky/sky_finder.h"
#include "../config/config.h"
#include "../config/mainconfig.h"

// include main header
#include "matchconfig.h"

//		***** CLASS CMatchConfig *****
////////////////////////////////////////////////////////////////////
// CMatchConfig EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(CMatchConfig, wxDialog)
	EVT_CHECKBOX( wxID_MATCH_STOP_ON_FIRST_FOUND, CMatchConfig::OnCheckStopOnFirstFound )
//	EVT_CHOICE( wxID_LOGIC_TYPE_SELECT, CMatchConfig::OnSelectLogic )
	EVT_BUTTON( wxID_OK, CMatchConfig::OnValidate )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:	Constructor
////////////////////////////////////////////////////////////////////
CMatchConfig::CMatchConfig( wxWindow *parent, const wxString& title ) : wxDialog(parent, -1,
                          title,
                          wxDefaultPosition,
                          wxDefaultSize,
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	wxFont smallFont( 6, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT );

	wxString vectSearchMethod[2];
	wxString vectProjectionMethod[2];
	wxString vectStrTransType[3];

	// area search method
	vectSearchMethod[0] = wxT("Swipe");
	vectSearchMethod[1] = wxT("Spiral");
	// Projection methods
	vectProjectionMethod[0] = wxT("Gnomonic");
	vectProjectionMethod[1] = wxT("Stereographic");
	// matching transformation type
	vectStrTransType[0] = wxT("Linear");
	vectStrTransType[1] = wxT("Quadratic");
	vectStrTransType[2] = wxT("Cubic");

	wxBoxSizer *pTopSizer = new wxBoxSizer( wxVERTICAL );
//	wxGridSizer *sizerPanel = new wxGridSizer(2,2,10,10);

	// building configure as a note book
	pNotebook = new wxNotebook( this, -1 ); //, wxPoint( 0, 0 ),
									//wxSize( 450, 540 ) );

	// add Search config panel
	wxPanel* pPanelSearch = new wxPanel( pNotebook );
	// add Proc panel
	wxPanel* pPanelProc = new wxPanel( pNotebook );
	// add Objects panel
	wxPanel* pPanelObjects = new wxPanel( pNotebook );
	// add Match panel
	wxPanel* pPanelMatch = new wxPanel( pNotebook );
	// add Finder panel
	wxPanel* pPanelFinder = new wxPanel( pNotebook );
	// add Solar panel
	wxPanel* pPanelSolar = new wxPanel( pNotebook );

	/////////////////////////////
	// Search panel sizer - layout of elements
	wxFlexGridSizer* sizerSearchPanel = new wxFlexGridSizer( 8, 2, 10, 10 );
	// layout of group of elements in the panel
	wxGridSizer* sizerSearchPage = new wxGridSizer( 1, 1, 10, 10 );
	sizerSearchPage->Add( sizerSearchPanel, 0,
                  wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 10  );

	/////////////////////////////
	// Search panel sizer - layout of elements
	wxFlexGridSizer* sizerProcPanel = new wxFlexGridSizer( 8, 2, 10, 10 );
	// layout of group of elements in the panel
	wxGridSizer* sizerProcPage = new wxGridSizer( 1, 1, 10, 10 );
	sizerProcPage->Add( sizerProcPanel, 0,
                  wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 10  );

	/////////////////////////////
	// Objects panel sizer - layout of elements
	sizerObjectsPanel = new wxFlexGridSizer( 7, 2, 10, 10 );
	// layout of group of elements in the panel
	wxGridSizer* sizerObjectsPage = new wxGridSizer( 1, 1, 10, 10 );
	sizerObjectsPage->Add( sizerObjectsPanel, 0,
                  wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 10 );

	/////////////////////////////
	// Match panel sizer - layout of elements
	wxFlexGridSizer* sizerMatchPanel = new wxFlexGridSizer( 7, 2, 10, 10 );
	// layout of group of elements in the panel
	wxGridSizer* sizerMatchPage = new wxGridSizer( 1, 1, 10, 10 );
	sizerMatchPage->Add( sizerMatchPanel, 0,
                  wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 10 );

	/////////////////////////////
	// :: FINDER :: panel sizer - layout of elements
	wxFlexGridSizer* sizerFinderPanel = new wxFlexGridSizer( 7, 1, 10, 10 );
	// layout of group of elements in the panel
	wxGridSizer* sizerFinderPage = new wxGridSizer( 1, 1, 10, 10 );
	sizerFinderPage->Add( sizerFinderPanel, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 10 );

	/////////////////////////////
	// :: SOLAR :: panel sizer - layout of elements
	wxFlexGridSizer* sizerSolarPanel = new wxFlexGridSizer( 7, 1, 10, 10 );
	// layout of group of elements in the panel
	wxGridSizer* sizerSolarPage = new wxGridSizer( 1, 1, 10, 10 );
	sizerSolarPage->Add( sizerSolarPanel, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 10 );

	/////////////////
	//  Search panel elements
	m_pSearchMethod = new wxChoice( pPanelSearch, wxID_SKY_AREA_SEARCH_METHOD,
										wxDefaultPosition, wxSize(-1,-1), 2, 
										vectSearchMethod, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pSearchMethod->SetSelection( 0 );
	m_pSearchAreaIncrementOnRA = new wxTextCtrl( pPanelSearch, -1, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );
	m_pSearchAreaIncrementOnDEC = new wxTextCtrl( pPanelSearch, -1, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );
	m_pRaSearchOverlapDivision = new wxTextCtrl( pPanelSearch, -1, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );
	m_pDecSearchOverlapDivision = new wxTextCtrl( pPanelSearch, -1, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );
	m_pSearchZoomSteps = new wxTextCtrl( pPanelSearch, -1, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );
	m_pSearchZoomStart = new wxTextCtrl( pPanelSearch, -1, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );
	m_pDoReverseZoomSearch = new wxCheckBox( pPanelSearch, wxID_REVERSE_ZOOM_SEARCH, wxT("")  );

	/////////////////
	//  Proc panel elements
	m_pProjectionMethod = new wxChoice( pPanelProc, wxID_SKY_AREA_PROJECTION_METHOD,
										wxDefaultPosition, wxSize(-1,-1), 2, 
										vectProjectionMethod, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pProjectionMethod->SetSelection( 0 );
	m_pDoMatchFineTune = new wxCheckBox( pPanelProc, wxID_MATCH_DO_FINETUNE, wxT("")  );
	m_pDoMatchFineTuneWithProjection = new wxCheckBox( pPanelProc, wxID_MATCH_DO_FINETUNE_WITH_PROJECTION, wxT("")  );
	m_pMatchByMostStars = new wxCheckBox( pPanelProc, wxID_MATCH_BY_MOST_STARS, wxT("")  );
	m_pMatchByBestFit = new wxCheckBox( pPanelProc, wxID_MATCH_BY_BEST_FIT, wxT("")  );
	m_pDoMatchRefineBestArea = new wxCheckBox( pPanelProc, wxID_MATCH_REFINE_BEST_AREA, wxT("")  );
	m_pDoMatchStopOnFirstFound = new wxCheckBox( pPanelProc, wxID_MATCH_STOP_ON_FIRST_FOUND, wxT("")  );
	m_pMatchStopOnMinFound = new wxTextCtrl( pPanelProc, wxID_MATCH_STOP_MIN_STARS_NEEDED, wxT(""),
									wxDefaultPosition, wxSize(60,-1) );
	m_pMatchStopOnMinFound->Disable();
	m_pAssocMatchMaxDist = new wxTextCtrl( pPanelProc, wxID_MATCH_ASSOC_MAX_DISTANCE, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );

	/////////////////
	//  Objects panel elements
	m_pImgObjectExtractByObjectShape = new wxCheckBox( pPanelObjects, wxID_IMAGE_OBJECT_EXTRACTION_BY_OBJ_SHAPE, wxT("")  );
	m_pImgObjectExtractByDetectionType = new wxCheckBox( pPanelObjects, wxID_IMAGE_OBJECT_EXTRACTION_BY_DETECT_TYPE, wxT("")  );
	m_pImgNoObjMatch = new wxTextCtrl( pPanelObjects, -1, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );
	m_pCatNoObjMatch = new wxTextCtrl( pPanelObjects, -1, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );
	m_pSearchMatchMinCatStars = new wxTextCtrl( pPanelObjects, -1, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );
	m_pMatchFirstLevelMinStars = new wxTextCtrl( pPanelObjects, -1, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );
	m_pMatchFineTuneImgStarsFactor = new wxTextCtrl( pPanelObjects, -1, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );
	m_pMatchFineTuneCatStarsFactor = new wxTextCtrl( pPanelObjects, -1, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );


	/////////////////
	//  Match panel elements
	m_pTransTypeSelect = new wxChoice( pPanelMatch, wxID_TRANS_TYPE_SELECT,
										wxDefaultPosition, wxSize(-1,-1), 3, 
										vectStrTransType, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pTransTypeSelect->SetSelection( 0 );
	m_pMatchRecalc = new wxCheckBox( pPanelMatch, wxID_CHECK_RECALC_MATCH, wxT("")  );

	m_pMaxMatchDist = new wxTextCtrl( pPanelMatch, -1, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );
	m_pTriangFit = new wxTextCtrl( pPanelMatch, -1, wxT(""),
									wxDefaultPosition, wxSize(100,-1) );
	m_pTriangScale = new wxTextCtrl( pPanelMatch, -1, wxT(""),
									wxDefaultPosition, wxSize(100,-1) );
	m_pMaxIter = new wxTextCtrl( pPanelMatch, -1, wxT(""),
									wxDefaultPosition, wxSize(100,-1) );
	m_pHaltSigma = new wxTextCtrl( pPanelMatch, -1, wxT(""),
									wxDefaultPosition, wxSize(100,-1) );


	/////////////////
	//  :: FINDER :: panel elements
	m_pLocateCatStars = new wxCheckBox( pPanelFinder, -1, wxT("Locate Catalog Stars")  );
	m_pLocateDso = new wxCheckBox( pPanelFinder, -1, wxT("Locate Deep Space Objects")  );
	m_pLocateXGamma = new wxCheckBox( pPanelFinder, -1, wxT("Locate Gamma/X-Ray Sources") );
	m_pLocateRadio = new wxCheckBox( pPanelFinder, -1, wxT("Locate Radio Sourcess")  );
	m_pLocateSupernovas = new wxCheckBox( pPanelFinder, -1, wxT("Locate Supernovas")  );
	m_pLocateBlackholes = new wxCheckBox( pPanelFinder, -1, wxT("Locate Blackholes")  );
	m_pLocateMStars = new wxCheckBox( pPanelFinder, -1, wxT("Locate Multiple/Double Stars")  );
	m_pLocateExoplanets = new wxCheckBox( pPanelFinder, -1, wxT("Locate Extra Solar Planets")  );
	m_pLocateAsteroids = new wxCheckBox( pPanelFinder, -1, wxT("Locate Asteroids")  );
	m_pLocateComets = new wxCheckBox( pPanelFinder, -1, wxT("Locate Comets")  );
	m_pLocateAes = new wxCheckBox( pPanelFinder, -1, wxT("Locate Artificial Earth Satellites")  );
	m_pLocateSolar = new wxCheckBox( pPanelFinder, -1, wxT("Locate Solar Planets/Moons/Sun")  );
	m_pFecthDetailsOnFind = new wxCheckBox( pPanelFinder, -1, wxT(" Fecth Objects Details After Match") );

	/////////////////
	//  :: SOLAR :: panel elements
	m_pTimeFrameRange = new wxSpinCtrl( pPanelSolar, -1, wxT(""), wxDefaultPosition, wxSize(60,-1), wxSP_ARROW_KEYS |wxSP_WRAP, 0, 100 );
	m_pTimeFrameRange->SetRange( 0, 30000 );
	m_pTimeFrameDivision = new wxSpinCtrl( pPanelSolar, -1, wxT(""), wxDefaultPosition, wxSize(60,-1), wxSP_ARROW_KEYS |wxSP_WRAP, 0, 100 );
	m_pTimeFrameDivision->SetRange( 0, 1000 );
	m_pOnlyTleInRange = new wxCheckBox( pPanelSolar, -1, wxT("Use Only TLE in Range(+/-h):")  );
	m_pTleInRangeValue = new wxSpinCtrl( pPanelSolar, -1, wxT(""), wxDefaultPosition, wxSize(60,-1), wxSP_ARROW_KEYS |wxSP_WRAP, 0, 100 );
	m_pTleInRangeValue->SetRange( 0, 20000 );

	///////////////////////////////////
	// Populate Search panel
	// search method used
	sizerSearchPanel->Add( new wxStaticText(pPanelSearch, -1, wxT("Search Method:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerSearchPanel->Add( m_pSearchMethod, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// Search Area Increment On RA
	sizerSearchPanel->Add( new wxStaticText(pPanelSearch, -1, wxT("Area Increment on RA:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerSearchPanel->Add( m_pSearchAreaIncrementOnRA, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// Search Area Increment On DEC
	sizerSearchPanel->Add( new wxStaticText(pPanelSearch, -1, wxT("Area Increment on DEC:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerSearchPanel->Add( m_pSearchAreaIncrementOnDEC, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// Ra Search Overlap Divisions
	sizerSearchPanel->Add( new wxStaticText(pPanelSearch, -1, wxT("Area Overlap Divisions on RA:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerSearchPanel->Add( m_pRaSearchOverlapDivision, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// Dec Search Overlap Divisions
	sizerSearchPanel->Add( new wxStaticText(pPanelSearch, -1, wxT("Area Overlap Divisions on DEC:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerSearchPanel->Add( m_pDecSearchOverlapDivision, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// Search Zoom Steps
	sizerSearchPanel->Add( new wxStaticText(pPanelSearch, -1, wxT("Search Zoom Steps:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerSearchPanel->Add( m_pSearchZoomSteps, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// Search Zoom Start
	sizerSearchPanel->Add( new wxStaticText(pPanelSearch, -1, wxT("Search Zoom Start:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerSearchPanel->Add( m_pSearchZoomStart, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// Reverse Zoom Search
	sizerSearchPanel->Add( new wxStaticText(pPanelSearch, -1, wxT("Reverse Zoom Search:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerSearchPanel->Add( m_pDoReverseZoomSearch, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	///////////////////////////////////
	// Populate Proc panel
	// projection method used
	sizerProcPanel->Add( new wxStaticText(pPanelProc, -1, wxT("Projection Method:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerProcPanel->Add( m_pProjectionMethod, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// DoMatchFineTune
	sizerProcPanel->Add( new wxStaticText(pPanelProc, -1, wxT("Do Match Finetune:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerProcPanel->Add( m_pDoMatchFineTune, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// DoMatchFineTuneWithProjection
	sizerProcPanel->Add( new wxStaticText(pPanelProc, -1, wxT("Finetune With Projection:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerProcPanel->Add( m_pDoMatchFineTuneWithProjection, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// MatchByMostStars
	sizerProcPanel->Add( new wxStaticText(pPanelProc, -1, wxT("Match By Most Stars:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerProcPanel->Add( m_pMatchByMostStars, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// MatchByBestFit
	sizerProcPanel->Add( new wxStaticText(pPanelProc, -1, wxT("Match By Best Fit:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerProcPanel->Add( m_pMatchByBestFit, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// DoMatchRefineBestArea
	sizerProcPanel->Add( new wxStaticText(pPanelProc, -1, wxT("Match Refine Best Area:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerProcPanel->Add( m_pDoMatchRefineBestArea, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// m_pDoMatchStopOnFirstFound
	sizerProcPanel->Add( new wxStaticText(pPanelProc, -1, wxT("Match Stop On First Found:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	wxFlexGridSizer* sizerProcStopFound = new wxFlexGridSizer( 1, 2, 0, 5 );
	sizerProcStopFound->Add( m_pDoMatchStopOnFirstFound, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerProcStopFound->Add( m_pMatchStopOnMinFound, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerProcPanel->Add( sizerProcStopFound, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// match my association distance
	sizerProcPanel->Add( new wxStaticText(pPanelProc, -1, wxT("Assoc match max distance:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerProcPanel->Add( m_pAssocMatchMaxDist, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );


	///////////////////////////////////
	// Populate Objects panel
	// extract object by shape
	sizerObjectsPanel->Add( new wxStaticText(pPanelObjects, -1, wxT("Img Extraction By Shape:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerObjectsPanel->Add( m_pImgObjectExtractByObjectShape, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// extract object by detection type
	sizerObjectsPanel->Add( new wxStaticText(pPanelObjects, -1, wxT("Img Extraction By Detect Type:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerObjectsPanel->Add( m_pImgObjectExtractByDetectionType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// max no of objects in the image to scan match
	sizerObjectsPanel->Add( new wxStaticText(pPanelObjects, -1, wxT("Image Objects To S-Match:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerObjectsPanel->Add( m_pImgNoObjMatch, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// max no of objects in the catalog to scan match
	sizerObjectsPanel->Add( new wxStaticText(pPanelObjects, -1, wxT("Catalog Objects To S-Match:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerObjectsPanel->Add( m_pCatNoObjMatch, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// search minimum cat stars
	sizerObjectsPanel->Add( new wxStaticText(pPanelObjects, -1, wxT("Min Catlog objects to Match:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerObjectsPanel->Add( m_pSearchMatchMinCatStars, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// minimum objects matched needed
	sizerObjectsPanel->Add( new wxStaticText(pPanelObjects, -1, wxT("Min Objects Matched needed:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerObjectsPanel->Add( m_pMatchFirstLevelMinStars, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// Fine tune image stars factor
	sizerObjectsPanel->Add( new wxStaticText(pPanelObjects, -1, wxT("Finetune Image Stars Fact:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerObjectsPanel->Add( m_pMatchFineTuneImgStarsFactor, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// Fine tune catalog stars factor
	sizerObjectsPanel->Add( new wxStaticText(pPanelObjects, -1, wxT("Finetune Catalog Stars Fact:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerObjectsPanel->Add( m_pMatchFineTuneCatStarsFactor, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );


	///////////////////////////////////
	// Populate Match Panel
	// trasform type
	sizerMatchPanel->Add(new wxStaticText(pPanelMatch, -1, wxT("Transformation Type:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerMatchPanel->Add( m_pTransTypeSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// recalculate match
	sizerMatchPanel->Add( new wxStaticText(pPanelMatch, -1, wxT("Re-Calculate Match:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerMatchPanel->Add( m_pMatchRecalc, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// maximum match distance
	sizerMatchPanel->Add( new wxStaticText(pPanelMatch, -1, wxT("Max Match Distance:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerMatchPanel->Add( m_pMaxMatchDist, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// Triangle fit tolerance
	sizerMatchPanel->Add(new wxStaticText(pPanelMatch, -1, wxT("Triangle Fit Tolerance:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerMatchPanel->Add( m_pTriangFit, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// Triangle refence scale
	sizerMatchPanel->Add(new wxStaticText(pPanelMatch, -1, wxT("Triangle Reference Scale:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerMatchPanel->Add( m_pTriangScale, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// max iterations
	sizerMatchPanel->Add(new wxStaticText(pPanelMatch, -1, wxT("Recursive Level:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerMatchPanel->Add( m_pMaxIter, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// halt sigma 
	sizerMatchPanel->Add(new wxStaticText(pPanelMatch, -1, wxT("Match Tolerance:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerMatchPanel->Add( m_pHaltSigma, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	///////////////////////////////////
	// Populate FINDER Panel
	wxFlexGridSizer* sizerFinderPanelCol = new wxFlexGridSizer( 10, 1, 5, 5 );
	sizerFinderPanelCol->Add( m_pLocateCatStars, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerFinderPanelCol->Add( m_pLocateDso, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerFinderPanelCol->Add( m_pLocateXGamma, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerFinderPanelCol->Add( m_pLocateRadio, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerFinderPanelCol->Add( m_pLocateSupernovas, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerFinderPanelCol->Add( m_pLocateBlackholes, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerFinderPanelCol->Add( m_pLocateMStars, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerFinderPanelCol->Add( m_pLocateExoplanets, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerFinderPanelCol->Add( m_pLocateAsteroids, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerFinderPanelCol->Add( m_pLocateComets, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerFinderPanelCol->Add( m_pLocateAes, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerFinderPanelCol->Add( m_pLocateSolar, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerFinderPanelCol->Add( m_pFecthDetailsOnFind, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	sizerFinderPanel->Add( sizerFinderPanelCol, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL  );

	///////////////////////////////////
	// Populate FINDER Panel
	// Time-frame tolerance
	wxFlexGridSizer* sizerSolarPanelTab = new wxFlexGridSizer( 2, 2, 5, 5 );
	sizerSolarPanelTab->Add(new wxStaticText(pPanelSolar, -1, wxT("Time-frame tolerance(s):")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerSolarPanelTab->Add( m_pTimeFrameRange, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// Time-frame division
	sizerSolarPanelTab->Add(new wxStaticText(pPanelSolar, -1, wxT("Time-frame division(s):")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerSolarPanelTab->Add( m_pTimeFrameDivision, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// only TLE in range
	sizerSolarPanelTab->Add( m_pOnlyTleInRange, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerSolarPanelTab->Add( m_pTleInRangeValue, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerSolarPanel->Add( sizerSolarPanelTab, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL  );

	////////////////////////
	// set the sizers
	pPanelSearch->SetSizer( sizerSearchPage );
	pNotebook->AddPage( pPanelSearch, wxT("Search") );
	pPanelProc->SetSizer( sizerProcPage );
	pNotebook->AddPage( pPanelProc, wxT("Handle") );
	pPanelObjects->SetSizer( sizerObjectsPage );
	pNotebook->AddPage( pPanelObjects, wxT("Objects") );
	pPanelMatch->SetSizer( sizerMatchPage );
	pNotebook->AddPage( pPanelMatch, wxT("Match") );
	pPanelFinder->SetSizer( sizerFinderPage );
	pNotebook->AddPage( pPanelFinder, wxT("Finder") );
	pPanelSolar->SetSizer( sizerSolarPage );
	pNotebook->AddPage( pPanelSolar, wxT("Solar") );

	pTopSizer->Add( pNotebook, 1, wxEXPAND | wxALL, 10 );
	pTopSizer->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxCENTRE | wxALL, 10 );

	// activate
	SetSizer( pTopSizer );
	SetAutoLayout(TRUE);
	pTopSizer->SetSizeHints(this);	
//	pTopSizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CMatchConfig
// Purpose:	Delete my object
// Input:	mothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CMatchConfig::~CMatchConfig( )
{
	// delete search panel elements
	delete( m_pSearchMethod );
	delete( m_pSearchAreaIncrementOnRA );
	delete( m_pSearchAreaIncrementOnDEC );
	delete( m_pRaSearchOverlapDivision );
	delete( m_pDecSearchOverlapDivision );
	delete( m_pSearchZoomSteps );
	delete( m_pSearchZoomStart );
	delete( m_pDoReverseZoomSearch );

	// delete proc panel elements
	delete( m_pProjectionMethod );
	delete( m_pDoMatchFineTune );
	delete( m_pDoMatchFineTuneWithProjection );
	delete( m_pMatchByMostStars );
	delete( m_pMatchByBestFit );
	delete( m_pDoMatchRefineBestArea );
	delete( m_pDoMatchStopOnFirstFound );
	delete( m_pMatchStopOnMinFound );
	delete( m_pAssocMatchMaxDist );

	// delete object panel elements
	delete( m_pImgObjectExtractByObjectShape );
	delete( m_pImgObjectExtractByDetectionType );
	delete( m_pImgNoObjMatch );
	delete( m_pCatNoObjMatch );
	delete( m_pSearchMatchMinCatStars );
	delete( m_pMatchFirstLevelMinStars );
	delete( m_pMatchFineTuneImgStarsFactor );
	delete( m_pMatchFineTuneCatStarsFactor );

	// delete match panel elements
    delete( m_pTransTypeSelect );
	delete( m_pMatchRecalc );
	delete( m_pMaxMatchDist );
	delete( m_pTriangFit );
	delete( m_pTriangScale );
	delete( m_pMaxIter );
	delete( m_pHaltSigma );

	// delete finder
	delete( m_pLocateCatStars );
	delete( m_pLocateDso );
	delete( m_pLocateXGamma );
	delete( m_pLocateRadio );
	delete( m_pLocateSupernovas );
	delete( m_pLocateBlackholes );
	delete( m_pLocateMStars );
	delete( m_pLocateExoplanets	 );
	delete( m_pLocateAsteroids );
	delete( m_pLocateComets );
	delete( m_pLocateAes );
	delete( m_pLocateSolar );
	delete( m_pFecthDetailsOnFind );

	// delete solar
	// time-frame
	delete( m_pTimeFrameRange );
	delete( m_pTimeFrameDivision );
	delete( m_pOnlyTleInRange );
	delete( m_pTleInRangeValue );
}

////////////////////////////////////////////////////////////////////
// Method:	SetConfig
// Class:	CMatchConfig
// Purpose:	Set and get my reference from the config object
// Input:	pointer to star catalog and main config
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CMatchConfig::SetConfig( CConfigMain* pMainConfig )
{
//	m_pDetectConfig = pDetectConfig;
	int nVarInt = 0;
	double nVarFloat = 0.0;
	wxString strMsg;

	//////////////////////////////////////////////////////////////
	// CONFIG :: Matching section :: SEARCH
	// get search method type
	m_pSearchMethod->SetSelection( pMainConfig->m_nSkySearchMethod );
	// search area increment on RA
	strMsg.Printf( wxT("%.6f"), pMainConfig->m_nSearchAreaIncrementOnRA );
	m_pSearchAreaIncrementOnRA->SetValue( strMsg );
	// search area increment on DEC
	strMsg.Printf( wxT("%.6f"), pMainConfig->m_nSearchAreaIncrementOnDEC );
	m_pSearchAreaIncrementOnDEC->SetValue( strMsg );
	// search area overlap division on RA
	strMsg.Printf( wxT("%d"), pMainConfig->m_nRaSearchOverlapDivision );
	m_pRaSearchOverlapDivision->SetValue( strMsg );
	// search area overlap division on DEC
	strMsg.Printf( wxT("%d"), pMainConfig->m_nDecSearchOverlapDivision );
	m_pDecSearchOverlapDivision->SetValue( strMsg );
	// search area zoom steps
	strMsg.Printf( wxT("%d"), pMainConfig->m_nSearchZoomSteps );
	m_pSearchZoomSteps->SetValue( strMsg );
	// search area zoom start
	strMsg.Printf( wxT("%d"), pMainConfig->m_nSearchZoomStart );
	m_pSearchZoomStart->SetValue( strMsg );
	// DoReverseZoomSearch
	m_pDoReverseZoomSearch->SetValue( (bool) pMainConfig->m_bDoReverseZoomSearch );

	/////////////////////////////////////////////////////////////////////////////////////////
	// CONFIG :: Matching section :: PROC
	// projection method
	m_pProjectionMethod->SetSelection( pMainConfig->m_nSkyProjectionMethod );
	// DoMatchFineTune
	m_pDoMatchFineTune->SetValue( pMainConfig->m_bDoMatchFineTune );
	// DoMatchFineTuneWithProjection
	m_pDoMatchFineTuneWithProjection->SetValue( (bool) pMainConfig->m_bDoMatchFineTuneWithProjection );
	// MatchByMostStars
	m_pMatchByMostStars->SetValue( (bool) pMainConfig->m_bMatchCheckByNoOfStars );
	// MatchByBestFit
	m_pMatchByBestFit->SetValue( pMainConfig->m_bMatchCheckByBestFit );
	// DoMatchRefineBestArea
	m_pDoMatchRefineBestArea->SetValue( pMainConfig->m_bDoMatchRefineBestArea );
	// DoMatchStopOnFirstFound
	m_pDoMatchStopOnFirstFound->SetValue( pMainConfig->m_bMatchStopWhenMinIsFound );
	if( pMainConfig->m_bMatchStopWhenMinIsFound == 0 )
		m_pMatchStopOnMinFound->Disable();
	else
		m_pMatchStopOnMinFound->Enable();
	// MatchStopOnMinFound
	strMsg.Printf( wxT("%d"), pMainConfig->m_nMatchMinFoundToStop );
	m_pMatchStopOnMinFound->SetValue( strMsg );
	// AssocMatchMaxDist
	strMsg.Printf( wxT("%.6f"), pMainConfig->m_nAssocMatchMaxDist );
	m_pAssocMatchMaxDist->SetValue( strMsg );

	//////////////////////////////////////////////////////////////
	// CONFIG :: Matching section :: OBJECTS
	// object extraction by object shape
	m_pImgObjectExtractByObjectShape->SetValue( pMainConfig->m_nImgObjectExtractByObjShape );
	// object extraction by detection type
	m_pImgObjectExtractByDetectionType->SetValue( pMainConfig->m_nImgObjectExtractByDetectType );
	// get number of objects in the image to match
	strMsg.Printf( wxT("%d"), pMainConfig->m_nScanMatchImgObjNo );
	m_pImgNoObjMatch->SetValue( strMsg );
	// get number of objects in the catalog to match
	strMsg.Printf( wxT("%d"), pMainConfig->m_nScanMatchCatObjNo );
	m_pCatNoObjMatch->SetValue( strMsg );
	// minimum catalog stars to use
	strMsg.Printf( wxT("%d"), pMainConfig->m_nSearchMatchMinCatStars );
	m_pSearchMatchMinCatStars->SetValue( strMsg );
	// minimum matched stars needed
	strMsg.Printf( wxT("%d"), pMainConfig->m_nMatchFirstLevelMinStars );
	m_pMatchFirstLevelMinStars->SetValue( strMsg );
	// finetune image stars factor
	strMsg.Printf( wxT("%.6f"), pMainConfig->m_nMatchFineTuneImgStarsFactor );
	m_pMatchFineTuneImgStarsFactor->SetValue( strMsg );
	// finetune catalog stars factor
	strMsg.Printf( wxT("%.6f"), pMainConfig->m_nMatchFineTuneCatStarsFactor );
	m_pMatchFineTuneCatStarsFactor->SetValue( strMsg );

	//////////////////////////////////////////////////////////////
	// CONFIG :: Matching section :: MATCH
	// get transformation type
	m_pTransTypeSelect->SetSelection( pMainConfig->m_nMatchTransType );
	// do recalculation
	m_pMatchRecalc->SetValue( pMainConfig->m_bMatchDoRecalc );
	// max distance between stars
	strMsg.Printf( wxT("%.6f"), pMainConfig->m_nMatchMaxDist );
	m_pMaxMatchDist->SetValue( strMsg );
	// trangle fit distance
	strMsg.Printf( wxT("%.8f"), pMainConfig->m_nMatchTriangFit );
	m_pTriangFit->SetValue( strMsg );
	// triangle scale
	strMsg.Printf( wxT("%.4f"), pMainConfig->m_nMatchTriangScale );
	m_pTriangScale->SetValue( strMsg );
	// max iterations
	strMsg.Printf( wxT("%d"), pMainConfig->m_nMatchMaxIter );
	m_pMaxIter->SetValue( strMsg );
	// halt sigma
	strMsg.Printf( wxT("%.20f"), pMainConfig->m_nMatchHaltSigma );
	m_pHaltSigma->SetValue( strMsg );

	//////////////////////////////////////////////////////////////
	// CONFIG :: Matching section :: FINDER
	m_pLocateCatStars->SetValue( pMainConfig->m_bMatchLocateCatStars );
	m_pLocateDso->SetValue( pMainConfig->m_bMatchLocateDso );
	m_pLocateXGamma->SetValue( pMainConfig->m_bMatchLocateXGamma );
	m_pLocateRadio->SetValue( pMainConfig->m_bMatchLocateRadio );
	m_pLocateSupernovas->SetValue( pMainConfig->m_bMatchLocateSupernovas );
	m_pLocateBlackholes->SetValue( pMainConfig->m_bMatchLocateBlackholes );
	m_pLocateMStars->SetValue( pMainConfig->m_bMatchLocateMStars );
	m_pLocateExoplanets->SetValue( pMainConfig->m_bMatchLocateExoplanets );
	m_pLocateAsteroids->SetValue( pMainConfig->m_bMatchLocateAsteroids );
	m_pLocateComets->SetValue( pMainConfig->m_bMatchLocateComets );
	m_pLocateAes->SetValue( pMainConfig->m_bMatchLocateAes );
	m_pLocateSolar->SetValue( pMainConfig->m_bMatchLocateSolar );
	m_pFecthDetailsOnFind->SetValue( pMainConfig->m_bMatchFetchDetails );

	//////////////////////////////////////////////////////////////
	// CONFIG :: Matching section :: SOLAR
	// time-frame
	m_pTimeFrameRange->SetValue( pMainConfig->m_nMatchLocateTimeFrameRange );
	m_pTimeFrameDivision->SetValue( pMainConfig->m_nMatchLocateTimeFrameDivision );
	m_pOnlyTleInRange->SetValue( pMainConfig->m_bOnlyTleInRange );
	m_pTleInRangeValue->SetValue( pMainConfig->m_nTleInRangeValue );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnCheckStopOnFirstFound
// Class:		CMatchConfig
// Purpose:		set stop on found and enable min entry field
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CMatchConfig::OnCheckStopOnFirstFound( wxCommandEvent& event )
{
	int nCheck = event.IsChecked( );

	if( nCheck )
	{
		m_pMatchStopOnMinFound->Enable();
	} else
	{
		m_pMatchStopOnMinFound->Disable();
	}
}

////////////////////////////////////////////////////////////////////
// Method:	OnValidate
// Class:	CMatchConfig
// Purpose:	validate my data on ok
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CMatchConfig::OnValidate( wxCommandEvent& event )
{
/*	wxString strMsg = "";
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
		wxMessageBox( strMsg, wxT("Data Error!"), wxOK|wxICON_ERROR );
	} else
		event.Skip();
*/
	event.Skip();
	return;
}

