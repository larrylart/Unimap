
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
#include "catalog_supernovas_asc.h"
#include "catalog_supernovas_ssc.h"

// main header
#include "catalog_supernovas.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogSupernovas
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogSupernovas::CSkyCatalogSupernovas( CSky* pSky ) : CSkyCatalog( pSky )
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
// Class:	CSkyCatalogSupernovas
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogSupernovas::~CSkyCatalogSupernovas()
{
	// remove :: catalogs
	UnloadCatalog( );

	// messier like this ?
//	free( m_vectMessier );
//	m_nMessier = 0;
}

/////////////////////////////////////////////////////////////////////
// Method:	UnloadCatalog
// Class:	CSkyCatalogSupernovas
// Purpose:	unload supernova catalog
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSkyCatalogSupernovas::UnloadCatalog( )
{
	if( m_vectData ) free( m_vectData );
	m_vectData = NULL;
	m_nData = 0;
	m_nAllocated = 0;

	return;
}

/*
/////////////////////////////////////////////////////////////////////
// Method:	sort stars  by magnitude
// Class:	CSkyCatalogSupernovas
// Purpose:	sort supernova in order of magnitude
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CSkyCatalogSupernovas::SortByMagnitude( )
{
//	unsigned long i = 0;
//	int bOrder = 1;
//	long nCount = 0;

	std::sort( m_vectRadio, m_vectRadio+m_nRadio, OnSortCatDso );

	m_nMaxMag = m_vectRadio[m_nDso-1].mag;
	m_nMinMag = m_vectRadio[0].mag;
	// return status ...
	return( m_nDso );
}

/////////////////////////////////////////////////////////////////////
long CSkyCatalogSupernovas::SortByA( )
{
	std::sort( m_vectRadio, m_vectRadio+m_nRadio, OnSortCatDsoByA );

	// return status ...
	return( m_nDso );
}
*/

