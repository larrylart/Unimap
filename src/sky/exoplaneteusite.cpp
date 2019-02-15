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
#include "catalog_exoplanets.h"
#include "../unimap.h"
#include "../util/func.h"
#include "../unimap_worker.h"

// main header
#include "exoplaneteusite.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CExoplanetEuSite
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CExoplanetEuSite::CExoplanetEuSite( CSkyCatalogExoplanets* pCatalogExoplanets, CUnimapWorker* pWorker )
{
	m_pCatalogExoplanets = pCatalogExoplanets;
	m_pUnimapWorker = pWorker;

	// set sites
	m_vectSite[0] = wxT("http://www.exoplanet.eu");

	m_nSite = 1;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CExoplanetEuSite
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CExoplanetEuSite::~CExoplanetEuSite( )
{

}

/////////////////////////////////////////////////////////////////////
// Method:	LoadFetchedFile
// Class:	CExoplanetEuSite
// Purpose:	load file catalog objects from file from the site
// Input:	file name
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CExoplanetEuSite::LoadFetchedFile( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[2000];
	wxString strWxLine = wxT("");
	// other variables used
	unsigned long nCatNo=0, nLongVar, nDiscYear=0;
	double nRaDeg=0, nDecDeg=0;
	unsigned char nPlStatus=0;
	int bFoundStart=0;
	int bError=0;
	int nColIndex=0;
	// regex
	wxRegEx reExoPlanetName = wxT( "[\t\\ ]*\\<td\\ align\\=\"left\"\ class\\=\"cellule1\"\\ nowrap\\>\\&nbsp\\;\\<a\\ href\\=\"./planet.php\\?p1\=(.*)\"\\ target\\=\"_blank\"\\>(.*)\\<\\/a\\>\\<\\/td\\>" );
	wxRegEx reExoPlanetColBasic = wxT( "[\t\\ ]+\\<td\\ class\\=\"cellule5\"\\>(.+)\\<\\/td\\>" );
	wxRegEx reExoPlanetColBasic4 = wxT( "\\<td\\ class\\=\"cellule4\"\\>(.+)\\<\\/td\\>" );
	wxRegEx reExoPlanetColBasic6 = wxT( "\\<td\\ class\\=\"cellule6\"\\>(.+)\\<\\/td\\>" );
	wxRegEx reExoPlanetColThree = wxT( "\\<td\\ class\\=\"cellule5\"\\>(.+)\\<\\/td\\>\\<td\\ class\\=\"cellule5\"\\>(.+)\\<\\/td\\>\\<td\\ class\\=\"cellule5\"\\>(.+)\\<\\/td\\>" );
	wxRegEx reExoPlanetColDate = wxT( "\\<td\\ style\\=\"font\\-size\\:.*\"\\>(.*)\\<\\/td\\>" );

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// do (un)allocation
	m_pCatalogExoplanets->UnloadCatalog( );
	// allocation by type 
	if( bRegion )
		m_pCatalogExoplanets->m_nAllocated = VECT_EXOPLANETS_START_MEM_ALLOC_BLOCK_SIZE;
	else
		m_pCatalogExoplanets->m_nAllocated = VECT_EXOPLANETS_START_MEM_ALLOC_BLOCK_SIZE;
	// main allocation
	m_pCatalogExoplanets->m_vectData = (DefCatExoPlanet*) malloc( m_pCatalogExoplanets->m_nAllocated*sizeof(DefCatExoPlanet) );

	// inti/calculate progress bar stuff
	long nPCount=0;
	int nPStep=0, nLineSize=0;
	long nFileSize = GetFileSize( pLocalFile );
	long nPCountStep = (long) nFileSize/1295;

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

		// set this to wx string
//		wxString strWxLine = strLine;	

		// check for last line before data
		if( reExoPlanetName.Matches( strLine ) )
		{
			wxString strExoplanetName = reExoPlanetName.GetMatch(strLine, 2 ).Trim(0).Trim(1);

			ResetObjCatExoplanet( m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData] );

			// set name
			wxStrcpy( m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].cat_name, strExoplanetName );

			m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].cat_no = m_pCatalogExoplanets->m_nData;
			m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].cat_type = CAT_OBJECT_TYPE_EXOPLANET_EU;
			m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData].type = SKY_OBJECT_TYPE_EXOPLANET;


			//set column index to 1
			nColIndex = 1;
			//increment counter
			m_pCatalogExoplanets->m_nData++;

			bFoundStart = 1;

			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogExoplanets->m_nData >= m_pCatalogExoplanets->m_nAllocated-1  )
		{
			// incremen counter
			m_pCatalogExoplanets->m_nAllocated += VECT_EXOPLANETS_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogExoplanets->m_vectData = (DefCatExoPlanet*) realloc( m_pCatalogExoplanets->m_vectData, (m_pCatalogExoplanets->m_nAllocated+1)*sizeof(DefCatExoPlanet) );
		}

		// :: MASS
		if( nColIndex == 1 && reExoPlanetColBasic.Matches( strLine ) )
		{
			wxString strMass = reExoPlanetColBasic.GetMatch(strLine, 1 ).Trim(0).Trim(1);
			if( !strMass.ToDouble( &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].mass) ) )
				m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].mass = 0;
			nColIndex++;
			continue;
		}

		// :: plRadius
		if( nColIndex == 2 && reExoPlanetColBasic.Matches( strLine ) )
		{
			wxString strRadius = reExoPlanetColBasic.GetMatch(strLine, 1 ).Trim(0).Trim(1);
			if( !strRadius.ToDouble( &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].radius) ) )
				m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].radius = 0;
			nColIndex++;
			continue;
		}

		// :: plPeriod
		if( nColIndex == 3 && reExoPlanetColBasic.Matches( strLine ) )
		{
			wxString strPeriod = reExoPlanetColBasic.GetMatch(strLine, 1 ).Trim(0).Trim(1);
			if( !strPeriod.ToDouble( &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].period) ) )
				m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].period = 0;
			nColIndex++;
			continue;
		}

		// :: SEM-MAJ AXIS
		if( nColIndex == 4 && reExoPlanetColBasic.Matches( strLine ) )
		{
			wxString strSemMajAxis = reExoPlanetColBasic.GetMatch(strLine, 1 ).Trim(0).Trim(1);
			if( !strSemMajAxis.ToDouble( &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].sm_axis) ) )
				m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].sm_axis = 0;
			nColIndex++;
			continue;
		}

		// :: eccentricity
		if( nColIndex == 5 && reExoPlanetColBasic.Matches( strLine ) )
		{
			wxString strEcc = reExoPlanetColBasic.GetMatch(strLine, 1 ).Trim(0).Trim(1);
			if( !strEcc.ToDouble( &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].eccentricity) ) )
				m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].eccentricity = 0;
			nColIndex++;
			continue;
		}

		// :: inclination - ang_dist - status
		if( nColIndex == 6 && reExoPlanetColThree.Matches( strLine ) )
		{
			wxString strIncl = reExoPlanetColThree.GetMatch(strLine, 1 ).Trim(0).Trim(1);
			if( !strIncl.ToDouble( &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].inclination) ) )
				m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].inclination = 0;
			nColIndex++;

			wxString strAngDist = reExoPlanetColThree.GetMatch(strLine, 2 ).Trim(0).Trim(1);
			if( !strAngDist.ToDouble( &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].ang_dist) ) )
				m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].ang_dist = 0;
			nColIndex++;

			wxString strStatus = reExoPlanetColThree.GetMatch(strLine, 3 ).Trim(0).Trim(1);
			m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].status = (unsigned char) strStatus[0];
			nColIndex++;

			continue;
		}

		// :: discovery year
		if( nColIndex == 9 && reExoPlanetColBasic.Matches( strLine ) )
		{
			wxString strDiscYear = reExoPlanetColBasic.GetMatch(strLine, 1 ).Trim(0).Trim(1);
			if( !strDiscYear.ToULong( &nDiscYear ) )
				 nDiscYear= 0;
			m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].discovery_year = nDiscYear;
			nColIndex++;
			continue;
		}

		// :: record update date
		if( nColIndex == 10 && reExoPlanetColDate.Matches( strLine ) )
		{
			wxString strUpdateDate = reExoPlanetColDate.GetMatch(strLine, 1 ).Trim(0).Trim(1);

			// process date
			wxDateTime mydate;
			mydate.ParseDate( strUpdateDate );
			m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].update_date = mydate.GetJDN();

			nColIndex++;
			continue;
		}

		// :: star RA
		if( nColIndex == 11 && reExoPlanetColBasic4.Matches( strLine ) )
		{
			wxString strStarRa = reExoPlanetColBasic4.GetMatch(strLine, 1 ).Trim(0).Trim(1);
			strStarRa.Replace( wxT("&nbsp;"), wxT(":"), true );
			if( !ConvertRaToDeg( strStarRa, nRaDeg ) ) 
				nRaDeg = 0.0;
			m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].ra = nRaDeg;
			nColIndex++;
			continue;
		}

		// :: star DEC
		if( nColIndex == 12 && reExoPlanetColBasic.Matches( strLine ) )
		{
			wxString strStarDec = reExoPlanetColBasic.GetMatch(strLine, 1 ).Trim(0).Trim(1);
			strStarDec.Replace( wxT("&nbsp;"), wxT(":"), true );
			if( !ConvertDecToDeg( strStarDec, nDecDeg ) ) 
				nDecDeg = 0.0;
			m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].dec = nDecDeg;
			nColIndex++;
			continue;
		}

		// :: STAR MAG V
		if( nColIndex == 13 && reExoPlanetColBasic.Matches( strLine ) )
		{
			wxString strMagV = reExoPlanetColBasic.GetMatch(strLine, 1 ).Trim(0).Trim(1);
			if( !strMagV.ToDouble( &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].star_mag_v) ) )
				m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].star_mag_v = 0;
			nColIndex++;
			continue;
		}

		// :: STAR MAG I
		if( nColIndex == 14 && reExoPlanetColBasic.Matches( strLine ) )
		{
			wxString strMagI = reExoPlanetColBasic.GetMatch(strLine, 1 ).Trim(0).Trim(1);
			if( !strMagI.ToDouble( &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].star_mag_i) ) )
				m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].star_mag_i = 0;
			nColIndex++;
			continue;
		}

		// :: STAR MAG H
		if( nColIndex == 15 && reExoPlanetColBasic.Matches( strLine ) )
		{
			wxString strMagH = reExoPlanetColBasic.GetMatch(strLine, 1 ).Trim(0).Trim(1);
			if( !strMagH.ToDouble( &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].star_mag_h) ) )
				m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].star_mag_h = 0;
			nColIndex++;
			continue;
		}

		// :: STAR MAG J
		if( nColIndex == 16 && reExoPlanetColBasic.Matches( strLine ) )
		{
			wxString strMagJ = reExoPlanetColBasic.GetMatch(strLine, 1 ).Trim(0).Trim(1);
			if( !strMagJ.ToDouble( &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].star_mag_j) ) )
				m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].star_mag_j = 0;
			nColIndex++;
			continue;
		}

		// :: STAR MAG K
		if( nColIndex == 17 && reExoPlanetColBasic.Matches( strLine ) )
		{
			wxString strMagK = reExoPlanetColBasic.GetMatch(strLine, 1 ).Trim(0).Trim(1);
			if( !strMagK.ToDouble( &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].star_mag_k) ) )
				m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].star_mag_k = 0;
			nColIndex++;
			continue;
		}

		// :: STAR DISTANCE
		if( nColIndex == 18 && reExoPlanetColBasic.Matches( strLine ) )
		{
			wxString strStarDist = reExoPlanetColBasic.GetMatch(strLine, 1 ).Trim(0).Trim(1);
			if( !strStarDist.ToDouble( &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].star_dist) ) )
				m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].star_dist = 0;
			nColIndex++;
			continue;
		}

		// :: STAR METALLICITY
		if( nColIndex == 19 && reExoPlanetColBasic.Matches( strLine ) )
		{
			wxString strStarFeH = reExoPlanetColBasic.GetMatch(strLine, 1 ).Trim(0).Trim(1);
			if( !strStarFeH.ToDouble( &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].star_feh) ) )
				m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].star_feh = 0;
			nColIndex++;
			continue;
		}

		// :: STAR MASS
		if( nColIndex == 20 && reExoPlanetColBasic.Matches( strLine ) )
		{
			wxString strStarMass = reExoPlanetColBasic.GetMatch(strLine, 1 ).Trim(0).Trim(1);
			if( !strStarMass.ToDouble( &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].star_mass) ) )
				m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].star_mass = 0;
			nColIndex++;
			continue;
		}

		// :: STAR RADIUS
		if( nColIndex == 21 && reExoPlanetColBasic6.Matches( strLine ) )
		{
			wxString strStarRadius = reExoPlanetColBasic6.GetMatch(strLine, 1 ).Trim(0).Trim(1);
			if( !strStarRadius.ToDouble( &(m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].star_radius) ) )
				m_pCatalogExoplanets->m_vectData[m_pCatalogExoplanets->m_nData-1].star_radius = 0;
			nColIndex++;
			continue;
		}

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
		m_pCatalogExoplanets->m_nCatalogLoaded = CATALOG_ID_EXOPLANET_EU;
		return( m_pCatalogExoplanets->m_nData );

	} else
	{
		m_pCatalogExoplanets->UnloadCatalog( );
		m_pCatalogExoplanets->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	FetchObjects
// Class:	CExoplanetEuSite
// Purpose:	fetch catalog data from exoplanet.eu site into a file
// Input:	file name to save localy
// Output:	how many bytes where loaded
/////////////////////////////////////////////////////////////////////
long CExoplanetEuSite::FetchObjects( const wxString& strFile, int nCatId, int bRegion,
										  double nCenterRA, double nCenterDEC, double nRadius )
{
	long nCount=0;
	int i = 0;
	wxString strLog;
	unsigned long nWebFileSize=0;

	wxString strUrl;
	char strQuery[2000];

	// check if region or all
//	if( !bRegion )
//	{
		// set catalog params

		// set aprox webfile size - for the meter 
		nWebFileSize=449150;

//	} else

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
			// http://www.exoplanet.eu/catalog-all.php?&munit=&runit=&punit=&mode=14&more=yes
			strUrl.Printf( wxT("%s/catalog-all.php?&munit=&runit=&punit=&mode=14&more=yes"), 
						m_vectSite[i] );

			// call online - to do call multiple site addresse - vizier has a few mirrors
			nCount = LoadWebFileT( strUrl, strFile, m_pUnimapWorker, nWebFileSize );
			// if success - exit loop
			if( nCount > 0 ) break;

			// log message
			if( m_pUnimapWorker && i<m_nSite-1 )
			{
				strLog.Printf( wxT("INFO :: failed to load from exoplanet.eu site %s try next mirror %s"), m_vectSite[i], m_vectSite[i-1] );
				m_pUnimapWorker->Log( strLog );
			}
		}

	} else
		nCount = 1;

	return( nCount );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetObjects
// Class:	CExoplanetEuSite
// Purpose:	get objects from online leda site
// Input:	position, size, catalog id 
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CExoplanetEuSite::GetObjects( int nCatId, int bRegion, double nCenterRA, 
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
	if( nCatId == CATALOG_ID_EXOPLANET_EU )
		strCatName = wxT("EXOPLANET_EU");

	// build name for local file
	strLocalFile.Printf( wxT("%s/%s_%.6f_%.6f_%.4f_cat.txt"), unMakeAppPath(wxT("data/remote/ExoplanetEu")),
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
	nLoadedBytes = FetchObjects( strLocalFile, nCatId, bRegion, nCenterRA, nCenterDEC, nRadius );

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
	if( nCatId == CATALOG_ID_EXOPLANET_EU )
	{
		nLoadedObjects = LoadFetchedFile( strLocalFile, bRegion );

	} else
		return( 0 );

	// check if anything was loaded
	if( nLoadedObjects > 0 )
	{
		// and now sort my catalog by magnitude
		if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Sort catalog objects ...") );
//		m_pCatalogExoplanets->SortByA( );

		// log message
		if( m_pUnimapWorker )
		{
			strLog.Printf( wxT("INFO :: loaded %d extra-solar planets"), m_pCatalogExoplanets->m_nData );
			m_pUnimapWorker->Log( strLog );
		}

	} else
	{
		// log message
		if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT("INFO :: failed to get dso objects from exoplanet site") );
	}

	return( nLoadedObjects );
}
