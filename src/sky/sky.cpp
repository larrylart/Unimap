////////////////////////////////////////////////////////////////////
// Package:		Sky implementation
// File:		sky.cpp
// Purpose:		manage sky objects/catalogs/structures/processes etc
//
// Created by:	Larry Lart on 11-11-2004
// Updated by:	Larry Lart on 24-05-2007
//				Larry Lart on 26-06-2008
//				Larry Lart on 22-09-2008 (clean up the search/matching)
//				Larry Lart on 08-03-2009 (change to sky/split finder etc)
//
// Copyright:	(c) 2004-2009 Larry Lart
// Licence:		Digital Entity
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

// opencv includes
//#include "highgui.h"
//#include "cv.h"
//#include "cvaux.h"

#include "wx/wxprec.h"
#include "wx/thread.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/regex.h>
#include <wx/string.h>
#include <wx/filesys.h>
#include <wx/html/htmlpars.h>
#include <wx/fs_inet.h>
#include <wx/tokenzr.h>

// custom headers
#ifdef _WIN32
#include "../util/func.h"
#endif

#include "../../lib/wcstools-3.7.7/libwcs/wcs.h"
// local includes
#include "star.h"
#include "constellations.h"
#include "sky_finder.h"
// catalogs
#include "catalog_dso.h"
#include "catalog_stars.h"
#include "catalog_stars_assoc.h"
#include "catalog_stars_names.h"
#include "catalog_dso_ngc.h"
#include "catalog_dso_ic.h"
#include "catalog_dso_messier.h"
#include "catalog_dso_assoc.h"
#include "catalog_dso_names.h"
// other cats
#include "catalog_radio.h"
#include "catalog_xgamma.h"
#include "catalog_supernovas.h"
#include "catalog_blackholes.h"
#include "catalog_mstars.h"
#include "catalog_exoplanets.h"
#include "catalog_asteroids.h"
#include "catalog_comets.h"
#include "catalog_aes.h"

// others
//#include "../logger/logger.h"
#include "../util/folders.h"
#include "../match/starmatch.h"
#include "../image/astroimage.h"
#include "../config/mainconfig.h"
#include "../unimap_worker.h"
#include "../unimap.h"
#include "../gui/frame.h"
#include "../gui/waitdialogmatch.h"

// main header
#include "sky.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSky
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSky::CSky( )
{
	m_pUnimapWorker = NULL;

	m_nDebugLevel = 0;

// not used ????
//	m_nCatObjMaxMatchNo = DEFAULT_CAT_MAX_OBJ_MATCH_NO;

//	m_nMagLimitIdx = 0;

// in main config
//	m_nDsoCatalogToUse = CATALOG_ID_NONE;

	// set default flags
//	m_bUseConstellation = 1;

	///////////////////////
	// NEW PLUG :: create my objects in slots

	// :: FIRST LOAD CAT CONFIG
	InitCatalogs( );

	////////////////
	// STARS catalog
	m_pCatalogStars = new CSkyCatalogStars( this );
	// stars assoc
	m_pCatalogStarsAssoc = new CSkyCatalogStarsAssoc( );
	// names
	m_pCatalogStarNames = new CSkyCatalogStarNames( this );

	//////////////////
	// main dso
	m_pCatalogDso = new CSkyCatalogDso( this );
	// ngc dso
	m_pCatalogDsoNgc = new CSkyCatalogDsoNgc( this );
	// ic dso
	m_pCatalogDsoIc = new CSkyCatalogDsoIc( this );
	// messier
	m_pCatalogDsoMessier = new CSkyCatalogDsoMessier( this );
	// dso assoc
	m_pCatalogDsoAssoc = new CSkyCatalogDsoAssoc();
	// dso names
	m_pCatalogDsoNames = new CSkyCatalogDsoNames( this );

	//////////////
	// other cats
	m_pCatalogRadio = new CSkyCatalogRadio( this );
	m_pCatalogXGamma = new CSkyCatalogXGamma( this );
	m_pCatalogSupernovas = new CSkyCatalogSupernovas( this );
	m_pCatalogBlackholes = new CSkyCatalogBlackholes( this );
	m_pCatalogMStars = new CSkyCatalogMStars( this );
	m_pCatalogExoplanets = new CSkyCatalogExoplanets( this );
	m_pCatalogAsteroids = new CSkyCatalogAsteroids( this );
	m_pCatalogComets = new CSkyCatalogComets( this );
	m_pCatalogAes = new CSkyCatalogAes( this );

	//////
	// sky finder
	m_pSkyFinder = new CSkyFinder( this, m_pCatalogStars );

	//////
	// constellations
	m_pConstellations = new CSkyConstellations( this );

	// schedule for download section - use worker paramas - to clean
//	m_nCatalogIdForDownloading = -1;
//	m_nCatalogTypeForDownloading = -1;
//	m_nCatalogSourceForDownloading = CATALOG_DOWNLOAD_SOURCE_VIZIER;

}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSky
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSky::~CSky()
{
	// :: stars
	delete( m_pCatalogStars );
	delete( m_pCatalogStarsAssoc );
	delete( m_pCatalogStarNames );
	// :: dso
	delete( m_pCatalogDso );
	delete( m_pCatalogDsoNgc );
	delete( m_pCatalogDsoIc );
	delete( m_pCatalogDsoMessier );
	delete( m_pCatalogDsoAssoc );
	delete( m_pCatalogDsoNames );
	// :: other cats
	delete( m_pCatalogRadio );
	delete( m_pCatalogXGamma );
	delete( m_pCatalogSupernovas );
	delete( m_pCatalogBlackholes );
	delete( m_pCatalogMStars );
	delete( m_pCatalogExoplanets );
	delete( m_pCatalogAsteroids );
	delete( m_pCatalogComets );
	delete( m_pCatalogAes );
	// :: sky finder
	delete( m_pSkyFinder );
	// :: constellations
	delete( m_pConstellations );
}

/////////////////////////////////////////////////////////////////////
bool CSky::HasGui()
{
	if( m_pSkyFinder && m_pSkyFinder->m_pWaitDialogMatch != NULL )
		return(true);
	else
		return(false);
}

/////////////////////////////////////////////////////////////////////
// Method:	SetWorker
// Class:	CSky
// Purpose:	set worker for me and childs
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSky::SetWorker(  CUnimapWorker* pWorker )
{
	m_pUnimapWorker = pWorker;
	m_pSkyFinder->m_pUnimapWorker = pWorker;
	m_pCatalogStars->m_pUnimapWorker = pWorker;
	m_pCatalogDso->m_pUnimapWorker = pWorker;
	m_pCatalogDsoNgc->m_pUnimapWorker = pWorker;
	m_pCatalogDsoIc->m_pUnimapWorker = pWorker;
	m_pCatalogDsoMessier->m_pUnimapWorker = pWorker;
	m_pConstellations->m_pUnimapWorker = pWorker;
	m_pCatalogRadio->m_pUnimapWorker = pWorker;
	m_pCatalogXGamma->m_pUnimapWorker = pWorker;
	m_pCatalogSupernovas->m_pUnimapWorker = pWorker;
	m_pCatalogBlackholes->m_pUnimapWorker = pWorker;
	m_pCatalogMStars->m_pUnimapWorker = pWorker;
	m_pCatalogExoplanets->m_pUnimapWorker = pWorker;
	m_pCatalogAsteroids->m_pUnimapWorker = pWorker;
	m_pCatalogComets->m_pUnimapWorker = pWorker;
	m_pCatalogAes->m_pUnimapWorker = pWorker;
}

/////////////////////////////////////////////////////////////////////
void CSky::SetMainFrame( CGUIFrame* pFrame )
{
	m_pFrame = pFrame;
	m_pSkyFinder->m_pFrame = pFrame;
	// also set frame to this
	m_pFrame->SetSky( this );
}

