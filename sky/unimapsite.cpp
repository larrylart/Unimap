
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
#include "catalog_stars.h"
#include "catalog_dso.h"
#include "../unimap.h"
#include "../util/func.h"
#include "../unimap_worker.h"

// main header
#include "unimapsite.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CUnimapSite
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CUnimapSite::CUnimapSite( CSkyCatalogStars* pCatalogStars, CUnimapWorker* pWorker )
{
	m_pCatalogStars = pCatalogStars;
	m_pUnimapWorker = pWorker;

	Init( );
}

/////////////////////////////////////////////////////////////////////
CUnimapSite::CUnimapSite( CSkyCatalogDso* pCatalogDso, CUnimapWorker* pWorker )
{
	m_pCatalogDso = pCatalogDso;
	m_pUnimapWorker = pWorker;

	Init( );
}

/////////////////////////////////////////////////////////////////////
void CUnimapSite::Init( )
{
	// set sites
	m_vectSite[0] = wxT("http://unimap.larryo.org");
	m_nSite = 1;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CUnimapSite
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CUnimapSite::~CUnimapSite( )
{

}

/////////////////////////////////////////////////////////////////////
// Method:	DownloadCatalog
// Class:	CUnimapSite
// Purpose:	fetch catalog data from online vizier database into a file
// Input:	file name to save localy, cat id
// Output:	how many bytes where loaded
/////////////////////////////////////////////////////////////////////
long CUnimapSite::DownloadCatalog( const wxString& strFile, int nCatalogId )
{
	long nCount=0;
	int i = 0;
	wxString strLog;
	unsigned long nWebFileSize=0;

	wxString strUrl;
	wxString strCatPath = wxT("");

	/////////////////////////
	// :: STARS
	if( nCatalogId == CATALOG_ID_SAO )
	{
		strCatPath = unMakeAppPath(wxT("data/catalog/stars/sao.adm"));
		// set aprox wefile size - for the meter 
		nWebFileSize=7209356;

	} else if( nCatalogId == CATALOG_ID_HIPPARCOS )
	{
		strCatPath = unMakeAppPath(wxT("data/catalog/stars/hipparcos.adm"));
		// set aprox wefile size - for the meter 
		nWebFileSize=3302740;

	} else if( nCatalogId == CATALOG_ID_TYCHO_2 )
	{
		strCatPath = unMakeAppPath(wxT("data/catalog/stars/tycho2.adm"));
		// set aprox wefile size - for the meter 
		nWebFileSize=80205444;

	/////////////////
	// :: DSO
	} else if( nCatalogId == CATALOG_ID_HYPERLEDA )
	{
		strCatPath = unMakeAppPath(wxT("data/catalog/dso/pgc.adm"));
		// set aprox wefile size - for the meter 
		nWebFileSize=0;
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
			strUrl.Printf( wxT("%s/%s"), m_vectSite[i], strCatPath );

			// call online - to do call multiple site addresse - vizier has a few mirrors
			nCount = LoadWebFileT( strUrl, strFile, m_pUnimapWorker, nWebFileSize );
			// if success - exit loop
			if( nCount > 0 ) break;

			// log message
			if( m_pUnimapWorker && i<m_nSite-1 )
			{
				strLog.Printf( wxT("INFO :: failed to load from  Unimap site %s try next mirror %s"), m_vectSite[i], m_vectSite[i-1] );
				m_pUnimapWorker->Log( strLog );
			}
		}

	} else
		nCount = 1;

	return( nCount );
}
