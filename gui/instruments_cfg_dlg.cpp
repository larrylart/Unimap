
// wx includes
#include "wx/notebook.h"
#include <wx/spinctrl.h>

// local headers
#include "../util/func.h"
#include "../observer/observer_hardware.h"
#include "../telescope/telescope.h"
#include "../telescope/ascom.h"
#include "../telescope/focuser.h"
#include "../camera/camera.h"

// main header
#include "instruments_cfg_dlg.h"

// CInstrumentsCfgDlg EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( CInstrumentsCfgDlg, wxDialog )
	// telescope
	EVT_CHECKBOX( wxID_INSTR_CFG_TEL_USE_ASCOM, CInstrumentsCfgDlg::OnUseAscom )
	EVT_BUTTON( wxID_INSTR_CFG_TEL_ASCOM_GET, CInstrumentsCfgDlg::OnAscomChooser )
	// focuser
	EVT_RADIOBUTTON( wxID_INSTR_CFG_FOCUSER_USE_TEL, CInstrumentsCfgDlg::OnUseFocuser )
	EVT_RADIOBUTTON( wxID_INSTR_CFG_FOCUSER_USE_ASCOM, CInstrumentsCfgDlg::OnUseFocuser )
	EVT_RADIOBUTTON( wxID_INSTR_CFG_FOCUSER_USE_CUSTOM, CInstrumentsCfgDlg::OnUseFocuser )
	EVT_BUTTON( wxID_INSTR_CFG_FOC_ASCOM_GET, CInstrumentsCfgDlg::OnAscomChooser )
	// camera
	EVT_CHECKBOX( wxID_INSTR_CFG_CAMERA_ACTIVATE, CInstrumentsCfgDlg::OnCameraActivate )
	EVT_CHOICE( wxID_INSTR_CFG_CAMERA_SELECT, CInstrumentsCfgDlg::OnCameraSelect )
	// validate catalogs on ok
	EVT_BUTTON( wxID_OK, CInstrumentsCfgDlg::OnValidate )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
