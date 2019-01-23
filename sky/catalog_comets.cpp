
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
//#include "solar_orbits.h"
// astro lib from lib ephem
#include "libastro/libastro.h"
#include "libastro/preferences.h"
// catalogs
#include "catalog_comets_cocom.h"

// main header
#include "catalog_comets.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogComets
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogComets::CSkyCatalogComets( CSky* pSky ) : CSkyCatalog( pSky )
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
// Class:	CSkyCatalogComets
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogComets::~CSkyCatalogComets()
{
	// remove :: catalogs
	UnloadCatalog( );

	// messier like this ?
//	free( m_vectMessier );
//	m_nMessier = 0;
}

/////////////////////////////////////////////////////////////////////
// Method:	UnloadCatalog
// Class:	CSkyCatalogComets
// Purpose:	unload exoplanets catalog
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSkyCatalogComets::UnloadCatalog( )
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
// Class:	CSkyCatalogComets
// Purpose:	sort comets in order of magnitude
// Input:	nothing
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CSkyCatalogComets::SortByMass( )
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
long CSkyCatalogComets::SortByA( )
{
	std::sort( m_vectData, m_vectData+m_nData, OnSortCatCometByA );

	// return status ...
	return( m_nDso );
}
*/

/////////////////////////////////////////////////////////////////////
// Method:	GetRaDec
// Class:	CSkyCatalogComets
// Purpose:	get exoplanets ra/dec by cat no
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogComets::GetRaDec( unsigned long nCatNo, double& nRa, double& nDec )
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
int CSkyCatalogComets::CalcRaDec( DefCatComet& comet, double nTime,
									double nLat, double nLon, double nAlt,
									double& nRa, double& nDec )
{
	Now earth_loc;
	Obj comet_obj;
	wxDateTime mydate;

	pref_set( PREF_EQUATORIAL, PREF_GEO );
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

/*
	/////////////////////////////////////////////
	// set object paramas ELLIPTIC
	comet_obj.e.eo_M = 0.0; //comet.mean_anomaly;
	comet_obj.e.eo_om = comet.arg_perihelion;
	comet_obj.e.eo_Om = comet.lon_orbital_node;
	comet_obj.e.eo_inc = comet.inclination;
	comet_obj.e.eo_e = comet.eccentricity;
	comet_obj.e.eo_a = comet.perihelion_distance/(1.0-comet.eccentricity); //comet.smaj_axis;
	// magnitude
//	ast_obj.e.eo_mag.m2 = comet.abs_mag_h;
//	ast_obj.e.eo_mag.whichm = 0;
	// epoch date (M reference), as an mjd
	mydate.Set( comet.epoch_comp );
	comet_obj.e.eo_cepoch = comet.epoch_comp-MJD0;
//	cal_mjd( mydate.GetMonth()+1, (double)mydate.GetDay(), mydate.GetYear(), &mjd );
//	ast_obj.e.eo_cepoch = mjd-0.5;

	//mydate.ParseDate( "2009/01/01" );
	// equinox year (inc/Om/om reference), as an mjd.
	//cal_mjd( 1, 1, 2000, &mjd );
	comet_obj.e.eo_epoch = J2000; //asteroid.epoch-MJD0-05;
	//ast_obj.e.eo_epoch = mydate.GetJDN()-MJD0-0.5;
	// other unknown ???
	comet_obj.e.eo_size = 0;
	// nominal first mjd this set is ok, else 0
	comet_obj.e.eo_startok = 0;
	// nominal last mjd this set is ok, else 0
	comet_obj.e.eo_endok = 0;

	comet_obj.any.co_type = ELLIPTICAL;
*/

/*
	/////////////////////////////////////////////
	// set object paramas HYPERBOLIC
	comet_obj.h.ho_om = comet.arg_perihelion;
	comet_obj.h.ho_Om = comet.lon_orbital_node;
	comet_obj.h.ho_inc = comet.inclination;
	comet_obj.h.ho_e = comet.eccentricity;
	comet_obj.h.ho_qp = comet.perihelion_distance;

	comet_obj.h.ho_ep = comet.epoch_comp-MJD0;
	comet_obj.h.ho_epoch = J2000;

	comet_obj.h.ho_size = 0;
	comet_obj.h.ho_startok = 0;
	comet_obj.h.ho_endok = 0;
	
	comet_obj.any.co_type = HYPERBOLIC;
*/


	/////////////////////////////////////////////
	// set object paramas PARABOLIC
	comet_obj.p.po_om = comet.arg_perihelion;
	comet_obj.p.po_Om = comet.lon_orbital_node;
	comet_obj.p.po_inc = comet.inclination;
//	comet_obj.p.po_e = comet.eccentricity;
	comet_obj.p.po_qp = comet.perihelion_distance;

	comet_obj.p.po_ep = comet.perihelion_date-MJD0;
	comet_obj.p.po_epoch = MJ2000;

	comet_obj.p.po_size = 0;
	comet_obj.p.po_startok = 0;
	comet_obj.p.po_endok = 0;
	
	comet_obj.any.co_type = PARABOLIC;


	////////////////////////////////
	// calculate asteroid position 
	Sol_CalcObjOrbitPos( &earth_loc, &comet_obj );

	// set back the values
	nRa = comet_obj.any.co_ra * RAD2DEG;
	nDec = comet_obj.any.co_dec * RAD2DEG;
//	nRa = ast_obj.any.co_gaera;
//	nDec = ast_obj.any.co_gaedec;

	return( 1 );
}


