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
#include "../util/folders.h"
#include "sky.h"
#include "catalog_stars.h"
#include "../util/func.h"
#include "../unimap_worker.h"
#include "vizier.h"

// main header
#include "catalog_stars_sao.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogStarsSao
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogStarsSao::CSkyCatalogStarsSao( CSkyCatalogStars* pCatalogStars, CUnimapWorker* pWorker )
{
	m_pCatalogStars = pCatalogStars;
	m_pUnimapWorker = pWorker;

	// set my stuff
	m_pCatalogStars->m_strFile = DEFAULT_SAO_BINARY_FILENAME;
	// toclean: this defaults set - i take this from file ?!!?
	m_pCatalogStars->m_strNameAbbreviation = wxT("Sao");
	m_pCatalogStars->m_strName = wxT("SAO Star Catalog");
	m_pCatalogStars->m_strLabel = wxT("SAO");
	m_pCatalogStars->m_strFullName = wxT("Smithsonian Astrophysical Observatory Star Catalog");

	m_pCatalogStars->m_strAuthor = wxT("SAO Staff");
	m_pCatalogStars->m_strOrganization = wxT("Smithsonian Astrophysical Observatory");
	m_pCatalogStars->m_strBibcode = wxT("I/131A/sao");
	m_pCatalogStars->m_strFootPrintImg = unMakeAppPath(wxT("data/catalog/stars/sao_footprint.gif"));

	m_pCatalogStars->m_strCreationDate = wxT("1966");
	m_pCatalogStars->m_strUpdateDate = wxT("1990");

	m_pCatalogStars->m_nCatalogLoaded = CATALOG_ID_SAO;
	m_pCatalogStars->m_nObjectType = CAT_OBJECT_TYPE_SAO;

	m_pCatalogStars->m_nRecordsNo = 258997;

	// set sources for full catalog access
	m_pCatalogStars->m_vectSourcesFull[0] = CATALOG_DOWNLOAD_SOURCE_VIZIER;
	m_pCatalogStars->m_vectSourcesFull[1] = CATALOG_DOWNLOAD_SOURCE_UNIMAP;
	m_pCatalogStars->m_nSourcesFull = 2;

	// set sources for query
	m_pCatalogStars->m_vectSourcesQuery[0] = CATALOG_QUERY_SOURCE_VIZIER;
	m_pCatalogStars->m_nSourcesQuery = 1;

	// set description
	m_pCatalogStars->m_strDescription = _T( "This machine-readable SAO catalog from the Astronomical Data Center is based on an original binary version of the Smithsonian Astrophysical Observatory Star Catalog (SAO, SAO Staff 1966). Subsequent improvements by T. A. Nagy (1979) included the addition of equatorial coordinates in radians and cross-identifications from the Table of Correspondences SAO/HD/DM/GC (Morin 1973). As a prelude to creation of the 1984 version of the SAO, a new version of the SAO-HD-GC-DM Cross Index was prepared (Roman, Warren, and Schofield 1983). The 1984 version of the SAO contained the corrected and extended cross identifications, all errata published up to January 1984 and known to the ADC, numerous errors forwarded to the ADC by colleagues, and errors discovered at the ADC during the course of this work. Clayton A. Smith of the U. S. Naval Observatory provided J2000.0 positions and proper motions for the SAO stars. Published and unpublished errors discovered in the previous version (1984) have been corrected (up to May 1991). The catalog contains SAO number; the right ascension and declination with a B1950.0 equinox and epoch; annual proper motion and its standard deviation, photographic and visual magnitudes; spectral type; references to sources; the Durchmusterung (DM) identifier if the star is listed in the Bonner DM (BD), Cordoba DM (CD), or Cape Photographic DM (CP); component identification; The Henry Draper (Extension) (HD or HDE) number; and J2000 positions and proper motions. Multiple-star component identifications have been added to stars where more than one SAO entry has the same DM number. The Henry Draper Extension (HDE) numbers have been added for stars found in both volumes of the extension. Data for duplicate SAO entries (those referring to the same star) have been flagged. J2000 positions in usual units and in radians have been added." );

}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyCatalogStarsSao
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogStarsSao::~CSkyCatalogStarsSao( )
{

}

