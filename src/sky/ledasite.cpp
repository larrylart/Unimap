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
#include <wx/filesys.h>
#include <wx/html/htmlpars.h>
#include <wx/fs_inet.h>
#include <wx/tokenzr.h>

// local headers
#include "../util/folders.h"
#include "../util/webfunc.h"
#include "sky.h"
//#include "catalog_stars.h"
#include "catalog_dso.h"
#include "../unimap.h"
#include "../util/func.h"
#include "../unimap_worker.h"

// main header
#include "ledasite.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CLedaSite
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CLedaSite::CLedaSite( CSkyCatalogDso* pCatalogDso, CUnimapWorker* pWorker )
{
	m_pCatalogDso = pCatalogDso;
	m_pUnimapWorker = pWorker;

	// set sites
	m_vectSite[0] = wxT("http://leda.univ-lyon1.fr/leda");
	m_vectSite[1] = wxT("http://draco.skyarchive.org/~hyperleda");
	m_vectSite[2] = wxT("http://www.brera.mi.astro.it/hypercat");
	m_vectSite[3] = wxT("http://atlas.obs-hp.fr/hyperleda");
	m_vectSite[4] = wxT("http://leda.iucaa.ernet.in/leda");

	m_nSite = 5;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CLedaSite
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CLedaSite::~CLedaSite( )
{

}

/////////////////////////////////////////////////////////////////////
// Method:	LoadHyperledaFileFromLeda
// Class:	CLedaSite
// Purpose:	load HYPERLEDA catalog objects from file from LEDA site
// Input:	file name
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CLedaSite::LoadHyperledaFileFromLeda( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
	wxString strWxLine = wxT("");
	// other variables used
	unsigned long nCatNo=0, nLongVar;
	double nRaDeg=0, nDecDeg=0, nMag=0, nLogD25=0, nLogR25=0, nA=0, nB=0, nPa=0;
	int bFoundStart=0;
	int bError=0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// do (un)allocation
	m_pCatalogDso->UnloadCatalog( );
	// allocation by type 
	if( bRegion )
		m_pCatalogDso->m_nDsoAllocated = 10000;
	else
		m_pCatalogDso->m_nDsoAllocated = 2991085;
	// main allocation
	m_pCatalogDso->m_vectDso = (DsoDefCatBasic*) malloc( m_pCatalogDso->m_nDsoAllocated*sizeof(DsoDefCatBasic) );

	// inti/calculate progress bar stuff
	long nPCount=0;
	int nPStep=0, nLineSize=0;
	long nFileSize = GetFileSize( pLocalFile );
	long nPCountStep = (long) nFileSize/100;

	// Reading lines from cfg file
	while( !feof( pLocalFile ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pLocalFile );
		nLineSize = wxStrlen( strLine );

		//////////////////////
		// check if line is a comment jump
		// if line empty (less the a minimum of 10 lets say - jump 
		if( nLineSize < 10 )
			continue;
		// check for hash
		if( strLine[0] == '<' )
			continue;

		// set this to wx string
		wxString strWxLine = strLine;	

		// check for last line before data
		if( !bFoundStart && strWxLine.Mid( 0, 10 ).Trim(0).Trim(1).CmpNoCase( wxT("pgc") ) == 0 )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogDso->m_nDso >= m_pCatalogDso->m_nDsoAllocated-1  )
		{
			// incremen counter
			m_pCatalogDso->m_nDsoAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogDso->m_vectDso = (DsoDefCatBasic*) realloc( m_pCatalogDso->m_vectDso, (m_pCatalogDso->m_nDsoAllocated+1)*sizeof(DsoDefCatBasic) );
		}

		// get star cat
		wxString strCatNo = strWxLine.Mid( 0, 11 ).Trim(0).Trim(1);
		if( !strCatNo.ToULong( &nCatNo ) ) 
			continue;
		// :: RA
		if( !strWxLine.Mid( 12, 12 ).Trim(0).Trim(1).ToDouble( &nRaDeg ) )
			nRaDeg = 0;
		// :: DEC
		if( !strWxLine.Mid( 25, 12 ).Trim(0).Trim(1).ToDouble( &nDecDeg ) )
			nDecDeg = 0;
		// :: A -> nLogD25
		if( !strWxLine.Mid( 38, 10 ).Trim(0).Trim(1).ToDouble( &nLogD25 ) )
			nLogD25 = 0.0;
		// :: B -> nLogR25
		if( !strWxLine.Mid( 49, 10 ).Trim(0).Trim(1).ToDouble( &nLogR25 ) )
			nLogR25 = 0.0;
		// :: PA - angle
		if( !strWxLine.Mid( 60, 8 ).Trim(0).Trim(1).ToDouble( &nPa ) )
			nPa = 0.0;

		// :: type
		wxString strType = strWxLine.Mid( 69, 7 ).Trim(0).Trim(1);

		// calculate a and b
		nA = (double) 0.1 * pow( 10.0, nLogD25 );
		nB = (double) (nA / pow( 10.0, nLogR25 ) )/2.0;
		nA = (double) nA/2.0;
		// adjust ra from hours to degrees
		nRaDeg = nRaDeg*15;

		// set star
		//m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].cat_name = NULL;
		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].cat_no = nCatNo;
//		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].zone_no = 0;
//		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].comp_no = 0;
		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].cat_type = CAT_OBJECT_TYPE_PGC;

		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].ra = nRaDeg;
		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].dec = nDecDeg;
		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].mag = 0;
		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].type = SKY_OBJECT_TYPE_GALAXY;

		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].a = nA;
		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].b = nB;
		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].pa = nPa;

		// increase star counter
		m_pCatalogDso->m_nDso++;

		///////
		// GUI :: update progress
		if( m_pUnimapWorker )
		{
			// check if break
			if( m_pUnimapWorker->m_bCurrentTaskSetToBreak )
			{
				bError = 1;
				break;
			}

			if( nPCount > nPCountStep )
			{
				nPCount=0;
				nPStep++;
				m_pUnimapWorker->SetWaitDialogProgress( nPStep );
				continue;

			} else
				nPCount += nLineSize;
		}
	}
	// close my file
	fclose( pLocalFile );

	// check end of read state
	if( !bError )
	{
		m_pCatalogDso->m_nCatalogLoaded = CATALOG_ID_HYPERLEDA;
		return( m_pCatalogDso->m_nDso );

	} else
	{
		m_pCatalogDso->UnloadCatalog( );
		m_pCatalogDso->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	FetchObjectsFromLeda
// Class:	CLedaSite
// Purpose:	fetch catalog data from hyperleda site into a file
// Input:	file name to save localy
// Output:	how many bytes where loaded
/////////////////////////////////////////////////////////////////////
long CLedaSite::FetchObjectsFromLeda( const wxString& strFile, int nCatId, int bRegion,
										  double nCenterRA, double nCenterDEC, double nRadius )
{
	long nCount=0;
	int i = 0;
	wxString strLog;
	unsigned long nWebFileSize=0;

	wxString strUrl;
	wxString strQuery;

	// check if region or all
	if( !bRegion )
	{
		// set catalog params
		// with check for a, b, pa
		//sprintf( strQuery, "select pgc,al2000,de2000,logd25,logr25,pa,type where logd25 is not NULL and logr25 is not NULL AND pa is not NULL order by pgc asc" );
		// with no check
		strQuery.Printf( wxT("select pgc,al2000,de2000,logd25,logr25,pa,type where al2000 is not NULL and de2000 is not NULL order by pgc asc") );
		// set aprox webfile size - for the meter 
		nWebFileSize=159621839;

	} else
	{
		// set catalog params
		double nDecRad = nCenterDEC*DEG2RAD;
		// with check for a, b, pa not null
		//sprintf( strQuery, "select pgc,al2000,de2000,logd25,logr25,pa,type where logd25 is not NULL and logr25 is not NULL AND pa is not NULL AND degrees(acos(sin(%lf)*sin(radians(de2000)) + cos(%lf)*cos(radians(de2000))*cos(radians(%lf-al2000*15)))) < %lf order by pgc asc", 
		//			nDecRad, nDecRad, nCenterRA, nRadius );
		// with no check
		strQuery.Printf( wxT("select pgc,al2000,de2000,logd25,logr25,pa,type where al2000 is not NULL and de2000 is not NULL AND degrees(acos(sin(%lf)*sin(radians(de2000)) + cos(%lf)*cos(radians(de2000))*cos(radians(%lf-al2000*15)))) < %lf order by pgc asc"), 
					nDecRad, nDecRad, nCenterRA, nRadius );
		//strQuery = "select pgc,al2000,de2000,logd25,logr25,pa,type where logd25 is not NULL and logr25 is not NULL AND pa is not NULL AND degrees(acos(sin(1.205)*sin(radians(de2000)) + cos(1.205)*cos(radians(de2000))*cos(radians(148.88822108-al2000*15)))) < 0.5 order by pgc asc";

	}

	//////////////////////////
	// now here we should check first if we already have that info localy
	if( !wxFileExists( wxString(strFile,wxConvUTF8) ) )
	{
		// for all available mirros
		for( i=0; i<m_nSite; i++ )
		{
			if( m_pUnimapWorker && m_pUnimapWorker->m_bCurrentTaskSetToBreak )
			{
				nCount = 0;
				break;
			}

			// build name for the url to request
			// http://leda.univ-lyon1.fr/leda/fullsqlmean.cgi?Query=select%20count(*)&separator=;&noval=-&mime=application/x-download
			strUrl.Printf( wxT("%s/fullsqlmean.cgi?Query=%s&separator=|&noval=-&mime=application/x-download"), 
						m_vectSite[i], URLEncode(strQuery) );

			// call online - to do call multiple site addresse - vizier has a few mirrors
			nCount = LoadWebFileT( strUrl, strFile, m_pUnimapWorker, nWebFileSize );
			// if success - exit loop
			if( nCount > 0 ) break;

			// log message
			if( m_pUnimapWorker && i<m_nSite-1 )
			{
				strLog.Printf( wxT("INFO :: failed to load from  LEDA site %s try next mirror %s"), m_vectSite[i], m_vectSite[i-1] );
				m_pUnimapWorker->Log( strLog );
			}
		}

	} else
		nCount = 1;

	return( nCount );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetObjects
// Class:	CLedaSite
// Purpose:	get objects from online leda site
// Input:	position, size, catalog id 
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CLedaSite::GetObjects( int nCatId, int bRegion, double nCenterRA, 
						   double nCenterDEC, double nRadius )
{
	wxString strLog;
	wxString strLocalFile;
	int i=0, j=0;
	wxString strCatName = wxT("UnknownCat");
	long nLoadedObjects=0, nLoadedBytes=0;

	/////////////////////////////////////
	// calculate fetch radius ??? - should be by calc distance ?
//	double nFetchRadius = (double) ( (double) sqrt( (double) nWidth*nWidth + (double) nHeight*nHeight )/2 );

	/////////////////
	// here we need to calculate max rad based on VB
	double nMaxRad = 1.5;

	///////////////////////////
	// set vars by cat type
	// :: DSO
	if( nCatId == CATALOG_ID_HYPERLEDA )
		strCatName = wxT("HYPERLEDA");

	// build name for local file
	strLocalFile.Printf( wxT("%s/%s_%.6f_%.6f_%.4f_cat.txt"), unMakeAppPath(wxT("data/remote/Leda")),
				strCatName, nCenterRA, nCenterDEC, nRadius );

	// log info
	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("INFO :: Fetching area orig(%.2f,%.2f) with radius=%.2f..."),
									nCenterRA, nCenterDEC, nRadius );
		m_pUnimapWorker->Log( strLog );
	}

	/////////
	// call method to get stars from online site
	nLoadedBytes = FetchObjectsFromLeda( strLocalFile, nCatId, bRegion, nCenterRA, nCenterDEC, nRadius );

	// check if anything was loaded
	if( nLoadedBytes <=0 )
	{
		if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT("Failed to fetch remote data...") );
		return( 0 );
	}

	// log info
	if( m_pUnimapWorker )
	{
		m_pUnimapWorker->SetWaitDialogMsg( wxT("Import data to local format ...") );

		strLog.Printf( wxT("INFO :: Loading file for area orig(%.2f,%.2f) with radius=%.2f..."),
								nCenterRA, nCenterDEC, nRadius );
		m_pUnimapWorker->Log( strLog );
	}

	////////////////////////////
	// and now load the file just downloaded
	if( nCatId == CATALOG_ID_HYPERLEDA )
	{
		nLoadedObjects = LoadHyperledaFileFromLeda( strLocalFile, bRegion );

	} else
		return( 0 );

	// check if anything was loaded
	if( nLoadedObjects > 0 )
	{
		// and now sort my catalog by magnitude
		if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Sort catalog objects ...") );
		m_pCatalogDso->SortByA( );

		// log message
		if( m_pUnimapWorker )
		{
			strLog.Printf( wxT("INFO :: loaded %d dso objects"), m_pCatalogDso->m_nDso );
			m_pUnimapWorker->Log( strLog );
		}

	} else
	{
		// log message
		if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT("INFO :: failed to get dso objects from LEDA site") );
	}

	return( nLoadedObjects );
}