/////////////////////////////////////////////////////////////////////
// Method:	GetName
// Class:	CSkyCatalogComets
// Purpose:	Get comet catalog name
// Input:	source, pointer to string to return
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogComets::GetName( DefCatComet& comet, wxString& strCometCatName )
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
	if( comet.type == SKY_OBJECT_TYPE_COMET )
	{
		bObjLabelUseCatNo = m_pSky->m_pConfig->m_bCOMObjLabelUseCatNo;
//		bObjLabelUseCName = m_pSky->m_pConfig->m_bRadObjLabelUseCName;
		bObjLabelUseType = m_pSky->m_pConfig->m_bCOMObjLabelUseType;

	} else
		return( 0 );

	if( comet.cat_type == CAT_OBJECT_TYPE_COCOM )
	{
		// if to use cat no
		if( bObjLabelUseCatNo )
		{
			if( comet.cat_name )
				vectLabelField[nLabelField++].Printf( wxT("%s"), comet.cat_name );
			else
				vectLabelField[nLabelField++].Printf( wxT("%d"), comet.cat_no );
		}

		//todo: add here option config check
//		sprintf( vectLabelField[nLabelField++], "%.1fKm", comet.iras_diam );

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
		strCometCatName = strName;
	} else
	{
		//strBlackholeCatName = wxT("");
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	DownloadCatalog
// Class:	CSkyCatalogComets
// Purpose:	download asteroids catalog from a remote location
// Input:	catalog number
// Output:	status 0=failure
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogComets::DownloadCatalog( int nCatId, int nCatSource )
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
		if( nCatId == CATALOG_ID_COCOM )
		{
			CSkyCatalogCometsCocom rCat( this, m_pUnimapWorker );
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
// Class:	CSkyCatalogComets
// Purpose:	simple load based on local flags(ie prev load)
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogComets::Load( int nType, int bRegion, double nCenterRa, 
									double nCenterDec, double nRadius )
{
	unsigned long nCount=0;

	// check load type
	int nLoadType = CATALOG_LOAD_FILE_BINARY;
	if( m_pSky->m_pConfig->m_bCOMCatLoadRemote ) nLoadType = CATALOG_LOAD_REMOTE;
	int nLoadSource = m_pSky->m_pConfig->m_nCOMCatLoadRemoteLocation;

	if( nType == CATALOG_ID_COCOM )
	{
		CSkyCatalogCometsCocom oCat( this, m_pUnimapWorker );
		nCount = oCat.Load( nLoadType, nLoadSource, 0, bRegion, nCenterRa, nCenterDec, nRadius );

		// load catalog maps
		//m_pSky->m_pCatalogDsoAssoc->LoadPGCCatMaps( m_pUnimapWorker );

	}


	return( nCount );
}

/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogComets::LoadBinary( double nCenterRa, double nCenterDec, 
										double nRadius, int bRegion )
{
	return( LoadBinary( m_strFile, m_nLastRegionLoadedCenterRa,
			m_nLastRegionLoadedCenterDec, m_nLastRegionLoadedWidth, m_bLastLoadRegion ) );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadBinary
// Class:	CSkyCatalogComets
// Purpose:	load the binary version of the comets catalog
// Input:	if to load region or all
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogComets::LoadBinary( const wxChar* strFile, double nCenterRa, double nCenterDec, 
										double nRadius, int bRegion )
{
	FILE* pFile = NULL;
	double nTimeRef=0;
	wxString strLog;
	double nDistDeg = 0;
	char strTmp[255];

	m_nData = 0;
	// info
	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("INFO :: Loading Comets %s catalog ..."), m_strName );
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
			m_nAllocated = VECT_COMETS_START_MEM_ALLOC_BLOCK_SIZE;
			// allocate my vector + 1 unit just to be sure
			m_vectData = (DefCatComet*) malloc( (m_nAllocated+1)*sizeof(DefCatComet) );

	} else
	{
		// now go at the end of the file and see the size
		fseek( pFile, 0, SEEK_END );
		// get the file size and calculate how many elements i have
		int nRecords = (int) ( ftell( pFile )/( sizeof(unsigned long)+5*sizeof(double) ));
		// allocate my vector + 1 unit just to be sure
		m_vectData = (DefCatComet*) malloc( (nRecords+1)*sizeof(DefCatComet) );
		// se allocated
		m_nAllocated = nRecords+1;
	}
	// go at the begining of the file
	fseek( pFile, 0, SEEK_SET );

	// now read all comets info from binary format
	while( !feof( pFile ) )
	{
		ResetObjCatComet( m_vectData[m_nData] );

		// init some
		m_vectData[m_nData].cat_type = CAT_OBJECT_TYPE_COMET;
		m_vectData[m_nData].type = SKY_OBJECT_TYPE_COMET;

		// :: name - 19 chars
		if( !fread( (void*) &strTmp, sizeof(char), 19, pFile ) )
			break;
		strTmp[19] = '\0';
		wxStrncpy( m_vectData[m_nData].cat_name, wxString(strTmp,wxConvUTF8), 19 );
		// :: catalog number 
		if( !fread( (void*) &(m_vectData[m_nData].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: comet properties
		if( !fread( (void*) &(m_vectData[m_nData].note_update_date), sizeof(double), 1, pFile ) )
			break;

		if( !fread( (void*) &strTmp, sizeof(char), 13, pFile ) )
			break;
		strTmp[13] = '\0';
		wxStrncpy( m_vectData[m_nData].iau_code, wxString(strTmp,wxConvUTF8), 13 );

		if( !fread( (void*) &strTmp, sizeof(char), 11, pFile ) )
			break;
		strTmp[11] = '\0';
		wxStrncpy( m_vectData[m_nData].comp_name, wxString(strTmp,wxConvUTF8), 11 );

		if( !fread( (void*) &(m_vectData[m_nData].epoch_comp), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].rel_effect), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].perihelion_date), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].perihelion_distance), sizeof(double), 5, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].arg_perihelion), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].lon_orbital_node), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].inclination), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].eccentricity), sizeof(double), 1, pFile ) )
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
			m_nAllocated += VECT_COMETS_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_vectData = (DefCatComet*) realloc( m_vectData, (m_nAllocated+1)*sizeof(DefCatComet) );
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
			strLog.Printf( wxT("DEBUG :: Comets %s catalog(%d objects) was loaded in %lu.%lu seconds"), m_strName, 
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
// Class:	CSkyCatalogComets
// Purpose:	export comets catalog as binary
// Input:	reference vector,size, and file to export to
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyCatalogComets::ExportBinary( DefCatComet* vectCatalog,
									unsigned int nSize )
{
	FILE* pFile = NULL;
	unsigned long i = 0;
	char strTmp[255];
	wxString strWxTmp=wxT("");

	// open file to write
	pFile = wxFopen( m_strFile, wxT("wb") );
	if( !pFile )
	{
		return( 0 );
	}
	// now write all comets info in binary format
	for( i=0; i<nSize; i++ )
	{
		// :: name - 19 chars
		strWxTmp = vectCatalog[i].cat_name;
		strncpy( strTmp, strWxTmp.ToAscii(), 19 );
		if( !fwrite( (void*) &strTmp, sizeof(char), 19, pFile ) )
			break;
		// :: catalog number
		if( !fwrite( (void*) &(vectCatalog[i].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: comet properties
		if( !fwrite( (void*) &(vectCatalog[i].note_update_date), sizeof(double), 1, pFile ) )
			break;

		strWxTmp = vectCatalog[i].iau_code;
		strncpy( strTmp, strWxTmp.ToAscii(), 13 );
		if( !fwrite( (void*) &strTmp, sizeof(char), 13, pFile ) )
			break;

		strWxTmp = vectCatalog[i].comp_name;
		strncpy( strTmp, strWxTmp.ToAscii(), 11 );
		if( !fwrite( (void*) &strTmp, sizeof(char), 11, pFile ) )
			break;

		if( !fwrite( (void*) &(vectCatalog[i].epoch_comp), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].rel_effect), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].perihelion_date), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].perihelion_distance), sizeof(double), 5, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].arg_perihelion), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].lon_orbital_node), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].inclination), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].eccentricity), sizeof(double), 1, pFile ) )
			break;
	}

	fclose( pFile );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
void ResetObjCatComet( DefCatComet& src )
{
	memset( &src, 0, sizeof(DefCatComet) );

/******************
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

	bzero( src.iau_code, 14 );
	bzero( src.comp_name, 11 );

	src.note_update_date = 0.0;
	src.epoch_comp=0.0;
	src.rel_effect=0;

	src.perihelion_date=0.0;
	src.perihelion_distance=0.0;

	src.arg_perihelion=0.0;
	src.lon_orbital_node=0.0;
	src.inclination=0.0;

	src.eccentricity=0.0;

	src.cat_type=0;
	src.type=0;
************************/

};
