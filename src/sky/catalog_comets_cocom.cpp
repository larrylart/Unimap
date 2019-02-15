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
#include "catalog_comets_cocom.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogCometsCocom
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogCometsCocom::CSkyCatalogCometsCocom( CSkyCatalogComets* pCatalogComets, CUnimapWorker* pWorker ) 
{
	m_pCatalogComets = pCatalogComets;
	m_pUnimapWorker = pWorker;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyCatalogCometsCocom
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogCometsCocom::~CSkyCatalogCometsCocom()
{
	
}

/////////////////////////////////////////////////////////////////////
// Method:	Load
// Class:	CSkyCatalogCometsCocom
// Purpose:	load catalog
// Input:	cat type, source, limit obj, region, pos, size
// Output:	no of objects loaded
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogCometsCocom::Load( int nLoadType, int nLoadSource, int nLoadLimit, 
									  int bRegion, double nCenterRa, double nCenterDec, double nRadius )
{
	// check catalog loaded
	if( m_pCatalogComets->m_vectData != NULL )
	{
		if( m_pCatalogComets->m_nCatalogLoaded != CATALOG_ID_COCOM )
			m_pCatalogComets->UnloadCatalog();
		else
			return( 0 );
	}

	unsigned long nObj = 0;
	
	// check if to load remote or local
	if( nLoadType == CATALOG_LOAD_FILE_BINARY )
	{
		// load catalog/region from file - use custom method
		nObj = LoadBinary( m_pCatalogComets->m_strFile, nCenterRa, nCenterDec, nRadius, bRegion );

	} else if( nLoadType == CATALOG_LOAD_REMOTE )
	{
		if( nLoadSource == CATALOG_QUERY_SOURCE_VIZIER )
		{
			CVizier oRemote( m_pCatalogComets, m_pUnimapWorker );
			nObj = oRemote.GetCometsFromVizier( CATALOG_ID_COCOM, bRegion, nCenterRa, nCenterDec, nRadius, nRadius );
		}
	}

	return( nObj );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadBinary
// Class:	CSkyCatalogCometsCocom
// Purpose:	load the binary version of the catalog
// Input:	if to load region or all
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogCometsCocom::LoadBinary( const wxString& strFile, double nCenterRa, double nCenterDec, 
										double nRadius, int bRegion )
{
	FILE* pFile = NULL;
	double nTimeRef=0;
	wxString strLog;
	double nDistDeg = 0;
//	char strTmp[255];

	m_pCatalogComets->m_nData = 0;
	// info
	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("INFO :: Loading Comets %s catalog ..."), m_pCatalogComets->m_strName );
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
	if( m_pCatalogComets->m_vectData != NULL ) m_pCatalogComets->UnloadCatalog( );

	// now check if region allocate a small block only else allocate full
	if( bRegion )
	{
			m_pCatalogComets->m_nAllocated = 1000;
			// allocate my vector + 1 unit just to be sure
			m_pCatalogComets->m_vectData = (DefCatComet*) malloc( (m_pCatalogComets->m_nAllocated+1)*sizeof(DefCatComet) );

	} else
	{
		// now go at the end of the file and see the size
		fseek( pFile, 0, SEEK_END );
		// get the file size and calculate how many elements i have
		int nRecords = (int) ( ftell( pFile )/( sizeof(unsigned long)+5*sizeof(double) ));
		// allocate my vector + 1 unit just to be sure
		m_pCatalogComets->m_vectData = (DefCatComet*) malloc( (nRecords+1)*sizeof(DefCatComet) );
		// se allocated
		m_pCatalogComets->m_nAllocated = nRecords+1;
	}
	// go at the begining of the file
	fseek( pFile, 0, SEEK_SET );

	// now write all comets info in binary format
	while( !feof( pFile ) )
	{
		ResetObjCatComet( m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData] );

		// init some
		m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].cat_type = CAT_OBJECT_TYPE_COCOM;
		m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].type = SKY_OBJECT_TYPE_COMET;

		// :: name - 29 chars
		if( !fread( (void*) &(m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].cat_name), sizeof(wxChar), 29, pFile ) )
			break;
//		strTmp[29] = '\0';
//		wxStrncpy( m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].cat_name, wxString(strTmp,wxConvUTF8), 29 );

		// :: catalog number 
		if( !fread( (void*) &(m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: comet properties
		if( !fread( (void*) &(m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].note_update_date), sizeof(double), 1, pFile ) )
			break;

		if( !fread( (void*) &(m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].iau_code), sizeof(wxChar), 13, pFile ) )
			break;
//		strTmp[13] = '\0';
//		wxStrncpy( m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].iau_code, wxString(strTmp,wxConvUTF8), 13 );

		if( !fread( (void*) &(m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].comp_name), sizeof(wxChar), 10, pFile ) )
			break;
