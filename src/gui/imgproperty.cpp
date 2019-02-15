////////////////////////////////////////////////////////////////////
// popup window to display image properties
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/statline.h"
#include "wx/minifram.h"
#include "wx/thread.h"
#include "wx/notebook.h"
#include <wx/calctrl.h>
#include <wx/datetime.h>

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// other local header
#include "frame.h"
#include "../util/func.h"
#include "../util/image_func.h"
#include "../image/astroimage.h"
#include "../image/image_note.h"
#include "HistogramView.h"
#include "location_dialog.h"
#include "../land/geoip.h"
#include "../weather/weather.h"
#include "../telescope/telescope.h"
#include "../observer/observer.h"
#include "simple_common_dlg.h"
#include "../unimap_worker.h"
#include "image_notes_dlg.h"
#include "waitdialog.h"

// main header
#include "imgproperty.h"

//		***** CLASS CImageProperty *****
////////////////////////////////////////////////////////////////////
// CImageTextProperty EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(CImageProperty, wxDialog)
//	EVT_CHECKBOX( wxID_CHECKBOX_TIMEFRAME, CImageProperty::OnCheckBox )
	EVT_BUTTON( wxID_BUTTON_LOCATION_MAP, CImageProperty::OnEartMapLocation )
	EVT_BUTTON( wxID_BUTTON_LOCATION_GEOIP, CImageProperty::OnGeoIpLocation )
	EVT_BUTTON( wxID_BUTTON_LOCATION_MY_SITES, CImageProperty::OnMySitesLocation )

	EVT_NOTEBOOK_PAGE_CHANGED( -1, CImageProperty::OnNotebookTab )
	EVT_CHOICE( wxID_OBSERVER_COUNTRY, CImageProperty::OnSelectCountry )
	EVT_CHOICE( wxID_OBSERVER_REGION, CImageProperty::OnSelectRegion )
	EVT_CHOICE( wxID_OBSERVER_CITY, CImageProperty::OnSelectCity )
	EVT_BUTTON( wxID_BUTTON_GET_WEATHER, CImageProperty::OnGetWeatherData )
	// date/time set buttons
	EVT_BUTTON( wxID_BUTTON_DATETIME_NOW, CImageProperty::OnDateTimeSet )
	EVT_BUTTON( wxID_BUTTON_DATETIME_EXIF, CImageProperty::OnDateTimeSet )
	EVT_BUTTON( wxID_BUTTON_DATETIME_FILE, CImageProperty::OnDateTimeSet )
	// get obs name
	EVT_BUTTON( wxID_BUTTON_OBSERVER_MYSELF, CImageProperty::OnObserverName )
	// get light pollution
	EVT_BUTTON( wxID_BUTTON_LP_GET, CImageProperty::OnLightPollutionGet )
	// notes buttons handlers
	EVT_LIST_ITEM_DESELECTED( wxID_BUTTON_NOTES_LIST, CImageProperty::OnNotesListSelect )
	EVT_LIST_ITEM_SELECTED( wxID_BUTTON_NOTES_LIST, CImageProperty::OnNotesListSelect )
	EVT_LIST_ITEM_ACTIVATED( wxID_BUTTON_NOTES_LIST, CImageProperty::OnNotesListAction )
	EVT_BUTTON( wxID_BUTTON_NOTES_ADD, CImageProperty::OnNotesAction )
	EVT_BUTTON( wxID_BUTTON_NOTES_EDIT, CImageProperty::OnNotesAction )
	EVT_BUTTON( wxID_BUTTON_NOTES_REMOVE, CImageProperty::OnNotesAction )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////
CImageProperty::CImageProperty( wxWindow *parent, const wxString& title, CAstroImage* pAstroImage ) : 
							wxDialog(parent, -1, title, wxDefaultPosition, wxDefaultSize,
							wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL |wxRESIZE_BORDER )
{
	m_pAstroImage = pAstroImage;
	int width, height;
	wxString strTmp=wxT("");
	this->GetClientSize(&width, &height);

	// init some
	m_pEarth = NULL;

	wxString vectTelescopeMount[2];
	wxString vectCondSeeing[3];
	wxString vectCondTransp[3];
	wxString vectCondLp[5];
	wxString vectStackMethod[7];
	wxString vectWeatherSourceData[3];

	vectTelescopeMount[0] = wxT("Altazimuth");
	vectTelescopeMount[1] = wxT("Equatorial");

	// weather
	vectWeatherSourceData[0] = wxT("Weather Underground");
	vectWeatherSourceData[1] = wxT("Weather.Com");

	vectCondSeeing[0] = wxT("Good");
	vectCondSeeing[1] = wxT("Fair");
	vectCondSeeing[2] = wxT("Bad");

	vectCondTransp[0] = wxT("Good");
	vectCondTransp[1] = wxT("Fair");
	vectCondTransp[2] = wxT("Bad");

	vectCondLp[0] = wxT("Heavy");
	vectCondLp[1] = wxT("Moderate");
	vectCondLp[2] = wxT("Little");
	vectCondLp[3] = wxT("Moon");
	vectCondLp[4] = wxT("None");

	vectStackMethod[0] = wxT("Average");
	vectStackMethod[1] = wxT("Median");
	vectStackMethod[2] = wxT("Maximum");
	vectStackMethod[3] = wxT("Kappa-Sigma Clipping");
	vectStackMethod[4] = wxT("Median Kappa-Sigma Clipping");
	vectStackMethod[5] = wxT("Auto Adaptive Weighted Average");
	vectStackMethod[6] = wxT("Entropy Weighted Average");

	wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );
