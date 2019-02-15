////////////////////////////////////////////////////////////////////
// Package:		sky definition
// File:		sky.h
// Purpose:		sky definition
//
// Created by:	Larry Lart on 11-11-2004
// Updated by:	Larry Lart on 04-07-2008
//				Larry Lart on 14-04-2009
//
// Copyright:	(c) 2004-2009 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _SKY_H
#define _SKY_H

// WX includes
#include "wx/wxprec.h"
#include <wx/regex.h>
#include <wx/string.h>
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
// projection types
#define SKY_PROJECTION_TYPE_TAN		0
#define SKY_PROJECTION_TYPE_ARC		1
#define SKY_PROJECTION_TYPE_CAR		2
#define SKY_PROJECTION_TYPE_AIT		3
#define SKY_PROJECTION_TYPE_STG		4
// default for config
#define DEFAULT_CATALOG_USE_REGION		0
#define DEFAULT_CATALOG_MAX_MAG_LIMIT	15.0
#define DEFAULT_CATALOG_USE_NGC			1
#define DEFAULT_CATALOG_USE_IC			1
#define DEFAULT_CATALOG_USE_PGC			0
#define DEFAULT_IMG_SCAN_MATCH_OBJ_NO	30
#define DEFAULT_CAT_SCAN_MATCH_OBJ_NO	30
#define DEFAULT_CAT_MAX_OBJ_MATCH_NO	20000

// projecttion defines
#define SPHTOL 0.00001
#define BADCVAL 0.0
// WAIT DIALOG GUI - DEFAULTS
#define WAIT_DIALOG_USE_NO_OF_CATALOG_STARS		500
// projection methods
#define SKY_TO_IMAGE_PROJECTION_METHOD_GNOMONIC			0
#define SKY_TO_IMAGE_PROJECTION_METHOD_STEREOGRAPHIC	1

// local includes
#include "star.h"
#include "catalog.h"
#include "constellations.h"

/////////////////////
// external classes
//class CLogger;
class CSkyFinder;
class CSkyCatalogDsoNgc;
class CSkyCatalogDsoIc;
class CSkyCatalogDsoMessier;
class CSkyCatalogDsoAssoc;
class CSkyCatalogDsoNames;
class CSkyCatalogStarsAssoc;
class CSkyCatalogStarNames;
class CSkyCatalogStars;
class CSkyCatalogDso;
class CSkyCatalogRadio;
class CSkyCatalogXGamma;
class CSkyCatalogSupernovas;
class CSkyCatalogBlackholes;
class CSkyCatalogMStars;
class CSkyCatalogExoplanets;
class CSkyCatalogAsteroids;
class CSkyCatalogComets;
class CSkyCatalogAes;
class CAstroImage;
class CConfigMain;
class CUnimapWorker;
class CUnimap;
class CGUIFrame;
//class CSkyConstellations;

class CSky  
{
// methods
public:
	CSky( );
	virtual ~CSky();

	void SetConfig(  CConfigMain* pConfig );
	void SetWorker(  CUnimapWorker* pWorker );
	void SetMainFrame( CGUIFrame* pFrame );
	void SetUnimap( CUnimap* pUnimap );

	bool HasGui();

	int InitCatalogs( );
	int GetCatalogIndexById( int nCatId );
	int GetCatalogIdByAbbreviation( const wxString& strCatAbbreviation );

	////////////////////////////
	// Projection handlers
	void ProjectStarAIT( StarDef& star, const double RaOrig, const double DecOrig );
	void ProjectStarCAR( StarDef& star, const double RaOrig, const double DecOrig );
	void ProjectStarARC( StarDef& star, const double RaOrig, const double DecOrig );
	void ProjectStarTAN( StarDef& star, const double RaOrig, const double DecOrig );
	void ProjectStarSTG( StarDef& star, const double RaOrig, const double DecOrig );
	void ProjectStarTAN2( StarDef& star, const double RaOrig, const double DecOrig );
	void ProjectStar( StarDef& star, const double x0, const double y0, int nMethod=-1 );
	// de-projection
	void DeprojectStarTAN( StarDef& star, const double x0, const double y0 );
	void DeprojectStarSTG( StarDef& star, const double x0, const double y0 );
	void DeprojectStar( StarDef& star, const double x0, const double y0, int nMethod=-1 );
	// other conversions
	void StoRect( float& nRA, float& nDec );
	void AltAztoRect( float& nAlt, float& nAz );

