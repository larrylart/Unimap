////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _ASTRO_IMAGE
#define _ASTRO_IMAGE

//#define DETECT_STAR_IMAGE_MAX_NO	10000
//#define CAT_DSO_IMAGE_MAX_NO		1000

// thumbnails size
#define DEFAULT_THUMBNAIL_WIDTH		120
#define DEFAULT_THUMBNAIL_HEIGHT	100
// image formats
#define ASTROIMAGE_TYPE_UNKNOWN	0
#define ASTROIMAGE_TYPE_JPG		1
#define ASTROIMAGE_TYPE_GIF		2
#define ASTROIMAGE_TYPE_BMP		3
#define ASTROIMAGE_TYPE_PNG		4
#define ASTROIMAGE_TYPE_PCX		5
#define ASTROIMAGE_TYPE_PNM		6
#define ASTROIMAGE_TYPE_TIF		7
#define ASTROIMAGE_TYPE_TGA		8

//////////////////
// image profile sections
#define	ASTROIMG_PROF_SECTION_ID_NONE				0
#define	ASTROIMG_PROF_SECTION_ID_PROPERTIES			1
#define	ASTROIMG_PROF_SECTION_ID_DETECTION			2
#define	ASTROIMG_PROF_SECTION_ID_MATCHING			3
#define	ASTROIMG_PROF_SECTION_ID_HINT				4
#define	ASTROIMG_PROF_SECTION_ID_AREAS				5
#define	ASTROIMG_PROF_SECTION_ID_DETECTED_STARS		6
#define	ASTROIMG_PROF_SECTION_ID_CAT_STARS			7
#define	ASTROIMG_PROF_SECTION_ID_CAT_DSO			8
#define	ASTROIMG_PROF_SECTION_ID_CAT_RADIO			9
#define	ASTROIMG_PROF_SECTION_ID_CAT_XGAMMA			10
#define	ASTROIMG_PROF_SECTION_ID_CAT_SUPERNOVA		11
#define	ASTROIMG_PROF_SECTION_ID_CAT_BLACKHOLE		12
#define	ASTROIMG_PROF_SECTION_ID_CAT_MSTARS			13
#define	ASTROIMG_PROF_SECTION_ID_CAT_EXOPLANETS		14
#define	ASTROIMG_PROF_SECTION_ID_CAT_ASTEROIDS		15
#define	ASTROIMG_PROF_SECTION_ID_CAT_COMETS			16
#define	ASTROIMG_PROF_SECTION_ID_CAT_AES			17
#define	ASTROIMG_PROF_SECTION_ID_CAT_PLANETS		18
#define	ASTROIMG_PROF_SECTION_ID_CAT_CONSTL			19
#define	ASTROIMG_PROF_SECTION_ID_NOTE				20

////////////
// channel extraction type
#define EXTRACT_PROCESSING_DATA_RED					1
#define EXTRACT_PROCESSING_DATA_GREEN				2
#define EXTRACT_PROCESSING_DATA_BLUE				3
#define EXTRACT_PROCESSING_DATA_DOUBLE_COMPOSITE	4
#define EXTRACT_PROCESSING_DATA_MEDIAN_COMPOSITE	5
#define EXTRACT_PROCESSING_DATA_WINNER				6

///////////
// sorting methods for objects in the image
#define SORT_OBJECTS_METHOD_MAGNITUDE_DESC			0
#define SORT_OBJECTS_METHOD_MAGNITUDE_ASC			1
#define SORT_OBJECTS_METHOD_FWHM_DESC				2
#define SORT_OBJECTS_METHOD_FWHM_ASC				3
#define SORT_OBJECTS_METHOD_FLUX_DESC				4
#define SORT_OBJECTS_METHOD_FLUX_ASC				5
#define SORT_OBJECTS_METHOD_SIZE_DESC				6
#define SORT_OBJECTS_METHOD_SIZE_ASC				7

