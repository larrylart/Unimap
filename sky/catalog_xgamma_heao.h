
#ifndef _CATALOG_XGAMMA_HEAO_A1_H
#define _CATALOG_XGAMMA_HEAO_A1_H

// local headers
#include "catalog_xgamma.h"

// external classes
class CUnimapWorker;

class CSkyCatalogXGammaHeao
{
// methods:
public:
	CSkyCatalogXGammaHeao( CSkyCatalogXGamma* pCatalogXGamma, CUnimapWorker* pWorker=NULL );
	~CSkyCatalogXGammaHeao( );

	/////////////////////////////////////
	// MSL radio sources catalog methods
	unsigned long Load( int nLoadType=0, int nLoadSource=0, int nLoadLimit=0, int bRegion=0, double nRa=0.0, double nDec=0.0, double nRadius=0.0 );

	unsigned long LoadBinary( const wxString& strFile, double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	int ExportBinary( DefCatBasicXGamma* vectCatalog, unsigned int nSize );

//////////////////////////
// DATA
public:
	CSkyCatalogXGamma* m_pCatalogXGamma;
	CUnimapWorker* m_pUnimapWorker;
};

#endif
