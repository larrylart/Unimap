
#ifndef _CATALOG_ASTEROIDS_OMP_H
#define _CATALOG_ASTEROIDS_OMP_H

// local headers
#include "catalog_asteroids.h"

// external classes
class CUnimapWorker;

class CSkyCatalogAsteroidsOmp
{
// methods:
public:
	CSkyCatalogAsteroidsOmp( CSkyCatalogAsteroids* pCatalogAsteroids, CUnimapWorker* pWorker=NULL );
	~CSkyCatalogAsteroidsOmp( );

	/////////////////////////////////////
	// load/export catalog methods
	unsigned long Load( int nLoadType=0, int nLoadSource=0, int nLoadLimit=0, int bRegion=0, double nRa=0.0, double nDec=0.0, double nRadius=0.0 );
	unsigned long LoadBinary( const wxString& strFile, double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	int ExportBinary( DefCatAsteroid* vectCatalog, unsigned int nSize );

//////////////////////////
// DATA
public:
	CSkyCatalogAsteroids* m_pCatalogAsteroids;
	CUnimapWorker* m_pUnimapWorker;
};

#endif