/////////////
// define block size allocation
#define STAR_DETECT_BLOCK_SIZE_ALLOC		50
#define STAR_CAT_BLOCK_SIZE_ALLOC			50
#define DSO_CAT_BLOCK_SIZE_ALLOC			20
#define RADIO_CAT_BLOCK_SIZE_ALLOC			20
#define XGAMMA_CAT_BLOCK_SIZE_ALLOC			20
#define SUPERNOVAS_CAT_BLOCK_SIZE_ALLOC		20
#define BLACKHOLES_CAT_BLOCK_SIZE_ALLOC		20
#define MSTARS_CAT_BLOCK_SIZE_ALLOC			20
#define EXOPLANETS_CAT_BLOCK_SIZE_ALLOC		20
#define ASTEROIDS_CAT_BLOCK_SIZE_ALLOC		20
#define COMETS_CAT_BLOCK_SIZE_ALLOC			20
#define AES_CAT_BLOCK_SIZE_ALLOC			20
#define SOLPLANETS_CAT_BLOCK_SIZE_ALLOC		10
#define	CONSTELLATION_BLOCK_SIZE_ALLOC		5
// flags
#define FLAG_FIND_OBJ_WITH_DISTANCE	1
#define FLAG_FIND_OBJ_NOT_SHOWN		2

// system headers
#include <time.h>
#include <vector>
#include <cmath>

// wxwidgets headers
#include <wx/image.h>
#include <wx/datetime.h>

// elynx headers
#include <elx/image/ImageVariant.h>
#include <elx/image/ImageImpl.h>
#include <elx/image/Pixels.h>

// some local headers
#include "../sky/star.h"
#include "../sky/constellations.h"
#include "../match/misc.h"
#include "digitalimage.h"
#include "imagearea.h"
#include "../observer/observer.h"
// other catalogs
#include "../sky/catalog_radio.h"
#include "../sky/catalog_xgamma.h"
#include "../sky/catalog_supernovas.h"
#include "../sky/catalog_blackholes.h"
#include "../sky/catalog_mstars.h"
#include "../sky/catalog_exoplanets.h"
#include "../sky/catalog_asteroids.h"
#include "../sky/catalog_comets.h"
#include "../sky/catalog_aes.h"
#include "../sky/solar_planets.h"

// external classes 
class CDigitalImage;
class CLogger;
class CImageGroup;
//class CImageArea;
class CSky;
class CConfigMain;
class CConfigDetect;
//class CStarMatch;
class CSextractor;
class CSkyCatalogStars;
class CImageNote;
class CImgFmtFits;

// global sorting methods
static bool OnSortImgStarByMagDesc( StarDef t1, StarDef t2 ) { return( t1.mag<t2.mag ); }
static bool OnSortImgStarByMagAsc( StarDef t1, StarDef t2 ) { return( t1.mag>t2.mag ); }
static bool OnSortImgStarByFwhmDesc( StarDef t1, StarDef t2 ) { return( t1.fwhm>t2.fwhm ); }
// :: satellites
static bool OnSortAesByVMagDesc( DefCatAes t1, DefCatAes t2 ) { return( t1.vmag<t2.vmag ); }
static bool OnSortAesByObsDistDesc( DefCatAes t1, DefCatAes t2 ) { return( t1.dist_from_obs>t2.dist_from_obs ); }
// constellations
static bool OnSortCnstlByScore( ImgConstellationStructure t1, ImgConstellationStructure t2 ) { return( t1.score>t2.score ); }

// structures
typedef struct
{
	double x;
	double y;
	double z;

} DisplacementPoint;

using namespace eLynx;
using namespace eLynx::Image;

// class: CAstroImage
class CAstroImage : public CDigitalImage
{

// public methods
public:
	// constructor/destructor
	CAstroImage( CImageGroup* pImageGroup, const wxString& strImagePath, 
							const wxString& strImageName, const wxString& strImageExt );
	~CAstroImage( );
	// initialization 
	void ResetParams( );
	int InitDetails( );
	// save/load image info - stars etc
	int LoadExifProfile( );
	int Load( int bLogFlag=1 );
	int LoadProfile( int bLogFlag=1 );
	int LoadCProfile( int bLogFlag=1 );
	int SaveProfile( int bLogFlag );
	int SaveImage( wxImage& rImage, const wxString& strFilename=wxT("") );
	// elynx handlers
	int LoadELynxImage( ImageVariant& rImage, const wxString& strFileName );
	wxImage* CreateImageFromELX( const ImageRGBub& iImage );
	boost::shared_ptr<ImageRGBub> _ELX_CreateRGBub( const ImageVariant& iImage, bool ibViewTransformed) const;

