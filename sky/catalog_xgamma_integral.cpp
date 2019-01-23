
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

//  wx
#include "wx/wxprec.h"
#include "wx/thread.h"
//precompiled headers
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif 
#include <wx/regex.h>
#include <wx/string.h>

// local headers
#include "sky.h"
#include "../util/func.h"
#include "../unimap_worker.h"
#include "vizier.h"

// main header
#include "catalog_xgamma_integral.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogXGammaIntegral
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogXGammaIntegral::CSkyCatalogXGammaIntegral( CSkyCatalogXGamma* pCatalogXGamma, CUnimapWorker* pWorker ) 
{
	m_pCatalogXGamma = pCatalogXGamma;
	m_pUnimapWorker = pWorker;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyCatalogXGammaIntegral
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogXGammaIntegral::~CSkyCatalogXGammaIntegral()
{
	
}

/////////////////////////////////////////////////////////////////////
// Method:	Load
// Class:	CSkyCatalogXGammaIntegral
// Purpose:	load INTEGRAL radio catalog
// Input:	cat type, source, limit obj, region, pos, size
// Output:	no of objects loaded
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogXGammaIntegral::Load( int nLoadType, int nLoadSource, int nLoadLimit, 
									  int bRegion, double nCenterRa, double nCenterDec, double nRadius )
{
	// check catalog loaded
	if( m_pCatalogXGamma->m_vectData != NULL )
	{
		if( m_pCatalogXGamma->m_nCatalogLoaded != CATALOG_ID_INTEGRAL )
			m_pCatalogXGamma->UnloadCatalog();
		else
			return( 0 );
	}

	unsigned long nObj = 0;
	
	// check if to load remote or local
	if( nLoadType == CATALOG_LOAD_FILE_BINARY )
	{
		// load catalog/region from file - use custom method
		nObj = LoadBinary( m_pCatalogXGamma->m_strFile, nCenterRa, nCenterDec, nRadius, bRegion );

	} else if( nLoadType == CATALOG_LOAD_REMOTE )
	{
		if( nLoadSource == CATALOG_QUERY_SOURCE_VIZIER )
		{
			CVizier oRemote( m_pCatalogXGamma, m_pUnimapWorker );
			nObj = oRemote.GetXGammaFromVizier( CATALOG_ID_INTEGRAL, bRegion, nCenterRa, nCenterDec, nRadius, nRadius );
		}
	}

	return( nObj );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadBinary
// Class:	CSkyCatalogXGammaIntegral
// Purpose:	load the binary version of the INTEGRAL radio sources catalog
// Input:	if to load region or all
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogXGammaIntegral::LoadBinary( const wxString& strFile, double nCenterRa, double nCenterDec, 
										double nRadius, int bRegion )
{
	FILE* pFile = NULL;
	double nTimeRef=0;
	wxString strLog;
	double nDistDeg = 0;

	m_pCatalogXGamma->m_nData = 0;
	// info
	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("INFO :: Loading Gamma Sources %s catalog ..."), m_pCatalogXGamma->m_strName );
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
	if( m_pCatalogXGamma->m_vectData != NULL ) m_pCatalogXGamma->UnloadCatalog( );

	// now check if region allocate a small block only else allocate full
	if( bRegion )
	{
			m_pCatalogXGamma->m_nAllocated = 300;
			// allocate my vector + 1 unit just to be sure
			m_pCatalogXGamma->m_vectData = (DefCatBasicXGamma*) malloc( (m_pCatalogXGamma->m_nAllocated+1)*sizeof(DefCatBasicXGamma) );

	} else
	{
		// now go at the end of the file and see the size
		fseek( pFile, 0, SEEK_END );
		// get the file size and calculate how many elements i have
		int nRecords = (int) ( ftell( pFile )/( sizeof(unsigned long)+5*sizeof(double) ));
		// allocate my vector + 1 unit just to be sure
		m_pCatalogXGamma->m_vectData = (DefCatBasicXGamma*) malloc( (nRecords+1)*sizeof(DefCatBasicXGamma) );
		// se allocated
		m_pCatalogXGamma->m_nAllocated = nRecords+1;
	}
	// go at the begining of the file
	fseek( pFile, 0, SEEK_SET );

	// now write all stars info in binary format
	while( !feof( pFile ) )
	{
		// init some
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_type = CAT_OBJECT_TYPE_INTEGRAL;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].type = SKY_OBJECT_TYPE_XGAMMA;

		// :: name - 22 chars
		if( !fread( (void*) &(m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_name), sizeof(char), 22, pFile ) )
			break;
		// :: catalog number 
		if( !fread( (void*) &(m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: position in the sky
		if( !fread( (void*) &(m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].ra), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].dec), sizeof(double), 1, pFile ) )
			break;
		// :: gamma sources properties
		if( !fread( (void*) &(m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].flux_band_1), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].flux_band_2), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].flux_band_3), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].flux_band_4), sizeof(double), 1, pFile ) )
			break;
		// band counts
		if( !fread( (void*) &(m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].count_band_1), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].count_band_2), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].count_band_3), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].count_band_4), sizeof(double), 1, pFile ) )
			break;
		// source type
		if( !fread( (void*) &(m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].source_type), sizeof(unsigned char), 1, pFile ) )
			break;

		// check if flag for region set
		if( bRegion )
		{
			// calculate distance from center to current object
			nDistDeg = CalcSkyDistance( m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].ra, 
										m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].dec, 
											nCenterRa, nCenterDec );
			// check if out continue
			if( nDistDeg > nRadius ) continue;

		}

		// reset other to zero
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].flux = 0.0;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].flux_band_1 = 0.0;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].flux_band_2 = 0.0;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].flux_band_3 = 0.0;

		// increment counter
		m_pCatalogXGamma->m_nData++;

		// check if I need to reallocate
		if( m_pCatalogXGamma->m_nData >= m_pCatalogXGamma->m_nAllocated )
		{
			// incremen counter
			m_pCatalogXGamma->m_nAllocated += VECT_XGAMMA_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogXGamma->m_vectData = (DefCatBasicXGamma*) realloc( m_pCatalogXGamma->m_vectData, 
											(m_pCatalogXGamma->m_nAllocated+1)*sizeof(DefCatBasicXGamma) );
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
			strLog.Printf( wxT("DEBUG :: Gamma %s catalog(%d objects) was loaded in %lu.%lu seconds"), m_pCatalogXGamma->m_strName, 
						m_pCatalogXGamma->m_nData, (unsigned long) nTimeRef/1000, (unsigned long) nTimeRef%1000 );
			m_pUnimapWorker->Log( strLog );
		}
	}

	// if load ok set for last to remember - for optimization
	if( m_pCatalogXGamma->m_nData > 0 )
	{
		m_pCatalogXGamma->m_bLastLoadRegion = bRegion;
		m_pCatalogXGamma->m_nLastRegionLoadedCenterRa = nCenterRa;
		m_pCatalogXGamma->m_nLastRegionLoadedCenterDec = nCenterDec;
		m_pCatalogXGamma->m_nLastRegionLoadedWidth = nRadius;
		m_pCatalogXGamma->m_nLastRegionLoadedHeight = nRadius;
	}

	return( m_pCatalogXGamma->m_nData );
}

