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
#include "exoplaneteusite.h"

// main header
#include "catalog_exoplanets_eu.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogExoplanetsEu
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogExoplanetsEu::CSkyCatalogExoplanetsEu( CSkyCatalogExoplanets* pCatalogExoplanets, CUnimapWorker* pWorker ) 
{
	m_pCatalogExoplanets = pCatalogExoplanets;
	m_pUnimapWorker = pWorker;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyCatalogExoplanetsEu
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogExoplanetsEu::~CSkyCatalogExoplanetsEu()
{
	
}

/////////////////////////////////////////////////////////////////////
// Method:	Load
// Class:	CSkyCatalogExoplanetsEu
// Purpose:	load catalog
// Input:	cat type, source, limit obj, region, pos, size
// Output:	no of objects loaded
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogExoplanetsEu::Load( int nLoadType, int nLoadSource, int nLoadLimit, 
									  int bRegion, double nCenterRa, double nCenterDec, double nRadius )
{
	// check catalog loaded
	if( m_pCatalogExoplanets->m_vectData != NULL )
	{
		if( m_pCatalogExoplanets->m_nCatalogLoaded != CATALOG_ID_EXOPLANET_EU )
			m_pCatalogExoplanets->UnloadCatalog();
		else
			return( 0 );
	}

	unsigned long nObj = 0;
	
	// check if to load remote or local
	if( nLoadType == CATALOG_LOAD_FILE_BINARY )
	{
		// load catalog/region from file - use custom method
		nObj = LoadBinary( m_pCatalogExoplanets->m_strFile, nCenterRa, nCenterDec, nRadius, bRegion );

	} else if( nLoadType == CATALOG_LOAD_REMOTE )
	{
		if( nLoadSource == CATALOG_QUERY_SOURCE_EXOPLANET_EU )
		{
			CExoplanetEuSite oRemote( m_pCatalogExoplanets, m_pUnimapWorker );
			nObj = oRemote.GetObjects( CATALOG_ID_EXOPLANET_EU, bRegion, nCenterRa, nCenterDec, nRadius );
		}
	}

	return( nObj );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadBinary
// Class:	CSkyCatalogExoplanetsEu
// Purpose:	load the binary version of the catalog
// Input:	if to load region or all
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogExoplanetsEu::LoadBinary( const wxString& strFile, double nCenterRa, double nCenterDec, 
										double nRadius, int bRegion )
{
	FILE* pFile = NULL;
	double nTimeRef=0;
	wxString strLog;
	double nDistDeg = 0;
	char strTmp[255];

	m_pCatalogExoplanets->m_nData = 0;
	// info
	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("INFO :: Loading Exo Planets %s catalog ..."), m_pCatalogExoplanets->m_strName );
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
	if( m_pCatalogExoplanets->m_vectData != NULL ) m_pCatalogExoplanets->UnloadCatalog( );

	// now check if region allocate a small block only else allocate full
	if( bRegion )
	{
			m_pCatalogExoplanets->m_nAllocated = VECT_EXOPLANETS_MEM_ALLOC_BLOCK_SIZE;
			// allocate my vector + 1 unit just to be sure
			m_pCatalogExoplanets->m_vectData = (DefCatExoPlanet*) malloc( (m_pCatalogExoplanets->m_nAllocated+1)*sizeof(DefCatExoPlanet) );

	} else
	{
		// now go at the end of the file and see the size
		fseek( pFile, 0, SEEK_END );
		// get the file size and calculate how many elements i have
		int nRecords = (int) ( ftell( pFile )/( sizeof(unsigned long)+5*sizeof(double) ));
		// allocate my vector + 1 unit just to be sure
		m_pCatalogExoplanets->m_vectData = (DefCatExoPlanet*) malloc( (nRecords+1)*sizeof(DefCatExoPlanet) );
		// se allocated
		m_pCatalogExoplanets->m_nAllocated = nRecords+1;
	}
	// go at the begining of the file
	fseek( pFile, 0, SEEK_SET );

	// now write all stars info in binary format
	while( !feof( pFile ) )
	{
		ResetObjCatExoplanet( m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData] );

		// init some
		m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].cat_type = CAT_OBJECT_TYPE_EXOPLANET_EU;
		m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].type = SKY_OBJECT_TYPE_EXOPLANET;

		// :: name - 22 chars
		if( !fread( (void*) &strTmp, sizeof(char), 22, pFile ) )
			break;
		strTmp[22] = '\0';
		wxStrncpy( m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].cat_name, wxString(strTmp,wxConvUTF8), 22 );
		// :: catalog number 
		if( !fread( (void*) &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: position in the sky
		if( !fread( (void*) &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].ra), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].dec), sizeof(double), 1, pFile ) )
			break;
		// :: exo-planets properties
		if( !fread( (void*) &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].mass), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].radius), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].period), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].sm_axis), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].eccentricity), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].inclination), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].ang_dist), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].status), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].discovery_year), sizeof(unsigned int), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].update_date), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].star_mag_v), sizeof(unsigned int), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].star_mag_i), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].star_mag_h), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].star_mag_j), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].star_mag_k), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].star_dist), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].star_feh), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].star_mass), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].star_radius), sizeof(double), 1, pFile ) )
			break;

		// check if flag for region set
		if( bRegion )
		{
			// calculate distance from center to current object
			nDistDeg = CalcSkyDistance( m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].ra, 
										m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].dec, 
											nCenterRa, nCenterDec );
			// check if out continue
			if( nDistDeg > nRadius ) continue;

		}

		// increment counter
		m_pCatalogExoplanets->m_nData++;

		// check if I need to reallocate
		if( m_pCatalogExoplanets->m_nData >= m_pCatalogExoplanets->m_nAllocated )
		{
			// incremen counter
			m_pCatalogExoplanets->m_nAllocated += VECT_EXOPLANETS_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogExoplanets->m_vectData = (DefCatExoPlanet*) realloc( m_pCatalogExoplanets->m_vectData, 
											(m_pCatalogExoplanets->m_nAllocated+1)*sizeof(DefCatExoPlanet) );
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
			strLog.Printf( wxT("DEBUG :: EXO-Planets %s catalog(%d objects) was loaded in %lu.%lu seconds"), m_pCatalogExoplanets->m_strName, 
						m_pCatalogExoplanets->m_nData, (unsigned long) nTimeRef/1000, (unsigned long) nTimeRef%1000 );
			m_pUnimapWorker->Log( strLog );
		}
	}

	// if load ok set for last to remember - for optimization
	if( m_pCatalogExoplanets->m_nData > 0 )
	{
		m_pCatalogExoplanets->m_bLastLoadRegion = bRegion;
		m_pCatalogExoplanets->m_nLastRegionLoadedCenterRa = nCenterRa;
		m_pCatalogExoplanets->m_nLastRegionLoadedCenterDec = nCenterDec;
		m_pCatalogExoplanets->m_nLastRegionLoadedWidth = nRadius;
		m_pCatalogExoplanets->m_nLastRegionLoadedHeight = nRadius;
	}

	return( m_pCatalogExoplanets->m_nData );
}

/////////////////////////////////////////////////////////////////////
// Method:	ExportBinary
// Class:	CSkyCatalogExoplanetsEu
// Purpose:	export catalog as binary
// Input:	reference vector,size, and file to export to
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyCatalogExoplanetsEu::ExportBinary( DefCatExoPlanet* vectCatalog,
									unsigned int nSize )
{
	FILE* pFile = NULL;
	unsigned long i = 0;
	char strTmp[255];
	wxString strWxTmp=wxT("");

	// open file to write
	pFile = wxFopen( m_pCatalogExoplanets->m_strFile, wxT("wb") );
	if( !pFile )
	{
		return( 0 );
	}
	// now write all stars info in binary format
	for( i=0; i<nSize; i++ )
	{
		// :: name - 22 chars
		strWxTmp = vectCatalog[i].cat_name;
		strncpy( strTmp, strWxTmp.ToAscii(), 22 );
		if( !fwrite( (void*) &strTmp, sizeof(char), 22, pFile ) )
			break;
		// :: catalog number
		if( !fwrite( (void*) &(vectCatalog[i].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: position in the sky
		if( !fwrite( (void*) &(vectCatalog[i].ra), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].dec), sizeof(double), 1, pFile ) )
			break;
		// :: exo-planet properties
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