	// file name/ini handlers
	void _GetFullName( char* strFile ){ sprintf( strFile, "%s%s%s%s%s", m_strImagePath, wxT("\\"), m_strImageName, wxT("."), m_strImageExt ); }
	void _GetImageName( char* strName ){ sprintf( strName, "%s%s%s", m_strImageName, wxT("."), m_strImageExt ); }
	wxString GetFullName( ){ wxString ret=wxT(""); ret.sprintf( wxT("%s%s%s%s%s"), m_strImagePath, wxT("\\"), m_strImageName, wxT("."), m_strImageExt ); return(ret); }
	wxString GetFullNameBase( ){ wxString ret=wxT(""); ret.sprintf( wxT("%s%s%s"), m_strImagePath, wxT("\\"), m_strImageName ); return(ret); }
	wxString GetImageName( ){ wxString ret=wxT(""); ret.sprintf( wxT("%s%s%s"), m_strImageName, wxT("."), m_strImageExt ); return(ret); }
	wxString GetIniFilePath( ){ wxString ret=wxT(""); ret.sprintf( wxT("%s%s%s%s"), m_strImagePath, wxT("\\"), m_strImageName, wxT(".ini") ); return(ret); }
	wxString GetIniFileName( ){ wxString ret=wxT(""); ret.sprintf( wxT("%s%s"), m_strImageName, wxT(".ini") ); return(ret); }

	// free mem allocated
	void FreeImageData( );
	void FreeData( int nFlag=0 );
	void FreeStars( );
	void FreeCatStars( );
	void FreeCatDso( );
	void FreeCatRadioSources( );
	void FreeCatXGammaSources( );
	void FreeCatSupernovas( );
	void FreeCatBlackholes( );
	void FreeCatMStars( );
	void FreeCatExoplanets( );
	void FreeCatAsteroids( );
	void FreeCatComets( );
	void FreeCatAes( );
	void FreeSolarPlanets( );
	void FreeConstellations( );

	void ClearDetected( );
	void ClearMatched( );

	void SetConfig( CConfigMain* pMainConfig, CConfigDetect* pDetectConfig );
	int SetPath( const wxString& strPath );
	// :: hint
	void SetHint( double nRa, double nDec, double nWidth, double nHeight );
	int IsHint( );
	int GetHint( double& nRa, double& nDec, double& nWidth, double& nHeight );

	void SetCoordByMinMaxRaDec( double minRA, double maxRA, double minDEC, double maxDEC );
	void SetMainCoord( double fieldCenterRa, double fieldCenterDec, 
						double fieldWidth, double fieldHeight, double fieldRadius );

	//////////////////
	// methods to get image, data from image and image details
	void GetImageFileInfo( );
	int GetImageExif( wxArrayString& vectKey, wxArrayString& vectValue );
	int GetImageFits( wxArrayString& vectKey, wxArrayString& vectValue );
	wxImage* GetImage( );
	wxImage* GetStackImage( );
	int GetImageData( int nChannels, int bUseAreas );
	int CloseImage( );

	// methods to do the processing
	int Detect( CSextractor* pDetect, int bSaveProfile );

	int CreateThumbnail( wxBitmap& pThumbnail, int nWidth=DEFAULT_THUMBNAIL_WIDTH, int nHeight=DEFAULT_THUMBNAIL_HEIGHT, int bAspectRatio=0 );
	int CreateThumbnailCustom( const wxString& strOutFile, int nWidth=DEFAULT_THUMBNAIL_WIDTH, int nHeight=DEFAULT_THUMBNAIL_HEIGHT, int bAspectRatio=0 );
	int LoadThumbnail( wxBitmap& pThumbnail );

	int ExtractData( unsigned char *vectImgData, int nType, int bUseAreas );

	////////////////////////////////////////////////
	// get/set? objects - stars/dso names
	void SetSky( CSky* pSky ){ m_pSky = pSky; }
		
