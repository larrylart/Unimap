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
#include "sky.h"
#include "../unimap.h"
#include "../unimap_worker.h"
#include "../util/func.h"
#include "../config/mainconfig.h"
#include "unimapsite.h"
// catalogs
#include "catalog_radio_specfind.h"
#include "catalog_radio_vlss.h"
#include "catalog_radio_nvss.h"
#include "catalog_radio_msl.h"

// main header
#include "catalog_radio.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogRadio
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogRadio::CSkyCatalogRadio( CSky* pSky ) : CSkyCatalog( pSky )
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
// Class:	CSkyCatalogRadio
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogRadio::~CSkyCatalogRadio()
{
	// remove :: catalogs
	UnloadCatalog( );

	// messier like this ?
//	free( m_vectMessier );
//	m_nMessier = 0;
}

/////////////////////////////////////////////////////////////////////
// Method:	UnloadCatalog
// Class:	CSkyCatalogRadio
// Purpose:	unload DSO catalog
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSkyCatalogRadio::UnloadCatalog( )
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
// Class:	CSkyCatalogRadio
// Purpose:	sort stars in order of magnitude
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CSkyCatalogRadio::SortByMagnitude( )
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
long CSkyCatalogRadio::SortByA( )
{
	std::sort( m_vectRadio, m_vectRadio+m_nRadio, OnSortCatDsoByA );

	// return status ...
	return( m_nDso );
}
*/

