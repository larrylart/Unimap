////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _WEATHER_UNDER_H
#define _WEATHER_UNDER_H

// wx includes
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif 
//precompiled headers
#include "wx/wxprec.h"
// other wx
#include <wx/string.h>
#include <wx/xml/xml.h>

// local headers
#include "../util/func.h"
#include "../util/webfunc.h"
#include "weather.h"

// class:	CWeatherUnder
///////////////////////////////////////////////////////
class CWeatherUnder
{
public:
	CWeatherUnder( ){};
	~CWeatherUnder( ){};

	// get station id(string) based on lat/lon
	int Get_StationId( double lat, double lon, wxString& strStationId );
	// get current xml
	int Get_Current( DefWeatherRecord& rec, double lat, double lon );
	// get current and forecast based on the mobile lite html pharser
	int Get_CurrentAndForecastHtml( DefWeatherRecord& rec, DefWeatherForecastRecord* vectRec, int& nRec, double lat, double lon );
	// get forcast xml
	int Get_Forecast( DefWeatherForecastRecord* vectRec, int& nRec, double lat, double lon );
	// get historical - combine get station and get hist cvs file
	int Get_Historical( DefWeatherRecord* vectRec, int& nRec, double lat, double lon, double time );

	//////////////////////
	// get daily forecast from html lite
	int Get_Forecast_Overview_viaHtml( CWeather* pWeather, double lat, double lon );
	int Get_Forecast_viaHtml( CWeather* pWeather, double lat, double lon );
	int Get_Forecast_Daily_LiteHtml( int nDay, DefWeatherForecastRecord* vectForecast, int firstRec, double lat, double lon );

};


#endif

