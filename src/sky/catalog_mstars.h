////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_MSTARS_H
#define _CATALOG_MSTARS_H

// local include
#include "./star.h"
#include "catalog.h"
#include "catalog_types.h"
//#include "catalog_mstars_assoc.h"
//#include "catalog_mstars_names.h"

// catalogs path
#define DEFAULT_WDS_BINARY_FILENAME		unMakeAppPath(wxT("data/catalog/mstars/wds.adm"))
#define DEFAULT_CCDM_BINARY_FILENAME	unMakeAppPath(wxT("data/catalog/mstars/ccdm.adm"))
// memory allocation
#define VECT_START_MEM_ALLOC_BLOCK_SIZE		200
#define VECT_MSTARS_MEM_ALLOC_BLOCK_SIZE	200

// external classes 
class CUnimapWorker;
class CSky;

// blackholes catalog structure
typedef struct
{
//#ifdef _DEBUG
	char cat_name[30];
//#else
//	char* cat_name;
//#endif

	// catalogs numbers
	unsigned long cat_no;

	// x/y in image
	double x;
	double y;

	// ra/dec - coordinates
	double ra;
	double dec;

	// magnitude
	double mag;
	double mag2;

	// [A-Z?] Concerned component
	char comp[10];

	// spectral type
	char spectral_type[11];
	
	// Position Angle at date Obs1 
	double pos_ang;
	double pos_ang2;

	// proper motion
	double pm_ra;
	double pm_dec;
	// Secondary Proper Motion 
	double pm_ra2;
	double pm_dec2;

	// proper motion
	char pm_note[5];

	// separation
	double sep;
	double sep2;

	// Number of observations
	unsigned int nobs;

	// Date of first satisfactory observation
	double obs_date;
	double obs_date2;

	// Discoverer Code/NAME (1 to 3 letters) and Number
	char discoverer[11];

	// Notes 
	char notes[11];

	// catalog type
	unsigned char cat_type;
	// object type
	unsigned char type;

} DefCatMStars;

// reset to zero function
void ResetObjCatMStars( DefCatMStars& src );

// global sorting methods
//static bool OnSortCatMStars( DefCatMStars t1, DefCatMStars t2 ) { return( t1.mass>t2.mass ); }
//static bool OnSortCatMStarsByA( DefCatMStars t1, DefCatMStars t2 ) { return( t1.a>t2.a ); }

class CSkyCatalogMStars : public CSkyCatalog
{
// methods:
public:
	CSkyCatalogMStars( CSky* pSky );
	~CSkyCatalogMStars( );

	// general dso methods
	void UnloadCatalog( );

//	long SortByMass( );
//	long SortByA( );
	int GetRaDec( unsigned long nCatNo, double& nRa, double& nDec );
	int GetName( DefCatMStars& mstars, wxString& strMStarsCatName );

	unsigned long Load( int nType, int bRegion=0, double nCenterRa=0, double nCenterDec=0, double nRadius=0 );
	unsigned long LoadBinary( double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	unsigned long LoadBinary( const wxChar* strFile, double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	int ExportBinary( DefCatMStars* vectCatalog, unsigned int nSize );

	// method to call to download catalogs from a remote location
	unsigned long DownloadCatalog( int nCatId, int nCatSource );

//////////////////////////
// DATA
public:
	CSky* m_pSky;
	CUnimapWorker* m_pUnimapWorker;

	// blackholes catalog - general purpose
	DefCatMStars*	m_vectData;

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

