////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _METEO_BLUE_SITE_H
#define _METEO_BLUE_SITE_H

// wx includes
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif 
//precompiled headers
#include "wx/wxprec.h"
// other wx
#include <wx/string.h>
// curl
//#include <curl/curl.h>
//#include "../httpengine/wxcurl_http.h"

// local headers
#include "weather.h"

// external classes
class CUnimapOnline;
class CUnimapWorker;

// class:	CMeteoBlue
///////////////////////////////////////////////////////
class CMeteoBlue
{
// methods
public:
	CMeteoBlue( );
	~CMeteoBlue( );

	// save data
	int SaveData( double lat, double lon );
	int LoadData( double lat, double lon );

	// login into the meteo blue site
	int Login( const char* strUser, const char* strPassword );
	// extend current account activation period .. +90 days thingie
	int ExtendAccount( );
	// register a new account
	int RegisterAccount( );

	// get/make astro-weather prediction data for lat/lon
	int GetPredictionData( CWeather* pWeather, double lat, double lon );

	int ProcessRow( int nRec, int nId, wxString& strText );

	// method to check if there is an update
	int IsToUpdate( double lat, double lon );

// data
public:
	CUnimapOnline* m_pUnimapOnline;
	CUnimapWorker* m_pUnimapWorker;

	wxString m_strUrl;
	//wxCurlHTTP* m_pHttp;

	CWeather* m_pWeather;
	
	wxString m_strAuthInput;

	// flags
	int m_bIsLogin;

};

#endif

