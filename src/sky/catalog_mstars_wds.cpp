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
#include "catalog_mstars_wds.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogMStarsWds
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogMStarsWds::CSkyCatalogMStarsWds( CSkyCatalogMStars* pCatalogMStars, CUnimapWorker* pWorker ) 
{
	m_pCatalogMStars = pCatalogMStars;
	m_pUnimapWorker = pWorker;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyCatalogMStarsWds
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogMStarsWds::~CSkyCatalogMStarsWds()
{
	
}

/////////////////////////////////////////////////////////////////////
// Method:	Load
// Class:	CSkyCatalogMStarsWds
// Purpose:	load catalog
// Input:	cat type, source, limit obj, region, pos, size
// Output:	no of objects loaded
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogMStarsWds::Load( int nLoadType, int nLoadSource, int nLoadLimit, 
									  int bRegion, double nCenterRa, double nCenterDec, double nRadius )
{
	// check catalog loaded
	if( m_pCatalogMStars->m_vectData != NULL )
	{
		if( m_pCatalogMStars->m_nCatalogLoaded != CATALOG_ID_WDS )
			m_pCatalogMStars->UnloadCatalog();
		else
			return( 0 );
	}

	unsigned long nObj = 0;
	
	// check if to load remote or local
	if( nLoadType == CATALOG_LOAD_FILE_BINARY )
	{
		// load catalog/region from file - use custom method
		nObj = LoadBinary( m_pCatalogMStars->m_strFile, nCenterRa, nCenterDec, nRadius, bRegion );

	} else if( nLoadType == CATALOG_LOAD_REMOTE )
	{
		if( nLoadSource == CATALOG_QUERY_SOURCE_VIZIER )
		{
			CVizier oRemote( m_pCatalogMStars, m_pUnimapWorker );
			nObj = oRemote.GetMStarsFromVizier( CATALOG_ID_WDS, bRegion, nCenterRa, nCenterDec, nRadius, nRadius );
		}
	}

	return( nObj );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadBinary
// Class:	CSkyCatalogMStarsWds
// Purpose:	load the binary version of the catalog
// Input:	if to load region or all
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogMStarsWds::LoadBinary( const wxString& strFile, double nCenterRa, double nCenterDec, 
										double nRadius, int bRegion )
{
	FILE* pFile = NULL;
	double nTimeRef=0;
	wxString strLog;
	double nDistDeg = 0;

	m_pCatalogMStars->m_nData = 0;
	// info
	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("INFO :: Loading Multiple/Double Stars %s catalog ..."), m_pCatalogMStars->m_strName );
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
	if( m_pCatalogMStars->m_vectData != NULL ) m_pCatalogMStars->UnloadCatalog( );

	// now check if region allocate a small block only else allocate full
	if( bRegion )
	{
			m_pCatalogMStars->m_nAllocated = 300;
			// allocate my vector + 1 unit just to be sure
			m_pCatalogMStars->m_vectData = (DefCatMStars*) malloc( (m_pCatalogMStars->m_nAllocated+1)*sizeof(DefCatMStars) );

	} else
	{
		// now go at the end of the file and see the size
		fseek( pFile, 0, SEEK_END );
		// get the file size and calculate how many elements i have
		int nRecords = (int) ( ftell( pFile )/( sizeof(unsigned long)+5*sizeof(double) ));
		// allocate my vector + 1 unit just to be sure
		m_pCatalogMStars->m_vectData = (DefCatMStars*) malloc( (nRecords+1)*sizeof(DefCatMStars) );
		// se allocated
		m_pCatalogMStars->m_nAllocated = nRecords+1;
	}
	// go at the begining of the file
	fseek( pFile, 0, SEEK_SET );

	// now write all stars info in binary format
	while( !feof( pFile ) )
	{
		ResetObjCatMStars( m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData] );

		// init some
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].cat_type = CAT_OBJECT_TYPE_WDS;
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].type = SKY_OBJECT_TYPE_MSTARS;

		// :: name - 22 chars
		if( !fread( (void*) &(m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].cat_name), sizeof(char), 11, pFile ) )
			break;

		// :: catalog number 
		if( !fread( (void*) &(m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: position in the sky
		if( !fread( (void*) &(m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].ra), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].dec), sizeof(double), 1, pFile ) )
			break;

		// :: mstars properties
		if( !fread( (void*) &(m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].discoverer), sizeof(char), 8, pFile ) )
			break;

		if( !fread( (void*) &(m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].comp), sizeof(char), 6, pFile ) )
			break;

		if( !fread( (void*) &(m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].nobs), sizeof(unsigned int), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].obs_date), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].obs_date2), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].pos_ang), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].pos_ang2), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].sep), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].sep2), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].mag), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].mag2), sizeof(double), 1, pFile ) )
			break;

		if( !fread( (void*) &(m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].spectral_type), sizeof(char), 11, pFile ) )
			break;

		if( !fread( (void*) &(m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].pm_ra), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].pm_dec), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].pm_ra2), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].pm_dec2), sizeof(double), 1, pFile ) )
			break;

		if( !fread( (void*) &(m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].notes), sizeof(char), 5, pFile ) )
			break;

		// check if flag for region set
		if( bRegion )
		{
			// calculate distance from center to current object
			nDistDeg = CalcSkyDistance( m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].ra, 
										m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].dec, 
											nCenterRa, nCenterDec );
			// check if out continue
			if( nDistDeg > nRadius ) continue;

		}

		// increment counter
		m_pCatalogMStars->m_nData++;

		// check if I need to reallocate
		if( m_pCatalogMStars->m_nData >= m_pCatalogMStars->m_nAllocated )
		{
			// incremen counter
			m_pCatalogMStars->m_nAllocated += VECT_MSTARS_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogMStars->m_vectData = (DefCatMStars*) realloc( m_pCatalogMStars->m_vectData, 
											(m_pCatalogMStars->m_nAllocated+1)*sizeof(DefCatMStars) );
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
			strLog.Printf( wxT("DEBUG :: Blackholes %s catalog(%d objects) was loaded in %lu.%lu seconds"), m_pCatalogMStars->m_strName, 
						m_pCatalogMStars->m_nData, (unsigned long) nTimeRef/1000, (unsigned long) nTimeRef%1000 );
			m_pUnimapWorker->Log( strLog );
		}
	}

	// if load ok set for last to remember - for optimization
	if( m_pCatalogMStars->m_nData > 0 )
	{
		m_pCatalogMStars->m_bLastLoadRegion = bRegion;
		m_pCatalogMStars->m_nLastRegionLoadedCenterRa = nCenterRa;
		m_pCatalogMStars->m_nLastRegionLoadedCenterDec = nCenterDec;
		m_pCatalogMStars->m_nLastRegionLoadedWidth = nRadius;
		m_pCatalogMStars->m_nLastRegionLoadedHeight = nRadius;
	}

	return( m_pCatalogMStars->m_nData );
}

