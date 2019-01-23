
// wx includes
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/datetime.h>

// local headers
#include "../weather/weather.h"
#include "../observer/observer_site.h"
#include "../unimap_worker.h"
#include "waitdialog.h"
#include "../observer/observer.h"
#include "location_dialog.h"
#include "../land/geoip.h"

// include main header
#include "weather_cforecast_dlg.h"

BEGIN_EVENT_TABLE(CWeatherCForecastDlg, wxDialog)
	EVT_RADIOBUTTON( wxID_WEATHER_FORECAST_POS_TYPE_A, CWeatherCForecastDlg::OnPosType )
	EVT_RADIOBUTTON( wxID_WEATHER_FORECAST_POS_TYPE_B, CWeatherCForecastDlg::OnPosType )
	EVT_TEXT( wxID_WEATHER_FORECAST_SET_LAT, CWeatherCForecastDlg::OnSetLatLon )
	EVT_TEXT( wxID_WEATHER_FORECAST_SET_LON, CWeatherCForecastDlg::OnSetLatLon )
	EVT_CHOICE( wxID_WEATHER_FORECAST_SITE_SELECT, CWeatherCForecastDlg::OnChangeSite )
	EVT_BUTTON( wxID_WEATHER_FORECAST_BUTTON_GET_DATA, CWeatherCForecastDlg::OnGetCustomData )
	// pos custom
	EVT_BUTTON( wxID_WEATHER_FORECAST_BUTTON_MAP, CWeatherCForecastDlg::OnEartMapLocation )
	EVT_BUTTON( wxID_WEATHER_FORECAST_BUTTON_GEOIP, CWeatherCForecastDlg::OnGeoIpLocation )
	// day forecast request
	EVT_RADIOBUTTON( -1, CWeatherCForecastDlg::OnSelectDay )