	// get star id by name
	long GetStarIdByName( StarDef* vectStar, unsigned long nStar, const wxChar* strStarName );
	long GetDsoObjByCatNo( unsigned long nCatNo, unsigned char nType );

	// process to find stars
	long SortByMagnitude( int bOrder=0 );
	long ProcessStars(  );
	long ExtractStars( StarDef* vectStar, long nStar, int bByShape, int bByDetectType, int nMatchFlag, 
						int bOrderMethod=SORT_OBJECTS_METHOD_MAGNITUDE_DESC, double nFwhmLimit=DBL_MAX );
	long ExtractMatchedStars( StarDef* vectSrcStar, StarDef* vectDestStar, long nStar );
	// set matched/not 
	long SetCatStarsMatched( StarDef* vectStar, long nStar );
	long SetCatStarsNotMatched( StarDef* vectStar, long nStar );
	long MatchNotMatchedStars( StarDef* vectCatStar, long nCatStar );
	long FindClosestObjectInImage( double x0, double y0, double nMinDist, int nMatched );
	// set catalog star names
	long AdjustStarDetailsFromCatalog( CSkyCatalogStars* pCatalogStars );
	// get object in image at XY
	int GetObjectAtXY( double x, double y, long nLastObjId, int nLastObjType, int& nFoundType, int nFlag );

	// adding, removing and editing stars
	long AddStar( double x, double y, int nCx, int nCy, double mag, double a, 
					double b, double theta, double kronfactor, 
					double fwhm, double flux, unsigned char nType );
	void SetStarStringCatalogName( StarDef& pStar, const char* strCatName );

	// - add catalog
	long AddCatStar( StarDef& star, unsigned char nType );
	long AddCatDso( StarDef& star, unsigned char nType );
	long AddRadioSource( DefCatBasicRadio& source );
	long AddXGammaSource( DefCatBasicXGamma& source );
	long AddSupernova( DefCatBasicSupernova& source );
	long AddBlackhole( DefCatBlackhole& source );
	long AddMStars( DefCatMStars& source );
	long AddExoplanet( DefCatExoPlanet& source );
	long AddAsteroid( DefCatAsteroid& source );
	long AddComet( DefCatComet& source );
	long AddAes( DefCatAes& source );
	long AddSolarPlanet( DefPlanet& source );

	long AddConstellation( const wxChar* strName, unsigned int id );
	int	 AddConstellationLine( unsigned int id, long x1, long y1, long x2, long y2 );
	double SetConstellationScore( unsigned int id );
	void SortConstellations();

	// methods from image areas
	int AddArea( int nX, int nY, int nWidth, int nHeight, int id=-1 );
	int EditArea( long nId, int nX, int nY, int nWidth, int nHeight );
	int FindAreaById( long nId );

	// calculate distorsion grid
	int CalcDistGrid( );
	void ClearDistGrid( );
	int CalcDisplacementPointsByPos( );
	void ClearDisplacementPoints( );
	int CalcDisplacementPointsByDist( );

	// image calculus methods
	void CalcPropFromDetails( );

	// this to use as a temporary test for finding dso
	void CalcMatchMinMax( );

	/////////////////
	// date/time functions
	double Get_UTC_JDN(){ return( m_nObsDateTime.FromUTC().GetJDN()-m_nObsTZSecs/86400.0 ); };

	/////////////////
	// notes
	int AddNote( CImageNote* pNote );
	void UpdateNote( long id, CImageNote* pNote );

	///////////////////
	// export to html
	void ExportToHtml( const wxString& strFile );

	// check if image has distances data
	int HasDistancesData();

	// get best title from dso names/star names or other objects
	int SetBestTitle( int bForce=0 );

// public data
public:
	// base variables
	wxString	m_strImageName;
	wxString	m_strImagePath;
	wxString	m_strImageExt;
	int			m_nId;
	wxString	m_strImgDataUID;

	wxImage*	m_pImage;
//	IplImage*	m_pIplImage;
//	IplImage*	m_pIplImageCopy;
	CImgFmtFits* m_pImgFmtFits;

	// image params
	int m_nWidth;
	int m_nHeight;

