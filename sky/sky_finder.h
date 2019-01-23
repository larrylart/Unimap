
#ifndef _SKY_FINDER_H
#define _SKY_FINDER_H

// SEARCH/MATCH defaults
#define DEFAULT_SEARCH_AREA_INCREMENT_ON_RA			3.0
#define DEFAULT_SEARCH_AREA_INCREMENT_ON_DEC		3.0
#define DEFAULT_SEARCH_MATCH_OVERLAP_DIVISION_RA	5
#define DEFAULT_SEARCH_MATCH_OVERLAP_DIVISION_DEC	5
#define	DEFAULT_SEARCH_MATCH_MIN_CATALOG_STARS		8
#define DEFAULT_SEARCH_MATCH_FIRST_LEVEL_MIN_STARS	5
#define DEFAULT_SEARCH_MATCH_NO_OF_ZOOM_STEPS		10
#define DEFAULT_SEARCH_MATCH_ZOOM_LEVEL_START		0
#define DEFAULT_MATCH_FINETUNE_IMG_STARS_NO_FACTOR	0.6
#define DEFAULT_MATCH_FINETUNE_CAT_STARS_NO_FACTOR	2
#define DEFAULT_MATCH_ASSOC_MAX_DISTANCE			1.1
// proc 
#define DEFAULT_MATCH_STOP_ON_FIRST_FOUND_MIN_NEEDED	8
// search methods
#define SKY_SEARCH_METHOD_SWIPE					0
#define SKY_SEARCH_METHOD_SPIRAL				1

// MATCHING STATUS CODES
#define MATCH_STATE_NONE						0
#define MATCH_STATE_FOUND						1
#define MATCH_STATE_CANCELLED					2
// MATCHING STATUSES 
#define MATCH_STATE_NOT_FOUND					11
#define MATCH_STATE_SMALL_CATALOG				12
#define MATCH_STATE_FEW_IMAGE_STARS				13
#define MATCH_STATE_HAS_NO_HINT					14

// local includes
#include "star.h"

/////////////////////
// external classes
//class CLogger;
class CSky;
class CSkyCatalogStars;
class CStarMatch;
class CAstroImage;
class CConfigMain;
class CUnimapWorker;
class CUnimap;
class CGUIFrame;
class CWaitDialogMatch;

class CSkyFinder
{
// methods
public:
	CSkyFinder( CSky* pSky, CSkyCatalogStars* pCatalogStars );
	~CSkyFinder( );

	void SetConfig(  CConfigMain* pConfig );

	////////////////////////
	// some logic function for search/match
	int CheckMatchByNoOfStarsAndFit( long nStars, long nBestStars,
											double nFit, double nBestFit );
	// methods to match image agaisnt catalog
	long SearchAllSky( CAstroImage* pAstroImage );
	long SearchByHint( CAstroImage* pAstroImage );

	void SetBestMatchArea( CAstroImage* pAstroImage, double startRA, double endRA, 
							double startDEC, double endDEC, 
							double centerRa, double centerDec, double fieldWidth, 
							double fieldHeight, double fieldRadius,
							double originRA, double originDEC,
							StarDef* vectImgStars, long nImgStars,
							StarDef* vectCatStars, long nCatStars );

	// general call to area search
	int SearchArea( CAstroImage* pAstroImage,
									double nAreaCenterRA, double nAreaCenterDEC,
									double nAreaRadiusRA, double nAreaRadiusDEC,
									int nSearchZoomStart, int nSearchZoomSteps, 
									int nSkySearchMethod, int nCallLevel );
	// search a sky area by spiral method
	int AreaSpiralSearch( CAstroImage* pAstroImage, int nCallLevel );
	// search a sky area by swipe method
	int AreaSwipeSearch( CAstroImage* pAstroImage, int nCallLevel );

