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
#include "celestraksite.h"

// main header
#include "catalog_aes_celestrak.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogAesCelestrak
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogAesCelestrak::CSkyCatalogAesCelestrak( CSkyCatalogAes* pCatalogAes, CUnimapWorker* pWorker ) 
{
	m_pCatalogAes = pCatalogAes;
	m_pUnimapWorker = pWorker;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyCatalogAesCelestrak
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogAesCelestrak::~CSkyCatalogAesCelestrak()
{
	
}

/////////////////////////////////////////////////////////////////////
// Method:	Load
// Class:	CSkyCatalogAesCelestrak
// Purpose:	load catalog
// Input:	cat type, source, limit obj, region, pos, size
// Output:	no of objects loaded
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogAesCelestrak::Load( int nLoadType, int nLoadSource, int nLoadLimit, 
									  int bRegion, double nCenterRa, double nCenterDec, double nRadius )
{
	// check catalog loaded - don't need this ?!!!
/*	if( m_pCatalogAes->m_vectData != NULL )
	{
		if( m_pCatalogAes->m_nCatalogLoaded != CATALOG_ID_CELESTRAK )
			m_pCatalogAes->UnloadCatalog();
		else
			return( 0 );
	}
*/
	unsigned long nObj = 0;
	
	// check if to load remote or local
	if( nLoadType == CATALOG_LOAD_FILE_BINARY )
	{
		// load catalog/region from file - use custom method
		nObj = LoadBinary( m_pCatalogAes->m_strFile, nCenterRa, nCenterDec, nRadius, bRegion );

	} else if( nLoadType == CATALOG_LOAD_REMOTE )
	{
		if( nLoadSource == CATALOG_QUERY_SOURCE_CELESTRAK )
		{
			CCelestrakSite oRemote( m_pCatalogAes, m_pUnimapWorker );
			nObj = oRemote.GetObjects( CATALOG_ID_CELESTRAK );
		}
	}

	return( nObj );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadBinary
// Class:	CSkyCatalogAesCelestrak
// Purpose:	load the binary version of the catalog
// Input:	if to load region or all
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogAesCelestrak::LoadBinary( const wxString& strFile, double nCenterRa, double nCenterDec, 
										double nRadius, int bRegion )
{
	FILE* pFile = NULL;
	double nTimeRef=0;
	wxString strLog;
	double nDistDeg = 0;
	int j=0;

	// info
	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("INFO :: Loading artificial earth satellite %s catalog ..."), m_pCatalogAes->m_strName );
		m_pUnimapWorker->Log( strLog );
	}

	nTimeRef = GetDTime();

	// open file to write
	pFile = wxFopen( strFile, wxT("rb") );
	if( !pFile )
	{
		return( 0 );
	}

	// do the initial allocation - first de-allocate
	if( m_pCatalogAes->m_vectData != NULL ) m_pCatalogAes->UnloadCatalog( );

	// now check if region allocate a small block only else allocate full
	if( bRegion )
	{
			m_pCatalogAes->m_nAllocated = VECT_AES_MEM_ALLOC_BLOCK_SIZE;
			// allocate my vector + 1 unit just to be sure
			m_pCatalogAes->m_vectData = (DefCatAes*) calloc( m_pCatalogAes->m_nAllocated+1, sizeof(DefCatAes) );

	} else
	{
		// now go at the end of the file and see the size
		fseek( pFile, 0, SEEK_END );
		// get the file size and calculate how many elements i have
		int nRecords = (int) ( ftell( pFile )/( sizeof(unsigned long)+5*sizeof(double) ));
		// allocate my vector + 1 unit just to be sure
		m_pCatalogAes->m_vectData = (DefCatAes*) calloc( nRecords+1, sizeof(DefCatAes) );
		// se allocated
		m_pCatalogAes->m_nAllocated = nRecords+1;
	}
	// go at the begining of the file
	fseek( pFile, 0, SEEK_SET );

	// now write all stars info in binary format
	while( !feof( pFile ) )
	{
		ResetObjCatAes( m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData] );

		// init some
		m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].cat_type = CAT_OBJECT_TYPE_CELESTRAK;
		m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].type = SKY_OBJECT_TYPE_AES;

		// :: name - 30 chars
		if( !fread( (void*) &(m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].cat_name), sizeof(char), 30, pFile ) )
			break;
		// :: catalog number 
		if( !fread( (void*) &(m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: position in the sky
		if( !fread( (void*) &(m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].ra), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].dec), sizeof(double), 1, pFile ) )
			break;

		// :: satellite properties
		if( !fread( (void*) &(m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].class_of_orbit), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].type_of_orbit), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].obj_type), sizeof(unsigned char), 1, pFile ) )
			break;

		/////////////////////////
		// read sat type vector
		unsigned char nSatTypeNo=0, nSatType=0;
		if( !fread( (void*) &nSatTypeNo, sizeof(unsigned char), 1, pFile ) )
			break;
		m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].nSatType = nSatTypeNo;
		//m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].nSatTypeAllocated = nSatTypeNo;
		//m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].vectSatType = (unsigned char*) malloc( nSatTypeNo*sizeof(unsigned char) );
		// load all sat types
		for( j=0; j<nSatTypeNo && j<100; j++ )
		{
			if( !fread( (void*) &nSatType, sizeof(unsigned char), 1, pFile ) )
				break;
			m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].vectSatType[j] = nSatType;
		}

		// :: other satellite properties
		if( !fread( (void*) &(m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].norad_cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].security_class), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].int_id_year), sizeof(unsigned int), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].int_id_launch_no), sizeof(unsigned int), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].int_id_launch_piece), sizeof(char), 4, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].ephemeris_type), sizeof(unsigned char), 1, pFile ) )
			break;

		////////////////////////
		// read TLE type
		unsigned int nSatTLENo=0;
		if( !fread( (void*) &nSatTLENo, sizeof(unsigned int), 1, pFile ) )
			break;
		m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].nTLERec = nSatTLENo;
		m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].nTLERecAllocated = nSatTLENo;
		// allocate 
		m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].vectTLERec = (DefTLERecord*) calloc( nSatTLENo, sizeof(DefTLERecord) );
		// read all TLE elements
		for( j=0; j<nSatTLENo; j++ )
		{
			DefTLERecord* tle = &(m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].vectTLERec[j]);

			if( !fread( (void*) &(tle->epoch_time), sizeof(double), 1, pFile ) )
				break;

			if( !fread( (void*) &(tle->orbit_decay_rate), sizeof(double), 1, pFile ) )
				break;
			if( !fread( (void*) &(tle->mean_motion_second_time_deriv), sizeof(double), 1, pFile ) )
				break;

			if( !fread( (void*) &(tle->object_drag_coeff), sizeof(double), 1, pFile ) )
				break;
			if( !fread( (void*) &(tle->element_number), sizeof(unsigned int), 1, pFile ) )
				break;

			if( !fread( (void*) &(tle->inclination), sizeof(double), 1, pFile ) )
				break;
			if( !fread( (void*) &(tle->asc_node_ra), sizeof(double), 1, pFile ) )
				break;
			if( !fread( (void*) &(tle->eccentricity), sizeof(double), 1, pFile ) )
				break;
			if( !fread( (void*) &(tle->arg_of_perigee), sizeof(double), 1, pFile ) )
				break;
			if( !fread( (void*) &(tle->mean_anomaly), sizeof(double), 1, pFile ) )
				break;
			if( !fread( (void*) &(tle->mean_motion), sizeof(double), 1, pFile ) )
				break;
			if( !fread( (void*) &(tle->orb_no_epoch), sizeof(unsigned int), 1, pFile ) )
				break;

			//m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].vectTLERec.push_back( tle );
		}

		// check if flag for region set
		if( bRegion )
		{
			// calculate distance from center to current object
			nDistDeg = CalcSkyDistance( m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].ra, 
										m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].dec, 
											nCenterRa, nCenterDec );
			// check if out continue
			if( nDistDeg > nRadius ) continue;

		}

		// increment counter
		m_pCatalogAes->m_nData++;

		// check if I need to reallocate
		if( m_pCatalogAes->m_nData >= m_pCatalogAes->m_nAllocated )
		{
			// incremen counter
			m_pCatalogAes->m_nAllocated += VECT_AES_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogAes->m_vectData = (DefCatAes*) _recalloc( m_pCatalogAes->m_vectData, 
											m_pCatalogAes->m_nAllocated+1, sizeof(DefCatAes) );
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
			strLog.Printf( wxT("DEBUG :: Artificial Earth Satellites %s catalog(%d objects) was loaded in %lu.%lu seconds"), 
						m_pCatalogAes->m_strName, m_pCatalogAes->m_nData, (unsigned long) nTimeRef/1000, (unsigned long) nTimeRef%1000 );
			m_pUnimapWorker->Log( strLog );
		}
	}

	// if load ok set for last to remember - for optimization
	if( m_pCatalogAes->m_nData > 0 )
	{
		m_pCatalogAes->m_bLastLoadRegion = bRegion;
		m_pCatalogAes->m_nLastRegionLoadedCenterRa = nCenterRa;
		m_pCatalogAes->m_nLastRegionLoadedCenterDec = nCenterDec;
		m_pCatalogAes->m_nLastRegionLoadedWidth = nRadius;
		m_pCatalogAes->m_nLastRegionLoadedHeight = nRadius;
	}

	return( m_pCatalogAes->m_nData );
}

