
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
#include "catalog_supernovas_asc.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogSupernovasAsc
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogSupernovasAsc::CSkyCatalogSupernovasAsc( CSkyCatalogSupernovas* pCatalogSupernovas, CUnimapWorker* pWorker ) 
{
	m_pCatalogSupernovas = pCatalogSupernovas;
	m_pUnimapWorker = pWorker;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyCatalogSupernovasAsc
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogSupernovasAsc::~CSkyCatalogSupernovasAsc()
{
	
}

/////////////////////////////////////////////////////////////////////
// Method:	Load
// Class:	CSkyCatalogSupernovasAsc
// Purpose:	load catalog
// Input:	cat type, source, limit obj, region, pos, size
// Output:	no of objects loaded
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogSupernovasAsc::Load( int nLoadType, int nLoadSource, int nLoadLimit, 
									  int bRegion, double nCenterRa, double nCenterDec, double nRadius )
{
	// check catalog loaded
	if( m_pCatalogSupernovas->m_vectData != NULL )
	{
		if( m_pCatalogSupernovas->m_nCatalogLoaded != CATALOG_ID_ASC )
			m_pCatalogSupernovas->UnloadCatalog();
		else
			return( 0 );
	}

	unsigned long nObj = 0;
	
	// check if to load remote or local
	if( nLoadType == CATALOG_LOAD_FILE_BINARY )
	{
		// load catalog/region from file - use custom method
		nObj = LoadBinary( m_pCatalogSupernovas->m_strFile, nCenterRa, nCenterDec, nRadius, bRegion );

	} else if( nLoadType == CATALOG_LOAD_REMOTE )
	{
		if( nLoadSource == CATALOG_QUERY_SOURCE_VIZIER )
		{
			CVizier oRemote( m_pCatalogSupernovas, m_pUnimapWorker );
			nObj = oRemote.GetSupernovasFromVizier( CATALOG_ID_ASC, bRegion, nCenterRa, nCenterDec, nRadius, nRadius );
		}
	}

	return( nObj );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadBinary
// Class:	CSkyCatalogSupernovasAsc
// Purpose:	load the binary version of the catalog
// Input:	if to load region or all
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogSupernovasAsc::LoadBinary( const wxString& strFile, double nCenterRa, double nCenterDec, 
										double nRadius, int bRegion )
{
	FILE* pFile = NULL;
	double nTimeRef=0;
	wxString strLog;
	double nDistDeg = 0;
	char strTmp[255];

	m_pCatalogSupernovas->m_nData = 0;
	// info
	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("INFO :: Loading Supernova %s catalog ..."), m_pCatalogSupernovas->m_strName );
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
	if( m_pCatalogSupernovas->m_vectData != NULL ) m_pCatalogSupernovas->UnloadCatalog( );

	// now check if region allocate a small block only else allocate full
	if( bRegion )
	{
			m_pCatalogSupernovas->m_nAllocated = 300;
			// allocate my vector + 1 unit just to be sure
			m_pCatalogSupernovas->m_vectData = (DefCatBasicSupernova*) malloc( (m_pCatalogSupernovas->m_nAllocated+1)*sizeof(DefCatBasicSupernova) );

	} else
	{
		// now go at the end of the file and see the size
		fseek( pFile, 0, SEEK_END );
		// get the file size and calculate how many elements i have
		int nRecords = (int) ( ftell( pFile )/( sizeof(unsigned long)+5*sizeof(double) ));
		// allocate my vector + 1 unit just to be sure
		m_pCatalogSupernovas->m_vectData = (DefCatBasicSupernova*) malloc( (nRecords+1)*sizeof(DefCatBasicSupernova) );
		// se allocated
		m_pCatalogSupernovas->m_nAllocated = nRecords+1;
	}
	// go at the begining of the file
	fseek( pFile, 0, SEEK_SET );

	// get pointer reference
	DefCatBasicSupernova*	vectData = m_pCatalogSupernovas->m_vectData;
	unsigned long nData=0;

	// now write all stars info in binary format
	while( !feof( pFile ) )
	{
		nData = m_pCatalogSupernovas->m_nData;

		ResetObjCatSupernovas( m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData] );

		// init some
		m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].cat_type = CAT_OBJECT_TYPE_ASC;
		m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].type = SKY_OBJECT_TYPE_SUPERNOVA;

		// :: name - 22 chars
		if( !fread( (void*) &strTmp, sizeof(char), 9, pFile ) )
			break;
		wxStrncpy( vectData[nData].cat_name, wxString(strTmp,wxConvUTF8), 9 );
		// :: catalog number 
		if( !fread( (void*) &(m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: position in the sky
		if( !fread( (void*) &(m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].ra), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].dec), sizeof(double), 1, pFile ) )
			break;
		// :: supernovas properties
		if( !fread( (void*) &(m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].unconfirmed), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].max_mag), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].gal_center_offset_x), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].gal_center_offset_y), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].date_max), sizeof(double), 1, pFile ) )
			break;
		// :: strings 
		if( !fread( (void*) &strTmp, sizeof(char), 20, pFile ) )
			break;
		wxStrncpy( vectData[nData].parent_galaxy, wxString(strTmp,wxConvUTF8), 20 );
		if( !fread( (void*) &strTmp, sizeof(char), 10, pFile ) )
			break;
		wxStrncpy( vectData[nData].sn_type, wxString(strTmp,wxConvUTF8), 10 );
		if( !fread( (void*) &strTmp, sizeof(char), 50, pFile ) )
			break;
		wxStrncpy( vectData[nData].discoverer, wxString(strTmp,wxConvUTF8), 50 );

		// check if flag for region set
		if( bRegion )
		{
			// calculate distance from center to current object
			nDistDeg = CalcSkyDistance( m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].ra, 
										m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].dec, 
											nCenterRa, nCenterDec );
			// check if out continue
			if( nDistDeg > nRadius ) continue;

		}

		// increment counter
		m_pCatalogSupernovas->m_nData++;

		// check if I need to reallocate
		if( m_pCatalogSupernovas->m_nData >= m_pCatalogSupernovas->m_nAllocated )
		{
			// incremen counter
			m_pCatalogSupernovas->m_nAllocated += VECT_SUPERNOVAS_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogSupernovas->m_vectData = (DefCatBasicSupernova*) realloc( m_pCatalogSupernovas->m_vectData, 
											(m_pCatalogSupernovas->m_nAllocated+1)*sizeof(DefCatBasicSupernova) );
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
			strLog.Printf( wxT("DEBUG :: Supernovas %s catalog(%d objects) was loaded in %lu.%lu seconds"), m_pCatalogSupernovas->m_strName, 
						m_pCatalogSupernovas->m_nData, (unsigned long) nTimeRef/1000, (unsigned long) nTimeRef%1000 );
			m_pUnimapWorker->Log( strLog );
		}
	}

	// if load ok set for last to remember - for optimization
	if( m_pCatalogSupernovas->m_nData > 0 )
	{
		m_pCatalogSupernovas->m_bLastLoadRegion = bRegion;
		m_pCatalogSupernovas->m_nLastRegionLoadedCenterRa = nCenterRa;
		m_pCatalogSupernovas->m_nLastRegionLoadedCenterDec = nCenterDec;
		m_pCatalogSupernovas->m_nLastRegionLoadedWidth = nRadius;
		m_pCatalogSupernovas->m_nLastRegionLoadedHeight = nRadius;
	}

	return( m_pCatalogSupernovas->m_nData );
}

