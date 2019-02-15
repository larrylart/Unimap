////////////////////////////////////////////////////////////////////
// package to handle Vizier data fetch for catalogs etc
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// system libs
#include <stdio.h>
//#include <malloc.h>
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
#include "catalog_stars.h"
#include "catalog_dso.h"
#include "catalog_radio.h"
#include "catalog_xgamma.h"
#include "catalog_supernovas.h"
#include "catalog_blackholes.h"
#include "catalog_mstars.h"
#include "catalog_asteroids.h"
#include "catalog_comets.h"
#include "../unimap.h"
#include "../util/func.h"
#include "../unimap_worker.h"
// spec catalogs
#include "catalog_xgamma_integral.h"

// main header
#include "vizier.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CVizier
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CVizier::CVizier( CSkyCatalogStars* pCatalogStars, CUnimapWorker* pWorker )
{
	m_pCatalogStars = pCatalogStars;
	m_pUnimapWorker = pWorker;

	Init( );
}

/////////////////////////////////////////////////////////////////////
CVizier::CVizier( CSkyCatalogDso* pCatalogDso, CUnimapWorker* pWorker )
{
	m_pCatalogDso = pCatalogDso;
	m_pUnimapWorker = pWorker;

	Init( );
}

/////////////////////////////////////////////////////////////////////
CVizier::CVizier( CSkyCatalogRadio* pCatalogRadio, CUnimapWorker* pWorker )
{
	m_pCatalogRadio = pCatalogRadio;
	m_pUnimapWorker = pWorker;

	Init( );
}

/////////////////////////////////////////////////////////////////////
CVizier::CVizier( CSkyCatalogXGamma* pCatalogXGamma, CUnimapWorker* pWorker )
{
	m_pCatalogXGamma = pCatalogXGamma;
	m_pUnimapWorker = pWorker;

	Init( );
}

/////////////////////////////////////////////////////////////////////
CVizier::CVizier( CSkyCatalogSupernovas* pCatalogSupernovas, CUnimapWorker* pWorker )
{
	m_pCatalogSupernovas = pCatalogSupernovas;
	m_pUnimapWorker = pWorker;

	Init( );
}

/////////////////////////////////////////////////////////////////////
CVizier::CVizier( CSkyCatalogBlackholes* pCatalogBlackholes, CUnimapWorker* pWorker )
{
	m_pCatalogBlackholes = pCatalogBlackholes;
	m_pUnimapWorker = pWorker;

	Init( );
}

/////////////////////////////////////////////////////////////////////
CVizier::CVizier( CSkyCatalogMStars* pCatalogMStars, CUnimapWorker* pWorker )
{
	m_pCatalogMStars = pCatalogMStars;
	m_pUnimapWorker = pWorker;

	Init( );
}

/////////////////////////////////////////////////////////////////////
CVizier::CVizier( CSkyCatalogAsteroids* pCatalogAsteroids, CUnimapWorker* pWorker )
{
	m_pCatalogAsteroids = pCatalogAsteroids;
	m_pUnimapWorker = pWorker;

	Init( );
}

/////////////////////////////////////////////////////////////////////
CVizier::CVizier( CSkyCatalogComets* pCatalogComets, CUnimapWorker* pWorker )
{
	m_pCatalogComets = pCatalogComets;
	m_pUnimapWorker = pWorker;

	Init( );
}


/////////////////////////////////////////////////////////////////////
void CVizier::Init( )
{
	// set sites
	m_vectSite[0] = wxT("http://vizier.u-strasbg.fr");
	m_vectSite[1] = wxT("http://vizier.ast.cam.ac.uk/vizier");
	m_vectSite[2] = wxT("http://vizier.cfa.harvard.edu/vizier");
	m_vectSite[3] = wxT("http://vizier.inasan.ru/vizier");
	m_vectSite[4] = wxT("http://vizier.nao.ac.jp/vizier");
	m_vectSite[5] = wxT("http://vizier.hia.nrc.ca/vizier");
	m_vectSite[6] = wxT("http://vizier.iucaa.ernet.in/vizier");
	m_vectSite[7] = wxT("http://data.bao.ac.cn/vizier");
	m_vectSite[8] = wxT("http://www.ukirt.jach.hawaii.edu/vizier");
	m_nSite = 9;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CVizier
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CVizier::~CVizier( )
{

}

/////////////////////////////////////////////////////////////////////
// Method:	LoadGSCFileFromVizier
// Class:	CVizier
// Purpose:	load GSC 2.3 star catalog from Vizier file
// Input:	file name
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CVizier::LoadGSCFileFromVizier( const wxString& strFile )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = wxT("");
	// other variables used
	unsigned char nCatType=0;
	unsigned long nObjType=0;
	double nRaDeg=0, nDecDeg=0, nMag=0, nMagF=0, nMagJ=0, nMagV=0, nMagN=0, nMagU=0, nMagB=0;
	int bHeaderEnd=0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// do (un)allocation
	m_pCatalogStars->UnloadCatalog( );
	// main allocation
	m_pCatalogStars->m_nStarAllocated = 100000;
	m_pCatalogStars->m_vectStar = (StarDefCatBasic*) malloc( m_pCatalogStars->m_nStarAllocated*sizeof(StarDefCatBasic) );

	// Reading lines from cfg file
	while( !feof( pLocalFile ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pLocalFile );
		// set this to wx string
		wxString strWxLine = strLine;

		//////////////////////
		// check if line is a comment jump
		if( strLine[0] == '#' || strLine[0] == ' ' )
			continue;
		// if line empty (less the a minimum of 10 lets say - jump 
		if( wxStrlen( strLine ) < 10 )
			continue;
		// check for second line == end header
		if( strLine[0] == '-' )
		{ 
			bHeaderEnd++;
			continue;
		}
		// add extra check for header pass -dirty hack
		if( bHeaderEnd < 2 ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogStars->m_nStar >= m_pCatalogStars->m_nStarAllocated  )
		{
			// incremen counter
			m_pCatalogStars->m_nStarAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogStars->m_vectStar = (StarDefCatBasic*) realloc( m_pCatalogStars->m_vectStar, (m_pCatalogStars->m_nStarAllocated+1)*sizeof(StarDefCatBasic) );
		}

		// get star cat name
		wxString strCatName = strWxLine.Mid( 0, 10 ).Trim(0).Trim(1);
		nCatType = CAT_OBJECT_TYPE_GSC;

		// ra in deg 15-26
		if( !strWxLine.Mid( 11, 10 ).ToDouble( &nRaDeg ) )
			nRaDeg = 0;
		// Dec in deg 28-39
		if( !strWxLine.Mid( 22, 10 ).ToDouble( &nDecDeg ) )
			nDecDeg = 0;

		/////////////////////////////
		// maginutes - lots of them - todo: get a better mixing mecanism

		// :: get Fmag - Magnitude in F photographic band (red)
		if( !strWxLine.Mid( 33, 5 ).ToDouble( &nMagF ) ) nMagF = 0.0;
		// :: get Vmag - Magnitude in V photographic band (green) 
		if( !strWxLine.Mid( 45, 5 ).ToDouble( &nMagV ) ) nMagV = 0.0;
		// :: get Jmag - Magnitude in Bj photographic band (blue) 
		if( !strWxLine.Mid( 39, 5 ).ToDouble( &nMagJ ) ) nMagJ = 0.0;

		///////////////
		// damn magnitude cases
		if( nMagF != 0 && nMagJ != 0 && nMagV != 0 ) 
			nMag = (double) ( (double) 0.333*nMagF + (double) 0.333*nMagJ + (double) 0.334*nMagV );
		else if( nMagF != 0 && nMagJ != 0 )
			nMag = (double) ( (double) 0.500*nMagF + (double) 0.500*nMagJ );
		else if( nMagV != 0 && nMagF != 0 )
			nMag = (double) ( (double) 0.500*nMagV + (double) 0.500*nMagF );
		else if( nMagJ != 0 && nMagV != 0 )
			nMag = (double) ( (double) 0.500*nMagJ + (double) 0.500*nMagV );
		else if( nMagV != 0 )
			nMag = (double) nMagV;
		else if( nMagF != 0 )
			nMag = (double) nMagF;
		else if( nMagJ != 0 )
			nMag = (double) nMagJ;
		else
		{
			// :: get Bmag - Magnitude in B band (Johnson blue) 
			if( !strWxLine.Mid( 63, 5 ).ToDouble( &nMagB ) ) nMagB = 0.0;
			// :: get Nmag - Magnitude in N photographic band (0.8µm) 
			if( !strWxLine.Mid( 51, 5 ).ToDouble( &nMagN ) ) nMagN = 0.0;
			// :: get Umag - Magnitude in U band (Johnson) 
			if( !strWxLine.Mid( 57, 5 ).ToDouble( &nMagU ) ) nMagU = 0.0;

			/////////////////
			// second & last mag cases
			if( nMagB != 0 && nMagN != 0 && nMagU != 0 )
				nMag = (double) ( (double) 0.333*nMagB + (double) 0.333*nMagN + (double) 0.334*nMagU );
			else if( nMagB != 0 && nMagN != 0 )
				nMag = (double) ( (double) 0.500*nMagB + (double) 0.500*nMagN );
			else if( nMagB != 0 && nMagU != 0 )
				nMag = (double) ( (double) 0.500*nMagB + (double) 0.500*nMagU );
			else if( nMagN != 0 && nMagU != 0 )
				nMag = (double) ( (double) 0.500*nMagN + (double) 0.500*nMagU );
			else if( nMagB != 0 )
				nMag = (double) nMagB;
			else if( nMagN != 0 )
				nMag = (double) nMagN;
			else if( nMagU != 0 )
				nMag = (double) nMagU;
			else
				nMag = 9999.99;
		}

		// debug - skip high mag
//		if( nMag > 20.0 ) continue;

		// get class:  0=star 1=Galaxy, 2=Blend 3=Non-star 4=unclassified 5=Defect 
		if( !strWxLine.Mid( 69, 1 ).ToULong( &nObjType ) ) 
			nObjType = SKY_OBJECT_TYPE_NONE;
		else
		{
			if( nObjType == 0 )
				nObjType = SKY_OBJECT_TYPE_STAR;
			else if( nObjType == 1 )
				nObjType = SKY_OBJECT_TYPE_GALAXY;
			else if( nObjType == 2 )
				nObjType = SKY_OBJECT_TYPE_BLEND;
			else if( nObjType == 3 )
				nObjType = SKY_OBJECT_TYPE_NON_STAR;
			else if( nObjType == 4 )
				nObjType = SKY_OBJECT_TYPE_UNCLASSIFIED;
			else if( nObjType == 5 )
				nObjType = SKY_OBJECT_TYPE_DEFECT;
			else
				nObjType = SKY_OBJECT_TYPE_NONE;
		}

		// debug - skip non stars ?
//		if( nObjType != SKY_OBJECT_TYPE_STAR ) continue;

		// set star name
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].cat_name = NULL;
		m_pCatalogStars->SetStarStringCatalogName( m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar], strCatName.ToAscii() );
		// set other params
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].cat_no = 0;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].zone_no = 0;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].comp_no = 0;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].cat_type = nCatType;

		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].ra = nRaDeg;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].dec = nDecDeg;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].mag = nMag;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].type = nObjType;

		// increase star counter
		m_pCatalogStars->m_nStar++;
	}
	// close my file
	fclose( pLocalFile );

	m_pCatalogStars->m_nCatalogLoaded = CATALOG_ID_GSC;

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	Load2MASSFileFromVizier
// Class:	CVizier
// Purpose:	load 2MASS star catalog from Vizier file
// Input:	file name
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CVizier::Load2MASSFileFromVizier( const wxString& strFile )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
	// other variables used
	unsigned char nCatType=0;
	double nRaDeg=0, nDecDeg=0, nMag=0, nMagJ=0, nMagH=0, nMagK=0, nRed=0, nBlue=0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// do (un)allocation
	m_pCatalogStars->UnloadCatalog( );
	// main allocation
	m_pCatalogStars->m_nStarAllocated = 100000;
	m_pCatalogStars->m_vectStar = (StarDefCatBasic*) malloc( m_pCatalogStars->m_nStarAllocated*sizeof(StarDefCatBasic) );

	// Reading lines from cfg file
	while( !feof( pLocalFile ) )
	{
		// clear buffers
		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pLocalFile );
		// set this to wx string
		wxString strWxLine = strLine;

		//////////////////////
		// check if line is a comment jump
		if( strLine[0] == '#' || strLine[0] == ' ' || strLine[0] == '-' )
			continue;
		// if line empty (less the a minimum of 10 lets say - jump 
		if( wxStrlen( strLine ) < 10 )
			continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogStars->m_nStar >= m_pCatalogStars->m_nStarAllocated  )
		{
			// incremen counter
			m_pCatalogStars->m_nStarAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogStars->m_vectStar = (StarDefCatBasic*) realloc( m_pCatalogStars->m_vectStar, (m_pCatalogStars->m_nStarAllocated+1)*sizeof(StarDefCatBasic) );
		}

		// get star cat name
		wxString strCatName = strWxLine.Mid( 0, 17 ).Trim(0).Trim(1);
		nCatType = CAT_OBJECT_TYPE_2MASS;

		// ra in deg 15-26
		if( !strWxLine.Mid( 18, 10 ).ToDouble( &nRaDeg ) )
			nRaDeg = 0;
		// Dec in deg 28-39
		if( !strWxLine.Mid( 29, 10 ).ToDouble( &nDecDeg ) )
			nDecDeg = 0;

		/////////////////////////////
		// maginute could be red/blue or infrared - main ir
		// :: get red
		if( !strWxLine.Mid( 40, 5 ).ToDouble( &nRed ) ) nRed = 0;
		// :: get blue
		if( !strWxLine.Mid( 46, 5 ).ToDouble( &nBlue ) ) nBlue = 0;

		// :: get infrared j/h/k
		int p=0;
		// :: infrared :: J
		if( strWxLine.Mid( 52, 6 ).ToDouble( &nMagJ ) ) 
			p++;
		else
			nMagJ=0;

		// :: infrared :: H
		if( strWxLine.Mid( 59, 6 ).ToDouble( &nMagH ) ) 
			p++;
		else
			nMagH=0;

		// :: infrared :: K
		if( strWxLine.Mid( 66, 6 ).ToDouble( &nMagK ) )
			p++;
		else
			nMagK=0;

		// calculate average mag
		if( p > 0 )
			// todo :: find a better way here to get mag in infra-red 
			nMag = 2.0+(double) (nMagJ+nMagH+nMagK)/(double)p;
		else if( nRed != 0 && nBlue != 0 ) 
			nMag = (double) ( (double) 0.444*nBlue + (double) 0.556*nRed );
		else if( nRed != 0 )
			nMag = 1.0+1.05*nRed;
		else if( nBlue != 0 )
			nMag = 1.0+1.1*nBlue;
		else
			nMag = 99999.9;

		// set star name
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].cat_name = NULL;
		m_pCatalogStars->SetStarStringCatalogName( m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar], strCatName.ToAscii() );
		// set other params
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].cat_no = 0;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].zone_no = 0;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].comp_no = 0;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].cat_type = nCatType;

		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].ra = nRaDeg;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].dec = nDecDeg;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].mag = nMag;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].type = SKY_OBJECT_TYPE_STAR;

		// increase star counter
		m_pCatalogStars->m_nStar++;
	}
	// close my file
	fclose( pLocalFile );

	m_pCatalogStars->m_nCatalogLoaded = CATALOG_ID_2MASS;

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadNOMADFileFromVizier
// Class:	CVizier
// Purpose:	load NOMAD(v1) star catalog from Vizier file
// Input:	file name
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CVizier::LoadNOMADFileFromVizier( const wxString& strFile )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
	wxRegEx reTychoNo = wxT( "([0-9]{4})\\-([0-9]{5})\\-([0-9]{1})" );
	wxRegEx reUsnoNo = wxT( "([0-9]{4})\\-([0-9]{7})" );
	// other variables used
	unsigned long nCatNo=0, nLongVar;
	unsigned char nCatType=0;
	unsigned int nZoneNo=0;
	unsigned char nCompNo=0;
	double nRaDeg=0, nDecDeg=0, nMag=0, nMagJ=0, nMagH=0, nMagK=0, nRed=0, nBlue=0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// do (un)allocation
	m_pCatalogStars->UnloadCatalog( );
	// main allocation
	m_pCatalogStars->m_nStarAllocated = 100000;
	m_pCatalogStars->m_vectStar = (StarDefCatBasic*) malloc( m_pCatalogStars->m_nStarAllocated*sizeof(StarDefCatBasic) );

	// Reading lines from cfg file
	while( !feof( pLocalFile ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pLocalFile );
		// set this to wx string
		wxString strWxLine = strLine;

		//////////////////////
		// check if line is a comment jump
		if( strLine[0] == '#' || strLine[0] == ' ' || strLine[0] == '-' )
			continue;
		// if line empty (less the a minimum of 10 lets say - jump 
		if( wxStrlen( strLine ) < 10 )
			continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogStars->m_nStar >= m_pCatalogStars->m_nStarAllocated  )
		{
			// incremen counter
			m_pCatalogStars->m_nStarAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogStars->m_vectStar = (StarDefCatBasic*) realloc( m_pCatalogStars->m_vectStar, (m_pCatalogStars->m_nStarAllocated+1)*sizeof(StarDefCatBasic) );
		}

		// get star cat
		wxString strNOMADCatNo = strWxLine.Mid( 0, 12 ).Trim(0).Trim(1);
		wxString strTYCHOCatNo = strWxLine.Mid( 13, 12 ).Trim(0).Trim(1);
		// check if there is any tycho number
		if( strTYCHOCatNo.Length() > 6 )
		{
			nCatType = CAT_OBJECT_TYPE_TYCHO;
			// get zone
			if( !strTYCHOCatNo.Mid( 0, 4 ).ToULong( &nLongVar ) )
				nLongVar=0;
			nZoneNo = (unsigned int) nLongVar;
			// get cat no
			if( !strTYCHOCatNo.Mid( 5, 5 ).ToULong( &nLongVar ) )
				nLongVar=0;
			nCatNo = nLongVar;
			// get comp no
			if( !strTYCHOCatNo.Mid( 11, 1 ).ToULong( &nLongVar ) )
				nLongVar=0;
			nCompNo = (unsigned char) nLongVar;

		} else 
		{
			nCatType = CAT_OBJECT_TYPE_NOMAD;
			// get zone
			if( !strNOMADCatNo.Mid( 0, 4 ).ToULong( &nLongVar ) )
				nLongVar=0;
			nZoneNo = (unsigned int) nLongVar;
			// get cat no
			if( !strNOMADCatNo.Mid( 5, 7 ).ToULong( &nLongVar ) )
				nLongVar=0;
			nCatNo = nLongVar;
			nCompNo = 0;

		} 

		// ra in deg 15-26
		if( !strWxLine.Mid( 26, 11 ).ToDouble( &nRaDeg ) )
			nRaDeg = 0;
		// Dec in deg 28-39
		if( !strWxLine.Mid( 38, 11 ).ToDouble( &nDecDeg ) )
			nDecDeg = 0;

		/////////////////////////////
		// maginute could be visual/blue or infrared from 2mass - use in order best one
		// :: visual
		if( !strWxLine.Mid( 50, 6 ).ToDouble( &nMag ) )
		{
			// :: get red
			if( !strWxLine.Mid( 64, 6 ).ToDouble( &nRed ) ) nRed = 0;
			// :: get blue
			if( !strWxLine.Mid( 57, 6 ).ToDouble( &nBlue ) ) nBlue = 0;

			// case ::  both - or just one component
			if( nRed != 0 && nBlue != 0 ) 
			{
				nMag = (double) ( (double) 0.200*nBlue + (double) 0.800*nRed );
				//nMag = (double) ( (double) 0.444*nBlue + (double) 0.556*nRed );
				//nMag = (5.0 * nRed + 3.0 * nBlue) / 8.0;

			} else
			{
				//continue;
				int p=0;
				// :: infrared :: J
				if( strWxLine.Mid( 71, 6 ).ToDouble( &nMagJ ) ) 
					p++;
				else
					nMagJ=0;

				// :: infrared :: H
				if( strWxLine.Mid( 78, 6 ).ToDouble( &nMagH ) ) 
					p++;
				else
					nMagH=0;

				// :: infrared :: K
				if( strWxLine.Mid( 85, 6 ).ToDouble( &nMagK ) )
					p++;
				else
					nMagK=0;

				// calculate average mag
				if( p > 0 )
					// todo :: find a better way here to get mag in infra-red 
					nMag = 2.0+(double) (nMagJ+nMagH+nMagK)/(double)p;
				else if( nRed != 0 )
					nMag = 1.0+1.05*nRed;
				else if( nBlue != 0 )
					nMag = 1.0+1.1*nBlue;
				else
					nMag = 99999.9;
			}
		} 	

		//if( nMag >18.0 ) continue;

		// set star
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].cat_name = NULL;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].cat_no = nCatNo;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].zone_no = nZoneNo;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].comp_no = nCompNo;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].cat_type = nCatType;

		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].ra = nRaDeg;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].dec = nDecDeg;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].mag = nMag;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].type = SKY_OBJECT_TYPE_STAR;

		// increase star counter
		m_pCatalogStars->m_nStar++;
	}
	// close my file
	fclose( pLocalFile );

	m_pCatalogStars->m_nCatalogLoaded = CATALOG_ID_NOMAD;

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadUSNB1FileFromVizier
// Class:	CVizier
// Purpose:	load USNB1 star catalog from file from vizier
// Input:	file name
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CVizier::LoadUSNOB1FileFromVizier( const wxString& strFile )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
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

	// do (un)allocation
	m_pCatalogStars->UnloadCatalog( );
	// main allocation
	m_pCatalogStars->m_nStarAllocated = 100000;
	m_pCatalogStars->m_vectStar = (StarDefCatBasic*) malloc( m_pCatalogStars->m_nStarAllocated*sizeof(StarDefCatBasic) );

	// Reading lines from cfg file
	while( !feof( pLocalFile ) )
	{
		// clear buffers
		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pLocalFile );
		// set this to wx string
		wxString strWxLine = strLine;

		//////////////////////
		// check if line is a comment jump
		if( strLine[0] == '#' || strLine[0] == ' ' || strLine[0] == '-' )
			continue;
		// if line empty (less the a minimum of 10 lets say - jump 
		if( wxStrlen( strLine ) < 10 )
			continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogStars->m_nStar >= m_pCatalogStars->m_nStarAllocated  )
		{
			// incremen counter
			m_pCatalogStars->m_nStarAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogStars->m_vectStar = (StarDefCatBasic*) realloc( m_pCatalogStars->m_vectStar, (m_pCatalogStars->m_nStarAllocated+1)*sizeof(StarDefCatBasic) );
		}

		// get star cat
		wxString strUSNOB1CatNo = strWxLine.Mid( 0, 12 ).Trim(0).Trim(1);
		wxString strTYCHOCatNo = strWxLine.Mid( 13, 12 ).Trim(0).Trim(1);
		// check if there is any tycho number
		if( strTYCHOCatNo.Length() > 0 )
		{
			nCatType = CAT_OBJECT_TYPE_TYCHO;
			// get zone
			if( !strTYCHOCatNo.Mid( 0, 4 ).ToULong( &nLongVar ) )
				nLongVar=0;
			nZoneNo = (unsigned int) nLongVar;
			// get cat no
			if( !strTYCHOCatNo.Mid( 5, 5 ).ToULong( &nLongVar ) )
				nLongVar=0;
			nCatNo = nLongVar;
			// get comp no
			if( !strTYCHOCatNo.Mid( 11, 1 ).ToULong( &nLongVar ) )
				nLongVar=0;
			nCompNo = (unsigned char) nLongVar;

		} else 
		{
			nCatType = CAT_OBJECT_TYPE_USNO;
			// get zone
			if( !strUSNOB1CatNo.Mid( 0, 4 ).ToULong( &nLongVar ) )
				nLongVar=0;
			nZoneNo = (unsigned int) nLongVar;
			// get cat no
			if( !strUSNOB1CatNo.Mid( 5, 7 ).ToULong( &nLongVar ) )
				nLongVar=0;
			nCatNo = nLongVar;
			nCompNo = 0;

		} 

		// ra in deg 15-26
		if( !strWxLine.Mid( 26, 10 ).ToDouble( &nRaDeg ) )
			nRaDeg = 0;
		// Dec in deg 28-39
		if( !strWxLine.Mid( 37, 10 ).ToDouble( &nDecDeg ) )
			nDecDeg = 0;

		/////////////////////////////
		// maginuted part has 5 components R1,B1,R2,B2,I
		// get B1
		if( !strWxLine.Mid( 48, 5 ).ToDouble( &nB1 ) )
			nB1 = 0;
		// get R1
		if( !strWxLine.Mid( 54, 5 ).ToDouble( &nR1 ) )
			nR1 = 0;
		// get B2
		if( !strWxLine.Mid( 60, 5 ).ToDouble( &nB2 ) )
			nB2 = 0;
		// get R2
		if( !strWxLine.Mid( 66, 5 ).ToDouble( &nR2 ) )
			nR2 = 0;
		// get I2
