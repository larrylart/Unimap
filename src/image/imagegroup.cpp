////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

//system headers
#include <sys/timeb.h>
#include <ctime>
#include <cmath>
//#include <time>
//#include <iostream>

// opencv includes
#include "highgui.h"
#include "cv.h"
//#include "cvaux.h"

//////
// WX includes
#include "wx/wxprec.h"
#include <wx/regex.h>
#include <wx/dir.h>
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/tokenzr.h>

// elynx headers
#include <elx/image/ImageFileManager.h>

/////////
// other local includes
#include "astroimage.h"
//#include "imagearea.h"
#include "../config/mainconfig.h"
#include "../sky/sky.h"
#include "../sky/sky_finder.h"
#include "../sky/catalog_stars.h"
#include "../sky/catalog_dso.h"
#include "../util/func.h"
#include "../logger/logger.h"
#include "../unimap_worker.h"
#include "../unimap.h"
#include "../gui/frame.h"
#include "../gui/waitdialogmatch.h"
#include "../gui/waitdialogstack.h"
#include "../match/starmatch.h"
#define NOCTYPES
#include "../proc/sextractor.h"
#undef NOCTYPES
#include "imagedb.h"
// source file - inline
#include "../stack/stack_func.h"

// main header
#include "imagegroup.h"


////////////////////////////////////////////////////////////////////
// Method:		Constructor
// Class:		CImageGroup
// Purpose:		build my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
CImageGroup::CImageGroup( const wxString& strGroupNameId )
{
	m_strGroupNameId = strGroupNameId;

	// reset hint
	m_bIsHint = 0;
	m_nHintOrigType = 0;
	m_nHintObjOrigType = 0;
	m_nHintObjName = 0;
	m_strHintObjId = wxT("");
	m_nHintOrigUnits = 0;
	m_nHintFieldType = 0;
	m_nHintCameraBrand = 0;
	m_nHintCameraType = 0;
	m_nHintFocalLength = 0;
	m_nHintFieldFormat = 0;
	// other flags
	m_bIsChanged = 0;

	m_nImageNode = 0;
//	m_nImageNodeAllocated = 0;
	m_nStack = 0;
	m_bRegistered = 0;
	m_nImageStackMethod = IMAGE_STACK_METHOD_AVERAGE;
	m_nStackFrameSigma = DEFAULT_STACK_CCDRED_SIGMAS;
	m_StackFrameIteration = DEFAULT_STACK_CCDRED_ITER;
	m_nStackFrameBlurMethod = 0;

	m_pImageDb = NULL;
	m_vectStack.empty();

	m_strGroupPath = wxT("");
	m_strStackBiasFrame = wxT("");
	m_strStackDarkFrame = wxT("");
	m_strStackFlatFrame = wxT("");

	// allocate maximum no of images by default
//	SetMaxNoOfImages( 0 );

	m_bDontSaveOnDestroy = 0;
	m_bNewStackOutput = 0;

	// set dynamic objs flag
	m_nDynamicObjMinDistMatch = DYNAMIC_OBJ_MIN_DISTANCE_MATCH;
	m_nObjDynamicsMagTol = DYNAMIC_OBJ_MAG_TOLERANCE;
	m_nObjDynamicsFwhmTol = DYNAMIC_OBJ_FWHM_TOLERANCE;
	m_nObjDynamicsKronFactorTol = DYNAMIC_OBJ_KRONFACTOR_TOLERANCE;
	m_nObjDynamicsFluxTol = DYNAMIC_OBJ_FLUX_TOLERANCE;
	m_nPathDevAngleTolerance = DYNAMIC_OBJ_PATH_DEV_ANGLE_TOLERANCE;
	m_nDynamicObjMatchMaxFwhm = DYNAMIC_OBJ_MATCH_MAX_FWHM;
	m_nHistCmpMethod = CV_COMP_BHATTACHARYYA;
	m_nObjHistogramNormalize = DEFAULT_DYNAMIC_OBJ_HIST_NORMALIZATION;

	// priorities ?? yet to use ?
	m_nObjDynamicsFwhmPrio = DYNAMIC_OBJ_FWHM_PRIORITY;
	m_nObjDynamicsFluxPrio = DYNAMIC_OBJ_FLUX_PRIORITY;
	m_nObjDynamicsKronFactorPrio = DYNAMIC_OBJ_KRONFACTOR_PRIORITY;
	
	//	m_nDynamicObjPathAllocated = 10;
	m_nDynamicObjPath = 0;
	m_vectDynamicObjPath.clear();
	m_nMinPathId = -1;
}

////////////////////////////////////////////////////////////////////
// Method:		Destructor
// Class:		CImageGroup
// Purpose:		destroy my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
CImageGroup::~CImageGroup()
{	
	if( m_bDontSaveOnDestroy == 0 ) Save( );
	// and now call to clear all group data
	ClearAll( );
}

////////////////////////////////////////////////////////////////////
void CImageGroup::ClearAll( )
{
	int i = 0;

	// free image nodes
	for( i=0; i<m_vectImageNode.size(); i++ )
	{
		delete( m_vectImageNode[i] );
	}

	// free stack data
	for( i=0; i<m_vectStack.size(); i++ )
	{
		if( m_vectStack[i].m_pStackTransRegister ) free( m_vectStack[i].m_pStackTransRegister );
		if( m_vectStack[i].m_pStackBackTrans ) free( m_vectStack[i].m_pStackBackTrans );
		if( m_vectStack[i].m_pStackNextTrans ) free( m_vectStack[i].m_pStackNextTrans );
		m_vectStack[i].m_pStackTransRegister = NULL;
		m_vectStack[i].m_pStackBackTrans = NULL;
		m_vectStack[i].m_pStackNextTrans = NULL;
	}
	m_vectStack.clear();

	m_vectImageNode.clear();
	m_nImageNode = 0;
	m_nStack = 0;

	// clear al ldynamic object paths
	ClearAllDynamicObjectPaths( );
}

