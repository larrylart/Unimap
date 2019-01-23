
#ifndef _CATALOG_MSTARS_WDS_H
#define _CATALOG_MSTARS_WDS_H

// local headers
#include "catalog_mstars.h"

// external classes
class CUnimapWorker;

class CSkyCatalogMStarsWds
{
// methods:
public:
	CSkyCatalogMStarsWds( CSkyCatalogMStars* pCatalogMStars, CUnimapWorker* pWorker=NULL );
	~CSkyCatalogMStarsWds( );

	/////////////////////////////////////
	// load/export catalog methods
	unsigned long Load( int nLoadType=0, int nLoadSource=0, int nLoadLimit=0, int bRegion=0, double nRa=0.0, double nDec=0.0, double nRadius=0.0 );
	unsigned long LoadBinary( const wxString& strFile, double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	int ExportBinary( DefCatMStars* vectCatalog, unsigned int nSize );

//////////////////////////
// DATA
public:
	CSkyCatalogMStars* m_pCatalogMStars;
	CUnimapWorker* m_pUnimapWorker;
};

#endif
