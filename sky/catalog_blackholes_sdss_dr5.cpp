
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
#include "catalog_blackholes_sdss_dr5.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogBlackholesSdssdr5
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogBlackholesSdssdr5::CSkyCatalogBlackholesSdssdr5( CSkyCatalogBlackholes* pCatalogBlackholes, CUnimapWorker* pWorker ) 
{
	m_pCatalogBlackholes = pCatalogBlackholes;
	m_pUnimapWorker = pWorker;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyCatalogBlackholesSdssdr5
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogBlackholesSdssdr5::~CSkyCatalogBlackholesSdssdr5()
{
	
}

/////////////////////////////////////////////////////////////////////
// Method:	Load
// Class:	CSkyCatalogBlackholesSdssdr5
// Purpose:	load catalog
// Input:	cat type, source, limit obj, region, pos, size
// Output:	no of objects loaded
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogBlackholesSdssdr5::Load( int nLoadType, int nLoadSource, int nLoadLimit, 
									  int bRegion, double nCenterRa, double nCenterDec, double nRadius )
{
	// check catalog loaded
	if( m_pCatalogBlackholes->m_vectData != NULL )
	{
		if( m_pCatalogBlackholes->m_nCatalogLoaded != CATALOG_ID_SDSS_DR5 )
			m_pCatalogBlackholes->UnloadCatalog();
		else
			return( 0 );
	}

	unsigned long nObj = 0;
	
	// check if to load remote or local
	if( nLoadType == CATALOG_LOAD_FILE_BINARY )
	{
		// load catalog/region from file - use custom method
		nObj = LoadBinary( m_pCatalogBlackholes->m_strFile, nCenterRa, nCenterDec, nRadius, bRegion );

	} else if( nLoadType == CATALOG_LOAD_REMOTE )
	{
		if( nLoadSource == CATALOG_QUERY_SOURCE_VIZIER )
		{
			CVizier oRemote( m_pCatalogBlackholes, m_pUnimapWorker );
			nObj = oRemote.GetBlackholesFromVizier( CATALOG_ID_SDSS_DR5, bRegion, nCenterRa, nCenterDec, nRadius, nRadius );
		}
	}

	return( nObj );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadBinary
// Class:	CSkyCatalogBlackholesSdssdr5
// Purpose:	load the binary version of the catalog
// Input:	if to load region or all
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogBlackholesSdssdr5::LoadBinary( const wxString& strFile, double nCenterRa, double nCenterDec, 
										double nRadius, int bRegion )
{
	FILE* pFile = NULL;
	double nTimeRef=0;
	wxString strLog;
	double nDistDeg = 0;
	char strTmp[255];

	m_pCatalogBlackholes->m_nData = 0;
	// info
	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("INFO :: Loading Blackholes %s catalog ..."), m_pCatalogBlackholes->m_strName );
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
	if( m_pCatalogBlackholes->m_vectData != NULL ) m_pCatalogBlackholes->UnloadCatalog( );

	// now check if region allocate a small block only else allocate full
	if( bRegion )
	{
			m_pCatalogBlackholes->m_nAllocated = 300;
			// allocate my vector + 1 unit just to be sure
			m_pCatalogBlackholes->m_vectData = (DefCatBlackhole*) malloc( (m_pCatalogBlackholes->m_nAllocated+1)*sizeof(DefCatBlackhole) );

	} else
	{
		// now go at the end of the file and see the size
		fseek( pFile, 0, SEEK_END );
		// get the file size and calculate how many elements i have
		int nRecords = (int) ( ftell( pFile )/( sizeof(unsigned long)+5*sizeof(double) ));
		// allocate my vector + 1 unit just to be sure
		m_pCatalogBlackholes->m_vectData = (DefCatBlackhole*) malloc( (nRecords+1)*sizeof(DefCatBlackhole) );
		// se allocated
		m_pCatalogBlackholes->m_nAllocated = nRecords+1;
	}
	// go at the begining of the file
	fseek( pFile, 0, SEEK_SET );

	// now write all stars info in binary format
	while( !feof( pFile ) )
	{
		ResetObjCatBlackhole( m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData] );

		// init some
		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].cat_type = CAT_OBJECT_TYPE_SDSS_DR5;
		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].type = SKY_OBJECT_TYPE_BLACKHOLE;

		// :: name - 19 chars
		if( !fread( (void*) &strTmp, sizeof(char), 19, pFile ) )
			break;
		wxStrncpy( m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].cat_name, wxString(strTmp,wxConvUTF8), 19 );
		// :: catalog number 
		if( !fread( (void*) &(m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		// :: position in the sky
		if( !fread( (void*) &(m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].ra), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].dec), sizeof(double), 1, pFile ) )
			break;
		// :: supernovas properties
		if( !fread( (void*) &(m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].target_type), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].mass), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].mass_hbeta), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].mass_mgii), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].mass_civ), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].luminosity), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].redshift), sizeof(double), 1, pFile ) )
			break;

		// check if flag for region set
		if( bRegion )
		{
			// calculate distance from center to current object
			nDistDeg = CalcSkyDistance( m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].ra, 
										m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].dec, 
											nCenterRa, nCenterDec );
			// check if out continue
			if( nDistDeg > nRadius ) continue;

		}

		// increment counter
		m_pCatalogBlackholes->m_nData++;

		// check if I need to reallocate
		if( m_pCatalogBlackholes->m_nData >= m_pCatalogBlackholes->m_nAllocated )
		{
			// incremen counter
			m_pCatalogBlackholes->m_nAllocated += VECT_BLACKHOLES_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogBlackholes->m_vectData = (DefCatBlackhole*) realloc( m_pCatalogBlackholes->m_vectData, 
											(m_pCatalogBlackholes->m_nAllocated+1)*sizeof(DefCatBlackhole) );
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
			strLog.Printf( wxT("DEBUG :: Blackholes %s catalog(%d objects) was loaded in %lu.%lu seconds"), m_pCatalogBlackholes->m_strName, 
						m_pCatalogBlackholes->m_nData, (unsigned long) nTimeRef/1000, (unsigned long) nTimeRef%1000 );
			m_pUnimapWorker->Log( strLog );
		}
	}

	// if load ok set for last to remember - for optimization
	if( m_pCatalogBlackholes->m_nData > 0 )
	{
		m_pCatalogBlackholes->m_bLastLoadRegion = bRegion;
		m_pCatalogBlackholes->m_nLastRegionLoadedCenterRa = nCenterRa;
		m_pCatalogBlackholes->m_nLastRegionLoadedCenterDec = nCenterDec;
		m_pCatalogBlackholes->m_nLastRegionLoadedWidth = nRadius;
		m_pCatalogBlackholes->m_nLastRegionLoadedHeight = nRadius;
	}

	return( m_pCatalogBlackholes->m_nData );
}