/////////////////////////////////////////////////////////////////////
// Method:	GetRaDec
// Class:	CSkyCatalogRadio
// Purpose:	get ngc ra/dec by cat no
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogRadio::GetRaDec( unsigned long nCatNo, double& nRa, double& nDec )
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
// Class:	CSkyCatalogRadio
// Purpose:	Get a radio source catalog name
// Input:	source, pointer to string to return
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogRadio::GetName( DefCatBasicRadio& radioSource, wxString& strRadioCatName )
{
	int k = 0;
//	char strCommonName[255];
	wxString strName;
	// vector of lavbel fields used to construct the label
	wxString vectLabelField[6];
	int nLabelField=0;
	int bObjLabelUseCatNo=0, bObjLabelUseCName=0, bObjLabelUseType=0;

	/////////////
	// set fields by object type
	if( radioSource.type == SKY_OBJECT_TYPE_RADIO )
	{
		bObjLabelUseCatNo = m_pSky->m_pConfig->m_bRadObjLabelUseCatNo;
//		bObjLabelUseCName = m_pSky->m_pConfig->m_bRadObjLabelUseCName;
		bObjLabelUseType = m_pSky->m_pConfig->m_bRadObjLabelUseType;

	}

	if( radioSource.cat_type == CAT_OBJECT_TYPE_SPECFIND )
	{
		// if to use cat no
		if( bObjLabelUseCatNo )
		{
			if( radioSource.cat_name )
				vectLabelField[nLabelField++].Printf( wxT("%hs"), radioSource.cat_name );
			else
				vectLabelField[nLabelField++].Printf( wxT("SPECFIND %d"), radioSource.cat_no );
		}

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%dMHz"), (unsigned long) radioSource.frequency );

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%.4lfmJy"), radioSource.flux_density );

		// if to use common name
//			if( bDSOObjLabelUseCName && GetRadioCommonName( strCommonName, radioSource.cat_no, CAT_OBJECT_TYPE_SPECFIND ) )
//				sprintf( vectLabelField[nLabelField++], "%s", strCommonName );

	} else if( radioSource.cat_type == CAT_OBJECT_TYPE_VLSS )
	{
		// if to use cat no
		if( bObjLabelUseCatNo )
		{
			if( radioSource.cat_name )
				vectLabelField[nLabelField++].Printf( wxT("VLSS%hs"), radioSource.cat_name );
			else
				vectLabelField[nLabelField++].Printf( wxT("VLSS%d"), radioSource.cat_no );
		}

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%.4lfJy"), radioSource.i_flux_density );

		// if to use common name
//		if( bDSOObjLabelUseCName && GetRadioCommonName( strCommonName, radioSource.cat_no, CAT_OBJECT_TYPE_VLSS ) )
//			sprintf( vectLabelField[nLabelField++], "%s", strCommonName );

	} else if( radioSource.cat_type == CAT_OBJECT_TYPE_NVSS )
	{
		// if to use cat no
		if( bObjLabelUseCatNo )
		{
			if( radioSource.cat_name )
				vectLabelField[nLabelField++].Printf( wxT("NVSS%hs"), radioSource.cat_name );
			else
				vectLabelField[nLabelField++].Printf( wxT("NVSS%d"), radioSource.cat_no );
		}

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%.4lfJy"), radioSource.i_flux_density );

		// if to use common name
//		if( bDSOObjLabelUseCName && GetRadioCommonName( strCommonName, radioSource.cat_no, CAT_OBJECT_TYPE_VLSS ) )
//			sprintf( vectLabelField[nLabelField++], "%s", strCommonName );

	} else if( radioSource.cat_type == CAT_OBJECT_TYPE_MSL )
	{
		// if to use cat no
		if( bObjLabelUseCatNo )
		{
			if( radioSource.cat_name )
				vectLabelField[nLabelField++].Printf( wxT("%hs"), radioSource.cat_name );
			else
				vectLabelField[nLabelField++].Printf( wxT("MSL %d"), radioSource.cat_no );
		}

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%dMHz"), (unsigned long) radioSource.frequency );

		//todo: add here option config check
		vectLabelField[nLabelField++].Printf( wxT("%.4lfJy"), radioSource.flux_density );

		// if to use common name
//		if( bDSOObjLabelUseCName && GetRadioCommonName( strCommonName, radioSource.cat_no, CAT_OBJECT_TYPE_VLSS ) )
//			sprintf( vectLabelField[nLabelField++], "%s", strCommonName );

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
		strRadioCatName = strName;
	} else
	{
		//strRadioCatName = wxT();
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	DownloadCatalog
// Class:	CSkyCatalogRadio
// Purpose:	download a star catalog from a remote location
// Input:	catalog number
// Output:	status 0=failure
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogRadio::DownloadCatalog( int nCatId, int nCatSource )
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
		if( nCatId == CATALOG_ID_SPECFIND )
		{
			CSkyCatalogRadioSpecfind rCat( this, m_pUnimapWorker );
			nCount = rCat.Load( CATALOG_LOAD_REMOTE, nCatSource );

			// if data present - export
			if( nCount > 0 )
			{
				// export binary - add an extra string for now - to test without breaking the existant
				//m_strFile += wxT( "_" );
				if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Save catalog in binary format ...") );
				rCat.ExportBinary( m_vectData, m_nData );
			}

		} else if( nCatId == CATALOG_ID_VLSS )
		{
			CSkyCatalogRadioVlss rCat( this, m_pUnimapWorker );
			nCount = rCat.Load( CATALOG_LOAD_REMOTE, nCatSource );

			// if data present - export
			if( nCount > 0 )
			{
				// export binary - add an extra string for now - to test without breaking the existant
				//m_strFile += wxT( "_" );
				if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Save catalog in binary format ...") );
				rCat.ExportBinary( m_vectData, m_nData );
			}

		} else if( nCatId == CATALOG_ID_NVSS )
		{
			CSkyCatalogRadioNvss rCat( this, m_pUnimapWorker );
			nCount = rCat.Load( CATALOG_LOAD_REMOTE, nCatSource );

			// if data present - export
			if( nCount > 0 )
			{
				// export binary - add an extra string for now - to test without breaking the existant
				//m_strFile += wxT( "_" );
				if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Save catalog in binary format ...") );
				rCat.ExportBinary( m_vectData, m_nData );
			}

		} else if( nCatId == CATALOG_ID_MSL )
		{
			CSkyCatalogRadioMsl rCat( this, m_pUnimapWorker );
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
// Class:	CSkyCatalogRadio
// Purpose:	simple load based on local flags(ie prev load)
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogRadio::Load( int nType, int bRegion, double nCenterRa, 
									double nCenterDec, double nRadius )
{
	unsigned long nCount=0;

	// check load type
	int nLoadType = CATALOG_LOAD_FILE_BINARY;
	if( m_pSky->m_pConfig->m_bRadioCatLoadRemote ) nLoadType = CATALOG_LOAD_REMOTE;
	int nLoadSource = m_pSky->m_pConfig->m_nRadioCatLoadRemoteLocation;

	if( nType == CATALOG_ID_SPECFIND )
	{
		// create SPECFIND object
		CSkyCatalogRadioSpecfind oCat( this, m_pUnimapWorker );
		nCount = oCat.Load( nLoadType, nLoadSource, 0, bRegion, nCenterRa, nCenterDec, nRadius );

		// load catalog maps
		//m_pSky->m_pCatalogDsoAssoc->LoadPGCCatMaps( m_pUnimapWorker );

	} else if( nType == CATALOG_ID_VLSS )
	{
		// create SPECFIND object
		CSkyCatalogRadioVlss oCat( this, m_pUnimapWorker );
		nCount = oCat.Load( nLoadType, nLoadSource, 0, bRegion, nCenterRa, nCenterDec, nRadius );

		// load catalog maps
		//m_pSky->m_pCatalogDsoAssoc->LoadPGCCatMaps( m_pUnimapWorker );

	} else if( nType == CATALOG_ID_NVSS )
	{
		// create SPECFIND object
		CSkyCatalogRadioNvss oCat( this, m_pUnimapWorker );
		nCount = oCat.Load( nLoadType, nLoadSource, 0, bRegion, nCenterRa, nCenterDec, nRadius );

		// load catalog maps
		//m_pSky->m_pCatalogDsoAssoc->LoadPGCCatMaps( m_pUnimapWorker );

	} else if( nType == CATALOG_ID_MSL )
	{
		// create SPECFIND object
		CSkyCatalogRadioMsl oCat( this, m_pUnimapWorker );
		nCount = oCat.Load( nLoadType, nLoadSource, 0, bRegion, nCenterRa, nCenterDec, nRadius );

		// load catalog maps
		//m_pSky->m_pCatalogDsoAssoc->LoadPGCCatMaps( m_pUnimapWorker );

	}

	return( nCount );
}

/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogRadio::LoadBinary( double nCenterRa, double nCenterDec, 
										double nRadius, int bRegion )
{
	return( LoadBinary( m_strFile, m_nLastRegionLoadedCenterRa,
			m_nLastRegionLoadedCenterDec, m_nLastRegionLoadedWidth, m_bLastLoadRegion ) );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadBinary
// Class:	CSkyCatalogRadio
// Purpose:	load the binary version of a radio source based catalog
// Input:	if to load region or all
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogRadio::LoadBinary( const wxString& strFile, double nCenterRa, double nCenterDec, 
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
		strLog.Printf( wxT("INFO :: Loading Radio Sources %s catalog ..."), m_strName );
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
			m_vectData = (DefCatBasicRadio*) malloc( (m_nAllocated+1)*sizeof(DefCatBasicRadio) );

	} else
	{
		// now go at the end of the file and see the size
		fseek( pFile, 0, SEEK_END );
		// get the file size and calculate how many elements i have
		int nRecords = (int) ( ftell( pFile )/( sizeof(unsigned long)+5*sizeof(double) ));
		// allocate my vector + 1 unit just to be sure
		m_vectData = (DefCatBasicRadio*) malloc( (nRecords+1)*sizeof(DefCatBasicRadio) );
		// se allocated
		m_nAllocated = nRecords+1;
	}
	// go at the begining of the file
	fseek( pFile, 0, SEEK_SET );

	// now write all stars info in binary format
	while( !feof( pFile ) )
	{
		// init some
		m_vectData[m_nData].cat_type = CAT_OBJECT_TYPE_RADIO;
		m_vectData[m_nData].type = SKY_OBJECT_TYPE_RADIO;

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
		// :: radio sources properties
		if( !fread( (void*) &(m_vectData[m_nData].no_spec_points), sizeof(unsigned int), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].frequency), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].flux_density), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].i_flux_density), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].spec_abscissa), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectData[m_nData].spec_slope), sizeof(double), 1, pFile ) )
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
			m_nAllocated += VECT_RADIO_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_vectData = (DefCatBasicRadio*) realloc( m_vectData, (m_nAllocated+1)*sizeof(DefCatBasicRadio) );
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
			strLog.Printf( wxT("DEBUG :: DSO %s catalog(%d objects) was loaded in %lu.%lu seconds"), m_strName, 
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
// Class:	CSkyCatalogRadio
// Purpose:	export a radio sources catalog as binary
// Input:	reference vector,size, and file to export to
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyCatalogRadio::ExportBinary( DefCatBasicRadio* vectCatalog,
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
		// :: radio sources properties
		if( !fwrite( (void*) &(vectCatalog[i].no_spec_points), sizeof(unsigned int), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].frequency), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].flux_density), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].i_flux_density), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].spec_abscissa), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].spec_slope), sizeof(double), 1, pFile ) )
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
