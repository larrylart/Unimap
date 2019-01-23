
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
#include "../config/mainconfig.h"
#include "sky.h"
#include "catalog_stars.h"
#include "catalog_dso.h"
#include "../util/func.h"
#include "../unimap_worker.h"

// main header
#include "nofs_navy_mil.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CNofsNavyMil
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CNofsNavyMil::CNofsNavyMil( CSkyCatalogStars* pCatalogStars, CUnimapWorker* pWorker )
{
	m_pCatalogStars = pCatalogStars;
	m_pUnimapWorker = pWorker;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CNofsNavyMil
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CNofsNavyMil::~CNofsNavyMil( )
{

}

/////////////////////////////////////////////////////////////////////
// Method:	FetchStarsFromUSNB1
// Class:	CNofsNavyMil
// Purpose:	load usno b1 star radius from online cat
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CNofsNavyMil::FetchUSNOB1StarsFromNavyMil( wxString& strFile, double nRa,
									   double nDec, double nRadius )
{
	int nState = 0;

//	FILE *pLocalFile;
	wxString strUrl;

	// now here we shoul d check first if we already have that info localy
	// for now just look for this specific file - in the future compute aprox location
	if( !wxFileExists( strFile ) )
	{
		// build name for the url to request
		strUrl.Printf( wxT("http://www.nofs.navy.mil/cgi-bin/vo_cone.cgi?CAT=USNO-B1&RA=%.12f&DEC=%.12f&SR=%.6f&VERB=1&clr=V&bri=%.6f&fai=%.6f&cftype=ASCII&slf=ddd.ddd%2Fdd.ddd&skey=Mag"),
							nRa, nDec, nRadius, 0.1, m_pCatalogStars->m_pSky->m_pConfig->m_nStarsCatMaxMagLimit );

		// call online 
		nState = LoadWebFile( strUrl, strFile );

	}

	return( nState );
}


/////////////////////////////////////////////////////////////////////
// Method:	GetStarsUSNOB1FromNavyMil
// Class:	CNofsNavyMil
// Purpose:	load sao star catalog
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CNofsNavyMil::GetUSNOB1StarsFromNavyMil( double nRa, double nDec,
									double nWidth, double nHeight )
{
	wxString strLog;
	wxString strLocalFile;
	int i=0, j=0;

	/////////////////////////////////////
	// calculate fetch radius
	double nFetchRadius = (double) ( (double) sqrt( (double) nWidth*nWidth + (double) nHeight*nHeight )/2 );

	/////////////////
	// here we need to calculate max rad based on VB
	double nMaxRad = 1.5;

	///////////
	// free current if any
	if( m_pCatalogStars->m_vectStar )
	{
		m_pCatalogStars->UnloadCatalog( );
	}
	// allocate my vector + 1 unit just to be sure
	m_pCatalogStars->m_nStarAllocated = 100000;
	m_pCatalogStars->m_vectStar = (StarDefCatBasic*) malloc( (m_pCatalogStars->m_nStarAllocated+1)*sizeof(StarDefCatBasic) );

	// set catalog stars to zero
	m_pCatalogStars->m_nStar = 0;

	if( nFetchRadius > nMaxRad  )
	{
		// set the star ra/dec
		double nRaStart = (double)(nRa - (double) (nWidth/2));
		double nDecStart = (double)(nDec - (double) (nHeight/2));

		double nOrigDec=0, nOrigRa=0;
		// calculate how many regions i need to fetch
		double nLength = (double) sqrt( (double) nMaxRad*nMaxRad*2 );
		int nRegionsX = (int) (nWidth/nLength) + 1;
		int nRegionsY = (int) (nHeight/nLength) + 1;

		// log info
		strLog.Printf( wxT("INFO :: there are %d regions to load"), nRegionsX*nRegionsY );
		m_pUnimapWorker->Log( strLog );

		for( i=0; i<nRegionsY; i++ )
		{
			nOrigDec = (double) (nDecStart + (double) (i*nLength) + (double) (nLength/2));

			for( j=0; j<nRegionsX; j++ )
			{
				nOrigRa = (double) (nRaStart + (double) (j*nLength) + (double) (nLength/2));

				// build name for local file
				strLocalFile.Printf( wxT("%s/usnob1_%.6f_%.6f_%.4f_%.4f%_%.4f_cat.txt"), unMakeAppPath(wxT("data/remote")),
										nOrigRa, nOrigDec, nMaxRad, nMaxRad, m_pCatalogStars->m_pSky->m_pConfig->m_nStarsCatMaxMagLimit );
				// log info
				strLog.Printf( wxT("INFO :: Fetching region (%d,%d) ..."), j, i );
				m_pUnimapWorker->Log( strLog );

				// fetch from us navy site
				FetchUSNOB1StarsFromNavyMil( strLocalFile, nOrigRa, nOrigDec, nMaxRad );

				// log info
				strLog.Printf( wxT("INFO :: Loading region (%d,%d) ..."), j, i );
				m_pUnimapWorker->Log( strLog );

				// and now load the file just downloaded
				LoadUSNB1FileFromNavyMil( strLocalFile );
			}
		}

	} else
	{
		// build name for local file
		strLocalFile.Printf( wxT("%s/usnob1_%.6f_%.6f_%.4f_%.4f%_%.4f_cat.txt"), unMakeAppPath(wxT("data/remote")),
								nRa, nDec, nWidth, nHeight, m_pCatalogStars->m_pSky->m_pConfig->m_nStarsCatMaxMagLimit );

		// log info
		strLog.Printf( wxT("INFO :: Fetching area orig(%.2f,%.2f) with radius=%.2f..."),
									nRa, nDec, nFetchRadius );
		m_pUnimapWorker->Log( strLog );

		FetchUSNOB1StarsFromNavyMil( strLocalFile, nRa, nDec, nFetchRadius );

		// log info
		strLog.Printf( wxT("INFO :: Loading area orig(%.2f,%.2f) with radius=%.2f..."),
									nRa, nDec, nFetchRadius );
		m_pUnimapWorker->Log( strLog );

		// and now load the file just downloaded
		LoadUSNB1FileFromNavyMil( strLocalFile );
	}
	// and now sort my catalog by magnitude
	m_pCatalogStars->SortByMagnitude( );

	// log message
	strLog.Printf( wxT("INFO :: loaded %d stars from USNB1 region"), m_pCatalogStars->m_nStar );
	m_pUnimapWorker->Log( strLog );

	return( 1 );
}


/////////////////////////////////////////////////////////////////////
// Method:	LoadUSNB1FileFromNavyMil
// Class:	CNofsNavyMil
// Purpose:	load USNB1 star catalog from navy mil
// Input:	file name
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CNofsNavyMil::LoadUSNB1FileFromNavyMil( const wxString& strFile )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
	wxRegEx reTychoNo = wxT( "([0-9]{4})\\-([0-9]{5})\\-([0-9]{1})" );
	wxRegEx reUsnoNo = wxT( "([0-9]{4})\\-([0-9]{7})" );
	// other variables used
	unsigned long nCatNo=0, nLongVar;
	unsigned char nCatType=0;
	unsigned int nZoneNo=0;
	unsigned char nCompNo=0;
	double nRaDeg=0, nDecDeg=0, nMag=0, nB1=0, nB2=0, nR1=0, nR2=0, nI2=0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}
	// Reading lines from cfg file
	while( !feof( pLocalFile ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pLocalFile );
		// set this to wx string
		wxString strWxLine = strLine;

		// if line empty jump
		if( wxStrlen( strLine ) )
			continue;

		// check if line is a comment jump
		if( !strWxLine.Mid( 0, 1 ).CmpNoCase( wxT("#") ) )
			continue;

		/////////////////////////////
		// here we extract field we need
		// get only the middle for now - i need to create a special
		// ca structure for the future
//		if( !strWxLine.Mid( 6, 5 ).ToULong( &nCatNo ) )
//			nCatNo = 0;

		// check for memory allocation
		if( m_pCatalogStars->m_nStar >= m_pCatalogStars->m_nStarAllocated  )
		{
			// incremen counter
			m_pCatalogStars->m_nStarAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogStars->m_vectStar = (StarDefCatBasic*) realloc( m_pCatalogStars->m_vectStar, (m_pCatalogStars->m_nStarAllocated+1)*sizeof(StarDefCatBasic) );
		}

		wxString strCatNo = strWxLine.Mid( 1, 12 );
		if( reTychoNo.Matches( strCatNo ) )
		{
			nCatType = CAT_OBJECT_TYPE_TYCHO;
			// get zone
			if( !reTychoNo.GetMatch(strCatNo, 1 ).ToULong( &nLongVar ) )
				nLongVar=0;
			nZoneNo = (unsigned int) nLongVar;
			// get cat no
			if( !reTychoNo.GetMatch(strCatNo, 2 ).ToULong( &nLongVar ) )
				nLongVar=0;
			nCatNo = nLongVar;
			// get comp no
			if( !reTychoNo.GetMatch(strCatNo, 3 ).ToULong( &nLongVar ) )
				nLongVar=0;
			nCompNo = (unsigned char) nLongVar;

		} else if( reUsnoNo.Matches( strCatNo ) )
		{
			nCatType = CAT_OBJECT_TYPE_USNO;
			// get zone
			if( !reUsnoNo.GetMatch(strCatNo, 1 ).ToULong( &nLongVar ) )
				nLongVar=0;
			nZoneNo = (unsigned int) nLongVar;
			// get cat no
			if( !reUsnoNo.GetMatch(strCatNo, 2 ).ToULong( &nLongVar ) )
				nLongVar=0;
			nCatNo = nLongVar;
			nCompNo = 0;

		} else
			continue;

		// now check if this is a dup
		if( m_pCatalogStars->IsCatStar( nCatType, nCatNo, nZoneNo, nCompNo ) != 0 )
			continue;

		// ra in deg 15-26
		if( !strWxLine.Mid( 14, 11 ).ToDouble( &nRaDeg ) )
			nRaDeg = 0;
		// Dec in deg 28-39
		if( !strWxLine.Mid( 26, 11 ).ToDouble( &nDecDeg ) )
			nDecDeg = 0;

		/////////////////////////////
		// maginuted part has 5 components R1,B1,R2,B2,I
		// get B1
		if( !strWxLine.Mid( 78, 7 ).ToDouble( &nB1 ) )
			nB1 = 0;
		// get R1
		if( !strWxLine.Mid( 86, 7 ).ToDouble( &nR1 ) )
			nR1 = 0;
		// get B2
		if( !strWxLine.Mid( 94, 7 ).ToDouble( &nB2 ) )
			nB2 = 0;
		// get R2
		if( !strWxLine.Mid( 102, 7 ).ToDouble( &nR2 ) )
			nR2 = 0;
		// get I2
		if( !strWxLine.Mid( 110, 7 ).ToDouble( &nI2 ) )
			nI2 = 0;

		// and now calculate magnitude
		// d15=3880, c21==GROWTH(C17:C19,C12:C14,C15,1)
		// c17 = (D12*POWER(10,(-C7/2.5)))
		// c19 = =(D14*POWER(10,(-C9/2.5)))
		// c12 = 440, c14 = 880, c15 = 550
		// d12 = 4400
		// c7 = =(C3+E3)/2 .... c3 = b1, e3=b2
		//  d14 = 2430
		// c9 = i = i2 ??
		// nMag = =(double) -2.5*log10(C21/D15)

		// calculate simple mag from usno b1
		nMag = (double) ( (double) 0.444*nB1 + (double) 0.556*nR1 );

		// set star
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].cat_no = nCatNo;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].zone_no = nZoneNo;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].comp_no = nCompNo;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].cat_type = nCatType;

		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].ra = nRaDeg;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].dec = nDecDeg;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].mag = nMag;

		// calculate min and max --- ???? do i need or just delete ?!!!!
		if( m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].ra < m_pCatalogStars->m_nMinX ) m_pCatalogStars->m_nMinX = m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].ra;
		if( m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].ra > m_pCatalogStars->m_nMaxX ) m_pCatalogStars->m_nMaxX = m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].ra;

		if( m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].dec < m_pCatalogStars->m_nMinY ) m_pCatalogStars->m_nMinY = m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].dec;
		if( m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].dec > m_pCatalogStars->m_nMaxY ) m_pCatalogStars->m_nMaxY = m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].dec;

		// increase star counter
		m_pCatalogStars->m_nStar++;
	}
	// close my file
	fclose( pLocalFile );

	m_pCatalogStars->m_nCatalogLoaded = CATALOG_ID_USNO_B1;

	return( 1 );
}
