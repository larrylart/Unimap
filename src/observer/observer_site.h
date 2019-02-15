////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _OBSERVER_SITE_H
#define _OBSERVER_SITE_H


// external classes
class CObserver;
class CObserverHardware;
class CWeather;

// class:	CObserverSite
///////////////////////////////////////////////////////
class CObserverSite
{
// public methods
public:
	CObserverSite( );
	~CObserverSite( );

	void ResetSite( );
	int GetCurrentWeather( );
	int GetCurrentWeatherWithHist( );
	int GetWeatherForecast( );
	int IsForecast_ToUpdate( );
	int GetSeeingPrediction( int engine );

	// individual functions
	int GetTemperature( double* vectData, int& nData, double& now );
	int GetHumidity( double* vectData, int& nData, double& now );
	int GetWind( double* vectData, int& nData, double& now );

// public data
public:
	CWeather*	m_pWeather;

	wxString	m_strSiteName;

	wxString	m_strCity;
	wxString	m_strProvince;
	wxString	m_strCountry;

	// :: latitude/longitude - most important
	double	m_nLatitude;
	double	m_nLongitude;
	double	m_nAltitude;

	wxString	m_strSiteNotes;
};

#endif