	// other
	CLogger*		m_pLogger;
	CConfigMain*	m_pMainConfig;
	CConfigDetect*	m_pDetectConfig;
	CSky*			m_pSky;
//	CSextractor*	m_pSextractor;
//	CStarMatch*		m_pStarMatch;

	// image data buffer used for detection
	double* m_vectImageBuff;

	// for start processing
	StarDef* m_vectStar;
	unsigned long m_nStar;
	unsigned long m_nStarAllocated;

	/////////////////////////////
	// here i keep stars from catalog found in the image
	StarDef* m_vectCatStar;
	unsigned long m_nCatStar;
	unsigned long m_nCatStarAllocated;

	/////////////////////////////
	// here i keep dso object from catalog found in the image
	StarDef* m_vectCatDso;
	unsigned long m_nCatDso;
	unsigned long m_nCatDsoAllocated;

	/////////////////////////////
	// RADIO SOURCES
	DefCatBasicRadio* m_vectCatRadio;
	unsigned long m_nCatRadio;
	unsigned long m_nCatRadioAllocated;

	/////////////////////////////
	// X-RAY/GAMMA SOURCES
	DefCatBasicXGamma* m_vectCatXGamma;
	unsigned long m_nCatXGamma;
	unsigned long m_nCatXGammaAllocated;

	/////////////////////////////
	// SUPERNOVAS
	DefCatBasicSupernova* m_vectCatSupernova;
	unsigned long m_nCatSupernova;
	unsigned long m_nCatSupernovaAllocated;

	/////////////////////////////
	// BLACKHOLES
	DefCatBlackhole* m_vectCatBlackhole;
	unsigned long m_nCatBlackhole;
	unsigned long m_nCatBlackholeAllocated;

	/////////////////////////////
	// MULTIPLE/DOUBLE STARS
	DefCatMStars* m_vectCatMStars;
	unsigned long m_nCatMStars;
	unsigned long m_nCatMStarsAllocated;

	/////////////////////////////
	// EXTRA-SOLAR PLANETS
	DefCatExoPlanet* m_vectCatExoplanet;
	unsigned long m_nCatExoplanet;
	unsigned long m_nCatExoplanetAllocated;

	/////////////////////////////
	// ASTEROIDS
	DefCatAsteroid* m_vectCatAsteroid;
	unsigned long m_nCatAsteroid;
	unsigned long m_nCatAsteroidAllocated;

	/////////////////////////////
	// COMETS
	DefCatComet* m_vectCatComet;
	unsigned long m_nCatComet;
	unsigned long m_nCatCometAllocated;

	/////////////////////////////
	// ARTIFICIAL EARTH SATELLITES (AES)
	DefCatAes* m_vectCatAes;
	unsigned long m_nCatAes;
	unsigned long m_nCatAesAllocated;

	/////////////////////////////
	// SOLAR PLANETS/SUN/MOON
	DefPlanet* m_vectSolarPlanets;
	unsigned long m_nSolarPlanets;
	unsigned long m_nSolarPlanetsAllocated;

	////////////////////////////////////
	// constellations
	ImgConstellationStructure* m_vectConstellation;
	int m_nConstellation;
	unsigned long m_nConstellationAllocated;

	//////////////////////////////////////
	// details vectors allocation
	StarBasicDetailsDef* m_vectStarDetail;
	unsigned long m_nStarDetail;
	StarBasicDetailsDef* m_vectCatStarDetail;
	unsigned long m_nCatStarDetail;
	StarBasicDetailsDef* m_vectCatDsoDetail;
	unsigned long m_nCatDsoDetail;

	/////////////////////////////////////
	// data from image node
	CImageGroup*	m_pImageGroup;
	// links to areas in the image with their properties
	std::vector<DefImageArea> m_vectArea;
//	int				m_nArea;

	//////////////////////////
	// image notes
	std::vector<CImageNote> m_vectNotes;

	int m_nLastSortOrderMethod;
	///////////////
	// min/max references
	double m_nMaxMag;
	double m_nMinMag;

	//////////////////////////////
	// data to store detection profile
	int m_nImgSourceType;
	int m_nUseChannel;

