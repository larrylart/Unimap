
#ifndef _CATALOG_AES_CELESTRAK_H
#define _CATALOG_AES_CELESTRAK_H

// local headers
#include "catalog_aes.h"

// external classes
class CUnimapWorker;

class CSkyCatalogAesCelestrak
{
// methods:
public:
	CSkyCatalogAesCelestrak( CSkyCatalogAes* pCatalogAes, CUnimapWorker* pWorker=NULL );
	~CSkyCatalogAesCelestrak( );

	/////////////////////////////////////
	// load/export catalog methods
	unsigned long Load( int nLoadType=0, int nLoadSource=0, int nLoadLimit=0, int bRegion=0, double nRa=0.0, double nDec=0.0, double nRadius=0.0 );
	unsigned long LoadBinary( const wxString& strFile, double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	int ExportBinary( DefCatAes* vectCatalog, unsigned int nSize );

//////////////////////////
// DATA
public:
	CSkyCatalogAes* m_pCatalogAes;
	CUnimapWorker* m_pUnimapWorker;
};

#endif
