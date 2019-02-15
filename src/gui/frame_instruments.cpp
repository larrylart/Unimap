////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// system headers
#include <tchar.h>
#include <stdio.h>
#include <atlbase.h>
#include <tchar.h>
#include <windows.h>
#include <vector>
#include <list>
#include <limits>
#include <algorithm>
#include <functional>
#include <numeric>
#include <commctrl.h>
#include <shellapi.h>
#include <wia.h>
#include <sti.h>
// system headers
#include <objbase.h>
//#include <atlbase.h>
//#include <gdiplus.h>


// open cv headers
#include "highgui.h"
//#include "_highgui.h"
#include "cv.h"
//#include "cvcam.h"
#include "cvaux.h"
//#include "videoInput.h"

// wx headers
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/gbsizer.h>
#include <wx/filename.h>
#include <wx/listctrl.h>
#include <wx/tglbtn.h>
#include <wx/minifram.h>
#include <wx/spinctrl.h>
#include <wx/image.h>

// local headers
#include "../util/func.h"
#include "../unimap_worker.h"
#include "../logger/logger.h"
#include "../unimap.h"
#include "../sky/star.h"
#include "../sky/sky.h"
#include "../observer/observer.h"
#include "../observer/observer_site.h"
#include "../observer/observer_hardware.h"
#include "../telescope/telescope.h" 
#include "../telescope/telworker.h"
#include "../telescope/focuser.h"
#include "astro_img_view.h"
//#include "camera_view.h"
#include "groupview.h"
#include "RadialProfileView.h"
#include "HistogramView.h"
#include "graph_base_view.h"
#include "../camera/camera.h"
#include "../camera/camera_canoneos.h"
#include "../camera/camera_video.h"
#include "../camera/worker_dcam.h"
#include "../camera/worker_vcam.h"
#include "canoneos_setup_dlg.h"
#include "../image/image_shift.h"
#include "../image/imagedb.h"
#include "../image/imagegroup.h"
#include "../image/astroimage.h"
#include "instruments_cfg_dlg.h"
#include "telescope_setup_dlg.h"
#include "skyobj_select_dlg.h"
#include "waitdialog.h"
#include "custom_wxwidgets.h"
//#include "wx_tglbtn.h"
#include "mmMultiButton.h"

// xpms
#include "../../resources/xpm/camera.xpm"

// main header
#include "frame.h"