/////////////////////////////////////////////////////////////////////
// Method:	LoadStripedSao
// Class:	CSkyCatalogStarsSao
// Purpose:	load striped (just columns id/ra/dec/mag) sao star catalog
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CSkyCatalogStarsSao::LoadStripedSao( const wxString& strFile )
{
	FILE* pFile = NULL;
	wxChar strLine[2000];
	wxString strMsg;
	// define my regex: event id = name, lat, ra, mag, spect
	wxRegEx reLine( wxT("^SAO ([0-9]+)\\|([0-9\\.\\-]+)\\|([0-9\\.\\-]+)\\|([0-9\\.\\-]+)\\|") );

	m_pCatalogStars->m_nMinX = DBL_MAX;
	m_pCatalogStars->m_nMinY = DBL_MAX;
	m_pCatalogStars->m_nMaxX = DBL_MIN;
	m_pCatalogStars->m_nMaxY = DBL_MIN;

	// debug
//	m_pUnimapWorker->Log( "INFO :: Loading configuration." );
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
		// get one line out of the config file
		wxFgets( strLine, 2000, pFile );
		//wxString strWxLine(strLine,wxConvUTF8);

		// check if event list
		if( reLine.Matches( strLine ) )
		{
			wxString strStarNo = reLine.GetMatch(strLine, 1 );
			wxString strStarRa = reLine.GetMatch(strLine, 2 );
			wxString strStarDec = reLine.GetMatch(strLine, 3 );
			wxString strStarMag = reLine.GetMatch(strLine, 4 );
			long nStarNo = 0;
			if( strStarNo.ToLong( &nStarNo ) )
			{
				double nRa =0, nDec = 0, nMag = 0;
				strStarRa.ToDouble( &nRa );
				strStarDec.ToDouble( &nDec );
				strStarMag.ToDouble( &nMag );
				// mark star
				m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].cat_no = nStarNo;
				m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].ra = nRa; //x
				m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].dec = nDec; //y
				m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].mag = nMag;

				// calculate min and max --- ???? do i need or just delete ?!!!!
				if( m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].ra < m_pCatalogStars->m_nMinX ) m_pCatalogStars->m_nMinX = m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].ra;
				if( m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].ra > m_pCatalogStars->m_nMaxX ) m_pCatalogStars->m_nMaxX = m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].ra;

				if( m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].dec < m_pCatalogStars->m_nMinY ) m_pCatalogStars->m_nMinY = m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].dec;
				if( m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].dec > m_pCatalogStars->m_nMaxY ) m_pCatalogStars->m_nMaxY = m_pCatalogStars->m_vectStar[m_pCatalogStars->m_nStar].dec;

				// increase star counter
				m_pCatalogStars->m_nStar++;

			}

		}
	}
	// close my file
	fclose( pFile );

	m_pCatalogStars->m_nCatalogLoaded = CATALOG_ID_SAO;

	return( 1 );
}


/////////////////////////////////////////////////////////////////////
// Method:	Load
// Class:	CSkyCatalogStarsSao
// Purpose:	load sao
// Input:	 load limit, region flag, ra/dec, size
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogStarsSao::Load( int nLoadType, int nLoadLimit, int bRegion, double nRa, double nDec,
										double nWidth, double nHeight )
{
	// check catalog loaded
	if( m_pCatalogStars->m_vectStar != NULL )
	{
		if( m_pCatalogStars->m_nCatalogLoaded != CATALOG_ID_SAO )
			m_pCatalogStars->UnloadCatalog();
			//free( m_vectStar );
		else
			return( 0 );
	}
	
	unsigned long nStars = 0;

	// case :: binary load
	if( nLoadType == CATALOG_LOAD_FILE_BINARY )
	{
		nStars = m_pCatalogStars->LoadBinary( DEFAULT_SAO_BINARY_FILENAME, nLoadLimit, bRegion, nRa, nDec, nWidth, nHeight );

	} else if( nLoadType == CATALOG_LOAD_REMOTE )
	{
		CVizier oRemote( m_pCatalogStars, m_pUnimapWorker );
		nStars = oRemote.GetStarsFromVizier( CATALOG_ID_SAO, bRegion, nRa, nDec, nWidth, nHeight );
	}

	return( nStars );
}

