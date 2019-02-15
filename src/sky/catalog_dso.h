////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_DSO_H
#define _CATALOG_DSO_H

// local include
#include "./star.h"
#include "catalog.h"
#include "catalog_types.h"
#include "catalog_dso_assoc.h"
#include "catalog_dso_names.h"

// catalogs path
#define DEFAULT_NGC_BINARY_FILENAME		unMakeAppPath(wxT("data/catalog/dso/ngc.adm"))
#define DEFAULT_IC_BINARY_FILENAME		unMakeAppPath(wxT("data/catalog/dso/ic.adm"))
#define DEFAULT_PGC_BINARY_FILENAME		unMakeAppPath(wxT("data/catalog/dso/pgc.adm"))
#define DEFAULT_MESSIER_BINARY_FILENAME	unMakeAppPath(wxT("data/catalog/dso/messier.adm"))
// memory allocation
#define VECT_START_MEM_ALLOC_BLOCK_SIZE	200
#define VECT_DSO_MEM_ALLOC_BLOCK_SIZE	200

// external classes
class CUnimapWorker;
class CSky;

// reset to zero function
void ResetObjCatDso( DsoDefCatBasic& src );

// global sorting methods
static bool OnSortCatDso( DsoDefCatBasic t1, DsoDefCatBasic t2 ) { return( t1.mag<t2.mag ); }
static bool OnSortCatDsoByA( DsoDefCatBasic t1, DsoDefCatBasic t2 ) { return( t1.a>t2.a ); }

class CSkyCatalogDso : public CSkyCatalog
{
// methods:
public:
	CSkyCatalogDso( CSky* pSky );
	~CSkyCatalogDso( );

	// general dso methods
	void UnloadCatalog( );

	long SortByMagnitude( );
	long SortByA( );
	int GetRaDec( unsigned long nCatNo, double& nRa, double& nDec );

	unsigned long Load( int nType, int bRegion=0, double nCenterRa=0, double nCenterDec=0, double nRadius=0 );
	unsigned long LoadBinary( double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	unsigned long LoadBinary( const wxString& strFile, double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	int ExportBinary( DsoDefCatBasic* vectCatalog, unsigned int nSize );

	// method to call to download catalogs from a remote location
	unsigned long DownloadCatalog( int nCatId, int nCatSource );

//////////////////////////
// DATA
public:
	CSky* m_pSky;
	CUnimapWorker* m_pUnimapWorker;

//	char m_strCatalogFile[255];
//	char m_strCatalogName[255];
//	char m_strCatalogLabel[255];

//	int m_nDsoCatLoaded;
//	int m_nDsoObjType;

	// DSO catalog - general purpose
	DsoDefCatBasic*		m_vectDso;
	unsigned long		m_nDso;
	unsigned long		m_nDsoAllocated;

	// mag references
	double m_nMinMag;
	double m_nMaxMag;

	////////////////////
	// last region loaded
	int m_bLastLoadRegion;
	double m_nLastRegionLoadedCenterRa;
	double m_nLastRegionLoadedCenterDec;
	double m_nLastRegionLoadedWidth;
	double m_nLastRegionLoadedHeight;

};

#endif