//		if( !strWxLine.Mid( 110, 7 ).ToDouble( &nI2 ) )
//			nI2 = 0;
/*
		// calculate simple mag from usno b1
		if( nB1 == 0 )
		{
			if( nR1 == 0 )
				if( nB2 == 0 )
					if( nR2 == 0 )
						nMag = 100;
					else
						nMag = nR2;
				else if( nR2 == 0 )
					nMag = nB2;
				else
					nMag = (double) ( (double) 0.444*nB2 + (double) 0.556*nR2 );
			else
				nMag = nR1;

		} else if( nR1 == 0 )
		{
			if( nR2 == 0 )
				nMag = nB1;
			else if( nB2 == 0 )
				nMag = (double) ( (double) 0.444*nB1 + (double) 0.556*nR2 );
			else
				nMag = (double) ( (double) 0.444*nB2 + (double) 0.556*nR2 );

		} else
			nMag = (double) ( (double) 0.444*nB1 + (double) 0.556*nR1 );
*/
		///////////////
		// damn magnitude cases
		if( nR1 != 0 && nB1 != 0 ) 
			nMag = (double) ( (double) 0.444*nB1 + (double) 0.556*nR1 );
		else if( nR2 != 0 && nB2 != 0 )
			nMag = (double) ( (double) 0.444*nB2 + (double) 0.556*nR2 );
		else
			continue;

/*
		else if( nR1 != 0 && nB2 != 0 ) 
			nMag = (double) ( (double) 0.444*nB2 + (double) 0.556*nR1 );
		else if( nR2 != 0 && nB1 != 0 )
			nMag = (double) ( (double) 0.444*nB1 + (double) 0.556*nR2 );
		else
			continue;

		else if( nR1 != 0 )
			nMag = nR1;
		else if( nR2 != 0 )
			nMag = nR2;
		else if( nB1 != 0 )
			nMag = nB1;
		else if( nB2 != 0 )
			nMag = nB2;
		else
			nMag = 100;
*/
		// debug - verify
		if( nMag == 0 )
			nMag = 101;

		// set star
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].cat_name = NULL;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].cat_no = nCatNo;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].zone_no = nZoneNo;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].comp_no = nCompNo;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].cat_type = nCatType;

		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].ra = nRaDeg;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].dec = nDecDeg;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].mag = nMag;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].type = SKY_OBJECT_TYPE_STAR;

		// increase star counter
		m_pCatalogStars->m_nStar++;
	}
	// close my file
	fclose( pLocalFile );

	m_pCatalogStars->m_nCatalogLoaded = CATALOG_ID_USNO_B1;

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadSAOFileFromVizier
// Class:	CVizier
// Purpose:	load SAO star catalog from file from vizier
// Input:	file name
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CVizier::LoadSAOFileFromVizier( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
	// other variables used
	unsigned long nCatNo=0, nLongVar;
	double nRaDeg=0, nDecDeg=0, nMag=0;
	int bFoundStart=0;
	int bError=0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// do (un)allocation
	m_pCatalogStars->UnloadCatalog( );
	// main allocation
	if( bRegion )
		m_pCatalogStars->m_nStarAllocated = 10000;
	else
		m_pCatalogStars->m_nStarAllocated = 259027;
	m_pCatalogStars->m_vectStar = (StarDefCatBasic*) malloc( m_pCatalogStars->m_nStarAllocated*sizeof(StarDefCatBasic) );

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
		if( strLine[0] == '#' )
			continue;
		
		// check for last line before data
		if( !bFoundStart && strLine[0] == '-' && strLine[1] == '-' && strLine[2] == '-' )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogStars->m_nStar >= m_pCatalogStars->m_nStarAllocated  )
		{
			// incremen counter
			m_pCatalogStars->m_nStarAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogStars->m_vectStar = (StarDefCatBasic*) realloc( m_pCatalogStars->m_vectStar, (m_pCatalogStars->m_nStarAllocated+1)*sizeof(StarDefCatBasic) );
		}

		// set this to wx string
		wxString strWxLine = strLine;

		// get star cat
		wxString strCatNo = strWxLine.Mid( 0, 6 ).Trim(0).Trim(1);
		if( !strCatNo.ToULong( &nCatNo ) ) 
				continue;
		// conver ra/dec from hexagesimal to decimal
		ConvertRaToDeg( strWxLine.Mid( 7, 12 ).Trim(0).Trim(1), nRaDeg );
		ConvertDecToDeg( strWxLine.Mid( 20, 12 ).Trim(0).Trim(1), nDecDeg );
		// check if photo mag else visual else a small number
		if( !strWxLine.Mid( 33, 4 ).Trim(0).Trim(1).ToDouble( &nMag ) )
			if( !strWxLine.Mid( 38, 4 ).Trim(0).Trim(1).ToDouble( &nMag ) )
				nMag = 9999.0;

		// set star
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].cat_name = NULL;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].cat_no = nCatNo;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].zone_no = 0;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].comp_no = 0;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].cat_type = CAT_OBJECT_TYPE_SAO;

		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].ra = nRaDeg;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].dec = nDecDeg;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].mag = nMag;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].type = SKY_OBJECT_TYPE_STAR;

		// increase star counter
		m_pCatalogStars->m_nStar++;

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
		m_pCatalogStars->m_nCatalogLoaded = CATALOG_ID_SAO;
		return( m_pCatalogStars->m_nStar );

	} else
	{
		m_pCatalogStars->UnloadCatalog( );
		m_pCatalogStars->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadHIPPARCOSFileFromVizier
// Class:	CVizier
// Purpose:	load HIPPARCOS star catalog from file from vizier
// Input:	file name
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CVizier::LoadHIPPARCOSFileFromVizier( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
	// other variables used
	unsigned long nCatNo=0, nLongVar;
	double nRaDeg=0, nDecDeg=0, nMag=0;
	int bFoundStart=0;
	int bError=0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile ) return( 0 );

	// do (un)allocation
	m_pCatalogStars->UnloadCatalog( );
	// main allocation
	if( bRegion )
		m_pCatalogStars->m_nStarAllocated = 10209;
	else
		m_pCatalogStars->m_nStarAllocated = 118209;
	m_pCatalogStars->m_vectStar = (StarDefCatBasic*) malloc( m_pCatalogStars->m_nStarAllocated*sizeof(StarDefCatBasic) );

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
		if( strLine[0] == '#' )
			continue;
		
		// check for last line before data
		if( !bFoundStart && strLine[0] == '-' && strLine[1] == '-' && strLine[2] == '-' )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogStars->m_nStar >= m_pCatalogStars->m_nStarAllocated  )
		{
			// incremen counter
			m_pCatalogStars->m_nStarAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogStars->m_vectStar = (StarDefCatBasic*) realloc( m_pCatalogStars->m_vectStar, (m_pCatalogStars->m_nStarAllocated+1)*sizeof(StarDefCatBasic) );
		}

		// set this to wx string
		wxString strWxLine = strLine;

		// get star cat
		wxString strCatNo = strWxLine.Mid( 0, 6 ).Trim(0).Trim(1);
		if( !strCatNo.ToULong( &nCatNo ) ) 
				continue;
		// conver ra/dec from hexagesimal to decimal
		ConvertRaToDeg( strWxLine.Mid( 7, 12 ).Trim(0).Trim(1), nRaDeg );
		ConvertDecToDeg( strWxLine.Mid( 20, 12 ).Trim(0).Trim(1), nDecDeg );
		// check if photo mag else visual else a small number
		if( !strWxLine.Mid( 33, 6 ).Trim(0).Trim(1).ToDouble( &nMag ) ) nMag = 9999.0;

		// set star
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].cat_name = NULL;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].cat_no = nCatNo;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].zone_no = 0;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].comp_no = 0;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].cat_type = CAT_OBJECT_TYPE_HIPPARCOS;

		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].ra = nRaDeg;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].dec = nDecDeg;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].mag = nMag;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].type = SKY_OBJECT_TYPE_STAR;

		// increase star counter
		m_pCatalogStars->m_nStar++;

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
		m_pCatalogStars->m_nCatalogLoaded = CATALOG_ID_HIPPARCOS;
		return( m_pCatalogStars->m_nStar );

	} else
	{
		m_pCatalogStars->UnloadCatalog( );
		m_pCatalogStars->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadTYCHOFileFromVizier
// Class:	CVizier
// Purpose:	load TYCHO-2 star catalog from file from vizier
// Input:	file name
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CVizier::LoadTYCHOFileFromVizier( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
	// other variables used
	unsigned long nCatNo=0, nZoneNo=0, nCompNo=0, nLongVar;
	double nRaDeg=0, nDecDeg=0, nMag=0;
	int bFoundStart=0;
	int bError=0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile ) return( 0 );

	// do (un)allocation
	m_pCatalogStars->UnloadCatalog( );
	// main allocation
	if( bRegion )
		m_pCatalogStars->m_nStarAllocated = 10000;
	else
		m_pCatalogStars->m_nStarAllocated = 2539913;
	m_pCatalogStars->m_vectStar = (StarDefCatBasic*) malloc( m_pCatalogStars->m_nStarAllocated*sizeof(StarDefCatBasic) );

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
		if( strLine[0] == '#' )
			continue;
		
		// check for last line before data
		if( !bFoundStart && strLine[0] == '-' && strLine[1] == '-' && strLine[2] == '-' )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogStars->m_nStar >= m_pCatalogStars->m_nStarAllocated  )
		{
			// incremen counter
			m_pCatalogStars->m_nStarAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogStars->m_vectStar = (StarDefCatBasic*) realloc( m_pCatalogStars->m_vectStar, (m_pCatalogStars->m_nStarAllocated+1)*sizeof(StarDefCatBasic) );
		}

		// set this to wx string
		wxString strWxLine = strLine;

		// get star cat
		if( !strWxLine.Mid( 0, 4 ).Trim(0).Trim(1).ToULong( &nZoneNo ) ) continue;
		if( !strWxLine.Mid( 5, 5 ).Trim(0).Trim(1).ToULong( &nCatNo ) ) continue;
		if( !strWxLine.Mid( 11, 1 ).Trim(0).Trim(1).ToULong( &nCompNo ) ) continue;
		// conver ra/dec from hexagesimal to decimal
		// this used to be different ? now ???
		//ConvertRaToDeg( strWxLine.Mid( 13, 12 ).Trim(0).Trim(1), nRaDeg );
		//ConvertDecToDeg( strWxLine.Mid( 16, 12 ).Trim(0).Trim(1), nDecDeg );
		// ra in deg 
		if( !strWxLine.Mid( 13, 12 ).ToDouble( &nRaDeg ) ) nRaDeg = 0.0;
		// Dec in deg 
		if( !strWxLine.Mid( 26, 12 ).ToDouble( &nDecDeg ) ) nDecDeg = 0.0;

		// check if photo mag else visual else a small number
		if( !strWxLine.Mid( 39, 6 ).Trim(0).Trim(1).ToDouble( &nMag ) ) nMag = 9999.0;

		// set star
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].cat_name = NULL;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].cat_no = nCatNo;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].zone_no = (unsigned int) nZoneNo;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].comp_no = (unsigned char) nCompNo;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].cat_type = CAT_OBJECT_TYPE_TYCHO;

		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].ra = nRaDeg;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].dec = nDecDeg;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].mag = nMag;
		m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].type = SKY_OBJECT_TYPE_STAR;

		// increase star counter
		m_pCatalogStars->m_nStar++;

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
		m_pCatalogStars->m_nCatalogLoaded = CATALOG_ID_TYCHO_2;
		return( m_pCatalogStars->m_nStar );

	} else
	{
		m_pCatalogStars->UnloadCatalog( );
		m_pCatalogStars->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadHyperledaFileFromVizier
// Class:	CVizier
// Purpose:	load HYPERLEDA dso catalog from file from vizier
// Input:	file name, region flag
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CVizier::LoadHyperledaFileFromVizier( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
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
	// main allocation
	if( bRegion )
		m_pCatalogDso->m_nDsoAllocated = 10000;
	else
		m_pCatalogDso->m_nDsoAllocated = 259027;
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
		if( strLine[0] == '#' )
			continue;
		
		// check for last line before data
		if( !bFoundStart && strLine[0] == '-' && strLine[1] == '-' && strLine[2] == '-' )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogDso->m_nDso >= m_pCatalogDso->m_nDsoAllocated  )
		{
			// incremen counter
			m_pCatalogDso->m_nDsoAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogDso->m_vectDso = (DsoDefCatBasic*) realloc( m_pCatalogDso->m_vectDso, (m_pCatalogDso->m_nDsoAllocated+1)*sizeof(DsoDefCatBasic) );
		}

		// set this to wx string
		wxString strWxLine = strLine;

		// get star cat
		wxString strCatNo = strWxLine.Mid( 0, 7 ).Trim(0).Trim(1);
		if( !strCatNo.ToULong( &nCatNo ) ) 
				continue;
		// conver ra/dec from hexagesimal to decimal
		ConvertRaToDeg( strWxLine.Mid( 8, 10 ).Trim(0).Trim(1), nRaDeg );
		ConvertDecToDeg( strWxLine.Mid( 19, 9 ).Trim(0).Trim(1), nDecDeg );

		// :: A -> nLogD25
		if( !strWxLine.Mid( 29, 5 ).Trim(0).Trim(1).ToDouble( &nLogD25 ) )
			nLogD25 = 0.0;
		// :: B -> nLogR25
		if( !strWxLine.Mid( 35, 5 ).Trim(0).Trim(1).ToDouble( &nLogR25 ) )
			nLogR25 = 0.0;
		// :: PA - angle
		if( !strWxLine.Mid( 41, 4 ).Trim(0).Trim(1).ToDouble( &nPa ) )
			nPa = 0.0;

		// :: type
		wxString strType = strWxLine.Mid( 46, 5 ).Trim(0).Trim(1);

		// calculate a and b
		nA = (double) 0.1 * pow( 10.0, nLogD25 );
		nB = (double) (nA / pow( 10.0, nLogR25 ) )/2.0;
		nA = (double) nA/2.0;

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
// Method:	LoadMgcFileFromVizier
// Class:	CVizier
// Purpose:	load MGC dso catalog from file from vizier
// Input:	file name
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CVizier::LoadMgcFileFromVizier( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
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
	// main allocation
	if( bRegion )
		m_pCatalogDso->m_nDsoAllocated = 10000;
	else
		m_pCatalogDso->m_nDsoAllocated = 69594;
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
		if( strLine[0] == '#' )
			continue;
		
		// check for last line before data
		if( !bFoundStart && strLine[0] == '-' && strLine[1] == '-' && strLine[2] == '-' )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogDso->m_nDso >= m_pCatalogDso->m_nDsoAllocated  )
		{
			// incremen counter
			m_pCatalogDso->m_nDsoAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogDso->m_vectDso = (DsoDefCatBasic*) realloc( m_pCatalogDso->m_vectDso, (m_pCatalogDso->m_nDsoAllocated+1)*sizeof(DsoDefCatBasic) );
		}

		// set this to wx string
		wxString strWxLine = strLine;

		// get star cat
		wxString strCatNo = strWxLine.Mid( 0, 5 ).Trim(0).Trim(1);
		if( !strCatNo.ToULong( &nCatNo ) ) 
				continue;

		// :: RA
		if( !strWxLine.Mid( 6, 9 ).Trim(0).Trim(1).ToDouble( &nRaDeg ) )
			nRaDeg = 0;
		// :: DEC
		if( !strWxLine.Mid( 16, 9 ).Trim(0).Trim(1).ToDouble( &nDecDeg ) )
			nDecDeg = 0;

		// :: A 
		if( !strWxLine.Mid( 26, 7 ).Trim(0).Trim(1).ToDouble( &nA ) )
			nA = 0.0;
		// :: B -> nLogR25
		if( !strWxLine.Mid( 34, 5 ).Trim(0).Trim(1).ToDouble( &nB ) )
			nB = 0.0;
		// :: PA - angle
		if( !strWxLine.Mid( 42, 5 ).Trim(0).Trim(1).ToDouble( &nPa ) )
			nPa = 0.0;

		// :: Bmag
		if( !strWxLine.Mid( 48, 6 ).Trim(0).Trim(1).ToDouble( &nMag ) )
			nMag = 0.0;

		// set star
		//m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].cat_name = NULL;
		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].cat_no = nCatNo;
//		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].zone_no = 0;
//		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].comp_no = 0;
		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].cat_type = CAT_OBJECT_TYPE_MGC;

		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].ra = nRaDeg;
		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].dec = nDecDeg;
		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].mag = nMag;
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
		m_pCatalogDso->m_nCatalogLoaded = CATALOG_ID_MGC;
		return( m_pCatalogDso->m_nDso );

	} else
	{
		m_pCatalogDso->UnloadCatalog( );
		m_pCatalogDso->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadUgcFileFromVizier
// Class:	CVizier
// Purpose:	load UGC dso catalog from file from vizier
// Input:	file name, region flag
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::LoadUgcFileFromVizier( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
	// other variables used
	unsigned long nCatNo=0, nLongVar, nInclination;
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
	// main allocation
	if( bRegion )
		m_pCatalogDso->m_nDsoAllocated = 500;
	else
		m_pCatalogDso->m_nDsoAllocated = 12939;
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
		if( strLine[0] == '#' )
			continue;
		
		// check for last line before data
		if( !bFoundStart && strLine[0] == '-' && strLine[1] == '-' && strLine[2] == '-' )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogDso->m_nDso >= m_pCatalogDso->m_nDsoAllocated  )
		{
			// incremen counter
			m_pCatalogDso->m_nDsoAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogDso->m_vectDso = (DsoDefCatBasic*) realloc( m_pCatalogDso->m_vectDso, (m_pCatalogDso->m_nDsoAllocated+1)*sizeof(DsoDefCatBasic) );
		}

		// set this to wx string
		wxString strWxLine = strLine;

		// get star cat
		wxString strCatNo = strWxLine.Mid( 0, 5 ).Trim(0).Trim(1);
		if( !strCatNo.ToULong( &nCatNo ) ) 
				continue;

		// conver ra/dec from hexagesimal to decimal
		ConvertRaToDeg( strWxLine.Mid( 20, 7 ).Trim(0).Trim(1), nRaDeg );
		ConvertDecToDeg( strWxLine.Mid( 28, 6 ).Trim(0).Trim(1), nDecDeg );
		// convert from b1950 to j2000
		m_pCatalogDso->m_pSky->Convert_SkyCoord_B1950_to_J2000( nRaDeg, nDecDeg );

		// :: A 
		if( !strWxLine.Mid( 35, 6 ).Trim(0).Trim(1).ToDouble( &nA ) )
			nA = 0.0;
		// :: B 
		if( !strWxLine.Mid( 42, 5 ).Trim(0).Trim(1).ToDouble( &nB ) )
			nB = 0.0;
		// :: PA - angle
		if( !strWxLine.Mid( 48, 3 ).Trim(0).Trim(1).ToDouble( &nPa ) )
			nPa = 0.0;

		// :: Pmag
		if( !strWxLine.Mid( 52, 4 ).Trim(0).Trim(1).ToDouble( &nMag ) )
			nMag = 0.0;

		// :: galaxy plane Inclination - todo: find how to use
		if( !strWxLine.Mid( 52, 4 ).Trim(0).Trim(1).ToULong( &nInclination ) )
			nInclination = 0.0;
		 
		// :: hubble morph type - todo: implement in object type
		wxString strMType = strWxLine.Mid( 59, 7 ).Trim(0).Trim(1);

		// set star
		//m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].cat_name = NULL;
		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].cat_no = nCatNo;
