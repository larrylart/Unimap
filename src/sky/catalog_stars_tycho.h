////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_STARTS_TYCHO_H
#define _CATALOG_STARTS_TYCHO_H

// local headers
#include "catalog_types.h"

class CSkyCatalogStars;
class CUnimapWorker;

class CSkyCatalogStarsTycho
{
// methods:
public:
	CSkyCatalogStarsTycho( CSkyCatalogStars* pCatalogStars, CUnimapWorker* pWorker );
	~CSkyCatalogStarsTycho( );

	// binary load/export methods
	int ExportTycho2Binary(  );
//	int LoadBinary( const char* strFile, int nLoadLimit );

	unsigned long Load( int nLoadType=0, int nLoadLimit=0,
								int bRegion=0, double nRa=0, double nDec=0,
										double nWidth=0, double nHeight=0 );

	unsigned long LoadTycho2Binary( const wxString& strFile, int nLoadLimit,
								int bRegion, double nRa=0, double nDec=0,
										double nWidth=0, double nHeight=0 );
	// tycho 2
	long LoadTycho2( const wxString& strFile );

//////////////////////////
// DATA
public:
	CSkyCatalogStars* m_pCatalogStars;
	CUnimapWorker* m_pUnimapWorker;
};

#endif
