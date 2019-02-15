////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_STARTS_SAO_H
#define _CATALOG_STARTS_SAO_H

// local headers
#include "catalog_types.h"

class CSkyCatalogStars;
class CUnimapWorker;

class CSkyCatalogStarsSao
{
// methods:
public:
	CSkyCatalogStarsSao( CSkyCatalogStars* pCatalogStars, CUnimapWorker* pWorker=NULL );
	~CSkyCatalogStarsSao( );

	unsigned long Load( int nLoadType=0, int nLoadLimit=0,
								int bRegion=0, double nRa=0, double nDec=0,
										double nWidth=0, double nHeight=0 );
	// load sao text/names
	long LoadStripedSao( const wxString& strFile );

//////////////////////////
// DATA
public:
	CSkyCatalogStars* m_pCatalogStars;
	CUnimapWorker* m_pUnimapWorker;

};

#endif