//		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].zone_no = 0;
//		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].comp_no = 0;
		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].cat_type = CAT_OBJECT_TYPE_UGC;

		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].ra = nRaDeg;
		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].dec = nDecDeg;
		m_pCatalogDso->m_vectDso[m_pCatalogDso->m_nDso].mag = nMag;
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
		m_pCatalogDso->m_nCatalogLoaded = CATALOG_ID_UGC;
		return( m_pCatalogDso->m_nDso );

	} else
	{
		m_pCatalogDso->UnloadCatalog( );
		m_pCatalogDso->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadSpecfindFileFromVizier
// Class:	CVizier
// Purpose:	load SPECFIND radio catalog from file from vizier
// Input:	file name, region flag
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::LoadSpecfindFileFromVizier( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
	// other variables used
	unsigned long nCatNo=0, nLongVar, nNPS;
	double nRaDeg=0, nDecDeg=0, nFreq=0, nFdFreq=0, nSpecSlop=0, nSpecAbscissa=0;
	int bFoundStart=0;
	int bError=0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// do (un)allocation
	m_pCatalogRadio->UnloadCatalog( );
	// main allocation
	if( bRegion )
		m_pCatalogRadio->m_nAllocated = 5000;
	else
		m_pCatalogRadio->m_nAllocated = 551852;
	m_pCatalogRadio->m_vectData = (DefCatBasicRadio*) malloc( m_pCatalogRadio->m_nAllocated*sizeof(DefCatBasicRadio) );

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
		if( strLine[0] == '#' )
			continue;
		
		// check for last line before data
		if( !bFoundStart && strLine[0] == '-' && strLine[1] == '-' && strLine[2] == '-' )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogRadio->m_nData >= m_pCatalogRadio->m_nAllocated  )
		{
			// incremen counter
			m_pCatalogRadio->m_nAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogRadio->m_vectData = (DefCatBasicRadio*) realloc( m_pCatalogRadio->m_vectData, (m_pCatalogRadio->m_nAllocated+1)*sizeof(DefCatBasicRadio) );
		}

		// set this to wx string
		wxString strWxLine = strLine;

		// get star cat
		wxString strCatNo = strWxLine.Mid( 0, 6 ).Trim(0).Trim(1);
		if( !strCatNo.ToULong( &nCatNo ) ) 
				continue;

		// :: name
		wxString strName = strWxLine.Mid( 7, 22 ).Trim(0).Trim(1);

		// :: RA
		if( !strWxLine.Mid( 30, 8 ).Trim(0).Trim(1).ToDouble( &nRaDeg ) )
			nRaDeg = 0;
		// :: DEC
		if( !strWxLine.Mid( 39, 8 ).Trim(0).Trim(1).ToDouble( &nDecDeg ) )
			nDecDeg = 0;

		// :: N - Number of points in spectrum
		if( !strWxLine.Mid( 48, 2 ).Trim(0).Trim(1).ToULong( &nNPS ) )
			nNPS = 0;

		// :: a - Slope of spectrum 
		if( !strWxLine.Mid( 51, 6 ).Trim(0).Trim(1).ToDouble( &nSpecSlop ) )
			nSpecSlop = 0.0;
		// :: b - Abscissa of spectrum 
		if( !strWxLine.Mid( 58, 6 ).Trim(0).Trim(1).ToDouble( &nSpecAbscissa ) )
			nSpecAbscissa = 0.0;

		// :: NU - Frequency Mhz
		if( !strWxLine.Mid( 65, 5 ).Trim(0).Trim(1).ToDouble( &nFreq ) )
			nFreq = 0.0;
		// :: SNU - Flux density at frequency nu - mJy
		if( !strWxLine.Mid( 71, 9 ).Trim(0).Trim(1).ToDouble( &nFdFreq ) )
			nFdFreq = 0.0;
		 
		// set radio source
		// :: name - i should have allocated strlen(strName) - but because of export binary - i wont
//#ifndef _DEBUG
//		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_name = (char*) malloc( 23*sizeof(char) );
//#endif
		bzero( m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_name, 24 );
		strcpy( m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_name, strName.ToAscii() );
		// :: cat no and type 
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_no = nCatNo;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_type = CAT_OBJECT_TYPE_SPECFIND;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].type = SKY_OBJECT_TYPE_RADIO;

		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].ra = nRaDeg;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].dec = nDecDeg;

		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].no_spec_points = (unsigned int) nNPS;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].frequency = nFreq;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].flux_density = nFdFreq;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].i_flux_density = 0;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].spec_abscissa = nSpecAbscissa;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].spec_slope = nSpecSlop;

		// increase star counter
		m_pCatalogRadio->m_nData++;

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
		m_pCatalogRadio->m_nCatalogLoaded = CATALOG_ID_SPECFIND;
		return( m_pCatalogRadio->m_nData );

	} else
	{
		m_pCatalogRadio->UnloadCatalog( );
		m_pCatalogRadio->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadVlssFileFromVizier
// Class:	CVizier
// Purpose:	load VLSS radio catalog from file from vizier
// Input:	file name, region flag
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::LoadVlssFileFromVizier( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
	// other variables used
	unsigned long nCatNo=0, nLongVar, nNPS;
	double nRaDeg=0, nDecDeg=0, nSi=0, nMajAx=0, nMinAx=0, nPA=0;
	int bFoundStart=0;
	int bError=0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// do (un)allocation
	m_pCatalogRadio->UnloadCatalog( );
	// main allocation
	if( bRegion )
		m_pCatalogRadio->m_nAllocated = 5000;
	else
		m_pCatalogRadio->m_nAllocated = 68311;
	m_pCatalogRadio->m_vectData = (DefCatBasicRadio*) malloc( m_pCatalogRadio->m_nAllocated*sizeof(DefCatBasicRadio) );

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
		if( strLine[0] == '#' )
			continue;
		
		// check for last line before data
		if( !bFoundStart && strLine[0] == '-' && strLine[1] == '-' && strLine[2] == '-' )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogRadio->m_nData >= m_pCatalogRadio->m_nAllocated  )
		{
			// incremen counter
			m_pCatalogRadio->m_nAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogRadio->m_vectData = (DefCatBasicRadio*) realloc( m_pCatalogRadio->m_vectData, (m_pCatalogRadio->m_nAllocated+1)*sizeof(DefCatBasicRadio) );
		}

		// set this to wx string
		wxString strWxLine = strLine;

		// get cat no
		wxString strCatNo = strWxLine.Mid( 0, 8 ).Trim(0).Trim(1);
		if( !strCatNo.ToULong( &nCatNo ) ) 
				continue;

		// :: name
		wxString strName = strWxLine.Mid( 9, 11 ).Trim(0).Trim(1);

		// conver ra/dec from hexagesimal to decimal
		ConvertRaToDeg( strWxLine.Mid( 22, 11 ).Trim(0).Trim(1), nRaDeg );
		ConvertDecToDeg( strWxLine.Mid( 34, 11 ).Trim(0).Trim(1), nDecDeg );

		// :: Si - Integrated flux density at 74MHz (Jy)
		if( !strWxLine.Mid( 46, 8 ).Trim(0).Trim(1).ToDouble( &nSi ) )
			nSi = 0.0;

		// :: MajAx
		if( !strWxLine.Mid( 55, 5 ).Trim(0).Trim(1).ToDouble( &nMajAx ) )
			nMajAx = 0.0;
		// :: MinAx
		if( !strWxLine.Mid( 61, 5 ).Trim(0).Trim(1).ToDouble( &nMinAx ) )
			nMinAx = 0.0;

		// :: PA - Position angle of Major Ax   
		if( !strWxLine.Mid( 67, 6 ).Trim(0).Trim(1).ToDouble( &nPA ) )
			nPA = 0.0;
		 
		// set radio source
		// :: name - i should have allocated strlen(strName) - but because of export binary - i wont
//#ifndef _DEBUG
//		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_name = (char*) malloc( 23*sizeof(char) );
//#endif
		bzero( m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_name, 24 );
		strcpy( m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_name, strName.ToAscii() );
		// :: cat no and type 
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_no = nCatNo;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_type = CAT_OBJECT_TYPE_VLSS;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].type = SKY_OBJECT_TYPE_RADIO;

		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].ra = nRaDeg;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].dec = nDecDeg;

		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].no_spec_points = 0;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].frequency = 74;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].flux_density = 0;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].i_flux_density = nSi;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].spec_abscissa = 0;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].spec_slope = 0;

		// increase star counter
		m_pCatalogRadio->m_nData++;

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
		m_pCatalogRadio->m_nCatalogLoaded = CATALOG_ID_VLSS;
		return( m_pCatalogRadio->m_nData );

	} else
	{
		m_pCatalogRadio->UnloadCatalog( );
		m_pCatalogRadio->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadNvssFileFromVizier
// Class:	CVizier
// Purpose:	load NVSS radio catalog from file from vizier
// Input:	file name, region flag
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::LoadNvssFileFromVizier( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
	// other variables used
	unsigned long nCatNo=0, nLongVar, nNPS;
	double nRaDeg=0, nDecDeg=0, nS14=0, nMajAx=0, nMinAx=0, 
			nPA=0, nResFlux=0, nPolFlux=0, nPolPA=0;
	int bFoundStart=0;
	int bError=0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// do (un)allocation
	m_pCatalogRadio->UnloadCatalog( );
	// main allocation
	if( bRegion )
		m_pCatalogRadio->m_nAllocated = 10000;
	else
		m_pCatalogRadio->m_nAllocated = 1773484;
	m_pCatalogRadio->m_vectData = (DefCatBasicRadio*) malloc( m_pCatalogRadio->m_nAllocated*sizeof(DefCatBasicRadio) );

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
		if( strLine[0] == '#' )
			continue;
		
		// check for last line before data
		if( !bFoundStart && strLine[0] == '-' && strLine[1] == '-' && strLine[2] == '-' )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogRadio->m_nData >= m_pCatalogRadio->m_nAllocated  )
		{
			// incremen counter
			m_pCatalogRadio->m_nAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogRadio->m_vectData = (DefCatBasicRadio*) realloc( m_pCatalogRadio->m_vectData, (m_pCatalogRadio->m_nAllocated+1)*sizeof(DefCatBasicRadio) );
		}

		// set this to wx string
		wxString strWxLine = strLine;

		// get cat no
		wxString strCatNo = strWxLine.Mid( 0, 8 ).Trim(0).Trim(1);
		if( !strCatNo.ToULong( &nCatNo ) ) 
				continue;

		// :: name
		wxString strName = strWxLine.Mid( 9, 14 ).Trim(0).Trim(1);

		// conver ra/dec from hexagesimal to decimal
		ConvertRaToDeg( strWxLine.Mid( 24, 11 ).Trim(0).Trim(1), nRaDeg );
		ConvertDecToDeg( strWxLine.Mid( 36, 11 ).Trim(0).Trim(1), nDecDeg );

		// :: Si - Integrated 1.4GHz flux density of radio source  (mJy)
		if( !strWxLine.Mid( 48, 8 ).Trim(0).Trim(1).ToDouble( &nS14 ) )
			nS14 = 0.0;

		// :: MajAx
		if( !strWxLine.Mid( 57, 5 ).Trim(0).Trim(1).ToDouble( &nMajAx ) )
			nMajAx = 0.0;
		// :: MinAx
		if( !strWxLine.Mid( 63, 5 ).Trim(0).Trim(1).ToDouble( &nMinAx ) )
			nMinAx = 0.0;

		// :: PA - Position angle of Major Ax   
		if( !strWxLine.Mid( 69, 5 ).Trim(0).Trim(1).ToDouble( &nPA ) )
			nPA = 0.0;
		
		// :: resFlux - Peak residual flux, in mJy/beam 
		if( !strWxLine.Mid( 75, 4 ).Trim(0).Trim(1).ToDouble( &nResFlux ) )
			nResFlux = 0.0;
		// :: polFlux - Integrated linearly polarized flux density   
		if( !strWxLine.Mid( 80, 6 ).Trim(0).Trim(1).ToDouble( &nPolFlux ) )
			nPolFlux = 0.0;
		// :: polPA - [-90,90] The position angle of polFlux 
		if( !strWxLine.Mid( 87, 5 ).Trim(0).Trim(1).ToDouble( &nPolPA ) )
			nPolPA = 0.0;


		// set radio source
		// :: name - i should have allocated strlen(strName) - but because of export binary - i wont
//#ifndef _DEBUG
//		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_name = (char*) malloc( 23*sizeof(char) );
//#endif
		bzero( m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_name, 24 );
		strcpy( m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_name, strName.ToAscii() );
		// :: cat no and type 
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_no = nCatNo;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_type = CAT_OBJECT_TYPE_NVSS;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].type = SKY_OBJECT_TYPE_RADIO;

		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].ra = nRaDeg;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].dec = nDecDeg;

		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].no_spec_points = 0;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].frequency = 1400;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].flux_density = 0;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].i_flux_density = nS14;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].spec_abscissa = 0;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].spec_slope = 0;

		// increase star counter
		m_pCatalogRadio->m_nData++;

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
		m_pCatalogRadio->m_nCatalogLoaded = CATALOG_ID_NVSS;
		return( m_pCatalogRadio->m_nData );

	} else
	{
		m_pCatalogRadio->UnloadCatalog( );
		m_pCatalogRadio->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadMslFileFromVizier
// Class:	CVizier
// Purpose:	load MSL radio catalog from file from vizier
// Input:	file name, region flag
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::LoadMslFileFromVizier( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
	// other variables used
	unsigned long nCatNo=0, nLongVar, nNPS;
	double nRaDeg=0, nDecDeg=0, nS=0, nFreq=0;
	int bFoundStart=0;
	int bError=0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// do (un)allocation
	m_pCatalogRadio->UnloadCatalog( );
	// main allocation
	if( bRegion )
		m_pCatalogRadio->m_nAllocated = 1000;
	else
		m_pCatalogRadio->m_nAllocated = 79493;
	m_pCatalogRadio->m_vectData = (DefCatBasicRadio*) malloc( m_pCatalogRadio->m_nAllocated*sizeof(DefCatBasicRadio) );

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
		if( strLine[0] == '#' )
			continue;
		
		// check for last line before data
		if( !bFoundStart && strLine[0] == '-' && strLine[1] == '-' && strLine[2] == '-' )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogRadio->m_nData >= m_pCatalogRadio->m_nAllocated  )
		{
			// incremen counter
			m_pCatalogRadio->m_nAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogRadio->m_vectData = (DefCatBasicRadio*) realloc( m_pCatalogRadio->m_vectData, (m_pCatalogRadio->m_nAllocated+1)*sizeof(DefCatBasicRadio) );
		}

		// set this to wx string
		wxString strWxLine = strLine;

		// get cat no
		wxString strCatNo = strWxLine.Mid( 0, 8 ).Trim(0).Trim(1);
		if( !strCatNo.ToULong( &nCatNo ) ) 
				continue;

		// :: name
		wxString strName = strWxLine.Mid( 9, 13 ).Trim(0).Trim(1);

		// conver ra/dec from hexagesimal to decimal
		ConvertRaToDeg( strWxLine.Mid( 23, 12 ).Trim(0).Trim(1), nRaDeg );
		ConvertDecToDeg( strWxLine.Mid( 36, 13 ).Trim(0).Trim(1), nDecDeg );
		// convert from b1950 to j2000
		m_pCatalogRadio->m_pSky->Convert_SkyCoord_B1950_to_J2000( nRaDeg, nDecDeg );

		// :: Freq - Frequency (MHz)
		if( !strWxLine.Mid( 50, 5 ).Trim(0).Trim(1).ToDouble( &nFreq ) )
			nFreq = 0.0;

		// :: S - Flux density (Jy)
		if( !strWxLine.Mid( 56, 10 ).Trim(0).Trim(1).ToDouble( &nS ) )
			nS = 0.0;

		// set radio source
		// :: name - i should have allocated strlen(strName) - but because of export binary - i wont
//#ifndef _DEBUG
//		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_name = (char*) malloc( 23*sizeof(char) );
//#endif
		bzero( m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_name, 24 );
		strcpy( m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_name, strName.ToAscii() );
		// :: cat no and type 
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_no = nCatNo;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_type = CAT_OBJECT_TYPE_MSL;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].type = SKY_OBJECT_TYPE_RADIO;

		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].ra = nRaDeg;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].dec = nDecDeg;

		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].no_spec_points = 0;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].frequency = nFreq;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].flux_density = nS;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].i_flux_density = 0;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].spec_abscissa = 0;
		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].spec_slope = 0;

		// increase star counter
		m_pCatalogRadio->m_nData++;

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
		m_pCatalogRadio->m_nCatalogLoaded = CATALOG_ID_MSL;
		return( m_pCatalogRadio->m_nData );

	} else
	{
		m_pCatalogRadio->UnloadCatalog( );
		m_pCatalogRadio->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadRosatFileFromVizier
// Class:	CVizier
// Purpose:	load ROSAT catalog from file from vizier
// Input:	file name, region flag
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::LoadRosatFileFromVizier( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
	// other variables used
	unsigned long nCatNo=0, nLongVar, nExpTime=0;
	double nRaDeg=0, nDecDeg=0, nCount=0, nBgCount=0, nSize=0;
	int bFoundStart=0;
	int bError=0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// do (un)allocation
	m_pCatalogXGamma->UnloadCatalog( );
	// main allocation
	if( bRegion )
		m_pCatalogXGamma->m_nAllocated = 5000;
	else
		m_pCatalogXGamma->m_nAllocated = 105924;
	m_pCatalogXGamma->m_vectData = (DefCatBasicXGamma*) malloc( m_pCatalogXGamma->m_nAllocated*sizeof(DefCatBasicXGamma) );

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
		if( strLine[0] == '#' )
			continue;
		
		// check for last line before data
		if( !bFoundStart && strLine[0] == '-' && strLine[1] == '-' && strLine[2] == '-' )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogXGamma->m_nData >= m_pCatalogXGamma->m_nAllocated  )
		{
			// incremen counter
			m_pCatalogXGamma->m_nAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogXGamma->m_vectData = (DefCatBasicXGamma*) realloc( m_pCatalogXGamma->m_vectData, (m_pCatalogXGamma->m_nAllocated+1)*sizeof(DefCatBasicXGamma) );
		}

		// set this to wx string
		wxString strWxLine = strLine;

		// get cat no
		wxString strCatNo = strWxLine.Mid( 0, 8 ).Trim(0).Trim(1);
		if( !strCatNo.ToULong( &nCatNo ) ) 
				continue;

		// :: name
		wxString strName = strWxLine.Mid( 9, 16 ).Trim(0).Trim(1);

		// :: RA
		if( !strWxLine.Mid( 26, 9 ).Trim(0).Trim(1).ToDouble( &nRaDeg ) )
			nRaDeg = 0;
		// :: DEC
		if( !strWxLine.Mid( 36, 9 ).Trim(0).Trim(1).ToDouble( &nDecDeg ) )
			nDecDeg = 0;

		// :: count ct/s
		if( !strWxLine.Mid( 46, 9 ).Trim(0).Trim(1).ToDouble( &nCount ) )
			nCount = 0.0;
		// :: Background countrate
		if( !strWxLine.Mid( 56, 9 ).Trim(0).Trim(1).ToDouble( &nBgCount ) )
			nBgCount = 0.0;
		// :: Exposure time
		if( !strWxLine.Mid( 66, 6 ).Trim(0).Trim(1).ToULong( &nExpTime ) )
			nExpTime = 0.0;

		// :: Source extent, by which the source image exceeds the point spread function
		if( !strWxLine.Mid( 73, 5 ).Trim(0).Trim(1).ToDouble( &nSize ) )
			nSize = 0.0;

		// Beginning/End of observation (year, offset 1900) 
		wxString strObsStart = strWxLine.Mid( 79, 16 ).Trim(0).Trim(1);
		wxString strObsEnd = strWxLine.Mid( 96, 16 ).Trim(0).Trim(1);

		// set radio source
		// :: name - i should have allocated strlen(strName) - but because of export binary - i wont
//#ifndef _DEBUG
//		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_name = (char*) malloc( 23*sizeof(char) );
//#endif
		bzero( m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_name, 24 );
		strcpy( m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_name, strName.ToAscii() );
		// :: cat no and type 
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_no = nCatNo;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_type = CAT_OBJECT_TYPE_ROSAT;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].type = SKY_OBJECT_TYPE_XGAMMA;

		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].ra = nRaDeg;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].dec = nDecDeg;

		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].count = nCount;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].bg_count = nBgCount;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].exp_time = nExpTime;
