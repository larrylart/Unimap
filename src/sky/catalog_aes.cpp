////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// system libs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// math libs
#include <vector>
#include <list>
#include <limits>
#include <algorithm>
#include <functional>
#include <numeric>

// local headers
#include "../util/func.h"
#include "sky.h"
#include "../unimap.h"
#include "../unimap_worker.h"
#include "../config/mainconfig.h"
#include "unimapsite.h"

#include "solar/afuncs.h"
//#include "solar_orbits.h"
// astro lib from lib ephem
//#include "libastro/astro.h"
#include "libastro/libastro.h"
#include "libastro/preferences.h"
// sat code
#include "sat_code/norad.h"
#include "sat_code/observe.h"
#include "sat_code/deep.h"
#include "sat_code/sdp4.h"
// catalogs
#include "catalog_aes_celestrak.h"

// main header
#include "catalog_aes.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogAes
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogAes::CSkyCatalogAes( CSky* pSky ) : CSkyCatalog( pSky )
{
	m_pSky = pSky;
	m_pUnimapWorker = NULL;

	// dso vector
	m_vectData = NULL;
	m_nData = 0;
	m_nAllocated = 0;

	m_nObjTleSearchEpochStart = 0;
	m_nObjTleSearchEpochEnd = 0;

	// reset last region loaded to zero
	m_bLastLoadRegion = 0;
	m_nLastRegionLoadedCenterRa = 0;
	m_nLastRegionLoadedCenterDec = 0;
	m_nLastRegionLoadedWidth = 0;
	m_nLastRegionLoadedHeight = 0;

	m_strName = wxT("");

	////////////////////
	// SITE PARTS/SATTELITES TYPES - inspireed from celestrak
	// Amateur Radio
	m_vectSatType[0].tle_file = wxT("amateur");
	m_vectSatType[0].desc = wxT("Amateur Radio");
	m_vectSatType[0].sat_type = EARTH_SAT_TYPE_AMATEUR_RADIO;
	m_vectSatType[0].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// CubeSats 
	m_vectSatType[1].tle_file = wxT("cubesat");
	m_vectSatType[1].desc = wxT("CubeSats");
	m_vectSatType[1].sat_type = EARTH_SAT_TYPE_CUBESATS;
	m_vectSatType[1].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Disaster Monitoring 
	m_vectSatType[2].tle_file = wxT("dmc");
	m_vectSatType[2].desc = wxT("Disaster Monitoring");
	m_vectSatType[2].sat_type = EARTH_SAT_TYPE_DISASTER_MONITORING;
	m_vectSatType[2].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Education 
	m_vectSatType[3].tle_file = wxT("education");
	m_vectSatType[3].desc = wxT("Education");
	m_vectSatType[3].sat_type = EARTH_SAT_TYPE_EDUCATION;
	m_vectSatType[3].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Engineering 
	m_vectSatType[4].tle_file = wxT("engineering");
	m_vectSatType[4].desc = wxT("Engineering");
	m_vectSatType[4].sat_type = EARTH_SAT_TYPE_ENGINEERING;
	m_vectSatType[4].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Galileo 
	m_vectSatType[5].tle_file = wxT("galileo");
	m_vectSatType[5].desc = wxT("Galileo");
	m_vectSatType[5].sat_type = EARTH_SAT_TYPE_GALILEO;
	m_vectSatType[5].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Geostationary 
	m_vectSatType[6].tle_file = wxT("geo");
	m_vectSatType[6].desc = wxT("Geostationary");
	m_vectSatType[6].sat_type = EARTH_SAT_TYPE_GEOSTATIONARY;
	m_vectSatType[6].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Geodetic 
	m_vectSatType[7].tle_file = wxT("geodetic");
	m_vectSatType[7].desc = wxT("Geodetic");
	m_vectSatType[7].sat_type = EARTH_SAT_TYPE_GEODETIC;
	m_vectSatType[7].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Glonass Operational 
	m_vectSatType[8].tle_file = wxT("glo-ops");
	m_vectSatType[8].desc = wxT("Glonass Operational");
	m_vectSatType[8].sat_type = EARTH_SAT_TYPE_GLO_OPS;
	m_vectSatType[8].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Globalstar 
	m_vectSatType[9].tle_file = wxT("globalstar");
	m_vectSatType[9].desc = wxT("Globalstar");
	m_vectSatType[9].sat_type = EARTH_SAT_TYPE_GLOBALSTAR;
	m_vectSatType[9].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// GOES 
	m_vectSatType[10].tle_file = wxT("goes");
	m_vectSatType[10].desc = wxT("GOES");
	m_vectSatType[10].sat_type = EARTH_SAT_TYPE_GOES;
	m_vectSatType[10].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Gorizont 
	m_vectSatType[11].tle_file = wxT("gorizont");
	m_vectSatType[11].desc = wxT("Gorizont");
	m_vectSatType[11].sat_type = EARTH_SAT_TYPE_GORIZONT;
	m_vectSatType[11].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// GPS Operational
	m_vectSatType[12].tle_file = wxT("gps-ops");
	m_vectSatType[12].desc = wxT("GPS Operational");
	m_vectSatType[12].sat_type = EARTH_SAT_TYPE_GPS_OPS;
	m_vectSatType[12].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Intelsat 
	m_vectSatType[13].tle_file = wxT("intelsat");
	m_vectSatType[13].desc = wxT("Intelsat");
	m_vectSatType[13].sat_type = EARTH_SAT_TYPE_INTELSAT;
	m_vectSatType[13].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Iridium 
	m_vectSatType[14].tle_file = wxT("iridium");
	m_vectSatType[14].desc = wxT("Iridium");
	m_vectSatType[14].sat_type = EARTH_SAT_TYPE_IRIDIUM;
	m_vectSatType[14].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Miscellaneous Military 
	m_vectSatType[15].tle_file = wxT("military");
	m_vectSatType[15].desc = wxT("Miscellaneous Military");
	m_vectSatType[15].sat_type = EARTH_SAT_TYPE_MISC_MILITARY;
	m_vectSatType[15].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Molniya 
	m_vectSatType[16].tle_file = wxT("molniya");
	m_vectSatType[16].desc = wxT("Molniya");
	m_vectSatType[16].sat_type = EARTH_SAT_TYPE_MOLNIYA;
	m_vectSatType[16].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Russian LEO Navigation 
	m_vectSatType[17].tle_file = wxT("musson");
	m_vectSatType[17].desc = wxT("Russian LEO Navigation");
	m_vectSatType[17].sat_type = EARTH_SAT_TYPE_MUSSON;
	m_vectSatType[17].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Navy Navigation Satellite System 
	m_vectSatType[18].tle_file = wxT("nnss");
	m_vectSatType[18].desc = wxT("Navy Navigation Satellite System ");
	m_vectSatType[18].sat_type = EARTH_SAT_TYPE_NNSS;
	m_vectSatType[18].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// NOAA 
	m_vectSatType[19].tle_file = wxT("noaa");
	m_vectSatType[19].desc = wxT("NOAA");
	m_vectSatType[19].sat_type = EARTH_SAT_TYPE_NOAA;
	m_vectSatType[19].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Orbcomm 
	m_vectSatType[20].tle_file = wxT("orbcomm");
	m_vectSatType[20].desc = wxT("Orbcomm");
	m_vectSatType[20].sat_type = EARTH_SAT_TYPE_ORBCOMM;
	m_vectSatType[20].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Other 
	m_vectSatType[21].tle_file = wxT("other");
	m_vectSatType[21].desc = wxT("Other");
	m_vectSatType[21].sat_type = EARTH_SAT_TYPE_OTHER;
	m_vectSatType[21].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Other Communications 
	m_vectSatType[22].tle_file = wxT("other-comm");
	m_vectSatType[22].desc = wxT("Other Communications");
	m_vectSatType[22].sat_type = EARTH_SAT_TYPE_OTHER_COMM;
	m_vectSatType[22].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Radar Calibration 
	m_vectSatType[23].tle_file = wxT("radar");
	m_vectSatType[23].desc = wxT("Radar Calibration");
	m_vectSatType[23].sat_type = EARTH_SAT_TYPE_RADAR;
	m_vectSatType[23].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Raduga 
	m_vectSatType[24].tle_file = wxT("raduga");
	m_vectSatType[24].desc = wxT("Raduga");
	m_vectSatType[24].sat_type = EARTH_SAT_TYPE_RADUGA;
	m_vectSatType[24].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Earth Resources
	m_vectSatType[25].tle_file = wxT("resource");
	m_vectSatType[25].desc = wxT("Earth Resources");
	m_vectSatType[25].sat_type = EARTH_SAT_TYPE_EARTH_RESOURCES;
	m_vectSatType[25].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Search & Rescue
	m_vectSatType[26].tle_file = wxT("sarsat");
	m_vectSatType[26].desc = wxT("Search & Rescue");
	m_vectSatType[26].sat_type = EARTH_SAT_TYPE_SEARCH_AND_RESCUE;
	m_vectSatType[26].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Satellite-Based Augmentation System 
	m_vectSatType[27].tle_file = wxT("sbas");
	m_vectSatType[27].desc = wxT("Satellite-Based Augmentation System");
	m_vectSatType[27].sat_type = EARTH_SAT_TYPE_SBAS;
	m_vectSatType[27].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Space & Earth Science 
	m_vectSatType[28].tle_file = wxT("science");
	m_vectSatType[28].desc = wxT("Space & Earth Science");
	m_vectSatType[28].sat_type = EARTH_SAT_TYPE_SCIENCE;
	m_vectSatType[28].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Space Stations 
	m_vectSatType[29].tle_file = wxT("stations");
	m_vectSatType[29].desc = wxT("Space Stations");
	m_vectSatType[29].sat_type = EARTH_SAT_TYPE_SPACE_STATION;
	m_vectSatType[29].obj_type = ARTIFICIAL_EARTH_SPACE_STATION;
	// Tracking & Data Relay Satellite System 
	m_vectSatType[30].tle_file = wxT("tdrss");
	m_vectSatType[30].desc = wxT("Tracking & Data Relay Satellite System");
	m_vectSatType[30].sat_type = EARTH_SAT_TYPE_TDRSS;
	m_vectSatType[30].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Weather 
	m_vectSatType[31].tle_file = wxT("weather");
	m_vectSatType[31].desc = wxT("Weather");
	m_vectSatType[31].sat_type = EARTH_SAT_TYPE_WEATHER;
	m_vectSatType[31].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Experimental Communications 
	m_vectSatType[32].tle_file = wxT("x-comm");
	m_vectSatType[32].desc = wxT("Experimental Communications");
	m_vectSatType[32].sat_type = EARTH_SAT_TYPE_XCOMM;
	m_vectSatType[32].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// Last 30 Days' Launches (tle-new.txt)
	m_vectSatType[33].tle_file = wxT("tle-new");
	m_vectSatType[33].desc = wxT("Last 30 Days' Launches");
	m_vectSatType[33].sat_type = EARTH_SAT_TYPE_30DAYS_NEW;
	m_vectSatType[33].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// 100 (or so) Brightest (visual.txt)
	m_vectSatType[34].tle_file = wxT("visual");
	m_vectSatType[34].desc = wxT("00 (or so) Brightest");
	m_vectSatType[34].sat_type = EARTH_SAT_TYPE_VISUAL;
	m_vectSatType[34].obj_type = ARTIFICIAL_EARTH_SATELLITE;

	/////////////////////
	// :: SPACE DEBRIS 
	// FENGYUN 1C Debris 
	m_vectSatType[35].tle_file = wxT("1999-025");
	m_vectSatType[35].desc = wxT("FENGYUN 1C Debris");
	m_vectSatType[35].sat_type = EARTH_SAT_TYPE_FENGYUN_1C;
	m_vectSatType[35].obj_type = ARTIFICIAL_EARTH_SATELLITE_DEBRIS;
	// Iridium 33 Debris 
	m_vectSatType[36].tle_file = wxT("iridium-33-debris");
	m_vectSatType[36].desc = wxT("Iridium 33 Debris");
	m_vectSatType[36].sat_type = EARTH_SAT_TYPE_IRIDUM_33;
	m_vectSatType[36].obj_type = ARTIFICIAL_EARTH_SATELLITE_DEBRIS;
	// Cosmos 2251 Debris 
	m_vectSatType[37].tle_file = wxT("cosmos-2251-debris");
	m_vectSatType[37].desc = wxT("Cosmos 2251 Debris");
	m_vectSatType[37].sat_type = EARTH_SAT_TYPE_COSMOS_2251;
	m_vectSatType[37].obj_type = ARTIFICIAL_EARTH_SATELLITE_DEBRIS;

	m_nSatType = 38;

	////////////////////////////////////////////
	// define object types
	// :: Unknown Object
	m_vectObjType[0].desc = wxT("Unknown Object");
	m_vectObjType[0].obj_type = EARTH_ORBIT_OBJECT_UNKNOWN;
	// :: Artificial Earth Satellites
	m_vectObjType[1].desc = wxT("Artificial Earth Satellites");
	m_vectObjType[1].obj_type = ARTIFICIAL_EARTH_SATELLITE;
	// :: Space Station
	m_vectObjType[2].desc = wxT("Space Stations");
	m_vectObjType[2].obj_type = ARTIFICIAL_EARTH_SPACE_STATION;
	// :: Artificial Earth Satellite Debris
	m_vectObjType[3].desc = wxT("Artificial Earth Satellite Debris");
	m_vectObjType[3].obj_type = ARTIFICIAL_EARTH_SATELLITE_DEBRIS;
	// :: Rocket/Shutle Debris
	m_vectObjType[4].desc = wxT("Rocket/Shutle Debris");
	m_vectObjType[4].obj_type = ARTIFICIAL_EARTH_SHUTLE_DEBRIS;
	// :: All Kind of Launch Debris
	m_vectObjType[5].desc = wxT("All Kind of Launch Debris");
	m_vectObjType[5].obj_type = ARTIFICIAL_EARTH_LAUNCH_DEBRIS;
	// :: Artificial Mixed Debris
	m_vectObjType[6].desc = wxT("Artificial Mixed Debris");
	m_vectObjType[6].obj_type = ARTIFICIAL_EARTH_MIXED_DEBRIS;
	// :: Not Classified Earth Orbit Objects
	m_vectObjType[7].desc = wxT("Not Classified Earth Orbit Objects");
	m_vectObjType[7].obj_type = EARTH_ORBIT_OBJECT_MIXED;

	m_nObjType = 8;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyCatalogAes
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogAes::~CSkyCatalogAes()
{
	// remove :: catalogs
	UnloadCatalog( );

	// messier like this ?
//	free( m_vectMessier );
//	m_nMessier = 0;
}

/////////////////////////////////////////////////////////////////////
// Method:	UnloadCatalog
// Class:	CSkyCatalogAes
// Purpose:	unload aes catalog
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSkyCatalogAes::UnloadCatalog( )
{
	if( m_vectData )
	{
		for( int i=0; i<m_nData; i++ )
		{
			//free( m_vectData[i].vectSatType );
			free( m_vectData[i].vectTLERec );
		}
		free( m_vectData );
	}

	m_vectData = NULL;
	m_nData = 0;
	m_nAllocated = 0;

	return;
}

/////////////////////////////////////////////////////////////////////
int CSkyCatalogAes::AddSatTypeToObject( DefCatAes* aes, unsigned char nSatType )
{
	if( aes->nSatType >= 100 ) return( 0 );

	aes->vectSatType[aes->nSatType] = nSatType;
	aes->nSatType++;

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
int CSkyCatalogAes::AddTLERecordToObject( DefCatAes* aes, DefTLERecord& tle )
{
	// check if I need to reallocate
	if( aes->nTLERec >= aes->nTLERecAllocated )
	{
		// incremen allocation counter
		aes->nTLERecAllocated += VECT_AES_TLE_MEM_ALLOC_BLOCK_SIZE;
		// reallocate
		aes->vectTLERec = (DefTLERecord*) _recalloc( aes->vectTLERec, aes->nTLERecAllocated, sizeof(DefTLERecord) );
	}
	
	DefTLERecord* aesTLE = &(aes->vectTLERec[aes->nTLERec]);

	aesTLE->epoch_time = tle.epoch_time;
	aesTLE->element_number = tle.element_number;
	aesTLE->mean_motion_second_time_deriv = tle.mean_motion_second_time_deriv;
	aesTLE->object_drag_coeff = tle.object_drag_coeff;
	aesTLE->orbit_decay_rate = tle.orbit_decay_rate;
	aesTLE->mean_motion = tle.mean_motion;
	aesTLE->inclination = tle.inclination;
	aesTLE->asc_node_ra = tle.asc_node_ra;
	aesTLE->eccentricity = tle.eccentricity;
	aesTLE->arg_of_perigee = tle.arg_of_perigee;
	aesTLE->mean_anomaly = tle.mean_anomaly;
	aesTLE->orb_no_epoch = tle.orb_no_epoch;

	// increment counter
	aes->nTLERec++;

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	SortByMass
// Class:	SortByNoradCatNo
// Purpose:	sort aes in order of norad catalog number
// Input:	nothing
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CSkyCatalogAes::SortByNoradCatNo( )
{
//	unsigned long i = 0;
//	int bOrder = 1;
//	long nCount = 0;

	std::sort( m_vectData, m_vectData+m_nData, OnSortCatAesByNoradNo );

//	m_nMaxMag = m_vectData[m_nData-1].mass;
//	m_nMinMag = m_vectData[0].mass;

	// return status ...
	return( m_nData );
}

/////////////////////////////////////////////////////////////////////
long CSkyCatalogAes::SortTLERecords( )
{
	unsigned long i=0;

	for( i=0; i<m_nData; i++ )
	{
		std::sort( m_vectData[i].vectTLERec, m_vectData[i].vectTLERec+m_vectData[i].nTLERec, OnSortCatAesTLERecords );
	}

	// return status ...
	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetSatTypeByNoradNo
// Class:	CSkyCatalogAes
// Purpose:	get/check if there is an object in the database with this
//			norad number
// Input:	norad catalog number
// Output:	pointer to AES object if found or NULL
/////////////////////////////////////////////////////////////////////
DefCatAes* CSkyCatalogAes::GetSatTypeByNoradNo( unsigned long nCatNo )
{
	unsigned long i=0;

	// for all satellites in my db 
	for( i=0; i<m_nData; i++ )
	{
		if( m_vectData[i].norad_cat_no == nCatNo ) return( &(m_vectData[i]) );
	}

	return( NULL );
}

/////////////////////////////////////////////////////////////////////
int CSkyCatalogAes::IsObjCategory( DefCatAes* src, unsigned char nObjType )
{
	for( int i=0; i<m_vectData[i].nTLERec; i++ )
		if( src->vectSatType[i] == nObjType ) return( 1 );

	return( 0 );
}

/////////////////////////////////////////////////////////////////////
int CSkyCatalogAes::IsObjTLEEpoch( DefCatAes* src, double nEpoch )
{
	for( int i=0; i<m_vectData[i].nTLERec; i++ )
		if( src->vectTLERec[i].epoch_time == nEpoch ) return( 1 );

	return( 0 );
}

/////////////////////////////////////////////////////////////////////
int CSkyCatalogAes::InitObjTleEpoch( DefCatAes* src, double nStartTime, double nEndTime )
{
	double nDiff=0, nStartMinDiff=DBL_MAX, nEndMinDiff=DBL_MAX;
	double nCurrentTLETime=0.0;
	double nStartObsTime = nStartTime-JD1900;
	double nEndObsTime = nEndTime-JD1900;

	m_nObjTleSearchEpochStart = 0;
	m_nObjTleSearchEpochEnd = src->nTLERec;

	// find anchor
	for( int i=0; i<src->nTLERec; i++ )
	{
		nCurrentTLETime = src->vectTLERec[i].epoch_time;

		// look for start - which is end in fact because of the reverse order
		nDiff = fabs( nCurrentTLETime - nStartObsTime );
		if( nDiff < nStartMinDiff ) 
		{
			m_nObjTleSearchEpochEnd = i;
			nStartMinDiff = nDiff;
		}

		// look for end
		nDiff = fabs( nCurrentTLETime - nEndObsTime );
		if( nDiff < nEndMinDiff ) 
		{
			m_nObjTleSearchEpochStart = i;
			nEndMinDiff = nDiff;
		}
	}

	return( 0 );
}

/////////////////////////////////////////////////////////////////////
long CSkyCatalogAes::GetBestTleId( DefCatAes* src, double nObsTime )
{
	int bFound = 0;
	double nDiff = DBL_MAX, nMinDiff = DBL_MAX;
	double nCurrentTLETime=0.0;	

	nObsTime = nObsTime - JD1900;
//	for( int i=0; i<=src->nTLERec; i++ )
	for( int i=m_nObjTleSearchEpochStart; i<=m_nObjTleSearchEpochEnd; i++ )
	{
		nCurrentTLETime = src->vectTLERec[i].epoch_time;
		nDiff = fabs( nCurrentTLETime - nObsTime );
		if( nDiff < nMinDiff ) 
		{
			bFound = i;
			nMinDiff = nDiff;
		}
	}

	return( bFound );
}

#define SAT_USE_SAT_CODE
#ifdef SAT_USE_SAT_CODE
/////////////////////////////////////////////////////////////////////
// Method:	CalcRaDec
// Class:	CSkyCatalogAes
// Purpose:	cal aes ra/dec by cat no - with sat code
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogAes::CalcRaDec( DefCatAes& aes, double nTime,
									double nLat, double nLon, double nAlt,
									double& nRa, double& nDec, double& nDist, int bEpochSet )
{
	double lat = 0., lon = 0., ht_in_meters = 0., jd = 0.;
	// default to SGP4 
	int ephem = 1;
	// Pointer to two-line elements set for satellite 
	tle_t tle;
	int is_deep=0;
	double sat_params[N_SAT_PARAMS], observer_loc[3];
	double rho_sin_phi, rho_cos_phi;
	double ra, dec, dist_to_satellite, t_since;
	// Satellite position vector
	double pos[3]; 
	// best TLE element
	long nTLEEpochId=0;

	// set observer position/time
	jd = nTime;
	lat = nLat * DEG2RAD;
	lon = nLon * DEG2RAD;
	ht_in_meters = nAlt;

	//////////////////////////////////////////
	// get closest tle epoch element for this object at observer time
	if( bEpochSet == 0 )
	{
		nTLEEpochId = GetBestTleId( &aes, nTime );
		aes.nTLERecInUse = nTLEEpochId;
	} else
		nTLEEpochId = 0;

	DefTLERecord* aesTLE = &(aes.vectTLERec[nTLEEpochId]);

	//////////////////
	// set my tle element
	tle.norad_number = aes.norad_cat_no;
	tle.ephemeris_type = aes.ephemeris_type;

	tle.xmo = aesTLE->mean_anomaly * DEG2RAD;
	tle.xnodeo = aesTLE->asc_node_ra * DEG2RAD;
	tle.omegao = aesTLE->arg_of_perigee * DEG2RAD;
	tle.xincl = aesTLE->inclination * DEG2RAD;

	// eccentricity - check if here is a decimal .ddddd
	tle.eo = aesTLE->eccentricity;

	// Make sure mean motion is null-terminated, since rev. no. may immediately follow.
	// sat->xno = atof( tbuff) * TWOPI / MINUTES_PER_DAY;
	tle.xno = aesTLE->mean_motion * TWOPI / MINUTES_PER_DAY;
	// line 1 - 33 - orbit_decay_rate
	tle.xndt2o = aesTLE->orbit_decay_rate * TWOPI / MINUTES_PER_DAY_SQUARED;
	// line 1 - 44 - mean_motion_second_time_deriv - not used in computation
	tle.xndd6o = aesTLE->mean_motion_second_time_deriv * TWOPI / MINUTES_PER_DAY_CUBED;

	// object_drag_coeff
	// sci(line1 + 53) * AE;
	tle.bstar = aesTLE->object_drag_coeff * AE;

	// epoch
	// sat->epoch = atof( line1 + 20) + J1900 + (double)year * 365. + (double)((year - 1) / 4);
	tle.epoch = aesTLE->epoch_time + MJD0;

	// line 1 - 64=4 - element_number
	tle.bulletin_number = aesTLE->element_number;
	//  line 1 - 7 - security_class
	tle.classification = aes.security_class;
	// line 1 - 9=8 - int_id_year+int_id_launch_no+int_id_launch_piece
//	sprintf( tle.intl_desig, "%.2d%.3d%s", aes.int_id_year, aes.int_id_launch_no, aes.int_id_launch_piece );

	// line 2 - 63 - orb_no_epoch
	tle.revolution_number = aesTLE->orb_no_epoch;

	//////////
	// select current TLE RECORD
	is_deep = select_ephemeris( &tle );

	// calculate observer coordinates
	lat_alt_to_parallax( lat, ht_in_meters, &rho_cos_phi, &rho_sin_phi );
	observer_cartesian_coords( jd, lon, rho_cos_phi, rho_sin_phi, observer_loc );

	// check by ephem type ?
	if( is_deep && (ephem == 1 || ephem == 2) )
		ephem += 2;    // switch to an SDx 
	if( !is_deep && (ephem == 3 || ephem == 4) )
		ephem -= 2;    // switch to an SGx 

//	ephem = tle.ephemeris_type;

	// debug
//	if( is_deep )
//		printf( "Deep-Space type Ephemeris (%s) selected:\n", ephem_names[ephem] );
//	else
//		printf( "Near-Earth type Ephemeris (%s) selected:\n", ephem_names[ephem] );

	////////////////////////////////////
	// Calling of NORAD routines 
	// Each NORAD routine (SGP, SGP4, SGP8, SDP4, SDP8)   
	// will be called in turn with the appropriate TLE set 
	t_since = (jd - tle.epoch) * 1440.;
	switch( ephem)
	{
		case 0:
			SGP_init( sat_params, &tle);
			SGP( t_since, &tle, sat_params, pos, NULL);
			break;

		case 1:
			SGP4_init( sat_params, &tle);
			SC_SGP4( t_since, &tle, sat_params, pos, NULL);
			break;

		case 2:
			SGP8_init( sat_params, &tle);
			SGP8( t_since, &tle, sat_params, pos, NULL);
			break;

		case 3:
			SDP4_init( sat_params, &tle);
			SC_SDP4( t_since, &tle, sat_params, pos, NULL);
			break;

		case 4:
			SDP8_init( sat_params, &tle);
			SDP8( t_since, &tle, sat_params, pos, NULL);
			break;
	}


	// calculate satellite RA/DEC & distance to satellite
	get_satellite_ra_dec_delta( observer_loc, pos, &ra, &dec, &dist_to_satellite );
//	double satRadius = sqrt (pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]);

	// transform to J2000
//	epoch_of_date_to_j2000( jd, &ra, &dec );

//	nut_eq( jd-MJD0, &ra, &dec );

//	DPT ra_dec, alt_az, latlon;
//	alt_az.y = op->s_alt;
//	alt_az.x = op->s_az;
//	latlon.y = lat;
//	latlon.x = lng;

	//full_alt_az_to_ra_dec( &ra_dec, &alt_az, mjd+JD1900, &latlon );
//	alt_az_to_ra_dec( op->s_alt, op->s_az, &ra, &dec, lat );

/*	DPT ra_dec, tmp;
//	ra = ra + green_sidereal_time( jd ) + lng;
	tmp.x = ra;
	tmp.y = dec;
	precess_pt( &ra_dec, &tmp, 2000. + (jd - J2000) / 365.25, 2000. );

	ra = -ra_dec.x;
	dec = ra_dec.y;
*/

/*
	double n_mjd = jd - MJD0;
	double n_mjed = n_mjd + deltat(n_mjd)/86400.0;

	Now np;
	np.n_mjd = jd - MJD0;
	np.n_lat = lat;
	np.n_lng = lon;
	np.n_elev = (double) nAlt/ERAD;
	np.n_pressure = 1010;
	np.n_tz = 0.0;
	np.n_temp = 15;
	np.n_epoch = MJ2000;

	double bet, lam, lsn, rsn;
	double alt, az;
	double dec_out;			// declination after parallax 
	// local hour angle before/after parallax 
	double ha_in, ha_out;	
	// local sidereal time 
	double lst;	
	// parallax correction 
	double dra, ddec;		
	double rho_topo;
	double rho = dist_to_satellite*1000./AU_KM;
	//double rho = (double) 358000./MAU;

	// keep mean coordinates 
	double tra = ra;
	double tdec = dec;

	// convert equatoreal ra/dec to mean geocentric ecliptic lat/long 
	eq_ecl( n_mjed, ra, dec, &bet, &lam );

	// find solar ecliptical long.(mean equinox) and distance from earth 
	sunpos( n_mjed, &lsn, &rsn, NULL );

	nut_eq( n_mjed, &ra, &dec );
//	ab_eq( n_mjed, lsn, &ra, &dec );


	// find parallax correction for equatoreal coords
	now_lst( &np, &lst );
	ha_in = hrrad(lst) - ra;
//	ha_in = ra;

	// convert to earth radii 
	//rho_topo = rho * MAU/ERAD;
	//rho_topo = (ERAD+dist_to_satellite*1000.)/ERAD;
	rho_topo = (ERAD+358000.*1000.)/ERAD;
	rho_topo = (dist_to_satellite*1000.)/ERAD;
	ta_par( ha_in, dec, lat, np.n_elev, &rho_topo, &ha_out, &dec_out );

	// transform into alt/az and apply refraction 
	hadec_aa( lat, ha_out, dec_out, &alt, &az );
	refract( np.n_pressure, np.n_temp, alt, &alt );

////////
// set here alt/az
//	op->s_alt = alt;
//	op->s_az = az;

	// Get parallax differences and apply to apparent or astrometric place
		// ra sign is opposite of ha 
	    dra = ha_in - ha_out;
	    ddec = dec_out - dec;
		// return topocentric distance in AU 
	    rho = rho_topo * ERAD/MAU; 

	// fill in ra/dec fields
		// astrometric geo/topocent 
	    ra = tra + dra;
	    dec = tdec + ddec;

//	radecrange( &ra, &dec );

	////////////
//	precess( n_mjed, MJ2000, &ra, &dec );
	precess( aesTLE->epoch_time, MJ2000, &ra, &dec );
	radecrange( &ra, &dec );
*/
//	now_lst( &np, &lst );
//	ra = hrrad(lst) - ra;

	// transform to J2000
	epoch_of_date_to_j2000( jd, &ra, &dec );
	radecrange( &ra, &dec );

	// set my ra/dec to return
	nRa = ra * RAD2DEG;
	nDec = dec * RAD2DEG;
	nDist = dist_to_satellite;

	return( 1 );
}

// use ecs-phem calc
#else

/////////////////////////////////////////////////////////////////////
// Method:	CalcRaDec
// Class:	CSkyCatalogAes
// Purpose:	cal aes ra/dec by cat no - with ecs-ephem
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogAes::CalcRaDec( DefCatAes& aes, double nTime,
									double nLat, double nLon, double nAlt,
									double& nRa, double& nDec, double& nDist, int bEpochSet )
{
	Now earth_loc;
	Obj aes_obj;
	wxDateTime mydate;
	long nTLEEpochId=0;

//	pref_set( PREF_EQUATORIAL, PREF_GEO );
	pref_set( PREF_EQUATORIAL, PREF_TOPO );
	////////////////////////////////////////////////////////////
	// set location/conditions on earth where the obs was taken
	earth_loc.n_mjd = nTime-JD1900; //MJD0;
/*	mydate.Set( nTime );
	double mjd;
	double nDay = (double) mydate.GetDay();
	int nMonth = mydate.GetMonth()+1;
	int nYear = mydate.GetYear();
	cal_mjd( nMonth, nDay, nYear, &mjd );
	double sec_part = (double) ((mydate.GetHour()*3600+mydate.GetMinute()*60+mydate.GetSecond())/86400.0);
	mjd += sec_part;
	earth_loc.n_mjd = mjd-0.5;
*/
	earth_loc.n_lat = nLat * DEG2RAD;
	earth_loc.n_lng = nLon * DEG2RAD;
	// stores it in earth radii from sea level
	earth_loc.n_elev = (double) nAlt/6378160.0;
	// time zone to set
	earth_loc.n_tz = 0;
	// dip of sun below hzn at twilight, >0 below, rads
	earth_loc.n_dip = 0.0;
	// desired precession display ep as an mjd, or EOD
	//mydate.ParseDate( "2008-01-01" );
	//earth_loc.n_epoch = nTime-MJD0;
	//earth_loc.n_epoch = nTime-JD1900;
	earth_loc.n_epoch = EOD;

	// atmospheric pressure, mBar
	earth_loc.n_pressure = 0; //1010;
	// atmospheric temp, degrees C
	earth_loc.n_temp = 15;
	// time zone name; 7 chars or less, always 0 at end
	strcpy( earth_loc.n_tznm, "" );

	//////////////////////////////////////////
	// get closest tle epoch element for this object at observer time
	if( bEpochSet == 0 )
	{
		nTLEEpochId = GetBestTleId( &aes, nTime );
		aes.nTLERecInUse = nTLEEpochId;
	} else
		nTLEEpochId = 0;

	DefTLERecord* aesTLE = &(aes.vectTLERec[nTLEEpochId]);

	/////////////////////////////////////////////
	// set object paramas EARTHSAT
	aes_obj.es.eso_epoch = aesTLE->epoch_time;
	aes_obj.es.eso_n = aesTLE->mean_motion;
	aes_obj.es.eso_inc = aesTLE->inclination;
	aes_obj.es.eso_raan = aesTLE->asc_node_ra;
	aes_obj.es.eso_e = aesTLE->eccentricity;

	aes_obj.es.eso_ap = aesTLE->arg_of_perigee;
	aes_obj.es.eso_M = aesTLE->mean_anomaly;
	aes_obj.es.eso_decay = aesTLE->orbit_decay_rate;
	aes_obj.es.eso_drag = aesTLE->object_drag_coeff;
	aes_obj.es.eso_orbit = aesTLE->orb_no_epoch;

	aes_obj.es.eso_startok = 0;
	aes_obj.es.eso_endok = 0;
	
	aes_obj.any.co_type = EARTHSAT;


	////////////////////////////////
	// calculate asteroid position 
	Sol_CalcObjOrbitPos( &earth_loc, &aes_obj );

	// set back the values - maybe here use object to set the other properties as well ??!
	nRa = aes_obj.any.co_ra;
	nDec = aes_obj.any.co_dec;

	return( 1 );
}

#endif

/////////////////////////////////////////////////////////////////////
// Method:	GetName
// Class:	CSkyCatalogAes
// Purpose:	Get satellite catalog name
// Input:	source, pointer to string to return
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogAes::GetName( DefCatAes& aes, wxString& strAesCatName )
{
	int k = 0;
//	char strCommonName[255];
	wxString strName;
	// vector of lavbel fields used to construct the label
	wxString vectLabelField[6];
	int nLabelField=0;
	int bObjLabelUseCatNo, bObjLabelUseCName, bObjLabelUseType;

	/////////////
	// set fields by object type
	if( aes.type == SKY_OBJECT_TYPE_AES )
	{
		bObjLabelUseCatNo = m_pSky->m_pConfig->m_bAESObjLabelUseCatNo;
//		bObjLabelUseCName = m_pSky->m_pConfig->m_bAESObjLabelUseCName;
		bObjLabelUseType = m_pSky->m_pConfig->m_bAESObjLabelUseType;

	} else
		return( 0 );

	if( aes.cat_type == CAT_OBJECT_TYPE_CELESTRAK )
	{
		// if to use cat no
		if( bObjLabelUseCatNo )
		{
			if( aes.cat_name )
				vectLabelField[nLabelField++].Printf( wxT("%hs"), aes.cat_name );
			else
				vectLabelField[nLabelField++].Printf( wxT("%d"), aes.cat_no );
		}

		//todo: add here option config check
//		sprintf( vectLabelField[nLabelField++], "%.1fKm", aes.iras_diam );

		//todo: add here option config check
//		sprintf( vectLabelField[nLabelField++], "%.2f", (int) aes.radius );


		// if to use common name
//			if( bDSOObjLabelUseCName && GetRadioCommonName( strCommonName, aes.cat_no, CAT_OBJECT_TYPE_SPECFIND ) )
//				sprintf( vectLabelField[nLabelField++], "%s", strCommonName );

	}

	////////////////////////////
	strName = wxT("");
	// cat aes :: now from all fields build name
	for( k=0; k<nLabelField; k ++ )
	{
		if( k == 0 ) 
			strName = vectLabelField[k];
		else
			strName += wxT(" - ") + vectLabelField[k];
	}
	
	if( nLabelField )
	{
		//strAesCatName = (char*) malloc( (strlen(strName)+1)*sizeof(char) );
		strAesCatName = strName;
	} else
	{
		//strAesCatName = wxT("");
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	DownloadCatalog
// Class:	CSkyCatalogAes
// Purpose:	download asteroids catalog from a remote location
// Input:	catalog number
// Output:	status 0=failure
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogAes::DownloadCatalog( int nCatId, int nCatSource, int bClear )
{
	unsigned long nCount = 0;

	// check if to clear the catalog
	if( bClear )
	{
		////////////////////////
		// do (un)allocation
		UnloadCatalog( );

	} else
	{
		// if not to clear load - binary
		Load( CATALOG_ID_CELESTRAK );
	}

	// if empty do the inital allocation
	if( !m_vectData || !m_nData )
	{
		// allocation 
		m_nAllocated = VECT_AES_START_MEM_ALLOC_BLOCK_SIZE;
		// main allocation
		m_vectData = (DefCatAes*) calloc( m_nAllocated+1, sizeof(DefCatAes) );

	}

	// check by source type - if unimap just call class unimapsite
	if( nCatSource == CATALOG_DOWNLOAD_SOURCE_UNIMAP )
	{
//		CUnimapSite rBinaryCat( this, m_pUnimapWorker );
//		strcat( m_strFile, "_unimap_test_" );
//		nCount = rBinaryCat.DownloadCatalog( m_strFile, nCatId );

	} else if( nCatSource == CATALOG_DOWNLOAD_SOURCE_CELESTRAK )
	{
		// just to make sure - not do it .. not needed in this case ???
//		UnloadCatalog( );

		//////////////
		// now check by type - catalog id
		if( nCatId == CATALOG_ID_CELESTRAK )
		{
			CSkyCatalogAesCelestrak rCat( this, m_pUnimapWorker );
			nCount = rCat.Load( CATALOG_LOAD_REMOTE, nCatSource );

			// if data present - export
			if( nCount > 0 )
			{
				// export binary - add an extra string for now - to test without breaking the existant
				//m_strFile += wxT( "_" );
				if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Save catalog in binary format ...") );

				// todo :: use local export binary - common in this case for all types of catalogs
				rCat.ExportBinary( m_vectData, m_nData );
			}

		}

	}

	return( nCount );
}

/////////////////////////////////////////////////////////////////////
// Method:	Load
// Class:	CSkyCatalogAes
// Purpose:	simple load based on local flags(ie prev load)
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogAes::Load( int nType, int bRegion, double nCenterRa, 
									double nCenterDec, double nRadius )
{
	unsigned long nCount=0;

	// check load type
	int nLoadType = CATALOG_LOAD_FILE_BINARY;
	if( m_pSky->m_pConfig->m_bAESCatLoadRemote ) nLoadType = CATALOG_LOAD_REMOTE;
	int nLoadSource = m_pSky->m_pConfig->m_nAESCatLoadRemoteLocation;

	if( nType == CATALOG_ID_CELESTRAK )
	{
		CSkyCatalogAesCelestrak oCat( this, m_pUnimapWorker );
		nCount = oCat.Load( nLoadType, nLoadSource, 0, bRegion, nCenterRa, nCenterDec, nRadius );

		// load catalog maps
		//m_pSky->m_pCatalogDsoAssoc->LoadPGCCatMaps( m_pUnimapWorker );

	}


	return( nCount );
}

/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogAes::LoadBinary( double nCenterRa, double nCenterDec, 
										double nRadius, int bRegion )
{
	return( LoadBinary( m_strFile, m_nLastRegionLoadedCenterRa,
			m_nLastRegionLoadedCenterDec, m_nLastRegionLoadedWidth, m_bLastLoadRegion ) );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadBinary
// Class:	CSkyCatalogAes
// Purpose:	load the binary version of the aes catalog
// Input:	if to load region or all
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogAes::LoadBinary( const wxChar* strFile, double nCenterRa, double nCenterDec, 
										double nRadius, int bRegion )
{
	FILE* pFile = NULL;
	double nTimeRef=0;
	wxString strLog;
	double nDistDeg = 0;

	m_nData = 0;
	// info
	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("INFO :: Loading Artificial Eart Satellites %s catalog ..."), m_strName );
		m_pUnimapWorker->Log( strLog );
	}

	nTimeRef = GetDTime();

	// open file to write
	pFile = wxFopen( strFile, wxT("rb") );
	if( !pFile )
	{
		return( 0 );
	}

	// do the initial allocation
	if( m_vectData != NULL ) UnloadCatalog( );

	// now check if region allocate a small block only else allocate full
	if( bRegion )
	{
			m_nAllocated = VECT_AES_START_MEM_ALLOC_BLOCK_SIZE;
			// allocate my vector + 1 unit just to be sure
			m_vectData = (DefCatAes*) malloc( (m_nAllocated+1)*sizeof(DefCatAes) );

	} else
	{
		// now go at the end of the file and see the size
		fseek( pFile, 0, SEEK_END );
		// get the file size and calculate how many elements i have
		int nRecords = (int) ( ftell( pFile )/( sizeof(unsigned long)+5*sizeof(double) ));
		// allocate my vector + 1 unit just to be sure
		m_vectData = (DefCatAes*) malloc( (nRecords+1)*sizeof(DefCatAes) );
		// se allocated
		m_nAllocated = nRecords+1;
	}
	// go at the begining of the file
	fseek( pFile, 0, SEEK_SET );

	// now read all aes info from binary format
	while( !feof( pFile ) )
	{
		ResetObjCatAes( m_vectData[m_nData] );

		// init some
		m_vectData[m_nData].cat_type = CAT_OBJECT_TYPE_AES;
		m_vectData[m_nData].type = SKY_OBJECT_TYPE_AES;

		// :: name - 22 chars
		if( !fread( (void*) &(m_vectData[m_nData].cat_name), sizeof(char), 19, pFile ) )
			break;
		// :: catalog number 
		if( !fread( (void*) &(m_vectData[m_nData].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: satellite properties

		// check if flag for region set
		if( bRegion )
		{
			// calculate distance from center to current object
			nDistDeg = CalcSkyDistance( m_vectData[m_nData].ra, 
										m_vectData[m_nData].dec, 
											nCenterRa, nCenterDec );
			// check if out continue
			if( nDistDeg > nRadius ) continue;

		}
		// increment counter
		m_nData++;

		// check if I need to reallocate
		if( m_nData >= m_nAllocated )
		{
			// incremen counter
			m_nAllocated += VECT_AES_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_vectData = (DefCatAes*) realloc( m_vectData, (m_nAllocated+1)*sizeof(DefCatAes) );
		}

	}
	// close file handler
	fclose( pFile );

	// debug
	if( DEBUG_SHOW_PROC_TIMES_ON_FILE_LOAD )
	{
		nTimeRef = GetDTime()-nTimeRef;
		// make log line
		if( m_pUnimapWorker )
		{
			strLog.Printf( wxT("DEBUG :: Artificial Earth Satellite %s catalog(%d objects) was loaded in %lu.%lu seconds"), m_strName, 
						m_nData, (unsigned long) nTimeRef/1000, (unsigned long) nTimeRef%1000 );
			m_pUnimapWorker->Log( strLog );
		}
	}

	// if load ok set for last to remember - for optimization
	if( m_nData > 0 )
	{
		m_bLastLoadRegion = bRegion;
		m_nLastRegionLoadedCenterRa = nCenterRa;
		m_nLastRegionLoadedCenterDec = nCenterDec;
		m_nLastRegionLoadedWidth = nRadius;
		m_nLastRegionLoadedHeight = nRadius;
	}

	return( m_nData );
}

/////////////////////////////////////////////////////////////////////
// Method:	ExportBinary
// Class:	CSkyCatalogAes
// Purpose:	export satellites catalog as binary
// Input:	reference vector,size, and file to export to
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyCatalogAes::ExportBinary( DefCatAes* vectCatalog,
									unsigned int nSize )
{
	FILE* pFile = NULL;
	unsigned long i = 0;

	// open file to write
	pFile = wxFopen( m_strFile, wxT("wb") );
	if( !pFile )
	{
		return( 0 );
	}
	// now write all aes info in binary format
	for( i=0; i<nSize; i++ )
	{
		// :: name - 22 chars
		if( !fwrite( (void*) &(vectCatalog[i].cat_name), sizeof(char), 19, pFile ) )
			break;
		// :: catalog number
		if( !fwrite( (void*) &(vectCatalog[i].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: satellite properties

	}

	fclose( pFile );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
void ResetObjCatAes( DefCatAes& src )
{
	memset( &src, 0, sizeof(DefCatAes) );
/*
//#ifdef _DEBUG
	 bzero( src.cat_name, 30 );
//#else
//	bzero( src.cat_name, strlen(src.cat_name) );
//#endif

	src.cat_no=0;
	src.x=0.0;
	src.y=0.0;
	src.ra=0.0;
	src.dec=0.0;

	src.class_of_orbit = 0;
	src.type_of_orbit = 0;

	src.obj_type = 0;

	// satellite type vector
//	if( src.vectSatType ) free( src.vectSatType );
//	src.vectSatType = NULL;
	src.nSatType = 0;
//	src.nSatTypeAllocated = 0;

	src.norad_cat_no = 0;
	src.security_class = 0;

	src.int_id_year = 0;
	src.int_id_launch_no = 0;
	bzero( src.int_id_launch_piece, 4 );

	// reset tle record
//	if( src.vectTLERec ) free( src.vectTLERec );
	src.vectTLERec = NULL;

	src.nTLERec = 0;
	src.nTLERecInUse = 0;

	src.ephemeris_type = 0;

	src.detect_time_shift = 0.0;

	src.vmag = 0.0;
	src.dist_from_obs = 0.0;

	src.cat_type=0;
	src.type=0;
*/
};

/////////////////////////////////////////////////////////////////////
void ResetObjCatAesTLERec( DefTLERecord& src )
{
	memset( &src, 0, sizeof(DefTLERecord) );
/*
	src.epoch_time = 0.0;
	src.orbit_decay_rate=0.0;
	src.mean_motion_second_time_deriv=0;

	src.object_drag_coeff=0.0;

	src.element_number = 0;

	src.inclination=0.0;

	src.asc_node_ra=0.0;
	src.eccentricity=0.0;
	src.arg_of_perigee=0.0;
	src.mean_anomaly=0.0;
	src.mean_motion=0.0;

	src.orb_no_epoch=0;
*/
};
