
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
#include "catalog_xgamma_rosat.h"
#include "catalog_xgamma_batse.h"
#include "catalog_xgamma_integral.h"
#include "catalog_xgamma_bepposax.h"
#include "catalog_xgamma_2xmmi.h"
#include "catalog_xgamma_heao.h"

// main header
#include "catalog_xgamma.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogXGamma
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogXGamma::CSkyCatalogXGamma( CSky* pSky ) : CSkyCatalog( pSky )
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
// Class:	CSkyCatalogXGamma
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogXGamma::~CSkyCatalogXGamma()
{
	// remove :: catalogs
	UnloadCatalog( );

	// messier like this ?
//	free( m_vectMessier );
//	m_nMessier = 0;
}

/////////////////////////////////////////////////////////////////////
// Method:	UnloadCatalog
// Class:	CSkyCatalogXGamma
// Purpose:	unload x-ray/gamma catalog
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSkyCatalogXGamma::UnloadCatalog( )
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
// Class:	CSkyCatalogXGamma
// Purpose:	sort stars in order of magnitude
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CSkyCatalogXGamma::SortByMagnitude( )
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
long CSkyCatalogXGamma::SortByA( )
{
	std::sort( m_vectRadio, m_vectRadio+m_nRadio, OnSortCatDsoByA );

	// return status ...
	return( m_nDso );
}
*/