//		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].i_flux_density = 0;
//		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].spec_abscissa = 0;
//		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].spec_slope = 0;

		// increase star counter
		m_pCatalogXGamma->m_nData++;

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
		m_pCatalogXGamma->m_nCatalogLoaded = CATALOG_ID_ROSAT;
		return( m_pCatalogXGamma->m_nData );

	} else
	{
		m_pCatalogXGamma->UnloadCatalog( );
		m_pCatalogXGamma->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadBatseFileFromVizier
// Class:	CVizier
// Purpose:	load BATSE catalog from file from vizier
// Input:	file name, region flag
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::LoadBatseFileFromVizier( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
	// other variables used
	unsigned long nCatNo=0, nLongVar, nStartTime=0, nTrigger=0, nDuration=0, nN50=0;
	double nRaDeg=0, nDecDeg=0, nFlux=0.0;
	int bFoundStart=0;
	int bError=0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// do (un)allocation
	m_pCatalogXGamma->UnloadCatalog( );
	// main allocation
	if( bRegion )
		m_pCatalogXGamma->m_nAllocated = 500;
	else
		m_pCatalogXGamma->m_nAllocated = 3906;
	m_pCatalogXGamma->m_vectData = (DefCatBasicXGamma*) malloc( m_pCatalogXGamma->m_nAllocated*sizeof(DefCatBasicXGamma) );

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
		if( strLine[0] == '#' )
			continue;
		
		// check for last line before data
		if( !bFoundStart && strLine[0] == '-' && strLine[1] == '-' && strLine[2] == '-' )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogXGamma->m_nData >= m_pCatalogXGamma->m_nAllocated  )
		{
			// incremen counter
			m_pCatalogXGamma->m_nAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogXGamma->m_vectData = (DefCatBasicXGamma*) realloc( m_pCatalogXGamma->m_vectData, (m_pCatalogXGamma->m_nAllocated+1)*sizeof(DefCatBasicXGamma) );
		}

		// set this to wx string
		wxString strWxLine = strLine;

		// get cat no
		wxString strCatNo = strWxLine.Mid( 0, 8 ).Trim(0).Trim(1);
		if( !strCatNo.ToULong( &nCatNo ) ) 
				continue;

		// :: name
		wxString strName = strWxLine.Mid( 9, 6 ).Trim(0).Trim(1);

		// :: RA
		if( !strWxLine.Mid( 34, 5 ).Trim(0).Trim(1).ToDouble( &nRaDeg ) )
			nRaDeg = 0;
		// :: DEC
		if( !strWxLine.Mid( 40, 5 ).Trim(0).Trim(1).ToDouble( &nDecDeg ) )
			nDecDeg = 0;

		// :: Start of the event within the day
		if( !strWxLine.Mid( 16, 5 ).Trim(0).Trim(1).ToULong( &nStartTime ) )
			nStartTime = 0;
		// :: BATSE trigger number, or 1 for non-triggered events   
		if( !strWxLine.Mid( 22, 4 ).Trim(0).Trim(1).ToULong( &nTrigger ) )
			nTrigger = 0.0;
		// :: Peak flux in the 50-300keV range ph/cm2/s 
		if( !strWxLine.Mid( 27, 6 ).Trim(0).Trim(1).ToDouble( &nFlux ) )
			nFlux = 0.0;

		// :: Duration rounded to integer seconds 
		if( !strWxLine.Mid( 46, 3 ).Trim(0).Trim(1).ToULong( &nDuration ) )
			nDuration = 0.0;
		// :: Number of 1.024s bins where the signal exceeds 50% of the peak value 
		if( !strWxLine.Mid( 50, 3 ).Trim(0).Trim(1).ToULong( &nN50 ) )
			nN50 = 0.0;

		// Terrestrial Date and Time of event start   
		wxString strEventStart = strWxLine.Mid( 56, 20 ).Trim(0).Trim(1);

		// set radio source
		// :: name - i should have allocated strlen(strName) - but because of export binary - i wont
//#ifndef _DEBUG
//		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_name = (char*) malloc( 23*sizeof(char) );
//#endif
		bzero( m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_name, 24 );
		strcpy( m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_name, strName.ToAscii() );
		// :: cat no and type 
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_no = nCatNo;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_type = CAT_OBJECT_TYPE_BATSE;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].type = SKY_OBJECT_TYPE_XGAMMA;

		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].ra = nRaDeg;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].dec = nDecDeg;

		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].flux = nFlux;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].start_time = nStartTime;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].duration = nDuration;

		// increase star counter
		m_pCatalogXGamma->m_nData++;

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
		m_pCatalogXGamma->m_nCatalogLoaded = CATALOG_ID_BATSE;
		return( m_pCatalogXGamma->m_nData );

	} else
	{
		m_pCatalogXGamma->UnloadCatalog( );
		m_pCatalogXGamma->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadIntegralFileFromVizier
// Class:	CVizier
// Purpose:	load INTEGRAL catalog from file from vizier
// Input:	file name, region flag
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::LoadIntegralFileFromVizier( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
	// other variables used
	unsigned long nCatNo=0, nLongVar;
	double nRaDeg=0, nDecDeg=0, nFluxBand1=0.0, nFluxBand2=0.0, nFluxBand3=0.0, 
			nFluxBand4=0.0, nCountBand1=0.0, nCountBand2=0.0, nCountBand3=0.0,
			nCountBand4=0.0;
	int bFoundStart=0;
	int bError=0, i=0;
	unsigned char nSourceType=0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// do (un)allocation
	m_pCatalogXGamma->UnloadCatalog( );
	// main allocation
	if( bRegion )
		m_pCatalogXGamma->m_nAllocated = 500;
	else
		m_pCatalogXGamma->m_nAllocated = 3906;
	m_pCatalogXGamma->m_vectData = (DefCatBasicXGamma*) malloc( m_pCatalogXGamma->m_nAllocated*sizeof(DefCatBasicXGamma) );

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
		if( strLine[0] == '#' )
			continue;
		
		// check for last line before data
		if( !bFoundStart && strLine[0] == '-' && strLine[1] == '-' && strLine[2] == '-' )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogXGamma->m_nData >= m_pCatalogXGamma->m_nAllocated  )
		{
			// incremen counter
			m_pCatalogXGamma->m_nAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogXGamma->m_vectData = (DefCatBasicXGamma*) realloc( m_pCatalogXGamma->m_vectData, (m_pCatalogXGamma->m_nAllocated+1)*sizeof(DefCatBasicXGamma) );
		}

		// set this to wx string
		wxString strWxLine = strLine;

		// get cat no
		wxString strCatNo = strWxLine.Mid( 0, 4 ).Trim(0).Trim(1);
		if( !strCatNo.ToULong( &nCatNo ) ) 
				continue;

		// :: name
		wxString strName = strWxLine.Mid( 5, 21 ).Trim(0).Trim(1);

		// conver ra/dec from hexagesimal to decimal
		ConvertRaToDeg( strWxLine.Mid( 27, 11 ).Trim(0).Trim(1), nRaDeg );
		ConvertDecToDeg( strWxLine.Mid( 39, 11 ).Trim(0).Trim(1), nDecDeg );

		////////////////////
		// :: 3-10 keV energy flux (JEMX) mW/m2 
		if( !strWxLine.Mid( 51, 8 ).Trim(0).Trim(1).ToDouble( &nFluxBand1 ) )
			nFluxBand1 = 0;
		// :: 10-30 keV energy flux (JEMX) mW/m2 
		if( !strWxLine.Mid( 60, 8 ).Trim(0).Trim(1).ToDouble( &nFluxBand2 ) )
			nFluxBand2 = 0.0;
		// :: 20-60 keV energy flux (ISGRI) mW/m2 
		if( !strWxLine.Mid( 69, 8 ).Trim(0).Trim(1).ToDouble( &nFluxBand3 ) )
			nFluxBand3 = 0.0;
		// :: 60-200 keV energy flux (ISGRI) mW/m2 
		if( !strWxLine.Mid( 78, 8 ).Trim(0).Trim(1).ToDouble( &nFluxBand4 ) )
			nFluxBand4 = 0.0;

		//////////////////////
		// :: CR3-10keV    JEMX soft band counting rates
		if( !strWxLine.Mid( 87, 6 ).Trim(0).Trim(1).ToDouble( &nCountBand1 ) )
			nCountBand1 = 0;
		// :: CR10-30keV   JEMX hard band counting rates
		if( !strWxLine.Mid( 94, 5 ).Trim(0).Trim(1).ToDouble( &nCountBand2 ) )
			nCountBand2 = 0.0;
		// :: CR20-60keV   ISGRI soft band counting rates
		if( !strWxLine.Mid( 100, 6 ).Trim(0).Trim(1).ToDouble( &nCountBand3 ) )
			nCountBand3 = 0.0;
		// :: CR60-200keV  ISGRI hard band counting rates
		if( !strWxLine.Mid( 107, 6 ).Trim(0).Trim(1).ToDouble( &nCountBand4 ) )
			nCountBand4 = 0.0;

		// Source type    
		wxString strSourceType = strWxLine.Mid( 114, 29 ).Trim(0).Trim(1);
		// check by type
		for( i=0; i<48; i++ )
		{
			if( strSourceType.CmpNoCase( wxString(m_vectCatIntegralTypes[i],wxConvUTF8) ) == 0 )
				nSourceType = i;
		}

		// set radio source
		// :: name - i should have allocated strlen(strName) - but because of export binary - i wont
//#ifndef _DEBUG
//		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_name = (char*) malloc( 23*sizeof(char) );
//#endif
		bzero( m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_name, 24 );
		strcpy( m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_name, strName.ToAscii() );
		// :: cat no and type 
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_no = nCatNo;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_type = CAT_OBJECT_TYPE_INTEGRAL;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].type = SKY_OBJECT_TYPE_XGAMMA;

		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].ra = nRaDeg;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].dec = nDecDeg;

		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].flux_band_1 = nFluxBand1;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].flux_band_2 = nFluxBand2;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].flux_band_3 = nFluxBand3;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].flux_band_4 = nFluxBand4;

		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].count_band_1 = nCountBand1;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].count_band_2 = nCountBand2;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].count_band_3 = nCountBand3;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].count_band_4 = nCountBand4;

		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].source_type = nSourceType;

		// increase star counter
		m_pCatalogXGamma->m_nData++;

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
		m_pCatalogXGamma->m_nCatalogLoaded = CATALOG_ID_INTEGRAL;
		return( m_pCatalogXGamma->m_nData );

	} else
	{
		m_pCatalogXGamma->UnloadCatalog( );
		m_pCatalogXGamma->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadBepposaxFileFromVizier
// Class:	CVizier
// Purpose:	load BEPPOSAX catalog from file from vizier
// Input:	file name, region flag
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::LoadBepposaxFileFromVizier( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
	// other variables used
	unsigned long nCatNo=0, nLongVar, nTime=0, nNA=0;
	double nRaDeg=0, nDecDeg=0, nTDet=0.0, nTA=0.0, nT90=0.0, 
			nFluence=0.0, nFPeak=0.0, nGamma=0.0;
	unsigned int nHour=0, nMin=0, nSec=0;
	int bFoundStart=0;
	int bError=0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// do (un)allocation
	m_pCatalogXGamma->UnloadCatalog( );
	// main allocation
	if( bRegion )
		m_pCatalogXGamma->m_nAllocated = 100;
	else
		m_pCatalogXGamma->m_nAllocated = 1082;
	m_pCatalogXGamma->m_vectData = (DefCatBasicXGamma*) malloc( m_pCatalogXGamma->m_nAllocated*sizeof(DefCatBasicXGamma) );

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
		if( strLine[0] == '#' )
			continue;
		
		// check for last line before data
		if( !bFoundStart && strLine[0] == '-' && strLine[1] == '-' && strLine[2] == '-' )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogXGamma->m_nData >= m_pCatalogXGamma->m_nAllocated  )
		{
			// incremen counter
			m_pCatalogXGamma->m_nAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogXGamma->m_vectData = (DefCatBasicXGamma*) realloc( m_pCatalogXGamma->m_vectData, (m_pCatalogXGamma->m_nAllocated+1)*sizeof(DefCatBasicXGamma) );
		}

		// set this to wx string
		wxString strWxLine = strLine;

		// get cat no
		wxString strCatNo = strWxLine.Mid( 0, 8 ).Trim(0).Trim(1);
		if( !strCatNo.ToULong( &nCatNo ) ) 
				continue;

		// :: name
		wxString strName = strWxLine.Mid( 9, 7 ).Trim(0).Trim(1);

		// :: RA
		if( !strWxLine.Mid( 26, 6 ).Trim(0).Trim(1).ToDouble( &nRaDeg ) )
			continue;
		// :: DEC
		if( !strWxLine.Mid( 33, 6 ).Trim(0).Trim(1).ToDouble( &nDecDeg ) )
			continue;

		// :: Time elapsed from the earliest GRB 
		if( !strWxLine.Mid( 40, 7 ).Trim(0).Trim(1).ToDouble( &nTDet ) )
			nTDet = 0.0;
		// :: Integrated fraction of Tdet    
		if( !strWxLine.Mid( 48, 7 ).Trim(0).Trim(1).ToDouble( &nTA ) )
			nTA = 0.0;
		// :: Number of intervals  
		if( !strWxLine.Mid( 56, 2 ).Trim(0).Trim(1).ToULong( &nNA ) )
			nNA = 0;

		// :: Time during which the burst integrated counts increase from 5% to 95% of the total counts  
		if( !strWxLine.Mid( 59, 7 ).Trim(0).Trim(1).ToDouble( &nT90 ) )
			nT90 = 0.0;
		// :: 40-700keV fluence in 10-6erg/cm2   
		if( !strWxLine.Mid( 67, 6 ).Trim(0).Trim(1).ToDouble( &nFluence ) )
			nFluence = 0.0;
		// :: 40-700keV peak flux in 10-7erg/cm2/s
		if( !strWxLine.Mid( 74, 6 ).Trim(0).Trim(1).ToDouble( &nFPeak ) )
			nFPeak = 0.0;
		// :: Best fit photon index   
		if( !strWxLine.Mid( 81, 6 ).Trim(0).Trim(1).ToDouble( &nGamma ) )
			nGamma = 0.0;

		// Terrestrial Date and Time of event start   
		wxString strEventTime = strWxLine.Mid( 17, 8 ).Trim(0).Trim(1);
		wxSscanf( strEventTime, wxT("%u:%u:%u"), &nHour, &nMin, &nSec );
		nTime = nHour*60*60+nMin*60+nSec;

		// set radio source
		// :: name - i should have allocated strlen(strName) - but because of export binary - i wont
//#ifndef _DEBUG
//		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_name = (char*) malloc( 23*sizeof(char) );
//#endif
		bzero( m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_name, 24 );
		strcpy( m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_name, strName.ToAscii() );
		// :: cat no and type 
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_no = nCatNo;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_type = CAT_OBJECT_TYPE_BEPPOSAX_GRBM;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].type = SKY_OBJECT_TYPE_XGAMMA;

		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].ra = nRaDeg;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].dec = nDecDeg;

		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].start_time = (unsigned long) nTime;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].time_det = (unsigned long) nTDet;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].interval_no = (unsigned int) nNA;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].duration = (unsigned long) nT90;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].fluence = nFluence;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].peak_flux = nFPeak;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].gamma = nGamma;

		// increase star counter
		m_pCatalogXGamma->m_nData++;

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
		m_pCatalogXGamma->m_nCatalogLoaded = CATALOG_ID_BEPPOSAX_GRBM;
		return( m_pCatalogXGamma->m_nData );

	} else
	{
		m_pCatalogXGamma->UnloadCatalog( );
		m_pCatalogXGamma->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	Load2XMMiFileFromVizier
// Class:	CVizier
// Purpose:	load 2XMMi catalog from file from vizier
// Input:	file name, region flag
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::Load2XMMiFileFromVizier( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
	// other variables used
	unsigned long nCatNo=0, nLongVar, nND=0;
	double nRaDeg=0, nDecDeg=0, nFluxBand1=0.0, nFluxBand2=0.0, nFluxBand3=0.0, 
			nFluxBand4=0.0, nFluxBand5=0.0, nFlux=0.0, nFlux14=0.0;
	int bFoundStart=0;
	int bError=0, i=0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// do (un)allocation
	m_pCatalogXGamma->UnloadCatalog( );
	// main allocation
	if( bRegion )
		m_pCatalogXGamma->m_nAllocated = 1000;
	else
		m_pCatalogXGamma->m_nAllocated = 221012;
	m_pCatalogXGamma->m_vectData = (DefCatBasicXGamma*) malloc( m_pCatalogXGamma->m_nAllocated*sizeof(DefCatBasicXGamma) );

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
		if( strLine[0] == '#' )
			continue;
		
		// check for last line before data
		if( !bFoundStart && strLine[0] == '-' && strLine[1] == '-' && strLine[2] == '-' )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogXGamma->m_nData >= m_pCatalogXGamma->m_nAllocated  )
		{
			// incremen counter
			m_pCatalogXGamma->m_nAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogXGamma->m_vectData = (DefCatBasicXGamma*) realloc( m_pCatalogXGamma->m_vectData, (m_pCatalogXGamma->m_nAllocated+1)*sizeof(DefCatBasicXGamma) );
		}

		// set this to wx string
		wxString strWxLine(strLine,wxConvUTF8);

		// get cat no
		wxString strCatNo = strWxLine.Mid( 0, 6 ).Trim(0).Trim(1);
		if( !strCatNo.ToULong( &nCatNo ) ) 
				continue;

		// :: name
		wxString strName = strWxLine.Mid( 7, 16 ).Trim(0).Trim(1);

		// :: RA
		if( !strWxLine.Mid( 24, 8 ).Trim(0).Trim(1).ToDouble( &nRaDeg ) )
			continue;
		// :: DEC
		if( !strWxLine.Mid( 33, 8 ).Trim(0).Trim(1).ToDouble( &nDecDeg ) )
			continue;

		////////////////////
		// :: Mean flux in 0.2-0.5keV band   
		if( !strWxLine.Mid( 42, 12 ).Trim(0).Trim(1).ToDouble( &nFluxBand1 ) )
			nFluxBand1 = 0;
		// :: Mean flux in 0.5-1.0keV band    
		if( !strWxLine.Mid( 55, 12 ).Trim(0).Trim(1).ToDouble( &nFluxBand2 ) )
			nFluxBand2 = 0.0;
		// :: Mean flux in 1.0-2.0keV band   
		if( !strWxLine.Mid( 68, 12 ).Trim(0).Trim(1).ToDouble( &nFluxBand3 ) )
			nFluxBand3 = 0.0;
		// :: Mean flux in 2.0-4.5keV band    
		if( !strWxLine.Mid( 81, 12 ).Trim(0).Trim(1).ToDouble( &nFluxBand4 ) )
			nFluxBand4 = 0.0;
		// :: Mean flux in 4.5-12keV band   
		if( !strWxLine.Mid( 94, 12 ).Trim(0).Trim(1).ToDouble( &nFluxBand5 ) )
			nFluxBand5 = 0.0;
		// :: Mean flux in 0.2-12keV band
		if( !strWxLine.Mid( 107, 12 ).Trim(0).Trim(1).ToDouble( &nFlux ) )
			nFlux = 0.0;
		// :: Mean flux in 0.5-4.5keV band   
		if( !strWxLine.Mid( 120, 12 ).Trim(0).Trim(1).ToDouble( &nFlux14 ) )
			nFlux14 = 0.0;

		// :: Number of detections   
		if( !strWxLine.Mid( 133, 2 ).Trim(0).Trim(1).ToULong( &nND ) )
			nND = 0;

		// set radio source
		// :: name - i should have allocated strlen(strName) - but because of export binary - i wont
//#ifndef _DEBUG
//		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_name = (char*) malloc( 23*sizeof(char) );
//#endif
		ResetObjCatXGamma( m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData] );

		strcpy( m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_name, strName.ToAscii() );
		// :: cat no and type 
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_no = nCatNo;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_type = CAT_OBJECT_TYPE_2XMMi;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].type = SKY_OBJECT_TYPE_XGAMMA;

		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].ra = nRaDeg;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].dec = nDecDeg;

		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].flux = nFlux;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].flux_band_1 = nFluxBand1;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].flux_band_2 = nFluxBand2;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].flux_band_3 = nFluxBand3;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].flux_band_4 = nFluxBand4;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].flux_band_5 = nFluxBand5;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].flux_band_6 = nFlux14;

		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].no_detections = nND;

		// increase star counter
		m_pCatalogXGamma->m_nData++;

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
		m_pCatalogXGamma->m_nCatalogLoaded = CATALOG_ID_2XMMi;
		return( m_pCatalogXGamma->m_nData );

	} else
	{
		m_pCatalogXGamma->UnloadCatalog( );
		m_pCatalogXGamma->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadHeaoA1FileFromVizier
// Class:	CVizier
// Purpose:	load HEAO A1 catalog from file from vizier
// Input:	file name, region flag
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::LoadHeaoA1FileFromVizier( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
	// other variables used
	unsigned long nCatNo=0, nLongVar;
	double nRaDeg=0, nDecDeg=0, nFlux=0.0;
	int bFoundStart=0;
	int bError=0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// do (un)allocation
	m_pCatalogXGamma->UnloadCatalog( );
	// main allocation
	if( bRegion )
		m_pCatalogXGamma->m_nAllocated = 100;
	else
		m_pCatalogXGamma->m_nAllocated = 842;
	m_pCatalogXGamma->m_vectData = (DefCatBasicXGamma*) malloc( m_pCatalogXGamma->m_nAllocated*sizeof(DefCatBasicXGamma) );

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
		if( strLine[0] == '#' )
			continue;
		
		// check for last line before data
		if( !bFoundStart && strLine[0] == '-' && strLine[1] == '-' && strLine[2] == '-' )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogXGamma->m_nData >= m_pCatalogXGamma->m_nAllocated  )
		{
			// incremen counter
			m_pCatalogXGamma->m_nAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogXGamma->m_vectData = (DefCatBasicXGamma*) realloc( m_pCatalogXGamma->m_vectData, (m_pCatalogXGamma->m_nAllocated+1)*sizeof(DefCatBasicXGamma) );
		}

		// set this to wx string
		wxString strWxLine = strLine;

		// get cat no
		wxString strCatNo = strWxLine.Mid( 0, 8 ).Trim(0).Trim(1);
		if( !strCatNo.ToULong( &nCatNo ) ) 
				continue;

		// :: name
		wxString strName = strWxLine.Mid( 9, 10 ).Trim(0).Trim(1);

		// :: RA
		if( !strWxLine.Mid( 20, 8 ).Trim(0).Trim(1).ToDouble( &nRaDeg ) )
			continue;
		// :: DEC
		if( !strWxLine.Mid( 27, 8 ).Trim(0).Trim(1).ToDouble( &nDecDeg ) )
			continue;

		// convert from b1950 to j2000
		m_pCatalogXGamma->m_pSky->Convert_SkyCoord_B1950_to_J2000( nRaDeg, nDecDeg );

		// :: Start of the event within the day
		if( !strWxLine.Mid( 34, 7 ).Trim(0).Trim(1).ToDouble( &nFlux ) )
			nFlux = 0;

		// set radio source
		// :: name - i should have allocated strlen(strName) - but because of export binary - i wont
//#ifndef _DEBUG
//		m_pCatalogRadio->m_vectData[m_pCatalogRadio->m_nData].cat_name = (char*) malloc( 23*sizeof(char) );
//#endif
		bzero( m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_name, 24 );
		strcpy( m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_name, strName.ToAscii() );
		// :: cat no and type 
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_no = nCatNo;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].cat_type = CAT_OBJECT_TYPE_HEAO_A1;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].type = SKY_OBJECT_TYPE_XGAMMA;

		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].ra = nRaDeg;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].dec = nDecDeg;

		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].flux = nFlux;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].start_time = 0;
		m_pCatalogXGamma->m_vectData[m_pCatalogXGamma->m_nData].duration = 0;

		// increase star counter
		m_pCatalogXGamma->m_nData++;

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
		m_pCatalogXGamma->m_nCatalogLoaded = CATALOG_ID_HEAO_A1;
		return( m_pCatalogXGamma->m_nData );

	} else
	{
		m_pCatalogXGamma->UnloadCatalog( );
		m_pCatalogXGamma->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadAscFileFromVizier
// Class:	CVizier
// Purpose:	load ASC catalog from file from vizier
// Input:	file name, region flag
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::LoadAscFileFromVizier( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
	// other variables used
	unsigned long nCatNo=0, nLongVar;
	double nRaDeg=0, nDecDeg=0, nOffsetX=0.0, nOffsetY=0.0, nMaxMag=0.0, nDate=0.0;
	int bFoundStart=0;
	int bError=0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// do (un)allocation
	m_pCatalogSupernovas->UnloadCatalog( );
	// main allocation
	if( bRegion )
		m_pCatalogSupernovas->m_nAllocated = 100;
	else
		m_pCatalogSupernovas->m_nAllocated = 4914;
	m_pCatalogSupernovas->m_vectData = (DefCatBasicSupernova*) malloc( m_pCatalogSupernovas->m_nAllocated*sizeof(DefCatBasicSupernova) );

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
		if( strLine[0] == '#' )
			continue;
		
		// check for last line before data
		if( !bFoundStart && strLine[0] == '-' && strLine[1] == '-' && strLine[2] == '-' )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogSupernovas->m_nData >= m_pCatalogSupernovas->m_nAllocated  )
		{
			// incremen counter
			m_pCatalogSupernovas->m_nAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogSupernovas->m_vectData = (DefCatBasicSupernova*) realloc( m_pCatalogSupernovas->m_vectData, (m_pCatalogSupernovas->m_nAllocated+1)*sizeof(DefCatBasicSupernova) );
		}

		// set this to wx string
		wxString strWxLine = strLine;

		// get cat no
		wxString strCatNo = strWxLine.Mid( 0, 8 ).Trim(0).Trim(1);
		if( !strCatNo.ToULong( &nCatNo ) ) 
				continue;

		// :: name
		wxString strName = strWxLine.Mid( 9, 6 ).Trim(0).Trim(1);

		// conver ra/dec from hexagesimal to decimal
		if( !ConvertRaToDeg( strWxLine.Mid( 35, 12 ).Trim(0).Trim(1), nRaDeg ) ) continue;
		if( !ConvertDecToDeg( strWxLine.Mid( 48, 12 ).Trim(0).Trim(1), nDecDeg ) ) continue;

		// :: unconfirmed
		char chrUnconf = strWxLine.Mid( 18, 1 ).Trim(0).Trim(1)[0];

		// :: X - offset from the galaxy nucleus
		if( !strWxLine.Mid( 61, 6 ).Trim(0).Trim(1).ToDouble( &nOffsetX ) )
			nOffsetX = 0;
		// :: X - offset from the galaxy nucleus
		if( !strWxLine.Mid( 68, 6 ).Trim(0).Trim(1).ToDouble( &nOffsetY ) )
			nOffsetY = 0;

		// :: If available, supernova magnitude at maximum (photometric band indicated) 
		if( !strWxLine.Mid( 75, 5 ).Trim(0).Trim(1).ToDouble( &nMaxMag ) )
			nMaxMag = 0;

		// :: supernova type
		wxString strSNType = strWxLine.Mid( 81, 8 ).Trim(0).Trim(1);
		// :: Name(s) of discoverer(s)
		wxString strDiscName = strWxLine.Mid( 101, 41 ).Trim(0).Trim(1);

		// :: Date of maximum or Discovery 
		wxString strDateMaxDisc = strWxLine.Mid( 90, 10 ).Trim(0).Trim(1);
		// process execption yyyy-mm
		wxRegEx reDateExcept = wxT( "^[0-9]{4}\\-[0-9]{2}$" );
		if( reDateExcept.Matches( strDateMaxDisc ) )
			strDateMaxDisc += wxT("-01");
		// process date
		wxDateTime mydate;
		mydate.ParseDate( strDateMaxDisc );
		nDate = mydate.GetJDN();

		if( nCatNo > 5000 || nCatNo < 0 ) 
			continue;

		// :: Name(s) of galaxy
		wxString strGalaxyName = strWxLine.Mid( 20, 14 ).Trim(0).Trim(1);

		// set radio source
		// :: name - i should have allocated strlen(strName) - but because of export binary - i wont
//#ifndef _DEBUG
//		m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].cat_name = (char*) malloc( 23*sizeof(char) );
//#endif
		//bzero( m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].cat_name, 24 );
		ResetObjCatSupernovas( m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData] );

		// set name
		wxStrcpy( m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].cat_name, strName );
		// :: cat no and type 
		m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].cat_no = nCatNo;
		m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].cat_type = CAT_OBJECT_TYPE_ASC;
		m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].type = SKY_OBJECT_TYPE_SUPERNOVA;

		m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].ra = nRaDeg;
		m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].dec = nDecDeg;

		m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].gal_center_offset_x = nOffsetX;
		m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].gal_center_offset_y = nOffsetY;
		m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].max_mag = nMaxMag;
		m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].unconfirmed = chrUnconf;
		// .. process date here Julian date number
		m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].date_max = nDate;

		// copy strings
		wxStrcpy( m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].parent_galaxy, strGalaxyName );
		wxStrcpy( m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].sn_type, strSNType );
		wxStrcpy( m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].discoverer, strDiscName );

		// increase star counter
		m_pCatalogSupernovas->m_nData++;

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
		m_pCatalogSupernovas->m_nCatalogLoaded = CATALOG_ID_ASC;
		return( m_pCatalogSupernovas->m_nData );

	} else
	{
		m_pCatalogSupernovas->UnloadCatalog( );
		m_pCatalogSupernovas->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadSscFileFromVizier
// Class:	CVizier
// Purpose:	load SSC catalog from file from vizier
// Input:	file name, region flag
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::LoadSscFileFromVizier( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
	// other variables used
	unsigned long nCatNo=0, nLongVar;
	double nRaDeg=0, nDecDeg=0, nGalMag=0.0, nMag=0.0, nDate=0.0;
	int bFoundStart=0;
	int bError=0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// do (un)allocation
	m_pCatalogSupernovas->UnloadCatalog( );
	// main allocation
	if( bRegion )
		m_pCatalogSupernovas->m_nAllocated = 100;
	else
		m_pCatalogSupernovas->m_nAllocated = 2991;
	m_pCatalogSupernovas->m_vectData = (DefCatBasicSupernova*) malloc( m_pCatalogSupernovas->m_nAllocated*sizeof(DefCatBasicSupernova) );

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
		if( strLine[0] == '#' )
			continue;
		
		// check for last line before data
		if( !bFoundStart && strLine[0] == '-' && strLine[1] == '-' && strLine[2] == '-' )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogSupernovas->m_nData >= m_pCatalogSupernovas->m_nAllocated  )
		{
			// incremen counter
			m_pCatalogSupernovas->m_nAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogSupernovas->m_vectData = (DefCatBasicSupernova*) realloc( m_pCatalogSupernovas->m_vectData, (m_pCatalogSupernovas->m_nAllocated+1)*sizeof(DefCatBasicSupernova) );
		}

		// set this to wx string
		wxString strWxLine = strLine;

		// get cat no
		wxString strCatNo = strWxLine.Mid( 0, 8 ).Trim(0).Trim(1);
		if( !strCatNo.ToULong( &nCatNo ) ) 
				continue;

		// :: name
		wxString strName = strWxLine.Mid( 9, 6 ).Trim(0).Trim(1);

		// conver ra/dec from hexagesimal to decimal
		if( !ConvertRaToDeg( strWxLine.Mid( 29, 12 ).Trim(0).Trim(1), nRaDeg ) ) continue;
		if( !ConvertDecToDeg( strWxLine.Mid( 42, 12 ).Trim(0).Trim(1), nDecDeg ) ) continue;

		// :: unconfirmed - uncert
		wxString strUncert = strWxLine.Mid( 218, 2 ).Trim(0).Trim(1);
		unsigned char chrUnconf = 0;
		// check and assing type 
		if( strUncert.Cmp( wxT("?") ) == 0 )
			chrUnconf = 1;
		else if( strUncert.Cmp( wxT("??") ) == 0 )
			chrUnconf = 2;

		// :: Photographic or other magnitude of galaxy
		if( !strWxLine.Mid( 61, 6 ).Trim(0).Trim(1).ToDouble( &nGalMag ) )
			nGalMag = 0;

		// :: Method of supernova discovery
		char chrMethod = strWxLine.Mid( 73, 1 ).Trim(0).Trim(1)[0];
		unsigned char disc_method=0;
		if( chrMethod == '*' ) disc_method = 1;

		// :: supernova magnitude 
		if( !strWxLine.Mid( 55, 4 ).Trim(0).Trim(1).ToDouble( &nMag ) )
			nMag = 0;

		// :: supernova type
		wxString strSNType = strWxLine.Mid( 60, 7 ).Trim(0).Trim(1);

		// :: Name(s) of discoverer(s)
		wxString strDiscName = strWxLine.Mid( 81, 136 ).Trim(0).Trim(1);

		// :: Date of Discovery 
		wxString strDateDisc = strWxLine.Mid( 75, 5 ).Trim(0).Trim(1);
		wxString reDateMonth = strDateDisc.Mid( 0, 3 ).Trim(0).Trim(1);
		wxString reDateDay = strDateDisc.Mid( 3, 2 ).Trim(0).Trim(1);
		//extract year from name
		wxString reDateYear = strName.Mid( 0, 4 ).Trim(0).Trim(1);

		// set right string
		strDateDisc = reDateDay + wxT(" ") + reDateMonth + wxT(" ") + reDateYear;
		// process date
		wxDateTime mydate;
		mydate.ParseDate( strDateDisc );
		nDate = mydate.GetJDN();

		if( nCatNo > 5000 || nCatNo < 0 ) 
			continue;

		// :: Name(s) of galaxy
		wxString strGalaxyName = strWxLine.Mid( 16, 12 ).Trim(0).Trim(1);

		// :: supernova type
		wxString strDiscProgCode = strWxLine.Mid( 221, 2 ).Trim(0).Trim(1);

		// set radio source
		// :: name - i should have allocated strlen(strName) - but because of export binary - i wont
//#ifndef _DEBUG
//		m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].cat_name = (char*) malloc( 23*sizeof(char) );
//#endif
		//bzero( m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].cat_name, 24 );
		ResetObjCatSupernovas( m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData] );

		// set name
		wxStrcpy( m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].cat_name, strName );
		// :: cat no and type 
		m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].cat_no = nCatNo;
		m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].cat_type = CAT_OBJECT_TYPE_SSC;
		m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].type = SKY_OBJECT_TYPE_SUPERNOVA;

		m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].ra = nRaDeg;
		m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].dec = nDecDeg;

		m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].disc_method = disc_method;
		m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].mag = nMag;
		m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].unconfirmed = chrUnconf;
		// .. process date here Julian date number
		m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].date_discovery = nDate;

		// copy strings
		wxStrcpy( m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].parent_galaxy, strGalaxyName );
		wxStrcpy( m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].sn_type, strSNType );
		wxStrcpy( m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].discoverer, strDiscName );
		wxStrncpy( m_pCatalogSupernovas->m_vectData[m_pCatalogSupernovas->m_nData].prog_code, strDiscProgCode, 2 );

		// increase star counter
		m_pCatalogSupernovas->m_nData++;

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
		m_pCatalogSupernovas->m_nCatalogLoaded = CATALOG_ID_SSC;
		return( m_pCatalogSupernovas->m_nData );

	} else
	{
		m_pCatalogSupernovas->UnloadCatalog( );
		m_pCatalogSupernovas->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadSdssdr5FileFromVizier
// Class:	CVizier
// Purpose:	load SDSS DR5 catalog from file from vizier
// Input:	file name, region flag
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::LoadSdssdr5FileFromVizier( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
	// other variables used
	unsigned long nCatNo=0, nLongVar;
	double nRaDeg=0, nDecDeg=0, nRedShift=0.0, nIMag=0.0, nLuminosity=0.0,
			nMass=0.0, nMassHBeta=0.0, nMassMGII=0.0, nMassCIV=0.0;
	int bFoundStart=0;
	int bError=0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// do (un)allocation
	m_pCatalogBlackholes->UnloadCatalog( );
	// main allocation
	if( bRegion )
		m_pCatalogBlackholes->m_nAllocated = 500;
	else
		m_pCatalogBlackholes->m_nAllocated = 77429;
	m_pCatalogBlackholes->m_vectData = (DefCatBlackhole*) malloc( m_pCatalogBlackholes->m_nAllocated*sizeof(DefCatBlackhole) );

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
		if( strLine[0] == '#' )
			continue;
		
		// check for last line before data
		if( !bFoundStart && strLine[0] == '-' && strLine[1] == '-' && strLine[2] == '-' )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogBlackholes->m_nData >= m_pCatalogBlackholes->m_nAllocated  )
		{
			// incremen counter
			m_pCatalogBlackholes->m_nAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogBlackholes->m_vectData = (DefCatBlackhole*) realloc( m_pCatalogBlackholes->m_vectData, (m_pCatalogBlackholes->m_nAllocated+1)*sizeof(DefCatBlackhole) );
		}

		// set this to wx string
		wxString strWxLine = strLine;

		// get cat no
		wxString strCatNo = strWxLine.Mid( 0, 8 ).Trim(0).Trim(1);
		if( !strCatNo.ToULong( &nCatNo ) ) 
				continue;

		// :: name
		wxString strName = strWxLine.Mid( 9, 18 ).Trim(0).Trim(1);

		// :: RA
		if( !strWxLine.Mid( 28, 10 ).Trim(0).Trim(1).ToDouble( &nRaDeg ) )
			continue;
		// :: DEC
		if( !strWxLine.Mid( 39, 10 ).Trim(0).Trim(1).ToDouble( &nDecDeg ) )
			continue;

		// :: redshift
		if( !strWxLine.Mid( 50, 7 ).Trim(0).Trim(1).ToDouble( &nRedShift ) )
			nRedShift = 0;

		// :: PSF i-band apparent magnitude 
		if( !strWxLine.Mid( 58, 7 ).Trim(0).Trim(1).ToDouble( &nIMag ) )
			nIMag = 0;

		// :: luminosity
		if( !strWxLine.Mid( 66, 9 ).Trim(0).Trim(1).ToDouble( &nLuminosity ) )
			nLuminosity = 0;

		// :: mass
		if( !strWxLine.Mid( 96, 7 ).Trim(0).Trim(1).ToDouble( &nMass ) )
			nMass = 0;

		// :: Mass HBeta
		if( !strWxLine.Mid( 104, 7 ).Trim(0).Trim(1).ToDouble( &nMassHBeta ) )
			nMassHBeta = 0;

		// :: nMassMGII
		if( !strWxLine.Mid( 112, 7 ).Trim(0).Trim(1).ToDouble( &nMassMGII ) )
			nMassMGII = 0;

		// :: MassCIV
		if( !strWxLine.Mid( 120, 7 ).Trim(0).Trim(1).ToDouble( &nMassCIV ) )
			nMassCIV = 0;

		// :: MJD date of spectroscopic observation  
		wxString strObsDate = strWxLine.Mid( 128, 6 ).Trim(0).Trim(1);

		// set radio source
		// :: name - i should have allocated strlen(strName) - but because of export binary - i wont
//#ifndef _DEBUG
//		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].cat_name = (char*) malloc( 23*sizeof(char) );
//#endif
		//bzero( m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].cat_name, 24 );
		ResetObjCatBlackhole( m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData] );

		// set name
		wxStrcpy( m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].cat_name, strName );
		// :: cat no and type 
		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].cat_no = nCatNo;
		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].cat_type = CAT_OBJECT_TYPE_SDSS_DR5;
		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].type = SKY_OBJECT_TYPE_BLACKHOLE;

		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].ra = nRaDeg;
		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].dec = nDecDeg;

