////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// system libs
#include <stdio.h>
#include <stdlib.h>
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
#include "solar_orbits.h"
// astro lib from lib ephem
#include "libastro/libastro.h"
#include "libastro/preferences.h"
// catalogs
#include "catalog_asteroids_omp.h"

// main header
#include "catalog_asteroids.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogAsteroids
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogAsteroids::CSkyCatalogAsteroids( CSky* pSky ) : CSkyCatalog( pSky )
{
	m_pSky = pSky;
	m_pUnimapWorker = NULL;

	// dso vector
	m_vectData = NULL;
	m_nData = 0;
	m_nAllocated = 0;

	// reset last region loaded to zero
	m_bLastLoadRegion = 0;
	m_nLastRegionLoadedCenterRa = 0;
	m_nLastRegionLoadedCenterDec = 0;
	m_nLastRegionLoadedWidth = 0;
	m_nLastRegionLoadedHeight = 0;

	m_strName = wxT("");
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyCatalogAsteroids
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogAsteroids::~CSkyCatalogAsteroids()
{
	// remove :: catalogs
	UnloadCatalog( );

	// messier like this ?
//	free( m_vectMessier );
//	m_nMessier = 0;
}

/////////////////////////////////////////////////////////////////////
// Method:	UnloadCatalog
// Class:	CSkyCatalogAsteroids
// Purpose:	unload exoplanets catalog
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSkyCatalogAsteroids::UnloadCatalog( )
{
	if( m_vectData ) free( m_vectData );
	m_vectData = NULL;
	m_nData = 0;
	m_nAllocated = 0;

	return;
}

/*
/////////////////////////////////////////////////////////////////////
// Method:	SortByMass
// Class:	CSkyCatalogAsteroids
// Purpose:	sort asteroids in order of magnitude
// Input:	nothing
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CSkyCatalogAsteroids::SortByMass( )
{
//	unsigned long i = 0;
//	int bOrder = 1;
//	long nCount = 0;

	std::sort( m_vectData, m_vectData+m_nData, OnSortCatBlackholes );

	m_nMaxMag = m_vectData[m_nData-1].mass;
	m_nMinMag = m_vectData[0].mass;
	// return status ...
	return( m_nData );
}

/////////////////////////////////////////////////////////////////////
long CSkyCatalogAsteroids::SortByA( )
{
	std::sort( m_vectRadio, m_vectRadio+m_nRadio, OnSortCatDsoByA );

	// return status ...
	return( m_nDso );
}
*/

/////////////////////////////////////////////////////////////////////
// Method:	GetRaDec
// Class:	CSkyCatalogAsteroids
// Purpose:	get exoplanets ra/dec by cat no
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogAsteroids::GetRaDec( unsigned long nCatNo, double& nRa, double& nDec )
{
	int i =0;

	// if catalog loaded return
	if( !m_vectData ) LoadBinary( m_strFile, 0, 0, 0, 0 );

	// look in the entire catalog for my cat no
	for( i=0; i<m_nData; i++ )
	{
		if( m_vectData[i].cat_no == nCatNo )
		{
			nRa = m_vectData[i].ra;
			nDec = m_vectData[i].dec;
		}
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
int CSkyCatalogAsteroids::CalcRaDec( DefCatAsteroid& asteroid, double nTime,
									double nLat, double nLon, 
									double& nRa, double& nDec )
{
	long epoch_jd;
	ELEMENTS elem;
	double ra_dec[4];
	double latlon[2];

	// set lat / lon
	latlon[0] = nLat * DEG2RAD;
	latlon[1] = nLon * DEG2RAD;

//	epoch_jd = m_vectData[nCatId].epoch;
	elem.mean_anomaly = asteroid.mean_anomaly;
	elem.arg_per = asteroid.arg_perihelion;
	elem.asc_node = asteroid.lon_asc_node;
	elem.incl = asteroid.inclination;
	elem.ecc = asteroid.eccentricity;
	elem.major_axis = asteroid.smaj_axis;

	// extract_astorb_dat
	wxDateTime mydate;
	mydate.Set( asteroid.epoch );
/*	int d = mydate.GetDay();
	int m = mydate.GetMonth();
	int y = mydate.GetYear();
	epoch_jd = dmy_to_day( mydate.GetDay(),          // day 
							mydate.GetMonth()+1,	 // month
							mydate.GetYear(),        // year
                             CALENDAR_GREGORIAN );
    elem.epoch = (double)epoch_jd + .5;
*/

/*    epoch_jd = dmy_to_day( epoch_jd % 100L,           // day 
                             (epoch_jd / 100L) % 100L,  // month
                             epoch_jd / 10000L,         // year
                             CALENDAR_GREGORIAN );
*/
	elem.epoch = asteroid.epoch - 0.5;

	// do_remaining_element_setup( &elem);
/*	elem.mean_anomaly  *= PI / 180.;
	elem.arg_per       *= PI / 180.;
	elem.asc_node      *= PI / 180.;
	elem.incl          *= PI / 180.;
*/

	elem.mean_anomaly  *= DEG2RAD;
	elem.arg_per       *= DEG2RAD;
	elem.asc_node      *= DEG2RAD;
	elem.incl          *= DEG2RAD;

	elem.q = elem.major_axis * (1. - elem.ecc);
	derive_quantities( &elem, SOLAR_GM );
	elem.angular_momentum = sqrt( SOLAR_GM * elem.q * (1. + elem.ecc));
	elem.perih_time = elem.epoch - elem.mean_anomaly * elem.t0;
	elem.is_asteroid = 1;
	elem.central_obj = 0;
	

	calc_asteroid_posn( nTime, &elem, ra_dec, latlon );

	// set back the values
	nRa = ra_dec[0];
	nDec = ra_dec[1];

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
int CSkyCatalogAsteroids::CalcRaDecX( DefCatAsteroid& asteroid, double nTime,
									double nLat, double nLon, double nAlt,
									double& nRa, double& nDec )
{
	Now earth_loc;
	Obj ast_obj;
	wxDateTime mydate;

//	pref_set(PREF_EQUATORIAL, PREF_GEO);
	pref_set(PREF_EQUATORIAL, PREF_TOPO);

	////////////////////////////////////////////////////////////
	// set location/conditions on earth where the obs was taken
	earth_loc.n_mjd = nTime-MJD0+0.5;
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
	earth_loc.n_elev = (double) nAlt/6378160.0;
	// time zone to set
	earth_loc.n_tz = 0;
	// dip of sun below hzn at twilight, >0 below, rads
	earth_loc.n_dip = 0;
	// desired precession display ep as an mjd, or EOD
//	mydate.ParseDate( "2000-01-01" );
	earth_loc.n_epoch = EOD;
	// atmospheric pressure, mBar
	earth_loc.n_pressure = 1010;
	// atmospheric temp, degrees C
	earth_loc.n_temp = 15;
	// time zone name; 7 chars or less, always 0 at end
	strcpy( earth_loc.n_tznm, "" );

	/////////////////////////////////////////////
	// set object paramas
	ast_obj.e.eo_M = asteroid.mean_anomaly;
	ast_obj.e.eo_om = asteroid.arg_perihelion;
	ast_obj.e.eo_Om = asteroid.lon_asc_node;
	ast_obj.e.eo_inc = asteroid.inclination;
	ast_obj.e.eo_e = asteroid.eccentricity;
	ast_obj.e.eo_a = asteroid.smaj_axis;
	// magnitude
	ast_obj.e.eo_mag.m2 = asteroid.abs_mag_h;
	ast_obj.e.eo_mag.whichm = 0;
	// epoch date (M reference), as an mjd
	mydate.Set( asteroid.epoch );
	ast_obj.e.eo_cepoch = asteroid.epoch-MJD0;
//	cal_mjd( mydate.GetMonth()+1, (double)mydate.GetDay(), mydate.GetYear(), &mjd );
//	ast_obj.e.eo_cepoch = mjd-0.5;

	//mydate.ParseDate( "2009/01/01" );
	// equinox year (inc/Om/om reference), as an mjd.
	//cal_mjd( 1, 1, 2000, &mjd );
	ast_obj.e.eo_epoch = MJ2000; //asteroid.epoch-MJD0-05;
	//ast_obj.e.eo_epoch = mydate.GetJDN()-MJD0-0.5;
	// other unknown ???
	ast_obj.e.eo_size = 0;
	// nominal first mjd this set is ok, else 0
	ast_obj.e.eo_startok = 0;
	// nominal last mjd this set is ok, else 0
	ast_obj.e.eo_endok = 0;

	////////////////////////////////
	// calculate asteroid position 
	ast_obj.any.co_type = ELLIPTICAL;
	Sol_CalcObjOrbitPos( &earth_loc, &ast_obj );

	// set back the values
	nRa = ast_obj.any.co_ra * RAD2DEG;
	nDec = ast_obj.any.co_dec * RAD2DEG;
//	nRa = ast_obj.any.co_gaera;
//	nDec = ast_obj.any.co_gaedec;

	return( 1 );
}


/////////////////////////////////////////////////////////////////////
// Method:	GetName
// Class:	CSkyCatalogAsteroids
// Purpose:	Get asteroid catalog name
// Input:	source, pointer to string to return
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogAsteroids::GetName( DefCatAsteroid& asteroid, wxString& strAsteroidCatName )
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
	if( asteroid.type == SKY_OBJECT_TYPE_ASTEROID )
	{
		bObjLabelUseCatNo = m_pSky->m_pConfig->m_bASTObjLabelUseCatNo;
//		bObjLabelUseCName = m_pSky->m_pConfig->m_bRadObjLabelUseCName;
		bObjLabelUseType = m_pSky->m_pConfig->m_bASTObjLabelUseType;

	} else
		return( 0 );

	if( asteroid.cat_type == CAT_OBJECT_TYPE_OMP )
	{
		// if to use cat no
		if( bObjLabelUseCatNo )
		{
			if( asteroid.cat_name )
				vectLabelField[nLabelField++].Printf( wxT("%hs"), asteroid.cat_name );
			else
				vectLabelField[nLabelField++].Printf( wxT("%d"), asteroid.cat_no );
		}

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%.5fKm"), asteroid.iras_diam );

		//todo: add here option config check
//		sprintf( vectLabelField[nLabelField++], "%.2f", (int) asteroid.radius );


		// if to use common name
//			if( bDSOObjLabelUseCName && GetRadioCommonName( strCommonName, radioSource.cat_no, CAT_OBJECT_TYPE_SPECFIND ) )
//				sprintf( vectLabelField[nLabelField++], "%s", strCommonName );

	}

	////////////////////////////
	strName = wxT("");
	// cat dso :: now from all fields build name
	for( k=0; k<nLabelField; k ++ )
	{
		if( k == 0 ) 
			strName = vectLabelField[k];
		else
			strName += wxT(" - ") + vectLabelField[k];
	}
	
	if( nLabelField )
	{
		//strBlackholeCatName = (char*) malloc( (strlen(strName)+1)*sizeof(char) );
		strAsteroidCatName = strName;
	} else
	{
		//strBlackholeCatName = wxT("");
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	DownloadCatalog
// Class:	CSkyCatalogAsteroids
// Purpose:	download asteroids catalog from a remote location
// Input:	catalog number
// Output:	status 0=failure
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogAsteroids::DownloadCatalog( int nCatId, int nCatSource )
{
	unsigned long nCount = 0;

	// check by source type - if unimap just call class unimapsite
	if( nCatSource == CATALOG_DOWNLOAD_SOURCE_UNIMAP )
	{
//		CUnimapSite rBinaryCat( this, m_pUnimapWorker );
//		strcat( m_strFile, "_unimap_test_" );
//		nCount = rBinaryCat.DownloadCatalog( m_strFile, nCatId );

	} else if( nCatSource == CATALOG_DOWNLOAD_SOURCE_VIZIER )
	{
		// just to make sure
		UnloadCatalog( );

		//////////////
		// now check by type - catalog id
		if( nCatId == CATALOG_ID_OMP )
		{
			CSkyCatalogAsteroidsOmp rCat( this, m_pUnimapWorker );
			nCount = rCat.Load( CATALOG_LOAD_REMOTE, nCatSource );

			// if data present - export
			if( nCount > 0 )
			{
				// export binary - add an extra string for now - to test without breaking the existant
				//m_strFile += wxT( "_" );
				if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Save catalog in binary format ...") );
				rCat.ExportBinary( m_vectData, m_nData );
			}

		}

	}

	return( nCount );
}

/////////////////////////////////////////////////////////////////////
// Method:	Load
// Class:	CSkyCatalogAsteroids
// Purpose:	simple load based on local flags(ie prev load)
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogAsteroids::Load( int nType, int bRegion, double nCenterRa, 
									double nCenterDec, double nRadius )
{
	unsigned long nCount=0;

	// check load type
	int nLoadType = CATALOG_LOAD_FILE_BINARY;
	if( m_pSky->m_pConfig->m_bASTCatLoadRemote ) nLoadType = CATALOG_LOAD_REMOTE;
	int nLoadSource = m_pSky->m_pConfig->m_nASTCatLoadRemoteLocation;

	if( nType == CATALOG_ID_OMP )
	{
		CSkyCatalogAsteroidsOmp oCat( this, m_pUnimapWorker );
		nCount = oCat.Load( nLoadType, nLoadSource, 0, bRegion, nCenterRa, nCenterDec, nRadius );

		// load catalog maps
		//m_pSky->m_pCatalogDsoAssoc->LoadPGCCatMaps( m_pUnimapWorker );

	}


	return( nCount );
}

/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogAsteroids::LoadBinary( double nCenterRa, double nCenterDec, 
										double nRadius, int bRegion )
{
	return( LoadBinary( m_strFile, m_nLastRegionLoadedCenterRa,
			m_nLastRegionLoadedCenterDec, m_nLastRegionLoadedWidth, m_bLastLoadRegion ) );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadBinary
// Class:	CSkyCatalogAsteroids
// Purpose:	load the binary version of the asteroids catalog
// Input:	if to load region or all
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogAsteroids::LoadBinary( const wxChar* strFile, double nCenterRa, double nCenterDec, 
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
		strLog.Printf( wxT("INFO :: Loading Asteroids %s catalog ..."), m_strName );
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
			m_nAllocated = VECT_ASTEROIDS_START_MEM_ALLOC_BLOCK_SIZE;
			// allocate my vector + 1 unit just to be sure
			m_vectData = (DefCatAsteroid*) malloc( (m_nAllocated+1)*sizeof(DefCatAsteroid) );

	} else
	{
		// now go at the end of the file and see the size
		fseek( pFile, 0, SEEK_END );
		// get the file size and calculate how many elements i have
		int nRecords = (int) ( ftell( pFile )/( sizeof(unsigned long)+5*sizeof(double) ));
		// allocate my vector + 1 unit just to be sure
		m_vectData = (DefCatAsteroid*) malloc( (nRecords+1)*sizeof(DefCatAsteroid) );
		// se allocated
		m_nAllocated = nRecords+1;
	}
	// go at the begining of the file
	fseek( pFile, 0, SEEK_SET );

	// now read all asteroids info from binary format
	while( !feof( pFile ) )
	{
		ResetObjCatAsteroid( m_vectData[m_nData] );

		// init some
		m_vectData[m_nData].cat_type = CAT_OBJECT_TYPE_ASTEROID;
		m_vectData[m_nData].type = SKY_OBJECT_TYPE_ASTEROID;

		// :: name - 19 chars
		if( !fread( (void*) &(m_vectData[m_nData].cat_name), sizeof(char), 19, pFile ) )
			break;
		// :: catalog number 
		if( !fread( (void*) &(m_vectData[m_nData].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: asteroid properties
		if( !fread( (void*) &(m_vectData[m_nData].ast_no), sizeof(unsigned long), 1, pFile ) )
			break;

		if( !fread( (void*) &(m_vectData[m_nData].comp_name), sizeof(char), 17, pFile ) )
			break;

		if( !fread( (void*) &(m_vectData[m_nData].abs_mag_h), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].slope_mag), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].color_index), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].iras_diam), sizeof(double), 1, pFile ) )
			break;

		if( !fread( (void*) &(m_vectData[m_nData].iras_class), sizeof(char), 5, pFile ) )
			break;

		if( !fread( (void*) &(m_vectData[m_nData].no_arc), sizeof(unsigned long), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].no_obs), sizeof(unsigned int), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].epoch), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].mean_anomaly), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].arg_perihelion), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].lon_asc_node), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].date_orb_comp), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].update_date), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].inclination), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].eccentricity), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].smaj_axis), sizeof(double), 1, pFile ) )
			break;

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
			m_nAllocated += VECT_ASTEROIDS_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_vectData = (DefCatAsteroid*) realloc( m_vectData, (m_nAllocated+1)*sizeof(DefCatAsteroid) );
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
			strLog.Printf( wxT("DEBUG :: Asteroids %s catalog(%d objects) was loaded in %lu.%lu seconds"), m_strName, 
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
// Class:	CSkyCatalogAsteroids
// Purpose:	export asteroids catalog as binary
// Input:	reference vector,size, and file to export to
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyCatalogAsteroids::ExportBinary( DefCatAsteroid* vectCatalog,
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
	// now write all asteroids info in binary format
	for( i=0; i<nSize; i++ )
	{
		// :: name - 19 chars
		if( !fwrite( (void*) &(vectCatalog[i].cat_name), sizeof(char), 19, pFile ) )
			break;
		// :: catalog number
		if( !fwrite( (void*) &(vectCatalog[i].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: asteroid properties
		if( !fwrite( (void*) &(vectCatalog[i].ast_no), sizeof(unsigned long), 1, pFile ) )
			break;

		if( !fwrite( (void*) &(vectCatalog[i].comp_name), sizeof(char), 17, pFile ) )
			break;

		if( !fwrite( (void*) &(vectCatalog[i].abs_mag_h), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].slope_mag), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].color_index), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].iras_diam), sizeof(double), 1, pFile ) )
			break;

		if( !fwrite( (void*) &(vectCatalog[i].iras_class), sizeof(char), 5, pFile ) )
			break;

		if( !fwrite( (void*) &(vectCatalog[i].no_arc), sizeof(unsigned long), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].no_obs), sizeof(unsigned int), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].epoch), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].mean_anomaly), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].arg_perihelion), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].lon_asc_node), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].date_orb_comp), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].update_date), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].inclination), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].eccentricity), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].smaj_axis), sizeof(double), 1, pFile ) )
			break;
	}

	fclose( pFile );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
void ResetObjCatAsteroid( DefCatAsteroid& src )
{
	memset( &src, 0, sizeof(DefCatAsteroid) );

/*
//#ifdef _DEBUG
	 bzero( src.cat_name, 20 );
//#else
//	bzero( src.cat_name, strlen(src.cat_name) );
//#endif

	src.cat_no=0;
	src.x=0.0;
	src.y=0.0;
	src.ra=0.0;
	src.dec=0.0;

	bzero( src.comp_name, 16 );
	bzero( src.iras_class, 5 );

	src.ast_no = 0.0;
	src.abs_mag_h=0.0;
	src.slope_mag=0.0;
	src.color_index=0.0;
	src.iras_diam=0.0;

	src.no_arc=0;
	src.no_obs=0;
	src.epoch=0.0;
	src.mean_anomaly=0.0;
	src.arg_perihelion=0.0;

	src.lon_asc_node=0.0;
	src.date_orb_comp=0.0;
	src.update_date=0.0;

	src.inclination=0.0;
	src.eccentricity=0.0;
	src.smaj_axis=0.0;

	src.cat_type=0;
	src.type=0;
*/
};
