
#ifndef _IMAGE_GROUP
#define _IMAGE_GROUP

// c++
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

// local includes
#include "../util/folders.h"

// local defines
#define MAX_IMAGE_NODE_GROUP 300
#define IMAGE_GROUP_INI_FILE_NAME  wxT("img_grp_")

#define DEFAULT_PATH_IMAGE_GROUP_INI_FILE		unMakeAppPath(wxT("data/imagedb/groupini_"))
//#define DEFAULT_PATH_IMAGE_LINK_IMAGE_INI_FILE	"data/image_link_image_db/group_"
#define DEFAULT_PATH_IMAGE_GROUP_LOAD_FILE		unMakeAppPath(wxT("data/imagedb/groupfile_"))
//#define DEFAULT_PATH_IMAGE_GROUP_LOAD_FILE_TEMP	"data/image_db_new/group_"

#define FILE_IMAGE_TYPES	wxT("*.*")
// stack align minimum rot/scale trans
#define STACK_ALIGN_MINIMUM_ROTATION_ANGLE		0.2
#define STACK_ALIGN_MINIMUM_SCALE_FACTOR		0.02

// minimum distance to match close object
#define DYNAMIC_OBJ_MATCH_MAX_FWHM				7.0
#define DYNAMIC_OBJ_MIN_DISTANCE_MATCH			6.0
#define DYNAMIC_OBJ_MAG_TOLERANCE				0.004
#define DYNAMIC_OBJ_KRONFACTOR_TOLERANCE		0.5
#define DYNAMIC_OBJ_FWHM_TOLERANCE				0.02
#define DYNAMIC_OBJ_FLUX_TOLERANCE				0.2
#define DYNAMIC_OBJ_FLUX_PRIORITY				40.0
#define DYNAMIC_OBJ_FWHM_PRIORITY				30.0
#define DYNAMIC_OBJ_KRONFACTOR_PRIORITY			30.0
#define DYNAMIC_OBJ_PATH_DEV_ANGLE_TOLERANCE	180.0
#define DEFAULT_DYNAMIC_OBJ_HIST_NORMALIZATION	10000.0

// local headers
#include "../sky/star.h"
#include "../match/misc.h"
#include "../observer/observer.h"

// external classes
class CAstroImage;
class CImageDb;
class CLogger;
class CSextractor;
class CSky;
class CWaitDialogStack;

// define image stack structure
typedef struct
{
	int image_id;
	double register_score;
	double angle_deg;
	double angle_rad;
	double scale;
	double shift_x;
	double shift_y;
	// properties: file name, time stamps, etc
	char	file_name[255];
	double	file_time;
	double	exim_time;
	// set pointers
	CAstroImage* p_image;
	// transformations
	TRANS* m_pStackTransRegister;
	TRANS* m_pStackBackTrans;
	TRANS* m_pStackNextTrans;

} ImageStack;

// define dynamic object path node structure
typedef struct
{
	int frame_id;
	int obj_id;
	double x;
	double y;

} PathNode;

// vector star list type
typedef vector <StarDef> typeStarList;
typedef vector <int> typeIntVect;
typedef vector <double> typeDoubleVect;
typedef vector <typeDoubleVect> typeDoubleMatrix;
typedef vector <PathNode> typePathNodeList;
typedef struct
{
	typePathNodeList vectNode;
	double path_score;

} PathData;

// class :: 
//////////////////////////////////
class CImageGroup
{

// data
public:
	// vector with my image objects
	std::vector<CAstroImage*> m_vectImageNode;
//	CAstroImage** m_vectImageNode;
	int m_nImageNode;
//	int m_nImageNodeAllocated;

	wxString m_strGroupNameId;
	wxString m_strGroupPath;

	// image database used
	CImageDb* m_pImageDb;
	// logger
	CLogger* m_pLogger;
	// used for stacking
	CWaitDialogStack* m_pWaitDialogStack;

	/////////////////////
	// group properties
	int m_nGroupType;
	int m_strGroupTitle;

	////////////
	// stack list
	vector <ImageStack> m_vectStack;
	int m_nStack;
	int m_nImageStackMethod;
	wxString m_strStackBiasFrame;
	wxString m_strStackDarkFrame;
	wxString m_strStackFlatFrame;
	double m_nStackFrameSigma;
	int m_StackFrameIteration;
	int m_nStackFrameBlurMethod;
	int m_bNewStackOutput;

	////////////////////
	// dynamic obj vars
	// :: track/paths vector
	vector <PathData> m_vectDynamicObjPath;
	int m_nDynamicObjPath;
//	int m_nDynamicObjPathAllocated;
	int m_nMinPathId;
	// other object dynamics minimum distance match tolerance
	double m_nDynamicObjMinDistMatch;
	// Object Dymanics Fwhm Tolerance
	double m_nObjDynamicsMagTol;
	// Object Dymanics Fwhm Tolerance
	double m_nObjDynamicsFwhmTol;
	// Object Dymanics Kron Factor Tolerance
	double m_nObjDynamicsKronFactorTol;
	// Object Dymanics Flux Tolerance
	double m_nObjDynamicsFluxTol;
	//  object dynamics path deviation angle tolerance
	double m_nPathDevAngleTolerance;
	// dynamic object use max fwhm to match
	double m_nDynamicObjMatchMaxFwhm;
	// histogram compare method
	int m_nHistCmpMethod;
	// histogram normalization
	double m_nObjHistogramNormalize;

