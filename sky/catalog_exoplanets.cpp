
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
// catalogs
#include "catalog_exoplanets_eu.h"

// main header
#include "catalog_exoplanets.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogExoplanets
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogExoplanets::CSkyCatalogExoplanets( CSky* pSky ) : CSkyCatalog( pSky )
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
// Class:	CSkyCatalogExoplanets
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogExoplanets::~CSkyCatalogExoplanets()
{
	// remove :: catalogs
	UnloadCatalog( );

	// messier like this ?
//	free( m_vectMessier );
//	m_nMessier = 0;
}

/////////////////////////////////////////////////////////////////////
// Method:	UnloadCatalog
// Class:	CSkyCatalogExoplanets
// Purpose:	unload exoplanets catalog
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSkyCatalogExoplanets::UnloadCatalog( )
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
// Class:	CSkyCatalogExoplanets
// Purpose:	sort exoplanets in order of magnitude
// Input:	nothing
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CSkyCatalogExoplanets::SortByMass( )
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
long CSkyCatalogExoplanets::SortByA( )
{
	std::sort( m_vectRadio, m_vectRadio+m_nRadio, OnSortCatDsoByA );

	// return status ...
	return( m_nDso );
}
*/

/////////////////////////////////////////////////////////////////////
// Method:	GetRaDec
// Class:	CSkyCatalogExoplanets
// Purpose:	get exoplanets ra/dec by cat no
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogExoplanets::GetRaDec( unsigned long nCatNo, double& nRa, double& nDec )
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
// Method:	GetName
// Class:	CSkyCatalogExoplanets
// Purpose:	Get exoplanets catalog name
// Input:	source, pointer to string to return
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogExoplanets::GetName( DefCatExoPlanet& exoplanet, wxString& strExoPlanetCatName )
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
	if( exoplanet.type == SKY_OBJECT_TYPE_EXOPLANET )
	{
		bObjLabelUseCatNo = m_pSky->m_pConfig->m_bEXPObjLabelUseCatNo;
//		bObjLabelUseCName = m_pSky->m_pConfig->m_bRadObjLabelUseCName;
		bObjLabelUseType = m_pSky->m_pConfig->m_bEXPObjLabelUseType;

	} else
		return( 0 );

	if( exoplanet.cat_type == CAT_OBJECT_TYPE_EXOPLANET_EU )
	{
		// if to use cat no
		if( bObjLabelUseCatNo )
		{
			if( exoplanet.cat_name )
				vectLabelField[nLabelField++].Printf( wxT("%s"), exoplanet.cat_name );
			else
				vectLabelField[nLabelField++].Printf( wxT("%d"), exoplanet.cat_no );
		}

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%.2lf"), exoplanet.mass );

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%.2lf"), exoplanet.radius );

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%.2lf"), exoplanet.period );

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%d"), (int) exoplanet.discovery_year );

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
		strExoPlanetCatName = strName;
	} else
	{
		//strBlackholeCatName = wxT("");
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	DownloadCatalog
// Class:	CSkyCatalogExoplanets
// Purpose:	download blackholes catalog from a remote location
// Input:	catalog number
// Output:	status 0=failure
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogExoplanets::DownloadCatalog( int nCatId, int nCatSource )
{
	unsigned long nCount = 0;

	// check by source type - if unimap just call class unimapsite
	if( nCatSource == CATALOG_DOWNLOAD_SOURCE_UNIMAP )
	{
//		CUnimapSite rBinaryCat( this, m_pUnimapWorker );
//		strcat( m_strFile, "_unimap_test_" );
//		nCount = rBinaryCat.DownloadCatalog( m_strFile, nCatId );

	} else if( nCatSource == CATALOG_DOWNLOAD_SOURCE_EXOPLANET_EU )
	{
		// just to make sure
		UnloadCatalog( );

		//////////////
		// now check by type - catalog id
		if( nCatId == CATALOG_ID_EXOPLANET_EU )
		{
			CSkyCatalogExoplanetsEu rCat( this, m_pUnimapWorker );
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
// Class:	CSkyCatalogExoplanets
// Purpose:	simple load based on local flags(ie prev load)
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogExoplanets::Load( int nType, int bRegion, double nCenterRa, 
									double nCenterDec, double nRadius )
{
	unsigned long nCount=0;

	// check load type
	int nLoadType = CATALOG_LOAD_FILE_BINARY;
	if( m_pSky->m_pConfig->m_bMSCatLoadRemote ) nLoadType = CATALOG_LOAD_REMOTE;
	int nLoadSource = m_pSky->m_pConfig->m_nMSCatLoadRemoteLocation;

	if( nType == CATALOG_ID_EXOPLANET_EU )
	{
		CSkyCatalogExoplanetsEu oCat( this, m_pUnimapWorker );
		nCount = oCat.Load( nLoadType, nLoadSource, 0, bRegion, nCenterRa, nCenterDec, nRadius );

		// load catalog maps
		//m_pSky->m_pCatalogDsoAssoc->LoadPGCCatMaps( m_pUnimapWorker );

	}


	return( nCount );
}

/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogExoplanets::LoadBinary( double nCenterRa, double nCenterDec, 
										double nRadius, int bRegion )
{
	return( LoadBinary( m_strFile, m_nLastRegionLoadedCenterRa,
			m_nLastRegionLoadedCenterDec, m_nLastRegionLoadedWidth, m_bLastLoadRegion ) );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadBinary
// Class:	CSkyCatalogExoplanets
// Purpose:	load the binary version of a blackhole based catalog
// Input:	if to load region or all
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogExoplanets::LoadBinary( const wxChar* strFile, double nCenterRa, double nCenterDec, 
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
		strLog.Printf( wxT("INFO :: Loading Exo-Planets %s catalog ..."), m_strName );
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
			m_nAllocated = VECT_EXOPLANETS_START_MEM_ALLOC_BLOCK_SIZE;
			// allocate my vector + 1 unit just to be sure
			m_vectData = (DefCatExoPlanet*) malloc( (m_nAllocated+1)*sizeof(DefCatExoPlanet) );

	} else
	{
		// now go at the end of the file and see the size
		fseek( pFile, 0, SEEK_END );
		// get the file size and calculate how many elements i have
		int nRecords = (int) ( ftell( pFile )/( sizeof(unsigned long)+5*sizeof(double) ));
		// allocate my vector + 1 unit just to be sure
		m_vectData = (DefCatExoPlanet*) malloc( (nRecords+1)*sizeof(DefCatExoPlanet) );
		// se allocated
		m_nAllocated = nRecords+1;
	}
	// go at the begining of the file
	fseek( pFile, 0, SEEK_SET );

	// now write all stars info in binary format
	while( !feof( pFile ) )
	{
		ResetObjCatExoplanet( m_vectData[m_nData] );

		// init some
		m_vectData[m_nData].cat_type = CAT_OBJECT_TYPE_EXOPLANET;
		m_vectData[m_nData].type = SKY_OBJECT_TYPE_EXOPLANET;

		// :: name - 23 chars
		if( !fread( (void*) &strTmp, sizeof(char), 23, pFile ) )
			break;
		strTmp[23] = '\0';
		wxStrncpy( m_vectData[m_nData].cat_name, wxString(strTmp,wxConvUTF8), 23 );
		// :: catalog number 
		if( !fread( (void*) &(m_vectData[m_nData].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: position in the sky
		if( !fread( (void*) &(m_vectData[m_nData].ra), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].dec), sizeof(double), 1, pFile ) )
			break;
		// :: exoplanet properties
		if( !fread( (void*) &(m_vectData[m_nData].mass), sizeof(double), 10, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].radius), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].period), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].sm_axis), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].eccentricity), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].inclination), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].ang_dist), sizeof(double), 1, pFile ) )
			break;

		if( !fread( (void*) &(m_vectData[m_nData].status), sizeof(unsigned char), 1, pFile ) )
			break;

		if( !fread( (void*) &(m_vectData[m_nData].discovery_year), sizeof(unsigned int), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].update_date), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].star_mag_v), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].star_mag_i), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].star_mag_h), sizeof(double), 1, pFile ) )
			break;

		if( !fread( (void*) &(m_vectData[m_nData].star_mag_j), sizeof(double), 1, pFile ) )
			break;

		if( !fread( (void*) &(m_vectData[m_nData].star_mag_k), sizeof(double), 1, pFile ) )
			break;

		if( !fread( (void*) &(m_vectData[m_nData].star_dist), sizeof(double), 1, pFile ) )
			break;

		if( !fread( (void*) &(m_vectData[m_nData].star_feh), sizeof(double), 1, pFile ) )
			break;

		if( !fread( (void*) &(m_vectData[m_nData].star_mass), sizeof(double), 1, pFile ) )
			break;

		if( !fread( (void*) &(m_vectData[m_nData].star_radius), sizeof(double), 1, pFile ) )
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
			m_nAllocated += VECT_EXOPLANETS_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_vectData = (DefCatExoPlanet*) realloc( m_vectData, (m_nAllocated+1)*sizeof(DefCatExoPlanet) );
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
			strLog.Printf( wxT("DEBUG :: Exo-Planets %s catalog(%d objects) was loaded in %lu.%lu seconds"), m_strName, 
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
// Class:	CSkyCatalogExoplanets
// Purpose:	export mstars catalog as binary
// Input:	reference vector,size, and file to export to
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyCatalogExoplanets::ExportBinary( DefCatExoPlanet* vectCatalog,
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
	// now write all stars info in binary format
	for( i=0; i<nSize; i++ )
	{
		// :: name - 22 chars
		strWxTmp = vectCatalog[i].cat_name;
		strncpy( strTmp, strWxTmp.ToAscii(), 23 );
		if( !fwrite( (void*) &strTmp, sizeof(char), 23, pFile ) )
			break;
		// :: catalog number
		if( !fwrite( (void*) &(vectCatalog[i].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: position in the sky
		if( !fwrite( (void*) &(vectCatalog[i].ra), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].dec), sizeof(double), 1, pFile ) )
			break;
		// :: exoplanet properties
		if( !fwrite( (void*) &(vectCatalog[i].mass), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].radius), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].period), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].sm_axis), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].eccentricity), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].inclination), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].ang_dist), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].status), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].discovery_year), sizeof(unsigned int), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].update_date), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].star_mag_v), sizeof(unsigned int), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].star_mag_i), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].star_mag_h), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].star_mag_j), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].star_mag_k), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].star_dist), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].star_feh), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].star_mass), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].star_radius), sizeof(double), 1, pFile ) )
			break;
	}

	fclose( pFile );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
void ResetObjCatExoplanet( DefCatExoPlanet& src )
{
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

	src.star_mag_v = 0.0;
	src.star_mag_i=0.0;
	src.star_mag_h=0.0;
	src.star_mag_j=0.0;
	src.star_mag_k=0.0;

	src.star_dist=0.0;
	src.star_feh=0.0;
	src.star_mass=0.0;
	src.star_radius=0.0;

	src.mass=0.0;
	src.radius=0.0;
	src.period=0.0;
	src.sm_axis=0.0;
	src.eccentricity=0.0;
	src.inclination=0.0;
	src.ang_dist=0.0;

	src.status=0.0;
	src.discovery_year=0.0;
	src.update_date=0.0;

	src.cat_type=0;
	src.type=0;
};