/////////////////////////////////////////////////////////////////////
// Method:	ExportBinary
// Class:	CSkyCatalogSupernovasAsc
// Purpose:	export catalog as binary
// Input:	reference vector,size, and file to export to
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyCatalogSupernovasAsc::ExportBinary( DefCatBasicSupernova* vectCatalog,
									unsigned int nSize )
{
	FILE* pFile = NULL;
	unsigned long i = 0;
	char strTmp[255];
	wxString strWxTmp=wxT("");

	// open file to write
	pFile = wxFopen( m_pCatalogSupernovas->m_strFile, wxT("wb") );
	if( !pFile )
	{
		return( 0 );
	}
	// now write all stars info in binary format
	for( i=0; i<nSize; i++ )
	{
		// :: name - 9 chars
		strWxTmp = vectCatalog[i].cat_name;
		strncpy( strTmp, strWxTmp.ToAscii(), 9 );
		if( !fwrite( (void*) &strTmp, sizeof(char), 9, pFile ) )
			break;

		// :: catalog number
		if( !fwrite( (void*) &(vectCatalog[i].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: position in the sky
		if( !fwrite( (void*) &(vectCatalog[i].ra), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].dec), sizeof(double), 1, pFile ) )
			break;
		// :: supernovas properties
		if( !fwrite( (void*) &(vectCatalog[i].unconfirmed), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].max_mag), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].gal_center_offset_x), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].gal_center_offset_y), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].date_max), sizeof(double), 1, pFile ) )
			break;

		// :: parent galaxy
		strWxTmp = vectCatalog[i].parent_galaxy;
		strncpy( strTmp, strWxTmp.ToAscii(), 20 );
		if( !fwrite( (void*) &strTmp, sizeof(char), 20, pFile ) )
			break;
		// :: supernova type
		strWxTmp = vectCatalog[i].sn_type;
		strncpy( strTmp, strWxTmp.ToAscii(), 10 );
		if( !fwrite( (void*) &strTmp, sizeof(char), 10, pFile ) )
			break;
		// :: discoverer
		strWxTmp = vectCatalog[i].discoverer;
		strncpy( strTmp, strWxTmp.ToAscii(), 50 );
		if( !fwrite( (void*) &strTmp, sizeof(char), 50, pFile ) )
			break;

	}

	fclose( pFile );

	return( 1 );
}