CInstrumentsCfgDlg::CInstrumentsCfgDlg(wxWindow *parent) : wxDialog(parent, -1,
                          wxT("Instruments Setup"), wxDefaultPosition, wxSize( -1, -1 ),
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	int i=0;
	m_nUseFocuser = 0;
	m_pTelescope = NULL;
	m_pFocuser = NULL;

	// telescope options
	wxString vectTelescopeSelect[3];
	vectTelescopeSelect[0] = wxT("No Telescope");
	vectTelescopeSelect[1] = wxT("Meade LX200");
	vectTelescopeSelect[2] = wxT("Celestron NexStar");
	// focuser options
	wxString vectFocuserSelect[3];
	vectFocuserSelect[0] = wxT("JMI Focuser");
	// serial port
	wxString vectSerialPortSelect[10];
	for( i=0; i<9; i++ ) vectSerialPortSelect[i].sprintf( wxT("%d"), i+1 );
	// serial port baud
	wxString vectSerialPortBaud[10];
	vectSerialPortBaud[0] = wxT("300");
	vectSerialPortBaud[1] = wxT("600");
	vectSerialPortBaud[2] = wxT("1200");
	vectSerialPortBaud[3] = wxT("2400");
	vectSerialPortBaud[4] = wxT("4800");
	vectSerialPortBaud[5] = wxT("9600");
	vectSerialPortBaud[6] = wxT("19200");
	vectSerialPortBaud[7] = wxT("38400");
	// camera interface
	wxString vectCamInterface[2];
	vectCamInterface[0] = wxT("Wia");
	vectCamInterface[1] = wxT("Factory");
	// conditions online
	wxString vectCondOnlineSelect[3];
	vectCondOnlineSelect[0] = wxT("None");
	vectCondOnlineSelect[1] = wxT("Weather Underground");
	// conditions own stations
	wxString vectCondOwnSelect[3];
	vectCondOwnSelect[0] = wxT("None");
	vectCondOwnSelect[1] = wxT("Vantage Pro");

	// main dialog sizer
	wxFlexGridSizer *pTopSizer = new wxFlexGridSizer( 2, 1, 5, 5 );

	// building configure as a note book
	wxNotebook* pNotebook = new wxNotebook( this, -1, wxPoint(-1,-1), wxSize(-1,-1), wxNB_TOP|wxNB_MULTILINE ); 
	// GENERAL PANEL
	wxPanel* pPanelGeneral = new wxPanel( pNotebook );
	// TELESCOPE PANEL
	wxPanel* pPanelTelescope = new wxPanel( pNotebook );
	// FOCUSER PANEL
	wxPanel* pPanelFocuser = new wxPanel( pNotebook );
	// CAMERA PANEL
	wxPanel* pPanelCamera = new wxPanel( pNotebook );
	// DOME PANEL
	wxPanel* pPanelDome = new wxPanel( pNotebook );
	// CONDITIONS PANEL
	wxPanel* pPanelCond = new wxPanel( pNotebook );
	// GUIDE PANEL
	wxPanel* pPanelGuide = new wxPanel( pNotebook );

	////////////
	// GENERAL SIZER
	wxFlexGridSizer* sizerGeneral = new wxFlexGridSizer( 3, 2, 10, 10 );
	sizerGeneral->AddGrowableCol( 1 );
	sizerGeneral->AddGrowableCol( 0 );
	wxBoxSizer* sizerGeneralPage = new wxBoxSizer( wxHORIZONTAL );
	sizerGeneralPage->Add( sizerGeneral, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxALL, 10 );

	////////////
	// TELESCOPE SIZER
	wxFlexGridSizer* sizerTelescope = new wxFlexGridSizer( 3, 2, 5, 5 );
	sizerTelescope->AddGrowableCol( 1 );
	sizerTelescope->AddGrowableCol( 0 );
	wxBoxSizer* sizerTelescopePage = new wxBoxSizer( wxHORIZONTAL );
	sizerTelescopePage->Add( sizerTelescope, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxALL, 10 );

	// FOCUSER SIZER
	wxFlexGridSizer* sizerFocuser = new wxFlexGridSizer( 3, 2, 5, 5 );
	sizerFocuser->AddGrowableCol( 1 );
	sizerFocuser->AddGrowableCol( 0 );
	wxBoxSizer* sizerFocuserPage = new wxBoxSizer( wxHORIZONTAL );
	sizerFocuserPage->Add( sizerFocuser, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxALL, 10 );

	// CAMERA SIZER
	wxFlexGridSizer* sizerCamera = new wxFlexGridSizer( 3, 1, 5, 5 );
	sizerCamera->AddGrowableCol( 0 );
	sizerCamera->AddGrowableRow( 0 );
	wxBoxSizer* sizerCameraPage = new wxBoxSizer( wxHORIZONTAL );
	sizerCameraPage->Add( sizerCamera, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxALL, 10 );
	// header
	wxFlexGridSizer* sizerCameraHead = new wxFlexGridSizer( 2, 2, 5, 5 );
	sizerCameraHead->AddGrowableCol( 1 );
	sizerCamera->Add( sizerCameraHead, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// properties
	pCameraPropBox = new wxStaticBox( pPanelCamera, -1, wxT("Properties"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* sizerCameraPropBox = new wxStaticBoxSizer( pCameraPropBox, wxVERTICAL );
	wxFlexGridSizer* sizerCameraProp = new wxFlexGridSizer( 3,2,5,5 );
	sizerCameraPropBox->Add( sizerCameraProp, 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxALL, 10 );
	sizerCamera->Add( sizerCameraPropBox, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW|wxLEFT|wxRIGHT, 5 );

	// DOME SIZER
	wxFlexGridSizer* sizerDome = new wxFlexGridSizer( 3, 2, 5, 5 );
	sizerDome->AddGrowableCol( 1 );
	sizerDome->AddGrowableCol( 0 );
	wxBoxSizer* sizerDomePage = new wxBoxSizer( wxHORIZONTAL );
	sizerDomePage->Add( sizerDome, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxALL, 10 );

	// CONDITIONS SIZER
	wxFlexGridSizer* sizerCond = new wxFlexGridSizer( 4, 2, 5, 5 );
	sizerCond->AddGrowableCol( 1 );
	sizerCond->AddGrowableCol( 0 );
	wxBoxSizer* sizerCondPage = new wxBoxSizer( wxHORIZONTAL );
	sizerCondPage->Add( sizerCond, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxALL, 10 );

	// GUIDE SIZER
	wxFlexGridSizer* sizerGuide = new wxFlexGridSizer( 4, 2, 10, 10 );
	sizerGuide->AddGrowableCol( 1 );
	sizerGuide->AddGrowableCol( 0 );
	wxBoxSizer* sizerGuidePage = new wxBoxSizer( wxHORIZONTAL );
	sizerGuidePage->Add( sizerGuide, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxALL, 10 );

	/////////////////////////
	// INSTRUMENTS CONFIG :: GENERAL
	m_pAutoDetection = new wxCheckBox( pPanelGeneral, -1, wxT("Auto Detection") );
	m_pAutoMatching = new wxCheckBox( pPanelGeneral, -1, wxT("Auto Matching") );
	m_pAutoRegister = new wxCheckBox( pPanelGeneral, -1, wxT("Auto Register") );
	m_pAutoStacking = new wxCheckBox( pPanelGeneral, -1, wxT("Auto Stacking") );
	m_pAutoUpload = new wxCheckBox( pPanelGeneral, -1, wxT("Auto Upload") );
	m_pNotifyQuality = new wxCheckBox( pPanelGeneral, -1, wxT("Notify Quality") );
	m_pNotifyDiscovery = new wxCheckBox( pPanelGeneral, -1, wxT("Notify Discovery") );
	m_pRemoteAccess = new wxCheckBox( pPanelGeneral, -1, wxT("Remote Access") );

	/////////////////////////
	// INSTRUMENTS CONFIG :: Guide
	m_pReverseNS = new wxCheckBox( pPanelGuide, -1, wxT("") );
	m_pReverseWE = new wxCheckBox( pPanelGuide, -1, wxT("") );
	m_pRaPulseLength = new wxSpinCtrl( pPanelGuide, -1, wxT("0"), wxDefaultPosition, wxSize(70,-1), wxSP_ARROW_KEYS, 0, 60000, 0 );
	m_pDecPulseLength = new wxSpinCtrl( pPanelGuide, -1, wxT("0"), wxDefaultPosition, wxSize(70,-1), wxSP_ARROW_KEYS, 0, 60000, 0 );

	/////////////////////////
	// INSTRUMENTS CONFIG :: TELESCOPE
	// :: ascom
	m_pTelUseAscom = new wxCheckBox( pPanelTelescope, wxID_INSTR_CFG_TEL_USE_ASCOM, wxT("") );
	m_pTelAscomLabel = new wxStaticText( pPanelTelescope, -1, wxT("Use Ascom") );
	m_pTelAscomDriverName = new wxTextCtrl( pPanelTelescope, -1, wxT(""), wxDefaultPosition, wxSize(150,-1) );
	m_pTelPickAscom = new wxButton( pPanelTelescope, wxID_INSTR_CFG_TEL_ASCOM_GET, wxT("Set"), wxPoint(-1, -1), wxSize(35,-1) );
	// telescope :: type
	m_pTelescopeLabel = new wxStaticText( pPanelTelescope, -1, wxT("Telescope:") );
	m_pTelescopeSelect = new wxChoice( pPanelTelescope, wxID_INSTR_CFG_TELESCOPE_SELECT, wxDefaultPosition, wxSize(-1,-1), 3, 
										vectTelescopeSelect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pTelescopeSelect->SetSelection(0);
	// telescope :: serial port
	m_pTelSerialPortLabel = new wxStaticText( pPanelTelescope, -1, wxT("Serial Port:") );
	m_pTelSerialPortSelect = new wxChoice( pPanelTelescope, -1, wxDefaultPosition, wxSize(-1,-1), 9, 
										vectSerialPortSelect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pTelSerialPortSelect->SetSelection(0);
	// telescope :: serial port baud
	m_pTelSerialBaudLabel = new wxStaticText( pPanelTelescope, -1, wxT("Baud Rate:") );
	m_pTelSerialPortBaud = new wxChoice( pPanelTelescope, -1, wxDefaultPosition, wxSize(-1,-1), 8, 
										vectSerialPortBaud, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pTelSerialPortBaud->SetSelection(0);

	///////////////
	// INSTRUMENTS CONFIG :: FOCUSER
	// :: telescope
	m_pFocUseTel = new wxRadioButton( pPanelFocuser, wxID_INSTR_CFG_FOCUSER_USE_TEL, wxT(""), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_pFocUseTelLabel = new wxStaticText( pPanelFocuser, -1, wxT("Use Current Telescope Driver") );
	// :: ascom
	m_pFocUseAscom = new wxRadioButton( pPanelFocuser, wxID_INSTR_CFG_FOCUSER_USE_ASCOM, wxT("") );
	m_pFocUseAscomLabel = new wxStaticText( pPanelFocuser, -1, wxT("Use Ascom") );
	m_pFocAscomDriverName = new wxTextCtrl( pPanelFocuser, -1, wxT(""), wxDefaultPosition, wxSize(150,-1) );
	m_pFocPickAscom = new wxButton( pPanelFocuser, wxID_INSTR_CFG_FOC_ASCOM_GET, wxT("Set"), wxPoint(-1, -1), wxSize(35,-1) );
	// :: custom focuser type
	m_pFocUseCustom = new wxRadioButton( pPanelFocuser, wxID_INSTR_CFG_FOCUSER_USE_CUSTOM, wxT("") );
	m_pFocuserLabel = new wxStaticText( pPanelFocuser, -1, wxT("Focuser:") );
	m_pFocuserSelect = new wxChoice( pPanelFocuser, wxID_INSTR_CFG_FOCUSER_SELECT, wxDefaultPosition, wxSize(-1,-1), 1, 
										vectFocuserSelect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pFocuserSelect->SetSelection(0);
	// focuser :: serial port
	m_pFocSerialPortLabel = new wxStaticText( pPanelFocuser, -1, wxT("Serial Port:") );
	m_pFocSerialPortSelect = new wxChoice( pPanelFocuser, -1, wxDefaultPosition, wxSize(-1,-1), 9, 
										vectSerialPortSelect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pFocSerialPortSelect->SetSelection(0);
	// focuser :: serial port baud
	m_pFocSerialBaudLabel = new wxStaticText( pPanelFocuser, -1, wxT("Baud Rate:") );
	m_pFocSerialPortBaud = new wxChoice( pPanelFocuser, -1, wxDefaultPosition, wxSize(-1,-1), 8, 
										vectSerialPortBaud, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pFocSerialPortBaud->SetSelection(0);

	/////////////
	// INSTRUMENTS CONFIG :: CAMERA
	m_pCameraSelect = new wxChoice( pPanelCamera, wxID_INSTR_CFG_CAMERA_SELECT, wxDefaultPosition, wxSize(220,-1), 0, 
										NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pCameraSelect->SetSelection(0);
	m_pCamActivate = new wxCheckBox( pPanelCamera, wxID_INSTR_CFG_CAMERA_ACTIVATE, wxT("Activate using Interface:") );
	m_pCamInterface = new wxChoice( pPanelCamera, -1, wxDefaultPosition, wxSize(70,-1), 1, 
										vectCamInterface, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pCamInterface->SetSelection(0);
	// prop
	m_pCamDoRecord = new wxCheckBox( pPanelCamera, -1, wxT("Imaging") );
	m_pCamDoGuide = new wxCheckBox( pPanelCamera, -1, wxT("Guiding") );
	m_pCamDoFocus = new wxCheckBox( pPanelCamera, -1, wxT("Focusing") );
	m_pCamDoFinder = new wxCheckBox( pPanelCamera, -1, wxT("View Finder") );
	m_pCamDoSpectroscopy = new wxCheckBox( pPanelCamera, -1, wxT("Spectroscopy") );
	m_pCamDoMonitor = new wxCheckBox( pPanelCamera, -1, wxT("Monitoring") );

	////////////////////
	// INSTRUMENTS CONFIG :: DOME
	// :: ascom
	m_pDomeUseAscom = new wxRadioButton( pPanelDome, wxID_INSTR_CFG_DOME_USE_ASCOM, wxT(""), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_pDomeUseAscomLabel = new wxStaticText( pPanelDome, -1, wxT("Use Ascom") );
	m_pDomeAscomDriverName = new wxTextCtrl( pPanelDome, -1, wxT(""), wxDefaultPosition, wxSize(150,-1) );
	m_pDomePickAscom = new wxButton( pPanelDome, wxID_INSTR_CFG_DOME_ASCOM_GET, wxT("Set"), wxPoint(-1, -1), wxSize(35,-1) );
	// :: own conditions station
	m_pDomeUseCustom = new wxRadioButton( pPanelDome, wxID_INSTR_CFG_DOME_USE_CUSTOM, wxT("") );
	m_pDomeCustomLabel = new wxStaticText( pPanelDome, -1, wxT("Own Hardware:") );
	m_pDomeCustomSelect = new wxChoice( pPanelDome, wxID_INSTR_CFG_DOME_CUSTOM_SELECT, wxDefaultPosition, wxSize(100,-1), 0, 
										NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pDomeCustomSelect->SetSelection(0);
	// disable for now - todo: implement
	m_pDomeUseCustom->Disable();
	m_pDomeCustomLabel->Disable();
	m_pDomeCustomLabel->Disable();

	////////////////////
	// INSTRUMENTS CONFIG :: CONDITIONS
	// :: conditions online
	m_pCondUseOnline = new wxRadioButton( pPanelCond, wxID_INSTR_CFG_COND_USE_ONLINE, wxT(""), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_pCondOnlineLabel = new wxStaticText( pPanelCond, -1, wxT("Online Data:") );
	m_pCondOnlineSelect = new wxChoice( pPanelCond, wxID_INSTR_CFG_COND_ONLINE_SELECT, wxDefaultPosition, wxSize(-1,-1), 2, 
										vectCondOnlineSelect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pCondOnlineSelect->SetSelection(1);
	// :: own conditions station
	m_pCondUseOwn = new wxRadioButton( pPanelCond, wxID_INSTR_CFG_COND_USE_OWN, wxT("") );
	m_pCondOwnLabel = new wxStaticText( pPanelCond, -1, wxT("Own Hardware:") );
	m_pCondOwnSelect = new wxChoice( pPanelCond, wxID_INSTR_CFG_COND_OWN_SELECT, wxDefaultPosition, wxSize(-1,-1), 2, 
										vectCondOwnSelect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pCondOwnSelect->SetSelection(1);
	// conditions station :: serial port
	m_pCondSerialPortLabel = new wxStaticText( pPanelCond, -1, wxT("Serial Port:") );
	m_pCondSerialPortSelect = new wxChoice( pPanelCond, -1, wxDefaultPosition, wxSize(-1,-1), 9, 
										vectSerialPortSelect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pCondSerialPortSelect->SetSelection(0);
	// conditions station :: serial port baud
	m_pCondSerialBaudLabel = new wxStaticText( pPanelCond, -1, wxT("Baud Rate:") );
	m_pCondSerialPortBaud = new wxChoice( pPanelCond, -1, wxDefaultPosition, wxSize(-1,-1), 8, 
										vectSerialPortBaud, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pCondSerialPortBaud->SetSelection(0);
	// for now disable all but online - todo: implement this latter
	m_pCondUseOwn->Disable();
	m_pCondOwnLabel->Disable();
	m_pCondOwnSelect->Disable();
	m_pCondSerialPortLabel->Disable();
	m_pCondSerialPortSelect->Disable();
	m_pCondSerialBaudLabel->Disable();
	m_pCondSerialPortBaud->Disable();

	/////////////////////////////////////////
	// populate GENERAL panel
	sizerGeneral->Add( m_pAutoDetection, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxLEFT, 10 );
	sizerGeneral->Add( m_pAutoMatching, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerGeneral->Add( m_pAutoRegister, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxLEFT, 10 );
	sizerGeneral->Add( m_pAutoStacking, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerGeneral->Add( m_pAutoUpload, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxLEFT, 10 );
	sizerGeneral->Add( m_pNotifyQuality, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerGeneral->Add( m_pNotifyDiscovery, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxLEFT, 10 );
	sizerGeneral->Add( m_pRemoteAccess, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	/////////////////////////////////////////
	// populate GUIDE panel
	sizerGuide->Add( new wxStaticText(pPanelGuide, -1, _T("Reverse North/South:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerGuide->Add( m_pReverseNS, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerGuide->Add( new wxStaticText(pPanelGuide, -1, _T("Reverse West/East:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerGuide->Add( m_pReverseWE, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
//	wxFlexGridSizer* sizerGuideA = new wxFlexGridSizer( 2, 2, 5, 5 );
	sizerGuide->Add( new wxStaticText(pPanelGuide, -1, _T("Ra Pulse Length:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerGuide->Add( m_pRaPulseLength, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerGuide->Add( new wxStaticText(pPanelGuide, -1, _T("Dec Pulse Length:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerGuide->Add( m_pDecPulseLength, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
//	sizerGuide->Add( sizerGuideA, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	/////////////////////////////////////////
	// populate TELESCOPE panel
	// :: ascom option
	wxFlexGridSizer* sizerTelAscomA = new wxFlexGridSizer( 1, 2, 5, 5 );
	sizerTelAscomA->AddGrowableCol( 0 );
	sizerTelAscomA->Add( m_pTelUseAscom, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerTelAscomA->Add( m_pTelAscomLabel, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerTelescope->Add( sizerTelAscomA, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	wxFlexGridSizer* sizerTelAscomB = new wxFlexGridSizer( 1, 2, 5, 5 );
	sizerTelAscomB->Add( m_pTelAscomDriverName, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerTelAscomB->Add( m_pTelPickAscom, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerTelescope->Add( sizerTelAscomB, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: my own telescope classes
	sizerTelescope->Add( m_pTelescopeLabel, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerTelescope->Add( m_pTelescopeSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: serial port
	sizerTelescope->Add( m_pTelSerialPortLabel, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerTelescope->Add( m_pTelSerialPortSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: serial baud
	sizerTelescope->Add( m_pTelSerialBaudLabel, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerTelescope->Add( m_pTelSerialPortBaud, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	/////////////////////////////////////////
	// populate FOCUSER panel
	// :: telescope
	sizerFocuser->Add( m_pFocUseTel, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerFocuser->Add( m_pFocUseTelLabel, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: ascom
	sizerFocuser->Add( m_pFocUseAscom, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	wxFlexGridSizer* sizerFocUseAscom = new wxFlexGridSizer( 1, 3, 5, 5 );
	sizerFocUseAscom->Add( m_pFocUseAscomLabel, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerFocUseAscom->Add( m_pFocAscomDriverName, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerFocUseAscom->Add( m_pFocPickAscom, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerFocuser->Add( sizerFocUseAscom, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: custom 
	sizerFocuser->Add( m_pFocUseCustom, 1, wxALIGN_RIGHT | wxALIGN_TOP|wxTOP, 3 );
	wxFlexGridSizer* sizerFocUseCustom = new wxFlexGridSizer( 2, 2, 5, 5 );
	sizerFocUseCustom->Add( m_pFocuserLabel, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerFocUseCustom->Add( m_pFocuserSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: custom serial 
	sizerFocUseCustom->Add( m_pFocSerialPortLabel, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerFocUseCustom->Add( m_pFocSerialPortSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: custom serial baud
	sizerFocUseCustom->Add( m_pFocSerialBaudLabel, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerFocUseCustom->Add( m_pFocSerialPortBaud, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// add custom sizer to focuser sizzer
	sizerFocuser->Add( sizerFocUseCustom, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	/////////////////////////////////////////
	// populate CAMERA panel
	// :: camera select
	sizerCameraHead->Add( new wxStaticText( pPanelCamera, -1, wxT("Camera:") ), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerCameraHead->Add( m_pCameraSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: camera activation
	sizerCameraHead->AddStretchSpacer();
	wxFlexGridSizer* sizerCameraAct = new wxFlexGridSizer( 1, 3, 5, 5 );
	sizerCameraAct->AddGrowableCol( 1 );
	sizerCameraAct->Add( m_pCamActivate, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerCameraAct->Add( m_pCamInterface, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerCameraHead->Add( sizerCameraAct, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: properties 
	sizerCameraProp->Add( m_pCamDoRecord, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerCameraProp->Add( m_pCamDoGuide, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxLEFT, 10 );
	sizerCameraProp->Add( m_pCamDoFocus, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerCameraProp->Add( m_pCamDoFinder, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxLEFT, 10 );
	sizerCameraProp->Add( m_pCamDoSpectroscopy, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerCameraProp->Add( m_pCamDoMonitor, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxLEFT, 10 );

	/////////////////////////////////////////
	// populate DOME panel
	// :: ascom
	sizerDome->Add( m_pDomeUseAscom, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	wxFlexGridSizer* sizerDomeUseAscom = new wxFlexGridSizer( 1, 3, 5, 5 );
	sizerDomeUseAscom->Add( m_pDomeUseAscomLabel, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerDomeUseAscom->Add( m_pDomeAscomDriverName, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerDomeUseAscom->Add( m_pDomePickAscom, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerDome->Add( sizerDomeUseAscom, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: custom 
	sizerDome->Add( m_pDomeUseCustom, 1, wxALIGN_RIGHT | wxALIGN_TOP|wxTOP, 3 );
	wxFlexGridSizer* sizerDomeUseCustom = new wxFlexGridSizer( 2, 2, 5, 5 );
	sizerDomeUseCustom->Add( m_pDomeCustomLabel, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerDomeUseCustom->Add( m_pDomeCustomSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: custom serial 
//	sizerDomeUseCustom->Add( m_pDomeSerialPortLabel, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
//	sizerDomeUseCustom->Add( m_pDomeSerialPortSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: custom serial baud
//	sizerDomeUseCustom->Add( m_pDomeSerialBaudLabel, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
//	sizerDomeUseCustom->Add( m_pDomeSerialPortBaud, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// add custom sizer to focuser sizzer
	sizerDome->Add( sizerDomeUseCustom, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	/////////////////////////////////////////
	// populate CONDITIONS panel
	// :: online data fetch
	sizerCond->Add( m_pCondUseOnline, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	wxFlexGridSizer* sizerCondOnline = new wxFlexGridSizer( 1, 3, 5, 5 );
	sizerCondOnline->Add( m_pCondOnlineLabel, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerCondOnline->Add( m_pCondOnlineSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerCond->Add( sizerCondOnline, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: local/own weather station
	sizerCond->Add( m_pCondUseOwn, 1, wxALIGN_RIGHT | wxALIGN_TOP|wxTOP, 3 );
	wxFlexGridSizer* sizerCondOwn = new wxFlexGridSizer( 2, 2, 5, 5 );
	sizerCondOwn->Add( m_pCondOwnLabel, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerCondOwn->Add( m_pCondOwnSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: custom serial 
	sizerCondOwn->Add( m_pCondSerialPortLabel, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerCondOwn->Add( m_pCondSerialPortSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: custom serial baud
	sizerCondOwn->Add( m_pCondSerialBaudLabel, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerCondOwn->Add( m_pCondSerialPortBaud, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// add custom sizer to focuser sizzer
	sizerCond->Add( sizerCondOwn, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	// add sizer/panels to notebook
	pPanelGeneral->SetSizer( sizerGeneralPage );
	pNotebook->AddPage( pPanelGeneral, wxT("General") );
	pPanelTelescope->SetSizer( sizerTelescopePage );
	pNotebook->AddPage( pPanelTelescope, wxT("Telescope") );
	pPanelFocuser->SetSizer( sizerFocuserPage );
	pNotebook->AddPage( pPanelFocuser, wxT("Focuser") );
	pPanelCamera->SetSizer( sizerCameraPage );
	pNotebook->AddPage( pPanelCamera, wxT("Camera") );
	pPanelDome->SetSizer( sizerDomePage );
	pNotebook->AddPage( pPanelDome, wxT("Dome") );
	pPanelCond->SetSizer( sizerCondPage );
	pNotebook->AddPage( pPanelCond, wxT("Conditions") );
	pPanelGuide->SetSizer( sizerGuidePage );
	pNotebook->AddPage( pPanelGuide, wxT("Guide") );

	pTopSizer->Add( pNotebook, 0, wxGROW | wxALL|wxEXPAND, 10 );
	pTopSizer->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 10 );

	pNotebook->Layout();
	SetSizer(pTopSizer);
	SetAutoLayout(TRUE);
	pTopSizer->Fit(this);
	Fit();
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
CInstrumentsCfgDlg::~CInstrumentsCfgDlg( ) 
{
	// general
	delete( m_pAutoDetection );
	delete( m_pAutoMatching );
	delete( m_pAutoRegister );
	delete( m_pAutoStacking );
	delete( m_pAutoUpload );
	delete( m_pNotifyQuality );
	delete( m_pNotifyDiscovery );
	delete( m_pRemoteAccess );

	// guide
	delete( m_pReverseNS );
	delete( m_pReverseWE );
	delete( m_pRaPulseLength );
	delete( m_pDecPulseLength );

	// telescope :: ascom
	delete( m_pTelUseAscom );
	delete( m_pTelAscomLabel );
	delete( m_pTelAscomDriverName );
	delete( m_pTelPickAscom );
	// telescope :: built in
	delete( m_pTelescopeLabel );
	delete( m_pTelescopeSelect );
	delete( m_pTelSerialPortLabel );
	delete( m_pTelSerialPortSelect );
	delete( m_pTelSerialBaudLabel );
	delete( m_pTelSerialPortBaud );

	// focuser
	delete( m_pFocUseTel );
	delete( m_pFocUseTelLabel );
	delete( m_pFocUseAscom );
	delete( m_pFocUseAscomLabel );
	delete( m_pFocAscomDriverName );
	delete( m_pFocPickAscom );
	delete( m_pFocUseCustom );
	delete( m_pFocuserLabel );
	delete( m_pFocuserSelect );
	delete( m_pFocSerialPortLabel );
	delete( m_pFocSerialPortSelect );
	delete( m_pFocSerialBaudLabel );
	delete( m_pFocSerialPortBaud );

	// camera select
	m_vectCamDevices.clear();
	delete( pCameraPropBox );
	delete( m_pCameraSelect );
	delete( m_pCamActivate );
	delete( m_pCamInterface );
	delete( m_pCamDoRecord );
	delete( m_pCamDoGuide );
	delete( m_pCamDoFocus );
	delete( m_pCamDoFinder );
	delete( m_pCamDoSpectroscopy );
	delete( m_pCamDoMonitor );

	// dome clean
	delete( m_pDomeUseAscom );
	delete( m_pDomeUseAscomLabel );
	delete( m_pDomeAscomDriverName );
	delete( m_pDomePickAscom );
	delete( m_pDomeUseCustom );
	delete( m_pDomeCustomLabel );
	delete( m_pDomeCustomSelect );

	// conditions clean
	delete( m_pCondUseOnline );
	delete( m_pCondOnlineLabel );
	delete( m_pCondOnlineSelect );
	delete( m_pCondUseOwn );
	delete( m_pCondOwnLabel );
	delete( m_pCondOwnSelect );
	delete( m_pCondSerialPortLabel );
	delete( m_pCondSerialPortSelect );
	delete( m_pCondSerialBaudLabel );
	delete( m_pCondSerialPortBaud );
}

////////////////////////////////////////////////////////////////////
void CInstrumentsCfgDlg::SetConfig( CTelescope* pTelescope, CFocuser* pFocuser, CCamera* pCamera )
{
	int i=0;
	wxString strTmp;

	m_pTelescope = pTelescope;
	m_pFocuser = pFocuser;
	m_pCamera = pCamera;

	SetUseTelescope( !(m_pTelescope->m_bUseAscom) );
	if( m_pTelescope->m_bUseAscom )
	{
		m_pFocUseAscom->SetValue( 1 );
		SetUseFocuser( 1 );

	} else
	{
		m_pFocUseTel->SetValue( 1 );
		SetUseFocuser( 0 );
	}

	m_pTelescopeSelect->SetSelection( m_pTelescope->m_nTelescopeDrvType );
	m_pTelSerialPortSelect->SetSelection( m_pTelescope->m_nSerialPortNo-1 );

	////////////
	// set camera	
	m_vectCamDevices.clear();
	m_pCameraSelect->Clear();
	for( i=0; i<m_pCamera->m_vectHWDevices.size(); i++ )
	{
		//populate local
		m_vectCamDevices.push_back( m_pCamera->m_vectHWDevices[i] );

		if( m_vectCamDevices[i].connected )
			strTmp.Printf( wxT("[C]:%s"),  m_vectCamDevices[i].strName );
		else
			strTmp.Printf( wxT("[D]:%s"),  m_vectCamDevices[i].strName );

		m_pCameraSelect->Append( strTmp );
	}
	m_pCameraSelect->SetSelection(0);
	m_nCameraId = 0;

	// set activation status - if any
	if( m_pCamera->m_vectHWDevices.size() ) 
	{
		SetCameraState( m_vectCamDevices[0].active );
		// set camera value
		SetCameraValues( );
	}
}

// Method:	OnUseAscom
////////////////////////////////////////////////////////////////////
void CInstrumentsCfgDlg::OnUseAscom( wxCommandEvent& pEvent )
{
	int bValue = pEvent.IsChecked();

	// enable/disable
	SetUseTelescope( !bValue );
	// also adjust focuser panel 
	if( bValue && m_nUseFocuser ==  0 )
	{
		m_pFocUseAscom->SetValue( 1 );
		SetUseFocuser( 1 );
	}
}

////////////////////////////////////////////////////////////////////
void CInstrumentsCfgDlg::SetUseTelescope( int nOpt )
{
	if( nOpt == 0 )
	{
		// enable
		m_pTelAscomLabel->Enable();
		m_pTelAscomDriverName->Enable();
		m_pTelPickAscom->Enable();
		// disable
		m_pTelescopeLabel->Disable();
		m_pTelescopeSelect->Disable();
		m_pTelSerialPortLabel->Disable();
		m_pTelSerialPortSelect->Disable();
		m_pTelSerialBaudLabel->Disable();
		m_pTelSerialPortBaud->Disable();
		// also disable focuser option 0
		m_pFocUseTel->Disable();

	} else if( nOpt == 1 )
	{
		// enable
		m_pTelescopeLabel->Enable();
		m_pTelescopeSelect->Enable();
		m_pTelSerialPortLabel->Enable();
		m_pTelSerialPortSelect->Enable();
		m_pTelSerialBaudLabel->Enable();
		m_pTelSerialPortBaud->Enable();
		// also enable focuser option 0
		m_pFocUseTel->Enable();
		// disable
		m_pTelAscomLabel->Disable();
		m_pTelAscomDriverName->Disable();
		m_pTelPickAscom->Disable();
	}
}

// Method:	OnAscomChooser
////////////////////////////////////////////////////////////////////
void CInstrumentsCfgDlg::OnAscomChooser( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();

	if( nId == wxID_INSTR_CFG_TEL_ASCOM_GET )
	{
		CAscom* pAscom = new CAscom( ASCOM_DRV_TYPE_TELESCOPE );
		// call ascom interface to select
		if( pAscom->SelectInstrument( ) )
		{
			m_pTelAscomDriverName->SetValue( pAscom->m_strDriverId );
		}
		delete( pAscom );

	} else if( nId == wxID_INSTR_CFG_FOC_ASCOM_GET )
	{
		CAscom* pAscom = new CAscom( ASCOM_DRV_TYPE_FOCUSER );
		// call ascom interface to select
		if( pAscom->SelectInstrument( ) )
		{
			m_pFocAscomDriverName->SetValue( pAscom->m_strDriverId );
		}
		delete( pAscom );
	}
}

// Method:	OnUseFocuser
////////////////////////////////////////////////////////////////////
void CInstrumentsCfgDlg::OnUseFocuser( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();
	m_nUseFocuser = 0;

	if( nId == wxID_INSTR_CFG_FOCUSER_USE_TEL )
		m_nUseFocuser = 0;
	else if( nId == wxID_INSTR_CFG_FOCUSER_USE_ASCOM )
		m_nUseFocuser = 1;
	else if( nId == wxID_INSTR_CFG_FOCUSER_USE_CUSTOM )
		m_nUseFocuser = 2;

	SetUseFocuser( m_nUseFocuser );
}

////////////////////////////////////////////////////////////////////
void CInstrumentsCfgDlg::SetUseFocuser( int nOpt )
{
	if( nOpt == 0 )
	{
		// enable
		m_pFocUseTelLabel->Enable();
		// disable
		m_pFocUseAscomLabel->Disable();
		m_pFocAscomDriverName->Disable();
		m_pFocPickAscom->Disable();
		m_pFocuserLabel->Disable();
		m_pFocuserSelect->Disable();
		m_pFocSerialPortLabel->Disable();
		m_pFocSerialPortSelect->Disable();
		m_pFocSerialBaudLabel->Disable();
		m_pFocSerialPortBaud->Disable();

	} else if( nOpt == 1 )
	{
		// enable
		m_pFocUseAscomLabel->Enable();
		m_pFocAscomDriverName->Enable();
		m_pFocPickAscom->Enable();
		// disable
		m_pFocUseTelLabel->Disable();
		m_pFocuserLabel->Disable();
		m_pFocuserSelect->Disable();
		m_pFocSerialPortLabel->Disable();
		m_pFocSerialPortSelect->Disable();
		m_pFocSerialBaudLabel->Disable();
		m_pFocSerialPortBaud->Disable();

	} else if( nOpt == 2 )
	{
		// enable
		m_pFocuserLabel->Enable();
		m_pFocuserSelect->Enable();
		m_pFocSerialPortLabel->Enable();
		m_pFocSerialPortSelect->Enable();
		m_pFocSerialBaudLabel->Enable();
		m_pFocSerialPortBaud->Enable();
		// disable
		m_pFocUseTelLabel->Disable();
		m_pFocUseAscomLabel->Disable();
		m_pFocAscomDriverName->Disable();
		m_pFocPickAscom->Disable();
	}
}

// Method:	OnCameraSelect
////////////////////////////////////////////////////////////////////
void CInstrumentsCfgDlg::OnCameraSelect( wxCommandEvent& pEvent )
{
	/////////////////
	// first: set previous camera settings
	GetCameraValues( );

	/////////////////
	// now set from vector to gui new camera
	m_nCameraId = m_pCameraSelect->GetSelection();
	// set activation status
	SetCameraState( m_vectCamDevices[m_nCameraId].active );
	// set camera value
	SetCameraValues( );
}

////////////////////////////////////////////////////////////////////
void CInstrumentsCfgDlg::GetCameraValues( )
{
	m_vectCamDevices[m_nCameraId].active = m_pCamActivate->GetValue();
	m_vectCamDevices[m_nCameraId].interface_id = m_pCamInterface->GetSelection();
	m_vectCamDevices[m_nCameraId].do_imaging = m_pCamDoRecord->GetValue();
	m_vectCamDevices[m_nCameraId].do_guide = m_pCamDoGuide->GetValue();
	m_vectCamDevices[m_nCameraId].do_focus = m_pCamDoFocus->GetValue();
	m_vectCamDevices[m_nCameraId].do_finder = m_pCamDoFinder->GetValue();
	m_vectCamDevices[m_nCameraId].do_spectroscopy = m_pCamDoSpectroscopy->GetValue();
	m_vectCamDevices[m_nCameraId].do_monitor = m_pCamDoMonitor->GetValue();
}

////////////////////////////////////////////////////////////////////
void CInstrumentsCfgDlg::SetCameraValues( )
{
	// if it has custom interface
	m_pCamInterface->Clear();
	m_pCamInterface->Append( wxT("Wia") );
	if( m_vectCamDevices[m_nCameraId].has_factory_driver )
		m_pCamInterface->Append( wxT("Factory") );

	m_pCamInterface->SetSelection( m_vectCamDevices[m_nCameraId].interface_id );
	m_pCamDoRecord->SetValue( m_vectCamDevices[m_nCameraId].do_imaging );
	m_pCamDoGuide->SetValue( m_vectCamDevices[m_nCameraId].do_guide );
	m_pCamDoFocus->SetValue( m_vectCamDevices[m_nCameraId].do_focus );
	m_pCamDoFinder->SetValue( m_vectCamDevices[m_nCameraId].do_finder );
	m_pCamDoSpectroscopy->SetValue( m_vectCamDevices[m_nCameraId].do_spectroscopy );
	m_pCamDoMonitor->SetValue( m_vectCamDevices[m_nCameraId].do_monitor );
}

// Method:	OnCameraActivate
////////////////////////////////////////////////////////////////////
void CInstrumentsCfgDlg::OnCameraActivate( wxCommandEvent& pEvent )
{
	int nState = pEvent.IsChecked();
	SetCameraState( nState );
}

////////////////////////////////////////////////////////////////////
void CInstrumentsCfgDlg::SetCameraState( int nState )
{
	if( nState == 0 )
	{
		m_pCamActivate->SetValue( 0 );
		pCameraPropBox->Disable();
		m_pCamDoRecord->Disable();
		m_pCamDoGuide->Disable();
		m_pCamDoFocus->Disable();
		m_pCamDoFinder->Disable();
		m_pCamDoSpectroscopy->Disable();
		m_pCamDoMonitor->Disable();

	} else if( nState == 1 )
	{
		m_pCamActivate->SetValue( 1 );
		pCameraPropBox->Enable();
		m_pCamDoRecord->Enable();
		m_pCamDoGuide->Enable();
		m_pCamDoFocus->Enable();
		m_pCamDoFinder->Enable();
		m_pCamDoSpectroscopy->Enable();
		m_pCamDoMonitor->Enable();
	}
}

// Method:	OnValidate
////////////////////////////////////////////////////////////////////
void CInstrumentsCfgDlg::OnValidate( wxCommandEvent& pEvent )
{
	GetCameraValues( );

	pEvent.Skip();
	return;
}
