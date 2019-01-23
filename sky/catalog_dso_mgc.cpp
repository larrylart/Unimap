
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

// main header
#include "catalog_dso_mgc.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogDsoMgc
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogDsoMgc::CSkyCatalogDsoMgc( CSkyCatalogDso* pCatalogDso, CUnimapWorker* pWorker ) 
{
	m_pCatalogDso = pCatalogDso;
	m_pUnimapWorker = pWorker;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyCatalogDsoMgc
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogDsoMgc::~CSkyCatalogDsoMgc()
{
	
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadPGC
// Class:	CSkyCatalogDsoMgc
// Purpose:	load MGC galaxy catalog
// Input:	cat type, source, limit obj, region, pos, size
// Output:	no of objects loaded
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogDsoMgc::Load( int nLoadType, int nLoadSource, int nLoadLimit, 
									  int bRegion, double nCenterRa, double nCenterDec, double nRadius )
{
	// check catalog loaded
	if( m_pCatalogDso->m_vectDso != NULL )
	{
		if( m_pCatalogDso->m_nCatalogLoaded != CATALOG_ID_MGC )
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
			nObj = oRemote.GetDsoFromVizier( CATALOG_ID_MGC, bRegion, nCenterRa, nCenterDec, nRadius, nRadius );
		}
	}

	return( nObj );
}

