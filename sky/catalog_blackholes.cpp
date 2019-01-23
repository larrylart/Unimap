
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
#include "catalog_blackholes_sdss_dr5.h"
#include "catalog_blackholes_rxe_agn.h"

// main header
#include "catalog_blackholes.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogBlackholes
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogBlackholes::CSkyCatalogBlackholes( CSky* pSky ) : CSkyCatalog( pSky )
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
// Class:	CSkyCatalogBlackholes
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogBlackholes::~CSkyCatalogBlackholes()
{
	// remove :: catalogs
	UnloadCatalog( );

	// messier like this ?
//	free( m_vectMessier );
//	m_nMessier = 0;
}

/////////////////////////////////////////////////////////////////////
// Method:	UnloadCatalog
// Class:	CSkyCatalogBlackholes
// Purpose:	unload blackhole catalog
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSkyCatalogBlackholes::UnloadCatalog( )
{
	if( m_vectData ) free( m_vectData );
	m_vectData = NULL;
	m_nData = 0;
	m_nAllocated = 0;

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	SortByMass
// Class:	CSkyCatalogBlackholes
// Purpose:	sort blackholes in order of magnitude
// Input:	nothing
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CSkyCatalogBlackholes::SortByMass( )
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

/*
/////////////////////////////////////////////////////////////////////
long CSkyCatalogBlackholes::SortByA( )
{
	std::sort( m_vectRadio, m_vectRadio+m_nRadio, OnSortCatDsoByA );

	// return status ...
	return( m_nDso );
}
*/

/////////////////////////////////////////////////////////////////////
// Method:	GetRaDec
// Class:	CSkyCatalogBlackholes
// Purpose:	get blackhole ra/dec by cat no
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogBlackholes::GetRaDec( unsigned long nCatNo, double& nRa, double& nDec )
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
// Class:	CSkyCatalogBlackholes
// Purpose:	Get blackhole catalog name
// Input:	source, pointer to string to return
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogBlackholes::GetName( DefCatBlackhole& blackhole, wxString& strBlackholeCatName )
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
	if( blackhole.type == SKY_OBJECT_TYPE_BLACKHOLE )
	{
		bObjLabelUseCatNo = m_pSky->m_pConfig->m_bBHObjLabelUseCatNo;
//		bObjLabelUseCName = m_pSky->m_pConfig->m_bRadObjLabelUseCName;
		bObjLabelUseType = m_pSky->m_pConfig->m_bBHObjLabelUseType;

	} else
		return( 0 );

	if( blackhole.cat_type == CAT_OBJECT_TYPE_SDSS_DR5 )
	{
		// if to use cat no
		if( bObjLabelUseCatNo )
		{
			if( blackhole.cat_name )
				vectLabelField[nLabelField++].Printf( wxT("SDSS%s"), blackhole.cat_name );
			else
				vectLabelField[nLabelField++].Printf( wxT("SDSS-DR5 %d"), blackhole.cat_no );
		}

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%.4f"), blackhole.mass );

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%.4f"), blackhole.luminosity );

		// if to use common name
//			if( bDSOObjLabelUseCName && GetRadioCommonName( strCommonName, radioSource.cat_no, CAT_OBJECT_TYPE_SPECFIND ) )
//				sprintf( vectLabelField[nLabelField++], "%s", strCommonName );

	} else if( blackhole.cat_type == CAT_OBJECT_TYPE_RXE_AGN )
	{
		// if to use cat no
		if( bObjLabelUseCatNo )
		{
			if( blackhole.cat_name )
				vectLabelField[nLabelField++].Printf( wxT("SDSS%s"), blackhole.cat_name );
			else
				vectLabelField[nLabelField++].Printf( wxT("RXE-AGN %d"), blackhole.cat_no );
		}

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%.4f"), blackhole.mass );

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%.4f"), blackhole.xray_luminosity );

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
		strBlackholeCatName = strName;
	} else
	{
		//strBlackholeCatName = wxT("");
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	DownloadCatalog
// Class:	CSkyCatalogBlackholes
// Purpose:	download blackholes catalog from a remote location
// Input:	catalog number
// Output:	status 0=failure
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogBlackholes::DownloadCatalog( int nCatId, int nCatSource )
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
		if( nCatId == CATALOG_ID_SDSS_DR5 )
		{
			CSkyCatalogBlackholesSdssdr5 rCat( this, m_pUnimapWorker );
			nCount = rCat.Load( CATALOG_LOAD_REMOTE, nCatSource );

			// if data present - export
			if( nCount > 0 )
			{
				// export binary - add an extra string for now - to test without breaking the existant
				//m_strFile += wxT( "_" );
				if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Save catalog in binary format ...") );
				rCat.ExportBinary( m_vectData, m_nData );
			}

		} else if( nCatId == CATALOG_ID_RXE_AGN )
		{
			CSkyCatalogBlackholesRxeagn rCat( this, m_pUnimapWorker );
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
// Class:	CSkyCatalogBlackholes
// Purpose:	simple load based on local flags(ie prev load)
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogBlackholes::Load( int nType, int bRegion, double nCenterRa, 
									double nCenterDec, double nRadius )
{
	unsigned long nCount=0;

	// check load type
	int nLoadType = CATALOG_LOAD_FILE_BINARY;
	if( m_pSky->m_pConfig->m_bBHCatLoadRemote ) nLoadType = CATALOG_LOAD_REMOTE;
	int nLoadSource = m_pSky->m_pConfig->m_nBHCatLoadRemoteLocation;

	if( nType == CATALOG_ID_SDSS_DR5 )
	{
		CSkyCatalogBlackholesSdssdr5 oCat( this, m_pUnimapWorker );
		nCount = oCat.Load( nLoadType, nLoadSource, 0, bRegion, nCenterRa, nCenterDec, nRadius );

		// load catalog maps
		//m_pSky->m_pCatalogDsoAssoc->LoadPGCCatMaps( m_pUnimapWorker );

	} else 	if( nType == CATALOG_ID_RXE_AGN )
	{
		CSkyCatalogBlackholesRxeagn oCat( this, m_pUnimapWorker );
		nCount = oCat.Load( nLoadType, nLoadSource, 0, bRegion, nCenterRa, nCenterDec, nRadius );

		// load catalog maps
		//m_pSky->m_pCatalogDsoAssoc->LoadPGCCatMaps( m_pUnimapWorker );

	}


	return( nCount );
}

/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogBlackholes::LoadBinary( double nCenterRa, double nCenterDec, 
										double nRadius, int bRegion )
{
	return( LoadBinary( m_strFile, m_nLastRegionLoadedCenterRa,
			m_nLastRegionLoadedCenterDec, m_nLastRegionLoadedWidth, m_bLastLoadRegion ) );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadBinary
// Class:	CSkyCatalogBlackholes
// Purpose:	load the binary version of a blackhole based catalog
// Input:	if to load region or all
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogBlackholes::LoadBinary( const wxChar* strFile, double nCenterRa, double nCenterDec, 
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
		strLog.Printf( wxT("INFO :: Loading Blackholes %s catalog ..."), m_strName );
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
			m_vectData = (DefCatBlackhole*) malloc( (m_nAllocated+1)*sizeof(DefCatBlackhole) );

	} else
	{
		// now go at the end of the file and see the size
		fseek( pFile, 0, SEEK_END );
		// get the file size and calculate how many elements i have
		int nRecords = (int) ( ftell( pFile )/( sizeof(unsigned long)+5*sizeof(double) ));
		// allocate my vector + 1 unit just to be sure
		m_vectData = (DefCatBlackhole*) malloc( (nRecords+1)*sizeof(DefCatBlackhole) );
		// se allocated
		m_nAllocated = nRecords+1;
	}
	// go at the begining of the file
	fseek( pFile, 0, SEEK_SET );

	// now write all stars info in binary format
	while( !feof( pFile ) )
	{
		ResetObjCatBlackhole( m_vectData[m_nData] );

		// init some
		m_vectData[m_nData].cat_type = CAT_OBJECT_TYPE_BLACKHOLE;
		m_vectData[m_nData].type = SKY_OBJECT_TYPE_BLACKHOLE;

		// :: name - 23 chars
		if( !fread( (void*) &strTmp, sizeof(char), 23, pFile ) )
			break;
		wxStrncpy( m_vectData[m_nData].cat_name, wxString(strTmp,wxConvUTF8), 23 );
		// :: catalog number
		if( !fread( (void*) &(m_vectData[m_nData].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: position in the sky
		if( !fread( (void*) &(m_vectData[m_nData].ra), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].dec), sizeof(double), 1, pFile ) )
			break;
		// :: blackhole properties
		if( !fread( (void*) &(m_vectData[m_nData].target_type), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].mass), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].mass_hbeta), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].mass_mgii), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].mass_civ), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].luminosity), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].radio_luminosity), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].xray_luminosity), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].redshift), sizeof(double), 1, pFile ) )
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
			m_nAllocated += VECT_BLACKHOLES_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_vectData = (DefCatBlackhole*) realloc( m_vectData, (m_nAllocated+1)*sizeof(DefCatBlackhole) );
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
			strLog.Printf( wxT("DEBUG :: Blackholes %s catalog(%d objects) was loaded in %lu.%lu seconds"), m_strName, 
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
// Class:	CSkyCatalogBlackholes
// Purpose:	export a blackhole sources catalog as binary
// Input:	reference vector,size, and file to export to
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyCatalogBlackholes::ExportBinary( DefCatBlackhole* vectCatalog,
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
		// :: name - 23 chars
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
		// :: blackhole properties
		if( !fwrite( (void*) &(vectCatalog[i].target_type), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].mass), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].mass_hbeta), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].mass_mgii), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].mass_civ), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].luminosity), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].radio_luminosity), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].xray_luminosity), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].redshift), sizeof(double), 1, pFile ) )
			break;
	}

	fclose( pFile );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
void ResetObjCatBlackhole( DefCatBlackhole& src )
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

	src.target_type = 0;
	
	src.mass = 0.0;
	src.mass_civ = 0.0;
	src.mass_hbeta = 0.0;
	src.mass_mgii = 0.0;
	src.luminosity = 0;
	src.radio_luminosity = 0;
	src.xray_luminosity = 0;

	src.redshift = 0.0;

	src.cat_type=0;
	src.type=0;
};