END_EVENT_TABLE()
// Method:	Constructor/Destructor
////////////////////////////////////////////////////////////////////
CWeatherCForecastDlg::CWeatherCForecastDlg( wxWindow *parent, const wxString& title, 
							CObserverSite* pSite, CUnimapWorker* pUnimapWorker ) : wxDialog(parent, -1,
							title, wxDefaultPosition, wxDefaultSize,
							wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL|wxRESIZE_BORDER )
{
	m_pSite = pSite;
	m_pWeather = pSite->m_pWeather;
	m_pUnimapWorker = pUnimapWorker;
	m_pWaitDlg = NULL;
	m_pCustomWeather = NULL;
	m_nDay = 0;

	// calculate layout for details
	int nDCol = 8;
	int nDRealStart = nDCol-m_pWeather->m_vectForecastRec[0].nDetail;

	int i=0;
	wxString strTmp;

	wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

	/////////////////////////////
	// main sizer
	wxFlexGridSizer* mainSizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	mainSizer->AddGrowableRow( 1 );

	////////////////
	// SITE/LOC SIZER
	wxFlexGridSizer* sizerPosData = new wxFlexGridSizer( 1, 2, 5, 10 );
	sizerPosData->AddGrowableCol( 0 );
	sizerPosData->AddGrowableCol( 1 );
	mainSizer->Add( sizerPosData, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND );

	// current weather data
	wxStaticBox* pCurrentBox = new wxStaticBox( this, -1, wxT("Current Weather Conditions"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* sizerCurrentBox = new wxStaticBoxSizer( pCurrentBox, wxHORIZONTAL );
	mainSizer->Add( sizerCurrentBox, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND );
	// data sizer
	wxFlexGridSizer* sizerCurrentData = new wxFlexGridSizer( 1, 2, 0, 0 );
	//sizerCurrentData->SetFlexibleDirection( wxVERTICAL );
	sizerCurrentData->AddGrowableCol( 1 );
	sizerCurrentBox->Add( sizerCurrentData, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND );
	/////////
	// forecast weather data
	wxStaticBox* pDailyDetailsBox = new wxStaticBox( this, -1, wxT("Daily Forecast"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* sizerDailyDetailsBox = new wxStaticBoxSizer( pDailyDetailsBox, wxHORIZONTAL );
	mainSizer->Add( sizerDailyDetailsBox, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND );
	// data sizer
	wxFlexGridSizer* sizerForecastData = new wxFlexGridSizer( 5, m_pWeather->m_nForecastRec+1, 3, 10 );
//	sizerForecastData->AddGrowableCol( 0 );
	sizerForecastData->SetFlexibleDirection( wxVERTICAL );
	sizerDailyDetailsBox->Add( sizerForecastData, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND );
	/////////
	// forecast weather data :: daily details
	wxStaticBox* pDayDetailsBox = new wxStaticBox( this, -1, wxT("Hourly Forecast"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* sizerDayDetailsBox = new wxStaticBoxSizer( pDayDetailsBox, wxVERTICAL );
	mainSizer->Add( sizerDayDetailsBox, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND );
	// data sizer
	sizerDayDetails = new wxFlexGridSizer( 5, nDCol+1, 3, 10 );
//	for( i=1; i<8; i++) sizerDayDetails->AddGrowableCol( i );
	sizerDayDetails->SetFlexibleDirection( wxVERTICAL );
	sizerDayDetailsBox->Add( sizerDayDetails, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxGROW|wxEXPAND );

	//////////////////////////////////////////////////////////
	// create gui data :: CURRENT WEATHER
	strTmp.Printf( wxT("./resources/weather/%s.gif"), m_pWeather->m_rCurrent.cond_icon );
	m_pCurrentIcon = new wxStaticBitmap( this, -1, wxBitmap( strTmp, wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxSize(64,64) );
	sizerCurrentData->Add( m_pCurrentIcon, 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxALL, 10 );
	// data grid
	wxFlexGridSizer* sizerCurrentDataGrid = new wxFlexGridSizer( 2, 1, 0, 0 );
	sizerCurrentDataGrid->SetFlexibleDirection( wxVERTICAL );
	sizerCurrentDataGrid->AddGrowableRow( 1 );
	sizerCurrentData->Add( sizerCurrentDataGrid, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 10 );
	// value labels
	wxFlexGridSizer* sizerCurrentValueTop = new wxFlexGridSizer( 1, 2, 5, 5 );
	sizerCurrentValueTop->AddGrowableCol( 0 );
	sizerCurrentValueTop->AddGrowableCol( 1 );
	sizerCurrentDataGrid->Add( sizerCurrentValueTop, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND );
	// :: Conditions
//	sizerCurrentValueTop->Add( new wxStaticText( this, -1, _T("Current Conditions:") ), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL );
	strTmp.Printf( wxT("Current Conditions: %s"), m_pWeather->m_rCurrent.cond_text );
	m_pCurrentCond = new wxStaticText( this, -1, strTmp );
	sizerCurrentValueTop->Add( m_pCurrentCond, 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );
	// :: Updated
//	sizerCurrentValueTop->Add( new wxStaticText( this, -1, _T("Data Updated at:") ), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL );
	wxDateTime update_time( m_pWeather->m_rCurrent.datetime );
	strTmp.Printf( wxT("Data Updated at: %s"), update_time.Format( wxT("%H:%M:%S %d/%m/%Y") ) );
	m_pCurrentDatetime = new wxStaticText( this, -1, strTmp );
	sizerCurrentValueTop->Add( m_pCurrentDatetime, 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );

	//////////
	// values sizer/elemnts
	wxFlexGridSizer* sizerCurrentValue = new wxFlexGridSizer( 3, 8, 5, 5 );
	sizerCurrentValue->SetFlexibleDirection( wxVERTICAL );
	sizerCurrentDataGrid->Add( sizerCurrentValue, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 10 );
	// :: temperature
	sizerCurrentValue->Add( new wxStaticText( this, -1, wxT("Temperature:") ), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL );
	strTmp.Printf( wxT("%d °C"), (int) m_pWeather->m_rCurrent.temperature );
	m_pCurrentTemp = new wxStaticText( this, -1, strTmp );
	sizerCurrentValue->Add( m_pCurrentTemp, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	// :: Dew Point
	sizerCurrentValue->Add( new wxStaticText( this, -1, wxT("Dew Point:") ), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL );
	strTmp.Printf( wxT("%d °C"), (int) m_pWeather->m_rCurrent.dew_point );
	m_pCurrentDew = new wxStaticText( this, -1, strTmp );
	sizerCurrentValue->Add( m_pCurrentDew, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	// :: windchill
	sizerCurrentValue->Add( new wxStaticText( this, -1, wxT("Windchill:") ), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL );
	strTmp.Printf( wxT("%d °C"), (int) m_pWeather->m_rCurrent.windchill );
	m_pCurrentWindchill = new wxStaticText( this, -1, strTmp );
	sizerCurrentValue->Add( m_pCurrentWindchill, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	// :: Humidity
	sizerCurrentValue->Add( new wxStaticText( this, -1, wxT("Humidity:") ), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL );
	strTmp.Printf( wxT("%d%s"), (int) m_pWeather->m_rCurrent.humidity, "%" );
	m_pCurrentHumidity = new wxStaticText( this, -1, strTmp );
	sizerCurrentValue->Add( m_pCurrentHumidity, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	// :: Wind Direction
	sizerCurrentValue->Add( new wxStaticText( this, -1, wxT("Wind Direction:") ), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL );
	strTmp.Printf( wxT("%d°/%s"), (int) m_pWeather->m_rCurrent.wind_dir, m_pWeather->m_rCurrent.wind_dir_str );
	m_pCurrentWinddir = new wxStaticText( this, -1, strTmp );
	sizerCurrentValue->Add( m_pCurrentWinddir, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	// :: Wind Speed
	sizerCurrentValue->Add( new wxStaticText( this, -1, wxT("Wind Speed:") ), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL );
	strTmp.Printf( wxT("%d km/h"), (int) m_pWeather->m_rCurrent.wind );
	m_pCurrentWind = new wxStaticText( this, -1, strTmp );
	sizerCurrentValue->Add( m_pCurrentWind, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	// :: Pressure
	sizerCurrentValue->Add( new wxStaticText( this, -1, wxT("Pressure:") ), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL );
	strTmp.Printf( wxT("%d hPa"), (int) m_pWeather->m_rCurrent.pressure );
	m_pCurrentPressure = new wxStaticText( this, -1, strTmp );
	sizerCurrentValue->Add( m_pCurrentPressure, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	// :: Visibility
	sizerCurrentValue->Add( new wxStaticText( this, -1, wxT("Visibility:") ), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL );
	strTmp.Printf( wxT("%d km"), (int) m_pWeather->m_rCurrent.visibility );
	m_pCurrentVis = new wxStaticText( this, -1, strTmp );
	sizerCurrentValue->Add( m_pCurrentVis, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );

	/////////////////////////////////
	// :: navigation controls
	m_pPosTypeA = new wxRadioButton( this,  wxID_WEATHER_FORECAST_POS_TYPE_A, _T(""), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_pSiteLabel = new wxStaticText( this, -1, wxT("Site:"));
	m_pSiteSelect = new wxChoice( this, wxID_WEATHER_FORECAST_SITE_SELECT, wxPoint(-1, -1), wxSize(160,-1), 0, NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: pos type addr
	m_pPosTypeB = new wxRadioButton( this,  wxID_WEATHER_FORECAST_POS_TYPE_B, _T("") );
	m_pLatLonLabel = new wxStaticText( this, -1, wxT("Lat(°N)/Lon(°E):"));
	m_pLatValue = new wxTextCtrl( this, wxID_WEATHER_FORECAST_SET_LAT, wxT(""), wxDefaultPosition, wxSize(50,-1) );
	m_pLonValue = new wxTextCtrl( this, wxID_WEATHER_FORECAST_SET_LON, wxT(""), wxDefaultPosition, wxSize(50,-1) );
	m_pMapButton = new wxButton( this, wxID_WEATHER_FORECAST_BUTTON_MAP, wxT("Map"), wxPoint(-1, -1), wxSize(30,-1) );
	m_pGeoIpButton = new wxButton( this, wxID_WEATHER_FORECAST_BUTTON_GEOIP, wxT("GeoIp"), wxPoint(-1, -1), wxSize(40,-1) );
	m_pGpsButton = new wxButton( this, wxID_WEATHER_FORECAST_BUTTON_GPS, wxT("Gps"), wxPoint(-1, -1), wxSize(30,-1) );
	m_pGetDataButton = new wxButton( this, wxID_WEATHER_FORECAST_BUTTON_GET_DATA, wxT("Get Data"), wxPoint(-1, -1), wxSize(55,-1) );
	m_pGpsButton->Disable();

	//////////////////////////
	// LAYOUT
	// :: site select
	wxFlexGridSizer* sizerPosAData = new wxFlexGridSizer( 1, 3, 0, 5 );
	sizerPosAData->Add( m_pPosTypeA, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL  );
	sizerPosAData->Add( m_pSiteLabel, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL  );
	sizerPosAData->Add( m_pSiteSelect, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	sizerPosData->Add( sizerPosAData, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL  );
	// :: coord select
	wxFlexGridSizer* sizerPosBData = new wxFlexGridSizer( 1, 8, 0, 5 );
	sizerPosBData->Add( m_pPosTypeB, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL  );
	sizerPosBData->Add( m_pLatLonLabel, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL  );
	sizerPosBData->Add( m_pLatValue, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	sizerPosBData->Add( m_pLonValue, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	// buttons
	sizerPosBData->Add( m_pMapButton, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	sizerPosBData->Add( m_pGeoIpButton, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	sizerPosBData->Add( m_pGpsButton, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	sizerPosBData->Add( m_pGetDataButton, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 10 );
	sizerPosData->Add( sizerPosBData, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL  );

	//////////////////////////////////////////////////////////
	// create gui data :: FORECAST DAILY
	for( i=0; i<10; i++ )
	{
		// init here the day radio button as well
		m_vectDayDetailSelect[i] = NULL;

		m_vectDate[i] = NULL;
		m_vectWday[i] = NULL;
		m_vectIcons[i] = NULL;
		m_vectLabel[i] = NULL;
		m_vectTempDay[i] = NULL;
		m_vectTempNight[i] = NULL;
	}
	// :: forecast :: date
	sizerForecastData->AddStretchSpacer();
	for( i=0; i<m_pWeather->m_nForecastRec;  i++ )
	{
		wxDateTime dt(m_pWeather->m_vectForecastRec[i].epoch);
		m_vectDate[i] = new wxStaticText( this, -1, dt.Format( wxT("%d/%m/%Y") ) );
		sizerForecastData->Add( m_vectDate[i], 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );
	}
	// :: forecast :: weekday
	sizerForecastData->AddStretchSpacer();
	for( i=0; i<m_pWeather->m_nForecastRec;  i++ )
	{
	//	sizerForecastData->Add( new wxStaticText( this, -1, _T(m_pWeather->m_vectForecastRec[i].weekday)), 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );
		wxDateTime dt(m_pWeather->m_vectForecastRec[i].epoch);
		m_vectWday[i] = new wxStaticText( this, -1, dt.Format( wxT("%A") ) );
		sizerForecastData->Add( m_vectWday[i], 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );
	}
	// :: forecast :: icons
	sizerForecastData->AddStretchSpacer();
	for( i=0; i<m_pWeather->m_nForecastRec;  i++ )
	{
		strTmp.Printf( wxT("./resources/weather/%s.gif"), m_pWeather->m_vectForecastRec[i].cond_icon_night );
		m_vectIcons[i] = new wxStaticBitmap( this, -1, wxBitmap( strTmp, wxBITMAP_TYPE_ANY ), 
											wxDefaultPosition, wxSize( 64, 64 ) );
		// add icon to sizer
//		if( i == 0 )
			sizerForecastData->Add( m_vectIcons[i], 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );
//		else
//			sizerForecastData->Add( m_vectIcons[i], 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxLEFT, 20 );
	}
	// :: forecast :: labels
	sizerForecastData->AddStretchSpacer();
	for( i=0; i<m_pWeather->m_nForecastRec;  i++ )
	{
		m_vectLabel[i] = new wxStaticText(this, -1, m_pWeather->m_vectForecastRec[i].cond_text_night);
		sizerForecastData->Add( m_vectLabel[i], 0, wxALIGN_CENTER|wxALIGN_TOP|wxBOTTOM, 5 );
	}
	// :: forecast :: temperature high
	sizerForecastData->Add( new wxStaticText( this, -1, wxT("Temp Day/High:")), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL );
	for( i=0; i<m_pWeather->m_nForecastRec;  i++ )
	{
		strTmp.Printf( wxT("%d °C"), (int) m_pWeather->m_vectForecastRec[i].temperature_day );
		m_vectTempDay[i] = new wxStaticText( this, -1, strTmp);
		sizerForecastData->Add( m_vectTempDay[i], 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );
	}
	// :: forecast :: temperature low
	sizerForecastData->Add( new wxStaticText( this, -1, wxT("Temp Night/Low:")), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL );
	for( i=0; i<m_pWeather->m_nForecastRec;  i++ )
	{
		strTmp.Printf( wxT("%d °C"), (int) m_pWeather->m_vectForecastRec[i].temperature_night );
		m_vectTempNight[i] = new wxStaticText( this, -1, strTmp);
		sizerForecastData->Add( m_vectTempNight[i], 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );
	}
	// :: forecast :: radio button day detail
	sizerForecastData->AddStretchSpacer();
	// larry :: here limit to 8 columns as per layout instead of number of records - to do this starts mondays
	for( i=0; i<nDCol /*m_pWeather->m_nForecastRec*/;  i++ )
	{
		if( i == 0 )
			m_vectDayDetailSelect[i] = new wxRadioButton( this, wxID_DAY_DETAIL_SELECT_START, _T(""), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
		else
			m_vectDayDetailSelect[i] = new wxRadioButton( this, wxID_DAY_DETAIL_SELECT_START+i, _T("") );
		// add to sizer
		sizerForecastData->Add( m_vectDayDetailSelect[i], 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxTOP, 5 );
	}

	///////////////////////
	// HOURLY FORECAST
	// :: time
	sizerDayDetails->AddStretchSpacer();
	for( i=0; i<nDCol; i++ )
	{
		if( i < nDRealStart )
			strTmp.Printf( wxT("00:00") );
		else
			strTmp.Printf( wxT("%d:00"), m_pWeather->m_vectForecastRec[0].vectDetail[i-nDRealStart].time );

		m_vectDayTime[i] = new wxStaticText( this, -1, strTmp);
		sizerDayDetails->Add( m_vectDayTime[i], 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );
	}
	// :: icons
	sizerDayDetails->AddStretchSpacer();
	for( i=0; i<nDCol; i++ )
	{
		if( i < nDRealStart )
			strTmp.Printf( wxT("./resources/weather/unknown.gif") );
		else
			strTmp.Printf( wxT("./resources/weather/%s.gif"), m_pWeather->m_vectForecastRec[0].vectDetail[i-nDRealStart].cond_icon );
		m_vectDayIcons[i] = new wxStaticBitmap( this, -1, wxBitmap( strTmp, wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxSize(64,64) );
		// add icon to sizer
		sizerDayDetails->Add( m_vectDayIcons[i], 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5 );
	}
	// :: forecast :: labels
	sizerDayDetails->AddStretchSpacer();
	for( i=0; i<nDCol; i++ )
	{
		if( i < nDRealStart )
			strTmp.Printf( wxT("n/a") );
		else
			strTmp.Printf( wxT("%s"), m_pWeather->m_vectForecastRec[0].vectDetail[i-nDRealStart].cond_text );
		
		m_vectDayLabel[i] = new wxStaticText( this, -1, strTmp);
		sizerDayDetails->Add( m_vectDayLabel[i], 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxBOTTOM, 5 );
	}
	// :: forecast :: temperature
	sizerDayDetails->Add( new wxStaticText( this, -1, wxT("Temperature:")), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL );
	for( i=0; i<nDCol; i++ )
	{
		if( i < nDRealStart )
			strTmp.Printf( wxT("n/a") );
		else
			strTmp.Printf( wxT("%d °C"), (int) m_pWeather->m_vectForecastRec[0].vectDetail[i-nDRealStart].temperature );

		m_vectDayTemperature[i] = new wxStaticText( this, -1, strTmp);
		sizerDayDetails->Add( m_vectDayTemperature[i], 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );
	}
	// :: forecast :: humidity
	sizerDayDetails->Add( new wxStaticText( this, -1, wxT("Humidity:")), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL );
	for( i=0; i<nDCol; i++ )
	{
		if( i < nDRealStart )
			strTmp.Printf( wxT("n/a") );
		else
			strTmp.Printf( wxT("%d%s"), (int) m_pWeather->m_vectForecastRec[0].vectDetail[i-nDRealStart].humidity, "%" );

		m_vectDayHumidity[i] = new wxStaticText( this, -1, strTmp);
		sizerDayDetails->Add( m_vectDayHumidity[i], 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );
	}
	// :: forecast :: precipitations 
	sizerDayDetails->Add( new wxStaticText( this, -1, wxT("Precipitations:")), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL );
	for( i=0; i<nDCol; i++ )
	{
		if( i < nDRealStart )
			strTmp.Printf( wxT("n/a") );
		else
			strTmp.Printf( wxT("%d%s"), (int) m_pWeather->m_vectForecastRec[0].vectDetail[i-nDRealStart].precip, "%" );

		m_vectDayPrecip[i] = new wxStaticText( this, -1, strTmp);
		sizerDayDetails->Add( m_vectDayPrecip[i], 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );
	}
	// :: forecast :: cloud cover 
	sizerDayDetails->Add( new wxStaticText( this, -1, wxT("Clouds Cover:")), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL );
	for( i=0; i<nDCol; i++ )
	{
		if( i < nDRealStart )
			strTmp.Printf( wxT("n/a") );
		else
			strTmp.Printf( wxT("%d%s"), (int) m_pWeather->m_vectForecastRec[0].vectDetail[i-nDRealStart].clouds, "%" );

		m_vectDayClouds[i] = new wxStaticText( this, -1, strTmp);
		sizerDayDetails->Add( m_vectDayClouds[i], 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );
	}

	// add main sizer
	topsizer->Add( mainSizer, 1, wxGROW|wxALL, 5 );
	// add buttons ok
//	topsizer->Add( CreateButtonSizer(wxOK), 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxALL, 10 );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
	Layout();
//	sizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
CWeatherCForecastDlg::~CWeatherCForecastDlg( )
{
	if( m_pCustomWeather ) delete( m_pCustomWeather );

	delete( m_pPosTypeA );
	delete( m_pPosTypeB );
	delete( m_pLatLonLabel );
	delete( m_pLatValue );
	delete( m_pLonValue );
	delete( m_pMapButton );
	delete( m_pGeoIpButton );
	delete( m_pGpsButton );
	delete( m_pGetDataButton );
	delete( m_pSiteLabel );
	delete( m_pSiteSelect );

	delete( m_pCurrentIcon );
	delete( m_pCurrentCond );
	delete( m_pCurrentDatetime );
	delete( m_pCurrentTemp );
	delete( m_pCurrentDew );
	delete( m_pCurrentWindchill );
	delete( m_pCurrentHumidity );
	delete( m_pCurrentWinddir );
	delete( m_pCurrentWind );
	delete( m_pCurrentPressure );
	delete( m_pCurrentVis );

	for( int i=0; i<10/*m_pWeather->m_nForecastRec*/;  i++ )
	{
		if( m_vectDate[i] ) delete( m_vectDate[i] );
		if( m_vectWday[i] ) delete( m_vectWday[i] );
		if( m_vectIcons[i] ) delete( m_vectIcons[i] );
		if( m_vectLabel[i] ) delete( m_vectLabel[i] );
		if( m_vectTempDay[i] ) delete( m_vectTempDay[i] );
		if( m_vectTempNight[i] ) delete( m_vectTempNight[i] );

		//delete( m_vectIcons[i] );
		if( m_vectDayDetailSelect[i] ) delete( m_vectDayDetailSelect[i] );
	}
	// clean day details
	for( int i=0; i<8;  i++ )
	{
		if( m_vectDayTime[i] ) delete( m_vectDayTime[i] );
		if( m_vectDayIcons[i] ) delete( m_vectDayIcons[i] );
		if( m_vectDayLabel[i] ) delete( m_vectDayLabel[i] );
		if( m_vectDayTemperature[i] ) delete( m_vectDayTemperature[i] );
		if( m_vectDayHumidity[i] ) delete( m_vectDayHumidity[i] );
		if( m_vectDayPrecip[i] ) delete( m_vectDayPrecip[i] );
		if( m_vectDayClouds[i] ) delete( m_vectDayClouds[i] );
	}
}	

////////////////////////////////////////////////////////////////////
void CWeatherCForecastDlg::SetConfig( CObserver* pObserver, int siteid, CUnimapWorker* pUnimapWorker )
{
	int i=0;
	m_pObserver = pObserver;
	m_pUnimapWorker = pUnimapWorker;
	m_nPosType = 0;

	// populate sites
	m_pSiteSelect->Clear();
	for( i=0; i<m_pObserver->m_nSite; i++ ) m_pSiteSelect->Append( m_pObserver->m_vectSite[i]->m_strSiteName );
//	m_pSiteSelect->SetSelection(siteid); - is done in set site
	
	// init/set by engine
	SetSite( siteid );

	// set default position
	SetPosType( 0 );

	// should be here ?
//	UpdateForecast();
}

////////////////////////////////////////////////////////////////////
void CWeatherCForecastDlg::OnSelectDay( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();
	int nSiteId = m_pSiteSelect->GetSelection();

	// if day details selection
	if( nId >= wxID_DAY_DETAIL_SELECT_START && nId < wxID_DAY_DETAIL_SELECT_START+100 )
	{
		// claculate my selected day
		m_nDay = nId - wxID_DAY_DETAIL_SELECT_START;

		// check if to update
		if( m_pWeather->IsDailyForecast_ToUpdate( m_nDay ) )
		{

			// prepare action command
			DefCmdAction act;
			act.id = THREAD_ACTION_WEATHER_FORECAST_DAY_GET;
			//act.handler = this->GetEventHandler();
			//act.vectInt[0] = nSiteId;
			act.vectInt[0] = m_nDay;
			// lat lon
			act.vectFloat[0] = m_nLat;
			act.vectFloat[1] = m_nLon;
			// send command to worker
			//m_pUnimapWorker->SetAction( act );
			if( m_nPosType == 0 )
				act.vectObj[0] = (void*) m_pObserver->m_vectSite[nSiteId]->m_pWeather;
			else if( m_nPosType == 1 )
				act.vectObj[0] = (void*) m_pCustomWeather;

			/////////////////
			// show wait dialog
			if( m_pWaitDlg ) delete( m_pWaitDlg );
			m_pWaitDlg = new CWaitDialog( this, wxT("Fetch Weather Data"),
									wxT("Updating weather day forecast data..."), 0, m_pUnimapWorker, &act );
			m_pWaitDlg->ShowModal();

			delete( m_pWaitDlg );
			m_pWaitDlg = NULL;
		}

		// update the day's data
		UpdateDay( );
	}

	return;
}

////////////////////////////////////////////////////////////////////
void CWeatherCForecastDlg::UpdateForecast( int bOther )
{
	int nSiteId = m_pSiteSelect->GetSelection();
	int i=0;
	DefCmdAction act;

	if( bOther )
	{
		if( !m_pCustomWeather ) m_pCustomWeather = new CWeather();
		m_pWeather = m_pCustomWeather;

		// prepare action command
		act.id = THREAD_ACTION_WEATHER_FORECAST_ALL_GET_CUSTOM;
		act.vectObj[0] = (void*) m_pWeather;
		act.vectFloat[0] = m_nLat;
		act.vectFloat[1] = m_nLon;

	} else
	{		
		// prepare action command
		act.id = THREAD_ACTION_WEATHER_FORECAST_ALL_GET;
		act.vectInt[0] = nSiteId;
	}

	/////////////////
	// show wait dialog
	CWaitDialog waitDialog( this, wxT("Wait..."), 
					wxT("Updating weather current and forecast data..."), 0, m_pUnimapWorker, &act );
	waitDialog.ShowModal();

	// get weather 
	if( !bOther ) m_pWeather = m_pObserver->m_vectSite[nSiteId]->m_pWeather;

	/////////
	// set icons
	wxString strTmp;
	wxString strIcon;
	wxString strCond;
	wxString strDatetime;
	wxString strTemp;
	wxString strDew;
	wxString strWindchill;
	wxString strHumidity;
	wxString strWindDir;
	wxString strWind;
	wxString strPressure;
	wxString strVis;

	// icon
	strIcon.Printf( wxT("./resources/weather/%s.gif"), m_pWeather->m_rCurrent.cond_icon );
	m_pCurrentIcon->SetBitmap( wxBitmap( strIcon, wxBITMAP_TYPE_ANY ) );
	// current conditions text
	strCond.Printf( wxT("Current Conditions: %s"), m_pWeather->m_rCurrent.cond_text );
	m_pCurrentCond->SetLabel( strCond );
	// update datetime
	wxDateTime update_time( m_pWeather->m_rCurrent.datetime );
	strDatetime.Printf( wxT("Data Updated at: %s"), update_time.Format( wxT("%H:%M:%S %d/%m/%Y") ) );
	m_pCurrentDatetime->SetLabel( strDatetime );

	// temperature
	strTemp.Printf( wxT("%d °C"), (int) m_pWeather->m_rCurrent.temperature );
	m_pCurrentTemp->SetLabel( strTemp );
	// dew
	strDew.Printf( wxT("%d °C"), (int) m_pWeather->m_rCurrent.dew_point );
	m_pCurrentDew->SetLabel( strDew );
	// wind chill
	strWindchill.Printf( wxT("%d °C"), (int) m_pWeather->m_rCurrent.windchill );
	m_pCurrentWindchill->SetLabel( strWindchill );
	// humidity
	strHumidity.Printf( wxT("%d%s"), (int) m_pWeather->m_rCurrent.humidity, "%" );
	m_pCurrentHumidity->SetLabel( strHumidity );
	// wind direction
	strWindDir.Printf( wxT("%d°/%s"), (int) m_pWeather->m_rCurrent.wind_dir, m_pWeather->m_rCurrent.wind_dir_str );
	m_pCurrentWinddir->SetLabel( strWindDir );
	// wind
	strWind.Printf( wxT("%d km/h"), (int) m_pWeather->m_rCurrent.wind );
	m_pCurrentWind->SetLabel( strWind );
	// pressure
	strPressure.Printf( wxT("%d hPa"), (int) m_pWeather->m_rCurrent.pressure );
	m_pCurrentPressure->SetLabel( strPressure );
	// visibility
	strVis.Printf( wxT("%d km"), (int) m_pWeather->m_rCurrent.visibility );
	m_pCurrentVis->SetLabel( strVis );

	///////////////
	// update daily
	for( i=0; i<m_pWeather->m_nForecastRec;  i++ )
	{
		// date
		wxDateTime dta(m_pWeather->m_vectForecastRec[i].epoch);
		// todo: here fix ... fixed length or dynamic add
		if( m_vectDate[i] ) m_vectDate[i]->SetLabel( dta.Format( wxT("%d/%m/%Y") ) );
		// week day
		wxDateTime dt(m_pWeather->m_vectForecastRec[i].epoch);
		if( m_vectWday[i] ) m_vectWday[i]->SetLabel( dt.Format( wxT("%A") ) );
		// icons
		strTmp.Printf( wxT("./resources/weather/%s.gif"), m_pWeather->m_vectForecastRec[i].cond_icon_night );
		if( m_vectIcons[i] ) m_vectIcons[i]->SetBitmap( wxBitmap( strTmp, wxBITMAP_TYPE_ANY ) ); 
		// labels
		if( m_vectLabel[i] ) m_vectLabel[i]->SetLabel( m_pWeather->m_vectForecastRec[i].cond_text_night );
		// temp day
		strTmp.Printf( wxT("%d °C"), (int) m_pWeather->m_vectForecastRec[i].temperature_day );
		if( m_vectTempDay[i] ) m_vectTempDay[i]->SetLabel( strTmp);
		// temp night
		strTmp.Printf( wxT("%d °C"), (int) m_pWeather->m_vectForecastRec[i].temperature_night );
		if( m_vectTempNight[i] ) m_vectTempNight[i]->SetLabel( strTmp);

	}

	// update hourly
	UpdateDay( bOther );
}

////////////////////////////////////////////////////////////////////
void CWeatherCForecastDlg::UpdateDay( int bOther )
{
	int i=0;
//	CWeather* pWeather = m_pWeather;
//	if( bOther ) pWeather = m_pCustomWeather;

	///////////////
	// calculate layout for details
	int nDCol = 8;
	int nDRealStart = nDCol - m_pWeather->m_vectForecastRec[m_nDay].nDetail;
	wxString strIcon;
	wxString strLabel;
	wxString strTime;
	wxString strTemperature;
	wxString strHumidity;
	wxString strPrecip;
	wxString strClouds;
	
	/////////////////////////////
	// POPULATE NEW VALUES
	for( i=0; i<nDCol; i++ )
	{
		if( i < nDRealStart )
		{
			strTime.Printf( wxT("00:00") );
			strIcon.Printf( wxT("./resources/weather/unknown.gif" ) );
			strLabel.Printf( wxT("n/a" ) );
			strTemperature.Printf( wxT("n/a") );
			strHumidity.Printf( wxT("n/a") );
			strPrecip.Printf( wxT("n/a") );
			strClouds.Printf( wxT("n/a") );

		} else
		{
			strTime.Printf( wxT("%d:00"), m_pWeather->m_vectForecastRec[m_nDay].vectDetail[i-nDRealStart].time );
			strIcon.Printf( wxT("./resources/weather/%s.gif"), m_pWeather->m_vectForecastRec[m_nDay].vectDetail[i-nDRealStart].cond_icon );
			strLabel.Printf( wxT("%s"), m_pWeather->m_vectForecastRec[m_nDay].vectDetail[i-nDRealStart].cond_text );
			strTemperature.Printf( wxT("%d °C"), (int) m_pWeather->m_vectForecastRec[m_nDay].vectDetail[i-nDRealStart].temperature );
			strHumidity.Printf( wxT("%d%s"), (int) m_pWeather->m_vectForecastRec[m_nDay].vectDetail[i-nDRealStart].humidity, "%" );
			strPrecip.Printf( wxT("%d%s"), (int) m_pWeather->m_vectForecastRec[m_nDay].vectDetail[i-nDRealStart].precip, "%" );
			strClouds.Printf( wxT("%d%s"), (int) m_pWeather->m_vectForecastRec[m_nDay].vectDetail[i-nDRealStart].clouds, "%" );
		}

		// set time
		m_vectDayTime[i]->SetLabel( strTime );
		// set new bitmap
		m_vectDayIcons[i]->SetBitmap( wxBitmap( strIcon, wxBITMAP_TYPE_ANY ) );
		// set label
		m_vectDayLabel[i]->SetLabel( strLabel );
		// set temperature
		m_vectDayTemperature[i]->SetLabel( strTemperature );
		// set humidity
		m_vectDayHumidity[i]->SetLabel( strHumidity );
		// set precipitations
		m_vectDayPrecip[i]->SetLabel( strPrecip );
		// set clouds cover
		m_vectDayClouds[i]->SetLabel( strClouds );
	}
	// update layout
//		sizerDayDetails->RecalcSizes();
	sizerDayDetails->Layout();
//		Fit();
//		Layout();
//		Update();
//		Refresh( FALSE );

}

////////////////////////////////////////////////////////////////////
void CWeatherCForecastDlg::OnPosType( wxCommandEvent& pEvent )
{
	int nType = pEvent.GetId();

	if( nType == wxID_WEATHER_FORECAST_POS_TYPE_A )
		SetPosType( 0 );
	else if( nType == wxID_WEATHER_FORECAST_POS_TYPE_B )
		SetPosType( 1 );

}

////////////////////////////////////////////////////////////////////
void CWeatherCForecastDlg::SetPosType( int type )
{
	if( type == 0 )
	{
		m_nPosType = 0;

		// set current lat/lon
		m_nLat = m_pSite->m_nLatitude;
		m_nLon = m_pSite->m_nLongitude;

		// enable 
		//m_pPosTypeA->Enable();
		m_pSiteLabel->Enable();
		m_pSiteSelect->Enable();
		// disable
		//m_pPosTypeB->Disable();
		m_pLatLonLabel->Disable();
		m_pLatValue->Disable();
		m_pLonValue->Disable();
		m_pMapButton->Disable();
		m_pGeoIpButton->Disable();
		m_pGpsButton->Disable();
		m_pGetDataButton->Disable();

	} else
	{
		m_nPosType = 1;
		if( !m_pCustomWeather ) m_pCustomWeather = new CWeather();

		// set current lat/lon from text entries
		m_nLat = wxAtof( m_pLatValue->GetLineText(0) );
		m_nLon = wxAtof( m_pLonValue->GetLineText(0) );

		// enable
		//m_pPosTypeB->Enable();
		m_pLatLonLabel->Enable();
		m_pLatValue->Enable();
		m_pLonValue->Enable();
		m_pMapButton->Enable();
		m_pGeoIpButton->Enable();
		//m_pGpsButton->Enable();
		m_pGetDataButton->Enable();
		//disable
		//m_pPosTypeA->Disable();
		m_pSiteLabel->Disable();
		m_pSiteSelect->Disable();
	}
}

////////////////////////////////////////////////////////////////////
void CWeatherCForecastDlg::OnSetLatLon( wxCommandEvent& pEvent )
{
	wxString strTmp;

	// set current lat/lon from text entries
	wxString strLat = m_pLatValue->GetLineText(0);
	m_nLat = wxAtof( strLat );
	wxString strLon = m_pLonValue->GetLineText(0);
	m_nLon = wxAtof( strLon );
	// enable/disable data button
	if( strLat.IsEmpty() || strLon.IsEmpty() )
		m_pGetDataButton->Disable();
	else
		m_pGetDataButton->Enable();

	strTmp.Printf( wxT("Lat/Lon: %.2lf %s/%.2lf %s - Weather Forecast"), m_nLat, wxT("°N"), m_nLon, wxT("°E") );
	SetTitle( strTmp );
}

////////////////////////////////////////////////////////////////////
void CWeatherCForecastDlg::OnChangeSite( wxCommandEvent& pEvent )
{
	int nId = m_pSiteSelect->GetSelection();

	SetSite( nId );
	UpdateForecast();
}

////////////////////////////////////////////////////////////////////
void CWeatherCForecastDlg::SetSite( int site )
{
	wxString strTmp;

	m_pSiteSelect->SetSelection( site );

	m_nCurrentSite = site;
	m_pSite = m_pObserver->m_vectSite[m_nCurrentSite];
	m_pWeather = m_pSite->m_pWeather;

	// set title
	strTmp.Printf( wxT("%s/%s - Lat/Lon: %.2lf %s/%.2lf %s - Weather Forecast"),
			m_pSite->m_strCity, m_pSite->m_strCountry, m_pSite->m_nLatitude, wxT("°N"), m_pSite->m_nLongitude, wxT("°E") );
	SetTitle( strTmp );
	// set custom fields
	strTmp.Printf( wxT("%.4lf"), m_pSite->m_nLatitude );
	m_pLatValue->ChangeValue( strTmp );
	strTmp.Printf( wxT("%.4lf"), m_pSite->m_nLongitude );
	m_pLonValue->ChangeValue( strTmp );

	// set local lat/lon
	m_nLat = m_pSite->m_nLatitude;
	m_nLon = m_pSite->m_nLongitude;

	// init/set by engine
//	SetSource( -1 );
}

////////////////////////////////////////////////////////////////////
void CWeatherCForecastDlg::OnGetCustomData( wxCommandEvent& pEvent )
{
	// set current lat/lon from text entries
	m_nLat = wxAtof( m_pLatValue->GetLineText(0) );
	m_nLon = wxAtof( m_pLonValue->GetLineText(0) );

	// update
	UpdateForecast(1);
}

////////////////////////////////////////////////////////////////////
void CWeatherCForecastDlg::OnEartMapLocation( wxCommandEvent& pEvent )
{
	wxString strTmp;

	CLocationEarth* pEarth = new CLocationEarth( NULL );
//	pEarth->LoadAllCountries( );

	CEarthmMapDlg* pMapDlg = new CEarthmMapDlg( this, _("Earth Map - Set Your Location") );
	pMapDlg->SetEarth( pEarth );
	pMapDlg->LoadData( this, m_pUnimapWorker );
	//pMapDlg->SetConfig( NULL, nCountryId, nRegionId, nCityId );
	// set address location
	pMapDlg->SetLocByName( m_pSite->m_strCountry, m_pSite->m_strProvince, m_pSite->m_strCity, 1 );

	// set over manually - latitude/longitude and altitude
	wxString strAlt = wxT("0.0");
	wxString strLat = m_pLatValue->GetLineText(0);
	wxString strLon = m_pLonValue->GetLineText(0);
	pMapDlg->SetCustomPositionFromString( strLat, strLon, strAlt );

	if( pMapDlg->ShowModal() == wxID_OK )
	{
		// also get/set latitude/longitude and altitude
		wxString strLat = pMapDlg->m_pLatitude->GetLineText(0);
		m_pLatValue->SetValue( strLat );
		wxString strLon = pMapDlg->m_pLongitude->GetLineText(0);
		m_pLonValue->SetValue( strLon );

		// set current lat/lon
		m_nLat = wxAtof( strLat );
		m_nLon = wxAtof( strLon );

		// set title
		strTmp.Printf( wxT("%s/%s - Lat/Lon: %.2lf %s/%.2lf %s - Seeing Prediction"),
			pMapDlg->m_pCity->GetStringSelection(), pMapDlg->m_pCountry->GetStringSelection(), m_nLat, wxT("°N"), m_nLon, wxT("°E") );
		SetTitle( strTmp );

	}
	delete( pMapDlg );
	delete( pEarth );
}

////////////////////////////////////////////////////////////////////
void CWeatherCForecastDlg::OnGeoIpLocation( wxCommandEvent& pEvent )
{
	wxString strCountry;
	wxString strCity;
	double nLat, nLon;
	wxString strTmp;

	CGeoIp ip_locator;
	ip_locator.GetOwnIp( strCountry, strCity, nLat, nLon );

	// :: set latitude :: is NORTH to SOUTH 90 to -90 
	strTmp.Printf( wxT("%.4f"), nLat );
	m_pLatValue->SetValue( strTmp );

	// :: set longitude :: is WEST - EAST ... -180 to 180
	strTmp.Printf( wxT("%.4f"), nLon );
	m_pLonValue->SetValue( strTmp );

	// set current lat/lon
	m_nLat = nLat;
	m_nLon = nLon;

	// set title
	strTmp.Printf( wxT("%s/%s - Lat/Lon: %.2lf %s/%.2lf %s - Seeing Prediction"), 
								strCity, strCountry, m_nLat, wxT("°N"), m_nLon, wxT("°E") );
	SetTitle( strTmp );

}
