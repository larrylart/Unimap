
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
#include "../util/folders.h"
#include "../util/func.h"
#include "../config/mainconfig.h"
#include "unimapsite.h"
// catalogs
#include "catalog_dso_pgc.h"
#include "catalog_dso_ugc.h"
#include "catalog_dso_mgc.h"

// main header
#include "catalog_dso.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogDso
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogDso::CSkyCatalogDso( CSky* pSky ) : CSkyCatalog( pSky )
{
	m_pSky = pSky;
	m_pUnimapWorker = NULL;

	// dso vector
	m_vectDso = NULL;
	m_nDso = 0;
	m_nDsoAllocated = 0;

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
// Class:	CSkyCatalogDso
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogDso::~CSkyCatalogDso()
{
	// remove :: catalogs
	UnloadCatalog( );

	// messier like this ?
//	free( m_vectMessier );
//	m_nMessier = 0;
}

/////////////////////////////////////////////////////////////////////
// Method:	UnloadCatalog
// Class:	CSkyCatalogDso
// Purpose:	unload DSO catalog
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSkyCatalogDso::UnloadCatalog( )
{
	if( m_vectDso ) free( m_vectDso );
	m_vectDso = NULL;
	m_nDso = 0;
	m_nDsoAllocated = 0;

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	sort stars  by magnitude
// Class:	CSkyCatalogDso
// Purpose:	sort stars in order of magnitude
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CSkyCatalogDso::SortByMagnitude( )
{
//	unsigned long i = 0;
//	int bOrder = 1;
//	long nCount = 0;

	std::sort( m_vectDso, m_vectDso+m_nDso, OnSortCatDso );

	m_nMaxMag = m_vectDso[m_nDso-1].mag;
	m_nMinMag = m_vectDso[0].mag;
	// return status ...
	return( m_nDso );
}

/////////////////////////////////////////////////////////////////////
long CSkyCatalogDso::SortByA( )
{
	std::sort( m_vectDso, m_vectDso+m_nDso, OnSortCatDsoByA );

	// return status ...
	return( m_nDso );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetRaDec
// Class:	CSkyCatalogDso
// Purpose:	get ngc ra/dec by cat no
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogDso::GetRaDec( unsigned long nCatNo, double& nRa, double& nDec )
{
	int i =0;

	// if catalog loaded return
	if( !m_vectDso ) LoadBinary( m_strFile, 0, 0, 0, 0 );

	// look in the entire catalog for my cat no
	for( i=0; i<m_nDso; i++ )
	{
		if( m_vectDso[i].cat_no == nCatNo )
		{
			nRa = m_vectDso[i].ra;
			nDec = m_vectDso[i].dec;
		}
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	DownloadCatalog
// Class:	CSkyCatalogDso
// Purpose:	download a star catalog from a remote location
// Input:	catalog number
// Output:	status 0=failure
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogDso::DownloadCatalog( int nCatId, int nCatSource )
{
	unsigned long nCount = 0;

	// check by source type - if unimap just call class unimapsite
	if( nCatSource == CATALOG_DOWNLOAD_SOURCE_UNIMAP )
	{
		CUnimapSite rBinaryCat( this, m_pUnimapWorker );
		//m_strFile = wxT( "_unimap_test_" );
		nCount = rBinaryCat.DownloadCatalog( m_strFile, nCatId );

	} else if( nCatSource == CATALOG_DOWNLOAD_SOURCE_VIZIER || 
				nCatSource == CATALOG_DOWNLOAD_SOURCE_LEDA )
	{
		// just to make sure
		UnloadCatalog( );

		//////////////
		// now check by type - catalog id
		if( nCatId == CATALOG_ID_HYPERLEDA )
		{
			CSkyCatalogDsoPgc rCat( this, m_pUnimapWorker );
			nCount = rCat.Load( CATALOG_LOAD_REMOTE, nCatSource );

			// if data present - export
			if( nCount > 0 )
			{
				// export binary - add an extra string for now - to test without breaking the existant
				//m_strFile += wxT( "_" );
				if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Save catalog in binary format ...") );
				ExportBinary( m_vectDso, m_nDso );
			}

		} else if( nCatId == CATALOG_ID_UGC )
		{
			CSkyCatalogDsoUgc rCat( this, m_pUnimapWorker );
			nCount = rCat.Load( CATALOG_LOAD_REMOTE, nCatSource );

			// if data present - export
			if( nCount > 0 )
			{
				// export binary - add an extra string for now - to test without breaking the existant
				//m_strFile += wxT( "_" );
				if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Save catalog in binary format ...") );
				ExportBinary( m_vectDso, m_nDso );
			}

		} else if( nCatId == CATALOG_ID_MGC )
		{
			CSkyCatalogDsoMgc rCat( this, m_pUnimapWorker );
			nCount = rCat.Load( CATALOG_LOAD_REMOTE, nCatSource );

			// if data present - export
			if( nCount > 0 )
			{
				// export binary - add an extra string for now - to test without breaking the existant
				//m_strFile += wxT( "_" );
				if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Save catalog in binary format ...") );
				ExportBinary( m_vectDso, m_nDso );
			}

		}

	}

	return( nCount );
}

/////////////////////////////////////////////////////////////////////
// Method:	Load
// Class:	CSkyCatalogDso
// Purpose:	simple load based on local flags(ie prev load)
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogDso::Load( int nType, int bRegion, double nCenterRa, 
									double nCenterDec, double nRadius )
{
	unsigned long nCount=0;

	// tocheck:  should probably use load profile all over the place ?
	int nCatIndex = m_pSky->GetCatalogIndexById( nType );
	if( nCatIndex < 0 ) return(0);

	SetProfile( m_pSky->m_vectCatalogDef[nCatIndex] );

	// check load type
	int nLoadType = CATALOG_LOAD_FILE_BINARY;
	if( m_pSky->m_pConfig->m_bDsoCatLoadRemote ) nLoadType = CATALOG_LOAD_REMOTE;
	int nLoadSource = m_pSky->m_pConfig->m_nDsoCatLoadRemoteLocation;

	if( nType == CATALOG_ID_HYPERLEDA )
	{
//		strcpy( m_strCatalogFile, DEFAULT_PGC_BINARY_FILENAME );
//		strcpy( m_strCatalogName, "PGC" );
//		m_nDsoObjType = CAT_OBJECT_TYPE_PGC;

		// create PGC object
		CSkyCatalogDsoPgc oCat( this, m_pUnimapWorker );
		nCount = oCat.Load( nLoadType, nLoadSource, 0, bRegion, nCenterRa, nCenterDec, nRadius );

		// load catalog maps
		m_pSky->m_pCatalogDsoAssoc->LoadPGCCatMaps( m_pUnimapWorker );

	} else if( nType == CATALOG_ID_UGC )
	{
		// create UGC object
		CSkyCatalogDsoUgc oCat( this, m_pUnimapWorker );
		nCount = oCat.Load( nLoadType, nLoadSource, 0, bRegion, nCenterRa, nCenterDec, nRadius );

		// load catalog maps
//		m_pSky->m_pCatalogDsoAssoc->LoadUGCCatMaps( m_pUnimapWorker );

	} else if( nType == CATALOG_ID_MGC )
	{
//		strcpy( m_strCatalogFile, DEFAULT_MGC_BINARY_FILENAME );
//		strcpy( m_strCatalogName, "MGC" );
//		m_nDsoObjType = CAT_OBJECT_TYPE_MGC;

		// create MGC object
		CSkyCatalogDsoMgc oCat( this, m_pUnimapWorker );
		nCount = oCat.Load( nLoadType, nLoadSource, 0, bRegion, nCenterRa, nCenterDec, nRadius );

		// load catalog maps
//		m_pSky->m_pCatalogDsoAssoc->LoadMGCCatMaps( m_pUnimapWorker );

	} else if( nType == CATALOG_ID_NGC )
	{
		m_strFile = DEFAULT_NGC_BINARY_FILENAME;
		m_strName = wxT("NGC");
		m_nObjectType = CAT_OBJECT_TYPE_NGC;

		LoadBinary( 0, 0, 0, 0 );

	} else if( nType == CATALOG_ID_IC )
	{
		m_strFile = DEFAULT_IC_BINARY_FILENAME;
		m_strName = wxT("IC");
		m_nObjectType = CAT_OBJECT_TYPE_IC;

		LoadBinary( 0, 0, 0, 0 );

	} else if( nType == CATALOG_ID_MESSIER )
	{
		m_strFile = DEFAULT_MESSIER_BINARY_FILENAME;
		m_strName = wxT("Messier");

		m_nObjectType = CAT_OBJECT_TYPE_MESSIER;

		m_pSky->m_pCatalogDsoAssoc->LoadMessierCatMaps( m_pUnimapWorker );

		LoadBinary( 0, 0, 0, 0 );
	}

	return( nCount );
}

/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogDso::LoadBinary( double nCenterRa, double nCenterDec, 
										double nRadius, int bRegion )
{
	return( LoadBinary( m_strFile, m_nLastRegionLoadedCenterRa,
			m_nLastRegionLoadedCenterDec, m_nLastRegionLoadedWidth, m_bLastLoadRegion ) );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadPGCBinary
// Class:	CSkyCatalogDso
// Purpose:	load the binary version of the PGC galaxy catalog
// Input:	if to load region or all
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogDso::LoadBinary( const wxString& strFile, double nCenterRa, double nCenterDec, 
										double nRadius, int bRegion )
{
	FILE* pFile = NULL;
	double nTimeRef=0;
	wxString strLog=wxT("");
	double nDistDeg = 0;

	m_nDso = 0;
	// info
	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("INFO :: Loading DSO %s catalog ..."), m_strNameAbbreviation );
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
	if( m_vectDso != NULL ) UnloadCatalog( );

	// now check if region allocate a small block only else allocate full
	if( bRegion )
	{
			m_nDsoAllocated = 300;
			// allocate my vector + 1 unit just to be sure
			m_vectDso = (DsoDefCatBasic*) malloc( (m_nDsoAllocated+1)*sizeof(DsoDefCatBasic) );

	} else
	{
		// now go at the end of the file and see the size
		fseek( pFile, 0, SEEK_END );
		// get the file size and calculate how many elements i have
		int nRecords = (int) ( ftell( pFile )/( sizeof(unsigned long)+5*sizeof(double) ));
		// allocate my vector + 1 unit just to be sure
		m_vectDso = (DsoDefCatBasic*) malloc( (nRecords+1)*sizeof(DsoDefCatBasic) );
		// se allocated
		m_nDsoAllocated = nRecords+1;
	}
	// go at the begining of the file
	fseek( pFile, 0, SEEK_SET );

	// now write all stars info in binary format
	while( !feof( pFile ) )
	{
		ResetObjCatDso( m_vectDso[m_nDso] );

		// init some
		m_vectDso[m_nDso].cat_type = CAT_OBJECT_TYPE_DSO;
		m_vectDso[m_nDso].type = SKY_OBJECT_TYPE_DSO;

		if( !fread( (void*) &(m_vectDso[m_nDso].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectDso[m_nDso].ra), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectDso[m_nDso].dec), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectDso[m_nDso].a), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectDso[m_nDso].b), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectDso[m_nDso].pa), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectDso[m_nDso].cat_type), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectDso[m_nDso].type), sizeof(unsigned char), 1, pFile ) )
			break;

		// check if flag for region set
		if( bRegion )
		{
			// calculate distance from center to current object
			nDistDeg = CalcSkyDistance( m_vectDso[m_nDso].ra, m_vectDso[m_nDso].dec, 
								nCenterRa, nCenterDec );
			// check if out continue
			if( nDistDeg > nRadius ) continue;

		}
		// increment counter
		m_nDso++;

		// check if I need to reallocate
		if( m_nDso >= m_nDsoAllocated )
		{
			// incremen counter
			m_nDsoAllocated += VECT_DSO_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_vectDso = (DsoDefCatBasic*) realloc( m_vectDso, (m_nDsoAllocated+1)*sizeof(DsoDefCatBasic) );
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
						m_nDso, (unsigned long) nTimeRef/1000, (unsigned long) nTimeRef%1000 );
			m_pUnimapWorker->Log( strLog );
		}
	}

	// if load ok set for last to remember - for optimization
	if( m_nDso > 0 )
	{
		m_bLastLoadRegion = bRegion;
		m_nLastRegionLoadedCenterRa = nCenterRa;
		m_nLastRegionLoadedCenterDec = nCenterDec;
		m_nLastRegionLoadedWidth = nRadius;
		m_nLastRegionLoadedHeight = nRadius;
	}

	return( m_nDso );
}

/////////////////////////////////////////////////////////////////////
// Method:	ExportBinary
// Class:	CSkyCatalogDso
// Purpose:	export a dso catalog as binary
// Input:	reference vector,size, and file to export to
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyCatalogDso::ExportBinary( DsoDefCatBasic* vectCatalog,
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
		if( !fwrite( (void*) &(vectCatalog[i].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].ra), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].dec), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].a), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].b), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].pa), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].cat_type), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].type), sizeof(unsigned char), 1, pFile ) )
			break;
	}

	fclose( pFile );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
void ResetObjCatDso( DsoDefCatBasic& src )
{
	src.ra = 0.0;
	src.dec = 0.0;
	src.a = 0.0;
	src.b = 0.0;
	src.mag = 0.0;
	src.cat_no = 0;
	src.cat_type = 0;
	src.pa = 0.0;
	src.type = 0;
}