//		strTmp[10] = '\0';
//		wxStrncpy( m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].comp_name, wxString(strTmp,wxConvUTF8), 10 );

		if( !fread( (void*) &(m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].epoch_comp), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].rel_effect), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].perihelion_date), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].perihelion_distance), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].arg_perihelion), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].lon_orbital_node), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].inclination), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].eccentricity), sizeof(double), 1, pFile ) )
			break;

		// check if flag for region set
		if( bRegion )
		{
			// calculate distance from center to current object
			nDistDeg = CalcSkyDistance( m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].ra, 
										m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].dec, 
											nCenterRa, nCenterDec );
			// check if out continue
			if( nDistDeg > nRadius ) continue;

		}

		// increment counter
		m_pCatalogComets->m_nData++;

		// check if I need to reallocate
		if( m_pCatalogComets->m_nData >= m_pCatalogComets->m_nAllocated )
		{
			// incremen counter
			m_pCatalogComets->m_nAllocated += VECT_COMETS_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogComets->m_vectData = (DefCatComet*) realloc( m_pCatalogComets->m_vectData, 
											(m_pCatalogComets->m_nAllocated+1)*sizeof(DefCatComet) );
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
			strLog.Printf( wxT("DEBUG :: Comets %s catalog(%d objects) was loaded in %lu.%lu seconds"), m_pCatalogComets->m_strName, 
						m_pCatalogComets->m_nData, (unsigned long) nTimeRef/1000, (unsigned long) nTimeRef%1000 );
			m_pUnimapWorker->Log( strLog );
		}
	}

	// if load ok set for last to remember - for optimization
	if( m_pCatalogComets->m_nData > 0 )
	{
		m_pCatalogComets->m_bLastLoadRegion = bRegion;
		m_pCatalogComets->m_nLastRegionLoadedCenterRa = nCenterRa;
		m_pCatalogComets->m_nLastRegionLoadedCenterDec = nCenterDec;
		m_pCatalogComets->m_nLastRegionLoadedWidth = nRadius;
		m_pCatalogComets->m_nLastRegionLoadedHeight = nRadius;
	}

	return( m_pCatalogComets->m_nData );
}

/////////////////////////////////////////////////////////////////////
// Method:	ExportBinary
// Class:	CSkyCatalogCometsCocom
// Purpose:	export catalog as binary
// Input:	reference vector,size, and file to export to
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyCatalogCometsCocom::ExportBinary( DefCatComet* vectCatalog,
									unsigned int nSize )
{
	FILE* pFile = NULL;
	unsigned long i = 0;
//	char strTmp[255];
//	wxString strWxTmp=wxT("");

	// open file to write
	pFile = wxFopen( m_pCatalogComets->m_strFile, wxT("wb") );
	if( !pFile )
	{
		return( 0 );
	}
	// now write all comets info in binary format
	for( i=0; i<nSize; i++ )
	{
		// :: name - 29 chars
//		strWxTmp = vectCatalog[i].cat_name;
//		strncpy( strTmp, strWxTmp.ToAscii(), 29 );
		if( !fwrite( (void*) &(vectCatalog[i].cat_name), sizeof(wxChar), 29, pFile ) )
			break;

		// :: catalog number
		if( !fwrite( (void*) &(vectCatalog[i].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: comets properties
		if( !fwrite( (void*) &(vectCatalog[i].note_update_date), sizeof(double), 1, pFile ) )
			break;

		//strWxTmp = vectCatalog[i].iau_code;
		//strncpy( strTmp, strWxTmp.ToAscii(), 13 );
		if( !fwrite( (void*) &(vectCatalog[i].iau_code), sizeof(wxChar), 13, pFile ) )
			break;

		//strWxTmp = vectCatalog[i].comp_name;
		//strncpy( strTmp, strWxTmp.ToAscii(), 10 );
		if( !fwrite( (void*) &(vectCatalog[i].comp_name), sizeof(wxChar), 10, pFile ) )
			break;

		if( !fwrite( (void*) &(vectCatalog[i].epoch_comp), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].rel_effect), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].perihelion_date), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].perihelion_distance), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].arg_perihelion), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].lon_orbital_node), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].inclination), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].eccentricity), sizeof(double), 1, pFile ) )
			break;
	}

	fclose( pFile );

	return( 1 );
}
