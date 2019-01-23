
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

// main header
#include "catalog_dso_messier.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogDsoMessier
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogDsoMessier::CSkyCatalogDsoMessier( CSky* pSky ) : CSkyCatalogDso( pSky )
{
	m_vectMessier = NULL;
	m_nMessier = 0;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyCatalogDsoMessier
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogDsoMessier::~CSkyCatalogDsoMessier( )
{
	if( m_vectMessier ) free( m_vectMessier );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetMessierRaDec
// Class:	CSkyCatalogDsoMessier
// Purpose:	get messier ra/dec by cat no
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogDsoMessier::GetMessierRaDec( unsigned long nCatNo, double& nRa,
														double& nDec )
{
	int i =0;

	// if catalog not loaded load
	if( !m_vectMessier )
	{
		//LoadMessier( "./star_catalog/messier_catalog.txt" );
		Load( );
	}
	// look in the entire catalog for my cat no
	for( i=0; i<m_nDso; i++ )
	{
		if( m_vectDso[i].cat_no == nCatNo )
		{
			nRa = m_vectDso[i].ra;
			nDec = m_vectDso[i].dec;
		}
	}


	return( 1 );
}

/////////////////////////////////////////////////////////////////////
int CSkyCatalogDsoMessier::Load( )
{
	//int nCount = LoadMessier( DEFAULT_MESSIER_ASCII_FILENAME );
	int nCount = CSkyCatalogDso::Load( CATALOG_ID_MESSIER );
	//m_pSky->m_pCatalogDsoAssoc->LoadMessierCatMaps( m_pUnimapWorker );

	return( nCount );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadMessier
// Class:	CSkyCatalogDsoMessier
// Purpose:	load messier catalog
// Input:	file name
// Output:	number of objects
/////////////////////////////////////////////////////////////////////
int CSkyCatalogDsoMessier::LoadMessier( const wxString& strFile )
{
	// if catalog loaded return
	if( m_vectDso ) return( 0 );

	FILE* pFile = NULL;
	wxChar strLine[2000];
	wxString strLog;
	double nTimeRef=0;
//	wxString strWxLine = "";
	// other variables used
	unsigned long nCatNo=0;
	double nRaDeg=0, nDecDeg=0, nMag=0;

	// start from 0
	m_nDso = 0;

	// allocate vector
	m_vectDso = (DsoDefCatBasic*) malloc( (111+1) * sizeof(DsoDefCatBasic) );

	// info
	if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT("INFO :: Loading Messier catalog ...") );
	nTimeRef = GetDTime();
	// Read the index
	pFile = wxFopen( strFile, wxT("r") );
	// check if there is any configuration to load
	if( !pFile )
	{
		// debug
		if( m_pUnimapWorker )
		{
			strLog.Printf( wxT("Warning :: could not open Messier file %s"), strFile );
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

		if( wxStrlen( strLine ) < 42 ) continue;

		// set this to wx string
		wxString strWxLine = strLine;

		// extract fields
		if( !strWxLine.Mid( 4, 4 ).ToULong( &nCatNo ) )
			nCatNo = 0;
		// ra - 9-18
		wxString strRa = strWxLine.Mid( 9, 10 );
		if( !ConvertRaToDeg( strRa, nRaDeg ) ) nRaDeg = 0;
		// dec 20-28
		wxString strDec = strWxLine.Mid( 20, 9 );
		if( !ConvertDecToDeg( strDec, nDecDeg ) ) nDecDeg = 0;

		// magnitude 30-33
		if( !strWxLine.Mid( 30, 4 ).ToDouble( &nMag ) ) nMag = 0;

		ResetObjCatDso( m_vectDso[m_nDso] );

		// here we set the cat - nice and clean even a dupe
		m_vectDso[m_nDso].cat_no = nCatNo;
		m_vectDso[m_nDso].ra = nRaDeg;
		m_vectDso[m_nDso].dec = nDecDeg;
		m_vectDso[m_nDso].mag = nMag;
		m_vectDso[m_nDso].a = 0.0;
		m_vectDso[m_nDso].b = 0.0;
		m_vectDso[m_nDso].pa = 0.0;

		// increase star counter
		m_nDso++;

	}
	// close my file
	fclose( pFile );

	// debug
	nTimeRef = GetDTime()-nTimeRef;
	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("DEBUG :: messier catalog was loaded in %lu.%lu"),
					(unsigned long) nTimeRef/1000, (unsigned long) nTimeRef%1000 );
		m_pUnimapWorker->Log( strLog );
	}

	return( 1 );

}
