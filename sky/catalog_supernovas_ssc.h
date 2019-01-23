
#ifndef _CATALOG_SUPERNOVAS_SSC_H
#define _CATALOG_SUPERNOVAS_SSC_H

// local headers
#include "catalog_supernovas.h"

// external classes
class CUnimapWorker;

class CSkyCatalogSupernovasSsc
{
// methods:
public:
	CSkyCatalogSupernovasSsc( CSkyCatalogSupernovas* pCatalogSupernovas, CUnimapWorker* pWorker=NULL );
	~CSkyCatalogSupernovasSsc( );

	/////////////////////////////////////
	// load/export catalog methods
	unsigned long Load( int nLoadType=0, int nLoadSource=0, int nLoadLimit=0, int bRegion=0, double nRa=0.0, double nDec=0.0, double nRadius=0.0 );
	unsigned long LoadBinary( const wxString& strFile, double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	int ExportBinary( DefCatBasicSupernova* vectCatalog, unsigned int nSize );

//////////////////////////
// DATA
public:
	CSkyCatalogSupernovas* m_pCatalogSupernovas;
	CUnimapWorker* m_pUnimapWorker;
};

#endif