/////////////////////////////////////////////////////////////////////
// Method:	ExportBinary
// Class:	CSkyCatalogBlackholesSdssdr5
// Purpose:	export catalog as binary
// Input:	reference vector,size, and file to export to
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyCatalogBlackholesSdssdr5::ExportBinary( DefCatBlackhole* vectCatalog,
									unsigned int nSize )
{
	FILE* pFile = NULL;
	unsigned long i = 0;
	char strTmp[255];
	wxString strWxTmp=wxT("");

	// open file to write
	pFile = wxFopen( m_pCatalogBlackholes->m_strFile, wxT("wb") );
	if( !pFile )
	{
		return( 0 );
	}
	// now write all stars info in binary format
	for( i=0; i<nSize; i++ )
	{
		// :: name - 19 chars
		strWxTmp = vectCatalog[i].cat_name;
		strncpy( strTmp, strWxTmp.ToAscii(), 19 );
		if( !fwrite( (void*) &strTmp, sizeof(char), 19, pFile ) )
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
		if( !fwrite( (void*) &(vectCatalog[i].target_type), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].mass), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].mass_hbeta), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].mass_mgii), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].mass_civ), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].luminosity), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(vectCatalog[i].redshift), sizeof(double), 1, pFile ) )
			break;
	}

	fclose( pFile );

	return( 1 );
}