	/////// prio to use ?
	// object dynamics fwhm procent priority
	double m_nObjDynamicsFwhmPrio;
	// object dynamics kron factor procent priority
	double m_nObjDynamicsKronFactorPrio;
	// object dynamics flux procent priority
	double m_nObjDynamicsFluxPrio;


	///////////////////////////
	// group hint variables
	// hint vars
	int m_nHintImgUseGroup;
	// the aprox field center position
	double m_nRelCenterRa;
	double m_nRelCenterDec;
	// the aprox field size
	double m_nRelFieldRa;
	double m_nRelFieldDec;
	// other hint vars
	int m_nHintOrigType;
	int m_nHintObjOrigType;
	int m_nHintObjName;
	wxString m_strHintObjId;
	int m_nHintOrigUnits;
	int m_nHintFieldType;
	// :: setup
	int m_nHintSetupId;
	// camera
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
	// field
	int m_nHintFieldFormat;
	// flags
	int m_bIsHint;
	int m_bIsChanged;
	int m_bRegistered;
	// other flags
	int m_bDontSaveOnDestroy;

// methods
public:

	// constructor/destructor
	CImageGroup( const wxString& strGroupNameId );
	~CImageGroup( );

	void ClearAll( );
	int Init( );

	int Load( );
	int Save( );

	wxString GetIniFilePath( ){ wxString ret=wxT(""); ret.Printf( wxT("%s%s%s"), DEFAULT_PATH_IMAGE_GROUP_LOAD_FILE, m_strGroupNameId, wxT(".ini") ); return(ret); }
	wxString GetIniFileName( ){ wxString ret=wxT(""); ret.Printf( wxT("%s%s"), m_strGroupNameId, wxT(".ini") ); return(ret); }

	int SetPath( const wxString& strGroupPath );
	void SetHint( double nRa, double nDec, double nWidth, double nHeight );

	int IsImageInStack( int nImageId );
	int GetImageNoById( int nImageId );
	int GetNewImageId( );
	int GetImageStackId( int nImageId );
	CAstroImage* GetImageByStackId( int nImageId );
	CAstroImage* FindImageByName( wxString& strImageName, wxString& strImageExt, int& nImgId );

	int SetMaxNoOfImages( int nImages );

	int AddAllImagesFromPath( int bRecursive=0 );
	int AddImage( const wxString& strImageFullPath, int bLog=1 );
	void DeleteImage( int nId );
	int AddImages( const wxString& strImages, int& nFirstImageId );

	// image sorting methods
	void SortImagesById( int bOrder );
	void SortImagesByName( int bOrder );
	void SortImagesByFileTime( int bOrder );
	// stack images sorting
	void SortStackImagesById( int bOrder );
	void SortStackImagesByName( int bOrder );
	void SortStackImagesByFileTime( int bOrder );

	// other image operations
	int SwapImagesPositionById( int nImageOneId, int nImageTwoId );
	int SwapStakImagesPositionById( int nImageOneId, int nImageTwoId );
	int RemoveImageByPosId( int nImageId );

	// detect all images in the group
	int Detect( CSextractor* pDetect );
	// match all images in the group
	int Match( CSky* pSky );
	// set hint for all images in the image
	int SetHintToAllImages( int nType );
	// initialize image stack
	void InitStack( );
	// register all images in the group
	int Register( );
	// aling all images in the group
	int Align( );

	///////////////////////////////
	// find dynamic objects - moving objects or objects which popup
	int FindDynamicObjects( );
	int AddDynamicObjectPath( );
	void ClearAllDynamicObjectPaths( );

	////////////////
	// stack methods
	int CombineImages( wxImage* pBaseImage, wxImage* pOperatorImage, int nMethod );
	int LoadStackImagesData( );
	void FreeStackImagesData( );
	int ExtractStackPixel( int x, int y, unsigned char* vectStackRed,
							unsigned char* vectStackGreen, unsigned char* vectStackBlue );
	int StackImages( void* pWaitDialogStack );
	int StackByMethod( wxImage* pStackImage, int w, int h, int nMethod );

	//////////////////
	// sort methods
	static bool AscCmpByImageId( ImageStack a, ImageStack b );
	static bool DesCmpByImageId( ImageStack a, ImageStack b );
	static bool AscCmpFileName( ImageStack a, ImageStack b );
	static bool DesCmpFileName( ImageStack a, ImageStack b );
	static bool AscCmpFileTimestamp( ImageStack a, ImageStack b );
	static bool DesCmpFileTimestamp( ImageStack a, ImageStack b );
	// sort dynamic objects paths
	static bool SortDescObjPaths( PathData a, PathData b );

};

#endif