/////////////////////////////////////////////////////////////////////
// Method:	GetRaDec
// Class:	CSkyCatalogXGamma
// Purpose:	get source ra/dec by cat no
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogXGamma::GetRaDec( unsigned long nCatNo, double& nRa, double& nDec )
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
// Class:	CSkyCatalogXGamma
// Purpose:	Get an xray/gamma source catalog name
// Input:	source, pointer to string to return
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogXGamma::GetName( DefCatBasicXGamma& xgammaSource, wxString& strXGammaCatName )
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
	if( xgammaSource.type == SKY_OBJECT_TYPE_XRAY || 
		xgammaSource.type == SKY_OBJECT_TYPE_XGAMMA )
	{
		bObjLabelUseCatNo = m_pSky->m_pConfig->m_bXRayObjLabelUseCatNo;
//		bObjLabelUseCName = m_pSky->m_pConfig->m_bXRayObjLabelUseCName;
		bObjLabelUseType = m_pSky->m_pConfig->m_bXRayObjLabelUseType;

	} else if( xgammaSource.type == SKY_OBJECT_TYPE_GAMMA )
	{
		bObjLabelUseCatNo = m_pSky->m_pConfig->m_bGammaObjLabelUseCatNo;
//		bObjLabelUseCName = m_pSky->m_pConfig->m_bRadObjLabelUseCName;
		bObjLabelUseType = m_pSky->m_pConfig->m_bGammaObjLabelUseType;

	} else
		return( 0 );

	if( xgammaSource.cat_type == CAT_OBJECT_TYPE_ROSAT )
	{
		// if to use cat no
		if( bObjLabelUseCatNo )
		{
			if( xgammaSource.cat_name )
				vectLabelField[nLabelField++].Printf( wxT("%hs"), xgammaSource.cat_name );
			else
				vectLabelField[nLabelField++].Printf( wxT("ROSAT %d"), xgammaSource.cat_no );
		}

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%.5lf"), xgammaSource.count );

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%.5lf"), xgammaSource.bg_count );

		// if to use common name
//			if( bDSOObjLabelUseCName && GetRadioCommonName( strCommonName, radioSource.cat_no, CAT_OBJECT_TYPE_SPECFIND ) )
//				sprintf( vectLabelField[nLabelField++], "%s", strCommonName );

	} else 	if( xgammaSource.cat_type == CAT_OBJECT_TYPE_BATSE )
	{
		// if to use cat no
		if( bObjLabelUseCatNo )
		{
			if( xgammaSource.cat_name )
				vectLabelField[nLabelField++].Printf( wxT("%hs"), xgammaSource.cat_name );
			else
				vectLabelField[nLabelField++].Printf( wxT("BATSE %d"), xgammaSource.cat_no );
		}

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%.5lf %s"), xgammaSource.flux, wxT("ph/cm2/s") );

		//todo: add here option config check
//		sprintf( vectLabelField[nLabelField++], "%.5lf", xgammaSource.bg_count );

	} else 	if( xgammaSource.cat_type == CAT_OBJECT_TYPE_INTEGRAL )
	{
		// if to use cat no
		if( bObjLabelUseCatNo )
		{
			if( xgammaSource.cat_name )
				vectLabelField[nLabelField++].Printf( wxT("%hs"), xgammaSource.cat_name );
			else
				vectLabelField[nLabelField++].Printf( wxT("INTEGRAL %d"), xgammaSource.cat_no );
		}

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%.4lf %s"), xgammaSource.count_band_1, wxT("ct/s") );

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%hs"), m_vectCatIntegralTypes[xgammaSource.source_type] );

	} else 	if( xgammaSource.cat_type == CAT_OBJECT_TYPE_BEPPOSAX_GRBM )
	{
		// if to use cat no
		if( bObjLabelUseCatNo )
		{
			if( xgammaSource.cat_name )
				vectLabelField[nLabelField++].Printf( wxT("%hs"), xgammaSource.cat_name );
			else
				vectLabelField[nLabelField++].Printf( wxT("BEPPOSAX %d"), xgammaSource.cat_no );
		}

		//todo: add here option config check
		// build time stamp
		unsigned int nHours = (unsigned int) (xgammaSource.start_time/3600);
		unsigned int nMinutes = (unsigned int) ((xgammaSource.start_time-nHours*3600)/60);
		unsigned int nSecs = (unsigned int) ((xgammaSource.start_time-(nHours*3600+nMinutes*60)));
		vectLabelField[nLabelField++].Printf( wxT("%.2d:%.2d:%.2d"), nHours, nMinutes, nSecs );

		//todo: add here option config check
//		sprintf( vectLabelField[nLabelField++], "%.5lf", xgammaSource.bg_count );

	} else 	if( xgammaSource.cat_type == CAT_OBJECT_TYPE_2XMMi )
	{
		// if to use cat no
		if( bObjLabelUseCatNo )
		{
			if( xgammaSource.cat_name )
				vectLabelField[nLabelField++].Printf( wxT("%hs"), xgammaSource.cat_name );
			else
				vectLabelField[nLabelField++].Printf( wxT("2XMMi %d"), xgammaSource.cat_no );
		}

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%.5e %s"), xgammaSource.flux, wxT(" mW/m2") );

		//todo: add here option config check
//		sprintf( vectLabelField[nLabelField++], "%s", m_vectCatIntegralTypes[xgammaSource.source_type] );

	} else 	if( xgammaSource.cat_type == CAT_OBJECT_TYPE_HEAO_A1 )
	{
		// if to use cat no
		if( bObjLabelUseCatNo )
		{
			if( xgammaSource.cat_name )
				vectLabelField[nLabelField++].Printf( wxT("%hs"), xgammaSource.cat_name );
			else
				vectLabelField[nLabelField++].Printf( wxT("HEAO-A1 %d"), xgammaSource.cat_no );
		}

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%.4lf %s"), xgammaSource.flux, wxT("ct/cm2/s") );
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
		strXGammaCatName = strName;
	} else
	{
		//strRadioCatName = wxT("");
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	DownloadCatalog
// Class:	CSkyCatalogXGamma
// Purpose:	download xray/gamma catalog from a remote location
// Input:	catalog number
// Output:	status 0=failure
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogXGamma::DownloadCatalog( int nCatId, int nCatSource )
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
		if( nCatId == CATALOG_ID_ROSAT )
		{
			CSkyCatalogXGammaRosat rCat( this, m_pUnimapWorker );
			nCount = rCat.Load( CATALOG_LOAD_REMOTE, nCatSource );

			// if data present - export
			if( nCount > 0 )
			{
				// export binary - add an extra string for now - to test without breaking the existant
				//m_strFile +=wxT( "_" );
				if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Save catalog in binary format ...") );
				rCat.ExportBinary( m_vectData, m_nData );
			}

		} else if( nCatId == CATALOG_ID_BATSE )
		{
			CSkyCatalogXGammaBatse rCat( this, m_pUnimapWorker );
			nCount = rCat.Load( CATALOG_LOAD_REMOTE, nCatSource );

			// if data present - export
			if( nCount > 0 )
			{
				// export binary - add an extra string for now - to test without breaking the existant
				//m_strFile +=wxT( "_" );
				if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Save catalog in binary format ...") );
				rCat.ExportBinary( m_vectData, m_nData );
			}

		} else if( nCatId == CATALOG_ID_INTEGRAL )
		{
			CSkyCatalogXGammaIntegral rCat( this, m_pUnimapWorker );
			nCount = rCat.Load( CATALOG_LOAD_REMOTE, nCatSource );

			// if data present - export
			if( nCount > 0 )
			{
				// export binary - add an extra string for now - to test without breaking the existant
				//m_strFile +=wxT( "_" );
				if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Save catalog in binary format ...") );
				rCat.ExportBinary( m_vectData, m_nData );
			}

		} else if( nCatId == CATALOG_ID_BEPPOSAX_GRBM )
		{
			CSkyCatalogXGammaBepposax rCat( this, m_pUnimapWorker );
			nCount = rCat.Load( CATALOG_LOAD_REMOTE, nCatSource );

			// if data present - export
			if( nCount > 0 )
			{
				// export binary - add an extra string for now - to test without breaking the existant
				//m_strFile +=wxT( "_" );
				if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Save catalog in binary format ...") );
				rCat.ExportBinary( m_vectData, m_nData );
			}

		} else if( nCatId == CATALOG_ID_2XMMi )
		{
			CSkyCatalogXGamma2XMMi rCat( this, m_pUnimapWorker );
			nCount = rCat.Load( CATALOG_LOAD_REMOTE, nCatSource );

			// if data present - export
			if( nCount > 0 )
			{
				// export binary - add an extra string for now - to test without breaking the existant
				//m_strFile +=wxT( "_" );
				if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Save catalog in binary format ...") );
				rCat.ExportBinary( m_vectData, m_nData );
			}

		} else if( nCatId == CATALOG_ID_HEAO_A1 )
		{
			CSkyCatalogXGammaHeao rCat( this, m_pUnimapWorker );
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
// Class:	CSkyCatalogXGamma
// Purpose:	simple load based on local flags(ie prev load)
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogXGamma::Load( int nType, int bRegion, double nCenterRa, 
									double nCenterDec, double nRadius )
{
	unsigned long nCount=0;

	// check load type
	int nLoadType = CATALOG_LOAD_FILE_BINARY;
	if( m_pSky->m_pConfig->m_bXGammaCatLoadRemote ) nLoadType = CATALOG_LOAD_REMOTE;
	int nLoadSource = m_pSky->m_pConfig->m_nXGammaCatLoadRemoteLocation;

	if( nType == CATALOG_ID_ROSAT )
	{
		// create ROSAT object
		CSkyCatalogXGammaRosat oCat( this, m_pUnimapWorker );
		nCount = oCat.Load( nLoadType, nLoadSource, 0, bRegion, nCenterRa, nCenterDec, nRadius );

		// load catalog maps
		//m_pSky->m_pCatalogDsoAssoc->LoadPGCCatMaps( m_pUnimapWorker );

	} else if( nType == CATALOG_ID_BATSE )
	{
		// create BATSE object
		CSkyCatalogXGammaBatse oCat( this, m_pUnimapWorker );
		nCount = oCat.Load( nLoadType, nLoadSource, 0, bRegion, nCenterRa, nCenterDec, nRadius );

		// load catalog maps
		//m_pSky->m_pCatalogDsoAssoc->LoadPGCCatMaps( m_pUnimapWorker );

	} else if( nType == CATALOG_ID_INTEGRAL )
	{
		// create INTEGRAL object
		CSkyCatalogXGammaIntegral oCat( this, m_pUnimapWorker );
		nCount = oCat.Load( nLoadType, nLoadSource, 0, bRegion, nCenterRa, nCenterDec, nRadius );

		// load catalog maps
		//m_pSky->m_pCatalogDsoAssoc->LoadPGCCatMaps( m_pUnimapWorker );

	} else if( nType == CATALOG_ID_BEPPOSAX_GRBM )
	{
		// create BEPPOSAX/GRBM object
		CSkyCatalogXGammaBepposax oCat( this, m_pUnimapWorker );
		nCount = oCat.Load( nLoadType, nLoadSource, 0, bRegion, nCenterRa, nCenterDec, nRadius );

		// load catalog maps
		//m_pSky->m_pCatalogDsoAssoc->LoadPGCCatMaps( m_pUnimapWorker );

	} else if( nType == CATALOG_ID_2XMMi )
	{
		// create 2XMMi object
		CSkyCatalogXGamma2XMMi oCat( this, m_pUnimapWorker );
		nCount = oCat.Load( nLoadType, nLoadSource, 0, bRegion, nCenterRa, nCenterDec, nRadius );

		// load catalog maps
		//m_pSky->m_pCatalogDsoAssoc->LoadPGCCatMaps( m_pUnimapWorker );

	} else if( nType == CATALOG_ID_HEAO_A1 )
	{
		// create 2XMMi object
		CSkyCatalogXGammaHeao oCat( this, m_pUnimapWorker );
		nCount = oCat.Load( nLoadType, nLoadSource, 0, bRegion, nCenterRa, nCenterDec, nRadius );

		// load catalog maps
		//m_pSky->m_pCatalogDsoAssoc->LoadPGCCatMaps( m_pUnimapWorker );

	}

	return( nCount );
}

/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogXGamma::LoadBinary( double nCenterRa, double nCenterDec, 
										double nRadius, int bRegion )
{
	return( LoadBinary( m_strFile, m_nLastRegionLoadedCenterRa,
			m_nLastRegionLoadedCenterDec, m_nLastRegionLoadedWidth, m_bLastLoadRegion ) );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadBinary
// Class:	CSkyCatalogXGamma
// Purpose:	load the binary version of a x-ray/gamma source based catalog
// Input:	if to load region or all
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogXGamma::LoadBinary( const wxString& strFile, double nCenterRa, double nCenterDec, 
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
		strLog.Printf( wxT("INFO :: Loading X-RAY/GAMMA Sources %s catalog ..."), m_strName );
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
			m_vectData = (DefCatBasicXGamma*) malloc( (m_nAllocated+1)*sizeof(DefCatBasicXGamma) );

	} else
	{
		// now go at the end of the file and see the size
		fseek( pFile, 0, SEEK_END );
		// get the file size and calculate how many elements i have
		int nRecords = (int) ( ftell( pFile )/( sizeof(unsigned long)+5*sizeof(double) ));
		// allocate my vector + 1 unit just to be sure
		m_vectData = (DefCatBasicXGamma*) malloc( (nRecords+1)*sizeof(DefCatBasicXGamma) );
		// se allocated
		m_nAllocated = nRecords+1;
	}
	// go at the begining of the file
	fseek( pFile, 0, SEEK_SET );

	// now write all stars info in binary format
	while( !feof( pFile ) )
	{
		ResetObjCatXGamma( m_vectData[m_nData] );

		// init some
		m_vectData[m_nData].cat_type = CAT_OBJECT_TYPE_XGAMMA;
		m_vectData[m_nData].type = SKY_OBJECT_TYPE_XGAMMA;

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
		// :: xray/gamme sources properties
		if( !fread( (void*) &(m_vectData[m_nData].count), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].bg_count), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].exp_time), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].flux), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].start_time), sizeof(unsigned long), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].duration), sizeof(unsigned long), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].duration), sizeof(unsigned long), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].burst_date_time), sizeof(unsigned long), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].flux_band_1), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].flux_band_2), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].flux_band_3), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].flux_band_4), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].flux_band_5), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].flux_band_6), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].source_type), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].time_det), sizeof(unsigned long), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].interval_no), sizeof(unsigned int), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].fluence), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].peak_flux), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].gamma), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].no_detections), sizeof(unsigned int), 1, pFile ) )
			break;
		// :: object/catalog types
		if( !fread( (void*) &(m_vectData[m_nData].cat_type), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].type), sizeof(unsigned char), 1, pFile ) )
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
			m_nAllocated += VECT_XGAMMA_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_vectData = (DefCatBasicXGamma*) realloc( m_vectData, (m_nAllocated+1)*sizeof(DefCatBasicXGamma) );
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
			strLog.Printf( wxT("DEBUG :: X-Ray/Gamma %s catalog(%d objects) was loaded in %lu.%lu seconds"), m_strName, 
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
// Class:	CSkyCatalogXGamma
// Purpose:	export a xray/gamma sources catalog as binary
// Input:	reference vector,size, and file to export to
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyCatalogXGamma::ExportBinary( DefCatBasicXGamma* vectCatalog,
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
		// :: xray/gamme sources properties
		if( !fwrite( (void*) &(m_vectData[i].count), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(m_vectData[i].bg_count), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(m_vectData[i].exp_time), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(m_vectData[i].flux), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(m_vectData[i].start_time), sizeof(unsigned long), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(m_vectData[i].duration), sizeof(unsigned long), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(m_vectData[i].duration), sizeof(unsigned long), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(m_vectData[i].burst_date_time), sizeof(unsigned long), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(m_vectData[i].flux_band_1), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(m_vectData[i].flux_band_2), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(m_vectData[i].flux_band_3), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(m_vectData[i].flux_band_4), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(m_vectData[i].flux_band_5), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(m_vectData[i].flux_band_6), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(m_vectData[i].source_type), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(m_vectData[i].time_det), sizeof(unsigned long), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(m_vectData[i].interval_no), sizeof(unsigned int), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(m_vectData[i].fluence), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(m_vectData[i].peak_flux), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(m_vectData[i].gamma), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(m_vectData[i].no_detections), sizeof(unsigned int), 1, pFile ) )
			break;
		// :: catalog and object type
		if( !fwrite( (void*) &(vectCatalog[i].cat_type), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].type), sizeof(unsigned char), 1, pFile ) )
			break;
	}

	fclose( pFile );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
void ResetObjCatXGamma( DefCatBasicXGamma& src )
{
	memset( &src, 0, sizeof(DefCatBasicXGamma) );
/*
//#ifdef _DEBUG
	 bzero( src.cat_name, 24 );
//#else
//	bzero( src.cat_name, strlen(src.cat_name) );
//#endif

	src.cat_no=0;
	src.x=0.0;
	src.y=0.0;
	src.ra=0.0;
	src.dec=0.0;
	src.count=0.0; 
	src.bg_count=0.0;
	src.exp_time=0.0;
	src.flux=0.0;
	src.start_time=0;
	src.duration=0;
	src.burst_date_time=0;

	src.flux_band_1=0.0;
	src.flux_band_2=0.0;
	src.flux_band_3=0.0;
	src.flux_band_4=0.0;
	src.flux_band_5=0.0;
	src.flux_band_6=0.0;

	src.count_band_1=0.0;
	src.count_band_2=0.0;
	src.count_band_3=0.0;
	src.count_band_4=0.0;

	src.source_type=0;

	src.time_det=0;
	src.interval_no=0;
	src.fluence=0.0;
	src.peak_flux=0.0;
	src.gamma=0.0;

	src.no_detections=0;

	src.cat_type=0;
	src.type=0;
*/
};