/////////////////////////////////////////////////////////////////////
// Method:	ExportBinary
// Class:	CSkyCatalogXGammaIntegral
// Purpose:	export INTEGRAL radio catalog as binary
// Input:	reference vector,size, and file to export to
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyCatalogXGammaIntegral::ExportBinary( DefCatBasicXGamma* vectCatalog,
									unsigned int nSize )
{
	FILE* pFile = NULL;
	unsigned long i = 0;

	// open file to write
	pFile = wxFopen( m_pCatalogXGamma->m_strFile, wxT("wb") );
	if( !pFile )
	{
		return( 0 );
	}
	// now write all stars info in binary format
	for( i=0; i<nSize; i++ )
	{
		// :: name - 22 chars
		if( !fwrite( (void*) &(vectCatalog[i].cat_name), sizeof(char), 22, pFile ) )
			break;
		// :: catalog number
		if( !fwrite( (void*) &(vectCatalog[i].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: position in the sky
		if( !fwrite( (void*) &(vectCatalog[i].ra), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].dec), sizeof(double), 1, pFile ) )
			break;
		// :: gamma sources properties
		if( !fwrite( (void*) &(vectCatalog[i].flux_band_1), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].flux_band_2), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].flux_band_3), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].flux_band_4), sizeof(double), 1, pFile ) )
			break;
		// band counts
		if( !fwrite( (void*) &(vectCatalog[i].count_band_1), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].count_band_2), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].count_band_3), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].count_band_4), sizeof(double), 1, pFile ) )
			break;
		// source type
		if( !fwrite( (void*) &(vectCatalog[i].source_type), sizeof(unsigned char), 1, pFile ) )
			break;
	}

	fclose( pFile );

	return( 1 );
}
