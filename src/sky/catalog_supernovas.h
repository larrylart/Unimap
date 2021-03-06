////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_SUPERNOVAS_H
#define _CATALOG_SUPERNOVAS_H

// local include
#include "./star.h"
#include "catalog.h"
#include "catalog_types.h"
//#include "catalog_supernovas_assoc.h"
//#include "catalog_supernovas_names.h"

// catalogs path
#define DEFAULT_ASC_BINARY_FILENAME		unMakeAppPath(wxT("data/catalog/supernovas/asc.adm"))
#define DEFAULT_SSC_BINARY_FILENAME		unMakeAppPath(wxT("data/catalog/supernovas/ssc.adm"))
// memory allocation
#define VECT_START_MEM_ALLOC_BLOCK_SIZE		200
#define VECT_SUPERNOVAS_MEM_ALLOC_BLOCK_SIZE	200

// external classes
class CUnimapWorker;
class CSky;

// supernovas catalog structure
typedef struct
{
//#ifdef _DEBUG
	wxChar cat_name[30];
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

	// designate uncertain and very uncertain SN
	unsigned char unconfirmed;

	// If available, supernova magnitude at maximum (photometric band indicated) 
	double mag;
	double max_mag;

	// Supernova type
	wxChar sn_type[10];

	// offset from the galaxy nucleus
	double gal_center_offset_x;
	double gal_center_offset_y;

	// parent galaxy
	wxChar parent_galaxy[20];

	// Date of maximum or Discovery 
	double date_max;
	double date_discovery;

	// Name(s) of discoverer(s)
	wxChar discoverer[150];

	// discovery method
	unsigned char disc_method;

	// Code for SN search programme (Observatory)
	wxChar prog_code[3];

	// catalog type
	unsigned char cat_type;
	// object type
	unsigned char type;

} DefCatBasicSupernova;

// reset to zero function
void ResetObjCatSupernovas( DefCatBasicSupernova& src );

// global sorting methods
//static bool OnSortCatSupernovas( DsoDefCatBasic t1, DsoDefCatBasic t2 ) { return( t1.mag<t2.mag ); }
//static bool OnSortCatSupernovasByA( DsoDefCatBasic t1, DsoDefCatBasic t2 ) { return( t1.a>t2.a ); }

class CSkyCatalogSupernovas : public CSkyCatalog
{
// methods:
public:
	CSkyCatalogSupernovas( CSky* pSky );
	~CSkyCatalogSupernovas( );

	// general dso methods
	void UnloadCatalog( );

//	long SortByMagnitude( );
//	long SortByA( );
	int GetRaDec( unsigned long nCatNo, double& nRa, double& nDec );
	int GetName( DefCatBasicSupernova& supernova, wxString& strSupernovaCatName );

	unsigned long Load( int nType, int bRegion=0, double nCenterRa=0, double nCenterDec=0, double nRadius=0 );
	unsigned long LoadBinary( double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	unsigned long LoadBinary( const wxChar* strFile, double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	int ExportBinary( DefCatBasicSupernova* vectCatalog, unsigned int nSize );

	// method to call to download catalogs from a remote location
	unsigned long DownloadCatalog( int nCatId, int nCatSource );

//////////////////////////
// DATA
public:
	CSky* m_pSky;
	CUnimapWorker* m_pUnimapWorker;

	// Supernovas catalog - general purpose
	DefCatBasicSupernova*	m_vectData;

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