	//////////////////////////////
	// data to store matching profile
	TRANS* m_pBestTrans;
	TRANS* m_pInvTrans;
	double m_nSkyProjectionScale;
	int m_nCatalogForMatching;

	///////////////////////////////////
	// DISTORTION GRIG
	double** m_vectDistGridX;
	double** m_vectDistGridY;
	std::vector <DisplacementPoint> m_vectDisplacementPoints;
	//double** m_vectDistGridZ;
	int m_nDistGridXUnits;
	int m_nDistGridYUnits;
	int m_nDistGridXDiv;
	int m_nDistGridYDiv;
	int m_bIsDistGrid;

	//////////////////////////
	// image params after matching
	double m_nRaStart;
	double m_nRaEnd;
	double m_nDecStart;
	double m_nDecEnd;
	double m_nOrigRa;
	double m_nOrigDec;
	double m_nFieldWidth;
	double m_nFieldHeight;
	double m_nRadius;
	double m_nImgRotAngle;
	double m_nImgTransScale;
	double m_nMagnitudeToRadiusRatio;
	// average distances
	double m_nDistanceAverageGalactic;
	double m_nDistanceAverageExtraGalactic;
	// number of bjects matched
	unsigned int m_nMatchedStars;

	////////////////////
	// SECTION :: HINT
	// flags
	int m_nUseHint;
	// ORIGIN
	int m_nHintOrigType;
	int m_nHintObjOrigType;
	int m_nHintObjName;
	wxString m_strHintObjId;
	int m_nHintOrigUnits;
	int m_nHintFieldType;
	// relative center given by user based on star common name or
	// a certain dso
	double m_nRelCenterRa;
	double m_nRelCenterDec;
	// :: setup
	int m_nHintSetupId;
	// :: camera
	int m_nHintCameraSource;
	int m_nHintCameraType; 
	int m_nHintCameraBrand; 
	int m_nHintCameraName;
	// sensor size
	double m_nHintSensorWidth;
	double m_nHintSensorHeight;
	// :: telescope or lens
	int	m_nHintTLensSource;
	DefTLensSetup m_rHintTLensSetup;
	// system focal length
	double m_nHintFocalLength;
	// field format
	int m_nHintFieldFormat;
	// the aprox field size
	double m_nRelFieldRa;
	double m_nRelFieldDec;

	////////////////////////////
	// and other Flags
	int m_bIsChanged;
	int m_nIsRelativeCenter;
	int m_nIsRelativeField;
	int m_bIsDetected;
	int m_bIsMatched;
	// thumbnails
	int m_bHasThumnail;

	/////////////////////////////
	// image properties
//	int m_nFileType;
	time_t m_nFileTime;

	///////
	// other img porperties
	// observer
	wxString	m_strObsName;
	wxString	m_strObsCountry;
	wxString	m_strObsRegion;
	wxString	m_strObsCity;
	// position on the earth
//	char	m_chObsLatitude;
	double	m_nObsLatitude;
//	char	m_chObsLongitude;
	double	m_nObsLongitude;
	double	m_nObsAltitude;
	// date/time when picture was taken
	wxDateTime	m_nObsDateTime;
	long m_nObsTZSecs;
	// target
	wxString	m_strTargetName;
	wxString	m_strTargetDesc;
	double	m_nTargetRa;
	double	m_nTargetDec;
	// equipment
	wxString	m_strTelescopeName;
	int		m_nTelescopeType;
	double	m_nTelescopeFocal;
	double	m_nTelescopeAperture;
	int		m_nTelescopeMount;
	wxString	m_strCameraName;
	// conditions
	int		m_nCondSeeing;
	int		m_nCondTransp;
	double	m_nCondWindSpeed;
	double	m_nCondAtmPressure;
	double	m_nCondTemp;
	double	m_nCondTempVar;
	double	m_nCondHumid;
	int		m_nCondLp;
	// imaging
	double	m_nExpTotalTime;
	int		m_nNoOfExp;
	int		m_nExpIso;
	wxString	m_strRegProg;
	wxString	m_strStackProg;
	int		m_nStackMethod;
	wxString	m_strProcProg;

};

#endif
