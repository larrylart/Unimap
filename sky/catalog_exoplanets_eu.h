
#ifndef _CATALOG_EXOPLANETS_EU_H
#define _CATALOG_EXOPLANETS_EU_H

// local headers
#include "catalog_exoplanets.h"

// external classes
class CUnimapWorker;

class CSkyCatalogExoplanetsEu
{
// methods:
public:
	CSkyCatalogExoplanetsEu( CSkyCatalogExoplanets* pCatalogExoplanets, CUnimapWorker* pWorker=NULL );
	~CSkyCatalogExoplanetsEu( );

	/////////////////////////////////////
	// load/export catalog methods
	unsigned long Load( int nLoadType=0, int nLoadSource=0, int nLoadLimit=0, int bRegion=0, double nRa=0.0, double nDec=0.0, double nRadius=0.0 );
	unsigned long LoadBinary( const wxString& strFile, double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	int ExportBinary( DefCatExoPlanet* vectCatalog, unsigned int nSize );

//////////////////////////
// DATA
public:
	CSkyCatalogExoplanets* m_pCatalogExoplanets;
	CUnimapWorker* m_pUnimapWorker;
};

#endif