//	wxGridSizer *sizerPanel = new wxGridSizer(2,2,10,10);

	// building configure as a note book
	m_pNotebook = new wxNotebook( this, -1 , wxDefaultPosition, wxSize( 470, 270 ) );

	// add panel :: info - image size time file date
	wxPanel* pPanelInfo = new wxPanel( m_pNotebook );
	// add panel :: Histogram - image size time file date
	wxPanel* pPanelHist = new wxPanel( m_pNotebook );
	// add panel :: exif - image exif data
	m_pPanelExif = new wxPanel( m_pNotebook );
	// add panel :: fits - fits keys from image data
	m_pPanelFits = new wxPanel( m_pNotebook );
	// add panel :: Observer - name, location name/long/lat, date time 
	m_pPanelObserver = new wxPanel( m_pNotebook );
	// add panel :: Target - name , location ra/dec alt/az?
	m_pPanelTarget = new wxPanel( m_pNotebook );
	// add panel :: Equipment - telescope, camera, barlow ?
	m_pPanelEquipment = new wxPanel( m_pNotebook );
	// add panel :: Condition - stacking, etc
	m_pPanelCondition = new wxPanel( m_pNotebook );
	// add panel :: Imaging - stacking, etc
	m_pPanelImaging = new wxPanel( m_pNotebook );
	// add panel :: notes
	m_pPanelNotes = new wxPanel( m_pNotebook );

	///////////////////////////////
	// layout of group of elements in the panel
	// :: info
	wxFlexGridSizer* sizerInfoBasePanel = new wxFlexGridSizer( 2, 1, 10, 10 );
	wxFlexGridSizer* sizerInfoAPanel = new wxFlexGridSizer( 1, 2, 10, 10 );
	//sizerInfoAPanel->AddGrowableCol( 1 );
	sizerInfoPanel = new wxFlexGridSizer( 7, 4, 10, 10 );
	sizerInfoPanel->AddGrowableCol( 1 );
	sizerInfoBasePanel->Add( sizerInfoAPanel, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	sizerInfoBasePanel->Add( sizerInfoPanel, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	sizerInfoPage = new wxGridSizer( 1, 1, 10, 10 );
	sizerInfoPage->Add( sizerInfoBasePanel, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// :: histogram
	wxGridSizer* sizerHistPage = new wxGridSizer( 1, 1, 10, 10 );
	// :: EXIF
//	wxFlexGridSizer* sizerExifPanel = new wxFlexGridSizer( 1, 1, 3, 3 );
	wxStaticBox* pExifBoxPanel = new wxStaticBox( m_pPanelExif, -1, wxT("Exif Image Data"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* pExifBoxSizer = new wxStaticBoxSizer( pExifBoxPanel, wxHORIZONTAL );
	wxGridSizer* sizerExifData = new wxGridSizer( 1, 1, 10, 10 );
	sizerExifData->Add( pExifBoxSizer, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// :: FITS
	wxStaticBox* pFitsBoxPanel = new wxStaticBox( m_pPanelFits, -1, wxT("Fits Image Data"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* pFitsBoxSizer = new wxStaticBoxSizer( pFitsBoxPanel, wxHORIZONTAL );
	wxGridSizer* sizerFitsData = new wxGridSizer( 1, 1, 10, 10 );
	sizerFitsData->Add( pFitsBoxSizer, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	/////////////////
	// :: observer
	wxFlexGridSizer* sizerObserverPanel = new wxFlexGridSizer( 4, 1, 3, 3 );
	sizerObserverPanel->AddGrowableCol( 0 );
	sizerObserverPanel->AddGrowableRow( 1 );
	sizerObserverPanel->AddGrowableRow( 0 );
	sizerObserverPanel->AddGrowableRow( 2 );
	wxFlexGridSizer* sizerObserverPanelA = new wxFlexGridSizer( 1, 2, 3, 3 );
	wxFlexGridSizer* sizerObserverPanelB = new wxFlexGridSizer( 4, 2, 3, 3 );
	sizerObserverPanelB->AddGrowableRow( 0 );
	sizerObserverPanelB->AddGrowableRow( 4 );
	sizerObserverPanelB->AddGrowableCol( 1 );
	sizerObserverPanelB->AddGrowableCol( 0 );
	wxFlexGridSizer* sizerObserverPanelC = new wxFlexGridSizer( 1, 2, 3, 3 );
	wxGridSizer* sizerObserverPage = new wxGridSizer( 1, 1, 10, 10 );
	sizerObserverPage->Add( sizerObserverPanel, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND|wxGROW );
	// box
	wxStaticBox* pObserverBox = new wxStaticBox( m_pPanelObserver, -1, wxT("Location/Site"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* pObserverBoxSizer = new wxStaticBoxSizer( pObserverBox, wxVERTICAL );
	pObserverBoxSizer->Add( sizerObserverPanelB, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND );
	// add to main grid
	sizerObserverPanel->Add( sizerObserverPanelA, 1, wxALIGN_BOTTOM | wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	sizerObserverPanel->Add( pObserverBoxSizer, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxLEFT|wxRIGHT, 5 );
	sizerObserverPanel->Add( sizerObserverPanelC, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	// :: target
	wxFlexGridSizer* sizerTargetPanel = new wxFlexGridSizer( 7, 2, 3, 3 );
	wxGridSizer* sizerTargetPage = new wxGridSizer( 1, 1, 10, 10 );
	sizerTargetPage->Add( sizerTargetPanel, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// :: equipment
	wxFlexGridSizer* sizerEquipmentPanel = new wxFlexGridSizer( 7, 2, 3, 3 );
	wxGridSizer* sizerEquipmentPage = new wxGridSizer( 1, 1, 10, 10 );
	sizerEquipmentPage->Add( sizerEquipmentPanel, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// :: Condition
	wxFlexGridSizer* sizerConditionPanel = new wxFlexGridSizer( 9, 2, 3, 3 );
	wxGridSizer* sizerConditionPage = new wxGridSizer( 1, 1, 10, 10 );
	sizerConditionPanel->AddGrowableRow( 0 );
	sizerConditionPanel->AddGrowableRow( 8 );
	sizerConditionPage->Add( sizerConditionPanel, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 10 );
	// :: Imaging 
	wxFlexGridSizer* sizerImagingPanel = new wxFlexGridSizer( 7, 2, 3, 3 );
	wxGridSizer* sizerImagingPage = new wxGridSizer( 1, 1, 10, 10 );
	sizerImagingPage->Add( sizerImagingPanel, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// :: Notes 
	wxFlexGridSizer* sizerNotesPanel = new wxFlexGridSizer( 2, 1, 3, 3 );
	sizerNotesPanel->AddGrowableCol( 0 );
	sizerNotesPanel->AddGrowableRow( 0 );
	wxGridSizer* sizerNotesPage = new wxGridSizer( 1, 1, 10, 10 );
	sizerNotesPage->Add( sizerNotesPanel, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND );

	///////////////////////////////////
	// create info panel elements
	m_pImageSize = new wxStaticText( pPanelInfo, -1, wxT("0 x 0 pixels") );
	m_pFileType = new wxStaticText( pPanelInfo, -1, wxT("Unknown File Type") );
	m_pRaCenter = new wxStaticText( pPanelInfo, -1, wxT("00:00:00") );
	m_pDecCenter = new wxStaticText( pPanelInfo, -1, wxT("00:00:00") );
	m_pFieldWidth = new wxStaticText( pPanelInfo, -1, wxT("00'00") );
	m_pFieldHeight = new wxStaticText( pPanelInfo, -1, wxT("00'00") );
	m_pFileTime = new wxStaticText( pPanelInfo, -1, wxT("2000/01/01 00:00:00") );

	////////////////////////////////////
	// create Histgram panel elements - ID_HISTOGRAM
	m_pImageHistogramView = new CHistogramView( pPanelHist, &m_rHistogram, NULL, true, -1 );

	////////////////////////////////////
	// create exif panel elements - todo: this and fits only be created when data passed in present
	m_pExifData = new wxListCtrl( m_pPanelExif, -1,
					wxPoint(-1,-1), wxSize(-1,-1), wxLC_REPORT|wxLC_HRULES|wxLC_VRULES );

	////////////////////////////////////
	// create exif panel elements
	m_pFitsData = new wxListCtrl( m_pPanelFits, -1,
					wxPoint(-1,-1), wxSize(-1,-1), wxLC_REPORT|wxLC_HRULES|wxLC_VRULES );

	///////////////////////////////////
	// create OBSERVER panel elements
	m_pObsName = new wxTextCtrl( m_pPanelObserver, wxID_OBSERVER_NAME, wxT(""), wxDefaultPosition, wxSize(220,-1) );
	
	m_pObsCountry = new wxChoice( m_pPanelObserver, wxID_OBSERVER_COUNTRY , wxDefaultPosition, wxSize(200,-1) );
	m_pObsRegion = new wxChoice( m_pPanelObserver, wxID_OBSERVER_REGION, wxDefaultPosition, wxSize(120,-1) );
	m_pObsCity = new wxChoice( m_pPanelObserver, wxID_OBSERVER_CITY, wxDefaultPosition, wxSize(160,-1) );

	m_pObsLatitude = new wxTextCtrl( m_pPanelObserver, wxID_OBSERVER_LATITUDE, wxT(""),
									wxDefaultPosition, wxSize(65,-1) );
	m_pObsLongitude = new wxTextCtrl( m_pPanelObserver, wxID_OBSERVER_LONGITUDE, wxT(""),
									wxDefaultPosition, wxSize(65,-1) );
	m_pObsAltitude = new wxTextCtrl( m_pPanelObserver, wxID_OBSERVER_ALTITUDE, wxT(""),
									wxDefaultPosition, wxSize(65,-1) );
	m_pObsDate = new wxDatePickerCtrl( m_pPanelObserver, wxID_OBSERVER_DATE, wxDefaultDateTime,
									wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN );
	//wxCalendarCtrl* m_pObsCal = new wxCalendarCtrl( m_pPanelObserver, -1 );
	m_pObsTime = new wxTextCtrl( m_pPanelObserver, wxID_OBSERVER_TIME, wxT(""),
									wxDefaultPosition, wxSize(60,-1) );
	///////////////////////////////////
	// create target panel elements
	m_pTargetName = new wxTextCtrl( m_pPanelTarget, wxID_TARGET_NAME, wxT(""),
									wxDefaultPosition, wxSize(200,-1) );
	m_pTargetDesc = new wxTextCtrl( m_pPanelTarget, wxID_TARGET_DESC, wxT(""),
									wxDefaultPosition, wxSize(300,-1) );
	m_pTargetRa = new wxTextCtrl( m_pPanelTarget, wxID_TARGET_RA, wxT(""),
									wxDefaultPosition, wxSize(100,-1) );
	m_pTargetDec = new wxTextCtrl( m_pPanelTarget, wxID_TARGET_DEC, wxT(""),
									wxDefaultPosition, wxSize(100,-1) );
	///////////////////////////////////
	// create equipment panel elements
	m_pTelescopeName = new wxTextCtrl( m_pPanelEquipment, wxID_TELESCOPE_NAME, wxT(""),
									wxDefaultPosition, wxSize(120,-1) );
	m_pTelescopeType = new wxChoice( m_pPanelEquipment, wxID_TELESCOPE_TYPE,
										wxDefaultPosition, wxSize(140,-1), 0, 0,
										//wxSize(140,-1), g_nTelescopeTypes, g_vectTelescopeTypes, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pTelescopeType->SetSelection( 0 );
	m_pTelescopeFocal = new wxTextCtrl( m_pPanelEquipment, wxID_TELESCOPE_SPEED, wxT(""),
									wxDefaultPosition, wxSize(120,-1) );
	m_pTelescopeAperture = new wxTextCtrl( m_pPanelEquipment, wxID_TELESCOPE_APERTURE, wxT(""),
									wxDefaultPosition, wxSize(120,-1) );
	m_pTelescopeMount = new wxChoice( m_pPanelEquipment, wxID_TELESCOPE_MOUNT,
										wxDefaultPosition,
										wxSize(80,-1), 2, vectTelescopeMount, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pTelescopeMount->SetSelection( 0 );
	m_pCameraName = new wxTextCtrl( m_pPanelEquipment, wxID_CAMERA_NAME, wxT(""),
									wxDefaultPosition, wxSize(200,-1) );

	///////////////////////////////////
	// create conditions panel elements
	m_pWeatherGetButton = new wxButton( m_pPanelCondition, wxID_BUTTON_GET_WEATHER, wxT("Get Weather"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	m_pWeatherGetSource = new wxChoice( m_pPanelCondition, wxID_SELECT_WEATHER_SOURCE,
										wxDefaultPosition,
										wxSize(-1,-1), 1, vectWeatherSourceData, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pWeatherGetSource->SetSelection( 0 );
	m_pCondSeeing = new wxChoice( m_pPanelCondition, wxID_CONDITION_SEEING,
										wxDefaultPosition,
										wxSize(60,-1), 3, vectCondSeeing, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pCondSeeing->SetSelection( 0 );
	m_pCondTransp = new wxChoice( m_pPanelCondition, wxID_CONDITION_TRANSPARENCY,
										wxDefaultPosition,
										wxSize(60,-1), 3, vectCondTransp, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pCondTransp->SetSelection( 0 );
	m_pCondWindSpeed = new wxTextCtrl( m_pPanelCondition, wxID_COND_WIND_SPEED, wxT(""), wxDefaultPosition, wxSize(60,-1) );
	m_pCondAtmPressure = new wxTextCtrl( m_pPanelCondition, wxID_COND_ATM_PRESSURE, wxT(""), wxDefaultPosition, wxSize(60,-1) );
	m_pCondTemp = new wxTextCtrl( m_pPanelCondition, wxID_COND_TEMPERATURE, wxT(""),
									wxDefaultPosition, wxSize(60,-1) );
	m_pCondTempVar = new wxTextCtrl( m_pPanelCondition, wxID_COND_TEMP_VARIATION, wxT(""),
									wxDefaultPosition, wxSize(60,-1) );
	m_pCondHumid = new wxTextCtrl( m_pPanelCondition, wxID_COND_HUMIDITY, wxT(""),
									wxDefaultPosition, wxSize(60,-1) );
	m_pCondLp = new wxChoice( m_pPanelCondition, wxID_CONDITION_LIGHT_POLUTION,
										wxDefaultPosition,
										wxSize(80,-1), 5, vectCondLp, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pCondLp->SetSelection( 0 );

	///////////////////////////////////
	// create Imaging panel elements
	m_pExpTotalTime = new wxTextCtrl( m_pPanelImaging, wxID_EXPOSURE_TOTAL_TIME, wxT("0"),
									wxDefaultPosition, wxSize(120,-1) );
	m_pNoOfExp = new wxTextCtrl( m_pPanelImaging, wxID_NO_EXPOSURES, wxT("0"),
									wxDefaultPosition, wxSize(120,-1) );
	m_pExpIso = new wxTextCtrl( m_pPanelImaging, wxID_EXPOSURE_ISO, wxT("0"),
									wxDefaultPosition, wxSize(120,-1) );
	m_pRegProg = new wxTextCtrl( m_pPanelImaging, wxID_REGISTRATION_PROGRAM, wxT(""),
									wxDefaultPosition, wxSize(250,-1) );
	m_pStackProg = new wxTextCtrl( m_pPanelImaging, wxID_STACKING_PROGRAM, wxT(""),
									wxDefaultPosition, wxSize(250,-1) );
	m_pStackMethod = new wxChoice( m_pPanelImaging, wxID_STACK_METHOD,
										wxDefaultPosition,
										wxSize(200,-1), 7, vectStackMethod, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pStackMethod->SetSelection( 0 );
	m_pProcProg = new wxTextCtrl( m_pPanelImaging, wxID_PROCESSING_PROGRAM, wxT(""),
									wxDefaultPosition, wxSize(250,-1) );

	///////////////////////////////////
	// create Notes panel elements
	m_pNotesTable = new wxListCtrl( m_pPanelNotes, wxID_BUTTON_NOTES_LIST,
			wxPoint(-1,-1), wxSize(-1,-1), wxLC_REPORT|wxLC_HRULES|wxLC_VRULES|wxLC_SINGLE_SEL );
	m_pNotesAddButton = new wxButton( m_pPanelNotes, wxID_BUTTON_NOTES_ADD, wxT("Add"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	m_pNotesEditButton = new wxButton( m_pPanelNotes, wxID_BUTTON_NOTES_EDIT, wxT("Edit"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	m_pNotesRemoveButton = new wxButton( m_pPanelNotes, wxID_BUTTON_NOTES_REMOVE, wxT("Delete"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );

	////////////////
	// Populate info sizer
	// file type
	sizerInfoAPanel->Add( new wxStaticText( pPanelInfo, -1, wxT("File type:")), 1,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL , 2  );
	sizerInfoAPanel->Add( m_pFileType, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	////////
	// image size
	sizerInfoPanel->Add( new wxStaticText( pPanelInfo, -1, wxT("Image size:")), 1,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerInfoPanel->Add( m_pImageSize, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// file date/time
	sizerInfoPanel->Add( new wxStaticText( pPanelInfo, -1, wxT("File time:")), 1,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxLEFT, 20  );
	sizerInfoPanel->Add( m_pFileTime, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	///////
	// ra center
	sizerInfoPanel->Add( new wxStaticText( pPanelInfo, -1, wxT("Ra Center:")), 1,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	sizerInfoPanel->Add( m_pRaCenter, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// dec center
	sizerInfoPanel->Add( new wxStaticText( pPanelInfo, -1, wxT("Dec Center:")), 1,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxLEFT, 20  );
	sizerInfoPanel->Add( m_pDecCenter, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	/////
	// Field Width
	sizerInfoPanel->Add( new wxStaticText( pPanelInfo, -1, wxT("Field Width:")), 1,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	sizerInfoPanel->Add( m_pFieldWidth, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// Field Height
	sizerInfoPanel->Add( new wxStaticText( pPanelInfo, -1, wxT("Field Height:")), 1,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxLEFT, 20  );
	sizerInfoPanel->Add( m_pFieldHeight, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	/////
	// Magnitude range
	sizerInfoPanel->Add( new wxStaticText( pPanelInfo, -1, wxT("Mag Range:")), 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL  );
	// auto adjust for big size - todo: clean this/check somewhere else - in load maybe/detect mag range maybe
	double nMaxMag = m_pAstroImage->m_nMaxMag;
	double nMinMag = m_pAstroImage->m_nMinMag;
	if( nMaxMag > 999 || nMaxMag < -999 ) nMaxMag = 0.0;
	if( nMinMag > 999 || nMinMag < -999 ) nMinMag = 0.0;
	strTmp.Printf( wxT("%.1f - %.1f"), nMaxMag, nMinMag );
	sizerInfoPanel->Add( new wxStaticText(pPanelInfo, -1, strTmp), 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	/////
	// Detected objects
	sizerInfoPanel->Add( new wxStaticText( pPanelInfo, -1, wxT("Detected:")), 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL  );
	strTmp.Printf( wxT("%d"), m_pAstroImage->m_nStar );
	sizerInfoPanel->Add( new wxStaticText(pPanelInfo, -1, strTmp), 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// Matched Objects
	sizerInfoPanel->Add( new wxStaticText( pPanelInfo, -1, wxT("Matched:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxLEFT, 20  );
	strTmp.Printf( wxT("%d"), m_pAstroImage->m_nMatchedStars );
	sizerInfoPanel->Add( new wxStaticText(pPanelInfo, -1, strTmp), 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	/////
	// catalog stars
	sizerInfoPanel->Add( new wxStaticText( pPanelInfo, -1, wxT("Catalog Stars:")), 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL  );
	strTmp.Printf( wxT("%d"), m_pAstroImage->m_nCatStar );
	sizerInfoPanel->Add( new wxStaticText(pPanelInfo, -1, strTmp), 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// Deep sky objects
	sizerInfoPanel->Add( new wxStaticText( pPanelInfo, -1, wxT("Deep Sky Obj:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxLEFT, 20  );
	strTmp.Printf( wxT("%d"), m_pAstroImage->m_nCatDso );
	sizerInfoPanel->Add( new wxStaticText(pPanelInfo, -1, strTmp), 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	/////
	// Radio
	sizerInfoPanel->Add( new wxStaticText( pPanelInfo, -1, wxT("Radio:")), 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL  );
	strTmp.Printf( wxT("%d"), m_pAstroImage->m_nCatRadio );
	sizerInfoPanel->Add( new wxStaticText(pPanelInfo, -1, strTmp), 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// x-ray/gamma
	sizerInfoPanel->Add( new wxStaticText( pPanelInfo, -1, wxT("X-Ray/Gamma:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxLEFT, 20  );
	strTmp.Printf( wxT("%d"), m_pAstroImage->m_nCatXGamma );
	sizerInfoPanel->Add( new wxStaticText(pPanelInfo, -1, strTmp), 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	// supernovas
	sizerInfoPanel->Add( new wxStaticText( pPanelInfo, -1, wxT("Supernovas:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxLEFT, 20  );
	strTmp.Printf( wxT("%d"), m_pAstroImage->m_nCatSupernova );
	sizerInfoPanel->Add( new wxStaticText(pPanelInfo, -1, strTmp), 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	////////////////
	// Populate Histogram sizer
	// image size
	sizerHistPage->Add( m_pImageHistogramView, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL|wxEXPAND, 2 );

	//////////////////////////////////
	// populate Exif panel
	// exif table 
	pExifBoxSizer->Add( m_pExifData, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW|wxEXPAND );

	//////////////////////////////////
	// populate FITS panel
	// fits table 
	pFitsBoxSizer->Add( m_pFitsData, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW|wxEXPAND );

	////////////////
	// Populate OBSERVER sizer
	// :: observer name
	wxFlexGridSizer* pObserverSizer = new wxFlexGridSizer( 1, 2, 0, 5 );
	wxButton* pObsMyselfButton = new wxButton( m_pPanelObserver, wxID_BUTTON_OBSERVER_MYSELF, wxT("Myself"), wxPoint( -1, -1 ), wxSize( 50, -1 ) );
	sizerObserverPanelA->Add( new wxStaticText( m_pPanelObserver, -1, wxT("Observer Name:")), 1,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	pObserverSizer->Add( m_pObsName, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	pObserverSizer->Add( pObsMyselfButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerObserverPanelA->Add( pObserverSizer, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: buttons
	sizerObserverPanelB->AddStretchSpacer();
	// next cell with gps,map,geoip
	wxFlexGridSizer* pLocSizer = new wxFlexGridSizer( 1, 4, 0, 5 );
	pLocSizer->AddGrowableRow( 0 );
	wxButton* pLocButton = new wxButton( m_pPanelObserver, wxID_BUTTON_LOCATION_MAP, wxT("Map"), wxPoint( -1, -1 ), wxSize( 35, -1 ) );
	wxButton* pLocGeoIpButton = new wxButton( m_pPanelObserver, wxID_BUTTON_LOCATION_GEOIP, wxT("GeoIp"), wxPoint( -1, -1 ), wxSize( 40, -1 ) );
	wxButton* pLocGpsButton = new wxButton( m_pPanelObserver, wxID_BUTTON_LOCATION_GPS, wxT("GPS"), wxPoint( -1, -1 ), wxSize( 35, -1 ) );
	m_pLocMySitesButton = new wxButton( m_pPanelObserver, wxID_BUTTON_LOCATION_MY_SITES, wxT("Own Sites"), wxPoint( -1, -1 ), wxSize( 60, -1 ) );
	pLocGpsButton->Disable();
	pLocSizer->Add( pLocButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	pLocSizer->Add( pLocGeoIpButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	pLocSizer->Add( pLocGpsButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	pLocSizer->Add( m_pLocMySitesButton, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxRIGHT, 2  );
	sizerObserverPanelB->Add( pLocSizer, 1, wxALIGN_LEFT | wxALIGN_BOTTOM| wxALL, 0  );
	// :: country 
	sizerObserverPanelB->Add( new wxStaticText( m_pPanelObserver, -1, wxT("Country:")), 1,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerObserverPanelB->Add( m_pObsCountry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: region
	sizerObserverPanelB->Add( new wxStaticText( m_pPanelObserver, -1, wxT("Provice/State:")), 1,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerObserverPanelB->Add( m_pObsRegion, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: city
	sizerObserverPanelB->Add( new wxStaticText( m_pPanelObserver, -1, wxT("City:")), 1,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerObserverPanelB->Add( m_pObsCity, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: latitude
	sizerObserverPanelB->Add( new wxStaticText( m_pPanelObserver, -1, wxT("Lat(°N)/Lon(°E)/Alt(m):")), 1,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// internal sizer
	wxFlexGridSizer* pLatLonSizer = new wxFlexGridSizer( 1, 3, 0, 5 );
	pLatLonSizer->Add( m_pObsLatitude, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	pLatLonSizer->Add( m_pObsLongitude, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	pLatLonSizer->Add( m_pObsAltitude, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerObserverPanelB->Add( pLatLonSizer, 1, wxALIGN_LEFT | wxALIGN_TOP| wxALL, 0  );
	// :: date/time
	wxFlexGridSizer* pDateTimeSizer = new wxFlexGridSizer( 1, 5, 0, 5 );
	wxButton* pDateTimeNowButton = new wxButton( m_pPanelObserver, wxID_BUTTON_DATETIME_NOW, wxT("Now"), wxPoint( -1, -1 ), wxSize( 35, -1 ) );

	// make case here if exif or fits
	wxString strButtonLabel = wxT("Exif");
	if( m_pAstroImage->m_pImgFmtFits ) strButtonLabel = wxT("Fits");
	m_pDateTimeExifButton = new wxButton( m_pPanelObserver, wxID_BUTTON_DATETIME_EXIF, strButtonLabel, wxPoint( -1, -1 ), wxSize( 35, -1 ) );
	
	m_pDateTimeExifButton->Disable();
	wxButton* pDateTimeFileButton = new wxButton( m_pPanelObserver, wxID_BUTTON_DATETIME_FILE, wxT("File"), wxPoint( -1, -1 ), wxSize( 35, -1 ) );
	sizerObserverPanelC->Add( new wxStaticText( m_pPanelObserver, -1, wxT("Date/Time:")), 1,
								wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	pDateTimeSizer->Add( m_pObsDate, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	pDateTimeSizer->Add( m_pObsTime, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	pDateTimeSizer->Add( pDateTimeNowButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	pDateTimeSizer->Add( m_pDateTimeExifButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	pDateTimeSizer->Add( pDateTimeFileButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerObserverPanelC->Add( pDateTimeSizer, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );

	////////////////
	// Populate TARGET sizer
	// target name
	sizerTargetPanel->Add( new wxStaticText( m_pPanelTarget, -1, wxT("Target Name:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerTargetPanel->Add( m_pTargetName, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// target description
	sizerTargetPanel->Add( new wxStaticText( m_pPanelTarget, -1, wxT("Target Description:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL  , 2  );
	sizerTargetPanel->Add( m_pTargetDesc, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL , 2  );
	// target ra
	sizerTargetPanel->Add( new wxStaticText( m_pPanelTarget, -1, wxT("Target Ra:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerTargetPanel->Add( m_pTargetRa, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// target dec
	sizerTargetPanel->Add( new wxStaticText( m_pPanelTarget, -1, wxT("Target Dec:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerTargetPanel->Add( m_pTargetDec, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );

	////////////////
	// Populate equipment sizer
	// telescope name
	sizerEquipmentPanel->Add( new wxStaticText( m_pPanelEquipment, -1, wxT("Telescope Name:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerEquipmentPanel->Add( m_pTelescopeName, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// telescope type
	sizerEquipmentPanel->Add( new wxStaticText( m_pPanelEquipment, -1, wxT("Telescope Type:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerEquipmentPanel->Add( m_pTelescopeType, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// telescope speed
	sizerEquipmentPanel->Add( new wxStaticText( m_pPanelEquipment, -1, wxT("Telescope Focal:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerEquipmentPanel->Add( m_pTelescopeFocal, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// telescope apperture
	sizerEquipmentPanel->Add( new wxStaticText( m_pPanelEquipment, -1, wxT("Telescope Aperture:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerEquipmentPanel->Add( m_pTelescopeAperture, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// telescope mount
	sizerEquipmentPanel->Add( new wxStaticText( m_pPanelEquipment, -1, wxT("Telescope Mount:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerEquipmentPanel->Add( m_pTelescopeMount, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// camera name
	sizerEquipmentPanel->Add( new wxStaticText( m_pPanelEquipment, -1, wxT("Camera Name:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerEquipmentPanel->Add( m_pCameraName, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );

	////////////////
	// Populate Conditions sizer
	// :: get online conditions
	sizerConditionPanel->Add( m_pWeatherGetButton, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxRIGHT, 2  );
	sizerConditionPanel->Add( m_pWeatherGetSource, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxLEFT, 2  );
	// wind speed
	sizerConditionPanel->Add( new wxStaticText( m_pPanelCondition, -1, wxT("Wind Speed(km/h):")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxRIGHT, 2  );
	sizerConditionPanel->Add( m_pCondWindSpeed, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxLEFT, 2  );
	// atmospheric pressure
	sizerConditionPanel->Add( new wxStaticText( m_pPanelCondition, -1, wxT("Pressure(mb):")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxRIGHT, 2  );
	sizerConditionPanel->Add( m_pCondAtmPressure, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxLEFT, 2  );
	// temperature/variations
	wxFlexGridSizer* pTempVarSizer = new wxFlexGridSizer( 1, 2, 0, 5 );
	sizerConditionPanel->Add( new wxStaticText( m_pPanelCondition, -1, wxT("Temperature/Variations(°C):")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxRIGHT, 2  );
	pTempVarSizer->Add( m_pCondTemp, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxRIGHT, 2  );
	pTempVarSizer->Add( m_pCondTempVar, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxRIGHT, 2  );
	sizerConditionPanel->Add( pTempVarSizer, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxLEFT, 2  );
	// humidity
	sizerConditionPanel->Add( new wxStaticText( m_pPanelCondition, -1, wxT("Humidity(%):")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxRIGHT, 2  );
	sizerConditionPanel->Add( m_pCondHumid, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxLEFT, 2  );
	// seeing/transparency
	wxFlexGridSizer* pSeeingTransSizer = new wxFlexGridSizer( 1, 2, 0, 5 );
	sizerConditionPanel->Add( new wxStaticText( m_pPanelCondition, -1, wxT("Seeing/Transparency:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxRIGHT, 2  );
	pSeeingTransSizer->Add( m_pCondSeeing, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxRIGHT, 2  );
	pSeeingTransSizer->Add( m_pCondTransp, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxRIGHT, 2  );
	sizerConditionPanel->Add( pSeeingTransSizer, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxLEFT, 2  );
	// light pollution
	wxFlexGridSizer* pLPSizer = new wxFlexGridSizer( 1, 3, 0, 5 );
	wxButton* pLPGetButton = new wxButton( m_pPanelCondition, wxID_BUTTON_LP_GET, wxT("Get"), wxPoint( -1, -1 ), wxSize( 35, -1 ) );
	wxButton* pLPEvalButton = new wxButton( m_pPanelCondition, wxID_BUTTON_LP_EVAL, wxT("Evaluate"), wxPoint( -1, -1 ), wxSize( 50, -1 ) );
	pLPEvalButton->Disable();
	sizerConditionPanel->Add( new wxStaticText( m_pPanelCondition, -1, wxT("Light Pollution:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxRIGHT, 2  );
	pLPSizer->Add( m_pCondLp, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxLEFT, 2  );
	pLPSizer->Add( pLPGetButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxLEFT, 2  );
	pLPSizer->Add( pLPEvalButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxLEFT, 2  );
	sizerConditionPanel->Add( pLPSizer, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxLEFT, 2  );

	////////////////
	// Populate Imaging sizer
	// exposure total time
	sizerImagingPanel->Add( new wxStaticText( m_pPanelImaging, -1, wxT("Exposure Total Time:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerImagingPanel->Add( m_pExpTotalTime, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// number of exposures
	sizerImagingPanel->Add( new wxStaticText( m_pPanelImaging, -1, wxT("Number Of Exposures Used in Stack:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerImagingPanel->Add( m_pNoOfExp, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// exposure iso
	sizerImagingPanel->Add( new wxStaticText( m_pPanelImaging, -1, wxT("Exposure ISO:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerImagingPanel->Add( m_pExpIso, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// registration program
	sizerImagingPanel->Add( new wxStaticText( m_pPanelImaging, -1, wxT("Registration Software Used:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerImagingPanel->Add( m_pRegProg, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// stacking  program used
	sizerImagingPanel->Add( new wxStaticText( m_pPanelImaging, -1, wxT("Stacking Software Used:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerImagingPanel->Add( m_pStackProg, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// stacking  method used
	sizerImagingPanel->Add( new wxStaticText( m_pPanelImaging, -1, wxT("Stacking Method Used:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerImagingPanel->Add( m_pStackMethod, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// processing software used
	sizerImagingPanel->Add( new wxStaticText( m_pPanelImaging, -1, wxT("Processing Software Used:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerImagingPanel->Add( m_pProcProg, 1,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );

	////////////////
	// Populate Notes sizer
	// :: main notes table
	sizerNotesPanel->Add( m_pNotesTable, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND| wxALL, 2  );
	// notes :: buttons
	wxFlexGridSizer* pNotesButtonsSizer = new wxFlexGridSizer( 1, 3, 0, 0 );
	pNotesButtonsSizer->AddGrowableCol( 0 );
	pNotesButtonsSizer->AddGrowableCol( 2 );
	pNotesButtonsSizer->Add( m_pNotesAddButton, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	pNotesButtonsSizer->Add( m_pNotesEditButton, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxLEFT, 10  );
	pNotesButtonsSizer->Add( m_pNotesRemoveButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxLEFT, 10  );
	sizerNotesPanel->Add( pNotesButtonsSizer, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND|wxTOP|wxBOTTOM, 3  );

	// calculate sizers
	topsizer->Add( m_pNotebook, 1, wxGROW | wxALL, 10 );
	topsizer->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxCENTRE | wxALL, 10 );

	pPanelInfo->SetSizer( sizerInfoPage );
	m_pNotebook->AddPage( pPanelInfo, wxT("Info") );
	pPanelHist->SetSizer( sizerHistPage );
	m_pNotebook->AddPage( pPanelHist, wxT("Histogram") );

	m_pPanelExif->SetSizer( pExifBoxSizer );
//	pNotebook->AddPage( m_pPanelExif, wxT("Exif") );

	m_pPanelFits->SetSizer( pFitsBoxSizer );

	m_pPanelObserver->SetSizer( sizerObserverPage );
//	pNotebook->AddPage( m_pPanelObserver, wxT("Observer") );

	m_pPanelTarget->SetSizer( sizerTargetPage );
//	pNotebook->AddPage( m_pPanelTarget, wxT("Target") );

	m_pPanelEquipment->SetSizer( sizerEquipmentPage );
//	pNotebook->AddPage( m_pPanelEquipment, wxT("Equipment") );

	m_pPanelCondition->SetSizer( sizerConditionPage );
//	pNotebook->AddPage( m_pPanelCondition, wxT("Conditions") );

	m_pPanelImaging->SetSizer( sizerImagingPage );
//	pNotebook->AddPage( m_pPanelImaging, wxT("Imaging") );

	m_pPanelNotes->SetSizer( sizerNotesPage );
//	pNotebook->AddPage( m_pPanelNotes, wxT("Notes") );

	// hide panels
	m_bPanelExif = 0;
	m_pPanelExif->Hide( );
	m_bPanelFits = 0;
	m_pPanelFits->Hide( );
	m_pPanelObserver->Hide( );
	m_pPanelTarget->Hide( );
	m_pPanelEquipment->Hide( );
	m_pPanelCondition->Hide( );
	m_pPanelImaging->Hide( );
	m_pPanelNotes->Hide( );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
//	sizer->Fit(this);
	Centre( wxBOTH );
}

////////////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////////////
CImageProperty::~CImageProperty( )
{
	delete( m_pImageHistogramView );

	// remove info
	delete( m_pImageSize );
	delete( m_pFileType );
	delete( m_pRaCenter );
	delete( m_pDecCenter );
	delete( m_pFieldWidth );
	delete( m_pFieldHeight );
	delete( m_pFileTime );
	// remove Exif
	delete( m_pExifData );
	// remove Exif
	delete( m_pFitsData );
	// remove observer
	delete( m_pObsName );
	delete( m_pObsCountry );
	delete( m_pObsRegion );
	delete( m_pObsCity );
	delete( m_pObsLatitude );
	delete( m_pObsLongitude );
	delete( m_pObsAltitude );
	delete( m_pObsDate );
	delete( m_pObsTime );
	delete( m_pDateTimeExifButton );
	delete( m_pLocMySitesButton );
	// remove target
	delete( m_pTargetName );
	delete( m_pTargetDesc );
	delete( m_pTargetRa );
	delete( m_pTargetDec );
	// remove equipment
	delete( m_pTelescopeName );
	delete( m_pTelescopeType );
	delete( m_pTelescopeFocal );
	delete( m_pTelescopeAperture );
	delete( m_pTelescopeMount );
	delete( m_pCameraName );
	// remove conditions
	delete( m_pWeatherGetButton );
	delete( m_pWeatherGetSource );
	delete( m_pCondSeeing );
	delete( m_pCondTransp );
	delete( m_pCondWindSpeed );
	delete( m_pCondAtmPressure );
	delete( m_pCondTemp );
	delete( m_pCondTempVar );
	delete( m_pCondHumid );
	delete( m_pCondLp );
	// remove imaging
	delete( m_pExpTotalTime );
	delete( m_pNoOfExp );
	delete( m_pExpIso );
	delete( m_pRegProg );
	delete( m_pStackProg );
	delete( m_pStackMethod );
	delete( m_pProcProg );
	// remove notes
	delete( m_pNotesTable );
	delete( m_pNotesAddButton );
	delete( m_pNotesEditButton );
	delete( m_pNotesRemoveButton );

	// delete notebook and panels
	delete( m_pPanelExif );
	delete( m_pPanelObserver );
	delete( m_pPanelTarget );
	delete( m_pPanelEquipment );
	delete( m_pPanelCondition );
	delete( m_pPanelImaging );
	delete( m_pPanelNotes );
	delete( m_pNotebook );

	if( m_pEarth != NULL ) delete( m_pEarth );
}

////////////////////////////////////////////////////////////////////
// Method:	SetHistogram
// Class:	CImageProperty
// Purpose:	Set my histogram
// Input:	pointer to astro image
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CImageProperty::SetHistogram( wxImage& pImage )
{
/*	// set histogram
	ImageVariant rElxImage;
	// get abstract image info
	const uint w = pImage.GetWidth();
	const uint h = pImage.GetHeight();
    const uint size = w*h*3;
	// create rgb image
	boost::shared_ptr<ImageRGBub> spImage( new ImageRGBub(w, h) );
    ubyte* prDst = (ubyte*) spImage->GetPixel(); 
    ubyte* prSrc = (ubyte*) pImage.GetData();
    // copy from wxImage to ImageVariant
    ::memcpy( (void*) prDst, (const void*) prSrc, size );
	// set back image
	rElxImage = *spImage;

	// change resolution? to get more interting 
	//rElxImage.ChangeResolution( RT_Float );

	// compute histogram
	rElxImage.ComputeHistogram( m_sHistogram );
*/
	m_rHistogram.ComputeHistogram( pImage );

	m_pImageHistogramView->UpdateHistogram( &m_rHistogram );
	//m_pImageHistogramView->UpdateTransfertFunction( iSetting.GetTransfertFunction() );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetConfig
// Class:	CImageProperty
// Purpose:	Set and get my reference using the astro image object
// Input:	pointer to astro image
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CImageProperty::SetConfig( CUnimapWorker* pUnimapWorker, CObserver* pObserver, int nUnitsFormat )
{
	m_pUnimapWorker = pUnimapWorker;
	m_pObserver = pObserver;

	wxString strTmp;
	int i=0, j=0;

	/////////////////////////
	// set image info
	// image size
	strTmp.Printf( wxT("%d x %d"), m_pAstroImage->m_nWidth, m_pAstroImage->m_nHeight );
	m_pImageSize->SetLabel( strTmp );
	// image type
	wxString strImgType = GetImageTypeDescByExt( m_pAstroImage->m_strImageExt );
	m_pFileType->SetLabel( strImgType );

	// set ra origin
	if( nUnitsFormat == UNITS_FORMAT_SEXAGESIMAL )
		RaDegToSexagesimal( m_pAstroImage->m_nOrigRa, strTmp );
	else if( nUnitsFormat == UNITS_FORMAT_DECIMAL )
		strTmp.Printf( wxT("%.6f"), m_pAstroImage->m_nOrigRa );
	m_pRaCenter->SetLabel( strTmp );
	// set dec origin
	if( nUnitsFormat == UNITS_FORMAT_SEXAGESIMAL )
		DecDegToSexagesimal( m_pAstroImage->m_nOrigDec, strTmp );
	else if( nUnitsFormat == UNITS_FORMAT_DECIMAL )
		strTmp.Printf( wxT("%.6f"), m_pAstroImage->m_nOrigDec );
	m_pDecCenter->SetLabel( strTmp );
	// set field width
	if( nUnitsFormat == UNITS_FORMAT_SEXAGESIMAL )
		DegToHHMMSS( m_pAstroImage->m_nFieldWidth, strTmp );
	else if( nUnitsFormat == UNITS_FORMAT_DECIMAL )
		strTmp.Printf( wxT("%.6f"), m_pAstroImage->m_nFieldWidth );
	m_pFieldWidth->SetLabel( strTmp );
	// set field width
	if( nUnitsFormat == UNITS_FORMAT_SEXAGESIMAL )
		DegToHHMMSS( m_pAstroImage->m_nFieldHeight, strTmp );
	else if( nUnitsFormat == UNITS_FORMAT_DECIMAL )
		strTmp.Printf( wxT("%.6f"), m_pAstroImage->m_nFieldHeight );
	m_pFieldHeight->SetLabel( strTmp );

	// file time
	char str_tmp[255];
	strftime( str_tmp, 255, "%Y/%m/%d %H:%M:%S", localtime( &(m_pAstroImage->m_nFileTime) ) );
	m_pFileTime->SetLabel( wxString(str_tmp,wxConvUTF8) );

	////////////////
	// Set Observer panel
	m_pObsName->SetLabel( wxString(m_pAstroImage->m_strObsName,wxConvUTF8) );
//	m_pObsLocation->SetLabel( m_pAstroImage->m_strObsLocation );

	// enable/disable own site button
	if( m_pObserver->m_nSite > 0 )
		m_pLocMySitesButton->Enable();
	else
		m_pLocMySitesButton->Disable();

	// :: latitude
//	sprintf( strTmp, "%.4f%c" , m_pAstroImage->m_nObsLatitude, m_pAstroImage->m_chObsLatitude );
	strTmp.Printf( wxT("%.6f") , m_pAstroImage->m_nObsLatitude );
	m_pObsLatitude->SetLabel( strTmp );
	// :: longitude
//	sprintf( strTmp, "%.4f%c" , m_pAstroImage->m_nObsLongitude, m_pAstroImage->m_chObsLongitude );
	strTmp.Printf( wxT("%.6f") , m_pAstroImage->m_nObsLongitude );
	m_pObsLongitude->SetLabel( strTmp );

	// :: altitude
	strTmp.Printf( wxT("%.1f"), m_pAstroImage->m_nObsAltitude );
	m_pObsAltitude->SetLabel( strTmp );
	// :: observer date/time
	m_pObsDate->SetValue( m_pAstroImage->m_nObsDateTime );
	m_pObsTime->SetLabel( m_pAstroImage->m_nObsDateTime.Format( wxT("%H:%M:%S") ) );

	////////////////
	// Set Target panel
	m_pTargetName->SetLabel( wxString(m_pAstroImage->m_strTargetName,wxConvUTF8) );
	m_pTargetDesc->SetLabel( wxString(m_pAstroImage->m_strTargetDesc,wxConvUTF8) );
	if( nUnitsFormat == UNITS_FORMAT_SEXAGESIMAL )
		RaDegToSexagesimal( m_pAstroImage->m_nTargetRa, strTmp );
	else if( nUnitsFormat == UNITS_FORMAT_DECIMAL )
		strTmp.Printf( wxT("%.6f"), m_pAstroImage->m_nTargetRa );
	m_pTargetRa->SetLabel( strTmp );
	if( nUnitsFormat == UNITS_FORMAT_SEXAGESIMAL )
		DecDegToSexagesimal( m_pAstroImage->m_nTargetDec, strTmp );
	else if( nUnitsFormat == UNITS_FORMAT_DECIMAL )
		strTmp.Printf( wxT("%.6f"), m_pAstroImage->m_nTargetDec );
	m_pTargetDec->SetLabel( strTmp );

	sizerInfoPanel->Layout();
	sizerInfoPage->Layout();
	GetSizer()->Layout( );
	////////////////
	// Set Equipment panel
	m_pTelescopeName->SetLabel( wxString(m_pAstroImage->m_strTelescopeName,wxConvUTF8) );
	// telescope type
	m_pTelescopeType->Clear();
	for( i=1; i<g_nTelescopeTypes; i++ ) m_pTelescopeType->Append( g_vectTelescopeTypes[i].type_label );
	m_pTelescopeType->SetSelection( m_pAstroImage->m_nTelescopeType );
	// set other
	strTmp.Printf( wxT("%.1f"), m_pAstroImage->m_nTelescopeFocal );
	m_pTelescopeFocal->SetLabel( strTmp );
	strTmp.Printf( wxT("%.1f"), m_pAstroImage->m_nTelescopeAperture );
	m_pTelescopeAperture->SetLabel( strTmp );
	m_pTelescopeMount->SetSelection( m_pAstroImage->m_nTelescopeMount );
	m_pCameraName->SetLabel( wxString(m_pAstroImage->m_strCameraName,wxConvUTF8) );
	////////////////
	// Set Conditions panel
	m_pCondSeeing->SetSelection( m_pAstroImage->m_nCondSeeing );
	m_pCondTransp->SetSelection( m_pAstroImage->m_nCondTransp );
	strTmp.Printf( wxT("%.1f"), m_pAstroImage->m_nCondWindSpeed );
	m_pCondWindSpeed->SetLabel( strTmp );
	strTmp.Printf( wxT("%.1f"), m_pAstroImage->m_nCondAtmPressure );
	m_pCondAtmPressure->SetLabel( strTmp );
	strTmp.Printf( wxT("%.1f"), m_pAstroImage->m_nCondTemp );
	m_pCondTemp->SetLabel( strTmp );
	strTmp.Printf( wxT("%.1f"), m_pAstroImage->m_nCondTempVar );
	m_pCondTempVar->SetLabel( strTmp );
	strTmp.Printf( wxT("%.1f"), m_pAstroImage->m_nCondHumid );
	m_pCondHumid->SetLabel( strTmp );
	m_pCondLp->SetSelection( m_pAstroImage->m_nCondLp );
	////////////////
	// Set Imaging panel
	strTmp.Printf( wxT("%.6f"), m_pAstroImage->m_nExpTotalTime );
	m_pExpTotalTime->SetLabel( strTmp );
	strTmp.Printf( wxT("%d"), m_pAstroImage->m_nNoOfExp );
	m_pNoOfExp->SetLabel( strTmp );
	strTmp.Printf( wxT("%d"), m_pAstroImage->m_nExpIso );
	m_pExpIso->SetLabel( strTmp );
	m_pRegProg->SetLabel( wxString(m_pAstroImage->m_strRegProg,wxConvUTF8) );
	m_pStackProg->SetLabel( wxString(m_pAstroImage->m_strStackProg,wxConvUTF8) );
	m_pStackMethod->SetSelection( m_pAstroImage->m_nStackMethod );
	m_pProcProg->SetLabel( wxString(m_pAstroImage->m_strProcProg,wxConvUTF8) );

	/////////////////////////////////
	// Set Notes panel
	SetNotesList( );
	// disable edit/delete buttons
	m_pNotesEditButton->Disable();
	m_pNotesRemoveButton->Disable();

	//////////////////////////////////////////////////////
	// process exif data
	wxArrayString vectKey;
	wxArrayString vectValue;
	// get exif
	m_pAstroImage->GetImageExif( vectKey, vectValue );
	// check if not empty
	if( vectKey.Count() > 0 )
	{
		m_bPanelExif = 1;
		m_pExifData->InsertColumn( 1, wxT("Label"), wxLIST_FORMAT_LEFT, 200 );
		m_pExifData->InsertColumn( 2, wxT("Value"), wxLIST_FORMAT_LEFT, 300 );
		// add data
		for( i=0; i<vectKey.Count(); i++ )
		{
			// add row
			long nIndex = m_pExifData->InsertItem( 55551+i, wxT("") );
			// set label
			m_pExifData->SetItem( nIndex, 0, vectKey[i] );
			// set value
			m_pExifData->SetItem( nIndex, 1, vectValue[i] );
		}

	}

	////////////////////
	// add the panels
	if( m_bPanelExif )
	{
		m_pDateTimeExifButton->Enable();
		m_pPanelExif->Show( );
		m_pNotebook->AddPage( m_pPanelExif, wxT("Exif") );
	}

	//////////////////////////////////////////////////////
	// process exif data
	vectKey.Clear();
	vectValue.Clear();
	// get exif
	m_pAstroImage->GetImageFits( vectKey, vectValue );
	// check if not empty
	if( vectKey.Count() > 0 )
	{
		m_bPanelFits = 1;
		m_pFitsData->InsertColumn( 1, wxT("Label"), wxLIST_FORMAT_LEFT, 200 );
		m_pFitsData->InsertColumn( 2, wxT("Value"), wxLIST_FORMAT_LEFT, 300 );
		// add data
		for( i=0; i<vectKey.Count(); i++ )
		{
			// add row
			long nIndex = m_pFitsData->InsertItem( 55551+i, wxT("") );
			// set label
			m_pFitsData->SetItem( nIndex, 0, vectKey[i] );
			// set value
			m_pFitsData->SetItem( nIndex, 1, vectValue[i] );
		}

	}

	////////////////////
	// add the panels
	if( m_bPanelFits )
	{
		m_pPanelFits->Show( );
		m_pNotebook->AddPage( m_pPanelFits, wxT("Fits") );
	}

	// :: observer
	m_nObserverPageId = m_pNotebook->GetPageCount();
	m_pPanelObserver->Show( );
	m_pNotebook->AddPage( m_pPanelObserver, wxT("Observer") );
	// :: target
	m_pPanelTarget->Show( );
	m_pNotebook->AddPage( m_pPanelTarget, wxT("Target") );
	// :: equipment
	m_pPanelEquipment->Show( );
	m_pNotebook->AddPage( m_pPanelEquipment, wxT("Equipment") );
	// :: conditions
	m_pPanelCondition->Show( );
	m_pNotebook->AddPage( m_pPanelCondition, wxT("Conditions") );
	// :: imaging
	m_pPanelImaging->Show( );
	m_pNotebook->AddPage( m_pPanelImaging, wxT("Imaging") );
	// :: Notes
	m_pPanelNotes->Show( );
	m_pNotebook->AddPage( m_pPanelNotes, wxT("Notes") );

	/////////////////
	// RE-LAYOUT
	m_pNotebook->Layout();

	// :: exif
	m_pPanelExif->GetSizer()->Layout( );
	m_pPanelExif->Layout( );
	// :: observer
	m_pPanelObserver->GetSizer()->Layout( );
	m_pPanelObserver->Layout( );
	// :: target
	m_pPanelTarget->GetSizer()->Layout( );
	m_pPanelTarget->Layout( );
	// :: equipment
	m_pPanelEquipment->GetSizer()->Layout( );
	m_pPanelEquipment->Layout( );
	// :: conditions
	m_pPanelCondition->GetSizer()->Layout( );
	m_pPanelCondition->Layout( );
	// :: imaging
	m_pPanelImaging->GetSizer()->Layout( );
	m_pPanelImaging->Layout( );
	// :: notes
	m_pPanelNotes->GetSizer()->Layout( );
	m_pPanelNotes->Layout( );

	GetSizer()->Layout( );
	Fit( );

	Refresh( FALSE );
	Update( );

	return;
}

////////////////////////////////////////////////////////////////////
void CImageProperty::SetNotesList( )
{
	// clean up
	int w=0, h=0, i=0;
	wxString strTmp;

	m_pNotesTable->GetSize( &w, &h );
	m_nNotesTitleSize = 276;
	m_pNotesTable->ClearAll();

	// create header
	m_pNotesTable->InsertColumn( 1, wxT(""), wxLIST_FORMAT_CENTRE, 20 );
	m_pNotesTable->InsertColumn( 2, wxT("Id"), wxLIST_FORMAT_CENTRE, 30 );
	m_pNotesTable->InsertColumn( 3, wxT("Date/Time"), wxLIST_FORMAT_CENTRE, 128 );
	m_pNotesTable->InsertColumn( 3, wxT("Title"), wxLIST_FORMAT_CENTRE, m_nNotesTitleSize );

	// create image list
	wxImageList* vectIconsList = new wxImageList( 16, 16, true, 2 );
	vectIconsList->Add( wxBitmap( wxT("resources/text.gif"), wxBITMAP_TYPE_GIF ) );
	vectIconsList->Add( wxBitmap( wxT("resources/audio.gif"), wxBITMAP_TYPE_GIF ) );
	// assign icons
	m_pNotesTable->AssignImageList( vectIconsList, wxIMAGE_LIST_SMALL );

	// add data
	for( i=0; i<m_pAstroImage->m_vectNotes.size(); i++ )
	{
		// :: 0 :: set icon
		long nIndex = m_pNotesTable->InsertItem( 55551+i, wxT("") );
		if( m_pAstroImage->m_vectNotes[i].m_nType == 0 )
			m_pNotesTable->SetItemImage( nIndex, 0 );
		else if( m_pAstroImage->m_vectNotes[i].m_nType == 1 )
			m_pNotesTable->SetItemImage( nIndex, 1 );
		else
			m_pNotesTable->SetItemImage( nIndex, 0 );

		// :: 1 :: set Id
		strTmp.Printf( wxT("%d"), m_pAstroImage->m_vectNotes[i].m_nId );
		m_pNotesTable->SetItem( nIndex, 1, strTmp );

		// :: 2 :: Date/Time
		m_pNotesTable->SetItem( nIndex, 2, m_pAstroImage->m_vectNotes[i].m_rDateTime.Format( wxT("%d/%m/%y %H:%M:%S") ) );

		// :: 3 :: title
		m_pNotesTable->SetItem( nIndex, 3, m_pAstroImage->m_vectNotes[i].m_strTitle );
	}
}

////////////////////////////////////////////////////////////////////
// Method:		OnEartMapLocation
// Class:		CImageProperty
// Purpose:		move image in the image list up or down
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CImageProperty::OnEartMapLocation( wxCommandEvent& pEvent )
{
	int nCountryId = m_pObsCountry->GetSelection( );
	int nRegionId = m_pObsRegion->GetSelection( );
	int nCityId = m_pObsCity->GetSelection( );

	CEarthmMapDlg* pMapDlg = new CEarthmMapDlg( this, wxT("Earth Map - Set Your Location") );
//	m_pEarth->LoadAllCountries();
	pMapDlg->SetEarth( m_pEarth );
	pMapDlg->LoadData( this, m_pUnimapWorker );
	pMapDlg->SetConfig( NULL, nCountryId, nRegionId, nCityId );
	// set over manually - latitude/longitude and altitude
	pMapDlg->SetCustomPositionFromString( m_pObsLatitude->GetLineText(0),
											m_pObsLongitude->GetLineText(0),
											m_pObsAltitude->GetLineText(0) );

//	pMapDlg->m_pLatitude->SetValue( m_pObsLatitude->GetLineText(0) );
//	pMapDlg->m_pLongitude->SetValue( m_pObsLongitude->GetLineText(0) );
//	pMapDlg->m_pAltitude->SetValue( m_pObsAltitude->GetLineText(0) );

	if( pMapDlg->ShowModal() == wxID_OK )
	{
		int nCountryId = pMapDlg->m_pCountry->GetSelection();
		int nRegionId = pMapDlg->m_pRegion->GetSelection();
		int nCityId = pMapDlg->m_pCity->GetSelection();

		m_pObsCountry->SetSelection( nCountryId );
		m_pEarth->SetRegionsWidget( m_pObsRegion, nCountryId, nRegionId );
		m_pEarth->SetCitiesWidget( m_pObsCity, nCountryId, nRegionId, nCityId );

		// also get latitude/longitude and altitude
		m_pObsLatitude->SetValue( pMapDlg->m_pLatitude->GetLineText(0) );
		m_pObsLongitude->SetValue( pMapDlg->m_pLongitude->GetLineText(0) );
		m_pObsAltitude->SetValue( pMapDlg->m_pAltitude->GetLineText(0) );

		/* THIS TO GET LATTER AS NUMBER ?
		double lat, lon, alt;
		char strTmp[255], cho;
		int len=0;
		// :: latitude
		strcpy( strTmp, pMapDlg->m_pLatitude->GetLineText(0).ToAscii() );
		len = strlen(strTmp);
		if( len > 0 )
		{
			cho = strTmp[len-1];
			strTmp[len-1] = 0;
			lat = atof( strTmp );
		}
		*/

	}

	delete( pMapDlg );
}

////////////////////////////////////////////////////////////////////
// Method:		OnGeoIpLocation
// Class:		CImageProperty
// Purpose:		get location based on my own ip
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CImageProperty::OnGeoIpLocation( wxCommandEvent& pEvent )
{
	wxString strCountry;
	wxString strCity;
	double nLat, nLon;
	wxString strTmp;

	CGeoIp ip_locator;
	ip_locator.GetOwnIp( strCountry, strCity, nLat, nLon );

	// :: set latitude :: is NORTH to SOUTH 90 to -90 
/*	if( nLat >= 0 )
		sprintf( strTmp, "%.4fN", fabs(nLat) );
	else
		sprintf( strTmp, "%.4fS", fabs(nLat) );
*/	strTmp.Printf( wxT("%.6f"), nLat );
	m_pObsLatitude->SetValue( strTmp );

	// :: set longitude :: is WEST - EAST ... -180 to 180
/*	if( nLon >= 0 )
		sprintf( strTmp, "%.4fE", fabs(nLon) );
	else
		sprintf( strTmp, "%.4fW", fabs(nLon) );
*/	strTmp.Printf( wxT("%.6f"), nLon );
	m_pObsLongitude->SetValue( strTmp );

	// :: set country/city
	int bSolved = 0;
	int nRegionId=0, nCityId=0;
	int nCountryId = m_pEarth->FindCountryByName( strCountry );
	if( nCountryId >= 0 )
	{
		m_pObsCountry->SetSelection( nCountryId );
		if( m_pEarth->FindRegionCityByName( strCity, nCountryId, nRegionId, nCityId ) )	bSolved = 1;
	}
	// if not solved - find closest to coord
	if( !bSolved )
	{
		if( m_pEarth->FindClosestCity( nLat, nLon, 10.0, nCountryId, nRegionId, nCityId ) ) bSolved = 1;
	}
	// finally if found
	if( bSolved )
	{
		m_pObsCountry->SetSelection( nCountryId );
		m_pEarth->SetRegionsWidget( m_pObsRegion, nCountryId, nRegionId );
		m_pEarth->SetCitiesWidget( m_pObsCity, nCountryId, nRegionId, nCityId );

		strTmp.Printf( wxT("%.4f"), m_pEarth->m_vectCountry[nCountryId].vectRegion[nRegionId].vectCity[nCityId].alt );
		m_pObsAltitude->SetValue( strTmp );
	}
}

////////////////////////////////////////////////////////////////////
// Method:		OnNotebookTab
// Class:		CImageProperty
// Purpose:		when notebook tab has been pressed
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CImageProperty::OnNotebookTab( wxNotebookEvent& pEvent )
{
	int nId = pEvent.GetSelection();

	if( nId == m_nObserverPageId ) 
	{
		// here initialize my location map obj
		if( !m_pEarth ) InitEarthLocation( );
	}
}


////////////////////////////////////////////////////////////////////
void CImageProperty::InitEarthLocation( )
{
	m_pEarth = new CLocationEarth( m_pUnimapWorker );
//	m_pEarth->LoadLocations( );

	// get here ids from astro image pointer
	int nCountryId = m_pEarth->FindCountryByName( m_pAstroImage->m_strObsCountry );
	int nRegionId = 0;
	int nCityId = 0;
	if( nCountryId >= 0 )
	{
		nRegionId = m_pEarth->FindRegionByName( nCountryId, m_pAstroImage->m_strObsRegion );
		if( nRegionId >= 0 )
		{
			nCityId = m_pEarth->FindCityByName( nCountryId, nRegionId, m_pAstroImage->m_strObsCity );
			if( nCityId < 0 ) nCityId = 0;
		} else
			nRegionId = 0;
	} else
		nCountryId = 0;

	m_pEarth->SetCountriesWidget( m_pObsCountry, nCountryId );
	m_pEarth->SetRegionsWidget( m_pObsRegion, nCountryId, nRegionId );
	m_pEarth->SetCitiesWidget( m_pObsCity, nCountryId, nRegionId, nCityId );
}

////////////////////////////////////////////////////////////////////
void CImageProperty::OnSelectCountry( wxCommandEvent& pEvent )
{
	int nCountryId = m_pObsCountry->GetSelection( );

	m_pEarth->SetRegionsWidget( m_pObsRegion, nCountryId, 0 );
	m_pEarth->SetCitiesWidget( m_pObsCity, nCountryId, 0, 0 );
	SetCity( );
	return;
}

////////////////////////////////////////////////////////////////////
void CImageProperty::OnSelectRegion( wxCommandEvent& pEvent )
{
	int nCountryId = m_pObsCountry->GetSelection( );
	int nRegionId = m_pObsRegion->GetSelection( );

	m_pEarth->SetCitiesWidget( m_pObsCity, nCountryId, nRegionId, 0 );
	SetCity( );
	return;
}

////////////////////////////////////////////////////////////////////
void CImageProperty::OnSelectCity( wxCommandEvent& pEvent )
{
	SetCity( );
	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetCity
// Class:	CImageProperty
// Purpose:	set current city info
// Input:	nothing
// Output:	status
////////////////////////////////////////////////////////////////////
void CImageProperty::SetCity( )
{
	wxString strTmp;

	int nCountryId = m_pObsCountry->GetSelection( );
	int nRegionId = m_pObsRegion->GetSelection( );
	int nCityId = m_pObsCity->GetSelection( );

	if( nCountryId < 0 || nRegionId < 0 || nCityId < 0 ) return;

	// :: LATITUDE :: is NORTH to SOUTH 90 to -90 
	double lat = m_pEarth->m_vectCountry[nCountryId].vectRegion[nRegionId].vectCity[nCityId].lat;
/*	if( lat >= 0 )
		sprintf( strTmp, "%.4fN", fabs(lat) );
	else
		sprintf( strTmp, "%.4fS", fabs(lat) );
*/	strTmp.Printf( wxT("%.6f"), lat );
	m_pObsLatitude->SetValue( strTmp );

	// :: LONGITUDE :: is WEST - EAST ... -180 to 180
	double lon = m_pEarth->m_vectCountry[nCountryId].vectRegion[nRegionId].vectCity[nCityId].lon;
/*	if( lon >= 0 )
		sprintf( strTmp, "%.4fE", fabs(lon) );
	else
		sprintf( strTmp, "%.4fW", fabs(lon) );
*/	strTmp.Printf( wxT("%.6f"), lon );
	m_pObsLongitude->SetValue( strTmp );

	// :: ALTITUDE
	strTmp.Printf( wxT("%.4f"), m_pEarth->m_vectCountry[nCountryId].vectRegion[nRegionId].vectCity[nCityId].alt );
	m_pObsAltitude->SetValue( strTmp );
}

////////////////////////////////////////////////////////////////////
void CImageProperty::OnGetWeatherData( wxCommandEvent& pEvent )
{
	double lat=0.0, lon=0.0;
	wxString strTmp;
	int nSourceId = m_pWeatherGetSource->GetSelection( );

	// process lat/lon from current location properties panel
	wxString strLat = m_pObsLatitude->GetLineText(0).Trim(0).Trim(1);
	if( !strLat.ToDouble( &lat ) ) lat = 0.0;
	wxString strLon = m_pObsLongitude->GetLineText(0).Trim(0).Trim(1);
	if( !strLon.ToDouble( &lon ) ) lon = 0.0;
	// set date/time - WITH TIME ZONE
	wxDateTime obsDateTime = m_pObsDate->GetValue();
	obsDateTime.ParseTime( m_pObsTime->GetLineText(0).Trim() );
	// calculate/set time zone
	int nTZShift = (int) round( lon/15.0 );
	double nObsTZSecs = (long) nTZShift * 3600;
	double timeObs = obsDateTime.FromUTC().GetJDN()-nObsTZSecs/86400.0;

	// fetch weather
	CWeather src;

	//////////////
	// set worker cmd action
	DefCmdAction act;
	act.id = THREAD_ACTION_GET_HIST_WEATER_DATA;
	act.vectObj[0] = (void*) &src;
	act.vectFloat[0] = lat;
	act.vectFloat[1] = lon;
	act.vectFloat[2] = timeObs;

	CWaitDialog waitDialog( this, wxT("Fetch Weather Data"), 
					wxT("Get Weather Data For Location..."), 0, m_pUnimapWorker, &act );
	waitDialog.ShowModal();

	////////
	// check for errors
	if( src.m_nFetchError ) 
	{
		wxMessageBox( src.m_strFetchError, wxT("Weather Data Fetch"), wxOK|wxICON_ERROR );

	} else
	{
		//src.GetCurrentWeatherData( lat, lon, nSourceId );
	//	src.GetWeatherData_AtDateTime( lat, lon, timeObs, 0 );

		//////////
		// todo: add here check if data was fetched ...

		// now set the menu items
		strTmp.Printf( wxT("%.1f"), src.m_rCurrent.wind );
		m_pCondWindSpeed->SetLabel( strTmp );
		strTmp.Printf( wxT("%.1f"), src.m_rCurrent.pressure );
		m_pCondAtmPressure->SetLabel( strTmp );
		strTmp.Printf( wxT("%.1f"), src.m_rCurrent.temperature );
		m_pCondTemp->SetLabel( strTmp );
		strTmp.Printf( wxT("%.1f"), src.m_rCurrent.temperature_var );
		m_pCondTempVar->SetLabel( strTmp );
		strTmp.Printf( wxT("%.1f"), src.m_rCurrent.humidity );
		m_pCondHumid->SetLabel( strTmp );
	}

	return;
}

////////////////////////////////////////////////////////////////////
void CImageProperty::OnDateTimeSet( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();

	if( nId == wxID_BUTTON_DATETIME_NOW )
	{
		wxDateTime now;
		now.SetToCurrent();
		m_pObsDate->SetValue( now ); 
		m_pObsTime->SetLabel( now.Format( wxT("%H:%M:%S") ) );

	} else if( nId == wxID_BUTTON_DATETIME_EXIF )
	{
		// :: observer date/time
		m_pObsDate->SetValue( m_pAstroImage->m_nObsDateTime );
		m_pObsTime->SetLabel( m_pAstroImage->m_nObsDateTime.Format( wxT("%H:%M:%S") ) );

	} else if( nId == wxID_BUTTON_DATETIME_FILE )
	{
		wxDateTime now( m_pAstroImage->m_nFileTime );
		m_pObsDate->SetValue( now );
		m_pObsTime->SetLabel( now.Format( wxT("%H:%M:%S") ) );

	}

	return;
}

////////////////////////////////////////////////////////////////////
void CImageProperty::OnObserverName( wxCommandEvent& pEvent )
{
	m_pObsName->SetLabel( m_pObserver->GetOberverFullName() );
}

////////////////////////////////////////////////////////////////////
void CImageProperty::OnMySitesLocation( wxCommandEvent& pEvent )
{
	if( m_pObserver->m_nSite <= 0 ) return;
	int i=0;
	wxString strTmp;

	CSimpleEntryAddDlg *pUseSite = new CSimpleEntryAddDlg( this, wxT("Select a site..."), wxT("Site Name:"), 1 );
	pUseSite->m_pNameSelect->Clear();
	for( i=0; i<m_pObserver->m_nSite; i++ ) pUseSite->m_pNameSelect->Append( m_pObserver->m_vectSite[i]->m_strSiteName );
	pUseSite->m_pNameSelect->SetSelection(0);

	if ( pUseSite->ShowModal( ) == wxID_OK )
	{
		int nId = pUseSite->m_pNameSelect->GetSelection();
		// :: set country/city
		int bSolved = 0;
		int nRegionId=0, nCityId=0;
		int nCountryId = m_pEarth->FindCountryByName( m_pObserver->m_vectSite[nId]->m_strCountry );
		if( nCountryId >= 0 )
		{
			m_pObsCountry->SetSelection( nCountryId );
			if( m_pEarth->FindRegionCityByName( m_pObserver->m_vectSite[nId]->m_strCity, nCountryId, nRegionId, nCityId ) )	bSolved = 1;
		}

		// if not solved - find closest to coord
		if( !bSolved )
		{
			if( m_pEarth->FindClosestCity( m_pObserver->m_vectSite[nId]->m_nLatitude, m_pObserver->m_vectSite[nId]->m_nLongitude, 
											10.0, nCountryId, nRegionId, nCityId ) ) bSolved = 1;
		}

		// finally if found
		if( bSolved )
		{
			m_pObsCountry->SetSelection( nCountryId );
			m_pEarth->SetRegionsWidget( m_pObsRegion, nCountryId, nRegionId );
			m_pEarth->SetCitiesWidget( m_pObsCity, nCountryId, nRegionId, nCityId );

			strTmp.Printf( wxT("%.4f"), m_pEarth->m_vectCountry[nCountryId].vectRegion[nRegionId].vectCity[nCityId].alt );
			m_pObsAltitude->SetValue( strTmp );
		}
	}
	delete( pUseSite );
}

////////////////////////////////////////////////////////////////////
void CImageProperty::OnLightPollutionGet( wxCommandEvent& pEvent )
{
	if( !m_pEarth ) InitEarthLocation( );
	double lat=0.0, lon=0.0;

	wxString strLat = m_pObsLatitude->GetLineText(0).Trim(0).Trim(1);
	if( !strLat.ToDouble( &lat ) ) lat=0.0;
	wxString strLon = m_pObsLongitude->GetLineText(0).Trim(0).Trim(1);
	if( !strLon.ToDouble( &lon ) ) lon=0.0;

	////////////////////
	// call to process - todo: make this nicer!!!
	//double level = m_pEarth->GetLightPollutionLevel( lat, lon );
	m_pEarth->SetCurrentLatLon( lat, lon );
//	m_pUnimapWorker->m_pEarth = m_pEarth;
//	m_pUnimapWorker->m_pValueSelect = m_pCondLp;

	///////////
	DefCmdAction act;
	act.id = THREAD_ACTION_LP_MAP_PROCESS;
	act.vectObj[0] = (void*) m_pEarth;
	act.vectObj[1] = (void*) m_pCondLp;

	/////////////////
	// show wait dialog
	CWaitDialog* pWaitDialog = new CWaitDialog( this, wxT("Process Light Pollution Map"), 
						wxT("Load Light Pollution Map ..."), 0, m_pUnimapWorker, &act );
	pWaitDialog->ShowModal();
	// delete/reset wait dialog
	delete( pWaitDialog );
	pWaitDialog = NULL;

}

////////////////////////////////////////////////////////////////////
void CImageProperty::SetLPLevel( double level )
{
	// set selection by level
	if( level >= 0.0 && level < 1.0 )		// none
		m_pCondLp->SetSelection(4);
	else if( level >= 1.0 && level < 2.0 )	// light
		m_pCondLp->SetSelection(2);
	else if( level >= 2.0 && level < 5.0 )	// medium
		m_pCondLp->SetSelection(1);
	else if( level >= 5.0 && level <= 7.0 )	// heavy
		m_pCondLp->SetSelection(0);
}

// notes action handler
////////////////////////////////////////////////////////////////////
void CImageProperty::OnNotesAction( wxCommandEvent& pEvent )
{
	int nActId = pEvent.GetId();

	////////////////////////////////
	// :: ADD NOTE
	if( nActId == wxID_BUTTON_NOTES_ADD )
	{
		CImageNotesDlg* pDlg =  new CImageNotesDlg( this, wxT("Add Image Note") );
		// set local note
		CImageNote* pNote = new CImageNote( m_pAstroImage );
		pDlg->SetConfig( pNote );
		// show dialog and wait ok 
		if( pDlg->ShowModal() == wxID_OK )
		{
			// add new note to the image notes or ?
			pNote->m_nType = pDlg->m_pNoteType->GetSelection();
			pNote->m_strTitle = pDlg->m_pNoteTitle->GetLineText(0);
			pNote->m_strMessage = pDlg->m_pNoteBody->GetValue();
			pNote->m_rDateTime.SetToCurrent();

			// add note to image
			m_pAstroImage->AddNote( pNote );

		} else
		{
			// remove audio file/folder if empty
		}
		// remove objects
		delete( pDlg );
		delete( pNote );

	////////////////////////////////////////////////
	// :: EDIT NOTE
	} else if( nActId == wxID_BUTTON_NOTES_EDIT )
	{
		// get curren note selected
		long item = -1;
		// get first item only
        item = m_pNotesTable->GetNextItem( item, wxLIST_NEXT_ALL,
											wxLIST_STATE_SELECTED );
		// if no item return
		if( item < 0 ) return;
  
		// get a copy of the note
		CImageNote* pNote = new CImageNote( m_pAstroImage );
		pNote->CopyFrom( m_pAstroImage->m_vectNotes[item] );

		// create the edit dialog
		CImageNotesDlg* pDlg =  new CImageNotesDlg( this, wxT("Edit Note"), 1 );
		// set config of note
		pDlg->SetConfig( pNote );

		// show dialog and wait ok 
		if( pDlg->ShowModal() == wxID_OK )
		{
			// add new note to the image notes or ?
			pNote->m_nType = pDlg->m_pNoteType->GetSelection();
			pNote->m_strTitle = pDlg->m_pNoteTitle->GetLineText(0);
			pNote->m_strMessage = pDlg->m_pNoteBody->GetValue();
			// todo: here story this in a diferent location ie: update
			pNote->m_rDateTime.SetToCurrent();

			// update note's values
			m_pAstroImage->UpdateNote( item, pNote );

		} else
		{
			// remove audio file/folder if empty 
			// check if different from what was before
		}
		// remove objects
		delete( pDlg );
		delete( pNote );

	//////////////////////////////////
	// :: DELETE NOTE
	} else if( nActId == wxID_BUTTON_NOTES_REMOVE )
	{
		// get curren note selected
		long item = -1;
		// get first item only
        item = m_pNotesTable->GetNextItem( item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
		// if no item return
		if( item < 0 ) return;

		// delete from vector element at that position
		m_pAstroImage->m_vectNotes.erase( m_pAstroImage->m_vectNotes.begin()+item );
	}

	SetNotesList( );
}

////////////////////////////////////////////////////////////////////
void CImageProperty::OnNotesListSelect( wxListEvent& pEvent )
{
	// get curren note selected
	long item = -1;
	// get first item only
    item = m_pNotesTable->GetNextItem( item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );

	if( item >= 0 )
	{
		m_pNotesEditButton->Enable();
		m_pNotesRemoveButton->Enable();

	} else
	{
		m_pNotesEditButton->Disable();
		m_pNotesRemoveButton->Disable();
	}
}

////////////////////////////////////////////////////////////////////
void CImageProperty::OnNotesListAction( wxListEvent& pEvent )
{
	int nId = pEvent.GetIndex();

	CImageNotesDlg* pDlg =  new CImageNotesDlg( this, wxT("View Note"), 3 );
	// set config of note
	pDlg->SetConfig( &(m_pAstroImage->m_vectNotes[nId]) );

	// show dialog 
	pDlg->ShowModal();
	
	delete( pDlg );

	return;
}