/////////////////////////////////////////////////////////////////////
// Method:	GetRaDec
// Class:	CSkyCatalogSupernovas
// Purpose:	get supernova ra/dec by cat no
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogSupernovas::GetRaDec( unsigned long nCatNo, double& nRa, double& nDec )
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
// Class:	CSkyCatalogSupernovas
// Purpose:	Get a supernovas catalog name
// Input:	source, pointer to string to return
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogSupernovas::GetName( DefCatBasicSupernova& supernova, wxString& strSupernovaCatName )
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
	if( supernova.type == SKY_OBJECT_TYPE_SUPERNOVA )
	{
		bObjLabelUseCatNo = m_pSky->m_pConfig->m_bSNObjLabelUseCatNo;
//		bObjLabelUseCName = m_pSky->m_pConfig->m_bRadObjLabelUseCName;
		bObjLabelUseType = m_pSky->m_pConfig->m_bSNObjLabelUseType;

	} else
		return( 0 );

	if( supernova.cat_type == CAT_OBJECT_TYPE_ASC )
	{
		// if to use cat no
		if( bObjLabelUseCatNo )
		{
			if( supernova.cat_name )
				vectLabelField[nLabelField++].Printf( wxT("%s"), supernova.cat_name );
			else
				vectLabelField[nLabelField++].Printf( wxT("ASC %d"), supernova.cat_no );
		}

		//todo: add here option config check
		wxDateTime mydate;
		mydate.Set( supernova.date_max );
		wxString strDate = mydate.Format( wxT("%d/%m/%Y") );
		vectLabelField[nLabelField++].Printf( wxT("%s"), strDate );

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%s"), supernova.discoverer );

		// if to use common name
//			if( bDSOObjLabelUseCName && GetRadioCommonName( strCommonName, radioSource.cat_no, CAT_OBJECT_TYPE_SPECFIND ) )
//				sprintf( vectLabelField[nLabelField++], "%s", strCommonName );

	} else if( supernova.cat_type == CAT_OBJECT_TYPE_SSC )
	{
		// if to use cat no
		if( bObjLabelUseCatNo )
		{
			if( supernova.cat_name )
				vectLabelField[nLabelField++].Printf( wxT("%s"), supernova.cat_name );
			else
				vectLabelField[nLabelField++].Printf( wxT("SSC %d"), supernova.cat_no );
		}

		//todo: add here option config check
		wxDateTime mydate;
		mydate.Set( supernova.date_discovery );
		wxString strDate = mydate.Format( wxT("%d/%m/%Y") );
		vectLabelField[nLabelField++].Printf( wxT("%s"), strDate );

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%s"), supernova.discoverer );

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
		//strRadioCatName = (char*) malloc( (strlen(strName)+1)*sizeof(char) );
		strSupernovaCatName = strName;
	} else
	{
		//strRadioCatName = wxT("");
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	DownloadCatalog
// Class:	CSkyCatalogSupernovas
// Purpose:	download supernovas catalog from a remote location
// Input:	catalog number
// Output:	status 0=failure
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogSupernovas::DownloadCatalog( int nCatId, int nCatSource )
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
		if( nCatId == CATALOG_ID_ASC )
		{
			CSkyCatalogSupernovasAsc rCat( this, m_pUnimapWorker );
			nCount = rCat.Load( CATALOG_LOAD_REMOTE, nCatSource );

			// if data present - export
			if( nCount > 0 )
			{
				// export binary - add an extra string for now - to test without breaking the existant
				//m_strFile += wxT( "_" );
				if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Save catalog in binary format ...") );
				rCat.ExportBinary( m_vectData, m_nData );
			}

		} else if( nCatId == CATALOG_ID_SSC )
		{
			CSkyCatalogSupernovasSsc rCat( this, m_pUnimapWorker );
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
// Class:	CSkyCatalogSupernovas
// Purpose:	simple load based on local flags(ie prev load)
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogSupernovas::Load( int nType, int bRegion, double nCenterRa, 
									double nCenterDec, double nRadius )
{
	unsigned long nCount=0;

	// check load type
	int nLoadType = CATALOG_LOAD_FILE_BINARY;
	if( m_pSky->m_pConfig->m_bSNCatLoadRemote ) nLoadType = CATALOG_LOAD_REMOTE;
	int nLoadSource = m_pSky->m_pConfig->m_nSNCatLoadRemoteLocation;

	if( nType == CATALOG_ID_ASC )
	{
		CSkyCatalogSupernovasAsc oCat( this, m_pUnimapWorker );
		nCount = oCat.Load( nLoadType, nLoadSource, 0, bRegion, nCenterRa, nCenterDec, nRadius );

		// load catalog maps
		//m_pSky->m_pCatalogDsoAssoc->LoadPGCCatMaps( m_pUnimapWorker );

	} else 	if( nType == CATALOG_ID_SSC )
	{
		CSkyCatalogSupernovasSsc oCat( this, m_pUnimapWorker );
		nCount = oCat.Load( nLoadType, nLoadSource, 0, bRegion, nCenterRa, nCenterDec, nRadius );

		// load catalog maps
		//m_pSky->m_pCatalogDsoAssoc->LoadPGCCatMaps( m_pUnimapWorker );

	}


	return( nCount );
}

/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogSupernovas::LoadBinary( double nCenterRa, double nCenterDec, 
										double nRadius, int bRegion )
{
	return( LoadBinary( m_strFile, m_nLastRegionLoadedCenterRa,
			m_nLastRegionLoadedCenterDec, m_nLastRegionLoadedWidth, m_bLastLoadRegion ) );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadBinary
// Class:	CSkyCatalogSupernovas
// Purpose:	load the binary version of a supernovas based catalog
// Input:	if to load region or all
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogSupernovas::LoadBinary( const wxChar* strFile, double nCenterRa, double nCenterDec, 
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
		strLog.Printf( wxT("INFO :: Loading Supernovas %s catalog ..."), m_strName );
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
			m_vectData = (DefCatBasicSupernova*) malloc( (m_nAllocated+1)*sizeof(DefCatBasicSupernova) );

	} else
	{
		// now go at the end of the file and see the size
		fseek( pFile, 0, SEEK_END );
		// get the file size and calculate how many elements i have
		int nRecords = (int) ( ftell( pFile )/( sizeof(unsigned long)+5*sizeof(double) ));
		// allocate my vector + 1 unit just to be sure
		m_vectData = (DefCatBasicSupernova*) malloc( (nRecords+1)*sizeof(DefCatBasicSupernova) );
		// se allocated
		m_nAllocated = nRecords+1;
	}
	// go at the begining of the file
	fseek( pFile, 0, SEEK_SET );

	// now write all stars info in binary format
	while( !feof( pFile ) )
	{
		ResetObjCatSupernovas( m_vectData[m_nData] );

		// init some
		m_vectData[m_nData].cat_type = CAT_OBJECT_TYPE_SUPERNOVA;
		m_vectData[m_nData].type = SKY_OBJECT_TYPE_SUPERNOVA;

		// :: name - 22 chars
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
		// :: supernovas properties
		if( !fread( (void*) &(m_vectData[m_nData].unconfirmed), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].mag), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].max_mag), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].gal_center_offset_x), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].gal_center_offset_y), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].date_max), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].date_discovery), sizeof(double), 1, pFile ) )
			break;
		// strings 
		if( !fread( (void*) &(m_vectData[m_nData].parent_galaxy), sizeof(char), 20, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].sn_type), sizeof(char), 10, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].discoverer), sizeof(char), 50, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].prog_code), sizeof(char), 2, pFile ) )
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
			m_nAllocated += VECT_SUPERNOVAS_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_vectData = (DefCatBasicSupernova*) realloc( m_vectData, (m_nAllocated+1)*sizeof(DefCatBasicSupernova) );
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
			strLog.Printf( wxT("DEBUG :: Supernovas %s catalog(%d objects) was loaded in %lu.%lu seconds"), m_strName, 
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
// Class:	CSkyCatalogSupernovas
// Purpose:	export a supernovas catalog as binary
// Input:	reference vector,size, and file to export to
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyCatalogSupernovas::ExportBinary( DefCatBasicSupernova* vectCatalog,
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
		// :: name - 22 chars
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
		// :: supernovas properties
		if( !fwrite( (void*) &(vectCatalog[i].unconfirmed), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].disc_method), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].mag), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].max_mag), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].gal_center_offset_x), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].gal_center_offset_y), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].date_max), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].date_discovery), sizeof(double), 1, pFile ) )
			break;
		// strings
		if( !fwrite( (void*) &(vectCatalog[i].parent_galaxy), sizeof(char), 20, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].sn_type), sizeof(char), 10, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].discoverer), sizeof(char), 50, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].prog_code), sizeof(char), 2, pFile ) )
			break;
	}

	fclose( pFile );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
void ResetObjCatSupernovas( DefCatBasicSupernova& src )
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
	
	src.unconfirmed = 0;
	src.mag = 0.0;
	src.max_mag = 0.0;
	src.gal_center_offset_x = 0.0;
	src.gal_center_offset_y = 0.0;
	src.date_discovery = 0;
	src.date_max = 0;
	src.disc_method = 0;

	// empty string at needed size
	bzero( src.sn_type, 10 );
	bzero( src.parent_galaxy, 10 );
	bzero( src.discoverer, 50 );
	bzero( src.prog_code, 3 );

	src.cat_type=0;
	src.type=0;
};