////////////////////////////////////////////////////////////////////
// Method:		SetMaxNoOfImages
// Class:		CImageGroup
// Purpose:		initialize my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CImageGroup::SetMaxNoOfImages( int nImages )
{
//	m_nImageNodeAllocated = nImages;
//	m_vectImageNode = (CAstroImage**) calloc( nImages + 1, sizeof( CAstroImage* ) );

	m_vectImageNode.resize( nImages );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		Init
// Class:		CImageGroup
// Purpose:		initialize my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CImageGroup::Init( )
{		
	int i = 0;

	// attempt to load ini
	if( Load( ) == 0 ) return( 0 );
	
//	LoadAreas( );

	// then initialize my nodes
	for( i=0; i<m_vectImageNode.size(); i++ )
	{
		// init removed - was empty
//		m_vectImageNode[i]->Init();
		// larry :: and here we load the profile for the image ?
		// in fact to this later whe you actualy load the image
//		m_vectImageNode[i]->LoadProfile( );
	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		SetPath
// Class:		CImageGroup
// Purpose:		initialize my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CImageGroup::SetPath( const wxString& strGroupPath )
{		
	m_strGroupPath = strGroupPath;

	return( 1 );
}


////////////////////////////////////////////////////////////////////
// Method:		Load
// Class:		CImageGroup
// Purpose:		load my group configuration
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CImageGroup::Load( )
{	
	FILE* pFile = NULL;
	wxString strFile;
	wxChar strLine[2000];
	wxChar strTmp[2000];
	int bFoundImage = 0;
	int bFoundHint = 0;
	int bFoundProperties = 0;
	int bNewLine = 0;
	int bFoundObjPaths = 0;
	long nLineSize = 0;
	// other vars
	long nVarLong = 0;
	double nVarFloat = 0.0;
	int i = 0;

	// properties regex
//	wxRegEx rePropRegistered = wxT( "^Registered=([0-9]+)" );
//	wxRegEx rePropStackMethod = wxT( "^StackMethod=([0-9]+)" );
//	wxRegEx rePropStackSigma = wxT( "^StackSigma=([0-9\\+\\.\\-]+)" );
//	wxRegEx rePropStackIterations = wxT( "^StackIterations=([0-9]+)" );
//	wxRegEx rePropBiasImage = wxT( "^BiasImage=([a-zA-Z0-9\\(\\)\\ \\_\\B\\.[:punct:]]+)" );
//	wxRegEx rePropDarkImage = wxT( "^DarkImage=([a-zA-Z0-9\\(\\)\\ \\_\\B\\.[:punct:]]+)" );
//	wxRegEx rePropFlatImage = wxT( "^FlatImage=([a-zA-Z0-9\\(\\)\\ \\_\\B\\.[:punct:]]+)" );
//	wxRegEx rePropStackBlurMethod = wxT( "^StackBlurMethod=([0-9]+)" );
	// :: dynamic object properties
//	wxRegEx rePropObjHistCmpMethod = wxT( "^DynamicObjHistCmpMethod=([0-9]+)" );
//	wxRegEx rePropObjHistNorm = wxT( "^DynamicObjHistNorm=([0-9\\+\\.\\-]+)" );
//	wxRegEx rePropDynamicObjMatchMaxFwhm = wxT( "^DynamicObjMatchMaxFwhm=([0-9\\+\\.\\-]+)" );
//	wxRegEx rePropDynamicObjMinDistMatch = wxT( "^DynamicObjMinDistMatch=([0-9\\+\\.\\-]+)" );
//	wxRegEx rePropObjDynamicsMagTol = wxT( "^ObjDynamicsMagTol=([0-9\\+\\.\\-]+)" );
//	wxRegEx rePropObjDynamicsKronFactorTol = wxT( "^ObjDynamicsKronFactorTol=([0-9\\+\\.\\-]+)" );
//	wxRegEx rePropObjDynamicsFwhmTol = wxT( "^ObjDynamicsFwhmTol=([0-9\\+\\.\\-]+)" );
//	wxRegEx rePropPathDevAngleTolerance = wxT( "^PathDevAngleTolerance=([0-9\\+\\.\\-]+)" );

	///////////////////////
	// images regex
//	wxRegEx reImageLabel = wxT( "^\\{([a-zA-Z0-9\\_[:punct:]]+)\\}" );
//	wxRegEx reImageId = wxT( "^id=([0-9]+)" );
//	wxRegEx reImagePath = wxT( "^path=([a-zA-Z0-9\\(\\)\\ \\_\\B\\.[:punct:]]+)" );
//	wxRegEx reImageFileTime = wxT( "^filetime=([0-9]+)" );
	// :: stack regex %d,%lf,%lf,%lf,%lf,%lf
//	wxRegEx reImageStack = wxT( "^stack=([0-9]+),([0-9\\+\\.\\-]+),([0-9\\+\\.\\-]+),([0-9\\+\\.\\-]+),([0-9\\+\\.\\-]+),([0-9\\+\\.\\-]+)" );

	///////////////////
	// :: REGEX :: HINT
//	wxRegEx reHintImgUseGroup = wxT( "^ImgUseGroup=([0-9]+)" );
	// regex for hints
//	wxRegEx reHintOrigRa = wxT( "^RelOrigRa=([0-9\\+\\.\\-]+)" );
//	wxRegEx reHintOrigDec = wxT( "^RelOrigDec=([0-9\\+\\.\\-]+)" );
//	wxRegEx reHintFieldWidth = wxT( "^RelFieldWidth=([0-9\\+\\.\\-]+)" );
//	wxRegEx reHintFieldHeight = wxT( "^RelFieldHeight=([0-9\\+\\.\\-]+)" );
//	wxRegEx reHintOrigType = wxT( "^OrigType=([0-9]+)" );
//	wxRegEx reHintObjOrigType = wxT( "^ObjOrigType=([0-9]+)" );
//	wxRegEx reHintObjName = wxT( "^ObjName=([0-9]+)" );
//	wxRegEx reHintObjId = wxT( "^ObjId=([0-9a-zA-Z\\ \\-]+)" );
//	wxRegEx reHintOrigUnits = wxT( "^OrigUnits=([0-9]+)" );
//	wxRegEx reHintFieldType = wxT( "^FieldType=([0-9]+)" );
	// hardware setup
//	wxRegEx reHintHardwareSetup = wxT( "^HardwareSetup=(.+)" );
	// camera
//	wxRegEx reHintCamera = wxT( "^Camera=(.+)" );
	// sensor size
//	wxRegEx reHintSensorWidth = wxT( "^SensorWidth=([0-9\\.]+)" );
//	wxRegEx reHintSensorHeight = wxT( "^SensorHeight=([0-9\\.]+)" );
	// tlens
//	wxRegEx reHintTLens = wxT( "^TLens=(.+)" );
	// system focal length
//	wxRegEx reHintFocalLength = wxT( "^FocalLength=([0-9\\.]+)" );
//	wxRegEx reHintFieldFormat = wxT( "^FieldFormat=([0-9]+)" );

	// path regex 
	wxRegEx reDynamicObjectPath = wxT( "^Path\\[([0-9]+)\\]=([0-9\\+\\.\\-]+),([0-9\\;\\,\\+\\.\\-]+)" );
	wxRegEx reDynamicObjectPathNode= wxT( "([0-9]+),([0-9]+),([0-9\\+\\.\\-]+),([0-9\\+\\.\\-]+)" );
	// trans regex
	wxRegEx reMatchNextTrans = wxT( "^NextTrans=(.+)$" );
	wxRegEx reMatchBackTrans = wxT( "^BackTrans=(.+)$" );
	wxRegEx reMatchTransRegister = wxT( "^RegTrans=(.+)$" );

	//////////////////
	// reset group counters - allocations etc
	ClearAll( );
	long nStackId = -1;


	// set group ini file name
	strFile.Printf( wxT("%s%s%s"), DEFAULT_PATH_IMAGE_GROUP_LOAD_FILE, m_strGroupNameId, wxT(".ini") );
	// open file to read
	pFile = wxFopen( strFile, wxT("r") );

	// check if there is any configuration to load
	if( !pFile ) return( -1 );

	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// clear thy buffers
//		memset( strLine, 0, 2000*sizeof(wxChar) );

		// get one line out of the config file
		wxFgets( strLine, 2000, pFile );
		nLineSize = wxStrlen( strLine );

		// if a minimum of 3 not read skip line
		if( nLineSize <= 3 ) continue;
		// if comment skip
		if( strLine[0] == '#' ) continue;

//		wxString strLine = strLine;

		// set new line flag
		bNewLine = 1;

		// :: first check for label indicators [] to speed up things
		if( strLine[0] == '[' && strLine[nLineSize-2] == ']' )
		{
			bNewLine = 0;
			// check for properties label
			if( wxStrncmp( strLine, wxT("[Properties]"), 12 ) == 0 )
			{
				bFoundProperties = 1;
				
				// set all other to zero
				bFoundHint = 0;
				bFoundObjPaths = 0;

			// check for hint label
			} else if( wxStrncmp( strLine, wxT("[Hint]"), 6 ) == 0 )
			{
				bFoundHint = 1;
				// also set local that we have a hint
				m_bIsHint = 1;
				// set all other to zero
				bFoundProperties = 0;
				bFoundObjPaths = 0;

			// :: check for image name label
			} else if( wxStrncmp( strLine, wxT("[DynamicObjectsPath]"), 20 ) == 0 )
			{
				bFoundObjPaths = 1;
				// reset the other stuff to zero
				bFoundHint = 0;
				bFoundProperties = 0;
				bFoundImage = 0;
			}

		// :: check for image name label
		} else if( strLine[0] == '{' && strLine[nLineSize-2] == '}' )
		{
			// reset the other stuff to zero
			bFoundObjPaths = 0;
			bFoundHint = 0;
			bFoundProperties = 0;
			bFoundImage = 0;

			// get image name
			wxSscanf( strLine, wxT("{%[^}]}\n"), strTmp );
			wxString strFileName = strTmp; //reImageLabel.GetMatch(strLine, 1 );

			// exception - if file doesn't exit skip - need full path here 
			//if( !wxFileExists(strFileName) ) continue;

			// extract image path/name/ext
			wxString strImagePath = wxT("");
			wxString strImageName = wxT("");
			wxString strImageExt = wxT("");
			wxSplitPath( strFileName, &strImagePath, &strImageName, &strImageExt );

			// first allocate more memory
//			m_nImageNodeAllocated = m_nImageNode;
//			m_vectImageNode = (CAstroImage**) realloc( m_vectImageNode, (m_nImageNode+1) * sizeof( CAstroImage* ) );

			// create image node
			//m_vectImageNode[m_nImageNode] = new CAstroImage( this, m_strGroupPath, strImageName, strImageExt );
			CAstroImage* pImage = new CAstroImage( this, m_strGroupPath, strImageName, strImageExt );
			pImage->m_pImageGroup = this;
			// set the id to current image node index - latter to load a specific id if any
			pImage->m_nId = m_nImageNode;

			// add to vector
			m_vectImageNode.push_back( pImage );

			// links ????
//			if( m_nImageNode > 0 )
//			{
//				// set next image node for previous image node
//				pLastImageNode->m_pNextImageNode = m_vectImageNode[m_nImageNode];
//				pLastImageNode->SetImageLink( m_vectImageNode[m_nImageNode] );
//				pLastImageNode->SetImageLink( m_vectImageNode[m_nImageNode] );
//				pLastImageNode->SetImageLink( m_vectImageNode[m_nImageNode] );

				// set previous node for current node
//				m_vectImageNode[m_nImageNode]->m_pPreviousImageNode = pLastImageNode;
//				m_vectImageNode[m_nImageNode]->SetImageLink( pLastImageNode );
//			}
			
//			pLastImageNode = m_vectImageNode[m_nImageNode];

			m_nImageNode = m_vectImageNode.size();
			nStackId = -1;

			bFoundImage = 1;

		////////////////////////////////////////////////////////////////////
		// then check for label properties
		} else if( bFoundHint && bNewLine ) 
		{
			// here we check by the individual hint labels
			if( wxStrncmp( strLine, wxT("ImgUseGroup="), 12 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("ImgUseGroup=%d\n"), &m_nHintImgUseGroup ) )
					m_nHintImgUseGroup = 0;

			} else if( wxStrncmp( strLine, wxT("RelOrigRa="), 10 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("RelOrigRa=%lf\n"), &m_nRelCenterRa ) )
					m_nRelCenterRa = 0.0;

			} else if( wxStrncmp( strLine, wxT("RelOrigDec="), 11 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("RelOrigDec=%lf\n"), &m_nRelCenterDec ) )
					m_nRelCenterDec = 0.0;

			} else if( wxStrncmp( strLine, wxT("RelFieldWidth="), 14 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("RelFieldWidth=%lf\n"), &m_nRelFieldRa ) )
					m_nRelFieldRa = 0.0;

			} else if( wxStrncmp( strLine, wxT("RelFieldHeight="), 15 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("RelFieldHeight=%lf\n"), &m_nRelFieldDec ) )
					m_nRelFieldDec = 0.0;

			} else if( wxStrncmp( strLine, wxT("OrigType="), 9 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("OrigType=%d\n"), &m_nHintOrigType ) )
					m_nHintOrigType = 0;

			} else if( wxStrncmp( strLine, wxT("ObjOrigType="), 12 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("ObjOrigType=%d\n"), &m_nHintObjOrigType ) )
					m_nHintObjOrigType = 0;

			} else if( wxStrncmp( strLine, wxT("ObjName="), 8 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("ObjName=%d\n"), &m_nHintObjName ) )
					m_nHintObjName = 0;

			} else if( wxStrncmp( strLine, wxT("ObjId="), 6 ) == 0 )
			{
				if( wxSscanf( strLine, wxT("ObjId=%[^\n]\n"), strTmp ) )
					m_strHintObjId = strTmp; //reHintObjId.GetMatch(strLine, 1 );
				else
					m_strHintObjId = wxT("");
			
			} else if( wxStrncmp( strLine, wxT("OrigUnits="), 10 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("OrigUnits=%d\n"), &m_nHintOrigUnits ) )
					m_nHintOrigUnits = 0;

			}  else if( wxStrncmp( strLine, wxT("FieldType="), 10 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("FieldType=%d\n"), &m_nHintFieldType ) )
					m_nHintFieldType = 0;

			// :: HARDWARE SETUP
			}  else if( wxStrncmp( strLine, wxT("HardwareSetup="), 14 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("HardwareSetup=%d\n"), &m_nHintSetupId ) )
					m_nHintSetupId = 0;

			///////////
			// :: CAMERA
			} else if( wxStrncmp( strLine, wxT("Camera="), 7 ) == 0 )
			{
				if( wxSscanf( strLine, wxT("Camera=%[^\n]\n"), strTmp ) )
				{
					int nCamSource=0, nCamType=0, nCamBrand=0, nCamName=0;
					// pharse string with scanf
					if( wxSscanf( strTmp, wxT("%d,(%d:%d:%d)"),
						&nCamSource, &nCamType, &nCamBrand, &nCamName ) )
					{
						m_nHintCameraSource = nCamSource;
						m_nHintCameraType = nCamType;
						m_nHintCameraBrand = nCamBrand;
						m_nHintCameraName = nCamName;
					}

				}

			// :: SENSOR SIZE
			} else if( wxStrncmp( strLine, wxT("SensorWidth="), 12 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("SensorWidth=%lf\n"), &m_nHintSensorWidth ) )
					m_nHintSensorWidth = 0.0;

			} else if( wxStrncmp( strLine, wxT("SensorHeight="), 13 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("SensorHeight=%lf\n"), &m_nHintSensorHeight ) )
					m_nHintSensorHeight = 0.0;

			///////////
			// HINT :: TELESCOPE/LENS
			} else if( wxStrncmp( strLine, wxT("TLens="), 6 ) == 0 )
			{
				if( wxSscanf( strLine, wxT("TLens=%[^\n]\n"), strTmp ) )
				{
					int nTLensSource=0, nLensType=0, nLensOptics=0, nLensBrand=0, nLensName=0,
						bBarlow=0, nBarlowLensBrand=0, nBarlowLensName=0, 
						bFocalReducer=0, nFocalReducerBrand=0, nFocalReducerName=0,
						bEyepiece=0, nEyepieceType=0, nEyepieceBrand=0, nEyepieceName=0,
						bCameraLens=0, nCameraLensOptics=0, nCameraLensBrand=0, nCameraLensName=0;

					// pharse string with scanf
					if( wxSscanf( strTmp, 
						wxT("%d,TL(%d:%d:%d:%d),BL(%d:%d:%d),FR(%d:%d:%d),EP(%d:%d:%d:%d),CL(%d:%d:%d:%d)"),
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
				}

			// :: SYSTEM FOCAL LENGTH
			} else if( wxStrncmp( strLine, wxT("FocalLength="), 12 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("FocalLength=%lf\n"), &m_nHintFocalLength ) )
					m_nHintFocalLength = 0.0;

			}  else if( wxStrncmp( strLine, wxT("FieldFormat="), 12 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("FieldFormat=%d\n"), &m_nHintFieldFormat ) )
					m_nHintFieldFormat = 0;
			}

		// :: get dynamic objects paths
		} else if( bFoundObjPaths && bNewLine ) 
		{	
			// here we check for objects paths
			if( reDynamicObjectPath.Matches( strLine ) )
			{
				int nPathId = 0;
				PathData newPath;

				// path id
				if( !reDynamicObjectPath.GetMatch(strLine, 1 ).ToLong( &nVarLong ) )
					nPathId = 0;
				else
					nPathId = (int) nVarLong;
				// path score
				if( !reDynamicObjectPath.GetMatch(strLine, 2 ).ToDouble( &(newPath.path_score) ) )
					newPath.path_score = 0.0;
				// path nodes
				wxString strNodes = reDynamicObjectPath.GetMatch( strLine, 3 );

				// add new path
				m_vectDynamicObjPath.push_back( newPath );
				nPathId = m_vectDynamicObjPath.size()-1;
				m_nDynamicObjPath++;

				// extract individual nodes
				wxStringTokenizer tkz( strNodes, wxT(";") );
				while ( tkz.HasMoreTokens() )
				{
					// get next node from input string
					wxString strNode = tkz.GetNextToken();
					// check if I get a match
					if( reDynamicObjectPathNode.Matches( strNode ) )
					{
						PathNode pathNode;

						// get frame id
						if( !reDynamicObjectPathNode.GetMatch( strNode, 1 ).ToLong( &nVarLong ) )
							pathNode.frame_id = 0;
						else
							pathNode.frame_id = (int) nVarLong;
						// get object id
						if( !reDynamicObjectPathNode.GetMatch( strNode, 2 ).ToLong( &nVarLong ) )
							pathNode.obj_id = 0;
						else
							pathNode.obj_id = (int) nVarLong;
						// get object x
						if( !reDynamicObjectPathNode.GetMatch( strNode, 3 ).ToDouble( &(pathNode.x) ) )
							pathNode.x = 0.0;
						// get object x
						if( !reDynamicObjectPathNode.GetMatch( strNode, 4 ).ToDouble( &(pathNode.y) ) )
							pathNode.y = 0.0;
						///////////
						// add node to path
						m_vectDynamicObjPath[nPathId].vectNode.push_back( pathNode );
					}
				}
			}

		// :: image properties
		} else if( bFoundProperties && bNewLine ) 
		{	
			// check properties
			if( wxStrncmp( strLine, wxT("Registered="), 11 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Registered=%d\n"), &m_bRegistered ) )
					m_bRegistered = 0;

			} else if( wxStrncmp( strLine, wxT("StackMethod="), 12 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("StackMethod=%d\n"), &m_nImageStackMethod ) )
					m_nImageStackMethod = 0;

			} else if( wxStrncmp( strLine, wxT("BiasImage="), 10 ) == 0 )
			{
				if( wxSscanf( strLine, wxT("BiasImage=%[^\n]\n"), strTmp ) )
					m_strStackBiasFrame = strTmp; //rePropBiasImage.GetMatch( strLine, 1 );
				else
					m_strStackBiasFrame = wxT("");

			} else if( wxStrncmp( strLine, wxT("DarkImage="), 10 ) == 0 )
			{
				if( wxSscanf( strLine, wxT("DarkImage=%[^\n]\n"), strTmp ) )
					m_strStackDarkFrame = strTmp; //rePropDarkImage.GetMatch( strLine, 1 );
				else
					m_strStackDarkFrame = wxT("");

			} else if( wxStrncmp( strLine, wxT("FlatImage="), 10 ) == 0 )
			{
				if( wxSscanf( strLine, wxT("FlatImage=%[^\n]\n"), strTmp ) )
					m_strStackFlatFrame = strTmp; //rePropFlatImage.GetMatch( strLine, 1 );
				else
					m_strStackFlatFrame = wxT("");

			} else if( wxStrncmp( strLine, wxT("StackSigma="), 11 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("StackSigma=%lf\n"), &m_nStackFrameSigma ) )
					m_nStackFrameSigma = DEFAULT_STACK_CCDRED_SIGMAS;

			} else if( wxStrncmp( strLine, wxT("StackIterations="), 16 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("StackIterations=%d\n"), &m_StackFrameIteration ) )
					m_StackFrameIteration = DEFAULT_STACK_CCDRED_ITER;

			} else if( wxStrncmp( strLine, wxT("StackBlurMethod="), 16 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("StackBlurMethod=%d\n"), &m_nStackFrameBlurMethod ) )
					m_nStackFrameBlurMethod = 0;

			////////////////////
			// dynamic object setup
			} else if( wxStrncmp( strLine, wxT("DynamicObjHistCmpMethod="), 24 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("DynamicObjHistCmpMethod=%d\n"), &m_nHistCmpMethod ) )
					m_nHistCmpMethod = CV_COMP_BHATTACHARYYA;

			} else if( wxStrncmp( strLine, wxT("DynamicObjHistNorm="), 19 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("DynamicObjHistNorm=%lf\n"), &m_nObjHistogramNormalize ) )
					m_nObjHistogramNormalize = DEFAULT_DYNAMIC_OBJ_HIST_NORMALIZATION;

			} else if( wxStrncmp( strLine, wxT("DynamicObjMatchMaxFwhm="), 23 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("DynamicObjMatchMaxFwhm=%lf\n"), &m_nDynamicObjMatchMaxFwhm ) )
					m_nDynamicObjMatchMaxFwhm = DYNAMIC_OBJ_MATCH_MAX_FWHM;

			} else if( wxStrncmp( strLine, wxT("DynamicObjMinDistMatch="), 23 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("DynamicObjMinDistMatch=%lf\n"), &m_nDynamicObjMinDistMatch ) )
					m_nDynamicObjMinDistMatch = DYNAMIC_OBJ_MIN_DISTANCE_MATCH;

			} else if( wxStrncmp( strLine, wxT("ObjDynamicsMagTol="), 18 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("ObjDynamicsMagTol=%lf\n"), &m_nObjDynamicsMagTol ) )
					m_nObjDynamicsMagTol = DYNAMIC_OBJ_MAG_TOLERANCE;

			} else if( wxStrncmp( strLine, wxT("ObjDynamicsKronFactorTol="), 25 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("ObjDynamicsKronFactorTol=%lf\n"), &m_nObjDynamicsKronFactorTol ) )
					m_nObjDynamicsKronFactorTol = DYNAMIC_OBJ_KRONFACTOR_TOLERANCE;

			} else if( wxStrncmp( strLine, wxT("ObjDynamicsFwhmTol="), 19 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("ObjDynamicsFwhmTol=%lf\n"), &m_nObjDynamicsFwhmTol ) )
					m_nObjDynamicsFwhmTol = DYNAMIC_OBJ_FWHM_TOLERANCE;

			} else if( wxStrncmp( strLine, wxT("PathDevAngleTolerance="), 22 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("PathDevAngleTolerance=%lf\n"), &m_nPathDevAngleTolerance ) )
					m_nPathDevAngleTolerance = DYNAMIC_OBJ_PATH_DEV_ANGLE_TOLERANCE;
			}

		// :: image prop :: id
		} else if( bFoundImage == 1 && wxStrncmp( strLine, wxT("id="), 3 ) == 0  )
		{
			if( !wxSscanf( strLine, wxT("id=%d\n"), &(m_vectImageNode[m_nImageNode-1]->m_nId) ) )
					m_vectImageNode[m_nImageNode-1]->m_nId = 0;

			// get/set image id
			//if( reImageId.GetMatch(strLine, 1 ).ToLong( &nVarLong ) )
			//	m_vectImageNode[m_nImageNode-1]->m_nId = (int) nVarLong;

		// :: image prop :: path
		} else if( bFoundImage == 1 && wxStrncmp( strLine, wxT("path="), 5 ) == 0  )
		{
			// get mage path
			wxSscanf( strLine, wxT("path=%[^\n]\n"), strTmp );
			wxString strImagePath = strTmp; //reImagePath.GetMatch(strLine, 1 );
			m_vectImageNode[m_nImageNode-1]->SetPath( strImagePath );

		// :: imape prop :: file time 
		} else if( bFoundImage == 1 && wxStrncmp( strLine, wxT("filetime="), 9 ) == 0  )
		{
			if( !wxSscanf( strLine, wxT("filetime=%lf\n"), &nVarFloat ) )
			{
				m_vectImageNode[m_nImageNode-1]->m_nFileTime = 0;

			} else
			{
				time_t fileTime = (time_t) nVarFloat;
				m_vectImageNode[m_nImageNode-1]->m_nFileTime = fileTime;
				m_vectImageNode[m_nImageNode-1]->m_nObsDateTime.Set( fileTime );
			}
		
		///////////////////////
		// :: image prop :: stack info
		} else if( bFoundImage == 1 && wxStrncmp( strLine, wxT("stack="), 6 ) == 0  )
		{
			double register_score=0.0, angle_rad=0.0, scale=0.0, shift_x=0.0, shift_y=0.0;
			// - if valid stack image id
			//if( reImageStack.GetMatch(strLine, 1 ).ToLong( &nVarLong ) )
			if( wxSscanf( strLine, wxT("stack=%d,%lf,%lf,%lf,%lf,%lf\n"), &nVarLong,
							&register_score, &angle_rad, &scale, &shift_x, &shift_y ) )
			{
				nStackId = nVarLong;

				// resize to 1000 max size  to fit - should use this the number of images from properties
				//if( m_vectStack.size() <= 0 ) m_vectStack.resize( 1000 );
				// add +1 - way better
				if( nStackId >= m_vectStack.size() ) m_vectStack.resize( nStackId + 1 );
				// set stack to image id
				m_vectStack[nStackId].image_id = m_vectImageNode[m_nImageNode-1]->m_nId;
				// set pointer to image
				m_vectStack[nStackId].p_image = m_vectImageNode[m_nImageNode-1];

				// set my vars
				m_vectStack[nStackId].register_score = register_score;
				m_vectStack[nStackId].angle_rad = angle_rad;
				m_vectStack[nStackId].scale = scale;
				m_vectStack[nStackId].shift_x = shift_x;
				m_vectStack[nStackId].shift_y = shift_y;

				// - register_score
//				if( !reImageStack.GetMatch(strLine, 2 ).ToDouble( &nVarFloat ) )
//					m_vectStack[nStackId].register_score = 0.0;
//				else
//					m_vectStack[nStackId].register_score = nVarFloat;
				// - angle_rad
//				if( !reImageStack.GetMatch(strLine, 3 ).ToDouble( &nVarFloat ) )
//					m_vectStack[nStackId].angle_rad = 0.0;
//				else
//					m_vectStack[nStackId].angle_rad = nVarFloat;
				// - scale
//				if( !reImageStack.GetMatch(strLine, 4 ).ToDouble( &nVarFloat ) )
//					m_vectStack[nStackId].scale = 0.0;
//				else
//					m_vectStack[nStackId].scale = nVarFloat;
				// - shift_x
//				if( !reImageStack.GetMatch(strLine, 5 ).ToDouble( &nVarFloat ) )
//					m_vectStack[nStackId].shift_x = 0.0;
//				else
//					m_vectStack[nStackId].shift_x = nVarFloat;
				// - shift_y
//				if( !reImageStack.GetMatch(strLine, 6 ).ToDouble( &nVarFloat ) )
//					m_vectStack[nStackId].shift_y = 0.0;
//				else
//					m_vectStack[nStackId].shift_y = nVarFloat;

				// set trans pointers to null for now
				m_vectStack[nStackId].m_pStackTransRegister = NULL;
				m_vectStack[nStackId].m_pStackBackTrans = NULL;
				m_vectStack[nStackId].m_pStackNextTrans = NULL;

				// increment stack counter
				m_nStack++;
			}

		// :: next trans
		} else if( reMatchNextTrans.Matches( strLine ) && bFoundImage == 1 )
		{
			if( nStackId < 0 ) continue;
			// :: allocate
			m_vectStack[nStackId].m_pStackNextTrans = (TRANS*) malloc( sizeof(TRANS) );
			// :: set
			GetTransFromString( reMatchNextTrans.GetMatch(strLine, 1 ).ToAscii(),
									m_vectStack[nStackId].m_pStackNextTrans );

		// :: back trans
		} else if( reMatchBackTrans.Matches( strLine ) && bFoundImage == 1 )
		{
			if( nStackId < 0 ) continue;
			// :: allocate
			m_vectStack[nStackId].m_pStackBackTrans = (TRANS*) malloc( sizeof(TRANS) );
			// :: set
			GetTransFromString( reMatchBackTrans.GetMatch(strLine, 1 ).ToAscii(),
									m_vectStack[nStackId].m_pStackBackTrans );

		// :: register trans
		} else if( reMatchTransRegister.Matches( strLine ) && bFoundImage == 1 )
		{
			if( nStackId < 0 ) continue;
			// :: allocate
			m_vectStack[nStackId].m_pStackTransRegister = (TRANS*) malloc( sizeof(TRANS) );
			// :: set
			GetTransFromString( reMatchTransRegister.GetMatch(strLine, 1 ).ToAscii(),
									m_vectStack[nStackId].m_pStackTransRegister );

		}

	}

	// close file
	fclose( pFile );

	// hack to reduce pre-stack size to normal todo:: move stack section separately
