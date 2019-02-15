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
#include "catalog_stars_tycho.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogStarsTycho
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogStarsTycho::CSkyCatalogStarsTycho( CSkyCatalogStars* pCatalogStars, CUnimapWorker* pWorker )
{
	m_pCatalogStars = pCatalogStars;
	m_pUnimapWorker = pWorker;

	// set my stuff
	m_pCatalogStars->m_strFile = DEFAULT_TYCHO2_BINARY_FILENAME;
	m_pCatalogStars->m_strName = wxT("TYCHO2");
	m_pCatalogStars->m_strLabel = wxT("TYCHO");
	m_pCatalogStars->m_nCatalogLoaded = CATALOG_ID_TYCHO_2;
	m_pCatalogStars->m_nObjectType = CAT_OBJECT_TYPE_TYCHO;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyCatalogStarsTycho
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogStarsTycho::~CSkyCatalogStarsTycho( )
{

}

/////////////////////////////////////////////////////////////////////
// Method:	LoadTycho2
// Class:	CSkyCatalogStarsTycho
// Purpose:	load Tycho 2 catalog
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CSkyCatalogStarsTycho::LoadTycho2( const wxString& strFile )
{
	FILE* pFile = NULL;
	wxChar strLine[2000];
//	char strMsg[255];
//	wxString strWxLine = wxT("");
	// other variables used
	unsigned long nCatNo=0, nZoneNo=0, nCompNo=0;
	double nRaDeg=0, nDecDeg=0, nMag=0;

	// init min and max
	double nMinX = DBL_MAX;
	double nMinY = DBL_MAX;
	double nMaxX = DBL_MIN;
	double nMaxY = DBL_MIN;

	// get pointer data from remote
	StarDefCatBasic* vectStar = m_pCatalogStars->m_vectStar;
	unsigned long nStar = 0;

	// allocate my vector + 1 unit just to be sure
	m_pCatalogStars->m_nStarAllocated = 2539913+1;
	vectStar = (StarDefCatBasic*) malloc( m_pCatalogStars->m_nStarAllocated*sizeof(StarDefCatBasic) );

	// open tycho 2 catalog
	pFile = wxFopen( strFile, wxT("r") );
	// check if there is any configuration to load
	if( !pFile )
	{
		// debug
//		sprintf( strMsg, "Warning :: could not open config file %s", strFile );
//		m_pUnimapWorker->Log( strMsg );
		return( -1 );
	}
	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pFile );

		// if line empty/less jump
		if( wxStrlen( strLine ) < 20 )
			continue;

		// set this to wx string
		wxString strWxLine = strLine;

		// check if object has no pos
		if( !strWxLine.Mid( 13, 1 ).CmpNoCase( wxT("X") ) )
			continue;

		/////////////////////////////
		// here we extract field we need
		// tycho 2 zone number
		if( !strWxLine.Mid( 0, 4 ).ToULong( &nZoneNo ) )
			nZoneNo = 0;
		// tycho 2 cat number
		if( !strWxLine.Mid( 5, 5 ).ToULong( &nCatNo ) )
			nCatNo = 0;
		// tycho 2 component number
		if( !strWxLine.Mid( 11, 1 ).ToULong( &nCompNo ) )
			nCompNo = 0;
		// ra in deg 15-26
		if( !strWxLine.Mid( 15, 12 ).ToDouble( &nRaDeg ) )
			nRaDeg = 0;
		// Dec in deg 28-39
		if( !strWxLine.Mid( 28, 12 ).ToDouble( &nDecDeg ) )
			nDecDeg = 0;
		// VMag 123-128
		if( !strWxLine.Mid( 123, 6 ).ToDouble( &nMag ) )
		{
			// try to get BT (blu mag instead ?
			if( !strWxLine.Mid( 110, 6 ).ToDouble( &nMag ) )
				nMag = 0;
		}

		vectStar[nStar].cat_type = CAT_OBJECT_TYPE_TYCHO;

		// set star
		vectStar[nStar].cat_name = NULL;
		vectStar[nStar].zone_no = (unsigned int) nZoneNo;
		vectStar[nStar].cat_no = nCatNo;
		vectStar[nStar].comp_no = (unsigned char) nCompNo;
		vectStar[nStar].ra = nRaDeg;
		vectStar[nStar].dec = nDecDeg;
		vectStar[nStar].mag = nMag;

		// calculate min and max --- ???? do i need or just delete ?!!!!
		if( vectStar[nStar].ra < nMinX ) nMinX = vectStar[nStar].ra;
		if( vectStar[nStar].ra > nMaxX ) nMaxX = vectStar[nStar].ra;

		if( vectStar[nStar].dec < nMinY ) nMinY = vectStar[nStar].dec;
		if( vectStar[nStar].dec > nMaxY ) nMaxY = vectStar[nStar].dec;

		// increase star counter
		nStar++;
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

	m_pCatalogStars->m_nCatalogLoaded = CATALOG_ID_TYCHO_2;

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	Load
// Class:	CSkyCatalogStarsTycho
// Purpose:	load sao
// Input:	 load limit, region flag, ra/dec, size
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogStarsTycho::Load( int nLoadType, int nLoadLimit, int bRegion, double nRa, double nDec,
										double nWidth, double nHeight )
{
	// check catalog loaded
	if( m_pCatalogStars->m_vectStar != NULL )
	{
		if( m_pCatalogStars->m_nCatalogLoaded != CATALOG_ID_TYCHO_2 )
			m_pCatalogStars->UnloadCatalog();
			//free( m_vectStar );
		else
			return( 0 );
	}
	
	unsigned long nStars = 0;

	// case :: binary load
	if( nLoadType == CATALOG_LOAD_FILE_BINARY )
	{
		nStars = LoadTycho2Binary( DEFAULT_TYCHO2_BINARY_FILENAME, nLoadLimit, bRegion, nRa, nDec, nWidth, nHeight );

	} else if( nLoadType == CATALOG_LOAD_REMOTE )
	{
		CVizier oRemote( m_pCatalogStars, m_pUnimapWorker );
		nStars = oRemote.GetStarsFromVizier( CATALOG_ID_TYCHO_2, bRegion, nRa, nDec, nWidth, nHeight );
	}

	return( nStars );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadTycho2Binary
// Class:	CSkyCatalogStarsTycho
// Purpose:	load the binary version of the catalog to increase speed
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogStarsTycho::LoadTycho2Binary( const wxString& strFile, int nLoadLimit,
								int bRegion, double nRa, double nDec,
										double nWidth, double nHeight )
{
	FILE* pFile = NULL;
	wxString strLog;
	// radius
	double nFetchRadius = 180.0;

	// calculate min/max
/*	double nMinRa = nRa-nWidth/2;
	double nMaxRa = nRa+nWidth/2;
	double nMinDec = nDec-nHeight/2;
	double nMaxDec = nDec+nHeight/2;
*/

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
		strLog.Printf( wxT("INFO :: loading tycho2 area orig(%.2f,%.2f) size(%.2f,%.2f)..."),
							nRa, nDec, nWidth, nHeight );
		m_pUnimapWorker->Log( strLog );
		// calculate radius 
		nFetchRadius = (double) ( (double) sqrt( (double) nWidth*nWidth + (double) nHeight*nHeight )/2 );

	} else
	{
		m_pUnimapWorker->Log( wxT("INFO :: loading the entire tycho2 catalog ...") );
	}

	// open file to write
	pFile = wxFopen( strFile, wxT("rb") );
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
		// first reset current record
		memset( &(vectStar[nStar]), 0, sizeof(StarDefCatBasic) );

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

/*		if( bRegion && !(m_vectStar[m_nStar].ra <= nMaxRa &&
						m_vectStar[m_nStar].ra >= nMinRa &&
						m_vectStar[m_nStar].dec <= nMaxDec &&
						m_vectStar[m_nStar].dec >= nMinDec) )
*/
		// debug - 8659-3107-1 - mimas
//		int dd=0;
//		if( vectStar[nStar].zone_no == 8659 &&
//			vectStar[nStar].cat_no == 3107 )
//			dd = 1;


		if( bRegion && CalcSkyDistance( nRa, nDec, 
			vectStar[nStar].ra, vectStar[nStar].dec ) > nFetchRadius )
		{
			// skip - don't add this star as is not in the region
			continue;
		}

		vectStar[nStar].cat_name = NULL;
		vectStar[nStar].cat_type = CAT_OBJECT_TYPE_TYCHO;

/*		// calculate min and max --- ???? do i need or just delete ?!!!!
		if( m_vectStar[m_nStar].ra < m_nMinX ) m_nMinX = m_vectStar[m_nStar].ra;
		if( m_vectStar[m_nStar].ra > m_nMaxX ) m_nMaxX = m_vectStar[m_nStar].ra;

		if( m_vectStar[m_nStar].dec < m_nMinY ) m_nMinY = m_vectStar[m_nStar].dec;
		if( m_vectStar[m_nStar].dec > m_nMaxY ) m_nMaxY = m_vectStar[m_nStar].dec;
*/
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

	m_pCatalogStars->m_nCatalogLoaded = CATALOG_ID_TYCHO_2;

	// log message
	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("INFO :: loaded %d stars from TYCHO2"), nStar );
		m_pUnimapWorker->Log( strLog );
	}

	return( nStar );
}

/////////////////////////////////////////////////////////////////////
// Method:	ExportTycho2Binary
// Class:	CSkyCatalogStarsTycho
// Purpose:	export current catalog as binary
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogStarsTycho::ExportTycho2Binary(  )
{
	FILE* pFile = NULL;
	unsigned long i = 0;

	// open file to write
	pFile = wxFopen( m_pCatalogStars->m_strFile, wxT("wb") );
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