//		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].target_type = nTargetType;
		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].mass = nMass;
		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].mass_hbeta = nMassHBeta;
		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].mass_mgii = nMassMGII;
		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].mass_civ = nMassCIV;
		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].luminosity = nLuminosity;
		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].redshift = nRedShift;

		// increase star counter
		m_pCatalogBlackholes->m_nData++;

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
		m_pCatalogBlackholes->m_nCatalogLoaded = CATALOG_ID_SDSS_DR5;
		return( m_pCatalogBlackholes->m_nData );

	} else
	{
		m_pCatalogBlackholes->UnloadCatalog( );
		m_pCatalogBlackholes->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadRxeagnFileFromVizier
// Class:	CVizier
// Purpose:	load RXE AGN catalog from file from vizier
// Input:	file name, region flag
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::LoadRxeagnFileFromVizier( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
	// other variables used
	unsigned long nCatNo=0, nLongVar;
	double nRaDeg=0, nDecDeg=0, nRedShift=0.0, nLumXRay=0.0, nLumRadio=0.0,
			nMass=0.0, nLumLogR=0.0;
	int bFoundStart=0;
	int bError=0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// do (un)allocation
	m_pCatalogBlackholes->UnloadCatalog( );
	// main allocation
	if( bRegion )
		m_pCatalogBlackholes->m_nAllocated = 120;
	else
		m_pCatalogBlackholes->m_nAllocated = 115;
	m_pCatalogBlackholes->m_vectData = (DefCatBlackhole*) malloc( m_pCatalogBlackholes->m_nAllocated*sizeof(DefCatBlackhole) );

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
		if( strLine[0] == '#' )
			continue;
		
		// check for last line before data
		if( !bFoundStart && strLine[0] == '-' && strLine[1] == '-' && strLine[2] == '-' )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogBlackholes->m_nData >= m_pCatalogBlackholes->m_nAllocated  )
		{
			// incremen counter
			m_pCatalogBlackholes->m_nAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogBlackholes->m_vectData = (DefCatBlackhole*) realloc( m_pCatalogBlackholes->m_vectData, (m_pCatalogBlackholes->m_nAllocated+1)*sizeof(DefCatBlackhole) );
		}

		// set this to wx string
		wxString strWxLine = strLine;

		// get cat no
		wxString strCatNo = strWxLine.Mid( 0, 8 ).Trim(0).Trim(1);
		if( !strCatNo.ToULong( &nCatNo ) ) 
				continue;

		// :: name
		wxString strName = strWxLine.Mid( 9, 19 ).Trim(0).Trim(1);

		// :: RA
		if( !strWxLine.Mid( 29, 9 ).Trim(0).Trim(1).ToDouble( &nRaDeg ) )
			continue;
		// :: DEC
		if( !strWxLine.Mid( 39, 8 ).Trim(0).Trim(1).ToDouble( &nDecDeg ) )
			continue;

		// :: redshift
		if( !strWxLine.Mid( 48, 6 ).Trim(0).Trim(1).ToDouble( &nRedShift ) )
			nRedShift = 0;

		// :: mass
		if( !strWxLine.Mid( 55, 6 ).Trim(0).Trim(1).ToDouble( &nMass ) )
			nMass = 0;

		// :: luminosity-xray
		if( !strWxLine.Mid( 62, 6 ).Trim(0).Trim(1).ToDouble( &nLumXRay ) )
			nLumXRay = 0;

		// :: luminosity-radio
		if( !strWxLine.Mid( 69, 6 ).Trim(0).Trim(1).ToDouble( &nLumRadio ) )
			nLumRadio = 0;

		// :: Ratio of X-ray to the Eddington luminosity 
		if( !strWxLine.Mid( 76, 6 ).Trim(0).Trim(1).ToDouble( &nLumLogR ) )
			nLumLogR = 0;

		// set blackhole
		// :: name - i should have allocated strlen(strName) - but because of export binary - i wont
//#ifndef _DEBUG
//		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].cat_name = (char*) malloc( 23*sizeof(char) );
//#endif
		//bzero( m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].cat_name, 24 );
		ResetObjCatBlackhole( m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData] );

		// set name
		wxStrcpy( m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].cat_name, strName );
		// :: cat no and type 
		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].cat_no = nCatNo;
		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].cat_type = CAT_OBJECT_TYPE_RXE_AGN;
		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].type = SKY_OBJECT_TYPE_BLACKHOLE;

		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].ra = nRaDeg;
		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].dec = nDecDeg;