//	m_vectStack.resize( m_nStack+1 );

	//////////////////////////////
	// init other group images details if not what ??? todo: clarify if this is needed
	for( i=0; i<m_vectImageNode.size(); i++ )
	{
		// get file time if any
		if( m_vectImageNode[i]->m_nFileTime <= 0 )
		{
			m_vectImageNode[i]->GetImageFileInfo( );
		}
	}


	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		Save
// Class:		CImageGroup
// Purpose:		save my group configuration
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CImageGroup::Save( )
{	
	FILE* pFile = NULL;
	wxString strFile;
	int i=0, j=0, nStackId=-1;

	strFile.Printf( wxT("%s%s%s"), DEFAULT_PATH_IMAGE_GROUP_LOAD_FILE, m_strGroupNameId, wxT(".ini") );

	pFile = wxFopen( strFile, wxT("w") );

	// check if there is any configuration to load
	if( !pFile ) return( -1 );

	///////////////////////////////
	// write properties
	fprintf( pFile, "[Properties]\n" );
	fprintf( pFile, "Registered=%d\n",  m_bRegistered );
	fprintf( pFile, "StackMethod=%d\n",  m_nImageStackMethod );
	wxFprintf( pFile, wxT("BiasImage=%s\n"),  m_strStackBiasFrame );
	wxFprintf( pFile, wxT("DarkImage=%s\n"),  m_strStackDarkFrame );
	wxFprintf( pFile, wxT("FlatImage=%s\n"),  m_strStackFlatFrame );
	fprintf( pFile, "StackSigma=%.15f\n",  m_nStackFrameSigma );
	fprintf( pFile, "StackIterations=%d\n",  m_StackFrameIteration );
	fprintf( pFile, "StackBlurMethod=%d\n",  m_nStackFrameBlurMethod );
	// :: dynamic properties
	fprintf( pFile, "DynamicObjHistCmpMethod=%d\n",  m_nHistCmpMethod );
	fprintf( pFile, "DynamicObjHistNorm=%.10f\n",  m_nObjHistogramNormalize );
	fprintf( pFile, "DynamicObjMatchMaxFwhm=%.10f\n",  m_nDynamicObjMatchMaxFwhm );
	fprintf( pFile, "DynamicObjMinDistMatch=%.10f\n",  m_nDynamicObjMinDistMatch );
	fprintf( pFile, "ObjDynamicsMagTol=%.10f\n",  m_nObjDynamicsMagTol );
	fprintf( pFile, "ObjDynamicsKronFactorTol=%.10f\n",  m_nObjDynamicsKronFactorTol );
	fprintf( pFile, "ObjDynamicsFwhmTol=%.10f\n",  m_nObjDynamicsFwhmTol );
	fprintf( pFile, "PathDevAngleTolerance=%.10f\n",  m_nPathDevAngleTolerance );
	// add new line at the end 
	fprintf( pFile, "\n" );

	///////////////////////////////
	// write hints
	if( m_bIsHint )
	{
		fprintf( pFile, "[Hint]\n" );
		fprintf( pFile, "ImgUseGroup=%d\n",  m_nHintImgUseGroup );
		fprintf( pFile, "RelOrigRa=%f\n",  m_nRelCenterRa );
		fprintf( pFile, "RelOrigDec=%f\n",  m_nRelCenterDec );
		fprintf( pFile, "RelFieldWidth=%f\n", m_nRelFieldRa );
		fprintf( pFile, "RelFieldHeight=%f\n", m_nRelFieldDec );
		// and all the other vars
		fprintf( pFile, "OrigType=%d\n", m_nHintOrigType );
		fprintf( pFile, "ObjOrigType=%d\n", m_nHintObjOrigType );
		fprintf( pFile, "ObjName=%d\n", m_nHintObjName );
		wxFprintf( pFile, wxT("ObjId=%s\n"), m_strHintObjId );
		fprintf( pFile, "OrigUnits=%d\n", m_nHintOrigUnits );
		fprintf( pFile, "FieldType=%d\n", m_nHintFieldType );
		// setup
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

	////////////////////////
	// write groups images info
	for( i=0; i<m_vectImageNode.size(); i++ )
	{
		// save image header/name
		wxFprintf( pFile, wxT("{%s.%s}\n"), m_vectImageNode[i]->m_strImageName, 
									m_vectImageNode[i]->m_strImageExt );
		// save image id
		fprintf( pFile, "id=%d\n", (int) m_vectImageNode[i]->m_nId );
		// save path	
		wxFprintf( pFile, wxT("path=%s\n"), m_vectImageNode[i]->m_strImagePath );
		// save file time stamp
		fprintf( pFile, "filetime=%d\n", m_vectImageNode[i]->m_nFileTime );

		///////////////////////////////////////
		// STACK :: save stack info
		// :: get element in the stack vector
		nStackId = GetImageStackId( m_vectImageNode[i]->m_nId );
		// if there is stacking info
		if( nStackId >= 0 )
		{
			// save here stack id - which I will use when load to do m_vectStack[nStackId]
			fprintf( pFile, "stack=%d,%lf,%lf,%lf,%lf,%lf\n", nStackId,
							m_vectStack[nStackId].register_score,
							m_vectStack[nStackId].angle_rad, 
							m_vectStack[nStackId].scale, 
							m_vectStack[nStackId].shift_x,
							m_vectStack[nStackId].shift_y );

			// write registrations trans info
			if( m_vectStack[nStackId].m_pStackTransRegister != NULL )
				WriteTransToFile( pFile, m_vectStack[nStackId].m_pStackTransRegister, "RegTrans=" );
			// write trans next info
			if( m_vectStack[nStackId].m_pStackNextTrans != NULL )
				WriteTransToFile( pFile, m_vectStack[nStackId].m_pStackNextTrans, "NextTrans=" );
			// write trans back info
			if( m_vectStack[nStackId].m_pStackBackTrans != NULL )
				WriteTransToFile( pFile, m_vectStack[nStackId].m_pStackBackTrans, "BackTrans=" ); 
		}

		// add spacer
		fprintf( pFile, "\n" );
	}

	////////////////////////////
	// write paths
	fprintf( pFile, "[DynamicObjectsPath]\n" );
	for( i=0; i<m_vectDynamicObjPath.size(); i++ )
	{
		fprintf( pFile, "Path[%d]=%.6f,", i, m_vectDynamicObjPath[i].path_score );
		// save all points in the path
		for( j=0; j<m_vectDynamicObjPath[i].vectNode.size(); j++ )
		{
			fprintf( pFile, "%d,%d,%.6f,%.6f;",
						m_vectDynamicObjPath[i].vectNode[j].frame_id,
						m_vectDynamicObjPath[i].vectNode[j].obj_id,
						m_vectDynamicObjPath[i].vectNode[j].x,
						m_vectDynamicObjPath[i].vectNode[j].y );
		}
		fprintf( pFile, "\n" );
	}
	fprintf( pFile, "\n" );

	// close file
	fclose( pFile );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		IsImageInStack
// Class:		CImageGroup
// Purpose:		find if image is in stack aready
// Input:		image id from the group main list of files
// Output:		is = 1, not = 0
////////////////////////////////////////////////////////////////////
int CImageGroup::IsImageInStack( int nImageId )
{
	int i=0;

	for( i=0; i<m_nStack; i++ )
	{
		if( m_vectStack[i].image_id == nImageId ) return( 1 );
	}

	return( 0 );
}

////////////////////////////////////////////////////////////////////
// Method:		GetNewImageId
// Class:		CImageGroup
// Purpose:		find image with max id and add 1 to it 
// Input:		nothing
// Output:		image max id
////////////////////////////////////////////////////////////////////
int CImageGroup::GetNewImageId( )
{
	int i=0;
	int nImageMaxId = -1;
	int nNewImageId = 0;

	// get max id 
	for( i=0; i<m_vectImageNode.size(); i++ )
	{
		if( m_vectImageNode[i]->m_nId > nImageMaxId ) nImageMaxId = m_vectImageNode[i]->m_nId;
	}
	// if there was a max found
	if( nImageMaxId >= 0 ) nNewImageId = nImageMaxId+1;

	return( nNewImageId );
}

////////////////////////////////////////////////////////////////////
// Method:		GetImageNoById
// Class:		CImageGroup
// Purpose:		find image number using id
// Input:		image id from the group main list of files
// Output:		image no
////////////////////////////////////////////////////////////////////
int CImageGroup::GetImageNoById( int nImageId )
{
	int i=0;

	for( i=0; i<m_vectImageNode.size(); i++ )
	{
		if( m_vectImageNode[i]->m_nId == nImageId ) return( i );
	}

	return( -1 );
}

////////////////////////////////////////////////////////////////////
// Method:		GetImageStackId
// Class:		CImageGroup
// Purpose:		find stack image id
// Input:		image id from the group main list of files
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CImageGroup::GetImageStackId( int nImageId )
{
	int i=0;

	for( i=0; i<m_nStack; i++ )
	{
		if( m_vectStack[i].image_id == nImageId ) return( i );
	}

	return( -1 );
}

////////////////////////////////////////////////////////////////////
// Method:		GetImageByStackId
// Class:		CImageGroup
// Purpose:		find image id
// Input:		image id from stack list of files
// Output:		nothing
////////////////////////////////////////////////////////////////////
CAstroImage* CImageGroup::GetImageByStackId( int nImageId )
{
	int i=0;

	for( i=0; i<m_vectImageNode.size(); i++ )
	{
		if( m_vectImageNode[i]->m_nId == m_vectStack[nImageId].image_id ) return( m_vectImageNode[i] );
	}

	return( NULL );
}

////////////////////////////////////////////////////////////////////
// Method:		RemoveImageByPosId
// Class:		CImageGroup
// Purpose:		remove image from the list by id
// Input:		image id
// Output:		status
////////////////////////////////////////////////////////////////////
int CImageGroup::RemoveImageByPosId( int nImageId )
{
	int i=0;

	// check if in stack - and remove from there too 
	int nStackId = GetImageStackId( m_vectImageNode[nImageId]->m_nId );
	if( nStackId >= 0 )
	{
		m_vectStack.erase( m_vectStack.begin( ) + nStackId );
		m_nStack--;
	}

	// save??? and free image data
//	m_vectImageNode[nImageId]->SaveProfile( 0 );
	m_vectImageNode[nImageId]->FreeData();

	// remove from main list
	for( i=nImageId+1; i<m_vectImageNode.size(); i++ )
	{
		SwapImagesPositionById( i-1, i );
	}

	// decrease by one
	m_nImageNode--;

	// just delete the last one
	delete( m_vectImageNode[m_nImageNode] );
	m_vectImageNode[m_nImageNode] == NULL;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		SwapImagesPositionById
// Class:		CImageGroup
// Purpose:		swap images in the list by id
// Input:		image one and two ids
// Output:		status
////////////////////////////////////////////////////////////////////
int CImageGroup::SwapImagesPositionById( int nImageOneId, int nImageTwoId )
{
	// first check if out of range
	if( nImageOneId < 0 || nImageOneId > m_nImageNode ||
		nImageTwoId < 0 || nImageTwoId >= m_nImageNode ) return( 0 );

	CAstroImage* pTmpImage = NULL;

	// swap 
	pTmpImage = m_vectImageNode[nImageOneId];
	// set first to second
	m_vectImageNode[nImageOneId] = m_vectImageNode[nImageTwoId];
	// and set secodn to first
	m_vectImageNode[nImageTwoId] = pTmpImage;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		SwapStakImagesPositionById
// Class:		CImageGroup
// Purpose:		swap stack images in the list by id
// Input:		image one and two ids
// Output:		status
////////////////////////////////////////////////////////////////////
int CImageGroup::SwapStakImagesPositionById( int nImageOneId, int nImageTwoId )
{
	// first check if out of range
	if( nImageOneId < 0 || nImageOneId > m_vectStack.size() ||
		nImageTwoId < 0 || nImageTwoId >= m_vectStack.size() ) return( 0 );

	ImageStack pTmpImageStack;

	// swap 
	pTmpImageStack = m_vectStack[nImageOneId];
	// set first to second
	m_vectStack[nImageOneId] = m_vectStack[nImageTwoId];
	// and set secodn to first
	m_vectStack[nImageTwoId] = pTmpImageStack;

	return( 1 );
}


////////////////////////////////////////////////////////////////////
// Method:		FindImageInGroupByName
// Class:		CImageGroup
// Purpose:		get current most loaded image 
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
CAstroImage* CImageGroup::FindImageByName( wxString& strImageName,
										  wxString& strImageExt, int& nImgId )
{
	int i = 0;
	CAstroImage* pImageNode = NULL;
	nImgId = -1;

	// for each image node in group
	for( i=0; i<m_vectImageNode.size(); i++ )
	{
		pImageNode = m_vectImageNode[i];
		// check if image is named by 
		if( strImageName.CmpNoCase( pImageNode->m_strImageName ) == 0 &&
			strImageExt.CmpNoCase( pImageNode->m_strImageExt ) == 0 )
		{
			nImgId = i;
			return( pImageNode );
		}
	}

	return( NULL );
}

////////////////////////////////////////////////////////////////////
// Method:		AddAllImagesFromPath
// Class:		CImageGroup
// Purpose:		get current most loaded image 
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CImageGroup::AddAllImagesFromPath( int bRecursive )
{
	wxString	strFileName;
	bool cont=0;

	wxDir dir( m_strGroupPath );

	// check if directory was opened ok
    if ( !dir.IsOpened() )
    {
		return( 0 );
	}

	// get first file - was FILE_IMAGE_TYPES or m_pImageDb->m_strImageFormatsInput
	if( !bRecursive )
		cont = dir.GetFirst( &strFileName, FILE_IMAGE_TYPES, wxDIR_FILES );
	else
		cont = dir.GetFirst( &strFileName, FILE_IMAGE_TYPES, wxDIR_FILES|wxDIR_DIRS );

	while ( cont )
    {
		if( !the_ImageFileManager.CanImport(strFileName.ToAscii()) )
		{
			cont = dir.GetNext( &strFileName );
			continue;
		}

		// first allocate more memory
//		m_vectImageNode = (CAstroImage**) realloc( m_vectImageNode, (m_nImageNode+1) * sizeof( CAstroImage* ) );

		// extract image path/name/ext
		wxString strImagePath = wxT("");
		wxString strImageName = wxT("");
		wxString strImageExt = wxT("");
		wxSplitPath( strFileName, &strImagePath, &strImageName, &strImageExt );

		// create image node
		//m_vectImageNode[m_nImageNode] = new CAstroImage( this, m_strGroupPath, strImageName, strImageExt );
		CAstroImage* pImage = new CAstroImage( this, m_strGroupPath, strImageName, strImageExt );
		// set link weigth
		pImage->m_pImageGroup = this;
		// add image to vector
		m_vectImageNode.push_back( pImage );

		// increment counter
		m_nImageNode = m_vectImageNode.size();

		cont = dir.GetNext( &strFileName );
    }
 
	return( m_nImageNode );
}

////////////////////////////////////////////////////////////////////
// Method:		AddImages
// Class:		CImageGroup
// Purpose:		add an image to group
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CImageGroup::AddImages( const wxString& strImages, int& nFirstImageId )
{
	wxString strImageFiles = strImages;
	int nNoOfImages = 0;

	// now for pharse every image in my string and added to to the group
	wxStringTokenizer tkz( strImageFiles, wxT(";") );
	while ( tkz.HasMoreTokens() )
	{
		// get next file from input string
		wxString strFile = tkz.GetNextToken();	
		// add image to group
		nFirstImageId = AddImage( strFile );
		// remove file from string
		strImageFiles.Replace( strFile+wxT(";"), wxT(""), false );

		nNoOfImages++;
	}

	return( nNoOfImages );
}

////////////////////////////////////////////////////////////////////
// Method:		AddImage
// Class:		CImageGroup
// Purpose:		add an image to group
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CImageGroup::AddImage( const wxString& strImageFullPath, int bLog )
{
	wxString strLog;
	wxString strFileName = strImageFullPath;
	wxString strImagePath = wxT("");
//	wxString strImageSubName = "";
	wxString strImageName = wxT("");
	wxString strImageExt = wxT("");
	int nImgId = -1;

	// check if directory was opened ok
	if( !wxFileExists( strFileName ) )
    {
		strLog.Printf( wxT("ERROR :: file=%s doesn't exist"), strImageFullPath );
		m_pLogger->Log( strLog );

		return( 0 );
	}

	// extract file path and name from this
	wxSplitPath( strFileName, &strImagePath, &strImageName, &strImageExt );
//	strImageName = strImageSubName + "." + strImageExt;

	// try to find image by full name and path to make sure that we don't have dups
	if( FindImageByName( strImageName, strImageExt, nImgId ) )
	{
		if( bLog )
		{
			strLog.Printf( wxT("ERROR :: file=%s.%s is already in this group"), strImageName, strImageExt );
			m_pLogger->Log( strLog );
		}

		return( 0 );
	}
	
	// also check if group path empty set 
	if( m_strGroupPath.IsEmpty() ) m_strGroupPath = strImagePath;

	// now add the file info to this group
	// first allocate more memory
//	m_vectImageNode = (CAstroImage**) realloc( m_vectImageNode, (m_nImageNode+1) * sizeof( CAstroImage* ) );

	// create image node
	//m_vectImageNode[m_nImageNode] = new CAstroImage( this, strImagePath, strImageName, strImageExt );
	CAstroImage* pImage = new CAstroImage( this, strImagePath, strImageName, strImageExt );
	// get next max id  & set my image id
	pImage->m_nId = GetNewImageId( );
	// add to vector
	m_vectImageNode.push_back( pImage );

	// increment counter
	m_nImageNode = m_vectImageNode.size();

	return( m_nImageNode-1 );
}

////////////////////////////////////////////////////////////////////
void CImageGroup::DeleteImage( int nId )
{
	// delete image allocation
	delete( m_vectImageNode[nId] );

	// remove from vector
	m_vectImageNode.erase( m_vectImageNode.begin() + nId );
	m_nImageNode = m_vectImageNode.size();
}

////////////////////////////////////////////////////////////////////
// Method:		Detect
// Class:		CImageGroup
// Purpose:		detect object in all images in my group
// Input:		pointer to extractor object
// Output:		how many detected successufully ?
////////////////////////////////////////////////////////////////////
int CImageGroup::Detect( CSextractor* pDetect )
{
	wxString strLog;
	int i=0;
	CAstroImage* pImageNode = NULL;

	// for each image node in group
	for( i=0; i<m_nImageNode; i++ )
	{
		pImageNode = m_vectImageNode[i];
		// I should perhaps clear first detected ?

		// log detection
		strLog.Printf( wxT("Detect image %d out of %d, name=%s ..."), i+1, m_nImageNode, pImageNode->m_strImageName );
		m_pImageDb->m_pUnimapWorker->SetWaitDialogMsg( strLog );
		// detect image
		pImageNode->Detect( pDetect, 1 );
	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		SetHintToAllImages
// Class:		CImageGroup
// Purpose:		set hint for all images in the group using a ref image?
// Input:		reference image for hint - the first image in the group?
// Output:		status
////////////////////////////////////////////////////////////////////
int CImageGroup::SetHintToAllImages( int nType )
{
	char strLog[255];
	int i=0;
	CAstroImage* pImageNode = NULL;
	// get pointer to frame
	CGUIFrame* pFrame = m_pImageDb->m_pUnimap->m_pFrame;

	// for each image node in group
	for( i=0; i<m_nImageNode; i++ )
	{
		pImageNode = m_vectImageNode[i];

		// check if current image from fram is in my group
		if( pFrame->m_pAstroImage && pFrame->m_pAstroImage == pImageNode && pFrame->m_pAstroImage->m_pImageGroup == this )
		{
			// the save last profile ?
//			pFrame->m_pAstroImage->SaveProfile( 0 );
			//pFrame->m_pAstroImage->FreeData( );
			//pFrame->m_pImgView->FreeData( );

		} else
		{
			// load image profile
			pImageNode->LoadProfile( 0 );
		}

		// if hint type = 1 = dont apply to images which already have a hit 
		if( nType == 1 && pImageNode->m_nUseHint > 0 ) continue; 

		//////////
		// SET group image HINT
		pImageNode->SetHint( m_nRelCenterRa, m_nRelCenterDec, m_nRelFieldRa, m_nRelFieldDec );
		pImageNode->m_nHintOrigType = m_nHintOrigType;
		pImageNode->m_nHintObjOrigType = m_nHintObjOrigType;
		pImageNode->m_nHintObjName = m_nHintObjName;
		pImageNode->m_strHintObjId = m_strHintObjId;
		pImageNode->m_nHintOrigUnits = m_nHintOrigUnits;
		pImageNode->m_nHintFieldType = m_nHintFieldType;
		// camera
		pImageNode->m_nHintCameraSource = m_nHintCameraSource;
		pImageNode->m_nHintCameraType = m_nHintCameraType;
		pImageNode->m_nHintCameraBrand = m_nHintCameraBrand;
		pImageNode->m_nHintCameraName = m_nHintCameraName;
		// sensor size
		pImageNode->m_nHintSensorWidth = m_nHintSensorWidth;
		pImageNode->m_nHintSensorHeight = m_nHintSensorHeight;
		// telescope/lens
		pImageNode->m_nHintTLensSource = m_nHintTLensSource;
		memcpy( &m_rHintTLensSetup, &m_rHintTLensSetup, sizeof(DefTLensSetup) );
		// system focal length
		pImageNode->m_nHintFocalLength = m_nHintFocalLength;
		// field
		pImageNode->m_nHintFieldFormat = m_nHintFieldFormat;

		pImageNode->m_bIsChanged = 1;
		// save image profile
		pImageNode->SaveProfile( 0 );
		////////
		// close loaded image - if not loaded in view
		if( pFrame->m_pAstroImage && pFrame->m_pAstroImage == pImageNode && pFrame->m_pAstroImage->m_pImageGroup == this )
		{
			// do something here ?
		} else
		{
			pImageNode->FreeImageData( );
		}

	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		Match
// Class:		CImageGroup
// Purpose:		match objects in all images (detected) in my group
// Input:		pointer to sky search engine object
// Output:		how many were matched successufully ?
////////////////////////////////////////////////////////////////////
int CImageGroup::Match( CSky* pSky )
{
	wxString strLog;
	int i=0;
	CAstroImage* pImageNode = NULL;
	int bKeepCurrentImage = 0;

	// get pointer to frame
	CGUIFrame* pFrame = m_pImageDb->m_pUnimap->m_pFrame;

	// get pointer to wait dialog
//	CWaitDialogMatch* pWaitDialogMatch = pFrame->m_pWaitDialogMatch;

	// for each image node in group
	for( i=0; i<m_nImageNode; i++ )
	{
		bKeepCurrentImage = 0;
		pImageNode = m_vectImageNode[i];

		// check if current image from fram is in my group
		if( pFrame->m_pAstroImage && pFrame->m_pAstroImage == pImageNode && pFrame->m_pAstroImage->m_pImageGroup == this )
		{
			bKeepCurrentImage = 1;
			// the save last profile ?
//			pFrame->m_pAstroImage->SaveProfile( 0 );
			//pFrame->m_pAstroImage->FreeData( );
			//pFrame->m_pImgView->FreeData( );
		} else
		{
			// load image profile
			pImageNode->LoadProfile( 0 );
		}

		// check if any objects detected (at least 3) else skip
		if( pImageNode->m_nStar <= 3 ) 
		{
			if( bKeepCurrentImage == 0 ) pImageNode->FreeData( );
			continue;
		}
		// set image node config
		pImageNode->SetConfig( m_pImageDb->m_pUnimap->m_pMainConfig, m_pImageDb->m_pUnimap->m_pDetectConfig );
		pImageNode->SetSky( pSky );


		// set wait dialog window title
		wxString strFullTitle = wxT( "Group Matching Process" );
		strFullTitle = strFullTitle + wxT( " :: " ) + wxString(m_strGroupNameId,wxConvUTF8) +
						wxT( " :: " ) + wxString(pImageNode->m_strImageName,wxConvUTF8);
		//pWaitDialogMatch->m_strTitle = strFullTitle;
		m_pImageDb->m_pUnimapWorker->SetWaitDialogTitle( strFullTitle );

		// log matching
		strLog.Printf( wxT("Look for sky object in image %d out of %d, name=%s ..."), i+1, m_nImageNode, pImageNode->m_strImageName );
		m_pImageDb->m_pUnimapWorker->SetWaitDialogMsg( strLog );

		/////////////////////////////////
		// do the individual astro-image matching
		int bSolved = pSky->SolveAstroImage( pImageNode, m_pImageDb->m_pUnimapWorker );

		// check if solved problems
		if( !bSolved ) 
		{
			if( bKeepCurrentImage == 0 ) pImageNode->FreeData();
			continue;

		} else
		{
			// save image profile
			pImageNode->SaveProfile( 0 );
			if( bKeepCurrentImage == 0 ) pImageNode->FreeData();
		}

	}

	////////////////////////////////////////////////
	// now at the end unload the matching catalog to free ram
//	pSky->m_pCatalogStars->UnloadCatalog( );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		InitStack
// Class:		CImageGroup
// Purpose:		init image stack
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CImageGroup::InitStack( )
{
	int i=0;

	// reset stack size to zero
	m_nStack = 0;

	// allocate my stack
	m_vectStack.resize( m_nImageNode );

	// for each image node in group
	for( i=0; i<m_nImageNode; i++ )
	{
		m_vectStack[i].p_image = m_vectImageNode[i];
		m_vectStack[i].image_id = m_vectImageNode[i]->m_nId;
		m_vectStack[i].angle_deg = 0.0;
		m_vectStack[i].angle_rad = 0.0;
		m_vectStack[i].register_score = 0;
		m_vectStack[i].scale = 0.0;
		m_vectStack[i].shift_x = 0.0;
		m_vectStack[i].shift_y = 0.0;

		// init matching params
		m_vectStack[i].m_pStackTransRegister = NULL;
		m_vectStack[i].m_pStackBackTrans = NULL;
		m_vectStack[i].m_pStackNextTrans = NULL;

		// increment counter
		m_nStack++;
	}

	// sort by file stamp
	SortStackImagesByFileTime( 1 );
	//std::sort( m_vectStack.begin(), m_vectStack.end(), AscCmpFileTimestamp );

//	m_nStack = m_nImageNode;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		SortImagesById
// Class:		CImageGroup
// Purpose:		sort image list by image id
// Input:		order flag - ascendent or descendent
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CImageGroup::SortImagesById( int bOrder )
{
	int i=0;
	int bNotWorder = 1; 

	while( bNotWorder ) 
	{
		bNotWorder = 0;
		// for each image node in group
		for( i=1; i<m_nImageNode; i++ )
		{
			// compare by order flag
			if( ( bOrder && (  m_vectImageNode[i-1]->m_nId < m_vectImageNode[i]->m_nId ) ) ||
				( !bOrder && ( m_vectImageNode[i-1]->m_nId > m_vectImageNode[i]->m_nId ) ) )
			{
				SwapImagesPositionById( i-1, i );
				bNotWorder = 1;
			} 
		}
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		SortImagesByName
// Class:		CImageGroup
// Purpose:		sort image list by name
// Input:		order flag - ascendent or descendent
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CImageGroup::SortImagesByName( int bOrder )
{
	int i=0;
	int bNotWorder = 1; 

	while( bNotWorder ) 
	{
		bNotWorder = 0;
		// for each image node in group
		for( i=1; i<m_nImageNode; i++ )
		{
			// compare by order flag
			if( ( bOrder && ( m_vectImageNode[i-1]->m_strImageName.CmpNoCase( m_vectImageNode[i]->m_strImageName ) > 0 ) ) ||
				( !bOrder && ( m_vectImageNode[i-1]->m_strImageName.CmpNoCase( m_vectImageNode[i]->m_strImageName ) ) < 0 ) )
			{
				SwapImagesPositionById( i-1, i );
				bNotWorder = 1;
			} 
		}
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		SortImagesByFileTime
// Class:		CImageGroup
// Purpose:		sort image list by image file time
// Input:		order flag - ascendent or descendent
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CImageGroup::SortImagesByFileTime( int bOrder )
{
	int i=0;
	int bNotWorder = 1; 

	while( bNotWorder ) 
	{
		bNotWorder = 0;
		// for each image node in group
		for( i=1; i<m_nImageNode; i++ )
		{
			// compare by order flag
			if( ( bOrder && (  m_vectImageNode[i-1]->m_nFileTime < m_vectImageNode[i]->m_nFileTime ) ) ||
				( !bOrder && ( m_vectImageNode[i-1]->m_nFileTime > m_vectImageNode[i]->m_nFileTime ) ) )
			{
				SwapImagesPositionById( i-1, i );
				bNotWorder = 1;
			} 
		}
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		SortStackImagesById
// Class:		CImageGroup
// Purpose:		sort stack images list by image id
// Input:		order flag - ascendent or descendent
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CImageGroup::SortStackImagesById( int bOrder )
{
	int i=0;
	int bNotWorder = 1; 

	if( bOrder )
	{
		// sort ascending
		std::sort( m_vectStack.begin(), m_vectStack.end(), AscCmpByImageId );

	} else
	{
		// sort descending
		std::sort( m_vectStack.begin(), m_vectStack.end(), DesCmpByImageId );
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		SortStackImagesByName
// Class:		CImageGroup
// Purpose:		sort stack images list by image name
// Input:		order flag - ascendent or descendent
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CImageGroup::SortStackImagesByName( int bOrder )
{
	int i=0;
	int bNotWorder = 1; 

	if( bOrder )
	{
		// sort ascending
		std::sort( m_vectStack.begin(), m_vectStack.end(), AscCmpFileName );

	} else
	{
		// sort descending
		std::sort( m_vectStack.begin(), m_vectStack.end(), DesCmpFileName );
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		SortStackImagesByFileTime
// Class:		CImageGroup
// Purpose:		sort stack images list by image file time stamp
// Input:		order flag - ascendent or descendent
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CImageGroup::SortStackImagesByFileTime( int bOrder )
{
	int i=0;
	int bNotWorder = 1; 

	if( bOrder )
	{
		// sort ascending
		std::sort( m_vectStack.begin(), m_vectStack.end(), AscCmpFileTimestamp );

	} else
	{
		// sort descending
		std::sort( m_vectStack.begin(), m_vectStack.end(), DesCmpFileTimestamp );
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		Register
// Class:		CImageGroup
// Purpose:		register group images out of the stack list
// Input:		nothing
// Output:		status
////////////////////////////////////////////////////////////////////
int CImageGroup::Register( )
{
	if( m_nImageNode <= 1 ) return( 0 );

	wxString strLog;
	int i=0;
	int nMatched = 0, nRegisterScore=0;
	double nAngleRad=0, nAngleDeg=0, nShiftX=0, nShiftY=0, nScale=0;

	// init stack if not initialized already
	if( m_vectStack.size() <= 0 ) InitStack( );

	// assign image references - 0 is default reference
	CAstroImage* pImgRef = NULL;
	CAstroImage* pImage = NULL;
	// get star match
	CStarMatch* pMatch = m_pImageDb->m_pUnimap->m_pStarMatch;
	// get max no of stars - for now use reference - todo :: feature calculate average
	long nStarsRefToExtract = 80; // hardcode : todo: larry 
	long nStarsRefExtracted = 0;
	long nStarsExtracted = 0;


	// define and allocate temp vectors
	StarDef* vectStarRef = (StarDef*) malloc( (nStarsRefToExtract+1)*sizeof(StarDef) );
	StarDef* vectStar = (StarDef*) malloc( (nStarsRefToExtract+1)*sizeof(StarDef) );
	
	// for each image node in group
	for( i=1; i<m_vectStack.size(); i++ )
	{
		///////////////////
		// get reference image pointer
		pImgRef = GetImageByStackId( i-1 );
		if( !pImgRef ) continue;
		// first init refe image
		pImgRef->LoadProfile( 0 );
		// check if enough stars
		if( pImgRef->m_nStar < 2 ) continue; 

//		nStarsRefToExtract = pImgRef->m_nStar;
		// extract reference image stars
		nStarsRefExtracted = pImgRef->ExtractStars( vectStarRef, nStarsRefToExtract, 1, 0, 1 );
		pImgRef->FreeData( );

		///////////////////
		// get current image pointer
		pImage = GetImageByStackId( i );
		if( !pImage ) continue;
		// load profile
		pImage->LoadProfile( 0 );
		// check if enough stars
		if( pImage->m_nStar < 2 ) continue; 
	
		// log detection
		strLog.Printf( wxT("register image %d out of %d, name=%s ..."), i, m_nStack-1, pImage->m_strImageName );
		m_pImageDb->m_pUnimapWorker->SetWaitDialogMsg( strLog );

		// extract stars from current
		nStarsExtracted = pImage->ExtractStars( vectStar, nStarsRefToExtract, 1, 0, 1 );
		pImage->FreeData( );

		// do compare to get trans from current image to - use another method ???
		pMatch->m_nMaxMatch = 0;
		nMatched = pMatch->Cmp( vectStar, nStarsExtracted, vectStarRef, nStarsRefExtracted, 0 );
	
		////////////////
		// here we add the trans to my trans vector ???!
		if( pMatch->m_nMaxMatch >= 3  )
		{
			double c = pMatch->m_pLastTrans->c;
			double b = pMatch->m_pLastTrans->b;
			nAngleRad = atan2( c, b );
			nAngleDeg = nAngleRad*RAD2DEG;
			nScale = sqrt( b*b + c*c );
			nShiftX = pMatch->m_pLastTrans->a;
			nShiftY = pMatch->m_pLastTrans->d;

			// set my stack
			nRegisterScore = 1;
			m_vectStack[i].angle_deg = nAngleDeg;
			m_vectStack[i].angle_rad = nAngleRad;
			m_vectStack[i].scale = nScale;
			m_vectStack[i].shift_x = nShiftX;
			m_vectStack[i].shift_y = nShiftY;
	
			///////////////////
			// save my transformation
			// :: check if trans allocated
			if( m_vectStack[i].m_pStackTransRegister == NULL )
				m_vectStack[i].m_pStackTransRegister = (TRANS*) malloc( sizeof(TRANS) );
			// save trans next
			pMatch->CopyTrans( pMatch->m_pLastTrans, m_vectStack[i].m_pStackTransRegister );

		} else
		{
			nRegisterScore = 0;
			// set trans to zero
			if( m_vectStack[i].m_pStackTransRegister != NULL )
			{
				free( m_vectStack[i].m_pStackTransRegister );
				m_vectStack[i].m_pStackTransRegister = NULL;
			}
		}
		// set my score and print to file - todo:: perhaps I should store this in goup config ?
		m_vectStack[i].register_score = nRegisterScore;
		
	}

	// free my data
	free( vectStarRef );
	free( vectStar );
	// set that stack was registered
	m_bRegistered = 1;
	// save group
	Save( );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		Align
// Class:		CImageGroup
// Purpose:		apply register to the registered group images out of 
//				the stack list
// Input:		nothing
// Output:		status
////////////////////////////////////////////////////////////////////
int CImageGroup::Align( )
{
	if( m_nImageNode <= 1 ) return( 0 );
	if( m_vectStack.size() <= 0 ) return( 0 );
	if( m_bRegistered == 0 ) return( 0 );

	wxString strLog;
	int i=0;
	double nAngleRad=0, nAngleDeg=0, nShiftX=0, nShiftY=0, nScale=1.000;
	wxPoint nRotOffset;
	CAstroImage* pImage = NULL;

	// for each image node in group
	for( i=1; i<m_nStack; i++ )
	{
		pImage = GetImageByStackId( i );
		if( !pImage ) continue;

		// check if image good for stack apply
		if( m_vectStack[i].register_score > 0 )
		{
			// :: rotation angle increment
			nAngleRad = (double) (nAngleRad+m_vectStack[i].angle_rad);
			// :: scale multiply with current value
			nScale = (double) (nScale*m_vectStack[i].scale);
			// :: shift increment
			nShiftX = (double) (m_vectStack[i].shift_x);
			nShiftY = (double) (m_vectStack[i].shift_y);

			//////////////////
			// get image
			wxImage* pImg = pImage->GetImage( );
			// check if load failed
			if( pImg == NULL || !pImg ) continue;
			// copy image localy
			wxImage pStackImg = pImg->Copy();
			wxImage pTmpImg;
			// release node image
			pImage->CloseImage( );
			// get center
			double nWidth = (double) pStackImg.GetWidth();
			double nHeight = (double) pStackImg.GetHeight();
			double nCenterX = (double) nWidth/2.0;
			double nCenterY = (double) nHeight/2.0;
			// rotation shift
			double nRotShiftX = 0.0;
			double nRotShiftY = 0.0;

			////////////////////////////////////////
			// :: ROTATE first - if not zero - adjust rot with tolerance
			if( fabs(nAngleRad*RAD2DEG) > STACK_ALIGN_MINIMUM_ROTATION_ANGLE )
			{
				// log rotation
				strLog.Printf( wxT("rotate image %d out of %d, name=%s ..."), i, m_nStack-1, pImage->m_strImageName );
				m_pImageDb->m_pUnimapWorker->SetWaitDialogMsg( strLog );
				// rotate image
				pTmpImg = pStackImg.Rotate( nAngleRad, wxPoint( nCenterX, nCenterY ), true, &nRotOffset );
				int w = pTmpImg.GetWidth();
				int h = pTmpImg.GetHeight();
				pStackImg = pTmpImg.Copy();
				// set rotation shift
				nRotShiftX = (double) (nWidth-w)/2.0;
				nRotShiftY = (double) (nHeight-h)/2.0;
				// cut down to my frame size
				pStackImg.Resize( wxSize( nWidth, nHeight), 
										wxPoint( (int) nRotShiftX, (int) nRotShiftY ),
										0, 0 , 0 );

			}

			///////////////////////////////////////////////
			// :: SCALE - if scale fact bigger the minimum tolerance
			if( fabs(nScale - 1.00000) > STACK_ALIGN_MINIMUM_SCALE_FACTOR )
			{
				// log scale
				strLog.Printf( wxT("scale image %d out of %d, name=%s ..."), i, m_nStack-1, pImage->m_strImageName );
				m_pImageDb->m_pUnimapWorker->SetWaitDialogMsg( strLog );
				// now scale image
				int nWidthNew = (int) round( pStackImg.GetWidth()*nScale );
				int nHeightNew = (int) round( pStackImg.GetHeight()*nScale );
				pStackImg.Rescale( nWidthNew, nHeightNew, wxIMAGE_QUALITY_HIGH );
			}

			///////////////////////////////////
			// :: TRANSLATE
			if( nShiftX != 0.000 || nShiftY != 0.000 )
			{
				// log translation
				strLog.Printf( wxT("translate image %d out of %d, name=%s ..."), i, m_nStack-1, pImage->m_strImageName );
				m_pImageDb->m_pUnimapWorker->SetWaitDialogMsg( strLog );
				// and translate
				int nType = 1;
				if( nType == 0 )
				{
					int nDiffX = (int) round( (pStackImg.GetWidth()*nScale+fabs(nShiftX)) );
					int nDiffY = (int) round( (pStackImg.GetHeight()*nScale+fabs(nShiftY)) );
					pStackImg.Resize( wxSize( nDiffX, nDiffY), 
										wxPoint( (int) round( nShiftX ), (int) round( nShiftY ) ),
										0, 0 , 0 );

				} else if( nType == 1 )
				{
					//int nDiffX = (int) (pStackImg.GetWidth()*nScale+0.49999);
					//int nDiffY = (int) (pStackImg.GetHeight()*nScale+0.49999);
					//int nWidthNew = pStackImg.GetWidth();
					//int nHeightNew = pStackImg.GetHeight();

//					pStackImg.Resize( wxSize( nWidth, nHeight), 
//										wxPoint( (int) nShiftX+0.49999, (int) nShiftY+0.49999 ),
//										0, 0 , 0 );
					// without correction
					int nx = (int) round(nShiftX);
					int ny = (int) round(nShiftY);
					pStackImg.Resize( wxSize( (int) nWidth, (int) nHeight), 
										wxPoint( nx, ny ),
										0, 0 , 0 );

				}
			}

			////////
			// save with _stack in the name
			wxString strFile;
			strFile.Printf( wxT("%s%s%s_stack.bmp"), pImage->m_strImagePath, wxT("\\"), pImage->m_strImageName );
			pStackImg.SaveFile( strFile, wxBITMAP_TYPE_BMP );
			// destroy image
			pStackImg.Destroy();
		}

	}

	return( 0 );
}

////////////////////////////////////////////////////////////////////
// Method:		FindDynamicObjects
// Class:		CImageGroup
// Purpose:		search my image sequence for dynamic objects
// Input:		nothing
// Output:		status
////////////////////////////////////////////////////////////////////
int CImageGroup::FindDynamicObjects( )
{
	int i=0, j=0, k=0, e=0, r=0;
	// keep common area diffs to next frame
	//typeStarList* vectTrackBack;
	vector <typeStarList> vectTrackBack;
	// keep common area diffs to back frame
	//typeStarList* vectTrackNext;
	vector <typeStarList> vectTrackNext;
	// image associated objects
	//typeIntVect* vectObjPathAssoc;
	vector <typeIntVect> vectObjPathAssoc;

	////////////////////////
	// histogram data from opencv
	int hist_size = 256;
	float range_0[]={0,256};
	float* ranges[] = { range_0 };
	//typeDoubleMatrix* vectHistogramDistance;
	vector <typeDoubleMatrix> vectHistogramDistance;

	IplImage *refHistImgRed=NULL, *refHistImgGreen=NULL, *refHistImgBlue=NULL, *refHistImgMask=NULL;
	IplImage *destHistImgRed=NULL, *destHistImgGreen=NULL, *destHistImgBlue=NULL, *destHistImgMask=NULL;

	// reference histogram vector
	CvHistogram** vectHistRefRed;
	CvHistogram** vectHistRefGreen;
	CvHistogram** vectHistRefBlue;
	// destination histogram vector
	CvHistogram** vectHistDestRed;
	CvHistogram** vectHistDestGreen;
	CvHistogram** vectHistDestBlue;

	//////////////////////////// def from register
	wxString strLog;
	int nMatched = 0, nRegisterScore=0;
	double nAngleRad=0, nAngleDeg=0, nShiftX=0, nShiftY=0, nScale=0;

	// init stack if not initialized already
	if( m_vectStack.size() <= 0 ) InitStack( );

	// clear all dynamic object paths
	ClearAllDynamicObjectPaths( );
	// get no of Frames
	int nNoOfFrames = m_vectStack.size();

	// allocate tracks vectors
	//vectTrackBack = (typeStarList*) calloc( nNoOfFrames, sizeof(typeStarList) );
	//vectTrackNext = (typeStarList*) calloc( nNoOfFrames, sizeof(typeStarList) );
	//vectObjPathAssoc = (typeIntVect*) calloc( nNoOfFrames, sizeof(typeIntVect) );
	// new vector alloc
	vectTrackBack.clear();
	vectTrackBack.resize( nNoOfFrames );
	vectTrackNext.clear();
	vectTrackNext.resize( nNoOfFrames );
	vectObjPathAssoc.clear();
	vectObjPathAssoc.resize( nNoOfFrames );

	// alocate histogram vector
	//vectHistogramDistance = (typeDoubleMatrix*) calloc( nNoOfFrames-1, sizeof(typeDoubleMatrix) );
	vectHistogramDistance.clear();
	vectHistogramDistance.resize( nNoOfFrames-1 );

	// assign image references - 0 is default reference
	CAstroImage* pImgRef = NULL;
	CAstroImage* pImage = NULL;
	// get star match
	CStarMatch* pMatch = m_pImageDb->m_pUnimap->m_pStarMatch;
	// get max no of stars - for now use reference - todo :: feature calculate average
	long nStarsRefAllocated = 500; // hardcode : max initial
	long nStarsAllocated = 500;
	long nStarsRefExtracted = 0;
	long nStarsExtracted = 0;

	// define and allocate temp vectors
	StarDef* vectStarRef = (StarDef*) malloc( (nStarsRefAllocated+1)*sizeof(StarDef) );
	StarDef* vectStar = (StarDef*) malloc( (nStarsAllocated+1)*sizeof(StarDef) );
	/////////////////////////// end of def from register

	// loop to my image time sequence - make sure it's orderd by time! 
	// - or just use stack ... should do

	///////////////////
	// main loop through image sequence - stack
	for( i=1; i<nNoOfFrames; i++ )
	{
		// if first ref frame allocate ref
		if( i == 1 )
		{
			///////////////////
			// GET reference image pointer
			pImgRef = GetImageByStackId( i-1 );
			if( !pImgRef ) continue;
			// first init refe image
			pImgRef->LoadProfile( 0 );
			// check if enough stars
			if( pImgRef->m_nStar < 2 )
			{
				pImgRef->FreeData( );
				continue; 
			}

			// :: handle reallocation if needed
			if( nStarsRefAllocated < pImgRef->m_nStar )
			{
				nStarsRefAllocated = pImgRef->m_nStar+1;
				vectStarRef = (StarDef*) realloc( vectStarRef, nStarsRefAllocated*sizeof(StarDef) );
			}

	//		nStarsRefToExtract = pImgRef->m_nStar;
			// extract reference image stars
			nStarsRefExtracted = pImgRef->ExtractStars( vectStarRef, pImgRef->m_nStar, 0, 0, 0, 0, m_nDynamicObjMatchMaxFwhm );

			// allocate and initialize assoc vectors
			vectObjPathAssoc[0].clear( );
			for( e=0; e<pImgRef->m_nStar; e++ ) vectObjPathAssoc[0].push_back( -1 );
		}

		///////////////////////////
		// GET current image pointer
		pImage = GetImageByStackId( i );
		if( !pImage ) continue;
		// load profile
		pImage->LoadProfile( 0 );
		// check if enough stars
		if( pImage->m_nStar < 2 )
		{
			pImage->FreeData( );
			continue; 
		}
	
		// log detection
		//wxMutexGuiEnter();
		strLog.Printf( wxT("Step %d out of %d :: analyze image sequence %s to %s ..."), i, m_nStack-1, pImgRef->m_strImageName, pImage->m_strImageName );
		m_pImageDb->m_pUnimapWorker->SetWaitDialogMsg( strLog );
		//wxMutexGuiLeave();

		// :: handle reallocation if needed
		if( nStarsAllocated < pImage->m_nStar )
		{
			nStarsAllocated = pImage->m_nStar+1;
			vectStar = (StarDef*) realloc( vectStar, nStarsAllocated*sizeof(StarDef) );
		}

		// extract stars from current
		nStarsExtracted = pImage->ExtractStars( vectStar, pImage->m_nStar, 0, 0, 0, 0, m_nDynamicObjMatchMaxFwhm );

		// allocate and initialize assoc vectors
		vectObjPathAssoc[i].clear( );
		for( e=0; e<pImage->m_nStar; e++ ) vectObjPathAssoc[i].push_back( -1 );

		// clear track next and back
		vectTrackNext[i-1].clear();
		vectTrackBack[i].clear();

		/////////////////////////////////
		// :: calc trans frame1 to frame2
		// do compare to get trans from current image to
		pMatch->m_nMaxMatch = 0;
		nMatched = pMatch->Cmp( vectStarRef, nStarsRefExtracted, vectStar, nStarsExtracted, 0 );
		if( nMatched  == 0 )
		{
			//wxMutexGuiEnter();
			strLog.Printf( wxT("ERROR :: TRANS from frame %d to frame %d failed"), i, i-1 );
			m_pImageDb->m_pUnimapWorker->SetWaitDialogMsg( strLog );
			//wxMutexGuiLeave();
			// then break
			break;
		}

		// check if next trans allocated
		if( m_vectStack[i-1].m_pStackNextTrans == NULL )
			m_vectStack[i-1].m_pStackNextTrans = (TRANS*) malloc( sizeof(TRANS) );
		// save trans next
		pMatch->CopyTrans( pMatch->m_pTrans, m_vectStack[i-1].m_pStackNextTrans );

		/////////////////////////////////////////////////////////////////////
		// :: extract what is not matched form reference frame to next
		for( j=0; j<nStarsRefExtracted; j++ )
		{
			// if not matched
			if( vectStarRef[j].match == 0 )
			{
				// calculate point projection in next frame
				double x = vectStarRef[j].x;
				double y = vectStarRef[j].y;
				// apply trans f1->f2 to current point
				pMatch->CalcPointTrans( x, y );
				// check if projection is in next frame
				if( ( x > 0 && x < pImage->m_nWidth ) && ( y > 0 && y < pImage->m_nHeight ) )
				{
					// add to my next list
					vectTrackNext[i-1].push_back( vectStarRef[j] );
				}
			}
		}

		//////////////////////////////////
		// :: inverse transformation FRAME(i) -> FRAME(i-1)
		if( pMatch->CalcInv( vectStar, nStarsExtracted, vectStarRef, nStarsRefExtracted ) == 0 )
		{
			//wxMutexGuiEnter();
			strLog.Printf( wxT("ERROR :: INVERSE TRANS from frame %d to frame %d failed"), i, i-1 );
			m_pImageDb->m_pUnimapWorker->SetWaitDialogMsg( strLog );
			//wxMutexGuiLeave();
			// then break
			break;
		}

		// check if next trans allocated
		if( m_vectStack[i].m_pStackBackTrans == NULL )
			m_vectStack[i].m_pStackBackTrans = (TRANS*) malloc( sizeof(TRANS) );
		// save trans back
		pMatch->CopyTrans( pMatch->m_pInvTrans, m_vectStack[i].m_pStackBackTrans );

		//////////////////////////////////////////////////////////
		// :: extract what is not matched from next frame to reference
		for( j=0; j<nStarsExtracted; j++ )
		{
			// if not matched
			if( vectStar[j].match == 0 )
			{
				// calculate point projection in next frame
				double x = vectStar[j].x;
				double y = vectStar[j].y;
				// apply trans f1->f2 to current point
				pMatch->CalcInvPointTrans( x, y );
				// check if projection is in next frame
				if( ( x > 0 && x < pImgRef->m_nWidth ) && ( y > 0 && y < pImgRef->m_nHeight ) )
				{
					// add to my back list
					vectTrackBack[i].push_back( vectStar[j] );
				}
			}
		}

		/////////////////////////////////////////////////////////////
		// now check for objects for which projection is in the a specific tolerance 
		// and remove them from list as well -  ??? or mark them somehow ?? think ?
		// :: for all objects not matched in ref frame - vectTrackNext
		for( std::vector<StarDef>::iterator t = vectTrackNext[i-1].begin(); t != vectTrackNext[i-1].end();  )
		{
			// calculate point projection in next frame
			double x = (*t).x;
			double y = (*t).y;
			// apply trans f1->f2 to current point
			pMatch->CalcPointTrans( x, y );

			// find smalles distance with projection to an object in the next frame
			double nDistEval = 0.0;
			double nDistMin = DBL_MAX;
			int nFoundMatch = -1;
			// for all objects in the destination(next) frame not matched - use vectTrackBack
			for( k=0; k < vectTrackBack[i].size(); k++ )
			{
				// calculate distance
				nDistEval = CalcPlanarDistance( x, y, vectTrackBack[i][k].x, vectTrackBack[i][k].y );
				// check if smaller
				if( nDistEval < nDistMin )
				{
					nDistMin = nDistEval;
					nFoundMatch = k;
				}
			}

			// check if object close in tolerance
			if( nFoundMatch != -1 && nDistMin != DBL_MAX && nDistMin < m_nDynamicObjMinDistMatch )
			{
				// remove from my ref next list
				t = vectTrackNext[i-1].erase( t );
				// also remove from dest list
				vectTrackBack[i].erase( vectTrackBack[i].begin() + nFoundMatch );

			} else
			{
				++t;
			}
		}

		// :: AND for all objects not matched in destination frame - vectTrackBack
		for( std::vector<StarDef>::iterator t = vectTrackBack[i].begin(); t != vectTrackBack[i].end();  )
		{
			// calculate point projection in next frame
			double x = (*t).x;
			double y = (*t).y;
			// apply trans f1->f2 to current point
			pMatch->CalcInvPointTrans( x, y );

			// find smalles distance with inverse projection to an object in the next frame
			double nDistEval = 0.0;
			double nDistMin = DBL_MAX;
			int nFoundMatch = -1;
			// for all objects in the destination(next) frame not matched - use vectTrackNext[i-1]
			for( k=0; k < vectTrackNext[i-1].size(); k++ )
			{
				// calculate distance
				nDistEval = CalcPlanarDistance( x, y, vectTrackNext[i-1][k].x, vectTrackNext[i-1][k].y );
				// check if smaller
				if( nDistEval < nDistMin )
				{
					nDistMin = nDistEval;
					nFoundMatch = k;
				}
			}

			// check if object close in tolerance
			if( nFoundMatch != -1 && nDistMin != DBL_MAX && nDistMin < m_nDynamicObjMinDistMatch )
			{
				// remove from my dest track back list
				t = vectTrackBack[i].erase( t );
				// also remove from ref next list
				vectTrackNext[i-1].erase( vectTrackNext[i-1].begin() + nFoundMatch );

			} else
			{
				++t;
			}
		}

		// store no of next/back possible dynamic objects
		int nTrackNext = vectTrackNext[i-1].size();
		int nTrackBack = vectTrackBack[i].size();

		///////////////////////////////////////////////////////////////////////
		// FIND TRACKS BY ANALIZING FLOW BETWEEN REFERNCE FRAME AND NEXT FRAME
		// now for the remaining objects do:
		// for object of1(x) check all objects of2(i) and find which profile fwhm or 
		// radial profile or flux is more similar with this and:
		// - set association 
		// - mark object from source as done
		// trajectory option -- rectilinar most realistc...

		int x=0, y=0;
		unsigned char* vectRefImgData;
		unsigned char* vectDestImgData;
		int nL = 0;
		int w_ref=0, h_ref=0, w_dest=0, h_dest=0;
		double nSX=0.0, nSY=0.0, a=0.0, b=0.0, nSR=0.0;
		// CV_COMP_CORREL, CV_COMP_CHISQR, CV_COMP_INTERSECT, CV_COMP_BHATTACHARYYA
		int bDoNormalizeHist = 1;
		// if normalization factor is zero then set flag not to normalize
		if( m_nObjHistogramNormalize == 0.0 ) 
			bDoNormalizeHist = 0;

		// allocate ref histograms
		vectHistRefRed = (CvHistogram**) calloc( nTrackNext, sizeof(CvHistogram*) );
		vectHistRefGreen = (CvHistogram**) calloc( nTrackNext, sizeof(CvHistogram*) );
		vectHistRefBlue = (CvHistogram**) calloc( nTrackNext, sizeof(CvHistogram*) );
		// allocate dest histograms
		vectHistDestRed = (CvHistogram**) calloc( nTrackBack, sizeof(CvHistogram*) );
		vectHistDestGreen = (CvHistogram**) calloc( nTrackBack, sizeof(CvHistogram*) );
		vectHistDestBlue = (CvHistogram**) calloc( nTrackBack, sizeof(CvHistogram*) );

		// load frame current and dest
//		if( i == 1 ) 
//		{
			vectRefImgData = (pImgRef->GetImage())->GetData();
			// get width
			w_ref = (pImgRef->GetImage())->GetWidth();
			h_ref = (pImgRef->GetImage())->GetHeight();
//		}

		//////////////////////////////
		// calculate histograms for ref
		for( r=0; r<nTrackNext; r++ )
		{
			// ellipse distance
			nSX = vectTrackNext[i-1][r].x;
			nSY = vectTrackNext[i-1][r].y;
			a = vectTrackNext[i-1][r].a * vectTrackNext[i-1][r].kronfactor;
			b = vectTrackNext[i-1][r].b*vectTrackNext[i-1][r].kronfactor;
			nSR = a < b ? a : b;

			// create source images to be used for histogram - based on obj size
			nL = b<a ? (int) round(a) : (int) round(b);
			int nRefImgWidth =  nL*2+1; //(int) round( a*2 );
			int nRefImgHeight = nL*2+1; //(int) round( b*2 );
			refHistImgRed = cvCreateImage( cvSize( nRefImgWidth, nRefImgHeight ), IPL_DEPTH_8U, 1 );
			refHistImgGreen = cvCreateImage( cvSize( nRefImgWidth, nRefImgHeight ), IPL_DEPTH_8U, 1 );
			refHistImgBlue = cvCreateImage( cvSize( nRefImgWidth, nRefImgHeight ), IPL_DEPTH_8U, 1 );
			refHistImgMask = cvCreateImage( cvSize( nRefImgWidth, nRefImgHeight ), IPL_DEPTH_8U, 1 );
			cvSetZero( refHistImgRed );
			cvSetZero( refHistImgGreen );
			cvSetZero( refHistImgBlue );
			cvSetZero( refHistImgMask );

			///////////////////////////////////////
			// :: GET RED-GREEN-BLUE SOURCE CANNELS
			int nRefStartY = (int) round(vectTrackNext[i-1][r].y-nL);
			int nRefEndY = (int) round( vectTrackNext[i-1][r].y+nL );
			int nRefStartX = (int) round( vectTrackNext[i-1][r].x-nL );
			int nRefEndX = (int) round( vectTrackNext[i-1][r].x+nL );
			// for all pixels in my extended cut window
			for( y = nRefStartY; y <= nRefEndY; y++ )
			{
				for( x = nRefStartX; x <= nRefEndX; x++ ) 
				{
					// calculate distance between my current point and object center
					double nDistSq = (x-nSX)*(x-nSX) + (y-nSY)*(y-nSY);
					// now here we do the ellipse heavy stuff
					double sin_a = sin( PI*(180.0-vectTrackNext[i-1][r].theta)/180.0 );
					double cos_a = cos( PI*(180.0-vectTrackNext[i-1][r].theta)/180.0 );
					double ap = nSY*((1.0-cos_a*cos_a)/sin_a)-(nSX-x)*cos_a-y*sin_a;
					double bp = sqrt( nDistSq - ap*ap );
					// and now calc my fact
					double kDist = (ap*ap)/(a*a) + (bp*bp)/(b*b);

					// get colon / row
					int nRow = x-nRefStartX;
					int nCol = y-nRefStartY;

					// just check for out of range - should not be the case ... however
					if( nRow < 0 || nCol < 0 || nRow >= nRefImgWidth || nCol >= nRefImgHeight )
							continue;

					// check if my current pixel is in my ellipse
					if( kDist <= 1 && x < w_ref && y < h_ref && x > 0 && y > 0 )
					{
						// :: set red
						((uchar*)(refHistImgRed->imageData + refHistImgRed->widthStep*nCol))[nRow] = vectRefImgData[w_ref*y*3+x*3];
						// :: set green
						((uchar*)(refHistImgGreen->imageData + refHistImgGreen->widthStep*nCol))[nRow] = vectRefImgData[w_ref*y*3+x*3+1];
						// :: set blue
						((uchar*)(refHistImgBlue->imageData + refHistImgBlue->widthStep*nCol))[nRow] = vectRefImgData[w_ref*y*3+x*3+2];
						// set mask - transparent
						((uchar*)(refHistImgMask->imageData + refHistImgMask->widthStep*nCol))[nRow] = 255;

					} else
					{
						// set mask - opaque
						((uchar*)(refHistImgMask->imageData + refHistImgMask->widthStep*nCol))[nRow] = 0;
					}
				}
			}

/*			if( i == 2 )
			{
				// debug :: test save res image
				cvSaveImage( "./refHistImgRed.jpg", refHistImgRed );
				cvSaveImage( "./refHistImgGreen.jpg", refHistImgGreen );
				cvSaveImage( "./refHistImgBlue.jpg", refHistImgBlue );
			}
*/
			///////////////////
			// :: RED CHANNEL
			// create
			vectHistRefRed[r] = cvCreateHist( 1, &hist_size, CV_HIST_ARRAY, ranges, 1 );
			// calculate histogram
			cvCalcHist( &refHistImgRed, vectHistRefRed[r], 0, refHistImgMask );
			// normalize histogram
			if( bDoNormalizeHist ) cvNormalizeHist( vectHistRefRed[r], m_nObjHistogramNormalize );

			///////////////////
			// :: GREEN CHANNEL
			// create
			vectHistRefGreen[r] = cvCreateHist( 1, &hist_size, CV_HIST_ARRAY, ranges, 1 );
			// calculate histogram
			cvCalcHist( &refHistImgGreen, vectHistRefGreen[r], 0, refHistImgMask );
			// normalize histogram
			if( bDoNormalizeHist ) cvNormalizeHist( vectHistRefGreen[r], m_nObjHistogramNormalize );

			///////////////////
			// :: BLUE CHANNEL
			// create
			vectHistRefBlue[r] = cvCreateHist( 1, &hist_size, CV_HIST_ARRAY, ranges, 1 );
			// calculate histogram
			cvCalcHist( &refHistImgBlue, vectHistRefBlue[r], 0, refHistImgMask );
			// normalize histogram
			if( bDoNormalizeHist ) cvNormalizeHist( vectHistRefBlue[r], m_nObjHistogramNormalize );

			// free used images
			cvReleaseImage( &refHistImgRed );
			cvReleaseImage( &refHistImgGreen );
			cvReleaseImage( &refHistImgBlue );
			cvReleaseImage( &refHistImgMask );
		}
		// and close current ref image
		pImgRef->CloseImage();

		/////////////////////////////////
		// calculate histogram for destination
		vectDestImgData = (pImage->GetImage())->GetData();
		// get sizes
		w_dest = (pImage->GetImage())->GetWidth();
		h_dest = (pImage->GetImage())->GetHeight();
		// for all objes in destination
		for( j=0; j<nTrackBack; j++ )
		{
			// calc ellipse distance
			nSX = vectTrackBack[i][j].x;
			nSY = vectTrackBack[i][j].y;
			a = vectTrackBack[i][j].a * vectTrackBack[i][j].kronfactor;
			b = vectTrackBack[i][j].b * vectTrackBack[i][j].kronfactor;
			nSR = a < b ? a : b;

			// create destination images to be used for histogram - based on obj size
			nL = b<a ? (int) round(a) : (int) round(b);
			int nDestImgWidth = nL*2+1; //(int) round( a*2 );
			int nDestImgHeight = nL*2+1; //(int) round( b*2 );
			destHistImgRed = cvCreateImage( cvSize( nDestImgWidth, nDestImgHeight ), IPL_DEPTH_8U, 1 );
			destHistImgGreen = cvCreateImage( cvSize( nDestImgWidth, nDestImgHeight ), IPL_DEPTH_8U, 1 );
			destHistImgBlue = cvCreateImage( cvSize( nDestImgWidth, nDestImgHeight ), IPL_DEPTH_8U, 1 );
			destHistImgMask = cvCreateImage( cvSize( nDestImgWidth, nDestImgHeight ), IPL_DEPTH_8U, 1 );
			cvSetZero( destHistImgRed );
			cvSetZero( destHistImgGreen );
			cvSetZero( destHistImgBlue );
			cvSetZero( destHistImgMask );

			///////////////////
			// :: GET RED-GREEN-BLUE DESTINATION CANNELS	
			// for all pixels in my extended cut window
			for( y = vectTrackBack[i][j].y-nL; y <= vectTrackBack[i][j].y+nL; y++ )
			{
				for( x = vectTrackBack[i][j].x-nL; x <= vectTrackBack[i][j].x+nL; x++ ) 
				{
					// calculate distance between my current point and object center
					double nDistSq = (x-nSX)*(x-nSX) + (y-nSY)*(y-nSY);
					// now here we do the ellipse heavy stuff
					double sin_a = sin( PI*(180.0-vectTrackBack[i][j].theta)/180.0 );
					double cos_a = cos( PI*(180.0-vectTrackBack[i][j].theta)/180.0 );
					double ap = nSY*((1.0-cos_a*cos_a)/sin_a)-(nSX-x)*cos_a-y*sin_a;
					double bp = sqrt( nDistSq - ap*ap );
					// and now calc my fact
					double kDist = (ap*ap)/(a*a) + (bp*bp)/(b*b);

					// get colon / row
					int nRow = x-(vectTrackBack[i][j].x-nL);
					int nCol = y-(vectTrackBack[i][j].y-nL);

					// just check for out of range - should not be the case ... however
					if( nRow < 0 || nCol < 0 || nRow >= nDestImgWidth || nCol >= nDestImgHeight )
							continue;

					// check if my current pixel is in my ellipse
					if( kDist <= 1 && x < w_dest && y < h_dest && x > 0 && y > 0 )
					{
						// :: set red
						((uchar*)(destHistImgRed->imageData + destHistImgRed->widthStep*nCol))[nRow] = vectDestImgData[w_dest*y*3+x*3];
						// :: set green
						((uchar*)(destHistImgGreen->imageData + destHistImgGreen->widthStep*nCol))[nRow] = vectDestImgData[w_dest*y*3+x*3+1];
						// :: set blue
						((uchar*)(destHistImgBlue->imageData + destHistImgBlue->widthStep*nCol))[nRow] = vectDestImgData[w_dest*y*3+x*3+2];
						// :: set mask - transparent
						((uchar*)(destHistImgMask->imageData + destHistImgMask->widthStep*nCol))[nRow] = 255;

					} else
					{
						// :: set mask - opaque
						((uchar*)(destHistImgMask->imageData + destHistImgMask->widthStep*nCol))[nRow] = 0;

					}
				}
			}

			// debug :: test save dest images
/*			if( i == 2 )
			{
				cvSaveImage( "./destHistImgRed.jpg", destHistImgRed );
				cvSaveImage( "./destHistImgGreen.jpg", destHistImgGreen );
				cvSaveImage( "./destHistImgBlue.jpg", destHistImgBlue );
			}
*/
			///////////////////
			// :: RED CHANNEL
			// create
			vectHistDestRed[j] = cvCreateHist( 1, &hist_size, CV_HIST_ARRAY, ranges, 1 );
			// calculate histogram
			cvCalcHist( &destHistImgRed, vectHistDestRed[j], 0, destHistImgMask );
			// normalize histogram
			if( bDoNormalizeHist ) cvNormalizeHist( vectHistDestRed[j], m_nObjHistogramNormalize );

			///////////////////
			// :: GREEN CHANNEL
			// create
			vectHistDestGreen[j] = cvCreateHist( 1, &hist_size, CV_HIST_ARRAY, ranges, 1 );
			// calculate histogram
			cvCalcHist( &destHistImgGreen, vectHistDestGreen[j], 0, destHistImgMask );
			// normalize histogram
			if( bDoNormalizeHist ) cvNormalizeHist( vectHistDestGreen[j], m_nObjHistogramNormalize );


			///////////////////
			// :: BLUE CHANNEL
			// create
			vectHistDestBlue[j] = cvCreateHist( 1, &hist_size, CV_HIST_ARRAY, ranges, 1 );
			// calculate histogram
			cvCalcHist( &destHistImgBlue, vectHistDestBlue[j], 0, destHistImgMask );
			// normalize histogram
			if( bDoNormalizeHist ) cvNormalizeHist( vectHistDestBlue[j], m_nObjHistogramNormalize );

			// free used images				
			cvReleaseImage( &destHistImgRed );				
			cvReleaseImage( &destHistImgGreen );				
			cvReleaseImage( &destHistImgBlue );
			cvReleaseImage( &destHistImgMask );
		}
		// swap image data to next
		vectRefImgData = vectDestImgData;
		w_ref = w_dest;
		h_ref = h_dest;
		// close if last
		if( i == nNoOfFrames-1 ) 
			pImage->CloseImage();

		// allocat distance masurements in the ref obj list
		//vectHistogramDistance[i-1] = (typeDoubleVect*) calloc( nTrackNext, sizeof(typeDoubleVect) );
		vectHistogramDistance[i-1].clear();

		///////////////////////////////////////
		// init histogram
		for( r=0; r<nTrackNext; r++ )
		{
			// push matrix for every point of ref ??
			typeDoubleVect vectHistogramDistanceNodeVect;
			vectHistogramDistanceNodeVect.clear();
			vectHistogramDistance[i-1].push_back( vectHistogramDistanceNodeVect );

			// allocate dist vector to next
			//vectHistogramDistance[i-1][r].resize( nTrackBack );
			//vectHistogramDistance[i-1][r].clear();

			// and for all objects in the next frame
			for( j=0; j<nTrackBack; j++ )
			{
				// init/reset vars
				double nHistogramDist=0.0, tmp=0.0;

				///////////////////
				// :: RED CHANNEL
				// compare hist
				tmp = cvCompareHist( vectHistRefRed[r], vectHistDestRed[j], m_nHistCmpMethod );
				nHistogramDist += tmp * tmp;

				///////////////////
				// :: GREEN CHANNEL
				// compare hist
				tmp = cvCompareHist( vectHistRefGreen[r], vectHistDestGreen[j], m_nHistCmpMethod );
				nHistogramDist += tmp * tmp;

				///////////////////
				// :: BLUE CHANNEL
				// compare hist
				tmp = cvCompareHist( vectHistRefBlue[r], vectHistDestBlue[j], m_nHistCmpMethod );
				nHistogramDist += tmp * tmp;

				// final distance/score between images
				nHistogramDist = sqrt( nHistogramDist );
				// set my object corelation
				vectHistogramDistance[i-1][r].push_back( nHistogramDist );
			}
		}
		// free histogram reference vector
		for( r=0; r<nTrackNext; r++ ) 
		{
			cvReleaseHist( &vectHistRefRed[r] );
			cvReleaseHist( &vectHistRefGreen[r] );
			cvReleaseHist( &vectHistRefBlue[r] );
		}
		free( vectHistRefRed );
		free( vectHistRefGreen );
		free( vectHistRefBlue );
		// free histogram destination vector
		for( j=0; j<nTrackBack; j++ )
		{
			cvReleaseHist( &vectHistDestRed[j] );
			cvReleaseHist( &vectHistDestGreen[j] );
			cvReleaseHist( &vectHistDestBlue[j] );
		}
		free( vectHistDestRed );
		free( vectHistDestGreen );
		free( vectHistDestBlue );

		///////////////////////////
		// the other vars I used to compare ... might wanna get ridof
		double nFwhmDiffPerc = DBL_MAX;
	//	double nRefShapeRatio = (double) (star.a/star.b);
	//	double nObjStapeRatio = 0.0;
	//	double nShapeDiff = DBL_MAX;
		double nMagDiffPerc = DBL_MAX;
		double nKronDiffPerc = DBL_MAX;
		double nRadiusADiffPerc = DBL_MAX;
		double nFluxDiffPerc = DBL_MAX;
		double nObjStateSum = 0.0;
		double nBestObjStateSum = DBL_MAX;
		int nBestFound = -1;

		// and for all objects in the reference frame
		for( r=0; r<vectTrackNext[i-1].size(); r++ )
		{
			nBestFound = -1;
			nBestObjStateSum = DBL_MAX;

			// and for all objects in the next frame
			for( j=0; j<vectTrackBack[i].size(); j++ )
			{
				// calculate sape ratio for target object
		//		nObjStapeRatio = (double) ( m_vectTrackBack[i][i].a/m_vectTrackBack[i][i].b );

				// calculate diffences procent to reference
				double nFirstFwhm = vectTrackNext[i-1][r].fwhm;
				double nNextFwhm = vectTrackBack[i][j].fwhm;
				nFwhmDiffPerc = (double) ( fabs( vectTrackNext[i-1][r].fwhm - vectTrackBack[i][j].fwhm )/vectTrackNext[i-1][r].fwhm );
		//		nShapeDiff = (double) fabs( nRefShapeRatio - nShapeDiff );
				double nFirstMag = vectTrackNext[i-1][r].mag;
				double nNextMag = vectTrackBack[i][j].mag;
				nMagDiffPerc = (double) fabs( fabs( nFirstMag - nNextMag )/nFirstMag );
				// a radius difference
				double nFirstRadiusA = vectTrackNext[i-1][r].kronfactor*vectTrackNext[i-1][r].a;
				double nNextRadiusA = vectTrackBack[i][j].kronfactor*vectTrackBack[i][j].a;
				nRadiusADiffPerc = (double) ( fabs( nFirstRadiusA - nNextRadiusA )/nFirstRadiusA );
				// kron factor difference
				nKronDiffPerc = (double) ( fabs( vectTrackNext[i-1][r].kronfactor - vectTrackBack[i][j].kronfactor )/vectTrackNext[i-1][r].kronfactor );
				nFluxDiffPerc = (double) ( fabs( vectTrackNext[i-1][r].flux - vectTrackBack[i][j].flux )/vectTrackNext[i-1][r].flux );

/*				if( ( i == 1 && r == 12 && j == 17 ) ||
					( i == 2 && r == 17 && j == 13 ) ||
					( i == 3 && r == 13 && j == 13 ) )
					nBestFound = nBestFound;
*/
				// compare star profile kronfactor, fwhm - for now latter maybe color profile/ entropy - or a/b, mag, theta?
				//if( /*nFwhmDiffPerc <= m_nObjDymanicsFwhmTol &&*/ nKronDiffPerc < m_nObjDynamicsKronFactorTol 
				//	/*&& nFluxDiffPerc <= m_nObjDymanicsFluxTol*/ )
//				if( nRadiusADiffPerc < m_nObjDynamicsKronFactorTol )
				if( nMagDiffPerc < m_nObjDynamicsMagTol &&  
					nKronDiffPerc < m_nObjDynamicsKronFactorTol && 
					nFwhmDiffPerc < m_nObjDynamicsFwhmTol )
				{
					// calculate sum
					//nObjStateSum = nFluxDiffPerc*m_nObjDymanicsFluxPrio + nFwhmDiffPerc*m_nObjDymanicsFwhmPrio + nKronDiffPerc*m_nObjDynamicsKronFactorPrio;
					nObjStateSum = vectHistogramDistance[i-1][r][j]; //nKronDiffPerc*0.4 + nMagDiff*0.5 + vectHistogramDistance[i-1][r][j]*0.1;
					// check sum with original
					if( nObjStateSum < nBestObjStateSum )
					{
						// set best found and best sum 
						nBestFound = j;
						nBestObjStateSum = nObjStateSum;
					}

				}
			}
			
			////////////////////
			// if found any similar object
			if( nBestFound >= 0 )
			{
				// get object current path [frame][obj]
				int nPathId = -1;
				nPathId = vectObjPathAssoc[i-1][r]; // [nBestFound] - that will be the next point

				// if there is a path already use cuurent path if
				if( nPathId >= 0 )
				{
					// get previous point in the path A - B(current ref - last in track) -> C Next matched node
					int nTrackNodeA = m_vectDynamicObjPath[nPathId].vectNode.size()-2;
					// get A x/y
					double nA_x = m_vectDynamicObjPath[nPathId].vectNode[nTrackNodeA].x;
					double nA_y = m_vectDynamicObjPath[nPathId].vectNode[nTrackNodeA].y;
					// get B x/y
					double nB_x = vectTrackNext[i-1][r].x;
					double nB_y = vectTrackNext[i-1][r].y;
					// get C x/y
					double nC_x = vectTrackBack[i][nBestFound].x;
					double nC_y = vectTrackBack[i][nBestFound].y;

					////////////
					// calculate triangle distance
					// :: calculate c
					double nACDiff_x = (double) fabs( nC_x - nA_x );
					double nACDiff_y = (double) fabs( nC_y - nA_y );
					double c = (double) sqrt( (double) (nACDiff_x*nACDiff_x) + (double) (nACDiff_y*nACDiff_y) );
					// :: calculate b
					double nABDiff_x = (double) fabs( nB_x - nA_x );
					double nABDiff_y = (double) fabs( nB_y - nA_y );
					double b = (double) sqrt( (double) (nABDiff_x*nABDiff_x) + (double) (nABDiff_y*nABDiff_y) );
					// :: calculate a
					double nBCDiff_x = (double) fabs( nC_x - nB_x );
					double nBCDiff_y = (double) fabs( nC_y - nB_y );
					double a = (double) sqrt( (double) (nBCDiff_x*nBCDiff_x) + (double) (nBCDiff_y*nBCDiff_y) );

					// calculate angle with previous point and see if in tolerance
					double nPathDevAngle = (double) fabs( 180.0 - (double) ( acos( ( (double) (a*a) + (double) (b*b) - (double) (c*c) )/( (double) (2.0*a*b) ) * RAD2DEG ) ) );
					// check if tolerance angle (degrees) more then accepted
					if(  nPathDevAngle > m_nPathDevAngleTolerance )
					{
						// mark good to be added - set path id to negative
						// larry :: disable this for now
						//nPathId = -1;
					}
				}

				// here check if the path is negative - after other checks
				if( nPathId < 0 )
				{
					// add new track
					nPathId = AddDynamicObjectPath( );
					// set ref frame obj assoc
					vectObjPathAssoc[i-1][r] = nPathId;
					// reset score to zero
					m_vectDynamicObjPath[nPathId].path_score = 0.0;

					// add first obj to the path
					PathNode firstNode;
					firstNode.frame_id = i-1;
					firstNode.obj_id = r;
					firstNode.x = vectTrackNext[i-1][r].x;
					firstNode.y = vectTrackNext[i-1][r].y;
					m_vectDynamicObjPath[nPathId].vectNode.push_back( firstNode );
				}		

				// set next frame obj assoc 
				vectObjPathAssoc[i][nBestFound] = nPathId;

				// add obj to the path
				PathNode foundNode;
				foundNode.frame_id = i;
				foundNode.obj_id = nBestFound;
				foundNode.x = vectTrackBack[i][nBestFound].x;
				foundNode.y = vectTrackBack[i][nBestFound].y;
				m_vectDynamicObjPath[nPathId].vectNode.push_back( foundNode );
				// reset score to zero
				m_vectDynamicObjPath[nPathId].path_score += vectHistogramDistance[i-1][r][nBestFound];
			}
		}

		///////////////////
		// now free allocated
		pImgRef->FreeData( );
//		pImage->FreeData( );
		// set img ref to next image
		pImgRef = pImage;
		pImage = NULL;
	}
	// free last image data
	if( pImgRef ) pImgRef->FreeData( );
	if( pImage ) pImage->FreeData( );

	// calculate average score
//	int nMinPathId = -1;
//	double nMinPathValue = DBL_MAX;
	for( i=0; i<m_nDynamicObjPath; i++ )
	{
		if( m_vectDynamicObjPath[i].vectNode.size() <= 2 ) continue;

		m_vectDynamicObjPath[i].path_score = (double) m_vectDynamicObjPath[i].path_score/(m_vectDynamicObjPath[i].vectNode.size()-1);
//		//and get min path id/value
//		if( m_vectDynamicObjPath[i].path_score < nMinPathValue )
//		{
//			nMinPathId = i;
//			nMinPathValue = m_vectDynamicObjPath[i].path_score;
//		}
	}
//	m_nMinPathId = nMinPathId;
	// now sort
	std::sort( m_vectDynamicObjPath.begin(), m_vectDynamicObjPath.end(), SortDescObjPaths );
	m_nMinPathId = 0;


	//////////////////////////////
	// HERE MARK THESE DYNAMIC OBJECTS IN IMAGES with a state ???


	//////////////////
	// clear all other vectors
	for( i=0; i<nNoOfFrames; i++ )
	{
		if( i<nNoOfFrames-1 )
		{
			// clear hist distance 
			for( j=0; j<vectHistogramDistance[i].size(); j++ )
			{
					vectHistogramDistance[i][j].clear();
			}
			vectHistogramDistance[i].clear();
		}

		vectObjPathAssoc[i].clear();
		vectTrackNext[i].clear();
		vectTrackBack[i].clear();

	}
	//free( vectHistogramDistance );
	vectHistogramDistance.clear();

	//free( vectObjPathAssoc );
	vectObjPathAssoc.clear();
	//free( vectTrackNext );
	vectTrackNext.clear();
	//free( vectTrackBack );
	vectTrackBack.clear();

	// star vectors
	free( vectStarRef );
	free( vectStar );

	// save group
	if( m_vectDynamicObjPath.size() > 0 ) Save( );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		AddDynamicObjectPath
// Class:		CImageGroup
// Purpose:		add a new dynamic object path
// Input:		nothing
// Output:		new path id
////////////////////////////////////////////////////////////////////
int CImageGroup::AddDynamicObjectPath( )
{
/*	// if empty - the allocate
	if( m_vectDynamicObjPath == NULL )
		m_vectDynamicObjPath = (PathData*) calloc( m_nDynamicObjPathAllocated+1, sizeof(PathData) );

	// check for allocation
	if( m_nDynamicObjPath+1 > m_nDynamicObjPathAllocated )
	{
		m_nDynamicObjPathAllocated += 10;
		m_vectDynamicObjPath = (PathData*) _recalloc( m_vectDynamicObjPath, m_nDynamicObjPathAllocated, sizeof(PathData) );
	}
*/
	PathData newPath;
	newPath.path_score = 0.0;
	m_vectDynamicObjPath.push_back( newPath );

	// get counter
	m_nDynamicObjPath = m_vectDynamicObjPath.size();

	return( m_nDynamicObjPath-1 );
}

////////////////////////////////////////////////////////////////////
// Method:		ClearAllDynamicObjectPaths
// Class:		CImageGroup
// Purpose:		clear all dynamic object paths
// Input:		nothing
// Output:		new path id
////////////////////////////////////////////////////////////////////
void CImageGroup::ClearAllDynamicObjectPaths( )
{
	int i =0;

	for( i=0; i<m_nDynamicObjPath; i++ ) m_vectDynamicObjPath[i].vectNode.clear();
	// clear main vector
	//free( m_vectDynamicObjPath );
	m_vectDynamicObjPath.clear();

	//m_vectDynamicObjPath = NULL;

	m_nDynamicObjPath = 0;
//	m_nDynamicObjPathAllocated = 10;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		SetHint
// Class:		CImageGroup
// Purpose:		set group hint on the field size and origin
// Input:		ra/dec, width/height
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CImageGroup::SetHint( double nRa, double nDec, double nWidth, 
													double nHeight )
{
	int i=0;
	CAstroImage* pImage = NULL;

	m_nRelCenterRa = nRa;
	m_nRelCenterDec = nDec;
	m_nRelFieldRa = nWidth;
	m_nRelFieldDec = nHeight;

	m_bIsHint = 1;

	// for each image node in group
	for( i=0; i<m_nImageNode; i++ )
	{
		pImage = m_vectImageNode[i];
		pImage->m_nUseHint = 2;
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		ExtractStackPixel
// Class:		CImageGroup
// Purpose:		extract stack pixel vector
// Input:		x, y and ref to RGB vectors
// Output:		state - 0 if error
////////////////////////////////////////////////////////////////////
int CImageGroup::ExtractStackPixel( int x, int y, unsigned char* vectStackRed,
									unsigned char* vectStackGreen,
									unsigned char* vectStackBlue )
{
	int i=0;
	// get pixel for all stack frames/images
	for( i=0; i<m_nStack; i++ )
	{
		vectStackRed[i] = m_vectStack[i].p_image->m_pImage->GetRed( x, y );
		vectStackGreen[i] = m_vectStack[i].p_image->m_pImage->GetGreen( x, y );
		vectStackBlue[i] = m_vectStack[i].p_image->m_pImage->GetBlue( x, y );
	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		LoadStackImagesData
// Class:		CImageGroup
// Purpose:		load data for all images in the stack
// Input:		nothing
// Output:		state - 0 if error
////////////////////////////////////////////////////////////////////
int CImageGroup::LoadStackImagesData( )
{
	int i=0;
	// load data for all images in the stack
	m_vectStack[0].p_image->GetImage( );
	for( i=1; i<m_nStack; i++ )
	{
		m_vectStack[i].p_image->GetStackImage( );
	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		FreeStackImagesData
// Class:		CImageGroup
// Purpose:		free data for all images in the stack
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CImageGroup::FreeStackImagesData( )
{
	int i=0;
	// load data for all images in the stack
	for( i=0; i<m_nStack; i++ )
	{
		m_vectStack[i].p_image->CloseImage( );
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	CombineImages
// Class:	CImageGroup
// Purpose:	combine frames/images
// Input:	pointer to images, method
// Output:	return 0 for errors
////////////////////////////////////////////////////////////////////
int CImageGroup::CombineImages( wxImage* pBaseImage, wxImage* pOperatorImage, int nMethod )
{
	char strLog[255];
	float nRedA=0, nGreenA=0, nBlueA=0;
	float nRedB=0, nGreenB=0, nBlueB=0;
	int w=0, h=0;
	int x=0, y=0;
	double mu=0, ll=0;

	// get/use main image size
	// get reference frame width and height - image[0]
	w = pBaseImage->GetWidth();
	h = pBaseImage->GetHeight();

	// get other proc data specific to some combine methods
	if( nMethod == IMAGES_COMBINE_METHOD_FLAT )
	{
/*		mu = fr1->stats.cavg;

		if( mu <= 0.0 )
		{
			err_printf("flat frame has negative avg: %.2f aborting\n", mu);
			return( 0 );
		}
		if( fabs(fr->exp.bias) > 2.0 )
		{
			err_printf("I refuse to flat a frame with non-null bias (%.f)\n", fr->exp.bias);
			return( 0 );
		}
		if( fabs(fr1->exp.bias) > 2.0 )
		{
			err_printf("I refuse to use a flat with non-null bias (%.f)\n", fr1->exp.bias);
			return -1;
		}

		ll = mu / MAX_FLAT_GAIN;
*/	}

	///////////////////////////////
	// todo: somthing here to align my frame with substracted one?????
	//////////////

	// for all pixels in the image
	for( y = 0; y < h; y++ )
	{
		for( x = 0; x < w; x++ )
		{
			// get current image rgb pixels
			nRedA = pBaseImage->GetRed( x, y );
			nGreenA = pBaseImage->GetGreen( x, y );
			nBlueA = pBaseImage->GetBlue( x, y );
			// get operator image rgb pixels
			nRedB = pOperatorImage->GetRed( x, y );
			nGreenB = pOperatorImage->GetGreen( x, y );
			nBlueB = pOperatorImage->GetBlue( x, y );

			// combine pixels by type
			if( nMethod == IMAGES_COMBINE_METHOD_SUBSTRACT )
			{
				// substract and check for limit to zero
				nRedA = nRedA-nRedB;
				nGreenA = nGreenA-nGreenB;
				nBlueA = nBlueA-nBlueB;

			} else if( nMethod == IMAGES_COMBINE_METHOD_ADD )
			{
				// add and check for limit to 255
				nRedA = nRedA+nRedB;
				nGreenA = nGreenA+nGreenB;
				nBlueA = nBlueA+nBlueB;		

			} else if( nMethod == IMAGES_COMBINE_METHOD_FLAT )
			{
				// do flat and check fro limits 0-255
				// :: red
				if( nRedB > ll )  
					nRedA = nRedA / nRedB * mu;
				else 
					nRedA = nRedA * IMAGES_COMBINE_MAX_FLAT_GAIN;
				// :: green
				if( nGreenB > ll )  
					nGreenA = nGreenA / nGreenB * mu;
				else 
					nGreenA = nGreenA * IMAGES_COMBINE_MAX_FLAT_GAIN;
				// :: blue
				if( nBlueB > ll )  
					nBlueA = nBlueA / nBlueB * mu;
				else 
					nBlueA = nBlueA * IMAGES_COMBINE_MAX_FLAT_GAIN;
			}

			////////////////////
			// check for pixel limits out of range 0-255 in rgb
			// :: red
			if( nRedA<0 ) 
				nRedA = 0;
			else if( nRedA>255 ) 
				nRedA = 255;
			// :: green
			if( nGreenA<0 ) 
				nGreenA = 0;
			else if( nGreenA>255 ) 
				nGreenA = 255;
			// :: blue
			if( nBlueA<0 ) 
				nBlueA = 0;
			else if( nBlueA>255 ) 
				nBlueA = 255;

			///////////////////////////
			// set pixel in my base image
			pBaseImage->SetRGB( x, y, nRedA, nGreenA, nBlueA );
		}
	}

	// compute noise data
	if( nMethod == IMAGES_COMBINE_METHOD_SUBSTRACT )
	{
		//fr->exp.bias = fr->exp.bias - fr1->exp.bias;
		//fr->exp.rdnoise = sqrt(sqr(fr->exp.rdnoise) + sqr(fr1->exp.rdnoise));

	} else if( nMethod == IMAGES_COMBINE_METHOD_ADD )
	{
		//fr->exp.bias = fr->exp.bias + fr1->exp.bias;
		//fr->exp.rdnoise = sqrt(sqr(fr->exp.rdnoise) + sqr(fr1->exp.rdnoise));

	} else if( nMethod == IMAGES_COMBINE_METHOD_FLAT )
	{
		//fr->exp.flat_noise = sqrt( sqr(fr1->exp.rdnoise) + mu / sqrt(fr1->exp.scale) ) / mu;
		// TODO :: take care of biases
	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	StackByMethod
// Class:	CImageGroup
// Purpose:	avg-stacking frames
// Input:	nothing
// Output:	return 0 for errors
////////////////////////////////////////////////////////////////////
int CImageGroup::StackByMethod( wxImage* pStackImage, int w, int h, int nMethod )
{
	int x=0, y=0;
	wxString strLog;
	// channel vectors
	unsigned char* vectStackRed;
	unsigned char* vectStackGreen;
	unsigned char* vectStackBlue;
	// channel values
	unsigned char nPixelRed=0;
	unsigned char nPixelGreen=0;
	unsigned char nPixelBlue=0;

	////////
	// if to apply bias/dark or flat frame - also might need to re-align
	//CombineImages( img1, img_bias, IMAGES_COMBINE_METHOD_SUBSTRACT );
	//CombineImages( img1, img_dark, IMAGES_COMBINE_METHOD_SUBSTRACT );
	//CombineImages( img1, img_bias, IMAGES_COMBINE_METHOD_FLAT );
	///////
	// :: call to realign ?

	//////////
	// allocate channels vectors
	vectStackRed = (unsigned char*) malloc( sizeof( unsigned char ) * (m_nStack + 1) );
	vectStackGreen = (unsigned char*) malloc( sizeof( unsigned char ) * (m_nStack + 1) );
	vectStackBlue = (unsigned char*) malloc( sizeof( unsigned char ) * (m_nStack + 1) );

	// log
	strLog.Printf( wxT("Stack %d frames ..."), m_nStack );
	m_pImageDb->m_pUnimapWorker->SetWaitDialogMsg( strLog );
	
	// set stack method by type
	if( nMethod == IMAGE_STACK_METHOD_AVERAGE )
		m_pWaitDialogStack->SetMethod( wxT("Average") );
	else if( nMethod == IMAGE_STACK_METHOD_MEDIAN )
		m_pWaitDialogStack->SetMethod( wxT("Median") );
	else if( nMethod == IMAGE_STACK_METHOD_MEAN_MEDIAN )
		m_pWaitDialogStack->SetMethod( wxT("Mean-Median") );
	else if( nMethod == IMAGE_STACK_METHOD_KAPPA_SIGMA )
		m_pWaitDialogStack->SetMethod( wxT("Kappa-Sigma") );
	// set image size
	m_pWaitDialogStack->SetImageSize( w, h );
	// set image 
	m_pWaitDialogStack->SetImage( pStackImage );

	// counters
	long nCount = 0;
	double nUnitTime=0, nUnitTimeSec=0;
	unsigned long nUnitsPerSec = 100;
	LARGE_INTEGER m_lnStart;
	LARGE_INTEGER m_lnEnd;
	LARGE_INTEGER m_lnFreq;
	memset( &m_lnStart, 0, sizeof(LARGE_INTEGER) );
	memset( &m_lnEnd, 0, sizeof(LARGE_INTEGER) );
	memset( &m_lnFreq, 0, sizeof(LARGE_INTEGER) );

	// :: for all pizels on X
	for( y = 0; y < h; y++ )
	{
		// :: and for all pixels on Y
		for( x = 0; x < w; x++ )
		{
			// set time start - if first step
			if( y == 0 && x == 0 )
			{
				QueryPerformanceFrequency(&m_lnFreq);
				QueryPerformanceCounter( &m_lnStart);
			}

			// extract stack pixel vectors for this position
			ExtractStackPixel( x, y, vectStackRed, vectStackGreen, vectStackBlue );

			//////////////////////////
			// PIXEL STACK :: by method
			if( nMethod == IMAGE_STACK_METHOD_AVERAGE ) 
			{
				// calculate pixel average for current pixel (RGB) in all frames
				nPixelRed = (unsigned char) CalcPixelAverage( vectStackRed, m_nStack, 0, 1 );
				nPixelGreen = (unsigned char) CalcPixelAverage( vectStackGreen, m_nStack, 0, 1 );
				nPixelBlue = (unsigned char) CalcPixelAverage( vectStackBlue, m_nStack, 0, 1 );

			} else if( nMethod == IMAGE_STACK_METHOD_MEDIAN )
			{
				// calculate pixel median for current pixel (RGB) in all frames
				nPixelRed = (unsigned char) CalcPixelMedian( vectStackRed, m_nStack, 0, 1 );
				nPixelGreen = (unsigned char) CalcPixelMedian( vectStackGreen, m_nStack, 0, 1 );
				nPixelBlue = (unsigned char) CalcPixelMedian( vectStackBlue, m_nStack, 0, 1 );

			} else if( nMethod == IMAGE_STACK_METHOD_MEAN_MEDIAN )
			{
				// calculate pixel mean-median for current pixel (RGB) in all frames
				nPixelRed = (unsigned char) CalcPixelMeanMedian( vectStackRed, m_nStack, m_nStackFrameSigma, m_StackFrameIteration );
				nPixelGreen = (unsigned char) CalcPixelMeanMedian( vectStackGreen, m_nStack, m_nStackFrameSigma, m_StackFrameIteration );
				nPixelBlue = (unsigned char) CalcPixelMeanMedian( vectStackBlue, m_nStack, m_nStackFrameSigma, m_StackFrameIteration );

			} else if( nMethod == IMAGE_STACK_METHOD_KAPPA_SIGMA )
			{
				// calculate pixel average for current pixel (RGB) in all frames
				nPixelRed = (unsigned char) CalcPixelKappaSigma( vectStackRed, m_nStack, m_nStackFrameSigma, m_StackFrameIteration );
				nPixelGreen = (unsigned char) CalcPixelKappaSigma( vectStackGreen, m_nStack, m_nStackFrameSigma, m_StackFrameIteration );
				nPixelBlue = (unsigned char) CalcPixelKappaSigma( vectStackBlue, m_nStack, m_nStackFrameSigma, m_StackFrameIteration );
			}
			// END OF PIXEL STACK
			/////////////////////////////////

			// set destination pixel
			pStackImage->SetRGB( x, y, nPixelRed, nPixelGreen, nPixelBlue );

			//////////////////
			// set time end - if first step
			if( y == 0 && x == 0 )
			{
				QueryPerformanceCounter( &m_lnEnd );
				__int64 nDiff(0) ;
				double dMircoSecond( 0 ) ;
				if( m_lnStart.QuadPart !=0 && m_lnEnd.QuadPart != 0 )
				{
					dMircoSecond = (double)(m_lnFreq.QuadPart/1000000);
					nDiff = m_lnEnd.QuadPart-m_lnStart.QuadPart;
					nUnitTime = nDiff/dMircoSecond;
					nUnitTimeSec = (double) nUnitTime/1000000;
					// calc units per sec
					nUnitsPerSec = (unsigned long) 1000000/nUnitTime;
					// calculate initial eta
					double nEta = (double) w*h*nUnitTimeSec;
					m_pWaitDialogStack->SetEta( nEta );
				}
			}

			//////////////////
			// log - time based ... update every second
			if( nCount % nUnitsPerSec == 0 )
			{
				float nFloatValue = (float) ( ( 0.299*nPixelRed + 0.587*nPixelGreen + 0.114*nPixelBlue ) * 65535 );
				m_pWaitDialogStack->SetPixelStatus( x, y, nFloatValue );
				// calculate current eta
				double nEta = (double) (w*h-nCount)*nUnitTimeSec;
				m_pWaitDialogStack->SetEta( nEta );
				// set image 
				m_pWaitDialogStack->SetImage( pStackImage );
			}

			nCount++;
		}
	}

	// free allocated data
	free( vectStackRed );
	free( vectStackGreen );
	free( vectStackBlue );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	StackImages
// Class:	CImageGroup
// Purpose:	stack the frames/images from my stack list
// Input:	stack method type ??
// Output:	return -1 for errors
////////////////////////////////////////////////////////////////////
int CImageGroup::StackImages( void* pWaitDialogStack )
{
	m_pWaitDialogStack = (CWaitDialogStack*) pWaitDialogStack;

	//first we check if we have a stack 
	if( m_nStack <= 0 || m_nStack > STACK_MAX_NO_OF_FRAMES ) return( 0 );

//	int nf = 0;
//	char msg[256];
//	double b = 0.0, rn = 0.0, fln = 0.0, sc = 0.0;
	double eff = 1.0;
	int w=0, h=0;
	wxImage* pStackImage;
	int bStackStatus = 1;
	wxString strFile;

	// reset stack flag to zero
	m_bNewStackOutput = 0;

	// load all images in the group - todo: check if sufficient memory
	if( !LoadStackImagesData( ) ) return( 0 );

	// get reference frame width and height - image[0]
	w = m_vectStack[0].p_image->m_nWidth;
	h = m_vectStack[0].p_image->m_nHeight;

	// create the ouput frame for stacking process
	pStackImage = new wxImage( w, h, true );

	// todo: see where I need to set this for the future
	// b += imf->fr->exp.bias;
	// rn += sqr(imf->fr->exp.rdnoise);
	// fln += imf->fr->exp.flat_noise;
	// sc += imf->fr->exp.scale;

	///////////////////////////////////////
	// :: do the actual stack by frame type
	if( !StackByMethod( pStackImage, w, h, m_nImageStackMethod ) ) bStackStatus = 0;

//	ofr->exp.rdnoise = sqrt(rn) / nf / eff;
//	ofr->exp.flat_noise = fln / nf;
//	ofr->exp.scale = sc;
//	ofr->exp.bias = b / nf;

//	if (ofr->exp.bias != 0.0)
//		scale_shift_frame(ofr, 1.0, -ofr->exp.bias);

//	noise_to_fits_header(ofr, &(ofr->exp));
//	strcpy(ofr->name, "stack_result");

	// save output image
	strFile.Printf( wxT("%s%s%s_comp.bmp"),  m_vectStack[0].p_image->m_strImagePath, wxT("\\"), m_strGroupNameId );
	pStackImage->SaveFile( strFile, wxBITMAP_TYPE_BMP  );
	// add stack result to the list
	AddImage( strFile, 0 );
	// set stack flag to 1
	m_bNewStackOutput = 1;

	// destroy my output image
	pStackImage->Destroy( );
	delete( pStackImage );

	// free images data
	FreeStackImagesData( );

	return( bStackStatus );
}

// :: some sorting functions
////////////////////////////////////////////////////////////////////
bool CImageGroup::AscCmpByImageId( ImageStack a, ImageStack b )
{ 
	return( a.p_image->m_nId < b.p_image->m_nId ); 
}
////////////////////////////////////////////////////////////////////
bool CImageGroup::DesCmpByImageId( ImageStack a, ImageStack b )
{ 
	return( a.p_image->m_nId > b.p_image->m_nId ); 
}
////////////////////////////////////////////////////////////////////
bool CImageGroup::AscCmpFileName( ImageStack a, ImageStack b )
{ 
	if( a.p_image->m_strImageName.CmpNoCase( b.p_image->m_strImageName ) > 0 )
		return( true ); 
	else
		return( false );
}
////////////////////////////////////////////////////////////////////
bool CImageGroup::DesCmpFileName( ImageStack a, ImageStack b )
{ 
	if( a.p_image->m_strImageName.CmpNoCase( b.p_image->m_strImageName ) < 0 )
		return( true ); 
	else
		return( false );
}
////////////////////////////////////////////////////////////////////
bool CImageGroup::AscCmpFileTimestamp( ImageStack a, ImageStack b )
{ 
	return( a.p_image->m_nFileTime < b.p_image->m_nFileTime ); 
}
////////////////////////////////////////////////////////////////////
bool CImageGroup::DesCmpFileTimestamp( ImageStack a, ImageStack b )
{ 
	return( a.p_image->m_nFileTime > b.p_image->m_nFileTime ); 
}
////////////////////////////////////////////////////////////////////
// object path sorting
////////////////////////////////////////////////////////////////////
bool CImageGroup::SortDescObjPaths( PathData a, PathData b )
{ 
	if( a.vectNode.size() > 3 && a.path_score < b.path_score )
		return( 1 ); 
	else
		return( 0 );
}


