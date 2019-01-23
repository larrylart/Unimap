
// system libs
#include <stdio.h>
//#include <malloc.h>
#include <stdlib.h>
#include <math.h>

//  wx
#include "wx/wxprec.h"
#include "wx/thread.h"
//precompiled headers
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif 
#include <wx/regex.h>
#include <wx/string.h>

// local headers
#include "weather_under.h"
#include "meteo_blue.h"
#include "clear_sky_chart.h"

// main header
#include "weather.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CWeather
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CWeather::CWeather( )
{
	int i=0;

	m_nPastRec = 0;
	m_nForecastRec = 0;
	m_nForecastRec = 0;
	m_nFirstForecastRec = 0;
	memset( &m_rCurrent, 0, sizeof(DefWeatherRecord) );
	m_nSeeingPredict = 0;
	m_nForecastFetchTime = 0;
	m_nFetchError = 0;
	m_strFetchError = wxT("");

	for( i=0; i<100; i++ )
		memset( &(m_vectPastRec[i]), 0, sizeof(DefWeatherRecord) );

	for( i=0; i<10; i++ )
		memset( &(m_vectForecastRec[i]), 0, sizeof(DefWeatherForecastRecord) );

	for( i=0; i<10; i++ )
		memset( &(m_vectSeeingPredict[i]), 0, sizeof(DefSeeingPredictDayRec) );
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CWeather
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CWeather::~CWeather( )
{

}

/////////////////////////////////////////////////////////////////////
int CWeather::GetCurrentWeatherData( double lat, double lon, int nSrcId )
{
//	char strStationId[255];
	int nState = 0;

	if( nSrcId == 0 )
	{
		CWeatherUnder src;
//		src.Get_StationId( lat, lon, strStationId );
		nState = src.Get_Current( m_rCurrent, lat, lon );
	}

	//////////////
	// set error
	if( !nState )
	{
		m_nFetchError = 1;
		m_strFetchError = wxT("Failed to retreive Data");
	}

	return( nState );
}

/////////////////////////////////////////////////////////////////////
int CWeather::GetWeatherData_AtDateTime( double lat, double lon, double obs_time, int nSrcId )
{
//	char strStationId[255];
	int nFound=-1, i=0;
	double nDiff=0.0, nMinDiff=DBL_MAX;

	if( nSrcId == 0 )
	{
		CWeatherUnder src;
		if( !src.Get_Historical( m_vectPastRec, m_nPastRec, lat, lon, obs_time ) ) 
		{
			m_nFetchError = 1;
			m_strFetchError = wxT("Failed to retreive Data");
			return( 0 );
		}
		// now get closest to time
		for( i=0; i<m_nPastRec; i++ )
		{
			nDiff = fabs(m_vectPastRec[i].datetime-obs_time);
			if( nDiff < nMinDiff )
			{
				nMinDiff = nDiff;
				nFound = i;
			}
		}

		if( nFound == -1 || (m_vectPastRec[nFound].temperature == 0.0 && m_vectPastRec[nFound].humidity == 0.0) )
		{
			m_nFetchError = 1;
			m_strFetchError = wxT("Failed to retreive Data");
			return( 0 );
		}

		// set current to best found
		m_rCurrent.datetime = m_vectPastRec[nFound].datetime;
		m_rCurrent.pressure = m_vectPastRec[nFound].pressure;
		m_rCurrent.wind = m_vectPastRec[nFound].wind;
		m_rCurrent.humidity = m_vectPastRec[nFound].humidity;
		m_rCurrent.temperature = m_vectPastRec[nFound].temperature;
		// calculate temp variation - mean - todo:: here check prev/next day for closest
		if( m_nPastRec == 1 )
			m_rCurrent.temperature_var = 0.0;
		else if( nFound == 0 )
			m_rCurrent.temperature_var = m_vectPastRec[nFound+1].temperature-m_vectPastRec[nFound].temperature;
		else if( nFound == m_nPastRec-1 )
			m_rCurrent.temperature_var = m_vectPastRec[nFound].temperature-m_vectPastRec[nFound-1].temperature;
		else 
		{
			double nDiffPrev = fabs(m_vectPastRec[nFound].datetime-m_vectPastRec[nFound-1].datetime);
			double nDiffNext = fabs(m_vectPastRec[nFound+1].datetime-m_vectPastRec[nFound].datetime);
			// check which closest
			if( nDiffPrev < nDiffNext )
				m_rCurrent.temperature_var = m_vectPastRec[nFound].temperature-m_vectPastRec[nFound-1].temperature;
			else
				m_rCurrent.temperature_var = m_vectPastRec[nFound+1].temperature-m_vectPastRec[nFound].temperature;
		}
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
int CWeather::GetWeatherData_Forecast( double lat, double lon, int nSrcId )
{
	if( nSrcId == 0 )
	{
		CWeatherUnder src;

		//if( !src.Get_Forecast( m_vectForecastRec, m_nForecastRec, lat, lon ) ) return( 0 );
		//if( !src.Get_CurrentAndForecastHtml( m_rCurrent, m_vectForecastRec, m_nForecastRec, lat, lon ) ) return( 0 );

		if( !src.Get_Forecast_viaHtml( this, lat, lon ) ) return( 0 );

	} else
		return( 0 );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
int CWeather::GetWeatherData_DailyForecast( int day, double lat, double lon, int nSrcId )
{
	if( nSrcId == 0 )
	{
		CWeatherUnder src;
		int nDayRec=7;

		if( !src.Get_Forecast_Daily_LiteHtml( day, m_vectForecastRec, m_nFirstForecastRec, lat, lon ) ) return( 0 );
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
int CWeather::IsDailyForecast_ToUpdate( int day )
{
	time_t tnow = time(NULL);
	long nDiff = tnow - m_vectForecastRec[day].fetch_time;
	// verify the difference - shoule over half and hour
	if( nDiff <= 1800 ) 
		return( 0 );
	else
		return( 1 );
}

/////////////////////////////////////////////////////////////////////
int CWeather::IsForecast_ToUpdate( )
{
	time_t tnow = time(NULL);
	long nDiff = tnow - m_nForecastFetchTime;
	// verify the difference - shoule over half and hour
	if( nDiff <= 1800 ) 
		return( 0 );
	else
		return( 1 );
}

/////////////////////////////////////////////////////////////////////
int CWeather::GetSeeingPrediction( double lat, double lon, int nSrcId )
{
	// to remove latter
//	return( 0 );

	if( nSrcId == 0 )
	{
		CMeteoBlue src;
		src.GetPredictionData( this, lat, lon );

	} else if( nSrcId == 1 )
	{
		CClearSkyChart src;
		src.GetPredictionData( this, lat, lon );
		// get image file
		m_strSeeingPredictImg = wxString(src.m_strImgFile,wxConvUTF8);
	}

	// set seeing fetch time
	m_nSeeingFetchTime = time( NULL );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
int CWeather::IsSeeing_ToUpdate( double lat, double lon, int type )
{
	// :: if meteoblue
	if( type == 0 )
	{
		CMeteoBlue src;
		return( src.IsToUpdate( lat, lon ) );

	// :: if clear sky charts
	} else if( type == 1 )
	{
		CClearSkyChart src;
		return( src.IsToUpdate( lat, lon ) );

	} else
		return( 0 );

/*
	time_t tnow = time(NULL);
	long nDiff = tnow - m_nSeeingFetchTime;
	// verify the difference - shoule over half a day
	if( nDiff <= 43200 ) 
		return( 0 );
	else
		return( 1 );
*/
}

/////////////////////////////////////////////////////////////////////
int CWeather::GetBestSeeingPredictEngine( double lat, double lon )
{
	// top-left: N78.753,  W175.039 = 
	// bottom-right: N17.778,  W77.077

	// if in clear sky window (north america)
	if( ( lat >= 17.0 && lat <= 50.0 ) && ( lon >= -176.0 && lon <= -78.0 ) )
	{
		// todo: here I should also check the distance to generated map

		// return clearsky engine
		return( 1 );

	} else
	{
		// default return meteoblue engine
		return( 0 );
	}
}
