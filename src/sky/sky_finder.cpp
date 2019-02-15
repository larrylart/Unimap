////////////////////////////////////////////////////////////////////
// SkyFinder version 1.2
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// system libs
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// math libs
#include <vector>
#include <list>
#include <limits>
#include <algorithm>
#include <functional>
#include <numeric>

// local headers
#include "../util/func.h"
#include "../util/geometry.h"
#include "libastro/libastro.h"
#include "sky.h"
#include "catalog_stars.h"
#include "catalog_dso_ngc.h"
#include "catalog_dso_ic.h"
#include "catalog_dso_assoc.h"
#include "../match/starmatch.h"
#include "../image/astroimage.h"
#include "../config/mainconfig.h"
#include "../unimap_worker.h"
#include "../unimap.h"
#include "../gui/frame.h"
#include "../gui/waitdialogmatch.h"
// other cats
#include "catalog_radio.h"
#include "catalog_supernovas.h"
#include "catalog_mstars.h"
#include "catalog_asteroids.h"
#include "catalog_aes.h"
#include "solar_planets.h"

// main header
#include "sky_finder.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyFinder
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyFinder::CSkyFinder( CSky* pSky, CSkyCatalogStars* pCatalogStars )
{
	m_pSky = pSky;
	m_pCatalogStars = pCatalogStars;

	m_pUnimapWorker = NULL;
	m_pWaitDialogMatch = NULL;

	m_nProjectionScale  = 10.0;
	m_bMatchStatus = MATCH_STATE_NONE;

	//////////////////////////////////////
	// default seach/matching params
	m_nSearchAreaCenterRA = 0.0;
	m_nSearchAreaCenterDEC = 0.0;
	m_nSearchAreaRadiusRA = 0.0;
	m_nSearchAreaRadiusDEC = 0.0;
	m_nSearchAreaStartRA = 0.0;
	m_nSearchAreaStartDEC = 0.0;
	m_nSearchAreaEndRA = 0.0;
	m_nSearchAreaEndDEC = 0.0;
	m_nSearchAreaWidth = 0.0;
	m_nSearchAreaHeight = 0.0;
	m_nSearchAreaZoomStart = 0;
	m_nSearchAreaZoomSteps = 0;
	// counters 
	m_nSearchBestMatch = 0;
	m_nSearchFirstBestMatch = 0;
	m_nSearchFirstFoundProjectionScale = 0.0;
	m_nSearchFoundProjectionScale = 0.0;
	m_nSearchFirstCenterRA = 0.0;
	m_nSearchFirstCenterDEC = 0.0;
	m_nSearchFirstRadiusRA = 0.0;
	m_nSearchFirstRadiusDEC = 0.0;
	m_nSearchBestFit = 0.0;
	// flags
	m_bSearcLimitRightRA = 0.0;
	m_bSearcLimitLeftRA = 0;
	m_bSearchLimitUpDEC = 0;
	m_bSearchLimitDownDEC = 0;

	// this is the origin in the sky where i found my image
	m_nWinOrigX = 0.0;
	m_nWinOrigY = 0.0;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyFinder
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyFinder::~CSkyFinder()
{

}

/////////////////////////////////////////////////////////////////////
// Method:	SetConfig
// Class:	CSkyFinder
// Purpose:	set the object with current config values
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSkyFinder::SetConfig(  CConfigMain* pConfig )
{
	m_pConfig = pConfig;
	// set other remote configs

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	CheckMatchByNoOfStarsAndFit
// Class:	CSkyFinder
// Purpose:	check if stars ver. fit beased on config rules
// Input:	no of stars and best, fit and best
// Output:	if better or not
/////////////////////////////////////////////////////////////////////
int CSkyFinder::CheckMatchByNoOfStarsAndFit( long nStars, long nBestStars,
											double nFit, double nBestFit )
{
	double nMaxDecentFit = 100.0;
	long nMinStarsNeeded = 11;

	// check by no of stars
	if( m_pConfig->m_bMatchCheckByNoOfStars )
	{
		if( nStars >= nBestStars )
		{
			// check if also by fit
			if( m_pConfig->m_bMatchCheckByBestFit )
			{
				if( nFit > 0.0 && nFit < nMaxDecentFit && nFit <= nBestFit ) 
					return( 1 );
				else
					return( 0 );
			} else
			{
				// if not return - got my best = 1 :)
				return( 1 );
			}
		} else
		{
			// no matter what else if not rule by max no is a failure
			return( 0 );
		}

	} else
	{
		// now again check if I got a fit
		if( m_pConfig->m_bMatchCheckByBestFit )
		{
			if( nFit > 0.0 && nFit < nMaxDecentFit && nFit <= nBestFit ) 
				return( 1 );
			else
				return( 0 );
		} else
		{
			// if not by fit either then I just do return ok 
			if( nStars >= nMinStarsNeeded ) 
				return( 1 );
			else
				return( 0 );
		}
	}

	return( 0 );
}

/////////////////////////////////////////////////////////////////////
// Method:	SearchAllSky
// Class:	CSkyFinder
// Purpose:	search stars in the entire sky (when no hint is given)
// Input:	pointer to star verctor
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CSkyFinder::SearchAllSky( CAstroImage* pAstroImage )
{
	char strLog[500];
	int nFoundStatus=0;

	// first we estimate de max area where this search could be done
	double nAreaWidth = 360.0;
	double nAreaHeight = 180.0;
	// also calculate radius
	double nAreaRadiusRA = 180.0;
	double nAreaRadiusDEC = 90.0;
	// field center
	double nAreaCenterRA = 180.0;
	double nAreaCenterDEC = 0.0;
	// field start
	double nAreaStartRA = nAreaCenterRA - nAreaRadiusRA;
	double nAreaStartDEC = nAreaCenterDEC - nAreaRadiusDEC;
	// field end
	double nAreaEndRA = nAreaCenterRA + nAreaRadiusRA;
	double nAreadEndDEC = nAreaCenterDEC + nAreaRadiusDEC;

	///////////////////////////////////////////////////////////
	// :: PREPARE/EXTRCT SKY IMAGE DATA FOR GUI WAIT DIALOG
//	CWaitDialogMatch* m_pWaitDialogMatch = m_pFrame->m_m_pWaitDialogMatch;
	// here we set the wait dialog
	if( m_pWaitDialogMatch ) m_pWaitDialogMatch->SetField( nAreaStartRA, nAreaStartDEC, nAreaWidth, nAreaHeight );
	// now we set the objects - first do a temp cut
	///////////////////////////////////////
	// cut window from catalog
	StarDef* vectTempCatWindow = NULL;
	double nMinX, nMaxX, nMinY, nMaxY;
	int nGuiCatStarsToExtract = WAIT_DIALOG_USE_NO_OF_CATALOG_STARS;
	// allocate initial size
	vectTempCatWindow = (StarDef*) malloc( (nGuiCatStarsToExtract+1)*sizeof(StarDef) );
	memset( vectTempCatWindow, 0, (nGuiCatStarsToExtract+1)*sizeof(StarDef) );
	int nGuiCatStarsExtracted = m_pCatalogStars->ExtractStarsFromCatalog( vectTempCatWindow, nGuiCatStarsToExtract,
											pAstroImage->m_nWidth, pAstroImage->m_nHeight,
											nMinX, nMaxX, nMinY, nMaxY, SKY_PROJECTION_TYPE_CAR );
	// calc scale
	double nWinScale = (double) (pAstroImage->m_nWidth/(nMaxX-nMinX));
	// set my object
	if( m_pWaitDialogMatch ) m_pWaitDialogMatch->SetObjects( vectTempCatWindow, nGuiCatStarsExtracted, nMinX, nMaxX, nMinY, nMaxY, nWinScale );
	// free object
	free( vectTempCatWindow );
	vectTempCatWindow = NULL;
	// also set image area
	if( m_pWaitDialogMatch ) m_pWaitDialogMatch->SetImageAreaFactor( m_pConfig->m_nSearchAreaIncrementOnRA, m_pConfig->m_nSearchAreaIncrementOnDEC );
	// call for update
	if( m_pUnimapWorker && m_pWaitDialogMatch ) m_pUnimapWorker->SetWaitDialogUpdate( );
	// END WAIT DIALOG GUI PREPARE
	////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////
	// Call to area search force method swipe ? or use config m_nSkySearchMethod
	nFoundStatus = SearchArea( pAstroImage, nAreaCenterRA, nAreaCenterDEC,
						nAreaRadiusRA, nAreaRadiusDEC, 3, 
						m_pConfig->m_nSearchZoomSteps, SKY_SEARCH_METHOD_SWIPE, 0 );
	// set flag if not found
	if( !nFoundStatus ) m_bMatchStatus = MATCH_STATE_HAS_NO_HINT;

	return( nFoundStatus );
}

/////////////////////////////////////////////////////////////////////
// Method:	SearchByHint
// Class:	CSkyFinder
// Purpose:	search stars using the image hint
// Input:	pointer to star verctor and star count
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CSkyFinder::SearchByHint( CAstroImage* pAstroImage )
{
	char strLog[500];
	int nFoundStatus=0;

	// extract hint
	double nHintCenterRa=0, nHintCenterDec=0, nHintWidth=0, nHintHeight=0;
	pAstroImage->GetHint( nHintCenterRa, nHintCenterDec, nHintWidth, nHintHeight );

	// first we estimate de max area where this search could be done
	double nAreaWidth = (double) nHintWidth * m_pConfig->m_nSearchAreaIncrementOnRA;
	double nAreaHeight = (double) nHintHeight * m_pConfig->m_nSearchAreaIncrementOnDEC;
	// also calculate radius
	double nAreaRadiusRA = (double) nAreaWidth/2.0;
	double nAreaRadiusDEC = (double) nAreaHeight/2.0;
	// field center - use orig m_nRelCenterRa, m_nRelCenterDec;
	double nAreaCenterRA = nHintCenterRa;
	double nAreaCenterDEC = nHintCenterDec;
	// field start
	double nAreaStartRA = nAreaCenterRA - nAreaRadiusRA;
	double nAreaStartDEC = nAreaCenterDEC - nAreaRadiusDEC;
	// field end
	double nAreaEndRA = nAreaCenterRA + nAreaRadiusRA;
	double nAreadEndDEC = nAreaCenterDEC + nAreaRadiusDEC;
	// calculate area radius
	double nAreaRadius = CalcSkyDistance( nAreaCenterRA, nAreaCenterDEC, nAreaStartRA, nAreaStartDEC );

	///////////////////////////////////////////////////////////
	// :: PREPARE/EXTRCT SKY IMAGE DATA FOR GUI WAIT DIALOG
//	CWaitDialogMatch* m_pWaitDialogMatch = m_pFrame->m_m_pWaitDialogMatch;
	// here we set the wait dialog
	if( m_pWaitDialogMatch ) 
	{
		m_pWaitDialogMatch->SetField( nAreaStartRA, nAreaStartDEC, nAreaWidth, nAreaHeight );
		// now we set the objects - first do a temp cut
		///////////////////////////////////////
		// cut window from catalog
		StarDef* vectTempCatWindow = NULL;
		double nMinX=0.0, nMaxX=0.0, nMinY=0.0, nMaxY=0.0;
		int nGuiCatStarsToExtract = WAIT_DIALOG_USE_NO_OF_CATALOG_STARS;
		// allocate initial size
		vectTempCatWindow = (StarDef*) malloc( (nGuiCatStarsToExtract+1)*sizeof(StarDef) );
		memset( vectTempCatWindow, 0, (nGuiCatStarsToExtract+1)*sizeof(StarDef) );
		int nGuiCatStarsExtracted = m_pCatalogStars->CutAreaFromCatalog( nAreaCenterRA, nAreaCenterDEC, nAreaRadius,
												vectTempCatWindow, nGuiCatStarsToExtract,
												pAstroImage->m_nWidth, pAstroImage->m_nHeight,
												nMinX, nMaxX, nMinY, nMaxY );
		// calc scale
		double nWinScale = (double) (pAstroImage->m_nWidth/(nMaxX-nMinX));
		// set my object
		m_pWaitDialogMatch->SetObjects( vectTempCatWindow, nGuiCatStarsExtracted, nMinX, nMaxX, nMinY, nMaxY, nWinScale );
		// free object
		free( vectTempCatWindow );
		vectTempCatWindow = NULL;
		// also set image area
		m_pWaitDialogMatch->SetImageAreaFactor( m_pConfig->m_nSearchAreaIncrementOnRA, m_pConfig->m_nSearchAreaIncrementOnDEC );
		// call for update
		m_pUnimapWorker->SetWaitDialogUpdate( );
	}
	// END WAIT DIALOG GUI PREPARE
	////////////////////////////////////////////////////

	//////////////////////////////////////////////////
	// Call to area search by method - etc
	nFoundStatus=SearchArea( pAstroImage, nAreaCenterRA, nAreaCenterDEC,
						nAreaRadiusRA, nAreaRadiusDEC, m_pConfig->m_nSearchZoomStart, 
						m_pConfig->m_nSearchZoomSteps, m_pConfig->m_nSkySearchMethod, 0 );

	return( nFoundStatus );
}

/////////////////////////////////////////////////////////////////////
// Method:	SearchArea
// Class:	CSkyFinder
// Purpose:	search an area in the sky
// Input:	pointer to astro image, area center, width height, step
//			and search method to use
// Output:	best found
/////////////////////////////////////////////////////////////////////
int CSkyFinder::SearchArea( CAstroImage* pAstroImage,
									double nAreaCenterRA, double nAreaCenterDEC,
									double nAreaRadiusRA, double nAreaRadiusDEC,
									int nSearchZoomStart, int nSearchZoomSteps, 
									int nSkySearchMethod, int nCallLevel )
{
	int nFoundStats = 0;

	// set area coordinates
	m_nSearchAreaCenterRA = nAreaCenterRA;
	m_nSearchAreaCenterDEC = nAreaCenterDEC;
	m_nSearchAreaRadiusRA = nAreaRadiusRA;
	m_nSearchAreaRadiusDEC = nAreaRadiusDEC;
	m_nSearchAreaStartRA = nAreaCenterRA-nAreaRadiusRA;
	m_nSearchAreaStartDEC = nAreaCenterDEC-nAreaRadiusDEC;
	m_nSearchAreaEndRA = nAreaCenterRA+nAreaRadiusRA;
	m_nSearchAreaEndDEC = nAreaCenterDEC+nAreaRadiusDEC;
	m_nSearchAreaWidth = (double) nAreaRadiusRA * 2.0;
	m_nSearchAreaHeight = (double) nAreaRadiusDEC * 2.0;
	m_nSearchAreaZoomStart = nSearchZoomStart;
	m_nSearchAreaZoomSteps = nSearchZoomSteps;
	//init search vars
	m_nSearchBestMatch = -1;
	m_nSearchFirstBestMatch = -1;
	m_nSearchFirstFoundProjectionScale = 10;
	m_nSearchFoundProjectionScale = 10;
	m_nSearchFirstCenterRA=0;
	m_nSearchFirstCenterDEC=0;
	m_nSearchFirstRadiusRA=0;
	m_nSearchFirstRadiusDEC=0;
	m_nSearchBestFit = DBL_MAX;

	// Call to area search by method
	if( nSkySearchMethod == SKY_SEARCH_METHOD_SWIPE )
	{
		// search by swiping the area
		nFoundStats = AreaSwipeSearch( pAstroImage, nCallLevel );

	} else if( nSkySearchMethod == SKY_SEARCH_METHOD_SPIRAL )
	{
		// search area in spiral
		nFoundStats = AreaSpiralSearch( pAstroImage, nCallLevel );

	} else
	{
		// by default do area swipe
		nFoundStats = AreaSwipeSearch( pAstroImage, nCallLevel );
	}

	///////////////////
	// adjust other params from star catalog - like name/etc
	pAstroImage->AdjustStarDetailsFromCatalog( m_pCatalogStars );

	return( nFoundStats );
}

/////////////////////////////////////////////////////////////////////
// Method:	AreaSpiralSearch
// Class:	CSkyFinder
// Purpose:	search an area in spiral on RA/DEC
// Input:	pointer to astro image, area center, width height, step
//			and call level (used for recursive calls)
// Output:	best found
/////////////////////////////////////////////////////////////////////
int CSkyFinder::AreaSpiralSearch( CAstroImage* pAstroImage, int nCallLevel )
{
	int nFoundStats = 0;
	wxString strLog;
	wxString strMsg;
//	double nMinX, nMaxX, nMinY, nMaxY;
	double startRA=0, endRA=0, startDEC=0, endDEC=0;
	double centerRa=0, centerDec=0, fieldWidth=0, fieldHeight=0, fieldRadius=0;
	int bSetToBreakLoop = 0;
	// fine tune props
	long nImgFinetuneStars = 0;
	long nCatFinetuneStars = 0;
	m_bMatchStatus = MATCH_STATE_NONE;

	// get pointer to wait dialog
//	CWaitDialogMatch* m_pWaitDialogMatch = m_pFrame->m_m_pWaitDialogMatch;

	// local vars
	int nZoomStep = 0;
	// image and catalog stars vectors dimentinons
	long nImgStarsExtracted = 0;
	long nImgStarsToExtract = m_pConfig->m_nScanMatchImgObjNo;
	long nImgStarsMax = (long) pAstroImage->m_nStar;
	if( nImgStarsToExtract > nImgStarsMax ) nImgStarsToExtract = nImgStarsMax;

	long nCatStarsExtracted = 0;
	long nCatStarsToExtract = m_pConfig->m_nScanMatchCatObjNo;
	long nCatStarsMax = (long) nImgStarsMax * m_pConfig->m_nMatchFineTuneCatStarsFactor;
	if( nCatStarsToExtract > nCatStarsMax ) nCatStarsToExtract = nCatStarsMax;

	// star vectors used
	StarDef* vectImgStars = NULL;
	StarDef* vectCatStars = NULL;

	// allocate image stars
	vectImgStars = (StarDef*) malloc( (nImgStarsMax+1)*sizeof(StarDef) );
	memset( vectImgStars, 0, (nImgStarsMax+1)*sizeof(StarDef) );
	// allocate catalog stars buffer size
	vectCatStars = (StarDef*) malloc( (nCatStarsMax+1)*sizeof(StarDef) );
	memset( vectCatStars, 0, (nCatStarsMax+1)*sizeof(StarDef) );

	// initialize best fit
	double nCurFit = DBL_MAX;

	// set max match to zero
	m_pStarMatch->m_nMaxMatch = 0;

	///////////////////////////////////////
	// ZOOM :: process by flag reverse/normal
	int nZoomInc=1, nSearchAreaZoomStart=0, nZoomLastStep=0;
	if( m_pConfig->m_bDoReverseZoomSearch )
	{
		nSearchAreaZoomStart = nSearchAreaZoomStart+m_nSearchAreaZoomSteps-1;
		if( nSearchAreaZoomStart < 0 ) nSearchAreaZoomStart=0;
		nZoomLastStep = m_nSearchAreaZoomStart;
		nZoomInc=-1;
	} else
	{
		nSearchAreaZoomStart = m_nSearchAreaZoomStart;
		nZoomLastStep = nSearchAreaZoomStart+m_nSearchAreaZoomSteps;
		nZoomInc=1;
	}

	// match analysis variables
	long nZLAvgCatStarsVal=0;
	long nZLAvgCatStars=0;
	long nALLAvgCatStarsVal=0;
	long nALLAvgCatStars=0;

	/////////////////////////////////////////////////////////////
	// FOR EACH ZOOM LEVEL
	int nZoomCount = 0;
	for( nZoomStep=nSearchAreaZoomStart; (!m_pConfig->m_bDoReverseZoomSearch && nZoomStep<nZoomLastStep ) || 
		(m_pConfig->m_bDoReverseZoomSearch && nZoomStep >nZoomLastStep) && !bSetToBreakLoop; nZoomStep+=nZoomInc )
	{
		nZLAvgCatStars=0;
		nZLAvgCatStarsVal = 0;

		///////////////////////////////
		// loop status check
		if( m_pWaitDialogMatch )
		{
			if( m_pWaitDialogMatch->m_bActionStatus == WAIT_DIALOG_MATCH_STATUS_STOP )
			{
				m_pWaitDialogMatch->m_bActionStatus = 0;
				break;

			} else if( m_pWaitDialogMatch->m_bActionStatus == WAIT_DIALOG_MATCH_STATUS_CANCEL )
			{
				break;
			}

			// set status
			strMsg.Printf( wxT("Do matching at zoom level %d ..."), nZoomStep );
			m_pUnimapWorker->SetWaitDialogMsg( strMsg );
			// now we use origin end values to calculate the cuts
			strLog.Printf( wxT("INFO :SS: zoom in at level=%d"), nZoomStep );
			m_pUnimapWorker->Log( strLog );
		}

		// how may sub division per x/y
		int nCutRatRA = 1+nZoomStep;
		int nCutRatDEC = 1+nZoomStep;

		// calculate my search width/height based on the zoom factor
		double nCutWidth = (double) (m_nSearchAreaWidth/nCutRatRA);
		double nCutHeight = (double) (m_nSearchAreaHeight/nCutRatDEC);
		// radius on ra/dec
		double nCutRadiusRA = (double) nCutWidth/2.0;
		double nCutRadiusDEC = (double) nCutHeight/2.0;
		// and also calculate the overlaping sub-division
		double nCutRAStep = (double) nCutWidth/m_pConfig->m_nRaSearchOverlapDivision;
		double nCutDECStep = (double) nCutHeight/m_pConfig->m_nDecSearchOverlapDivision;

		// calculate the center cut coord
		double nMidCutStartRA = m_nSearchAreaCenterRA - nCutRadiusRA;
		double nMidCutStartDEC = m_nSearchAreaCenterDEC - nCutRadiusDEC;

		/////////////////////////////////////////////////////////////
		// :: MATCH CENTRAL PIECE
		if( MatchImageToSkyArea( pAstroImage, 
								vectImgStars, nImgStarsToExtract,
								vectCatStars, nCatStarsToExtract,
								nMidCutStartRA, nMidCutStartDEC, 
								nCutWidth, nCutHeight ) == 5 )
		{
			bSetToBreakLoop = 1;
			break;
		}
		// prepare average
		nZLAvgCatStarsVal += m_nExtractedCatStars;
		nZLAvgCatStars++;
		nALLAvgCatStarsVal += m_nExtractedCatStars;
		nALLAvgCatStars++;
		/////////////////////////////////////////////////////////////
		// check catalog average extracted
		double pext = (double) nCatStarsToExtract / (double) m_nExtractedCatStars;
		if( m_nExtractedCatStars < 7 )
		{
			m_bMatchStatus = MATCH_STATE_SMALL_CATALOG;
			break;
		}
		/////////////////////////////////////////////////////////////

		// init search limit flags
		m_bSearcLimitRightRA = 0;
		m_bSearcLimitLeftRA = 0;
		m_bSearchLimitUpDEC = 0;
		m_bSearchLimitDownDEC = 0;
		double nLimitFact = 0;

		int nSpiralStep = 0;
		int bStopSpiral = 0;
		///////////////////////////////////////////////////////////////////
		// :: SPIRAL SEARCH
		for( nSpiralStep=1; bStopSpiral == 0 && !bSetToBreakLoop; nSpiralStep++ )
		{
			/////////////////
			// check for skip
			if( m_pWaitDialogMatch )
			{
				if( m_pWaitDialogMatch->m_bActionStatus == WAIT_DIALOG_MATCH_STATUS_SKIP )
				{
					m_pWaitDialogMatch->m_bActionStatus = 0;
					break;

				} else if( m_pWaitDialogMatch->m_bActionStatus == WAIT_DIALOG_MATCH_STATUS_STOP ||
							m_pWaitDialogMatch->m_bActionStatus == WAIT_DIALOG_MATCH_STATUS_CANCEL )
				{
					break;
				}
			}

			int nSpiralStepRight = nSpiralStep,
				nSpiralStepLeft	= nSpiralStep,
				nSpiralStepUp = nSpiralStep,
				nSpiralStepDown = nSpiralStep;
			// other adjustments
			double nSpiralAdjustRightRa = 0;
			double nSpiralAdjustLeftRa = 0;
			double nSpiralAdjustUpDec = 0;
			double nSpiralAdjustDownDec = 0;
			// then increment origin with divx and do dec from orig-divy to orig+divy
			double nCutStartRA = nMidCutStartRA + nSpiralStep*nCutRAStep;
			double nCutStartDec = nMidCutStartDEC - nSpiralStep*nCutDECStep;

			// check limit on RA  right/left
			if( nMidCutStartRA + nSpiralStep*nCutRAStep + nCutWidth > m_nSearchAreaEndRA )
			{
				//nSpiralStepDown--;
				nSpiralAdjustRightRa = m_nSearchAreaEndRA - (nMidCutStartRA + nSpiralStep*nCutRAStep + nCutWidth);
				m_bSearcLimitRightRA = 1;
			}
			if( nMidCutStartRA - nSpiralStep*nCutRAStep < m_nSearchAreaStartRA )
			{
				//nSpiralStepUp--;
				nSpiralAdjustLeftRa = m_nSearchAreaStartRA - (nMidCutStartRA - nSpiralStep*nCutRAStep);
				m_bSearcLimitLeftRA = 1;
			}
			// check limit on DEC up/down
			if( nMidCutStartDEC + nSpiralStep*nCutDECStep + nCutHeight > m_nSearchAreaEndDEC )
			{
				//nSpiralStepRight--;
				nSpiralAdjustUpDec = m_nSearchAreaEndDEC - (nMidCutStartDEC + nSpiralStep*nCutDECStep + nCutHeight);
				m_bSearchLimitUpDEC = 1; 
			}
			if( nMidCutStartDEC - nSpiralStep*nCutDECStep < m_nSearchAreaStartDEC )
			{
				// nSpiralStepLeft--;
				nSpiralAdjustDownDec = m_nSearchAreaStartDEC - (nMidCutStartDEC - nSpiralStep*nCutDECStep);
				m_bSearchLimitDownDEC = 1;
			}

			// if all flags true break loop
			if( m_bSearcLimitRightRA && m_bSearcLimitLeftRA && m_bSearchLimitUpDEC && m_bSearchLimitDownDEC ) 
			{
				bStopSpiral = 1;
//				break;
			}

			// if not over the limit at the right
//			if( !m_bSearcLimitRightRA )
//			{
				// set limit factor
				nLimitFact = nMidCutStartDEC + nSpiralStepRight*nCutDECStep+nSpiralAdjustUpDec;
				////////////////////////////
				// RIGHT :: for y-dy -> y+dy
				nCutStartRA = nMidCutStartRA + nSpiralStep*nCutRAStep + nSpiralAdjustRightRa;
				// for( nCutStartDec=nCutStartDec; nCutStartDec <= nLimitFact && !m_bSearcLimitRightRA; nCutStartDec+=nCutDECStep )
				for( nCutStartDec=nMidCutStartDEC - nSpiralStep*nCutDECStep+nSpiralAdjustDownDec; 
						nCutStartDec <= nLimitFact && !bSetToBreakLoop; nCutStartDec+=nCutDECStep )
				{
					// do match 
					if( MatchImageToSkyArea( pAstroImage, 
											vectImgStars, nImgStarsToExtract,
											vectCatStars, nCatStarsToExtract,
											nCutStartRA, nCutStartDec, 
											nCutWidth, nCutHeight ) == 5 )
					{
						bSetToBreakLoop = 1;
						break;
					}
					// prepare average
					nZLAvgCatStarsVal += m_nExtractedCatStars;
					nZLAvgCatStars++;
					nALLAvgCatStarsVal += m_nExtractedCatStars;
					nALLAvgCatStars++;
				}
//			}

			// if not over the limit at the top
//			if( !m_bSearchLimitUpDEC )
//			{
				// set limit factor
				nLimitFact = nMidCutStartRA - nSpiralStepUp*nCutRAStep+nSpiralAdjustLeftRa;
				////////////////////////////
				// UP/TOP :: for x+dx -> x-dx
				nCutStartDec = nMidCutStartDEC + nSpiralStepRight*nCutDECStep+nSpiralAdjustUpDec;
				for( nCutStartRA=nMidCutStartRA + nSpiralStep*nCutRAStep + nSpiralAdjustRightRa; 
						nCutStartRA >= nLimitFact && !bSetToBreakLoop; nCutStartRA-=nCutRAStep )
				{
					// do match 
					if( MatchImageToSkyArea( pAstroImage, 
											vectImgStars, nImgStarsToExtract,
											vectCatStars, nCatStarsToExtract,
											nCutStartRA, nCutStartDec, 
											nCutWidth, nCutHeight ) == 5 )
					{
						bSetToBreakLoop = 1;
						break;
					}
					// prepare average
					nZLAvgCatStarsVal += m_nExtractedCatStars;
					nZLAvgCatStars++;
					nALLAvgCatStarsVal += m_nExtractedCatStars;
					nALLAvgCatStars++;
				}
//			}

			// if not over the limit at the left
//			if( !m_bSearcLimitLeftRA )
//			{
				// set limit factor
				nLimitFact = nMidCutStartDEC - nSpiralStepLeft*nCutDECStep+nSpiralAdjustDownDec;
				////////////////////////////
				// LEFT :: for y+dy -> y-dy
				nCutStartRA = nMidCutStartRA - nSpiralStepUp*nCutRAStep+nSpiralAdjustLeftRa;
				for( nCutStartDec=nMidCutStartDEC + nSpiralStepRight*nCutDECStep+nSpiralAdjustUpDec; 
						nCutStartDec >= nLimitFact && !bSetToBreakLoop; nCutStartDec-=nCutDECStep )
				{
					// do match 
					if( MatchImageToSkyArea( pAstroImage, 
											vectImgStars, nImgStarsToExtract,
											vectCatStars, nCatStarsToExtract,
											nCutStartRA, nCutStartDec, 
											nCutWidth, nCutHeight ) == 5 )
					{
						bSetToBreakLoop = 1;
						break;
					}
					// prepare average
					nZLAvgCatStarsVal += m_nExtractedCatStars;
					nZLAvgCatStars++;
					nALLAvgCatStarsVal += m_nExtractedCatStars;
					nALLAvgCatStars++;
				}
//			}

			// if not over the limit at the bottom
//			if( !m_bSearchLimitDownDEC )
//			{
				// set limit factor
				nLimitFact = nMidCutStartRA + nSpiralStepDown*nCutRAStep+nSpiralAdjustRightRa;
				////////////////////////////
				// BOTTOM/DOWN :: for x-dx -> x+dx
				nCutStartDec = nMidCutStartDEC - nSpiralStepLeft*nCutDECStep+nSpiralAdjustDownDec;
				for( nCutStartRA=nMidCutStartRA - nSpiralStepUp*nCutRAStep+nSpiralAdjustLeftRa; 
						nCutStartRA <= nLimitFact && !bSetToBreakLoop; nCutStartRA+=nCutRAStep )
				{
					// do match 
					if( MatchImageToSkyArea( pAstroImage, 
											vectImgStars, nImgStarsToExtract,
											vectCatStars, nCatStarsToExtract,
											nCutStartRA, nCutStartDec, 
											nCutWidth, nCutHeight ) == 5 )
					{
						bSetToBreakLoop = 1;
						break;
					}
					// prepare average
					nZLAvgCatStarsVal += m_nExtractedCatStars;
					nZLAvgCatStars++;
					nALLAvgCatStarsVal += m_nExtractedCatStars;
					nALLAvgCatStars++;
				}
//			}

			if( bSetToBreakLoop ) break;
		}
		// calculate and check average per this zoom search
		if( nZLAvgCatStars != 0 && nZLAvgCatStarsVal != 0 && nZoomStep<nSearchAreaZoomStart && nZoomCount < 5 )
		{
			nZLAvgCatStarsVal = nZLAvgCatStarsVal/nZLAvgCatStars;
			if( nZLAvgCatStarsVal < 7 )
			{
				m_bMatchStatus = MATCH_STATE_SMALL_CATALOG;
				break;
			}
		}		
		// End of SPIRAL SEARCH
		/////////////////////////////////////////////////////////
		if( bSetToBreakLoop ) break;

		nZoomCount++;
	}
	// End of Zoom Search
	///////////////////////////////////////////////////////

	////////////////
	// check for small catalog status
	if( m_bMatchStatus == MATCH_STATE_SMALL_CATALOG )
	{
		if( m_pWaitDialogMatch ) m_pWaitDialogMatch->m_bActionStatus = 0;
		pAstroImage->ClearMatched( );

		// free stars vectors
		free( vectCatStars );
		free( vectImgStars );

		return( 0 );
	}

	///////////////////////////////////////////
	// check for cancel status
	int bActionStatus = 0;
	if( m_pWaitDialogMatch ) bActionStatus = m_pWaitDialogMatch->m_bActionStatus;
	if( bActionStatus != WAIT_DIALOG_MATCH_STATUS_CANCEL )
	{
		// if no best match found use the first match
		if( m_nSearchBestMatch <=0 )
		{
			// if at least there is a valid first best match
			if( m_nSearchFirstBestMatch > 0 )
			{
				m_pStarMatch->m_nMaxMatch = m_nSearchFirstBestMatch;
				// do fine tune without recalculation on projection
				if( FineTuneMatch( pAstroImage,
									vectImgStars, nImgFinetuneStars,
									vectCatStars, nCatFinetuneStars,
									m_nSearchFirstCenterRA, m_nSearchFirstCenterDEC,
									m_nSearchFirstRadiusRA, m_nSearchFirstRadiusDEC, nCurFit,
									startRA, endRA, startDEC, endDEC , 
									centerRa, centerDec, fieldWidth, fieldHeight, fieldRadius,
									0 ) )
				{
					m_nSearchBestMatch = m_pStarMatch->m_nMaxMatch;
					// set GUI dialog data
					if( m_pWaitDialogMatch ) m_pWaitDialogMatch->SetBestMatch( m_nSearchFirstBestMatch, m_pStarMatch->m_nMaxMatch );

					// now set best area to the image
					SetBestMatchArea( pAstroImage, startRA, endRA, startDEC, endDEC,
										centerRa, centerDec, fieldWidth, fieldHeight, fieldRadius,
										m_nSearchFirstCenterRA-m_nSearchFirstRadiusRA, 
										m_nSearchFirstCenterDEC-m_nSearchFirstRadiusDEC,
										vectImgStars, nImgFinetuneStars,
										vectCatStars, nCatFinetuneStars );

				} else
				{
					nFoundStats = 0;
					pAstroImage->ClearMatched( );
					m_bMatchStatus = MATCH_STATE_NOT_FOUND;
				}

			} else
			{
				nFoundStats = 0;
				pAstroImage->ClearMatched( );
				m_bMatchStatus = MATCH_STATE_NOT_FOUND;
			}

		} 
		
		//////////////////////
		// now if found 
		if( m_nSearchBestMatch > 0 )
		{
			// check if to to do fine area check
			if( m_pConfig->m_bDoMatchRefineBestArea && nCallLevel == 0 )
			{
				/////////////////////////////
				// FINE SWIPE AREA - here is where I call area swipe again
				// AreaSwipeMatch( pAstroImage, nCutCenterRA,nCutCenterDEC,
				//						nCutRadiusRA, nCutRadiusDEC,
				//						nSearchZoomSteps, 1 );
				//
				/////////////////////////////
			}

			///////////////////////
			// put back the best last trans and inverse for latter star match dos etc
			m_pStarMatch->CopyTrans( pAstroImage->m_pBestTrans, m_pStarMatch->m_pLastTrans );
			m_pStarMatch->CopyTrans( pAstroImage->m_pBestTrans, m_pStarMatch->m_pBestTrans );
			m_pStarMatch->CopyTrans( pAstroImage->m_pInvTrans, m_pStarMatch->m_pInvTrans );

			m_nProjectionScale = pAstroImage->m_nSkyProjectionScale;
			nFoundStats = m_nSearchBestMatch;
			m_bMatchStatus = MATCH_STATE_FOUND;

		} else
		{
			m_bMatchStatus = MATCH_STATE_NOT_FOUND;
		}

	} else
	{
		nFoundStats = 0;
		if( m_pWaitDialogMatch ) m_pWaitDialogMatch->m_bActionStatus = 0;
		pAstroImage->ClearMatched( );
		m_bMatchStatus = MATCH_STATE_CANCELLED;
	}

	// free stars vectors
	free( vectCatStars );
	free( vectImgStars );

	return( nFoundStats );
}

/////////////////////////////////////////////////////////////////////
// Method:	AreaSwipeSearch
// Class:	CSkyFinder
// Purpose:	search an area by swiping it on RA/DEC
// Input:	pointer to astro image, area center, width height, step
//			and call level (used for recursive calls)
// Output:	best found
/////////////////////////////////////////////////////////////////////
int CSkyFinder::AreaSwipeSearch( CAstroImage* pAstroImage, int nCallLevel )
{
	int nFoundStats = 0;
	wxString strLog;
	wxString strMsg;
//	double nMinX, nMaxX, nMinY, nMaxY;
	double startRA=0, endRA=0, startDEC=0, endDEC=0;
	double centerRa=0, centerDec=0, fieldWidth=0, fieldHeight=0, fieldRadius=0;
	int bSetToBreakLoop = 0;
	// fine tune props
	long nImgFinetuneStars = 0;
	long nCatFinetuneStars = 0;

	m_bMatchStatus = MATCH_STATE_NONE;

	// get pointer to wait dialog
//	CWaitDialogMatch* m_pWaitDialogMatch = m_pFrame->m_m_pWaitDialogMatch;

	// local vars
	int nZoomStep = 0;
	// image and catalog stars vectors dimentinons
	long nImgStarsExtracted = 0;
	long nImgStarsToExtract = m_pConfig->m_nScanMatchImgObjNo;
	long nImgStarsMax = (long) pAstroImage->m_nStar;
	if( nImgStarsToExtract > nImgStarsMax ) nImgStarsToExtract = nImgStarsMax;

	long nCatStarsExtracted = 0;
	long nCatStarsToExtract = m_pConfig->m_nScanMatchCatObjNo;
	long nCatStarsMax = (long) nImgStarsMax * m_pConfig->m_nMatchFineTuneCatStarsFactor;
	if( nCatStarsToExtract > nCatStarsMax ) nCatStarsToExtract = nCatStarsMax;

	// star vectors used
	StarDef* vectImgStars = NULL;
	StarDef* vectCatStars = NULL;

	// allocate image stars
	vectImgStars = (StarDef*) malloc( (nImgStarsMax+1)*sizeof(StarDef) );
	memset( vectImgStars, 0, (nImgStarsMax+1)*sizeof(StarDef) );
	// allocate catalog stars buffer sizeif
	if( nCatStarsMax < nCatStarsToExtract ) nCatStarsMax = nCatStarsToExtract;
	vectCatStars = (StarDef*) malloc( (nCatStarsMax+1)*sizeof(StarDef) );
	memset( vectCatStars, 0, (nCatStarsMax+1)*sizeof(StarDef) );

	// initialize best fit
	double nCurFit = DBL_MAX;

	// set max match to zero
	m_pStarMatch->m_nMaxMatch = 0;

	///////////////////////////////////////
	// ZOOM :: process by flag reverse/normal
	int nZoomInc=1, nSearchAreaZoomStart=0, nZoomLastStep=0;
	if( m_pConfig->m_bDoReverseZoomSearch )
	{
		nSearchAreaZoomStart = nSearchAreaZoomStart+m_nSearchAreaZoomSteps-1;
		if( nSearchAreaZoomStart < 0 ) nSearchAreaZoomStart=0;
		nZoomLastStep = m_nSearchAreaZoomStart;
		nZoomInc=-1;
	} else
	{
		nSearchAreaZoomStart = m_nSearchAreaZoomStart;
		nZoomLastStep = nSearchAreaZoomStart+m_nSearchAreaZoomSteps;
		nZoomInc=1;
	}

	// match analysis variables
	long nZLAvgCatStarsVal=0;
	long nZLAvgCatStars=0;
	long nALLAvgCatStarsVal=0;
	long nALLAvgCatStars=0;

	/////////////////////////////
	// now for variable zoom level in
	int nZoomCount = 0;
	for( nZoomStep=nSearchAreaZoomStart; (!m_pConfig->m_bDoReverseZoomSearch && nZoomStep<nZoomLastStep ) || 
		(m_pConfig->m_bDoReverseZoomSearch && nZoomStep >nZoomLastStep); nZoomStep+=nZoomInc )
	{
		nZLAvgCatStars=0;
		nZLAvgCatStarsVal = 0;

		///////////////////////////////
		// loop status check
		if( m_pWaitDialogMatch )
		{
			if( m_pWaitDialogMatch->m_bActionStatus == WAIT_DIALOG_MATCH_STATUS_STOP )
			{
				m_pWaitDialogMatch->m_bActionStatus = 0;
				break;

			} else if( m_pWaitDialogMatch->m_bActionStatus == WAIT_DIALOG_MATCH_STATUS_CANCEL )
			{
				break;
			}
		
			// set status
			strMsg.Printf( wxT("Do matching at zoom level %d ..."), nZoomStep );
			m_pUnimapWorker->SetWaitDialogMsg( strMsg );
			// now we use origin end values to calculate the cuts
			strLog.Printf( wxT("INFO :SS: zoom in at level=%d"), nZoomStep );
			m_pUnimapWorker->Log( strLog );
		}

		// how may sub division per x/y
		int nCutRatRA = 1+nZoomStep;
		int nCutRatDEC = 1+nZoomStep;

		double nCutRA = 0;
		double nCutDEC = 0;

		// calculate my search width/height based on the zoom factor
		double nCutWidth = (double) (m_nSearchAreaWidth/nCutRatRA);
		double nCutHeight = (double) (m_nSearchAreaHeight/nCutRatDEC);
		// and also calculate the overlaping sub-division
		double nCutRAStep = (double) nCutWidth/m_pConfig->m_nRaSearchOverlapDivision;
		double nCutDECStep = (double) nCutHeight/m_pConfig->m_nDecSearchOverlapDivision;

		nCatStarsExtracted = 0;
		m_bSearcLimitRightRA=0;

		/////////////////////////////////////////////////
		// :: SWIPE ON RA
		for( nCutRA=m_nSearchAreaStartRA; nCutRA<m_nSearchAreaEndRA && !m_bSearcLimitRightRA; nCutRA += nCutRAStep )
		{
			/////////////////
			// check for skip
			if( m_pWaitDialogMatch )
			{
				if( m_pWaitDialogMatch->m_bActionStatus == WAIT_DIALOG_MATCH_STATUS_SKIP )
				{
					m_pWaitDialogMatch->m_bActionStatus = 0;
					break;

				} else if( m_pWaitDialogMatch->m_bActionStatus == WAIT_DIALOG_MATCH_STATUS_STOP ||
							m_pWaitDialogMatch->m_bActionStatus == WAIT_DIALOG_MATCH_STATUS_CANCEL )
				{
					break;
				}
			}
			// reset dec flag limit
			m_bSearchLimitUpDEC = 0;

			//////////////////////////////////////
			// :: SWIPE ON DEC
			for( nCutDEC=m_nSearchAreaStartDEC; nCutDEC<m_nSearchAreaEndDEC && !m_bSearchLimitUpDEC; nCutDEC += nCutDECStep )
			{
				////////////////////////////////
				// here we do the match thingie
				int nRetMatchStatus = MatchImageToSkyArea( pAstroImage, 
										vectImgStars, nImgStarsToExtract,
										vectCatStars, nCatStarsToExtract,
										nCutRA, nCutDEC, nCutWidth, nCutHeight );
				// check if to break stop
				if( nRetMatchStatus && m_pConfig->m_bMatchStopWhenMinIsFound && 
					m_nSearchBestMatch >= m_pConfig->m_nMatchMinFoundToStop )
				{
					bSetToBreakLoop = 1;
					break;
				}
				// prepare average
				nZLAvgCatStarsVal += m_nExtractedCatStars;
				nZLAvgCatStars++;
				nALLAvgCatStarsVal += m_nExtractedCatStars;
				nALLAvgCatStars++;
			}

			if( bSetToBreakLoop ) break;
		}
		// calculate and check average per this zoom search
		if( nZLAvgCatStars != 0 && nZLAvgCatStarsVal != 0 && nZoomStep<nSearchAreaZoomStart && nZoomCount < 5 )
		{
			nZLAvgCatStarsVal = nZLAvgCatStarsVal/nZLAvgCatStars;
			if( nZLAvgCatStarsVal < 7 )
			{
				m_bMatchStatus = MATCH_STATE_SMALL_CATALOG;
				break;
			}
		}		

		if( bSetToBreakLoop ) break;

		nZoomCount++;
		// end of zoom level
	}

	////////////////
	// check for small catalog status
	if( m_bMatchStatus == MATCH_STATE_SMALL_CATALOG )
	{
		if( m_pWaitDialogMatch ) m_pWaitDialogMatch->m_bActionStatus = 0;
		pAstroImage->ClearMatched( );

		// free stars vectors
		free( vectCatStars );
		free( vectImgStars );

		return( 0 );
	}

	///////////////////////////////////////////
	// check for cancel status
	int bActionStatus = 0;
	if( m_pWaitDialogMatch ) bActionStatus = m_pWaitDialogMatch->m_bActionStatus;
	if( bActionStatus != WAIT_DIALOG_MATCH_STATUS_CANCEL )
	{
		// if no best match found use the first match
		if( m_nSearchBestMatch <=0 )
		{
			// if at least there is a valid first best match
			if( m_nSearchFirstBestMatch > 0 )
			{
				m_pStarMatch->m_nMaxMatch = m_nSearchFirstBestMatch;
				// do fine tune without recalculation on projection
				if( FineTuneMatch( pAstroImage,
									vectImgStars, nImgFinetuneStars,
									vectCatStars, nCatFinetuneStars,
									m_nSearchFirstCenterRA, m_nSearchFirstCenterDEC,
									m_nSearchFirstRadiusRA, m_nSearchFirstRadiusDEC, nCurFit,
									startRA, endRA, startDEC, endDEC, 
									centerRa, centerDec, fieldWidth, fieldHeight, fieldRadius, 0 ) )
				{
					m_nSearchBestMatch = m_pStarMatch->m_nMaxMatch;

					// set GUI dialog data
					if( m_pWaitDialogMatch ) m_pWaitDialogMatch->SetBestMatch( m_nSearchFirstBestMatch, m_pStarMatch->m_nMaxMatch );

					//////////////////////
					// now set best area to the image
					SetBestMatchArea( pAstroImage, startRA, endRA, startDEC, endDEC,
										centerRa, centerDec, fieldWidth, fieldHeight, fieldRadius,
										m_nSearchFirstCenterRA-m_nSearchFirstRadiusRA, 
										m_nSearchFirstCenterDEC-m_nSearchFirstRadiusDEC,
										vectImgStars, nImgFinetuneStars,
										vectCatStars, nCatFinetuneStars );

				} else
				{
					nFoundStats = 0;
					pAstroImage->ClearMatched( );
					m_bMatchStatus = MATCH_STATE_NOT_FOUND;
				}

			} else
			{
				nFoundStats = 0;
				pAstroImage->ClearMatched( );
				m_bMatchStatus = MATCH_STATE_NOT_FOUND;
			}

		}
		
		//////////////////////
		// now if found 
		if( m_nSearchBestMatch > 0 )
		{
			// check if to to do fine area check
			if( m_pConfig->m_bDoMatchRefineBestArea && nCallLevel == 0 )
			{
				/////////////////////////////
				// FINE SWIPE AREA - here is where I call area swipe again
				// AreaSwipeMatch( pAstroImage, nCutCenterRA,nCutCenterDEC,
				//						nCutRadiusRA, nCutRadiusDEC,
				//						nSearchZoomSteps, 1 );
				//
				/////////////////////////////
			}

			///////////////////////
			// put back the best last trans and inverse for latter star match dos etc
			m_pStarMatch->CopyTrans( pAstroImage->m_pBestTrans, m_pStarMatch->m_pLastTrans );
			m_pStarMatch->CopyTrans( pAstroImage->m_pBestTrans, m_pStarMatch->m_pBestTrans );
			m_pStarMatch->CopyTrans( pAstroImage->m_pInvTrans, m_pStarMatch->m_pInvTrans );

			m_nProjectionScale = pAstroImage->m_nSkyProjectionScale;
			nFoundStats = m_nSearchBestMatch;
			m_bMatchStatus = MATCH_STATE_FOUND;
		}

	} else
	{
		nFoundStats = 0;
		if( m_pWaitDialogMatch ) m_pWaitDialogMatch->m_bActionStatus = 0;
		pAstroImage->ClearMatched( );
		m_bMatchStatus = MATCH_STATE_CANCELLED;
	}

	// free stars vectors
	free( vectCatStars );
	free( vectImgStars );

	return( nFoundStats );
}

/////////////////////////////////////////////////////////////////////
// Method:	MatchImageToSkyArea
// Class:	CSkyFinder
// Purpose:	match image to sky area
// Input:	pointer to astro image, orig ra/dec , width, height
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyFinder::MatchImageToSkyArea( CAstroImage* pAstroImage,
								StarDef* vectImgStars, long nImgStarsToExtract,
								StarDef* vectCatStars, long nCatStarsToExtract,
								double nCutRA, double nCutDEC,
								double nCutWidth, double nCutHeight )
{
	int nReturnStatus = 0;
	long nImgStarsExtracted = 0;
	long nCatStarsExtracted = 0;
	// fine tune props
	long nImgFinetuneStars = 0;
	long nCatFinetuneStars = 0;

	wxString strLog;
	char strMsg[500];
	double nMinX=0.0, nMaxX=0.0, nMinY=0.0, nMaxY=0.0;
	double startRA=0, endRA=0, startDEC=0, endDEC=0, centerRa=0, 
			centerDec=0, fieldWidth=0, fieldHeight=0, fieldRadius=0;
	double nCurFit = DBL_MAX;

	// get pointer to wait dialog
//	CWaitDialogMatch* m_pWaitDialogMatch = m_pFrame->m_m_pWaitDialogMatch;

	///////////////////////
	// check for stop/cancel/pause or skip
	if( m_pWaitDialogMatch )
	{
		if( m_pWaitDialogMatch->m_bActionStatus == WAIT_DIALOG_MATCH_STATUS_SKIP ||
			m_pWaitDialogMatch->m_bActionStatus == WAIT_DIALOG_MATCH_STATUS_STOP ||
			m_pWaitDialogMatch->m_bActionStatus == WAIT_DIALOG_MATCH_STATUS_CANCEL )
		{
				return( 0 );

		} else if( m_pWaitDialogMatch->m_bActionStatus == WAIT_DIALOG_MATCH_STATUS_PAUSE )
		{
			while( m_pWaitDialogMatch->m_bActionStatus == WAIT_DIALOG_MATCH_STATUS_PAUSE ) wxThread::Sleep( 50 );
		}
	}

	// set cut references star/end/radius
	double nCutStartRA = nCutRA;
	double nCutStartDEC = nCutDEC;
	double nCutRadiusRA = (double) nCutWidth/2.0;
	double nCutRadiusDEC = (double) nCutHeight/2.0;
	double nCutCenterRA = nCutStartRA+nCutRadiusRA;
	double nCutCenterDEC = nCutStartDEC+nCutRadiusDEC;
	// get the big radius
	double nCutRadius = CalcSkyDistance( nCutRA, nCutDEC, nCutCenterRA, nCutCenterDEC );

	// if hight flows over
	if( nCutDEC + nCutHeight > m_nSearchAreaEndDEC )
	{
		// move origin on Y down to fit height
		nCutStartDEC = m_nSearchAreaEndDEC-nCutHeight;
		nCutCenterDEC = nCutStartDEC+nCutRadiusDEC;
		// se that we reached limit on y
		m_bSearchLimitUpDEC = 1;
	}
	// if width flows over
	if( nCutRA + nCutWidth > m_nSearchAreaEndRA  )
	{
		// move origin of x back to fit width
		nCutStartRA = m_nSearchAreaEndRA-nCutWidth;
		nCutCenterRA = nCutStartRA+nCutRadiusRA;
		// se that we reached limit on x
		m_bSearcLimitRightRA = 1;
	}

	// do the adjustment over the top
	AdjustRaDec( nCutStartRA, nCutStartDEC );
	AdjustRaDec( nCutCenterRA, nCutCenterDEC );

	//////////////////////
	// need to extract stars here not at the begining -  because these get screwd in fine tune
	nImgStarsExtracted = pAstroImage->ExtractStars( vectImgStars, nImgStarsToExtract,
							m_pConfig->m_nImgObjectExtractByObjShape, m_pConfig->m_nImgObjectExtractByDetectType, 1 );

	///////////////////////////////////////
	// cut window from catalog
	nCatStarsExtracted = m_pCatalogStars->CutAreaFromCatalog( nCutCenterRA, nCutCenterDEC, nCutRadius,
								vectCatStars, nCatStarsToExtract,
								pAstroImage->m_nWidth, pAstroImage->m_nHeight,
								nMinX, nMaxX, nMinY, nMaxY );

	// set back how much was extracted - this is to be used to evaluate 
	// the matching process and feedback to user
	m_nExtractedImgStars = nImgStarsExtracted;
	m_nExtractedCatStars = nCatStarsExtracted;

//	ResetStarVectorMatch( vectCatStars, nCatStarsExtracted );
//	ResetStarVectorMatch( vectImgStars, nImgStarsExtracted );

	/////////////////////////////////////
	// set cross for cut
	if( m_pWaitDialogMatch )
	{
		m_pWaitDialogMatch->SetTarget( nCutCenterRA, nCutCenterDEC,
									nCutWidth, nCutHeight, nCatStarsExtracted );
		m_pUnimapWorker->SetWaitDialogUpdate( );
	}

//	Sleep( 500 );

	//////////////////////////////////////////////////
	// if there are enough point in my window do match
	if( nCatStarsExtracted > m_pConfig->m_nSearchMatchMinCatStars )
	{
		m_pStarMatch->m_nMaxMatch = 0;
		// now do the matching
		int nResult = m_pStarMatch->Cmp( vectImgStars, nImgStarsExtracted,
											vectCatStars, nCatStarsExtracted, 0 );
		if( nResult )
		{
			// now to think about - i could stop here or
			// finish a complete scan to find the most cetered
			// and memorize central position and scale on xy
			// >= ?
//			if( m_pStarMatch->m_nMaxMatch >= nBestMatch )
			if( m_pStarMatch->m_nMaxMatch >= m_pConfig->m_nMatchFirstLevelMinStars )
			{
				/////////////////////////////////////
				// store  first best in case no other option use even that
				// >= ?
				if( m_pStarMatch->m_nMaxMatch >= m_nSearchFirstBestMatch )
				{
					// moved here ... to get best only when best fit
					m_nSearchFirstRadiusRA = nCutRadiusRA;
					m_nSearchFirstRadiusDEC = nCutRadiusDEC;
					m_nSearchFirstCenterRA = nCutCenterRA;
					m_nSearchFirstCenterDEC = nCutCenterDEC;
					// store projection scale
					m_nSearchFirstFoundProjectionScale = (double) (pAstroImage->m_nWidth/(nMaxX-nMinX));

					m_nSearchFirstBestMatch  = m_pStarMatch->m_nMaxMatch;
					// log
					if( m_pWaitDialogMatch )
					{
						m_pWaitDialogMatch->SetMostMatched( m_nSearchFirstBestMatch );
						m_pUnimapWorker->SetWaitDialogUpdate( );
					}
				}

				int nTempBestMatch  = m_pStarMatch->m_nMaxMatch;
				// todo :: check if I uset this ???
				m_nSearchFoundProjectionScale = (double) (pAstroImage->m_nWidth/(nMaxX-nMinX));

				// debug :: log
				strLog.Printf( wxT("INFO :: FIRST found max no of stars =%d"), m_pStarMatch->m_nMaxMatch );
				m_pUnimapWorker->Log( strLog );

				////////////////////////////////////////
				// if no fine tune then skip - also check for stop flag here
				if( m_pConfig->m_bDoMatchFineTune == 0 ) return( 1 );
				/////////////////////////////////

				///////////////////
				// here we do fine tune match
				if( FineTuneMatch( pAstroImage,
									vectImgStars, nImgFinetuneStars,
									vectCatStars, nCatFinetuneStars,
									nCutCenterRA, nCutCenterDEC,
									nCutRadiusRA, nCutRadiusDEC, nCurFit,
									startRA, endRA, startDEC, endDEC, 
									centerRa, centerDec, fieldWidth, fieldHeight, fieldRadius,
									m_pConfig->m_bDoMatchFineTuneWithProjection ) )
				{
					// now check if I got a best of 
					if( CheckMatchByNoOfStarsAndFit( m_pStarMatch->m_nMaxMatch, m_nSearchBestMatch, nCurFit, m_nSearchBestFit ) )
					{
						// get a best fit
						if( nCurFit > 0.0 && nCurFit <= m_nSearchBestFit ) m_nSearchBestFit = nCurFit;
						// save best no of stars matched
						m_nSearchBestMatch = m_pStarMatch->m_nMaxMatch;
						// set GUI dialog data
						if( m_pWaitDialogMatch ) m_pWaitDialogMatch->SetBestMatch( nTempBestMatch, m_pStarMatch->m_nMaxMatch );

						// now set best area to the image
						SetBestMatchArea( pAstroImage, startRA, endRA, startDEC, endDEC,
											centerRa, centerDec, fieldWidth, fieldHeight, fieldRadius,
											m_nSearchAreaStartRA, m_nSearchAreaStartDEC,
											vectImgStars, nImgFinetuneStars,
											vectCatStars, nCatFinetuneStars );
					}
					// return status is 1
					nReturnStatus = 1;

				} else
				{
					strLog.Printf( wxT("INFO :: LESS no of stars found on TUNE=%d"), m_pStarMatch->m_nMaxMatch  );
					m_pUnimapWorker->Log( strLog );
				}

			} else
			{
				strLog.Printf( wxT("INFO :: LESS no of stars found=%d then orig=%d"), m_pStarMatch->m_nMaxMatch, m_nSearchBestMatch );
				m_pUnimapWorker->Log( strLog );
			}

		}
	}

	// log
	if( m_pWaitDialogMatch )
	{
		m_pWaitDialogMatch->SetFound( m_pStarMatch->m_nMaxMatch );
		m_pUnimapWorker->SetWaitDialogUpdate( );
	}

	///////////////////////////
	// here we do the set to break stuff
	if( nReturnStatus && m_pConfig->m_bMatchStopWhenMinIsFound && 
		m_nSearchBestMatch >= m_pConfig->m_nMatchMinFoundToStop )
	{
		nReturnStatus = 5;
	}

	return( nReturnStatus );
}

/////////////////////////////////////////////////////////////////////
// Method:	FineTuneMatch
// Class:	CSkyFinder
// Purpose:	re-calculate image project in the sky 2nd, 3rd and inv
// Input:	pointer to astro image, orig x/y amnd radius x/y
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyFinder::FineTuneMatch( CAstroImage* pAstroImage,
								StarDef* vectImgStars, long& nImgStarCount,
								StarDef* vectCatStars, long& nCatStarCount,
								double nCenterRA, double nCenterDEC,
								double nRadiusRA, double nRadiusDEC, double& nFit,
								double& startRA, double& endRA, double& startDEC, double& endDEC,
								double& centerRa, double& centerDec, double& fieldWidth, 
								double& fieldHeight, double& fieldRadius, int bRecalcWithPrj )
{
	wxString strLog;
	int nReturnStatus = 1;
	double nMinX=0, nMaxX=0, nMinY=0, nMaxY=0;

	//////////////////////
	// now if there was not star found at all exit
	if( m_pStarMatch->m_nMaxMatch < 4 ) return( 0 );

	long nImgStarsToExtract = 0, nImgStarsExtracted = 0;
	long nCatStarsToExtract = 0, nCatStarsExtracted = 0;

	double startPrjRA = 0, endPrjRA = 0, startPrjDEC = 0, endPrjDEC = 0;
	double nPrjFieldWidth = 0, nPrjFieldHeight = 0;

	// ra/dec coords
	double nRaCenter = nCenterRA;
	double nDecCenter = nCenterDEC;
	double nRaStart = nCenterRA-nRadiusRA;
	double nDecStart = nCenterDEC-nRadiusDEC;
	double nCatCutWidth = 2.0*nRadiusRA;
	double nCatCutHeight = 2.0*nRadiusDEC;
	double nRaOrigError = 0;
	double nDecOrigError = 0;
	// calculate cut radius
	double nCutRadius = CalcSkyDistance( nRaCenter, nDecCenter, nRaStart, nDecStart );
	double nPrjRadius = 0;

	// now we try to recalculate again with refit
	m_pUnimapWorker->Log( wxT("INFO :: FINE TUNE - Recalculate and do refit with projection") );

	// now set how may stars to match - use all for now
	if( bRecalcWithPrj )
		nImgStarsToExtract = (long) pAstroImage->m_nStar * m_pConfig->m_nMatchFineTuneImgStarsFactor;
	else
		nImgStarsToExtract = (long) pAstroImage->m_nStar;

	nImgStarsExtracted = pAstroImage->ExtractStars( vectImgStars, nImgStarsToExtract, 
							m_pConfig->m_nImgObjectExtractByObjShape, m_pConfig->m_nImgObjectExtractByDetectType, 1 );
	// and now set how many stars should be extracted from catalog
	nCatStarsToExtract = (long) nImgStarsExtracted * m_pConfig->m_nMatchFineTuneCatStarsFactor;
	// cut stars from catalog
	nCatStarsExtracted = m_pCatalogStars->CutAreaFromCatalog( nRaCenter, nDecCenter, nCutRadius, 
									vectCatStars, nCatStarsToExtract,
									pAstroImage->m_nWidth, pAstroImage->m_nHeight,
									nMinX, nMaxX, nMinY, nMaxY );
	// calculate projection scale - to use latter
	m_nProjectionScale = (double) (pAstroImage->m_nWidth/(nMaxX-nMinX));

	///////////////////////////////
	// READJUST no of stars in image if catalog less
 	if( nCatStarsExtracted < nImgStarsExtracted )
	{
		nImgStarsToExtract = (long) nCatStarsExtracted;
		nImgStarsExtracted = pAstroImage->ExtractStars( vectImgStars, nImgStarsToExtract, 
								m_pConfig->m_nImgObjectExtractByObjShape, m_pConfig->m_nImgObjectExtractByDetectType, 1 );
	}

	// adjust the extracted settings
	m_nExtractedImgStars = nImgStarsExtracted;
	m_nExtractedCatStars = nCatStarsExtracted;

//	ResetStarVectorMatch( vectCatStars, nCatStarsExtracted );
//	ResetStarVectorMatch( vectImgStars, nImgStarsExtracted );

	////////////////////////////////////////
	// set here other trans
	//m_pStarMatch->SetTransType( 2 );
	// then compare
	if( m_pStarMatch->Cmp( vectImgStars, nImgStarsExtracted, vectCatStars, nCatStarsExtracted, 0 ) )
	{
		int nmatch = m_pStarMatch->m_pTrans->nm;
		nReturnStatus = 1;
		nFit = m_pStarMatch->m_pTrans->sig;

		strLog.Printf( wxT("INFO :: second level more stars found=%d, fit=%f"), m_pStarMatch->m_nMaxMatch, nFit );
		m_pUnimapWorker->Log( strLog );

		//sprintf( strLog, "INFO ::list of matched stars: %s", m_pStarMatch->m_strFoundStar );
		//m_pUnimapWorker->Log( strLog );

//		m_pStarMatch->Cmp(  vectCatStars, nCatStarsExtracted, vectImgStars, nImgStarsExtracted, 0 );
//		nmatch = m_pStarMatch->m_pTrans->nm;

		if( m_pStarMatch->CalcInv( vectCatStars, nCatStarsExtracted, vectImgStars, nImgStarsExtracted ) != 0 )
		{
			strLog.Printf( wxT("INFO :: skyarea BEFORE recalc (%.4f,%.4f) (%.4f,%.4f)"),
								(double) nRaCenter,  (double) nDecCenter,
										(double) 2*nRadiusRA, (double) 2*nRadiusDEC );
			m_pUnimapWorker->Log( strLog );
			////// -- calculate sky projection
			CalcImgSkyProjection( pAstroImage, nRaCenter, nDecCenter, startPrjRA, endPrjRA, 
									startPrjDEC, endPrjDEC, nPrjFieldWidth, nPrjFieldHeight, nPrjRadius );
//			nPrjFieldWidth = fabs( endPrjRA-startPrjRA );
//			nPrjFieldHeight = fabs( endPrjDEC-startPrjDEC );
			///// - end of sky projection calculation
			strLog.Printf( wxT("INFO :: skyarea AFTER recalc (%.4f,%.4f) (%.4f,%.4f)"),
								startPrjRA+(nPrjFieldWidth/2),
								startPrjDEC+(nPrjFieldHeight/2),
								nPrjFieldWidth, nPrjFieldHeight );
			m_pUnimapWorker->Log( strLog );

			///////////////////
			// check for big projection error
			nRaOrigError = fabs( nRaStart-startPrjRA );
			nDecOrigError = fabs( nDecStart-startPrjDEC );
			// if projection is within an acceptable range then set ra/dec based on projection
			if( nPrjFieldWidth < 2.0*nCatCutWidth && nPrjFieldHeight < 2.0*nCatCutHeight &&
//				nRaOrigError < nCatCutWidth/10 && nDecOrigError < nCatCutHeight/10 &&
				nPrjFieldWidth > 0.0 && nPrjFieldHeight > 0.0 && nFit < 10.0 )
			{
//				nRaCenter = (double) startPrjRA + (nPrjFieldWidth/2.0);
//				nDecCenter = (double) startPrjDEC + (nPrjFieldHeight/2.0);

//				nRaStart = startPrjRA;
//				nDecStart = startPrjDEC;
				// use version with current center just width/height adjusted
				nRaStart = nRaCenter-(nPrjFieldWidth/2.0);
				nDecStart = nDecCenter-(nPrjFieldHeight/2.0);

				nCatCutWidth = nPrjFieldWidth;
				nCatCutHeight = nPrjFieldHeight;
				// adjust radius
				nCutRadius = nPrjRadius;
			}

		} else
		{
			m_pUnimapWorker->Log( wxT("INFO :: FINE TUNE Level 2 failed to calc inv trans") );
			nReturnStatus = 0;
		}

		///////////////////////////////////////////////////////////////////////////
		// THIRD CALCULATION IF or not if flag = 0 - TODO :: HERE SEE WHAT THE HECK IS WITH THIS ?!!!
		//  WHY DO I NEED A THIRD RECALCULATION ???? - is BUGGY ANYWAY
		//bRecalcWithPrj = 0;
		if( bRecalcWithPrj && nReturnStatus )
		{
			// calculate how many stars to extract on fine tune with projection
			nImgStarsToExtract = (long) pAstroImage->m_nStar;
			nCatStarsToExtract = (long) nImgStarsToExtract * m_pConfig->m_nMatchFineTuneCatStarsFactor;
			// extract image stars
			nImgStarsExtracted = pAstroImage->ExtractStars( vectImgStars, nImgStarsToExtract, 
									m_pConfig->m_nImgObjectExtractByObjShape, m_pConfig->m_nImgObjectExtractByDetectType, 1 );
			// extract catalog stars
			nCatStarsExtracted = m_pCatalogStars->CutAreaFromCatalog( nRaCenter, nDecCenter, nCutRadius,
										vectCatStars, nCatStarsToExtract,
										pAstroImage->m_nWidth, pAstroImage->m_nHeight,
										nMinX, nMaxX, nMinY, nMaxY, 1 );

			// calculate projection scale - to use latter
			m_nProjectionScale = (double) ( pAstroImage->m_nWidth/(nMaxX-nMinX) );

			///////////////////////////////
			// READJUST no of stars in image if catalog less
			if( nCatStarsExtracted < nImgStarsExtracted )
			{
				nImgStarsToExtract = (long) nCatStarsExtracted;
				nImgStarsExtracted = pAstroImage->ExtractStars( vectImgStars, nImgStarsToExtract, 
										m_pConfig->m_nImgObjectExtractByObjShape, m_pConfig->m_nImgObjectExtractByDetectType, 1 );
			}


			// reset on of matches
			m_pStarMatch->m_nMaxMatch = 0;
			ResetStarVectorMatch( vectCatStars, nCatStarsExtracted );
			ResetStarVectorMatch( vectImgStars, nImgStarsExtracted );

			// set here other trans
			//m_pStarMatch->SetTransType( 2 );
			// then compare
			if( m_pStarMatch->Cmp( vectImgStars, nImgStarsExtracted, vectCatStars, nCatStarsExtracted, 0 ) )
			{
				nFit = m_pStarMatch->m_pTrans->sig;

				// fitness condition - todo:: see not to be hardcoded anylonger - maybe dynamic based on the previous one
				if( nFit < 10.0 )
				{
					strLog.Printf( wxT("INFO :: third level more stars found=%d, fit=%f"), m_pStarMatch->m_nMaxMatch, nFit );
					m_pUnimapWorker->Log( strLog );
					nReturnStatus = 1;

					// also calculate the inverse
					if( m_pStarMatch->CalcInv( vectCatStars, nCatStarsExtracted, vectImgStars, nImgStarsExtracted ) == 0 )
					{
						m_pUnimapWorker->Log( wxT("INFO :: faild to calc inv trans") );
						nReturnStatus = 0;

					} else
					{
						// calculate img sky projection again
						CalcImgSkyProjection( pAstroImage, nRaCenter, nDecCenter, startPrjRA, endPrjRA, 
												startPrjDEC, endPrjDEC, nPrjFieldWidth, nPrjFieldHeight, nPrjRadius );
	//					nPrjFieldWidth = fabs( endPrjRA-startPrjRA );
	//					nPrjFieldHeight = fabs( endPrjDEC-startPrjDEC );

						// adjust for errors ??
						nRaOrigError = fabs( nRaStart-startPrjRA );
						nDecOrigError = fabs( nDecStart-startPrjDEC );
						// check if projection is in an acceptable range
						if( nPrjFieldWidth < 2.0*nCatCutWidth && nPrjFieldHeight < 2.0*nCatCutHeight &&
	//						nRaOrigError < nCatCutWidth/10 && nDecOrigError < nCatCutHeight/10 &&
							nPrjFieldWidth > 0.0 && nPrjFieldHeight > 0.0  )
						{						
	//						nRaCenter = (double) startPrjRA + (nPrjFieldWidth/2);
	//						nDecCenter = (double) startPrjDEC + (nPrjFieldHeight/2);

	//						nRaStart = startPrjRA;
	//						nDecStart = startPrjDEC;
							// use version with current center just width/height adjusted
							nRaStart = nRaCenter-(nPrjFieldWidth/2.0);
							nDecStart = nDecCenter-(nPrjFieldHeight/2.0);
							// adjust size
							nCatCutWidth = nPrjFieldWidth;
							nCatCutHeight = nPrjFieldHeight;
							// adjust radius
							nCutRadius = nPrjRadius;

						}

						nReturnStatus = 1;
					}

				} else
				{
					nReturnStatus = 0;
				}
	


			} else
			{
				m_pUnimapWorker->Log( wxT("INFO :: FINE TUNE - Third level recalc could not find any more matches") );
				nReturnStatus = 0;
			}

			// END OF RECALC
			///////////////////////////////

		} else
		{
			//if( bRecalcWithPrj == 0 && ) nReturnStatus = 0;
		}

	} else
	{
		m_pUnimapWorker->Log( wxT("INFO :: FINE TUNE Second level could not find more stars") );
		nReturnStatus = 0;
		return( 0 );
	}
	// now set return if success
	if( nReturnStatus == 1 )
	{
		startRA = nRaStart;
		endRA = nRaStart + nCatCutWidth;
		startDEC = nDecStart;
		endDEC = nDecStart + nCatCutHeight;
		// center
		centerRa = nRaCenter;
		centerDec = nDecCenter;
		// size & radius
		fieldWidth = nCatCutWidth;
		fieldHeight = nCatCutHeight;
		// hack ?
		fieldRadius = nPrjRadius; //nCutRadius;
		// set stars extracted
		nImgStarCount = nImgStarsExtracted;
		nCatStarCount = nCatStarsExtracted;
	}

	// here log field size
	strLog.Printf( wxT("INFO :: field size width=%.3f height=%.3f"), 2*nRadiusRA, 2*nRadiusDEC );
	m_pUnimapWorker->Log( strLog );

	return( nReturnStatus );
}

/////////////////////////////////////////////////////////////////////
// Method:	SetBestMatchArea
// Class:	CSkyFinder
// Purpose:	set best match area for image, log and update gui
// Input:	pointer to astro image and ra dec limits
// Output:	status
/////////////////////////////////////////////////////////////////////
void CSkyFinder::SetBestMatchArea( CAstroImage* pAstroImage,
										double startRA, double endRA,
										double startDEC, double endDEC,
										double centerRa, double centerDec, double fieldWidth, 
										double fieldHeight, double fieldRadius,
										double originRA, double originDEC,
										StarDef* vectImgStars, long nImgStars,
										StarDef* vectCatStars, long nCatStars )
{
	wxString strLog;

	// set dimensions
	double nFieldWidth = endRA - startRA;
	double nFieldHeight = endDEC - startDEC;
	double nRadiusRA = (double) (nFieldWidth/2);
	double nRadiusDEC = (double) (nFieldHeight/2);

	// set image field coords/size
	pAstroImage->SetCoordByMinMaxRaDec( startRA, endRA, startDEC, endDEC );
	pAstroImage->SetMainCoord( centerRa, centerDec, fieldWidth, fieldHeight, fieldRadius );
	///////////////////
	// copy last transcations
	m_pStarMatch->ResetTrans( pAstroImage->m_pBestTrans );
	m_pStarMatch->ResetTrans( pAstroImage->m_pInvTrans );
	m_pStarMatch->CopyTrans( m_pStarMatch->m_pLastTrans, pAstroImage->m_pBestTrans );
	m_pStarMatch->CopyTrans( m_pStarMatch->m_pInvTrans, pAstroImage->m_pInvTrans );

	// set image projection scale
	pAstroImage->m_nSkyProjectionScale = m_nProjectionScale;

	// set cat number for the matched stars
	pAstroImage->SetCatStarsMatched( vectImgStars, nImgStars );
	// now chekc for match by cloest distance
	pAstroImage->MatchNotMatchedStars( vectCatStars, nCatStars );
	// also set stars with no match
	pAstroImage->SetCatStarsNotMatched( vectCatStars, nCatStars );

	// calculate coordinates for the rest of the objects
	CalcNoMatchObjProjection( pAstroImage );

	// set no of objectes matched
	pAstroImage->m_nMatchedStars = m_pStarMatch->m_nMaxMatch;

	// keep the origin where my objects where found
	// use instead the image orig
	m_nWinOrigX = pAstroImage->m_nOrigRa;
	m_nWinOrigY = pAstroImage->m_nOrigDec;

	/////////////
	// here log field size
	strLog.Printf( wxT("INFO :: SET BEST Field size width=%.3f height=%.3f"), 2*nRadiusRA, 2*nRadiusDEC );
	m_pUnimapWorker->Log( strLog );

	// log
	strLog.Printf( wxT("INFO :: FOUND BEST NO=%d"), m_pStarMatch->m_nMaxMatch );
	m_pUnimapWorker->Log( strLog );
//	strLog.Printf( wxT("INFO :: best list of matched stars: %s"), m_pStarMatch->m_strFoundStar );
//	m_pUnimapWorker->Log( strLog );

	// and also set best target area
	if( m_pWaitDialogMatch )
	{
		m_pWaitDialogMatch->SetBestTarget( (startRA+nRadiusRA)-originRA,
					(startDEC+nRadiusDEC)-originDEC, nFieldWidth, nFieldHeight );
		m_pUnimapWorker->SetWaitDialogUpdate( );
	}

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	FindDso
// Class:	CSkyFinder
// Purpose:	find dos using ngcic catalog
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CSkyFinder::FindDso( CAstroImage* pAstroImage )
{
	wxString strLog;
	wxString strName = wxT("");
	unsigned long i=0;
	StarDef star, tmpstar;
	unsigned char nType = 0;
	double nDistDeg = 0;
	// some temp vars
	double ang = 0;
	double xdiff = 0;
	double ydiff = 0;

	//////////////////////////////////////////////////////
	// if flag set to use ngc
	if( m_pConfig->m_bUseNgc )
	{
		if( !m_pSky->m_pCatalogDsoNgc->m_vectDso ) m_pSky->m_pCatalogDsoNgc->LoadNgcBinary(); // use latter- Load( );
		//////////////////////
		// search ngc
		for( i=0; i<m_pSky->m_pCatalogDsoNgc->m_nDso; i++ )
		{
			////////////////////////////////////////////
			// calculate distance 
			nDistDeg = CalcSkyDistance( m_pSky->m_pCatalogDsoNgc->m_vectDso[i].ra, m_pSky->m_pCatalogDsoNgc->m_vectDso[i].dec, 
								pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );

/*			if( m_vectNgc[i].ra >= pAstroImage->m_nRaStart &&
				m_vectNgc[i].ra <= pAstroImage->m_nRaEnd &&
				m_vectNgc[i].dec >= pAstroImage->m_nDecStart &&
				m_vectNgc[i].dec <= pAstroImage->m_nDecEnd )
*/
			if( nDistDeg <= pAstroImage->m_nRadius )
			{
				memset( &star, 0, sizeof(StarDef) );
				star.ra = m_pSky->m_pCatalogDsoNgc->m_vectDso[i].ra;
				star.dec = m_pSky->m_pCatalogDsoNgc->m_vectDso[i].dec;

				// project star using the found orig
				m_pSky->ProjectStar( star, pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );
				// larry :: my damn multiplication factor
				star.x = star.x*pAstroImage->m_nSkyProjectionScale; //10;
				star.y = star.y*pAstroImage->m_nSkyProjectionScale; //10;
				// and now apply inverse trans
				m_pStarMatch->ApplyObjInv( star );
	//				star.x = -star.x;

				// check if object is in my image
				if( star.x >= 0 && star.y >= 0 &&
					star.x <= pAstroImage->m_nWidth &&
					star.y <= pAstroImage->m_nHeight )
				{
					// now check specificatly for messier
					// if( m_vectNgc[i].messier_no != 0 )
					int nMessierNo = m_pSky->m_pCatalogDsoAssoc->IsNgcIcObjectMessier( m_pSky->m_pCatalogDsoNgc->m_vectDso[i].cat_no, CAT_OBJECT_TYPE_NGC ); 
					if( nMessierNo >= 0 )
					{
						strLog.Printf( wxT("INFO :: found messier %d"), nMessierNo );
						m_pUnimapWorker->Log( strLog );

						nType = SKY_OBJECT_TYPE_DSO;
						// set my star
						star.cat_type = CAT_OBJECT_TYPE_MESSIER;
						star.cat_no = nMessierNo;

					} else
					{
						strLog.Printf( wxT("INFO :: found NGC %d"), m_pSky->m_pCatalogDsoNgc->m_vectDso[i].cat_no );
						m_pUnimapWorker->Log( strLog );
						nType = SKY_OBJECT_TYPE_DSO;

						// set my star
						star.cat_type = CAT_OBJECT_TYPE_NGC;
						star.cat_no = m_pSky->m_pCatalogDsoNgc->m_vectDso[i].cat_no;
					}

					///////////////////////////
					// calc projection for ellipse a/b
					memset( &tmpstar, 0, sizeof(StarDef) );
					ang = m_pSky->m_pCatalogDsoNgc->m_vectDso[i].pa*DEG2RAD;
					tmpstar.ra = m_pSky->m_pCatalogDsoNgc->m_vectDso[i].ra+(m_pSky->m_pCatalogDsoNgc->m_vectDso[i].a/60)*sin(ang);
					tmpstar.dec =m_pSky->m_pCatalogDsoNgc->m_vectDso[i].dec+(m_pSky->m_pCatalogDsoNgc->m_vectDso[i].a/60)*cos(ang);
//					tmpstar.dec = m_vectNgc[i].b*cos(m_vectNgc[i].pa);

					// project star on tangent plan using the found orig
					m_pSky->ProjectStar( tmpstar, m_nWinOrigX, m_nWinOrigY );
					// larry :: my damn multiplication factor
					tmpstar.x = tmpstar.x*m_nProjectionScale; //10;
					tmpstar.y = tmpstar.y*m_nProjectionScale; //10;
					// and now apply inverse trans
					m_pStarMatch->ApplyObjInv( tmpstar );

					xdiff = tmpstar.x-star.x;
					ydiff = tmpstar.y-star.y;

					star.a = sqrt( xdiff*xdiff + ydiff*ydiff );

					/////////// and for b is ::
					ang = (90.0-m_pSky->m_pCatalogDsoNgc->m_vectDso[i].pa)*DEG2RAD;
					tmpstar.ra = m_pSky->m_pCatalogDsoNgc->m_vectDso[i].ra+(m_pSky->m_pCatalogDsoNgc->m_vectDso[i].b/60)*sin(ang);
					tmpstar.dec = m_pSky->m_pCatalogDsoNgc->m_vectDso[i].dec+(m_pSky->m_pCatalogDsoNgc->m_vectDso[i].b/60)*cos(ang);
//					tmpstar.dec = m_vectNgc[i].b*cos(m_vectNgc[i].pa);

					// project star on tangent plan using the found orig
					m_pSky->ProjectStar( tmpstar, m_nWinOrigX, m_nWinOrigY );
					// larry :: my damn multiplication factor
					tmpstar.x = tmpstar.x*m_nProjectionScale; //10;
					tmpstar.y = tmpstar.y*m_nProjectionScale; //10;
					// and now apply inverse trans
					m_pStarMatch->ApplyObjInv( tmpstar );

					xdiff = tmpstar.x-star.x;
					ydiff = tmpstar.y-star.y;

					star.b = sqrt( xdiff*xdiff + ydiff*ydiff );

//					star.a = m_vectNgc[i].a;
//					star.b = m_vectNgc[i].b;

					star.theta = m_pSky->m_pCatalogDsoNgc->m_vectDso[i].pa;
					star.mag = m_pSky->m_pCatalogDsoNgc->m_vectDso[i].mag;
					star.kronfactor = 1;

					// and now add this to vector of catalog object in the image
					pAstroImage->AddCatDso( star, nType );

				}

			}

		}
		// unload catalog - todo: find out why this is a problem
		//m_pSky->m_pCatalogDsoNgc->UnloadCatalog();
	}

	///////////////////////////////////////////////////////////
	// if set to use Ic catalog
	if( m_pConfig->m_bUseIc )
	{
		if( !m_pSky->m_pCatalogDsoIc->m_vectDso ) m_pSky->m_pCatalogDsoIc->LoadIcBinary(); // use latter common Load( );
		//////////////////////
		// search ic
		for( i=0; i<m_pSky->m_pCatalogDsoIc->m_nDso; i++ )
		{
			////////////////////////////////////////////
			// calculate distance 
			nDistDeg = CalcSkyDistance( m_pSky->m_pCatalogDsoIc->m_vectDso[i].ra, m_pSky->m_pCatalogDsoIc->m_vectDso[i].dec, 
								pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );

/*			if( m_vectIc[i].ra >= pAstroImage->m_nRaStart &&
				m_vectIc[i].ra <= pAstroImage->m_nRaEnd &&
				m_vectIc[i].dec >= pAstroImage->m_nDecStart &&
				m_vectIc[i].dec <= pAstroImage->m_nDecEnd )
*/
			if( nDistDeg <= pAstroImage->m_nRadius )
			{
				memset( &star, 0, sizeof(StarDef) );
				star.ra = m_pSky->m_pCatalogDsoIc->m_vectDso[i].ra;
				star.dec = m_pSky->m_pCatalogDsoIc->m_vectDso[i].dec;
				// project star on tangent plan using the found orig
				m_pSky->ProjectStar( star, m_nWinOrigX, m_nWinOrigY );
				// larry :: my damn multiplication factor
				star.x = star.x*m_nProjectionScale; //10;
				star.y = star.y*m_nProjectionScale; //10;
				// and now apply inverse trans
				m_pStarMatch->ApplyObjInv( star );
	//				star.x = -star.x;

				// check if object is in my image
				if( star.x >= 0 && star.y >= 0 &&
					star.x <= pAstroImage->m_nWidth &&
					star.y <= pAstroImage->m_nHeight )
				{
					// now check specificatly for messier
					// if( m_vectIc[i].messier_no != 0 )
					int nMessierNo = m_pSky->m_pCatalogDsoAssoc->IsNgcIcObjectMessier( m_pSky->m_pCatalogDsoIc->m_vectDso[i].cat_no, CAT_OBJECT_TYPE_IC ); 
					if( nMessierNo >= 0 )
					{
						strLog.Printf( wxT("INFO :: found messier %d"), nMessierNo );
						m_pUnimapWorker->Log( strLog );

						nType = SKY_OBJECT_TYPE_DSO;
						// set my star
						star.cat_type = CAT_OBJECT_TYPE_MESSIER;
						star.cat_no = nMessierNo;

					} else
					{
						strLog.Printf( wxT("INFO :: found IC %d"), m_pSky->m_pCatalogDsoIc->m_vectDso[i].cat_no );
						m_pUnimapWorker->Log( strLog );
						nType = SKY_OBJECT_TYPE_DSO;

						// set my star
						star.cat_type = CAT_OBJECT_TYPE_IC;
						star.cat_no = m_pSky->m_pCatalogDsoIc->m_vectDso[i].cat_no;

					}
					///////////////////////////
					// calc projection for ellipse a/b
					memset( &tmpstar, 0, sizeof(StarDef) );
					ang = m_pSky->m_pCatalogDsoIc->m_vectDso[i].pa*PI/180;
					tmpstar.ra = m_pSky->m_pCatalogDsoIc->m_vectDso[i].ra+(m_pSky->m_pCatalogDsoIc->m_vectDso[i].a/60)*sin(ang);
					tmpstar.dec =m_pSky->m_pCatalogDsoIc->m_vectDso[i].dec+(m_pSky->m_pCatalogDsoIc->m_vectDso[i].a/60)*cos(ang);
//					tmpstar.dec = m_vectNgc[i].b*cos(m_vectNgc[i].pa);

					// project star on tangent plan using the found orig
					m_pSky->ProjectStar( tmpstar, m_nWinOrigX, m_nWinOrigY );
					// larry :: my damn multiplication factor
					tmpstar.x = tmpstar.x*m_nProjectionScale; //10;
					tmpstar.y = tmpstar.y*m_nProjectionScale; //10;
					// and now apply inverse trans
					m_pStarMatch->ApplyObjInv( tmpstar );

					xdiff = tmpstar.x-star.x;
					ydiff = tmpstar.y-star.y;

					star.a = sqrt( xdiff*xdiff + ydiff*ydiff );

					/////////// and for b is ::
					ang = (90.0-m_pSky->m_pCatalogDsoIc->m_vectDso[i].pa)*PI/180;
					tmpstar.ra = m_pSky->m_pCatalogDsoIc->m_vectDso[i].ra+(m_pSky->m_pCatalogDsoIc->m_vectDso[i].b/60)*sin(ang);
					tmpstar.dec = m_pSky->m_pCatalogDsoIc->m_vectDso[i].dec+(m_pSky->m_pCatalogDsoIc->m_vectDso[i].b/60)*cos(ang);
//					tmpstar.dec = m_vectNgc[i].b*cos(m_vectNgc[i].pa);

					// project star on tangent plan using the found orig
					m_pSky->ProjectStar( tmpstar, m_nWinOrigX, m_nWinOrigY );
					// larry :: my damn multiplication factor
					tmpstar.x = tmpstar.x*m_nProjectionScale; //10;
					tmpstar.y = tmpstar.y*m_nProjectionScale; //10;
					// and now apply inverse trans
					m_pStarMatch->ApplyObjInv( tmpstar );

					xdiff = tmpstar.x-star.x;
					ydiff = tmpstar.y-star.y;

					star.b = sqrt( xdiff*xdiff + ydiff*ydiff );

//					star.a = m_vectIc[i].a;
//					star.b = m_vectIc[i].b;
					star.theta = m_pSky->m_pCatalogDsoIc->m_vectDso[i].pa;
					star.mag = m_pSky->m_pCatalogDsoIc->m_vectDso[i].mag;
					star.kronfactor = 1;

					// and now add this to vector of catalog object in the image
					pAstroImage->AddCatDso( star, nType );
				}
			}

		}
		// unload catalog - todo: find out why this is a problem
		//m_pSky->m_pCatalogDsoIc->UnloadCatalog();
	}

	///////////////////////////////////////////////////////////
	// TODO :: extend generalize this to all kind of dso catalogs
	// if flag set to use PGC??? na... galaxy catalog
	int bIsInRadius = 0;
	// CATALOG_ID_NONE = 0 - has to be more then that
	if( m_pConfig->m_nDsoCatalogToUse > 0 )
	{
		if( !m_pSky->m_pCatalogDso->m_vectDso && m_pSky->m_pCatalogDso->m_nDso <= 0 ) 
		{
			m_pSky->m_pCatalogDso->Load( m_pConfig->m_nDsoCatalogToUse, 
									m_pConfig->m_bDsoCatLoadLocalRegion, 
									pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec, 
									pAstroImage->m_nRadius );
		}
		//////////////////////
		// search dso catalog
		for( i=0; i<m_pSky->m_pCatalogDso->m_nDso; i++ )
		{
			DsoDefCatBasic* pDso = &(m_pSky->m_pCatalogDso->m_vectDso[i]);

			if( !m_pConfig->m_bDsoCatLoadLocalRegion )
			{
				////////////////////////////////////////////
				// calculate distance 
				nDistDeg = CalcSkyDistance( pDso->ra, pDso->dec, pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );
				if( nDistDeg <= pAstroImage->m_nRadius ) 
					bIsInRadius = 1;
				else
					bIsInRadius = 0;
			}

			// check if in the area at large first the with projection
/*			if( m_vectPGC[i].ra >= pAstroImage->m_nRaStart &&
				m_vectPGC[i].ra <= pAstroImage->m_nRaEnd &&
				m_vectPGC[i].dec >= pAstroImage->m_nDecStart &&
				m_vectPGC[i].dec <= pAstroImage->m_nDecEnd )
*/			
			if( m_pConfig->m_bDsoCatLoadLocalRegion || bIsInRadius )
			{
				star.ra = pDso->ra;
				star.dec = pDso->dec;

				// project star on tangent plan using the found orig
				m_pSky->ProjectStar( star, m_nWinOrigX, m_nWinOrigY );
				// larry :: my damn multiplication factor
				star.x = star.x*m_nProjectionScale; //10;
				star.y = star.y*m_nProjectionScale; //10;
				// and now apply inverse trans
				m_pStarMatch->ApplyObjInv( star );
	//				star.x = -star.x;

				//////////////////////////////////////////////
				// check if object is in my image
				if( star.x >= 0 && star.y >= 0 &&
					star.x <= pAstroImage->m_nWidth &&
					star.y <= pAstroImage->m_nHeight )
				{
					//////////////////////////////////
					// CHECK FOR MAPPING WITH OTHER CATS
					// if ngc/ic loaded then check if this pgc is ngc/ic and skip
					if( m_pConfig->m_bUseNgc  && m_pSky->m_pCatalogDsoAssoc->IsPgcObjectNgc( pDso->cat_no ) ) continue;
					if( m_pConfig->m_bUseIc && m_pSky->m_pCatalogDsoAssoc->IsPgcObjectIc( pDso->cat_no ) ) continue;
	
					// now check specificatly for messier
					unsigned long nMessierNo = m_pSky->m_pCatalogDsoAssoc->IsPgcObjectMessier( pDso->cat_no );
					if( nMessierNo )
					{
						strLog.Printf( wxT("INFO :: found Messier %d"), nMessierNo );
						m_pUnimapWorker->Log( strLog );

						nType = SKY_OBJECT_TYPE_DSO;
						// set dso cat no
						star.cat_type = CAT_OBJECT_TYPE_MESSIER;
						star.cat_no = nMessierNo;

					} else
					{
						// :: todo/check:why do I need here objtype in arguments ?
						m_pSky->m_pCatalogDsoNames->GetDsoSimpleCatName( *pDso, m_pSky->m_pCatalogDso->m_nObjectType, strName );
						strLog.Printf( wxT("INFO :: found galaxy %s"), strName );
						m_pUnimapWorker->Log( strLog );
						nType = SKY_OBJECT_TYPE_DSO;

						// set my star
						star.cat_type = pDso->cat_type; // ??? why this ??? m_pSky->m_pCatalogDso->m_nObjectType; //CAT_OBJECT_TYPE_PGC;
						star.cat_no = m_pSky->m_pCatalogDso->m_vectDso[i].cat_no;
					}

					///////////////////////////
					// calc projection for ellipse a/b
					ang = pDso->pa*PI/180;
					tmpstar.ra = pDso->ra+(pDso->a/60)*sin(ang);
					tmpstar.dec = pDso->dec+(pDso->a/60)*cos(ang);
//					tmpstar.dec = m_vectNgc[i].b*cos(m_vectNgc[i].pa);

					// project star on tangent plan using the found orig
					m_pSky->ProjectStar( tmpstar, m_nWinOrigX, m_nWinOrigY );
					// larry :: my damn multiplication factor
					tmpstar.x = tmpstar.x*m_nProjectionScale; //10;
					tmpstar.y = tmpstar.y*m_nProjectionScale; //10;
					// and now apply inverse trans
					m_pStarMatch->ApplyObjInv( tmpstar );

					xdiff = tmpstar.x-star.x;
					ydiff = tmpstar.y-star.y;

					star.a = sqrt( xdiff*xdiff + ydiff*ydiff );

					/////////// and for b is ::
					ang = (90.0-pDso->pa)*PI/180;
					tmpstar.ra = pDso->ra+(pDso->b/60)*sin(ang);
					tmpstar.dec = pDso->dec+(pDso->b/60)*cos(ang);
//					tmpstar.dec = m_vectNgc[i].b*cos(m_vectNgc[i].pa);

					// project star on tangent plan using the found orig
					m_pSky->ProjectStar( tmpstar, m_nWinOrigX, m_nWinOrigY );
					// larry :: my damn multiplication factor
					tmpstar.x = tmpstar.x*m_nProjectionScale; //10;
					tmpstar.y = tmpstar.y*m_nProjectionScale; //10;
					// and now apply inverse trans
					m_pStarMatch->ApplyObjInv( tmpstar );

					xdiff = tmpstar.x-star.x;
					ydiff = tmpstar.y-star.y;

					star.b = sqrt( xdiff*xdiff + ydiff*ydiff );

//					star.a = m_vectNgc[i].a;
//					star.b = m_vectNgc[i].b;

					star.theta = pDso->pa;
					star.mag = 0; // m_vectPGC[i].mag;
					star.kronfactor = 1;

					// and now add this to vector of catalog object in the image
					star.type = nType;
					pAstroImage->AddCatDso( star, nType );

				}

			}

		}
		// uload dso if region ???
		//if( m_pConfig->m_bDsoCatLoadLocalRegion && m_pSky->m_pCatalogDso->m_vectDso ) 
		m_pSky->m_pCatalogDso->UnloadCatalog( );
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	FindCatStars
// Class:	CSkyFinder
// Purpose:	find catalog stars using current star catalog catalog
// Input:	pointer to astro image
// Output:	status/how many stars found ?
/////////////////////////////////////////////////////////////////////
long CSkyFinder::FindCatStars( CAstroImage* pAstroImage )
{
	wxString strLog;
	unsigned long i=0;
	StarDef star;
	unsigned char nType = 0;
	double nDistDeg = 0;
	// some temp vars
	double ang = 0;
	double xdiff = 0;
	double ydiff = 0;

	// if flag set to use catalog stars ????
//	if( m_bShowcatStars )
//	{
//		if( !m_vectStar ) LoadCatStars( 0 );
		//////////////////////
		// search ngc
		for( i=0; i<m_pSky->m_pCatalogStars->m_nStar; i++ )
		{
			////////////////////////////////////////////
			// calculate distance 
			nDistDeg = CalcSkyDistance( m_pSky->m_pCatalogStars->m_vectStar[i].ra, m_pSky->m_pCatalogStars->m_vectStar[i].dec, 
								pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );

/*			if( m_vectNgc[i].ra >= pAstroImage->m_nRaStart &&
				m_vectNgc[i].ra <= pAstroImage->m_nRaEnd &&
				m_vectNgc[i].dec >= pAstroImage->m_nDecStart &&
				m_vectNgc[i].dec <= pAstroImage->m_nDecEnd )
*/
			if( nDistDeg <= pAstroImage->m_nRadius )
			{
				star.ra = m_pSky->m_pCatalogStars->m_vectStar[i].ra;
				star.dec = m_pSky->m_pCatalogStars->m_vectStar[i].dec;

				// project star using the found orig
				m_pSky->ProjectStar( star, pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );
				// larry :: my damn multiplication factor
				star.x = star.x*pAstroImage->m_nSkyProjectionScale; //10;
				star.y = star.y*pAstroImage->m_nSkyProjectionScale; //10;
				// and now apply inverse trans
				m_pStarMatch->ApplyObjInv( star );
	//				star.x = -star.x;

				// check if object is in my image
				if( star.x >= 0 && star.y >= 0 &&
					star.x <= pAstroImage->m_nWidth &&
					star.y <= pAstroImage->m_nHeight )
				{
//					sprintf( strLog, "INFO :: found cat star %d", m_vectStar[i].cat_no );
//					m_pUnimapWorker->Log( strLog );
					nType = SKY_OBJECT_TYPE_STAR;

					// set my star
					star.cat_type = m_pSky->m_pCatalogStars->m_vectStar[i].cat_type;
					star.cat_no = m_pSky->m_pCatalogStars->m_vectStar[i].cat_no;

//					star.a = m_vectStar[i].a;
//					star.b = m_vectStar[i].b;

					star.theta = 0; //m_vectStar[i].pa;

					star.mag = m_pSky->m_pCatalogStars->m_vectStar[i].mag;
					star.kronfactor = 1;

					// and now add this to vector of catalog object in the image
					pAstroImage->AddCatStar( star, nType );

				}

			}

		}
//	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	FindRadioSources
// Class:	CSkyFinder
// Purpose:	find radio sources using the current radio catalog
// Input:	pointer to astro image
// Output:	status/how many sources were found ?
/////////////////////////////////////////////////////////////////////
long CSkyFinder::FindRadioSources( CAstroImage* pAstroImage )
{
	wxString strLog;
	unsigned long i=0;
	StarDef star;
	unsigned char nType = 0;
	double nDistDeg = 0;
	// some temp vars
	double ang = 0;
	double xdiff = 0;
	double ydiff = 0;

	// if flag set to use radio catalog - CATALOG_ID_NONE = 0 has to be more then that
	if( m_pConfig->m_nRadioCatalogToUse > 0 )
	{
		if( !m_pSky->m_pCatalogRadio->m_vectData && m_pSky->m_pCatalogRadio->m_nData <= 0 ) 
		{
			m_pSky->m_pCatalogRadio->Load( m_pConfig->m_nRadioCatalogToUse, 
									m_pConfig->m_bRadioCatLoadLocalRegion, 
									pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec, 
									pAstroImage->m_nRadius );
		}

		// unload current radio sources in image
		pAstroImage->FreeCatRadioSources( );

		//////////////////////
		// search all radio sources from catalog
		for( i=0; i<m_pSky->m_pCatalogRadio->m_nData; i++ )
		{
			////////////////////////////////////////////
			// calculate distance 
			nDistDeg = CalcSkyDistance( m_pSky->m_pCatalogRadio->m_vectData[i].ra,
										m_pSky->m_pCatalogRadio->m_vectData[i].dec, 
										pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );

			// check if in my radius
			if( nDistDeg <= pAstroImage->m_nRadius )
			{
				star.ra = m_pSky->m_pCatalogRadio->m_vectData[i].ra;
				star.dec = m_pSky->m_pCatalogRadio->m_vectData[i].dec;

				// project star using the found orig
				m_pSky->ProjectStar( star, pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );
				// larry :: my damn multiplication factor
				star.x = star.x*pAstroImage->m_nSkyProjectionScale; //10;
				star.y = star.y*pAstroImage->m_nSkyProjectionScale; //10;
				// and now apply inverse trans
				m_pStarMatch->ApplyObjInv( star );

				/////////////////////////
				// check if object is in my image
				if( star.x >= 0 && star.y >= 0 &&
					star.x <= pAstroImage->m_nWidth &&
					star.y <= pAstroImage->m_nHeight )
				{
					strLog.Printf( wxT("INFO :: found radio source %d, name=%hs"), 
								m_pSky->m_pCatalogRadio->m_vectData[i].cat_no, 
								m_pSky->m_pCatalogRadio->m_vectData[i].cat_name );
					m_pUnimapWorker->Log( strLog );

					// set x/y
					m_pSky->m_pCatalogRadio->m_vectData[i].x = star.x;
					m_pSky->m_pCatalogRadio->m_vectData[i].y = star.y;

					// and now add this to vector of catalog object in the image
					pAstroImage->AddRadioSource( m_pSky->m_pCatalogRadio->m_vectData[i] );

				}

			}

		}
		// unload catalog
		m_pSky->m_pCatalogRadio->UnloadCatalog();
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	FindXGammaSources
// Class:	CSkyFinder
// Purpose:	find x-ray/gamma sources using the current catalog
// Input:	pointer to astro image
// Output:	status/how many sources were found ?
/////////////////////////////////////////////////////////////////////
long CSkyFinder::FindXGammaSources( CAstroImage* pAstroImage )
{
	wxString strLog;
	unsigned long i=0;
	StarDef star;
	unsigned char nType = 0;
	double nDistDeg = 0;
	// some temp vars
	double ang = 0;
	double xdiff = 0;
	double ydiff = 0;

	// if flag set to use x-ray/gamma catalog - CATALOG_ID_NONE = 0 has to be more then that
	if( m_pConfig->m_nXGammaCatalogToUse > 0 )
	{
		if( !m_pSky->m_pCatalogXGamma->m_vectData && m_pSky->m_pCatalogXGamma->m_nData <= 0 ) 
		{
			m_pSky->m_pCatalogXGamma->Load( m_pConfig->m_nXGammaCatalogToUse, 
									m_pConfig->m_bXGammaCatLoadLocalRegion, 
									pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec, 
									pAstroImage->m_nRadius );
		}

		// unload current radio sources in image
		pAstroImage->FreeCatXGammaSources( );

		//////////////////////
		// search all xray/gamma sources from catalog
		for( i=0; i<m_pSky->m_pCatalogXGamma->m_nData; i++ )
		{
			////////////////////////////////////////////
			// calculate distance 
			nDistDeg = CalcSkyDistance( m_pSky->m_pCatalogXGamma->m_vectData[i].ra,
										m_pSky->m_pCatalogXGamma->m_vectData[i].dec, 
										pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );

			// check if in my radius
			if( nDistDeg <= pAstroImage->m_nRadius )
			{
				star.ra = m_pSky->m_pCatalogXGamma->m_vectData[i].ra;
				star.dec = m_pSky->m_pCatalogXGamma->m_vectData[i].dec;

				// project star using the found orig
				m_pSky->ProjectStar( star, pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );
				// larry :: my damn multiplication factor
				star.x = star.x*pAstroImage->m_nSkyProjectionScale; //10;
				star.y = star.y*pAstroImage->m_nSkyProjectionScale; //10;
				// and now apply inverse trans
				m_pStarMatch->ApplyObjInv( star );

				/////////////////////////
				// check if object is in my image
				if( star.x >= 0 && star.y >= 0 &&
					star.x <= pAstroImage->m_nWidth &&
					star.y <= pAstroImage->m_nHeight )
				{
					strLog.Printf( wxT("INFO :: found x-ray/gamma source %d, name=%hs"), 
								m_pSky->m_pCatalogXGamma->m_vectData[i].cat_no, 
								m_pSky->m_pCatalogXGamma->m_vectData[i].cat_name );
					m_pUnimapWorker->Log( strLog );

					// set x/y
					m_pSky->m_pCatalogXGamma->m_vectData[i].x = star.x;
					m_pSky->m_pCatalogXGamma->m_vectData[i].y = star.y;

					// and now add this to vector of catalog object in the image
					pAstroImage->AddXGammaSource( m_pSky->m_pCatalogXGamma->m_vectData[i] );

				}

			}

		}
		// unload catalog
		m_pSky->m_pCatalogXGamma->UnloadCatalog();
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	FindSupernovas
// Class:	CSkyFinder
// Purpose:	find supernovas using the current catalog
// Input:	pointer to astro image
// Output:	status/how many supernovas were found ?
/////////////////////////////////////////////////////////////////////
long CSkyFinder::FindSupernovas( CAstroImage* pAstroImage )
{
	wxString strLog;
	unsigned long i=0;
	StarDef star, tmpstar;
	unsigned char nType = 0;
	double nDistDeg = 0;
	// some temp vars
	double ang = 0;
	double xdiff = 0;
	double ydiff = 0;

	// if flag set to use supernovas catalog - CATALOG_ID_NONE = 0 has to be more then that
	if( m_pConfig->m_nSNCatalogToUse > 0 )
	{
		if( !m_pSky->m_pCatalogSupernovas->m_vectData && m_pSky->m_pCatalogSupernovas->m_nData <= 0 ) 
		{
			m_pSky->m_pCatalogSupernovas->Load( m_pConfig->m_nSNCatalogToUse, 
									m_pConfig->m_bSNCatLoadLocalRegion, 
									pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec, 
									pAstroImage->m_nRadius );
		}

		// unload current supernovas in image
		pAstroImage->FreeCatSupernovas( );

		//////////////////////
		// search all supernovas from catalog
		for( i=0; i<m_pSky->m_pCatalogSupernovas->m_nData; i++ )
		{
			////////////////////////////////////////////
			// calculate distance 
			nDistDeg = CalcSkyDistance( m_pSky->m_pCatalogSupernovas->m_vectData[i].ra,
										m_pSky->m_pCatalogSupernovas->m_vectData[i].dec, 
										pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );

			// check if in my radius
			if( nDistDeg <= pAstroImage->m_nRadius )
			{
				star.ra = m_pSky->m_pCatalogSupernovas->m_vectData[i].ra;
				star.dec = m_pSky->m_pCatalogSupernovas->m_vectData[i].dec;

				// project star using the found orig
				m_pSky->ProjectStar( star, pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );
				// larry :: my damn multiplication factor
				star.x = star.x*pAstroImage->m_nSkyProjectionScale; //10;
				star.y = star.y*pAstroImage->m_nSkyProjectionScale; //10;
				// and now apply inverse trans
				m_pStarMatch->ApplyObjInv( star );

				/////////////////////////
				// check if object is in my image
				if( star.x >= 0 && star.y >= 0 &&
					star.x <= pAstroImage->m_nWidth &&
					star.y <= pAstroImage->m_nHeight )
				{
					strLog.Printf( wxT("INFO :: found Supernova %d, name=%s"), 
								m_pSky->m_pCatalogSupernovas->m_vectData[i].cat_no, 
								m_pSky->m_pCatalogSupernovas->m_vectData[i].cat_name );
					m_pUnimapWorker->Log( strLog );

					// set x/y
					m_pSky->m_pCatalogSupernovas->m_vectData[i].x = star.x;
					m_pSky->m_pCatalogSupernovas->m_vectData[i].y = star.y;

					// and now add this to vector of catalog object in the image
					pAstroImage->AddSupernova( m_pSky->m_pCatalogSupernovas->m_vectData[i] );

				}

			}

		}
		// unload catalog
		m_pSky->m_pCatalogSupernovas->UnloadCatalog();
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	FindBlackholes
// Class:	CSkyFinder
// Purpose:	find blackholes using the current catalog
// Input:	pointer to astro image
// Output:	status/how many blackholes were found ?
/////////////////////////////////////////////////////////////////////
long CSkyFinder::FindBlackholes( CAstroImage* pAstroImage )
{
	wxString strLog;
	unsigned long i=0;
	StarDef star, tmpstar;
	unsigned char nType = 0;
	double nDistDeg = 0;
	// some temp vars
	double ang = 0;
	double xdiff = 0;
	double ydiff = 0;

	// if flag set to use blackholes catalog - CATALOG_ID_NONE = 0 has to be more then that
	if( m_pConfig->m_nBHCatalogToUse > 0 )
	{
		if( !m_pSky->m_pCatalogBlackholes->m_vectData && m_pSky->m_pCatalogBlackholes->m_nData <= 0 ) 
		{
			m_pSky->m_pCatalogBlackholes->Load( m_pConfig->m_nBHCatalogToUse, 
									m_pConfig->m_bBHCatLoadLocalRegion, 
									pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec, 
									pAstroImage->m_nRadius );
		}

		// unload current blackholes in image
		pAstroImage->FreeCatBlackholes( );

		//////////////////////
		// search all blacholes from catalog
		for( i=0; i<m_pSky->m_pCatalogBlackholes->m_nData; i++ )
		{
			////////////////////////////////////////////
			// calculate distance 
			nDistDeg = CalcSkyDistance( m_pSky->m_pCatalogBlackholes->m_vectData[i].ra,
										m_pSky->m_pCatalogBlackholes->m_vectData[i].dec, 
										pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );

			// check if in my radius
			if( nDistDeg <= pAstroImage->m_nRadius )
			{
				star.ra = m_pSky->m_pCatalogBlackholes->m_vectData[i].ra;
				star.dec = m_pSky->m_pCatalogBlackholes->m_vectData[i].dec;

				// project star using the found orig
				m_pSky->ProjectStar( star, pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );
				// larry :: my damn multiplication factor
				star.x = star.x*pAstroImage->m_nSkyProjectionScale; //10;
				star.y = star.y*pAstroImage->m_nSkyProjectionScale; //10;
				// and now apply inverse trans
				m_pStarMatch->ApplyObjInv( star );

				/////////////////////////
				// check if object is in my image
				if( star.x >= 0 && star.y >= 0 &&
					star.x <= pAstroImage->m_nWidth &&
					star.y <= pAstroImage->m_nHeight )
				{
					strLog.Printf( wxT("INFO :: found Black Hole %d, name=%s"), 
								m_pSky->m_pCatalogBlackholes->m_vectData[i].cat_no, 
								m_pSky->m_pCatalogBlackholes->m_vectData[i].cat_name );
					m_pUnimapWorker->Log( strLog );

					// set x/y
					m_pSky->m_pCatalogBlackholes->m_vectData[i].x = star.x;
					m_pSky->m_pCatalogBlackholes->m_vectData[i].y = star.y;

					// and now add this to vector of catalog object in the image
					pAstroImage->AddBlackhole( m_pSky->m_pCatalogBlackholes->m_vectData[i] );

				}

			}

		}
		// unload catalog
		m_pSky->m_pCatalogBlackholes->UnloadCatalog();
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	FindMStars
// Class:	CSkyFinder
// Purpose:	find multiple/double stars using the current catalog
// Input:	pointer to astro image
// Output:	status/how many multiple/double stars were found ?
/////////////////////////////////////////////////////////////////////
long CSkyFinder::FindMStars( CAstroImage* pAstroImage )
{
	wxString strLog;
	unsigned long i=0;
	double nDistDeg = 0;
	double nX=0.0, nY=0.0;

	// get cat ref
	CSkyCatalogMStars* pCat = m_pSky->m_pCatalogMStars;

	// if flag set to use blackholes catalog - CATALOG_ID_NONE = 0 has to be more then that
	if( m_pConfig->m_nMSCatalogToUse > 0 )
	{
		if( !pCat->m_vectData && pCat->m_nData <= 0 ) 
		{
			pCat->Load( m_pConfig->m_nMSCatalogToUse, 
						m_pConfig->m_bMSCatLoadLocalRegion, 
						pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec, 
						pAstroImage->m_nRadius );
		}

		// unload current blackholes in image
		pAstroImage->FreeCatMStars( );

		//////////////////////
		// search all multiple/double stars from catalog
		for( i=0; i<pCat->m_nData; i++ )
		{
			// get object references
			DefCatMStars* pObj = &(pCat->m_vectData[i]);

			////////////////////////////////////////////
			// calculate distance 
			nDistDeg = CalcSkyDistance( pObj->ra, pObj->dec, pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );

			// check if in my radius
			if( nDistDeg <= pAstroImage->m_nRadius )
			{
				// project from sky
				m_pSky->ProjectSkyCoordToImageXY( pAstroImage, pObj->ra, pObj->dec, nX, nY );

				/////////////////////////
				// check if object is in my image
				if( nX >= 0 && nY >= 0 && nX <= pAstroImage->m_nWidth && nY <= pAstroImage->m_nHeight )
				{
					strLog.Printf( wxT("INFO :: found Multiple/Double Stars System %d, name=%s"), pObj->cat_no, wxString(pObj->cat_name,wxConvUTF8) );
					m_pUnimapWorker->Log( strLog );

					// set x/y
					pObj->x = nX;
					pObj->y = nY;

					// and now add this to vector of catalog object in the image
					pAstroImage->AddMStars( *pObj );
				}
			}
		}
		// unload catalog
		pCat->UnloadCatalog();
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	FindExoplanets
// Class:	CSkyFinder
// Purpose:	find extra-solar planets using the current catalog
// Input:	pointer to astro image
// Output:	status/how many exoplanets were found ?
/////////////////////////////////////////////////////////////////////
long CSkyFinder::FindExoplanets( CAstroImage* pAstroImage )
{
	wxString strLog;
	unsigned long i=0;
//	unsigned char nType = 0;
	double nDistDeg = 0;
	double nX=0.0, nY=0.0;

	// get cat ref
	CSkyCatalogExoplanets* pCat = m_pSky->m_pCatalogExoplanets;

	// if flag set to use exoplanets catalog - CATALOG_ID_NONE = 0 has to be more then that
	if( m_pConfig->m_nEXPCatalogToUse > 0 )
	{
		if( !pCat->m_vectData && pCat->m_nData <= 0 ) 
		{
			pCat->Load( m_pConfig->m_nEXPCatalogToUse, 
						m_pConfig->m_bEXPCatLoadLocalRegion, 
						pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec, 
						pAstroImage->m_nRadius );
		}

		// unload current exoplanets in image
		pAstroImage->FreeCatExoplanets( );

		//////////////////////
		// search all multiple/double stars from catalog
		for( i=0; i<pCat->m_nData; i++ )
		{
			// get object references
			DefCatExoPlanet* pObj = &(pCat->m_vectData[i]);

			////////////////////////////////////////////
			// calculate distance 
			nDistDeg = CalcSkyDistance( pObj->ra, pObj->dec, pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );

			// check if in my radius
			if( nDistDeg <= pAstroImage->m_nRadius )
			{
				// project from sky
				m_pSky->ProjectSkyCoordToImageXY( pAstroImage, pObj->ra, pObj->dec, nX, nY );

				/////////////////////////
				// check if object is in my image
				if( nX >= 0 && nY >= 0 && nX <= pAstroImage->m_nWidth && nY <= pAstroImage->m_nHeight )
				{
					strLog.Printf( wxT("INFO :: found Extra-Solar Planet %d, name=%s"), pObj->cat_no, pObj->cat_name );
					m_pUnimapWorker->Log( strLog );

					// set x/y
					pObj->x = nX;
					pObj->y = nY;

					// and now add this to vector of catalog object in the image
					pAstroImage->AddExoplanet( *pObj );
				}
			}
		}
		// unload catalog
		pCat->UnloadCatalog();
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	FindAsteroids
// Class:	CSkyFinder
// Purpose:	find asteroids using the current catalog
// Input:	pointer to astro image
// Output:	status/how many asteroids were found ?
/////////////////////////////////////////////////////////////////////
long CSkyFinder::FindAsteroids( CAstroImage* pAstroImage )
{
	wxString strLog;
	unsigned long i=0;
//	StarDef star, tmpstar;
	unsigned char nType = 0;
	double nDistDeg = 0;
	// coords
	double nX=0.0, nY=0.0;
	double nRa=0.0, nDec=0.0, nLat=pAstroImage->m_nObsLatitude, 
		nLon=pAstroImage->m_nObsLongitude, nAlt=pAstroImage->m_nObsAltitude;
	double timeTaken = pAstroImage->Get_UTC_JDN();

	// get cat ref
	CSkyCatalogAsteroids* pCat = m_pSky->m_pCatalogAsteroids;
	// if flag set to use asteroids catalog - CATALOG_ID_NONE = 0 has to be more then that
	if( m_pConfig->m_nASTCatalogToUse > 0 )
	{
		if( !pCat->m_vectData && pCat->m_nData <= 0 ) 
		{
			pCat->Load( m_pConfig->m_nASTCatalogToUse, 
						m_pConfig->m_bASTCatLoadLocalRegion, 
						pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec, 
						pAstroImage->m_nRadius );
		}

		// unload current asteroids in image
		pAstroImage->FreeCatAsteroids( );

		//////////////////////
		// search all asteroids from catalog
		for( i=0; i<pCat->m_nData; i++ )
		{
			// get object references
			DefCatAsteroid* pObj = &(pCat->m_vectData[i]);

			///////////////////////////////////////
			// GET ASTEROID POSITION IN THE SKY AT THE PICTURE TIME
			pCat->CalcRaDecX( *pObj, timeTaken, nLat, nLon, nAlt, nRa, nDec );
			// calculate distance 
			nDistDeg = CalcSkyDistance( nRa, nDec, pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );

			// check if in my radius
			if( nDistDeg <= pAstroImage->m_nRadius )
			{
				// project from sky
				m_pSky->ProjectSkyCoordToImageXY( pAstroImage, nRa, nDec, nX, nY );

				/////////////////////////
				// check if object is in my image
				if( nX >= 0 && nY >= 0 && nX <= pAstroImage->m_nWidth && nY <= pAstroImage->m_nHeight )
				{
					strLog.Printf( wxT("INFO :: found Asteroid %d, name=%hs"), pObj->cat_no, pObj->cat_name );
					m_pUnimapWorker->Log( strLog );

					// set x/y
					pObj->x = nX;
					pObj->y = nY;
					// set ra/dec
					pObj->ra = nRa;
					pObj->dec = nDec;


					// and now add this to vector of catalog object in the image
					pAstroImage->AddAsteroid( *pObj );

				}

			}
		}
		// unload catalog
		pCat->UnloadCatalog();
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	FindComets
// Class:	CSkyFinder
// Purpose:	find comets using the current catalog
// Input:	pointer to astro image
// Output:	status/how many comets were found ?
/////////////////////////////////////////////////////////////////////
long CSkyFinder::FindComets( CAstroImage* pAstroImage )
{
	wxString strLog;
	unsigned long i=0;
//	StarDef star, tmpstar;
	unsigned char nType = 0;
	double nDistDeg = 0;
	// coords
	double nX=0.0, nY=0.0;
	double nRa=0.0, nDec=0.0, nLat=pAstroImage->m_nObsLatitude, 
		nLon=pAstroImage->m_nObsLongitude, nAlt=pAstroImage->m_nObsAltitude;
	double timeTaken = pAstroImage->Get_UTC_JDN();

	// get cat ref
	CSkyCatalogComets* pCat = m_pSky->m_pCatalogComets;
	// if flag set to use comets catalog - CATALOG_ID_NONE = 0 has to be more then that
	if( m_pConfig->m_nCOMCatalogToUse > 0 )
	{
		if( !pCat->m_vectData && pCat->m_nData <= 0 ) 
		{
			pCat->Load( m_pConfig->m_nCOMCatalogToUse, 
							m_pConfig->m_bCOMCatLoadLocalRegion, 
							pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec, 
							pAstroImage->m_nRadius );
		}

		// unload current comets in image
		pAstroImage->FreeCatComets( );

		//////////////////////
		// search all comets from catalog
		for( i=0; i<pCat->m_nData; i++ )
		{
			// get object references
			DefCatComet* pObj = &(pCat->m_vectData[i]);

			///////////////////////////////////////
			// GET COMETS POSITION IN THE SKY AT THE PICTURE TIME
			pCat->CalcRaDec( *pObj, timeTaken, nLat, nLon, nAlt, nRa, nDec );

			// calculate distance 
			nDistDeg = CalcSkyDistance( nRa, nDec, pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );

			// check if in my radius
			if( nDistDeg <= pAstroImage->m_nRadius )
			{
				// project from sky
				m_pSky->ProjectSkyCoordToImageXY( pAstroImage, nRa, nDec, nX, nY );

				/////////////////////////
				// check if object is in my image
				if( nX >= 0 && nY >= 0 && nX <= pAstroImage->m_nWidth && nY <= pAstroImage->m_nHeight )
				{
					strLog.Printf( wxT("INFO :: found Comet %d, name=%s"), pObj->cat_no, pObj->cat_name );
					m_pUnimapWorker->Log( strLog );

					// set x/y
					pObj->x = nX;
					pObj->y = nY;
					// set ra/dec
					pObj->ra = nRa;
					pObj->dec = nDec;

					// and now add this to vector of catalog object in the image
					pAstroImage->AddComet( *pObj );

				}

			}

		}
		// unload catalog
		pCat->UnloadCatalog();
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	FindAes
// Class:	CSkyFinder
// Purpose:	find comets using the current catalog
// Input:	pointer to astro image
// Output:	status/how many comets were found ?
/////////////////////////////////////////////////////////////////////
long CSkyFinder::FindAes( CAstroImage* pAstroImage )
{
	wxString strLog;
	unsigned long i=0;
	double t=0.0;
	unsigned char nType = 0;
	double nDistDeg = 0;
	// coords
	double nRa=0.0, nDec=0.0, nX=0.0, nY=0.0, nSatDist=0.0;
	double nLat=pAstroImage->m_nObsLatitude;
	double nLon=pAstroImage->m_nObsLongitude; 
	double nAlt=pAstroImage->m_nObsAltitude;
	double nTemp = pAstroImage->m_nCondTemp;
	double nPressure = pAstroImage->m_nCondAtmPressure;
	wxDateTime myTime;

	////////////////////
	// time zone processing
//	double timeTaken = pAstroImage->m_nObsDateTime.GetJDN();
	double timeTaken = pAstroImage->Get_UTC_JDN();

	// time-frame range and division - 1h = 0.04166, 1m = 0.000694 -> fract = seconds/86400
	double nTimeFrameRange = (double) m_pConfig->m_nMatchLocateTimeFrameRange/86400.0;
	double nTimeFrameDiv = (double) m_pConfig->m_nMatchLocateTimeFrameDivision/86400.0;
	double nTimeFrameShiftEnd = (double) (m_pConfig->m_nMatchLocateTimeFrameRange+pAstroImage->m_nExpTotalTime)/86400.0;
	// start end time frame
	double nTimeFrameStart = timeTaken-nTimeFrameRange;
	double nTimeFrameEnd = timeTaken+nTimeFrameShiftEnd;
	// vars used for calc by method 1
	double nA_Ra=0.0, nA_Dec=0.0, nB_Ra=0.0, nB_Dec=0.0, nA_X=0.0, nA_Y=0.0, nB_X=0.0, nB_Y=0.0;

	// check error division zero
	if( nTimeFrameDiv == 0 ) return(-1);

	// get cat ref
	CSkyCatalogAes* pCat = m_pSky->m_pCatalogAes;
	// if flag set to use comets catalog - CATALOG_ID_NONE = 0 has to be more then that
	if( m_pConfig->m_bMatchLocateAes && m_pConfig->m_nAESCatalogToUse > 0 )
	{
		if( !pCat->m_vectData && pCat->m_nData <= 0 ) 
		{
			pCat->Load( m_pConfig->m_nAESCatalogToUse, 
						m_pConfig->m_bAESCatLoadLocalRegion, 
						pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec, 
						pAstroImage->m_nRadius );
		}

		// unload current satellites data in the image
		pAstroImage->FreeCatAes( );


		//////////////////////
		// search all satellites from catalog
		for( i=0; i<pCat->m_nData; i++ )
		{
			// get object references
			DefCatAes* pObj = &(pCat->m_vectData[i]);

			// DEBUG :: extract just ISS - skip all other
//			if( pObj->norad_cat_no != 25544 ) continue;

			// :: if object has no tle record - skip
			if( pObj->nTLERec == 0 ) 
				continue;

			// :: case when looking just for a specific object category
			if( m_pConfig->m_nAESCatObjCat > 0 && 
				!pCat->IsObjCategory( pObj, m_pConfig->m_nAESCatObjCat ) ) continue;

			// :: case when looking for a specific object type
			if( m_pConfig->m_nAESCatObjType > 0 && 
				m_pConfig->m_nAESCatObjType != pObj->obj_type ) continue;

			// :: case when not looking for debris or other specific object
			if( m_pConfig->m_bAESCatDontUseType && 
				( pObj->obj_type == ARTIFICIAL_EARTH_SATELLITE_DEBRIS || 
					pObj->obj_type == ARTIFICIAL_EARTH_MIXED_DEBRIS ) ) continue;

/*
			/////////////////
			// METHOD :: 1 - calculate ends and intersect with image - faster
			// LOOK HERE TO READJUST TIME WINDOW IF CYRRENT IS TO LARGE ?!!! (small doesn' matter)

			// aproximate number of seconds per obit ... use tle zero - high precision latter use closest tle
			double nSecondsPerOneOrbit = 86400.0/m_pSky->m_pCatalogAes->m_vectData[i].vectTLERec[0].mean_motion;
			double nWidthSecs = (nSecondsPerOneOrbit*pAstroImage->m_nRadius)/360.0;
			double nWidthPerc = nWidthSecs/86400.0;

//			for( t=-30; t<30; t++ )
//			{
			t=0.0;

			nTimeFrameStart = timeTaken+t*nWidthPerc-nWidthPerc;
			nTimeFrameEnd = timeTaken+t*nWidthPerc+(pAstroImage->m_nExpTotalTime+nWidthSecs)/86400.0;

//			nTimeFrameStart = timeTaken-300.0/86400.0;
//			nTimeFrameEnd = timeTaken+300.0/86400.0;

			// initialize TLE epoch anchor 
			m_pSky->m_pCatalogAes->InitObjTleEpoch( &(m_pSky->m_pCatalogAes->m_vectData[i]), nTimeFrameStart, nTimeFrameEnd );

			// GET SATELLITE POSITION IN THE SKY AT START TIME - POINT A
			m_pSky->m_pCatalogAes->CalcRaDec( m_pSky->m_pCatalogAes->m_vectData[i], nTimeFrameStart, 
																		nLat, nLon, nAlt, nA_Ra, nA_Dec );
			// project A from sky
			m_pSky->ProjectSkyCoordToImageXY( pAstroImage, nA_Ra, nA_Dec, nA_X, nA_Y );

			// GET SATELLITE POSITION IN THE SKY AT START TIME - POINT B
			m_pSky->m_pCatalogAes->CalcRaDec( m_pSky->m_pCatalogAes->m_vectData[i], nTimeFrameEnd, 
																		nLat, nLon, nAlt, nB_Ra, nB_Dec );
			// project B from sky
			m_pSky->ProjectSkyCoordToImageXY( pAstroImage, nB_Ra, nB_Dec, nB_X, nB_Y );

			int nIA_X=0.0, nIA_Y=0.0, nIB_X=0.0, nIB_Y=0.0;
			// calculate line intersect image
			if( IsLineIntersectRectZeroAx( (int) round(nA_X), (int) round(nA_Y), 
											(int) round(nB_X), (int) round(nB_Y),
											pAstroImage->m_nWidth, pAstroImage->m_nHeight,
											nIA_X, nIA_Y, nIB_X, nIB_Y ) )
			{
				//double nTimeFrameTs = m_pConfig->m_nMatchLocateTimeFrameRange*2.0+pAstroImage->m_nExpTotalTime;
				double nTimeFrameTs = nWidthSecs*2.0+pAstroImage->m_nExpTotalTime;
				//double nTimeFrameTs = 600.0;

				// find which intersection point is close to A
				double nOrbSegA_IA = sqrt( sqr(nIA_X-nA_X) + sqr(nIA_Y-nA_Y) );
				double nOrbSegA_IB = sqrt( sqr(nIB_X-nA_X) + sqr(nIB_Y-nA_Y) );
				if( nOrbSegA_IA > nOrbSegA_IB ) 
					nOrbSegA_IA = nOrbSegA_IB;

				double nOrbLen = sqrt( sqr(nB_X-nA_X) + sqr(nB_Y-nA_Y) );
				double nOrbSegLen = sqrt( sqr(nIB_X-nIA_X) + sqr(nIB_Y-nIA_Y) );
				
				double nOrbSegStartTime = (nOrbSegA_IA*nTimeFrameTs)/nOrbLen;
				double nOrbSegTime = (nOrbSegLen*nTimeFrameTs)/nOrbLen;
				double nOrbSegTimeCenter = nOrbSegStartTime+nOrbSegTime/2.0;
				double nTimeShiftPerc = nOrbSegTimeCenter/86400.0;

				/////////////////////////
				// GET SATELLITE POSITION IN THE SKY AT START TIME - POINT A
				m_pSky->m_pCatalogAes->CalcRaDec( m_pSky->m_pCatalogAes->m_vectData[i], nTimeFrameStart+nTimeShiftPerc, 
																			nLat, nLon, nAlt, nRa, nDec );
				// project from sky
				m_pSky->ProjectSkyCoordToImageXY( pAstroImage, nRa, nDec, nX, nY );

				// here TODO: adjust time based of velocity estimation for mid segment in image
				myTime.Set( nTimeFrameStart+nTimeShiftPerc );
				sprintf( strLog, "INFO :: found Artificial Eart Satellite %d, name=%s, date_time=%s", 
							m_pSky->m_pCatalogAes->m_vectData[i].cat_no, 
							m_pSky->m_pCatalogAes->m_vectData[i].cat_name, myTime.Format().ToAscii() );
				m_pUnimapWorker->Log( strLog );

				// set x/y
				m_pSky->m_pCatalogAes->m_vectData[i].x = nX;
				m_pSky->m_pCatalogAes->m_vectData[i].y = nY;
				// set ra/dec
				m_pSky->m_pCatalogAes->m_vectData[i].ra = nRa;
				m_pSky->m_pCatalogAes->m_vectData[i].dec = nDec;
				// set time shift
				m_pSky->m_pCatalogAes->m_vectData[i].detect_time_shift = nTimeFrameStart+nTimeShiftPerc-timeTaken;

				// and now add this to vector of catalog object in the image
				pAstroImage->AddAes( m_pSky->m_pCatalogAes->m_vectData[i] );

				// exit this loop 
				//break;

			} else
				continue;

//			}
*/

			/////////////////
			// METHOD :: 2 - scan step by step - slower

			// initialize TLE epoch anchor 
			pCat->InitObjTleEpoch( pObj, nTimeFrameStart, nTimeFrameEnd );

			/////////////////////////////
			// scan expore time + config time-frame range
			// RECOMPUTE TIME STEPS HERE !!! HOW FINE DO I ACTUALLY NEED TO LOOK ?!!!! - USE SAT SPEED!!!!
			for( t=-nTimeFrameRange; t<=nTimeFrameShiftEnd; t+=nTimeFrameDiv )
			{
				///////////////////////////////////////
				// GET SATELLITE POSITION IN THE SKY AT THE PICTURE TIME
				pCat->CalcRaDec( *pObj, timeTaken+t, nLat, nLon, nAlt, nRa, nDec, nSatDist );

				////////////////////////////////////////////
				// calculate distance 
//	//			nRa *= RAD2DEG;
//	//			nDec *= RAD2DEG;
				nDistDeg = CalcSkyDistance( nRa, nDec, pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );

				// check if in my radius
				if( nDistDeg <= pAstroImage->m_nRadius )
				{
					// project from sky
					m_pSky->ProjectSkyCoordToImageXY( pAstroImage, nRa, nDec, nX, nY );

					/////////////////////////
					// check if object is in my image
					if( nX >= 0 && nY >= 0 && nX <= pAstroImage->m_nWidth && nY <= pAstroImage->m_nHeight )
					{
						myTime.Set( timeTaken+t );
						strLog.Printf( wxT("INFO :: found Artificial Eart Satellite %d, name=%hs, date_time=%s"), 
									pObj->cat_no, 
									pObj->cat_name, myTime.Format() );
						m_pUnimapWorker->Log( strLog );

						// set x/y
						pObj->x = nX;
						pObj->y = nY;
						// set ra/dec
						pObj->ra = nRa;
						pObj->dec = nDec;
						// set time shift
						pObj->detect_time_shift = t;

						// set sat distance from obs
						pObj->dist_from_obs = nSatDist;

						// and now add this to vector of catalog object in the image
						pAstroImage->AddAes( *pObj );

						// exit this loop 
						break;
					}

				}
			}

		}
		// unload catalog
		pCat->UnloadCatalog( );

	} else
		return( 0 );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	FindSolarPlanets
// Class:	CSkyFinder
// Purpose:	find solar planets using the current catalog
// Input:	pointer to astro image
// Output:	status/how many comets were found ?
/////////////////////////////////////////////////////////////////////
long CSkyFinder::FindSolarPlanets( CAstroImage* pAstroImage )
{
	wxString strLog;
	int i=0, bVis=0;
	double t=0.0;
	unsigned char nType = 0;
	double nDistDeg = 0;
	// coords
	double nRa=0.0, nDec=0.0, nX=0.0, nY=0.0, nSatDist=0.0;
	double nLat=pAstroImage->m_nObsLatitude, nLon=pAstroImage->m_nObsLongitude, 
			nAlt=pAstroImage->m_nObsAltitude;
	wxDateTime myTime;

	////////////////////
	// time zone processing
	double timeTaken = pAstroImage->Get_UTC_JDN();
	// time-frame range and division - 1h = 0.04166, 1m = 0.000694 -> fract = seconds/86400
	double nTimeFrameRange = (double) m_pConfig->m_nMatchLocateTimeFrameRange/86400.0;
	double nTimeFrameDiv = (double) m_pConfig->m_nMatchLocateTimeFrameDivision/86400.0;
	double nTimeFrameShiftEnd = (double) (m_pConfig->m_nMatchLocateTimeFrameRange+pAstroImage->m_nExpTotalTime)/86400.0;
	// start end time frame
	double nTimeFrameStart = timeTaken-nTimeFrameRange;
	double nTimeFrameEnd = timeTaken+nTimeFrameShiftEnd;

	// define our solar planets+sun+moon
	DefPlanet vectPlanets[40];
	InitSolarPlanets( vectPlanets );

	// get cat ref
	CSkyCatalogAes* pCat = m_pSky->m_pCatalogAes;

	// for each solar planet/sun/moon
	for( i=0; i<g_nSolarObjects; i++ )
	{
		///////////////////////////////
		// CASE :: natural satellites of planets
		if( vectPlanets[i].obj_type == 1 )
		{
			DefPlanet* pPlanet = GetSolarPlanetObjById( vectPlanets, vectPlanets[i].obj_orb_id );
			if( !pPlanet ) continue;

			double timeFound = timeTaken+pPlanet->detect_time_shift;
			SolarPlanet_CalcRaDec( vectPlanets[i], timeFound, nLat, nLon, nAlt, nRa, nDec, nSatDist, bVis );
			// visibilty check
			if( bVis == 0 ) continue;

			// calculate distance 
			nDistDeg = CalcSkyDistance( nRa, nDec, pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );

			// check if in my radius
			if( nDistDeg <= pAstroImage->m_nRadius )
			{
				// project from sky
				m_pSky->ProjectSkyCoordToImageXY( pAstroImage, nRa, nDec, nX, nY );

				/////////////////////////
				// check if object is in my image
				if( nX >= 0 && nY >= 0 && nX <= pAstroImage->m_nWidth && nY <= pAstroImage->m_nHeight )
				{
					myTime.Set( timeFound );
					strLog.Printf( wxT("INFO :: found Planet %d, name=%s, date_time=%s"), 
								vectPlanets[i].obj_id, 
								vectPlanets[i].obj_name, myTime.Format() );
					m_pUnimapWorker->Log( strLog );

					// set x/y
					vectPlanets[i].x = nX;
					vectPlanets[i].y = nY;
					// set ra/dec
					vectPlanets[i].ra = nRa;
					vectPlanets[i].dec = nDec;
					// set time shift
					vectPlanets[i].detect_time_shift = pPlanet->detect_time_shift;

					// set sat distance from obs
					vectPlanets[i].dist_from_obs = nSatDist;

					// and now add this to vector of catalog object in the image
					pAstroImage->AddSolarPlanet( vectPlanets[i] );

					// exit this loop 
					//break;
				}
			}
			continue;
		}

		////////////////////////////////
		// CASE :: for planets - just search
		//for( t=-0.07; t<0.07; t+=0.0001 )
		for( t=-nTimeFrameRange; t<=nTimeFrameShiftEnd; t+=nTimeFrameDiv )
		{

			// calculate ra/dec
			SolarPlanet_CalcRaDec( vectPlanets[i], timeTaken+t, nLat, nLon, nAlt, nRa, nDec, nSatDist, bVis );

			// visibilty check
//			if( bVis == 0 ) continue;

			// calculate distance 
			nDistDeg = CalcSkyDistance( nRa, nDec, pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );

			// check if in my radius
			if( nDistDeg <= pAstroImage->m_nRadius )
			{
				// project from sky
				m_pSky->ProjectSkyCoordToImageXY( pAstroImage, nRa, nDec, nX, nY );

				/////////////////////////
				// check if object is in my image
				if( nX >= 0 && nY >= 0 && nX <= pAstroImage->m_nWidth && nY <= pAstroImage->m_nHeight )
				{
					myTime.Set( timeTaken+t );
					strLog.Printf( wxT("INFO :: found Planet %d, name=%s, date_time=%s"), 
								vectPlanets[i].obj_id, 
								vectPlanets[i].obj_name, myTime.Format() );
					m_pUnimapWorker->Log( strLog );

					// set x/y
					vectPlanets[i].x = nX;
					vectPlanets[i].y = nY;
					// set ra/dec
					vectPlanets[i].ra = nRa;
					vectPlanets[i].dec = nDec;
					// set time shift
					vectPlanets[i].detect_time_shift = t;

					// set sat distance from obs
					vectPlanets[i].dist_from_obs = nSatDist;

					// and now add this to vector of catalog object in the image
					pAstroImage->AddSolarPlanet( vectPlanets[i] );

					// exit this loop 
					break;
				}
			}

		}
		// unload catalog
		pCat->UnloadCatalog();

	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	CalcNoMatchObjProjection
// Class:	CSkyFinder
// Purpose:	calculate image project in the sky
// Input:	pointer to astro image
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyFinder::CalcNoMatchObjProjection( CAstroImage* pAstroImage )
{
	long i = 0;
	double x =0, y = 0;
	double x0 = 0, y0 = 0;
	StarDef star;

	// for every star in my input vector
	for( i=0; i<pAstroImage->m_nStar; i++ )
	{
		// if there was no match
		if( pAstroImage->m_vectStar[i].match != 1 )
		{
			x = pAstroImage->m_vectStar[i].x;
			y = pAstroImage->m_vectStar[i].y;

			m_pStarMatch->CalcPointTrans( x, y );

			// de-project coord from image to
			x0 = pAstroImage->m_nOrigRa;
			y0 = pAstroImage->m_nOrigDec;
//			StarDef star;
			star.x = x/m_nProjectionScale;
			star.y = y/m_nProjectionScale;

//			x /= m_nProjectionScale;
//			y /= m_nProjectionScale;

			// de-project
//			starTmp.x=x; starTmp.y=y;
			m_pSky->DeprojectStar( star, x0, y0 );

			pAstroImage->m_vectStar[i].ra = star.ra;
			pAstroImage->m_vectStar[i].dec = star.dec;
		}
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	CalcImgSkyProjection
// Class:	CSkyFinder
// Purpose:	calculate image project in the sky
// Input:	pointer to astro image
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyFinder::CalcImgSkyProjection( CAstroImage* pAstroImage,
										double nCenterRA, double nCenterDEC,
										double& startRA, double& endRA,
										double& startDEC, double& endDEC, 
										double& fieldWidth, double& fieldHeight, double& fieldRadius )
{
	AdjustRaDec( nCenterRA, nCenterDEC );

	double nRa1 = 0, nDec1 = 0, nRa2 = 0, nDec2 = 0, nRa3 = 0, nDec3 = 0, nRa4 = 0, nDec4 = 0;
	// Here should work better now since i fixed the y=-y in de project star

	////////////////////////////////////////////
	// this part is where i calculate the corners of projected window
	double	x1=0, y1=0,
			x3=pAstroImage->m_nWidth, y3=pAstroImage->m_nHeight,
			x2=x3, y2=0,
			x4=0, y4=y3;

	// transform window using inverse
	m_pStarMatch->CalcWindowTrans( x1, y1, x2, y2, x3, y3, x4, y4 );

	// do the scale thing
//		double nRevWinScale = (double) (pAstroImage->m_nWidth/(nMaxX-nMinX));
	x1 /= m_nProjectionScale;
	y1 /= m_nProjectionScale;

	x2 /= m_nProjectionScale;
	y2 /= m_nProjectionScale;

	x3 /= m_nProjectionScale;
	y3 /= m_nProjectionScale;

	x4 /= m_nProjectionScale;
	y4 /= m_nProjectionScale;

	StarDef starTmp;

	// do corner 1
	starTmp.x=x1; starTmp.y=y1;
	m_pSky->DeprojectStar( starTmp, nCenterRA, nCenterDEC );
	double minRa = starTmp.ra, minDec = starTmp.dec,
			maxRa = starTmp.ra, maxDec = starTmp.dec;
	nRa1 = starTmp.ra;
	nDec1 = starTmp.dec;
	// do corner 2
	starTmp.x=x2; starTmp.y=y2;
	m_pSky->DeprojectStar( starTmp, nCenterRA, nCenterDEC );
	nRa2 = starTmp.ra;
	nDec2 = starTmp.dec;
	if( starTmp.ra < minRa ) minRa=starTmp.ra;
	if(	starTmp.dec < minDec ) minDec=starTmp.dec;
	if( starTmp.ra > maxRa ) maxRa=starTmp.ra;
	if(	starTmp.dec > maxDec ) maxDec=starTmp.dec;
	// do corner 3
	starTmp.x=x3; starTmp.y=y3;
	m_pSky->DeprojectStar( starTmp, nCenterRA, nCenterDEC );
	nRa3 = starTmp.ra;
	nDec3 = starTmp.dec;
	if( starTmp.ra < minRa ) minRa=starTmp.ra;
	if(	starTmp.dec < minDec ) minDec=starTmp.dec;
	if( starTmp.ra > maxRa ) maxRa=starTmp.ra;
	if(	starTmp.dec > maxDec ) maxDec=starTmp.dec;
	// do corner 4
	starTmp.x=x4; starTmp.y=y4;
	m_pSky->DeprojectStar( starTmp, nCenterRA, nCenterDEC );
	nRa4 = starTmp.ra;
	nDec4 = starTmp.dec;
	if( starTmp.ra < minRa ) minRa=starTmp.ra;
	if(	starTmp.dec < minDec ) minDec=starTmp.dec;
	if( starTmp.ra > maxRa ) maxRa=starTmp.ra;
	if(	starTmp.dec > maxDec ) maxDec=starTmp.dec;

	/////////////////////////////////////////
	// now recalc my origin - or just consider the middle of the frame for now
//	double nOrigRa = (double) pAstroImage->m_nWidth/2;
//	double nOrigDec = (double) pAstroImage->m_nHeight/2;
//	m_pStarMatch->CalcPointTrans( nOrigRa, nOrigDec );
//	nOrigRa /= m_nProjectionScale;
//	nOrigDec /= m_nProjectionScale;
//	starTmp.x=nOrigRa; starTmp.y=nOrigDec;
//	DeprojectStar( starTmp, nFoundX, nFoundY );
//	nOrigRa = starTmp.ra;
//	nOrigDec = starTmp.dec;

	//////////////////////////////////////////
	// and here i set my astro image
	// larry fix 1
	if( maxRa > 360. ) maxRa -= 360.;
	if( minRa > 360. ) minRa -= 360.;
	if( minRa > maxRa )
	{
		double tt = minRa;
		minRa = maxRa;
		maxRa = tt;
	}

	// - this is debuging ...
	double err = 0;
	if( maxDec < -90. || maxDec > 90. || minDec < -90. || minDec > 90. )
		err = 1;

	// make the adjustments to the coord system
	AdjustRaDec( nCenterRA, nCenterDEC );
	AdjustRaDec( nCenterRA, nCenterDEC );

	// now set return
	startRA = minRa;
	endRA = maxRa;
	startDEC = minDec;
	endDEC = maxDec;
	// and also set the field width/height
	double nW1 = CalcSkyDistance( nRa1, nDec1, nRa2, nDec2 );
	double nW2 = CalcSkyDistance( nRa4, nDec4, nRa3, nDec3 );
	fieldWidth = nW1;
	if( nW2 > fieldWidth ) fieldWidth = nW2;
	// and also het max of height
	double nH1 = CalcSkyDistance( nRa1, nDec1, nRa4, nDec4 );
	double nH2 = CalcSkyDistance( nRa2, nDec2, nRa3, nDec3 );
	fieldHeight = nH1;
	if( nH2 > fieldHeight ) fieldHeight = nH2; 

//	fieldRadius = CalcSkyDistance( nRa1, nDec1, nCenterRA, nCenterDEC );

	double f1 = CalcSkyDistance( nRa1, nDec1, nCenterRA, nCenterDEC );
	double f2 = CalcSkyDistance( nRa2, nDec2, nCenterRA, nCenterDEC );
	double f3 = CalcSkyDistance( nRa3, nDec3, nCenterRA, nCenterDEC );
	double f4 = CalcSkyDistance( nRa4, nDec4, nCenterRA, nCenterDEC );
	fieldRadius = f1;
	if( f2 > fieldRadius ) fieldRadius = f2;
	if( f3 > fieldRadius ) fieldRadius = f3;
	if( f4 > fieldRadius ) fieldRadius = f4;

	return( 1 );
}
