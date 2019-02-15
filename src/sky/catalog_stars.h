////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_STARS_H
#define _CATALOG_STARS_H

// local headers
#include "./star.h"
#include "catalog_types.h"
#include "catalog.h"
#include "../util/folders.h"

// star catalogs
#include "catalog_stars_sao.h"
#include "catalog_stars_hipparcos.h"
#include "catalog_stars_tycho.h"
#include "catalog_stars_usnob.h"
#include "catalog_stars_nomad.h"

// default catalog file names
#define DEFAULT_SAO_BINARY_FILENAME			unMakeAppPath(wxT("data/catalog/stars/sao.adm"))
#define DEFAULT_HIPPARCOS_BINARY_FILENAME	unMakeAppPath(wxT("data/catalog/stars/hipparcos.adm"))
#define DEFAULT_TYCHO2_BINARY_FILENAME		unMakeAppPath(wxT("data/catalog/stars/tycho2.adm"))
#define DEFAULT_USNOB1_BINARY_FILENAME		unMakeAppPath(wxT("data/catalog/stars/usnob1_part.adm"))
// blocks
#define VECT_STARS_START_MEM_ALLOC_BLOCK_SIZE	200

// external classes
class CSky;

// global sorting methods
static bool OnSortCatStar( StarDefCatBasic t1, StarDefCatBasic t2 ) { return( t1.mag<t2.mag ); }

class CSkyCatalogStars : public CSkyCatalog
{
// methods:
public:
	CSkyCatalogStars( CSky* pSky );
	~CSkyCatalogStars( );

	/////////////////////////
	// methods to process catalog
	long SortByMagnitude( );
	int ExtractStarsFromCatalog( StarDef* vectStar, int nStarCount,
										unsigned int nImgWidth, unsigned int nImgHeight,
										double& nMinX, double& nMaxX, 
										double& nMinY, double& nMaxY, int nPrjType );
	long CutAreaFromCatalog( double nCenterRa, double nCenterDec, double nRadius,
					StarDef* vectStar, int nStarCount,
					unsigned int nImgWidth, unsigned int nImgHeight,
					double& nMinX, double& nMaxX, double& nMinY, double& nMaxY, 
					int bWithImgProjection = 0, double nPrjScale=0.0 );

	///////////////////////////////////////////
	// global load
	void UnloadCatalog( );
	int ExportBinary(  );
	unsigned long LoadBinary( const wxChar* strFile, int nLoadLimit,
								int bRegion, double nRa=0, double nDec=0,
										double nWidth=0, double nHeight=0 );

	unsigned long LoadCatalog( int nCatType );
	unsigned long LoadCatalogRegion( int nCatType, double nRa, double nDec,
											double nWidth, double nHeight );
	// method to call to download catalogs from a remote location
	unsigned long DownloadCatalog( int nCatId, int nCatSource );

	// other methods
	int IsCatStar( unsigned char nCatType, unsigned long nCatNo, 
						unsigned int nZoneNo, unsigned char nCompNo, char* strCatName=NULL );

	int GetStarRaDec( unsigned long nCatNo, double& nRa, double& nDec );
	void SetStarStringCatalogName( StarDefCatBasic& pStar, const char* strCatName );
	int GetSimpleCatName( StarDefCatBasic& pStar, wxString& strStarName );

//////////////////////
// DATA
public:

	//////////////////
	// the main vector where i store the catalog stars
	StarDefCatBasic* m_vectStar;
	unsigned long m_nStar;
	unsigned long m_nStarAllocated;

	////////////////////
	// last region loaded
	int m_bLastLoadRegion;
	double m_nLastRegionLoadedCenterRa;
	double m_nLastRegionLoadedCenterDec;
	double m_nLastRegionLoadedWidth;
	double m_nLastRegionLoadedHeight;

	// other
	double m_nMinX, m_nMinY, m_nMaxX, m_nMaxY;
	double m_nMaxMag;
	double m_nMinMag;


};

#endif