/////////////////////////////////////////////////////////////////////
// Method:	ExportBinary
// Class:	CSkyCatalogAesCelestrak
// Purpose:	export catalog as binary
// Input:	reference vector,size, and file to export to
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyCatalogAesCelestrak::ExportBinary( DefCatAes* vectCatalog,
									unsigned int nSize )
{
	FILE* pFile = NULL;
	unsigned long i = 0;
	int j=0;

	// open file to write
	pFile = wxFopen( m_pCatalogAes->m_strFile, wxT("wb") );
	if( !pFile )
	{
		return( 0 );
	}
	// now write all stars info in binary format
	for( i=0; i<nSize; i++ )
	{
		// :: name - 30 chars
		if( !fwrite( (void*) &(vectCatalog[i].cat_name), sizeof(char), 30, pFile ) )
			break;
		// :: catalog number
		if( !fwrite( (void*) &(vectCatalog[i].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: position in the sky
		if( !fwrite( (void*) &(vectCatalog[i].ra), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].dec), sizeof(double), 1, pFile ) )
			break;

		// :: satellite properties
		if( !fwrite( (void*) &(vectCatalog[i].class_of_orbit), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].type_of_orbit), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].obj_type), sizeof(unsigned char), 1, pFile ) )
			break;

		///////////////////////
		// write sat type vector
		unsigned char nSatTypeNo = (unsigned char) vectCatalog[i].nSatType;
		unsigned char nSatType = 0;
		if( !fwrite( (void*) &nSatTypeNo, sizeof(unsigned char), 1, pFile ) )
			break;
		for( j=0; j<nSatTypeNo; j++ )
		{
			nSatType = vectCatalog[i].vectSatType[j];
			if( !fwrite( (void*) &nSatType, sizeof(unsigned char), 1, pFile ) )
				break;
		}

		// :: other satellite properties
		if( !fwrite( (void*) &(vectCatalog[i].norad_cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].security_class), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].int_id_year), sizeof(unsigned int), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].int_id_launch_no), sizeof(unsigned int), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].int_id_launch_piece), sizeof(char), 4, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].ephemeris_type), sizeof(unsigned char), 1, pFile ) )
			break;

		//////////////////////////////
		// WRITE TLE VECTOR
		// :: first write no of tle elements
		unsigned int nSatTLENo = (unsigned int) vectCatalog[i].nTLERec;
		if( !fwrite( (void*) &nSatTLENo, sizeof(unsigned int), 1, pFile ) )
			break;
		// loop through all tle elements and write them - right one after another
		for( j=0; j<nSatTLENo; j++ )
		{
			if( !fwrite( (void*) &(vectCatalog[i].vectTLERec[j].epoch_time), sizeof(double), 1, pFile ) )
				break;
			if( !fwrite( (void*) &(vectCatalog[i].vectTLERec[j].orbit_decay_rate), sizeof(double), 1, pFile ) )
				break;
			if( !fwrite( (void*) &(vectCatalog[i].vectTLERec[j].mean_motion_second_time_deriv), sizeof(double), 1, pFile ) )
				break;

			if( !fwrite( (void*) &(vectCatalog[i].vectTLERec[j].object_drag_coeff), sizeof(double), 1, pFile ) )
				break;
			if( !fwrite( (void*) &(vectCatalog[i].vectTLERec[j].element_number), sizeof(unsigned int), 1, pFile ) )
				break;

			if( !fwrite( (void*) &(vectCatalog[i].vectTLERec[j].inclination), sizeof(double), 1, pFile ) )
				break;
			if( !fwrite( (void*) &(vectCatalog[i].vectTLERec[j].asc_node_ra), sizeof(double), 1, pFile ) )
				break;
			if( !fwrite( (void*) &(vectCatalog[i].vectTLERec[j].eccentricity), sizeof(double), 1, pFile ) )
				break;
			if( !fwrite( (void*) &(vectCatalog[i].vectTLERec[j].arg_of_perigee), sizeof(double), 1, pFile ) )
				break;
			if( !fwrite( (void*) &(vectCatalog[i].vectTLERec[j].mean_anomaly), sizeof(double), 1, pFile ) )
				break;
			if( !fwrite( (void*) &(vectCatalog[i].vectTLERec[j].mean_motion), sizeof(double), 1, pFile ) )
				break;
			if( !fwrite( (void*) &(vectCatalog[i].vectTLERec[j].orb_no_epoch), sizeof(unsigned int), 1, pFile ) )
				break;
		}
	}

	fclose( pFile );

	return( 1 );
}
