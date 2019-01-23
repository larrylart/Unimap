
#ifndef _CATALOG_BLACKHOLES_RXE_AGN_H
#define _CATALOG_BLACKHOLES_RXE_AGN_H

// local headers
#include "catalog_blackholes.h"

// external classes
class CUnimapWorker;

class CSkyCatalogBlackholesRxeagn
{
// methods:
public:
	CSkyCatalogBlackholesRxeagn( CSkyCatalogBlackholes* pCatalogBlackholes, CUnimapWorker* pWorker=NULL );
	~CSkyCatalogBlackholesRxeagn( );

	/////////////////////////////////////
	// load/export catalog methods
	unsigned long Load( int nLoadType=0, int nLoadSource=0, int nLoadLimit=0, int bRegion=0, double nRa=0.0, double nDec=0.0, double nRadius=0.0 );
	unsigned long LoadBinary( const wxString& strFile, double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	int ExportBinary( DefCatBlackhole* vectCatalog, unsigned int nSize );

//////////////////////////
// DATA
public:
	CSkyCatalogBlackholes* m_pCatalogBlackholes;
	CUnimapWorker* m_pUnimapWorker;
};

#endif