//		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].target_type = nTargetType;
		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].mass = nMass;
		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].radio_luminosity = nLumRadio;
		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].xray_luminosity = nLumXRay;
		m_pCatalogBlackholes->m_vectData[m_pCatalogBlackholes->m_nData].redshift = nRedShift;

		// increase star counter
		m_pCatalogBlackholes->m_nData++;

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
		m_pCatalogBlackholes->m_nCatalogLoaded = CATALOG_ID_RXE_AGN;
		return( m_pCatalogBlackholes->m_nData );

	} else
	{
		m_pCatalogBlackholes->UnloadCatalog( );
		m_pCatalogBlackholes->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadWdsFileFromVizier
// Class:	CVizier
// Purpose:	load WDS catalog from file from vizier
// Input:	file name, region flag
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::LoadWdsFileFromVizier( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
	// other variables used
	unsigned long nCatNo=0, nLongVar=0, nNObs=0;
	double nRaDeg=0, nDecDeg=0, nObs1=0.0, nObs2=0.0, nPa1=0.0, nPa2=0.0,
			nSep1=0.0, nSep2=0.0, nMag1=0.0, nMag2=0.0, nPmRA1=0.0, nPmDEC1=0.0,
			nPmRA2=0.0, nPmDEC2=0.0;
	int bFoundStart=0;
	int bError=0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// do (un)allocation
	m_pCatalogMStars->UnloadCatalog( );
	// main allocation
	if( bRegion )
		m_pCatalogMStars->m_nAllocated = 1000;
	else
		m_pCatalogMStars->m_nAllocated = 104275;
	m_pCatalogMStars->m_vectData = (DefCatMStars*) malloc( m_pCatalogMStars->m_nAllocated*sizeof(DefCatMStars) );

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
		if( strLine[0] == '#' )
			continue;
		
		// check for last line before data
		if( !bFoundStart && strLine[0] == '-' && strLine[1] == '-' && strLine[2] == '-' )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogMStars->m_nData >= m_pCatalogMStars->m_nAllocated  )
		{
			// incremen counter
			m_pCatalogMStars->m_nAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogMStars->m_vectData = (DefCatMStars*) realloc( m_pCatalogMStars->m_vectData, (m_pCatalogMStars->m_nAllocated+1)*sizeof(DefCatMStars) );
		}

		// set this to wx string
		wxString strWxLine = strLine;

		// get cat no
		wxString strCatNo = strWxLine.Mid( 0, 8 ).Trim(0).Trim(1);
		if( !strCatNo.ToULong( &nCatNo ) ) 
				continue;

		// :: name
		wxString strName = strWxLine.Mid( 9, 10 ).Trim(0).Trim(1);

		// conver ra/dec from hexagesimal to decimal
		if( !ConvertRaToDeg( strWxLine.Mid( 20, 11 ).Trim(0).Trim(1), nRaDeg ) ) continue;
		if( !ConvertDecToDeg( strWxLine.Mid( 32, 11 ).Trim(0).Trim(1), nDecDeg ) ) continue;

		// :: disc
		wxString strDisc = strWxLine.Mid( 44, 7 ).Trim(0).Trim(1);
		// :: comp
		wxString strComp = strWxLine.Mid( 52, 5 ).Trim(0).Trim(1);

		// :: obs1
		if( !strWxLine.Mid( 58, 4 ).Trim(0).Trim(1).ToDouble( &nObs1 ) ) nObs1 = 0;
		// :: obs2
		if( !strWxLine.Mid( 63, 4 ).Trim(0).Trim(1).ToDouble( &nObs2 ) ) nObs2 = 0;
		// :: number of obs
		if( !strWxLine.Mid( 68, 4 ).Trim(0).Trim(1).ToULong( &nNObs ) ) nNObs = 0;

		// :: pa1
		if( !strWxLine.Mid( 73, 3 ).Trim(0).Trim(1).ToDouble( &nPa1 ) ) nPa1 = 0;
		// :: pa2
		if( !strWxLine.Mid( 77, 3 ).Trim(0).Trim(1).ToDouble( &nPa2 ) ) nPa2 = 0;

		// :: sep1
		if( !strWxLine.Mid( 81, 5 ).Trim(0).Trim(1).ToDouble( &nSep1 ) ) nSep1 = 0;
		// :: sep2
		if( !strWxLine.Mid( 87, 5 ).Trim(0).Trim(1).ToDouble( &nSep2 ) ) nSep2 = 0;

		// :: mag1
		if( !strWxLine.Mid( 93, 5 ).Trim(0).Trim(1).ToDouble( &nMag1 ) ) nMag1 = 0;
		// :: mag2
		if( !strWxLine.Mid( 99, 5 ).Trim(0).Trim(1).ToDouble( &nMag2 ) ) nMag2 = 0;

		// :: spectral type
		wxString strSpecType = strWxLine.Mid( 105, 10 ).Trim(0).Trim(1);

		// :: pmRA1
		if( !strWxLine.Mid( 116, 4 ).Trim(0).Trim(1).ToDouble( &nPmRA1 ) ) nPmRA1 = 0;
		// :: pmDEC1
		if( !strWxLine.Mid( 121, 4 ).Trim(0).Trim(1).ToDouble( &nPmDEC1 ) ) nPmDEC1 = 0;

		// :: pmRA2
		if( !strWxLine.Mid( 126, 4 ).Trim(0).Trim(1).ToDouble( &nPmRA2 ) ) nPmRA2 = 0;
		// :: pmDEC2
		if( !strWxLine.Mid( 131, 4 ).Trim(0).Trim(1).ToDouble( &nPmDEC2 ) ) nPmDEC2 = 0;

		// :: notes
		wxString strNotes = strWxLine.Mid( 136, 4 ).Trim(0).Trim(1);

		// set mstars
		// :: name - i should have allocated strlen(strName) - but because of export binary - i wont
//#ifndef _DEBUG
//		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].cat_name = (char*) malloc( 23*sizeof(char) );
//#endif
		//bzero( m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].cat_name, 24 );
		ResetObjCatMStars( m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData] );

		// set name
		strcpy( m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].cat_name, strName.ToAscii() );
		// :: cat no and type 
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].cat_no = nCatNo;
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].cat_type = CAT_OBJECT_TYPE_WDS;
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].type = SKY_OBJECT_TYPE_MSTARS;

		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].ra = nRaDeg;
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].dec = nDecDeg;

		strcpy( m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].discoverer, strDisc.ToAscii() );
		strcpy( m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].comp, strComp.ToAscii() );

		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].nobs = (unsigned int) nNObs;
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].obs_date = nObs1;
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].obs_date2 = nObs2;
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].pos_ang = nPa1;
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].pos_ang2 = nPa2;
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].sep = nSep1;
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].sep2 = nSep2;
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].mag = nMag1;
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].mag2 = nMag2;

		strcpy( m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].spectral_type, strSpecType.ToAscii() );

		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].pm_ra = nPmRA1;
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].pm_dec = nPmDEC1;
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].pm_ra2 = nPmRA2;
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].pm_dec2 = nPmDEC2;

		strcpy( m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].notes, strNotes.ToAscii() );

		// increase star counter
		m_pCatalogMStars->m_nData++;

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
		m_pCatalogMStars->m_nCatalogLoaded = CATALOG_ID_WDS;
		return( m_pCatalogBlackholes->m_nData );

	} else
	{
		m_pCatalogMStars->UnloadCatalog( );
		m_pCatalogMStars->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadCcdmFileFromVizier
// Class:	CVizier
// Purpose:	load CCDM catalog from file from vizier
// Input:	file name, region flag
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::LoadCcdmFileFromVizier( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
	// other variables used
	unsigned long nCatNo=0, nLongVar=0, nNObs=0;
	double nRaDeg=0, nDecDeg=0, nObs=0.0, nPa=0.0, nSep=0.0, nMag=0.0,
			nPmRA=0.0, nPmDEC=0.0;
	int bFoundStart=0;
	int bError=0;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// do (un)allocation
	m_pCatalogMStars->UnloadCatalog( );
	// main allocation
	if( bRegion )
		m_pCatalogMStars->m_nAllocated = 1000;
	else
		m_pCatalogMStars->m_nAllocated = 105838;
	m_pCatalogMStars->m_vectData = (DefCatMStars*) malloc( m_pCatalogMStars->m_nAllocated*sizeof(DefCatMStars) );

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
		if( strLine[0] == '#' )
			continue;
		
		// check for last line before data
		if( !bFoundStart && strLine[0] == '-' && strLine[1] == '-' && strLine[2] == '-' )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogMStars->m_nData >= m_pCatalogMStars->m_nAllocated  )
		{
			// incremen counter
			m_pCatalogMStars->m_nAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogMStars->m_vectData = (DefCatMStars*) realloc( m_pCatalogMStars->m_vectData, (m_pCatalogMStars->m_nAllocated+1)*sizeof(DefCatMStars) );
		}

		// set this to wx string
		wxString strWxLine = strLine;

		// get cat no
		wxString strCatNo = strWxLine.Mid( 0, 8 ).Trim(0).Trim(1);
		if( !strCatNo.ToULong( &nCatNo ) ) 
				continue;

		// :: name
		wxString strName = strWxLine.Mid( 9, 10 ).Trim(0).Trim(1);

		// :: RA
		if( !strWxLine.Mid( 20, 8 ).Trim(0).Trim(1).ToDouble( &nRaDeg ) )
			continue;
		// :: DEC
		if( !strWxLine.Mid( 29, 8 ).Trim(0).Trim(1).ToDouble( &nDecDeg ) )
			continue;

		// :: RComp
		wxString strRComp = strWxLine.Mid( 38, 1 ).Trim(0).Trim(1);
		// :: comp
		wxString strComp = strWxLine.Mid( 40, 1 ).Trim(0).Trim(1);
		// :: disc
		wxString strDisc = strWxLine.Mid( 42, 1 ).Trim(0).Trim(1);

		// :: obs - year
		if( !strWxLine.Mid( 50, 4 ).Trim(0).Trim(1).ToDouble( &nObs ) ) nObs = 0;
		// :: pa - theta
		if( !strWxLine.Mid( 55, 3 ).Trim(0).Trim(1).ToDouble( &nPa ) ) nPa = 0;
	
		// :: sep - rho
		if( !strWxLine.Mid( 59, 6 ).Trim(0).Trim(1).ToDouble( &nSep ) ) nSep = 0;
		
		// :: number of obs
		if( !strWxLine.Mid( 66, 2 ).Trim(0).Trim(1).ToULong( &nNObs ) ) nNObs = 0;

		// :: mag - Vmag
		if( !strWxLine.Mid( 69, 4 ).Trim(0).Trim(1).ToDouble( &nMag ) ) nMag = 0;

		// :: spectral type
		wxString strSpecType = strWxLine.Mid( 74, 2 ).Trim(0).Trim(1);

		// :: PMnote
		wxString strPMNote = strWxLine.Mid( 77, 1 ).Trim(0);

		// :: pmRA
		if( !strWxLine.Mid( 79, 5 ).Trim(0).Trim(1).ToDouble( &nPmRA ) ) nPmRA = 0;
		// :: pmDEC
		if( !strWxLine.Mid( 85, 5 ).Trim(0).Trim(1).ToDouble( &nPmDEC ) ) nPmDEC = 0;

		// :: notes1
		wxString strNotes1 = strWxLine.Mid( 91, 1 ).Trim(0);
		// :: notes2
		wxString strNotes2 = strWxLine.Mid( 93, 1 ).Trim(0);

		// set mstars
		// :: name - i should have allocated strlen(strName) - but because of export binary - i wont
//#ifndef _DEBUG
//		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].cat_name = (char*) malloc( 23*sizeof(char) );
//#endif
		//bzero( m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].cat_name, 24 );
		ResetObjCatMStars( m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData] );

		// set name
		strcpy( m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].cat_name, strName.ToAscii() );
		// :: cat no and type 
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].cat_no = nCatNo;
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].cat_type = CAT_OBJECT_TYPE_CCDM;
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].type = SKY_OBJECT_TYPE_MSTARS;

		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].ra = nRaDeg;
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].dec = nDecDeg;

		strcpy( m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].discoverer, strDisc.ToAscii() );
		strcpy( m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].comp, strComp.ToAscii() );

		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].nobs = (unsigned int) nNObs;
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].obs_date = nObs;
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].pos_ang = nPa;
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].sep = nSep;
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].mag = nMag;

		strcpy( m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].spectral_type, strSpecType.ToAscii() );

		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].pm_ra = nPmRA;
		m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].pm_dec = nPmDEC;

		strcpy( m_pCatalogMStars->m_vectData[m_pCatalogMStars->m_nData].notes, strNotes1.ToAscii() );

		// increase star counter
		m_pCatalogMStars->m_nData++;

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
		m_pCatalogMStars->m_nCatalogLoaded = CATALOG_ID_CCDM;
		return( m_pCatalogMStars->m_nData );

	} else
	{
		m_pCatalogMStars->UnloadCatalog( );
		m_pCatalogMStars->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadOmpFileFromVizier
// Class:	CVizier
// Purpose:	load OMP catalog from file from vizier
// Input:	file name, region flag
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::LoadOmpFileFromVizier( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
	// other variables used
	unsigned long nCatNo=0, nLongVar=0, nAstNo, nNArc=0.0, nNObs=0;
	double nRaDeg=0, nDecDeg=0, nMagH=0.0, nMagSlope=0.0, nMagBV=0.0, nDiam=0.0,
			nMeanAnomaly=0.0, nArgPerih=0.0, nAscNode=0.0, nIncl=0.0, 
			nEcc=0.0, nSMajAxis=0.0, nEpoch=0.0, nDateOrb=0.0, nModDate=0.0;
	int bFoundStart=0;
	int bError=0;
	// process date
	wxDateTime mydate;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// do (un)allocation
	m_pCatalogAsteroids->UnloadCatalog( );
	// main allocation
	if( bRegion )
		m_pCatalogAsteroids->m_nAllocated = 1000;
	else
		m_pCatalogAsteroids->m_nAllocated = 455000;
	m_pCatalogAsteroids->m_vectData = (DefCatAsteroid*) malloc( m_pCatalogAsteroids->m_nAllocated*sizeof(DefCatAsteroid) );

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
		if( strLine[0] == '#' )
			continue;
		
		// check for last line before data
		if( !bFoundStart && strLine[0] == '-' && strLine[1] == '-' && strLine[2] == '-' )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogAsteroids->m_nData >= m_pCatalogAsteroids->m_nAllocated  )
		{
			// incremen counter
			m_pCatalogAsteroids->m_nAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogAsteroids->m_vectData = (DefCatAsteroid*) realloc( m_pCatalogAsteroids->m_vectData, (m_pCatalogAsteroids->m_nAllocated+1)*sizeof(DefCatAsteroid) );
		}

		// set this to wx string
		wxString strWxLine = strLine;

		// get cat no
		wxString strCatNo = strWxLine.Mid( 0, 8 ).Trim(0).Trim(1);
		if( !strCatNo.ToULong( &nCatNo ) ) 
				continue;

		// :: asteroid number
		if( !strWxLine.Mid( 9, 6 ).Trim(0).Trim(1).ToULong( &nAstNo ) ) nAstNo = 0;

		// :: name
		wxString strName = strWxLine.Mid( 16, 18 ).Trim(0).Trim(1);
		// :: comp name
		wxString strCompName = strWxLine.Mid( 25, 15 ).Trim(0).Trim(1);

		// :: mag h
		if( !strWxLine.Mid( 51, 5 ).Trim(0).Trim(1).ToDouble( &nMagH ) )
			nMagH = 0.0;
		// :: mag slope
		if( !strWxLine.Mid( 57, 5 ).Trim(0).Trim(1).ToDouble( &nMagSlope ) )
			nMagSlope = 0.0;
		// :: mag b-v
		if( !strWxLine.Mid( 63, 5 ).Trim(0).Trim(1).ToDouble( &nMagBV ) )
			nMagBV = 0.0;
		// :: diameter(km)
		if( !strWxLine.Mid( 69, 5 ).Trim(0).Trim(1).ToDouble( &nDiam ) )
			nDiam = 0.0;

		// :: IRAScl
		wxString strIRAScl = strWxLine.Mid( 75, 4 ).Trim(0).Trim(1);

		// :: Orbital arc, days, spanned by observations used in orbit computation.
		if( !strWxLine.Mid( 80, 5 ).Trim(0).Trim(1).ToULong( &nNArc ) ) nNArc = 0;
		// :: Number of observations used in orbit computation.
		if( !strWxLine.Mid( 86, 4 ).Trim(0).Trim(1).ToULong( &nNObs ) ) nNObs = 0.0;

		// :: Epoch
		mydate.ParseDate( strWxLine.Mid( 91, 10 ).Trim(0).Trim(1) );
		nEpoch = mydate.GetJDN();

		// :: Mean anomaly
		if( !strWxLine.Mid( 102, 10 ).Trim(0).Trim(1).ToDouble( &nMeanAnomaly ) ) nMeanAnomaly = 0.0;
		// :: arg_perih
		if( !strWxLine.Mid( 113, 10 ).Trim(0).Trim(1).ToDouble( &nArgPerih ) ) nArgPerih = 0.0;
		// :: asc_node
		if( !strWxLine.Mid( 124, 10 ).Trim(0).Trim(1).ToDouble( &nAscNode ) ) nAscNode = 0.0;

		// :: Inclination
		if( !strWxLine.Mid( 135, 10 ).Trim(0).Trim(1).ToDouble( &nIncl ) ) nIncl = 0.0;
		// :: Eccentricity
		if( !strWxLine.Mid( 146, 11 ).Trim(0).Trim(1).ToDouble( &nEcc ) ) nEcc = 0.0;
		// :: Semimajor axis
		if( !strWxLine.Mid( 158, 13 ).Trim(0).Trim(1).ToDouble( &nSMajAxis ) ) nSMajAxis = 0.0;

		// :: Date Orb
		mydate.ParseDate( strWxLine.Mid( 172, 10 ).Trim(0).Trim(1) );
		nDateOrb = mydate.GetJDN();
		// :: mod
		char chMod = strWxLine.Mid( 183, 1 ).Trim(0).Trim(1)[0];
		// :: modDate
		mydate.ParseDate( strWxLine.Mid( 185, 10 ).Trim(0).Trim(1) );
		nModDate = mydate.GetJDN();

		// set mstars
		// :: name - i should have allocated strlen(strName) - but because of export binary - i wont
//#ifndef _DEBUG
//		m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].cat_name = (char*) malloc( 19*sizeof(char) );
//#endif
		//bzero( m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].cat_name, 24 );
		ResetObjCatAsteroid( m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData] );

		// set name
		strcpy( m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].cat_name, strName.ToAscii() );
		// :: cat no and type 
		m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].cat_no = nCatNo;
		m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].cat_type = CAT_OBJECT_TYPE_OMP;
		m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].type = SKY_OBJECT_TYPE_ASTEROID;

		m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].ast_no = nAstNo;
		// set comp name
		strcpy( m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].comp_name, strCompName.ToAscii() );

		m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].abs_mag_h = nMagH;
		m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].slope_mag = nMagSlope;
		m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].color_index = nMagBV;
		m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].iras_diam = nDiam;

		// set comp name
		strcpy( m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].iras_class, strIRAScl.ToAscii() );

		m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].no_arc = nNArc;
		m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].no_obs = (unsigned int) nNObs;

		m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].epoch = nEpoch;
		m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].mean_anomaly = nMeanAnomaly;
		m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].arg_perihelion = nArgPerih;
		m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].lon_asc_node = nAscNode;

		m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].date_orb_comp = nDateOrb;
		m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].update_date = nModDate;

		m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].inclination = nIncl;
		m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].eccentricity = nEcc;
		m_pCatalogAsteroids->m_vectData[m_pCatalogAsteroids->m_nData].smaj_axis = nSMajAxis;

		// increase star counter
		m_pCatalogAsteroids->m_nData++;

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
		m_pCatalogAsteroids->m_nCatalogLoaded = CATALOG_ID_OMP;
		return( m_pCatalogAsteroids->m_nData );

	} else
	{
		m_pCatalogAsteroids->UnloadCatalog( );
		m_pCatalogAsteroids->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadCocomFileFromVizier
// Class:	CVizier
// Purpose:	load COCOM catalog from file from vizier
// Input:	file name, region flag
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::LoadCocomFileFromVizier( const wxString& strFile, int bRegion )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
//	wxString strWxLine = "";
	// other variables used
	unsigned long nCatNo=0, nLongVar=0, nRelEffect=0;
	double nRaDeg=0, nDecDeg=0, nUpdateDate=0.0, nCompEpoch=0.0, nXPosVect=0.0,
			nYPosVect=0.0, nZPosVect=0.0, nXVelVect=0.0, nYVelVect=0.0, 
			nZVelVect=0.0, nXNonGravForce=0.0, nYNonGravForce=0.0,
			nZNonGravForce=0.0, nPerihDate=0.0, nPerihDist=0.0, nEcc=0.0,
			nArgPerih=0.0, nLonOrbNode=0.0, nIncl=0.0, nH1=0.0, nR1=0.0, 
			nD1=0.0, nH2=0.0, nR2=0.0, nD2=0.0;
	int bFoundStart=0;
	int bError=0;
	// process date
	wxDateTime mydate;

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// do (un)allocation
	m_pCatalogComets->UnloadCatalog( );
	// main allocation
	if( bRegion )
		m_pCatalogComets->m_nAllocated = 1000;
	else
		m_pCatalogComets->m_nAllocated = 1000;
	m_pCatalogComets->m_vectData = (DefCatComet*) malloc( m_pCatalogComets->m_nAllocated*sizeof(DefCatComet) );

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
		if( strLine[0] == '#' )
			continue;
		
		// check for last line before data
		if( !bFoundStart && strLine[0] == '-' && strLine[1] == '-' && strLine[2] == '-' )
		{
			bFoundStart = 1;
			continue;
		}

		if( !bFoundStart ) continue;

		//////////////////////
		// check for memory allocation
		if( m_pCatalogComets->m_nData >= m_pCatalogComets->m_nAllocated  )
		{
			// incremen counter
			m_pCatalogComets->m_nAllocated += VECT_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_pCatalogComets->m_vectData = (DefCatComet*) realloc( m_pCatalogComets->m_vectData, (m_pCatalogComets->m_nAllocated+1)*sizeof(DefCatComet) );
		}

		// set this to wx string
		wxString strWxLine = strLine;

		// get cat no
		wxString strCatNo = strWxLine.Mid( 0, 8 ).Trim(0).Trim(1);
		if( !strCatNo.ToULong( &nCatNo ) ) 
				continue;

		// :: Upade date
		mydate.ParseDate( strWxLine.Mid( 9, 10 ).Trim(0).Trim(1) );
		nUpdateDate = mydate.GetJDN();

		// :: code - IAU code for the comet
		wxString strIAUCode = strWxLine.Mid( 20, 12 ).Trim(0).Trim(1);
		// :: name - IAU Name of the comet
		wxString strIAUName = strWxLine.Mid( 33, 28 ).Trim(0).Trim(1);
		// :: comp name - Orbit Computer name
		wxString strCompName = strWxLine.Mid( 62, 9 ).Trim(0).Trim(1);

		// :: elem - Epoch of the computed elements (JD)
		if( !strWxLine.Mid( 72, 9 ).Trim(0).Trim(1).ToDouble( &nCompEpoch ) )
			nCompEpoch = 0.0;
		// :: n_elem - [0,1] relativistic effects
		if( !strWxLine.Mid( 82, 1 ).Trim(0).Trim(1).ToULong( &nRelEffect ) )
			nRelEffect = 0;

		// :: X0 - X coordinate of position vector
		if( !strWxLine.Mid( 84, 21 ).Trim(0).Trim(1).ToDouble( &nXPosVect ) )
			nXPosVect = 0.0;
		// :: Y0 - Y coordinate of position vector
		if( !strWxLine.Mid( 106, 21 ).Trim(0).Trim(1).ToDouble( &nYPosVect ) )
			nYPosVect = 0.0;
		// :: Z0 - Z coordinate of position vector
		if( !strWxLine.Mid( 128, 21 ).Trim(0).Trim(1).ToDouble( &nZPosVect ) )
			nZPosVect = 0.0;

		// :: Xdot - X-component of Velocity vector
		if( !strWxLine.Mid( 150, 21 ).Trim(0).Trim(1).ToDouble( &nXVelVect ) )
			nXVelVect = 0.0;
		// :: Ydot - Y-component of Velocity vector
		if( !strWxLine.Mid( 172, 21 ).Trim(0).Trim(1).ToDouble( &nYVelVect ) )
			nYVelVect = 0.0;
		// :: Zdot - Z-component of Velocity vector
		if( !strWxLine.Mid( 194, 21 ).Trim(0).Trim(1).ToDouble( &nZVelVect ) )
			nZVelVect = 0.0;

		// :: A1 - X-component of non-gravitational forces
		if( !strWxLine.Mid( 216, 21 ).Trim(0).Trim(1).ToDouble( &nXNonGravForce ) )
			nXNonGravForce = 0.0;
		// :: A2 - Y-component of non-gravitational forces
		if( !strWxLine.Mid( 238, 21 ).Trim(0).Trim(1).ToDouble( &nYNonGravForce ) )
			nYNonGravForce = 0.0;
		// :: A3 - Z-component of non-gravitational forces
		if( !strWxLine.Mid( 260, 21 ).Trim(0).Trim(1).ToDouble( &nZNonGravForce ) )
			nZNonGravForce = 0.0;

		// :: T0 - Date of perihelion
		if( !strWxLine.Mid( 282, 14 ).Trim(0).Trim(1).ToDouble( &nPerihDate ) )
			nPerihDate = 0.0;
		// :: q - Perihelion distance
		if( !strWxLine.Mid( 297, 12 ).Trim(0).Trim(1).ToDouble( &nPerihDist ) )
			nPerihDist = 0.0;

		// :: e - Orbit eccentricity
		if( !strWxLine.Mid( 310, 10 ).Trim(0).Trim(1).ToDouble( &nEcc ) ) nEcc = 0.0;
		// :: omega - Argument of perihelion
		if( !strWxLine.Mid( 321, 10 ).Trim(0).Trim(1).ToDouble( &nArgPerih ) ) nArgPerih = 0.0;
		// :: Omega - Longitude of orbital node
		if( !strWxLine.Mid( 332, 10 ).Trim(0).Trim(1).ToDouble( &nLonOrbNode ) ) nLonOrbNode = 0.0;
		// :: i - Inclination of the orbit
		if( !strWxLine.Mid( 343, 10 ).Trim(0).Trim(1).ToDouble( &nIncl ) ) nIncl = 0.0;

		// :: H1 - Constant term for magnitude
		if( !strWxLine.Mid( 354, 5 ).Trim(0).Trim(1).ToDouble( &nH1 ) ) nH1 = 0.0;
		// :: R1 - Term in log(r) for magnitude
		if( !strWxLine.Mid( 360, 5 ).Trim(0).Trim(1).ToDouble( &nR1 ) ) nR1 = 0.0;
		// :: D1 - Term in log(D) for magnitude
		if( !strWxLine.Mid( 366, 5 ).Trim(0).Trim(1).ToDouble( &nD1 ) ) nD1 = 0.0;

		// :: H2 - Constant term for magnitude of nucleus
		if( !strWxLine.Mid( 372, 5 ).Trim(0).Trim(1).ToDouble( &nH2 ) ) nH2 = 0.0;
		// :: R2 - Term in log(r) for magnitude of nucleus
		if( !strWxLine.Mid( 378, 5 ).Trim(0).Trim(1).ToDouble( &nR2 ) ) nR2 = 0.0;
		// :: D2 - Term in log(D) for magnitude of nucleus
		if( !strWxLine.Mid( 384, 5 ).Trim(0).Trim(1).ToDouble( &nD2 ) ) nD2 = 0.0;

		// :: comment note
		wxString strCommentNote = strWxLine.Mid( 390, 4 ).Trim(0).Trim(1);

		// set comets
		// :: name - i should have allocated strlen(strName) - but because of export binary - i wont
//#ifndef _DEBUG
//		m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].cat_name = (char*) malloc( 19*sizeof(char) );
//#endif
		//bzero( m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].cat_name, 24 );
		ResetObjCatComet( m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData] );

		// set name
		wxStrcpy( m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].cat_name, strIAUName );
		// :: cat no and type 
		m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].cat_no = nCatNo;
		m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].cat_type = CAT_OBJECT_TYPE_COCOM;
		m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].type = SKY_OBJECT_TYPE_COMET;

		// set comp name
		wxStrcpy( m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].iau_code, strIAUCode );
		// set comp name
		wxStrcpy( m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].comp_name, strCompName );

		m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].note_update_date = nUpdateDate;

		m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].epoch_comp = nCompEpoch;
		m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].rel_effect = (unsigned char) nRelEffect;
		m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].perihelion_date = nPerihDate;
		m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].perihelion_distance = nPerihDist;

		m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].eccentricity = nEcc;
		m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].arg_perihelion = nArgPerih;

		m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].lon_orbital_node = nLonOrbNode;
		m_pCatalogComets->m_vectData[m_pCatalogComets->m_nData].inclination = nIncl;

		// increase star counter
		m_pCatalogComets->m_nData++;

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
		m_pCatalogComets->m_nCatalogLoaded = CATALOG_ID_COCOM;
		return( m_pCatalogComets->m_nData );

	} else
	{
		m_pCatalogComets->UnloadCatalog( );
		m_pCatalogComets->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	GetStarsFromVizier
// Class:	CVizier
// Purpose:	load star from online vizier
// Input:	position, size, catalog id 
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::GetStarsFromVizier( int nCatId, int bRegion, double nCenterRA, double nCenterDEC,
									double nWidth, double nHeight )
{
	wxString strLog;
	wxString strLocalFile;
	int i=0, j=0;
	wxString strCatName = wxT("UnknownCat");
	long nLoadedObjects=0, nLoadedBytes=0;

	/////////////////////////////////////
	// calculate fetch radius ??? - should be by calc distance ?
	double nFetchRadius = (double) ( (double) sqrt( (double) nWidth*nWidth + (double) nHeight*nHeight )/2 );

	/////////////////
	// here we need to calculate max rad based on VB
	double nMaxRad = 1.5;

	///////////////////////////
	// set vars by cat type
	// :: STARS
	if( nCatId == CATALOG_ID_USNO_B1 )
		strCatName = wxT("USNBO-10");
	else if( nCatId == CATALOG_ID_SAO )
		strCatName = wxT("SAO");
	else if( nCatId == CATALOG_ID_HIPPARCOS )
		strCatName = wxT("HIPPARCOS");
	else if( nCatId == CATALOG_ID_TYCHO_2 )
		strCatName = wxT("TYCHO-2");
	else if( nCatId == CATALOG_ID_NOMAD )
		strCatName = wxT("NOMAD");
	else if( nCatId == CATALOG_ID_2MASS )
		strCatName = wxT("2MASS");
	else if( nCatId == CATALOG_ID_GSC )
		strCatName = wxT("GSC");
	// :: DSO

	// build name for local file
	strLocalFile.Printf( wxT("%s/%s_%.6f_%.6f_%.4f_%.4f_cat.txt"), unMakeAppPath(wxT("data/remote/Vizier")),
				strCatName, nCenterRA, nCenterDEC, nWidth, nHeight );

	// log info
	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("INFO :: Fetching area orig(%.2f,%.2f) with radius=%.2f..."),
									nCenterRA, nCenterDEC, nFetchRadius );
		m_pUnimapWorker->Log( strLog );
	}

	/////////
	// call method to get stars from online site
	nLoadedBytes = FetchObjectsFromVizier( strLocalFile, nCenterRA, nCenterDEC, nFetchRadius, nCatId );

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
								nCenterRA, nCenterDEC, nFetchRadius );
		m_pUnimapWorker->Log( strLog );
	}

	////////////////////////////
	// and now load the file just downloaded
	if( nCatId == CATALOG_ID_USNO_B1 )
	{
		nLoadedObjects = LoadUSNOB1FileFromVizier( strLocalFile );

	} else if( nCatId == CATALOG_ID_SAO )
	{
		nLoadedObjects = LoadSAOFileFromVizier( strLocalFile, bRegion );

	} else if( nCatId == CATALOG_ID_HIPPARCOS )
	{
		nLoadedObjects = LoadHIPPARCOSFileFromVizier( strLocalFile, bRegion );

	} else if( nCatId == CATALOG_ID_TYCHO_2 )
	{
		nLoadedObjects = LoadTYCHOFileFromVizier( strLocalFile, bRegion );

	} else if( nCatId == CATALOG_ID_NOMAD )
	{
		nLoadedObjects = LoadNOMADFileFromVizier( strLocalFile );

	} else if( nCatId == CATALOG_ID_2MASS )
	{
		nLoadedObjects = Load2MASSFileFromVizier( strLocalFile );

	} else if( nCatId == CATALOG_ID_GSC )
	{
		nLoadedObjects = LoadGSCFileFromVizier( strLocalFile );

	} else
		return( 0 );

	// check if anything was loaded
	if( nLoadedObjects > 0 )
	{
		// and now sort my catalog by magnitude
		if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Sort catalog objects ...") );
		m_pCatalogStars->SortByMagnitude( );

		// log message
		if( m_pUnimapWorker )
		{
			strLog.Printf( wxT("INFO :: loaded %d stars"), m_pCatalogStars->m_nStar );
			m_pUnimapWorker->Log( strLog );
		}

	} else
	{
		// log message
		if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT("INFO :: failed to get stars from vizier") );
	}

	return( nLoadedObjects );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetDsoFromVizier
// Class:	CVizier
// Purpose:	load dso object from online vizier
// Input:	position, size, catalog id 
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::GetDsoFromVizier( int nCatId, int bRegion, double nCenterRA, double nCenterDEC,
									double nWidth, double nHeight )
{
	wxString strLog;
	wxString strLocalFile;
	int i=0, j=0;
	wxString strCatName = wxT("UnknownCat");
	long nLoadedObjects=0, nLoadedBytes=0;

	/////////////////////////////////////
	// calculate fetch radius ??? - should be by calc distance ?
	double nFetchRadius = (double) ( (double) sqrt( (double) nWidth*nWidth + (double) nHeight*nHeight )/2 );

	/////////////////
	// here we need to calculate max rad based on VB
	double nMaxRad = 1.5;

	///////////////////////////
	// set vars by cat type
	// :: DSO
	if( nCatId == CATALOG_ID_HYPERLEDA )
		strCatName = wxT("HYPERLEDA");
	else if( nCatId == CATALOG_ID_MGC )
		strCatName = wxT("MGC");
	else if( nCatId == CATALOG_ID_UGC )
		strCatName = wxT("UGC");

	// build name for local file
	strLocalFile.Printf( wxT("%s/%s_%.6f_%.6f_%.4f_%.4f_cat.txt"), unMakeAppPath(wxT("data/remote/Vizier")),
				strCatName, nCenterRA, nCenterDEC, nWidth, nHeight );

	// log info
	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("INFO :: Fetching area orig(%.2f,%.2f) with radius=%.2f..."),
									nCenterRA, nCenterDEC, nFetchRadius );
		m_pUnimapWorker->Log( strLog );
	}

	/////////
	// call method to get stars from online site
	nLoadedBytes = FetchObjectsFromVizier( strLocalFile, nCenterRA, nCenterDEC, nFetchRadius, nCatId );

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
								nCenterRA, nCenterDEC, nFetchRadius );
		m_pUnimapWorker->Log( strLog );
	}

	////////////////////////////
	// and now load the file just downloaded
	if( nCatId == CATALOG_ID_HYPERLEDA )
	{
		nLoadedObjects = LoadHyperledaFileFromVizier( strLocalFile, bRegion );

	} else if( nCatId == CATALOG_ID_MGC )
	{
		nLoadedObjects = LoadMgcFileFromVizier( strLocalFile, bRegion );

	} else if( nCatId == CATALOG_ID_UGC )
	{
		nLoadedObjects = LoadUgcFileFromVizier( strLocalFile, bRegion );

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
		if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT("INFO :: failed to get dso objects from vizier") );
	}

	return( nLoadedObjects );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetRadioFromVizier
// Class:	CVizier
// Purpose:	get radio sources from online vizier
// Input:	position, size, catalog id 
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::GetRadioFromVizier( int nCatId, int bRegion, double nCenterRA, double nCenterDEC,
									double nWidth, double nHeight )
{
	wxString strLog;
	wxString strLocalFile;
	int i=0, j=0;
	wxString strCatName = wxT("UnknownCat");
	long nLoadedObjects=0, nLoadedBytes=0;

	/////////////////////////////////////
	// calculate fetch radius ??? - should be by calc distance ?
	double nFetchRadius = (double) ( (double) sqrt( (double) nWidth*nWidth + (double) nHeight*nHeight )/2 );

	/////////////////
	// here we need to calculate max rad based on VB
	double nMaxRad = 1.5;

	///////////////////////////
	// set vars by cat type
	// :: DSO
	if( nCatId == CATALOG_ID_SPECFIND )
		strCatName = wxT("SPECFIND");
	else if( nCatId == CATALOG_ID_VLSS )
		strCatName = wxT("VLSS");
	else if( nCatId == CATALOG_ID_NVSS )
		strCatName = wxT("NVSS");
	else if( nCatId == CATALOG_ID_MSL )
		strCatName = wxT("MSL");

	// build name for local file
	strLocalFile.Printf( wxT("%s/%s_%.6f_%.6f_%.4f_%.4f_cat.txt"), unMakeAppPath(wxT("data/remote/Vizier")),
				strCatName, nCenterRA, nCenterDEC, nWidth, nHeight );

	// log info
	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("INFO :: Fetching radio sources in area orig(%.2f,%.2f) with radius=%.2f..."),
									nCenterRA, nCenterDEC, nFetchRadius );
		m_pUnimapWorker->Log( strLog );
	}

	/////////
	// call method to get stars from online site
	nLoadedBytes = FetchObjectsFromVizier( strLocalFile, nCenterRA, nCenterDEC, nFetchRadius, nCatId );

	// check if anything was loaded
	if( nLoadedBytes <=0 )
	{
		if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT("Failed to fetch remote data from vizier...") );
		return( 0 );
	}

	// log info
	if( m_pUnimapWorker )
	{
		m_pUnimapWorker->SetWaitDialogMsg( wxT("Import data to local format ...") );

		strLog.Printf( wxT("INFO :: Loading file for area orig(%.2f,%.2f) with radius=%.2f..."),
								nCenterRA, nCenterDEC, nFetchRadius );
		m_pUnimapWorker->Log( strLog );
	}

	////////////////////////////
	// and now load the file just downloaded
	if( nCatId == CATALOG_ID_SPECFIND )
	{
		nLoadedObjects = LoadSpecfindFileFromVizier( strLocalFile, bRegion );

	} else if( nCatId == CATALOG_ID_VLSS )
	{
		nLoadedObjects = LoadVlssFileFromVizier( strLocalFile, bRegion );

	} else if( nCatId == CATALOG_ID_NVSS )
	{
		nLoadedObjects = LoadNvssFileFromVizier( strLocalFile, bRegion );

	} else if( nCatId == CATALOG_ID_MSL )
	{
		nLoadedObjects = LoadMslFileFromVizier( strLocalFile, bRegion );

	} else
		return( 0 );

	// check if anything was loaded
	if( nLoadedObjects > 0 )
	{
		// and now sort my catalog by magnitude
//		if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( "Sort catalog objects ..." );
//		m_pCatalogRadio->SortBy??( );

		// log message
		if( m_pUnimapWorker )
		{
			strLog.Printf( wxT("INFO :: loaded %d dso objects"), m_pCatalogRadio->m_nData );
			m_pUnimapWorker->Log( strLog );
		}

	} else
	{
		// log message
		if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT("INFO :: failed to get radio sources from vizier") );
	}

	return( nLoadedObjects );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetXGammaFromVizier
// Class:	CVizier
// Purpose:	get x-ray/gamma sources from online vizier
// Input:	position, size, catalog id 
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::GetXGammaFromVizier( int nCatId, int bRegion, double nCenterRA, double nCenterDEC,
									double nWidth, double nHeight )
{
	wxString strLog;
	wxString strLocalFile;
	int i=0, j=0;
	wxString strCatName = wxT("UnknownCat");
	long nLoadedObjects=0, nLoadedBytes=0;

	/////////////////////////////////////
	// calculate fetch radius ??? - should be by calc distance ?
	double nFetchRadius = (double) ( (double) sqrt( (double) nWidth*nWidth + (double) nHeight*nHeight )/2 );

	/////////////////
	// here we need to calculate max rad based on VB
	double nMaxRad = 1.5;

	///////////////////////////
	// set vars by cat type
	// :: DSO
	if( nCatId == CATALOG_ID_ROSAT )
		strCatName = wxT("ROSAT");
	else if( nCatId == CATALOG_ID_BATSE )
		strCatName = wxT("BATSE");
	else if( nCatId == CATALOG_ID_INTEGRAL )
		strCatName = wxT("INTEGRAL");
	else if( nCatId == CATALOG_ID_BEPPOSAX_GRBM )
		strCatName = wxT("BEPPOSAX_GRBM");
	else if( nCatId == CATALOG_ID_2XMMi )
		strCatName = wxT("2XMMi");
	else if( nCatId == CATALOG_ID_HEAO_A1 )
		strCatName = wxT("HEAO_A1");

	// build name for local file
	strLocalFile.Printf( wxT("%s/%s_%.6f_%.6f_%.4f_%.4f_cat.txt"), unMakeAppPath(wxT("data/remote/Vizier")),
				strCatName, nCenterRA, nCenterDEC, nWidth, nHeight );

	// log info
	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("INFO :: Fetching x-ray/gamma sources in area orig(%.2f,%.2f) with radius=%.2f..."),
									nCenterRA, nCenterDEC, nFetchRadius );
		m_pUnimapWorker->Log( strLog );
	}

	/////////
	// call method to get stars from online site
	nLoadedBytes = FetchObjectsFromVizier( strLocalFile, nCenterRA, nCenterDEC, nFetchRadius, nCatId );

	// check if anything was loaded
	if( nLoadedBytes <=0 )
	{
		if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT("Failed to fetch remote data from vizier...") );
		return( 0 );
	}

	// log info
	if( m_pUnimapWorker )
	{
		m_pUnimapWorker->SetWaitDialogMsg( wxT("Import data to local format ...") );

		strLog.Printf( wxT("INFO :: Loading file for area orig(%.2f,%.2f) with radius=%.2f..."),
								nCenterRA, nCenterDEC, nFetchRadius );
		m_pUnimapWorker->Log( strLog );
	}

	////////////////////////////
	// and now load the file just downloaded
	if( nCatId == CATALOG_ID_ROSAT )
	{
		nLoadedObjects = LoadRosatFileFromVizier( strLocalFile, bRegion );

	} else if( nCatId == CATALOG_ID_BATSE )
	{
		nLoadedObjects = LoadBatseFileFromVizier( strLocalFile, bRegion );

	} else if( nCatId == CATALOG_ID_INTEGRAL )
	{
		nLoadedObjects = LoadIntegralFileFromVizier( strLocalFile, bRegion );

	} else if( nCatId == CATALOG_ID_BEPPOSAX_GRBM )
	{
		nLoadedObjects = LoadBepposaxFileFromVizier( strLocalFile, bRegion );

	} else if( nCatId == CATALOG_ID_2XMMi )
	{
		nLoadedObjects = Load2XMMiFileFromVizier( strLocalFile, bRegion );

	} else if( nCatId == CATALOG_ID_HEAO_A1 )
	{
		nLoadedObjects = LoadHeaoA1FileFromVizier( strLocalFile, bRegion );

	} else
		return( 0 );

	// check if anything was loaded
	if( nLoadedObjects > 0 )
	{
		// and now sort my catalog by magnitude
//		if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( "Sort catalog objects ..." );
//		m_pCatalogXGamma->SortBy??( );

		// log message
		if( m_pUnimapWorker )
		{
			strLog.Printf( wxT("INFO :: loaded %d X-RAY/GAMMA sources"), m_pCatalogXGamma->m_nData );
			m_pUnimapWorker->Log( strLog );
		}

	} else
	{
		// log message
		if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT("INFO :: failed to get x-ray/gamma sources from vizier") );
	}

	return( nLoadedObjects );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetSupernovasFromVizier
// Class:	CVizier
// Purpose:	get supernovas sources from online vizier
// Input:	position, size, catalog id 
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::GetSupernovasFromVizier( int nCatId, int bRegion, double nCenterRA, double nCenterDEC,
									double nWidth, double nHeight )
{
	wxString strLog;
	wxString strLocalFile;
	int i=0, j=0;
	wxString strCatName = wxT("UnknownCat");
	long nLoadedObjects=0, nLoadedBytes=0;

	/////////////////////////////////////
	// calculate fetch radius ??? - should be by calc distance ?
	double nFetchRadius = (double) ( (double) sqrt( (double) nWidth*nWidth + (double) nHeight*nHeight )/2 );

	/////////////////
	// here we need to calculate max rad based on VB
	double nMaxRad = 1.5;

	///////////////////////////
	// set vars by cat type
	// :: DSO
	if( nCatId == CATALOG_ID_ASC )
		strCatName = wxT("ASC");
	else if( nCatId == CATALOG_ID_SSC )
		strCatName = wxT("SSC");
	else
		return( 0 );

	// build name for local file
	strLocalFile.Printf( wxT("%s/%s_%.6f_%.6f_%.4f_%.4f_cat.txt"), unMakeAppPath(wxT("data/remote/Vizier")),
				strCatName, nCenterRA, nCenterDEC, nWidth, nHeight );

	// log info
	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("INFO :: Fetching supernovas in area orig(%.2f,%.2f) with radius=%.2f..."),
									nCenterRA, nCenterDEC, nFetchRadius );
		m_pUnimapWorker->Log( strLog );
	}

	/////////
	// call method to get stars from online site
	nLoadedBytes = FetchObjectsFromVizier( strLocalFile, nCenterRA, nCenterDEC, nFetchRadius, nCatId );

	// check if anything was loaded
	if( nLoadedBytes <=0 )
	{
		if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT("Failed to fetch remote data from vizier...") );
		return( 0 );
	}

	// log info
	if( m_pUnimapWorker )
	{
		m_pUnimapWorker->SetWaitDialogMsg( wxT("Import data to local format ...") );

		strLog.Printf( wxT("INFO :: Loading file for area orig(%.2f,%.2f) with radius=%.2f..."),
								nCenterRA, nCenterDEC, nFetchRadius );
		m_pUnimapWorker->Log( strLog );
	}

	////////////////////////////
	// and now load the file just downloaded
	if( nCatId == CATALOG_ID_ASC )
	{
		nLoadedObjects = LoadAscFileFromVizier( strLocalFile, bRegion );

	} else if( nCatId == CATALOG_ID_SSC )
	{
		nLoadedObjects = LoadSscFileFromVizier( strLocalFile, bRegion );

	} else
		return( 0 );

	// check if anything was loaded
	if( nLoadedObjects > 0 )
	{
		// and now sort my catalog by magnitude
//		if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( "Sort catalog objects ..." );
//		m_pCatalogXGamma->SortBy??( );

		// log message
		if( m_pUnimapWorker )
		{
			strLog.Printf( wxT("INFO :: loaded %d supernovas"), m_pCatalogSupernovas->m_nData );
			m_pUnimapWorker->Log( strLog );
		}

	} else
	{
		// log message
		if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT("INFO :: failed to get supernovas sources from vizier") );
	}

	return( nLoadedObjects );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetBlackholesFromVizier
// Class:	CVizier
// Purpose:	get blackholes sources from online vizier
// Input:	position, size, catalog id 
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::GetBlackholesFromVizier( int nCatId, int bRegion, double nCenterRA, double nCenterDEC,
									double nWidth, double nHeight )
{
	wxString strLog;
	wxString strLocalFile;
	int i=0, j=0;
	wxString strCatName = wxT("UnknownCat");
	long nLoadedObjects=0, nLoadedBytes=0;

	/////////////////////////////////////
	// calculate fetch radius ??? - should be by calc distance ?
	double nFetchRadius = (double) ( (double) sqrt( (double) nWidth*nWidth + (double) nHeight*nHeight )/2 );

	/////////////////
	// here we need to calculate max rad based on VB
	double nMaxRad = 1.5;

	///////////////////////////
	// set vars by cat type
	// :: DSO
	if( nCatId == CATALOG_ID_SDSS_DR5 )
		strCatName = wxT("SDSS-DR5");
	else if( nCatId == CATALOG_ID_RXE_AGN )
		strCatName = wxT("RXE-AGN");
	else
		return( 0 );

	// build name for local file
	strLocalFile.Printf( wxT("%s/%s_%.6f_%.6f_%.4f_%.4f_cat.txt"), unMakeAppPath(wxT("data/remote/Vizier")),
				strCatName, nCenterRA, nCenterDEC, nWidth, nHeight );

	// log info
	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("INFO :: Fetching blackholes in area orig(%.2f,%.2f) with radius=%.2f..."),
									nCenterRA, nCenterDEC, nFetchRadius );
		m_pUnimapWorker->Log( strLog );
	}

	/////////
	// call method to get stars from online site
	nLoadedBytes = FetchObjectsFromVizier( strLocalFile, nCenterRA, nCenterDEC, nFetchRadius, nCatId );

	// check if anything was loaded
	if( nLoadedBytes <=0 )
	{
		if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT("Failed to fetch remote data from vizier...") );
		return( 0 );
	}

	// log info
	if( m_pUnimapWorker )
	{
		m_pUnimapWorker->SetWaitDialogMsg( wxT("Import data to local format ...") );

		strLog.Printf( wxT("INFO :: Loading file for area orig(%.2f,%.2f) with radius=%.2f..."),
								nCenterRA, nCenterDEC, nFetchRadius );
		m_pUnimapWorker->Log( strLog );
	}

	////////////////////////////
	// and now load the file just downloaded
	if( nCatId == CATALOG_ID_SDSS_DR5 )
	{
		nLoadedObjects = LoadSdssdr5FileFromVizier( strLocalFile, bRegion );

	} else if( nCatId == CATALOG_ID_RXE_AGN )
	{
		nLoadedObjects = LoadRxeagnFileFromVizier( strLocalFile, bRegion );

	} else
		return( 0 );

	// check if anything was loaded
	if( nLoadedObjects > 0 )
	{
		// and now sort my catalog by magnitude
		if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Sort catalog objects ...") );
		m_pCatalogBlackholes->SortByMass( );

		// log message
		if( m_pUnimapWorker )
		{
			strLog.Printf( wxT("INFO :: loaded %d blackholes"), m_pCatalogBlackholes->m_nData );
			m_pUnimapWorker->Log( strLog );
		}

	} else
	{
		// log message
		if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT("INFO :: failed to get blackholes from vizier") );
	}

	return( nLoadedObjects );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetMStarsFromVizier
