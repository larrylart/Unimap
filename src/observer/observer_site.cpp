////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// system libs
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// wx
#include "wx/wxprec.h"
#include <wx/datetime.h>

// local headers
#include "../weather/weather.h"

// main header
#include "observer_site.h"

////////////////////////////////////////////////////////////////////
CObserverSite::CObserverSite( )
{
	m_pWeather = NULL;

	ResetSite( );
}

////////////////////////////////////////////////////////////////////
void CObserverSite::ResetSite( )
{
	m_strSiteName = wxT( "" );
	m_strCity = wxT( "" );
	m_strProvince = wxT( "" );
	m_strCountry = wxT( "" );
	m_strSiteNotes = wxT( "" );
	m_nLatitude = 0.0;
	m_nLongitude = 0.0;
	m_nAltitude = 0.0;

	return;
}

////////////////////////////////////////////////////////////////////
CObserverSite::~CObserverSite( ) 
{
	if( m_pWeather ) delete( m_pWeather );
}

////////////////////////////////////////////////////////////////////
int CObserverSite::GetCurrentWeather( ) 
{
	if( !m_pWeather ) m_pWeather = new CWeather( );

	wxDateTime timeObs;
	timeObs.SetToCurrent();

	m_pWeather->GetCurrentWeatherData( m_nLatitude, m_nLongitude, 0 );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CObserverSite::GetCurrentWeatherWithHist( ) 
{
	if( !m_pWeather ) m_pWeather = new CWeather( );

	wxDateTime timeObs;
	timeObs.SetToCurrent();

	m_pWeather->GetWeatherData_AtDateTime( m_nLatitude, m_nLongitude, timeObs.GetJDN(), 0 );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CObserverSite::GetWeatherForecast( ) 
{
	if( !m_pWeather ) m_pWeather = new CWeather( );

	m_pWeather->GetWeatherData_Forecast( m_nLatitude, m_nLongitude, 0 );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CObserverSite::IsForecast_ToUpdate( ) 
{
	if( !m_pWeather ) m_pWeather = new CWeather( );

	return( m_pWeather->IsForecast_ToUpdate() );
}

////////////////////////////////////////////////////////////////////
int CObserverSite::GetSeeingPrediction( int engine ) 
{
	if( !m_pWeather ) m_pWeather = new CWeather( );

	m_pWeather->GetSeeingPrediction( m_nLatitude, m_nLongitude, engine );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CObserverSite::GetTemperature( double* vectData, int& nData, double& now )
{
	int i = 0;

	for( i=0; i<m_pWeather->m_nPastRec; i++ )
	{
		vectData[i] = m_pWeather->m_vectPastRec[i].temperature;
		if( i >= nData ) break;
	}
	// check if less data then requested
	if( i < nData ) nData = i;
	// get now 
	now = m_pWeather->m_rCurrent.temperature;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CObserverSite::GetHumidity( double* vectData, int& nData, double& now )
{
	int i = 0;

	for( i=0; i<m_pWeather->m_nPastRec; i++ )
	{
		vectData[i] = m_pWeather->m_vectPastRec[i].humidity;
		if( i >= nData ) break;
	}
	// check if less data then requested
	if( i < nData ) nData = i;
	// get now 
	now = m_pWeather->m_rCurrent.humidity;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CObserverSite::GetWind( double* vectData, int& nData, double& now )
{
	int i = 0;

	for( i=0; i<m_pWeather->m_nPastRec; i++ )
	{
		vectData[i] = m_pWeather->m_vectPastRec[i].wind;
		if( i >= nData ) break;
	}
	// check if less data then requested
	if( i < nData ) nData = i;
	// get now 
	now = m_pWeather->m_rCurrent.wind;

	return( 1 );
}
