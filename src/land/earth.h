////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _LOCATION_EARTH_H
#define _LOCATION_EARTH_H

// system
#include <stdio.h>
#include <stdlib.h>

// wx
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// defines
#define EARTH_LIGTH_POLLUTION_MAP_FILE unMakeAppPath(wxT("data/earth/light_pollution/earth_lp_map.gif"))

// :: city
typedef struct
{
	//char* name;
	//wxChar name[50];
	char name[50];
	double lat;
	double lon;
	double alt;

} locCity;

// :: region
typedef struct
{
	//char* name;
	wxChar name[50];
	locCity* vectCity;
	int nCity;

} locRegion;

// :: country
typedef struct
{
	locRegion* vectRegion;
	int nRegion;
	char code[3];
//	char* name;
	wxChar name[50];

} locCountry;

// external classes
class CUnimapWorker;

class CLocationEarth
{
public:
	CLocationEarth( CUnimapWorker* pUnimapWorker );
	~CLocationEarth( );

	void FreeData( );

	// load countries definition
	int LoadCountries( );
	// load country 
	int LoadCountry( int nId );
	// load all countries
	int LoadAllCountries( );

	int FindCountryId( const char* strCode );
	int FindClosestCity( double lat, double lon, double max, int& country, int& region, int& city );
	int FindCountryByName( const wxString& strName );
	int FindRegionByName( int nCountryId, const wxString& strName );
	int FindCityByName( int nCountryId, int nRegionId, const wxString& strName );
	int FindRegionCityByName( const wxString& strName, int nCountryId, int& nRegionId, int& nCityId );
	// ligh pollution handler
	double GetLightPollutionLevel( );
	double GetLPLevelFromImage( wxImage* pImg, double lat, double lon );
	void GetLPLevelAsString( double level, wxString& strLevel );
	// set current location
	void SetCurrentLatLon( double lat, double lon ){ m_nCurrentLat=lat; m_nCurrentLon=lon; };

	// gui calls
	void SetCountriesWidget( wxChoice* pWidget, int nSelect=0 );
	void SetRegionsWidget( wxChoice* pWidget, int nCountryId, int nSelect=0 );
	void SetCitiesWidget( wxChoice* pWidget, int nCountryId, int nRegionId, int nSelect=0 );

// data
public:

	CUnimapWorker* m_pUnimapWorker;

	locCountry m_vectCountry[250];
	int m_nCountry;
	// current coords
	double m_nCurrentLat;
	double m_nCurrentLon;
	// lp level
	double m_nCurrentLPLevel;

};

#endif
