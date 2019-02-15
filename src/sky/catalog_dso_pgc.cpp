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
#include "catalog_dso.h"
#include "../util/func.h"
#include "../unimap_worker.h"
#include "vizier.h"
#include "ledasite.h"

// main header
#include "catalog_dso_pgc.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogDsoPgc
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogDsoPgc::CSkyCatalogDsoPgc( CSkyCatalogDso* pCatalogDso, CUnimapWorker* pWorker ) 
{
	m_pCatalogDso = pCatalogDso;
	m_pUnimapWorker = pWorker;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyCatalogDsoPgc
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogDsoPgc::~CSkyCatalogDsoPgc()
{
	
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadPGCAscii
// Class:	CSkyCatalogDsoPgc
// Purpose:	load PGC galaxy catalog
// Input:	file name with galaxy catalog
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CSkyCatalogDsoPgc::LoadPGCAscii( const wxString& strFile )
{
	// if catalog loaded return
	if( m_pCatalogDso->m_vectDso ) return( 0 );

	// get pointer data from remote
	DsoDefCatBasic* vectDso = m_pCatalogDso->m_vectDso;
	unsigned long nDso = 0;

	// my vars
	FILE* pFile = NULL;
	wxChar strLine[2000];
	wxString strLog;
	double nTimeRef=0;
//	wxString strWxLine = wxT("");
	// other variables used
	unsigned long nCatNo=0;
	double nRaDeg=0, nDecDeg=0, nLogD25=0, nLogR25=0, nA=0, nB=0, nPa=0;

	// allocate vector - default hardcoded - small:1356816 - full:2915377
	vectDso = (DsoDefCatBasic*) malloc( (2915377+1) * sizeof(DsoDefCatBasic) );

	// info
//	m_pUnimapWorker->Log( "INFO :: Loading PGC catalog ..." );
	nTimeRef = GetDTime();
	// Read the index
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
		// set this to wx string
		wxString strWxLine = strLine;

		// now check if i might need to jump over - just use a random min string of 10
		if( wxStrlen( strLine ) < 10 ) continue;

		// get pgc number
		if( !strWxLine.Mid( 0, 11 ).Trim(0).Trim(1).ToULong( &nCatNo ) ) nCatNo = 0;
		// now get/convert RA to number
		if( !strWxLine.Mid( 12, 12 ).Trim(0).Trim(1).ToDouble( &nRaDeg ) ) nRaDeg = 0;
		// get/convert DEC to number
		if( !strWxLine.Mid( 25, 12 ).Trim(0).Trim(1).ToDouble( &nDecDeg ) ) nDecDeg = 0;
		// get logd25 
		if( !strWxLine.Mid( 38, 10 ).Trim(0).Trim(1).ToDouble( &nLogD25 ) ) nLogD25 = 0;
		// get logr25 
		if( !strWxLine.Mid( 49, 10 ).Trim(0).Trim(1).ToDouble( &nLogR25 ) ) nLogR25 = 0;
		// get pa angle
		if( !strWxLine.Mid( 60, 8 ).Trim(0).Trim(1).ToDouble( &nPa ) ) nPa = 0;

		// calculate a and b
		nA = (double) 0.1 * pow( 10.0, nLogD25 );
		nB = (double) (nA / pow( 10.0, nLogR25 ) )/2.0;
		nA = (double) nA/2.0;
		// adjust ra from hours to degrees
		nRaDeg = nRaDeg*15;

		// here we set the cat - nice and clean even a dupe
		vectDso[nDso].cat_no = nCatNo;
		vectDso[nDso].ra = nRaDeg;
		vectDso[nDso].dec = nDecDeg;
		vectDso[nDso].a = nA;
		vectDso[nDso].b = nB;
		vectDso[nDso].pa = nPa;
	
		// increase object counter
		nDso++;

	}
	// close my file
	fclose( pFile );

	// set pointer data to
	m_pCatalogDso->m_vectDso = vectDso;
	m_pCatalogDso->m_nDso = nDso;

	// debug
	if( DEBUG_SHOW_PROC_TIMES_ON_FILE_LOAD )
	{
		nTimeRef = GetDTime()-nTimeRef;
		strLog.Printf( wxT("DEBUG :: PGC galaxy catalog was loaded in %lu.%lu"),
						(unsigned long) nTimeRef/1000, (unsigned long) nTimeRef%1000 );
	//	m_pUnimapWorker->Log( strLog );
	}

	return( nDso );

}

/////////////////////////////////////////////////////////////////////
// Method:	LoadPGC
// Class:	CSkyCatalogDsoPgc
// Purpose:	load PGC galaxy catalog maps
// Input:	if to load region or all
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogDsoPgc::Load( int nLoadType, int nLoadSource, int nLoadLimit, 
									  int bRegion, double nCenterRa, double nCenterDec, double nRadius )
{
	// check catalog loaded
	if( m_pCatalogDso->m_vectDso != NULL )
	{
		if( m_pCatalogDso->m_nCatalogLoaded != CATALOG_ID_HYPERLEDA )
			m_pCatalogDso->UnloadCatalog();
			//free( m_vectStar );
		else
			return( 0 );
	}

	unsigned long nObj = 0;
	
	// check if to load remote or local
	if( nLoadType == CATALOG_LOAD_FILE_BINARY )
	{
		// load catalog/region from file
		nObj = m_pCatalogDso->LoadBinary( nCenterRa, nCenterDec, nRadius, bRegion );

	} else if( nLoadType == CATALOG_LOAD_REMOTE )
	{
		if( nLoadSource == CATALOG_QUERY_SOURCE_VIZIER )
		{
			CVizier oRemote( m_pCatalogDso, m_pUnimapWorker );
			nObj = oRemote.GetDsoFromVizier( CATALOG_ID_HYPERLEDA, bRegion, nCenterRa, nCenterDec, nRadius, nRadius );

		} else if( nLoadSource == CATALOG_QUERY_SOURCE_LEDA )
		{
			CLedaSite oRemote( m_pCatalogDso, m_pUnimapWorker );
			nObj = oRemote.GetObjects( CATALOG_ID_HYPERLEDA, bRegion, nCenterRa, nCenterDec, nRadius );
		}
	}

	return( nObj );
}