// Class:	CVizier
// Purpose:	get multiple/double stars from online vizier
// Input:	position, size, catalog id 
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::GetMStarsFromVizier( int nCatId, int bRegion, double nCenterRA, double nCenterDEC,
									double nWidth, double nHeight )
{
	wxString strLog;
	wxString strLocalFile;
	int i=0, j=0;
	wxString strCatName = wxT("UnknownCat");
	long nLoadedObjects=0, nLoadedBytes=0;

	/////////////////////////////////////
	// calculate fetch radius ??? - should be by calc distance ?
	double nFetchRadius = (double) ( (double) sqrt( (double) nWidth*nWidth + (double) nHeight*nHeight )/2 );

	/////////////////
	// here we need to calculate max rad based on VB
	double nMaxRad = 1.5;

	///////////////////////////
	// set vars by cat type
	// :: DSO
	if( nCatId == CATALOG_ID_WDS )
		strCatName = wxT("WDS");
	else if( nCatId == CATALOG_ID_CCDM )
		strCatName = wxT("CCDM");
	else
		return( 0 );

	// build name for local file
	strLocalFile.Printf( wxT("%s/%s_%.6f_%.6f_%.4f_%.4f_cat.txt"), unMakeAppPath(wxT("data/remote/Vizier")),
				strCatName, nCenterRA, nCenterDEC, nWidth, nHeight );

	// log info
	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("INFO :: Fetching multiple stars in area orig(%.2f,%.2f) with radius=%.2f..."),
									nCenterRA, nCenterDEC, nFetchRadius );
		m_pUnimapWorker->Log( strLog );
	}

	/////////
	// call method to get stars from online site
	nLoadedBytes = FetchObjectsFromVizier( strLocalFile, nCenterRA, nCenterDEC, nFetchRadius, nCatId );

	// check if anything was loaded
	if( nLoadedBytes <=0 )
	{
		if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT("Failed to fetch remote data from vizier...") );
		return( 0 );
	}

	// log info
	if( m_pUnimapWorker )
	{
		m_pUnimapWorker->SetWaitDialogMsg( wxT("Import data to local format ...") );

		strLog.Printf( wxT("INFO :: Loading file for area orig(%.2f,%.2f) with radius=%.2f..."),
								nCenterRA, nCenterDEC, nFetchRadius );
		m_pUnimapWorker->Log( strLog );
	}

	////////////////////////////
	// and now load the file just downloaded
	if( nCatId == CATALOG_ID_WDS )
	{
		nLoadedObjects = LoadWdsFileFromVizier( strLocalFile, bRegion );

	} else if( nCatId == CATALOG_ID_CCDM )
	{
		nLoadedObjects = LoadCcdmFileFromVizier( strLocalFile, bRegion );

	} else
		return( 0 );

	// check if anything was loaded
	if( nLoadedObjects > 0 )
	{
		// and now sort my catalog by magnitude
		if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Sort catalog objects ...") );
//		m_pCatalogMStars->SortByMag( );

		// log message
		if( m_pUnimapWorker )
		{
			strLog.Printf( wxT("INFO :: loaded %d multiple stars"), m_pCatalogMStars->m_nData );
			m_pUnimapWorker->Log( strLog );
		}

	} else
	{
		// log message
		if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT("INFO :: failed to get multiple stars from vizier") );
	}

	return( nLoadedObjects );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetAsteroidsFromVizier
// Class:	CVizier
// Purpose:	get asteroids from online vizier
// Input:	position, size, catalog id 
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::GetAsteroidsFromVizier( int nCatId, int bRegion, double nCenterRA, double nCenterDEC,
									double nWidth, double nHeight )
{
	wxString strLog;
	wxString strLocalFile;
	int i=0, j=0;
	wxString strCatName = wxT("UnknownCat");
	long nLoadedObjects=0, nLoadedBytes=0;

	/////////////////////////////////////
	// calculate fetch radius ??? - should be by calc distance ?
	double nFetchRadius = (double) ( (double) sqrt( (double) nWidth*nWidth + (double) nHeight*nHeight )/2 );

	/////////////////
	// here we need to calculate max rad based on VB
	double nMaxRad = 1.5;

	///////////////////////////
	// set vars by cat type
	// :: Orbits of Minor Planets
	if( nCatId == CATALOG_ID_OMP )
		strCatName = wxT("OMP");
	else
		return( 0 );

	// build name for local file
	strLocalFile.Printf( wxT("%s/%s_%.6f_%.6f_%.4f_%.4f_cat.txt"), unMakeAppPath(wxT("data/remote/Vizier")),
				strCatName, nCenterRA, nCenterDEC, nWidth, nHeight );

	// log info
	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("INFO :: Fetching asteroids in area orig(%.2f,%.2f) with radius=%.2f..."),
									nCenterRA, nCenterDEC, nFetchRadius );
		m_pUnimapWorker->Log( strLog );
	}

	/////////
	// call method to get stars from online site
	nLoadedBytes = FetchObjectsFromVizier( strLocalFile, nCenterRA, nCenterDEC, nFetchRadius, nCatId );

	// check if anything was loaded
	if( nLoadedBytes <=0 )
	{
		if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT("Failed to fetch remote data from vizier...") );
		return( 0 );
	}

	// log info
	if( m_pUnimapWorker )
	{
		m_pUnimapWorker->SetWaitDialogMsg( wxT("Import data to local format ...") );

		strLog.Printf( wxT("INFO :: Loading file for area orig(%.2f,%.2f) with radius=%.2f..."),
								nCenterRA, nCenterDEC, nFetchRadius );
		m_pUnimapWorker->Log( strLog );
	}

	////////////////////////////
	// and now load the file just downloaded
	if( nCatId == CATALOG_ID_OMP )
	{
		nLoadedObjects = LoadOmpFileFromVizier( strLocalFile, bRegion );

	} else 
		return( 0 );

	// check if anything was loaded
	if( nLoadedObjects > 0 )
	{
		// and now sort my catalog by magnitude
		if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Sort catalog objects ...") );
//		m_pCatalogMStars->SortByMag( );

		// log message
		if( m_pUnimapWorker )
		{
			strLog.Printf( wxT("INFO :: loaded %d asteroids"), m_pCatalogAsteroids->m_nData );
			m_pUnimapWorker->Log( strLog );
		}

	} else
	{
		// log message
		if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT("INFO :: failed to get asteroids objects from vizier") );
	}

	return( nLoadedObjects );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetCometsFromVizier
// Class:	CVizier
// Purpose:	get comets from online vizier
// Input:	position, size, catalog id 
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CVizier::GetCometsFromVizier( int nCatId, int bRegion, double nCenterRA, double nCenterDEC,
									double nWidth, double nHeight )
{
	wxString strLog;
	wxString strLocalFile;
	int i=0, j=0;
	wxString strCatName = wxT("UnknownCat");
	long nLoadedObjects=0, nLoadedBytes=0;

	/////////////////////////////////////
	// calculate fetch radius ??? - should be by calc distance ?
	double nFetchRadius = (double) ( (double) sqrt( (double) nWidth*nWidth + (double) nHeight*nHeight )/2 );

	/////////////////
	// here we need to calculate max rad based on VB
	double nMaxRad = 1.5;

	///////////////////////////
	// set vars by cat type
	// :: The catalog of Comets
	if( nCatId == CATALOG_ID_COCOM )
		strCatName = wxT("COCOM");
	else
		return( 0 );

	// build name for local file
	strLocalFile.Printf( wxT("%s/%s_%.6f_%.6f_%.4f_%.4f_cat.txt"), unMakeAppPath(wxT("data/remote/Vizier")),
				strCatName, nCenterRA, nCenterDEC, nWidth, nHeight );

	// log info
	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("INFO :: Fetching comets in area orig(%.2f,%.2f) with radius=%.2f..."),
									nCenterRA, nCenterDEC, nFetchRadius );
		m_pUnimapWorker->Log( strLog );
	}

	/////////
	// call method to get stars from online site
	nLoadedBytes = FetchObjectsFromVizier( strLocalFile, nCenterRA, nCenterDEC, nFetchRadius, nCatId );

	// check if anything was loaded
	if( nLoadedBytes <=0 )
	{
		if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT("Failed to fetch remote data from vizier...") );
		return( 0 );
	}

	// log info
	if( m_pUnimapWorker )
	{
		m_pUnimapWorker->SetWaitDialogMsg( wxT("Import data to local format ...") );

		strLog.Printf( wxT("INFO :: Loading file for area orig(%.2f,%.2f) with radius=%.2f..."),
								nCenterRA, nCenterDEC, nFetchRadius );
		m_pUnimapWorker->Log( strLog );
	}

	////////////////////////////
	// and now load the file just downloaded
	if( nCatId == CATALOG_ID_COCOM )
	{
		nLoadedObjects = LoadCocomFileFromVizier( strLocalFile, bRegion );

	} else 
		return( 0 );

	// check if anything was loaded
	if( nLoadedObjects > 0 )
	{
		// and now sort my catalog by magnitude
		if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Sort catalog objects ...") );
//		m_pCatalogComets->SortByMag( );

		// log message
		if( m_pUnimapWorker )
		{
			strLog.Printf( wxT("INFO :: loaded %d comets"), m_pCatalogComets->m_nData );
			m_pUnimapWorker->Log( strLog );
		}

	} else
	{
		// log message
		if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT("INFO :: failed to get comets data from vizier") );
	}

	return( nLoadedObjects );
}

/////////////////////////////////////////////////////////////////////
// Method:	FetchObjectsFromVizier
// Class:	CVizier
// Purpose:	fetch catalog data from online vizier database into a file
// Input:	file name where to fetch center ra/dec and radius, cat id
// Output:	how many bytes where loaded
/////////////////////////////////////////////////////////////////////
long CVizier::FetchObjectsFromVizier( const wxString& strFile, 
										  double nCenterRA, double nCenterDEC, 
										  double nRadius, int nCatId )
{
	int nState = 0;
	int i = 0;
	wxString strLog;
	unsigned long nWebFileSize=0;

	wxString strUrl;
	wxString strCatCode = wxT("");
	wxString strCatFields = wxT("");

	// TODO :: footprint image of the catalog
	// footprint?catid=34370467&amp;-galactic&amp;-proj&amp;aitoff&amp;-col&amp;rainbow&amp;-z&amp;8&amp;-c&amp;0.0+0.0&amp;-s&amp;1
	// - http://cdsarc.u-strasbg.fr/viz-bin/footprint?catid=34370467&J/A%2BA/437/467&-gal&-z&5&-swing&1
	// - http://cdsarc.u-strasbg.fr/viz-bin/footprint?catid=34370467&amp;-galactic&amp;-proj&amp;aitoff&amp;-col&amp;rainbow&amp;-z&amp;6 <- less then 7

	// set vars by cat type
	if( nCatId == CATALOG_ID_USNO_B1 )
	{
		strCatCode = wxT("I/284/out");
		strCatFields = wxT("USNO-B1.0,Tycho-2,RAJ2000,DEJ2000,B1mag,R1mag,B2mag,R2mag");//&R1s/g=%3E5";

	} else if( nCatId == CATALOG_ID_SAO )
	{
		strCatCode = wxT("I/131A/sao");
		strCatFields = wxT("SAO,RA2000,DE2000,Pmag,Vmag");
		// set aprox wefile size - for the meter :p
		nWebFileSize=11391203;

	} else if( nCatId == CATALOG_ID_HIPPARCOS )
	{
		strCatCode = wxT("I/196/main");
		strCatFields = wxT("HIC,RAJ2000,DEJ2000,Vmag");
		nWebFileSize=4847612;

	} else if( nCatId == CATALOG_ID_TYCHO_2 )
	{
		strCatCode = wxT("I/259/tyc2,I/259/suppl_1,I/259/suppl_2");
		strCatFields = wxT("TYC1,TYC2,TYC3,RA(ICRS),DE(ICRS),VTmag");
		nWebFileSize=119377128;

	} else if( nCatId == CATALOG_ID_NOMAD )
	{
		strCatCode = wxT("I/297/out");
		strCatFields = wxT("NOMAD1,Tycho-2,RAJ2000,DEJ2000,Vmag,Bmag,Rmag,Jmag,Hmag,Kmag");
		nWebFileSize=0;

	} else if( nCatId == CATALOG_ID_2MASS )
	{
		strCatCode = wxT("II/246/out");
		strCatFields = wxT("2MASS,RAJ2000,DEJ2000,Rmag,Bmag,Jmag,Hmag,Kmag");
		nWebFileSize=0;

	} else if( nCatId == CATALOG_ID_GSC )
	{
		strCatCode = wxT("I/305/out");
		strCatFields = wxT("GSC2.3,RAJ2000,DEJ2000,Fmag,jmag,Vmag,Nmag,Umag,Bmag,Class");
		nWebFileSize=0;

	/////////////////////////////////////////////////////
	// :: DSO
	} else if( nCatId == CATALOG_ID_UGC )
	{
		strCatCode = wxT("VII/26D/catalog");
		strCatFields = wxT("UGC,MCG,RA1950,DE1950,MajAxis,MinAxis,PA,Pmag,i,Hubble");
		nWebFileSize=827301;

	} else if( nCatId == CATALOG_ID_MCG )
	{
		strCatCode = wxT("VII/62A/mcg");
		// other name is ngc or ic
		strCatFields = wxT("MCG,OtherName,RA1950,DE1950,MajAxis,MinAxis,Mag");
		nWebFileSize=0;

	} else if( nCatId == CATALOG_ID_MGC )
	{
		strCatCode = wxT("VII/240/mgczcat");
		strCatFields = wxT("MGC,RAJ2000,DEJ2000,a,b,PA,Bmag");
		nWebFileSize=3898669;

	} else if( nCatId == CATALOG_ID_HYPERLEDA )
	{
		strCatCode = wxT("VII/237/pgc");
		strCatFields = wxT("PGC,RAJ2000,DEJ2000,logD25,logR25,PA,MType");
		nWebFileSize=47397402;

	/////////////////////////////////////////////////////
	// :: Gamma/X-Ray
	} else if( nCatId == CATALOG_ID_ROSAT )
	{
		strCatCode = wxT("IX/29/rass_fsc");
		// fields
		strCatFields = wxT("recno,1RXS,RAJ2000,DEJ2000,Count,bgCt,ExpTime,Extent,ObsBeg,ObsEnd");
		nWebFileSize=12077388;

	} else if( nCatId == CATALOG_ID_BATSE )
	{
		strCatCode = wxT(". J/ApJ/563/80/table2 ");
		// fields
		strCatFields = wxT("recno,[STK2001],Start,Trigger,Flux,RAJ2000,DEJ2000,T90,N50,K98,Burst");
		nWebFileSize=306535;

	} else if( nCatId == CATALOG_ID_INTEGRAL )
	{
		strCatCode = wxT("J/A+A/411/L59/catalog");
		// -10 keV energy flux (JEMX)
		strCatFields = wxT("Seq,Name,RAJ2000,DEJ2000,F3-10keV,F10-30keV,F20-60keV,F60-200keV,CR3-10keV,CR10-30keV,CR20-60keV,CR60-200keV,Type");
		nWebFileSize=142608;

	} else if( nCatId == CATALOG_ID_BEPPOSAX_GRBM )
	{
		strCatCode = wxT("J/ApJS/180/192/table2");
		// -10 keV energy flux (JEMX)
		strCatFields = wxT("recno,Name,Time,RAJ2000,DEJ2000,Tdet,Ta,Na,T90,fluence,Fpeak,Gamma");
		nWebFileSize=96163;

	} else if( nCatId == CATALOG_ID_2XMMi )
	{
		strCatCode = wxT("IX/40/xmm2is");
		// -10 keV energy flux (JEMX)
		strCatFields = wxT("Source,2XMMi,RAJ2000,DEJ2000,Flux1,Flux2,Flux3,Flux4,Flux5,Flux,Flux14,Nd");
		nWebFileSize=30281048;

	} else if( nCatId == CATALOG_ID_HEAO_A1 )
	{
		strCatCode = wxT("VII/80/sources");
		// -10 keV energy flux (JEMX)
		strCatFields = wxT("recno,name,RA1950,DE1950,flux");
		nWebFileSize=37394;

	/////////////////////////////////////////////////////
	// :: Radio
	} else if( nCatId == CATALOG_ID_SPECFIND )
	{
		strCatCode = wxT("VIII/85/spectra");
		// use name or SPECFIND + seq
		strCatFields = wxT("Seq,Name,RAJ2000,DEJ2000,N,a,b,nu,S(nu)");
		nWebFileSize=45253472;

	} else if( nCatId == CATALOG_ID_VLSS )
	{
		strCatCode = wxT("VIII/79A/catalog");
		// 
		strCatFields = wxT("recno,VLSS,RAJ2000,DEJ2000,Si,MajAx,MinAx,PA");
		nWebFileSize=4772109;

	} else if( nCatId == CATALOG_ID_NVSS )
	{
		strCatCode = wxT("VIII/65/nvss");
		// 
		strCatFields = wxT("recno,NVSS,RAJ2000,DEJ2000,S1.4,MajAxis,MinAxis,PA,resFlux,polFlux,polPA");
		nWebFileSize=163017256;
	
	} else if( nCatId == CATALOG_ID_MSL )
	{
		strCatCode = wxT("VII/2A/master");
		// 
		strCatFields = wxT("recno,Name,RA1950,DE1950,Freq,S");
		nWebFileSize=5404670;

	/////////////////////////////////////////////////////
	// :: MULTIPLE/DOUBLE/BINARY STARS
	} else if( nCatId == CATALOG_ID_WDS )
	{
		strCatCode = wxT("B/wds/wds");
		strCatFields = wxT("recno,WDS,RAJ2000,DEJ2000,Disc,Comp,Obs1,Obs2,Nobs,pa1,pa2,sep1,sep2,mag1,mag2,SpType,pmRA1,pmDE1,pmRA2,pmDE2,Notes");
		nWebFileSize=14566074;

	} else if( nCatId == CATALOG_ID_CCDM )
	{
		strCatCode = wxT("I/274/ccdm");
		strCatFields = wxT("recno,CCDM,RAJ2000,DEJ2000,rComp,Comp,Disc,Year,theta,rho,Obs,Vmag,Sp,pmNote,pmRA,pmDE,,Note1,Note2");
		nWebFileSize=10057767;

	/////////////////////////////////////////////////////
	// :: SUPERNOVAS
	} else if( nCatId == CATALOG_ID_SSC )
	{
		strCatCode = wxT("II/256/sn");
		// Gname - Galaxy identification from NGC (N), IC (I), MCG (M), UGC (U), PGC (P) and ESO (E) catalogues, or LEDA database
		// Disc - Discoverer(s), Type - SN type, datDisc - Date of discovery (within year), Rem - Method of supernova discovery  
		strCatFields = wxT("recno,SN,Gname,RA2000,DE2000,mag,Type,Gmag,Rem,datDisc,Disc,Uncert,Code");
		nWebFileSize=673578;

	} else if( nCatId == CATALOG_ID_ASC )
	{
		strCatCode = wxT("B/sn/sncat");
		// EpMax  - Date of maximum or Discovery (see n_EpMax)
		strCatFields = wxT("recno,SN,n_SN,u_SN,Galaxy,RAJ2000,DEJ2000,x,y,MaxMag,Type,EpMax,Disc");
		nWebFileSize=548929;

	/////////////////////////////////////////////////////
	// :: BLACKHOLES
	} else if( nCatId == CATALOG_ID_SDSS_DR5 )
	{
		strCatCode = wxT("J/ApJ/680/169/table1");
		// 
		strCatFields = wxT("recno,SDSS,RAJ2000,DEJ2000,z,imag,L5100,L3000,L1350,BHvir,BHHb,BHMgII,BHCIV,ObsDate");
		nWebFileSize=10532904;

	} else if( nCatId == CATALOG_ID_RXE_AGN )
	{
		strCatCode = wxT("J/ApJ/645/890/table1");
		// 
		strCatFields = wxT("recno,SDSS,_RA,_DE,z,logM,logLX,logLr,logR");
		nWebFileSize=11435;

	/////////////////////////////////////////////////////
	// :: ASTEROIDS
	} else if( nCatId == CATALOG_ID_OMP )
	{
		strCatCode = wxT("B/astorb/astorb");
		// Planet - Asteroid number (blank if unnumbered), H - Absolute magnitude H parameter 
		// Diam  - IRAS diameter, Nobs - Number of observations used in orbit computation
		// i - Inclination, e - Eccentricity, a - Semimajor axis
		strCatFields = wxT("recno,Planet,Name,CompName,H,G,B-V,Diam,IRAScl,Narc,Nobs,Epoch,M,omega,Omega,i,e,a,DateOrb,mod,modDate");
		nWebFileSize=87667058;

	/////////////////////////////////////////////////////
	// :: COMETS
	} else if( nCatId == CATALOG_ID_COCOM )
	{
		strCatCode = wxT("B/comets/comets");
		// Database of the orbital elements of comets (Rocher, 2007) ReadMe 
		strCatFields = wxT("recno,Udate,Code,Name,OrbName,Elem,n_Elem,X0,Y0,Z0,Xdot,Ydot,Zdot,A1,A2,A3,T0,q,e,omega,Omega,i,H1,R1,D1,H2,R2,D2,Cnote");
		nWebFileSize=311893;

	}

	// now here we shoul d check first if we already have that info localy
	// for now just look for this specific file - in the future compute aprox location
	if( !wxFileExists( strFile ) )
	{
		// for all available mirros
		for( i=0; i<m_nSite; i++ )
		{
			if( m_pUnimapWorker && m_pUnimapWorker->m_bCurrentTaskSetToBreak )
			{
				nState = 0;
				break;
			}

			// build name for the url to request
			// http://webviz.u-strasbg.fr/viz-bin/asu-txt?-source=I/284/out&-c=10.0%2010.0&-oc.form=dec&-c.r=1.1&-c.u=deg&-c.geom=r&-out.form=text/plain&-out.max=unlimited&-out=USNO-B1.0,Tycho-2,RAJ2000,DEJ2000,B1mag,R1mag,B2mag,R2mag
			if( nCatId == CATALOG_ID_OMP || nCatId == CATALOG_ID_COCOM )
			{
				strUrl.Printf( wxT("%s/viz-bin/asu-txt?-source=%s&-oc.form=dec&-c.u=deg&-c.geom=r&-out.form=text/plain&-out.max=unlimited&-out=%s"),
								m_vectSite[i], strCatCode, strCatFields );
			} else
			{
				strUrl.Printf( wxT("%s/viz-bin/asu-txt?-source=%s&-c=%.10f %.10f&-oc.form=dec&-c.r=%.10f&-c.u=deg&-c.geom=r&-out.form=text/plain&-out.max=unlimited&-out=%s"),
								m_vectSite[i], strCatCode, nCenterRA, nCenterDEC, nRadius, strCatFields );
			}

			// call online - to do call multiple site addresse - vizier has a few mirrors
			nState = LoadWebFileT( strUrl, strFile, m_pUnimapWorker, nWebFileSize );
			// if success - exit loop
			if( nState > 0 ) break;

			// log message
			if( m_pUnimapWorker && i<m_nSite-1 )
			{
				strLog.Printf( wxT("INFO :: failed to load from  Vizier site %s try next mirror %s"), m_vectSite[i], m_vectSite[i-1] );
				m_pUnimapWorker->Log( strLog );
			}
		}

	} else
		nState = 1;

	return( nState );
}