	/////////////////////////////////////////
	// system coord conversion
	void Convert_SkyCoord_B1950_to_J2000( double& ra, double& dec );

	/////////
	// transforms using match object
	void TransObj_SkyToImage( StarDef& star );
	int CalcPointTrans( double& x, double& y );
	void ProjectSkyCoordToImageXY( CAstroImage* pAstroImage, double nRa, double nDec, double& x, double& y );

	// loads...
	unsigned long LoadMatchCatalog( );
	unsigned long LoadMatchCatalogRegion( double nRa, double nDec, double nWidth, double nHeight );
	// downloads
	unsigned long DownloadCatalog( int nCatType, int nCatId, int nCatSrcId );

	// object finds
	long FindDsoObjectsInImage( CAstroImage* pAstroImage );
	long FindRadioSourcesInImage( CAstroImage* pAstroImage );
	long FindXGammaSourcesInImage( CAstroImage* pAstroImage );
	long FindSupernovasInImage( CAstroImage* pAstroImage );
	long FindBlackholesInImage( CAstroImage* pAstroImage );
	long FindMStarsInImage( CAstroImage* pAstroImage );
	long FindExoplanetsInImage( CAstroImage* pAstroImage );
	long FindAsteroidsInImage( CAstroImage* pAstroImage );
	long FindCometsInImage( CAstroImage* pAstroImage );
	long FindAesInImage( CAstroImage* pAstroImage );
	long FindSolarPlanetsInImage( CAstroImage* pAstroImage );

	int SolveAstroImage( CAstroImage* pAstroImage, CUnimapWorker* pUnimapWorker );

	// other sky calculations
	void CalcFOVByInstr( double& w, double& h, double focal, double swidth, double sheight );

//////////////////////
// DATA
public:
	CConfigMain* m_pConfig;

	CUnimapWorker*	m_pUnimapWorker;
	CUnimap*		m_pUnimap;
	CGUIFrame*		m_pFrame;
//	CLogger*		m_pLogger;

	////////////////////////////
	// :: NEW OBJ PLUG
	// sky finder
	CSkyFinder*		m_pSkyFinder;
	// constellations
	CSkyConstellations*	m_pConstellations;

	//////////////////
	// stars catalog object
	CSkyCatalogStars* m_pCatalogStars;
	// assoc
	CSkyCatalogStarsAssoc*	m_pCatalogStarsAssoc;
	// names
	CSkyCatalogStarNames*	m_pCatalogStarNames;

	///////////////
	// main DSO catalog object
	CSkyCatalogDso* m_pCatalogDso;
	// NGC dso
	CSkyCatalogDsoNgc* m_pCatalogDsoNgc;
	// IC dso
	CSkyCatalogDsoIc* m_pCatalogDsoIc;
	// messier
	CSkyCatalogDsoMessier* m_pCatalogDsoMessier;

	// assoc catalog to catalog 
	CSkyCatalogDsoAssoc*	m_pCatalogDsoAssoc;
	// dso names 
	CSkyCatalogDsoNames*	m_pCatalogDsoNames;

	/////////
	// other cats
	CSkyCatalogRadio* m_pCatalogRadio;
	CSkyCatalogXGamma* m_pCatalogXGamma;
	CSkyCatalogSupernovas* m_pCatalogSupernovas;
	CSkyCatalogBlackholes* m_pCatalogBlackholes;
	CSkyCatalogMStars* m_pCatalogMStars;
	CSkyCatalogExoplanets* m_pCatalogExoplanets;
	CSkyCatalogAsteroids* m_pCatalogAsteroids;
	CSkyCatalogComets* m_pCatalogComets;
	CSkyCatalogAes* m_pCatalogAes;

	/////////
	// var for catalog downloading
	// commented this out - use id/type/source from worker params
//	int m_nCatalogIdForDownloading;
//	int m_nCatalogTypeForDownloading;
//	int m_nCatalogSourceForDownloading;

	// vector with all catalogs
	CatalogDef m_vectCatalogDef[100];
	int m_nCatalogDef;

//	long   m_nMagLimitIdx;

//	unsigned int m_nSizeData;

	// check the buffer pointer
//	unsigned long m_nFirst;
//	unsigned long m_nLast;
//	unsigned long m_nCurrent;

	// other flags
	int	m_nDebugLevel;


	// matching vars - not used ?????
//	unsigned long	m_nCatObjMaxMatchNo;

};

#endif

