
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
#include "catalog_radio_nvss.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogRadioNvss
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogRadioNvss::CSkyCatalogRadioNvss( CSkyCatalogRadio* pCatalogRadio, CUnimapWorker* pWorker ) 
{
	m_pCatalogRadio = pCatalogRadio;
	m_pUnimapWorker = pWorker;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyCatalogRadioNvss
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogRadioNvss::~CSkyCatalogRadioNvss()
{
	
}

/////////////////////////////////////////////////////////////////////
// Method:	Load
// Class:	CSkyCatalogRadioNvss
// Purpose:	load VLSS radio catalog
// Input:	cat type, source, limit obj, region, pos, size
// Output:	no of objects loaded
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogRadioNvss::Load( int nLoadType, int nLoadSource, int nLoadLimit, 
									  int bRegion, double nCenterRa, double nCenterDec, double nRadius )
{
	// check catalog loaded
	if( m_pCatalogRadio->m_vectData != NULL )
	{
		if( m_pCatalogRadio->m_nCatalogLoaded != CATALOG_ID_NVSS )
			m_pCatalogRadio->UnloadCatalog();
		else
			return( 0 );
	}

	unsigned long nObj = 0;
	
	// check if to load remote or local
	if( nLoadType == CATALOG_LOAD_FILE_BINARY )
	{
		// load catalog/region from file - use custom method
		nObj = LoadBinary( m_pCatalogRadio->m_strFile, nCenterRa, nCenterDec, nRadius, bRegion );

	} else if( nLoadType == CATALOG_LOAD_REMOTE )
	{
		if( nLoadSource == CATALOG_QUERY_SOURCE_VIZIER )
		{
			CVizier oRemote( m_pCatalogRadio, m_pUnimapWorker );
			nObj = oRemote.GetRadioFromVizier( CATALOG_ID_NVSS, bRegion, nCenterRa, nCenterDec, nRadius, nRadius );
		}
	}

	return( nObj );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadBinary
// Class:	CSkyCatalogRadioNvss
// Purpose:	load the binary version of the NVSS radio sources catalog
// Input:	if to load region or all
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogRadioNvss::LoadBinary( const wxString& strFile, double nCenterRa, double nCenterDec, 
										double nRadius, int bRegion )
{
	FILE* pFile = NULL;
	double nTimeRef=0;
	wxString strLog;
	double nDistDeg = 0;

	m_pCatalogRadio->m_nData = 0;
	// info
	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("INFO :: Loading Radio Sources %s catalog ..."), m_pCatalogRadio->m_strName );
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
	if( m_pCatalogRadio->m_vectData != NULL ) m_pCatalogRadio->UnloadCatalog( );

	// now check if region allocate a small block only else allocate full
	if( bRegion )
	{
			m_pCatalogRadio->m_nAllocated = 300;
			// allocate my vector + 1 unit just to be sure
			m_pCatalogRadio->m_vectData = (DefCatBasicRadio*) malloc( (m_pCatalogRadio->m_nAllocated+1)*sizeof(DefCatBasicRadio) );

	} else
	{
		// now go at the end of the file and see the size
		fseek( pFile, 0, SEEK_END );
		// get the file size and calculate how many elements i have
		int nRecords = (int) ( ftell( pFile )/( 15+sizeof(unsigned long)+3*sizeof(double) ));
		// allocate my vector + 1 unit just to be sure
		m_pCatalogRadio->m_vectData = (DefCatBasicRadio*) malloc( (nRecords+1)*sizeof(DefCatBasicRadio) );
		// se allocated
		m_pCatalogRadio->m_nAllocated = nRecords+1;
	}
	// go at the begining of the file
	fseek( pFile, 0, SEEK_SET );

	// now write all stars info in binary format
	while( !feof( pFile ) )
	{
		memset( &(m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData]), 0, sizeof(DefCatBasicRadio) );

		// init some
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_type = CAT_OBJECT_TYPE_NVSS;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].type = SKY_OBJECT_TYPE_RADIO;
		// hard code frequency to 74Mhz
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].frequency = 1400.0;

		// :: name - 22 chars
		if( !fread( (void*) &(m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_name), sizeof(char), 15, pFile ) )
			break;
		// :: catalog number 
		if( !fread( (void*) &(m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: position in the sky
		if( !fread( (void*) &(m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].ra), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].dec), sizeof(double), 1, pFile ) )
			break;
		// :: radio sources properties
		if( !fread( (void*) &(m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].i_flux_density), sizeof(double), 1, pFile ) )
			break;
		// :: object/catalog types
