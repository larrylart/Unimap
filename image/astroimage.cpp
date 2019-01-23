////////////////////////////////////////////////////////////////////
// Astronomical Image Class
// Updated:	12/10/2009
////////////////////////////////////////////////////////////////////

//system headers
#include <math.h>
#include <stdio.h>
#include <sys/timeb.h>

// math libs
#include <vector>
#include <list>
#include <limits>
#include <algorithm>
#include <functional>
#include <numeric>

// opencv includes
//#include "highgui.h"
//#include "cv.h"
//#include "cvaux.h"

// WX includes
#include "wx/wxprec.h"
#include <wx/regex.h>
#include <wx/image.h>
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/tokenzr.h>
#include <wx/filename.h>

/////////
// exiv2 library - for image details extraction
#include "../lib/exiv2-0.20/msvc/include/exiv2/image.hpp"
#include "../lib/exiv2-0.20/msvc/include/exiv2/exif.hpp"
#include <iostream>
#include <iomanip>
#include <cassert>

////////////////////
// eLynx
#include <elx/core/CoreMacros.h>
#include <elx/math/MathCore.h>
#include <elx/math/TransfertFunction.h>
//#include <elx/image/ImageVariant.h>
#include <elx/image/AbstractImageManager.h>
#include <elx/image/ImageFileManager.h>
#include <elx/image/IImageFilePlugin.h>
#include <elx/image/PixelIterator.h>
// also include my custom fits reader
#include "format/image_fits.h"

// local headers
#include "../util/folders.h"
#include "../util/func.h"
#include "../util/geometry.h"
#include "../util/crypt-md5.h"
#include "digitalimage.h"
#include "imagegroup.h"
//#include "imagearea.h"
#include "image_note.h"
#include "../logger/logger.h"
#include "../sky/sky.h"
#include "../sky/catalog_stars.h"
#include "../sky/catalog_stars_names.h"
#include "../sky/catalog_stars_assoc.h"
#include "../sky/catalog_dso.h"
#include "../sky/catalog_dso_names.h"
#include "../sky/catalog_dso_assoc.h"
#include "../config/mainconfig.h"
#include "../config/detectconfig.h"
#include "../land/earth.h"

#include "../match/starmatch.h"
#define NOCTYPES
#include "../proc/sextractor.h"
#undef NOCTYPES

// main header
#include "astroimage.h"

using namespace eLynx;
using namespace eLynx::Math;
using namespace eLynx::Image;

////////////////////////////////////////////////////////////////////
// Method:		Constructor
// Class:		CAstroImage
// Purpose:		build my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
CAstroImage::CAstroImage( CImageGroup* pImageGroup, const wxString& strImagePath,
							const wxString& strImageName, const wxString& strImageExt )
{
//	m_strImageName = (char*) malloc( strlen(strImageName)+1 );
//	m_strImagePath = (char*) malloc( strlen(strImagePath)+1 );

	m_strImagePath = strImagePath;
	m_strImageName = strImageName;
	m_strImageExt = strImageExt;
	m_pImageGroup = pImageGroup;

	// init matching params
	m_pBestTrans = (TRANS*) malloc( sizeof(TRANS) );
	m_pInvTrans = (TRANS*) malloc( sizeof(TRANS) );

	ResetParams();

//	wxInitAllImageHandlers();
}

////////////////////////////////////////////////////////////////////
// Method:		Destructor
// Class:		CAstroImage
// Purpose:		destroy my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
CAstroImage::~CAstroImage()
{
	int i = 0;

	// first we save
//	SaveProfile( );

	// then we free data - image buffer
	FreeData( );

//	free( m_strImageName );
//	free( m_strImagePath );

	// we alloc this in the constructor and we free in destructor
	free( m_pBestTrans );
	free( m_pInvTrans );
}

