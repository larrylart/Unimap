////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_DSO_MGC_H
#define _CATALOG_DSO_MGC_H

class CSkyCatalogDso;
class CUnimapWorker;

class CSkyCatalogDsoMgc
{
// methods:
public:
	CSkyCatalogDsoMgc( CSkyCatalogDso* pCatalogDso, CUnimapWorker* pWorker=NULL );
	~CSkyCatalogDsoMgc( );

	/////////////////////////////////////
	// PGC galaxy catalog methods
	unsigned long Load( int nLoadType=0, int nLoadSource=0, int nLoadLimit=0, int bRegion=0, double nRa=0.0, double nDec=0.0, double nRadius=0.0 );

//////////////////////////
// DATA
public:
	CSkyCatalogDso* m_pCatalogDso;
	CUnimapWorker* m_pUnimapWorker;
};

#endif