//		if( !fread( (void*) &(m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_type), sizeof(unsigned char), 1, pFile ) )
//			break;
//		if( !fread( (void*) &(m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].type), sizeof(unsigned char), 1, pFile ) )
//			break;

		// check if flag for region set
		if( bRegion )
		{
			// calculate distance from center to current object
			nDistDeg = CalcSkyDistance( m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].ra, 
										m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].dec, 
											nCenterRa, nCenterDec );
			// check if out continue
			if( nDistDeg > nRadius ) continue;

		}
		// increment counter
		m_pCatalogRadio->m_nData++;

		// check if I need to reallocate
		if( m_pCatalogRadio->m_nData >= m_pCatalogRadio->m_nAllocated )
		{
			// incremen counter
			m_pCatalogRadio->m_nAllocated += VECT_RADIO_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogRadio->m_vectData = (DefCatBasicRadio*) realloc( m_pCatalogRadio->m_vectData, 
											(m_pCatalogRadio->m_nAllocated+1)*sizeof(DefCatBasicRadio) );
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
			strLog.Printf( wxT("DEBUG :: Radio %s catalog(%d objects) was loaded in %lu.%lu seconds"), m_pCatalogRadio->m_strName, 
						m_pCatalogRadio->m_nData, (unsigned long) nTimeRef/1000, (unsigned long) nTimeRef%1000 );
			m_pUnimapWorker->Log( strLog );
		}
	}

	// if load ok set for last to remember - for optimization
	if( m_pCatalogRadio->m_nData > 0 )
	{
		m_pCatalogRadio->m_bLastLoadRegion = bRegion;
		m_pCatalogRadio->m_nLastRegionLoadedCenterRa = nCenterRa;
		m_pCatalogRadio->m_nLastRegionLoadedCenterDec = nCenterDec;
		m_pCatalogRadio->m_nLastRegionLoadedWidth = nRadius;
		m_pCatalogRadio->m_nLastRegionLoadedHeight = nRadius;
	}

	return( m_pCatalogRadio->m_nData );
}

/////////////////////////////////////////////////////////////////////
// Method:	ExportBinary
// Class:	CSkyCatalogRadioNvss
// Purpose:	export a dso catalog as binary
// Input:	reference vector,size, and file to export to
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyCatalogRadioNvss::ExportBinary( DefCatBasicRadio* vectCatalog,
									unsigned int nSize )
{
	FILE* pFile = NULL;
	unsigned long i = 0;

	// open file to write
	pFile = wxFopen( m_pCatalogRadio->m_strFile, wxT("wb") );
	if( !pFile )
	{
		return( 0 );
	}
	// now write all stars info in binary format
	for( i=0; i<nSize; i++ )
	{
		// :: name - 22 chars
		if( !fwrite( (void*) &(vectCatalog[i].cat_name), sizeof(char), 15, pFile ) )
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
		if( !fwrite( (void*) &(vectCatalog[i].i_flux_density), sizeof(double), 1, pFile ) )
			break;
		// :: catalog and object type
//		if( !fwrite( (void*) &(vectCatalog[i].cat_type), sizeof(unsigned char), 1, pFile ) )
//			break;
//		if( !fwrite( (void*) &(vectCatalog[i].type), sizeof(unsigned char), 1, pFile ) )
//			break;
	}

	fclose( pFile );

	return( 1 );
}
