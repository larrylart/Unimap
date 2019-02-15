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
#include "catalog_stars.h"
#include "../util/func.h"
#include "../unimap_worker.h"
#include "vizier.h"

// main header
#include "catalog_stars_hipparcos.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogStarsHipparcos
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogStarsHipparcos::CSkyCatalogStarsHipparcos( CSkyCatalogStars* pCatalogStars, CUnimapWorker* pWorker )
{
	m_pCatalogStars = pCatalogStars;
	m_pUnimapWorker = pWorker;

	// set my stuff
	m_pCatalogStars->m_strFile = DEFAULT_HIPPARCOS_BINARY_FILENAME;
	m_pCatalogStars->m_strName = wxT("Hipparcos");
	m_pCatalogStars->m_strLabel = wxT("HIP");
	m_pCatalogStars->m_nCatalogLoaded = CATALOG_ID_HIPPARCOS;
	m_pCatalogStars->m_nObjectType = CAT_OBJECT_TYPE_HIPPARCOS;

}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyCatalogStarsHipparcos
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogStarsHipparcos::~CSkyCatalogStarsHipparcos( )
{

}

/////////////////////////////////////////////////////////////////////
// Method:	LoadTycho2
// Class:	CSkyCatalogStarsHipparcos
// Purpose:	load ascii format catalog
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CSkyCatalogStarsHipparcos::LoadAscii( const wxString& strFile )
{
	FILE* pFile = NULL;
	wxChar strLine[2000];
	wxString strLog;
	// other variables used
	wxChar strCatNo[255];
	wxChar strRa[255];
	wxChar strDec[255];
	wxChar strMag[255];

	// init min and max
	double nMinX = DBL_MAX;
	double nMinY = DBL_MAX;
	double nMaxX = DBL_MIN;
	double nMaxY = DBL_MIN;

	// get pointer data from remote
	StarDefCatBasic* vectStar = m_pCatalogStars->m_vectStar;
	unsigned long nStar = 0;

	// allocate my vector + 1 unit just to be sure
	m_pCatalogStars->m_nStarAllocated = 117955+1;
	vectStar = (StarDefCatBasic*) malloc( m_pCatalogStars->m_nStarAllocated*sizeof(StarDefCatBasic) );

	// open tycho 2 catalog
	pFile = wxFopen( strFile, wxT("r") );
	// check if there is any configuration to load
	if( !pFile )
	{
		// debug
		if( m_pUnimapWorker )
		{
			strLog.Printf( wxT("Warning :: could not open Hipparcos catalog file %s"), strFile );
			m_pUnimapWorker->Log( strLog );
		}
		return( -1 );
	}
	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pFile );

		if( wxStrlen( strLine ) < 6 )  continue;

		// check if obj
		if( wxSscanf( strLine, wxT("%[^\t]\t%[^\t]\t%[^\t]\t%[^\0]\0"), strCatNo, strRa, strDec, strMag ) )
		{
			vectStar[nStar].zone_no = 0;
			vectStar[nStar].cat_no = (unsigned long) wxAtol( strCatNo );
			vectStar[nStar].comp_no = 0;
			vectStar[nStar].ra = wxAtof( strRa );
			vectStar[nStar].dec = wxAtof( strDec );
			vectStar[nStar].mag = wxAtof( strMag );

			vectStar[nStar].cat_type = CAT_OBJECT_TYPE_HIPPARCOS;

			// calculate min and max --- ???? do i need or just delete ?!!!!
			if( vectStar[nStar].ra < nMinX ) nMinX = vectStar[nStar].ra;
			if( vectStar[nStar].ra > nMaxX ) nMaxX = vectStar[nStar].ra;

			if( vectStar[nStar].dec < nMinY ) nMinY = vectStar[nStar].dec;
			if( vectStar[nStar].dec > nMaxY ) nMaxY = vectStar[nStar].dec;

			// increase star counter
			nStar++;

		}
	}
	// close my file
	fclose( pFile );

	// set min/max
	m_pCatalogStars->m_nMinX = nMinX;
	m_pCatalogStars->m_nMinY = nMinY;
	m_pCatalogStars->m_nMaxX = nMaxX;
	m_pCatalogStars->m_nMaxY = nMaxY;

	// set pointer data to
	m_pCatalogStars->m_vectStar = vectStar;
	m_pCatalogStars->m_nStar = nStar;

	m_pCatalogStars->m_nCatalogLoaded = CATALOG_ID_HIPPARCOS;

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	Load
// Class:	CSkyCatalogStarsHipparcos
// Purpose:	load sao
// Input:	 load limit, region flag, ra/dec, size
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogStarsHipparcos::Load( int nLoadType, int nLoadLimit, int bRegion, double nRa, double nDec,
										double nWidth, double nHeight )
{
	// check catalog loaded
	if( m_pCatalogStars->m_vectStar != NULL )
	{
		if( m_pCatalogStars->m_nCatalogLoaded != CATALOG_ID_HIPPARCOS )
			m_pCatalogStars->UnloadCatalog();
		else
			return( 0 );
	}
	
	unsigned long nStars = 0;

	// case :: binary load
	if( nLoadType == CATALOG_LOAD_FILE_BINARY )
	{
		nStars = m_pCatalogStars->LoadBinary( DEFAULT_HIPPARCOS_BINARY_FILENAME, nLoadLimit, bRegion, nRa, nDec, nWidth, nHeight );

	} else if( nLoadType == CATALOG_LOAD_REMOTE )
	{
		CVizier oRemote( m_pCatalogStars, m_pUnimapWorker );
		nStars = oRemote.GetStarsFromVizier( CATALOG_ID_HIPPARCOS, bRegion, nRa, nDec, nWidth, nHeight );
	}

	return( nStars );
}