/////////////////////////////////////////////////////////////////////
void CSky::SetUnimap( CUnimap* pUnimap )
{
	m_pUnimap = pUnimap;
	m_pSkyFinder->m_pUnimap = pUnimap;
}

/////////////////////////////////////////////////////////////////////
// Method:	SetConfig
// Class:	CSky
// Purpose:	set the object with current config values
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSky::SetConfig(  CConfigMain* pConfig )
{
	m_pConfig = pConfig;
	// set skyfinder config
	m_pSkyFinder->SetConfig( pConfig );
	m_pCatalogStarNames->m_pConfig = pConfig;
	m_pCatalogDsoNames->m_pConfig = pConfig;

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	InitCatalogs
// Class:	CSky
// Purpose:	init my catalogs list
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSky::InitCatalogs( )
{
	FILE* pFile = NULL;
	int bNewCatalog=0, bStartDescSection=0;
	wxChar strLine[2100];
	wxChar strTmp[2100];
//	wxString strWxTmp = wxT("");
	int nSize=0;
	long nVarLong=0;
	unsigned long nVarULong=0;

	// regex defs
	wxRegEx reId = wxT( "^Id=([0-9]+)" );
	wxRegEx reType = wxT( "^Type=(.+)" );
	wxRegEx reVersion = wxT( "^Version=(.+)" );
	wxRegEx reCustomUse = wxT( "^CustomUse=([0-9]+)" );
	wxRegEx reFile = wxT( "^File=(.+)" );
//	wxRegEx reNameFull = wxT( "^NameFull=(.+)" );
	wxRegEx reNameShort = wxT( "^NameShort=(.+)" );
	wxRegEx reAuthor = wxT( "^Author=(.+)" );
	wxRegEx reOrg = wxT( "^Org=(.+)" );
	wxRegEx reCreationDate = wxT( "^CreationDate=(.+)$" );
	wxRegEx reUpdateDate = wxT( "^UpdateDate=(.+)$" );
	wxRegEx reRows = wxT( "^Rows=(.+)" );
	wxRegEx reBibcode = wxT( "^Bibcode=(.+)" );
	wxRegEx reSourcesFull = wxT( "^SourcesFull=(.+)" );
	wxRegEx reSourcesQuery = wxT( "^SourcesQuery=(.+)" );
	wxRegEx reFootprintImg = wxT( "^FootprintImg=(.+)" );
	wxRegEx reSourceFtp = wxT( "^SourceFtp=(.+)" );
	wxRegEx reSourceHttp = wxT( "^SourceHttp=(.+)" );
	// description section
	wxRegEx reDescStart = wxT( "^%Desc_START" );
	wxRegEx reDescEnd = wxT( "^%Desc_END" );

	// open local file to read from
	pFile = wxFopen( unMakeAppPath(wxT("catalogs.ini")), wxT("r") );
	if( !pFile )
	{
		return( 0 );
	}

	m_nCatalogDef = 0;
	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// clear buffers
		memset( &strLine, 0, 2000 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pFile );
		nSize = wxStrlen( strLine );
		if( !bStartDescSection && nSize < 3 ) continue;
		// check for comments
		if( !bStartDescSection && strLine[0] == '#' ) continue;

//		wxString strWxLine = strLine;

		///////////
		// here we check for labels
		if( strLine[0] == '[' && wxSscanf( strLine, wxT("\[%[^\]]\]"), strTmp ) )
		{
			m_vectCatalogDef[m_nCatalogDef].m_strNameAbbreviation = strTmp;
			m_vectCatalogDef[m_nCatalogDef].m_nCustomUse = 0;
			m_vectCatalogDef[m_nCatalogDef].m_nSourcesFull = 0;
			m_vectCatalogDef[m_nCatalogDef].m_nSourcesQuery = 0;
			m_vectCatalogDef[m_nCatalogDef].m_strDescription = wxT("");
			m_vectCatalogDef[m_nCatalogDef].m_nRecordsNo = 0;
			m_vectCatalogDef[m_nCatalogDef].m_nObjectType = CATALOG_TYPE_OTHER;
			m_nCatalogDef++;

			bStartDescSection = 0;
			bNewCatalog = 1;

			continue;

		////////////////////////////////
		// :: DESC COLLECT SECTION
		} else if( bStartDescSection )
		{
			// check for the end
			if( bNewCatalog && reDescEnd.Matches( strLine ) )
			{
				bStartDescSection=0;
				continue;
			}
			// add to string description
			wxString strNewLine = strLine;
			strNewLine.Trim(0).Trim(1);
			if( strNewLine.IsEmpty() )
				m_vectCatalogDef[m_nCatalogDef-1].m_strDescription += wxT("\n\n");
			else
			{
				strNewLine += wxT(" ");
				m_vectCatalogDef[m_nCatalogDef-1].m_strDescription += strNewLine;
			}

		//////////////////////////
		// :: OTHER PROPS
		} else if( bNewCatalog && reId.Matches( strLine ) )
		{
			if( !reId.GetMatch(strLine, 1 ).Trim(0).Trim(1).ToLong( &nVarLong ) )
				m_vectCatalogDef[m_nCatalogDef-1].m_nId = 0;
			else
				m_vectCatalogDef[m_nCatalogDef-1].m_nId = (int) nVarLong;

		} else if( bNewCatalog && reCustomUse.Matches( strLine ) )
		{
			if( !reCustomUse.GetMatch(strLine, 1 ).Trim(0).Trim(1).ToLong( &nVarLong ) )
				m_vectCatalogDef[m_nCatalogDef-1].m_nCustomUse = 0;
			else
				m_vectCatalogDef[m_nCatalogDef-1].m_nCustomUse = (int) nVarLong;

		} else if( bNewCatalog && reType.Matches( strLine ) )
		{
			wxString strType = reType.GetMatch(strLine, 1 ).Trim(0).Trim(1);

			if( strType == wxT("STARS") )
				m_vectCatalogDef[m_nCatalogDef-1].m_nObjectType = CATALOG_TYPE_STARS;
			else if( strType == wxT("DSO") )
				m_vectCatalogDef[m_nCatalogDef-1].m_nObjectType = CATALOG_TYPE_DSO;
			else if( strType == wxT("RADIO") )
				m_vectCatalogDef[m_nCatalogDef-1].m_nObjectType = CATALOG_TYPE_RADIO;
			else if( strType == wxT("XGAMMA") )
				m_vectCatalogDef[m_nCatalogDef-1].m_nObjectType = CATALOG_TYPE_XGAMMA;
			else if( strType == wxT("SN") )
				m_vectCatalogDef[m_nCatalogDef-1].m_nObjectType = CATALOG_TYPE_SUPERNOVAS;
			else if( strType == wxT("BH") )
				m_vectCatalogDef[m_nCatalogDef-1].m_nObjectType = CATALOG_TYPE_BLACKHOLES;
			else if( strType == wxT("MS") )
				m_vectCatalogDef[m_nCatalogDef-1].m_nObjectType = CATALOG_TYPE_MSTARS;
			else if( strType == wxT("EXP") )
				m_vectCatalogDef[m_nCatalogDef-1].m_nObjectType = CATALOG_TYPE_EXOPLANETS;
			else if( strType == wxT("AST") )
				m_vectCatalogDef[m_nCatalogDef-1].m_nObjectType = CATALOG_TYPE_ASTEROIDS;
			else if( strType == wxT("COM") )
				m_vectCatalogDef[m_nCatalogDef-1].m_nObjectType = CATALOG_TYPE_COMETS;
			else if( strType == wxT("AES") )
				m_vectCatalogDef[m_nCatalogDef-1].m_nObjectType = CATALOG_TYPE_AES;
			else
				m_vectCatalogDef[m_nCatalogDef-1].m_nObjectType = CATALOG_TYPE_OTHER;

		} else if( bNewCatalog && reVersion.Matches( strLine ) )
		{
			m_vectCatalogDef[m_nCatalogDef-1].m_strVersion = reVersion.GetMatch(strLine, 1 ).Trim(0).Trim(1);

		} else if( bNewCatalog && reFile.Matches( strLine ) )
		{
			m_vectCatalogDef[m_nCatalogDef-1].m_strFile = reFile.GetMatch(strLine, 1 ).Trim(0).Trim(1);

		} else if( bNewCatalog && reRows.Matches( strLine ) )
		{
			if( !reRows.GetMatch(strLine, 1 ).Trim(0).Trim(1).ToULong( &nVarULong ) )
				m_vectCatalogDef[m_nCatalogDef-1].m_nRecordsNo = 0;
			else
				m_vectCatalogDef[m_nCatalogDef-1].m_nRecordsNo = (unsigned long) nVarULong;

		} else if( bNewCatalog && wxStrncmp( strLine, wxT("NameFull="), 9 ) == 0 )
		{
			if( wxSscanf( strLine, wxT("NameFull=%[^\n]\n"), strTmp ) )
				m_vectCatalogDef[m_nCatalogDef-1].m_strFullName = strTmp;

		} else if( bNewCatalog && reNameShort.Matches( strLine ) )
		{
			m_vectCatalogDef[m_nCatalogDef-1].m_strName = reNameShort.GetMatch(strLine, 1 ).Trim(0).Trim(1);

		} else if( bNewCatalog && reAuthor.Matches( strLine ) )
		{
			m_vectCatalogDef[m_nCatalogDef-1].m_strAuthor = reAuthor.GetMatch(strLine, 1 ).Trim(0).Trim(1);

		} else if( bNewCatalog && reOrg.Matches( strLine ) )
		{
			m_vectCatalogDef[m_nCatalogDef-1].m_strOrganization = reOrg.GetMatch(strLine, 1 ).Trim(0).Trim(1);

		} else if( bNewCatalog && reCreationDate.Matches( strLine ) )
		{
			m_vectCatalogDef[m_nCatalogDef-1].m_strCreationDate = reCreationDate.GetMatch(strLine, 1 ).Trim(0).Trim(1);

		} else if( bNewCatalog && reUpdateDate.Matches( strLine ) )
		{
			m_vectCatalogDef[m_nCatalogDef-1].m_strUpdateDate = reUpdateDate.GetMatch(strLine, 1 ).Trim(0).Trim(1);

		} else if( bNewCatalog && reBibcode.Matches( strLine ) )
		{
			m_vectCatalogDef[m_nCatalogDef-1].m_strBibcode = reBibcode.GetMatch(strLine, 1 ).Trim(0).Trim(1);

		} else if( bNewCatalog && reFootprintImg.Matches( strLine ) )
		{
			m_vectCatalogDef[m_nCatalogDef-1].m_strFootPrintImg = unMakeAppPath(reFootprintImg.GetMatch(strLine, 1 ).Trim(0).Trim(1));

		} else if( bNewCatalog && reSourcesFull.Matches( strLine ) )
		{
			wxString strSources = reSourcesFull.GetMatch(strLine, 1 ).Trim(0).Trim(1);

			m_vectCatalogDef[m_nCatalogDef-1].m_nSourcesFull = 0;
			int nIndex = 0;
			wxStringTokenizer tkz( strSources, wxT(",") );
			while ( tkz.HasMoreTokens() )
			{
				wxString strSource = tkz.GetNextToken();
				// check if emty - aka end of line
				if( strSource.Length() != 0 )
				{
					// check by known type
					if( strSource == wxT("VIZIER") )
					{
						m_vectCatalogDef[m_nCatalogDef-1].m_vectSourcesFull[nIndex] = CATALOG_DOWNLOAD_SOURCE_VIZIER;
						nIndex++;

					} else if( strSource == wxT("UNIMAP") )
					{
						m_vectCatalogDef[m_nCatalogDef-1].m_vectSourcesFull[nIndex] = CATALOG_DOWNLOAD_SOURCE_UNIMAP;
						nIndex++;

					} else if( strSource == wxT("LEDA") )
					{
						m_vectCatalogDef[m_nCatalogDef-1].m_vectSourcesFull[nIndex] = CATALOG_DOWNLOAD_SOURCE_LEDA;
						nIndex++;

					} else if( strSource == wxT("EXPEU") )
					{
						m_vectCatalogDef[m_nCatalogDef-1].m_vectSourcesFull[nIndex] = CATALOG_DOWNLOAD_SOURCE_EXOPLANET_EU;
						nIndex++;

					} else if( strSource == wxT("CELESTRAK") )
					{
						m_vectCatalogDef[m_nCatalogDef-1].m_vectSourcesFull[nIndex] = CATALOG_DOWNLOAD_SOURCE_CELESTRAK;
						nIndex++;
					}
				}
			}
			// now set my index
			m_vectCatalogDef[m_nCatalogDef-1].m_nSourcesFull = nIndex;

		} else if( bNewCatalog && reSourcesQuery.Matches( strLine ) )
		{
			wxString strSources = reSourcesQuery.GetMatch(strLine, 1 ).Trim(0).Trim(1);

			m_vectCatalogDef[m_nCatalogDef-1].m_nSourcesQuery = 0;
			int nIndex = 0;
			wxStringTokenizer tkz( strSources, wxT(",") );
			while ( tkz.HasMoreTokens() )
			{
				wxString strSource = tkz.GetNextToken();
				// check if emty - aka end of line
				if( strSource.Length() != 0 )
				{
					// check by known type
					if( strSource == wxT("VIZIER") )
					{
						m_vectCatalogDef[m_nCatalogDef-1].m_vectSourcesQuery[nIndex] = CATALOG_QUERY_SOURCE_VIZIER;
						nIndex++;

					} else if( strSource == wxT("NOFS_NAVY_MIL") )
					{
						m_vectCatalogDef[m_nCatalogDef-1].m_vectSourcesQuery[nIndex] = CATALOG_QUERY_SOURCE_NOFS_NAVY_MIL;
						nIndex++;

					} else if( strSource == wxT("LEDA") )
					{
						m_vectCatalogDef[m_nCatalogDef-1].m_vectSourcesQuery[nIndex] = CATALOG_QUERY_SOURCE_LEDA;
						nIndex++;

					} else if( strSource == wxT("EXPEU") )
					{
						m_vectCatalogDef[m_nCatalogDef-1].m_vectSourcesQuery[nIndex] = CATALOG_QUERY_SOURCE_EXOPLANET_EU;
						nIndex++;

					} else if( strSource == wxT("CELESTRAK") )
					{
						m_vectCatalogDef[m_nCatalogDef-1].m_vectSourcesQuery[nIndex] = CATALOG_QUERY_SOURCE_CELESTRAK;
						nIndex++;
					}
				}
			}
			// now set my index
			m_vectCatalogDef[m_nCatalogDef-1].m_nSourcesQuery = nIndex;

		} else if( bNewCatalog && reDescStart.Matches( strLine ) )
		{
			bStartDescSection = 1;
		}

	}
	// close my file
	fclose( pFile );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Purpose:	get catalog id be abbreviation or -1 if not found
/////////////////////////////////////////////////////////////////////
int CSky::GetCatalogIdByAbbreviation( const wxString& strCatAbbreviation )
{
	for( int i=0; i<m_nCatalogDef; i++ )
		if( strCatAbbreviation.CmpNoCase( m_vectCatalogDef[i].m_strNameAbbreviation ) == 0 ) 
			return( m_vectCatalogDef[i].m_nId );

	return( -1 );
}

/////////////////////////////////////////////////////////////////////
// Purpose:	get catalog index of vector by id
/////////////////////////////////////////////////////////////////////
int CSky::GetCatalogIndexById( int nCatId )
{
	for( int i=0; i<m_nCatalogDef; i++ )
		if( m_vectCatalogDef[i].m_nId == nCatId ) return( i );

	return( -1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	TransObj_SkyToImage
// Class:	CSky
// Purpose:	set the object with current config values
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSky::TransObj_SkyToImage( StarDef& star )
{
	m_pSkyFinder->m_pStarMatch->ApplyObjInv( star );
}

////////////////////////////////////////////////////////////////////
// Method:		CalcPointTrans
// Class:		CSky
// Purpose:		calculate x, y best trans
// Input:		reference to x, y double coord
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CSky::CalcPointTrans( double& x, double& y )
{
	return( m_pSkyFinder->m_pStarMatch->CalcPointTrans( x, y ) );
}

////////////////////////////////////////////////////////////////////
// Method:	StoRect
// Class:	CSky
// Purpose:	Spherical to Rect conversion, results in unit vector.
//			in puts in RA, Dec, outputs RECTVEX.x.y.z
//			RECTVEC conv is z = Dec=90, x =(RA=0Dec=0),...y = (RA=90Dec=0)
//			Uses shpere to rect eqs, corrects RA to phi convention,
//			so should probably be named CtoRect since it converts celest
//			coords to rect coord.
////////////////////////////////////////////////////////////////////
void CSky::StoRect( float& nRA, float& nDec )
{
//	RECTVEC R;
	double theta,phi;
	double DtoR = ( 2 * 3.14159265) / 360;
	double RtoD = 1 / DtoR;
	theta = DtoR * nRA;
	phi = DtoR * (90 - fabs(nDec));

//	R.x = cos(theta) * sin(phi);
//	R.y = sin(theta) * sin(phi);
//	R.z = cos(phi);

	nRA = cos(theta) * sin(phi);
	nDec = sin(theta) * sin(phi);

//	if (Dec < 0)
//		R.z = -R.z;

//	return(R);
}

////////////////////////////////////////////////////////////
//AltAztoRect()
//AltAz to Rect conversion
//inputs Alt Az, outputs RECTXYZ.
//RECTVEC convention is;x = north, y= west, z=zenith
/////////////////////////////////////////////////////////////
void CSky::AltAztoRect( float& nAlt, float& nAz )
{

//	RECTVEC R;
	double theta,phi;
	double DtoR = ( 2 * 3.14159265) / 360;
	double RtoD = 1 / DtoR;
	theta = DtoR *(360.0 -  nAz);
	phi = DtoR * (90.0 - fabs(nAlt));

//	R.x = cos(theta) * sin(phi);
//	R.y = sin(theta) * sin(phi);
//	R.z = cos(phi);

	nAlt = cos(theta) * sin(phi);
	nAz = sin(theta) * sin(phi);

//	if (Alt < 0)
//		R.z = -R.z;

//	return(R);
}

/////////////////////////////////////////////////////////////////////
// Method:	ProjectStarAIT
// Class:	CSky
// Purpose:	AIT: Hammer-Aitoff projection.
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSky::ProjectStarAIT( StarDef& star, const double RaOrig,
									const double DecOrig )
{
	double cthe, w;
	double pw0, r0=0.0;

	if( r0 == 0.0 ) r0 = PI_FACT;
	pw0 = 2.0*r0*r0;

	double RaRad = (star.ra-180.0) * PI_FACT;
	double DecRad = star.dec * PI_FACT;

	cthe = cos( DecRad );
	w = sqrt(pw0/(1.0 + cthe*cos( RaRad/2.0 )));
	star.x = 2.0*w*cthe*sin( RaRad/2.0 );
	star.y = w*sin( DecRad );

	return;
}
/////////////////////////////////////////////////////////////////////
// Method:	ProjectStarCAR
// Class:	CSky
// Purpose:	CAR: Cartesian projection.
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSky::ProjectStarCAR( StarDef& star, const double RaOrig,
									const double DecOrig )
{
	double r0=0.0, w0=1.0;

	if( r0 == 0.0 )
	{
		r0 = PI_FACT;
		w0 = 1.0;
	} else 
	{
		w0 = r0*PI_FACT;
	}


	star.x = w0 * star.ra;
	star.y = w0 * star.dec;

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	ProjectStarARC
// Class:	CSky
// Purpose:	ARC: zenithal/azimuthal equidistant projection.
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSky::ProjectStarARC( StarDef& star, const double RaOrig,
									const double DecOrig )
{
	double r;
	double r0=0.0,w0=1.0;

	if( r0 == 0.0 )
	{
		r0 = PI_FACT;
		w0 = 1.0;
	} else
	{
		w0 = r0*PI_FACT;
	}

	double RaRad = (star.ra-180.0) * PI_FACT;

	r =  w0 * (90.0 - star.dec);
	star.x =  r*sin( RaRad );
	star.y = -r*cos( RaRad );

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	ProjectStarTan
// Class:	CSky
// Purpose:	project gnomic tangent star
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSky::ProjectStarTAN2( StarDef& star, const double RaOrig,
									const double DecOrig )
{
    int	ira, idec, niter;
    double ra, dec, cosdec, sindec, cosra, sinra, x, y, phi, theta;
    double s, r, dphi, z, dpi, dhalfpi, twopi, tx;
    double xm, ym, f, fx, fy, g, gx, gy, denom, dx, dy;
    double colatp, coslatp, sinlatp, longp, sphtol;

	ira = 0;
	idec = 1;

    //  Compute the transformation from celestial coordinates ra and
	// dec to native coordinates phi and theta. this is the spherical
	// geometry part of the transformation

    ra  = (star.ra - RaOrig) * DEG2RAD;
    dec = star.dec * DEG2RAD;
    cosra = cos (ra);
    sinra = sin (ra);
    cosdec = cos (dec);
    sindec = sin (dec);
    colatp = (90.0 - DecOrig) * DEG2RAD;
    coslatp = cos (colatp);
    sinlatp = sin (colatp);
	// what ????
	longp = 180.0 * DEG2RAD;

	dpi = PI;
    dhalfpi = dpi * 0.5;
    twopi = PI + PI;
    sphtol = SPHTOL;

    // Compute phi
    x = sindec * sinlatp - cosdec * coslatp * cosra;
    if (fabs(x) < sphtol)
		x = -cos (dec + colatp) + cosdec * coslatp * (1.0 - cosra);

    y = -cosdec * sinra;
    if (x != 0.0 || y != 0.0)
		dphi = atan2 (y, x);
    else
		dphi = ra - dpi;

    phi = longp + dphi;
    if (phi > dpi)
		phi = phi - twopi;
    else if (phi < -dpi)
		phi = phi + twopi;

    // Compute theta
    if( fmod (ra, dpi) == 0.0 )
	{
		theta = dec + cosra * colatp;
		if (theta > dhalfpi)
			theta = dpi - theta;

		if (theta < -dhalfpi)
			theta = -dpi - theta;

	} else
	{
		z = sindec * coslatp + cosdec * sinlatp * cosra;
		if( fabs (z) > 0.99 )
		{
			if (z >= 0.0)
				theta = acos (sqrt(x * x + y * y));
			else
				theta = -acos (sqrt(x * x + y * y));

		} else
			theta = asin (z);
	}

    //  Compute the transformation from native coordinates phi and theta
	// to projected coordinates x and y

    s = sin( theta );
    if( s == 0.0 )
	{
		x = BADCVAL;
		y = BADCVAL;

	} else
	{
		r = RAD2DEG * cos (theta) / s;

		x  = r * sin (phi);
		y = -r * cos (phi);
	}

	star.x = x;
	star.y = y;

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	ProjectStarTAN
// Class:	CSky
// Purpose:	TAN: gnomonic projection
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSky::ProjectStarTAN( StarDef& star, const double x0,
									const double y0 )
{
   double dtheta = (star.ra - x0) * DEG2RAD;
   double phi = star.dec * DEG2RAD, phi0;
   double x, y, z, cos_phi0, sin_phi0, cos_phi, sin_phi;
   int is_drift_scan = 0;

   // - convert - now we're all in radians
   phi0 = y0 * DEG2RAD;
   cos_phi0 = cos( phi0 );
   if( is_drift_scan )
   {
      star.x = (star.ra - x0) * cos_phi0;
      star.y -= y0;

	} else
	{
		sin_phi0 = sin( phi0 );
		cos_phi = cos( phi );
		sin_phi = sin( phi );
		x = sin( dtheta) * cos_phi;
		y = cos_phi0 * sin_phi - cos_phi *  cos( dtheta ) * sin_phi0;
		z = sqrt( 1. - x * x - y * y);

		// TAN projection
		z = 1. / z;

		star.x = x * z * RAD2DEG;
		star.y = y * z * RAD2DEG;
		star.y = -star.y;
	}

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	ProjectStarSTG
// Class:	CSky
// Purpose:	STG: stereographic projection
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSky::ProjectStarSTG( StarDef& star, const double x0,
									const double y0 )
{
   double dtheta = (star.ra - x0) * DEG2RAD;
   double phi = star.dec * DEG2RAD, phi0;
   double x, y, z, cos_phi0, sin_phi0, cos_phi, sin_phi;
   int is_drift_scan = 0;

   // - convert - now we're all in radians
   phi0 = y0 * DEG2RAD;
   cos_phi0 = cos( phi0 );
   if( is_drift_scan )
   {
      star.x = (star.ra - x0) * cos_phi0;
      star.y -= y0;

	} else
	{
		sin_phi0 = sin( phi0 );
		cos_phi = cos( phi );
		sin_phi = sin( phi );
		x = sin( dtheta) * cos_phi;
		y = cos_phi0 * sin_phi - cos_phi *  cos( dtheta ) * sin_phi0;
		z = sqrt( 1. - x * x - y * y);

		// STG projection
		z = 2. / (1. + z);

		star.x = x * z * RAD2DEG;
		star.y = y * z * RAD2DEG;
		star.y = -star.y;
	}

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	ProjectStar
// Class:	CSky
// Purpose: projection by type
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSky::ProjectStar( StarDef& star, const double x0,
										const double y0, int nMethod )
{
	
	if( nMethod == -1 )
	{
		// check by config projection by type
		if( m_pConfig->m_nSkyProjectionMethod == SKY_TO_IMAGE_PROJECTION_METHOD_GNOMONIC )
		{
			ProjectStarTAN( star, x0, y0 );

		} else if( m_pConfig->m_nSkyProjectionMethod == SKY_TO_IMAGE_PROJECTION_METHOD_STEREOGRAPHIC )
		{
			ProjectStarSTG( star, x0, y0 );
		} else
		{
			// in all other cases just do setereographic
			ProjectStarSTG( star, x0, y0 );
		} 

	} else
	{
			if( nMethod == SKY_PROJECTION_TYPE_ARC )
				ProjectStarARC( star, x0, y0 );
			else if( nMethod == SKY_PROJECTION_TYPE_CAR )
				ProjectStarCAR( star, x0, y0 );
			else if( nMethod == SKY_PROJECTION_TYPE_AIT )
				ProjectStarAIT( star, x0, y0 );
			else if( nMethod == SKY_PROJECTION_TYPE_TAN )
				ProjectStarTAN( star, x0, y0 );
			else if( nMethod == SKY_PROJECTION_TYPE_STG )
				ProjectStarSTG( star, x0, y0 );
			else
				ProjectStarSTG( star, x0, y0 );
	}

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	DeprojectStarTAN
// Class:	CSky
// Purpose:	deproject star back in ra dec using xy plate coords
//			stored in ra dec - TAN: gnomonic
// Input:	reference to star and the center of the plate in the sky
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSky::DeprojectStarTAN( StarDef& star, const double x0,
													const double y0 )
{
	star.y = -star.y;

	double x, y, z, y1, z1, d1;
	double cos_phi0, sin_phi0, phi0;
	int is_drift_scan = 0;

	// now we're all in radians
	phi0 = y0 * DEG2RAD;
	cos_phi0 = cos( phi0 );
	x = (double) star.x * DEG2RAD;
	y = (double) star.y * DEG2RAD;

	if( is_drift_scan)
	{
		star.dec = y0 + star.y;
		star.ra = x0 + star.x / cos_phi0;

	} else
	{
		sin_phi0 = sin( phi0);
		d1 = x * x + y * y;

		// TAN - de projection
		d1 = 1. / sqrt( 1. + d1 );

		x *= d1;
		y *= d1;
		z = sqrt( 1. - x * x - y * y );
		y1 = y * sin_phi0 - z * cos_phi0;
		z1 = y * cos_phi0 + z * sin_phi0;
		star.dec = asin( z1 ) * RAD2DEG;
		star.ra = x0 + atan2( x, -y1) * RAD2DEG;
	}

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	DeprojectStarSTG
// Class:	CSky
// Purpose:	deproject star back in ra dec using xy plate coords
//			stored in ra dec - STG: stereographic
// Input:	reference to star and the center of the plate in the sky
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSky::DeprojectStarSTG( StarDef& star, const double x0,
													const double y0 )
{
	star.y = -star.y;

	double x, y, z, y1, z1, d1;
	double cos_phi0, sin_phi0, phi0;
	int is_drift_scan = 0;

	// now we're all in radians
	phi0 = y0 * DEG2RAD;
	cos_phi0 = cos( phi0);
	x = (double) star.x * DEG2RAD;
	y = (double) star.y * DEG2RAD;

	if( is_drift_scan)
	{
		star.dec = y0 + star.y;
		star.ra = x0 + star.x / cos_phi0;

	} else
	{
		sin_phi0 = sin( phi0);
		d1 = x * x + y * y;

		// STG - deprojection
		d1 = 4. / ( 4. + d1 );

		x *= d1;
		y *= d1;
		z = sqrt( 1. - x * x - y * y );
		y1 = y * sin_phi0 - z * cos_phi0;
		z1 = y * cos_phi0 + z * sin_phi0;
		star.dec = asin( z1 ) * RAD2DEG;
		star.ra = x0 + atan2( x, -y1) * RAD2DEG;
	}

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	DeprojectStar
// Class:	CSky
// Purpose:	deproject star back in ra dec using xy plate coords
//			stored in ra dec by type
// Input:	reference to star and the center of the plate in the sky
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSky::DeprojectStar( StarDef& star, const double x0, const double y0, int nMethod )
{
	if( nMethod == -1 )
	{
		// check projection by type
		if( m_pConfig->m_nSkyProjectionMethod == SKY_TO_IMAGE_PROJECTION_METHOD_GNOMONIC )
		{
			DeprojectStarTAN( star, x0, y0 );

		} else if( m_pConfig->m_nSkyProjectionMethod == SKY_TO_IMAGE_PROJECTION_METHOD_STEREOGRAPHIC )
		{
			DeprojectStarSTG( star, x0, y0 );
		} else
		{
			// in all other cases just do setereographic
			DeprojectStarSTG( star, x0, y0 );
		} 

	} else
	{
		if( nMethod == SKY_PROJECTION_TYPE_TAN )
			DeprojectStarTAN( star, x0, y0 );
		else if( nMethod == SKY_PROJECTION_TYPE_STG )
			DeprojectStarSTG( star, x0, y0 );
		else
			DeprojectStarSTG( star, x0, y0 );
	}

	// adjust ra/dec
	AdjustRaDec( star.ra, star.dec );

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	DownloadCatalog
// Class:	CSky
// Purpose:	downloading star catalog 
// Input:	nothing
// Output:	no of objects in the downloaded catalog
/////////////////////////////////////////////////////////////////////
unsigned long CSky::DownloadCatalog( int nCatType, int nCatId, int nCatSrcId )
{
	if( nCatId < 0 ) return( 0 );
	// get cat index or exit if none
	int nCatIndex = GetCatalogIndexById( nCatId );
	if( nCatIndex < 0 ) return(0);

	unsigned long nLoaded = 0;

	// this is the way initially was
	//int nCatType = m_nCatalogTypeForDownloading;
	//int nCatId = m_nCatalogIdForDownloading;
	//int nCatSrcId = m_nCatalogSourceForDownloading;

	// download catalog
	if( nCatType == CATALOG_TYPE_STARS )
	{
		CSkyCatalogStars* pCat;
		pCat = new CSkyCatalogStars( this );
		pCat->m_pUnimapWorker = m_pUnimapWorker;
		pCat->SetProfile( m_vectCatalogDef[nCatIndex] );

		nLoaded = pCat->DownloadCatalog( nCatId, nCatSrcId );

		delete( pCat );

	} else if( nCatType == CATALOG_TYPE_DSO )
	{
		CSkyCatalogDso* pCat;
		pCat = new CSkyCatalogDso( this );
		pCat->m_pUnimapWorker = m_pUnimapWorker;
		pCat->SetProfile( m_vectCatalogDef[nCatIndex] );

		nLoaded = pCat->DownloadCatalog( nCatId, nCatSrcId );

		delete( pCat );

	} else if( nCatType == CATALOG_TYPE_RADIO )
	{
		CSkyCatalogRadio* pCat;
		pCat = new CSkyCatalogRadio( this );
		pCat->m_pUnimapWorker = m_pUnimapWorker;
		pCat->SetProfile( m_vectCatalogDef[nCatIndex] );

		nLoaded = pCat->DownloadCatalog( nCatId, nCatSrcId );

		delete( pCat );

	} else if( nCatType == CATALOG_TYPE_XGAMMA )
	{
		CSkyCatalogXGamma* pCat;
		pCat = new CSkyCatalogXGamma( this );
		pCat->m_pUnimapWorker = m_pUnimapWorker;
		pCat->SetProfile( m_vectCatalogDef[nCatIndex] );

		nLoaded = pCat->DownloadCatalog( nCatId, nCatSrcId );

		delete( pCat );

	} else if( nCatType == CATALOG_TYPE_SUPERNOVAS )
	{
		CSkyCatalogSupernovas* pCat;
		pCat = new CSkyCatalogSupernovas( this );
		pCat->m_pUnimapWorker = m_pUnimapWorker;
		pCat->SetProfile( m_vectCatalogDef[nCatIndex] );

		nLoaded = pCat->DownloadCatalog( nCatId, nCatSrcId );

		delete( pCat );

	} else if( nCatType == CATALOG_TYPE_BLACKHOLES )
	{
		CSkyCatalogBlackholes* pCat;
		pCat = new CSkyCatalogBlackholes( this );
		pCat->m_pUnimapWorker = m_pUnimapWorker;
		pCat->SetProfile( m_vectCatalogDef[nCatIndex] );

		nLoaded = pCat->DownloadCatalog( nCatId, nCatSrcId );

		delete( pCat );

	} else if( nCatType == CATALOG_TYPE_MSTARS )
	{
		CSkyCatalogMStars* pCat;
		pCat = new CSkyCatalogMStars( this );
		pCat->m_pUnimapWorker = m_pUnimapWorker;
		pCat->SetProfile( m_vectCatalogDef[nCatIndex] );

		nLoaded = pCat->DownloadCatalog( nCatId, nCatSrcId );

		delete( pCat );

	} else if( nCatType == CATALOG_TYPE_EXOPLANETS )
	{
		CSkyCatalogExoplanets* pCat;
		pCat = new CSkyCatalogExoplanets( this );
		pCat->m_pUnimapWorker = m_pUnimapWorker;
		pCat->SetProfile( m_vectCatalogDef[nCatIndex] );

		nLoaded = pCat->DownloadCatalog( nCatId, nCatSrcId );

		delete( pCat );

	} else if( nCatType == CATALOG_TYPE_ASTEROIDS )
	{
		CSkyCatalogAsteroids* pCat;
		pCat = new CSkyCatalogAsteroids( this );
		pCat->m_pUnimapWorker = m_pUnimapWorker;
		pCat->SetProfile( m_vectCatalogDef[nCatIndex] );

		nLoaded = pCat->DownloadCatalog( nCatId, nCatSrcId );

		delete( pCat );

	} else if( nCatType == CATALOG_TYPE_COMETS )
	{
		CSkyCatalogComets* pCat;
		pCat = new CSkyCatalogComets( this );
		pCat->m_pUnimapWorker = m_pUnimapWorker;
		pCat->SetProfile( m_vectCatalogDef[nCatIndex] );

		nLoaded = pCat->DownloadCatalog( nCatId, nCatSrcId );

		delete( pCat );

	} else if( nCatType == CATALOG_TYPE_AES )
	{
		CSkyCatalogAes* pCat;
		pCat = new CSkyCatalogAes( this );
		pCat->m_pUnimapWorker = m_pUnimapWorker;
		pCat->SetProfile( m_vectCatalogDef[nCatIndex] );

		nLoaded = pCat->DownloadCatalog( nCatId, nCatSrcId );

		delete( pCat );
	}
	
	return( nLoaded );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadMatchCatalogRegion
// Class:	CSky
// Purpose:	load the entire catalog needed for matching
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSky::LoadMatchCatalog( )
{
	// load config - m_nCatalogForMatching
	unsigned long nLoaded = m_pCatalogStars->LoadCatalog( m_pConfig->m_nCatalogForMatching );
	return( nLoaded );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadMatchCatalogRegion
// Class:	CSky
// Purpose:	load a region from catalog
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSky::LoadMatchCatalogRegion( double nRa, double nDec,
												   double nWidth, double nHeight )
{
	unsigned long nLoaded = m_pCatalogStars->LoadCatalogRegion( m_pConfig->m_nCatalogForMatching, nRa, nDec, nWidth, nHeight );
	return( nLoaded );
}

/////////////////////////////////////////////////////////////////////
// Method:	FindDsoObjectsInImage
// Class:	CSky
// Purpose:	find all DSO objects in image
// Input:	image to look at
// Output:	no of DSO objects -1 if an error has occured
/////////////////////////////////////////////////////////////////////
long CSky::FindDsoObjectsInImage( CAstroImage* pAstroImage )
{
	long nFound = -1;

	if( m_pConfig->m_bMatchLocateDso && m_pConfig->m_nDsoCatalogToUse > 0 || m_pConfig->m_bUseNgc 
		|| m_pConfig->m_bUseIc || m_pConfig->m_bUseMessier )
	{
		int nCatIndex = GetCatalogIndexById( m_pConfig->m_nDsoCatalogToUse );
		if( nCatIndex > 0 ) 
		{
			m_pCatalogDso->SetProfile( m_vectCatalogDef[nCatIndex] );
		}

		// set start search
		if( m_pUnimapWorker && HasGui() ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Searching for DSO Objects ...") );
		nFound = m_pSkyFinder->FindDso( pAstroImage );

		//unload radio catalog
		if( nCatIndex > 0 ) m_pCatalogDso->UnloadCatalog( );

	}

	return( nFound );
}

/////////////////////////////////////////////////////////////////////
// Method:	FindRadioSourcesInImage
// Class:	CSky
// Purpose:	find all radio sources in image
// Input:	image to look at
// Output:	noof radio sourcesor -1 if an error has occured
/////////////////////////////////////////////////////////////////////
long CSky::FindRadioSourcesInImage( CAstroImage* pAstroImage )
{
	long nFound = -1;

	if( m_pConfig->m_bMatchLocateRadio && m_pConfig->m_nRadioCatalogToUse > 0 )
	{
		int nCatIndex = GetCatalogIndexById( m_pConfig->m_nRadioCatalogToUse );
		if( nCatIndex >= 0 ) 
		{
			m_pCatalogRadio->SetProfile( m_vectCatalogDef[nCatIndex] );
			// set start search
			if( m_pUnimapWorker && HasGui() ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Searching for Radio Sources ...") );
			nFound = m_pSkyFinder->FindRadioSources( pAstroImage );
			//unload radio catalog
			m_pCatalogRadio->UnloadCatalog( );
		}

	}

	return( nFound );
}

/////////////////////////////////////////////////////////////////////
// Method:	FindXGammaSourcesInImage
// Class:	CSky
// Purpose:	find all x-ray/gamma sources in image
// Input:	image to look at
// Output:	noof radio sourcesor -1 if an error has occured
/////////////////////////////////////////////////////////////////////
long CSky::FindXGammaSourcesInImage( CAstroImage* pAstroImage )
{
	long nFound = -1;

	if( m_pConfig->m_bMatchLocateXGamma && m_pConfig->m_nXGammaCatalogToUse > 0 )
	{
		int nCatIndex = GetCatalogIndexById( m_pConfig->m_nXGammaCatalogToUse );
		if( nCatIndex >= 0 ) 
		{
			m_pCatalogXGamma->SetProfile( m_vectCatalogDef[nCatIndex] );
			// set start search
			if( m_pUnimapWorker && HasGui() ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Searching for X-Ray/Gamma Sources ...") );
			nFound = m_pSkyFinder->FindXGammaSources( pAstroImage );
			//unload xgamma catalog
			m_pCatalogXGamma->UnloadCatalog( );
		}

	}

	return( nFound );
}

/////////////////////////////////////////////////////////////////////
// Method:	FindSupernovasInImage
// Class:	CSky
// Purpose:	find all supernovas in image
// Input:	image to look at
// Output:	no of supernovas found -1 if an error has occured
/////////////////////////////////////////////////////////////////////
long CSky::FindSupernovasInImage( CAstroImage* pAstroImage )
{
	long nFound = -1;

	if( m_pConfig->m_bMatchLocateSupernovas && m_pConfig->m_nSNCatalogToUse > 0 )
	{
		int nCatIndex = GetCatalogIndexById( m_pConfig->m_nSNCatalogToUse );
		
		if( nCatIndex >= 0 )
		{
			m_pCatalogSupernovas->SetProfile( m_vectCatalogDef[nCatIndex] );
			// set start search
			if( m_pUnimapWorker && HasGui() ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Searching for Supernovas ...") );
			nFound = m_pSkyFinder->FindSupernovas( pAstroImage );
			// unload supernovas catalog
			m_pCatalogSupernovas->UnloadCatalog( );
		}
	}

	return( nFound );
}

/////////////////////////////////////////////////////////////////////
// Method:	FindBlackholesInImage
// Class:	CSky
// Purpose:	find all blackholes in image
// Input:	image to look at
// Output:	no of blackoles found -1 if an error has occured
/////////////////////////////////////////////////////////////////////
long CSky::FindBlackholesInImage( CAstroImage* pAstroImage )
{
	long nFound = -1;

	if( m_pConfig->m_bMatchLocateBlackholes && m_pConfig->m_nBHCatalogToUse > 0 )
	{
		int nCatIndex = GetCatalogIndexById( m_pConfig->m_nBHCatalogToUse );
		
		if( nCatIndex >= 0 )
		{
			m_pCatalogBlackholes->SetProfile( m_vectCatalogDef[nCatIndex] );
			// set start search
			if( m_pUnimapWorker && HasGui() ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Searching for Blackholes ...") );
			nFound = m_pSkyFinder->FindBlackholes( pAstroImage );
			// unload supernovas catalog
			m_pCatalogBlackholes->UnloadCatalog( );
		}
	}

	return( nFound );
}

/////////////////////////////////////////////////////////////////////
// Method:	FindMStarsInImage
// Class:	CSky
// Purpose:	find all multiple/double stars in image
// Input:	image to look at
// Output:	no of multiple/double stars found -1 if an error has occured
/////////////////////////////////////////////////////////////////////
long CSky::FindMStarsInImage( CAstroImage* pAstroImage )
{
	long nFound = -1;

	if( m_pConfig->m_bMatchLocateMStars && m_pConfig->m_nMSCatalogToUse > 0 )
	{
		int nCatIndex = GetCatalogIndexById( m_pConfig->m_nMSCatalogToUse );
		
		if( nCatIndex >= 0 )
		{
			m_pCatalogMStars->SetProfile( m_vectCatalogDef[nCatIndex] );
			// set start search
			if( m_pUnimapWorker && HasGui() ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Searching for Multiple/Double Stars ...") );
			nFound = m_pSkyFinder->FindMStars( pAstroImage );
			// unload multiple/double stars catalog
			m_pCatalogMStars->UnloadCatalog( );
		}
	}

	return( nFound );
}

/////////////////////////////////////////////////////////////////////
// Method:	FindExoplanetsInImage
// Class:	CSky
// Purpose:	find all extra-solar planets in image
// Input:	image to look at
// Output:	no of exoplanets found -1 if an error has occured
/////////////////////////////////////////////////////////////////////
long CSky::FindExoplanetsInImage( CAstroImage* pAstroImage )
{
	long nFound = -1;

	if( m_pConfig->m_bMatchLocateExoplanets && m_pConfig->m_nEXPCatalogToUse > 0 )
	{
		int nCatIndex = GetCatalogIndexById( m_pConfig->m_nEXPCatalogToUse );
		
		if( nCatIndex >= 0 )
		{
			m_pCatalogExoplanets->SetProfile( m_vectCatalogDef[nCatIndex] );
			// set start search
			if( m_pUnimapWorker && HasGui() ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Searching for Extra-Solar Planets ...") );
			nFound = m_pSkyFinder->FindExoplanets( pAstroImage );
			// unload exo-planets catalog
			m_pCatalogExoplanets->UnloadCatalog( );
		}
	}

	return( nFound );
}

/////////////////////////////////////////////////////////////////////
// Method:	FindAsteroidsInImage
// Class:	CSky
// Purpose:	find all asteroids in the image
// Input:	image to look at
// Output:	no of asteroids found -1 if an error has occured
/////////////////////////////////////////////////////////////////////
long CSky::FindAsteroidsInImage( CAstroImage* pAstroImage )
{
	long nFound = -1;

	if( m_pConfig->m_bMatchLocateAsteroids && m_pConfig->m_nASTCatalogToUse > 0 )
	{
		int nCatIndex = GetCatalogIndexById( m_pConfig->m_nASTCatalogToUse );
		
		if( nCatIndex >= 0 )
		{
			m_pCatalogAsteroids->SetProfile( m_vectCatalogDef[nCatIndex] );
			// set start search
			if( m_pUnimapWorker && HasGui() ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Searching for Asteroids ...") );
			nFound = m_pSkyFinder->FindAsteroids( pAstroImage );
			// unload catalog
			m_pCatalogAsteroids->UnloadCatalog( );
		}
	}

	return( nFound );
}

/////////////////////////////////////////////////////////////////////
// Method:	FindCometsInImage
// Class:	CSky
// Purpose:	find all comets in the image
// Input:	image to look at
// Output:	no of comets found -1 if an error has occured
/////////////////////////////////////////////////////////////////////
long CSky::FindCometsInImage( CAstroImage* pAstroImage )
{
	long nFound = -1;

	if( m_pConfig->m_bMatchLocateComets && m_pConfig->m_nCOMCatalogToUse > 0 )
	{
		int nCatIndex = GetCatalogIndexById( m_pConfig->m_nCOMCatalogToUse );
		
		if( nCatIndex >= 0 )
		{
			m_pCatalogComets->SetProfile( m_vectCatalogDef[nCatIndex] );
			// set start search
			if( m_pUnimapWorker && HasGui() ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Searching for Comets ...") );
			nFound = m_pSkyFinder->FindComets( pAstroImage );
			// unload catalog
			m_pCatalogComets->UnloadCatalog( );
		}
	}

	return( nFound );
}

/////////////////////////////////////////////////////////////////////
// Method:	FindAesInImage
// Class:	CSky
// Purpose:	find all artificial earth satellites in the image
// Input:	image to look at
// Output:	no of satellites found -1 if an error has occured
/////////////////////////////////////////////////////////////////////
long CSky::FindAesInImage( CAstroImage* pAstroImage )
{
	long nFound = -1;

	if( m_pConfig->m_bMatchLocateAes && m_pConfig->m_nAESCatalogToUse > 0 )
	{
		int nCatIndex = GetCatalogIndexById( m_pConfig->m_nAESCatalogToUse );
		
		if( nCatIndex >= 0 )
		{
			m_pCatalogAes->SetProfile( m_vectCatalogDef[nCatIndex] );
			// set start search
			if( m_pUnimapWorker && HasGui() ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Searching for Artificial Eart Satellites ...") );
			nFound = m_pSkyFinder->FindAes( pAstroImage );
			// if found do other operations
			if( nFound )
			{
				std::sort( pAstroImage->m_vectCatAes, pAstroImage->m_vectCatAes+pAstroImage->m_nCatAes, OnSortAesByObsDistDesc );
			}

		}
	}

	return( nFound );
}

/////////////////////////////////////////////////////////////////////
// Method:	FindSolarPlanetsInImage
// Class:	CSky
// Purpose:	find all solar planets and their natural satellites 
//			in the current image
// Input:	image to look at
// Output:	no of satellites found -1 if an error has occured
/////////////////////////////////////////////////////////////////////
long CSky::FindSolarPlanetsInImage( CAstroImage* pAstroImage )
{
	long nFound = -1;

	if( m_pConfig->m_bMatchLocateSolar )
	{
		// set start search
		if( m_pUnimapWorker && HasGui() ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Searching for Planets ...") );
		nFound = m_pSkyFinder->FindSolarPlanets( pAstroImage );
	}

	return( nFound );
}

/////////////////////////////////////////////////////////////////////
void CSky::Convert_SkyCoord_B1950_to_J2000( double& ra, double& dec )
{
	fk425( &ra, &dec );
}

/////////////////////////////////////////////////////////////////////
void CSky::ProjectSkyCoordToImageXY( CAstroImage* pAstroImage, double nRa, double nDec, double& x, double& y )
{
	StarDef star;
	star.ra = nRa;
	star.dec = nDec;

	// project star using the found orig
	ProjectStar( star, pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );
	// larry :: my damn multiplication factor
	star.x = star.x*pAstroImage->m_nSkyProjectionScale; //10;
	star.y = star.y*pAstroImage->m_nSkyProjectionScale; //10;
	// and now apply inverse trans
	m_pSkyFinder->m_pStarMatch->ApplyObjInv( star );

	x = star.x;
	y = star.y;
}

// plate solve astro-image
////////////////////////////////////////////////////////////////////
int CSky::SolveAstroImage( CAstroImage* pAstroImage, CUnimapWorker* pUnimapWorker )
{
	pAstroImage->m_pSky = this;

	// now if i got a hint 
	if( pAstroImage->IsHint() && m_pConfig->m_bStarsCatLoadLocalRegion )
	{
		if( pUnimapWorker && HasGui() ) pUnimapWorker->SetWaitDialogMsg( wxT("Loading catalog region ...") );
		// get hint data
		double nHintCenterRa=0, nHintCenterDec=0, nHintFieldWidth=0, nHintFieldHeight=0;
		pAstroImage->GetHint( nHintCenterRa, nHintCenterDec, nHintFieldWidth, nHintFieldHeight );
		// load my match catalog region
		int nLoad = LoadMatchCatalogRegion( nHintCenterRa, nHintCenterDec, 
									nHintFieldWidth*m_pConfig->m_nSearchAreaIncrementOnRA, 
									nHintFieldHeight*m_pConfig->m_nSearchAreaIncrementOnDEC );

	} else
	{
		if( pUnimapWorker && HasGui() ) pUnimapWorker->SetWaitDialogMsg( wxT("Loading catalog ...") );
		// load the entire catalog if any
		LoadMatchCatalog( );
	}

	// check if there are stars in the catalog
	if( m_pCatalogStars->m_nStar < 30 )
	{
		if( pUnimapWorker ) pUnimapWorker->DisplayNotification( wxT("There are no stars or to few in the catalog\n"), 1 );
//		m_pFrame->GetToolBar()->ToggleTool( wxID_BUTTON_MATCH, 0 );
		return( 0 );
	}

	// clear image match info
	pAstroImage->ClearMatched( );

	if( pUnimapWorker && HasGui() ) pUnimapWorker->Log( wxT("INFO :: start looking for a match") );
	// use this to search using user data
	if( pUnimapWorker && HasGui() ) pUnimapWorker->SetWaitDialogMsg( wxT("Searching for objects pattern match ...") );

	////////////////////////////
	// :: SEARCH only area if hint or full 
	int nFoundStatus=0;
	if( pAstroImage->IsHint() )
		nFoundStatus = m_pSkyFinder->SearchByHint( pAstroImage );
	else
		nFoundStatus = m_pSkyFinder->SearchAllSky( pAstroImage );
	////////////////////////////
	
	// if found ... do the other calcs
	if( nFoundStatus )
	{
		pAstroImage->CalcMatchMinMax( );
		// set current catalog id used
		pAstroImage->m_nCatalogForMatching = m_pConfig->m_nCatalogForMatching;

		// look for other catalog stars - i do this instead si set cat stars not matched
//		m_pUnimap->SetWaitDialogMsg( "Searching for other catalog objects ..." );
//		m_pCatalog->FindCatStars( pAstroImage );

		/////////////////////
		// FIND :: DSO
		FindDsoObjectsInImage( pAstroImage );

		/////////////////
		// FIND :: RADIO SOURCES
		FindRadioSourcesInImage( pAstroImage );

		/////////////////
		// FIND :: X-RAY/GAMMA SOURCES
		FindXGammaSourcesInImage( pAstroImage );

		/////////////////
		// FIND :: SUPERNOVAS
		FindSupernovasInImage( pAstroImage );

		/////////////////
		// FIND :: BLACKHOLES
		FindBlackholesInImage( pAstroImage );

		/////////////////
		// FIND :: MULTIPLE/DOUBLE STARS
		FindMStarsInImage( pAstroImage );

		/////////////////
		// FIND :: EXTRA-SOLAR PLANERS
		FindExoplanetsInImage( pAstroImage );

		/////////////////
		// FIND :: ASTEROIDS
		FindAsteroidsInImage( pAstroImage );

		/////////////////
		// FIND :: COMETS
		FindCometsInImage( pAstroImage );

		/////////////////
		// FIND :: ARTIFICIAL EARTH SATELLITES
		FindAesInImage( pAstroImage );

		/////////////////
		// FIND :: SOLAR SYSTEM PLANET/SUN/MOON
		FindSolarPlanetsInImage( pAstroImage );

		// look for constelations
		if( pUnimapWorker && HasGui() ) pUnimapWorker->SetWaitDialogMsg( wxT("Searching for Constellations ...") );
		m_pConstellations->FindConstellation( pAstroImage );

		// now set the star names
		pAstroImage->SetSky( this );

		// attempt to set best title 
		pAstroImage->SetBestTitle();

		///////////////////////
		// here we set that the image was hanged 
		pAstroImage->m_bIsChanged = 1;
		pAstroImage->m_bIsMatched = 1;
	}

	////////////////////////////////////////////////
	// now at the end unload the matching catalog to free ram
	m_pCatalogStars->UnloadCatalog( );
	
	return( 1 );
}

////////////////////////////////////////////////////////////////////
void CSky::CalcFOVByInstr( double& w, double& h, double focal, double swidth, double sheight )
{
	double nAngSize=0.0;
	// width
	nAngSize = (double) 2 * atan( (double) swidth / (double) (2 * focal ) );
	w = (double) (57.3 * nAngSize);
	// height
	nAngSize = (double) 2 * atan( (double) sheight / (double) (2 * focal ) );
	h = (double) (57.3 * nAngSize);

	// return (2 * Math.atan(x / (2 * f)) * 180 / Math.PI);
	// reverse calc fov
	//f = (x / (2 * Math.tan(Math.PI * fov / 360)));
	//return f / crop;

}

