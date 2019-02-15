////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _WEATHER_CURRENT_FORECAST_DLG_H
#define _WEATHER_CURRENT_FORECAST_DLG_H

// wx includes
#include "wx/wxprec.h"

// gui ids
enum
{
	wxID_WEATHER_FORECAST_POS_TYPE_A = 28555,
	wxID_WEATHER_FORECAST_SITE_SELECT,
	wxID_WEATHER_FORECAST_POS_TYPE_B,
	wxID_WEATHER_FORECAST_SET_LAT,
	wxID_WEATHER_FORECAST_SET_LON,
	wxID_WEATHER_FORECAST_BUTTON_MAP,
	wxID_WEATHER_FORECAST_BUTTON_GEOIP,
	wxID_WEATHER_FORECAST_BUTTON_GPS,
	wxID_WEATHER_FORECAST_BUTTON_GET_DATA
};
// radio select ranges
#define wxID_DAY_DETAIL_SELECT_START	8800

// external classes
class CObserverSite;
class CWeather;
class CUnimapWorker;
class CWaitDialog;

// class:	CWeatherCForecastDlg
///////////////////////////////////////////////////////
class CWeatherCForecastDlg : public wxDialog
{
// methods
public:
    CWeatherCForecastDlg( wxWindow *parent, const wxString& title, CObserverSite* pSite, CUnimapWorker* pUnimapWorker );
	~CWeatherCForecastDlg( );

	void SetConfig( CObserver* pObserver, int siteid, CUnimapWorker* pUnimapWorker );

	void UpdateForecast( int bOther=0 );

/////////////
// ::DATA
public:
	CObserverSite*	m_pSite;
	CWeather*		m_pWeather;
	CWeather*		m_pCustomWeather;
	CUnimapWorker*	m_pUnimapWorker;
	CObserver*		m_pObserver;

	int m_nDay;

	CWaitDialog*	m_pWaitDlg;

	// gui elem
//	wxStaticBitmap* m_vectIcons[10];
	// day details select 
	wxRadioButton* m_vectDayDetailSelect[10];

	//////////
	// day incons
	wxFlexGridSizer* sizerDayDetails;

	// base forecast records
	wxStaticBitmap* m_pCurrentIcon;
	wxStaticText* m_pCurrentCond;
	wxStaticText* m_pCurrentDatetime;
	wxStaticText* m_pCurrentTemp;
	wxStaticText* m_pCurrentDew;
	wxStaticText* m_pCurrentWindchill;
	wxStaticText* m_pCurrentHumidity;
	wxStaticText* m_pCurrentWinddir;
	wxStaticText* m_pCurrentWind;
	wxStaticText* m_pCurrentPressure;
	wxStaticText* m_pCurrentVis;

	// daily forecast records
	wxStaticText*	m_vectDate[10];
	wxStaticText*	m_vectWday[10];
	wxStaticBitmap*	m_vectIcons[10];
	wxStaticText*	m_vectLabel[10];
	wxStaticText*	m_vectTempDay[10];
	wxStaticText*	m_vectTempNight[10];

	// hourly forecast records
	wxStaticText*	m_vectDayTime[8];
	wxStaticBitmap*	m_vectDayIcons[8];
	wxStaticText*	m_vectDayLabel[8];
	wxStaticText*	m_vectDayTemperature[8];
	wxStaticText*	m_vectDayHumidity[8];
	wxStaticText*	m_vectDayPrecip[8];
	wxStaticText*	m_vectDayClouds[8];

	////////
	// top chooser buttons
	wxRadioButton*	m_pPosTypeA;
	wxRadioButton*	m_pPosTypeB;
	wxStaticText*	m_pSiteLabel;
	wxChoice*		m_pSiteSelect;
	wxStaticText*	m_pLatLonLabel;
	wxTextCtrl*		m_pLatValue;
	wxTextCtrl*		m_pLonValue;
	wxButton*		m_pMapButton;
	wxButton*		m_pGeoIpButton;
	wxButton*		m_pGpsButton;
	wxButton*		m_pGetDataButton;

	int m_nPosType;
	int m_nCurrentSite;
	// local coord
	double m_nLat;
	double m_nLon;

private:
	void OnSelectDay( wxCommandEvent& pEvent );
	void UpdateDay( int bOther=0 );

	// void
	void OnPosType( wxCommandEvent& pEvent );
	void SetPosType( int type );
	void OnSetLatLon( wxCommandEvent& pEvent );
	void OnChangeSite( wxCommandEvent& pEvent );
	void SetSite( int site );
	void OnGetCustomData( wxCommandEvent& pEvent );
	void OnEartMapLocation( wxCommandEvent& pEvent );
	void OnGeoIpLocation( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif
