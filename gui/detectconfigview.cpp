////////////////////////////////////////////////////////////////////
// Package:		Detect Config View implementation
// File:		configview.cpp
// Purpose:		define a config view window
//
// Created by:	Larry Lart on 24/06/2007
// Updated by:	Larry Lart on 06/03/2010
//
// Copyright:	(c) 2007-2010 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

#include "wx/wxprec.h"
#include "wx/statline.h"
#include "wx/minifram.h"
#include "wx/thread.h"
#include "wx/notebook.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// local headers
#include "../config/config.h"
#include "../config/detectconfig.h"
#include "../config/mainconfig.h"
#include "../unimap.h"
#include "../proc/detectfilter.h"
#include "simple_common_dlg.h"

// main header
#include "detectconfigview.h"

//		***** CLASS CDetectConfigView *****
////////////////////////////////////////////////////////////////////
// CDetectConfigView EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(CDetectConfigView, wxDialog)
	EVT_BUTTON( wxID_BUTTON_ADD_PROFILE, CDetectConfigView::OnAddProfile )
	EVT_BUTTON( wxID_BUTTON_DELETE_PROFILE, CDetectConfigView::OnDeleteProfile )
	EVT_CHOICE( wxID_SELECT_DETECT_PROFILE, CDetectConfigView::OnSelectDetectProfile )
	EVT_CHECKBOX( wxID_SELECT_DETECT_DO_CLEAN, CDetectConfigView::OnUseCleaning )
	EVT_CHECKBOX( wxID_SELECT_DETECT_USE_CHECKIMAGE, CDetectConfigView::OnUseCheckImage )
	EVT_BUTTON( wxID_BUTTON_CHECK_IMAGE_BROWSE, CDetectConfigView::OnCheckImageBrowse )
	// validate observer data on ok
	EVT_BUTTON( wxID_OK, CDetectConfigView::OnValidate )
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CDetectConfigView
// Purpose:	Initialize my about dialog
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
CDetectConfigView::CDetectConfigView(wxWindow *parent) : wxDialog( parent, -1,
                          _("Detection configuration"),
                          wxDefaultPosition, wxDefaultSize, //wxSize( 350, 310 ),
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	wxFont smallFont( 6, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT );
	int i=0;
	// initialize local vars
	m_pDetectConfig = NULL;
	m_pMainConfig = NULL;
	m_nSelectedProfile = 0;

	wxString vectStrImageChannels[7];
	wxString vectStrProfileSelect[10];
	wxString vectStrDetectType[2];
	wxString vectStrFilterName[2];
	wxString vectStrMaskType[3];
	wxString vectStrThreshType[2];
	wxString vectStrWeightType[6];
	wxString vectStrBackPhotoType[2];
//	wxString vectStrVerboseType[4];
	wxString vectStrCheckImageType[18];

	// init profile select
	vectStrProfileSelect[0] = wxT("Default");

	// init image channels channels
	vectStrImageChannels[0] = wxT("Red");
	vectStrImageChannels[1] = wxT("Green");
	vectStrImageChannels[2] = wxT("Blue");
	vectStrImageChannels[3] = wxT("Gray");
	vectStrImageChannels[4] = wxT("Median");
	vectStrImageChannels[5] = wxT("Best");
	// init selection for image type
	vectStrDetectType[0] = wxT("CCD");
	vectStrDetectType[1] = wxT("FILM");
	// init selection filter name
	vectStrFilterName[0] = wxT("None");
	vectStrFilterName[1] = wxT("Default");
	// init mask type
	vectStrMaskType[0] = wxT("None");
	vectStrMaskType[1] = wxT("Blank");
	vectStrMaskType[2] = wxT("Correct");
	// init thresh type
	vectStrThreshType[0] = wxT("Absolute");
	vectStrThreshType[1] = wxT("Relative");
	// init Weight Type
	vectStrWeightType[0] = wxT("None");
	vectStrWeightType[1] = wxT("Background");
	vectStrWeightType[2] = wxT("Rms Map");
	vectStrWeightType[3] = wxT("Var Map");
	vectStrWeightType[4] = wxT("Weight Map");
	vectStrWeightType[5] = wxT("Interpolation");
	// init :: Back Photo Type
	vectStrBackPhotoType[0] = wxT("Global");
	vectStrBackPhotoType[1] = wxT("Local");
	// init verbose type
//	vectStrVerboseType[0] = wxT("Quiet");
//	vectStrVerboseType[1] = wxT("Normal");
//	vectStrVerboseType[2] = wxT("Warning");
//	vectStrVerboseType[3] = wxT("Full");
	// Check Image Type
//	vectStrCheckImageType[0] = wxT("None");
	vectStrCheckImageType[0] = wxT("Identical");
	vectStrCheckImageType[1] = wxT("Background");
	vectStrCheckImageType[2] = wxT("Mini Background");
	vectStrCheckImageType[3] = wxT("Mini Back Rms");
	vectStrCheckImageType[4] = wxT("Sub Background");
	vectStrCheckImageType[5] = wxT("Filtered");
	vectStrCheckImageType[6] = wxT("Objects");
	vectStrCheckImageType[7] = wxT("Apertures");
	vectStrCheckImageType[8] = wxT("Segmentation");
	vectStrCheckImageType[9] = wxT("Association");
	vectStrCheckImageType[10] = wxT("Sub Objects");
	vectStrCheckImageType[11] = wxT("Sub PSF Protos");
	vectStrCheckImageType[12] = wxT("Sub PCP Protos");
	vectStrCheckImageType[13] = wxT("PCP Protos");
	vectStrCheckImageType[14] = wxT("PCAP Protos");
	vectStrCheckImageType[15] = wxT("Map SOM");

	// main dialog sizer
	wxFlexGridSizer *topsizer = new wxFlexGridSizer( 9, 1, 1, 10 );
	topsizer->AddGrowableRow( 1 );

	//////////////////////////////////////////////////
	// create profile chooser sizer
	wxStaticBox* pProfileBox = new wxStaticBox( this, -1, wxT("Profile Chooser"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* pProfileSizer = new wxStaticBoxSizer( pProfileBox, wxVERTICAL );
	// layout page
	wxGridSizer* pProfilePageSizer = new wxGridSizer( 2, 1, 5, 5 );
	// select sizer
	wxFlexGridSizer* pProfileSelectSizer = new wxFlexGridSizer( 1, 3, 5, 5 );
	pProfileSelectSizer->AddGrowableCol( 1 );
	// profile buttons sizer
	wxGridSizer* pProfileButtonsSizer = new wxGridSizer( 1, 2, 5, 10 );
	// add to the profile page sizer
	pProfilePageSizer->Add( pProfileSelectSizer, 1,
                  wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	pProfilePageSizer->Add( pProfileButtonsSizer, 1,
                  wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// addprofile page sizer to the main box sizer
	pProfileSizer->Add( pProfilePageSizer, 1,
                  wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	/////////////////////////////////////
	// create elements of the profile
	m_pProfileSelect = new wxChoice( this, wxID_SELECT_DETECT_PROFILE,
										wxDefaultPosition,
										wxSize(200,-1), 1, vectStrProfileSelect, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pProfileSelect->SetSelection( 0 );

	m_pAddProfile = new wxButton( this, wxID_BUTTON_ADD_PROFILE, wxT("Add Profile"),
										wxPoint( -1, -1 ),
										wxSize( 100, -1 ) );

	m_pDeleteProfile = new wxButton( this, wxID_BUTTON_DELETE_PROFILE, wxT("Delete Profile"),
										wxPoint( -1, -1 ),
										wxSize( 100, -1 ) );

	/////////////////////////////////////////
	// populate profile chooser sizer
	// row profile selction 
	pProfileSelectSizer->Add(new wxStaticText( this, -1, wxT("Select Profile:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	pProfileSelectSizer->Add( m_pProfileSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row profile buttons 
	pProfileButtonsSizer->Add( m_pAddProfile, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL  );
	pProfileButtonsSizer->Add( m_pDeleteProfile, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL  );

	//////////////////////////////////////////////////
	// building the detect configure as a note book
	wxNotebook *pNotebook = new wxNotebook( this, -1 ); //, wxPoint( 0, 0 ),
									//wxSize( 450, 540 ) );

	////////////////////////////////////////////////////////////////
	// CREATE PANELS
	// add Detect config panel
	wxPanel* pPanelDetect = new wxPanel( pNotebook );
	// add processingf config panel
	wxPanel* pPanelProc = new wxPanel( pNotebook );
	// add photometry config panel
	wxPanel* pPanelPhotometry = new wxPanel( pNotebook );
	// add classification config panel
	wxPanel* pPanelClass = new wxPanel( pNotebook );
	// add Gesture config panel
//	wxPanel* pPanelMisc = new wxPanel( pNotebook );


	/////////////////////////////////
	// Detection panel sizer - layout of elements
	wxGridSizer *sizerDetectPanel = new wxGridSizer( 4, 2, 5, 5 );
	// layout of group of elements in the panel 
	wxGridSizer* sizerDetectPage = new wxGridSizer( 1, 1, 10, 10 );
	sizerDetectPage->Add( sizerDetectPanel, 1,
                  wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	//////////////////////////////////
	// processing panel sizer
	wxFlexGridSizer *sizerProcPanel = new wxFlexGridSizer( 4, 2, 10, 5 );
	// layout of group of elements in the panel 
	wxGridSizer* sizerProcPage = new wxGridSizer( 1, 1, 2, 2 );
	sizerProcPage->Add( sizerProcPanel, 1,
                  wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// set column 0 growable
	sizerProcPanel->AddGrowableCol( 1 );

	//////////////////////////////////
	// Photometry panel sizer
	wxFlexGridSizer *sizerPhotometryPanel = new wxFlexGridSizer( 12, 2, 8, 5 );
	// layout of group of elements in the panel 
	wxGridSizer* sizerPhotometryPage = new wxGridSizer( 1, 1, 2, 2 );
	//sizerPhotometryPage->AddSpacer(10);
	sizerPhotometryPage->Add( sizerPhotometryPanel, 0,
                  wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL |wxBOTTOM | wxTOP, 10 );
	// set column 0 growable
	sizerPhotometryPanel->AddGrowableCol( 0, 20 );

	//////////////////////////////////
	// Class panel sizer
	wxFlexGridSizer *sizerClassPanel = new wxFlexGridSizer( 2, 2, 8, 5 );
	// layout of group of elements in the panel 
	wxGridSizer* sizerClassPage = new wxGridSizer( 1, 1, 2, 2 );
	sizerClassPage->Add( sizerClassPanel, 0,
                  wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// set column 0 growable
	sizerClassPanel->AddGrowableCol( 0, 20 );

	//////////////////////////////////
	// Misc panel sizer
//	wxFlexGridSizer *sizerMiscPanel = new wxFlexGridSizer( 3, 2, 10, 10 );
	// layout of group of elements in the panel 
//	wxGridSizer* sizerMiscPage = new wxGridSizer( 1, 1, 2, 2 );
//	sizerMiscPage->Add( sizerMiscPanel, 0,
//                  wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// set column 0 growable
//	sizerMiscPanel->AddGrowableCol( 0, 20 );


	////////////////////////////////////
	// Build items for panel Detect
	m_pImageChannels = new wxChoice( pPanelDetect, -1,
										wxDefaultPosition,
										wxSize(60,-1), 6, vectStrImageChannels, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pImageChannels->SetSelection( 0 );

	m_pUseMaskAreas = new wxCheckBox( pPanelDetect, -1, wxT("") );

	m_pDetectType = new wxChoice( pPanelDetect, -1,
										wxDefaultPosition,
										wxSize(50,-1), 2, vectStrDetectType, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pDetectType->SetSelection( 0 );

	m_pDetectMinArea = new wxTextCtrl( pPanelDetect, -1, wxT(""),
									wxDefaultPosition, wxSize(50,-1) );

	m_pDetectThresh = new wxTextCtrl( pPanelDetect, -1, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );

	m_pAnalysisThresh = new wxTextCtrl( pPanelDetect, -1, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );

	m_pDeblendNThresh = new wxTextCtrl( pPanelDetect, -1, wxT(""),
									wxDefaultPosition, wxSize(50,-1) );

	m_pDeblendNMinCont = new wxTextCtrl( pPanelDetect, -1, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );
	// clean y/n
	m_pDetectClean = new wxCheckBox( pPanelDetect, wxID_SELECT_DETECT_DO_CLEAN, wxT("Detection Cleaning:") );

	// clean param
	m_pDetectCleanParam = new wxTextCtrl( pPanelDetect, -1, wxT(""),
									wxDefaultPosition, wxSize(50,-1) );


	//////////////////////////////////////
	// Build Items for panel processing
	// use filter
	m_pDetectFilter = new wxCheckBox( pPanelProc, -1, wxT("") );
	// detection filter name
	m_pDetectFilterName = new wxChoice( pPanelProc, -1,
										wxDefaultPosition,
										wxSize(100,-1), 2, vectStrFilterName, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pDetectFilterName->SetSelection( 0 );
	// filter threshold
	m_pFilterThresh = new wxTextCtrl( pPanelProc, -1, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );
	// MaskType
	m_pMaskType  = new wxChoice( pPanelProc, -1,
										wxDefaultPosition,
										wxSize(100,-1), 3, vectStrMaskType, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pMaskType->SetSelection( 0 );
	// ThreshType
	m_pThreshType = new wxChoice( pPanelProc, -1,
										wxDefaultPosition,
										wxSize(100,-1), 2, vectStrThreshType, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pThreshType->SetSelection( 0 );
	// WeightType
	m_pWeightType = new wxChoice( pPanelProc, -1,
										wxDefaultPosition,
										wxSize(100,-1), 6, vectStrWeightType, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pWeightType->SetSelection( 0 );

	// ScanIsoApRatio
	m_pScanIsoApRatio = new wxTextCtrl( pPanelProc, -1, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );
	// Blank
	m_pMiscBlank = new wxCheckBox( pPanelProc, -1, wxT("") );


	//////////////////////////////////////
	// Build Items for panel Photometry

	// PhotApertures
	m_pPhotApertures = new wxTextCtrl( pPanelPhotometry, -1, wxT(""),
									wxDefaultPosition, wxSize(50,-1) );
	// PhotAutoparamKron
	m_pPhotAutoparamKron = new wxTextCtrl( pPanelPhotometry, -1, wxT(""),
									wxDefaultPosition, wxSize(55,-1) );
	// PhotAutoparamMinRad
	m_pPhotAutoparamMinRad = new wxTextCtrl( pPanelPhotometry, -1, wxT(""),
									wxDefaultPosition, wxSize(55,-1) );
	// PhotPetroparamFact
	m_pPhotPetroparamFact = new wxTextCtrl( pPanelPhotometry, -1, wxT(""),
									wxDefaultPosition, wxSize(55,-1) );
	// PhotPetroparamMinRad
	m_pPhotPetroparamMinRad = new wxTextCtrl( pPanelPhotometry, -1, wxT(""),
									wxDefaultPosition, wxSize(55,-1) );
	// SaturationLevel
	m_pSaturationLevel = new wxTextCtrl( pPanelPhotometry, -1, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );
	// MagZeroPoint
	m_pMagZeroPoint = new wxTextCtrl( pPanelPhotometry, -1, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );
	// MagGamma
	m_pMagGamma = new wxTextCtrl( pPanelPhotometry, -1, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );
	// Gain
	m_pPhotometryGain = new wxTextCtrl( pPanelPhotometry, -1, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );
	// PixelScale
	m_pPixelScale = new wxTextCtrl( pPanelPhotometry, -1, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );


	//////////////////////////////////////
	// Build Items for panel classification

	// SeeingFwhm
	m_pSeeingFwhm = new wxTextCtrl( pPanelClass, -1, wxT(""),
									wxDefaultPosition, wxSize(80,-1) );
	// StarNnwName
	m_pStarNnwName = new wxChoice( pPanelClass, -1,
										wxDefaultPosition,
										wxSize(100,-1), 0, NULL, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pStarNnwName->SetSelection( 0 );
	// BackSize
	m_pBackSize = new wxTextCtrl( pPanelClass, -1, wxT(""),
									wxDefaultPosition, wxSize(50,-1) );
	// BackFilterSize
	m_pBackFilterSize = new wxTextCtrl( pPanelClass, -1, wxT(""),
									wxDefaultPosition, wxSize(50,-1) );
	// BackPhotoType
	m_pBackPhotoType = new wxChoice( pPanelClass, -1,
										wxDefaultPosition,
										wxSize(100,-1), 2, vectStrBackPhotoType, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pBackPhotoType->SetSelection( 0 );
	// BackFilterThresh
	m_pBackFilterThresh = new wxTextCtrl( pPanelClass, -1, wxT(""), wxDefaultPosition, wxSize(80,-1) );

	// use check image
	m_pUseCheckImage = new wxCheckBox( pPanelClass, wxID_SELECT_DETECT_USE_CHECKIMAGE, wxT("Check-Image Type:") );
	// CheckImageType
	m_pCheckImageType = new wxChoice( pPanelClass, -1, wxDefaultPosition,
										wxSize(100,-1), 16, vectStrCheckImageType, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pCheckImageType->SetSelection( 0 );
	// CheckImageName
	m_pCheckImageName = new wxTextCtrl( pPanelClass, -1, wxT(""), wxDefaultPosition, wxSize(130,-1) );
	m_pCheckImagePick = new wxButton( pPanelClass, wxID_BUTTON_CHECK_IMAGE_BROWSE, wxT("Check-Image File"),
										wxPoint( -1, -1 ), wxSize( 100, -1 ) );

	//////////////////////////////////////
	// Build Items for panel Misc
	// MemObjStack
//	m_pMemObjStack = new wxTextCtrl( pPanelMisc, -1, wxT(""),
//									wxDefaultPosition, wxSize(60,-1) );
	// MemPixStack
//	m_pMemPixStack = new wxTextCtrl( pPanelMisc, -1, wxT(""),
//									wxDefaultPosition, wxSize(60,-1) );
	// MemBufSize
//	m_pMemBufSize = new wxTextCtrl( pPanelMisc, -1, wxT(""),
//									wxDefaultPosition, wxSize(60,-1) );
	// VerboseType
//	m_pVerboseType = new wxChoice( pPanelMisc, -1,
//										wxDefaultPosition,
//										wxSize(100,-1), 4, vectStrVerboseType, 
//										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
//	m_pVerboseType->SetSelection( 0 );
	// WriteXml
//	m_pWriteXml = new wxCheckBox( pPanelMisc, -1, wxT("") );
	// XmlName
//	m_pXmlName = new wxTextCtrl( pPanelMisc, -1, wxT(""),
//									wxDefaultPosition, wxSize(60,-1) );

	/////////////////////////////////////////
	// populate Detect panel
	// row :: image channels
	sizerDetectPanel->Add(new wxStaticText( pPanelDetect, -1, wxT("Image Channels:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerDetectPanel->Add( m_pImageChannels, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row :: use areas mask
	sizerDetectPanel->Add(new wxStaticText( pPanelDetect, -1, wxT("Use Areas To Mask:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerDetectPanel->Add( m_pUseMaskAreas, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row image type 
	sizerDetectPanel->Add(new wxStaticText( pPanelDetect, -1, wxT("Image Type:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerDetectPanel->Add( m_pDetectType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row min detection area
	sizerDetectPanel->Add(new wxStaticText( pPanelDetect, -1, wxT("Minimum Detection Area:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerDetectPanel->Add( m_pDetectMinArea, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row for detection threshold
	sizerDetectPanel->Add(new wxStaticText( pPanelDetect, -1, wxT("Detection Threshold:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerDetectPanel->Add( m_pDetectThresh, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row for Analysis threshold
	sizerDetectPanel->Add(new wxStaticText( pPanelDetect, -1, wxT("Analysis Threshold:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerDetectPanel->Add( m_pAnalysisThresh, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row for deblending division
	sizerDetectPanel->Add(new wxStaticText( pPanelDetect, -1, wxT("Deblending Division:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerDetectPanel->Add( m_pDeblendNThresh, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row for deblending contrast
	sizerDetectPanel->Add(new wxStaticText( pPanelDetect, -1, wxT("Deblending Contrast:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerDetectPanel->Add( m_pDeblendNMinCont, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row for detect clean
//	sizerDetectPanel->Add(new wxStaticText( pPanelDetect, -1, wxT("Detection Cleaning:")), 0,
//                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
//	sizerDetectPanel->Add( m_pDetectClean, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row for detect clean param
//	sizerDetectPanel->Add(new wxStaticText( pPanelDetect, -1, wxT("Cleaning Param:")), 0,
//                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerDetectPanel->Add( m_pDetectClean, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerDetectPanel->Add( m_pDetectCleanParam, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	/////////////////////////////////////////////
	// populate Processing panel	
	// row filter use 
	sizerProcPanel->Add(new wxStaticText( pPanelProc, -1, wxT("Use Filter:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerProcPanel->Add( m_pDetectFilter, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row filter Name 
	sizerProcPanel->Add(new wxStaticText( pPanelProc, -1, wxT("Filter Name:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerProcPanel->Add( m_pDetectFilterName, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row filter treshold 
	sizerProcPanel->Add(new wxStaticText( pPanelProc, -1, wxT("Filter Treshold:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerProcPanel->Add( m_pFilterThresh, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row mask type 
	sizerProcPanel->Add(new wxStaticText( pPanelProc, -1, wxT("Mask Type:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerProcPanel->Add( m_pMaskType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row treshold type 
	sizerProcPanel->Add(new wxStaticText( pPanelProc, -1, wxT("Treshold Type:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerProcPanel->Add( m_pThreshType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row Weight Type 
	sizerProcPanel->Add(new wxStaticText( pPanelProc, -1, wxT("Weight Type:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerProcPanel->Add( m_pWeightType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row :: Scan Iso Ap Ratio
	sizerProcPanel->Add(new wxStaticText( pPanelProc, -1, wxT("Scan Iso Aperture Ratio:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerProcPanel->Add( m_pScanIsoApRatio, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row :: Use Blank
	sizerProcPanel->Add(new wxStaticText( pPanelProc, -1, wxT("Use Blank:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerProcPanel->Add( m_pMiscBlank, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	/////////////////////////////////////////////
	// populate Photometry panel	
	// row Phot Apertures 
	sizerPhotometryPanel->Add(new wxStaticText( pPanelPhotometry, -1, wxT("Apertures:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerPhotometryPanel->Add( m_pPhotApertures, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row Phot Autoparam Kron/Min Rad
	sizerPhotometryPanel->Add(new wxStaticText( pPanelPhotometry, -1, wxT("Autoparam Kron/Radius:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	wxFlexGridSizer *sizerPhotometryPanelAP = new wxFlexGridSizer( 1, 2, 0, 3 );
	sizerPhotometryPanelAP->Add( m_pPhotAutoparamKron, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerPhotometryPanelAP->Add( m_pPhotAutoparamMinRad, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerPhotometryPanel->Add( sizerPhotometryPanelAP, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row Phot Petroparam Fact/Min Rad
	sizerPhotometryPanel->Add(new wxStaticText( pPanelPhotometry, -1, wxT("Petroparam Fact/Radius:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	wxFlexGridSizer *sizerPhotometryPanelPP = new wxFlexGridSizer( 1, 2, 0, 3 );
	sizerPhotometryPanelPP->Add( m_pPhotPetroparamFact, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerPhotometryPanelPP->Add( m_pPhotPetroparamMinRad, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerPhotometryPanel->Add( sizerPhotometryPanelPP, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row Saturation Level
	sizerPhotometryPanel->Add(new wxStaticText( pPanelPhotometry, -1, wxT("Saturation Level:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerPhotometryPanel->Add( m_pSaturationLevel, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row Mag Zero Point
	sizerPhotometryPanel->Add(new wxStaticText( pPanelPhotometry, -1, wxT("Magnitude Zero Point:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerPhotometryPanel->Add( m_pMagZeroPoint, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row Mag Gamma
	sizerPhotometryPanel->Add(new wxStaticText( pPanelPhotometry, -1, wxT("Magnitude Gamma:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerPhotometryPanel->Add( m_pMagGamma, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row Photometry Gain
	sizerPhotometryPanel->Add(new wxStaticText( pPanelPhotometry, -1, wxT("Photometry Gain:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerPhotometryPanel->Add( m_pPhotometryGain, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row Pixel Scale
	sizerPhotometryPanel->Add(new wxStaticText( pPanelPhotometry, -1, wxT("Pixel Scale:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerPhotometryPanel->Add( m_pPixelScale, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	/////////////////////////////////////////////
	// populate classification panel	
	// row :: Seeing Fwhm
	sizerClassPanel->Add(new wxStaticText( pPanelClass, -1, wxT("Seeing Fwhm:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerClassPanel->Add( m_pSeeingFwhm, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row :: Star Nnw Name
	sizerClassPanel->Add(new wxStaticText( pPanelClass, -1, wxT("Classifier Profile:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerClassPanel->Add( m_pStarNnwName, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row :: Back Size
	sizerClassPanel->Add(new wxStaticText( pPanelClass, -1, wxT("Background Block Size:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerClassPanel->Add( m_pBackSize, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row :: Back Filter Size
	sizerClassPanel->Add(new wxStaticText( pPanelClass, -1, wxT("Background Filter Size:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerClassPanel->Add( m_pBackFilterSize, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row :: Back Photo Type
	sizerClassPanel->Add(new wxStaticText( pPanelClass, -1, wxT("Background Photo Type:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerClassPanel->Add( m_pBackPhotoType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row :: Back Filter Thresh
	sizerClassPanel->Add(new wxStaticText( pPanelClass, -1, wxT("Background Filter Thresh:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerClassPanel->Add( m_pBackFilterThresh, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row :: Check Image Type
	sizerClassPanel->Add( m_pUseCheckImage, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerClassPanel->Add( m_pCheckImageType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row :: Check Image Name
//	sizerClassPanel->Add(new wxStaticText( pPanelClass, -1, wxT("Check-Image File:")), 0,
//                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerClassPanel->Add( m_pCheckImagePick, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
//	wxFlexGridSizer *sizerClassPanelCI = new wxFlexGridSizer( 1, 2, 5, 0 );
//	sizerClassPanelCI->Add( m_pCheckImageName, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
//	sizerClassPanelCI->Add( m_pCheckImagePick, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerClassPanel->Add( m_pCheckImageName, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	/////////////////////////////////////////////
	// populate Misc panel	
	// row :: Mem Obj Stack
//	sizerMiscPanel->Add(new wxStaticText( pPanelMisc, -1, wxT("Mem Obj Stack:")), 0,
//                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
//	sizerMiscPanel->Add( m_pMemObjStack, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row :: Mem Pix Stack
//	sizerMiscPanel->Add(new wxStaticText( pPanelMisc, -1, wxT("Mem Pix Stack:")), 0,
//                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
//	sizerMiscPanel->Add( m_pMemPixStack, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row :: Mem Buf Size
//	sizerMiscPanel->Add(new wxStaticText( pPanelMisc, -1, wxT("Mem Buf Size:")), 0,
//                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
//	sizerMiscPanel->Add( m_pMemBufSize, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row :: Verbose Type
//	sizerMiscPanel->Add(new wxStaticText( pPanelMisc, -1, wxT("Verbose Type:")), 0,
//                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
//	sizerMiscPanel->Add( m_pVerboseType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row :: Write Xml
//	sizerMiscPanel->Add(new wxStaticText( pPanelMisc, -1, wxT("Write Xml:")), 0,
//                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
//	sizerMiscPanel->Add( m_pWriteXml, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row :: Xml Name
//	sizerMiscPanel->Add(new wxStaticText( pPanelMisc, -1, wxT("Xml Name:")), 0,
//                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
//	sizerMiscPanel->Add( m_pXmlName, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );


//	topsizer->Add( pProfileSelectSizer, 1, wxGROW | wxALL, 10 );
	topsizer->Add( pProfileSizer, 1, wxALIGN_CENTER|wxGROW | wxALL, 3 );
	topsizer->Add( pNotebook, 1, wxALIGN_CENTER|wxGROW | wxALL, 3 );
	topsizer->Add( CreateButtonSizer(wxOK | wxCANCEL ), 1, wxALIGN_CENTER| wxCENTRE | wxALL, 5 );

	// add Detect
	pPanelDetect->SetSizer( sizerDetectPage );
	pNotebook->AddPage( pPanelDetect, _("Detection") );
	// add Processing
	pPanelProc->SetSizer( sizerProcPage );
	pNotebook->AddPage( pPanelProc, _("Processing") );
	// add Photometry
	pPanelPhotometry->SetSizer( sizerPhotometryPage );
	pNotebook->AddPage( pPanelPhotometry, _("Photometry") );
	// add Classification
	pPanelClass->SetSizer( sizerClassPage );
	pNotebook->AddPage( pPanelClass, _("Classification") );

	// add Misc
//	pPanelMisc->SetSizer( sizerMiscPage );
//	pNotebook->AddPage( pPanelMisc, _("Misc") );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
//	sizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CDetectConfigView
// Purpose:	destroy my  dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CDetectConfigView::~CDetectConfigView( ) 
{
	m_pDetectConfig = NULL;
	m_pMainConfig = NULL;
	m_vectProfile.clear();

	// new 
	delete( m_pProfileSelect );
	delete( m_pAddProfile );
	delete( m_pDeleteProfile );

	// detection
	delete( m_pImageChannels );
	delete( m_pUseMaskAreas );
	delete( m_pDetectType );
	delete( m_pDetectMinArea );
	delete( m_pDetectThresh );
	delete( m_pAnalysisThresh );
	delete( m_pDeblendNThresh );
	delete( m_pDeblendNMinCont );
	delete( m_pDetectClean );
	delete( m_pDetectCleanParam );

	// filter
	delete( m_pDetectFilter );
	delete( m_pDetectFilterName );
	delete( m_pFilterThresh );
	delete( m_pMaskType );
	delete( m_pThreshType );
	delete( m_pWeightType );
	delete( m_pScanIsoApRatio );
	delete( m_pMiscBlank );

	// Photometry
	delete( m_pPhotApertures );
	delete( m_pPhotAutoparamKron );
	delete( m_pPhotAutoparamMinRad );
	delete( m_pPhotPetroparamFact );
	delete( m_pPhotPetroparamMinRad );
	delete( m_pSaturationLevel );
	delete( m_pMagZeroPoint );
	delete( m_pMagGamma );
	delete( m_pPhotometryGain );
	delete( m_pPixelScale );
	// Proc
	delete( m_pSeeingFwhm );
	delete( m_pStarNnwName );
	delete( m_pBackSize );
	delete( m_pBackFilterSize );
	delete( m_pBackPhotoType );
	delete( m_pBackFilterThresh );
	delete( m_pUseCheckImage );
	delete( m_pCheckImageType );
	delete( m_pCheckImageName );
	delete( m_pCheckImagePick );
	// Misc
//	delete( m_pMemObjStack );
//	delete( m_pMemPixStack );
//	delete( m_pMemBufSize );
//	delete( m_pVerboseType );
//	delete( m_pWriteXml );
//	delete( m_pXmlName );
}

////////////////////////////////////////////////////////////////////
// Purpose:	store my current selected profile
////////////////////////////////////////////////////////////////////
void CDetectConfigView::StoreSelectedProfile( ) 
{
	long nIntVar = 0;
	double nFloatVar = 0;

	// get current profile pointer
	DefConfigDetector* profile = &(m_vectProfile[m_nSelectedProfile]);

	////////////////////////////
	// TAB :: DETECT
	// image channels
	profile->ImageChannels = (int) m_pImageChannels->GetSelection()+1;
	// mask areas
	profile->MaskAreas = (int) m_pUseMaskAreas->GetValue();
	// Image Select Type
	profile->ImageType = m_pDetectType->GetSelection();
	// minimum area in pixels
	if( m_pDetectMinArea->GetLineText(0).ToLong( &nIntVar ) )
		profile->DetectMinArea = (int) nIntVar;
	// area threshold
	if( m_pDetectThresh->GetLineText(0).ToDouble( &nFloatVar ) )
		profile->DetectThresh = nFloatVar;
	// Analysis threshold
	if( m_pAnalysisThresh->GetLineText(0).ToDouble( &nFloatVar ) )
		profile->AnalysisThresh = nFloatVar;
	// deblending sub-threshold
	if( m_pDeblendNThresh->GetLineText(0).ToLong( &nIntVar ) )
		profile->DeblendNThresh = (int) nIntVar;
	// deblending contrast
	if( m_pDeblendNMinCont->GetLineText(0).ToDouble( &nFloatVar ) )
		profile->DeblendNMinCont = nFloatVar;
	// detect clean
	profile->DetectClean = (int) m_pDetectClean->GetValue();
	// Detect Clean Param
	if( m_pDetectCleanParam->GetLineText(0).ToLong( &nIntVar ) )
		profile->CleanParam = (int) nIntVar;

	////////////////////////
	// TAB :: PROCESSING
	// use filter y/n
	profile->isFilter = (int) m_pDetectFilter->GetValue();
	// filter name
	profile->FilterName = (int) m_pDetectFilterName->GetSelection();
	// m_pFilterThresh - CONF_FILTER_THRESH / float
	if( m_pFilterThresh->GetLineText(0).ToDouble( &nFloatVar ) )
		profile->FilterThresh = nFloatVar;
	// m_pMaskType CONF_MASK_TYPE
	profile->MaskType = (int) m_pMaskType->GetSelection();
	// m_pThreshType CONF_THRESH_TYPE
	profile->ThreshType = (int) m_pThreshType->GetSelection();
	// m_pWeightType CONF_WEIGHT_TYPE
	profile->WeightType = (int) m_pWeightType->GetSelection();
	// m_pScanIsoApRatio CONF_SCAN_ISOAPRATIO
	if( m_pScanIsoApRatio->GetLineText(0).ToDouble( &nFloatVar ) )
		profile->ScanIsoApRatio = nFloatVar;
	// m_pMiscBlank CONF_BLANK
	profile->Blank = (int) m_pMiscBlank->GetValue();

	////////////////////////
	// TAB :: PHOTOMETRY
	// m_pPhotApertures CONF_PHOT_APERTURES
	if( m_pPhotApertures->GetLineText(0).ToLong( &nIntVar ) )
		profile->PhotApertures = (int) nIntVar;
	// m_pPhotAutoparamKron CONF_PHOT_AUTOPARAMS_KRON
	if( m_pPhotAutoparamKron->GetLineText(0).ToDouble( &nFloatVar ) )
		profile->PhotAutoparamKron = nFloatVar;
	// m_pPhotAutoparamMinRad CONF_PHOT_AUTOPARAMS_MINRAD
	if( m_pPhotAutoparamMinRad->GetLineText(0).ToDouble( &nFloatVar ) )
		profile->PhotAutoparamMinRad = nFloatVar;
	// m_pPhotPetroparamFact CONF_PHOT_PETROPARAMS_FACT
	if( m_pPhotPetroparamFact->GetLineText(0).ToDouble( &nFloatVar ) )
		profile->PhotPetroparamFact = nFloatVar;
	// m_pPhotPetroparamMinRad CONF_PHOT_PETROPARAMS_MINRAD
	if( m_pPhotPetroparamMinRad->GetLineText(0).ToDouble( &nFloatVar ) )
		profile->PhotPetroparamMinRad = nFloatVar;
	// m_pSaturationLevel CONF_SATUR_LEVEL
	if( m_pSaturationLevel->GetLineText(0).ToDouble( &nFloatVar ) )
		profile->SaturationLevel = nFloatVar;
	// m_pMagZeroPoint CONF_MAG_ZEROPOINT
	if( m_pMagZeroPoint->GetLineText(0).ToDouble( &nFloatVar ) )
		profile->MagZeroPoint = nFloatVar;
	// m_pMagGamma CONF_MAG_GAMMA
	if( m_pMagGamma->GetLineText(0).ToDouble( &nFloatVar ) )
		profile->MagGamma = nFloatVar;
	// m_pPhotometryGain CONF_GAIN
	if( m_pPhotometryGain->GetLineText(0).ToDouble( &nFloatVar ) )
		profile->Gain = nFloatVar;
	// m_pPixelScale CONF_PIXEL_SCALE
	if( m_pPixelScale->GetLineText(0).ToDouble( &nFloatVar ) )
		profile->PixelScale = nFloatVar;

	////////////////////////
	// TAB :: CLASSIFICATION
	// m_pSeeingFwhm CONF_SEEING_FWHM float
	if( m_pSeeingFwhm->GetLineText(0).ToDouble( &nFloatVar ) )
		profile->SeeingFwhm = nFloatVar;
	// m_pStarNnwName CONF_STARNNW_NAME
	wxStrcpy( profile->NNClassName, m_pStarNnwName->GetStringSelection() );
	// m_pBackSize CONF_BACK_SIZE int
	if( m_pBackSize->GetLineText(0).ToLong( &nIntVar ) )
		profile->BackSize = (int) nIntVar;
	// m_pBackFilterSize CONF_BACK_FILTERSIZE int
	if( m_pBackFilterSize->GetLineText(0).ToLong( &nIntVar ) )
		profile->BackFilterSize = (int) nIntVar;
	// m_pBackPhotoType CONF_BACKPHOTO_TYPE
	profile->BackPhotoType = (int) m_pBackPhotoType->GetSelection();
	// m_pBackFilterThresh CONF_BACK_FILTERTHRESH
	if( m_pBackFilterThresh->GetLineText(0).ToDouble( &nFloatVar ) )
		profile->BackFilterThresh = nFloatVar;
	// m_pCheckImageType CONF_CHECKIMAGE_TYPE int
	if( m_pUseCheckImage->IsChecked() )
		profile->CheckImageType = (int) m_pCheckImageType->GetSelection()+1;
	else
		profile->CheckImageType = 0;
	// m_pCheckImageName CONF_CHECKIMAGE_NAME string
	wxStrcpy( profile->CheckImageFile, m_pCheckImageName->GetLineText(0) );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetConfig
// Class:	CDetectConfigView
// Purpose:	Set and get my reference from the config object
// Input:	pointer to detect config and main config
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDetectConfigView::SetConfig( CConfigDetect* pDetectConfig, CConfigMain* pMainConfig )
{
	m_pDetectConfig = pDetectConfig;
	m_pMainConfig = pMainConfig;
	int nVarInt = 0;
	int i=0, nProfileId=0;
	double nVarFloat = 0;

	// adjust profile selection - set selection list
	m_pProfileSelect->Clear();
	for( i = 0; i<pDetectConfig->m_vectProfile.size(); i++ )
		m_pProfileSelect->Append( pDetectConfig->m_vectProfile[i].name );

	// First get current profile last used from config
	pMainConfig->GetIntVar( SECTION_ID_DETECTION, CONF_DETECTION_PROFILE, nProfileId, 0 );
	// set my current profile
	m_pProfileSelect->SetSelection( nProfileId );
	m_nSelectedProfile = nProfileId;
									
	// copy detection profiles localy
	m_vectProfile.clear();
	for( i=0; i<pDetectConfig->m_vectProfile.size(); i++ )
	{
		DefConfigDetector src;
		memcpy( &src, &(pDetectConfig->m_vectProfile[i]), sizeof(DefConfigDetector) );
		m_vectProfile.push_back( src );
	}
	// set current profile
	SetCurrentProfile( );
}

////////////////////////////////////////////////////////////////////
void CDetectConfigView::SetCurrentProfile( )
{
	int i=0;
	wxString strVal=wxT("");
	wxString strVar=wxT("");
	// get current selected profile
	DefConfigDetector* profile = &(m_vectProfile[m_nSelectedProfile]);

	//////////////////////////////////////////////////////////////
	// CONFIG :: Detection Section
	// set image channels
	m_pImageChannels->SetSelection( profile->ImageChannels-1 );
	// set use areas mask
	m_pUseMaskAreas->SetValue( profile->MaskAreas );
	// set image type
	m_pDetectType->SetSelection( profile->ImageType );
	// set minimum area
	strVal.Printf( wxT("%d"), profile->DetectMinArea );
	m_pDetectMinArea->SetValue( strVal );
	// get detection threshold
	strVal.Printf( wxT("%.9f"), profile->DetectThresh );
	m_pDetectThresh->SetValue( strVal );
	// get analysis threshold
	strVal.Printf( wxT("%.9f"), profile->AnalysisThresh );
	m_pAnalysisThresh->SetValue( strVal );
	// get deblending sub-thresholds
	strVal.Printf( wxT("%d"), profile->DeblendNThresh );
	m_pDeblendNThresh->SetValue( strVal );
	// get deblending threshold
	strVal.Printf( wxT("%.9f"), profile->DeblendNMinCont );
	m_pDeblendNMinCont->SetValue( strVal );
	// use Detect Clean
	m_pDetectClean->SetValue( profile->DetectClean );
	SetCleaningState( m_pDetectClean->IsChecked() );
	// get Detect Clean Param
	strVal.Printf( wxT("%d"), profile->CleanParam );
	m_pDetectCleanParam->SetValue( strVal );
	//////////////////////////////////////////////////////////////
	// CONFIG :: Processing Section
	// use filter y/n
	m_pDetectFilter->SetValue( (bool) profile->isFilter );	
	// get filter type/name
	// create list from config - do every time 
	wxArrayString vecFilt;
	vecFilt.Alloc( m_pDetectConfig->m_nDetectFilter );
	// add to vector
	for( i=0; i<m_pDetectConfig->m_nDetectFilter; i++ )
		vecFilt.Add( m_pDetectConfig->m_vectDetectFilter[i]->m_strFilterName );
	// now set the list obj
	m_pDetectFilterName->Freeze( );
	m_pDetectFilterName->Clear( );
	m_pDetectFilterName->Append( vecFilt );
	m_pDetectFilterName->Thaw( );
	// now get current
	m_pDetectFilterName->SetSelection( profile->FilterName );
	// get filter treshold
	strVal.Printf( wxT("%.9f"), profile->FilterThresh );
	m_pFilterThresh->SetValue( strVal );
	// get mask type
	m_pMaskType->SetSelection( profile->MaskType );
	// get treshold type
	m_pThreshType->SetSelection( profile->ThreshType );
	// get weight type
	m_pWeightType->SetSelection( profile->WeightType );
	// get Scan Iso Ap Ratio
	strVal.Printf( wxT("%.9f"), profile->ScanIsoApRatio );
	m_pScanIsoApRatio->SetValue( strVal );
	// use Blank
	m_pMiscBlank->SetValue( profile->Blank );
	//////////////////////////////////////////////////////////////
	// CONFIG :: Photometry Section
	// get Phot Apertures
	strVal.Printf( wxT("%d"), profile->PhotApertures );
	m_pPhotApertures->SetValue( strVal );
	// get Phot Autoparam Kron
	strVal.Printf( wxT("%.5f"), profile->PhotAutoparamKron );
	m_pPhotAutoparamKron->SetValue( strVal );
	// get Phot Autoparam MinRad
	strVal.Printf( wxT("%.5f"), profile->PhotAutoparamMinRad );
	m_pPhotAutoparamMinRad->SetValue( strVal );
	// get Phot Petroparam Fact
	strVal.Printf( wxT("%.5f"), profile->PhotPetroparamFact );
	m_pPhotPetroparamFact->SetValue( strVal );
	// get Phot Petroparam MinRad
	strVal.Printf( wxT("%.5f"), profile->PhotPetroparamMinRad );
	m_pPhotPetroparamMinRad->SetValue( strVal );
	// get Saturation Level
	strVal.Printf( wxT("%.3f"), profile->SaturationLevel );
	m_pSaturationLevel->SetValue( strVal );
	// get Mag Zero Point
	strVal.Printf( wxT("%.9f"), profile->MagZeroPoint );
	m_pMagZeroPoint->SetValue( strVal );
	// get Mag Gamma
	strVal.Printf( wxT("%.9f"), profile->MagGamma );
	m_pMagGamma->SetValue( strVal );
	// get Photometry Gain
	strVal.Printf( wxT("%.9f"), profile->Gain );
	m_pPhotometryGain->SetValue( strVal );
	// get Pixel Scale
	strVal.Printf( wxT("%.9f"), profile->PixelScale );
	m_pPixelScale->SetValue( strVal );
	//////////////////////////////////////////////////////////////
	// CONFIG :: Classification Section
	// get Seeing Fwhm
	strVal.Printf( wxT("%.9f"), profile->SeeingFwhm );
	m_pSeeingFwhm->SetValue( strVal );
	////////////
	// get Star Nnw Name - string ? /label ?
	// fir populate
	m_pStarNnwName->Clear();
	if( m_pDetectConfig->m_vectNNWClass.Count() )
	{
		for( i=0; i<m_pDetectConfig->m_vectNNWClass.Count(); i++ )
			m_pStarNnwName->Append( m_pDetectConfig->m_vectNNWClass[i] );
		m_pStarNnwName->SetSelection(0);
	} else
		m_pStarNnwName->Disable();
	// now get the value from config
	strVar = profile->NNClassName;
	if( !strVar.IsEmpty() )
	{
		// now check here which index should i select
		for( i=0; i<m_pDetectConfig->m_vectNNWClass.Count(); i++ )
		{
			if( strVar.CmpNoCase( m_pDetectConfig->m_vectNNWClass[i] ) == 0 )
				m_pStarNnwName->SetSelection(i);
		}
	}
	// get Back Size
	strVal.Printf( wxT("%d"), profile->BackSize );
	m_pBackSize->SetValue( strVal );
	// get Back Filter Size
	strVal.Printf( wxT("%d"), profile->BackFilterSize );
	m_pBackFilterSize->SetValue( strVal );
	// get Back Photo Type
	m_pBackPhotoType->SetSelection( profile->BackPhotoType );
	// get Back Filter Thresh
	strVal.Printf( wxT("%.9f"), profile->BackFilterThresh );
	m_pBackFilterThresh->SetValue( strVal );
	// get Check Image Type
	if( profile->CheckImageType <= 0 )
	{
		SetCheckImageState(0);
		m_pCheckImageType->SetSelection(0);

	} else
	{
		SetCheckImageState(1);
		m_pCheckImageType->SetSelection( profile->CheckImageType-1 );
	}
	// get Check Image Name - string
	m_pCheckImageName->SetValue( profile->CheckImageFile );

	//////////////////////////////////////////////////////////////
	// CONFIG :: Misc Section
	// get Mem Obj Stack
/*	if( pDetectConfig->GetIntVar( nProfileId, CONF_MEMORY_OBJSTACK, nVarInt ) == 0 )
	{
		strMsg.Printf( wxT("%d"), 0 );
		m_pMemObjStack->SetValue( strMsg );
	} else 
	{
		strMsg.Printf( wxT("%d"), nVarInt );
		m_pMemObjStack->SetValue( strMsg );
	}
	// get Mem Pix Stack
	if( pDetectConfig->GetIntVar( nProfileId, CONF_MEMORY_PIXSTACK, nVarInt ) == 0 )
	{
		strMsg.Printf( wxT("%d"), 0 );
		m_pMemPixStack->SetValue( strMsg );
	} else 
	{
		strMsg.Printf( wxT("%d"), nVarInt );
		m_pMemPixStack->SetValue( strMsg );
	}
	// get Mem Buf Size
	if( pDetectConfig->GetIntVar( nProfileId, CONF_MEMORY_BUFSIZE, nVarInt ) == 0 )
	{
		strMsg.Printf( wxT("%d"), 0 );
		m_pMemBufSize->SetValue( strMsg );
	} else 
	{
		strMsg.Printf( wxT("%d"), nVarInt );
		m_pMemBufSize->SetValue( strMsg );
	}
	// get Verbose Type
	if( pDetectConfig->GetIntVar( nProfileId, CONF_VERBOSE_TYPE, nVarInt ) == 0 )
	{
		m_pVerboseType->SetSelection( 0 );

	} else 
	{
		m_pVerboseType->SetSelection( nVarInt );
	}
	// use Write Xml
	if( pDetectConfig->GetIntVar( nProfileId, CONF_WRITE_XML, nVarInt ) == 0 )
	{
		m_pWriteXml->SetValue( 0 );

	} else 
	{
		m_pWriteXml->SetValue( nVarInt );	
	}
	// get Xml Name - string
	if( pDetectConfig->GetStrVar( nProfileId, CONF_XML_NAME, strVar ) == 0 )
	{
		strMsg.Printf( wxT("%s"), wxT("") );
		m_pXmlName->SetValue( strMsg );
	} else 
	{
		strMsg.Printf( wxT("%s"), strVar );
		m_pXmlName->SetValue( strMsg );
	}
*/

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnAddProfile
// Class:		CDetectConfigView
// Purpose:		add a new profile / show entry dialog
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CDetectConfigView::OnAddProfile( wxCommandEvent& pEvent )
{
	CSimpleEntryAddDlg *pAddProfile = new CSimpleEntryAddDlg( this, wxT("Add New Detection Profile"), wxT("Profile Name:") );
	if ( pAddProfile->ShowModal( ) == wxID_OK )
	{
		// check if entry
		wxString strProfileName = pAddProfile->m_pName->GetLineText(0);
		int nProfileId=0;
		if( strProfileName != wxT("") )
		{
			nProfileId = m_pDetectConfig->AddDetectProfile( strProfileName );
		}
		// set selection to new profile
		m_pProfileSelect->SetSelection( nProfileId );
		m_pMainConfig->SetIntVar( SECTION_ID_DETECTION, CONF_DETECTION_PROFILE, nProfileId );
		m_nSelectedProfile = nProfileId;

		// set current profile configuration
		SetCurrentProfile( );
	}

	delete( pAddProfile );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnDeleteProfile
// Class:		CDetectConfigView
// Purpose:		delete profile / ask for confirmation
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CDetectConfigView::OnDeleteProfile( wxCommandEvent& pEvent )
{
	int nRet = wxMessageBox( wxT("Delete Profile?"), wxT("Confirm"), wxYES_NO, this );
	if( nRet == wxYES )
	{
		int nId = m_pProfileSelect->GetSelection();
		//m_pDetectConfig->DeleteProfile( nId );
		m_vectProfile.erase( m_vectProfile.begin()+nId );
		m_pProfileSelect->Delete( nId );

		// select current profile as the last after delete
		int nLastElem = m_vectProfile.size()-1;
		if( nId >= nLastElem )
			m_pProfileSelect->SetSelection( nLastElem );
		else
			m_pProfileSelect->SetSelection( nId );
		SelectDetectProfile( );
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnSelectDetectProfile
// Class:		CDetectConfigView
// Purpose:		select detect profile
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CDetectConfigView::OnSelectDetectProfile( wxCommandEvent& pEvent )
{
	// save previous selection
	StoreSelectedProfile( );
	// select current selection
	SelectDetectProfile( );
	return;
}

////////////////////////////////////////////////////////////////////
void CDetectConfigView::SelectDetectProfile( )
{
	int nSelect = m_pProfileSelect->GetSelection();

	// keep curent selection
	m_nSelectedProfile = nSelect;
	// save to main config
	m_pMainConfig->SetIntVar( SECTION_ID_DETECTION, CONF_DETECTION_PROFILE, nSelect );
	// also set to detect config
	m_pDetectConfig->m_nDetectProfileId = nSelect;

	// set current profile configuration
	SetCurrentProfile( );

	// if current selection is 0 disable button delete else enable
	if( nSelect <= 0 )
		m_pDeleteProfile->Disable();
	else
		m_pDeleteProfile->Enable();
}

////////////////////////////////////////////////////////////////////
void CDetectConfigView::OnUseCleaning( wxCommandEvent& pEvent )
{
	SetCleaningState( pEvent.IsChecked() );
}

////////////////////////////////////////////////////////////////////
void CDetectConfigView::SetCleaningState( int bValue )
{
	if( bValue )
		m_pDetectCleanParam->Enable();
	else
		m_pDetectCleanParam->Disable();
}

////////////////////////////////////////////////////////////////////
void CDetectConfigView::OnUseCheckImage( wxCommandEvent& pEvent )
{
	SetCheckImageState( pEvent.IsChecked() );
}

////////////////////////////////////////////////////////////////////
void CDetectConfigView::SetCheckImageState( int bValue )
{
	if( bValue )
	{
		m_pCheckImageType->Enable();
		m_pCheckImageName->Enable();
		m_pCheckImagePick->Enable();

	} else
	{
		m_pCheckImageType->Disable();
		m_pCheckImageName->Disable();
		m_pCheckImagePick->Disable();
	}
}

////////////////////////////////////////////////////////////////////
void CDetectConfigView::OnCheckImageBrowse( wxCommandEvent& pEvent )
{
	wxFileDialog fileDlg( this, wxT("Select Check-Image to use..."),
							wxGetCwd(), wxT(""), wxT("*.*"), 
							wxFD_OPEN  | wxFD_FILE_MUST_EXIST );
	
	if( fileDlg.ShowModal() == wxID_OK )
	{
		m_pCheckImageName->SetValue( fileDlg.GetPath() );
	}
}

////////////////////////////////////////////////////////////////////
void CDetectConfigView::OnValidate( wxCommandEvent& pEvent )
{
	// :: store last selected setup
	StoreSelectedProfile( );

	pEvent.Skip();
	return;
}