////////////////////////////////////////////////////////////////////
void CAstroImage::ResetParams()
{
	// reset the id to -1
	m_nId = -1;

	m_nFileTime = 0;
	m_pImage = NULL;
	m_pImgFmtFits = NULL;
	m_pLogger = NULL;

	m_pImage = NULL;
	m_pLogger = NULL;
	m_pSky = NULL;

	m_vectImageBuff = NULL;

	// initialize detect profile
	m_nImgSourceType = 0;
	m_nUseChannel = 0;

	// initialize my vector components
	m_vectArea.clear();
	// :: stars
	m_vectStar = NULL;
	m_nStar = 0;
	m_nStarAllocated = 0;
	// :: catalog stars
	m_vectCatStar = NULL;
	m_nCatStarAllocated = 0;
	m_nCatStar = 0;
	// :: dso
	m_vectCatDso = NULL;
	m_nCatDso = 0;
	m_nCatDsoAllocated = 0;
	// :: radio
	m_vectCatRadio = NULL;
	m_nCatRadio = 0;
	m_nCatRadioAllocated = 0;
	// :: xray/gamma
	m_vectCatXGamma = NULL;
	m_nCatXGamma = 0;
	m_nCatXGammaAllocated = 0;
	// :: supernovas
	m_vectCatSupernova = NULL;
	m_nCatSupernova = 0;
	m_nCatSupernovaAllocated = 0;
	// :: blackholes
	m_vectCatBlackhole = NULL;
	m_nCatBlackhole = 0;
	m_nCatBlackholeAllocated = 0; 
	// :: multiple/double stars
	m_vectCatMStars = NULL;
	m_nCatMStars = 0;
	m_nCatMStarsAllocated = 0;
	// :: extra-solar planets
	m_vectCatExoplanet = NULL;
	m_nCatExoplanet = 0;
	m_nCatExoplanetAllocated = 0;
	// :: asteroids
	m_vectCatAsteroid = NULL;
	m_nCatAsteroid = 0;
	m_nCatAsteroidAllocated = 0;
	// :: comets
	m_vectCatComet = NULL; 
	m_nCatComet = 0;
	m_nCatCometAllocated = 0;
	// :: AES
	m_vectCatAes = NULL;
	m_nCatAes = 0;
	m_nCatAesAllocated = 0;
	// :: SOLAR PLANET/MOON/SUN
	m_vectSolarPlanets = NULL;
	m_nSolarPlanets = 0;
	m_nSolarPlanetsAllocated = 0;

	// init hint vars
	m_nUseHint = 0;
	m_nHintOrigType = 0;
	m_nHintObjOrigType = 0;
	m_nHintObjName = 0;
	m_strHintObjId = wxT("");
	m_nHintOrigUnits = 0;
	m_nHintFieldType = 0;
	m_nHintSetupId = 0;
	// camera data - to move in image properties
	m_nHintCameraSource = 0;
	m_nHintCameraType = 0;
	m_nHintCameraBrand = 0;
	m_nHintCameraName = 0;
	// sensor
	m_nHintSensorWidth = 0.0;
	m_nHintSensorHeight = 0.0;
	// init tlens
	m_nHintTLensSource = 0;
	memset( &m_rHintTLensSetup, 0, sizeof(DefTLensSetup) );

	// initialize flags
	m_bIsChanged = 0;
	m_nIsRelativeCenter = 0;
	m_nIsRelativeField = 0;
	m_bIsDetected = 0;
	m_bIsMatched = 0;

	// init match variables
	m_nMatchedStars = 0;
	m_nRaStart = 0;
	m_nRaEnd = 0;
	m_nDecStart = 0;
	m_nDecEnd = 0;
	m_nOrigRa = 0;
	m_nOrigDec = 0;
	m_nFieldWidth = 0;
	m_nFieldHeight = 0;
	m_nRadius = 0;
	m_nImgRotAngle = 0;
	m_nMagnitudeToRadiusRatio = 0;
	m_nImgTransScale = 1;
	m_nMaxMag = 0;//DBL_MAX;
	m_nMinMag = 0;//DBL_MIN;

	// other img porperties
	// observer
	m_strObsName = wxT( "" );
	m_strObsCountry = wxT( "" );
	m_strObsRegion = wxT( "" );
	m_strObsCity = wxT( "" );
//	m_chObsLatitude=0;
//	m_chObsLongitude=0;
	m_nObsLatitude = 0.0;
	m_nObsLongitude = 0.0;
	m_nObsAltitude = 0.0;
	/////////
	// date-time :: set default to UTC and current time
	m_nObsDateTime.SetToCurrent();
	m_nObsDateTime.MakeUTC();

	// target
	m_strTargetName = wxT("" );
	m_strTargetDesc = wxT("" );
	m_nTargetRa = 0;
	m_nTargetDec = 0;
	// equipment
	m_strTelescopeName = wxT( "" );
	m_nTelescopeType = 0;
	m_nTelescopeFocal = 0.0;
	m_nTelescopeAperture = 0.0;
	m_nTelescopeMount = 0;
	m_strCameraName = wxT( "" );
	// conditions
	m_nCondSeeing = 0;
	m_nCondTransp = 0;
	m_nCondWindSpeed = 0;
	m_nCondAtmPressure = 0.0;
	m_nCondTemp = 0;
	m_nCondTempVar = 0;
	m_nCondHumid = 0;
	m_nCondLp = 0;
	// imaging
	m_nExpTotalTime = 0;
	m_nNoOfExp = 0;
	m_nExpIso = 0;
	m_strRegProg = wxT( "" );
	m_strStackProg = wxT( "" );
	m_nStackMethod = 0;
	m_strProcProg = wxT( "" );

	// init constelations
	m_vectConstellation = NULL;
	m_nConstellation = 0;
	m_nConstellationAllocated = 0;

	memset( m_pBestTrans, 0, sizeof(TRANS) );
	memset( m_pInvTrans, 0, sizeof(TRANS) );
	m_nSkyProjectionScale = 0;
	m_nCatalogForMatching = -1;
	// reset grid
	m_bIsDistGrid = 0;
	m_vectDistGridX = NULL;
	m_vectDistGridY = NULL;

	m_nDistanceAverageGalactic = 0;
	m_nDistanceAverageExtraGalactic = 0;
	// details section
	m_vectStarDetail = NULL;
	m_vectCatStarDetail = NULL;
	m_vectCatDsoDetail = NULL;
	m_nStarDetail = 0;
	m_nCatStarDetail = 0;
	m_nCatDsoDetail = 0;
	// init thumbnail
	m_bHasThumnail = 0;

	m_nLastSortOrderMethod = -1;

	m_vectNotes.clear();

	m_vectDisplacementPoints.clear();

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		FreeData
// Class:		CAstroImage
// Purpose:		free all my data dynamic allocated by the current image
// Input:		flag 1 = don't close image
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CAstroImage::FreeData( int nFlag )
{
	m_vectNotes.clear();

	if( nFlag != 1 ) CloseImage( );

	//if( m_pImage != NULL ) delete( m_pImage );

	// free image data
	FreeImageData( );

	// free memmory alocated for area
	m_vectArea.clear();

	// deallocate names from stars/dso data
	FreeStars( );
	FreeCatStars( );
	FreeCatDso( );
	FreeCatRadioSources( );
	FreeCatXGammaSources( );
	FreeCatSupernovas( );
	FreeCatBlackholes( );
	FreeCatMStars( );
	FreeCatExoplanets( );
	FreeCatAsteroids( );
	FreeCatComets( );
	FreeCatAes( );
	FreeSolarPlanets( );

	// clear distortion grid
	ClearDistGrid( );
	// clear displacement points
	ClearDisplacementPoints( );

	// free catalog stars
	FreeCatStars( );

	// deallocate names and line from constelattions
	FreeConstellations( );

	// reset other data
	m_nCatalogForMatching = -1;
	if( m_pImgFmtFits ) delete( m_pImgFmtFits );
	m_pImgFmtFits = NULL;

	ResetParams();

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		FreeImageData
// Class:		CAstroImage
// Purpose:		free my image data used for detection
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CAstroImage::FreeImageData( )
{
	if( m_vectImageBuff ) free( m_vectImageBuff );
	m_vectImageBuff = NULL;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		FreeStars
// Class:		CAstroImage
// Purpose:		free all image stars
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CAstroImage::FreeStars( )
{
	int i=0;

	for( i=0; i<m_nStar; i++ )
	{
		if( m_vectStar[i].cat_name )free( m_vectStar[i].cat_name );
	}

	// deallocate star vector data
	if( m_vectStar ) free( m_vectStar );
	m_nStarAllocated = 0;
	m_nStar=0;
	m_vectStar = NULL;

	// deallocate details
	if( m_vectStarDetail ) free( m_vectStarDetail );
	m_vectStarDetail = NULL;
	m_nStarDetail = 0;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		FreeCatStars
// Class:		CAstroImage
// Purpose:		free all catalog stars
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CAstroImage::FreeCatStars( )
{
	int i=0;

	for( i=0; i<m_nCatStar; i++ )
	{
		if( m_vectCatStar[i].cat_name )free( m_vectCatStar[i].cat_name );
	}

	// deallocate star vector data
	if( m_vectCatStar ) free( m_vectCatStar );
	m_nCatStarAllocated = 0;
	m_nCatStar=0;
	m_vectCatStar = NULL;

	// deallocate details
	if( m_vectCatStarDetail ) free( m_vectCatStarDetail );
	m_vectCatStarDetail = NULL;
	m_nCatStarDetail = 0;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		FreeCatDso
// Class:		CAstroImage
// Purpose:		free all catalog dso
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CAstroImage::FreeCatDso( )
{
	// deallocate star vector data
	if( m_vectCatDso ) free( m_vectCatDso );
	m_nCatDsoAllocated = 0;
	m_nCatDso=0;
	m_vectCatDso = NULL;

	// deallocate details
	if( m_vectCatDsoDetail ) free( m_vectCatDsoDetail );
	m_vectCatDsoDetail = NULL;
	m_nCatDsoDetail = 0;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		FreeRadioSources
// Class:		CAstroImage
// Purpose:		free image's radio sources
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CAstroImage::FreeCatRadioSources( )
{
	// deallocate radio vector data
	if( m_vectCatRadio ) free( m_vectCatRadio );
	m_nCatRadioAllocated = 0;
	m_nCatRadio=0;
	m_vectCatRadio = NULL;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		FreeCatXGammaSources
// Class:		CAstroImage
// Purpose:		free image's x-ray/gamma sources
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CAstroImage::FreeCatXGammaSources( )
{
	// deallocate x-ray/gamma vector data
	if( m_vectCatXGamma ) free( m_vectCatXGamma );
	m_nCatXGammaAllocated = 0;
	m_nCatXGamma=0;
	m_vectCatXGamma = NULL;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		FreeCatSupernovas
// Class:		CAstroImage
// Purpose:		free image's supernovas
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CAstroImage::FreeCatSupernovas( )
{
	// deallocate supernovas vector data
	if( m_vectCatSupernova ) free( m_vectCatSupernova );
	m_nCatSupernovaAllocated = 0;
	m_nCatSupernova=0;
	m_vectCatSupernova = NULL;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		FreeCatBlackholes
// Class:		CAstroImage
// Purpose:		free image's blackholes
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CAstroImage::FreeCatBlackholes( )
{
	// deallocate blackholes vector data
	if( m_vectCatBlackhole ) free( m_vectCatBlackhole );
	m_nCatBlackholeAllocated = 0;
	m_nCatBlackhole=0;
	m_vectCatBlackhole = NULL;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		FreeCatMStars
// Class:		CAstroImage
// Purpose:		free image's multiple/double stars
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CAstroImage::FreeCatMStars( )
{
	// deallocate multiple/double stars vector data
	if( m_vectCatMStars ) free( m_vectCatMStars );
	m_nCatMStarsAllocated = 0;
	m_nCatMStars=0;
	m_vectCatMStars = NULL;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		FreeCatExoplanets
// Class:		CAstroImage
// Purpose:		free image's extra-solar planets
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CAstroImage::FreeCatExoplanets( )
{
	// deallocate exoplanets vector data
	if( m_vectCatExoplanet ) free( m_vectCatExoplanet );
	m_nCatExoplanetAllocated = 0;
	m_nCatExoplanet=0;
	m_vectCatExoplanet = NULL;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		FreeCatAsteroids
// Class:		CAstroImage
// Purpose:		free image's asteroids objects
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CAstroImage::FreeCatAsteroids( )
{
	// deallocate asteroids vector data
	if( m_vectCatAsteroid ) free( m_vectCatAsteroid );
	m_nCatAsteroidAllocated = 0;
	m_nCatAsteroid=0;
	m_vectCatAsteroid = NULL;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		FreeCatComets
// Class:		CAstroImage
// Purpose:		free image's comets objects
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CAstroImage::FreeCatComets( )
{
	// deallocate comets vector data
	if( m_vectCatComet ) free( m_vectCatComet );
	m_nCatCometAllocated = 0;
	m_nCatComet=0;
	m_vectCatComet = NULL;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		FreeCatAes
// Class:		CAstroImage
// Purpose:		free image's artficial earth satellites objects
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CAstroImage::FreeCatAes( )
{
	// deallocate comets vector data
	if( m_vectCatAes ) 
	{
		for( int i=0; i<m_nCatAes; i++ )
		{
			//free( m_vectCatAes[i].vectSatType );
			free( m_vectCatAes[i].vectTLERec );
		}
		free( m_vectCatAes );
	}

	m_nCatAesAllocated = 0;
	m_nCatAes=0;
	m_vectCatAes = NULL;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		FreeSolarPlanets
// Class:		CAstroImage
// Purpose:		free image's solar planets/sun/moon objects
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CAstroImage::FreeSolarPlanets( )
{
	// deallocate solar planets/sun/moon vector data
	if( m_vectSolarPlanets ) free( m_vectSolarPlanets );
	m_nSolarPlanetsAllocated = 0;
	m_nSolarPlanets=0;
	m_vectSolarPlanets = NULL;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		FreeConstellations
// Class:		CAstroImage
// Purpose:		free all constelations
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CAstroImage::FreeConstellations( )
{
	int i = 0;

	// deallocate names and line from constelattions
	for( i=0; i<m_nConstellation; i++ )
	{
		if( m_vectConstellation[i].name != NULL )
		{
			free( m_vectConstellation[i].name );
			m_vectConstellation[i].name = NULL;
		}
		// also do the line
		m_vectConstellation[i].nLine = 0;
		free( m_vectConstellation[i].vectLine );
		m_vectConstellation[i].vectLine = NULL;
	}
	// and constelations basic structure
	if( m_vectConstellation ) free( m_vectConstellation );
	m_vectConstellation = NULL;
	m_nConstellation = 0;
	m_nConstellationAllocated = 0;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		ClearDetected
// Class:		CAstroImage
// Purpose:		free/clear data related with detection and the
//				matching process
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CAstroImage::ClearDetected( )
{
	int i = 0;

	m_bIsDetected = 0;
	m_bIsMatched = 0;
	m_nMatchedStars = 0;

	// deallocate vectors data
	FreeStars( );
	FreeCatStars( );
	FreeCatDso( );
	FreeCatRadioSources( );
	FreeCatXGammaSources( );
	FreeCatSupernovas( );
	FreeCatBlackholes( );
	FreeCatMStars( );
	FreeCatExoplanets( );
	FreeCatAsteroids( );
	FreeCatComets( );
	FreeCatAes( );
	FreeSolarPlanets( );

	// deallocate names and line from constelattions
	FreeConstellations( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		ClearMatched
// Class:		CAstroImage
// Purpose:		free/clear data related with detection and the
//				matching process
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CAstroImage::ClearMatched( )
{
	int i = 0;

	m_bIsMatched = 0;
	m_nMatchedStars = 0;

	// reset match id
	for( i=0; i<m_nStar; i++ )
	{
		m_vectStar[i].match = 0;
	}

	///////////////////////////////
	// dealocate star details
	if( m_vectStarDetail ) free( m_vectStarDetail );
	m_vectStarDetail = NULL;
	m_nStarDetail = 0;

	// deallocate memory used by cat objects
	FreeCatStars( );
	FreeCatDso( );
	FreeCatRadioSources( );
	FreeCatXGammaSources( );
	FreeCatSupernovas( );
	FreeCatBlackholes( );
	FreeCatMStars( );
	FreeCatExoplanets( );
	FreeCatAsteroids( );
	FreeCatComets( );
	FreeCatAes( );
	FreeSolarPlanets( );

	// deallocate names and line from constelattions
	FreeConstellations( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		InitDetails
// Class:		CAstroImage
// Purpose:		initialize/allocate object details
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CAstroImage::InitDetails( )
{
	unsigned long i = 0;
	// allocate :: stars
	if( !m_vectStarDetail && m_nStar > 0 )
		m_vectStarDetail = (StarBasicDetailsDef*) malloc( m_nStar*sizeof(StarBasicDetailsDef) );
	// allocate :: cat stars
	if( !m_vectCatStarDetail && m_nCatStar > 0 )
		m_vectCatStarDetail = (StarBasicDetailsDef*) malloc( m_nCatStar*sizeof(StarBasicDetailsDef) );
	// allocate :: cat dso
	if( !m_vectCatDsoDetail && m_nCatDso > 0 )
		m_vectCatDsoDetail = (StarBasicDetailsDef*) malloc( m_nCatDso*sizeof(StarBasicDetailsDef) );

	// reset :: stars
	for( i=0; i<m_nStar; i++ )
	{
//		m_vectStarDetail[i].distance = 0;
		m_vectStarDetail[i].parallax = 0;
		m_vectStarDetail[i].radial_velocity = 0;
		m_vectStarDetail[i].spectral_type = 0;
	}
	// reset :: cat stars
	for( i=0; i<m_nCatStar; i++ )
	{
//		m_vectCatStarDetail[i].distance = 0;
		m_vectCatStarDetail[i].parallax = 0;
		m_vectCatStarDetail[i].radial_velocity = 0;
		m_vectCatStarDetail[i].spectral_type = 0;
	}
	// reset :: cat dso
	for( i=0; i<m_nCatDso; i++ )
	{
//		m_vectCatDsoDetail[i].distance = 0;
		m_vectCatDsoDetail[i].parallax = 0;
		m_vectCatDsoDetail[i].radial_velocity = 0;
		m_vectCatDsoDetail[i].spectral_type = 0;
	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	SetConfig
// Class:	CAstroImage
// Purpose:	Set and get my reference from the config objects
// Input:	pointer to detect config and main config
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImage::SetConfig( CConfigMain* pMainConfig,
								CConfigDetect* pDetectConfig )
{
	int bChange = 0;
	m_pMainConfig = pMainConfig;
	m_pDetectConfig = pDetectConfig;

	// check if default updates then save config
	if( bChange > 0 ) m_pMainConfig->Save( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		SetPath
// Class:		CAstroImage
// Purpose:		set my image path
// Input:		pointer to path string
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CAstroImage::SetPath( const wxString& strPath )
{
//	if( strlen( strPath ) != strlen( m_strImagePath ) )
//	{
//		// reallocate memory space used by path
//		m_strImagePath = (char*) realloc( m_strImagePath, strlen(strPath)+1 );
//	}
	// copy over the new info
	m_strImagePath = strPath;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		Detect
// Class:		CAstroImage
// Purpose:		detect object in image
// Input:		pointer to detect flag if to save or not after detect
// Output:		how many objects where detetcted or zero if none
////////////////////////////////////////////////////////////////////
int CAstroImage::Detect( CSextractor* pDetect, int bSaveProfile )
{
	// clear current detectd if any
	ClearDetected( );

	////////////////////////
	// inititalize
	pDetect->InitData( );
	pDetect->Init( );
	// Do Detection
	pDetect->Make( this );

	// do extra processing
	ProcessStars( );

	///////////////////////
	// here we set that the image was hanged
	m_bIsChanged = 1;
	m_bIsDetected = 1;
	m_nImgSourceType = pDetect->prefs.detect_type;

	// save if set to save
	if( bSaveProfile ) SaveProfile( 0 );

	// close current detection
	pDetect->Close( );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		IsHint
// Class:		CAstroImage
// Purpose:		find if image has hint on his own or group use
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CAstroImage::IsHint( )
{
	if( m_nUseHint )
		return( 1 );
	else if( m_nUseHint == 2 && m_pImageGroup->m_bIsHint )
		return( 1 );

	return( 0 );
}

////////////////////////////////////////////////////////////////////
// Method:		GetHint
// Class:		CAstroImage
// Purpose:		find if image has hint on his own or group use
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CAstroImage::GetHint( double& nRa, double& nDec, double& nWidth, double& nHeight )
{
	if( m_nUseHint )
	{
 		nRa = m_nRelCenterRa;
		nDec = m_nRelCenterDec;
		nWidth = m_nRelFieldRa;
		nHeight = m_nRelFieldDec;

		return( 1 );

	} else if( m_nUseHint == 2 && m_pImageGroup->m_bIsHint )
	{
		nRa = m_pImageGroup->m_nRelCenterRa;
		nDec = m_pImageGroup->m_nRelCenterDec;
		nWidth = m_pImageGroup->m_nRelFieldRa;
		nHeight = m_pImageGroup->m_nRelFieldDec;

		return( 1 );
	}

	return( 0 );
}

////////////////////////////////////////////////////////////////////
// Method:		SetHint
// Class:		CAstroImage
// Purpose:		set image hint on the field size and origin
// Input:		pointer to path string
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CAstroImage::SetHint( double nRa, double nDec, double nWidth,
													double nHeight )
{
	m_nRelCenterRa = nRa;
	m_nRelCenterDec = nDec;
	m_nRelFieldRa = nWidth;
	m_nRelFieldDec = nHeight;

	m_nUseHint = 1;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		SetCoordByMinMaxRaDec
// Class:		CAstroImage
// Purpose:		initialize my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CAstroImage::SetCoordByMinMaxRaDec( double minRA, double maxRA,
										double minDEC, double maxDEC )
{
	m_nRaStart = minRA;
	m_nRaEnd = maxRA;
	m_nDecStart = minDEC;
	m_nDecEnd = maxDEC;
	// set field size
	m_nFieldWidth = maxRA-minRA;
	m_nFieldHeight = maxDEC-minDEC;
	// set origin
	m_nOrigRa = minRA+m_nFieldWidth/2.0;
	m_nOrigDec = minDEC+m_nFieldHeight/2.0;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		SetMainCoord
// Class:		CAstroImage
// Purpose:		initialize my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CAstroImage::SetMainCoord( double fieldCenterRa, double fieldCenterDec,
								double fieldWidth, double fieldHeight, double fieldRadius )
{
/*	m_nRaStart = fieldCenterRa-fieldWidth/2.0;
	m_nRaEnd = fieldCenterDec-fieldHeight/2.0;
	m_nDecStart = fieldCenterRa+fieldWidth/2.0;
	m_nDecEnd = fieldCenterDec+fieldHeight/2.0;
*/
	// set field size
	m_nFieldWidth = fieldWidth;
	m_nFieldHeight = fieldHeight;
	// set origin
	m_nOrigRa = fieldCenterRa;
	m_nOrigDec = fieldCenterDec;
	// radius
	m_nRadius = fieldRadius;

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	GetStarIdByName
// Class:	CAstroImage
// Purpose:	Get a star id by name
// Input:	star name , reference to string
// Output:	star id
/////////////////////////////////////////////////////////////////////
long CAstroImage::GetStarIdByName( StarDef* vectStar, unsigned long nStar, const wxChar* strStarName )
{
	long nStarId = -1;
	unsigned long nCatZone=0;
	unsigned long nCatNo=0;
	unsigned long nCatComp=0;
	long i = 0;
	// star ids regex
	wxRegEx reStarLabelSAO = wxT( "\\ *SAO\\ *([0-9]+)" );
	wxRegEx reStarLabelHIP = wxT( "\\ *HIP\\ *([0-9]+)" );
	wxRegEx reStarLabelTYCHO = wxT( "\\ *TYC\\ *([0-9]+)\\-([0-9]+)\\-([0-9]+)" );
	wxRegEx reStarLabelUSNOB = wxT( "\\ *USNOB\\ *([0-9]+)\\-([0-9]+)" );
	wxRegEx reStarLabelNOMAD = wxT( "\\ *NOMAD\\ *([0-9]+)\\-([0-9]+)" );
	wxRegEx reStarLabel2MASS = wxT( "\\ *2MASS\\ *(.+)" );
	wxRegEx reStarLabelGSC = wxT( "\\ *GSC\\ *(.+)" );

//	wxString strWxStarName(strStarName,wxConvUTF8);
//	wxChar strFormat[50] = wxT("SAO %lu");
//	StarDef* pStar;

	// if sao
	if( reStarLabelSAO.Matches( strStarName ) )
//	if( wxSscanf( strStarName, strFormat, &nCatNo ) )
	{
		if( !reStarLabelSAO.GetMatch( strStarName, 1 ).ToULong( &nCatNo ) )
			return( -1 );
		// for all catalog stars
		//pStar = vectStar;
		for( i=0; i<nStar; i++ )
		{
			if( vectStar[i].cat_no == nCatNo && vectStar[i].cat_type == CAT_OBJECT_TYPE_SAO  ) return( i );
			//pStar++;
		}

	} else if( reStarLabelHIP.Matches( strStarName ) )
	{
		if( !reStarLabelHIP.GetMatch( strStarName, 1 ).ToULong( &nCatNo ) )
			return( -1 );
		// for all catalog stars
		for( i=0; i<nStar; i++ )
		{
			if( vectStar[i].cat_type == CAT_OBJECT_TYPE_HIPPARCOS && vectStar[i].cat_no == nCatNo ) return( i );
		}

	} else if( reStarLabelTYCHO.Matches( strStarName ) )
	{

		if( !reStarLabelTYCHO.GetMatch( strStarName, 1 ).ToULong( &nCatZone ) ||
			!reStarLabelTYCHO.GetMatch( strStarName, 2 ).ToULong( &nCatNo ) ||
			!reStarLabelTYCHO.GetMatch( strStarName, 3 ).ToULong( &nCatComp ) )
			return( -1 );
		// for all catalog stars
		for( i=0; i<nStar; i++ )
		{
			if( vectStar[i].cat_type == CAT_OBJECT_TYPE_TYCHO &&
				vectStar[i].zone_no == nCatZone &&
				vectStar[i].cat_no == nCatNo &&
				vectStar[i].comp_no == nCatComp ) return( i );
		}

	} else if( reStarLabelUSNOB.Matches( strStarName ) )
	{

		if( !reStarLabelUSNOB.GetMatch( strStarName, 1 ).ToULong( &nCatZone ) ||
			!reStarLabelUSNOB.GetMatch( strStarName, 2 ).ToULong( &nCatNo ) )
			return( -1 );
		// for all catalog stars
		for( i=0; i<nStar; i++ )
		{
			if( vectStar[i].cat_type == CAT_OBJECT_TYPE_USNO &&
				vectStar[i].zone_no == nCatZone &&
				vectStar[i].cat_no == nCatNo ) return( i );
		}

	} else if( reStarLabelNOMAD.Matches( strStarName ) )
	{

		if( !reStarLabelNOMAD.GetMatch( strStarName, 1 ).ToULong( &nCatZone ) ||
			!reStarLabelNOMAD.GetMatch( strStarName, 2 ).ToULong( &nCatNo ) )
			return( -1 );
		// for all catalog stars
		for( i=0; i<nStar; i++ )
		{
			if( vectStar[i].cat_type == CAT_OBJECT_TYPE_NOMAD &&
				vectStar[i].zone_no == nCatZone &&
				vectStar[i].cat_no == nCatNo ) return( i );
		}

	} else if( reStarLabel2MASS.Matches( strStarName ) )
	{
		wxString strWxCatName = reStarLabel2MASS.GetMatch( strStarName, 1 ).Trim(0).Trim(1);
		char strCatName[255];
		strcpy( strCatName, strWxCatName.ToAscii() );

		// for all catalog stars
		for( i=0; i<nStar; i++ )
		{
			if( vectStar[i].cat_name )
				if( stricmp( strCatName, vectStar[i].cat_name ) == 0 ) return( i );
		}

	} else if( reStarLabelGSC.Matches( strStarName ) )
	{
		wxString strWxCatName = reStarLabelGSC.GetMatch( strStarName, 1 ).Trim(0).Trim(1);
		char strCatName[255];
		strcpy( strCatName, strWxCatName.ToAscii() );

		// for all catalog stars
		for( i=0; i<nStar; i++ )
		{
			if( vectStar[i].cat_name )
				if( stricmp( strCatName, vectStar[i].cat_name ) == 0 ) return( i );
		}
	}

	return( nStarId );
}

// set best title from dso names/star names or other objects
/////////////////////////////////////////////////////////////////////
int CAstroImage::SetBestTitle( int bForce )
{
	// first verify if title is not already set
	if( !m_strTargetName.IsEmpty() ) return(0);

	int bCnstl=0, bStarFound=0, bDsoFound=0;
	wxString strCnstl=wxT("");
	wxString strDsoName=wxT("");
	wxString strStarName=wxT("");

	// first check constelation
	if( m_nConstellation > 0 && m_nFieldWidth > 20 && m_nFieldHeight > 10 &&
		m_vectConstellation[0].score > 0 )
	{
		bCnstl = 1;
		strCnstl = m_vectConstellation[0].name;
		strCnstl += wxT(" Constellation");
	}

	// check dso object
	if( m_nCatDso > 0 )
	{
		bDsoFound = m_pSky->m_pCatalogDsoNames->GetDosCatName( m_vectCatDso[0], strDsoName );
	}

	// then check star 
	if( m_nCatStar > 0 )
	{
		bStarFound = m_pSky->m_pCatalogStarNames->GetCatStarName( 0, m_vectCatStar[0], strStarName );
	}

	// now based on what i found decide what the title should be
	if( bCnstl ) 
		m_strTargetName = strCnstl;
	else if( bDsoFound == 2 ) 
		m_strTargetName = strDsoName;
	else if( bStarFound == 2 )
		m_strTargetName = strStarName;
	else if( bDsoFound == 1 )
		m_strTargetName = strDsoName;
	else if( bStarFound == 1 )
		m_strTargetName = strStarName;

	return( 0 );
}

////////////////////////////////////////////////////////////////////
// Method:		GetImageExif
// Class:		CAstroImage
// Purpose:		get image exif information
// Input:		string array labels, string array value, no of val
// Output:		status
////////////////////////////////////////////////////////////////////
int CAstroImage::GetImageExif( wxArrayString& vectKey, wxArrayString& vectValue )
{
	// try this code ...
	try
	{
		wxString strFile;
	//	wxString strKey

		strFile.Printf( wxT("%s%s%s%s%s"), m_strImagePath, wxT("\\"), m_strImageName, wxT("."), m_strImageExt );
		// check if file exists
		if( !wxFileExists( strFile ) ) return( 0 );

		std::string std_file(strFile.mb_str());
		Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open( std_file );

		if( !image.get() )
			return( 0 );

		image->readMetadata();

		Exiv2::ExifData &exifData = image->exifData();
		if( exifData.empty() ) return( 0 );
	   
		// se my iterator
		Exiv2::ExifData::const_iterator end = exifData.end();
		// for every key
		for( Exiv2::ExifData::const_iterator i = exifData.begin(); i != end; ++i )
		{
			// fields: i->key(), i->tag(), i->typeName(), i->count(), i->value()
			// insert key and value
	//		wxSting strKey.printf( "%s", i->key() );
			wxString strKey = wxString(i->key().c_str(),wxConvUTF8);
			vectKey.Add( strKey );
	//		vectKey.Last().Printf( "%s", i->key() );
			wxString strVal = wxString(i->value().toString().c_str(),wxConvUTF8);;
			vectValue.Add( strVal );

		}

		image->clearMetadata();
		image->clearExifData();
		image->clearIptcData();
		image->clearXmpPacket();
		image->clearXmpData();
		image->clearComment();
		Exiv2::Image* pimg = image.release();
		delete( pimg );
	}
	// cath errors
	catch (Exiv2::AnyError& e) 
	{
		wxString strMsg=wxT("");
		strMsg.Printf( wxT("WARNING :: Caught Exiv2 exception =%d"), e );
		m_pLogger->Log( strMsg );
		return(0);
	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CAstroImage::GetImageFits( wxArrayString& vectKey, wxArrayString& vectValue )
{
	if( !m_pImgFmtFits || !m_pImgFmtFits->m_bHasHeader ) return(0);

	int i=0;

	for( i=0; i<m_pImgFmtFits->m_vectHeader.size(); i++ )
	{
		vectKey.Add( m_pImgFmtFits->m_vectHeader[i].name ); 
		vectValue.Add( m_pImgFmtFits->m_vectHeader[i].value );
	}

	return(1);
}

////////////////////////////////////////////////////////////////////
// Method:		LoadExifProfile
// Class:		CAstroImage
// Purpose:		load/parse image exif information
// Input:		nothing
// Output:		status
////////////////////////////////////////////////////////////////////
int CAstroImage::LoadExifProfile( )
{
	int i=0;
	wxArrayString vectKey;
	wxArrayString vectValue;
	// other custom
	wxString strCameraName=wxT("");
	wxString strCameraModel=wxT("");
	double nVarDouble=0.0;
	long nVarLong=0;

	// get exif
	if( !GetImageExif( vectKey, vectValue ) ) return( 0 );
	// check if not empty
	if( vectKey.Count() > 0 )
	{
	// add data
		for( i=0; i<vectKey.Count(); i++ )
		{
			// check for date time
			if( vectKey[i].CmpNoCase( wxT("Exif.Image.DateTime") ) == 0 )
			{
				vectValue[i].Replace( wxT(":"), wxT("/"), 0 );
				vectValue[i].Replace( wxT(":"), wxT("/"), 0 );

				m_nObsDateTime.ParseDateTime( vectValue[i] );
				// calculate/set time zone
				int nTZShift = (int) round(m_nObsLongitude/15.0);
				m_nObsTZSecs = (long) nTZShift * 3600;

			} else if( vectKey[i].CmpNoCase( wxT("Exif.Image.Software") ) == 0 )
			{
				m_strProcProg = vectValue[i];

			} else if( vectKey[i].CmpNoCase( wxT("Exif.Image.Make") ) == 0 )
			{
				strCameraName = vectValue[i];

			} else if( vectKey[i].CmpNoCase( wxT("Exif.Image.Model") ) == 0 )
			{
				strCameraModel = vectValue[i];

			} else if( vectKey[i].CmpNoCase( wxT("Exif.Image.Artist") ) == 0 )
			{
				m_strObsName = vectValue[i];

			} else if( vectKey[i].CmpNoCase( wxT("Exif.Photo.ExposureTime") ) == 0 )
			{
				// remove last "/1" ?? if any ?
				wxRegEx reFirst = wxT( "^([0-9\\+\\.\\-]+)" );
				wxRegEx reFirstL = wxT( "^([0-9\\+\\.\\-]+)\\/([0-9\\+\\.\\-]+)" );
				if( reFirstL.Matches( vectValue[i] ) )
				{
					double nA=0.0, nB=0.0;
					wxString strExpTimeA = reFirstL.GetMatch( vectValue[i], 1 ).Trim(0).Trim(1);
					wxString strExpTimeB = reFirstL.GetMatch( vectValue[i], 2 ).Trim(0).Trim(1);
					if( !strExpTimeA.ToDouble( &nA ) ) nA = 0.0;
					if( !strExpTimeB.ToDouble( &nB ) ) nB = 0.0;
					// double first var correction
					nA += 1.0;
					// check which valid
					if( nA == 0.0 && nB != 0.0 )
						m_nExpTotalTime = nB;
					else if( nB == 0.0 && nA != 0.0 )
						m_nExpTotalTime = nA;
					else if( nB != 0.0 && nA != 0.0 )
						m_nExpTotalTime = (double) nA/nB;

				} else if( reFirst.Matches( vectValue[i] ) )
				{
					wxString strExpTime = reFirst.GetMatch( vectValue[i], 1 ).Trim(0).Trim(1);
					if( strExpTime.ToDouble( &nVarDouble ) )
						m_nExpTotalTime = nVarDouble;
				}

			} else if( vectKey[i].CmpNoCase( wxT("Exif.Photo.ISOSpeedRatings") ) == 0 )
			{
				if( vectValue[i].ToLong( &nVarLong ) )
					m_nExpIso = (int) nVarLong;

			} else if( vectKey[i].CmpNoCase( wxT("Exif.Photo.FocalLength") ) == 0 )
			{

			}
		}

		// check if either camera or model
		if( !strCameraName.IsEmpty() || !strCameraModel.IsEmpty() )
		{
			wxString strCamera = strCameraName + wxT(" ") + strCameraModel;
			m_strCameraName = strCamera;
		}
	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		SaveImage
// Class:		CAstroImage
// Purpose:		save image - todo: implement exif/fits details
// Input:		ref to image
// Output:		status
////////////////////////////////////////////////////////////////////
int CAstroImage::SaveImage( wxImage& rImage, const wxString& strFilename )
{
	int nStatus = 1;

	// get abstract image info
	const uint32 w = rImage.GetWidth();
	const uint32 h = rImage.GetHeight();

	ImageVariant rElxImage; //( PF_RGBub, w, h );
//  if (!rElxImage.IsValid())
//    return false;
	//boost::shared_ptr<ImageRGBub> spImageRGBub = _ELX_CreateRGBub( rElxImage, 0 );

	// Copy from wxImage to ImageRGBub(alias ImageVariant)
//	boost::shared_ptr<ImageRGBub> spImageRGBub = (ImageRGBub) rElxImage.GetImpl();
  // get int
	boost::shared_ptr<ImageRGBub> spImage( new ImageRGBub(w, h) );

  // PixelIterator<PixelRGBub const> px_begin = elxConstDowncast<PixelRGBub>(rElxImage.Begin());

    const unsigned char* prDst = (const unsigned char*) spImage->GetPixel(); //spImageRGBub->GetPixel();
    unsigned char* prSrc = rImage.GetData();

	// dirty trick

    // copy map
    const uint32 size = w*h*3;
    ::memcpy( (void*) prDst, prSrc, size );
	// set back image
//	boost::shared_ptr<AbstractImage> spAbstractImage;
	rElxImage = *spImage;

/*  bool ok = _Image.ChangeResolution(RT_UByte);
  if (!ok || _Image.GetResolution() != RT_UByte)
    elxThrow(elxErrOperationFailed, "Unable to change image resolution.");

	nStatus = _Image.ChangePixelFormat(PF_RGBub);
	if( !nStatus || _Image.GetPixelFormat() != PF_RGBub )
		elxThrow(elxErrOperationFailed, "Unable to change image pixel format.");
*/

//	nStatus = _Image.ImageVariant::Save( name.c_str() );
//	if( !nStatus ) elxThrow(elxErrOperationFailed, "Unable to save image.");


	//ProgressNotifier iNotifier;
	//nStatus = the_ImageFileManager.Export( spAbstractImage, strFilename, iNotifier );
	//nStatus = spAbstractImage.ImageVariant::Save( strFilename );

	////////////////////////
	// SPECIAL CASE - FITS IMAGE
	wxFileName myfile(strFilename);
	wxString strFileExt = myfile.GetExt();
	if( strFileExt.CmpNoCase( wxT("fit") ) == 0 || strFileExt.CmpNoCase( wxT("fits") ) == 0 ||
		strFileExt.CmpNoCase( wxT("fts") ) == 0 )
	{
		if( !m_pImgFmtFits ) m_pImgFmtFits = new CImgFmtFits();

		// updates fits table
		m_pImgFmtFits->SetHeader( this );

		ProgressNotifier iNotifier;
		//ImageFileInfo Info;
		nStatus = (bool) m_pImgFmtFits->Save( rElxImage, strFilename.ToAscii(), iNotifier );

		/////////////////
		// also read header info
//		m_pImgFmtFits->WriteHeader( strFile );

	/////////////////////////
	// for any other format use my image library
	} else
	{
		nStatus = rElxImage.Save( strFilename.ToAscii() );
	}

	return( nStatus );
}

////////////////////////////////////////////////////////////////////
// Method:		GetImage
// Class:		CAstroImage
// Purpose:		get image
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
wxImage* CAstroImage::GetImage( )
{
	if( m_pImage != NULL ) return( m_pImage );

	wxString strFile;
	wxString strLog=wxT("");

	strFile.Printf( wxT("%s%s%s%s%s"), m_strImagePath, wxT("\\"), m_strImageName, wxT("."), m_strImageExt );
	m_strImgDataUID = wxString( md5_of_file( strFile.mb_str() ), wxConvUTF8 );

	// get file time - if not set already
	if( m_nFileTime <= 0 )
	{
		time_t fileTime = wxFileModificationTime( strFile );
		m_nFileTime = fileTime;
		m_nObsDateTime.Set( fileTime );
	}

	// now here we do with the handler
//	m_pImage = new wxImage( strFile, wxBITMAP_TYPE_ANY );

	ImageVariant rImage;


	////////////////////////
	// SPECIAL CASE - FITS IMAGE
	if( m_strImageExt.CmpNoCase( wxT("fit") ) == 0 || m_strImageExt.CmpNoCase( wxT("fits") ) == 0 ||
		m_strImageExt.CmpNoCase( wxT("fts") ) == 0 )
	{
		if( !m_pImgFmtFits ) m_pImgFmtFits = new CImgFmtFits();

		ProgressNotifier iNotifier;
		ImageFileInfo Info;
		int nStatus = (bool) m_pImgFmtFits->Load( rImage, strFile.ToAscii(), Info, iNotifier );
		if( !nStatus && m_pLogger )
		{
			wxString strMsg=wxT("");
			strMsg.Printf( wxT("ERROR :: Unable to load image=%s"), strFile );
			m_pLogger->Log( strMsg );
			delete( m_pImgFmtFits );
			m_pImgFmtFits = NULL;
			return( NULL );
		}

		// hack - todo: use dynamic - change resolution to 8bit
		int cres = rImage.GetResolution();
		int  ok = rImage.ChangeToUByteFullDynamic(); //ChangeResolution(RT_UINT8);
		// unable to downsample
		if( ( !ok || rImage.GetResolution() != RT_UINT8 ) && m_pLogger )
			m_pLogger->Log( wxT("WARNING :: unable to downsample image resolution") );

		/////////////////
		// also read header info
//		m_pImgFmtFits->ReadHeader( strFile );

		boost::shared_ptr<ImageRGBub> spImageRGBub = _ELX_CreateRGBub( rImage, 0 );
		m_pImage = CreateImageFromELX( *spImageRGBub );

	/////////////////////////
	// for any other format use my image library
	} else
	{
		if( !LoadELynxImage( rImage, strFile ) ) 
		{
			// try loading it with wxwidgets
			m_pImage = new wxImage( strFile, wxBITMAP_TYPE_ANY );
			if( !m_pImage ) 
			{
				strLog.Printf( wxT("ERROR :A: Unable to load image=%s"), strFile );
				m_pLogger->Log( strLog );

				return( NULL );
			}

		} else
		{
			boost::shared_ptr<ImageRGBub> spImageRGBub = _ELX_CreateRGBub( rImage, 0 );
			m_pImage = CreateImageFromELX( *spImageRGBub );
		}
	}

	// check if image ok
	if( m_pImage->IsOk() )
	{
		m_nHeight = m_pImage->GetHeight( );
		m_nWidth = m_pImage->GetWidth( );

		// return wximage pointer
		return( m_pImage );

	} else
	{
		strLog.Printf( wxT("ERROR :: Unable to load image=%s"), strFile );
		m_pLogger->Log( strLog );

		if( m_pImage ) delete( m_pImage );
		m_pImage = NULL;
		return( NULL );
	}

}

////////////////////////////////////////////////////////////////////
// Method:		LoadELynxImage
// Class:		CAstroImage
int CAstroImage::LoadELynxImage( ImageVariant& rImage, const wxString& strFileName )
{

	// load from file
/*  ImageVariant(const char * iprFilename,
      ImageFileInfo * oprInfo=NULL,
      ProgressNotifier& iNotifier=ProgressNotifier_NULL);
*/

//	ImageFileManager the_ImageFileManager;
	//ImageVariant _Image;

	// astro image loader
	//bool ok = _Image.Load( strFileName, 1 );
	ProgressNotifier iNotifier;
    ImageFileInfo Info;
    int nStatus = (bool) the_ImageFileManager.Import( rImage, strFileName.ToAscii(), &Info, iNotifier );

	if( !nStatus && m_pLogger ) 
	{
//		wxString strMsg=wxT("");
//		strMsg.Printf( wxT("ERROR :: Unable to load image=%s"), strFileName );
//		m_pLogger->Log( strMsg );
		//elxThrow(elxErrOperationFailed, elxMsgFormat("Unable to load image %s.", strFileName ));
	}

/*
	// hack - todo: use dynamic - change resolution to 8bit
	int cres = rImage.GetResolution();
	int  ok = rImage.ChangeToUByteFullDynamic(); //ChangeResolution(RT_UINT8);
	// unable to downsample
	if( !ok || rImage.GetResolution() != RT_UINT8 )
	{
		if( m_pLogger ) m_pLogger->Log( wxT("WARNING :: unable to downsample image resolution") );

//    elxThrow(elxErrOperationFailed,
//      elxMsgFormat("Unable to convert image %s to float resolution.",
//        _Name.c_str()));
	}
*/
	return( nStatus );
}

////////////////////////////////////////////////////////////////////
// Method:		CreateImageFromELX
// Class:		CAstroImage
// Purpose:		create an wxImage from an ImageRGBub. caller is
//				responsible to delete the created object when no
//				longer used
// Input:		ImageRGBub ref
// Output:		pointer to wxImage
////////////////////////////////////////////////////////////////////
wxImage* CAstroImage::CreateImageFromELX( const ImageRGBub& iImage )
{
  if (PF_RGBub != iImage.GetPixelFormat())
    return NULL;

  // get abstract image info
  const uint32 w = iImage.GetWidth();
  const uint32 h = iImage.GetHeight();

  wxImage * psImage = new wxImage(w,h, false);
  if (NULL != psImage)
  {
    const unsigned char * prSrc = (const unsigned char*)iImage.GetPixel();
    unsigned char * prDst = psImage->GetData();

    // copy map
    const uint32 size = w*h*3;
    ::memcpy(prDst, prSrc, size);

    return psImage;
  }

  return NULL;

}

////////////////////////////////////////////////////////////////////
// Method:		_ELX_CreateRGBub
// Class:		CAstroImage
// Purpose:		get stack image with _stack.bmp sufix+extension
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
boost::shared_ptr<ImageRGBub> CAstroImage::_ELX_CreateRGBub( const ImageVariant& iImage,
											bool ibViewTransformed) const
{
	// create an RGBub image from original image
	uint32 channelMask = CM_All;
	uint32 _channelMask = CM_All;
	bool _bBlendAlpha = true;
	EBayerToColorConversion _BCCMethod = BCC_Default;
	const AbstractTransfertFunction * prTransfertFunction = NULL;

//	if( ibViewTransformed ) prTransfertFunction = &_transfertFunction;

	switch( iImage.GetPixelMode() )
	{
		case PM_Color: channelMask = _channelMask;  break;
		case PM_Grey:
		case PM_Complex:
		default: break;
	}

	if( iImage.IsBayer() )
		return elxCreateImageRGBub( *iImage.GetImpl(), prTransfertFunction, _BCCMethod, iImage.GetBayerMatrix(), _channelMask );
	else
	{
		return elxCreateImageRGBub( *iImage.GetImpl(), prTransfertFunction, channelMask, _bBlendAlpha );
	}
}

////////////////////////////////////////////////////////////////////
// Method:		GetStackImage
// Class:		CAstroImage
// Purpose:		get stack image with _stack.bmp sufix+extension
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
wxImage* CAstroImage::GetStackImage( )
{
	wxString strFile;
	if( m_pImage != NULL ) return( m_pImage );

	strFile.Printf( wxT("%s%s%s%s"), m_strImagePath, wxT("\\"),
							m_strImageName, wxT("_stack.bmp") );

	// get file time - if not set already
	if( m_nFileTime <= 0 )
	{
		time_t fileTime = wxFileModificationTime( strFile );
		m_nFileTime = fileTime;
		m_nObsDateTime.Set( fileTime );
		// calculate/set time zone
		int nTZShift = (int) round(m_nObsLongitude/15.0);
		m_nObsTZSecs = (long) nTZShift * 3600;
	}

	m_pImage = new wxImage( strFile, wxBITMAP_TYPE_ANY );

	if( m_pImage->IsOk() )
	{
		m_nHeight = m_pImage->GetHeight( );
		m_nWidth = m_pImage->GetWidth( );

		// return wximage pointer
		return( m_pImage );

	} else
		return( NULL );

}

////////////////////////////////////////////////////////////////////
// Method:		GetImageData
// Class:		CAstroImage
// Purpose:		get image data in my buffer
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CAstroImage::GetImageData( int nChannels, int bUseAreas )
{
	if( GetImage() != NULL )
	{
		// extract data in gray scape for processing
//		ExtractData( m_pImage->GetData(), EXTRACT_PROCESSING_DATA_RED );
//		ExtractData( m_pImage->GetData(), EXTRACT_PROCESSING_DATA_BLUE );
//		ExtractData( m_pImage->GetData(), EXTRACT_PROCESSING_DATA_GREEN );
//		ExtractData( m_pImage->GetData(), EXTRACT_PROCESSING_DATA_DOUBLE_COMPOSITE );

		// extract data by channels and 
		ExtractData( m_pImage->GetData(), nChannels, bUseAreas );

		// return wximage pointer
		return( 1 );

	} else
		return( 0 );
}

////////////////////////////////////////////////////////////////////
// Method:		GetImageFileInfo
// Class:		CAstroImage
// Purpose:		get image file time
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CAstroImage::GetImageFileInfo( )
{
	wxString strFile;

	strFile.Printf( wxT("%s%s%s%s%s"), m_strImagePath, wxT("\\"),
							m_strImageName, wxT("."), m_strImageExt );


	// check if file
	if( !wxFileExists(strFile) ) return;

	//////////////
	// :: get file time
	time_t fileTime = wxFileModificationTime( strFile );
	m_nFileTime = fileTime;

	m_nObsDateTime.Set( fileTime );
	// calculate/set time zone
	int nTZShift = (int) round(m_nObsLongitude/15.0);
	m_nObsTZSecs = (long) nTZShift * 3600;

	//////////////////
	// get file size

	////////////////////
	// get file/image type

	//////////////////////
	// :: get exim image info if any

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		CreateThumbnail
// Class:		CAstroImage
// Purpose:		create thumbnail out of image
// Input:		reference to bitmap
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CAstroImage::CreateThumbnail( wxBitmap& pThumbnail, int nWidth, int nHeight, int bAspectRatio )
{
	wxString strThumbnailFile;

	if( m_pImage && m_pImage->IsOk() )
	{
		pThumbnail = wxBitmap(m_pImage->Scale( nWidth, nHeight , wxIMAGE_QUALITY_HIGH ) );

	} else
	{
		wxString strFile;
		strFile.Printf( wxT("%s%s%s%s%s"), m_strImagePath, wxT("\\"), m_strImageName, wxT("."), m_strImageExt );

		wxImage* pImage;
		ImageVariant rImage;
		if( !LoadELynxImage( rImage, strFile ) ) return( 0 );
		boost::shared_ptr<ImageRGBub> spImageRGBub = _ELX_CreateRGBub( rImage, 0 );
		pImage = CreateImageFromELX( *spImageRGBub );

		if( pImage->IsOk() )
		{
			pThumbnail = wxBitmap( pImage->Scale( nWidth, nHeight, wxIMAGE_QUALITY_HIGH ) );

		} else
			return( 0 );
/*
		wxImage* pImage = new wxImage( strFile, wxBITMAP_TYPE_ANY );
		if( pImage->IsOk() )
		{
			pThumbnail = wxBitmap( pImage->Scale( DEFAULT_THUMBNAIL_WIDTH, DEFAULT_THUMBNAIL_HEIGHT, wxIMAGE_QUALITY_HIGH ) );
		} else
		{
			return( 0 );
		}
*/
		// close
		if( pImage )
		{
			pImage->Destroy( );
			delete( pImage );
		}
	}
	// now save thumbnail
	// :: first set the folder
	strThumbnailFile.Printf( wxT("%s%s"), GROUP_IMAGE_THUMBNAILS_PATH, m_pImageGroup->m_strGroupNameId );
	// :: then check if exists
	if( !wxDirExists( strThumbnailFile ) ) wxMkdir( strThumbnailFile );
	// :: then build full name
	strThumbnailFile += wxT("/") + m_strImageName + wxT("_icon.jpg");
	// :: then save icon
	pThumbnail.SaveFile( strThumbnailFile, wxBITMAP_TYPE_JPEG );

	// set flag that it has thumbnail
	m_bHasThumnail = 1;

	return( 1 );
}

// Method:	CreateThumbnailCustom
////////////////////////////////////////////////////////////////////
int CAstroImage::CreateThumbnailCustom( const wxString& strOutFile, int nWidth, int nHeight, int bAspectRatio )
{
	wxString strPathFull;

	// check by type if image or elynx
	if( m_pImage && m_pImage->IsOk() )
	{
		wxImage pImage = m_pImage->Copy();
	
		double nImgWidth = pImage.GetWidth();
		double nImgHeight = pImage.GetHeight();
		// if ASPECT RATIO flag present - calculate
		if( bAspectRatio )
		{
			double nRatio = 0;
			if( nImgWidth >= nImgHeight )
			{
				nRatio = (double) nWidth / (double) nImgWidth;
				nHeight = (double) nImgHeight *  nRatio;

			} else
			{
				nRatio = (double) nHeight / (double) nImgHeight;
				nWidth = (double) nImgWidth * nRatio;
			}
		}

		pImage.Scale( nWidth, nHeight , wxIMAGE_QUALITY_HIGH );
		// :: then save it
		pImage.SaveFile( strOutFile, wxBITMAP_TYPE_JPEG );

	} else
	{
		wxString strFile;
		strFile.Printf( wxT("%s%s%s%s%s"), m_strImagePath, wxT("\\"), m_strImageName, wxT("."), m_strImageExt );

		wxImage* pImage;
		ImageVariant rImage;
		LoadELynxImage( rImage, strFile ); 

		double nImgWidth = rImage.GetWidth();
		double nImgHeight = rImage.GetHeight();
		// if ASPECT RATIO flag present - calculate
		if( bAspectRatio )
		{
			double nRatio = 0;
			if( nImgWidth >= nImgHeight )
			{
				nRatio = (double) nWidth / (double) nImgWidth;
				nHeight = (double) nImgHeight *  nRatio;

			} else
			{
				nRatio = (double) nHeight / (double) nImgHeight;
				nWidth = (double) nImgWidth * nRatio;
			}
		}

		rImage.Resize( nWidth, nHeight );

		rImage.Save( strOutFile.ToAscii() );
	}


	// set flag that it has thumbnail
	m_bHasThumnail = 1;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		LoadThumbnail
// Class:		CAstroImage
// Purpose:		create thumbnail out of image
// Input:		reference to bitmap
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CAstroImage::LoadThumbnail( wxBitmap& pThumbnail )
{
	wxString strThumbnailGroupPath;
	wxString strThumbnailFile;
	int bState = 1;

	strThumbnailGroupPath.Printf( wxT("%s%s"), GROUP_IMAGE_THUMBNAILS_PATH, m_pImageGroup->m_strGroupNameId );
	strThumbnailFile.Printf( wxT("%s\/%s_icon.jpg"), strThumbnailGroupPath, m_strImageName );

	if( wxFileExists( strThumbnailFile ) )
	{
//		if( !pThumbnail.IsOk() )
//			pThumbnail = wxBitmap( DEFAULT_THUMBNAIL_WIDTH, DEFAULT_THUMBNAIL_HEIGHT );
		// just load my icon
		if( !pThumbnail.LoadFile( strThumbnailFile, wxBITMAP_TYPE_JPEG ) )
		{
			bState = CreateThumbnail( pThumbnail );
		}

	} else
	{
		// just create
		bState = CreateThumbnail( pThumbnail );
	}

	return( bState );
}

////////////////////////////////////////////////////////////////////
// Method:		CloseImage
// Class:		CAstroImage
// Purpose:		initialize my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CAstroImage::CloseImage( )
{
	if( m_pImage )
	{
		m_pImage->Destroy( );
		delete( m_pImage );
	}
	m_pImage = NULL;

	return( 1 );

}

////////////////////////////////////////////////////////////////////
// Method:		ExtractData
// Class:		CAstroImage
// Purpose:		extract image data from rgb in gray scale for processing
//				unsign methods
// Input:		pointer to image data
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CAstroImage::ExtractData( unsigned char *vectImgData, int nType, int bUseAreas )
{
	unsigned int nCounter = 0;
	unsigned char* vectImgDataT=NULL;
	int a=0, i=0, j=0;

	/////////
	// build alpha channel with transparency based on the areas
	if( bUseAreas && m_vectArea.size() )
	{
		unsigned long ilen = m_nHeight*m_nWidth*sizeof(unsigned char);
		// allocate 
		vectImgDataT = (unsigned char*) malloc ( ilen+1 );
		// set all to tranparent
		memset( vectImgDataT, 1, ilen );
		// for every area
		for( a=0; a<m_vectArea.size(); a++ )
		{
			// write area data
			int ax = (int) m_vectArea[a].x;
			int ay = (int) m_vectArea[a].y;
			int aw = (int) ax+m_vectArea[a].width;
			int ah = (int) ay+m_vectArea[a].height;
			// set area
			for( i=ay; i<ah; i++ )
			{
				for( j=ax; j<aw; j++ )
				{
					unsigned long index = i*m_nWidth+j;
					if( index >= ilen ) 
						continue;
					else
						vectImgDataT[index] = 0;
				}
			}
		}
	}

	////////////////////////
	// check if buffer allocated - then free
	if( m_vectImageBuff != NULL ) free( m_vectImageBuff );
	// copy raw data
	m_vectImageBuff = (double*) malloc (m_nHeight * m_nWidth * sizeof (double));

	/////////////////////
	// fill the source extractor buffer with my data
	for( i=0; i<m_nHeight; i++ )
	{
		for( j=0; j<m_nWidth; j++ )
		{
			unsigned long index = i*m_nWidth+j;
			// check if in area and transparency
			if( bUseAreas && vectImgDataT && vectImgDataT[index] == 0 )
			{
				// set to zero and do next
				m_vectImageBuff[index] = 0.0;
				nCounter += 3;
				continue;
			}

			unsigned char nRed = vectImgData[nCounter++];
			unsigned char nGreen = vectImgData[nCounter++];
			unsigned char nBlue = vectImgData[nCounter++];

			// now do on type of extraction
			if( nType == EXTRACT_PROCESSING_DATA_RED )
			{
				// one col channel - red ?
				m_vectImageBuff[index] = nRed;

			} else if( nType == EXTRACT_PROCESSING_DATA_GREEN )
			{
				// one col channel green
				m_vectImageBuff[index] = nGreen;

			} else if( nType == EXTRACT_PROCESSING_DATA_BLUE )
			{
				// one col channel blue
				m_vectImageBuff[index] = nBlue;

			}else if( nType == EXTRACT_PROCESSING_DATA_DOUBLE_COMPOSITE )
			{
				// calculate composite color for all channels and multiply
				m_vectImageBuff[index] = (double) ( ( 0.299*nRed + 0.587*nGreen + 0.114*nBlue ) * 65535 );

			} else if( nType == EXTRACT_PROCESSING_DATA_MEDIAN_COMPOSITE )
			{
				// calculate median sum of color channels
				m_vectImageBuff[index] = (unsigned char) ((nRed + nGreen + nBlue)/3);

			} else if( nType == EXTRACT_PROCESSING_DATA_WINNER )
			{
				// larry's hack ... get max from all of them
				unsigned char max = nRed;
				if( nGreen > max ) max = nGreen;
				if( nBlue > max ) max = nBlue;
				m_vectImageBuff[index] = max;

			// for anything else just do green
			} else 
			{
				// one col channel green
				m_vectImageBuff[index] = nGreen;

			}
		}
	}

	// free areas layer if present
	if( vectImgDataT ) free( vectImgDataT );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	sort stars  by magnitude
// Class:	CAstroImage
// Purpose:	sort stars in order of magnitude
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CAstroImage::SortByMagnitude( int bOrder )
{
	if( m_nStar <= 0 ) return( 0 );

	long i = 0;

	if( bOrder == 0 )
	{
		std::sort( m_vectStar, m_vectStar+m_nStar, OnSortImgStarByMagDesc );
		m_nLastSortOrderMethod = SORT_OBJECTS_METHOD_MAGNITUDE_DESC;

	} else
	{
		std::sort( m_vectStar, m_vectStar+m_nStar, OnSortImgStarByMagAsc );
		m_nLastSortOrderMethod = SORT_OBJECTS_METHOD_MAGNITUDE_ASC;
	}

	// now set id to order no
	for( i=0; i<m_nStar; i++ )
	{
		m_vectStar[i].cat_no = i;
	}

/* - I see no point for this here - this is to be used from set match
	// set min/max
	if( bOrder == 0 )
	{
		m_nMaxMag = m_vectStar[m_nStar-1].mag;
		m_nMinMag = m_vectStar[0].mag;

	} else
	{
		m_nMinMag = m_vectStar[m_nStar-1].mag;
		m_nMaxMag = m_vectStar[0].mag;
	}
*/

	// return number of stars
	return( m_nStar );
}

/////////////////////////////////////////////////////////////////////
// Method:	ExtractStars
// Class:	CAstroImage
// Purpose:	extract only stars for match processing - no galaxies
// Input:	reference to vector to be populated and max to be extracted
// Output:	number of stars extracted
/////////////////////////////////////////////////////////////////////
long CAstroImage::ExtractStars( StarDef* vectStar, long nStar,
							   int bByShape, int bByDetectType,
							   int nMatchFlag, int bOrderMethod, double nFwhmLimit )
{
	long i = 0;
	long nCount = 0;
	double nElpsFactor = 0;

	// first we clean the incoming vector
	for( i=0; i<nStar; i++ )
	{
		// clear and set to null
		if( vectStar[nCount].cat_name )
		{
			free( vectStar[nCount].cat_name );
			vectStar[nCount].cat_name = NULL;
		}
	}
	memset( vectStar, 0, nStar*sizeof(StarDef) );

	// check if to sort
	if( bOrderMethod != m_nLastSortOrderMethod )
	{
		if( bOrderMethod == SORT_OBJECTS_METHOD_MAGNITUDE_DESC )
			SortByMagnitude( 0 );
		else if( bOrderMethod == SORT_OBJECTS_METHOD_MAGNITUDE_ASC )
			SortByMagnitude( 1 );
		else if( bOrderMethod == SORT_OBJECTS_METHOD_FWHM_DESC )
			std::sort( m_vectStar, m_vectStar+m_nStar, OnSortImgStarByFwhmDesc );
		// set method to sort used
		m_nLastSortOrderMethod = bOrderMethod;
	}

	// for every star in my image
	for( i=0; i<m_nStar; i++ )
	{
		// calculate ellipse factor - also check if a > b
		if( m_vectStar[i].a >= m_vectStar[i].b )
			nElpsFactor = (double) (m_vectStar[i].a/m_vectStar[i].b);
		else
			nElpsFactor = (double) (m_vectStar[i].b/m_vectStar[i].a);

		// now check by flags and/or ellongation a/b and detect type = 0 = star
		if( ( bByShape && nElpsFactor < 2 ) ||
			( bByDetectType && m_vectStar[i].type == 0 ) ||
			( bByShape == 0 && bByDetectType == 0 ) )
		{
			// we check for fwhm limit
			if( nFwhmLimit < DBL_MAX && m_vectStar[i].fwhm <= nFwhmLimit )
				break;

//			vectStar[nCount] = &(m_vectStar[i]);

			// set the id to the order no
			vectStar[nCount].img_id = i;
			// set the rest of variables
			vectStar[nCount].x = m_vectStar[i].x;
			vectStar[nCount].y = m_vectStar[i].y;
			vectStar[nCount].mag = m_vectStar[i].mag;
			vectStar[nCount].cat_no = m_vectStar[i].cat_no;

			// check if to reset match flag
			if( nMatchFlag )
				vectStar[nCount].match = 0;
			else
				vectStar[nCount].match = m_vectStar[i].match;

			vectStar[nCount].a = m_vectStar[i].a;
			vectStar[nCount].b = m_vectStar[i].b;
			vectStar[nCount].theta = m_vectStar[i].theta;
			vectStar[nCount].kronfactor = m_vectStar[i].kronfactor;
			vectStar[nCount].fwhm = m_vectStar[i].fwhm;
			vectStar[nCount].flux = m_vectStar[i].flux;

			// increment found count
			nCount++;
		}
		// now check if we found as many as we wanted
		if( nCount >= nStar ) break;
	}

	// return number of stars found
	return( nCount );
}

/////////////////////////////////////////////////////////////////////
// Method:	ExtractMatchedStars
// Class:	CAstroImage
// Purpose:	extract only stars which where a match
// Input:	reference to vector with match stars, destiantion and no?
// Output:	number of stars extracted
/////////////////////////////////////////////////////////////////////
long CAstroImage::ExtractMatchedStars( StarDef* vectSrcStar, StarDef* vectDestStar, long nStar )
{
	long i = 0;
	long nCount = 0;

	// for every star in my image
	for( i=0; i<nStar; i++ )
	{
		// now check by ellongation a/b
		if( vectSrcStar[i].match )
		{
//			vectStar[nCount] = &(m_vectStar[i]);

			// set the id to the order no
			vectDestStar[nCount].img_id = i;
			// set the rest of variables
			vectDestStar[nCount].x = vectSrcStar[i].x;
			vectDestStar[nCount].y = vectSrcStar[i].y;
			vectDestStar[nCount].mag = vectSrcStar[i].mag;
			vectDestStar[nCount].cat_no = vectSrcStar[i].cat_no;
			vectDestStar[nCount].match = vectSrcStar[i].match;
			vectDestStar[nCount].a = vectSrcStar[i].a;
			vectDestStar[nCount].b = vectSrcStar[i].b;
			vectDestStar[nCount].theta = vectSrcStar[i].theta;
			vectDestStar[nCount].kronfactor = vectSrcStar[i].kronfactor;

			// increment found count
			nCount++;
		}
	}

	// return number of stars found
	return( nCount );
}

/////////////////////////////////////////////////////////////////////
void CAstroImage::SetStarStringCatalogName( StarDef& pStar, const char* strCatName )
{
	if( pStar.cat_name ) free( pStar.cat_name );
	pStar.cat_name = (char*) malloc( (strlen(strCatName)+1)*sizeof(char) );
	strcpy( pStar.cat_name, strCatName );
}

/////////////////////////////////////////////////////////////////////
// Method:	AdjustStarDetailsFromCatalog
// Class:	CAstroImage
// Purpose:	set catalog number from vector using the id nymber
// Input:	reference to vector to be populated and max to be extracted
// Output:	number of stars extracted
/////////////////////////////////////////////////////////////////////
long CAstroImage::AdjustStarDetailsFromCatalog( CSkyCatalogStars* pCatalogStars )
{
	long i = 0;

	///////////
	// for every star in my image detection
	for( i=0; i<m_nStar; i++ )
	{
		unsigned long nCatId = m_vectStar[i].cat_id;
		// if name free & copy
		if( m_vectStar[i].match && pCatalogStars->m_vectStar[nCatId].cat_name )
		{
			if( m_vectStar[i].cat_name ) free( m_vectStar[i].cat_name );
			m_vectStar[i].cat_name = (char*) malloc( (strlen(pCatalogStars->m_vectStar[nCatId].cat_name)+1)*sizeof(char) );
			strcpy( m_vectStar[i].cat_name, pCatalogStars->m_vectStar[nCatId].cat_name );
		}
	}

	////////////
	// for every star in my catalog
	for( i=0; i<m_nCatStar; i++ )
	{
		unsigned long nCatId = m_vectCatStar[i].cat_id;
		// if name free & copy
		if( pCatalogStars->m_vectStar[nCatId].cat_name )
		{
			if( m_vectCatStar[i].cat_name ) free( m_vectCatStar[i].cat_name );
			m_vectCatStar[i].cat_name = (char*) malloc( (strlen(pCatalogStars->m_vectStar[nCatId].cat_name)+1)*sizeof(char) );
			strcpy( m_vectCatStar[i].cat_name, pCatalogStars->m_vectStar[nCatId].cat_name );
		}
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	SetCatStarsMatched
// Class:	CAstroImage
// Purpose:	set catalog number from vector using the id nymber
// Input:	reference to vector to be populated and max to be extracted
// Output:	number of stars extracted
/////////////////////////////////////////////////////////////////////
long CAstroImage::SetCatStarsMatched( StarDef* vectStar, long nStar )
{
	unsigned long nId = 0;
	long i = 0;
	long nCount = 0;
	m_nMagnitudeToRadiusRatio = 0.;
	double nMagnitudeToRadiusRatio = 0.;
	double nMagShiftAvg = 0.;

	// set the center in the sky where the image was found
//	m_nOrigRa = nRaOrig;
//	m_nOrigDec = nDecOrig;

	m_nMaxMag = DBL_MAX;
	m_nMinMag = DBL_MIN;

	// for every star in my input vector
	for( i=0; i<nStar; i++ )
	{
		// if there was a match
		if( vectStar[i].match == 1 )
		{
			// get originating object id in the image
			nId = vectStar[i].img_id;

			// we get the max mag from the matched only
			if( vectStar[i].vmag < m_nMaxMag ) m_nMaxMag = vectStar[i].vmag;

			////////////////////
			// NAME :: use catalog loaded !!!! - save free/malloc
/*			if( vectStar[i].cat_name )
			{
				if( m_vectStar[nCount].cat_name ) free( m_vectStar[nCount].cat_name );
				m_vectStar[nCount].cat_name = (char*) malloc( (strlen(vectStar[i].cat_name)+1)*sizeof(char) );
				strcpy( m_vectStar[nCount].cat_name, vectStar[i].cat_name );

			}
*/
			m_vectStar[nId].cat_no = vectStar[i].cat_no;
			m_vectStar[nId].cat_type = vectStar[i].cat_type;
			m_vectStar[nId].zone_no = vectStar[i].zone_no;
			m_vectStar[nId].comp_no = vectStar[i].comp_no;
			// set originating catalog id - for letter data population
			m_vectStar[nId].cat_id = vectStar[i].cat_id;

			m_vectStar[nId].vmag = vectStar[i].vmag;
			m_vectStar[nId].match = vectStar[i].match;

			// calculate average mag shift
			nMagShiftAvg += (m_vectStar[nId].vmag - m_vectStar[nId].mag);

			// now de-project star - don't need these now because i use
			// the catalog ra dec reference - but might be useful in
			// the future at least to test it
//			DeprojectStar( vectStar[i], nRaOrig, nDecOrig );
			// and now set the ra dec in main
			m_vectStar[nId].ra = vectStar[i].ra;
			m_vectStar[nId].dec = vectStar[i].dec;

			//////////////////////
			// calculate average ratio in between magnitune and radius as a
			double nMagRadRatio = fabs( (vectStar[i].a*vectStar[i].kronfactor*0.75)/vectStar[i].mag );
//			if( m_nMagnitudeToRadiusRatio == 0 )
//				m_nMagnitudeToRadiusRatio = nMagRadRatio;
//			else
//				m_nMagnitudeToRadiusRatio = (m_nMagnitudeToRadiusRatio+nMagRadRatio)/2;

			nMagnitudeToRadiusRatio += nMagRadRatio;

			// incerment found counter
			nCount++;

		}
//		else
//		{
//			nId = vectStar[i].id;
//			m_vectStar[nId].ra = vectStar[i].ra;
//			m_vectStar[nId].dec = vectStar[i].dec;
//		}
	}

	// calc averages
	m_nMagnitudeToRadiusRatio = (double) nMagnitudeToRadiusRatio/nCount;
	nMagShiftAvg = nMagShiftAvg/nCount;

	for( i=0; i<m_nStar; i++ )
	{
		if( m_vectStar[i].mag != 0.0 )
		{
			m_vectStar[i].mag += nMagShiftAvg;
			// from all get the min mag
			if( m_vectStar[i].mag > m_nMinMag && m_vectStar[i].mag < 30.0 )
				m_nMinMag = m_vectStar[i].mag;
		}
	}

	return( nCount );
}

/////////////////////////////////////////////////////////////////////
// Method:	SetCatStarsNotMatched
// Class:	CAstroImage
// Purpose:	set catalog stars not matched
// Input:	reference to vector and size of the vector
// Output:	number of stars not matched
/////////////////////////////////////////////////////////////////////
long CAstroImage::SetCatStarsNotMatched( StarDef* vectStar, long nStar )
{
	unsigned long nId = 0;
	long i = 0;
	long nCount = 0;
	StarDef star;
	unsigned char nType = 0;

	// first we deallocate if allocated
	FreeCatStars( );

	////////////
	// for every star in my input vector
	for( i=0; i<nStar; i++ )
	{
		// if there was no match
		if( vectStar[i].match == 0 )
		{
			memset( &star, 0, sizeof(StarDef) );

			star.ra = vectStar[i].ra;
			star.dec = vectStar[i].dec;

			// project star using the found orig
			m_pSky->ProjectStar( star, m_nOrigRa, m_nOrigDec );
			// larry :: my damn multiplication factor
			star.x = star.x*m_nSkyProjectionScale; //10;
			star.y = star.y*m_nSkyProjectionScale; //10;
			// and now apply inverse trans
			m_pSky->TransObj_SkyToImage( star );

			// check if object is in my image
			if( star.x >= 0 && star.y >= 0 && star.x <= m_nWidth && star.y <= m_nHeight )
			{
				nType = SKY_OBJECT_TYPE_STAR;

				// set my star
				star.cat_type = vectStar[i].cat_type;
				star.cat_no = vectStar[i].cat_no;

				// 0ther stuff
				star.cat_no = vectStar[i].cat_no;
				star.cat_type = vectStar[i].cat_type;
				star.zone_no = vectStar[i].zone_no;
				star.comp_no = vectStar[i].comp_no;
				// set originating catalog id
				star.cat_id = vectStar[i].cat_id;

				star.vmag = vectStar[i].vmag;
				star.mag = vectStar[i].mag;
				star.match = vectStar[i].match;
				star.a = fabs(vectStar[i].mag*m_nMagnitudeToRadiusRatio);
				star.b = fabs(vectStar[i].mag*m_nMagnitudeToRadiusRatio);
				star.theta = 0; //m_vectStar[i].pa;
				star.kronfactor = 1;

				// and now add this to vector of catalog object in the image
				long nStar =AddCatStar( star, nType );
				//check if to set the name
//					if( nStar >= 0 )
//						SetStarStringCatalogName( m_vectCatStar[nStar], vectStar[i].cat_name );

				///////////////
				// we also readjust the max magnitude here
				if( vectStar[i].vmag < m_nMaxMag ) m_nMaxMag = vectStar[i].vmag;

			}

			// increment found counter
			nCount++;

		}
	}

	return( nCount );
}

/////////////////////////////////////////////////////////////////////
// Method:	MatchNotMatchedStars
// Class:	CAstroImage
// Purpose:	try to match stars which are not matched
// Input:	reference to cat star vector and size of the vector
// Output:	number of stars not matched
/////////////////////////////////////////////////////////////////////
long CAstroImage::MatchNotMatchedStars( StarDef* vectCatStar, long nCatStar )
{
	long nFoundId = -1;
	unsigned long nId = 0;
	long i = 0;
	long nCount = 0;
	StarDef star;

	////////////
	// for every star in my input vector
	for( i=0; i<nCatStar; i++ )
	{
		// if there was no match
		if( vectCatStar[i].match == 0 )
		{
			memset( &star, 0, sizeof(StarDef) );

			star.ra = vectCatStar[i].ra;
			star.dec = vectCatStar[i].dec;

			// project star using the found orig
			m_pSky->ProjectStar( star, m_nOrigRa, m_nOrigDec );
			// larry :: my damn multiplication factor
			star.x = star.x*m_nSkyProjectionScale; //10;
			star.y = star.y*m_nSkyProjectionScale; //10;
			// and now apply inverse trans
			m_pSky->TransObj_SkyToImage( star );

			// check if object is in my image
			if( star.x >= 0 && star.y >= 0 &&
				star.x <= m_nWidth &&
				star.y <= m_nHeight )
			{
				// no check if in distance
				nFoundId = FindClosestObjectInImage( star.x, star.y, m_pMainConfig->m_nAssocMatchMaxDist, 0 );
				// if found then set my object as matched
				if( nFoundId >= 0 )
				{
					m_vectStar[nFoundId].match = 2;
					vectCatStar[i].match = 2;

					// if name -copy
/*					if( vectCatStar[i].cat_name )
					{
						if( m_vectStar[nCount].cat_name ) free( m_vectStar[nCount].cat_name );
						m_vectStar[nCount].cat_name = (char*) malloc( (strlen(vectCatStar[i].cat_name)+1)*sizeof(char) );
						strcpy( m_vectStar[nCount].cat_name, vectCatStar[i].cat_name );

					}
*/
					//nId = vectCatStar[i].id;
					m_vectStar[nFoundId].cat_no = vectCatStar[i].cat_no;
					m_vectStar[nFoundId].cat_type = vectCatStar[i].cat_type;
					m_vectStar[nFoundId].zone_no = vectCatStar[i].zone_no;
					m_vectStar[nFoundId].comp_no = vectCatStar[i].comp_no;
					// set star originating catalog id
					m_vectStar[nFoundId].cat_id = vectCatStar[i].cat_id;

					m_vectStar[nFoundId].vmag = vectCatStar[i].vmag;
 					m_vectStar[nFoundId].match = vectCatStar[i].match;

					// and now set the ra dec in main
					m_vectStar[nFoundId].ra = vectCatStar[i].ra;
					m_vectStar[nFoundId].dec = vectCatStar[i].dec;

					// increment found counter
					nCount++;

				}
			}
		}
	}
	// also increment matched
	m_nMatchedStars += nCount;

	return( nCount );
}

////////////////////////////////////////////////////////////////////
// Method:	GetObjectAtXY
// Class:	CAstroImage
// Purose:	find object in image at coord x/y
// Input:	coord x/y, ref to type, and flag
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CAstroImage::GetObjectAtXY( double x, double y, long nLastObjId, int nLastObjType, int& nFoundType, int nFlag )
{
	int i = 0;
	double nDist = 0;
	nFoundType = 0;
	int nFound = -1;
	double nFoundDist = DBL_MAX;
	// cycle vars
	double a, b, nSX, nSY;

	// object vars
	StarDef* pObj;
	double o_a=0.0, o_b=0.0, o_k=0.0;
	unsigned char o_x_a=0, o_y_a=0;

	///////////////////////////////////////////////
	// FIRST WE CHECK IF IT'S THE SAME OBJECT(s) 
	// - maybe I should pass in a vector of objects in objects last found
	// - most times the mouse will move just a little
	if( nLastObjType == 1 && m_vectStar != NULL && nLastObjId < m_nStar )
	{
		// get quick obj ref
		pObj = &(m_vectStar[nLastObjId]);
		o_a = pObj->a;
		o_b = pObj->b;
		o_k = pObj->kronfactor;
		o_x_a = pObj->x_a;
		o_y_a = pObj->y_a;
		nSX = pObj->x;
		nSY = pObj->y;
		// calculate
		a = o_a*o_k;
		b = o_b*o_k;

		if( IsPointInEllipse( x, y, nSX, nSY, a, b, pObj->theta, nFoundDist ) )
		{
			nFound = nLastObjId;
			nFoundDist = nDist;
			// set b found type to star=1
			nFoundType = 1;
			return(nFound);
		}

	} else if( nLastObjType == 2 && m_vectCatStar != NULL && i < m_nCatStar )
	{
		// get quick obj ref
		pObj = &(m_vectCatStar[i]);
		o_a = pObj->a;
		o_b = pObj->b;
		o_k = pObj->kronfactor;
		o_x_a = pObj->x_a;
		o_y_a = pObj->y_a;
		// calculate
		nSX = pObj->x;
		nSY = pObj->y;
		a = o_a*o_k;
		b = o_b*o_k;

		if( IsPointInEllipse( x, y, nSX, nSY, a, b, pObj->theta, nFoundDist ) )
		{
			nFound = nLastObjId;
			nFoundDist = nDist;
			// set b found type to star=1
			nFoundType = 1;
			return(nFound);
		}

	} else if( nLastObjType == 3  && m_vectCatDso != NULL && i < m_nCatDso )
	{
		// get quick obj ref
		pObj = &(m_vectCatDso[i]);
		o_a = pObj->a;
		o_b = pObj->b;
		o_k = pObj->kronfactor;
		o_x_a = pObj->x_a;
		o_y_a = pObj->y_a;
		// calculate
		double nSX = pObj->x;
		double nSY = pObj->y;
		/// here we calculate a & b
		double a = 1;
		double b = 1;
		// if not a then b
		if( o_a != 0 )
			a = o_a;
		else if( o_b != 0 )
			a = o_b;
		// if not b then a
		if( o_b != 0 )
			b = o_b;
		else if( o_a != 0 )
			b = o_a;

		//  check if in my object
		if( IsPointInEllipse( x, y, nSX, nSY, a, b, pObj->theta, nFoundDist ) )
		{
			nFound = nLastObjId;
			nFoundDist = nDist;
			// set b found type to star=1
			nFoundType = 3;
		}
	}

	// calculate zone based on x, y
	int w_s = (int) (m_nWidth/10);
	int h_s = (int) (m_nHeight/10);
	unsigned char X_A = (unsigned char) ((x/w_s)+1);
	unsigned char Y_A = (unsigned char) ((y/h_s)+1);

	////////////////////////////////////////////////
	// :: SEARCH FOR DETECTED STARS
	// for every star in my astro image
	for( i=0; i<m_nStar; i++ )
	{
		// get quick obj ref
		pObj = &(m_vectStar[i]);
		o_a = pObj->a;
		o_b = pObj->b;
		o_k = pObj->kronfactor;
		o_x_a = pObj->x_a;
		o_y_a = pObj->y_a;

		// skip last already check 
		if( nLastObjType == 1 && i == nLastObjId ) continue;

		// check flag and skip if not
		if( (nFlag & FLAG_FIND_OBJ_WITH_DISTANCE) && pObj->distance == 0 ) continue;

		// check flag if to skip already selected
		if( (nFlag & FLAG_FIND_OBJ_NOT_SHOWN) && pObj->state == OBJECT_STATE_CUSTOM_SHOW ) continue;

		// check for zone if not base = 0 and not the same
		if( o_x_a != 0 && o_y_a != 0 && o_x_a != X_A && o_y_a != Y_A ) continue;

		// calculate
		nSX = pObj->x;
		nSY = pObj->y;
		a = o_a*o_k;
		b = o_b*o_k;

		//  check if in my object
		if( IsPointInEllipse( x, y, nSX, nSY, a, b, pObj->theta, nFoundDist ) )
		{
			nFound = i;
			nFoundDist = nDist;
			// set b found type to star=1
			nFoundType = 1;
		}
	}

	////////////////////////////////////////////////
	// :: SEARCH FOR CATALOG STARS
	// for every star in my astro image
	for( i=0; i<m_nCatStar; i++ )
	{
		// get quick obj ref
		pObj = &(m_vectCatStar[i]);
		o_a = pObj->a;
		o_b = pObj->b;
		o_k = pObj->kronfactor;
		o_x_a = pObj->x_a;
		o_y_a = pObj->y_a;

		// skip last already check 
		if( nLastObjType == 2 && i == nLastObjId ) continue;

		// check flag and skip if not
		if( (nFlag & FLAG_FIND_OBJ_WITH_DISTANCE) && pObj->distance == 0 ) continue;

		// check for zone if not base = 0 and not the same
		if( o_x_a != 0 && o_y_a != 0 && o_x_a != X_A && o_y_a != Y_A ) continue;

		// calculate
		nSX = pObj->x;
		nSY = pObj->y;
		a = o_a*o_k;
		b = o_b*o_k;

		//  check if in my object
		if( IsPointInEllipse( x, y, nSX, nSY, a, b, pObj->theta, nFoundDist ) )
		{
			nFound = i;
			nFoundDist = nDist;
			// set b found type to star=1
			nFoundType = 2;
		}
	}

	////////////////////////////////////////////////
	// :: SEARCH FOR DSO's
	// for every star in my astro image
	for( i=0; i<m_nCatDso; i++ )
	{
		// get quick obj ref
		pObj = &(m_vectCatDso[i]);
		o_a = pObj->a;
		o_b = pObj->b;
		o_k = pObj->kronfactor;
		o_x_a = pObj->x_a;
		o_y_a = pObj->y_a;

		// skip last already check 
		if( nLastObjType == 3 && i == nLastObjId ) continue;

		// check flag and skip if not
		if( (nFlag & FLAG_FIND_OBJ_WITH_DISTANCE) && pObj->distance == 0 ) continue;

		// check for zone if not base = 0 and not the same
		if( o_x_a != 0 && o_y_a != 0 && o_x_a != X_A && o_y_a != Y_A ) continue;

		// calculate
		double nSX = pObj->x;
		double nSY = pObj->y;
		/// here we calculate a & b
		double a = 1;
		double b = 1;
		// if not a then b
		if( o_a != 0 )
			a = o_a;
		else if( o_b != 0 )
			a = o_b;
		// if not b then a
		if( o_b != 0 )
			b = o_b;
		else if( o_a != 0 )
			b = o_a;

		//  check if in my object
		if( IsPointInEllipse( x, y, nSX, nSY, a, b, pObj->theta, nFoundDist ) )
		{
			nFound = i;
			nFoundDist = nDist;
			// set b found type to star=1
			nFoundType = 3;
		}
	}

	return( nFound );
}

/////////////////////////////////////////////////////////////////////
// Method:	Find Closest Object In Image
// Class:	CAstroImage
// Purpose:	find closest object in the image x/y (not ra/dec)
// Input:	point coord, flag - ie closest matched or not
// Output:	id if found >=0 or -1 if not
/////////////////////////////////////////////////////////////////////
long CAstroImage::FindClosestObjectInImage( double x0, double y0, double nMinDist, int nMatched )
{
	long nFoundId = -1;
	long i = 0;
	double nFoundMinDist=DBL_MAX;
	double nDist=DBL_MAX,nA=0,nB=0;
//	StarDef star;
	unsigned char nType = 0;

	////////////
	// for every star in my image
	for( i=0; i<m_nStar; i++ )
	{
		// if there was no match
		if( m_vectStar[i].match == nMatched  )
		{
			// calculate distance from my ref to star x/y
			nA = fabs(x0-m_vectStar[i].x);
			nB = fabs(y0-m_vectStar[i].y);
			nDist = (double) sqrt( nA*nA + nB*nB );
			if( nDist > nMinDist ) continue;
			// check if in mimum
			if( nDist < nFoundMinDist )
			{
				nFoundMinDist = nDist;
				nFoundId = i;
			}
		}
	}

	return( nFoundId );
}

// Purpose:	add a note to image/position in image
/////////////////////////////////////////////////////////////////////
int CAstroImage::AddNote( CImageNote* pNote )
{

	pNote->m_nId = m_vectNotes.size();

	// add to vector
	m_vectNotes.push_back( *pNote );

	return( m_vectNotes.size() );
}

/////////////////////////////////////////////////////////////////////
void CAstroImage::UpdateNote( long id, CImageNote* pNote )
{
	m_vectNotes[id].CopyFrom( *pNote );
}

/////////////////////////////////////////////////////////////////////
// Method:	AddStar
// Class:	CAstroImage
// Purpose:	add a new star
// Input:	x,y origin and mag .. and the rest ...
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CAstroImage::AddStar( double x, double y, int nCx, int nCy, double mag, double a,
						  double b, double theta, double kronfactor,
						  double fwhm, double flux, unsigned char nType )
{
	// here we deal with the allocation
	if( !m_vectStar )
	{
		// first allocation
		m_nStarAllocated = STAR_DETECT_BLOCK_SIZE_ALLOC;
		m_vectStar = (StarDef*) malloc( m_nStarAllocated*sizeof(StarDef) );

	} else if( m_nStar == m_nStarAllocated )
	{
		// allocate an extra block
		m_nStarAllocated += STAR_DETECT_BLOCK_SIZE_ALLOC;
		m_vectStar = (StarDef*) realloc( m_vectStar, m_nStarAllocated*sizeof(StarDef) );
	}
	// reset star
	memset( &(m_vectStar[m_nStar]), 0, sizeof(StarDef) );

	m_vectStar[m_nStar].img_id = m_nStar;

	m_vectStar[m_nStar].x = x;
	m_vectStar[m_nStar].y = y;
	m_vectStar[m_nStar].cx = nCx;
	m_vectStar[m_nStar].cy = nCy;
	//////////////////
	// calculate area/quadrant where the star is in the image
	GetEllipseImageQuadrant( x, y, a, b, theta, m_nWidth, m_nHeight, 10, 10, m_vectStar[m_nStar].x_a, m_vectStar[m_nStar].y_a );

	m_vectStar[m_nStar].mag = mag;
	// default sao_no to -1
	m_vectStar[m_nStar].cat_no = 0;
	m_vectStar[m_nStar].zone_no = 0;
	m_vectStar[m_nStar].comp_no = 0;
	m_vectStar[m_nStar].cat_id = 0;
	m_vectStar[m_nStar].cat_type = 0;

	// elipse params
	m_vectStar[m_nStar].a = a;
	m_vectStar[m_nStar].b = b;
	m_vectStar[m_nStar].theta = theta;
	m_vectStar[m_nStar].kronfactor = kronfactor;

	// initialize other fields
	m_vectStar[m_nStar].match = 0;
//	m_vectStar[m_nStar].name = NULL;
	m_vectStar[m_nStar].type = nType;
	m_vectStar[m_nStar].ra = 0;
	m_vectStar[m_nStar].dec = 0;
	m_vectStar[m_nStar].vmag = 0;

	m_vectStar[m_nStar].state = 0;
	m_vectStar[m_nStar].distance = 0;
	m_vectStar[m_nStar].cat_name = NULL;

	m_vectStar[m_nStar].fwhm = fwhm;
	m_vectStar[m_nStar].flux = flux;

//	if( m_nStar < DETECT_STAR_IMAGE_MAX_NO )
		m_nStar++;
//	else
//		return( -1 );

	return( m_nStar-1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	AddCatStar
// Class:	CAstroImage
// Purpose:	add a new star from catalog not detected to image
// Input:	reference to star
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CAstroImage::AddCatStar( StarDef& star, unsigned char nType )
{
	// here we deal with the allocation
	if( !m_vectCatStar )
	{
		// first allocation
		m_nCatStarAllocated = STAR_CAT_BLOCK_SIZE_ALLOC;
		m_vectCatStar = (StarDef*) malloc( m_nCatStarAllocated*sizeof(StarDef) );

	} else if( m_nCatStar == m_nCatStarAllocated )
	{
		// allocate an extra block
		m_nCatStarAllocated += STAR_CAT_BLOCK_SIZE_ALLOC;
		m_vectCatStar = (StarDef*) realloc( m_vectCatStar, m_nCatStarAllocated*sizeof(StarDef) );
	}
	// reset cat star
	memset( &(m_vectCatStar[m_nCatStar]), 0, sizeof(StarDef) );

	m_vectCatStar[m_nCatStar].img_id = m_nCatStar;
	m_vectCatStar[m_nCatStar].cat_id = star.cat_id;

	m_vectCatStar[m_nCatStar].x = star.x;
	m_vectCatStar[m_nCatStar].y = star.y;
	//////////////////
	// calculate area/qadrant where the star is in the image
	GetEllipseImageQuadrant( star.x, star.y, star.a, star.b, star.theta, m_nWidth, m_nHeight, 10, 10, m_vectCatStar[m_nCatStar].x_a, m_vectCatStar[m_nCatStar].y_a );

	m_vectCatStar[m_nCatStar].ra = star.ra;
	m_vectCatStar[m_nCatStar].dec = star.dec;
	m_vectCatStar[m_nCatStar].cat_no = star.cat_no;
	m_vectCatStar[m_nCatStar].type = nType;

// todo :: for now I don't store this info
	m_vectCatStar[m_nCatStar].a = star.a;
	m_vectCatStar[m_nCatStar].b = star.b;
//	m_vectCatStar[m_nCatStar].a = 3;
//	m_vectCatStar[m_nCatStar].b = 3;


	m_vectCatStar[m_nCatStar].theta = star.theta;
	m_vectCatStar[m_nCatStar].mag = star.mag;
	m_vectCatStar[m_nCatStar].cat_type = star.cat_type;

	// initialize other fields
	m_vectCatStar[m_nCatStar].match = 0;
//	m_vectCatStar[m_nCatStar].name = NULL;
//	m_vectCatStar[m_nCatStar].mag = 0;
	m_vectCatStar[m_nCatStar].vmag = 0;
//	m_vectCatStar[m_nCatStar].theta = 0;
	m_vectCatStar[m_nCatStar].kronfactor = star.kronfactor;
	m_vectCatStar[m_nCatStar].fwhm = 0;
	m_vectCatStar[m_nCatStar].zone_no = star.zone_no;
	m_vectCatStar[m_nCatStar].comp_no = star.comp_no;
	m_vectCatStar[m_nCatStar].state = 0;
	m_vectCatStar[m_nCatStar].distance = 0.0;
	m_vectCatStar[m_nCatStar].cx = star.x;
	m_vectCatStar[m_nCatStar].cy = star.y;

	m_vectCatStar[m_nCatStar].cat_name = NULL;

//	if( m_nCatDso < CAT_DSO_IMAGE_MAX_NO )
		m_nCatStar++;
//	else
//		return( -1 );

	return( m_nCatStar-1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	AddCatDso
// Class:	CAstroImage
// Purpose:	add a new dso to the catalog dso objects in trhe image
// Input:	reference to star
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CAstroImage::AddCatDso( StarDef& star, unsigned char nType )
{
	// here we deal with the allocation
	if( !m_vectCatDso )
	{
		// first allocation
		m_nCatDsoAllocated = DSO_CAT_BLOCK_SIZE_ALLOC;
		m_vectCatDso = (StarDef*) malloc( m_nCatDsoAllocated*sizeof(StarDef) );

	} else if( m_nCatDso == m_nCatDsoAllocated )
	{
		// allocate an extra block
		m_nCatDsoAllocated += DSO_CAT_BLOCK_SIZE_ALLOC;
		m_vectCatDso = (StarDef*) realloc( m_vectCatDso, m_nCatDsoAllocated*sizeof(StarDef) );
	}
	// reset cat dso
	memset( &(m_vectCatDso[m_nCatDso]), 0, sizeof(StarDef) );

	m_vectCatDso[m_nCatDso].img_id = m_nCatDso;

	m_vectCatDso[m_nCatDso].x = star.x;
	m_vectCatDso[m_nCatDso].y = star.y;
	//////////////////
	// calculate area/qadrant where the star is in the image
	GetEllipseImageQuadrant( star.x, star.y, star.a, star.b, star.theta, m_nWidth, m_nHeight, 10, 10, m_vectCatDso[m_nCatDso].x_a, m_vectCatDso[m_nCatDso].y_a );

	m_vectCatDso[m_nCatDso].ra = star.ra;
	m_vectCatDso[m_nCatDso].dec = star.dec;
	m_vectCatDso[m_nCatDso].cat_no = star.cat_no;
//	m_vectCatDso[m_nCatDso].type = nType;
	m_vectCatDso[m_nCatDso].type = star.cat_type;

	m_vectCatDso[m_nCatDso].a = star.a;
	m_vectCatDso[m_nCatDso].b = star.b;
	m_vectCatDso[m_nCatDso].theta = star.theta;
	m_vectCatDso[m_nCatDso].mag = star.mag;
	m_vectCatDso[m_nCatDso].cat_type = star.cat_type;

	// initialize other fields
	m_vectCatDso[m_nCatDso].match = 0;
//	m_vectCatDso[m_nCatDso].name = NULL;
//	m_vectCatDso[m_nCatDso].mag = 0;
	m_vectCatDso[m_nCatDso].vmag = 0;
//	m_vectCatDso[m_nCatDso].a = 0;
//	m_vectCatDso[m_nCatDso].b = 0;
//	m_vectCatDso[m_nCatDso].theta = 0;
	m_vectCatDso[m_nCatDso].kronfactor = 0;
	m_vectCatDso[m_nCatDso].fwhm = 0;
	m_vectCatDso[m_nCatDso].zone_no = 0;
	m_vectCatDso[m_nCatDso].comp_no = 0;
	m_vectCatDso[m_nCatDso].state = 0;
	m_vectCatDso[m_nCatDso].distance = 0.0;
	m_vectCatDso[m_nCatDso].cx = star.x;
	m_vectCatDso[m_nCatDso].cy = star.y;

//	if( m_nCatDso < CAT_DSO_IMAGE_MAX_NO )
		m_nCatDso++;
//	else
//		return( -1 );

	return( m_nCatDso-1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	AddRadioSource
// Class:	CAstroImage
// Purpose:	add a new dso to the catalog dso objects in trhe image
// Input:	reference to star
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CAstroImage::AddRadioSource( DefCatBasicRadio& source )
{
	// here we deal with the allocation
	if( !m_vectCatRadio )
	{
		// first allocation
		m_nCatRadioAllocated = RADIO_CAT_BLOCK_SIZE_ALLOC;
		m_vectCatRadio = (DefCatBasicRadio*) malloc( m_nCatRadioAllocated*sizeof(DefCatBasicRadio) );

	} else if( m_nCatRadio == m_nCatRadioAllocated )
	{
		// allocate an extra block
		m_nCatRadioAllocated += RADIO_CAT_BLOCK_SIZE_ALLOC;
		m_vectCatRadio = (DefCatBasicRadio*) realloc( m_vectCatRadio, m_nCatRadioAllocated*sizeof(DefCatBasicRadio) );
	}
	memset( &(m_vectCatRadio[m_nCatRadio]), 0, sizeof(DefCatBasicRadio) );

	// copy block
	memcpy( &(m_vectCatRadio[m_nCatRadio]), &source, sizeof(DefCatBasicRadio) );

/*
	// if name set
	if( source.cat_name )
	{
//#ifndef _DEBUG
//		if( m_vectCatRadio[m_nCatRadio].cat_name ) free( m_vectCatRadio[m_nCatRadio].cat_name );
//		m_vectCatRadio[m_nCatRadio].cat_name = (char*) malloc( (strlen(source.cat_name)+1)*sizeof(source.cat_name) );
//#endif
		wxStrcpy( m_vectCatRadio[m_nCatRadio].cat_name, source.cat_name );
	}

	m_vectCatRadio[m_nCatRadio].cat_no = source.cat_no;

	m_vectCatRadio[m_nCatRadio].ra = source.ra;
	m_vectCatRadio[m_nCatRadio].dec = source.dec;
	// x/y -from projection - do i need this ?
	m_vectCatRadio[m_nCatRadio].x = source.x;
	m_vectCatRadio[m_nCatRadio].y = source.y;

	// set properties
	m_vectCatRadio[m_nCatRadio].frequency = source.frequency;
	m_vectCatRadio[m_nCatRadio].flux_density = source.flux_density;
	m_vectCatRadio[m_nCatRadio].i_flux_density = source.i_flux_density;
	m_vectCatRadio[m_nCatRadio].no_spec_points = source.no_spec_points;
	m_vectCatRadio[m_nCatRadio].spec_abscissa = source.spec_abscissa;
	m_vectCatRadio[m_nCatRadio].spec_slope = source.spec_slope;

	// set types
	m_vectCatRadio[m_nCatRadio].cat_type = source.cat_type;
	m_vectCatRadio[m_nCatRadio].type = source.type;


//	m_vectCatDso[m_nCatDso].img_id = m_nCatDso;
*/

	m_nCatRadio++;

	return( m_nCatRadio-1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	AddXGammaSource
// Class:	CAstroImage
// Purpose:	add a x-ray/gamma source from catalog to the image
// Input:	reference to x-ray/gamma source
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CAstroImage::AddXGammaSource( DefCatBasicXGamma& source )
{
	// here we deal with the allocation
	if( !m_vectCatXGamma )
	{
		// first allocation
		m_nCatXGammaAllocated = XGAMMA_CAT_BLOCK_SIZE_ALLOC;
		m_vectCatXGamma = (DefCatBasicXGamma*) malloc( m_nCatXGammaAllocated*sizeof(DefCatBasicXGamma) );

	} else if( m_nCatXGamma == m_nCatXGammaAllocated )
	{
		// allocate an extra block
		m_nCatXGammaAllocated += XGAMMA_CAT_BLOCK_SIZE_ALLOC;
		m_vectCatXGamma = (DefCatBasicXGamma*) realloc( m_vectCatXGamma, m_nCatXGammaAllocated*sizeof(DefCatBasicXGamma) );
	}
	memset( &(m_vectCatXGamma[m_nCatXGamma]), 0, sizeof(DefCatBasicXGamma) );

	// copy block
	memcpy( &(m_vectCatXGamma[m_nCatXGamma]), &source, sizeof(DefCatBasicXGamma) );

/*
	// if name set
	if( source.cat_name )
	{
//#ifndef _DEBUG
//		if( m_vectCatXGamma[m_nCatXGamma].cat_name ) free( m_vectCatXGamma[m_nCatXGamma].cat_name );
//		m_vectCatXGamma[m_nCatXGamma].cat_name = (char*) malloc( (strlen(source.cat_name)+1)*sizeof(source.cat_name) );
//#endif
		wxStrcpy( m_vectCatXGamma[m_nCatXGamma].cat_name, source.cat_name );
	}

	m_vectCatXGamma[m_nCatXGamma].cat_no = source.cat_no;

	m_vectCatXGamma[m_nCatXGamma].ra = source.ra;
	m_vectCatXGamma[m_nCatXGamma].dec = source.dec;
	// x/y -from projection - do i need this ?
	m_vectCatXGamma[m_nCatXGamma].x = source.x;
	m_vectCatXGamma[m_nCatXGamma].y = source.y;

	// set properties
	m_vectCatXGamma[m_nCatXGamma].count = source.count;
	m_vectCatXGamma[m_nCatXGamma].bg_count = source.bg_count;
	m_vectCatXGamma[m_nCatXGamma].exp_time = source.exp_time;
	m_vectCatXGamma[m_nCatXGamma].flux = source.flux;
	m_vectCatXGamma[m_nCatXGamma].start_time = source.start_time;
	m_vectCatXGamma[m_nCatXGamma].duration = source.duration;

	m_vectCatXGamma[m_nCatXGamma].burst_date_time = source.burst_date_time;
	m_vectCatXGamma[m_nCatXGamma].flux_band_1 = source.flux_band_1;
	m_vectCatXGamma[m_nCatXGamma].flux_band_2 = source.flux_band_2;
	m_vectCatXGamma[m_nCatXGamma].flux_band_3 = source.flux_band_3;
	m_vectCatXGamma[m_nCatXGamma].flux_band_4 = source.flux_band_4;
	m_vectCatXGamma[m_nCatXGamma].flux_band_5 = source.flux_band_5;
	m_vectCatXGamma[m_nCatXGamma].flux_band_6 = source.flux_band_6;

	m_vectCatXGamma[m_nCatXGamma].count_band_1 = source.count_band_1;
	m_vectCatXGamma[m_nCatXGamma].count_band_2 = source.count_band_2;
	m_vectCatXGamma[m_nCatXGamma].count_band_3 = source.count_band_3;
	m_vectCatXGamma[m_nCatXGamma].count_band_4 = source.count_band_4;

	m_vectCatXGamma[m_nCatXGamma].source_type = source.source_type;
	m_vectCatXGamma[m_nCatXGamma].time_det = source.time_det;
	m_vectCatXGamma[m_nCatXGamma].interval_no = source.interval_no;
	m_vectCatXGamma[m_nCatXGamma].fluence = source.fluence;
	m_vectCatXGamma[m_nCatXGamma].peak_flux = source.peak_flux;
	m_vectCatXGamma[m_nCatXGamma].gamma = source.gamma;
	m_vectCatXGamma[m_nCatXGamma].no_detections = source.no_detections;

	// set types
	m_vectCatXGamma[m_nCatXGamma].cat_type = source.cat_type;
	m_vectCatXGamma[m_nCatXGamma].type = source.type;

//	m_vectCatDso[m_nCatDso].img_id = m_nCatDso; ??? do i need image id ?
*/

	m_nCatXGamma++;

	return( m_nCatXGamma-1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	AddSupernova
// Class:	CAstroImage
// Purpose:	add a supernova from catalog to the image
// Input:	reference to supernova to be added
// Output:	last add index
/////////////////////////////////////////////////////////////////////
long CAstroImage::AddSupernova( DefCatBasicSupernova& source )
{
	// here we deal with the allocation
	if( !m_vectCatSupernova )
	{
		// first allocation
		m_nCatSupernovaAllocated = SUPERNOVAS_CAT_BLOCK_SIZE_ALLOC;
		m_vectCatSupernova = (DefCatBasicSupernova*) malloc( m_nCatSupernovaAllocated*sizeof(DefCatBasicSupernova) );

	} else if( m_nCatSupernova == m_nCatSupernovaAllocated )
	{
		// allocate an extra block
		m_nCatSupernovaAllocated += SUPERNOVAS_CAT_BLOCK_SIZE_ALLOC;
		m_vectCatSupernova = (DefCatBasicSupernova*) realloc( m_vectCatSupernova, m_nCatSupernovaAllocated*sizeof(DefCatBasicSupernova) );
	}
	memset( &(m_vectCatSupernova[m_nCatSupernova]), 0, sizeof(DefCatBasicSupernova) );

	// if name set
	if( source.cat_name )
	{
//#ifndef _DEBUG
//		if( m_vectCatSupernova[m_nCatSupernova].cat_name ) free( m_vectCatSupernova[m_nCatSupernova].cat_name );
//		m_vectCatSupernova[m_nCatSupernova].cat_name = (char*) malloc( (strlen(source.cat_name)+1)*sizeof(source.cat_name) );
//#endif
		wxStrcpy( m_vectCatSupernova[m_nCatSupernova].cat_name, source.cat_name );
	}

	m_vectCatSupernova[m_nCatSupernova].cat_no = source.cat_no;

	m_vectCatSupernova[m_nCatSupernova].ra = source.ra;
	m_vectCatSupernova[m_nCatSupernova].dec = source.dec;
	// x/y - from projection - do i need this ?
	m_vectCatSupernova[m_nCatSupernova].x = source.x;
	m_vectCatSupernova[m_nCatSupernova].y = source.y;

	// set properties
	m_vectCatSupernova[m_nCatSupernova].mag = source.mag;
	m_vectCatSupernova[m_nCatSupernova].max_mag = source.max_mag;
	m_vectCatSupernova[m_nCatSupernova].unconfirmed = source.unconfirmed;
	m_vectCatSupernova[m_nCatSupernova].gal_center_offset_x = source.gal_center_offset_x;
	m_vectCatSupernova[m_nCatSupernova].gal_center_offset_y = source.gal_center_offset_y;
	m_vectCatSupernova[m_nCatSupernova].date_max = source.date_max;
	m_vectCatSupernova[m_nCatSupernova].date_discovery = source.date_discovery;
	m_vectCatSupernova[m_nCatSupernova].disc_method = source.disc_method;

	// strings
	wxStrcpy( m_vectCatSupernova[m_nCatSupernova].sn_type, source.sn_type );
	wxStrcpy( m_vectCatSupernova[m_nCatSupernova].parent_galaxy, source.parent_galaxy );
	wxStrcpy( m_vectCatSupernova[m_nCatSupernova].discoverer, source.discoverer );
	wxStrcpy( m_vectCatSupernova[m_nCatSupernova].prog_code, source.prog_code );

	// set types
	m_vectCatSupernova[m_nCatSupernova].cat_type = source.cat_type;
	m_vectCatSupernova[m_nCatSupernova].type = source.type;

//	m_vectCatSupernova[m_nCatSupernova].img_id = m_nCatDso; ??? do i need image id ?

	m_nCatSupernova++;

	return( m_nCatSupernova-1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	AddBlackhole
// Class:	CAstroImage
// Purpose:	add a blackhole from catalog to the image
// Input:	reference to blackhole to be added
// Output:	last added index
/////////////////////////////////////////////////////////////////////
long CAstroImage::AddBlackhole( DefCatBlackhole& source )
{
	// here we deal with the allocation
	if( !m_vectCatBlackhole )
	{
		// first allocation
		m_nCatBlackholeAllocated = BLACKHOLES_CAT_BLOCK_SIZE_ALLOC;
		m_vectCatBlackhole = (DefCatBlackhole*) malloc( m_nCatBlackholeAllocated*sizeof(DefCatBlackhole) );

	} else if( m_nCatBlackhole == m_nCatBlackholeAllocated )
	{
		// allocate an extra block
		m_nCatBlackholeAllocated += BLACKHOLES_CAT_BLOCK_SIZE_ALLOC;
		m_vectCatBlackhole = (DefCatBlackhole*) realloc( m_vectCatBlackhole, m_nCatBlackholeAllocated*sizeof(DefCatBlackhole) );
	}
	memset( &(m_vectCatBlackhole[m_nCatBlackhole]), 0, sizeof(DefCatBlackhole) );

	// if name set
	if( source.cat_name )
	{
//#ifndef _DEBUG
//		if( m_vectCatBlackhole[m_nCatBlackhole].cat_name ) free( m_vectCatSupernova[m_nCatBlackhole].cat_name );
//		m_vectCatBlackhole[m_nCatBlackhole].cat_name = (char*) malloc( (strlen(source.cat_name)+1)*sizeof(source.cat_name) );
//#endif
		wxStrcpy( m_vectCatBlackhole[m_nCatBlackhole].cat_name, source.cat_name );
	}

	m_vectCatBlackhole[m_nCatBlackhole].cat_no = source.cat_no;

	m_vectCatBlackhole[m_nCatBlackhole].ra = source.ra;
	m_vectCatBlackhole[m_nCatBlackhole].dec = source.dec;
	// x/y - from projection - do i need this ?
	m_vectCatBlackhole[m_nCatBlackhole].x = source.x;
	m_vectCatBlackhole[m_nCatBlackhole].y = source.y;

	// set properties
	m_vectCatBlackhole[m_nCatBlackhole].target_type = source.target_type;
	m_vectCatBlackhole[m_nCatBlackhole].mass = source.mass;
	m_vectCatBlackhole[m_nCatBlackhole].mass_hbeta = source.mass_hbeta;
	m_vectCatBlackhole[m_nCatBlackhole].mass_mgii = source.mass_mgii;
	m_vectCatBlackhole[m_nCatBlackhole].mass_civ = source.mass_civ;
	m_vectCatBlackhole[m_nCatBlackhole].luminosity = source.luminosity;
	m_vectCatBlackhole[m_nCatBlackhole].radio_luminosity = source.radio_luminosity;
	m_vectCatBlackhole[m_nCatBlackhole].xray_luminosity = source.xray_luminosity;
	m_vectCatBlackhole[m_nCatBlackhole].redshift = source.redshift;

	// set types
	m_vectCatBlackhole[m_nCatBlackhole].cat_type = source.cat_type;
	m_vectCatBlackhole[m_nCatBlackhole].type = source.type;

//	m_vectCatBlackhole[m_nCatBlackhole].img_id = m_nCatBlackhole; ??? do i need image id ?

	m_nCatBlackhole++;

	return( m_nCatBlackhole-1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	AddMStars
// Class:	CAstroImage
// Purpose:	add a multiple/double stars from catalog to the image
// Input:	reference to multiple/double stars to be added
// Output:	last added index
/////////////////////////////////////////////////////////////////////
long CAstroImage::AddMStars( DefCatMStars& source )
{
	// here we deal with the allocation
	if( !m_vectCatMStars )
	{
		// first allocation
		m_nCatMStarsAllocated = MSTARS_CAT_BLOCK_SIZE_ALLOC;
		m_vectCatMStars = (DefCatMStars*) malloc( m_nCatMStarsAllocated*sizeof(DefCatMStars) );

	} else if( m_nCatMStars == m_nCatMStarsAllocated )
	{
		// allocate an extra block
		m_nCatMStarsAllocated += MSTARS_CAT_BLOCK_SIZE_ALLOC;
		m_vectCatMStars = (DefCatMStars*) realloc( m_vectCatMStars, m_nCatMStarsAllocated*sizeof(DefCatMStars) );
	}
	// reset to zero
	memset( &(m_vectCatMStars[m_nCatMStars]), 0, sizeof(DefCatMStars) );

	// just copy block. ingnore the rest
	memcpy( &(m_vectCatMStars[m_nCatMStars]), &source, sizeof(DefCatMStars) );

	/*
	// if name set
	if( source.cat_name )
	{
//#ifndef _DEBUG
//		if( m_vectCatMStars[m_nCatMStars].cat_name ) free( m_vectCatMStars[m_nCatMStars].cat_name );
//		m_vectCatMStars[m_nCatMStars].cat_name = (char*) malloc( (strlen(source.cat_name)+1)*sizeof(source.cat_name) );
//#endif
		wxStrcpy( m_vectCatMStars[m_nCatMStars].cat_name, source.cat_name );
	}

	m_vectCatMStars[m_nCatMStars].cat_no = source.cat_no;

	m_vectCatMStars[m_nCatMStars].ra = source.ra;
	m_vectCatMStars[m_nCatMStars].dec = source.dec;
	// x/y - from projection - do i need this ?
	m_vectCatMStars[m_nCatMStars].x = source.x;
	m_vectCatMStars[m_nCatMStars].y = source.y;

	// set properties
	wxStrcpy( m_vectCatMStars[m_nCatMStars].spectral_type, source.spectral_type );
	wxStrcpy( m_vectCatMStars[m_nCatMStars].comp, source.comp );

	m_vectCatMStars[m_nCatMStars].mag = source.mag;
	m_vectCatMStars[m_nCatMStars].mag2 = source.mag2;

	m_vectCatMStars[m_nCatMStars].pos_ang = source.pos_ang;
	m_vectCatMStars[m_nCatMStars].pos_ang2 = source.pos_ang2;
	m_vectCatMStars[m_nCatMStars].pm_ra = source.pm_ra;
	m_vectCatMStars[m_nCatMStars].pm_dec = source.pm_dec;
	m_vectCatMStars[m_nCatMStars].pm_ra2 = source.pm_ra2;
	m_vectCatMStars[m_nCatMStars].pm_dec2 = source.pm_dec2;
	wxStrcpy( m_vectCatMStars[m_nCatMStars].pm_note, source.pm_note );
	m_vectCatMStars[m_nCatMStars].sep = source.sep;
	m_vectCatMStars[m_nCatMStars].sep2 = source.sep2;
	m_vectCatMStars[m_nCatMStars].nobs = source.nobs;
	m_vectCatMStars[m_nCatMStars].obs_date = source.obs_date;
	m_vectCatMStars[m_nCatMStars].obs_date2 = source.obs_date2;
	wxStrcpy( m_vectCatMStars[m_nCatMStars].discoverer, source.discoverer );
	wxStrcpy( m_vectCatMStars[m_nCatMStars].notes, source.notes );

	// set types
	m_vectCatMStars[m_nCatMStars].cat_type = source.cat_type;
	m_vectCatMStars[m_nCatMStars].type = source.type;

//	m_vectCatMStars[m_nCatBlackhole].img_id = m_nCatBlackhole; ??? do i need image id ?
*/

	m_nCatMStars++;

	return( m_nCatMStars-1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	AddExoplanet
// Class:	CAstroImage
// Purpose:	add an extra-solar planet from catalog to the image
// Input:	reference to extra-solar planet to be added
// Output:	last added index
/////////////////////////////////////////////////////////////////////
long CAstroImage::AddExoplanet( DefCatExoPlanet& source )
{
	// here we deal with the allocation
	if( !m_vectCatExoplanet )
	{
		// first allocation
		m_nCatExoplanetAllocated = EXOPLANETS_CAT_BLOCK_SIZE_ALLOC;
		m_vectCatExoplanet = (DefCatExoPlanet*) malloc( m_nCatExoplanetAllocated*sizeof(DefCatExoPlanet) );

	} else if( m_nCatExoplanet == m_nCatExoplanetAllocated )
	{
		// allocate an extra block
		m_nCatExoplanetAllocated += EXOPLANETS_CAT_BLOCK_SIZE_ALLOC;
		m_vectCatExoplanet = (DefCatExoPlanet*) realloc( m_vectCatExoplanet, m_nCatExoplanetAllocated*sizeof(DefCatExoPlanet) );
	}
	memset( &(m_vectCatExoplanet[m_nCatExoplanet]), 0, sizeof(DefCatExoPlanet) );

	// if name set
	if( source.cat_name )
	{
//#ifndef _DEBUG
//		if( m_vectCatExoplanet[m_nCatExoplanet].cat_name ) free( m_vectCatExoplanet[m_nCatExoplanet].cat_name );
//		m_vectCatExoplanet[m_nCatExoplanet].cat_name = (char*) malloc( (strlen(source.cat_name)+1)*sizeof(source.cat_name) );
//#endif
		wxStrcpy( m_vectCatExoplanet[m_nCatExoplanet].cat_name, source.cat_name );
	}

	m_vectCatExoplanet[m_nCatExoplanet].cat_no = source.cat_no;

	m_vectCatExoplanet[m_nCatExoplanet].ra = source.ra;
	m_vectCatExoplanet[m_nCatExoplanet].dec = source.dec;
	// x/y - from projection - do i need this ?
	m_vectCatExoplanet[m_nCatExoplanet].x = source.x;
	m_vectCatExoplanet[m_nCatExoplanet].y = source.y;

	m_vectCatExoplanet[m_nCatExoplanet].mass = source.mass;
	m_vectCatExoplanet[m_nCatExoplanet].radius = source.radius;
	m_vectCatExoplanet[m_nCatExoplanet].period = source.period;
	m_vectCatExoplanet[m_nCatExoplanet].sm_axis = source.sm_axis;
	m_vectCatExoplanet[m_nCatExoplanet].eccentricity = source.eccentricity;
	m_vectCatExoplanet[m_nCatExoplanet].inclination = source.inclination;
	m_vectCatExoplanet[m_nCatExoplanet].ang_dist = source.ang_dist;

	m_vectCatExoplanet[m_nCatExoplanet].status = source.status;
	m_vectCatExoplanet[m_nCatExoplanet].discovery_year = source.discovery_year;
	m_vectCatExoplanet[m_nCatExoplanet].update_date = source.update_date;

	m_vectCatExoplanet[m_nCatExoplanet].star_mag_v = source.star_mag_v;
	m_vectCatExoplanet[m_nCatExoplanet].star_mag_i = source.star_mag_i;
	m_vectCatExoplanet[m_nCatExoplanet].star_mag_h = source.star_mag_h;
	m_vectCatExoplanet[m_nCatExoplanet].star_mag_j = source.star_mag_j;
	m_vectCatExoplanet[m_nCatExoplanet].star_mag_k = source.star_mag_k;
	m_vectCatExoplanet[m_nCatExoplanet].star_dist = source.star_dist;
	m_vectCatExoplanet[m_nCatExoplanet].star_feh = source.star_feh;
	m_vectCatExoplanet[m_nCatExoplanet].star_mass = source.star_mass;
	m_vectCatExoplanet[m_nCatExoplanet].star_radius = source.star_radius;

	// set types
	m_vectCatExoplanet[m_nCatExoplanet].cat_type = source.cat_type;
	m_vectCatExoplanet[m_nCatExoplanet].type = source.type;

//	m_vectCatExoplanet[m_nCatExoplanet].img_id = m_nCatBlackhole; ??? do i need image id ?

	m_nCatExoplanet++;

	return( m_nCatExoplanet-1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	AddAsteroid
// Class:	CAstroImage
// Purpose:	add an asteroid from catalog to the image
// Input:	reference to asteroid to be added
// Output:	last added index
/////////////////////////////////////////////////////////////////////
long CAstroImage::AddAsteroid( DefCatAsteroid& source )
{
	// here we deal with the allocation
	if( !m_vectCatAsteroid )
	{
		// first allocation
		m_nCatAsteroidAllocated = ASTEROIDS_CAT_BLOCK_SIZE_ALLOC;
		m_vectCatAsteroid = (DefCatAsteroid*) malloc( m_nCatAsteroidAllocated*sizeof(DefCatAsteroid) );

	} else if( m_nCatAsteroid == m_nCatAsteroidAllocated )
	{
		// allocate an extra block
		m_nCatAsteroidAllocated += ASTEROIDS_CAT_BLOCK_SIZE_ALLOC;
		m_vectCatAsteroid = (DefCatAsteroid*) realloc( m_vectCatAsteroid, m_nCatAsteroidAllocated*sizeof(DefCatAsteroid) );
	}
	memset( &(m_vectCatAsteroid[m_nCatAsteroid]), 0, sizeof(DefCatAsteroid) );

	// copy all
	memcpy( &(m_vectCatAsteroid[m_nCatAsteroid]), &source, sizeof(DefCatAsteroid) );

/*
	// if name set
	if( source.cat_name )
	{
//#ifndef _DEBUG
//		if( m_vectCatAsteroid[m_nCatAsteroid].cat_name ) free( m_vectCatAsteroid[m_nCatAsteroid].cat_name );
//		m_vectCatAsteroid[m_nCatAsteroid].cat_name = (char*) malloc( (strlen(source.cat_name)+1)*sizeof(source.cat_name) );
//#endif
		wxStrcpy( m_vectCatAsteroid[m_nCatAsteroid].cat_name, source.cat_name );
	}

	m_vectCatAsteroid[m_nCatAsteroid].cat_no = source.cat_no;

	m_vectCatAsteroid[m_nCatAsteroid].ra = source.ra;
	m_vectCatAsteroid[m_nCatAsteroid].dec = source.dec;
	// x/y - from projection - do i need this ?
	m_vectCatAsteroid[m_nCatAsteroid].x = source.x;
	m_vectCatAsteroid[m_nCatAsteroid].y = source.y;

	m_vectCatAsteroid[m_nCatAsteroid].ast_no = source.ast_no;
	wxStrcpy( m_vectCatAsteroid[m_nCatAsteroid].comp_name, source.comp_name );

	m_vectCatAsteroid[m_nCatAsteroid].abs_mag_h = source.abs_mag_h;
	m_vectCatAsteroid[m_nCatAsteroid].slope_mag = source.slope_mag;
	m_vectCatAsteroid[m_nCatAsteroid].color_index = source.color_index;
	m_vectCatAsteroid[m_nCatAsteroid].iras_diam = source.iras_diam;
	wxStrcpy( m_vectCatAsteroid[m_nCatAsteroid].iras_class, source.iras_class );

	m_vectCatAsteroid[m_nCatAsteroid].no_arc = source.no_arc;
	m_vectCatAsteroid[m_nCatAsteroid].no_obs = source.no_obs;
	m_vectCatAsteroid[m_nCatAsteroid].epoch = source.epoch;
	m_vectCatAsteroid[m_nCatAsteroid].mean_anomaly = source.mean_anomaly;
	m_vectCatAsteroid[m_nCatAsteroid].arg_perihelion = source.arg_perihelion;
	m_vectCatAsteroid[m_nCatAsteroid].lon_asc_node = source.lon_asc_node;

	m_vectCatAsteroid[m_nCatAsteroid].date_orb_comp = source.date_orb_comp;
	m_vectCatAsteroid[m_nCatAsteroid].update_date = source.update_date;
	m_vectCatAsteroid[m_nCatAsteroid].inclination = source.inclination;
	m_vectCatAsteroid[m_nCatAsteroid].eccentricity = source.eccentricity;
	m_vectCatAsteroid[m_nCatAsteroid].smaj_axis = source.smaj_axis;

	// set types
	m_vectCatAsteroid[m_nCatAsteroid].cat_type = source.cat_type;
	m_vectCatAsteroid[m_nCatAsteroid].type = source.type;

//	m_vectCatAsteroid[m_nCatAsteroid].img_id = m_nCatBlackhole; ??? do i need image id ?

*/
	m_nCatAsteroid++;

	return( m_nCatAsteroid-1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	AddComet
// Class:	CAstroImage
// Purpose:	add a comet from catalog to the image
// Input:	reference to comet to be added
// Output:	last added index
/////////////////////////////////////////////////////////////////////
long CAstroImage::AddComet( DefCatComet& source )
{
	// here we deal with the allocation
	if( !m_vectCatComet )
	{
		// first allocation
		m_nCatCometAllocated = COMETS_CAT_BLOCK_SIZE_ALLOC;
		m_vectCatComet = (DefCatComet*) malloc( m_nCatCometAllocated*sizeof(DefCatComet) );

	} else if( m_nCatComet == m_nCatCometAllocated )
	{
		// allocate an extra block
		m_nCatCometAllocated += COMETS_CAT_BLOCK_SIZE_ALLOC;
		m_vectCatComet = (DefCatComet*) realloc( m_vectCatComet, m_nCatCometAllocated*sizeof(DefCatComet) );
	}
	memset( &(m_vectCatComet[m_nCatComet]), 0, sizeof(DefCatComet) );

	// if name set
	if( source.cat_name )
	{
//#ifndef _DEBUG
//		if( m_vectCatComet[m_nCatComet].cat_name ) free( m_vectCatComet[m_nCatComet].cat_name );
//		m_vectCatComet[m_nCatComet].cat_name = (char*) malloc( (strlen(source.cat_name)+1)*sizeof(source.cat_name) );
//#endif
		wxStrcpy( m_vectCatComet[m_nCatComet].cat_name, source.cat_name );
	}

	m_vectCatComet[m_nCatComet].cat_no = source.cat_no;

	m_vectCatComet[m_nCatComet].ra = source.ra;
	m_vectCatComet[m_nCatComet].dec = source.dec;
	// x/y - from projection - do i need this ?
	m_vectCatComet[m_nCatComet].x = source.x;
	m_vectCatComet[m_nCatComet].y = source.y;

	wxStrcpy( m_vectCatComet[m_nCatComet].iau_code, source.iau_code );
	wxStrcpy( m_vectCatComet[m_nCatComet].comp_name, source.comp_name );

	m_vectCatComet[m_nCatComet].note_update_date = source.note_update_date;

	m_vectCatComet[m_nCatComet].epoch_comp = source.epoch_comp;
	m_vectCatComet[m_nCatComet].rel_effect = source.rel_effect;

	m_vectCatComet[m_nCatComet].perihelion_date = source.perihelion_date;
	m_vectCatComet[m_nCatComet].perihelion_distance = source.perihelion_distance;

	m_vectCatComet[m_nCatComet].arg_perihelion = source.arg_perihelion;
	m_vectCatComet[m_nCatComet].lon_orbital_node = source.lon_orbital_node;
	m_vectCatComet[m_nCatComet].inclination = source.inclination;
	m_vectCatComet[m_nCatComet].eccentricity = source.eccentricity;

	// set types
	m_vectCatComet[m_nCatComet].cat_type = source.cat_type;
	m_vectCatComet[m_nCatComet].type = source.type;

//	m_vectCatComet[m_nCatComet].img_id = m_nCatComet; ??? do i need image id ?

	m_nCatComet++;

	return( m_nCatComet-1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	AddAes
// Class:	CAstroImage
// Purpose:	add an artficial earth satellite from catalog to the image
// Input:	reference to artificial earth satellite to be added
// Output:	last added index
/////////////////////////////////////////////////////////////////////
long CAstroImage::AddAes( DefCatAes& source )
{
	int i=0;

	// here we deal with the allocation
	if( !m_vectCatAes )
	{
		// first allocation
		m_nCatAesAllocated = AES_CAT_BLOCK_SIZE_ALLOC;
		m_vectCatAes = (DefCatAes*) calloc( m_nCatAesAllocated, sizeof(DefCatAes) );

	} else if( m_nCatAes == m_nCatAesAllocated )
	{
		// allocate an extra block
		m_nCatAesAllocated += AES_CAT_BLOCK_SIZE_ALLOC;
		m_vectCatAes = (DefCatAes*) _recalloc( m_vectCatAes, m_nCatAesAllocated, sizeof(DefCatAes) );
	}
	memset( &(m_vectCatAes[m_nCatAes]), 0, sizeof(DefCatAes) );

	// if name set
	if( source.cat_name )
	{
//#ifndef _DEBUG
//		if( m_vectCatAes[m_nCatAes].cat_name ) free( m_vectCatAes[m_nCatAes.cat_name );
//		m_vectCatAes[m_nCatAes].cat_name = (char*) malloc( (strlen(source.cat_name)+1)*sizeof(source.cat_name) );
//#endif
		strcpy( m_vectCatAes[m_nCatAes].cat_name, source.cat_name );
	}

	m_vectCatAes[m_nCatAes].cat_no = source.cat_no;

	m_vectCatAes[m_nCatAes].ra = source.ra;
	m_vectCatAes[m_nCatAes].dec = source.dec;
	// x/y - from projection - do i need this ?
	m_vectCatAes[m_nCatAes].x = source.x;
	m_vectCatAes[m_nCatAes].y = source.y;

	strcpy( m_vectCatAes[m_nCatAes].int_id_launch_piece, source.int_id_launch_piece );

	m_vectCatAes[m_nCatAes].class_of_orbit = source.class_of_orbit;
	m_vectCatAes[m_nCatAes].type_of_orbit = source.type_of_orbit;
	m_vectCatAes[m_nCatAes].obj_type = source.obj_type;
	// sat types
//	m_vectCatAes[m_nCatAes].sat_type.clear();
	for( i=0; i<source.nSatType; i++ ) m_vectCatAes[m_nCatAes].vectSatType[i] = source.vectSatType[i];
	m_vectCatAes[m_nCatAes].nSatType = source.nSatType;

	m_vectCatAes[m_nCatAes].norad_cat_no = source.norad_cat_no;
	m_vectCatAes[m_nCatAes].security_class = source.security_class;
	m_vectCatAes[m_nCatAes].int_id_year = source.int_id_year;
	m_vectCatAes[m_nCatAes].int_id_launch_no = source.int_id_launch_no;
	m_vectCatAes[m_nCatAes].ephemeris_type = source.ephemeris_type;

	m_vectCatAes[m_nCatAes].vmag = source.vmag;
	m_vectCatAes[m_nCatAes].dist_from_obs = source.dist_from_obs;

	// IF remote tle set - then get used tle id 
	if( source.vectTLERec )
	{
		unsigned long t = source.nTLERecInUse;
		m_vectCatAes[m_nCatAes].vectTLERec = (DefTLERecord*) calloc( 1, sizeof(DefTLERecord) );
		m_vectCatAes[m_nCatAes].nTLERec = 1;
		DefTLERecord* tle = m_vectCatAes[m_nCatAes].vectTLERec;
		// set tle element
		tle->epoch_time = source.vectTLERec[t].epoch_time;
		tle->orbit_decay_rate = source.vectTLERec[t].orbit_decay_rate;
		tle->mean_motion_second_time_deriv = source.vectTLERec[t].mean_motion_second_time_deriv;

		tle->object_drag_coeff = source.vectTLERec[t].object_drag_coeff;
		tle->element_number = source.vectTLERec[t].element_number;

		tle->inclination = source.vectTLERec[t].inclination;
		tle->asc_node_ra = source.vectTLERec[t].asc_node_ra;
		tle->eccentricity = source.vectTLERec[t].eccentricity;
		tle->arg_of_perigee = source.vectTLERec[t].arg_of_perigee;
		tle->mean_anomaly = source.vectTLERec[t].mean_anomaly;
		tle->mean_motion = source.vectTLERec[t].mean_motion;
		tle->orb_no_epoch = source.vectTLERec[t].orb_no_epoch;

		// push element in tle
	//	m_vectCatAes[m_nCatAes].vectTLERec.push_back( tle );
		m_vectCatAes[m_nCatAes].nTLERecInUse = source.nTLERecInUse;
	}

	m_vectCatAes[m_nCatAes].detect_time_shift = source.detect_time_shift;

	// set types
	m_vectCatAes[m_nCatAes].cat_type = source.cat_type;
	m_vectCatAes[m_nCatAes].type = source.type;

//	m_vectCatAes[m_nCatAes].img_id = m_nCatAes; ??? do i need image id ?

	m_nCatAes++;

	return( m_nCatAes-1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	AddSolarPlanet
// Class:	CAstroImage
// Purpose:	add a solar planet, natural satellite, sun or moon from
//			catalog to the image
// Input:	reference to solar object to be added
// Output:	last added index
/////////////////////////////////////////////////////////////////////
long CAstroImage::AddSolarPlanet( DefPlanet& source )
{
	int i=0;

	// here we deal with the allocation
	if( !m_vectSolarPlanets )
	{
		// first allocation
		m_nSolarPlanetsAllocated = SOLPLANETS_CAT_BLOCK_SIZE_ALLOC;
		m_vectSolarPlanets = (DefPlanet*) calloc( m_nSolarPlanetsAllocated, sizeof(DefPlanet) );

	} else if( m_nSolarPlanets == m_nSolarPlanetsAllocated )
	{
		// allocate an extra block
		m_nSolarPlanetsAllocated += SOLPLANETS_CAT_BLOCK_SIZE_ALLOC;
		m_vectSolarPlanets = (DefPlanet*) _recalloc( m_vectSolarPlanets, m_nSolarPlanetsAllocated, sizeof(DefPlanet) );
	}
	// set current to zero
	memset( &(m_vectSolarPlanets[m_nSolarPlanets]), 0, sizeof(DefPlanet) );

	// set name
	wxStrcpy( m_vectSolarPlanets[m_nSolarPlanets].obj_name, source.obj_name );

	m_vectSolarPlanets[m_nSolarPlanets].obj_id = source.obj_id;
	m_vectSolarPlanets[m_nSolarPlanets].obj_type = source.obj_type;
	m_vectSolarPlanets[m_nSolarPlanets].obj_orb_id = source.obj_orb_id;

	m_vectSolarPlanets[m_nSolarPlanets].ra = source.ra;
	m_vectSolarPlanets[m_nSolarPlanets].dec = source.dec;
	// x/y - from projection - do i need this ?
	m_vectSolarPlanets[m_nSolarPlanets].x = source.x;
	m_vectSolarPlanets[m_nSolarPlanets].y = source.y;

	m_vectSolarPlanets[m_nSolarPlanets].detect_time_shift = source.detect_time_shift;
	m_vectSolarPlanets[m_nSolarPlanets].dist_from_obs = source.dist_from_obs;

	// other properties
	m_vectSolarPlanets[m_nSolarPlanets].mass = source.mass;
	m_vectSolarPlanets[m_nSolarPlanets].volume = source.volume;

	m_nSolarPlanets++;

	return( m_nSolarPlanets-1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	AddConstellation
// Class:	CAstroImage
// Purpose:	add a new constelation found in the image
// Input:	constelation name, and cat id
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CAstroImage::AddConstellation( const wxChar* strName, unsigned int id )
{
	// CHECK ALLOCATION :: here we deal with the allocation
	if( !m_vectConstellation )
	{
		// first allocation
		m_nConstellationAllocated = CONSTELLATION_BLOCK_SIZE_ALLOC;
		m_vectConstellation = (ImgConstellationStructure*) malloc( m_nConstellationAllocated*sizeof(ImgConstellationStructure) );

	} else if( m_nConstellation == m_nConstellationAllocated )
	{
		// allocate an extra block
		m_nConstellationAllocated += CONSTELLATION_BLOCK_SIZE_ALLOC;
		m_vectConstellation = (ImgConstellationStructure*) realloc( m_vectConstellation, m_nConstellationAllocated*sizeof(ImgConstellationStructure) );
	}
	// reset to zero
	memset( &(m_vectConstellation[m_nConstellation]), 0, sizeof(ImgConstellationStructure) );

	m_vectConstellation[m_nConstellation].name = (wxChar*) malloc( (wxStrlen( strName )+1)*sizeof(wxChar) );
	wxStrcpy( m_vectConstellation[m_nConstellation].name, strName );
	m_vectConstellation[m_nConstellation].id = id;

	m_vectConstellation[m_nConstellation].vectLine = NULL;
	m_vectConstellation[m_nConstellation].nLine = 0;

	m_nConstellation++;

	return( m_nConstellation-1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	AddConstellationLine
// Class:	CAstroImage
// Purpose:	add a new constelation line
// Input:	constelation line coord
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CAstroImage::AddConstellationLine( unsigned int id, long x1, long y1, long x2, long y2 )
{
	int nLine = m_vectConstellation[id].nLine;

	if( m_vectConstellation[id].vectLine == NULL )
		m_vectConstellation[id].vectLine = (ImgConstellationLine*) malloc( (nLine+1)*sizeof(ImgConstellationLine) );
	else
		m_vectConstellation[id].vectLine = (ImgConstellationLine*) realloc( m_vectConstellation[id].vectLine, (nLine+1)*sizeof(ImgConstellationLine) );

	memset( &(m_vectConstellation[id].vectLine[nLine]), 0, sizeof(ImgConstellationLine) );

	m_vectConstellation[id].vectLine[nLine].x1 = x1;
	m_vectConstellation[id].vectLine[nLine].y1 = y1;
	m_vectConstellation[id].vectLine[nLine].x2 = x2;
	m_vectConstellation[id].vectLine[nLine].y2 = y2;

	m_vectConstellation[id].nLine++;


	return( 1 );
}

/////////////////////////////////////////////////////////////////////
double CAstroImage::SetConstellationScore( unsigned int id )
{
	int i=0, no=0;
	double total=0.0;

	for( i=0; i<m_vectConstellation[id].nLine; i++ )
	{
		long x1 = m_vectConstellation[id].vectLine[i].x1;
		long y1 = m_vectConstellation[id].vectLine[i].y1;
		long x2 = m_vectConstellation[id].vectLine[i].x2;
		long y2 = m_vectConstellation[id].vectLine[i].y2;
		//////////
		// calculate a score for constelation as how relevant is to the image
		// this is to be used for set auto-title and maybe in some other places
		if( x1 >= 0 && x1 < m_nWidth && y1 >= 0 && y1 < m_nHeight ) total += 1.0;
		no++;
		if( x2 >= 0 && x2 < m_nWidth && y2 >= 0 && y2 < m_nHeight ) total += 1.0;
		no++;
	}

	m_vectConstellation[id].score = (double) (total/(double) no);

	return( m_vectConstellation[id].score );
}

/////////////////////////////////////////////////////////////////////
void CAstroImage::SortConstellations( )
{
	std::sort( m_vectConstellation, m_vectConstellation+m_nConstellation, OnSortCnstlByScore );
	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	CalcMatchMinMax
// Class:	CAstroImage
// Purpose:	calculate maximum and minimum for matched stars on x/y
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CAstroImage::CalcMatchMinMax( )
{

	/// also calc rot angle
	if( m_pBestTrans )
	{
		m_nImgRotAngle = ( atan2( m_pBestTrans->c,m_pBestTrans->b ) * 180 )/PI;
		m_nImgTransScale = sqrt(m_pBestTrans->b*m_pBestTrans->b + m_pBestTrans->c*m_pBestTrans->c);
	}

	// disable for now to see what's up
	return;

	unsigned long i=0;
	// init min and max
	m_nRaStart = DBL_MAX;
	m_nDecStart = DBL_MAX;
	m_nRaEnd = DBL_MIN;
	m_nDecEnd = DBL_MIN;

// this break is here to find out why ?!! urgent!!!
	for( i=0; i<m_nStar; i++ )
	{
		// only look for these found to match
		if( m_vectStar[i].match ==  1 )
		{
			if( m_vectStar[i].ra < m_nRaStart ) m_nRaStart = m_vectStar[i].ra;
			if( m_vectStar[i].ra > m_nRaEnd ) m_nRaEnd = m_vectStar[i].ra;
			if( m_vectStar[i].dec < m_nDecStart ) m_nDecStart = m_vectStar[i].dec;
			if( m_vectStar[i].dec > m_nDecEnd ) m_nDecEnd = m_vectStar[i].dec;
		}
	}

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	CalcPropFromDetails
// Class:	CAstroImage
// Purpose:	calculate image properties(averge distance,etc) from
//			image details
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CAstroImage::CalcPropFromDetails( )
{
	unsigned long i=0;
	m_nDistanceAverageGalactic = 0;
	m_nDistanceAverageExtraGalactic = 0;
	long nStarFound = 0;
	long nCatStarFound = 0;
	long nCatDsoFound = 0;

	m_bIsChanged = 1;

	//////////////////////////////////
	// CALCULATE :: GALACTIC AVERAGE DISTANCE
	// for image stars
	for( i=0; i<m_nStar; i++ )
	{
		// calculate distance based on paralax
		if( m_vectStarDetail[i].parallax != 0 )
		{
			// formula - d = 1/(myp/1000) parsec ( 1 parsec = 3.26 )
			m_vectStar[i].distance = (double) (1.0/(m_vectStarDetail[i].parallax/1000.0))*UNIT_DISTANCE_PARSEC_LY;
			// add
			m_nDistanceAverageGalactic += fabs( m_vectStar[i].distance );
			// inc count
			nStarFound++;

		} else
		{
			m_vectStar[i].distance = 0;
			continue;
		}
	}

	//////////////////////////////
	// for cat stars
	for( i=0; i<m_nCatStar; i++ )
	{
		// calculate distance based on paralax
		if( m_vectCatStarDetail[i].parallax != 0 )
		{
			// formula - d = 1/(myp/1000) parsec ( 1 parsec = 3.26 )
			m_vectCatStar[i].distance = (double) (1.0/(m_vectCatStarDetail[i].parallax/1000.0))*UNIT_DISTANCE_PARSEC_LY;
			// add
			m_nDistanceAverageGalactic += fabs( m_vectCatStar[i].distance );
			// inc count
			nCatStarFound++;

		} else
		{
			m_vectCatStar[i].distance = 0;
			continue;
		}

	}
	// calculate average galactic
	if( nStarFound+nCatStarFound > 0 )
		m_nDistanceAverageGalactic = (double) m_nDistanceAverageGalactic/(nStarFound+nCatStarFound);

	//////////////////////////////////
	// CALCULATE :: EXTRA-GALACTIC AVERAGE DISTANCE
	// for cat dso
	for( i=0; i<m_nCatDso; i++ )
	{
		if( m_vectCatDso[i].distance > 0 )
		{
			// add
			m_nDistanceAverageExtraGalactic += fabs( m_vectCatDso[i].distance );
			// inc non zero found
			nCatDsoFound++;
		}
	}
	// calculate average extra-galactic
	if( nCatDsoFound )
		m_nDistanceAverageExtraGalactic = (double) m_nDistanceAverageExtraGalactic/nCatDsoFound;


	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	CalcDisplacementPointsByPos
// Class:	CAstroImage
// Purpose:	calculate displacement points based on star position shift
// Input:	nothing
// Output:	status
/////////////////////////////////////////////////////////////////////
int CAstroImage::CalcDisplacementPointsByPos( )
{
	int i=0, nFound=0;

	m_vectDisplacementPoints.clear();

	///////////////////////////////
	// calculate distortion - for every star project real ra/dec and cal dif
	double diffX=0, diffY=0;
	StarDef star;
	DisplacementPoint nDisPoint;
	double nDist = 0.0;
	double nAngle = 0.0;
	// for all detected stars in my image
	for( i=0; i<m_nStar; i++ )
	{
		// if not matched skip
		if( m_vectStar[i].match == 0 ) continue;

		memset( &star, 0, sizeof(StarDef) );

		//////////////////////
		// get ra/dec - compute projected x/y
		star.ra = m_vectStar[i].ra;
		star.dec = m_vectStar[i].dec;
		// project star using the orig ra/dec
		m_pSky->ProjectStar( star, m_nOrigRa, m_nOrigDec );
		// apply my damn sky multiplication factor
		star.x = star.x*m_nSkyProjectionScale;
		star.y = star.y*m_nSkyProjectionScale;
		// and now apply inverse trans
		m_pSky->TransObj_SkyToImage( star );
		/////////////////////
		// calculate diff between real xy and projected xy
		diffX = m_vectStar[i].x-star.x;
		diffY = m_vectStar[i].y-star.y;
		// calculate distance
		nDist = (double) sqrt( sqr(diffX) + sqr(diffY) );
		// check if difference more then
		if( nDist > 2.1 )
		{
			// calculate angle
			nAngle = (double) atan2( diffY, diffX );
			// set dispalcement point x/y
			nDisPoint.x = m_vectStar[i].x;
			nDisPoint.y = m_vectStar[i].y;
			//check by angle if pos or negative
			if( nAngle > -PI/2 && nAngle < PI/2 )
				nDisPoint.z = nDist;
			else
				nDisPoint.z = -nDist;
			// insert in my displacement vector
			m_vectDisplacementPoints.push_back( nDisPoint );
		}
	}

	nFound = m_vectDisplacementPoints.size();

	return( nFound );
}

/////////////////////////////////////////////////////////////////////
// Method:	CalcDisplacementPointsByDist
// Class:	CAstroImage
// Purpose:	calculate displacement points based on star distortion
// Input:	nothing
// Output:	status
/////////////////////////////////////////////////////////////////////
int CAstroImage::CalcDisplacementPointsByDist( )
{
	int i=0, nFound=0;

	m_vectDisplacementPoints.clear();

	///////////////////////////////
	// calculate distortion - for every star project real ra/dec and cal dif
	double diffX=0, diffY=0;
	StarDef star;
	DisplacementPoint nDisPoint;
	double nDist = 0.0;
	double nAngle = 0.0;
	// for all detected stars in my image
	for( i=0; i<m_nStar; i++ )
	{
		// if not matched skip
		//if( m_vectStar[i].match == 2 ) continue;

		//////////////////////
		// compute center shift
		diffX = (double) (m_vectStar[i].x - (double) m_vectStar[i].cx);
		diffY = (double) (m_vectStar[i].y - (double) m_vectStar[i].cy);
		nDist = (double) sqrt( sqr(diffX) + sqr(diffY) );

		// check if difference more then my thresh
		if( nDist > 1.7 )
		{
			// calculate angle
			nAngle = (double) atan2( diffY, diffX );
			// set dispalcement point x/y
			nDisPoint.x = m_vectStar[i].x;
			nDisPoint.y = m_vectStar[i].y;
			//check by angle if pos or negative
			if( nAngle > -PI/2 && nAngle < PI/2 )
				nDisPoint.z = nDist;
			else
				nDisPoint.z = -nDist;
			// insert in my displacement vector
			m_vectDisplacementPoints.push_back( nDisPoint );
		}
	}

	nFound = m_vectDisplacementPoints.size();

	return( nFound );
}

/////////////////////////////////////////////////////////////////////
// Method:	CalcDistGrid
// Class:	CAstroImage
// Purpose:	calculate distorsion grid
// Input:	nothing
// Output:	status
/////////////////////////////////////////////////////////////////////
int CAstroImage::CalcDistGrid( )
{
	int i=0, x=0, y=0;

	// hard code set division size
	m_nDistGridXDiv = 15;
	m_nDistGridYDiv = 15;
	// calculate divisions on x/y
	m_nDistGridXUnits = (int) m_nWidth/m_nDistGridXDiv+2;
	m_nDistGridYUnits = (int) m_nHeight/m_nDistGridYDiv+2;
	// allocate my dist grid vectors
	m_vectDistGridX = (double**) malloc( m_nDistGridYUnits*sizeof(double*) );
	for(i=0;i<m_nDistGridYUnits;i++ ) m_vectDistGridX[i] = (double*) malloc( m_nDistGridXUnits*sizeof(double) );
	m_vectDistGridY = (double**) malloc( m_nDistGridYUnits*sizeof(double*) );
	for(i=0;i<m_nDistGridYUnits;i++ ) m_vectDistGridY[i] = (double*) malloc( m_nDistGridXUnits*sizeof(double) );
	//m_vectDistGridZ = (double**) malloc( m_nDistGridYUnits*sizeof(double*) );
	//for(i=0;i<m_nDistGridYUnits;i++ ) m_vectDistGridZ[i] = (double*) malloc( m_nDistGridXUnits*sizeof(double) );
	// also allocate temp error vectors and counts
	double** vectDistGridErrorX = (double**) malloc( m_nDistGridYUnits*sizeof(double*) );
	int** vectDistGridErrorXCount = (int**) malloc( m_nDistGridYUnits*sizeof(int*) );
	for(i=0;i<m_nDistGridYUnits;i++ )
	{
		vectDistGridErrorX[i] = (double*) malloc( m_nDistGridXUnits*sizeof(double) );
		vectDistGridErrorXCount[i] = (int*) malloc( m_nDistGridXUnits*sizeof(int) );
	}
	// errors/counts on Y
	double** vectDistGridErrorY = (double**) malloc( m_nDistGridYUnits*sizeof(double*) );
	int** vectDistGridErrorYCount = (int**) malloc( m_nDistGridYUnits*sizeof(int*) );
	for(i=0;i<m_nDistGridYUnits;i++ )
	{
		vectDistGridErrorY[i] = (double*) malloc( m_nDistGridXUnits*sizeof(double) );
		vectDistGridErrorYCount[i] = (int*) malloc( m_nDistGridXUnits*sizeof(int) );
	}

	//////////////////////
	// initiate grid
	for( y=0; y<m_nDistGridYUnits; y++ )
	{
		for( x=0; x<m_nDistGridXUnits; x++ )
		{
			m_vectDistGridX[y][x] = (double) m_nDistGridXDiv*x;
			m_vectDistGridY[y][x] = (double) m_nDistGridYDiv*y;
			//m_vectDistGridZ[y][x] = 0.0;
			// set error vectors
			vectDistGridErrorX[y][x] = 0;
			vectDistGridErrorY[y][x] = 0;
			// set count vectors
			vectDistGridErrorXCount[y][x] = 0;
			vectDistGridErrorYCount[y][x] = 0;
		}
	}

	///////////////////////////////
	// calculate distortion - for every star project real ra/dec and cal dif
	double diffX=0, diffY=0;
	StarDef star;
	double nPointX = 0;
	double nPointY = 0;
	int nGridIdAOnX = 0;
	int nGridIdAOnY = 0;
	int nGridIdOnX = 0;
	int nGridIdOnY = 0;
	double nCurrentErrorX = 0;
	double nCurrentErrorY = 0;
	double nPointRadius = 0;
	//double nUnitRadius = sqrt( (double)(m_nDistGridXDiv/2)*(m_nDistGridXDiv/2)+(m_nDistGridYDiv/2)*(m_nDistGridYDiv/2) );
	double nUnitRadius = 1;
	for( i=0; i<m_nStar; i++ )
	{
		// if not matched skip
		if( m_vectStar[i].match == 0 ) continue;

		//////////////////////
		// get ra/dec - compute projected x/y
		star.ra = m_vectStar[i].ra;
		star.dec = m_vectStar[i].dec;
		// project star using the orig ra/dec
		m_pSky->ProjectStar( star, m_nOrigRa, m_nOrigDec );
		// apply my damn sky multiplication factor
		star.x = star.x*m_nSkyProjectionScale;
		star.y = star.y*m_nSkyProjectionScale;
		// and now apply inverse trans
		m_pSky->TransObj_SkyToImage( star );
		/////////////////////
		// calculate diff between real xy and projected xy
		diffX = m_vectStar[i].x-star.x;
		diffY = m_vectStar[i].y-star.y;
		// multiplication factor ??
		diffX *= 2.0;
		diffY *= 2.0;
		///////////////////////
		// get the four grid point in the vecinity
		nGridIdAOnX = (int) star.x/m_nDistGridXDiv;
		nGridIdAOnY = (int) star.y/m_nDistGridYDiv;

		// POINT 1 :: set errors/deviations
		nGridIdOnX = nGridIdAOnX;
		nGridIdOnY = nGridIdAOnY;
		nPointX = m_vectDistGridX[nGridIdOnY][nGridIdOnX];
		nPointY = m_vectDistGridY[nGridIdOnY][nGridIdOnX];
		//nPointRadius = sqrt( pow( fabs(star.x-nPointX), 2)+pow( fabs(star.y-nPointY),2 ) );
		nPointRadius = 1;
		// get/set error on x
		vectDistGridErrorX[nGridIdOnY][nGridIdOnX] += diffX*(nUnitRadius/nPointRadius);
		vectDistGridErrorXCount[nGridIdOnY][nGridIdOnX]++;
		// get/set error on y
		vectDistGridErrorY[nGridIdOnY][nGridIdOnX] += diffY*(nUnitRadius/nPointRadius);
		vectDistGridErrorYCount[nGridIdOnY][nGridIdOnX]++;

		// POINT 2 :: set errors/deviations
		nGridIdOnX = nGridIdAOnX+1;
		nGridIdOnY = nGridIdAOnY;
		nPointX = m_vectDistGridX[nGridIdOnY][nGridIdOnX];
		nPointY = m_vectDistGridY[nGridIdOnY][nGridIdOnX];
		//nPointRadius = sqrt( pow( fabs(star.x-nPointX), 2)+pow( fabs(star.y-nPointY),2 ) );
		nPointRadius = 1;
		// get/set error on x
		vectDistGridErrorX[nGridIdOnY][nGridIdOnX] += diffX*(nUnitRadius/nPointRadius);
		vectDistGridErrorXCount[nGridIdOnY][nGridIdOnX]++;
		// get/set error on y
		vectDistGridErrorY[nGridIdOnY][nGridIdOnX] += diffY*(nUnitRadius/nPointRadius);
		vectDistGridErrorYCount[nGridIdOnY][nGridIdOnX]++;

		// POINT 3 :: set errors/deviations
		nGridIdOnX = nGridIdAOnX+1;
		nGridIdOnY = nGridIdAOnY+1;
		nPointX = m_vectDistGridX[nGridIdOnY][nGridIdOnX];
		nPointY = m_vectDistGridY[nGridIdOnY][nGridIdOnX];
		//nPointRadius = sqrt( pow( fabs(star.x-nPointX), 2)+pow( fabs(star.y-nPointY),2 ) );
		nPointRadius = 1;
		// get/set error on x
		vectDistGridErrorX[nGridIdOnY][nGridIdOnX] += diffX*(nUnitRadius/nPointRadius);
		vectDistGridErrorXCount[nGridIdOnY][nGridIdOnX]++;
		// get/set error on y
		vectDistGridErrorY[nGridIdOnY][nGridIdOnX] += diffY*(nUnitRadius/nPointRadius);
		vectDistGridErrorYCount[nGridIdOnY][nGridIdOnX]++;

		// POINT 4 :: set errors/deviations
		nGridIdOnX = nGridIdAOnX;
		nGridIdOnY = nGridIdAOnY+1;
		nPointX = m_vectDistGridX[nGridIdOnY][nGridIdOnX];
		nPointY = m_vectDistGridY[nGridIdOnY][nGridIdOnX];
		//nPointRadius = sqrt( pow( fabs(star.x-nPointX), 2)+pow( fabs(star.y-nPointY),2 ) );
		nPointRadius = 1;
		// get/set error on x
		vectDistGridErrorX[nGridIdOnY][nGridIdOnX] += diffX*(nUnitRadius/nPointRadius);
		vectDistGridErrorXCount[nGridIdOnY][nGridIdOnX]++;
		// get/set error on y
		vectDistGridErrorY[nGridIdOnY][nGridIdOnX] += diffY*(nUnitRadius/nPointRadius);
		vectDistGridErrorYCount[nGridIdOnY][nGridIdOnX]++;

	}

	//////////////////////////////////////////////
	// set distortion grid again - with errors this time
	for( y=0; y<m_nDistGridYUnits; y++ )
	{
		for( x=0; x<m_nDistGridXUnits; x++ )
		{
			double error_x = 0.0;
			double error_y = 0.0;
			// adjust with errors by addition
			if( vectDistGridErrorXCount[y][x] != 0 )
			{
				error_x = (double) (vectDistGridErrorX[y][x]/vectDistGridErrorXCount[y][x]);
				m_vectDistGridX[y][x] = m_vectDistGridX[y][x] + error_x;
			}
			if( vectDistGridErrorYCount[y][x] != 0 )
			{
				error_y = (double) (vectDistGridErrorY[y][x]/vectDistGridErrorYCount[y][x]);
				m_vectDistGridY[y][x] = m_vectDistGridY[y][x] + error_y;
			}
			// calculate z - shift cause as sqrt
			//m_vectDistGridZ[y][x] = (double) sqrt( sqr(error_x) + sqr(error_y) );
		}
	}

	// free temporary error vectors
	for(i=0;i<m_nDistGridYUnits;i++ )
	{
		free( vectDistGridErrorX[i] );
		free( vectDistGridErrorXCount[i] );
	}
	free( vectDistGridErrorX );
	vectDistGridErrorX = NULL;
	free( vectDistGridErrorXCount );
	vectDistGridErrorXCount = NULL;
	// and y vectos
	for(i=0;i<m_nDistGridYUnits;i++ )
	{
		free( vectDistGridErrorY[i] );
		free( vectDistGridErrorYCount[i] );
	}
	free( vectDistGridErrorY );
	vectDistGridErrorY = NULL;
	free( vectDistGridErrorYCount );
	vectDistGridErrorYCount = NULL;

	// set flag to have dist grid
	m_bIsDistGrid = 1;

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	ClearDisplacementPoints
// Class:	CAstroImage
// Purpose:	clear displacement points
// Input:	nothing
// Output:	status
/////////////////////////////////////////////////////////////////////
void CAstroImage::ClearDisplacementPoints( )
{
	m_vectDisplacementPoints.clear();
	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	ClearDistGrid
// Class:	CAstroImage
// Purpose:	clear free distorsion grid
// Input:	nothing
// Output:	status
/////////////////////////////////////////////////////////////////////
void CAstroImage::ClearDistGrid( )
{
	m_bIsDistGrid = 0;
	int i=0;

	// free main vectors - to move this in close image - stop grid
	if( m_vectDistGridX )
	{
		for(i=0;i<m_nDistGridYUnits;i++ ) free( m_vectDistGridX[i] );
		free( m_vectDistGridX );
		m_vectDistGridX = NULL;
	}
	if( m_vectDistGridY )
	{
		for(i=0;i<m_nDistGridYUnits;i++ ) free( m_vectDistGridY[i] );
		free( m_vectDistGridY );
		m_vectDistGridY = NULL;
	}
//	if( m_vectDistGridZ )
//	{
//		for(i=0;i<m_nDistGridYUnits;i++ ) free( m_vectDistGridZ[i] );
//		free( m_vectDistGridZ );
//		m_vectDistGridZ = NULL;
//	}

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	ProcessStars
// Class:	CAstroImage
// Purpose:	locate stars in image
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CAstroImage::ProcessStars(  )
{

//	float nAverage = GetImgAverage( );
//	int nStarCount = SearchStars( nAverage );

	// call sextractor
//	m_pSextractor->Make( this );

	SortByMagnitude( );

//	DrawStars( 0, 255, 0 );
//	SaveResult( "result.bmp" );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Purpose:	add a new area to image
////////////////////////////////////////////////////////////////////
int CAstroImage::AddArea( int nX, int nY, int nWidth, int nHeight, int id )
{
	DefImageArea area;

	if( id < 0 )
		area.id = m_vectArea.size();
	else
		area.id = id;

	area.x = (double) nX;
	area.y = (double) nY;
	area.width = (double) nWidth;
	area.height = (double) nHeight;

	m_vectArea.push_back( area );

//	m_nArea++;
//	m_pImageGroup->SaveImageNodeLinkToArea( );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Purpose:	edit area of image
////////////////////////////////////////////////////////////////////
int CAstroImage::EditArea( long nId, int nX, int nY, int nWidth, int nHeight )
{
	int nAreaId = FindAreaById( nId );

	if( nAreaId < 0 ) return( -1 );

	m_vectArea[nAreaId].x = nX;
	m_vectArea[nAreaId].y = nY;
	m_vectArea[nAreaId].width = nWidth;
	m_vectArea[nAreaId].height = nHeight;

//	m_pImageGroup->SaveImageNodeLinkToArea( );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Purpose:	find an area by the area id
////////////////////////////////////////////////////////////////////
int CAstroImage::FindAreaById( long nId )
{
	int i = 0;
	int nArea = m_vectArea.size();

	for( i=0; i<nArea; i++ )
	{
		if( m_vectArea[i].id == nId )
		{
			return( i );
		}
	}

	return( -1 );
}

////////////////////////////////////////////////////////////////////
// Method:		GetDsoObjByCatNo
// Class:		CAstroImage
// Purpose:		get dso id by cat no and type
// Input:		dso cat no and dso cat type
// Output:		dso id
////////////////////////////////////////////////////////////////////
long CAstroImage::GetDsoObjByCatNo( unsigned long nCatNo, unsigned char nType )
{
	unsigned long i = 0;

	// check all dso objects inmy images
	for( i=0; i<m_nCatDso; i++ )
	{
		if( m_vectCatDso[i].cat_type == nType &&
			m_vectCatDso[i].cat_no == nCatNo ) return( i );
	}

	return( -1 );
}

////////////////////////////////////////////////////////////////////
// Method:		SaveProfile
// Class:		CAstroImage
// Purpose:		save profile with settings, areas, stars, catalog etc
// Input:		if to log flag
// Output:		status of the save
////////////////////////////////////////////////////////////////////
int CAstroImage::SaveProfile( int bLogFlag )
{
	wxString strLog;
	FILE* pFile = NULL;
	wxString strFile;
	int i = 0, j = 0;
//	wxChar strTmp[255];
	// strings used in profile saving
	wxChar strCatName[255];
	char str_catname[255];
	wxChar strSNType[255];
	wxChar strParentGalaxy[255];
	wxString strDiscoverer;
	wxChar strProgDiscCode[255];

	// first we check if there was a chaneg made else exit
	if( !m_bIsChanged ) return( 0 );

	// build file name
	strFile.Printf( wxT("%s%s%s%s"), m_strImagePath, wxT("\\"), m_strImageName, wxT(".ini") );

	if( bLogFlag )
	{
		strLog.Printf( wxT("INFO :: saving image %s.%s profile ..."), m_strImageName, m_strImageExt );
		m_pLogger->Log( strLog );
	}

	// open reflex file to write
	pFile = wxFopen( strFile, wxT("w") );
	// check if there is any configuration to load
	if( !pFile ) return( 0 );

	///////////////////////////////
	// write Properties
	fprintf( pFile, "[Properties]\n" );
	wxFprintf( pFile, wxT("ImgUID=%s\n"), m_strImgDataUID );
	fprintf( pFile, "OrigRa=%f\n",  m_nOrigRa );
	fprintf( pFile, "OrigDec=%f\n",  m_nOrigDec );
	fprintf( pFile, "FieldWidth=%f\n", m_nFieldWidth );
	fprintf( pFile, "FieldHeight=%f\n", m_nFieldHeight );
	fprintf( pFile, "FieldRadius=%f\n", m_nRadius );
	fprintf( pFile, "GalacticAverageDistance=%f\n", m_nDistanceAverageGalactic );
	fprintf( pFile, "ExtraGalacticAverageDistance=%f\n", m_nDistanceAverageExtraGalactic );
	fprintf( pFile, "ImageWidth=%d\n", m_nWidth );
	fprintf( pFile, "ImageHeight=%d\n", m_nHeight );
	fprintf( pFile, "MaxMag=%f\n", m_nMaxMag );
	fprintf( pFile, "MinMag=%f\n", m_nMinMag );

	// other img porperties
	fprintf( pFile, "MagToRadRatio=%f\n", m_nMagnitudeToRadiusRatio );
	// observer
	wxFprintf( pFile, wxT("ObsName=%s\n"), m_strObsName );
	wxFprintf( pFile, wxT("ObsCountry=%s\n"), m_strObsCountry );
	wxFprintf( pFile, wxT("ObsRegion=%s\n"), m_strObsRegion );
	wxFprintf( pFile, wxT("ObsCity=%s\n"), m_strObsCity );

//	fprintf( pFile, "ObsLatitude=%lf%c\n", m_nObsLatitude, m_chObsLatitude );
//	fprintf( pFile, "ObsLongitude=%lf%c\n", m_nObsLongitude, m_chObsLongitude );
	fprintf( pFile, "ObsLatitude=%lf\n", m_nObsLatitude );
	fprintf( pFile, "ObsLongitude=%lf\n", m_nObsLongitude );

	fprintf( pFile, "ObsAltitude=%lf\n", m_nObsAltitude );
	wxFprintf( pFile, wxT("ObsDateTime=%s\n"), m_nObsDateTime.Format( wxT("%m/%d/%Y %H:%M:%S") ) );
	// target
	wxFprintf( pFile, wxT("TargetName=%s\n"), m_strTargetName );
	wxFprintf( pFile, wxT("TargetDesc=%s\n"), m_strTargetDesc );
	fprintf( pFile, "TargetRa=%lf\n", m_nTargetRa );
	fprintf( pFile, "TargetDec=%lf\n", m_nTargetDec );
	// equipment
	wxFprintf( pFile, wxT("TelescopeName=%s\n"), m_strTelescopeName );
	fprintf( pFile, "TelescopeType=%d\n", m_nTelescopeType );
	fprintf( pFile, "TelescopeFocal=%lf\n", m_nTelescopeFocal );
	fprintf( pFile, "TelescopeAperture=%lf\n", m_nTelescopeAperture );
	fprintf( pFile, "TelescopeMount=%d\n", m_nTelescopeMount );
	wxFprintf( pFile, wxT("CameraName=%s\n"), m_strCameraName );
	// conditions
	fprintf( pFile, "CondSeeing=%d\n", m_nCondSeeing );
	fprintf( pFile, "CondTransp=%d\n", m_nCondTransp );
	fprintf( pFile, "CondWindSpeed=%lf\n", m_nCondWindSpeed );
	fprintf( pFile, "CondAtmPressure=%lf\n", m_nCondAtmPressure );
	fprintf( pFile, "CondTemp=%lf\n", m_nCondTemp );
	fprintf( pFile, "CondTempVar=%lf\n", m_nCondTempVar );
	fprintf( pFile, "CondHumid=%lf\n", m_nCondHumid );
	fprintf( pFile, "CondLp=%d\n", m_nCondLp );
	// imaging
	fprintf( pFile, "ExpTotalTime=%lf\n", m_nExpTotalTime );
	fprintf( pFile, "NoOfExp=%d\n", m_nNoOfExp );
	fprintf( pFile, "ExpIso=%d\n", m_nExpIso );
	wxFprintf( pFile, wxT("RegProg=%s\n"), m_strRegProg );
	wxFprintf( pFile, wxT("StackProg=%s\n"), m_strStackProg );
	wxFprintf( pFile, wxT("StackMethod=%d\n"), m_nStackMethod );
	wxFprintf( pFile, wxT("ProcProg=%s\n"), m_strProcProg );
	// add new line at the end
	fprintf( pFile, "\n" );

	///////////////////////////////
	// write Detection profile
	if( m_bIsDetected )
	{
		fprintf( pFile, "[Detection]\n" );
		fprintf( pFile, "ImgType=%d\n", m_nImgSourceType );
		fprintf( pFile, "UseChannel=%d\n", m_nUseChannel );
		// add new line at the end
		fprintf( pFile, "\n" );
	}

	///////////////////////////////
	// write Matching profile
	if( m_bIsMatched )
	{
		fprintf( pFile, "[Matching]\n" );
		// write best trans
		if( m_pBestTrans != NULL )
			WriteTransToFile( pFile, m_pBestTrans, "BestTrans=" );
		// write inverse trans
		if( m_pInvTrans != NULL )
			WriteTransToFile( pFile, m_pInvTrans, "InvTrans=" );

		// write sky projection scale
		fprintf( pFile, "SkyPrjScale=%lf\n", m_nSkyProjectionScale );
		// write catalog abreviation used
		if( m_nCatalogForMatching >=0 ) 
		{
			int nCatIndex = m_pSky->GetCatalogIndexById( m_nCatalogForMatching );
			wxFprintf( pFile, wxT("MatchCatalog=%s\n"), m_pSky->m_vectCatalogDef[nCatIndex].m_strNameAbbreviation );
		}

		// add new line at the end
		fprintf( pFile, "\n" );

	}

	///////////////////////////////
	// write hints
	if( m_nUseHint )
	{
		fprintf( pFile, "[Hint]\n" );
		fprintf( pFile, "HintType=%d\n", m_nUseHint );
		fprintf( pFile, "RelOrigRa=%f\n",  m_nRelCenterRa );
		fprintf( pFile, "RelOrigDec=%f\n",  m_nRelCenterDec );
		fprintf( pFile, "RelFieldWidth=%f\n", m_nRelFieldRa );
		fprintf( pFile, "RelFieldHeight=%f\n", m_nRelFieldDec );
		// and all the other vars
		fprintf( pFile, "OrigType=%d\n", m_nHintOrigType );
		fprintf( pFile, "ObjOrigType=%d\n", m_nHintObjOrigType );
		wxFprintf( pFile, wxT("ObjName=%d\n"), m_nHintObjName );
		wxFprintf( pFile, wxT("ObjId=%s\n"), m_strHintObjId );
		fprintf( pFile, "OrigUnits=%d\n", m_nHintOrigUnits );
		fprintf( pFile, "FieldType=%d\n", m_nHintFieldType );
		// hardware setup
		fprintf( pFile, "HardwareSetup=%d\n", m_nHintSetupId );
		////////
		// camera  - TODO :: move this in image properties instead
		fprintf( pFile, "Camera=%d,(%d:%d:%d)\n", m_nHintCameraSource,
					m_nHintCameraType, m_nHintCameraBrand, m_nHintCameraName );
		// sensor size 
		fprintf( pFile, "SensorWidth=%f\n", m_nHintSensorWidth );
		fprintf( pFile, "SensorHeight=%f\n", m_nHintSensorHeight );
		////////
		// telescope/lens - TODO :: move this in image properties instead
		fprintf( pFile, "TLens=%d,TL(%d:%d:%d:%d),BL(%d:%d:%d),FR(%d:%d:%d),EP(%d:%d:%d:%d),CL(%d:%d:%d:%d)\n",
						// TLens Source
						m_nHintTLensSource,
						// main lens
						m_rHintTLensSetup.nLensType, 
						m_rHintTLensSetup.nLensOptics,
						m_rHintTLensSetup.nLensBrand,
						m_rHintTLensSetup.nLensName,
						// barllow lens
						m_rHintTLensSetup.bBarlow,
						m_rHintTLensSetup.nBarlowLensBrand,
						m_rHintTLensSetup.nBarlowLensName,
						// focal reducer
						m_rHintTLensSetup.bFocalReducer,
						m_rHintTLensSetup.nFocalReducerBrand,
						m_rHintTLensSetup.nFocalReducerName,
						// eyepiece
						m_rHintTLensSetup.bEyepiece, 
						m_rHintTLensSetup.nEyepieceType,
						m_rHintTLensSetup.nEyepieceBrand,
						m_rHintTLensSetup.nEyepieceName,
						// camera 
						m_rHintTLensSetup.bCameraLens, 
						m_rHintTLensSetup.nCameraLensOptics,
						m_rHintTLensSetup.nCameraLensBrand,
						m_rHintTLensSetup.nCameraLensName );
		// system focal length
		fprintf( pFile, "FocalLength=%f\n", m_nHintFocalLength );
		// field
		fprintf( pFile, "FieldFormat=%d\n", m_nHintFieldFormat );

		// add new line at the end
		fprintf( pFile, "\n" );
	}

	///////////////////////////////
	// write areas
	if( m_vectArea.size() > 0 )
	{
		fprintf( pFile, "[Areas]\n" );
		// save all areas
		for( i=0; i<m_vectArea.size(); i++ )
		{
			fprintf( pFile, "%d=%d,%d,%d,%d\n", m_vectArea[i].id,
					(int) m_vectArea[i].x, (int) m_vectArea[i].y,
					(int) m_vectArea[i].width, (int) m_vectArea[i].height );
		}
		// add new line at the end
		fprintf( pFile, "\n" );
	}

	///////////////////////////////
	// write detected stars
	if( m_nStar > 0 )
	{
		fprintf( pFile, "[Detected Stars]\n" );
		// save every detected star
		for( i=0; i<m_nStar; i++ )
		{
			// set star cat name if any
			if( m_vectStar[i].cat_name )
				strcpy( str_catname, m_vectStar[i].cat_name );
			else
				strcpy( str_catname, " " );

			//                      1  2  3  4   5  6  7   8   9  10 11 12  13  14  15  16  17  18  19  20  21  22
			fprintf( pFile, "DStar=%u,%u,%u,%lu,%u,%s,%u,%u,%lf,%lf,%d,%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",
					i, m_vectStar[i].cat_type, m_vectStar[i].zone_no,	// 1,2,3
					m_vectStar[i].cat_no, m_vectStar[i].comp_no,		// 4,5
					str_catname,											// 6
					m_vectStar[i].type, m_vectStar[i].match,			// 6,7
					m_vectStar[i].x, m_vectStar[i].y,					// 8,9
					m_vectStar[i].cx, m_vectStar[i].cy,					// 10,11
					m_vectStar[i].ra, m_vectStar[i].dec,				// 12,13
					m_vectStar[i].mag, m_vectStar[i].vmag,				// 14,15
					m_vectStar[i].a, m_vectStar[i].b,					// 16,17
					m_vectStar[i].kronfactor, m_vectStar[i].theta,		// 18,19
					m_vectStar[i].fwhm, m_vectStar[i].flux, m_vectStar[i].distance );		// 20,21,22
		}
		// add new line at the end
		fprintf( pFile, "\n" );
	}

	///////////////////////////////
	// write catalog stars
	if( m_nCatStar > 0 )
	{

		fprintf( pFile, "[Catalog Stars]\n" );
		// save every catalog star
		for( i=0; i<m_nCatStar; i++ )
		{
			// set star cat name if any
			if( m_vectCatStar[i].cat_name )
				strcpy( str_catname, m_vectCatStar[i].cat_name );
			else
				strcpy( str_catname, " " );

			//                      1  2  3  4   5  6  7  8   9  10  11  12  13  14  15  16  17  18  19
			fprintf( pFile, "CStar=%u,%u,%u,%lu,%u,%s,%u,%u,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",
					i, m_vectCatStar[i].cat_type, m_vectCatStar[i].zone_no,	// 1,2,3
					m_vectCatStar[i].cat_no, m_vectCatStar[i].comp_no,		// 4,5
					str_catname,												// 6
					m_vectCatStar[i].type, m_vectCatStar[i].match,			// 6,7
					m_vectCatStar[i].x, m_vectCatStar[i].y,					// 8,9
					m_vectCatStar[i].ra, m_vectCatStar[i].dec,				// 10,11
					m_vectCatStar[i].mag, m_vectCatStar[i].vmag,			// 12,13
					m_vectCatStar[i].a, m_vectCatStar[i].b,					// 14,15
					m_vectCatStar[i].kronfactor, m_vectCatStar[i].theta,	// 16, 17
					m_vectCatStar[i].fwhm, m_vectCatStar[i].distance );		// 18, 19
		}

		// add new line at the end
		fprintf( pFile, "\n" );
	}

	///////////////////////////////
	// write catalog :: DSO
	if( m_nCatDso > 0 )
	{
		fprintf( pFile, "[Catalog Dso]\n" );
		// save every catalog dso
		for( i=0; i<m_nCatDso; i++ )
		{
			//                     1  2  3  4   5  6  7   8   9  10  11  12  13  14  15  16  17
			wxFprintf( pFile, wxT("CDso=%u,%u,%u,%lu,%u,%u,%u,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n"),
					i, m_vectCatDso[i].cat_type, m_vectCatDso[i].zone_no,	// 1,2,3
					m_vectCatDso[i].cat_no, m_vectCatDso[i].comp_no,		// 4,5
					m_vectCatDso[i].type, m_vectCatDso[i].match,			// 6,7
					m_vectCatDso[i].x, m_vectCatDso[i].y,					// 8,9
					m_vectCatDso[i].ra, m_vectCatDso[i].dec,				// 10,11
					m_vectCatDso[i].mag, m_vectCatDso[i].vmag,			// 12,13
					m_vectCatDso[i].a, m_vectCatDso[i].b,					// 14,15
					m_vectCatDso[i].kronfactor, m_vectCatDso[i].theta, m_vectCatDso[i].distance );	// 16,17

		}
		// add new line at the end
		fprintf( pFile, "\n" );
	}

	///////////////////////////////
	// write catalog :: RADIO SOURCES
	if( m_nCatRadio > 0 )
	{
		fprintf( pFile, "[Catalog Radio]\n" );
		// save every catalog radio source
		for( i=0; i<m_nCatRadio; i++ )
		{
			// set star cat name if any
			if( m_vectCatRadio[i].cat_name && strlen(m_vectCatRadio[i].cat_name) )
				strcpy( str_catname, m_vectCatRadio[i].cat_name );
			else
				strcpy( str_catname, " " );

			//                       1  2  3   4   5   6   7   8   9  10  11 12  13  14  15
			fprintf( pFile, "CRadio=%u,%u,%s,%lu,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%u,%lf,%lf,%u\n",
					i, m_vectCatRadio[i].cat_type, str_catname,					// 1,2,3
					m_vectCatRadio[i].cat_no, m_vectCatRadio[i].ra,				// 4,5
					m_vectCatRadio[i].dec, m_vectCatRadio[i].x,					// 6,7
					m_vectCatRadio[i].y, m_vectCatRadio[i].frequency,					// 8,9
					m_vectCatRadio[i].flux_density, m_vectCatRadio[i].i_flux_density,	// 10,11
					m_vectCatRadio[i].no_spec_points, m_vectCatRadio[i].spec_slope,			// 12,13
					m_vectCatRadio[i].spec_abscissa, m_vectCatRadio[i].type );				// 14,15
		}
		// add new line at the end
		fprintf( pFile, "\n" );
	}

	///////////////////////////////
	// write catalog :: X-RAY/GAMMA SOURCES
	if( m_nCatXGamma > 0 )
	{
		fprintf( pFile, "[Catalog XGamma]\n" );
		// save every catalog xray-gamma source
		for( i=0; i<m_nCatXGamma; i++ )
		{
			// set object cat name if any
			if( m_vectCatXGamma[i].cat_name && strlen(m_vectCatXGamma[i].cat_name) )
				strcpy( str_catname, m_vectCatXGamma[i].cat_name );
			else
				strcpy( str_catname, " " );

			//                        1  2  3   4   5   6   7   8   9  10  11  12  13  14  15 16 17 18 19 20 21  22  23  24  25 26  27  28  29  30  31  32 33
			fprintf( pFile, "CXGamma=%u,%u,%s,%lu,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%e,%lu,%lu,%lu,%e,%e,%e,%e,%e,%e,%lf,%lf,%lf,%lf,%u,%lu,%lu,%lf,%lf,%lf,%lu,%u\n",
					i, m_vectCatXGamma[i].cat_type, str_catname,							// 1,2,3
					m_vectCatXGamma[i].cat_no, m_vectCatXGamma[i].ra,					// 4,5
					m_vectCatXGamma[i].dec, m_vectCatXGamma[i].x,						// 6,7
					m_vectCatXGamma[i].y, m_vectCatXGamma[i].count,						// 8,9
					m_vectCatXGamma[i].bg_count, m_vectCatXGamma[i].exp_time,			// 10,11
					m_vectCatXGamma[i].flux, m_vectCatXGamma[i].start_time,				// 12,13
					m_vectCatXGamma[i].duration, m_vectCatXGamma[i].burst_date_time,	// 14,15
					m_vectCatXGamma[i].flux_band_1, m_vectCatXGamma[i].flux_band_2,		// 16,17
					m_vectCatXGamma[i].flux_band_3, m_vectCatXGamma[i].flux_band_4,		// 18,19
					m_vectCatXGamma[i].flux_band_5, m_vectCatXGamma[i].flux_band_6,		// 20,21
					m_vectCatXGamma[i].count_band_1, m_vectCatXGamma[i].count_band_2,	// 22,23
					m_vectCatXGamma[i].count_band_3, m_vectCatXGamma[i].count_band_4,	// 24,25
					m_vectCatXGamma[i].source_type, m_vectCatXGamma[i].time_det,		// 26,27
					m_vectCatXGamma[i].interval_no, m_vectCatXGamma[i].fluence,			// 28,29
					m_vectCatXGamma[i].peak_flux, m_vectCatXGamma[i].gamma,				// 30,31
					m_vectCatXGamma[i].no_detections, m_vectCatXGamma[i].type );		// 32,33
		}
		// add new line at the end
		fprintf( pFile, "\n" );
	}

	///////////////////////////////
	// write catalog :: SUPERNOVAS
	if( m_nCatSupernova > 0 )
	{
		fprintf( pFile, "[Catalog Supernova]\n" );
		// save every catalog supernova
		for( i=0; i<m_nCatSupernova; i++ )
		{
			// set supernova cat name if any
			if( m_vectCatSupernova[i].cat_name )
				wxStrcpy( strCatName, m_vectCatSupernova[i].cat_name );
			else
				wxStrcpy( strCatName, wxT(" ") );
			// :: set default supernova type
			if( wxStrlen( m_vectCatSupernova[i].sn_type ) )
				wxStrcpy( strSNType, m_vectCatSupernova[i].sn_type );
			else
				wxStrcpy( strSNType, wxT(" ") );
			// :: set default parent galaxy
			if( wxStrlen( m_vectCatSupernova[i].parent_galaxy ) )
				wxStrcpy( strParentGalaxy, m_vectCatSupernova[i].parent_galaxy );
			else
				wxStrcpy( strParentGalaxy, wxT(" ") );
			// :: set default dicoverer
			if( wxStrlen( m_vectCatSupernova[i].discoverer ) )
			{
				strDiscoverer = m_vectCatSupernova[i].discoverer;
				strDiscoverer.Replace( wxT(","), wxT(";") );

			} else
				strDiscoverer = wxT(" ");
			// :: program dicovery code
			if( wxStrlen( m_vectCatSupernova[i].prog_code ) )
				wxStrcpy( strProgDiscCode, m_vectCatSupernova[i].prog_code );
			else
				wxStrcpy( strProgDiscCode, wxT(" ") );

			//				             1  2  3   4   5   6   7   8  9  10  11 12  13  14 15  16  17 18 19 20 21
			wxFprintf( pFile, wxT("CSupernova=%u,%u,%s,%lu,%lf,%lf,%lf,%lf,%u,%lf,%lf,%s,%lf,%lf,%s,%lf,%lf,%s,%u,%s,%u\n"),
					i, m_vectCatSupernova[i].cat_type, strCatName,									// 1,2,3
					m_vectCatSupernova[i].cat_no, m_vectCatSupernova[i].ra,							// 4,5
					m_vectCatSupernova[i].dec, m_vectCatSupernova[i].x,								// 6,7
					m_vectCatSupernova[i].y, m_vectCatSupernova[i].unconfirmed,						// 8,9
					m_vectCatSupernova[i].mag, m_vectCatSupernova[i].max_mag,						// 10,11
					strSNType, m_vectCatSupernova[i].gal_center_offset_x,							// 12,13
					m_vectCatSupernova[i].gal_center_offset_y, strParentGalaxy,						// 14,15
					m_vectCatSupernova[i].date_max, m_vectCatSupernova[i].date_discovery,			// 16,17
					strDiscoverer, m_vectCatSupernova[i].disc_method,						// 18,19
					strProgDiscCode, m_vectCatSupernova[i].type );									// 20,21

		}
		// add new line at the end
		fprintf( pFile, "\n" );
	}

	///////////////////////////////
	// write catalog :: BLACKHOLES
	if( m_nCatBlackhole > 0 )
	{
		fprintf( pFile, "[Catalog BlackHole]\n" );
		// save every catalog blackhole
		for( i=0; i<m_nCatBlackhole; i++ )
		{
			// set star cat name if any
			if( m_vectCatBlackhole[i].cat_name )
				wxStrcpy( strCatName, m_vectCatBlackhole[i].cat_name );
			else
				wxStrcpy( strCatName, wxT(" ") );

			//                           1  2  3   4   5   6   7   8   9  10  11  12  13  14  15  16 17 18
			wxFprintf( pFile, wxT("CBlackHole=%u,%u,%s,%lu,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%u,%u\n"),
					i, m_vectCatBlackhole[i].cat_type, strCatName,									// 1,2,3
					m_vectCatBlackhole[i].cat_no, m_vectCatBlackhole[i].ra,							// 4,5
					m_vectCatBlackhole[i].dec, m_vectCatBlackhole[i].x,								// 6,7
					m_vectCatBlackhole[i].y, m_vectCatBlackhole[i].mass,							// 8,9
					m_vectCatBlackhole[i].mass_hbeta, m_vectCatBlackhole[i].mass_mgii,				// 10,11
					m_vectCatBlackhole[i].mass_civ, m_vectCatBlackhole[i].luminosity,				// 12,13
					m_vectCatBlackhole[i].radio_luminosity, m_vectCatBlackhole[i].xray_luminosity,	// 14,15
					m_vectCatBlackhole[i].redshift, m_vectCatBlackhole[i].target_type,				// 16,17
					m_vectCatBlackhole[i].type );													// 18
		}
		// add new line at the end
		fprintf( pFile, "\n" );
	}

	///////////////////////////////
	// write catalog :: MULTIPLE/DOUBLE STARS
	if( m_nCatMStars > 0 )
	{
		char strCatComp[255];
		char strCatSpecType[255];
		char strCatPMNote[255];
		char strCatDisc[255];
		char strCatNotes[255];

		fprintf( pFile, "[Catalog MStars]\n" );
		// save every catalog MStars
		for( i=0; i<m_nCatMStars; i++ )
		{
			// set cat name if any
			if( m_vectCatMStars[i].cat_name && strlen( m_vectCatMStars[i].cat_name ) )
				strcpy( str_catname, m_vectCatMStars[i].cat_name );
			else
				strcpy( str_catname, " " );
			// set comp if any
			if( m_vectCatMStars[i].comp && strlen( m_vectCatMStars[i].comp ) )
				strcpy( strCatComp, m_vectCatMStars[i].comp );
			else
				strcpy( strCatComp, " " );
			// set spectral type if any
			if( m_vectCatMStars[i].spectral_type && strlen( m_vectCatMStars[i].spectral_type ) )
				strcpy( strCatSpecType, m_vectCatMStars[i].spectral_type );
			else
				strcpy( strCatSpecType, " " );
			// set proper motion note if any
			if( m_vectCatMStars[i].pm_note && strlen( m_vectCatMStars[i].pm_note ) )
				strcpy( strCatPMNote, m_vectCatMStars[i].pm_note );
			else
				strcpy( strCatPMNote, " " );
			// set Discoverer if any
			if( m_vectCatMStars[i].discoverer && strlen( m_vectCatMStars[i].discoverer ) )
				strcpy( strCatDisc, m_vectCatMStars[i].discoverer );
			else
				strcpy( strCatDisc, " " );
			// set Notes if any
			if( m_vectCatMStars[i].notes && strlen( m_vectCatMStars[i].notes ) )
				strcpy( strCatNotes, m_vectCatMStars[i].notes );
			else
				strcpy( strCatNotes, " " );

			//                        1  2  3   4   5   6   7   8   9  10 11 12  13  14  15  16 17   18 19  20  21 22  23  24 25 26 27
			fprintf( pFile, "CMStars=%u,%u,%s,%lu,%lf,%lf,%lf,%lf,%lf,%lf,%s,%s,%lf,%lf,%lf,%lf,%lf,%lf,%s,%lf,%lf,%d,%lf,%lf,%s,%s,%u\n",
					i, m_vectCatMStars[i].cat_type, str_catname,						// 1,2,3
					m_vectCatMStars[i].cat_no, m_vectCatMStars[i].ra,				// 4,5
					m_vectCatMStars[i].dec, m_vectCatMStars[i].x,					// 6,7
					m_vectCatMStars[i].y, m_vectCatMStars[i].mag,					// 8,9
					m_vectCatMStars[i].mag2, strCatComp,							// 10,11
					strCatSpecType, m_vectCatMStars[i].pos_ang,						// 12,13
					m_vectCatMStars[i].pos_ang2, m_vectCatMStars[i].pm_ra,			// 14,15
					m_vectCatMStars[i].pm_dec, m_vectCatMStars[i].pm_ra2,			// 16,17
					m_vectCatMStars[i].pm_dec2, strCatPMNote,						// 18,19
					m_vectCatMStars[i].sep, m_vectCatMStars[i].sep2,				// 20,21
					m_vectCatMStars[i].nobs, m_vectCatMStars[i].obs_date,			// 22,23
					m_vectCatMStars[i].obs_date2, strCatDisc,						// 24,25
					strCatNotes, m_vectCatMStars[i].type );							// 26,27
		}
		// add new line at the end
		fprintf( pFile, "\n" );
	}

	///////////////////////////////
	// write catalog :: EXTRA-SOLAR PLANETS
	if( m_nCatExoplanet > 0 )
	{
		fprintf( pFile, "[Catalog Exoplanets]\n" );
		// save every catalog exoplanets
		for( i=0; i<m_nCatExoplanet; i++ )
		{
			// set star cat name if any
			if( m_vectCatExoplanet[i].cat_name )
				wxStrcpy( strCatName, m_vectCatExoplanet[i].cat_name );
			else
				wxStrcpy( strCatName, wxT(" ") );

			//                           1  2  3   4   5   6   7   8   9  10  11  12  13  14  15 16 17  18  19  20  21  22  23  24  25  26  27 28
			wxFprintf( pFile, wxT("CExoplanet=%u,%u,%s,%lu,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%u,%u,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%u\n"),
					i, m_vectCatExoplanet[i].cat_type, strCatName,									// 1,2,3
					m_vectCatExoplanet[i].cat_no, m_vectCatExoplanet[i].ra,							// 4,5
					m_vectCatExoplanet[i].dec, m_vectCatExoplanet[i].x,								// 6,7
					m_vectCatExoplanet[i].y, m_vectCatExoplanet[i].mass,							// 8,9
					m_vectCatExoplanet[i].radius, m_vectCatExoplanet[i].period,						// 10,11
					m_vectCatExoplanet[i].sm_axis, m_vectCatExoplanet[i].eccentricity,				// 12,13
					m_vectCatExoplanet[i].inclination, m_vectCatExoplanet[i].ang_dist,				// 14,15
					m_vectCatExoplanet[i].status, m_vectCatExoplanet[i].discovery_year,				// 16,17
					m_vectCatExoplanet[i].update_date, m_vectCatExoplanet[i].star_mag_v,			// 18,19
					m_vectCatExoplanet[i].star_mag_i, m_vectCatExoplanet[i].star_mag_h,				// 20,21
					m_vectCatExoplanet[i].star_mag_j, m_vectCatExoplanet[i].star_mag_k,				// 22,23
					m_vectCatExoplanet[i].star_dist, m_vectCatExoplanet[i].star_feh,				// 24,25
					m_vectCatExoplanet[i].star_mass, m_vectCatExoplanet[i].star_radius,				// 26,27
					m_vectCatExoplanet[i].type );													// 28
		}
		// add new line at the end
		fprintf( pFile, "\n" );
	}

	///////////////////////////////
	// write catalog :: ASTEROIDS
	if( m_nCatAsteroid > 0 )
	{
		fprintf( pFile, "[Catalog Asteroids]\n" );
		// save every catalog Asteriods
		for( i=0; i<m_nCatAsteroid; i++ )
		{
			char strCompName[255];
			char strIrasClass[255];

			// set star cat name if any
			if( m_vectCatAsteroid[i].cat_name && strlen( m_vectCatAsteroid[i].cat_name ) )
				strcpy( str_catname, m_vectCatAsteroid[i].cat_name );
			else
				strcpy( str_catname, " " );
			// comp name
			if( m_vectCatAsteroid[i].comp_name && strlen( m_vectCatAsteroid[i].comp_name ) )
				strcpy( strCompName, m_vectCatAsteroid[i].comp_name );
			else
				strcpy( strCompName, " " );
			// iras class
			if( m_vectCatAsteroid[i].iras_class && strlen( m_vectCatAsteroid[i].iras_class ) )
				strcpy( strIrasClass, m_vectCatAsteroid[i].iras_class );
			else
				strcpy( strIrasClass, " " );

			//                           1  2  3   4   5  6   7   8   9 10  11  12  13  14 15  16 17  18  19  20  21  22  23  24  25  26 27
			fprintf( pFile, "CAsteroid=%u,%u,%s,%lu,%lf,%lf,%lf,%lf,%lu,%s,%lf,%lf,%lf,%lf,%s,%lu,%u,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%u\n",
					i, m_vectCatAsteroid[i].cat_type, str_catname,								// 1,2,3
					m_vectCatAsteroid[i].cat_no, m_vectCatAsteroid[i].ra,						// 4,5
					m_vectCatAsteroid[i].dec, m_vectCatAsteroid[i].x,							// 6,7
					m_vectCatAsteroid[i].y, m_vectCatAsteroid[i].ast_no,						// 8,9
					strCompName, m_vectCatAsteroid[i].abs_mag_h,								// 10,11
					m_vectCatAsteroid[i].slope_mag, m_vectCatAsteroid[i].color_index,			// 12,13
					m_vectCatAsteroid[i].iras_diam, strIrasClass,								// 14,15
					m_vectCatAsteroid[i].no_arc, m_vectCatAsteroid[i].no_obs,					// 16,17
					m_vectCatAsteroid[i].epoch, m_vectCatAsteroid[i].mean_anomaly,				// 18,19
					m_vectCatAsteroid[i].arg_perihelion, m_vectCatAsteroid[i].lon_asc_node,		// 20,21
					m_vectCatAsteroid[i].date_orb_comp, m_vectCatAsteroid[i].update_date,		// 22,23
					m_vectCatAsteroid[i].inclination, m_vectCatAsteroid[i].eccentricity,		// 24,25
					m_vectCatAsteroid[i].smaj_axis, m_vectCatAsteroid[i].type );				// 26,27
		}
		// add new line at the end
		fprintf( pFile, "\n" );
	}

	///////////////////////////////
	// write catalog :: COMETS
	if( m_nCatComet > 0 )
	{
		fprintf( pFile, "[Catalog Comets]\n" );
		// save every catalog Comets
		for( i=0; i<m_nCatComet; i++ )
		{
			wxChar strCompName[255];
			wxChar strIAUCode[255];

			// set star cat name if any
			if( m_vectCatComet[i].cat_name && wxStrlen( m_vectCatComet[i].cat_name ) )
				wxStrcpy( strCatName, m_vectCatComet[i].cat_name );
			else
				wxStrcpy( strCatName, wxT(" ") );
			// comp name
			if( m_vectCatComet[i].comp_name && wxStrlen( m_vectCatComet[i].comp_name ) )
				wxStrcpy( strCompName, m_vectCatComet[i].comp_name );
			else
				wxStrcpy( strCompName, wxT(" ") );
			// IAU Code
			if( m_vectCatComet[i].iau_code && wxStrlen( m_vectCatComet[i].iau_code ) )
				wxStrcpy( strIAUCode, m_vectCatComet[i].iau_code );
			else
				wxStrcpy( strIAUCode, wxT(" ") );

			//                       1  2  3   4   5  6   7   8   9 10 11  12 13  14  15  16  17  18  19 20
			wxFprintf( pFile, wxT("CComet=%u,%u,%s,%lu,%lf,%lf,%lf,%lf,%lf,%s,%s,%lf,%u,%lf,%lf,%lf,%lf,%lf,%lf,%u\n"),
					i, m_vectCatComet[i].cat_type, strCatName,									// 1,2,3
					m_vectCatComet[i].cat_no, m_vectCatComet[i].ra,								// 4,5
					m_vectCatComet[i].dec, m_vectCatComet[i].x,									// 6,7
					m_vectCatComet[i].y, m_vectCatComet[i].note_update_date,					// 8,9
					strIAUCode, strCompName,													// 10,11
					m_vectCatComet[i].epoch_comp, m_vectCatComet[i].rel_effect,					// 12,13
					m_vectCatComet[i].perihelion_date, m_vectCatComet[i].perihelion_distance,	// 14,15
					m_vectCatComet[i].arg_perihelion, m_vectCatComet[i].lon_orbital_node,		// 16,17
					m_vectCatComet[i].inclination, m_vectCatComet[i].eccentricity,				// 18,19
					m_vectCatComet[i].type );													// 20
		}
		// add new line at the end
		fprintf( pFile, "\n" );
	}

	////////////////////////////////////////////////////////////////
	// write catalog :: ARTIFICIAL EARTH SATELLITES
	if( m_nCatAes > 0 )
	{
		fprintf( pFile, "[Catalog AES]\n" );
		// save every catalog Comets
		for( i=0; i<m_nCatAes; i++ )
		{
			char strIntIdLaunchPiece[255];
			char strVectSatType[500];
			memset( strVectSatType, 0, 100 );

			// set star cat name if any
			if( m_vectCatAes[i].cat_name && strlen( m_vectCatAes[i].cat_name ) )
				strcpy( str_catname, m_vectCatAes[i].cat_name );
			else
				strcpy( str_catname, " " );
			// int_id_launch_piece
			if( m_vectCatAes[i].int_id_launch_piece && strlen( m_vectCatAes[i].int_id_launch_piece ) )
				strcpy( strIntIdLaunchPiece, m_vectCatAes[i].int_id_launch_piece );
			else
				strcpy( strIntIdLaunchPiece, " " );
			// buid string satellite type
			if( m_vectCatAes[i].nSatType > 0 )
			{
				wxString strWxSatTypeVal = wxT("");
				for( j=0; j<m_vectCatAes[i].nSatType; j++ )
				{
					strWxSatTypeVal.Printf( wxT("%d:"), m_vectCatAes[i].vectSatType[j] );
					strcat( strVectSatType, strWxSatTypeVal.ToAscii() );
				}
			} else
				strcpy( strVectSatType, " " );

			//                     1  2  3   4   5  6   7   8   9 10 11  12 13 14 15 16 17  18  19  20  21  22  23  24 25  26  27  28  29  30  31 32 33
			fprintf( pFile, "CAes=%u,%u,%s,%lu,%lf,%lf,%lf,%lf,%u,%u,%s,%lu,%u,%u,%u,%s,%u,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%u,%lf,%lf,%lf,%lf,%lf,%lf,%u,%u\n",
					i, m_vectCatAes[i].cat_type, str_catname,											// 1,2,3
					m_vectCatAes[i].cat_no, m_vectCatAes[i].ra,											// 4,5
					m_vectCatAes[i].dec, m_vectCatAes[i].x,												// 6,7
					m_vectCatAes[i].y, m_vectCatAes[i].obj_type,										// 8,9
					m_vectCatAes[i].nSatType, strVectSatType,											// 10,11
					m_vectCatAes[i].norad_cat_no, m_vectCatAes[i].security_class,						// 12,13
					m_vectCatAes[i].int_id_year, m_vectCatAes[i].int_id_launch_no,						// 14,15
					strIntIdLaunchPiece, m_vectCatAes[i].ephemeris_type,								// 16,17
					m_vectCatAes[i].detect_time_shift, m_vectCatAes[i].vmag,							// 18,19
					m_vectCatAes[i].dist_from_obs, m_vectCatAes[i].vectTLERec[0].epoch_time,	// 20,21
					m_vectCatAes[i].vectTLERec[0].orbit_decay_rate,								// 22
					m_vectCatAes[i].vectTLERec[0].mean_motion_second_time_deriv,				// 23
					m_vectCatAes[i].vectTLERec[0].object_drag_coeff,							// 24
					m_vectCatAes[i].vectTLERec[0].element_number,								// 25
					m_vectCatAes[i].vectTLERec[0].inclination,									// 26 
					m_vectCatAes[i].vectTLERec[0].asc_node_ra,									// 27
					m_vectCatAes[i].vectTLERec[0].eccentricity,									// 28
					m_vectCatAes[i].vectTLERec[0].arg_of_perigee,								// 29
					m_vectCatAes[i].vectTLERec[0].mean_anomaly,									// 30
					m_vectCatAes[i].vectTLERec[0].mean_motion,									// 31
					m_vectCatAes[i].vectTLERec[0].orb_no_epoch, m_vectCatAes[i].type );			// 32,33
		}
		// add new line at the end
		fprintf( pFile, "\n" );
	}

	///////////////////////////////
	// write catalog :: Solar
	if( m_nSolarPlanets > 0 )
	{
		fprintf( pFile, "[Catalog Solar]\n" );
		// save every catalog Comets
		for( i=0; i<m_nSolarPlanets; i++ )
		{
			// set cat name if any
			if( m_vectSolarPlanets[i].obj_name && wxStrlen( m_vectSolarPlanets[i].obj_name ) )
				wxStrcpy( strCatName, m_vectSolarPlanets[i].obj_name );
			else
				wxStrcpy( strCatName, wxT(" ") );

			//                       1  2  3  4   5   6   7   8  9  10  11
			wxFprintf( pFile, wxT("CSolar=%u,%u,%s,%u,%lf,%lf,%lf,%lf,%u,%lf,%lf\n"),
					i, m_vectSolarPlanets[i].obj_type, strCatName,					// 1,2,3
					m_vectSolarPlanets[i].obj_id, m_vectSolarPlanets[i].ra,			// 4,5
					m_vectSolarPlanets[i].dec, m_vectSolarPlanets[i].x,				// 6,7
					m_vectSolarPlanets[i].y, m_vectSolarPlanets[i].obj_orb_id,		// 8,9
					m_vectSolarPlanets[i].dist_from_obs,							// 10
					m_vectSolarPlanets[i].detect_time_shift );						// 11
		}
		// add new line at the end
		fprintf( pFile, "\n" );
	}

	////////////////////////////////////////////////////////////////
	// write :: CONSTELLATIONS
	if( m_nConstellation > 0 )
	{
		fprintf( pFile, "[Constellation]\n" );
		// save every constellation
		for( i=0; i<m_nConstellation; i++ )
		{
			wxFprintf( pFile, wxT("Const=%d,%s,{"), m_vectConstellation[i].id, m_vectConstellation[i].name );
			// save every line
			for( j=0; j<m_vectConstellation[i].nLine; j++ )
			{
				fprintf( pFile, "(%d,%d,%d,%d)", m_vectConstellation[i].vectLine[j].x1,
							m_vectConstellation[i].vectLine[j].y1,
							m_vectConstellation[i].vectLine[j].x2,
							m_vectConstellation[i].vectLine[j].y2 );
				if( j < m_vectConstellation[i].nLine-1 ) fprintf( pFile, "," );
			}
			// close lines
			fprintf( pFile, "}\n" );
		}
	}

	////////////////////////////////////////////////////////////////
	// write :: NOTES
	if( m_vectNotes.size() > 0 )
	{
		for( i=0; i<m_vectNotes.size(); i++ )
		{
			fprintf( pFile, "\n[Note]\n" );
			fprintf( pFile, "Id=%d\n", m_vectNotes[i].m_nId );
			fprintf( pFile, "Type=%d\n", m_vectNotes[i].m_nType );
			fprintf( pFile, "DateTime=%lf\n", m_vectNotes[i].m_rDateTime.GetJDN() );
			wxFprintf( pFile, wxT("Title=%s\n"), m_vectNotes[i].m_strTitle );
			if( m_vectNotes[i].m_nType == 1 ) 
				wxFprintf( pFile, wxT("SndFile=%s\n"), m_vectNotes[i].m_strSoundFile );
			if(  m_vectNotes[i].x >= 0 && m_vectNotes[i].y  >= 0 ) 
				wxFprintf( pFile, wxT("Pos=%lf,%lf\n"), m_vectNotes[i].x, m_vectNotes[i].y );
			// message block - multiple lines
			wxFprintf( pFile, wxT(">>>MSG<<<\n%s\n<<<MSG>>>\n"), m_vectNotes[i].m_strMessage );

		}
	}

	// close my file handler
	fclose( pFile );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CAstroImage::Load( int bLogFlag )
{
	// get image data
	GetImage();

	// load image profile if any
	LoadProfile( bLogFlag );

	// if no hint and fits
	if( !m_nUseHint && m_pImgFmtFits )
	{
		// get ra/dec/focal
		double ra=0.0, dec=0.0, focal=0.0;
		int ishint =  m_pImgFmtFits->GetHint( ra, dec, focal );
		if( ishint & 1 )
		{
			m_nUseHint = 1;
			if( ishint & 1 ) 
			{
				m_nHintOrigType = 1;

				m_nRelCenterRa = ra;
				m_nRelCenterDec = dec;
			}
			if( ishint & 2 )
			{
				m_nHintFocalLength = focal;
				// just set a defaul -todo: use data either from fits or by camera name
				m_nHintSensorWidth = 22.5;
				m_nHintSensorHeight = 15.0;
				// also calculate field based on focal - move method to sky
				m_pSky->CalcFOVByInstr( m_nRelFieldRa, m_nRelFieldDec, m_nHintFocalLength, m_nHintSensorWidth, m_nHintSensorHeight );
			}
		}
	}

	return(1);
}

////////////////////////////////////////////////////////////////////
int CAstroImage::LoadProfile( int bLogFlag )
{
	int ret=0;

	// first try to load my custom profile
	ret = LoadCProfile( bLogFlag );
	// then try exif data
	ret = LoadExifProfile( );
	// if fits load header
	if( m_pImgFmtFits )
	{
		ret = m_pImgFmtFits->ReadHeader( GetFullName() );

		//////////////////
		// process fits value to custom profile if not set
		if( ( m_nObsLatitude == 0.0 && m_nObsLongitude == 0.0 ) &&
			( m_pImgFmtFits->m_nSiteLat != 0.0 && m_pImgFmtFits->m_nSiteLon != 0.0) )
		{
			m_nObsLatitude = m_pImgFmtFits->m_nSiteLat;
			m_nObsLongitude = m_pImgFmtFits->m_nSiteLon;
			// a;so get closest location
			CLocationEarth earth( NULL );
			earth.LoadAllCountries();
			int country=0, region=0, city=0;
			if( earth.FindClosestCity( m_nObsLatitude, m_nObsLongitude, 30.0, country, region, city ) )
			{
				m_strObsCity = wxString( earth.m_vectCountry[country].vectRegion[region].vectCity[city].name, wxConvUTF8 );
				m_strObsRegion = wxString( earth.m_vectCountry[country].vectRegion[region].name, wxConvUTF8 );
				m_strObsCountry = wxString( earth.m_vectCountry[country].name, wxConvUTF8 );
				m_nObsAltitude = earth.m_vectCountry[country].vectRegion[region].vectCity[city].alt;
			}
		}
		// process date time always fits header
		m_nObsDateTime.Set( m_pImgFmtFits->m_nObsDatetime );
		// set observer name
		if( m_strObsName.IsEmpty() ) 
		{
			if( !m_pImgFmtFits->m_strObserverName.IsEmpty() ) m_strObsName = m_pImgFmtFits->m_strObserverName;
		}

		////////////////////
		// target name
		if( m_strTargetName.IsEmpty() ) 
		{
			if( !m_pImgFmtFits->m_strObjectName.IsEmpty() ) m_strTargetName = m_pImgFmtFits->m_strObjectName;
		}
		// target desc/observation type
		if( m_strTargetDesc.IsEmpty() ) 
		{
			if( !m_pImgFmtFits->m_strObservationType.IsEmpty() ) m_strTargetDesc = m_pImgFmtFits->m_strObservationType;
		}
		// target ra/dec
		if( m_nTargetRa == 0.0 && m_nTargetDec == 0.0 )
		{
			if( m_pImgFmtFits->m_nTargetRa != 0.0 &&  m_pImgFmtFits->m_nTargetDec != 0.0 )
			{
				m_nTargetRa = m_pImgFmtFits->m_nTargetRa;
				m_nTargetDec = m_pImgFmtFits->m_nTargetDec;

			} else if( m_pImgFmtFits->m_nObjectRa != 0.0 &&  m_pImgFmtFits->m_nObjectDec != 0.0 )
			{
				m_nTargetRa = m_pImgFmtFits->m_nObjectRa;
				m_nTargetDec = m_pImgFmtFits->m_nObjectDec;

			} else if( m_pImgFmtFits->m_nTelRa != 0.0 &&  m_pImgFmtFits->m_nTelDec != 0.0 )
			{
				m_nTargetRa = m_pImgFmtFits->m_nTelRa;
				m_nTargetDec = m_pImgFmtFits->m_nTelDec;
			}
		}

		////////////////////
		// telescope name
		if( m_strTelescopeName.IsEmpty() ) 
		{
			if( !m_pImgFmtFits->m_strTelescope.IsEmpty() ) m_strTelescopeName = m_pImgFmtFits->m_strTelescope;
		}
		// focal length
		if( m_nTelescopeFocal == 0.0 )
		{
			if( m_pImgFmtFits->m_nFocalLength != 0.0 ) m_nTelescopeFocal = m_pImgFmtFits->m_nFocalLength;
		}
		// telescope aperture
		if( m_nTelescopeAperture == 0.0 )
		{
			if( m_pImgFmtFits->m_nApertureDiameter != 0.0 ) m_nTelescopeAperture = m_pImgFmtFits->m_nApertureDiameter;
		}
		// camera name
		if( m_strCameraName.IsEmpty() ) 
		{
			if( !m_pImgFmtFits->m_strCamera.IsEmpty() ) 
				m_strCameraName = m_pImgFmtFits->m_strCamera;
			else if( !m_pImgFmtFits->m_strDetector.IsEmpty() ) 
				m_strCameraName = m_pImgFmtFits->m_strDetector;
			else if( !m_pImgFmtFits->m_strInstrument.IsEmpty() ) 
				m_strCameraName = m_pImgFmtFits->m_strInstrument;
		}

		////////////////////
		// imaging/software/iso
		if( m_nExpIso == 0.0 ) 
		{
			if( m_pImgFmtFits->m_nIsoSpeed != 0.0 ) m_nExpIso = m_pImgFmtFits->m_nIsoSpeed;
		}
		// exposure time
		if( m_nExpTotalTime == 0.0 ) 
		{
			if( m_pImgFmtFits->m_nExpTime != 0.0 ) m_nExpTotalTime = m_pImgFmtFits->m_nExpTime;
		}
		// software used to create 
		if( m_strProcProg.IsEmpty() ) 
		{
			if( !m_pImgFmtFits->m_strSoftwareMod.IsEmpty() ) 
				m_strProcProg = m_pImgFmtFits->m_strSoftwareMod;
			else if( !m_pImgFmtFits->m_strSoftwareCre.IsEmpty() ) 
				m_strProcProg = m_pImgFmtFits->m_strSoftwareCre;
		}
	}

	return( ret );
}

////////////////////////////////////////////////////////////////////
// Method:		LoadProfile
// Class:		CAstroImage
// Purpose:		load profile with settings, areas, stars, catalog etc
// Input:		log flag
// Output:		status of the load
////////////////////////////////////////////////////////////////////
int CAstroImage::LoadCProfile( int bLogFlag )
{
	wxString strLog;
	FILE* pFile = NULL;
	wxString strFile=wxT("");
	wxChar strLine[3000];
	wxChar strTmp[2000];
	int nLineSize = 0;
	long nVarLong;
	double nVarDouble=0.0;
	// variables to be used in reading the data
	unsigned int nCount=0, nCatType=0, nZoneNo=0, nCompNo=0,
					nType=0, nMatch=0;
	unsigned long nCatNo=0;
	long nStar=0, nCatDso=0;
	double nX=0, nY=0, nRa=0, nDec=0, nMag=0, nVmag=0, nA=0,
			nB=0, nKronfactor=0, nTheta=0, nFwhm=0, nFlux=0, nDist=0;
	int nCx=0, nCy=0;
	wxChar strCatName[255];
	char str_catname[255];
	// flags
	int bNewLine=0;
	int nFoundSection=0;
	int bNoteMessageBody=0;

	// regex for properties labels
	wxRegEx rePropGalAvgDist = wxT( "^GalacticAverageDistance=([0-9\\+\\.\\-]+)" );
	wxRegEx rePropExtraGalAvgDist = wxT( "^ExtraGalacticAverageDistance=([0-9\\+\\.\\-]+)" );
	wxRegEx rePropMagToRadRatio = wxT( "^MagToRadRatio=([0-9\\+\\.\\-]+)" );
	wxRegEx rePropImageWidth = wxT( "^ImageWidth=([0-9]+)" );
	wxRegEx rePropImageHeight = wxT( "^ImageHeight=([0-9]+)" );
	// observer regex
	wxRegEx rePropObsName = wxT( "^ObsName=(.+)$" );
	wxRegEx rePropObsCountry = wxT( "^ObsCountry=(.+)$" );
	wxRegEx rePropObsRegion = wxT( "^ObsRegion=(.+)$" );
	wxRegEx rePropObsCity = wxT( "^ObsCity=(.+)$" );
	wxRegEx rePropObsLatitude = wxT( "^ObsLatitude=(.+)$" );
	wxRegEx rePropObsLongitude = wxT( "^ObsLongitude=(.+)$" );
	wxRegEx rePropObsAltitude = wxT( "^ObsAltitude=(.+)$" );
	wxRegEx rePropObsDateTime = wxT( "^ObsDateTime=(.+)$" );
	wxRegEx rePropTargetName = wxT( "^TargetName=(.+)$" );
	wxRegEx rePropTargetDesc = wxT( "^TargetDesc=(.+)$" );
	wxRegEx rePropTargetRa = wxT( "^TargetRa=(.+)$" );
	wxRegEx rePropTargetDec = wxT( "^TargetDec=(.+)$" );
	wxRegEx rePropTelescopeName = wxT( "^TelescopeName=(.+)$" );
	wxRegEx rePropTelescopeType = wxT( "^TelescopeType=(.+)$" );
	wxRegEx rePropTelescopeFocal = wxT( "^TelescopeFocal=(.+)$" );
	wxRegEx rePropTelescopeAperture = wxT( "^TelescopeAperture=(.+)$" );
	wxRegEx rePropTelescopeMount = wxT( "^TelescopeMount=(.+)$" );
	wxRegEx rePropCameraName = wxT( "^CameraName=(.+)$" );
	wxRegEx rePropCondSeeing = wxT( "^CondSeeing=(.+)$" );
	wxRegEx rePropCondTransp = wxT( "^CondTransp=(.+)$" );
	wxRegEx rePropCondWindSpeed = wxT( "^CondWindSpeed=(.+)$" );
	wxRegEx rePropCondAtmPressure = wxT( "^CondAtmPressure=(.+)$" );
	wxRegEx rePropCondTemp = wxT( "^CondTemp=(.+)$" );
	wxRegEx rePropCondTempVar = wxT( "^CondTempVar=(.+)$" );
	wxRegEx rePropCondHumid = wxT( "^CondHumid=(.+)$" );
	wxRegEx rePropCondLp = wxT( "^CondLp=(.+)$" );
	wxRegEx rePropExpTotalTime = wxT( "^ExpTotalTime=(.+)$" );
	wxRegEx rePropNoOfExp = wxT( "^NoOfExp=(.+)$" );
	wxRegEx rePropExpIso = wxT( "^ExpIso=(.+)$" );
	wxRegEx rePropRegProg = wxT( "^RegProg=(.+)$" );
	wxRegEx rePropStackProg = wxT( "^StackProg=(.+)$" );
	wxRegEx rePropStackMethod = wxT( "^StackMethod=(.+)$" );
	wxRegEx rePropProcProg = wxT( "^ProcProg=(.+)$" );
	// regex for hints
	wxRegEx reHintType = wxT( "^HintType=([0-9]+)" );
	wxRegEx reHintOrigRa = wxT( "^RelOrigRa=([0-9\\+\\.\\-]+)" );
	wxRegEx reHintOrigDec = wxT( "^RelOrigDec=([0-9\\+\\.\\-]+)" );
	wxRegEx reHintFieldWidth = wxT( "^RelFieldWidth=([0-9\\+\\.\\-]+)" );
	wxRegEx reHintFieldHeight = wxT( "^RelFieldHeight=([0-9\\+\\.\\-]+)" );
	wxRegEx reHintOrigType = wxT( "^OrigType=([0-9]+)" );
	wxRegEx reHintObjOrigType = wxT( "^ObjOrigType=([0-9]+)" );
	wxRegEx reHintObjName = wxT( "^ObjName=([0-9]+)" );
	wxRegEx reHintObjId = wxT( "^ObjId=([0-9a-zA-Z\\ \\-]+)" );
	wxRegEx reHintOrigUnits = wxT( "^OrigUnits=([0-9]+)" );
	wxRegEx reHintFieldType = wxT( "^FieldType=([0-9]+)" );
	// setup
	// hardware setup
	wxRegEx reHintHardwareSetup = wxT( "^HardwareSetup=(.+)" );
	// camera
	wxRegEx reHintCamera = wxT( "^Camera=(.+)" );
	// sensor
	wxRegEx reHintSensorWidth = wxT( "^SensorWidth=([0-9\\.]+)" );
	wxRegEx reHintSensorHeight = wxT( "^SensorHeight=([0-9\\.]+)" );
	// telescope
	wxRegEx reHintTLens = wxT( "^TLens=(.+)" );
	// system focal length
	wxRegEx reHintFocalLength = wxT( "^FocalLength=([0-9\\.]+)" );
	// field
	wxRegEx reHintFieldFormat = wxT( "^FieldFormat=([0-9]+)" );
	// regex for detection
	wxRegEx reDetectImageType = wxT( "^ImgType=([0-9]+)" );
	wxRegEx reDetectUseChannel = wxT( "^UseChannel=([0-9]+)" );
	// regex for matching
	wxRegEx reMatchBestTrans = wxT( "^BestTrans=(.+)$" );
	wxRegEx reMatchInvTrans = wxT( "^InvTrans=(.+)$" );
	wxRegEx reMatchSkyPrjScale = wxT( "^SkyPrjScale=([0-9\\+\\.\\-]+)" );
	// regex for catalog radio
	wxRegEx reCatalogRadio = wxT( "^CRadio=(.+)$" );
	// regex for catalog x-ray/gamma
	wxRegEx reCatalogXGamma = wxT( "^CXGamma=(.+)$" );
	// regex for Supernovas
//	wxRegEx reCatalogSupernova = wxT( "^CSupernova=(.+)$" );
	// regex for Blackholes
	wxRegEx reCatalogBlackhole = wxT( "^CBlackHole=(.+)$" );
	// regex for MStars
	wxRegEx reCatalogMStars = wxT( "^CMStars=(.+)$" );
	// regex for Exoplanets
	wxRegEx reCatalogExoplanet = wxT( "^CExoplanet=(.+)$" );
	// regex for Asteroids
	wxRegEx reCatalogAsteroid = wxT( "^CAsteroid=(.+)$" );
	// regex for Comets
	wxRegEx reCatalogComet = wxT( "^CComet=(.+)$" );
	// regex for AES
	wxRegEx reCatalogAes = wxT( "^CAes=(.+)$" );
	// regex for Solar
	wxRegEx reCatalogSolar = wxT( "^CSolar=(.+)$" );
	// regex for constellations
	wxRegEx reConstellations = wxT( "^Const=(.+),(.+),\\{(.+)\\}" );
	wxRegEx reConstellationLine = wxT( "\\(([0-9\\+\\-]+),([0-9\\+\\-]+),([0-9\\+\\-]+),([0-9\\+\\-]+)\\)" );

	strFile.Printf( wxT("%s%s%s%s"), m_strImagePath, wxT("\\"), m_strImageName, wxT(".ini") );

	pFile = wxFopen( strFile, wxT("r") );

	// check if there is any configuration to load
	if( !pFile )
	{
		if( bLogFlag )
		{
			strLog.Printf( wxT("INFO :: image %s.%s doesn't have a profile"), m_strImageName, m_strImageExt );
			m_pLogger->Log( strLog );
		}

		return( -1 );
	}
	//  log if to log ...
	if( bLogFlag )
	{
		strLog.Printf( wxT("INFO :: loading image %s.%s profile ..."), m_strImageName, m_strImageExt );
		m_pLogger->Log( strLog );
	}

	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// clear thy buffers
//		bzero( strLine, 2000 );

		// get one line out of the config file
//		bzero( strLine, 2000 );
		wxFgets( strLine, 2000, pFile );
		nLineSize = wxStrlen( strLine );
//		wxString strLine(strLine,wxConvUTF8);
		bNewLine = 1;

		// first check for label indicators [] to speed up things
		if( strLine[0] == '[' && strLine[nLineSize-2] == ']' )
		{
			bNewLine = 0;
			///////////////////
			// check for section label
			if( wxStrncmp( strLine, wxT("[Properties]"), 12 ) == 0 )
				nFoundSection = ASTROIMG_PROF_SECTION_ID_PROPERTIES;
			else if( wxStrncmp( strLine, wxT("[Detection]"), 11 ) == 0 )
			{
				nFoundSection = ASTROIMG_PROF_SECTION_ID_DETECTION;
				m_bIsDetected = 1;

			} else if( wxStrncmp( strLine, wxT("[Matching]"), 10 ) == 0 )
			{
				nFoundSection = ASTROIMG_PROF_SECTION_ID_MATCHING;
				// also set local flag matched
				m_bIsDetected = 1;
				m_bIsMatched = 1;

			} else if( wxStrncmp( strLine, wxT("[Hint]"), 6 ) == 0 )
			{
				nFoundSection = ASTROIMG_PROF_SECTION_ID_HINT;
				// also set local that we have a hint
				//m_bIsHint = 1;

			} else if( wxStrncmp( strLine, wxT("[Areas]"), 7 ) == 0 )
				nFoundSection = ASTROIMG_PROF_SECTION_ID_AREAS;
			else if( wxStrncmp( strLine, wxT("[Detected Stars]"), 16 ) == 0 )
				nFoundSection = ASTROIMG_PROF_SECTION_ID_DETECTED_STARS;
			 else if( wxStrncmp( strLine, wxT("[Catalog Stars]"), 15 ) == 0 )
				nFoundSection = ASTROIMG_PROF_SECTION_ID_CAT_STARS;
			else if( wxStrncmp( strLine, wxT("[Catalog Dso]"), 13 ) == 0 )
				nFoundSection = ASTROIMG_PROF_SECTION_ID_CAT_DSO;
			else if( wxStrncmp( strLine, wxT("[Catalog Radio]"), 15 ) == 0 )
				nFoundSection = ASTROIMG_PROF_SECTION_ID_CAT_RADIO;
			else if( wxStrncmp( strLine, wxT("[Catalog XGamma]"), 16 ) == 0 )
				nFoundSection = ASTROIMG_PROF_SECTION_ID_CAT_XGAMMA;
			else if( wxStrncmp( strLine, wxT("[Catalog Supernova]"), 19 ) == 0 )
				nFoundSection = ASTROIMG_PROF_SECTION_ID_CAT_SUPERNOVA;
			else if( wxStrncmp( strLine, wxT("[Catalog BlackHole]"), 19 ) == 0 )
				nFoundSection = ASTROIMG_PROF_SECTION_ID_CAT_BLACKHOLE;
			else if( wxStrncmp( strLine, wxT("[Catalog MStars]"), 16 ) == 0 )
				nFoundSection = ASTROIMG_PROF_SECTION_ID_CAT_MSTARS;
			else if( wxStrncmp( strLine, wxT("[Catalog Exoplanets]"), 20 ) == 0 )
				nFoundSection = ASTROIMG_PROF_SECTION_ID_CAT_EXOPLANETS;
			else if( wxStrncmp( strLine, wxT("[Catalog Asteroids]"), 19 ) == 0 )
				nFoundSection = ASTROIMG_PROF_SECTION_ID_CAT_ASTEROIDS;
			else if( wxStrncmp( strLine, wxT("[Catalog Comets]"), 16 ) == 0 )
				nFoundSection = ASTROIMG_PROF_SECTION_ID_CAT_COMETS;
			else if( wxStrncmp( strLine, wxT("[Catalog AES]"), 13 ) == 0 )
				nFoundSection = ASTROIMG_PROF_SECTION_ID_CAT_AES;
			else if( wxStrncmp( strLine, wxT("[Catalog Solar]"), 15 ) == 0 )
				nFoundSection = ASTROIMG_PROF_SECTION_ID_CAT_PLANETS;
			else if( wxStrncmp( strLine, wxT("[Constellation]"), 15 ) == 0 )
				nFoundSection = ASTROIMG_PROF_SECTION_ID_CAT_CONSTL;
			else if( wxStrncmp( strLine, wxT("[Note]"), 6 ) == 0 )
			{
				nFoundSection = ASTROIMG_PROF_SECTION_ID_NOTE;
				CImageNote note(this);
				m_vectNotes.push_back( note );
				bNoteMessageBody = 0;
			}


		// now we check by flags for every section with it's labels
		/////////////////////////////////
		// SECTION :: PROPERTIES
		} else if( nFoundSection == ASTROIMG_PROF_SECTION_ID_PROPERTIES && bNewLine )
		{
			// here we check by the individual properties labels
			if( wxStrncmp( strLine, wxT("ImgUID="), 7 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("ImgUID=%s\n"), strTmp ) ) m_strImgDataUID = strTmp;

			} else if( wxStrncmp( strLine, wxT("OrigRa="), 7 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("OrigRa=%lf\n"), &m_nOrigRa ) ) m_nOrigRa = 0;

			} else if( wxStrncmp( strLine, wxT("OrigDec="), 8 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("OrigDec=%lf\n"), &m_nOrigDec ) ) m_nOrigDec = 0;

			} else if( wxStrncmp( strLine, wxT("FieldWidth="), 11 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("FieldWidth=%lf\n"), &m_nFieldWidth ) ) m_nFieldWidth = 0;

			} else if( wxStrncmp( strLine, wxT("FieldHeight="), 12 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("FieldHeight=%lf\n"), &m_nFieldHeight ) ) m_nFieldHeight = 0;

			} else if( rePropImageWidth.Matches( strLine ) )
			{
				if( !rePropImageWidth.GetMatch(strLine, 1 ).ToLong( &nVarLong ) )
					m_nWidth = 0;
				else
					m_nWidth = (int) nVarLong;

			} else if( rePropImageHeight.Matches( strLine ) )
			{
				if( !rePropImageHeight.GetMatch(strLine, 1 ).ToLong( &nVarLong ) )
					m_nHeight = 0;
				else
					m_nHeight = (int) nVarLong;

			} else if( wxStrncmp( strLine, wxT("MaxMag="), 7 ) == 0 )
			{
				if( wxSscanf( strLine, wxT("MaxMag=%lf\n"), &nVarDouble ) ) m_nMaxMag = nVarDouble;

			} else if( wxStrncmp( strLine, wxT("MinMag="), 7 ) == 0 )
			{
				if( wxSscanf( strLine, wxT("MinMag=%lf\n"), &nVarDouble ) ) m_nMinMag = nVarDouble;

			} else if( wxStrncmp( strLine, wxT("FieldRadius="), 12 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("FieldRadius=%lf\n"), &m_nRadius ) ) m_nRadius = 0;

			} else if( rePropGalAvgDist.Matches( strLine ) )
			{
				if( !rePropGalAvgDist.GetMatch(strLine, 1 ).ToDouble( &m_nDistanceAverageGalactic ) )
					m_nDistanceAverageGalactic = 0;

			} else if( rePropExtraGalAvgDist.Matches( strLine ) )
			{
				if( !rePropExtraGalAvgDist.GetMatch(strLine, 1 ).ToDouble( &m_nDistanceAverageExtraGalactic ) )
					m_nDistanceAverageExtraGalactic = 0;

			} else if( rePropMagToRadRatio.Matches( strLine ) )
			{
				if( !rePropMagToRadRatio.GetMatch(strLine, 1 ).ToDouble( &m_nMagnitudeToRadiusRatio ) )
					m_nMagnitudeToRadiusRatio = 0;

			} else if( rePropObsName.Matches( strLine ) )
			{
				m_strObsName = rePropObsName.GetMatch(strLine, 1 ).Trim();

			} else if( rePropObsCountry.Matches( strLine ) )
			{
				m_strObsCountry = rePropObsCountry.GetMatch(strLine, 1 ).Trim();

			} else if( rePropObsRegion.Matches( strLine ) )
			{
				m_strObsRegion = rePropObsRegion.GetMatch(strLine, 1 ).Trim();

			} else if( rePropObsCity.Matches( strLine ) )
			{
				m_strObsCity = rePropObsCity.GetMatch(strLine, 1 ).Trim();

			} else if( rePropObsLatitude.Matches( strLine ) )
			{
				// this is NORTH to SOUTH 90 to -90 
				wxString strPropObsLatitude = rePropObsLatitude.GetMatch(strLine, 1 ).Trim(0).Trim(1);
/*				char chL = strPropObsLatitude.Last();
				if( chL == 'N'  || chL == 'n' || chL == 'S' || chL == 's' )
				{
//					m_chObsLatitude = chL;
					strPropObsLatitude.RemoveLast();
				}
*/				if( !strPropObsLatitude.ToDouble( &m_nObsLatitude ) ) m_nObsLatitude = 0.0;
//				if( chL == 'S' || chL == 's' ) m_nObsLatitude *= -1;

			} else if( rePropObsLongitude.Matches( strLine ) )
			{
				// WEST - EAST ... -180 to 180
				wxString strPropObsLongitude = rePropObsLongitude.GetMatch(strLine, 1 ).Trim(0).Trim(1);
/*				char chL = strPropObsLongitude.Last();
				if( chL == 'E' || chL == 'e' || chL == 'W' || chL == 'w' )
				{
//					m_chObsLongitude = chL;
					strPropObsLongitude.RemoveLast();
				}
*/				if( !strPropObsLongitude.ToDouble( &m_nObsLongitude ) ) m_nObsLongitude = 0.0;
//				if( chL == 'W' || chL == 'w' ) m_nObsLongitude *= -1.0;

			} else if( rePropObsAltitude.Matches( strLine ) )
			{
				if( !rePropObsAltitude.GetMatch(strLine, 1 ).Trim().ToDouble( &m_nObsAltitude ) )
					m_nObsAltitude = 0;

			} else if( rePropObsDateTime.Matches( strLine ) )
			{
				//const wxChar* tret = 
				m_nObsDateTime.ParseDateTime( rePropObsDateTime.GetMatch(strLine, 1).Trim() );
				//wxString strTTmp = m_nObsDateTime.Format( wxT("%m/%d/%Y %H:%M:%S") );
				//double jdn = m_nObsDateTime.FromUTC().GetJDN();
				// calculate/set time zone
				int nTZShift = (int) round(m_nObsLongitude/15.0);
				m_nObsTZSecs = (long) nTZShift * 3600;
				// set time-zone to my current object
				//m_nObsDateTime.FromUTC().MakeTimezone( m_nObsTZSecs );

			} else if( rePropTargetName.Matches( strLine ) )
			{
				m_strTargetName = rePropTargetName.GetMatch(strLine, 1 ).Trim();

			} else if( rePropTargetDesc.Matches( strLine ) )
			{
				m_strTargetDesc = rePropTargetDesc.GetMatch(strLine, 1 ).Trim();

			} else if( rePropTargetRa.Matches( strLine ) )
			{
				if( !rePropTargetRa.GetMatch(strLine, 1 ).Trim().ToDouble( &m_nTargetRa ) )
					m_nTargetRa = 0;

			} else if( rePropTargetDec.Matches( strLine ) )
			{
				if( !rePropTargetDec.GetMatch(strLine, 1 ).Trim().ToDouble( &m_nTargetDec ) )
					m_nTargetDec = 0;

			} else if( rePropTelescopeName.Matches( strLine ) )
			{
				m_strTelescopeName = rePropTelescopeName.GetMatch(strLine, 1 ).Trim();

			} else if( rePropTelescopeType.Matches( strLine ) )
			{
				if( !rePropTelescopeType.GetMatch(strLine, 1 ).Trim().ToLong( &nVarLong ) )
					m_nTelescopeType = 0;
				else
					m_nTelescopeType = nVarLong;

			} else if( rePropTelescopeFocal.Matches( strLine ) )
			{
				if( !rePropTelescopeFocal.GetMatch(strLine, 1 ).Trim().ToDouble( &m_nTelescopeFocal ) )
					m_nTelescopeFocal = 0;

			} else if( rePropTelescopeAperture.Matches( strLine ) )
			{
				if( !rePropTelescopeAperture.GetMatch(strLine, 1 ).Trim().ToDouble( &m_nTelescopeAperture ) )
					m_nTelescopeAperture = 0;

			} else if( rePropTelescopeMount.Matches( strLine ) )
			{
				if( !rePropTelescopeMount.GetMatch(strLine, 1 ).Trim().ToLong( &nVarLong ) )
					m_nTelescopeMount = 0;
				else
					m_nTelescopeMount = nVarLong;

			} else if( rePropCameraName.Matches( strLine ) )
			{
				m_strCameraName = rePropCameraName.GetMatch(strLine, 1 ).Trim();

			} else if( rePropCondSeeing.Matches( strLine ) )
			{
				if( !rePropCondSeeing.GetMatch(strLine, 1 ).Trim().ToLong( &nVarLong ) )
					m_nCondSeeing = 0;
				else
					m_nCondSeeing = nVarLong;

			} else if( rePropCondTransp.Matches( strLine ) )
			{
				if( !rePropCondTransp.GetMatch(strLine, 1 ).Trim().ToLong( &nVarLong ) )
					m_nCondTransp = 0;
				else
					m_nCondTransp = nVarLong;

			} else if( rePropCondWindSpeed.Matches( strLine ) )
			{
				if( !rePropCondWindSpeed.GetMatch(strLine, 1 ).Trim().ToDouble( &m_nCondWindSpeed ) )
					m_nCondWindSpeed = 0;

			} else if( rePropCondAtmPressure.Matches( strLine ) )
			{
				if( !rePropCondAtmPressure.GetMatch(strLine, 1 ).Trim().ToDouble( &m_nCondAtmPressure ) )
					m_nCondAtmPressure = 0;

			} else if( rePropCondTemp.Matches( strLine ) )
			{
				if( !rePropCondTemp.GetMatch(strLine, 1 ).Trim().ToDouble( &m_nCondTemp ) )
					m_nCondTemp = 0;

			} else if( rePropCondTempVar.Matches( strLine ) )
			{
				if( !rePropCondTempVar.GetMatch(strLine, 1 ).Trim().ToDouble( &m_nCondTempVar ) )
					m_nCondTempVar = 0;

			} else if( rePropCondHumid.Matches( strLine ) )
			{
				if( !rePropCondHumid.GetMatch(strLine, 1 ).Trim().ToDouble( &m_nCondHumid ) )
					m_nCondHumid = 0;

			} else if( rePropCondLp.Matches( strLine ) )
			{
				if( !rePropCondLp.GetMatch(strLine, 1 ).Trim().ToLong( &nVarLong ) )
					m_nCondLp = 0;
				else
					m_nCondLp = nVarLong;

			} else if( rePropExpTotalTime.Matches( strLine ) )
			{
				wxString strPropExpTotalTime = rePropExpTotalTime.GetMatch(strLine, 1 ).Trim(0).Trim(1);
				if( strPropExpTotalTime.ToDouble( &nVarDouble ) )
					m_nExpTotalTime = nVarDouble;

			} else if( rePropNoOfExp.Matches( strLine ) )
			{
				if( !rePropNoOfExp.GetMatch(strLine, 1 ).Trim().ToLong( &nVarLong ) )
					m_nNoOfExp = 0;
				else
					m_nNoOfExp = nVarLong;

			} else if( rePropExpIso.Matches( strLine ) )
			{
				wxString strPropExpIso = rePropExpIso.GetMatch(strLine, 1 ).Trim(0).Trim(1);
				if( strPropExpIso.ToLong( &nVarLong ) )
					m_nExpIso = nVarLong;

			} else if( rePropRegProg.Matches( strLine ) )
			{
				m_strRegProg = rePropRegProg.GetMatch(strLine, 1 ).Trim();

			} else if( rePropStackProg.Matches( strLine ) )
			{
				m_strStackProg = rePropStackProg.GetMatch(strLine, 1 ).Trim();

			} else if( rePropStackMethod.Matches( strLine ) )
			{
				if( !rePropStackMethod.GetMatch(strLine, 1 ).Trim().ToLong( &nVarLong ) )
					m_nStackMethod = 0;
				else
					m_nStackMethod = nVarLong;

			} else if( rePropProcProg.Matches( strLine ) )
			{
				wxString strProcProg = rePropProcProg.GetMatch(strLine, 1 ).Trim(0).Trim(1);
				if( !strProcProg.IsEmpty() ) m_strProcProg = strProcProg;

			}
			// now calculate also star/end ra dec
			m_nRaStart = m_nOrigRa-m_nFieldWidth/2;
			m_nRaEnd = m_nOrigRa+m_nFieldWidth/2;
			m_nDecStart = m_nOrigDec-m_nFieldHeight/2;
			m_nDecEnd = m_nOrigDec+m_nFieldHeight/2;

		////////////////////////////////////////////////////
		// SECTION :: IMAGE HINT
		} else if( nFoundSection == ASTROIMG_PROF_SECTION_ID_HINT && bNewLine )
		{
			// here we check by the individual hint labels
			if( reHintType.Matches( strLine ) )
			{
				if( !reHintType.GetMatch(strLine, 1 ).ToLong( &nVarLong ) )
					m_nUseHint = 0;
				else
					m_nUseHint = (int) nVarLong;

			} else if( reHintOrigRa.Matches( strLine ) )
			{
				if( !reHintOrigRa.GetMatch(strLine, 1 ).ToDouble( &m_nRelCenterRa ) )
					m_nRelCenterRa = 0;

			} else if( reHintOrigDec.Matches( strLine ) )
			{
				if( !reHintOrigDec.GetMatch(strLine, 1 ).ToDouble( &m_nRelCenterDec ) )
					m_nRelCenterDec = 0;

			} else if( reHintFieldWidth.Matches( strLine ) )
			{
				if( !reHintFieldWidth.GetMatch(strLine, 1 ).ToDouble( &m_nRelFieldRa ) )
					m_nRelFieldRa = 0;

			} else if( reHintFieldHeight.Matches( strLine ) )
			{
				if( !reHintFieldHeight.GetMatch(strLine, 1 ).ToDouble( &m_nRelFieldDec ) )
					m_nRelFieldDec = 0;

			} else if( reHintOrigType.Matches( strLine ) )
			{
				if( !reHintOrigType.GetMatch(strLine, 1 ).ToLong( &nVarLong ) )
					m_nHintOrigType = 0;
				else
					m_nHintOrigType = (int) nVarLong;

			} else if( reHintObjOrigType.Matches( strLine ) )
			{
				if( !reHintObjOrigType.GetMatch(strLine, 1 ).ToLong( &nVarLong ) )
					m_nHintObjOrigType = 0;
				else
					m_nHintObjOrigType = (int) nVarLong;

			} else if( reHintObjName.Matches( strLine ) )
			{
				if( !reHintObjName.GetMatch(strLine, 1 ).ToLong( &nVarLong ) )
					m_nHintObjName = 0;
				else
					m_nHintObjName = (int) nVarLong;

			} else if( reHintObjId.Matches( strLine ) )
			{
				m_strHintObjId = reHintObjId.GetMatch(strLine, 1 );

			} else if( reHintOrigUnits.Matches( strLine ) )
			{
				if( !reHintOrigUnits.GetMatch(strLine, 1 ).ToLong( &nVarLong ) )
					m_nHintOrigUnits = 0;
				else
					m_nHintOrigUnits = (int) nVarLong;

			} else if( reHintFieldType.Matches( strLine ) )
			{
				if( !reHintFieldType.GetMatch(strLine, 1 ).ToLong( &nVarLong ) )
					m_nHintFieldType = 0;
				else
					m_nHintFieldType = (int) nVarLong;

			// :: HARDWARE SETUP
			}  else if( reHintHardwareSetup.Matches( strLine ) )
			{
				if( !reHintHardwareSetup.GetMatch(strLine, 1 ).ToLong( &nVarLong ) )
					m_nHintSetupId = 0;
				else
					m_nHintSetupId = (int) nVarLong;

			///////////
			// HINT :: CAMERA
			} else if( reHintCamera.Matches( strLine ) )
			{
				int nCamSource=0, nCamType=0, nCamBrand=0, nCamName=0;
				// pharse string with scanf
				if( wxSscanf( reHintCamera.GetMatch(strLine, 1 ), wxT("%d,(%d:%d:%d)\n"),
					&nCamSource, &nCamType, &nCamBrand, &nCamName ) )
				{
					m_nHintCameraSource = nCamSource;
					m_nHintCameraType = nCamType;
					m_nHintCameraBrand = nCamBrand;
					m_nHintCameraName = nCamName;
				}

			// :: SENSOR SIZE
			} else if( reHintSensorWidth.Matches( strLine ) )
			{
				if( !reHintSensorWidth.GetMatch(strLine, 1 ).ToDouble( &m_nHintSensorWidth ) )
					m_nHintSensorWidth = 0;

			} else if( reHintSensorHeight.Matches( strLine ) )
			{
				if( !reHintSensorHeight.GetMatch(strLine, 1 ).ToDouble( &m_nHintSensorHeight ) )
					m_nHintSensorHeight = 0;

			///////////
			// HINT :: TELESCOPE/LENS
			} else if( reHintTLens.Matches( strLine ) )
			{
				int nTLensSource=0, nLensType=0, nLensOptics=0, nLensBrand=0, nLensName=0,
					bBarlow=0, nBarlowLensBrand=0, nBarlowLensName=0, 
					bFocalReducer=0, nFocalReducerBrand=0, nFocalReducerName=0,
					bEyepiece=0, nEyepieceType=0, nEyepieceBrand=0, nEyepieceName=0,
					bCameraLens=0, nCameraLensOptics=0, nCameraLensBrand=0, nCameraLensName=0;
				// pharse string with scanf
				if( wxSscanf( reHintTLens.GetMatch(strLine, 1 ), 
					wxT("%d,TL(%d:%d:%d:%d),BL(%d:%d:%d),FR(%d:%d:%d),EP(%d:%d:%d:%d),CL(%d:%d:%d:%d)\n"),
					&nTLensSource, &nLensType, &nLensOptics, &nLensBrand, &nLensName,
					&bBarlow, &nBarlowLensBrand, &nBarlowLensName, 
					&bFocalReducer, &nFocalReducerBrand, &nFocalReducerName,
					&bEyepiece, &nEyepieceType, &nEyepieceBrand, &nEyepieceName,
					&bCameraLens, &nCameraLensOptics, &nCameraLensBrand, &nCameraLensName ) )
				{
					m_nHintTLensSource = nTLensSource;
					m_rHintTLensSetup.nLensType = nLensType;
					m_rHintTLensSetup.nLensOptics = nLensOptics;
					m_rHintTLensSetup.nLensBrand = nLensBrand;
					m_rHintTLensSetup.nLensName = nLensName;
					// barlow
					m_rHintTLensSetup.bBarlow = bBarlow;
					m_rHintTLensSetup.nBarlowLensBrand = nBarlowLensBrand;
					m_rHintTLensSetup.nBarlowLensName = nBarlowLensName;
					// focal reducer
					m_rHintTLensSetup.bFocalReducer = bFocalReducer;
					m_rHintTLensSetup.nFocalReducerBrand = nFocalReducerBrand;
					m_rHintTLensSetup.nFocalReducerName = nFocalReducerName;
					// eyepiece
					m_rHintTLensSetup.bEyepiece = bEyepiece;
					m_rHintTLensSetup.nEyepieceType = nEyepieceType;
					m_rHintTLensSetup.nEyepieceBrand = nEyepieceBrand;
					m_rHintTLensSetup.nEyepieceName = nEyepieceName;
					// camera lens
					m_rHintTLensSetup.bCameraLens = bCameraLens;
					m_rHintTLensSetup.nCameraLensOptics = nCameraLensOptics;
					m_rHintTLensSetup.nCameraLensBrand = nCameraLensBrand;
					m_rHintTLensSetup.nCameraLensName = nCameraLensName;
				}

			// :: SYSTEM FOCAL LENGTH
			} else if( reHintFocalLength.Matches( strLine ) )
			{
				if( !reHintFocalLength.GetMatch(strLine, 1 ).ToDouble( &m_nHintFocalLength ) )
					m_nHintFocalLength = 0;

			}  else if( reHintFieldFormat.Matches( strLine ) )
			{
				if( !reHintFieldFormat.GetMatch(strLine, 1 ).ToLong( &nVarLong ) )
					m_nHintFieldFormat = 0;
				else
					m_nHintFieldFormat = (int) nVarLong;
			}

		/////////////////////////////////////////////////////////////
		//  SECTION :: DETECTION
		} else if( nFoundSection == ASTROIMG_PROF_SECTION_ID_DETECTION && bNewLine )
		{
			// here we check by the individual detection labels
			if( reDetectImageType.Matches( strLine ) )
			{
			} else if( reDetectUseChannel.Matches( strLine ) )
			{
			}

		//////////////////////////////////////////////////////////////
		// SECTION :: MATCHING
		} else if( nFoundSection == ASTROIMG_PROF_SECTION_ID_MATCHING )
		{
			// here we check by the individual matching labels
			if( reMatchBestTrans.Matches( strLine ) )
			{
				GetTransFromString( reMatchBestTrans.GetMatch(strLine, 1 ).ToAscii(), m_pBestTrans );

			} else if( reMatchInvTrans.Matches( strLine ) )
			{
				GetTransFromString( reMatchInvTrans.GetMatch(strLine, 1 ).ToAscii(), m_pInvTrans );

			} else if( reMatchSkyPrjScale.Matches( strLine ) )
			{
				if( !reMatchSkyPrjScale.GetMatch(strLine, 1 ).ToDouble( &m_nSkyProjectionScale ) )
					m_nSkyProjectionScale = 0;

			} else if( wxStrncmp( strLine, wxT("MatchCatalog="), 13 ) == 0 )
			{
				if( wxSscanf( strLine, wxT("MatchCatalog=%[^\n]\n"), strTmp ) )
				{
					wxString strCatAbbreviation = strTmp;
					strCatAbbreviation.Trim(0).Trim(1);
					m_nCatalogForMatching = m_pSky->GetCatalogIdByAbbreviation( strCatAbbreviation );
				}
			}

		//////////////////////////////////////////////////////////////
		// SECTION :: AREA
		} else if( nFoundSection == ASTROIMG_PROF_SECTION_ID_AREAS && bNewLine )
		{
			int nAreaX=0, nAreaY=0, nAreaW=0, nAreaH=0, nAreaId=0;
			if( wxSscanf( strLine, wxT("%d=%d,%d,%d,%d\n"), &nAreaId, &nAreaX, &nAreaY, &nAreaW, &nAreaH ) )
			{
					AddArea( nAreaX, nAreaY, nAreaW, nAreaH, nAreaId );
			}

		//////////////////////////////////////////////////////////////
		// SECTION :: DETECTED STARS
		}  else if( nFoundSection == ASTROIMG_PROF_SECTION_ID_DETECTED_STARS && bNewLine )
		{
			// here we check by the individual label
			//if( reDetectedStar.Matches( strLine ) )
			if( wxStrncmp( strLine, wxT("DStar="), 6 ) == 0 )
			{
				memset( &str_catname, 0, 100 );
				// pharse string with scanf
				//if( wxSscanf( reDetectedStar.GetMatch(strLine, 1 ).GetData(),
				wxString strWxLine = strLine;
				if( sscanf( strWxLine.ToAscii(),
					"DStar=%u,%u,%u,%lu,%u,%[^,],%u,%u,%lf,%lf,%d,%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",
					&nCount, &nCatType, &nZoneNo,	// 1,2,3
					&nCatNo, &nCompNo,				// 4,5
					str_catname,						// 6
					&nType, &nMatch,				// 6,7
					&nX, &nY,						// 8,9
					&nCx, &nCy,						// 8,9
					&nRa, &nDec,					// 10,11
					&nMag, &nVmag,					// 12,13
					&nA, &nB,						// 14,15
					&nKronfactor, &nTheta, &nFwhm, &nFlux, &nDist ) )		// 16,17,18
				{
					nStar = AddStar( nX, nY, nCx, nCy, nMag, nA, nB, nTheta, nKronfactor, nFwhm, nFlux, (unsigned char) nType );
					if(  nStar >= 0 )
					{
						if( strcmp( str_catname, " " ) != 0 )
							SetStarStringCatalogName( m_vectStar[nStar], str_catname );

						m_vectStar[nStar].cat_type = (unsigned char) nCatType;
						m_vectStar[nStar].zone_no = (unsigned int) nZoneNo;
						m_vectStar[nStar].cat_no = nCatNo;
						m_vectStar[nStar].comp_no = (unsigned char) nCompNo;
						m_vectStar[nStar].type = (unsigned char) nType;
						m_vectStar[nStar].match = (unsigned char) nMatch;
						m_vectStar[nStar].ra = nRa;
						m_vectStar[nStar].dec = nDec;
						m_vectStar[nStar].vmag = nVmag;
						m_vectStar[nStar].fwhm = nFwhm;
						m_vectStar[nStar].flux = nFlux;
						m_vectStar[nStar].distance = nDist;

						// now increment the match counter if matched
						if( m_vectStar[nStar].match == 1 ) m_nMatchedStars++;
					}
				}
			}

		//////////////////////////////////////////////////////////////
		// SECTION :: CATALOG STARS
		} else if( nFoundSection == ASTROIMG_PROF_SECTION_ID_CAT_STARS && bNewLine )
		{
			memset( &str_catname, 0, 100 );
			// now we load here catalog stars
			//if( reCatalogStar.Matches( strLine ) )
			if( wxStrncmp( strLine, wxT("CStar="), 6 ) == 0 )
			{
				// pharse string with scanf
				//if( wxSscanf( reCatalogStar.GetMatch(strLine, 1 ).GetData(),
				wxString strWxLine = strLine;
				if( sscanf( strWxLine.ToAscii(),
					"CStar=%u,%u,%u,%lu,%u,%[^,],%u,%u,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",
					&nCount, &nCatType, &nZoneNo,	// 1,2,3
					&nCatNo, &nCompNo,				// 4,5
					str_catname,						// 6
					&nType, &nMatch,				// 6,7
					&nX, &nY,						// 8,9
					&nRa, &nDec,					// 10,11
					&nMag, &nVmag,					// 12,13
					&nA, &nB,						// 14,15
					&nKronfactor, &nTheta, &nFwhm, &nDist ) )		// 16,17,18
				{
					StarDef star;
					memset( &star, 0, sizeof(StarDef) );

					star.ra = nRa;
					star.dec = nDec;
					star.x = nX;
					star.y = nY;
					star.mag = nMag;
					star.a = nA;
					star.b = nB;
					star.kronfactor = nKronfactor;
					star.cat_no = nCatNo;
					star.theta = nTheta;

					nStar = AddCatStar( star, (unsigned char) nType );
					if(  nStar >= 0 )
					{
						if( strcmp( str_catname, " " ) != 0 )
							SetStarStringCatalogName( m_vectCatStar[nStar], str_catname );

						m_vectCatStar[nStar].cat_type = (unsigned char) nCatType;
						m_vectCatStar[nStar].zone_no = (unsigned int) nZoneNo;
						m_vectCatStar[nStar].cat_no = nCatNo;
						m_vectCatStar[nStar].comp_no = (unsigned char) nCompNo;
						m_vectCatStar[nStar].type = (unsigned char) nType;
						m_vectCatStar[nStar].match = (unsigned char) nMatch;
						m_vectCatStar[nStar].ra = nRa;
						m_vectCatStar[nStar].dec = nDec;
						m_vectCatStar[nStar].vmag = nVmag;
						m_vectCatStar[nStar].fwhm = nFwhm;
						m_vectCatStar[nStar].kronfactor = nKronfactor;
						m_vectCatStar[nStar].distance = nDist;
					}
				}
			}

		//////////////////////////////////////////////////////////////
		// SECTION :: CATALOG DSO
		} else if( nFoundSection == ASTROIMG_PROF_SECTION_ID_CAT_DSO && bNewLine )
		{
			// and here we look to load dso objects
			//if( reCatalogDso.Matches( strLine ) )
			if( wxStrncmp( strLine, wxT("CDso="), 5 ) == 0 )
			{
				// pharse string with scanf
				//if( wxSscanf( reCatalogDso.GetMatch(strLine, 1 ).GetData(),
				if( wxSscanf( strLine,
					wxT("CDso=%u,%u,%u,%lu,%u,%u,%u,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n"),
					&nCount, &nCatType, &nZoneNo,	// 1,2,3
					&nCatNo, &nCompNo,				// 4,5
					&nType, &nMatch,				// 6,7
					&nX, &nY,						// 8,9
					&nRa, &nDec,					// 10,11
					&nMag, &nVmag,					// 12,13
					&nA, &nB,						// 14,15
					&nKronfactor, &nTheta, &nDist ) )		// 16,17
				{
					StarDef star;
					memset( &star, 0, sizeof(StarDef) );

					star.x = nX;
					star.y = nY;
					star.cat_no = nCatNo;
					star.type = (unsigned char) nType;
					nCatDso = AddCatDso( star, (unsigned char) nType );

					if(  nCatDso >= 0 )
					{
						m_vectCatDso[nCatDso].cat_type = (unsigned char) nCatType;
						m_vectCatDso[nCatDso].zone_no = (unsigned int) nZoneNo;
						m_vectCatDso[nCatDso].a = nA;
						m_vectCatDso[nCatDso].b = nB;
						m_vectCatDso[nCatDso].comp_no = (unsigned char) nCompNo;
						m_vectCatDso[nCatDso].mag =  nMag;
						m_vectCatDso[nCatDso].match = (unsigned char) nMatch;
						m_vectCatDso[nCatDso].ra = nRa;
						m_vectCatDso[nCatDso].dec = nDec;
						m_vectCatDso[nCatDso].vmag = nVmag;
						m_vectCatDso[nCatDso].kronfactor = nKronfactor;
						m_vectCatDso[nCatDso].theta = nTheta;
						m_vectCatDso[nCatDso].distance = nDist;
					}
				}

			}

		//////////////////////////////////////////////////////////////
		// SECTION :: CATALOG RADIO
		} else if( nFoundSection == ASTROIMG_PROF_SECTION_ID_CAT_RADIO && bNewLine )
		{
			// and here we look to load radio sources
			if( reCatalogRadio.Matches( strLine ) )
			{
				double nFreq=0.0, nDFlux=0.0, nIDFlux=0.0, nSpecSlope=0.0, nSpecAbscissa=0.0;
				unsigned int nNSPoints=0;
				long nCatRadio = -1;

				// pharse string with scanf
				if( sscanf( reCatalogRadio.GetMatch(strLine, 1 ).ToAscii(),
					"%u,%u,%[^,],%lu,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%u,%lf,%lf,%u\n",
					&nCount, &nCatType, str_catname,		// 1,2,3
					&nCatNo, &nRa,						// 4,5
					&nDec, &nX,							// 6,7
					&nY, &nFreq,						// 8,9
					&nDFlux, &nIDFlux,					// 10,11
					&nNSPoints, &nSpecSlope,			// 12,13
					&nSpecAbscissa, &nType ) )			// 14,15
				{
					DefCatBasicRadio radioSource;
					memset( &radioSource, 0, sizeof(DefCatBasicRadio) );

					radioSource.x = nX;
					radioSource.y = nY;
					radioSource.cat_no = nCatNo;
					radioSource.type = (unsigned char) nType;
					nCatRadio = AddRadioSource( radioSource );

					if( nCatRadio >= 0 )
					{
						if( strcmp( str_catname, " " ) != 0 )
							strcpy( m_vectCatRadio[nCatRadio].cat_name, str_catname );

						m_vectCatRadio[nCatRadio].cat_type = (unsigned char) nCatType;
						m_vectCatRadio[nCatRadio].no_spec_points = (unsigned int) nNSPoints;
						m_vectCatRadio[nCatRadio].frequency = nFreq;
						m_vectCatRadio[nCatRadio].flux_density = nDFlux;
						m_vectCatRadio[nCatRadio].i_flux_density = nIDFlux;
						m_vectCatRadio[nCatRadio].spec_slope =  nSpecSlope;
						m_vectCatRadio[nCatRadio].spec_abscissa = nSpecAbscissa;
						m_vectCatRadio[nCatRadio].ra = nRa;
						m_vectCatRadio[nCatRadio].dec = nDec;
					}
				}
			}

		//////////////////////////////////////////////////////////////
		// SECTION :: CATALOG XRAY-GAMMA
		} else if( nFoundSection == ASTROIMG_PROF_SECTION_ID_CAT_XGAMMA && bNewLine )
		{
			// and here we look to load x-ray/gamma sources
			if( reCatalogXGamma.Matches( strLine ) )
			{
				double nCount=0.0, nBgCount=0.0, nExpTime=0.0, nFlux=0.0,
					nFluxBand1=0.0, nFluxBand2=0.0, nFluxBand3=0.0, nFluxBand4=0.0,
					nFluxBand5=0.0, nFluxBand6=0.0, nFluence=0.0, nPeakFlux=0.0,
					nFluxCount1=0, nFluxCount2=0, nFluxCount3=0, nFluxCount4=0, nGamma=0.0;
				unsigned int nNoDetect=0, nIntervalNo;
				unsigned long nStartTime=0, nDuration, nBurstDateTime, nTimeDet;
				unsigned int nSourceType=0;
				long nCatXGamma = -1;

				// pharse string with scanf
				if( sscanf( reCatalogXGamma.GetMatch(strLine, 1 ).ToAscii(),
					//1  2   3    4   5   6   7   8   9   10  11  12  13  14  15
					"%u,%u,%[^,],%lu,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%le,%lu,%lu,%lu,%le,%le,%le,%le,%le,%le,%lf,%lf,%lf,%lf,%u,%lu,%lu,%lf,%lf,%lf,%lu,%u\n",
					&nCount, &nCatType, str_catname,		// 1,2,3
					&nCatNo, &nRa,						// 4,5
					&nDec, &nX,							// 6,7
					&nY, &nCount,						// 8,9
					&nBgCount, &nExpTime,				// 10,11
					&nFlux, &nStartTime,				// 12,13
					&nDuration, &nBurstDateTime,		// 14,15
					&nFluxBand1, &nFluxBand2,			// 16,17
					&nFluxBand3, &nFluxBand4,			// 18,19
					&nFluxBand5, &nFluxBand6,			// 20,21
					&nFluxCount1, &nFluxCount2,			// 22,23
					&nFluxCount3, &nFluxCount4,			// 24,25
					&nSourceType, &nTimeDet,			// 26,27
					&nIntervalNo, &nFluence,			// 28,29
					&nPeakFlux, &nGamma,				// 30,31
					&nNoDetect, &nType ) )				// 32,33
				{
					DefCatBasicXGamma xgammaSource;
					memset( &xgammaSource, 0, sizeof(DefCatBasicXGamma) );

					xgammaSource.x = nX;
					xgammaSource.y = nY;
					xgammaSource.cat_no = nCatNo;
					xgammaSource.type = (unsigned char) nType;
					nCatXGamma = AddXGammaSource( xgammaSource );

					if( nCatXGamma >= 0 )
					{
						if( strcmp( str_catname, " " ) != 0 )
							strcpy( m_vectCatXGamma[nCatXGamma].cat_name, str_catname );

						m_vectCatXGamma[nCatXGamma].ra = nRa;
						m_vectCatXGamma[nCatXGamma].dec = nDec;
						m_vectCatXGamma[nCatXGamma].cat_type = (unsigned char) nCatType;

						m_vectCatXGamma[nCatXGamma].count = nCount;
						m_vectCatXGamma[nCatXGamma].bg_count = nBgCount;
						m_vectCatXGamma[nCatXGamma].exp_time = nExpTime;
						m_vectCatXGamma[nCatXGamma].flux =  nFlux;
						m_vectCatXGamma[nCatXGamma].start_time = nStartTime;
						m_vectCatXGamma[nCatXGamma].burst_date_time = nBurstDateTime;
						m_vectCatXGamma[nCatXGamma].flux_band_1 = nFluxBand1;
						m_vectCatXGamma[nCatXGamma].flux_band_2 = nFluxBand2;
						m_vectCatXGamma[nCatXGamma].flux_band_3 = nFluxBand3;
						m_vectCatXGamma[nCatXGamma].flux_band_4 = nFluxBand4;
						m_vectCatXGamma[nCatXGamma].flux_band_5 = nFluxBand5;
						m_vectCatXGamma[nCatXGamma].flux_band_6 = nFluxBand6;
						m_vectCatXGamma[nCatXGamma].count_band_1 = nFluxCount1;
						m_vectCatXGamma[nCatXGamma].count_band_2 = nFluxCount2;
						m_vectCatXGamma[nCatXGamma].count_band_3 = nFluxCount3;
						m_vectCatXGamma[nCatXGamma].count_band_4 = nFluxCount4;
						m_vectCatXGamma[nCatXGamma].source_type = (unsigned char) nSourceType;
						m_vectCatXGamma[nCatXGamma].time_det = nTimeDet;
						m_vectCatXGamma[nCatXGamma].interval_no = nIntervalNo;
						m_vectCatXGamma[nCatXGamma].fluence = nFluence;
						m_vectCatXGamma[nCatXGamma].peak_flux = nPeakFlux;
						m_vectCatXGamma[nCatXGamma].gamma = nGamma;
						m_vectCatXGamma[nCatXGamma].no_detections = nNoDetect;

					}
				}
			}

		//////////////////////////////////////////////////////////////
		// SECTION :: CATALOG SUPERNOVAS
		} else if( nFoundSection == ASTROIMG_PROF_SECTION_ID_CAT_SUPERNOVA && bNewLine )
		{
			// and here we look to load supernovas
			if( wxStrncmp( strLine, wxT("CSupernova="), 11 ) == 0 )
			{
				double nMag=0.0, nMaxMag=0.0, nGalCenterOffsetX=0.0, nGalCenterOffsetY=0.0,
					nDateMax=0.0, nDateDiscovery=0.0;
				//unsigned char nUnconf=0, nSNType=0;
				int nDiscMethod=0, nUnconf=0;
				long nCatSupernova = -1;
				wxChar strSNType[255];
				wxChar strParentGalaxy[255];
				wxChar strDiscoverer[255];
				wxChar strProgDiscCode[255];
				//continue;

				// pharse string with scanf
				if( wxSscanf( strLine,
					//          1   2   3    4   5   6   7   8  9   10  11   12   13  14   15   16  17   18  19   20  21
					wxT("CSupernova=%u,%u,%[^,],%lu,%lf,%lf,%lf,%lf,%d,%lf,%lf,%[^,],%lf,%lf,%[^,],%lf,%lf,%[^,],%d,%[^,],%u\n"),
					&nCount, &nCatType, strCatName,				// 1,2,3
					&nCatNo, &nRa,								// 4,5
					&nDec, &nX,									// 6,7
					&nY, &nUnconf,								// 8,9
					&nMag, &nMaxMag,							// 10,11
					strSNType, &nGalCenterOffsetX,				// 12,13
					&nGalCenterOffsetY, strParentGalaxy,		// 14,15
					&nDateMax, &nDateDiscovery,					// 16,17
					strDiscoverer, &nDiscMethod,				// 18,19
					strProgDiscCode, &nType ) )					// 20,21
				{
					DefCatBasicSupernova srcSupernova;
					memset( &srcSupernova, 0, sizeof(DefCatBasicSupernova) );

					srcSupernova.x = nX;
					srcSupernova.y = nY;
					srcSupernova.cat_no = nCatNo;
					srcSupernova.type = (unsigned char) nType;
					nCatSupernova = AddSupernova( srcSupernova );

					// if valid supernova add
					if( nCatSupernova >= 0 )
					{
						if( wxStrcmp( strCatName, wxT(" ") ) != 0 ) wxStrcpy( m_vectCatSupernova[nCatSupernova].cat_name, strCatName );
						if( wxStrcmp( strSNType, wxT(" ") ) != 0 ) wxStrcpy( m_vectCatSupernova[nCatSupernova].sn_type, strSNType );
						if( wxStrcmp( strParentGalaxy, wxT(" ") ) != 0 ) wxStrcpy( m_vectCatSupernova[nCatSupernova].parent_galaxy, strParentGalaxy );
						if( wxStrcmp( strDiscoverer, wxT(" ") ) != 0 ) wxStrcpy( m_vectCatSupernova[nCatSupernova].discoverer, strDiscoverer );
						if( wxStrcmp( strProgDiscCode, wxT(" ") ) != 0 ) wxStrcpy( m_vectCatSupernova[nCatSupernova].prog_code, strProgDiscCode );

						m_vectCatSupernova[nCatSupernova].ra = nRa;
						m_vectCatSupernova[nCatSupernova].dec = nDec;
						m_vectCatSupernova[nCatSupernova].cat_type = (unsigned char) nCatType;

						m_vectCatSupernova[nCatSupernova].unconfirmed = (unsigned char) nUnconf;
						m_vectCatSupernova[nCatSupernova].mag = nMag;
						m_vectCatSupernova[nCatSupernova].max_mag = nMaxMag;
						m_vectCatSupernova[nCatSupernova].gal_center_offset_x = nGalCenterOffsetX;
						m_vectCatSupernova[nCatSupernova].gal_center_offset_y = nGalCenterOffsetY;
						m_vectCatSupernova[nCatSupernova].date_max = nDateMax;
						m_vectCatSupernova[nCatSupernova].date_discovery = nDateDiscovery;
						m_vectCatSupernova[nCatSupernova].disc_method = (unsigned char) nDiscMethod;
					}
				}
			}

		//////////////////////////////////////////////////////////////
		// SECTION :: CATALOG BLACKHOLES
		} else if( nFoundSection == ASTROIMG_PROF_SECTION_ID_CAT_BLACKHOLE && bNewLine )
		{
			// and here we look to load blackoles
			if( reCatalogBlackhole.Matches( strLine ) )
			{
				double nMass=0.0, nMassHBeta=0.0, nMassMGII=0.0, nMasCiv=0.0,
					nLuminosity=0.0, nXrayLuminosity=0.0, nRadioLuminosity=0.0, nRedshift=0.0;
				int nTargetType=0;
				long nCatSrc = -1;

				// pharse string with scanf
				if( wxSscanf( reCatalogBlackhole.GetMatch(strLine, 1 ),
					wxT("%u,%u,%[^,],%lu,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%u,%u\n"),
					&nCount, &nCatType, strCatName,				// 1,2,3
					&nCatNo, &nRa,								// 4,5
					&nDec, &nX,									// 6,7
					&nY, &nMass,								// 8,9
					&nMassHBeta, &nMassMGII,					// 10,11
					&nMasCiv, &nLuminosity,						// 12,13
					&nRadioLuminosity, &nXrayLuminosity,		// 14,15
					&nRedshift, &nTargetType,					// 16,17
					 &nType ) )									// 18
				{
					DefCatBlackhole src;
					memset( &src, 0, sizeof(DefCatBlackhole) );

					src.x = nX;
					src.y = nY;
					src.cat_no = nCatNo;
					src.type = (unsigned char) nType;
					nCatSrc = AddBlackhole( src );

					if( nCatSrc >= 0 )
					{
						if( wxStrcmp( strCatName, wxT(" ") ) != 0 ) wxStrcpy( m_vectCatBlackhole[nCatSrc].cat_name, strCatName );

						m_vectCatBlackhole[nCatSrc].ra = nRa;
						m_vectCatBlackhole[nCatSrc].dec = nDec;
						m_vectCatBlackhole[nCatSrc].mass = nMass;
						m_vectCatBlackhole[nCatSrc].mass_hbeta = nMassHBeta;
						m_vectCatBlackhole[nCatSrc].mass_mgii = nMassMGII;
						m_vectCatBlackhole[nCatSrc].mass_civ = nMasCiv;
						m_vectCatBlackhole[nCatSrc].luminosity =  nLuminosity;
						m_vectCatBlackhole[nCatSrc].radio_luminosity = nRadioLuminosity;
						m_vectCatBlackhole[nCatSrc].xray_luminosity = nXrayLuminosity;
						m_vectCatBlackhole[nCatSrc].redshift = nRedshift;
						m_vectCatBlackhole[nCatSrc].target_type = (unsigned char) nTargetType;
						m_vectCatBlackhole[nCatSrc].cat_type = (unsigned char) nCatType;
					}
				}

			}

		//////////////////////////////////////////////////////////////
		// SECTION :: CATALOG MULTIPLE/DOUBLE STARS
		} else if( nFoundSection == ASTROIMG_PROF_SECTION_ID_CAT_MSTARS && bNewLine )
		{
			// and here we look to load multiple/double stars
			if( reCatalogMStars.Matches( strLine ) )
			{
				char strCatComp[255];
				char strCatSpecType[255];
				char strCatPMNote[255];
				char strCatDisc[255];
				char strCatNotes[255];

				double nMag=0.0, nMag2=0.0, nPA=0.0, nPA2=0.0, nPmRA=0.0,
						nPmDEC=0.0, nPmRA2=0.0, nPmDEC2=0.0, nSep=0.0, nSep2=0.0,
						nObsDate=0.0, nObsDate2=0.0;
				unsigned int nNObs=0;
				long nCatSrc = -1;

				// pharse string with scanf
				if( sscanf( reCatalogMStars.GetMatch(strLine, 1 ).ToAscii(),
					"%u,%u,%[^,],%lu,%lf,%lf,%lf,%lf,%lf,%lf,%[^,],%[^,],%lf,%lf,%lf,%lf,%lf,%lf,%[^,],%lf,%lf,%d,%lf,%lf,%[^,],%[^,],%u\n",
					&nCount, &nCatType, str_catname,	// 1,2,3
					&nCatNo, &nRa,					// 4,5
					&nDec, &nX,						// 6,7
					&nY, &nMag,						// 8,9
					&nMag2, strCatComp,				// 10,11
					strCatSpecType, &nPA,			// 12,13
					&nPA2, &nPmRA,					// 14,15
					&nPmDEC, &nPmRA2,				// 16,17
					&nPmDEC2, strCatPMNote,			// 16,17
					&nSep, &nSep2,					// 16,17
					&nNObs, &nObsDate,				// 16,17
					&nObsDate2, strCatDisc,			// 16,17
					strCatNotes, &nType ) )			// 16,17
				{
					DefCatMStars src;
					memset( &src, 0, sizeof(DefCatMStars) );

					src.x = nX;
					src.y = nY;
					src.cat_no = nCatNo;
					src.type = (unsigned char) nType;
					nCatSrc = AddMStars( src );

					if( nCatSrc >= 0 )
					{
						if( strcmp( str_catname, " " ) != 0 ) strcpy( m_vectCatMStars[nCatSrc].cat_name, str_catname );
						if( strcmp( strCatComp, " " ) != 0 ) strcpy( m_vectCatMStars[nCatSrc].comp, strCatComp );
						if( strcmp( strCatSpecType, " " ) != 0 ) strcpy( m_vectCatMStars[nCatSrc].spectral_type, strCatSpecType );
						if( strcmp( strCatPMNote, " " ) != 0 ) strcpy( m_vectCatMStars[nCatSrc].pm_note, strCatPMNote );
						if( strcmp( strCatDisc, " " ) != 0 ) strcpy( m_vectCatMStars[nCatSrc].discoverer, strCatDisc );
						if( strcmp( strCatNotes, " " ) != 0 ) strcpy( m_vectCatMStars[nCatSrc].notes, strCatNotes );

						m_vectCatMStars[nCatSrc].ra = nRa;
						m_vectCatMStars[nCatSrc].dec = nDec;
						m_vectCatMStars[nCatSrc].mag = nMag;
						m_vectCatMStars[nCatSrc].mag2 = nMag2;
						m_vectCatMStars[nCatSrc].pos_ang = nPA;
						m_vectCatMStars[nCatSrc].pos_ang2 = nPA2;
						m_vectCatMStars[nCatSrc].pm_ra =  nPmRA;
						m_vectCatMStars[nCatSrc].pm_dec = nPmDEC;
						m_vectCatMStars[nCatSrc].pm_ra2 =  nPmRA2;
						m_vectCatMStars[nCatSrc].pm_dec2 = nPmDEC2;
						m_vectCatMStars[nCatSrc].sep = nSep;
						m_vectCatMStars[nCatSrc].sep2 = nSep2;
						m_vectCatMStars[nCatSrc].nobs = nNObs;
						m_vectCatMStars[nCatSrc].obs_date = nObsDate;
						m_vectCatMStars[nCatSrc].obs_date2 = nObsDate2;
						m_vectCatMStars[nCatSrc].cat_type = (unsigned char) nCatType;
					}
				}

			}

		//////////////////////////////////////////////////////////////
		// SECTION :: CATALOG EXTRA-SOLAR PLANETS
		} else if( nFoundSection == ASTROIMG_PROF_SECTION_ID_CAT_EXOPLANETS && bNewLine )
		{
			// and here we look to load extra-solar planets
			if( reCatalogExoplanet.Matches( strLine ) )
			{
				double nMass=0.0, nRadius=0.0, nPeriod=0.0, nSmAxis=0.0, nEccentricity=0.0,
					nInclination=0.0, nAngDist=0.0, nUpdateDate=0.0, nStarMagV=0.0,
					nStarMagI=0.0, nStarMagH=0.0, nStarMagJ=0.0, nStarMagK=0.0, nStarDist=0.0,
					nStarFeH=0.0, nStarMass=0.0, nStarRadius=0.0;
				unsigned int nPlStatus=0;
				unsigned int nDiscoveryYear=0.0;
				long nCatSrc = -1;

				// pharse string with scanf
				if( wxSscanf( reCatalogExoplanet.GetMatch(strLine, 1 ),
					//    1  2   3    4   5   6   7   8   9   10  11  12  13  14  15 16 17
					wxT("%u,%u,%[^,],%lu,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%u,%u,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%u\n"),
					&nCount, &nCatType, strCatName,				// 1,2,3
					&nCatNo, &nRa,								// 4,5
					&nDec, &nX,									// 6,7
					&nY, &nMass,								// 8,9
					&nRadius, &nPeriod,							// 10,11
					&nSmAxis, &nEccentricity,					// 12,13
					&nInclination, &nAngDist,					// 14,15
					&nPlStatus, &nDiscoveryYear,				// 16,17
					&nUpdateDate, &nStarMagV,					// 18,19
					&nStarMagI, &nStarMagH,						// 20,21
					&nStarMagJ, &nStarMagK,						// 22,23
					&nStarDist, &nStarFeH,						// 24,25
					&nStarMass, &nStarRadius,					// 26,27
					&nType ) )									// 28
				{
					DefCatExoPlanet src;
					memset( &src, 0, sizeof(DefCatExoPlanet) );

					src.x = nX;
					src.y = nY;
					src.cat_no = nCatNo;
					src.type = (unsigned char) nType;
					nCatSrc = AddExoplanet( src );

					if( nCatSrc >= 0 )
					{
						if( wxStrcmp( strCatName, wxT(" ") ) != 0 ) wxStrcpy( m_vectCatExoplanet[nCatSrc].cat_name, strCatName );

						m_vectCatExoplanet[nCatSrc].ra = nRa;
						m_vectCatExoplanet[nCatSrc].dec = nDec;
						m_vectCatExoplanet[nCatSrc].mass = nMass;
						m_vectCatExoplanet[nCatSrc].radius = nRadius;
						m_vectCatExoplanet[nCatSrc].period = nPeriod;
						m_vectCatExoplanet[nCatSrc].sm_axis = nSmAxis;
						m_vectCatExoplanet[nCatSrc].eccentricity =  nEccentricity;
						m_vectCatExoplanet[nCatSrc].inclination = nInclination;
						m_vectCatExoplanet[nCatSrc].ang_dist = nAngDist;

						m_vectCatExoplanet[nCatSrc].status = (unsigned char) nPlStatus;
						m_vectCatExoplanet[nCatSrc].discovery_year = nDiscoveryYear;
						m_vectCatExoplanet[nCatSrc].update_date = nUpdateDate;

						m_vectCatExoplanet[nCatSrc].star_mag_v = nStarMagV;
						m_vectCatExoplanet[nCatSrc].star_mag_i = nStarMagI;
						m_vectCatExoplanet[nCatSrc].star_mag_h = nStarMagH;
						m_vectCatExoplanet[nCatSrc].star_mag_j = nStarMagJ;
						m_vectCatExoplanet[nCatSrc].star_mag_k = nStarMagK;
						m_vectCatExoplanet[nCatSrc].star_dist = nStarDist;
						m_vectCatExoplanet[nCatSrc].star_feh = nStarFeH;
						m_vectCatExoplanet[nCatSrc].star_mass = nStarMass;
						m_vectCatExoplanet[nCatSrc].star_radius = nStarRadius;

						m_vectCatExoplanet[nCatSrc].cat_type = (unsigned char) nCatType;
					}
				}

			}

		//////////////////////////////////////////////////////////////
		// SECTION :: CATALOG ASTEROIDS
		} else if( nFoundSection == ASTROIMG_PROF_SECTION_ID_CAT_ASTEROIDS && bNewLine )
		{
			char strCompName[255];
			char strIrasClass[255];
			unsigned long nAstNo=0, nNoArc=0;
			unsigned int nNoObs=0;
			double nAbsMagH=0.0, nSlopeMag=0.0, nColorIndex=0.0, nIrasDiam=0.0,
					nEpoch=0.0, nMeanAnomaly=0.0, nArgPerihelion=0.0, nLonAscNode=0.0,
					nDateOrbComp=0.0, nUpdateDate=0.0, nInclination=0.0,
					nEccentricity=0.0, nSmajAxis=0.0;

			// and here we look to load asteroids
			if( reCatalogAsteroid.Matches( strLine ) )
			{
				long nCatSrc = -1;

				// pharse string with scanf
				if( sscanf( reCatalogAsteroid.GetMatch(strLine, 1 ).ToAscii(),
					"%u,%u,%[^,],%lu,%lf,%lf,%lf,%lf,%lu,%[^,],%lf,%lf,%lf,%lf,%[^,],%lu,%u,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%u\n",
					&nCount, &nCatType, str_catname,				// 1,2,3
					&nCatNo, &nRa,								// 4,5
					&nDec, &nX,									// 6,7
					&nY, &nAstNo,								// 8,9
					strCompName, &nAbsMagH,						// 10,11
					&nSlopeMag, &nColorIndex,					// 12,13
					&nIrasDiam, strIrasClass,					// 14,15
					&nNoArc, &nNoObs,							// 16,17
					&nEpoch, &nMeanAnomaly,						// 18,19
					&nArgPerihelion, &nLonAscNode,				// 20,21
					&nDateOrbComp, &nUpdateDate,				// 22,23
					&nInclination, &nEccentricity,				// 24,25
					&nSmajAxis, &nType ) )						// 26,27
				{
					DefCatAsteroid src;
					memset( &src, 0, sizeof(DefCatAsteroid) );

					src.x = nX;
					src.y = nY;
					src.cat_no = nCatNo;
					src.type = (unsigned char) nType;
					nCatSrc = AddAsteroid( src );

					if( nCatSrc >= 0 )
					{
						if( strcmp( str_catname, " " ) != 0 ) strcpy( m_vectCatAsteroid[nCatSrc].cat_name, str_catname );
						if( strcmp( strCompName, " " ) != 0 ) strcpy( m_vectCatAsteroid[nCatSrc].comp_name, strCompName );
						if( strcmp( strIrasClass, " " ) != 0 ) strcpy( m_vectCatAsteroid[nCatSrc].iras_class, strIrasClass );

						m_vectCatAsteroid[nCatSrc].ra = nRa;
						m_vectCatAsteroid[nCatSrc].dec = nDec;
						m_vectCatAsteroid[nCatSrc].ast_no = nAstNo;
						m_vectCatAsteroid[nCatSrc].abs_mag_h = nAbsMagH;
						m_vectCatAsteroid[nCatSrc].slope_mag = nSlopeMag;
						m_vectCatAsteroid[nCatSrc].color_index = nColorIndex;
						m_vectCatAsteroid[nCatSrc].iras_diam =  nIrasDiam;
						m_vectCatAsteroid[nCatSrc].no_arc = nNoArc;
						m_vectCatAsteroid[nCatSrc].no_obs = nNoObs;
						m_vectCatAsteroid[nCatSrc].epoch = nEpoch;
						m_vectCatAsteroid[nCatSrc].mean_anomaly = nMeanAnomaly;
						m_vectCatAsteroid[nCatSrc].arg_perihelion = nArgPerihelion;
						m_vectCatAsteroid[nCatSrc].lon_asc_node = nLonAscNode;
						m_vectCatAsteroid[nCatSrc].date_orb_comp = nDateOrbComp;
						m_vectCatAsteroid[nCatSrc].update_date = nUpdateDate;
						m_vectCatAsteroid[nCatSrc].inclination = nInclination;
						m_vectCatAsteroid[nCatSrc].eccentricity = nEccentricity;
						m_vectCatAsteroid[nCatSrc].smaj_axis = nSmajAxis;
						m_vectCatAsteroid[nCatSrc].cat_type = (unsigned char) nCatType;
					}
				}

			}

		//////////////////////////////////////////////////////////////
		// SECTION :: CATALOG COMETS
		} else if( nFoundSection == ASTROIMG_PROF_SECTION_ID_CAT_COMETS && bNewLine )
		{
			wxChar strCompName[255];
			wxChar strIAUCode[255];
			unsigned int nRelEffect=0;
			double nNoteUpdateDate=0.0, nArgPerihelion=0.0, nLonOrbitalNode=0.0,
					nEpochComp=0.0, nPerihelionDistance=0.0, nInclination=0.0,
					nPerihelionDate=0.0, nEccentricity=0.0;

			// and here we look to load comet data
			if( reCatalogComet.Matches( strLine ) )
			{
				long nCatSrc = -1;

				// pharse string with scanf
				if( wxSscanf( reCatalogComet.GetMatch(strLine, 1 ),
					wxT("%u,%u,%[^,],%lu,%lf,%lf,%lf,%lf,%lf,%[^,],%[^,],%lf,%u,%lf,%lf,%lf,%lf,%lf,%lf,%u\n"),
					&nCount, &nCatType, strCatName,				// 1,2,3
					&nCatNo, &nRa,								// 4,5
					&nDec, &nX,									// 6,7
					&nY, &nNoteUpdateDate,						// 8,9
					strIAUCode, strCompName,					// 10,11
					&nEpochComp, &nRelEffect,					// 12,13
					&nPerihelionDate, &nPerihelionDistance,		// 14,15
					&nArgPerihelion, &nLonOrbitalNode,			// 16,17
					&nInclination, &nEccentricity,				// 18,19
					&nType ) )									// 20
				{
					DefCatComet src;
					memset( &src, 0, sizeof(DefCatComet) );

					src.x = nX;
					src.y = nY;
					src.cat_no = nCatNo;
					src.type = (unsigned char) nType;
					nCatSrc = AddComet( src );

					if( nCatSrc >= 0 )
					{
						if( wxStrcmp( strCatName, wxT(" ") ) != 0 ) wxStrcpy( m_vectCatComet[nCatSrc].cat_name, strCatName );
						if( wxStrcmp( strCompName, wxT(" ") ) != 0 ) wxStrcpy( m_vectCatComet[nCatSrc].comp_name, strCompName );
						if( wxStrcmp( strIAUCode, wxT(" ") ) != 0 ) wxStrcpy( m_vectCatComet[nCatSrc].iau_code, strIAUCode );

						m_vectCatComet[nCatSrc].ra = nRa;
						m_vectCatComet[nCatSrc].dec = nDec;
						m_vectCatComet[nCatSrc].note_update_date = nNoteUpdateDate;

						m_vectCatComet[nCatSrc].epoch_comp = nEpochComp;
						m_vectCatComet[nCatSrc].rel_effect = (unsigned char) nRelEffect;

						m_vectCatComet[nCatSrc].perihelion_date = nPerihelionDate;
						m_vectCatComet[nCatSrc].perihelion_distance =  nPerihelionDistance;
						m_vectCatComet[nCatSrc].arg_perihelion = nArgPerihelion;
						m_vectCatComet[nCatSrc].lon_orbital_node = nLonOrbitalNode;
						m_vectCatComet[nCatSrc].inclination = nInclination;
						m_vectCatComet[nCatSrc].eccentricity = nEccentricity;

						m_vectCatComet[nCatSrc].cat_type = (unsigned char) nCatType;
					}
				}

			}

		//////////////////////////////////////////////////////////////
		// SECTION :: CATALOG AES - ARTFICIAL EARTH SATELLITES
		} else if( nFoundSection == ASTROIMG_PROF_SECTION_ID_CAT_AES && bNewLine )
		{
			char strVectSatType[500];
			char strIntIdLaunchPiece[255];
			unsigned int nIntIdYear=0, nIntIdLaunchNo=0, nElementNumber=0, nOrbNoEpoch=0;
			double nDetectTimeShift=0.0, nVmag=0.0, nDistFromObs=0.0,
					nEpochTime=0.0, nOrbitDecayRate=0.0, nMeanMotionSecondTimeDeriv=0.0,
					nObjectDragCoeff=0.0, nInclination=0.0, nAscNodeRa=0,
					nEccentricity=0.0, nArgOfPerigee=0.0, nMeanAnomaly=0.0, nMeanMotion=0.0;
			unsigned int nObjType=0, nSatType=0, nSecurityClass=0, nEphemerisType=0;
			unsigned long nNoradCatNo=0;

			// and here we look to load aes data
			if( reCatalogAes.Matches( strLine ) )
			{
				long nCatSrc = -1;

				// pharse string with scanf
				if( sscanf( reCatalogAes.GetMatch( strLine, 1 ).ToAscii(),
					"%u,%u,%[^,],%lu,%lf,%lf,%lf,%lf,%u,%u,%[^,],%lu,%u,%u,%u,%[^,],%u,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%u,%lf,%lf,%lf,%lf,%lf,%lf,%u,%u\n",
					&nCount, &nCatType, str_catname,						// 1,2,3
					&nCatNo, &nRa,										// 4,5
					&nDec, &nX,											// 6,7
					&nY, &nObjType,										// 8,9
					&nSatType, strVectSatType,							// 10,11
					&nNoradCatNo, &nSecurityClass,						// 12,13
					&nIntIdYear, &nIntIdLaunchNo,						// 14,15
					strIntIdLaunchPiece, &nEphemerisType,				// 16,17
					&nDetectTimeShift, &nVmag,							// 18,19
					&nDistFromObs, &nEpochTime,							// 20,21
					&nOrbitDecayRate, &nMeanMotionSecondTimeDeriv,		// 22,23
					&nObjectDragCoeff, &nElementNumber,					// 24,25
					&nInclination, &nAscNodeRa,							// 26,27
					&nEccentricity, &nArgOfPerigee,						// 28,29
					&nMeanAnomaly, &nMeanMotion,						// 30,31
					&nOrbNoEpoch, &nType ) )							// 32,33
				{
					DefCatAes src;
					ResetObjCatAes( src );

					src.x = nX;
					src.y = nY;
					src.cat_no = nCatNo;
					src.type = (unsigned char) nType;
					nCatSrc = AddAes( src );
					if( nCatSrc >= 0 )
					{
						if( strcmp( str_catname, " " ) != 0 ) strcpy( m_vectCatAes[nCatSrc].cat_name, str_catname );
						if( strcmp( strIntIdLaunchPiece, " " ) != 0 ) 
							strcpy( m_vectCatAes[nCatSrc].int_id_launch_piece, strIntIdLaunchPiece );

						m_vectCatAes[nCatSrc].ra = nRa;
						m_vectCatAes[nCatSrc].dec = nDec;
						m_vectCatAes[nCatSrc].obj_type = (unsigned char) nObjType;
						m_vectCatAes[nCatSrc].nSatType = nSatType;
						// set satellites types
						int k=0;
						wxString strWxVectSatType(strVectSatType,wxConvUTF8);

						wxStringTokenizer tkz( strWxVectSatType, wxT(":") );
						while ( tkz.HasMoreTokens() )
						{
							wxString strSatType = tkz.GetNextToken();
							m_vectCatAes[nCatSrc].vectSatType[k] = wxAtoi( strSatType );
							k++;
						}	

						m_vectCatAes[nCatSrc].norad_cat_no = nNoradCatNo;
						m_vectCatAes[nCatSrc].security_class = (unsigned char) nSecurityClass;

						m_vectCatAes[nCatSrc].int_id_year = nIntIdYear;
						m_vectCatAes[nCatSrc].int_id_launch_no =  nIntIdLaunchNo;

						m_vectCatAes[nCatSrc].ephemeris_type = (unsigned char) nEphemerisType;
						m_vectCatAes[nCatSrc].detect_time_shift = nDetectTimeShift;
						m_vectCatAes[nCatSrc].vmag = nVmag;
						m_vectCatAes[nCatSrc].dist_from_obs = nDistFromObs;
						// TLE record
						m_vectCatAes[nCatSrc].nTLERec = 1;
						m_vectCatAes[nCatSrc].nTLERecAllocated = 1;
						m_vectCatAes[nCatSrc].nTLERecInUse = 0;
						// allocate 1 TLE record
						m_vectCatAes[nCatSrc].vectTLERec = (DefTLERecord*) calloc( 1, sizeof(DefTLERecord) );
						// set tle record
						m_vectCatAes[nCatSrc].vectTLERec[0].epoch_time = nEpochTime;
						m_vectCatAes[nCatSrc].vectTLERec[0].orbit_decay_rate = nOrbitDecayRate;
						m_vectCatAes[nCatSrc].vectTLERec[0].mean_motion_second_time_deriv = nMeanMotionSecondTimeDeriv;
						m_vectCatAes[nCatSrc].vectTLERec[0].object_drag_coeff = nObjectDragCoeff;
						m_vectCatAes[nCatSrc].vectTLERec[0].element_number = nElementNumber;
						m_vectCatAes[nCatSrc].vectTLERec[0].inclination = nInclination;
						m_vectCatAes[nCatSrc].vectTLERec[0].asc_node_ra = nAscNodeRa;
						m_vectCatAes[nCatSrc].vectTLERec[0].eccentricity = nEccentricity;
						m_vectCatAes[nCatSrc].vectTLERec[0].arg_of_perigee = nArgOfPerigee;
						m_vectCatAes[nCatSrc].vectTLERec[0].mean_anomaly = nMeanAnomaly;
						m_vectCatAes[nCatSrc].vectTLERec[0].mean_motion = nMeanMotion;
						m_vectCatAes[nCatSrc].vectTLERec[0].orb_no_epoch = nOrbNoEpoch;

						// other not used yet
						m_vectCatAes[nCatSrc].class_of_orbit = nEpochTime;
						m_vectCatAes[nCatSrc].type_of_orbit = nEpochTime;

						m_vectCatAes[nCatSrc].cat_type = (unsigned char) nCatType;
					}
				}

			}

		//////////////////////////////////////////////////////////////
		// SECTION :: CATALOG SOLAR PLANETS/SUN/MOON
		} else if( nFoundSection == ASTROIMG_PROF_SECTION_ID_CAT_PLANETS && bNewLine )
		{
			wxChar strCompName[255];
			unsigned int nObjOrbId=0;
			double nDetectTimeShift=0.0, nDistFromObs=0.0;

			// and here we look to load solar objects data
			if( reCatalogSolar.Matches( strLine ) )
			{
				long nCatSrc = -1;

				// pharse string with scanf
				if( wxSscanf( reCatalogSolar.GetMatch(strLine, 1 ),
					wxT("%u,%u,%[^,],%u,%lf,%lf,%lf,%lf,%u,%lf,%lf\n"),
					&nCount, &nType, strCatName,				// 1,2,3
					&nCatNo, &nRa,								// 4,5
					&nDec, &nX,									// 6,7
					&nY, &nObjOrbId,							// 8,9
					&nDistFromObs, &nDetectTimeShift ) )		// 10,11
				{
					DefPlanet src;
					memset( &src, 0, sizeof(DefPlanet) );
					src.x = nX;
					src.y = nY;
					src.obj_id = nCatNo;
					src.obj_type = (unsigned char) nType;
					nCatSrc = AddSolarPlanet( src );
					if( nCatSrc >= 0 )
					{
						if( wxStrcmp( strCatName, wxT(" ") ) != 0 ) wxStrcpy( m_vectSolarPlanets[nCatSrc].obj_name, strCatName );

						m_vectSolarPlanets[nCatSrc].ra = nRa;
						m_vectSolarPlanets[nCatSrc].dec = nDec;
						m_vectSolarPlanets[nCatSrc].obj_orb_id = nObjOrbId;

						m_vectSolarPlanets[nCatSrc].dist_from_obs = nDistFromObs;
						m_vectSolarPlanets[nCatSrc].detect_time_shift = nDetectTimeShift;
					}
				}

			}

		//////////////////////////////////////////////////////////////
		// SECTION :: CATALOG CONSTELATIONS
		} else if( nFoundSection == ASTROIMG_PROF_SECTION_ID_CAT_CONSTL && bNewLine )
		{
			// and here we look to constellations
			if( reConstellations.Matches( strLine ) )
			{
				wxString strConstId = reConstellations.GetMatch( strLine, 1 );
				wxString strConstName = reConstellations.GetMatch( strLine, 2 );
				int nConstId = AddConstellation( strConstName, wxAtoi( strConstId ) );
				// extract constellation lines and add
				wxString strConstellationLines = reConstellations.GetMatch( strLine, 3 );
				while( reConstellationLine.Matches( strConstellationLines ) )
				{
					wxString strX1 = reConstellationLine.GetMatch( strConstellationLines, 1 );
					wxString strY1 = reConstellationLine.GetMatch( strConstellationLines, 2 );
					wxString strX2 = reConstellationLine.GetMatch( strConstellationLines, 3 );
					wxString strY2 = reConstellationLine.GetMatch( strConstellationLines, 4 );
					// add constelation line
					AddConstellationLine( nConstId, wxAtoi(strX1), wxAtoi(strY1), wxAtoi(strX2), wxAtoi(strY2) );
					// remove string fromline
					wxString strConstLine = wxT("(") + strX1 + wxT(",") + strY1 + wxT(",") + strX2 + wxT(",") + strY2 + wxT(")");
					strConstellationLines.Replace( strConstLine, wxT(""), false );
				}
			}

		//////////////////////////////////////////////////////////////
		// SECTION :: NOTES
		} else if( nFoundSection == ASTROIMG_PROF_SECTION_ID_NOTE && bNewLine )
		{
			int nNote = m_vectNotes.size()-1;

			// first check for message body start/end
			if( wxStrncmp( strLine, wxT(">>>MSG<<<"), 9 ) == 0 )
			{
				bNoteMessageBody = 1;
				continue;

			} else if( wxStrncmp( strLine, wxT("<<<MSG>>>"), 9 ) == 0 )
			{
				bNoteMessageBody = 0;
				m_vectNotes[nNote].m_strMessage.Trim();
				continue;
			}

			// then check if message body started
			if( bNoteMessageBody )
			{
				m_vectNotes[nNote].m_strMessage += strLine;
				continue;
			}

			if( wxStrncmp( strLine, wxT("Id="), 3 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Id=%d\n"), &(m_vectNotes[nNote].m_nId) ) ) m_vectNotes[nNote].m_nId = 0;

			} else if( wxStrncmp( strLine, wxT("Type="), 5 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Type=%d\n"), &(m_vectNotes[nNote].m_nType) ) ) m_vectNotes[nNote].m_nType = 0;

			} else if( wxStrncmp( strLine, wxT("DateTime="), 9 ) == 0 )
			{
				double jdn=0.0;
				if( !wxSscanf( strLine, wxT("DateTime=%lf\n"), &jdn ) ) 
					m_vectNotes[nNote].m_rDateTime.SetToCurrent();
				else
					m_vectNotes[nNote].m_rDateTime.Set( jdn );

			} else if( wxStrncmp( strLine, wxT("Title="), 6 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Title=%[^\n]\n"), strTmp ) ) 
					m_vectNotes[nNote].m_strTitle = wxT("");
				else
					m_vectNotes[nNote].m_strTitle = strTmp;

			} else if( wxStrncmp( strLine, wxT("SndFile="), 8 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("SndFile=%[^\n]\n"), strTmp ) ) 
					m_vectNotes[nNote].m_strSoundFile = wxT("");
				else
					m_vectNotes[nNote].m_strSoundFile = strTmp;

			} else if( wxStrncmp( strLine, wxT("Pos="), 4 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Pos=%lf,%lf\n"), &(m_vectNotes[nNote].x), &(m_vectNotes[nNote].y) ) )
				{
					m_vectNotes[nNote].x = 0.0;
					m_vectNotes[nNote].y = 0.0;
				}
			}
		}
	}

	// calculate rotation angle
	if( m_pBestTrans )
	{
		m_nImgRotAngle = ( atan2( m_pBestTrans->c, m_pBestTrans->b ) * 180 ) / PI;
		m_nImgTransScale = sqrt( m_pBestTrans->b*m_pBestTrans->b + m_pBestTrans->c*m_pBestTrans->c );
	}

	// close file
	fclose( pFile );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
void CAstroImage::ExportToHtml( const wxString& strFile )
{
	FILE* pFile = NULL;
//	wxString strFile;

	wxString strHtml=wxT("");
	// add header
	strHtml = wxT("<html>\n<body>\n<center>\n");
	// title
	strHtml += wxT("<b><font size=\"5\" face=\"arial\">") + m_strImageName + wxT("</font></b><br><br>\n");
	// add image
	strHtml += wxT("<img src=\"") + GetImageName() + wxT("\" alt=\"") + m_strImageName + wxT("\" width=800>");
	// add tail
	strHtml += wxT("</center>\n</body>\n</html>\n");

	// save file
	pFile = wxFopen( strFile, wxT("w") );
	// write text
	wxFputs( strHtml, pFile );
	// close file
	fclose( pFile );
}

// check if image has distances data on stars or galaxies
// check in order of fewer - first galaxies the catalog stars then 
// detected stars
/////////////////////////////////////////////////////////////////////
int CAstroImage::HasDistancesData()
{
	int i=0;

	///////////////////////////////
	// check DSO
	if( m_nCatDso > 0 )
	{
		// for every catalog dso
		for( i=0; i<m_nCatDso; i++ )
			if( m_vectCatDso[i].distance > 0.0 ) return(1);
	}

	///////////////////////////////
	// check catalog stars
	if( m_nCatStar > 0 )
	{
		// save every catalog star
		for( i=0; i<m_nCatStar; i++ )
			if( m_vectCatStar[i].distance > 0.0 ) return(1);
	}

	////////////////////
	// check in detetcted stars
	if( m_nStar > 0 )
	{
		// for every detected star
		for( i=0; i<m_nStar; i++ )
			// if matched
			if( m_vectStar[i].match > 0 && m_vectStar[i].distance > 0.0 ) return(1);
	}

	return(0);
}