////////////////////////////////////////////////////////////////////
// Method:	AddInstrumentsPanel
////////////////////////////////////////////////////////////////////
void CGUIFrame::AddInstrumentsPanel( )
{
	m_pTelescopeWorker = NULL;
	m_pDCameraWorker = NULL;
	m_pVCameraWorker = NULL;
	m_bIsInstrClosing = 0;
	m_nCurrentSiteId = 0;

	// create objects
	m_pProcImgShift = new CImageShift( );

	// default bg 
//	SetBackgroundColour( wxNullColour );

	/////////////////
	// TOP :: GRAPH SIZER
	wxFlexGridSizer* sizerGraph = new wxFlexGridSizer( 3, 1, 0, 0 );
	sizerGraph->AddGrowableRow( 0 );
	sizerGraph->AddGrowableRow( 1 );
	sizerGraph->AddGrowableRow( 2 );
	// histogram box
	wxStaticBox* pHistViewBox = new wxStaticBox( m_pGraphPanel, -1, wxT("Histogram"), wxPoint(-1,-1), wxSize(100,80) );
	wxStaticBoxSizer* sizerHistViewBox = new wxStaticBoxSizer( pHistViewBox, wxHORIZONTAL );
	sizerGraph->Add( sizerHistViewBox, 0, wxGROW | wxEXPAND );
	// graph/cond box
	pCondViewBox = new wxStaticBox( m_pGraphPanel, -1, wxT("Cond: 0°C/0%/0kmph"), wxPoint(-1,-1), wxSize(100,80) );
	wxStaticBoxSizer* sizerCondViewBox = new wxStaticBoxSizer( pCondViewBox, wxHORIZONTAL );
	sizerGraph->Add( sizerCondViewBox, 0, wxGROW | wxEXPAND );
	// FWHM box
	wxStaticBox* pFocusViewBox = new wxStaticBox( m_pGraphPanel, -1, wxT("Fwhm"), wxPoint(-1,-1), wxSize(100,80) );
	wxStaticBoxSizer* sizerFocusViewBox = new wxStaticBoxSizer( pFocusViewBox, wxHORIZONTAL );
	sizerGraph->Add( sizerFocusViewBox, 0, wxGROW | wxEXPAND );
	// add to panel
	m_pGraphPanel->SetSizer( sizerGraph );

	////////////////////////////////
	// BOTTOM :: MAIN :: camera & position/telescope & focuser control sizer
	wxFlexGridSizer* sizerCamPosFocusCtrl = new wxFlexGridSizer( 1, 3, 0, 0 );
	sizerCamPosFocusCtrl->AddGrowableCol( 0 );
	sizerCamPosFocusCtrl->AddGrowableCol( 2 );
	sizerCamPosFocusCtrl->AddGrowableCol( 1 );

	//////////////////////
	// BOTTOM :: CAMERA control box and sizer
	wxStaticBox* pCamCtrlBox = new wxStaticBox( m_pCamTelPanel, -1, wxT("Camera"), wxPoint(-1,-1), wxSize(200,-1) );
	wxStaticBoxSizer* sizerCamCtrlBox = new wxStaticBoxSizer( pCamCtrlBox, wxVERTICAL );
	wxFlexGridSizer* sizerCamData = new wxFlexGridSizer( 3, 1, 0, 0 );
	sizerCamCtrlBox->Add( sizerCamData, 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxALL, 2 );	
	// camera ctrl buttons sizer
	wxFlexGridSizer* sizerCamDataCtrl = new wxFlexGridSizer( 1, 6, 0, 0 );
	//sizerCamDataCtrl->AddGrowableCol( 0 );
	//sizerCamDataCtrl->AddGrowableCol( 4 );
	sizerCamData->Add( sizerCamDataCtrl, 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );
	// camera properties sizer
	wxFlexGridSizer* sizerCamDataProp = new wxFlexGridSizer( 3, 3, 0, 0 );
	sizerCamDataProp->AddGrowableCol( 0 );
	sizerCamDataProp->AddGrowableCol( 2 );
	sizerCamData->Add( sizerCamDataProp, 1, wxGROW|wxTOP, 5 );

	// # add to pos and camera ctrl sizer
	sizerCamPosFocusCtrl->Add( sizerCamCtrlBox, 1, wxGROW | wxEXPAND|wxRIGHT, 2 );

	//////////////////////
	// :: POSITION control box and sizer
	wxFlexGridSizer* sizerPosition = new wxFlexGridSizer( 2, 1, 5, 5 );
//	sizerPosition->AddGrowableCol( 0 );
	wxStaticBox* pPosCtrlBox = new wxStaticBox( m_pCamTelPanel, -1, wxT("Position"), wxPoint(200,-1), wxDefaultSize );
	wxStaticBoxSizer* sizerPosCtrlBox = new wxStaticBoxSizer( pPosCtrlBox, wxVERTICAL );
	sizerPosCtrlBox->Add( sizerPosition, 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );

	/////////////////
	// telescope connect/setup/info
	wxFlexGridSizer* sizerTelCtrlInfoMain = new wxFlexGridSizer( 1, 5, 0, 0 );
	sizerTelCtrlInfoMain->AddGrowableCol( 4 );
	// add to position
	sizerPosition->Add( sizerTelCtrlInfoMain, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxGROW );

	///////
	// telescope control main
	wxFlexGridSizer* sizerTelCtrlMain = new wxFlexGridSizer( 1, 2, 0, 0 );
	sizerTelCtrlMain->AddGrowableCol( 0 );
	// telescope control base
	wxFlexGridSizer* sizerTelCtrlBase = new wxFlexGridSizer( 2, 1, 0, 0 );
	sizerTelCtrlBase->AddGrowableRow( 0 );
	sizerTelCtrlBase->AddGrowableRow( 1 );

	// telescope control base data
	wxFlexGridSizer* sizerTelCtrlBaseData = new wxFlexGridSizer( 1, 6, 0, 0 );
	sizerTelCtrlBaseData->AddGrowableRow( 0 );
	sizerTelCtrlBaseData->AddGrowableCol( 1 );
	sizerTelCtrlBaseData->AddGrowableCol( 3 );
	// telescope control base buttons
	wxFlexGridSizer* sizerTelCtrlBaseButtons = new wxFlexGridSizer( 1, 5, 0, 0 );
	sizerTelCtrlBaseButtons->AddGrowableRow( 0 );
	sizerTelCtrlBaseButtons->AddGrowableCol( 3 );

	// add to base data & buttons
	sizerTelCtrlBase->Add( sizerTelCtrlBaseData, 1, wxALIGN_BOTTOM|wxGROW|wxTOP, 2 );
	sizerTelCtrlBase->Add( sizerTelCtrlBaseButtons, 1, wxALIGN_TOP|wxTOP|wxEXPAND, 5 );

	// telescope navigation sub sizer
	wxFlexGridSizer* sizerTelCtrlNav = new wxFlexGridSizer( 3, 3, 2, 2 );

	// add both to main
	sizerTelCtrlMain->Add( sizerTelCtrlBase, 1, wxLEFT|wxGROW );
	sizerTelCtrlMain->Add( sizerTelCtrlNav, 1, wxLEFT, 10 );
	// add to position
	sizerPosition->Add( sizerTelCtrlMain, 1, wxLEFT  );

	// # add to pos and camera ctrl sizer
	sizerCamPosFocusCtrl->Add( sizerPosCtrlBox, 1,  wxGROW | wxEXPAND|wxRIGHT, 2 );

	//////////////////////
	// FOCUS control box and sizer
	wxStaticBox* pFocusCtrlBox = new wxStaticBox( m_pCamTelPanel, -1, wxT("Focuser"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* sizerFocusCtrlBox = new wxStaticBoxSizer( pFocusCtrlBox, wxVERTICAL );
	wxFlexGridSizer* sizerFocusData = new wxFlexGridSizer( 2,1,5,5 );
	sizerFocusCtrlBox->Add( sizerFocusData, 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxALL, 2 );
	// focus ctrl buttons sizer
	wxFlexGridSizer* sizerFocusDataCtrl = new wxFlexGridSizer( 1, 5, 0, 0 );
	sizerFocusDataCtrl->AddGrowableCol( 0 );
	sizerFocusDataCtrl->AddGrowableCol( 4 );
	sizerFocusData->Add( sizerFocusDataCtrl, 1, wxGROW );
	// focus paramas sizers
	wxFlexGridSizer* sizerFocusDataParam = new wxFlexGridSizer( 2, 1, 5, 0 );
	sizerFocusDataParam->AddGrowableCol( 0 );
//	sizerFocusDataParam->AddGrowableCol( 1 );
	sizerFocusData->Add( sizerFocusDataParam, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxGROW|wxTOP, 3 );

	// # add to pos and camera ctrl sizer
	sizerCamPosFocusCtrl->Add( sizerFocusCtrlBox, 1, wxGROW  );
	// add to panel
	m_pCamTelPanel->SetSizer( sizerCamPosFocusCtrl );

	// :: CREATE GUI ELEMENTS
	////////////////////////////////////////////////////////////////
	// set proc image view to my view
	m_pImgView->m_pProcImgShift = m_pProcImgShift;
	// histogram view
	m_pHistogramView = new CHistogramView( m_pGraphPanel, &m_rHistogram, NULL, true, -1, wxDefaultPosition, 1 );
	// focus graph
	m_pFocusView = new CRadialProfileView( m_pGraphPanel, -1, wxDefaultPosition, 1 );
	// conditions graph
	m_pCondView = new CGraphBaseView( m_pGraphPanel, -1, wxDefaultPosition, 1 );

	///////////////////////
	// :: camera
	//m_pCameraConnect = new wxBitmapButton( m_pCamTelPanel, wxID_INSTR_CAMERA_CONNECT_BUTTON, wxBitmap( wxT("resources/disconnect.png"), wxBITMAP_TYPE_PNG ), wxPoint(-1, -1), wxSize(22,22) );
	m_pCameraConnect = new mmMultiButton( m_pCamTelPanel, wxID_INSTR_CAMERA_CONNECT_BUTTON, 
									wxEmptyString, wxT("Camera connection"),
									wxBitmap( wxT("resources/disconnect.png"), wxBITMAP_TYPE_PNG ), 
									wxPoint(-1, -1), wxSize(22,22), mmMB_NO_AUTOSIZE|mmMB_TOGGLE );

	m_pCameraStart = new wxBitmapButton( m_pCamTelPanel, wxID_INSTR_CTRL_CAMERA_START, wxBitmap( wxT("resources/record.png"), wxBITMAP_TYPE_PNG ), wxPoint(-1, -1), wxSize(22,22) );
	m_pCameraPause = new wxBitmapButton( m_pCamTelPanel, wxID_INSTR_CTRL_CAMERA_PAUSE, wxBitmap( wxT("resources/pause.gif"), wxBITMAP_TYPE_GIF ), wxPoint(-1, -1), wxSize(22,22) );
	m_pCameraStop = new wxBitmapButton( m_pCamTelPanel, wxID_INSTR_CTRL_CAMERA_STOP, wxBitmap( wxT("resources/stop.gif"), wxBITMAP_TYPE_GIF ), wxPoint(-1, -1), wxSize(22,22) );
	m_pCameraSetup = new wxBitmapButton( m_pCamTelPanel, wxID_INSTR_CTRL_CAMERA_SETUP, wxBitmap( wxT("resources/config.png"), wxBITMAP_TYPE_PNG ), wxPoint(-1, -1), wxSize(22,22) );
	m_pCameraFormat = new wxBitmapButton( m_pCamTelPanel, wxID_INSTR_CTRL_CAMERA_FORMAT, wxBitmap( wxT("resources/icon_video.gif"), wxBITMAP_TYPE_GIF ), wxPoint(-1, -1), wxSize(22,22) );
	// :: camera :: exposure
	m_pUseCamExp = new wxCheckBox( m_pCamTelPanel, wxID_INSTR_CTRL_CAMERA_USE_EXP, wxT("") );
	m_pCamExpLabel = new wxStaticText( m_pCamTelPanel, -1, wxT("ETime(s):"));
	m_pCamExpValue = new wxSpinCtrl( m_pCamTelPanel, -1, wxT("0"), wxDefaultPosition, wxSize(70,-1), wxSP_ARROW_KEYS, 0, 60000, 0 );
	// :: camera :: use frames
	m_pUseCamFrames = new wxCheckBox( m_pCamTelPanel, wxID_INSTR_CTRL_CAMERA_USE_FRAMES, wxT("") );
	m_pCamFramesLabel = new wxStaticText( m_pCamTelPanel, -1, wxT("Frames:"));
	m_pCamFramesValue = new wxSpinCtrl( m_pCamTelPanel, -1, wxT("0"), wxDefaultPosition, wxSize(70,-1), wxSP_ARROW_KEYS, 0, 60000, 0 );
	// :: camera :: use Delay
	m_pUseCamDelay = new wxCheckBox( m_pCamTelPanel, wxID_INSTR_CTRL_CAMERA_USE_DELAY, wxT("") );
	m_pCamDelayLabel = new wxStaticText( m_pCamTelPanel, -1, wxT("Delay(s):"));
	m_pCamDelayValue = new wxSpinCtrl( m_pCamTelPanel, -1, wxT("0"), wxDefaultPosition, wxSize(70,-1), wxSP_ARROW_KEYS, 0, 60000, 0 );
	// disable by default
	SetCameraCtrlState( 0 );

	//////////////////////////////
	// position/connect objects
//	m_pTelescopeConnectButton = new wxBitmapToggleButton( m_pCamTelPanel, wxID_INSTR_TELESCOPE_CONNECT_BUTTON, wxBitmap( wxT("resources/disconnect.png"), wxBITMAP_TYPE_PNG ), wxPoint(-1, -1), wxSize(22,22) );
//	m_pTelescopeConnectButton = new wxTTButton( m_pCamTelPanel, wxID_INSTR_TELESCOPE_CONNECT_BUTTON, wxT(""), wxPoint(-1, -1), wxSize(22,22) );
	m_pTelescopeConnectButton = new mmMultiButton( m_pCamTelPanel, wxID_INSTR_TELESCOPE_CONNECT_BUTTON, wxEmptyString, wxT("Telescope connection"),
					wxBitmap( wxT("resources/disconnect.png"), wxBITMAP_TYPE_PNG ), 
					wxPoint(-1, -1), wxSize(22,22), mmMB_NO_AUTOSIZE|mmMB_TOGGLE );

	//	new wxToggleButton( this, wxID_INSTR_TELESCOPE_CONNECT_BUTTON, "Connect", wxPoint(-1, -1), wxSize(-1,-1) );
	m_pTelescopeAlignButton = new wxBitmapButton( m_pCamTelPanel, wxID_INSTR_TELESCOPE_ALIGN_BUTTON, wxBitmap( wxT("resources/align.png"), wxBITMAP_TYPE_PNG ), wxPoint(-1, -1), wxSize(22,22) );
	m_pTelescopeParkButton = new wxBitmapButton( m_pCamTelPanel, wxID_INSTR_TELESCOPE_PARK_BUTTON, wxBitmap( wxT("resources/parking.png"), wxBITMAP_TYPE_PNG ), wxPoint(-1, -1), wxSize(22,22) );
	m_pTelescopeSetupButton = new wxBitmapButton( m_pCamTelPanel, wxID_INSTR_TELESCOPE_SETUP_BUTTON, wxBitmap( wxT("resources/config.png"), wxBITMAP_TYPE_PNG ), wxPoint(-1, -1), wxSize(22,22) );

	// :: LCD INFO SCREEN
	m_pInfoScreen = new wxStaticText( m_pCamTelPanel, -1, wxT(" 00:00:00.000 : +00:00:00.00 "), wxDefaultPosition, wxSize(-1,-1), wxALIGN_CENTRE| wxBORDER_SUNKEN|wxST_NO_AUTORESIZE );
	wxFont infoFont( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD );
	m_pInfoScreen->SetFont( infoFont );
	m_pInfoScreen->SetBackgroundColour( *wxBLACK );
	m_pInfoScreen->SetForegroundColour( *wxRED );

	// coord nav
//	m_pGotoButton = new wxBitmapButton( m_pCamTelPanel, wxID_INSTR_TELESCOPE_GOTO, wxBitmap( wxT("resources/goto.gif"), wxBITMAP_TYPE_GIF ), wxPoint(-1, -1), wxSize(22,22) );
	m_pGotoButton = new mmMultiButton( m_pCamTelPanel, wxID_INSTR_TELESCOPE_GOTO, wxEmptyString, wxT("Slew to target"),
									wxBitmap( wxT("resources/goto.gif"), wxBITMAP_TYPE_GIF ), 
									wxPoint(-1, -1), wxSize(22,22), mmMB_NO_AUTOSIZE|mmMB_TOGGLE );

	m_pTargetPick = new wxBitmapButton( m_pCamTelPanel, wxID_INSTR_TELESCOPE_OBJECT, wxBitmap( wxT("resources/sky_obj.gif"), wxBITMAP_TYPE_GIF ), wxPoint(-1, -1), wxSize(22,22) );
	m_pTargetRa = new wxTextCtrl( m_pCamTelPanel, -1, wxT(""), wxDefaultPosition, wxSize(90,-1) );
	m_pTargetDec = new wxTextCtrl( m_pCamTelPanel, -1, wxT(""), wxDefaultPosition, wxSize(90,-1) );

	// goto/sync buttons
	m_pGuideButton = new mmMultiButton( m_pCamTelPanel, wxID_INSTR_TELESCOPE_GUIDE, wxEmptyString, wxT("Guide Telescope"),
					wxBitmap( wxT("resources/target.png"), wxBITMAP_TYPE_PNG ), wxPoint(-1, -1), wxSize(22,22), mmMB_NO_AUTOSIZE|mmMB_TOGGLE );
	m_pTrackButton = new mmMultiButton( m_pCamTelPanel, wxID_INSTR_TELESCOPE_TRACK, wxEmptyString, wxT("Track Object"),
					wxBitmap( wxT("resources/track.png"), wxBITMAP_TYPE_PNG ), wxPoint(-1, -1), wxSize(22,22), mmMB_NO_AUTOSIZE|mmMB_TOGGLE );
	m_pCircleButton = new mmMultiButton( m_pCamTelPanel, wxID_INSTR_TELESCOPE_CIRCLES, wxEmptyString, wxT("Spiral Search"),
					wxBitmap( wxT("resources/find_spiral.gif"), wxBITMAP_TYPE_GIF ), wxPoint(-1, -1), wxSize(22,22), mmMB_NO_AUTOSIZE|mmMB_TOGGLE );
	// :: telescope scipt functions
	m_pScriptPlayButton = new mmMultiButton( m_pCamTelPanel, wxID_INSTR_TELESCOPE_SCRIPT_PLAY, wxEmptyString, wxT("Play Action Script"),
					wxBitmap( wxT("resources/script_play.gif"), wxBITMAP_TYPE_GIF ), wxPoint(-1, -1), wxSize(22,22), mmMB_NO_AUTOSIZE|mmMB_TOGGLE );
	// script select
	m_pScriptSelect = new wxChoice( m_pCamTelPanel, wxID_INSTR_TELESCOPE_SCRIPT_SELECT, wxDefaultPosition, wxSize(165,-1), 0, 
										NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pScriptSelect->SetSelection(0);
	m_pScriptSelect->Disable();
	
//	m_pUnwrapButton = new wxBitmapButton( this, wxID_INSTR_TELESCOPE_UNWRAP, wxBitmap( "resources/wrap.png", wxBITMAP_TYPE_PNG ), wxPoint(-1, -1), wxSize(22,22) );
//	m_pHiprecButton = new wxBitmapButton( this, wxID_INSTR_TELESCOPE_HIPREC, wxBitmap( "resources/hiprec.gif", wxBITMAP_TYPE_GIF ), wxPoint(-1, -1), wxSize(22,22) );
//	m_pTempCtrlButton = new wxBitmapButton( this, wxID_INSTR_TELESCOPE_TEMPCTRL, wxBitmap( "resources/temp_ctrl.gif", wxBITMAP_TYPE_GIF ), wxPoint(-1, -1), wxSize(22,22) );

	///////////////////
	// navigation buttons
	m_pMoveWest = new wxSTButton( m_pCamTelPanel, wxID_INSTR_MOVE_WEST, wxT("W"), wxPoint(-1, -1), wxSize(20,20) );
	m_pMoveEast = new wxSTButton( m_pCamTelPanel, wxID_INSTR_MOVE_EAST, wxT("E"), wxPoint(-1, -1), wxSize(20,20) );
	m_pMoveNorth = new wxSTButton( m_pCamTelPanel, wxID_INSTR_MOVE_NORTH, wxT("N"), wxPoint(-1, -1), wxSize(20,20) );
	m_pMoveSouth = new wxSTButton( m_pCamTelPanel, wxID_INSTR_MOVE_SOUTH, wxT("S"), wxPoint(-1, -1), wxSize(20,20) );
	m_pMoveStop = new wxSTButton( m_pCamTelPanel, wxID_INSTR_MOVE_STOP, wxT("O"), wxPoint(-1, -1), wxSize(20,20) );

	//////////
	// FOCUSER
//	m_pFocusConnect = new wxSTIButton( m_pCamTelPanel, wxID_INSTR_FOCUS_CONNECT, wxBitmap( wxT("resources/disconnect.png"), wxBITMAP_TYPE_PNG ), wxPoint(-1, -1), wxSize(22,22) );
	m_pFocusConnect = new mmMultiButton( m_pCamTelPanel, wxID_INSTR_FOCUS_CONNECT, wxEmptyString, wxT("Focuser connection"), 
							wxBitmap( wxT("resources/disconnect.png"), wxBITMAP_TYPE_PNG ), 
							wxPoint(-1, -1), wxSize(22,22), mmMB_NO_AUTOSIZE|mmMB_TOGGLE );

	m_pFocusIn = new wxSTIButton( m_pCamTelPanel, wxID_INSTR_FOCUS_IN, wxBitmap( wxT("resources/moveleft.gif"), wxBITMAP_TYPE_GIF ), wxPoint(-1, -1), wxSize(22,22) );
	m_pFocusZero = new wxSTIButton( m_pCamTelPanel, wxID_INSTR_FOCUS_ZERO, wxBitmap( wxT("resources/pin.gif"), wxBITMAP_TYPE_GIF ), wxPoint(-1, -1), wxSize(22,22) );
	m_pFocusOut = new wxSTIButton( m_pCamTelPanel, wxID_INSTR_FOCUS_OUT, wxBitmap( wxT("resources/moveright.gif"), wxBITMAP_TYPE_GIF ), wxPoint(-1, -1), wxSize(22,22) );
	m_pFocusSetup = new wxSTIButton( m_pCamTelPanel, wxID_INSTR_FOCUS_SETUP, wxBitmap( wxT("resources/config.png"), wxBITMAP_TYPE_PNG ), wxPoint(-1, -1), wxSize(22,22) );
	// properties
	m_pFocusUseAuto = new wxCheckBox( m_pCamTelPanel, wxID_INSTR_FOCUS_USE_AUTO, wxT("") );
	m_pFocusUseAutoLabel = new wxStaticText( m_pCamTelPanel, -1, wxT("Auto-Focus Adjustment"));
	m_pFocusUseUnits = new wxCheckBox( m_pCamTelPanel, wxID_INSTR_FOCUS_USE_UNITS, wxT("") );
	m_pFocusUseUnitsLabel = new wxStaticText( m_pCamTelPanel, -1, wxT("Units:"));
	m_pFocusUnits = new wxSpinCtrl( m_pCamTelPanel, -1, wxT("0"), wxDefaultPosition, wxSize(70,-1), wxSP_ARROW_KEYS, -60000, 60000, 0 );
	m_pFocusUseSlope = new wxCheckBox( m_pCamTelPanel, wxID_INSTR_FOCUS_USE_SLOPE, wxT("") );
	m_pFocusUseSlopeLabel = new wxStaticText( m_pCamTelPanel, -1, wxT("Slope:"));
	m_pFocusSlope = new wxSpinCtrl( m_pCamTelPanel, -1, wxT("0"), wxDefaultPosition, wxSize(70,-1), wxSP_ARROW_KEYS, -60000, 60000, 0 );
	// Disable by default
	m_pFocusUseUnitsLabel->Disable();
	m_pFocusUnits->Disable();
	m_pFocusUseSlopeLabel->Disable();
	m_pFocusSlope->Disable();

	// LAYOUT
	////////////////////////////////////////////////////////////////
	// LAYOUT :: camera view
//	sizerCamViewBox->Add( m_pImgView, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW|wxEXPAND );
	// graphs
	sizerHistViewBox->Add( m_pHistogramView, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW|wxEXPAND );
	sizerCondViewBox->Add( m_pCondView, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW|wxEXPAND );
	sizerFocusViewBox->Add( m_pFocusView, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW|wxEXPAND );

	//////////////////////////////////
	// CAMERA
	// :: control buttons
	sizerCamDataCtrl->Add( m_pCameraConnect, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL  );
	sizerCamDataCtrl->Add( m_pCameraStart, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 10  );
	sizerCamDataCtrl->Add( m_pCameraPause, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	sizerCamDataCtrl->Add( m_pCameraStop, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	sizerCamDataCtrl->Add( m_pCameraSetup, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 10 );
	sizerCamDataCtrl->Add( m_pCameraFormat, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 10 );
	// :: exposure length
	sizerCamDataProp->Add( m_pUseCamExp, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL  );
	sizerCamDataProp->Add( m_pCamExpLabel, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL  );
	sizerCamDataProp->Add( m_pCamExpValue, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5  );
	// :: use frames
	sizerCamDataProp->Add( m_pUseCamFrames, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL  );
	sizerCamDataProp->Add( m_pCamFramesLabel, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5  );
	sizerCamDataProp->Add( m_pCamFramesValue, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5  );
	// :: use delay
	sizerCamDataProp->Add( m_pUseCamDelay, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL  );
	sizerCamDataProp->Add( m_pCamDelayLabel, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5  );
	sizerCamDataProp->Add( m_pCamDelayValue, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5  );

	///////////////////////////////////
	// INFO CONNECT/SETUP
	sizerTelCtrlInfoMain->Add( m_pTelescopeConnectButton, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	sizerTelCtrlInfoMain->Add( m_pTelescopeSetupButton, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	sizerTelCtrlInfoMain->Add( m_pTelescopeAlignButton, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 10 );
	sizerTelCtrlInfoMain->Add( m_pTelescopeParkButton, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5  );

	// :: INFO SCRREN
	sizerTelCtrlInfoMain->Add( m_pInfoScreen, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxGROW|wxEXPAND|wxLEFT, 5 );

	//////////////////////////////
	// object/coord nav
	// :: CAT
//	sizerTelCtrlBase->Add( new wxStaticText( this, -1, "CAT:" ), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
//	sizerTelCtrlBase->Add( m_pObjCatSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: OBJ
//	sizerTelCtrlBase->Add( new wxStaticText( this, -1, "OBJ:" ), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
//	sizerTelCtrlBase->Add( m_pObjNameSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	// :: GOTO/PICK BUTTON
	sizerTelCtrlBaseData->Add( m_pGotoButton, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	sizerTelCtrlBaseData->Add( m_pTargetPick, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL|wxLEFT, 5  );
	// :: RA
	m_pTargetRaLabel = new wxStaticText( m_pCamTelPanel, -1, wxT("RA:") );
	sizerTelCtrlBaseData->Add( m_pTargetRaLabel, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL|wxLEFT, 5  );
	sizerTelCtrlBaseData->Add( m_pTargetRa, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: DEC
	m_pTargetDecLabel = new wxStaticText( m_pCamTelPanel, -1, wxT("DEC:") );
	sizerTelCtrlBaseData->Add( m_pTargetDecLabel, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL|wxLEFT, 5  );
	sizerTelCtrlBaseData->Add( m_pTargetDec, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: track guide and find in circles
	sizerTelCtrlBaseButtons->Add( m_pGuideButton, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	sizerTelCtrlBaseButtons->Add( m_pTrackButton, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	sizerTelCtrlBaseButtons->Add( m_pCircleButton, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	// script functions
	sizerTelCtrlBaseButtons->Add( m_pScriptPlayButton, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxLEFT, 10 );
	sizerTelCtrlBaseButtons->Add( m_pScriptSelect, 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	// scope functions
//	sizerTelCtrlBaseButtons->Add( m_pUnwrapButton, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxLEFT, 10 );
//	sizerTelCtrlBaseButtons->Add( m_pHiprecButton, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
//	sizerTelCtrlBaseButtons->Add( m_pTempCtrlButton, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );

	//////////////////
	// add to sizer naviagtion buttons
	// :: row 1
	sizerTelCtrlNav->AddStretchSpacer();
	sizerTelCtrlNav->Add( m_pMoveNorth, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerTelCtrlNav->AddStretchSpacer();
	// :: row 2
	sizerTelCtrlNav->Add( m_pMoveWest, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerTelCtrlNav->Add( m_pMoveStop, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerTelCtrlNav->Add( m_pMoveEast, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: row 3
	sizerTelCtrlNav->AddStretchSpacer();
	sizerTelCtrlNav->Add( m_pMoveSouth, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerTelCtrlNav->AddStretchSpacer();

	////////////////////
	// FOCUSER
	// :: control buttons
	sizerFocusDataCtrl->Add( m_pFocusConnect, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxRIGHT, 10 );
	sizerFocusDataCtrl->Add( m_pFocusIn, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL  );
	sizerFocusDataCtrl->Add( m_pFocusZero, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	sizerFocusDataCtrl->Add( m_pFocusOut, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	sizerFocusDataCtrl->Add( m_pFocusSetup, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxLEFT, 10 );
	// :: use auto
	wxFlexGridSizer* sizerFocusDataParamA = new wxFlexGridSizer( 1, 2, 0, 0 );
	sizerFocusDataParamA->Add( m_pFocusUseAuto, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL  );
	sizerFocusDataParamA->Add( m_pFocusUseAutoLabel, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5  );
	sizerFocusDataParam->Add( sizerFocusDataParamA, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5  );
	// :: use units
	wxFlexGridSizer* sizerFocusDataParamB = new wxFlexGridSizer( 2, 3, 0, 0 );
	sizerFocusDataParamB->Add( m_pFocusUseUnits, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL  );
	sizerFocusDataParamB->Add( m_pFocusUseUnitsLabel, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5  );
	sizerFocusDataParamB->Add( m_pFocusUnits, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5  );
	// :: use slope
	sizerFocusDataParamB->Add( m_pFocusUseSlope, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL  );
	sizerFocusDataParamB->Add( m_pFocusUseSlopeLabel, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5  );
	sizerFocusDataParamB->Add( m_pFocusSlope, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5  );
	sizerFocusDataParam->Add( sizerFocusDataParamB, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
}

////////////////////////////////////////////////////////////////////
// Method:	DeleteInstrumentsPanel
////////////////////////////////////////////////////////////////////
void CGUIFrame::DeleteInstrumentsPanel( )
{
	m_bIsInstrClosing = 1;

	// remove main worker timers
	m_pUnimapWorker->RemoveTimedAction( THREAD_ACTION_TIMED_WEATHER_ONLINE );

	StopTelWorker( );
	StopDCamWorker( );
	StopVCamWorker( );

	/////////////////
	// disconnect cameras - todo: implement for multiple
	int nDevId = m_pCameraSelect->GetSelection();
	
//	if( m_rImage.IsOk() ) m_rImage.Destroy();
	if( m_pProcImgShift ) delete( m_pProcImgShift );
	m_vectCamDevices.clear();

//	delete( pCondViewBox );
	// toolbar
//	delete( m_pCameraSelect );

	// views
//	delete( m_pGroupImgView );
//	delete( m_pImgView );
	delete( m_pHistogramView );
	delete( m_pFocusView );

	// camera
	delete(	m_pCameraConnect );
	delete(	m_pCameraStart );
	delete( m_pCameraPause );
	delete( m_pCameraStop );
	delete( m_pCameraSetup );
	delete( m_pCameraFormat );
	delete( m_pUseCamExp );
	delete( m_pCamExpLabel );
	delete( m_pCamExpValue );
	delete( m_pUseCamFrames );
	delete( m_pCamFramesLabel );
	delete( m_pCamFramesValue );
	delete( m_pUseCamDelay );
	delete( m_pCamDelayLabel );
	delete( m_pCamDelayValue );

	// info
	delete( m_pTelescopeConnectButton );
	delete( m_pTelescopeAlignButton );
	delete( m_pTelescopeParkButton );
	delete( m_pTelescopeSetupButton );
	delete( m_pInfoScreen );

	// object/coord nav
//	delete( m_pObjCatSelect );
//	delete( m_pObjNameSelect );
//	delete( m_pObjNameEntry );
	delete( m_pGotoButton );
	delete( m_pTargetPick );
	delete( m_pTargetRaLabel );
	delete( m_pTargetRa );
	delete( m_pTargetDecLabel );
	delete( m_pTargetDec );
	delete( m_pGuideButton );
	delete( m_pTrackButton );
	delete( m_pCircleButton );
	// script
	delete( m_pScriptPlayButton );
	delete( m_pScriptSelect );
	// func 2
//	delete( m_pUnwrapButton );
//	delete( m_pHiprecButton );
//	delete( m_pTempCtrlButton );
	// navigation buttons
	delete( m_pMoveWest );
	delete( m_pMoveEast );
	delete( m_pMoveNorth );
	delete( m_pMoveSouth );
	delete( m_pMoveStop );
	
	// focuser
	delete( m_pFocusConnect );
	delete( m_pFocusIn );
	delete( m_pFocusZero );
	delete( m_pFocusOut );
	delete( m_pFocusSetup );
	delete( m_pFocusUseAuto );
	delete( m_pFocusUseAutoLabel );
	delete( m_pFocusUseUnits );
	delete( m_pFocusUseUnitsLabel );
	delete( m_pFocusUnits );
	delete( m_pFocusUseSlope );
	delete( m_pFocusUseSlopeLabel );
	delete( m_pFocusSlope );
}

// enable/disable camera control state
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetCameraCtrlState( int bState )
{
	if( bState == 0 )
	{
		m_pCameraConnect->SetDefaultBitmap_noref( wxBitmap( wxT("resources/disconnect.png"), wxBITMAP_TYPE_PNG ) );
		m_pCameraConnect->SetToggleDown(0);

		m_pCameraStart->Disable();
		m_pCameraPause->Disable();
		m_pCameraStop->Disable();
		m_pCameraSetup->Disable();
		m_pCameraFormat->Disable();
		// disable exposure
		SetCameraExpCtrlState( 0 );
		m_pUseCamExp->Disable();
		// disable frames
		SetCameraFramesCtrlState( 0 );
		m_pUseCamFrames->Disable();
		// disable delay
		SetCameraDelayCtrlState( 0 );
		m_pUseCamDelay->Disable();

	} else
	{
		m_pCameraConnect->SetDefaultBitmap_noref( wxBitmap( wxT("resources/connect.png"), wxBITMAP_TYPE_PNG ) );
		m_pCameraConnect->SetToggleDown(1);

		m_pCameraStart->Enable();
		m_pCameraPause->Enable();
		m_pCameraStop->Enable();
		m_pCameraSetup->Enable();
		m_pCameraFormat->Enable();
		// enable exposure
		m_pUseCamExp->Enable();
		SetCameraExpCtrlState( m_pUseCamExp->IsChecked() );
		// enable frames
		m_pUseCamFrames->Enable();
		SetCameraFramesCtrlState( m_pUseCamFrames->IsChecked() );
		// disable delay
		m_pUseCamDelay->Enable();
		SetCameraDelayCtrlState( m_pUseCamDelay->IsChecked() );
	}
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::SetCameraExpCtrlState( int bState )
{
	if( bState )
	{
		m_pCamExpLabel->Enable();
		m_pCamExpValue->Enable();

	} else
	{
		m_pCamExpLabel->Disable();
		m_pCamExpValue->Disable();
	}
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::SetCameraFramesCtrlState( int bState )
{
	if( bState )
	{
		m_pCamFramesLabel->Enable();
		m_pCamFramesValue->Enable();

	} else
	{
		m_pCamFramesLabel->Disable();
		m_pCamFramesValue->Disable();
	}
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::SetCameraDelayCtrlState( int bState )
{
	if( bState )
	{
		m_pCamDelayLabel->Enable();
		m_pCamDelayValue->Enable();

	} else
	{
		m_pCamDelayLabel->Disable();
		m_pCamDelayValue->Disable();
	}
}

////////////////////////////////////////////////////////////////////
// Method:	SetIntrumentsConfig
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetIntrumentsConfig( CTelescope* pTelescope, CFocuser* pFocuser, 
							  CCamera* pCamera, CObserver* pObserver )
{
	int i=0;

	m_pTelescope = pTelescope;
	m_pFocuser = pFocuser;
	m_pCamera = pCamera;
	m_pObserver = pObserver;

	// set default toolbar buttons
//	this->GetToolBar()->ToggleTool( wxID_INSTR_SHOW_CTRL_POSITION, 1 );
	m_pInstrToolBar->ToggleTool( wxID_INSTR_SHOW_CTRL_CAMERA, 1 );
	m_pInstrToolBar->ToggleTool( wxID_INSTR_SHOW_CTRL_FOCUS, 1 );
	m_pInstrToolBar->ToggleTool( wxID_INSTR_SHOW_CTRL_GRAPH, 1 );
	// set default flags
	m_bInstrCameraView = 1;

	// set camera
	m_pCameraSelect->Clear();
	m_vectCamDevices.clear();

	for( i=0; i<m_pCamera->m_vectHWDevices.size(); i++ )
	{
		if( m_pCamera->m_vectHWDevices[i].connected )
		{
			m_vectCamDevices.push_back( m_pCamera->m_vectHWDevices[i] );
			m_pCameraSelect->Append( m_pCamera->m_vectHWDevices[i].strName );
		}
	}
	// check if there are any cameras
	if( m_pCameraSelect->GetCount() > 0 )
	{
		// set default selection and type of panel?
		m_pCameraSelect->SetSelection(0);
		m_pProcImgShift->SetCameraSource(0);
		m_nCurrentCamera = 0;

	} else
	{
		m_pInstrToolBar->EnableTool( wxID_INSTR_SHOW_CTRL_CAMERA, 0 );
		m_pCameraSelect->Disable();
		m_pCameraConnect->Disable();
		m_nCurrentCamera = -1;
	}

	// set telescope view
	SetTelescopeRaDecEntry( 0.0, 0.0 );
	SetGuiOnTelescopeState( 0 );

	///////////////
	// focuser
	m_pFocusConnect->SetToggleDown(0);
	if( m_pFocuser->m_nFocuserInterface == 0 )
		m_pFocusConnect->Enable(0);
	else
		m_pFocusConnect->Enable(1);

	//////////////
	// start conditions graph from main thread
	m_pCondView->SetConfig( 3 );

	// activate timed action to fetch weather data
	DefCmdTimedAction act;
	act.id = THREAD_ACTION_TIMED_WEATHER_ONLINE;
	act.time_type = 0;
	act.time_int = 5*1000;
	act.last_time = 0;
	// set site id
	act.vectInt[0] = m_nCurrentSiteId;
	// set current handler
	act.handler = this->GetEventHandler();
	// set timed action
	m_pUnimapWorker->SetTimedAction( act );
}

/*
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetThumbnails( )
{
	int i=0;

	m_pGroupImgView->ClearAll();

	int nImgNo = m_pCamera->m_vectCameraFrames.size();
	wxImageList* imageList = new wxImageList( 120, 100, true, nImgNo );

	for( i=0; i<nImgNo; i++ )
	{
		int id = nImgNo-1-i;
		wxFileName myfile( m_pCamera->m_vectCameraFrames[id].file_name );

		imageList->Add( wxBitmap( wxImage( m_pCamera->m_vectCameraFrames[id].icon_filename ) ) );
		m_pGroupImgView->InsertItem( i, myfile.GetName() );
		m_pGroupImgView->SetItemImage( i, i );
	}

	m_pGroupImgView->AssignImageList( imageList, wxIMAGE_LIST_NORMAL );
}
*/

////////////////////////////////////////////////////////////////////
void CGUIFrame::SetHistogram( )
{
	if( !m_pImgView->m_pImage ) return;

	// cal hist
	m_rHistogram.ComputeHistogram( *(m_pImgView->m_pImage) );
	m_pHistogramView->UpdateHistogram( &m_rHistogram );
}

/*
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnThumbs( wxCommandEvent& pEvent )
{
	if( pEvent.IsChecked( ) )
	{
		m_pGroupImgView->Show();

	} else
	{
		m_pGroupImgView->Hide();
	}

	GetSizer()->Layout();
	Layout();
	Fit();

	Update();
	Refresh(false);
}
*/

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnActivateCameraView( wxCommandEvent& pEvent )
{
	int nState = pEvent.IsChecked();
	int nDevId = m_pCameraSelect->GetSelection();
	// check if to activate or not
	if( nState )
	{
		m_pCameraSelect->Enable();
		m_bInstrCameraView = 1;
		// check if camera active
		if( m_pCamera->IsConnected( m_vectCamDevices[nDevId].own_id ) )
		{
			// uload image 
			UnloadSelectedImage( 1 );
		}

	} else
	{
		m_pCameraSelect->Disable();
		m_bInstrCameraView = 0;
	}
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnSelectCamera( wxCommandEvent& pEvent )
{
	// set current camera to not update gui 
	if( m_nCurrentCamera >= 0 ) 
		m_pCamera->SetGuiUpdate( m_vectCamDevices[m_nCurrentCamera].own_id, 0 );

	m_nCurrentCamera = m_pCameraSelect->GetSelection();
	// set next camera to update gui
	m_pCamera->SetGuiUpdate( m_vectCamDevices[m_nCurrentCamera].own_id, 1 );
	// set new camera source for the tracker
	m_pProcImgShift->SetCameraSource( m_nCurrentCamera );

	// set camera panel state as if connected
	SetCameraCtrlState( m_pCamera->IsConnected( m_vectCamDevices[m_nCurrentCamera].own_id ) );
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnInstrSettings( wxCommandEvent& pEvent )
{
	CInstrumentsCfgDlg* pDlg = new CInstrumentsCfgDlg( this );
	pDlg->SetConfig( m_pUnimap->m_pTelescope, m_pUnimap->m_pFocuser, m_pUnimap->m_pCamera );
	// check if return is ok
	if( pDlg->ShowModal( ) == wxID_OK )
	{
		SetInstrumentsSettings( pDlg );
	}
	// delete object
	delete( pDlg );

	GetToolBar()->ToggleTool( wxID_INSTR_SHOW_CTRL_SETTINGS, 0 );

	return;
}

////////////////////////////////////////////////////////////////////
// Purpose:	when scope button pressed
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnScope( wxCommandEvent& pEvent )
{
	m_pTelescope->SetTelescope();

	return;
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnTelescopeSetup( wxCommandEvent& pEvent )
{
	CTelescopeSetupDlg* pDlg = new CTelescopeSetupDlg( this, m_pTelescope, wxT("Telescope Setup") );
	pDlg->SetConfig( m_pObserver, m_pCamera, m_pUnimapWorker );
	// show
	if( pDlg->ShowModal() == wxID_OK  )
	{
		// update here telescope setup
		m_nCurrentSiteId = pDlg->m_pSiteNameSelect->GetSelection();
	}
	delete( pDlg );
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnTelescopeObjSet( wxCommandEvent& pEvent )
{
	CSkyObjSelectDlg* pDlg = new CSkyObjSelectDlg( this, wxT("Select Sky Object/Source") );
	pDlg->m_pMainConfig = m_pMainConfig;
	pDlg->SetConfig( m_pSky );
	// show
	if( pDlg->ShowModal() == wxID_OK  )
	{
		// get ra/dec
		m_nTelescopeTargetRa = pDlg->m_nRaDeg;
		m_nTelescopeTargetDec = pDlg->m_nDecDeg;
		// set entry
		SetTelescopeRaDecEntry( pDlg->m_nRaDeg, pDlg->m_nDecDeg );
	}
	delete( pDlg );
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::SetTelescopeRaDecEntry( double ra, double dec )
{
	wxString strRa=wxT("");
	wxString strDec=wxT("");

	// by entry type
	if( m_pMainConfig->m_nSkyCoordEntryFormat == UNITS_FORMAT_SEXAGESIMAL )
	{
		// set ra/dec in Sexgesimal
		RaDegToSexagesimal( ra, strRa );
		DecDegToSexagesimal( dec, strDec );

	} else
	{
		strRa.Printf( wxT("%.6f"), ra );
		strDec.Printf( wxT("%.6f"), dec );
	}
	// set in gui
	m_pTargetRa->SetValue( strRa );
	m_pTargetDec->SetValue( strDec );
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnTelescopeConnect( wxCommandEvent& pEvent )
{
	if( !m_pTelescope->IsConnected() )
	{
//		m_pTelescopeConnectButton->SetLabel( wxT("Connecting...") );

		// and now start thread worker
		StartTelWorker( );

		// now set command for worker
		DefCmdAction cmd;
		cmd.id = THREAD_TELESCOPE_ACTION_CONNECT;
		m_pTelescopeWorker->SetAction( cmd );

	} else
	{
//		m_pTelescopeConnectButton->SetLabel( wxT("Closing...") );
		// now set command for worker
		DefCmdAction cmd;
		cmd.id = THREAD_TELESCOPE_ACTION_DISCONNECT;
		m_pTelescopeWorker->SetAction( cmd );
	}

	return;
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnUpdateInfo( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();

	if( nId == wxID_TELESCOPE_INFO_CONNECTED )
	{
		//m_pTelescopeConnectButton->SetLabel( wxT("Disconnect") );
//		m_pTelescopeConnectButton->SetBitmapLabel( wxBitmap( wxT("resources/connect.png"), wxBITMAP_TYPE_PNG ) );
		m_pTelescopeConnectButton->SetToggleDown(1);
		m_pTelescopeConnectButton->SetDefaultBitmap_noref( wxBitmap( wxT("resources/connect.png"), wxBITMAP_TYPE_PNG ) );
		//m_pTelescopeConnectButton->SetValue( 0 );
		// enable gui on telescope connect
		SetGuiOnTelescopeState( 1 );

		// also set the focuser if telescope driver
		if( m_pFocuser->m_nFocuserInterface == 0 )
		{
			m_pFocusConnect->SetDefaultBitmap_noref( wxBitmap( wxT("resources/connect.png"), wxBITMAP_TYPE_PNG ) );
			m_pFocusConnect->SetToggleDown(1);
			SetGuiFocuserPanelState(1);
		}


	} else if( nId == wxID_TELESCOPE_INFO_DISCONNECTED )
	{
		//m_pTelescopeConnectButton->SetLabel( wxT("Connect") );
//		m_pTelescopeConnectButton->SetBitmapLabel( wxBitmap( wxT("resources/disconnect.png"), wxBITMAP_TYPE_PNG ) );
		m_pTelescopeConnectButton->SetToggleDown(0);
		m_pTelescopeConnectButton->SetDefaultBitmap_noref( wxBitmap( wxT("resources/disconnect.png"), wxBITMAP_TYPE_PNG ) );
		//m_pTelescopeConnectButton->SetValue( 0 );
		// now stop worker
		StopTelWorker();
		// disable gui on telescope disconnect
		SetGuiOnTelescopeState( 0 );

		// also set the focuser if telescope driver
		if( m_pFocuser->m_nFocuserInterface == 0 )
		{
			m_pFocusConnect->SetDefaultBitmap_noref( wxBitmap( wxT("resources/disconnect.png"), wxBITMAP_TYPE_PNG ) );
			m_pFocusConnect->SetToggleDown(0);
			SetGuiFocuserPanelState(0);
		}

	} else if( nId == wxID_TELESCOPE_INFO_UPDATE )
	{
		SetTelescopeInfo( m_pTelescope->m_nCurrentRa, m_pTelescope->m_nCurrentDec );
		m_pTelescopeWorker->m_bUpdateTelescopeInfo = 1;

	}
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::SetGuiOnTelescopeState( int bState )
{
	if( bState == 0 )
	{
		m_pTelescopeAlignButton->Disable();
		m_pTelescopeParkButton->Disable();
		m_pTelescopeSetupButton->Disable();
		m_pGotoButton->Disable();
		m_pTargetPick->Disable();
		m_pInfoScreen->Disable();
		m_pTargetRaLabel->Disable();
		m_pTargetRa->Disable();
		m_pTargetDecLabel->Disable();
		m_pTargetDec->Disable();
		m_pTrackButton->Enable(0);
		m_pGuideButton->Enable(0);
		m_pCircleButton->Enable(0);
		m_pScriptPlayButton->Enable(0);
		m_pScriptSelect->Disable();
		m_pMoveWest->Disable();
		m_pMoveEast->Disable();
		m_pMoveNorth->Disable();
		m_pMoveSouth->Disable();
		m_pMoveStop->Disable();

	} else
	{
		m_pTelescopeAlignButton->Enable();
		m_pTelescopeParkButton->Enable();
		m_pTelescopeSetupButton->Enable();
		m_pGotoButton->Enable(1);
		m_pTargetPick->Enable();
		m_pInfoScreen->Enable();
		m_pTargetRaLabel->Enable();
		m_pTargetRa->Enable();
		m_pTargetDecLabel->Enable();
		m_pTargetDec->Enable();
		m_pTrackButton->Enable(1);
		m_pGuideButton->Enable(1);
		m_pCircleButton->Enable(1);
		m_pScriptPlayButton->Enable(1);
//		m_pScriptSelect->Enable();
		m_pMoveWest->Enable();
		m_pMoveEast->Enable();
		m_pMoveNorth->Enable();
		m_pMoveSouth->Enable();
		m_pMoveStop->Enable();
	}

	// check type of focuser interface
	if( m_pFocuser->m_nFocuserInterface == FOCUSER_INTERFACE_TELESCOPE )
	{
		SetGuiFocuserPanelState( bState );
	}
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::SetGuiFocuserPanelState( int nState )
{
	// check if to enable or disable
	if( nState )
	{
		m_pFocusIn->Enable();
		m_pFocusZero->Enable();
		m_pFocusOut->Enable();
		m_pFocusSetup->Enable();
		m_pFocusUseAuto->Enable();
		m_pFocusUseUnitsLabel->Enable();
		m_pFocusUseUnits->Enable();
		m_pFocusUseSlope->Enable();

	} else
	{
		m_pFocusIn->Disable();
		m_pFocusZero->Disable();
		m_pFocusOut->Disable();
		m_pFocusSetup->Disable();
		m_pFocusUseAuto->Disable();
		m_pFocusUseUnitsLabel->Disable();
		m_pFocusUseUnits->Disable();
		m_pFocusUseSlope->Disable();
	}

	// check option state
	SetFocuserPanelAutoState( (int) m_pFocusUseAuto->GetValue() );
	SetFocuserPanelUnitsState( (int) m_pFocusUseUnits->GetValue() );
	SetFocuserPanelSlopeState( (int) m_pFocusUseSlope->GetValue() );
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::SetTelescopeInfo( double ra, double dec )
{
	wxString strTmp=wxT("");

	// :: ra/dec
	if( m_pMainConfig->m_nSkyCoordDisplayFormat == UNITS_FORMAT_SEXAGESIMAL )
	{
		wxString strRa=wxT("");
		wxString strDec=wxT("");
		RaDegToSexagesimal( ra, strRa );
		DecDegToSexagesimal( dec, strDec );
		strTmp.Printf( wxT(" %s : %s "), strRa, strDec );
		
	} else
	{
		strTmp.Printf( wxT(" %.6f : %.6f "), ra, dec );
	}

	m_pInfoScreen->SetLabel( strTmp );
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::StartTelWorker( )
{
	m_pTelescopeWorker = new CTelescopeWorker( this, m_pLogger );
	m_pTelescopeWorker->SetConfig( m_pTelescope, m_pFocuser );

	if ( m_pTelescopeWorker->Create() != wxTHREAD_NO_ERROR )
	{
//		m_pLogger->Add( "ERROR :: worker thread initialization failure\n" );
	} else
	{
//		m_pLogger->Add( "INFO :: thread initialization ok\n" );
	}
	// set local pointers
//	m_pTelescopeWorker->m_pUnimap = m_pUnimap;

	// start the thread
	if ( m_pTelescopeWorker->Run() != wxTHREAD_NO_ERROR )
	{
//		m_pLogger->Add( "ERROR :: worker thread running failure\n" );
	} else
	{
//		m_pLogger->Add( "INFO :: worker thread running ok\n" );
	}

	return;
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::StopTelWorker( )
{
	if( m_pTelescopeWorker )
	{
		m_pTelescopeWorker->Delete( );
		//delete( m_pTelescopeWorker );
		m_pTelescopeWorker = NULL;
	}
}

// Method:	OnTelescopeGoto
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnTelescopeGoto( wxCommandEvent& pEvent )
{
	SlewToPosition();
}

// Method:	OnTelescopeGoto
////////////////////////////////////////////////////////////////////
void CGUIFrame::SlewToPosition( int bRemote )
{
	if( !m_pTelescopeWorker ) return;

	// get coordinates
	double ra=0.0, dec=0.0;
	wxString strRa = m_pTargetRa->GetLineText(0);
	wxString strDec = m_pTargetDec->GetLineText(0);
	// convert by type
	if( m_pMainConfig->m_nSkyCoordEntryFormat == UNITS_FORMAT_SEXAGESIMAL )
	{
		if( !ConvertRaToDeg( strRa, ra ) ) return;
		if( !ConvertDecToDeg( strDec, dec ) ) return;

	} else
	{
		if( !strRa.ToDouble( &ra ) ) return;
		if( !strDec.ToDouble( &dec ) ) return;
	}

	// if call from remote window set button toggle down
	if( bRemote ) m_pGotoButton->SetToggleDown(1);

	// send telescope goto position command
	DefCmdAction cmd;
	cmd.id = THREAD_TELESCOPE_ACTION_GOTO;
	cmd.vectFloat[0] = ra;
	cmd.vectFloat[1] = dec;

	/////////////////
	// show wait dialog
	CWaitDialog* pWaitDialog = new CWaitDialog( this, wxT("Telescope Process"), 
						wxT("Slew to position ..."), 0, m_pTelescopeWorker, &cmd, WAITDLG_WORKER_TEL );
	pWaitDialog->ShowModal();
	// delete/reset wait dialog
	delete( pWaitDialog );

	// set button back on
	m_pGotoButton->SetToggleDown(0);
}

// Method:	OnTelescopeMove
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnTelescopeMove( wxCommandEvent& pEvent )
{
	if( !m_pTelescopeWorker ) return;

	int nId = pEvent.GetId();

	// check by id
	if( nId == wxID_INSTR_MOVE_WEST ) 
	{
		if( m_pMoveWest->m_bGoingDown == 1 )
			m_pTelescopeWorker->StartMovingWest();
		else
			m_pTelescopeWorker->StopSlew();

	} else if( nId == wxID_INSTR_MOVE_EAST )
	{
		if( m_pMoveEast->m_bGoingDown == 1 )
			m_pTelescopeWorker->StartMovingEast();
		else
			m_pTelescopeWorker->StopSlew();

	} else if( nId == wxID_INSTR_MOVE_NORTH )
	{
		if( m_pMoveNorth->m_bGoingDown == 1 )
			m_pTelescopeWorker->StartMovingNorth();
		else
			m_pTelescopeWorker->StopSlew();

	} else if( nId == wxID_INSTR_MOVE_SOUTH )
	{
		if( m_pMoveSouth->m_bGoingDown == 1 )
			m_pTelescopeWorker->StartMovingSouth();
		else
			m_pTelescopeWorker->StopSlew();
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnFocusOptions
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnFocusOptions( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();
	int nState = pEvent.IsChecked();
	// check id
	if( nId == wxID_INSTR_FOCUS_USE_AUTO )
		SetFocuserPanelAutoState( nState );
	else if( nId == wxID_INSTR_FOCUS_USE_UNITS )
		SetFocuserPanelUnitsState( nState );
	else if( nId == wxID_INSTR_FOCUS_USE_SLOPE )
		SetFocuserPanelSlopeState( nState );
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::SetFocuserPanelAutoState( int nState )
{
	if( nState )
		m_pFocusUseAutoLabel->Enable();
	else
		m_pFocusUseAutoLabel->Disable();
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::SetFocuserPanelUnitsState( int nState )
{
	if( nState )
	{
		m_pFocusUseUnitsLabel->Enable();
		m_pFocusUnits->Enable();

	} else
	{
		m_pFocusUseUnitsLabel->Disable();
		m_pFocusUnits->Disable();
	}
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::SetFocuserPanelSlopeState( int nState )
{
	if( nState )
	{
		m_pFocusUseSlopeLabel->Enable();
		m_pFocusSlope->Enable();

	} else
	{
		m_pFocusUseSlopeLabel->Disable();
		m_pFocusSlope->Disable();
	}
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnFocuserConnect( wxCommandEvent& pEvent )
{
	if( !m_pFocuser->IsConnected() )
	{
		m_pFocuser->Connect();
		m_pFocusConnect->SetDefaultBitmap_noref( wxBitmap( wxT("resources/connect.png"), wxBITMAP_TYPE_PNG ) );
		SetGuiFocuserPanelState(1);

	} else
	{
		m_pFocuser->Disconnect();
		m_pFocusConnect->SetDefaultBitmap_noref( wxBitmap( wxT("resources/disconnect.png"), wxBITMAP_TYPE_PNG ) );
		SetGuiFocuserPanelState(0);
	}
}

// handle focus in/out/zero buttons
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnFocuserMove( wxCommandEvent& pEvent )
{
	if( !m_pTelescopeWorker ) return;

	// get source event id
	int nId = pEvent.GetId();
	// check by button 
	if( nId == wxID_INSTR_FOCUS_IN )
	{
		if( m_pFocusIn->m_bGoingDown == 1 )
			m_pTelescopeWorker->StartFocusIn();
		else
			m_pTelescopeWorker->StopFocus();

	} else if( nId == wxID_INSTR_FOCUS_OUT )
	{
		if( m_pFocusOut->m_bGoingDown == 1 )
			m_pTelescopeWorker->StartFocusOut();
		else
			m_pTelescopeWorker->StopFocus();
	}

}

////////////////////////////////////////////////////////////////////
// Method:	OnCameraOptions
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnCameraOptions( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();
	int nState = pEvent.IsChecked();
	// check id
	if( nId == wxID_INSTR_CTRL_CAMERA_USE_EXP )
		SetCameraExpCtrlState( nState );
	else if( nId == wxID_INSTR_CTRL_CAMERA_USE_FRAMES )
		SetCameraFramesCtrlState( nState );
	else if( nId == wxID_INSTR_CTRL_CAMERA_USE_DELAY )
		SetCameraDelayCtrlState( nState );
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnCameraConnect( wxCommandEvent& pEvent )
{
	int nDevId = m_pCameraSelect->GetSelection();

	// tocheck: test here if it works in all cases
	if( !m_pCamera->IsConnected( m_vectCamDevices[nDevId].own_id ) )
	{
		//m_pCameraConnect->SetLabel( wxT("Connecting...") );
		// initialize
		m_pCamera->Init( m_vectCamDevices[nDevId].own_id );

		///////////////////////////
		// BY CAMERA TYPE DIGITAL/VIDEO
		if( m_vectCamDevices[nDevId].wia_type == 2 )
		{
			// and now start thread worker
			StartDCamWorker( );
			// also set camera worker
			m_pCamera->m_pDCameraWorker = m_pDCameraWorker;

			// now set command for worker
			DefCmdAction cmd;
			cmd.id = THREAD_DCAMERA_ACTION_CONNECT;
			cmd.vectInt[0] = m_vectCamDevices[nDevId].own_id;
			//m_pDCameraWorker->SetAction( cmd );

			/////////////////
			// show wait dialog
			CWaitDialog* pWaitDialog = new CWaitDialog( this, wxT("Digital Camera"), 
								wxT("Establish connection ..."), 0, m_pDCameraWorker, &cmd, WAITDLG_WORKER_DCAM );
			pWaitDialog->ShowModal();
			// delete/reset wait dialog
			delete( pWaitDialog );

			// after this set connect
			//m_pCameraConnect->SetLabel( wxT("Disconnect") );
			m_pCameraConnect->SetDefaultBitmap_noref( wxBitmap( wxT("resources/connect.png"), wxBITMAP_TYPE_PNG ) );
			// enable buttons
			m_pCameraStart->Enable();
			m_pCameraPause->Enable();
			m_pCameraStop->Enable();
			m_pCameraSetup->Enable();
			m_pCameraFormat->Enable();

			// enable gui on camera connect -- this is for video todo: ??
//			if( m_vectCamDevices[nDevId].wia_type == 3 )
//				m_pCamera->m_pCameraVideo->SetConfig( m_pCamera, this );

		} else if( m_vectCamDevices[nDevId].wia_type == 3 )
		{
			UnloadSelectedImage( 1 );
			m_pImgView->m_bIsCameraVideo = 1;
			// and now start thread worker
			StartVCamWorker( );
			// also set camera worker
			m_pCamera->m_pVCameraWorker = m_pVCameraWorker;

			// enable buttons
			m_pCameraSetup->Enable();
			m_pCameraFormat->Enable();

			// now set command for worker
			DefCmdAction cmd;
			cmd.id = THREAD_VCAMERA_ACTION_CONNECT;
			cmd.vectInt[0] = m_vectCamDevices[nDevId].own_id;
			m_pVCameraWorker->SetAction( cmd );
		}

	} else
	{
		//m_pCameraConnect->SetLabel( wxT("Closing...") );

		///////////////////////////
		// BY CAMERA TYPE DIGITAL/VIDEO
		if( m_vectCamDevices[nDevId].wia_type == 2 )
		{
			// now set command for worker
			DefCmdAction cmd;
			cmd.id = THREAD_DCAMERA_ACTION_DISCONNECT;
			m_pDCameraWorker->SetAction( cmd );

		} else if( m_vectCamDevices[nDevId].wia_type == 3 )
		{
			// now set command for worker
			DefCmdAction cmd;
			cmd.id = THREAD_VCAMERA_ACTION_DISCONNECT;
			m_pVCameraWorker->SetAction( cmd );
		}
	}

	return;
}

// Digital camera thread
////////////////////////////////////////////////////////////////////
void CGUIFrame::StartDCamWorker( )
{
	m_pDCameraWorker = new CDCameraWorker( this, m_pLogger );
	m_pDCameraWorker->SetConfig( m_pCamera );

	if ( m_pDCameraWorker->Create() != wxTHREAD_NO_ERROR )
	{
//		m_pLogger->Add( "ERROR :: worker thread initialization failure\n" );
	} else
	{
//		m_pLogger->Add( "INFO :: thread initialization ok\n" );
	}
	// set local pointers
//	m_pTelescopeWorker->m_pUnimap = m_pUnimap;

	// start the thread
	if ( m_pDCameraWorker->Run() != wxTHREAD_NO_ERROR )
	{
//		m_pLogger->Add( "ERROR :: worker thread running failure\n" );
	} else
	{
//		m_pLogger->Add( "INFO :: worker thread running ok\n" );
	}

	return;
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::StopDCamWorker( )
{
	if( m_pDCameraWorker )
	{
		m_pDCameraWorker->Delete( );
		//delete( m_pTelescopeWorker );
		m_pDCameraWorker = NULL;
	}
}

// Video camera thread
////////////////////////////////////////////////////////////////////
void CGUIFrame::StartVCamWorker( )
{
	m_pVCameraWorker = new CVCameraWorker( this, m_pLogger );
	m_pVCameraWorker->SetConfig( m_pCamera );

	if ( m_pVCameraWorker->Create() != wxTHREAD_NO_ERROR )
	{
//		m_pLogger->Add( "ERROR :: worker thread initialization failure\n" );
	} else
	{
//		m_pLogger->Add( "INFO :: thread initialization ok\n" );
	}
	// set local pointers
//	m_pTelescopeWorker->m_pUnimap = m_pUnimap;

	// start the thread
	if ( m_pVCameraWorker->Run() != wxTHREAD_NO_ERROR )
	{
//		m_pLogger->Add( "ERROR :: worker thread running failure\n" );
	} else
	{
//		m_pLogger->Add( "INFO :: worker thread running ok\n" );
	}

	return;
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::StopVCamWorker( )
{
	if( m_pVCameraWorker )
	{
		// set view type to non video
		m_pImgView->m_bIsCameraVideo = 0;

		m_pVCameraWorker->Delete( );
		//delete( m_pTelescopeWorker );
		m_pVCameraWorker = NULL;
	}
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnCameraUpdateInfo( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();
	int nDevId = m_pCameraSelect->GetSelection();

	if( nId == wxID_DCAMERA_INFO_CONNECTED )
	{
		//m_pCameraConnect->SetLabel( wxT("Disconnect") );
		//m_pCameraConnect->SetValue( 0 );
		m_pCameraConnect->SetDefaultBitmap_noref( wxBitmap( wxT("resources/connect.png"), wxBITMAP_TYPE_PNG ) );

		// enable gui on camera connect
//		m_pCamera->SetGuiUpdate( m_vectCamDevices[m_nCurrentCamera].own_id, this, 1 );
		//if( m_vectCamDevices[nDevId].wia_type == 3 )
		//	m_pCamera->m_pCameraVideo->SetConfig( m_pCamera, this );

	} else if( nId == wxID_DCAMERA_INFO_DISCONNECTED )
	{
		//m_pCameraConnect->SetLabel( wxT("Connect") );
		//m_pCameraConnect->SetValue( 0 );
		m_pCameraConnect->SetDefaultBitmap_noref( wxBitmap( wxT("resources/disconnect.png"), wxBITMAP_TYPE_PNG ) );

		// enable buttons
		m_pCameraStart->Disable();
		m_pCameraPause->Disable();
		m_pCameraStop->Disable();
		m_pCameraSetup->Disable();
		m_pCameraFormat->Disable();

		// now stop worker by type 2=digital, 3=video
		if( m_vectCamDevices[nDevId].wia_type == 2 )
			StopDCamWorker();
		else if( m_vectCamDevices[nDevId].wia_type == 3 )
			StopVCamWorker();

		// disconnect camera - this because the init was done via gui
		m_pCamera->Disconnect( m_vectCamDevices[nDevId].own_id );

		// disable gui on camera disconnect

	} else if( nId == wxID_DCAMERA_INFO_PHOTO_TAKEN )
	{
		// set here button back and ????

	} else if( nId == wxID_DCAMERA_GOT_NEW_IMAGE )
	{
		// set last image
		int nLastFrame = m_pCamera->m_vectCameraFrames.size()-1;
		// add to current group
		int nImageId = m_pImageDb->m_pCurrentGroup->AddImage( m_pCamera->m_vectCameraFrames[nLastFrame].file_name );
		// add to select 
		m_pImageSelect->Append( m_pImageDb->m_pCurrentGroup->m_vectImageNode[nImageId]->m_strImageName );

		// update thumbnails if visible
		if( m_pMainConfig->m_bShowImageThumbs )
		{
			int nSelectGroup = m_pGroupSelect->GetSelection();
			UpdateGroupThumbnails( nSelectGroup );
		}

		// select image in group
		if( m_bGroupThumbnailsInitialized ) m_pGroupImgView->SelectImage( nImageId );
		m_pImageSelect->SetSelection( nImageId+1 );
		LoadSelectedImage( 1 );

/*
		m_rImage.LoadFile( m_pCamera->m_vectCameraFrames[nLastImgId].file_name );
		// create Thumbnail
		wxImage imgIcon = m_rImage.Scale( 120, 100 , wxIMAGE_QUALITY_HIGH );
		// make icon file name
		wxString strFile;
		wxFileName myfile( m_pCamera->m_vectCameraFrames[nLastImgId].file_name );
		strFile.Printf( wxT("%s/%s_icon.jpg"), myfile.GetPath(), myfile.GetName() );
		m_pCamera->m_vectCameraFrames[nLastImgId].icon_filename = strFile;
		// save icon
		imgIcon.SaveFile( strFile );

		m_pImgView->SetWxImage( &m_rImage );
*/
		// todo: call here set camera image instead
		IplImage* pIplImage = WxImageToIplImage( *(m_pImgView->m_pImage) );
		SetCameraImage( m_pImgView->m_pImage, pIplImage, 0 );
		cvReleaseImage( &pIplImage );

//		SetHistogram( );
//		SetThumbnails( );
	}
}

// Method:	OnCameraAction
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnCameraAction( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();
	int nDevId = m_pCameraSelect->GetSelection();

	// check by id
	if( nId == wxID_INSTR_CTRL_CAMERA_START ) 
	{
		// if digital snap
		if( m_vectCamDevices[nDevId].wia_type == 2 )
		{
			//////////////
			// here if view use progress bar - if view active and no processing
/*			DefCmdAction cmd;
			cmd.id = THREAD_DCAMERA_ACTION_TAKE_PHOTO;
			cmd.vectInt[0] = m_vectCamDevices[nDevId].own_id;
			cmd.vectStr[0] = m_pImageDb->m_pCurrentGroup->m_strGroupPath;

			/////////////////
			// show wait dialog
			CWaitDialog* pWaitDialog = new CWaitDialog( this, wxT("Digital Camera"), 
								wxT("Take Photo ..."), 1, m_pDCameraWorker, &cmd, WAITDLG_WORKER_DCAM );
			pWaitDialog->ShowModal();
			// delete/reset wait dialog
			delete( pWaitDialog );
*/
			/////////
			// if not to use progress bar
			m_pDCameraWorker->TakePhoto( m_vectCamDevices[nDevId].own_id,
							m_pImageDb->m_pCurrentGroup->m_strGroupPath );
		}
	}

}

// Method:	OnCameraSetup
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnCameraSetup( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();
	int nDevId = m_pCameraSelect->GetSelection();

	// digital camera snap setup
	if( m_vectCamDevices[nDevId].wia_type == 2 && m_vectCamDevices[nDevId].interface_id == 1 )
	{
		CCanonEosSetupDlg* pDlg = new CCanonEosSetupDlg( this, wxT("Canon EOS Setup") );
		pDlg->SetConfig( m_pCamera->m_pCameraCanonEos );

		if( pDlg->ShowModal() == wxID_OK )
		{
			// get/set values to the camera here ... todo: maybe call this in the action thread
			// then in thread I can wait for the command to get completed - or make a method
			// set propeties whick will take a vector of commands - not to lock/unlock everytime?
			m_pDCameraWorker->SetProperty( (unsigned long) kEdsPropID_Tv,  (unsigned long) pDlg->GetTv() );
			m_pDCameraWorker->SetProperty( (unsigned long) kEdsPropID_ISOSpeed, (unsigned long) pDlg->GetIso() );
			m_pDCameraWorker->SetProperty( (unsigned long) kEdsPropID_Av, (unsigned long) pDlg->GetAv() );
			m_pDCameraWorker->SetProperty( (unsigned long) kEdsPropID_MeteringMode, (unsigned long) pDlg->GetMm() );
		}
		delete( pDlg );

	// video camera setup
	} else if( m_vectCamDevices[nDevId].wia_type == 3 )
	{
		// check if camera video and running 
		if( !m_pCamera->m_pCameraVideo || !m_pCamera->m_pCameraVideo->m_isRunning ) return;

		// call show camera settins
		m_pCamera->m_pCameraVideo->ShowSettingsDlg( );
	}
}

// Method:	OnCameraFormat
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnCameraFormat( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();
	int nDevId = m_pCameraSelect->GetSelection();

	// if camera video 
	if( m_vectCamDevices[nDevId].wia_type == 3 )
	{
		// check if camera video and running 
		if( !m_pCamera->m_pCameraVideo || !m_pCamera->m_pCameraVideo->m_isRunning ) return;

		// call show camera format
		m_pCamera->m_pCameraVideo->ShowFormatDlg( );

	}
}

// Purpose:	set an image from camera in the view and also set the
//			graphs: histogram, focus
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetCameraImage( wxImage* pImage, IplImage* pIplImage, int bSetView )
{
	// check if view
	if( !m_bInstrCameraView ) return;

	// set my image - this is for video only
	if( bSetView ) m_pImgView->SetWxImage( pImage, 1, 0 );

	/////////
	// set histogram
	SetHistogram( );

	/////////
	// set tracking
	if( pIplImage && m_pProcImgShift )
	{
		double x=0, y=0;
		m_pProcImgShift->Track( pIplImage, x, y );
		m_pImgView->m_nMousePicX = x;
		m_pImgView->m_nMousePicY = y;
		m_pImgView->Refresh( false );
	}

	// set radial plot graph
	if( m_pImgView->m_bMainTarget && m_pFocusView )
	{
		////////////
		// hard code setup to show - todo: remove
		// do set radial plot
		StarDef obj; 
		// position
		obj.x = m_pImgView->m_nMousePicX; 
		obj.y = m_pImgView->m_nMousePicY; 
		// sizw
		obj.a = 10.0; 
		obj.b = 10.0; 
		// orientation
		obj.theta = 0.0; 
		// ??
		obj.kronfactor = 3.0; 
		obj.fwhm = 8.514443;
		m_pFocusView->SetData( *pImage, &obj, 0 );
		m_pFocusView->Refresh( false );
	}

	return;
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnCondUpdate( wxCommandEvent& pEvent )
{
	if( m_bIsInstrClosing ) return;

	double vectData[200];
	double nTemp=0.0, nHumd=0.0, nWind=0.0;
	int nData=20;
	wxString strTmp;

	// get/set temperature
	m_pObserver->m_vectSite[m_nCurrentSiteId]->GetTemperature( vectData, nData, nTemp );
	m_pCondView->SetData( vectData, nData, 0 );
	// get/set humidity
	m_pObserver->m_vectSite[m_nCurrentSiteId]->GetHumidity( vectData, nData, nHumd );
	m_pCondView->SetData( vectData, nData, 1 );
	// get/set wind
	m_pObserver->m_vectSite[m_nCurrentSiteId]->GetWind( vectData, nData, nWind );
	m_pCondView->SetData( vectData, nData, 2 );

	// set sizer label
	int t = (int) round(nTemp);
	int h = (int) round(nHumd);
	int w = (int) round(nWind);
	strTmp.Printf( wxT("Cond: %d%s/%d%s/%d%s"), t, wxT("°C"), h, wxT("%"), w, wxT("kmph") );
	pCondViewBox->SetLabel( strTmp );

	m_pCondView->Refresh( false );

	return;
}
