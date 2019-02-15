////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _WEATHER_SEEING_PREDICTION_DLG_H
#define _WEATHER_SEEING_PREDICTION_DLG_H

// wx includes
#include "wx/wxprec.h"

// gui ids
enum
{
	wxID_WEATHER_SEEING_SITE_SELECT = 18400,
	wxID_WEATHER_SEEING_SOURCE_SELECT,
	wxID_WEATHER_SEEING_PREV_DATE,
	wxID_WEATHER_SEEING_NEXT_DATE,
	wxID_WEATHER_SEEING_DATE_SELECT,
	wxID_WEATHER_SEEING_POS_TYPE_A,
	wxID_WEATHER_SEEING_POS_TYPE_B,
	wxID_WEATHER_SEEING_SET_LAT,
	wxID_WEATHER_SEEING_SET_LON,
	// buttons
	wxID_WEATHER_SEEING_BUTTON_MAP,
	wxID_WEATHER_SEEING_BUTTON_GEOIP,
	wxID_WEATHER_SEEING_BUTTON_GPS,
	wxID_WEATHER_SEEING_BUTTON_GET_DATA
};

// external classes
class CObserverSite;
class CWeather;
class CUnimapWorker;
class CWaitDialog;
class CGridDataView;

// class:	CWeatherSeeingDlg
///////////////////////////////////////////////////////
class CWeatherSeeingDlg : public wxDialog
{
// methods
public:
    CWeatherSeeingDlg( wxWindow *parent, const wxString& title );
	~CWeatherSeeingDlg( );

	void SetConfig( CObserver* pObserver, int siteid, int engine, CUnimapWorker* pUnimapWorker );

	void InitMeteoBlueDataTable( );
	void SetSelectDateRange( );
	void SetPredictionData( int nDayId );

	void SetSite( int site );
	void SetSource( int source, int type=0 );

/////////////
// ::DATA
public:
	CObserver*		m_pObserver;
	CObserverSite*	m_pSite;
	CWeather*		m_pWeather;
	CWeather*		m_pCustomWeather;
	CUnimapWorker*	m_pUnimapWorker;

	int m_nPosType;
	int m_nCurrentEngine;
	int m_nCurrentSite;

	CWaitDialog*	m_pWaitDlg;

	// local coord
	double m_nLat;
	double m_nLon;

	// GUI ELEM
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

	wxChoice*		m_pSourceSelect;
	wxButton*		m_pPrevDateButton;
	wxChoice*		m_pDateSelect;
	wxButton*		m_pNextDateButton;
	CGridDataView*	m_pDataView;
	wxStaticText*	m_pSourceRights;

private:
	void OnPosType( wxCommandEvent& pEvent );
	void SetPosType( int type );
	void OnChangeSite( wxCommandEvent& pEvent );
	void OnSetLatLon( wxCommandEvent& pEvent );
	void OnGetCustomData( wxCommandEvent& pEvent );
	void OnEartMapLocation( wxCommandEvent& pEvent );
	void OnGeoIpLocation( wxCommandEvent& pEvent );
	void OnChangeSource( wxCommandEvent& pEvent );
	void OnChangePredDate( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif

