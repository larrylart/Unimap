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
#include "catalog_asteroids_omp.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogAsteroidsOmp
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogAsteroidsOmp::CSkyCatalogAsteroidsOmp( CSkyCatalogAsteroids* pCatalogAsteroids, CUnimapWorker* pWorker ) 
{
	m_pCatalogAsteroids = pCatalogAsteroids;
	m_pUnimapWorker = pWorker;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyCatalogAsteroidsOmp
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogAsteroidsOmp::~CSkyCatalogAsteroidsOmp()
{
	
}

/////////////////////////////////////////////////////////////////////
// Method:	Load
// Class:	CSkyCatalogAsteroidsOmp
// Purpose:	load catalog
// Input:	cat type, source, limit obj, region, pos, size
// Output:	no of objects loaded
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogAsteroidsOmp::Load( int nLoadType, int nLoadSource, int nLoadLimit, 
									  int bRegion, double nCenterRa, double nCenterDec, double nRadius )
{
	// check catalog loaded
	if( m_pCatalogAsteroids->m_vectData != NULL )
	{
		if( m_pCatalogAsteroids->m_nCatalogLoaded != CATALOG_ID_OMP )
			m_pCatalogAsteroids->UnloadCatalog();
		else
			return( 0 );
	}

	unsigned long nObj = 0;
	
	// check if to load remote or local
	if( nLoadType == CATALOG_LOAD_FILE_BINARY )
	{
		// load catalog/region from file - use custom method
		nObj = LoadBinary( m_pCatalogAsteroids->m_strFile, nCenterRa, nCenterDec, nRadius, bRegion );

	} else if( nLoadType == CATALOG_LOAD_REMOTE )
	{
		if( nLoadSource == CATALOG_QUERY_SOURCE_VIZIER )
		{
			CVizier oRemote( m_pCatalogAsteroids, m_pUnimapWorker );
			nObj = oRemote.GetAsteroidsFromVizier( CATALOG_ID_OMP, bRegion, nCenterRa, nCenterDec, nRadius, nRadius );
		}
	}

	return( nObj );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadBinary
// Class:	CSkyCatalogAsteroidsOmp
// Purpose:	load the binary version of the catalog
// Input:	if to load region or all
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogAsteroidsOmp::LoadBinary( const wxString& strFile, double nCenterRa, double nCenterDec, 
										double nRadius, int bRegion )
{
	FILE* pFile = NULL;
	double nTimeRef=0;
	wxString strLog;
	double nDistDeg = 0;

	m_pCatalogAsteroids->m_nData = 0;
	// info
	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("INFO :: Loading Asteroids %s catalog ..."), m_pCatalogAsteroids->m_strName );
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
	if( m_pCatalogAsteroids->m_vectData != NULL ) m_pCatalogAsteroids->UnloadCatalog( );

	// now check if region allocate a small block only else allocate full
	if( bRegion )
	{
			m_pCatalogAsteroids->m_nAllocated = 1000;
			// allocate my vector + 1 unit just to be sure
			m_pCatalogAsteroids->m_vectData = (DefCatAsteroid*) malloc( (m_pCatalogAsteroids->m_nAllocated+1)*sizeof(DefCatAsteroid) );

	} else
	{
		// now go at the end of the file and see the size
		fseek( pFile, 0, SEEK_END );
		// get the file size and calculate how many elements i have
		int nRecords = (int) ( ftell( pFile )/( sizeof(unsigned long)+5*sizeof(double) ));
		// allocate my vector + 1 unit just to be sure
		m_pCatalogAsteroids->m_vectData = (DefCatAsteroid*) malloc( (nRecords+1)*sizeof(DefCatAsteroid) );
		// se allocated
		m_pCatalogAsteroids->m_nAllocated = nRecords+1;
	}
	// go at the begining of the file
	fseek( pFile, 0, SEEK_SET );

	// now write all stars info in binary format
	while( !feof( pFile ) )
	{
		ResetObjCatAsteroid( m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData] );

		// init some
		m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].cat_type = CAT_OBJECT_TYPE_OMP;
		m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].type = SKY_OBJECT_TYPE_ASTEROID;

		// :: name - 19 chars
		if( !fread( (void*) &(m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].cat_name), sizeof(char), 19, pFile ) )
			break;

		// :: catalog number 
		if( !fread( (void*) &(m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: asteroid properties
		if( !fread( (void*) &(m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].ast_no), sizeof(unsigned long), 1, pFile ) )
			break;

		if( !fread( (void*) &(m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].comp_name), sizeof(char), 17, pFile ) )
			break;

		if( !fread( (void*) &(m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].abs_mag_h), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].slope_mag), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].color_index), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].iras_diam), sizeof(double), 1, pFile ) )
			break;

		if( !fread( (void*) &(m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].iras_class), sizeof(char), 5, pFile ) )
			break;

		if( !fread( (void*) &(m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].no_arc), sizeof(unsigned long), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].no_obs), sizeof(unsigned int), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].epoch), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].mean_anomaly), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].arg_perihelion), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].lon_asc_node), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].date_orb_comp), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].update_date), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].inclination), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].eccentricity), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].smaj_axis), sizeof(double), 1, pFile ) )
			break;

		// check if flag for region set
		if( bRegion )
		{
			// calculate distance from center to current object
			nDistDeg = CalcSkyDistance( m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].ra, 
										m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].dec, 
											nCenterRa, nCenterDec );
			// check if out continue
			if( nDistDeg > nRadius ) continue;

		}

		// increment counter
		m_pCatalogAsteroids->m_nData++;

		// check if I need to reallocate
		if( m_pCatalogAsteroids->m_nData >= m_pCatalogAsteroids->m_nAllocated )
		{
			// incremen counter
			m_pCatalogAsteroids->m_nAllocated += VECT_ASTEROIDS_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogAsteroids->m_vectData = (DefCatAsteroid*) realloc( m_pCatalogAsteroids->m_vectData, 
											(m_pCatalogAsteroids->m_nAllocated+1)*sizeof(DefCatAsteroid) );
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
			strLog.Printf( wxT("DEBUG :: Asteroids %s catalog(%d objects) was loaded in %lu.%lu seconds"), m_pCatalogAsteroids->m_strName, 
						m_pCatalogAsteroids->m_nData, (unsigned long) nTimeRef/1000, (unsigned long) nTimeRef%1000 );
			m_pUnimapWorker->Log( strLog );
		}
	}

	// if load ok set for last to remember - for optimization
	if( m_pCatalogAsteroids->m_nData > 0 )
	{
		m_pCatalogAsteroids->m_bLastLoadRegion = bRegion;
		m_pCatalogAsteroids->m_nLastRegionLoadedCenterRa = nCenterRa;
		m_pCatalogAsteroids->m_nLastRegionLoadedCenterDec = nCenterDec;
		m_pCatalogAsteroids->m_nLastRegionLoadedWidth = nRadius;
		m_pCatalogAsteroids->m_nLastRegionLoadedHeight = nRadius;
	}

	return( m_pCatalogAsteroids->m_nData );
}

/////////////////////////////////////////////////////////////////////
// Method:	ExportBinary
// Class:	CSkyCatalogAsteroidsOmp
// Purpose:	export catalog as binary
// Input:	reference vector,size, and file to export to
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyCatalogAsteroidsOmp::ExportBinary( DefCatAsteroid* vectCatalog,
									unsigned int nSize )
{
	FILE* pFile = NULL;
	unsigned long i = 0;

	// open file to write
	pFile = wxFopen( m_pCatalogAsteroids->m_strFile, wxT("wb") );
	if( !pFile )
	{
		return( 0 );
	}
	// now write all stars info in binary format
	for( i=0; i<nSize; i++ )
	{
		// :: name - 19 chars
		if( !fwrite( (void*) &(vectCatalog[i].cat_name), sizeof(char), 19, pFile ) )
			break;
		// :: catalog number
		if( !fwrite( (void*) &(vectCatalog[i].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: asteroids properties
		if( !fwrite( (void*) &(vectCatalog[i].ast_no), sizeof(unsigned long), 1, pFile ) )
			break;

		if( !fwrite( (void*) &(vectCatalog[i].comp_name), sizeof(char), 17, pFile ) )
			break;

		if( !fwrite( (void*) &(vectCatalog[i].abs_mag_h), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].slope_mag), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].color_index), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].iras_diam), sizeof(double), 1, pFile ) )
			break;

		if( !fwrite( (void*) &(vectCatalog[i].iras_class), sizeof(char), 5, pFile ) )
			break;

		if( !fwrite( (void*) &(vectCatalog[i].no_arc), sizeof(unsigned long), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].no_obs), sizeof(unsigned int), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].epoch), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].mean_anomaly), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].arg_perihelion), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].lon_asc_node), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].date_orb_comp), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].update_date), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].inclination), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].eccentricity), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].smaj_axis), sizeof(double), 1, pFile ) )
			break;
	}

	fclose( pFile );

	return( 1 );
}