	// match image to a fixed sky area (+fine tune adjust)
	int MatchImageToSkyArea( CAstroImage* pAstroImage,
								StarDef* vectImgStars, long nImgStarsToExtract,
								StarDef* vectCatStars, long nCatStarsToExtract,
								double nCutRA, double nCutDEC,
								double nCutWidth, double nCutHeight );

	// recalculate first with more stars and then with projection
	int FineTuneMatch( CAstroImage* pAstroImage, 
						StarDef* vectImgStars, long& nImgStarCount,
						StarDef* vectCatStars, long& nCatStarCount,
						double nCenterRA, double nCenterDEC,
						double nRadiusRA, double nRadiusDEC, double& nFit, 
						double& startRA, double& endRA, double& startDEC, double& endDEC, 
						double& centerRa, double& centerDec, double& fieldWidth, 
						double& fieldHeight, double& fieldRadius, int bRecalcWithPrj=1 );

	////////////////
	// finders - should I keep here or ?
	long FindDso( CAstroImage* pAstroImage );
	long FindCatStars( CAstroImage* pAstroImage );
	long FindRadioSources( CAstroImage* pAstroImage );
	long FindXGammaSources( CAstroImage* pAstroImage );
	long FindSupernovas( CAstroImage* pAstroImage );
	long FindBlackholes( CAstroImage* pAstroImage );
	long FindMStars( CAstroImage* pAstroImage );
	long FindExoplanets( CAstroImage* pAstroImage );
	long FindAsteroids( CAstroImage* pAstroImage );
	long FindComets( CAstroImage* pAstroImage );
	long FindAes( CAstroImage* pAstroImage );
	long FindSolarPlanets( CAstroImage* pAstroImage );

	// other calc methods
	int CalcImgSkyProjection( CAstroImage* pAstroImage, double nCenterRA, double nCenterDEC,
								double& startRA, double& endRA, double& startDEC, double& endDEC,
								double& fieldWidth, double& fieldHeight, double& fieldRadius );
	int CalcNoMatchObjProjection( CAstroImage* pAstroImage );

////////////////////////////
// data
public:

	CSky* m_pSky;
	CConfigMain* m_pConfig;
	CWaitDialogMatch* m_pWaitDialogMatch;
	// catalog pass from main sky
	CSkyCatalogStars* m_pCatalogStars;

	CStarMatch*		m_pStarMatch;
	CUnimapWorker*	m_pUnimapWorker;
	CUnimap*		m_pUnimap;
	CGUIFrame*		m_pFrame;

	/////////////////////////////////////
	// AREA SEARCH VARS AND FLAGS - to go in class search
	double m_nSearchAreaCenterRA;
	double m_nSearchAreaCenterDEC;
	double m_nSearchAreaRadiusRA;
	double m_nSearchAreaRadiusDEC;
	double m_nSearchAreaStartRA;
	double m_nSearchAreaStartDEC;
	double m_nSearchAreaEndRA;
	double m_nSearchAreaEndDEC;
	double m_nSearchAreaWidth;
	double m_nSearchAreaHeight;
	int m_nSearchAreaZoomStart;
	int m_nSearchAreaZoomSteps;
	// counters 
	int m_nSearchBestMatch;
	int m_nSearchFirstBestMatch;
	double m_nSearchFirstFoundProjectionScale;
	double m_nSearchFoundProjectionScale;
	double m_nSearchFirstCenterRA;
	double m_nSearchFirstCenterDEC;
	double m_nSearchFirstRadiusRA;
	double m_nSearchFirstRadiusDEC;
	double m_nSearchBestFit;
	// flags
	unsigned char m_bSearcLimitRightRA;
	unsigned char m_bSearcLimitLeftRA;
	unsigned char m_bSearchLimitUpDEC;
	unsigned char m_bSearchLimitDownDEC;

	// this is the origin in the sky where i found my image
	double m_nWinOrigX;
	double m_nWinOrigY;
	double m_nProjectionScale;

	// status of matching process
	int m_bMatchStatus;

	long m_nExtractedImgStars;
	long m_nExtractedCatStars;
};

#endif

