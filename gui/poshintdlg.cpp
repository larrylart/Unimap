////////////////////////////////////////////////////////////////////
// Package:		Position Hint Dialog implementation
// File:		poshintdlg.cpp
// Purpose:		create dialog to set image position hint
//
// Created by:	Larry Lart on 28/06/2007
// Updated by:	Larry Lart on 14/02/2010
//
// Copyright:	(c) 2004-2010 Larry Lart
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

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// local headers
#include "../util/func.h"
#include "../config/mainconfig.h"
#include "../sky/sky.h"
#include "../sky/catalog_stars.h"
#include "../sky/catalog_stars_names.h"
#include "../sky/catalog_dso.h"
#include "../sky/catalog_dso_ngc.h"
#include "../sky/catalog_dso_ic.h"
#include "../sky/catalog_dso_messier.h"
#include "../image/astroimage.h"
#include "../image/imagegroup.h"
#include "../camera/camera.h"
#include "../telescope/telescope.h"
#include "hardware_select_dlg.h"
#include "tlens_select_dlg.h"

// main header
#include "poshintdlg.h"

////////////////////////////////////////////////////////////////////
// CPosHintDlg EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( CPosHintDlg, wxDialog )
	// image hint/type
	EVT_CHOICE( wxID_SELECT_USE_HINT, CPosHintDlg::OnUseHint )
	// group use hint/apply to images
	EVT_CHECKBOX( wxID_CHECK_GROUP_USE_HINT, CPosHintDlg::OnUseGroupHint )
	// origin
	EVT_CHOICE( wxID_SELECT_ORIG_TYPE, CPosHintDlg::OnSelectOrigType )
	EVT_CHOICE( wxID_SELECT_ORIG_NAME, CPosHintDlg::OnSelectOrigName )
	EVT_CHOICE( wxID_SELECT_ORIG_ENTRY_TYPE, CPosHintDlg::OnEntryOrigType )
	EVT_TEXT( wxID_SELECT_ORIG_ID, CPosHintDlg::OnSetObjectId )

	EVT_RADIOBUTTON( wxID_RADIO_ORIG_A, CPosHintDlg::OnRadioOrigEntry )
	EVT_RADIOBUTTON( wxID_RADIO_ORIG_B, CPosHintDlg::OnRadioOrigEntry )

	EVT_RADIOBUTTON( wxID_RADIO_FIELD_A, CPosHintDlg::OnRadioFieldEntry )
	EVT_RADIOBUTTON( wxID_RADIO_FIELD_B, CPosHintDlg::OnRadioFieldEntry )
	// sensor size
	EVT_TEXT( wxID_SENSOR_WIDTH, CPosHintDlg::OnSetSensorSize )
	EVT_TEXT( wxID_SENSOR_HEIGHT, CPosHintDlg::OnSetSensorSize )
	EVT_BUTTON( wxID_CAMERA_SELECT_BUTTON, CPosHintDlg::OnSelectCamera )
	// telescope/lens
	EVT_TEXT( wxID_SET_FOCAL_VALUE, CPosHintDlg::OnSetFocalLength )
	EVT_BUTTON( wxID_TLENS_SELECT_BUTTON, CPosHintDlg::OnSelectTLens )
	// field
	EVT_CHOICE( wxID_SELECT_FIELD_FORMAT_TYPE, CPosHintDlg::OnFieldFormat )
	// final validation
	EVT_BUTTON( wxID_OK, CPosHintDlg::OnValidate )
END_EVENT_TABLE()


