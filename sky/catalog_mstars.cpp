
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
#include "catalog_mstars_wds.h"
#include "catalog_mstars_ccdm.h"

// main header
#include "catalog_mstars.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogMStars
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogMStars::CSkyCatalogMStars( CSky* pSky ) : CSkyCatalog( pSky )
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
// Class:	CSkyCatalogMStars
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogMStars::~CSkyCatalogMStars()
{
	// remove :: catalogs
	UnloadCatalog( );

	// messier like this ?
//	free( m_vectMessier );
//	m_nMessier = 0;
}

/////////////////////////////////////////////////////////////////////
// Method:	UnloadCatalog
// Class:	CSkyCatalogMStars
// Purpose:	unload multiple stars catalog
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSkyCatalogMStars::UnloadCatalog( )
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
// Class:	CSkyCatalogMStars
// Purpose:	sort multiple stars in order of magnitude
// Input:	nothing
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CSkyCatalogMStars::SortByMass( )
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
long CSkyCatalogMStars::SortByA( )
{
	std::sort( m_vectRadio, m_vectRadio+m_nRadio, OnSortCatDsoByA );

	// return status ...
	return( m_nDso );
}
*/

/////////////////////////////////////////////////////////////////////
// Method:	GetRaDec
// Class:	CSkyCatalogMStars
// Purpose:	get blackhole ra/dec by cat no
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogMStars::GetRaDec( unsigned long nCatNo, double& nRa, double& nDec )
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
// Class:	CSkyCatalogMStars
// Purpose:	Get blackhole catalog name
// Input:	source, pointer to string to return
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogMStars::GetName( DefCatMStars& mstars, wxString& strMStarsCatName )
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
	if( mstars.type == SKY_OBJECT_TYPE_MSTARS )
	{
		bObjLabelUseCatNo = m_pSky->m_pConfig->m_bMSObjLabelUseCatNo;
//		bObjLabelUseCName = m_pSky->m_pConfig->m_bRadObjLabelUseCName;
		bObjLabelUseType = m_pSky->m_pConfig->m_bMSObjLabelUseType;

	} else
		return( 0 );

	if( mstars.cat_type == CAT_OBJECT_TYPE_WDS )
	{
		// if to use cat no
		if( bObjLabelUseCatNo )
		{
			if( mstars.cat_name )
				vectLabelField[nLabelField++].Printf( wxT("WDS%s"), mstars.cat_name );
			else
				vectLabelField[nLabelField++].Printf( wxT("WDS%d"), mstars.cat_no );
		}

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%hs"), mstars.discoverer );

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%d"), (int) mstars.obs_date );

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%d"), (int) mstars.pos_ang );

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%.4f"), mstars.sep );

		// if to use common name
//			if( bDSOObjLabelUseCName && GetRadioCommonName( strCommonName, radioSource.cat_no, CAT_OBJECT_TYPE_SPECFIND ) )
//				sprintf( vectLabelField[nLabelField++], "%s", strCommonName );

	} else if( mstars.cat_type == CAT_OBJECT_TYPE_CCDM )
	{
		// if to use cat no
		if( bObjLabelUseCatNo )
		{
			if( mstars.cat_name )
				vectLabelField[nLabelField++].Printf( wxT("CCDM%s"), wxString(mstars.cat_name,wxConvUTF8) );
			else
				vectLabelField[nLabelField++].Printf( wxT("CCDM%d"), mstars.cat_no );
		}

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%s"), wxString(mstars.discoverer,wxConvUTF8) );

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%d"), (int) mstars.obs_date );

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%d"), (int) mstars.pos_ang );

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%.4f"), mstars.sep );

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
		strMStarsCatName = strName;
	} else
	{
		//strBlackholeCatName = wxT("");
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	DownloadCatalog
// Class:	CSkyCatalogMStars
// Purpose:	download blackholes catalog from a remote location
// Input:	catalog number
// Output:	status 0=failure
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogMStars::DownloadCatalog( int nCatId, int nCatSource )
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
		if( nCatId == CATALOG_ID_WDS )
		{
			CSkyCatalogMStarsWds rCat( this, m_pUnimapWorker );
			nCount = rCat.Load( CATALOG_LOAD_REMOTE, nCatSource );

			// if data present - export
			if( nCount > 0 )
			{
				// export binary - add an extra string for now - to test without breaking the existant
				//m_strFile += wxT( "_" );
				if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Save catalog in binary format ...") );
				rCat.ExportBinary( m_vectData, m_nData );
			}

		} else if( nCatId == CATALOG_ID_CCDM )
		{
			CSkyCatalogMStarsCcdm rCat( this, m_pUnimapWorker );
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
// Class:	CSkyCatalogMStars
// Purpose:	simple load based on local flags(ie prev load)
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogMStars::Load( int nType, int bRegion, double nCenterRa, 
									double nCenterDec, double nRadius )
{
	unsigned long nCount=0;

	// check load type
	int nLoadType = CATALOG_LOAD_FILE_BINARY;
	if( m_pSky->m_pConfig->m_bMSCatLoadRemote ) nLoadType = CATALOG_LOAD_REMOTE;
	int nLoadSource = m_pSky->m_pConfig->m_nMSCatLoadRemoteLocation;

	if( nType == CATALOG_ID_WDS )
	{
		CSkyCatalogMStarsWds oCat( this, m_pUnimapWorker );
		nCount = oCat.Load( nLoadType, nLoadSource, 0, bRegion, nCenterRa, nCenterDec, nRadius );

		// load catalog maps
		//m_pSky->m_pCatalogDsoAssoc->LoadPGCCatMaps( m_pUnimapWorker );

	} else 	if( nType == CATALOG_ID_CCDM )
	{
		CSkyCatalogMStarsCcdm oCat( this, m_pUnimapWorker );
		nCount = oCat.Load( nLoadType, nLoadSource, 0, bRegion, nCenterRa, nCenterDec, nRadius );

		// load catalog maps
		//m_pSky->m_pCatalogDsoAssoc->LoadPGCCatMaps( m_pUnimapWorker );

	}


	return( nCount );
}

/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogMStars::LoadBinary( double nCenterRa, double nCenterDec, 
										double nRadius, int bRegion )
{
	return( LoadBinary( m_strFile, m_nLastRegionLoadedCenterRa,
			m_nLastRegionLoadedCenterDec, m_nLastRegionLoadedWidth, m_bLastLoadRegion ) );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadBinary
// Class:	CSkyCatalogMStars
// Purpose:	load the binary version of a blackhole based catalog
// Input:	if to load region or all
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogMStars::LoadBinary( const wxChar* strFile, double nCenterRa, double nCenterDec, 
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
		strLog.Printf( wxT("INFO :: Loading Multiple-Stars %s catalog ..."), m_strName );
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
			m_nAllocated = 300;
			// allocate my vector + 1 unit just to be sure
			m_vectData = (DefCatMStars*) malloc( (m_nAllocated+1)*sizeof(DefCatMStars) );

	} else
	{
		// now go at the end of the file and see the size
		fseek( pFile, 0, SEEK_END );
		// get the file size and calculate how many elements i have
		int nRecords = (int) ( ftell( pFile )/( sizeof(unsigned long)+5*sizeof(double) ));
		// allocate my vector + 1 unit just to be sure
		m_vectData = (DefCatMStars*) malloc( (nRecords+1)*sizeof(DefCatMStars) );
		// se allocated
		m_nAllocated = nRecords+1;
	}
	// go at the begining of the file
	fseek( pFile, 0, SEEK_SET );

	// now write all stars info in binary format
	while( !feof( pFile ) )
	{
		ResetObjCatMStars( m_vectData[m_nData] );

		// init some
		m_vectData[m_nData].cat_type = CAT_OBJECT_TYPE_MSTARS;
		m_vectData[m_nData].type = SKY_OBJECT_TYPE_MSTARS;

		// :: name - 23 chars
		if( !fread( (void*) &(m_vectData[m_nData].cat_name), sizeof(char), 23, pFile ) )
			break;

		// :: catalog number 
		if( !fread( (void*) &(m_vectData[m_nData].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: position in the sky
		if( !fread( (void*) &(m_vectData[m_nData].ra), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].dec), sizeof(double), 1, pFile ) )
			break;
		// :: mstars properties
		if( !fread( (void*) &(m_vectData[m_nData].comp), sizeof(unsigned char), 1, pFile ) )
			break;

		// :: spectral type
		if( !fread( (void*) &(m_vectData[m_nData].spectral_type), sizeof(char), 10, pFile ) )
			break;

		if( !fread( (void*) &(m_vectData[m_nData].pos_ang), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].pos_ang2), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].pm_ra), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].pm_dec), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].pm_ra2), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].pm_dec2), sizeof(double), 1, pFile ) )
			break;

		if( !fread( (void*) &(m_vectData[m_nData].pm_note), sizeof(char), 5, pFile ) )
			break;

		if( !fread( (void*) &(m_vectData[m_nData].sep), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].sep2), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].nobs), sizeof(unsigned int), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].obs_date), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].obs_date2), sizeof(double), 1, pFile ) )
			break;

		if( !fread( (void*) &(m_vectData[m_nData].discoverer), sizeof(char), 10, pFile ) )
			break;

		if( !fread( (void*) &(m_vectData[m_nData].discoverer), sizeof(char), 10, pFile ) )
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
			m_nAllocated += VECT_MSTARS_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_vectData = (DefCatMStars*) realloc( m_vectData, (m_nAllocated+1)*sizeof(DefCatMStars) );
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
			strLog.Printf( wxT("DEBUG :: Multiple/Double Stars %s catalog(%d objects) was loaded in %lu.%lu seconds"), m_strName, 
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
// Class:	CSkyCatalogMStars
// Purpose:	export mstars catalog as binary
// Input:	reference vector,size, and file to export to
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyCatalogMStars::ExportBinary( DefCatMStars* vectCatalog,
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
	// now write all stars info in binary format
	for( i=0; i<nSize; i++ )
	{
		// :: name - 23 chars
		if( !fwrite( (void*) &(vectCatalog[i].cat_name), sizeof(char), 23, pFile ) )
			break;
		// :: catalog number
		if( !fwrite( (void*) &(vectCatalog[i].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: position in the sky
		if( !fwrite( (void*) &(vectCatalog[i].ra), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].dec), sizeof(double), 1, pFile ) )
			break;
		// :: mstars properties
		if( !fwrite( (void*) &(vectCatalog[i].comp), sizeof(unsigned char), 1, pFile ) )
			break;

		if( !fwrite( (void*) &(vectCatalog[i].spectral_type), sizeof(char), 10, pFile ) )
			break;

		if( !fwrite( (void*) &(vectCatalog[i].pos_ang), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].pos_ang2), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].pm_ra), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].pm_dec), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].pm_ra2), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].pm_dec2), sizeof(double), 1, pFile ) )
			break;

		if( !fwrite( (void*) &(vectCatalog[i].pm_note), sizeof(char), 5, pFile ) )
			break;

		if( !fwrite( (void*) &(vectCatalog[i].sep), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].sep2), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].nobs), sizeof(unsigned int), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].obs_date), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].obs_date2), sizeof(double), 1, pFile ) )
			break;

		if( !fwrite( (void*) &(vectCatalog[i].discoverer), sizeof(char), 10, pFile ) )
			break;

		if( !fwrite( (void*) &(vectCatalog[i].notes), sizeof(char), 10, pFile ) )
			break;
	}

	fclose( pFile );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
void ResetObjCatMStars( DefCatMStars& src )
{
	memset( &src, 0, sizeof(DefCatMStars) );

	return;

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

	src.mag=0.0;
	src.mag2=0.0;
	src.nobs=0;
	src.obs_date=0.0;
	src.obs_date2=0.0;
	src.pos_ang=0.0;
	src.pos_ang2=0.0;
	src.pm_ra=0.0;
	src.pm_dec=0.0;
	src.pm_ra2=0.0;
	src.pm_dec2=0.0;
	src.sep=0.0;
	src.sep2=0.0;

	bzero( src.cat_name, 30 );
	bzero( src.comp, 10 );
	bzero( src.spectral_type, 11 );
	bzero( src.pm_note, 5 );
	bzero( src.discoverer, 11 );
	bzero( src.notes, 11 );

	src.cat_type=0;
	src.type=0;
};