/*
/////////////////////////////////////////////////////////////////////
// Method:	LoadBinary
// Class:	CSkyCatalogStarsHipparcos
// Purpose:	load the binary version of the catalog to increase speed
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogStarsHipparcos::LoadBinary( const char* strFile, int nLoadLimit,
								int bRegion, double nRa, double nDec,
										double nWidth, double nHeight )
{
	FILE* pFile = NULL;
	char strLog[255];
	// radius
	double nFetchRadius = 180.0;

	// init min and max
	double nMinX = DBL_MAX;
	double nMinY = DBL_MAX;
	double nMaxX = DBL_MIN;
	double nMaxY = DBL_MIN;

	// get pointer data from remote
	StarDefCatBasic* vectStar = m_pCatalogStars->m_vectStar;
	unsigned long nStar = 0;

	if( bRegion )
	{
		sprintf( strLog, "INFO :: loading tycho2 area orig(%.2f,%.2f) size(%.2f,%.2f)...",
							nRa, nDec, nWidth, nHeight );
		m_pUnimapWorker->Log( strLog );
		// calculate radius 
		nFetchRadius = (double) ( (double) sqrt( (double) nWidth*nWidth + (double) nHeight*nHeight )/2 );

	} else
	{
		m_pUnimapWorker->Log( "INFO :: loading the entire tycho2 catalog ..." );
	}

	// open file to write
	pFile = fopen( strFile, "rb" );
	if( !pFile )
	{
		return( 0 );
	}

	// do the initial allocation
	if( vectStar != NULL ) m_pCatalogStars->UnloadCatalog();

	// check if limit
	if( nLoadLimit > 0 )
	{
		// allocate my vector + 1 unit just to be sure
		vectStar = (StarDefCatBasic*) malloc( (nLoadLimit+1)*sizeof(StarDefCatBasic) );
		// also set load limit just in case
		m_pCatalogStars->m_nStarAllocated = nLoadLimit;

	} else
	{
		// if I am in a region
		if( bRegion )
		{
			m_pCatalogStars->m_nStarAllocated = 300;

			// allocate my vector + 1 unit just to be sure
			vectStar = (StarDefCatBasic*) malloc( (m_pCatalogStars->m_nStarAllocated+1)*sizeof(StarDefCatBasic) );

		} else
		{
			// now go at the end of the file and see the size
			fseek( pFile, 0, SEEK_END );
			// get the file size and calculate how many elements i have
			int nRecords = (int) ( ftell( pFile )/( sizeof(unsigned long)+3*sizeof(double) ));
			// allocate my vector + 1 unit just to be sure
			vectStar = (StarDefCatBasic*) malloc( (nRecords+1)*sizeof(StarDefCatBasic) );
			// go at the begining of the file
			fseek( pFile, 0, SEEK_SET );
			// and also set load limit
			m_pCatalogStars->m_nStarAllocated = nRecords+1;
		}
	}
	////////////////////
	// now write all stars info in binary format
	while( !feof( pFile ) )
	{
		if( !fread( (void*) &(vectStar[nStar].zone_no), sizeof(unsigned int), 1, pFile ) )
			break;
		if( !fread( (void*) &(vectStar[nStar].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		if( !fread( (void*) &(vectStar[nStar].comp_no), sizeof(unsigned char), 1, pFile ) )
			break;
		if( !fread( (void*) &(vectStar[nStar].ra), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(vectStar[nStar].dec), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(vectStar[nStar].mag), sizeof(double), 1, pFile ) )
			break;

		if( bRegion && CalcSkyDistance( nRa, nDec, 
			vectStar[nStar].ra, vectStar[nStar].dec ) > nFetchRadius )
		{
			// skip - don't add this star as is not in the region
			continue;
		}

		vectStar[nStar].cat_type = CAT_OBJECT_TYPE_TYCHO;

		// increment counter
		nStar++;


		// check if i am over the load limit
		if( nLoadLimit > 0 && nStar >= nLoadLimit ) break;

		// check if I need to reallocate
		if( nStar >= m_pCatalogStars->m_nStarAllocated )
		{
			// incremen counter
			m_pCatalogStars->m_nStarAllocated += VECT_STARS_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			vectStar = (StarDefCatBasic*) realloc( vectStar, (m_pCatalogStars->m_nStarAllocated+1)*sizeof(StarDefCatBasic) );
		}
	}

	fclose( pFile );

	// set min/max
	m_pCatalogStars->m_nMinX = nMinX;
	m_pCatalogStars->m_nMinY = nMinY;
	m_pCatalogStars->m_nMaxX = nMaxX;
	m_pCatalogStars->m_nMaxY = nMaxY;

	// set pointer data to
	m_pCatalogStars->m_vectStar = vectStar;
	m_pCatalogStars->m_nStar = nStar;

	m_pCatalogStars->m_nStarCatLoaded = CATALOG_ID_TYCHO_2;

	// log message
	if( m_pUnimapWorker )
	{
		sprintf( strLog, "INFO :: loaded %d stars from TYCHO2", nStar );
		m_pUnimapWorker->Log( strLog );
	}

	return( nStar );
}

/////////////////////////////////////////////////////////////////////
// Method:	ExportTycho2Binary
// Class:	CSkyCatalogStarsHipparcos
// Purpose:	export current catalog as binary
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogStarsHipparcos::ExportBinary(  )
{
	FILE* pFile = NULL;
	unsigned long i = 0;

	// open file to write
	pFile = fopen( DEFAULT_TYCHO2_BINARY_FILENAME, "wb" );
	if( !pFile )
	{
		return( 0 );
	}
	// now write all stars info in binary format
	for( i=0; i<m_pCatalogStars->m_nStar; i++ )
	{
		// first check to see if my object has the right type
		if( m_pCatalogStars->m_vectStar[i].cat_type == CAT_OBJECT_TYPE_TYCHO )
		{
			if( !fwrite( (void*) &(m_pCatalogStars->m_vectStar[i].zone_no), sizeof(unsigned int), 1, pFile ) )
				break;
			if( !fwrite( (void*) &(m_pCatalogStars->m_vectStar[i].cat_no), sizeof(unsigned long), 1, pFile ) )
				break;
			if( !fwrite( (void*) &(m_pCatalogStars->m_vectStar[i].comp_no), sizeof(unsigned char), 1, pFile ) )
				break;
			if( !fwrite( (void*) &(m_pCatalogStars->m_vectStar[i].ra), sizeof(double), 1, pFile ) )
				break;
			if( !fwrite( (void*) &(m_pCatalogStars->m_vectStar[i].dec), sizeof(double), 1, pFile ) )
				break;
			if( !fwrite( (void*) &(m_pCatalogStars->m_vectStar[i].mag), sizeof(double), 1, pFile ) )
				break;
		}
	}

	fclose( pFile );

	return( 1 );
}
*/