////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CPosHintDlg
// Purpose:	Initialize my about dialog // 
// Input:	pointer to reference window 
// Output:	nothing
////////////////////////////////////////////////////////////////////
CPosHintDlg::CPosHintDlg(wxWindow *parent, const wxString& title, int nType )
               : wxDialog(parent, -1,
                          title,
                          wxDefaultPosition,
                          wxSize( 360, 440 ),
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	// init here first some defaults
	m_bUseHint = 0;
	m_bUseGroupHint = 0;
	m_nOrigType = 0;
	m_nRaDeg=0; 
	m_nDecDeg=0;
	m_nSensorWidth = 0;
	m_nSensorHeight = 0;
	m_nPixelSize = 0;
	m_nFieldX = 0;
	m_nFieldY = 0;
	m_nFocalLength = 2000;
	m_pFocal = NULL;
	// default camera selection to -1
	m_nCameraSource = 0;
	m_nCameraType = 0;
	m_nCameraBrand = 6;
	m_nCameraName = 0;
	// tlens system
	m_nTLensSource = 0;
	memset( &m_rLensSetup, 0, sizeof(DefTLensSetup) );

	int i=0;
	// initialize local vars
	wxString vectImgUsePosHint[5];
	wxString vectObjReference[20];
	wxString vectObjNameId[2];
	wxString vectRaDecEntryType[2];
	wxString vectFieldFormat[2];
	wxString vectHardwareSetup[2];
	vectHardwareSetup[0] = wxT("None");
	wxString vectImgUseGroupPosHint[5];
	vectImgUseGroupPosHint[0] = wxT("Don't Apply Group Hint To Images");
	vectImgUseGroupPosHint[1] = wxT("Apply Group Hint To Images With No Hint");
	vectImgUseGroupPosHint[2] = wxT("Overwrite Group Hint On All Images ");
	// type of hints
	vectImgUsePosHint[0] = wxT("None");
	vectImgUsePosHint[1] = wxT("Custom");
	vectImgUsePosHint[2] = wxT("Image");
	vectImgUsePosHint[3] = wxT("Group");

	// init selection for sky object sources
	vectObjReference[0] = wxT("Star Common Name");
	vectObjReference[1] = wxT("Dso Common Name");
	vectObjReference[2] = wxT("Constellation Names");
	vectObjReference[3] = wxT("Messier Id");
	vectObjReference[4] = wxT("Ngc Id");
	vectObjReference[5] = wxT("Ic Id");
	vectObjReference[6] = wxT("Sao Id");

	// init selection for x10 controler type
	vectObjNameId[0] = wxT("bal1");
	vectObjNameId[1] = wxT("bla2");

	// ra/dec entry type
	vectRaDecEntryType[0] = wxT("Sexagesimal");
	vectRaDecEntryType[1] = wxT("Decimal");

	// field format type
	vectFieldFormat[0] = wxT("DD:MM:SS");
	vectFieldFormat[1] = wxT("Decimal");

	// main dialog sizer
	wxFlexGridSizer *topsizer = new wxFlexGridSizer( 3, 1, 5, 5 );

	/////////////////////////////
	// Image origin hint sizer - layout of elements
	wxFlexGridSizer* sizerOrigChoice = new wxFlexGridSizer( 2, 2, 10, 10 );
	sizerOrigChoice->AddGrowableCol( 1 );
	// panel for seting origin useing names and ids
	sizerOrigAPanel = new wxFlexGridSizer( 2, 2, 3, 10 );
	sizerOrigAPanel->AddGrowableCol( 1 );
	// panel for seting origin useing ra/dec
	wxFlexGridSizer* sizerOrigBPanel = new wxFlexGridSizer( 2, 2, 3, 10 );
//	sizerOrigAPanel->AddGrowableCol( 0 );

	// layout of group of elements in the orig panel 
	m_pOrigBox = new wxStaticBox( this, -1, wxT("Image Origin Hint"), wxPoint(-1,-1), wxDefaultSize );
	
	wxStaticBoxSizer* sizerOrigPage = new wxStaticBoxSizer( m_pOrigBox, wxHORIZONTAL );
	sizerOrigPage->Add( sizerOrigChoice, 0,
                  wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	
	// create radio options
	m_pOrigOptA = new wxRadioButton( this, wxID_RADIO_ORIG_A, wxT(""), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_pOrigOptB = new wxRadioButton( this, wxID_RADIO_ORIG_B, wxT("") );

	// add orig pannel a to the choice panel
	sizerOrigChoice->Add( m_pOrigOptA, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	sizerOrigChoice->Add( sizerOrigAPanel, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	sizerOrigChoice->Add( m_pOrigOptB, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	sizerOrigChoice->Add( sizerOrigBPanel, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	/////////////////////////////
	// Image field hint sizer - layout of elements
	wxFlexGridSizer* sizerFieldPanel = new wxFlexGridSizer( 2, 2, 10, 10 );
	sizerFieldPanel->AddGrowableCol( 1 );
	// panel for seting origin useing names and ids
	wxFlexGridSizer* sizerFieldPanelA = new wxFlexGridSizer( 2, 2, 3, 10 );
//	sizerFieldPanelA->AddGrowableCol( 0 );
	// panel for seting origin useing ra/dec
	wxFlexGridSizer* sizerFieldPanelB = new wxFlexGridSizer( 2, 2, 3, 10 );
//	sizerFieldPanelB->AddGrowableCol( 0 );

	// layout of group of elements in the orig panel 
	m_pFieldBox = new wxStaticBox( this, -1, wxT("Field Size Hint"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* sizerFieldPage = new wxStaticBoxSizer( m_pFieldBox, wxHORIZONTAL );
	sizerFieldPage->Add( sizerFieldPanel, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	// create radio options for panels A & B 
	m_pFieldOptA = new wxRadioButton( this, wxID_RADIO_FIELD_A, wxT(""), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_pFieldOptB = new wxRadioButton( this, wxID_RADIO_FIELD_B, wxT("") );

	// add orig pannel a to the choice panel
	sizerFieldPanel->Add( m_pFieldOptA, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	sizerFieldPanel->Add( sizerFieldPanelA, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	sizerFieldPanel->Add( m_pFieldOptB, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	sizerFieldPanel->Add( sizerFieldPanelB, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	///////////////////////////////////////////
	// elements use hint head 
	// :: use image position hint
	if( nType == 0 )
	{
		m_pUseImgPosHint = new wxChoice( this, wxID_SELECT_USE_HINT,
										wxDefaultPosition, wxSize(-1,-1), 4, vectImgUsePosHint, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
		m_pUseImgPosHint->SetSelection(0);

		m_pUseGroupPosHint = NULL;
		m_pUseImgGroupPosHint = NULL;

		wxFlexGridSizer* sizerHeadChooser = new wxFlexGridSizer( 1, 2, 0, 0 );
		sizerHeadChooser->Add( new wxStaticText( this, -1, wxT("Use Hint:") ), 0,wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxRIGHT, 10 );
		sizerHeadChooser->Add( m_pUseImgPosHint, 0,wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
		topsizer->Add( sizerHeadChooser, 0,wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 5 );

	} else if( nType == 1 )
	{
		m_pUseGroupPosHint = new wxCheckBox( this, wxID_CHECK_GROUP_USE_HINT, wxT("Set Group Position Hint") );
		// :: use group position hint
		m_pUseImgGroupPosHint = new wxChoice( this, wxID_SELECT_IMG_USE_GROUP_HINT,
										wxDefaultPosition, wxSize(-1,-1), 3, vectImgUseGroupPosHint, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
		m_pUseImgGroupPosHint->SetSelection(0);
		// set image to null
		m_pUseImgPosHint = NULL;

		// add to sizer
		wxGridSizer* sizerHeadChooser = new wxGridSizer( 2, 1, 0, 0 );
		sizerHeadChooser->Add( m_pUseGroupPosHint, 0,wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 2 );
		wxFlexGridSizer* sizerHeadChooserSelect = new wxFlexGridSizer( 1, 2, 0, 0 );
		sizerHeadChooserSelect->Add( new wxStaticText( this, -1, wxT("Action:")), 0,wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2 );
		sizerHeadChooserSelect->Add( m_pUseImgGroupPosHint, 0,wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2 );
		sizerHeadChooser->Add( sizerHeadChooserSelect, 0,wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2 );
		topsizer->Add( sizerHeadChooser, 0,wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 5 );
	}

	///////////////////////////////////////////
	// elements definition origin by names and ids
	m_pObjectTypeLabel = new wxStaticText( this, -1, wxT("Object Reference From:"));
	m_pObjectType = new wxChoice( this, wxID_SELECT_ORIG_TYPE,
										wxDefaultPosition,
										wxSize(140,-1), 7, vectObjReference, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pObjectType->SetSelection( 0 );

	m_pObjectNameLabel = new wxStaticText( this, -1, wxT("Select Object Name:"));
	m_pObjectName = new wxChoice( this, wxID_SELECT_ORIG_NAME,
										wxDefaultPosition, wxSize(200,-1), 2, vectObjNameId, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pObjectName->SetSelection( 0 );
	// object id entry
	m_pObjectIdLabel = new wxStaticText( this, -1, wxT("Select Object Id:"));
	m_pObjectId = new wxTextCtrl( this, wxID_SELECT_ORIG_ID, wxT(""), wxDefaultPosition, wxSize(90,-1) );

	///////////////////////////////////////////
	// elements definition origin by ra dec coords
	m_pRaDecEntryTypeLabel = new wxStaticText( this, -1, wxT("Select Format:"));
	m_pRaDecEntryType = new wxChoice( this, wxID_SELECT_ORIG_ENTRY_TYPE,
										wxDefaultPosition, wxSize(90,-1), 2, vectRaDecEntryType, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pRaDecEntryType->SetSelection( 0 );

	// entry field for ra in deg
	m_pRaDecLabel = new wxStaticText( this, -1, wxT("Right Ascension/Declination:"));
	m_pRaEntry = new wxTextCtrl( this, wxID_SET_ORIG_RA_VALUE, wxT(""), wxDefaultPosition, wxSize(90,-1) );
	m_pDecEntry = new wxTextCtrl( this, wxID_SET_ORIG_DEC_VALUE, wxT(""), wxDefaultPosition, wxSize(90,-1) );

	///////////////////////////////////////////
	// elements definition for field size by camera and focal length
	m_pSetupLabel = new wxStaticText( this, -1, wxT("Hardware Setup:"));
	m_pSetupSelect = new wxChoice( this, wxID_SELECT_HARWARE_SETUP,
										wxDefaultPosition, wxSize(90,-1), 1, vectHardwareSetup, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pSetupSelect->SetSelection( 0 );

	m_pCameraTypeLabel = new wxStaticText( this, -1, wxT("Sensor Size(mm):"));
	m_pSensorWidth = new wxTextCtrl( this, wxID_SENSOR_WIDTH, wxT(""), wxDefaultPosition, wxSize(60,-1) );
	m_pSensorHeight = new wxTextCtrl( this, wxID_SENSOR_HEIGHT, wxT(""), wxDefaultPosition, wxSize(60,-1) );
	m_pCameraSelectButton = new wxButton( this, wxID_CAMERA_SELECT_BUTTON, wxT("Select"), wxPoint( -1, -1 ), wxSize( 40, -1 ) );

	// entry field for ra in deg
	m_pFocalLabel = new wxStaticText( this, -1, wxT("Focal Length(mm):"));
	m_pFocal = new wxTextCtrl( this, wxID_SET_FOCAL_VALUE, wxT("2000"), wxDefaultPosition, wxSize(70,-1) );
	m_pTLensSelectButton = new wxButton( this, wxID_TLENS_SELECT_BUTTON, wxT("Select"), wxPoint( -1, -1 ), wxSize( 40, -1 ) );

	///////////////////////////////////////////
	// elements definition for field size by camera and focal length
	m_pFieldFormatLabel = new wxStaticText( this, -1, wxT("Select Format:"));
	m_pFieldFormat = new wxChoice( this, wxID_SELECT_FIELD_FORMAT_TYPE,
										wxDefaultPosition,
										wxSize(90,-1), 2, vectFieldFormat, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pFieldFormat->SetSelection( 0 );

	// field size elements
	m_pFieldSizeLabel = new wxStaticText( this, -1, wxT("Field Size(width/height):"));
	m_pFieldWidth = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize(90,-1) );
	m_pFieldHeight = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize(90,-1) );

	//////////////////////////////////////////////////
	// populate origin A panel
	// :: object type
	sizerOrigAPanel->Add( m_pObjectTypeLabel, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerOrigAPanel->Add( m_pObjectType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: object name
	sizerOrigAPanel->Add( m_pObjectNameLabel, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerOrigAPanel->Add( m_pObjectName, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: object id
	sizerOrigAPanel->Add( m_pObjectIdLabel, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerOrigAPanel->Add( m_pObjectId, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// hide id entry
	m_pObjectIdLabel->Hide();
	m_pObjectId->Hide();

	//////////////////////////////////////////////////
	// populate origin B panel
	// ::ra dec entry type
	sizerOrigBPanel->Add( m_pRaDecEntryTypeLabel, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL ) ;
	sizerOrigBPanel->Add( m_pRaDecEntryType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: RA in deg
	sizerOrigBPanel->Add( m_pRaDecLabel, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	wxFlexGridSizer* sizerOrigRaDec = new wxFlexGridSizer( 1, 2, 2, 2 );
	sizerOrigRaDec->Add( m_pRaEntry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerOrigRaDec->Add( m_pDecEntry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerOrigBPanel->Add( sizerOrigRaDec, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	//////////////////////////////////////////////////
	// populate field size by camera type panel
	// row for setup
	sizerFieldPanelA->Add( m_pSetupLabel, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerFieldPanelA->Add( m_pSetupSelect, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	// row for camera type
	sizerFieldPanelA->Add( m_pCameraTypeLabel, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	wxFlexGridSizer* sizerFieldPanelACam = new wxFlexGridSizer( 1, 3, 2, 2 );
	sizerFieldPanelACam->Add( m_pSensorWidth, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerFieldPanelACam->Add( m_pSensorHeight, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerFieldPanelACam->Add( m_pCameraSelectButton, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxLEFT, 5  );
	sizerFieldPanelA->Add( sizerFieldPanelACam, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row for object name
	sizerFieldPanelA->Add( m_pFocalLabel, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	wxFlexGridSizer* sizerFieldPanelATLens = new wxFlexGridSizer( 1, 2, 2, 2 );
	sizerFieldPanelATLens->Add( m_pFocal, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerFieldPanelATLens->Add( m_pTLensSelectButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxLEFT, 5  );
	sizerFieldPanelA->Add( sizerFieldPanelATLens, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	//////////////////////////////////////////////////
	// populate field size by direct entry
	// :: field untis type
	sizerFieldPanelB->Add( m_pFieldFormatLabel, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerFieldPanelB->Add( m_pFieldFormat, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: field size 
	sizerFieldPanelB->Add( m_pFieldSizeLabel, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	wxFlexGridSizer* sizerFieldSize = new wxFlexGridSizer( 1, 2, 2, 2 );
	sizerFieldSize->Add( m_pFieldWidth, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerFieldSize->Add( m_pFieldHeight, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerFieldPanelB->Add( sizerFieldSize, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	// calculate sizers
	topsizer->Add( sizerOrigPage, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW | wxLEFT|wxRIGHT, 10 );
	topsizer->Add( sizerFieldPage, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW | wxLEFT|wxRIGHT, 10 );
	topsizer->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 10 );

//	pMainPanel->SetSizer( sizerMainPage );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
//	sizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CPosHintDlg
// Purpose:	destroy my  dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CPosHintDlg::~CPosHintDlg( ) 
{
	if( m_pUseImgPosHint ) delete( m_pUseImgPosHint );
	if( m_pUseGroupPosHint ) delete( m_pUseGroupPosHint );
	if( m_pUseImgGroupPosHint ) delete( m_pUseImgGroupPosHint );

	delete( m_pOrigOptA );
	delete( m_pOrigOptB );
	delete( m_pObjectType );
	delete( m_pObjectName );
	delete( m_pRaDecEntryType );
	delete( m_pObjectIdLabel );
	delete( m_pObjectId );

	delete( m_pObjectTypeLabel );
	delete( m_pObjectNameLabel );
	delete( m_pRaDecEntryTypeLabel );
	delete( m_pRaDecLabel );
	delete( m_pRaEntry );
	delete( m_pDecEntry );

	// setup
	delete( m_pSetupLabel );
	delete( m_pSetupSelect );
	// camera
	delete( m_pCameraTypeLabel );
	delete( m_pSensorWidth );
	delete( m_pSensorHeight );
	delete( m_pCameraSelectButton );
	// tlens
	delete( m_pFocalLabel );
	delete( m_pFocal );
	delete( m_pTLensSelectButton );
	// fields
	delete( m_pFieldFormatLabel );
	delete( m_pFieldFormat );
	delete( m_pFieldSizeLabel );
	delete( m_pFieldWidth );
	delete( m_pFieldHeight );

	delete( m_pFieldOptA );
	delete( m_pFieldOptB );

	delete( m_pOrigBox );
	delete( m_pFieldBox );

//	delete( sizerOrigAPanel );
}

////////////////////////////////////////////////////////////////////
// Method:	SetConfig
// Class:	CPosHintDlg
// Purpose:	Set and get my reference from the config object
// Input:	pointer to detect config and main config
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CPosHintDlg::SetConfig( CConfigMain* pMainConfig, CSky* pSky, 
								CAstroImage* pAstroImage, CObserver* pObserver, 
								CCamera* pCamera, CTelescope* pTelescope )
{
	m_pMainConfig = pMainConfig;
	m_pSky = pSky;
	m_pAstroImage = pAstroImage;
	m_pObserver = pObserver;
	m_pCamera = pCamera;
	m_pTelescope = pTelescope;

	int i=0, nVarInt=0;
	double nVarFloat=0.0;
	wxString strTmp;


	// now we check saved config in either main config or image
	if( pAstroImage->m_nUseHint )
	{
		m_pUseImgPosHint->SetSelection( pAstroImage->m_nUseHint );
		m_bUseHint = 1;

		// also set if to use group hint
//?use?	m_bUseGroupHint = pAstroImage->m_pImageGroup->m_bIsHint;
//		m_pUseGroupPosHint->SetValue( m_bUseGroupHint );

		// extract my params from image
		//SetObjectNameList( pAstroImage->m_nHintObjOrigType );
		m_pObjectType->SetSelection( pAstroImage->m_nHintObjOrigType );
		SetSelectOrigType( );
		m_pObjectName->SetSelection( pAstroImage->m_nHintObjName );
		m_pObjectId->SetValue( wxString(pAstroImage->m_strHintObjId,wxConvUTF8) );
		m_pRaDecEntryType->SetSelection( pAstroImage->m_nHintOrigUnits );
		m_nRaDeg = pAstroImage->m_nRelCenterRa;
		m_nDecDeg = pAstroImage->m_nRelCenterDec;
		if( pAstroImage->m_nHintOrigType == 0 )
		{
			m_pOrigOptA->SetValue( 1 );
			DisableOrigB(  );
		} else
		{
			m_pOrigOptB->SetValue( 1 );
			DisableOrigA(  );
		}

		// field
		if( pAstroImage->m_nHintFieldType == 0 )
		{
			// activate field option A
			m_pFieldOptA->SetValue( 1 );
			DisableFieldB(  );
			// 
		} else
		{
			// activate field option A
			m_pFieldOptB->SetValue( 1 );
			DisableFieldA(  );
		}

		// setups
		m_nSetupId = pAstroImage->m_nHintSetupId;
		m_pSetupSelect->Clear();
		m_pSetupSelect->Freeze();
		m_pSetupSelect->Append( wxT("None") );
		for( i=0; i<m_pObserver->m_pHardware->m_vectSetup.size(); i++ )
			m_pSetupSelect->Append( m_pObserver->m_pHardware->m_vectSetup[i].name );
		m_pSetupSelect->SetSelection( m_nSetupId );
		m_pSetupSelect->Thaw();

		// get camera
		m_nCameraSource = pAstroImage->m_nHintCameraSource;
		m_nCameraType = pAstroImage->m_nHintCameraType;
		m_nCameraBrand = pAstroImage->m_nHintCameraBrand;
		m_nCameraName = pAstroImage->m_nHintCameraName;

		// set sensor
		m_nSensorWidth = pAstroImage->m_nHintSensorWidth;
		strTmp.Printf( wxT("%.2f"), pAstroImage->m_nHintSensorWidth );
		m_pSensorWidth->ChangeValue( strTmp );
		m_nSensorHeight = pAstroImage->m_nHintSensorHeight;
		strTmp.Printf( wxT("%.2f"), pAstroImage->m_nHintSensorHeight );
		m_pSensorHeight->ChangeValue( strTmp );
	
		// get tlens
		m_nTLensSource = pAstroImage->m_nHintTLensSource;
		memcpy( &m_rLensSetup, &(pAstroImage->m_rHintTLensSetup), sizeof(DefTLensSetup) );
		// copy name to own - if source
		if( m_nTLensSource == 1 )
		{
			m_rLensSetup.nLensNameOwn = m_rLensSetup.nLensName;
			m_rLensSetup.nBarlowLensNameOwn = m_rLensSetup.nBarlowLensName;
			m_rLensSetup.nFocalReducerNameOwn = m_rLensSetup.nFocalReducerName;
			m_rLensSetup.nEyepieceNameOwn = m_rLensSetup.nEyepieceName;
			m_rLensSetup.nCameraLensNameOwn = m_rLensSetup.nCameraLensName;
		}

		// set focal
		strTmp.Printf( wxT("%.2f"), pAstroImage->m_nHintFocalLength );
		m_pFocal->SetValue( strTmp );
		m_pFieldFormat->SetSelection( pAstroImage->m_nHintFieldFormat );
		m_nFieldX = pAstroImage->m_nRelFieldRa;
		m_nFieldY = pAstroImage->m_nRelFieldDec;

	} else
	{
		// set default data
		SetDefaultData( );

		// disable all for start
		DisableAll( );
	}

	SetOrig( );
	// se my camera values using the selection -- to do here using new stuff 
//	SetCameraValues( );
	// set focal
	SetFocalLength( );
	// calculate field values
	if( m_pFieldOptA->GetValue() ) CalculateFieldValues( );
	// set field values
	SetField( );
	

	// set the list of object names
//	SetObjectNameList( ORIGIN_LIST_TYPE_STAR_NAMES );
	// activate orig option A
//	m_pOrigOptA->SetValue( 1 );
//	DisableOrigB(  );
	// activate field option A
//	m_pFieldOptA->SetValue( 1 );
//	DisableFieldB(  );

	// disable all for start
//	DisableAll( );

	GetSizer()->Layout();
	Layout();
	Fit( );
	Refresh( false );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetGroupConfig
// Class:	CPosHintDlg
// Purpose:	Set and get my reference from the config object
// Input:	pointer to detect config and main config
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CPosHintDlg::SetGroupConfig( CConfigMain* pMainConfig, CSky* pSky, 
								CImageGroup* pImageGroup, CObserver* pObserver, 
								CCamera* pCamera, CTelescope* pTelescope )
{
	m_pMainConfig = pMainConfig;
	m_pSky = pSky;
	m_pObserver = pObserver;
	m_pCamera = pCamera;
	m_pTelescope = pTelescope;

	int i=0, nVarInt = 0;
	double nVarFloat = 0;
	wxString strTmp;

	// now we check saved config in either main config or image
	if( pImageGroup->m_bIsHint )
	{
		m_pUseGroupPosHint->SetValue( 1 );
		m_bUseGroupHint = 1;

		m_bUseHint = 1; // do I use this ???? this was meant to be used with image only ?

		// extract my params from image
		//SetObjectNameList( pAstroImage->m_nHintObjOrigType );
		m_pObjectType->SetSelection( pImageGroup->m_nHintObjOrigType );
		SetSelectOrigType( );
		m_pObjectName->SetSelection( pImageGroup->m_nHintObjName );
		m_pObjectId->SetValue( wxString(pImageGroup->m_strHintObjId,wxConvUTF8) );
		m_pRaDecEntryType->SetSelection( pImageGroup->m_nHintOrigUnits );
		m_nRaDeg = pImageGroup->m_nRelCenterRa;
		m_nDecDeg = pImageGroup->m_nRelCenterDec;
		if( pImageGroup->m_nHintOrigType == 0 )
		{
			m_pOrigOptA->SetValue( 1 );
			DisableOrigB(  );
		} else
		{
			m_pOrigOptB->SetValue( 1 );
			DisableOrigA(  );
		}

		// field
		if( pImageGroup->m_nHintFieldType == 0 )
		{
			// activate field option A
			m_pFieldOptA->SetValue( 1 );
			DisableFieldB(  );
		} else
		{
			// activate field option A
			m_pFieldOptB->SetValue( 1 );
			DisableFieldA(  );
		}

		// setups
		m_nSetupId = pImageGroup->m_nHintSetupId;
		m_pSetupSelect->Clear();
		m_pSetupSelect->Freeze();
		m_pSetupSelect->Append( wxT("None") );
		for( i=0; i<m_pObserver->m_pHardware->m_vectSetup.size(); i++ )
			m_pSetupSelect->Append( m_pObserver->m_pHardware->m_vectSetup[i].name );
		m_pSetupSelect->SetSelection( m_nSetupId );
		m_pSetupSelect->Thaw();

		// get camera
		m_nCameraSource = pImageGroup->m_nHintCameraSource;
		m_nCameraType = pImageGroup->m_nHintCameraType;
		m_nCameraBrand = pImageGroup->m_nHintCameraBrand;
		m_nCameraName = pImageGroup->m_nHintCameraName;

		// set sensor
		m_nSensorWidth = pImageGroup->m_nHintSensorWidth;
		strTmp.Printf( wxT("%.2f"), pImageGroup->m_nHintSensorWidth );
		m_pSensorWidth->ChangeValue( strTmp );
		m_nSensorHeight = pImageGroup->m_nHintSensorHeight;
		strTmp.Printf( wxT("%.2f"), pImageGroup->m_nHintSensorHeight );
		m_pSensorHeight->ChangeValue( strTmp );
	
		// get tlens
		m_nTLensSource = pImageGroup->m_nHintTLensSource;
		memcpy( &m_rLensSetup, &(pImageGroup->m_rHintTLensSetup), sizeof(DefTLensSetup) );
		// copy name to own - if source
		if( m_nTLensSource == 1 )
		{
			m_rLensSetup.nLensNameOwn = m_rLensSetup.nLensName;
			m_rLensSetup.nBarlowLensNameOwn = m_rLensSetup.nBarlowLensName;
			m_rLensSetup.nFocalReducerNameOwn = m_rLensSetup.nFocalReducerName;
			m_rLensSetup.nEyepieceNameOwn = m_rLensSetup.nEyepieceName;
			m_rLensSetup.nCameraLensNameOwn = m_rLensSetup.nCameraLensName;
		}

		// set focal
		strTmp.Printf( wxT("%.2f"), pImageGroup->m_nHintFocalLength );
		m_pFocal->SetValue( strTmp );
		m_pFieldFormat->SetSelection( pImageGroup->m_nHintFieldFormat );
		m_nFieldX = pImageGroup->m_nRelFieldRa;
		m_nFieldY = pImageGroup->m_nRelFieldDec;

	} else
	{
		// set default data
		SetDefaultData( );

		// disable all for start
		DisableAll( );
	}

	SetOrig( );
	// se my camera values using the selection
//	SetCameraValues( );
	// set focal
	SetFocalLength( );
	// calculate field values
	if( m_pFieldOptA->GetValue() ) CalculateFieldValues( );
	// set field values
	SetField( );

	GetSizer()->Layout();
	Layout();
	Fit( );
	Refresh( false );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetDefaultData
// Class:	CPosHintDlg
// Purpose:	set default data
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CPosHintDlg::SetDefaultData( )
{
	int nVarInt = 0;
	double nVarFloat = 0;
	wxString strTmp;

	if( m_pUseGroupPosHint ) m_pUseGroupPosHint->SetValue( 0 );
	if( m_pUseImgGroupPosHint ) m_pUseImgGroupPosHint->SetSelection( 0 );
	m_bUseHint = 0;

	SetObjectNameList( ORIGIN_LIST_TYPE_STAR_NAMES );
//		SetSelectOrigType( );
	m_pOrigOptA->SetValue( 1 );
	DisableOrigB(  );

	// get only the field size settings from main config
	// get the field type entry
	if( m_pMainConfig->GetIntVar( SECTION_ID_HINT, CONF_HINT_USE_FIELD_TYPE, nVarInt ) != 0 )
	{
		if( nVarInt == 0 )
		{
			// activate field option A
			m_pFieldOptA->SetValue( 1 );
			DisableFieldB(  );
		} else
		{
			// activate field option A
			m_pFieldOptB->SetValue( 1 );
			DisableFieldA(  );
		}

	} else
	{
		// activate field option A
		m_pFieldOptA->SetValue( 1 );
		DisableFieldB(  );

	}

	// sensor width
	if( m_pMainConfig->GetDoubleVar( SECTION_ID_HINT, CONF_HINT_SENSOR_WIDTH, nVarFloat ) != 0 )
	{
		strTmp.Printf( wxT("%.2f"), nVarFloat );
		m_pSensorWidth->SetValue( strTmp );

	} else
	{
		m_pSensorWidth->SetValue( wxT("22.5") );
	}
	// sensor height
	if( m_pMainConfig->GetDoubleVar( SECTION_ID_HINT, CONF_HINT_SENSOR_HEIGHT, nVarFloat ) != 0 )
	{
		strTmp.Printf( wxT("%.2f"), nVarFloat );
		m_pSensorHeight->SetValue( strTmp );

	} else
	{
		m_pSensorHeight->SetValue( wxT("15.0") );
	}

	// get focal length
	if( m_pMainConfig->GetDoubleVar( SECTION_ID_HINT, CONF_HINT_FOCAL_LENGTH, nVarFloat ) != 0 )
	{
		strTmp.Printf( wxT("%.2f"), nVarFloat );
		m_pFocal->SetValue( strTmp );
	} else
	{
		m_pFocal->SetValue( wxT("2000.00") );
	}
	// get type of units to display
	if( m_pMainConfig->GetIntVar( SECTION_ID_HINT, CONF_HINT_FIELD_UNITS, nVarInt ) != 0 )
	{
		m_pFieldFormat->SetSelection( nVarInt );

	} else
	{
		m_pFieldFormat->SetSelection( 0 );
	}
	// get field width
	if( m_pMainConfig->GetDoubleVar( SECTION_ID_HINT, CONF_HINT_FIELD_WIDTH, nVarFloat ) != 0 )
	{
//			sprintf( strTmp, "%f", nVarFloat );
//			m_pFieldX->SetValue( strTmp );
		m_nFieldX = nVarFloat;
	} else
	{
//			m_pFieldX->SetValue( "00" );
		m_nFieldX = nVarFloat;
	}
	// get field height
	if( m_pMainConfig->GetDoubleVar( SECTION_ID_HINT, CONF_HINT_FIELD_HEIGHT, nVarFloat ) != 0 )
	{
//			sprintf( strTmp, "%f", nVarFloat );
//			m_pFieldY->SetValue( strTmp );
		m_nFieldY = nVarFloat;
	} else
	{
//			m_pFieldY->SetValue( "00" );
		m_nFieldY = 0;
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnUseHint
// Class:		CPosHintDlg
// Purpose:		activate deactivate the use of hint entries
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CPosHintDlg::OnUseHint( wxCommandEvent& pEvent )
{
	int nHint = m_pUseImgPosHint->GetSelection();

	if( nHint )
	{
		EnableAll( );

		// activate orig option A
		m_pOrigOptA->SetValue( 1 );
		DisableOrigB(  );
		// activate field option A
		m_pFieldOptA->SetValue( 1 );
		DisableFieldB(  );

		m_bUseHint = 1;

	} else
	{
		DisableAll( );
		m_bUseHint = 0;
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnUseGroupHint
// Class:		CPosHintDlg
// Purpose:		activate/deactivate if to use group hint
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CPosHintDlg::OnUseGroupHint( wxCommandEvent& pEvent )
{
	int nCheck = pEvent.IsChecked( );

	if( nCheck )
	{
		EnableAll( );

		// activate orig option A
		m_pOrigOptA->SetValue( 1 );
		DisableOrigB(  );
		// activate field option A
		m_pFieldOptA->SetValue( 1 );
		DisableFieldB(  );

		m_bUseGroupHint = 1;

	} else
	{
		DisableAll( );
		m_bUseGroupHint = 0;
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetObjectNameList
// Class:	CPosHintDlg
// Purpose:	Set and get my reference from the config object
// Input:	pointer to detect config and main config
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CPosHintDlg::SetObjectNameList( int nType )
{
	int i = 0;

	if( nType == ORIGIN_LIST_TYPE_DSO_NAMES )
	{
		wxArrayString vecObj;
		vecObj.Alloc( m_pSky->m_pCatalogDsoNames->m_nDsoName+1 );

		// populate list from catalog entries
		for( i=0; i<m_pSky->m_pCatalogDsoNames->m_nDsoName; i++ )
		{
			vecObj.Add( wxString(m_pSky->m_pCatalogDsoNames->m_vectDsoName[i].name,wxConvUTF8) );
			//m_pObjectName->Append( m_pCatalog->m_vectDsoName[i].name );
		}
		m_pObjectName->Freeze( );
		m_pObjectName->Clear( );
//		m_pObjectName->Disable();
		m_pObjectName->Append( vecObj );
//		m_pObjectName->Enable();
		m_pObjectName->Thaw( );
		m_nOrigType = ORIGIN_LIST_TYPE_DSO_NAMES;

	} else if( nType == ORIGIN_LIST_TYPE_STAR_NAMES )
	{
		wxArrayString vecObj;
		vecObj.Alloc( m_pSky->m_pCatalogStarNames->m_nStarName+1 );

		// populate star names list from catalog entries
		for( i=0; i<m_pSky->m_pCatalogStarNames->m_nStarName; i++ )
		{
			vecObj.Add( wxString(m_pSky->m_pCatalogStarNames->m_vectStarName[i].name,wxConvUTF8) );
		}
		m_pObjectName->Freeze( );
		m_pObjectName->Clear( );
		m_pObjectName->Append( vecObj );
		m_pObjectName->Thaw( );
		m_nOrigType = ORIGIN_LIST_TYPE_STAR_NAMES;

	} else if( nType == ORIGIN_LIST_TYPE_CONST_NAMES )
	{
		wxArrayString vecObj;
		vecObj.Alloc( m_pSky->m_pConstellations->m_nConstName+1 );

		// populate constelattions names list from catalog entries
		for( i=0; i<m_pSky->m_pConstellations->m_nConstName; i++ )
		{
			vecObj.Add( wxString(m_pSky->m_pConstellations->m_vectConstName[i].name,wxConvUTF8) );
		}
		m_pObjectName->Freeze( );
		m_pObjectName->Clear( );
		m_pObjectName->Append( vecObj );
		m_pObjectName->Thaw( );

		m_nOrigType = ORIGIN_LIST_TYPE_CONST_NAMES;

	} 

	return;
}

// Purpose:	extract focal length from entry and set it
////////////////////////////////////////////////////////////////////
void CPosHintDlg::SetFocalLength( )
{
	if( !m_pFocal->GetLineText(0).ToDouble( &m_nFocalLength ) )
		m_nFocalLength = 0;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnSelectOrigType
// Class:		CPosHintDlg
// Purpose:		select object origination type
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CPosHintDlg::OnSelectOrigType( wxCommandEvent& pEvent )
{

	SetSelectOrigType(  );

	// reset layout
	sizerOrigAPanel->Layout( );
	GetSizer()->Layout( );
	Fit( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		SetSelectOrigType
// Class:		CPosHintDlg
// Purpose:		set orig fields based on a type
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CPosHintDlg::SetSelectOrigType( )
{
	int nSelect = m_pObjectType->GetSelection();

	if( nSelect == ORIGIN_LIST_TYPE_DSO_NAMES ||
		nSelect == ORIGIN_LIST_TYPE_STAR_NAMES ||
		nSelect == ORIGIN_LIST_TYPE_CONST_NAMES )
	{
		// hide id entry
		m_pObjectIdLabel->Hide();
		m_pObjectId->Hide();
		// show name entry
		m_pObjectNameLabel->Show();
		m_pObjectName->Show();

		// and set the list of object in the next select
		SetObjectNameList( nSelect );

	} else if( nSelect == ORIGIN_LIST_TYPE_MESSIER_ID ||
				nSelect == ORIGIN_LIST_TYPE_NGC_ID ||
				nSelect == ORIGIN_LIST_TYPE_IC_ID ||
				nSelect == ORIGIN_LIST_TYPE_SAO_ID )
	{
		// show id entry
		m_pObjectIdLabel->Show();
		m_pObjectId->Show();
		// hide Name entry
		m_pObjectNameLabel->Hide();
		m_pObjectName->Hide();
		//special case
		if( nSelect == ORIGIN_LIST_TYPE_SAO_ID )
		{
			m_pSky->m_pCatalogStars->LoadCatalog( CATALOG_ID_SAO ); 
		}
	}

	return;
}

///////////////////////////////////////////////////////////////////
// Method:		OnSetObjectId
// Class:		CPosHintDlg
// Purpose:		when text in the object id was chernaged find object
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CPosHintDlg::OnSetObjectId( wxCommandEvent& pEvent )
{
	if( !m_pObjectId ) return;
	int nSelect = m_pObjectType->GetSelection();
	wxString strNo = m_pObjectId->GetLineText(0).Trim(0).Trim(1);
	if( strNo.IsEmpty() ) return;

	unsigned long nCatNo = 0;
	if( !strNo.ToULong( &nCatNo ) )
	{
		wxMessageBox( wxT("Only digits [0-9] accepted!"), wxT("Invalid Entry!"), wxOK|wxICON_ERROR );
		return;
	}

	if( nSelect == ORIGIN_LIST_TYPE_NGC_ID )
	{
		m_pSky->m_pCatalogDsoNgc->GetRaDec( nCatNo, m_nRaDeg, m_nDecDeg );

	} else if( nSelect == ORIGIN_LIST_TYPE_IC_ID )
	{
		m_pSky->m_pCatalogDsoIc->GetRaDec( nCatNo, m_nRaDeg, m_nDecDeg );

	} else if( nSelect == ORIGIN_LIST_TYPE_MESSIER_ID )
	{
		m_pSky->m_pCatalogDsoMessier->GetRaDec( nCatNo, m_nRaDeg, m_nDecDeg );
	
	} else if( nSelect == ORIGIN_LIST_TYPE_SAO_ID )
	{
		m_pSky->m_pCatalogStars->GetStarRaDec( nCatNo, m_nRaDeg, m_nDecDeg );
	}


	// and now set orig
	SetOrig( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		UpdateRaDecValues
// Class:		CPosHintDlg
// Purpose:		update RA value
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CPosHintDlg::UpdateRaDecValues( int nType )
{
	int nSelect = m_pRaDecEntryType->GetSelection();
	// if is not negative the we use type value useful for conversion update fro
	// a format in another - in this case type will be the previous format 
	if( nType >= 0 ) nSelect = nType;

	wxString strRa = m_pRaEntry->GetLineText(0);
	wxString strDec = m_pDecEntry->GetLineText(0);

	if( nSelect == ORIGIN_COORD_ENTRY_TYPE_DEG )
	{
		// just set the ra directly from deg value in the box
//		if( m_pRaEntry->IsModified() )
			if( !strRa.ToDouble( &m_nRaDeg ) )
				m_nRaDeg = 0;
		// set dec directly from deg value in the box
//		if( m_pDecEntry->IsModified() )
			if( !strDec.ToDouble( &m_nDecDeg ) )
				m_nDecDeg = 0;

	} else
	{
		// we need to convert this from sexagesimal base
//		if( m_pRaEntry->IsModified() )
			if( !ConvertRaToDeg( strRa, m_nRaDeg ) )
				m_nRaDeg = 0;
		// convert dec from sexagesimal base
//		if( m_pDecEntry->IsModified() )
			if( !ConvertDecToDeg( strDec, m_nDecDeg ) )
				m_nDecDeg = 0;
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnSelectOrigName
// Class:		CPosHintDlg
// Purpose:		select object origination name
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CPosHintDlg::OnSelectOrigName( wxCommandEvent& pEvent )
{
	m_nRaDeg=0; 
	m_nDecDeg=0;

	int nSelect = pEvent.GetSelection( );

	if( m_nOrigType == ORIGIN_LIST_TYPE_STAR_NAMES )
	{
		//m_nRaDeg = m_pCatalog->m_vectStarName[nSelect].ra;
		//m_nDecDeg = m_pCatalog->m_vectStarName[nSelect].dec;
		m_pSky->m_pCatalogStarNames->GetNameRaDec( nSelect, m_nRaDeg, m_nDecDeg );

	} else if( m_nOrigType == ORIGIN_LIST_TYPE_DSO_NAMES )
	{
		m_pSky->m_pCatalogDsoNames->GetDsoNameRaDec( nSelect, m_nRaDeg, m_nDecDeg );

	} else if( m_nOrigType == ORIGIN_LIST_TYPE_CONST_NAMES )
	{
		m_nRaDeg = m_pSky->m_pConstellations->m_vectConstName[nSelect].ra;
		m_nDecDeg = m_pSky->m_pConstellations->m_vectConstName[nSelect].dec;
	}

	// display by selected format 
	SetOrig( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		SetOrig
// Class:		CPosHintDlg
// Purpose:		set orig coord by format type
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CPosHintDlg::SetOrig( )
{
	wxString strTmp;

	// get selected format 
	int nFormat = m_pRaDecEntryType->GetSelection();
	// write the entries
	if( nFormat == 0 )
	{
		// set ra
		RaDegToSexagesimal( m_nRaDeg, strTmp );
		m_pRaEntry->SetLabel( strTmp );

		// set dec
		DecDegToSexagesimal( m_nDecDeg, strTmp );
		m_pDecEntry->SetLabel( strTmp );

	} else
	{
		strTmp.Printf( wxT("%.12f"), m_nRaDeg );
		m_pRaEntry->SetValue( strTmp );
		strTmp.Printf( wxT("%.8f"), m_nDecDeg );
		m_pDecEntry->SetValue( strTmp );
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnEntryOrigType
// Class:		CPosHintDlg
// Purpose:		select entry type between decimal and sexade
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CPosHintDlg::OnEntryOrigType( wxCommandEvent& pEvent )
{
	// update values in decimal if modified
	if( m_pRaDecEntryType->GetSelection() == 0 )
		UpdateRaDecValues( 1 );
	else
		UpdateRaDecValues( 0 );

	// and set the fields to the format
	SetOrig( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnRadioOrigEntry
// Class:		CPosHintDlg
// Purpose:		select orig entry format
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CPosHintDlg::OnRadioOrigEntry( wxCommandEvent& pEvent )
{
	if( m_pOrigOptA->GetValue() )
	{
		DisableOrigB( );
		EnableOrigA( );

	} else if( m_pOrigOptB->GetValue() )
	{
		DisableOrigA( );
		EnableOrigB( );
	}

	return;
}

void CPosHintDlg::EnableOrigA(  )
{
	m_pObjectTypeLabel->Enable();
	m_pObjectNameLabel->Enable();
	m_pObjectType->Enable();
	m_pObjectName->Enable();
	m_pObjectIdLabel->Enable();
	m_pObjectId->Enable();
}

void CPosHintDlg::EnableOrigB(  )
{
	m_pRaDecEntryTypeLabel->Enable();
	m_pRaDecEntryType->Enable();
	m_pRaDecLabel->Enable();
	m_pRaEntry->Enable();
	m_pDecEntry->Enable();
}

void CPosHintDlg::DisableOrigA(  )
{
	m_pObjectTypeLabel->Disable();
	m_pObjectNameLabel->Disable();
	m_pObjectType->Disable();
	m_pObjectName->Disable();
	m_pObjectIdLabel->Disable();
	m_pObjectId->Disable();

}

void CPosHintDlg::DisableOrigB(  )
{
	m_pRaDecEntryTypeLabel->Disable();
	m_pRaDecEntryType->Disable();
	m_pRaDecLabel->Disable();
	m_pRaEntry->Disable();
	m_pDecEntry->Disable();
}

////////////////////////////////////////////////////////////////////
// Method:		OnRadioFieldEntry
// Class:		CPosHintDlg
// Purpose:		select field entry format
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CPosHintDlg::OnRadioFieldEntry( wxCommandEvent& pEvent )
{
	if( m_pFieldOptA->GetValue() )
	{
		DisableFieldB( );
		EnableFieldA( );

	} else if( m_pFieldOptB->GetValue() )
	{
		DisableFieldA( );
		EnableFieldB( );
	}

	return;
}

void CPosHintDlg::EnableFieldA( )
{
	m_pSetupLabel->Enable();
	m_pSetupSelect->Enable();
	m_pCameraTypeLabel->Enable();
	m_pSensorWidth->Enable();
	m_pSensorHeight->Enable();
	m_pCameraSelectButton->Enable();
	m_pFocalLabel->Enable();
	m_pFocal->Enable();
	m_pTLensSelectButton->Enable();
}

void CPosHintDlg::EnableFieldB( )
{
	m_pFieldFormatLabel->Enable();
	m_pFieldFormat->Enable();
	m_pFieldSizeLabel->Enable();
	m_pFieldWidth->Enable();
	m_pFieldHeight->Enable();
}

void CPosHintDlg::DisableFieldA( )
{
	m_pSetupLabel->Disable();
	m_pSetupSelect->Disable();
	m_pCameraTypeLabel->Disable();
	m_pSensorWidth->Disable();
	m_pSensorHeight->Disable();
	m_pCameraSelectButton->Disable();
	m_pFocalLabel->Disable();
	m_pFocal->Disable();
	m_pTLensSelectButton->Disable();	
}

void CPosHintDlg::DisableFieldB( )
{
	m_pFieldFormatLabel->Disable();
	m_pFieldFormat->Disable();
	m_pFieldSizeLabel->Disable();
	m_pFieldWidth->Disable();
	m_pFieldHeight->Disable();
}

void CPosHintDlg::EnableAll( )
{
	m_pOrigOptA->Enable( );
	m_pOrigOptB->Enable( );
	m_pFieldOptA->Enable( );
	m_pFieldOptB->Enable( );
	m_pOrigBox->Enable( );
	m_pFieldBox->Enable( );

	EnableOrigA( );
	EnableOrigB( );
	EnableFieldA( );
	EnableFieldB( );
}

void CPosHintDlg::DisableAll( )
{
	m_pOrigOptA->Disable( );
	m_pOrigOptB->Disable( );
	m_pFieldOptA->Disable( );
	m_pFieldOptB->Disable( );
	m_pOrigBox->Disable( );
	m_pFieldBox->Disable( );

	DisableOrigA( );
	DisableOrigB( );
	DisableFieldA( );
	DisableFieldB( );
}

////////////////////////////////////////////////////////////////////
// Method:		OnFieldFormat
// Class:		CPosHintDlg
// Purpose:		select/set efield format 
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CPosHintDlg::OnFieldFormat( wxCommandEvent& pEvent )
{
	if( m_pFieldFormat->GetSelection() == 0 )
		UpdateFieldValues( 1 );
	else
		UpdateFieldValues( 0 );

	SetField( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnSetSensorSize
// Class:		CPosHintDlg
// Purpose:		on change sensor values
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CPosHintDlg::OnSetSensorSize( wxCommandEvent& event )
{
	// get sensor size from text entries
	if( !m_pSensorWidth->GetLineText(0).ToDouble( &m_nSensorWidth ) ) m_nSensorWidth = 0.0;
	if( !m_pSensorHeight->GetLineText(0).ToDouble( &m_nSensorHeight ) ) m_nSensorHeight = 0.0;

	// calculate field values
	CalculateFieldValues( );
	// set field values
	SetField( );

	return;
}

////////////////////////////////////////////////////////////////////
void CPosHintDlg::OnSelectCamera( wxCommandEvent& pEvent )
{
	wxString strTmp;

	CHardwareSelectDlg *pSelectCamera = new CHardwareSelectDlg( this, wxT("Select Camera"), wxT("Camera") );
	pSelectCamera->SetData( m_pObserver, m_pCamera, 0, m_nCameraSource, m_nCameraType, m_nCameraBrand, m_nCameraName );
	if( pSelectCamera->ShowModal( ) == wxID_OK )
	{
		m_nCameraSource = pSelectCamera->m_nSourceId;
		// check type of source all=0, own = 1
		if( m_nCameraSource == 0 )
		{
			m_nCameraType = pSelectCamera->m_pTypeSelect->GetSelection();
			m_nCameraBrand = pSelectCamera->GetBrandId();
			m_nCameraName = pSelectCamera->GetNameId();

			if( pSelectCamera->m_vectBrandId.size() == 0 || pSelectCamera->m_vectNameId.size() == 0 )
			{
				m_nSensorWidth = 0.0;
				m_nSensorHeight = 0.0;
				m_nPixelSize = 0.0;

			} else
			{
				// get values from def vector
				m_nSensorWidth = m_pCamera->m_vectCameraBrandDef[m_nCameraBrand].vectData[m_nCameraName].sensor_width;
				m_nSensorHeight = m_pCamera->m_vectCameraBrandDef[m_nCameraBrand].vectData[m_nCameraName].sensor_height;
				m_nPixelSize = m_pCamera->m_vectCameraBrandDef[m_nCameraBrand].vectData[m_nCameraName].sensor_psize;
			}

		} else if( m_nCameraSource == 1 )
		{
			m_nCameraName = pSelectCamera->m_pNameSelect->GetSelection();
			m_nSensorWidth = m_pObserver->m_pHardware->m_vectCamera[m_nCameraName].sensor_width;
			m_nSensorHeight = m_pObserver->m_pHardware->m_vectCamera[m_nCameraName].sensor_height;
			m_nPixelSize = m_pObserver->m_pHardware->m_vectCamera[m_nCameraName].sensor_psize;
		}
		// set gui
        strTmp.Printf( wxT("%.2f"), m_nSensorWidth  );
		m_pSensorWidth->ChangeValue( strTmp );
		strTmp.Printf( wxT("%.2f"), m_nSensorHeight  );
		m_pSensorHeight->ChangeValue( strTmp );

		// do the math :: calculate field values
		CalculateFieldValues( );
		
		// set field values
		SetField( );

		// Also set camera name in main properties
		m_pAstroImage->m_strCameraName = m_pCamera->m_vectCameraBrandDef[m_nCameraBrand].vectData[m_nCameraName].name;
	}
	delete( pSelectCamera );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		CalculateFieldValues
// Class:		CPosHintDlg
// Purpose:		calculate field values
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CPosHintDlg::CalculateFieldValues( )
{
	m_pSky->CalcFOVByInstr( m_nFieldX, m_nFieldY, m_nFocalLength, m_nSensorWidth, m_nSensorHeight );

/*
	double nAngSize = 0;
	// width
	nAngSize = (double) 2 * atan( (double) m_nSensorWidth / (double) (2 * m_nFocalLength ) );
	m_nFieldX = (double) (57.3 * nAngSize);
	// height
	nAngSize = (double) 2 * atan( (double) m_nSensorHeight / (double) (2 * m_nFocalLength ) );
	m_nFieldY = (double) (57.3 * nAngSize);

	return;
*/
}

////////////////////////////////////////////////////////////////////
// Method:		UpdateFieldValues
// Class:		CPosHintDlg
// Purpose:		update field values
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CPosHintDlg::UpdateFieldValues( int nType )
{
	// this is where we check if the format was modified
	int nFieldFormat = m_pFieldFormat->GetSelection();
	// if is not negative the we use type value useful for conversion update fro
	// a format in another - in this case type will be the previous format 
	if( nType >= 0 ) nFieldFormat = nType;

	wxString strFieldX = m_pFieldWidth->GetLineText(0);
	wxString strFieldY = m_pFieldHeight->GetLineText(0);
	
	if( nFieldFormat == FIELD_SIZE_ENTRY_TYPE_DEG )
	{
		// check to see if field x was modified
//		if( m_pFieldWidth->IsModified() )
			if( !strFieldX.ToDouble( &m_nFieldX ) )
				m_nFieldX = 0;
		// check to see if field y was modified
//		if( m_pFieldHeight->IsModified() )
			if( !strFieldY.ToDouble( &m_nFieldY ) )
				m_nFieldY = 0;

	} else
	{
		// check to see if field x was modified
//		if( m_pFieldWidth->IsModified() )
			if( !ConvertDecToDeg( strFieldX, m_nFieldX ) )
				m_nFieldX = 0;
		// check to see if field y was modified
//		if( m_pFieldHeight->IsModified() )
			if( !ConvertDecToDeg( strFieldY, m_nFieldY ) )
				m_nFieldY = 0;
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		SetField
// Class:		CPosHintDlg
// Purpose:		set field size by format type
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CPosHintDlg::SetField( )
{
	wxString strTmp;

	// get selected format 
	int nFormat = m_pFieldFormat->GetSelection();
	// write the entries
	if( nFormat == 0 )
	{
		double ival = 0;
		long nLVal = 0;

		ival = m_nFieldX;
	
		nLVal = (long) ( ival*(60. * 60. * 1000. + 0.001 ) ) ; //+ 0.5; // +5 or +50 ?
        strTmp.Printf( wxT("%02ld:%02ld:%02ld.%03ld"),  nLVal / 3600000L,
				(nLVal / 60000) % 60L, (nLVal / 1000) % 60L, nLVal % 1000L );
		m_pFieldWidth->SetValue( strTmp );

		// now do y
		ival = m_nFieldY;
		
		nLVal = (long) ( ival*(60. * 60. * 1000. + 0.001 ) ) ; //+ 0.5; // +5 or +50 ?
        strTmp.Printf( wxT("%02ld:%02ld:%02ld.%03ld"),  nLVal / 3600000L,
				(nLVal / 60000) % 60L, (nLVal / 1000) % 60L, nLVal % 1000L );
		m_pFieldHeight->SetValue( strTmp );

	} else
	{
		strTmp.Printf( wxT("%.5f"), m_nFieldX );
		m_pFieldWidth->SetValue( strTmp );
		strTmp.Printf( wxT("%.5f"), m_nFieldY );
		m_pFieldHeight->SetValue( strTmp );
	}

	return;
}

////////////////////////////////////////////////////////////////////
void CPosHintDlg::OnSelectTLens( wxCommandEvent& pEvent )
{
	wxString strTmp;

	CTLensSelectDlg *pSelectTLens = new CTLensSelectDlg( this, wxT("Select Lens System") );
	pSelectTLens->SetData( m_pObserver, m_pCamera, m_pTelescope, m_nTLensSource, m_rLensSetup );
	if( pSelectTLens->ShowModal( ) == wxID_OK )
	{
		m_nTLensSource = pSelectTLens->m_nSourceId;
		pSelectTLens->GetAllFields();
		memcpy( &m_rLensSetup, &(pSelectTLens->m_rLensSetup), sizeof(DefTLensSetup) );

		// calculate system focal length
		double nFocal = pSelectTLens->CalculateFocalLength( m_rLensSetup );
		// set focal length
		strTmp.Printf( wxT("%.2lf"), nFocal );
		m_pFocal->ChangeValue( strTmp );

		// do the math :: calculate field values
		CalculateFieldValues( );
		// set field values
		SetField( );

		// also select telescope name in main properties - todo: move this in hardware/telescope/camera classes
		if( m_rLensSetup.nLensType == 0 && m_pTelescope->IsTelescopeDef(m_rLensSetup.nLensBrand,m_rLensSetup.nLensName) )
		{
			if( m_nTLensSource == 0 )
				m_pAstroImage->m_strTelescopeName = m_pTelescope->m_vectTelescopeBrandDef[m_rLensSetup.nLensBrand].vectData[m_rLensSetup.nLensName].name;
			else if( m_nTLensSource == 1 )
				m_pAstroImage->m_strTelescopeName = m_pObserver->m_pHardware->m_vectTelescope[m_rLensSetup.nLensNameOwn].name;

		} else if( m_rLensSetup.nLensType == 1 && m_pCamera->IsCameraLensDef(m_rLensSetup.nLensBrand,m_rLensSetup.nLensName) )
		{
			if( m_nTLensSource == 0 && m_pCamera->IsCameraLensDef(m_rLensSetup.nLensBrand,m_rLensSetup.nLensName) )
				m_pAstroImage->m_strTelescopeName = m_pCamera->m_vectCameraLensBrandDef[m_rLensSetup.nLensBrand].vectData[m_rLensSetup.nLensName].name;
			else if( m_nTLensSource == 1 && m_pObserver->m_pHardware->IsCameraLens(m_rLensSetup.nLensNameOwn) )
				m_pAstroImage->m_strTelescopeName = m_pObserver->m_pHardware->m_vectCameraLens[m_rLensSetup.nLensNameOwn].name;
		}
	}
	delete( pSelectTLens );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnSetFocalLength
// Class:		CPosHintDlg
// Purpose:		gets set when i cheange focal
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CPosHintDlg::OnSetFocalLength( wxCommandEvent& pEvent )
{
	if( !m_pFocal ) return;

	SetFocalLength( );
	CalculateFieldValues( );
	SetField( );

	return;
}

////////////////////////////////////////////////////////////////////
void CPosHintDlg::OnValidate( wxCommandEvent& event )
{
	int nSelect = m_pObjectType->GetSelection();

	if( m_pOrigOptA->GetValue() )
	{
		if( nSelect == ORIGIN_LIST_TYPE_NGC_ID || 
			nSelect == ORIGIN_LIST_TYPE_IC_ID ||
			nSelect == ORIGIN_LIST_TYPE_MESSIER_ID ||
			nSelect == ORIGIN_LIST_TYPE_SAO_ID )
		{
			unsigned long nCatNo = 0;
			if( !m_pObjectId->GetLineText(0).ToULong( &nCatNo ) )
			{
				wxMessageBox( wxT("Invalid catalog number!"), wxT("Data Entry Error!"), wxOK|wxICON_ERROR );
				return;

			} else
			{
				if( nSelect == ORIGIN_LIST_TYPE_NGC_ID )
				{
					m_pSky->m_pCatalogDsoNgc->GetRaDec( nCatNo, m_nRaDeg, m_nDecDeg );

				} else if( nSelect == ORIGIN_LIST_TYPE_IC_ID )
				{
					m_pSky->m_pCatalogDsoIc->GetRaDec( nCatNo, m_nRaDeg, m_nDecDeg );

				} else if( nSelect == ORIGIN_LIST_TYPE_MESSIER_ID )
				{
					m_pSky->m_pCatalogDsoMessier->GetRaDec( nCatNo, m_nRaDeg, m_nDecDeg );
				
				} else if( nSelect == ORIGIN_LIST_TYPE_SAO_ID )
				{
					m_pSky->m_pCatalogStars->GetStarRaDec( nCatNo, m_nRaDeg, m_nDecDeg );
				}
			}

		} else
		{
			if( m_nOrigType == ORIGIN_LIST_TYPE_STAR_NAMES )
			{
				//m_nRaDeg = m_pCatalog->m_vectStarName[nSelect].ra;
				//m_nDecDeg = m_pCatalog->m_vectStarName[nSelect].dec;
				m_pSky->m_pCatalogStarNames->GetNameRaDec( nSelect, m_nRaDeg, m_nDecDeg );

			} else if( m_nOrigType == ORIGIN_LIST_TYPE_DSO_NAMES )
			{
				m_pSky->m_pCatalogDsoNames->GetDsoNameRaDec( nSelect, m_nRaDeg, m_nDecDeg );

			} else if( m_nOrigType == ORIGIN_LIST_TYPE_CONST_NAMES )
			{
				m_nRaDeg = m_pSky->m_pConstellations->m_vectConstName[nSelect].ra;
				m_nDecDeg = m_pSky->m_pConstellations->m_vectConstName[nSelect].dec;
			}
		}

	} else if( m_pOrigOptB->GetValue() )
	{
		// update values in decimal if modified
		if( m_pRaDecEntryType->GetSelection() == 0 )
			UpdateRaDecValues( 0 );
		else
			UpdateRaDecValues( 1 );
	}

	///////
	// final check if ra/dec is not zero
	if( (m_nRaDeg == 0.0 && m_nDecDeg == 0.0) )
	{
		int answer  = wxMessageBox( wxT("Hint position ra/dec is zero!\nAccept these values anyway?"), wxT("Data Entry Warning!"), wxYES_NO|wxICON_EXCLAMATION );
		if( answer != wxYES ) return;
	}

	event.Skip();
	return;
}