/////////////////////////////////////////////////////////////////////
// Method:	ExportBinary
// Class:	CSkyCatalogMStarsWds
// Purpose:	export catalog as binary
// Input:	reference vector,size, and file to export to
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyCatalogMStarsWds::ExportBinary( DefCatMStars* vectCatalog,
									unsigned int nSize )
{
	FILE* pFile = NULL;
	unsigned long i = 0;

	// open file to write
	pFile = wxFopen( m_pCatalogMStars->m_strFile, wxT("wb") );
	if( !pFile )
	{
		return( 0 );
	}
	// now write all stars info in binary format
	for( i=0; i<nSize; i++ )
	{
		// :: name - 11 chars
		if( !fwrite( (void*) &(vectCatalog[i].cat_name), sizeof(char), 11, pFile ) )
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
		if( !fwrite( (void*) &(vectCatalog[i].discoverer), sizeof(char), 8, pFile ) )
			break;

		if( !fwrite( (void*) &(vectCatalog[i].comp), sizeof(char), 6, pFile ) )
			break;

		if( !fwrite( (void*) &(vectCatalog[i].nobs), sizeof(unsigned int), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].obs_date), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].obs_date2), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].pos_ang), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].pos_ang2), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].sep), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].sep2), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].mag), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].mag2), sizeof(double), 1, pFile ) )
			break;

		if( !fwrite( (void*) &(vectCatalog[i].spectral_type), sizeof(char), 11, pFile ) )
			break;

		if( !fwrite( (void*) &(vectCatalog[i].pm_ra), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].pm_dec), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].pm_ra2), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].pm_dec2), sizeof(double), 1, pFile ) )
			break;

		if( !fwrite( (void*) &(vectCatalog[i].notes), sizeof(char), 5, pFile ) )
			break;

	}

	fclose( pFile );

	return( 1 );
}
